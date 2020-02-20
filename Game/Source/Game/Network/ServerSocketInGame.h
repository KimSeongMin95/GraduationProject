// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Packet.h"

#define SERVER_PORT		9000

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

class GAME_API cServerSocketInGame
{
private:
	FuncProcess	fnProcess[100];	// ��Ŷ ó�� ����ü

protected:
	stSOCKETINFO*	SocketInfo = nullptr;	// ���� ����
	SOCKET			ListenSocket;			// ���� ���� ����
	HANDLE			hIOCP;					// IOCP ��ü �ڵ�
	
	bool			bAccept;				// ��û ���� �÷��� (���� ������)
	HANDLE			hMainHandle;			// ���� ������ �ڵ�	

	bool			bWorkerThread;			// �۾� ������ ���� �÷��� (��Ŀ ������)
	HANDLE*			hWorkerHandle = nullptr;// �۾� ������ �ڵ�		
	int				nThreadCnt;				// �۾� ������ ����

	

public:
	// WSAAccept�� ��� Ŭ���̾�Ʈ�� new stSOCKETINFO()�� ����
	std::map<SOCKET, stSOCKETINFO*> Clients;
	CRITICAL_SECTION csClients;

public:
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
	void Send(stSOCKETINFO* pSocketInfo);

	// Ŭ���̾�Ʈ ���� ���
	void Recv(stSOCKETINFO* pSocketInfo);

	// �̱��� ��ü ��������
	static cServerSocketInGame* GetSingleton()
	{
		static cServerSocketInGame ins;
		return &ins;
	}

	////////////////////////
	// ���
	////////////////////////





/*** Console for log : Start ***/
private:
	FILE* fp_console = nullptr;

	void AllocMyConsole();
	void FreeMyConsole();
/*** Console for log : End ***/
};
