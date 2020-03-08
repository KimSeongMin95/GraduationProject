// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Packet.h"


// ��Ŷ ó�� �Լ� ������
struct FuncProcess
{
	// RecvStream�� ������ ����, pSocket�� Overlapped I/O �۾��� �߻��� IOCP ���� ����ü ����
	void(*funcProcessPacket)(stringstream& RecvStream, stSOCKETINFO* pSocketInfo);
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
	stSOCKETINFO*	SocketInfo = nullptr;	// ���� ����
	SOCKET			ListenSocket;			// ���� ���� ����
	HANDLE			hIOCP;					// IOCP ��ü �ڵ�
	
	bool			bAccept;				// ��û ���� �÷��� (���� ������)
	HANDLE			hMainHandle;			// ���� ������ �ڵ�	

	bool			bWorkerThread;			// �۾� ������ ���� �÷��� (��Ŀ ������)
	HANDLE*			hWorkerHandle = nullptr;// �۾� ������ �ڵ�		
	DWORD			nThreadCnt;				// �۾� ������ ����

public:
	/** ���Ӽ����� �ӽ� ���� */
	SOCKET SocketID;

	// WSAAccept�� ��� Ŭ���̾�Ʈ�� new stSOCKETINFO()�� ����
	static std::map<SOCKET, stSOCKETINFO*> GameClients;
	static CRITICAL_SECTION csGameClients;

	// Connected Ŭ���̾�Ʈ�� InfoOfPlayer ����
	static std::map<SOCKET, cInfoOfPlayer> InfoOfClients;
	static CRITICAL_SECTION csInfoOfClients;

	// 
	static std::map<SOCKET, cInfoOfScoreBoard> InfosOfScoreBoard;
	static CRITICAL_SECTION csInfosOfScoreBoard;

	class cClientSocket* ClientSocket = nullptr;

	static cThreadSafetyQueue<SOCKET> tsqObserver;

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
	void CloseSocket(stSOCKETINFO* pSocketInfo);

	// Ŭ���̾�Ʈ���� �۽�
	static void Send(stSOCKETINFO* pSocketInfo);

	// Ŭ���̾�Ʈ ���� ���
	void Recv(stSOCKETINFO* pSocketInfo);

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
	static void Connected(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void ScoreBoard(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void Observation(stringstream& RecvStream, stSOCKETINFO* pSocket);
};
