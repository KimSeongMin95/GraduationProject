// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Packet.h"


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
class GAME_API cServerSocketInGame
{
private:
	FuncProcess	fnProcess[100];	// ��Ŷ ó�� ����ü

	static int ServerPort;
	bool bIsServerOn;

protected:
	stSOCKETINFO*	 SocketInfo = nullptr;	  // ���� ����
	SOCKET			 ListenSocket;			  // ���� ���� ����
	HANDLE			 hIOCP;					  // IOCP ��ü �ڵ�
	
	bool			 bAccept;				  // ��û ���� �÷��� (���� ������)
	CRITICAL_SECTION csAccept;
	HANDLE			 hMainHandle;			  // ���� ������ �ڵ�	

	bool			 bWorkerThread;			  // �۾� ������ ���� �÷��� (��Ŀ ������)
	HANDLE*			 hWorkerHandle = nullptr; // �۾� ������ �ڵ�		
	DWORD			 nThreadCnt;			  // �۾� ������ ����

public:
	/** ���Ӽ����� �ӽ� ���� */
	SOCKET SocketID;

	class cClientSocket* ClientSocket = nullptr;

	// WSAAccept�� ��� Ŭ���̾�Ʈ�� new stSOCKETINFO()�� ����, (������ ���� �� �Ҵ� �����뵵)
	static map<SOCKET, stSOCKETINFO*> GC_SocketInfo;
	static CRITICAL_SECTION csGC_SocketInfo;

	// WSAAccept�� ��� Ŭ���̾�Ʈ�� new stSOCKETINFO()�� ����, (delete ����)
	static std::map<SOCKET, stSOCKETINFO*> GameClients;
	static CRITICAL_SECTION csGameClients;

	// ������ �����͸� ���� ���� ����
	static map<SOCKET, deque<char*>*> MapOfRecvDeque;
	static CRITICAL_SECTION csMapOfRecvDeque;


	static unsigned int CountOfSend;
	static CRITICAL_SECTION csCountOfSend;


	// Connected Ŭ���̾�Ʈ�� InfoOfPlayer ����
	static std::map<SOCKET, cInfoOfPlayer> InfoOfClients;
	static CRITICAL_SECTION csInfoOfClients;

	static std::map<SOCKET, cInfoOfScoreBoard> InfosOfScoreBoard;
	static CRITICAL_SECTION csInfosOfScoreBoard;

	static std::map<int, cInfoOfPioneer> InfosOfPioneers;
	static CRITICAL_SECTION csInfosOfPioneers;

public:
	////////////////////////
	// �⺻
	////////////////////////
	cServerSocketInGame();
	~cServerSocketInGame();

	// ���� ��� �� ���� ���� ����
	bool Initialize();

	// ���� ����
	void StartServer();

	// ���� ����
	void CloseServer();

	// �۾� ������ ����
	bool CreateWorkerThread();

	// �۾� ������
	void WorkerThread();

	// Ŭ���̾�Ʈ ���� ����
	static void CloseSocket(SOCKET Socket);

	// Ŭ���̾�Ʈ���� �۽�
	static void Send(stringstream& SendStream, SOCKET Socket);

	// Ŭ���̾�Ʈ ���� ���
	static void Recv(SOCKET Socket);

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


	// �̱��� ��ü ��������
	static cServerSocketInGame* GetSingleton()
	{
		static cServerSocketInGame ins;
		return &ins;
	}


	////////////////////////
	// Ȯ��
	////////////////////////
	bool IsServerOn() { return bIsServerOn; }
	int GetServerPort() { return ServerPort; }


	////////////////////////
	// ���
	////////////////////////
	static void Broadcast(stringstream& SendStream);
	static void BroadcastExceptOne(stringstream& SendStream, SOCKET Except);

	static void Connected(stringstream& RecvStream, SOCKET Socket);

	static void ScoreBoard(stringstream& RecvStream, SOCKET Socket);

	static void SendSpaceShip(cInfoOfSpaceShip InfoOfSpaceShip);

	static void Observation(stringstream& RecvStream, SOCKET Socket);
	static cThreadSafetyQueue<SOCKET> tsqObserver;

	static void SendSpawnPioneer(cInfoOfPioneer InfoOfPioneer);
	static void SendSpawnedPioneer(SOCKET Socket);

	static void DiedPioneer(stringstream& RecvStream, SOCKET Socket);

	static void InfoOfPioneer(stringstream& RecvStream, SOCKET Socket);
	static cThreadSafetyQueue<cInfoOfPioneer> tsqInfoOfPioneer;
};
