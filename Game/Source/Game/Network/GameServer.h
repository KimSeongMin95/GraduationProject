// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Packet.h"

#include "CoreMinimal.h"

// ��Ŷ ó�� �Լ� ������
struct FuncProcess
{
	// RecvStream�� ������ ����, pSocket�� Overlapped I/O �۾��� �߻��� IOCP ���� ����ü ����
	void(*funcProcessPacket)(stringstream& RecvStream, SOCKET Socket);
	FuncProcess()
	{
		funcProcessPacket = nullptr;
	}
};

/**
 * ���� Ŭ���̾�Ʈ�� ���� �� ��Ŷ ó���� ����ϴ� Ŭ���� (���� ����)
 */
class GAME_API cGameServer
{
private:
	FuncProcess	fnProcess[100];	// ��Ŷ ó�� ����ü

	static int ServerPort;
	static CRITICAL_SECTION csServerPort;

protected:
	SOCKET			 ListenSocket;			// ���� ���� ����
	HANDLE			 hIOCP;					// IOCP ��ü �ڵ�

	bool			 bAccept;				// ��û ���� �÷���
	CRITICAL_SECTION csAccept;				//
	HANDLE			 hAcceptThreadHandle;	// Accept ������ �ڵ�	

	bool			 bIOThread;
	HANDLE*			 hIOThreadHandle;		// IO ������ �ڵ�		
	DWORD			 nIOThreadCnt;			// IO ������ ����


public:
	/** ���Ӽ����� �ӽ� ���� */
	static SOCKET SocketID;
	static CRITICAL_SECTION csSocketID;

	static int PossessedID;
	static CRITICAL_SECTION csPossessedID;


	// WSAAccept(...)�� ��� Ŭ���̾�Ʈ�� new stCompletionKey()�� ����
	static unordered_map<SOCKET, stCompletionKey*> GameClients;
	static CRITICAL_SECTION csGameClients;

	// ������ �����͸� ���� ���� ����
	static unordered_map<SOCKET, deque<char*>*> MapOfRecvDeque;
	static CRITICAL_SECTION csMapOfRecvDeque;

	// WSASend(...)�� �����ϸ� ++, ������ �Ϸ�ǰų� �����ϸ� --
	static unsigned int CountOfSend;
	static CRITICAL_SECTION csCountOfSend;

	/**************************************************/

	// Connected Ŭ���̾�Ʈ�� InfoOfPlayer ����
	static unordered_map<SOCKET, cInfoOfPlayer> InfoOfClients;
	static CRITICAL_SECTION csInfoOfClients;

	static unordered_map<SOCKET, cInfoOfScoreBoard> InfosOfScoreBoard;
	static CRITICAL_SECTION csInfosOfScoreBoard;

	static unordered_map<SOCKET, SOCKET> Observers;
	static CRITICAL_SECTION csObservers;


	///////////////////////////////////////////
	// Pioneer ����ȭ
	///////////////////////////////////////////
	static unordered_map<int, cInfoOfPioneer_Socket> InfosOfPioneer_Socket;
	static CRITICAL_SECTION csInfosOfPioneer_Socket;

	static unordered_map<int, cInfoOfPioneer_Animation> InfosOfPioneer_Animation;
	static CRITICAL_SECTION csInfosOfPioneer_Animation;

	static unordered_map<int, cInfoOfPioneer_Stat> InfosOfPioneer_Stat;
	static CRITICAL_SECTION csInfosOfPioneer_Stat;


	///////////////////////////////////////////
	// Building ����ȭ
	///////////////////////////////////////////
	static unordered_map<int, cInfoOfBuilding_Spawn> InfoOfBuilding_Spawn;
	static CRITICAL_SECTION csInfoOfBuilding_Spawn;

	static unordered_map<int, cInfoOfBuilding_Stat> InfoOfBuilding_Stat;
	static CRITICAL_SECTION csInfoOfBuilding_Stat;


	///////////////////////////////////////////
	// Enemy ����ȭ
	///////////////////////////////////////////
	static unordered_map<int, cInfoOfEnemy_Spawn> InfoOfEnemies_Spawn;
	static CRITICAL_SECTION csInfoOfEnemies_Spawn;

	static unordered_map<int, cInfoOfEnemy_Animation> InfoOfEnemies_Animation;
	static CRITICAL_SECTION csInfoOfEnemies_Animation;

	static unordered_map<int, cInfoOfEnemy_Stat> InfoOfEnemies_Stat;
	static CRITICAL_SECTION csInfoOfEnemies_Stat;


public:
	////////////////////////
	// �⺻
	////////////////////////
	cGameServer();
	~cGameServer();

	// �ʱ�ȭ ���н� ����
	void CloseListenSocketAndCleanupWSA();

	// ���� ��� �� ���� ���� ����
	bool Init();

	// Accept ������ ����
	bool CreateAcceptThread();

	// ���� ����
	void AcceptThread();

	// IO ������ ����
	bool CreateIOThread();

