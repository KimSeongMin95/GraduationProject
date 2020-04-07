// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
class MainServer
{
private:
	FuncProcess	fnProcess[100];	// ��Ŷ ó�� ����ü

protected:
	SOCKET			 ListenSocket;			// ���� ���� ����
	HANDLE			 hIOCP;					// IOCP ��ü �ڵ�

	bool			 bAccept;				// ��û ���� �÷���
	CRITICAL_SECTION csAccept;				//
	HANDLE			 hAcceptThreadHandle;	// Accept ������ �ڵ�	

	HANDLE*			 hIOThreadHandle;		// IO ������ �ڵ�		
	DWORD			 nIOThreadCnt;			// IO ������ ����


public:
	// WSAAccept(...)�� ��� Ŭ���̾�Ʈ�� new stCompletionKey()�� ����
	static map<SOCKET, stCompletionKey*> Clients;
	static CRITICAL_SECTION csClients;

	// ������ �����͸� ���� ���� ����
	static map<SOCKET, deque<char*>*> MapOfRecvDeque;
	static CRITICAL_SECTION csMapOfRecvDeque;

	// WSASend(...)�� �����ϸ� ++, ������ �Ϸ�ǰų� �����ϸ� --
	static unsigned int CountOfSend;
	static CRITICAL_SECTION csCountOfSend;

	/******************************************************/

	// Login�� Ŭ���̾�Ʈ�� InfoOfPlayer ����
	static std::map<SOCKET, cInfoOfPlayer> InfoOfClients;
	static CRITICAL_SECTION csInfoOfClients;

	// CreateGame�� Ŭ���̾�Ʈ�� cInfoOfGame ����
	static std::map<SOCKET, cInfoOfGame> InfoOfGames;
	static CRITICAL_SECTION csInfoOfGames;

public:
	////////////////////////
	// �⺻
	////////////////////////
	MainServer();
	~MainServer();

	// 
	void SetIPv4AndPort(IN_ADDR& IPv4, USHORT& Port);

	// �ʱ�ȭ ���н� ����
	void CloseListenSocketAndCleanupWSA();

	// ���� ��� �� ���� ���� ����
	bool Initialize();

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
	void CloseServer();

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

	////////////////////////////////////////////////
	// (�ӽ�) ��Ŷ ������� ���� ���� ������ �Լ�
	////////////////////////////////////////////////
	static void VerifyPacket(char* DataBuffer, bool send);

	// �̱��� ��ü ��������
	static MainServer* GetSingleton()
	{
		static MainServer server;
		return &server;
	}


	////////////////////////
	// Ȯ��
	////////////////////////
	bool IsServerOn();


	////////////////////////
	// Main Server / Min Clients
	////////////////////////
	static void Broadcast(stringstream& SendStream);
	static void BroadcastExceptOne(stringstream& SendStream, SOCKET Except);

	static void Login(stringstream& RecvStream, SOCKET Socket);

	static void CreateGame(stringstream& RecvStream, SOCKET Socket);

	static void FindGames(stringstream& RecvStream, SOCKET Socket);

	static void JoinOnlineGame(stringstream& RecvStream, SOCKET Socket);

	static void DestroyWaitingGame(stringstream& RecvStream, SOCKET Socket);

	static void ExitWaitingGame(stringstream& RecvStream, SOCKET Socket);

	static void ModifyWaitingGame(stringstream& RecvStream, SOCKET Socket);

	static void StartWaitingGame(stringstream& RecvStream, SOCKET Socket);

	///////////////////////////////////////////
	// Main Server / Game Server, Game Clients
	///////////////////////////////////////////
	static void ActivateGameServer(stringstream& RecvStream, SOCKET Socket);

	static void RequestInfoOfGameServer(stringstream& RecvStream, SOCKET Socket);
};
