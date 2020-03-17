#pragma once

#include "Packet.h"

using namespace std;

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

class IocpServerBase
{
protected:
	stSOCKETINFO*	SocketInfo = nullptr;	 // ���� ����, �����Ҵ��ϰ� Clients�� �Ѱ��ִ� �뵵
	SOCKET			ListenSocket;			 // ���� ���� ����
	HANDLE			hIOCP;					 // IOCP ��ü �ڵ�

	bool			bAccept;				 // ��û ���� �÷���

	bool			bWorkerThread;			 // �۾� ������ ���� �÷���
	HANDLE*			hWorkerHandle = nullptr; // �۾� ������ �ڵ�		
	DWORD			nThreadCnt;				 // �۾� ������ ����

public:
	// WSAAccept�� ��� Ŭ���̾�Ʈ�� new stSOCKETINFO()�� ����, (������ ���� �� �Ҵ� �����뵵)
	static queue<stSOCKETINFO*> GC_SocketInfo;
	static CRITICAL_SECTION csGC_SocketInfo;

	// WSAAccept�� ��� Ŭ���̾�Ʈ�� new stSOCKETINFO()�� ����, (delete ����)
	static map<SOCKET, stSOCKETINFO*> Clients;
	static CRITICAL_SECTION csClients;

	// ������ �����͸� ���� ���� ����
	static map<SOCKET, deque<char*>*> MapOfRecvDeque;
	static CRITICAL_SECTION csMapOfRecvDeque;


public:
	IocpServerBase();
	virtual ~IocpServerBase();

	// ���� ��� �� ���� ���� ����
	bool Initialize();

	// ���� ����
	virtual void StartServer();

	// �۾� ������ ����
	virtual bool CreateWorkerThread();

	// �۾� ������
	virtual void WorkerThread();

	// Ŭ���̾�Ʈ ���� ����
	virtual void CloseSocket(SOCKET Socket);

	// Ŭ���̾�Ʈ���� �۽�
	virtual void Send(stringstream& SendStream, SOCKET Socket);

	// Ŭ���̾�Ʈ ���� ���
	virtual void Recv(SOCKET Socket);

	///////////////////////////////////////////
	// stringstream�� �� �տ� size�� �߰�
	///////////////////////////////////////////
	static bool AddSizeInStream(stringstream& DataStream, stringstream& FinalStream);

	///////////////////////////////////////////
	// ���� ���� ũ�� ����
	///////////////////////////////////////////
	void SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf);
};