	// �۾� ������
	void IOThread();

	// Ŭ���̾�Ʈ ���� ����
	static void CloseSocket(SOCKET Socket, stOverlappedMsg* OverlappedMsg);

	// ���� ����
	void Close();

	// Ŭ���̾�Ʈ���� �۽�
	static void Send(stringstream& SendStream, SOCKET Socket);

	// Ŭ���̾�Ʈ ���� ���
	static void Recv(SOCKET Socket, stOverlappedMsg* ReceivedOverlappedMsg);

	///////////////////////////////////////////
	// stringstream�� �� �տ� size�� �߰�
	///////////////////////////////////////////
	static bool AddSizeInStream(stringstream& DataStream, stringstream& FinalStream);

	///////////////////////////////////////////
	// ���� ���� ũ�� ����
	///////////////////////////////////////////
	void SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf);

	///////////////////////////////////////////
	// ������ �����͸� �����ϴ� ������ �����͸� ȹ��
	///////////////////////////////////////////
	void GetDataInRecvDeque(deque<char*>* RecvDeque, char* DataBuffer);

	///////////////////////////////////////////
	// ��Ŷ�� ó���մϴ�.
	///////////////////////////////////////////
	void ProcessReceivedPacket(char* DataBuffer, SOCKET Socket);

	////////////////////////////////////////////////
	// ��뷮 ��Ŷ ���� 
	////////////////////////////////////////////////
	template<typename T>
	static void DivideHugePacket(SOCKET Socket, stringstream& SendStream, EPacketType PacketType, T& queue);

	//////////////////////////////////////////////////
	//// (�ӽ�) ��Ŷ ������� ���� ���� ������ �Լ�
	//////////////////////////////////////////////////
	//static void VerifyPacket(char* DataBuffer, bool send);

	// �̱��� ��ü ��������
	static cGameServer* GetSingleton()
	{
		static cGameServer gameServer;
		return &gameServer;
	}


	////////////////////////
	// Ȯ��
	////////////////////////
	bool IsServerOn();
	int GetServerPort();

	////////////////////////
	// ���
	////////////////////////
	static void Broadcast(stringstream& SendStream);
	static void BroadcastExceptOne(stringstream& SendStream, SOCKET Except);

	static void Connected(stringstream& RecvStream, SOCKET Socket);

	static void ScoreBoard(stringstream& RecvStream, SOCKET Socket);

	static void SendSpaceShip(cInfoOfSpaceShip InfoOfSpaceShip);

	static void Observation(stringstream& RecvStream, SOCKET Socket);
	static int SizeOfObservers();
	static void InsertAtObersers(SOCKET Socket);

	static void SendSpawnPioneer(cInfoOfPioneer InfoOfPioneer);
	static void SendSpawnedPioneer(SOCKET Socket);

	static void DiedPioneer(stringstream& RecvStream, SOCKET Socket);
	static cThreadSafetyQueue<int> tsqDiedPioneer;

	static void InfoOfPioneer_Animation(stringstream& RecvStream, SOCKET Socket);
	static cThreadSafetyQueue<cInfoOfPioneer_Animation> tsqInfoOfPioneer_Animation;

	static void PossessPioneer(stringstream& RecvStream, SOCKET Socket);
	bool PossessingPioneer(cInfoOfPioneer_Socket Socket);
	static cThreadSafetyQueue<cInfoOfPioneer_Socket> tsqInfoOfPioneer_Socket;

	static void InfoOfPioneer_Stat(stringstream& RecvStream, SOCKET Socket);
	static cThreadSafetyQueue<cInfoOfPioneer_Stat> tsqInfoOfPioneer_Stat;

	static void SendInfoOfProjectile(cInfoOfProjectile InfoOfProjectile);
	static void InfoOfProjectile(stringstream& RecvStream, SOCKET Socket);
	static cThreadSafetyQueue<cInfoOfProjectile> tsqInfoOfProjectile;

	static void SendInfoOfResources(cInfoOfResources InfoOfResources);

	static void SendInfoOfBuilding_Spawn(cInfoOfBuilding_Spawn Spawn);
	static void SendInfoOfBuilding_Spawned(SOCKET Socket);
	static void RecvInfoOfBuilding_Spawn(stringstream& RecvStream, SOCKET Socket);
	static cThreadSafetyQueue<cInfoOfBuilding_Spawn> tsqInfoOfBuilding_Spawn;

	static void SendInfoOfBuilding_Stat(stringstream& RecvStream, SOCKET Socket);

	static void SendDestroyBuilding(int IDOfBuilding);


	static void SendSpawnEnemy(cInfoOfEnemy InfoOfEnemy);
	static void SendSpawnedEnemy(SOCKET Socket);

	static void SendInfoOfEnemy_Animation(stringstream& RecvStream, SOCKET Socket);

	static void SendInfoOfEnemy_Stat(stringstream& RecvStream, SOCKET Socket);

	static void SendDestroyEnemy(int IDOfEnemy, int IDOfPioneer, int Exp);
};
