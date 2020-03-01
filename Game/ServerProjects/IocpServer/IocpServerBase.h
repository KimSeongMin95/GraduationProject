#pragma once

#include "Packet.h"

using namespace std;

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
	// WSAAccept�� ��� Ŭ���̾�Ʈ�� new stSOCKETINFO()�� ����
	static std::map<SOCKET, stSOCKETINFO*> Clients;
	static CRITICAL_SECTION csClients;

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
	virtual void CloseSocket(stSOCKETINFO* pSocketInfo);

	// Ŭ���̾�Ʈ���� �۽�
	virtual void Send(stSOCKETINFO* pSocketInfo);

	// Ŭ���̾�Ʈ ���� ���
	virtual void Recv(stSOCKETINFO* pSocketInfo);
};