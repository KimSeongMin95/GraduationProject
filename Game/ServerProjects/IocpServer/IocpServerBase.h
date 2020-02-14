#pragma once

// ��Ƽ����Ʈ ���� ���� define
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// winsock2 ����� ���� �Ʒ� �ڸ�Ʈ �߰�
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <iostream>
#include <process.h>
#include <sstream>
#include <algorithm>
#include <string>

#include "Packet.h"

using namespace std;

#define	MAX_BUFFER		4096
#define SERVER_PORT		8000
#define MAX_CLIENTS		1000

// IOCP ���� ����ü
struct stSOCKETINFO
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;
	SOCKET			socket;
	char			messageBuffer[MAX_BUFFER];
	int				recvBytes;
	int				sendBytes;

	string			IPv4Addr; // Ŭ���̾�Ʈ�� IP �ּ�
	int				Port;	  // Ŭ���̾�Ʈ�� Port �ּ�
};

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
	stSOCKETINFO*	SocketInfo;		// ���� ����
	SOCKET			ListenSocket;	// ���� ���� ����
	HANDLE			hIOCP;			// IOCP ��ü �ڵ�
	bool			bAccept;		// ��û ���� �÷���
	bool			bWorkerThread;	// �۾� ������ ���� �÷���
	HANDLE*			hWorkerHandle;	// �۾� ������ �ڵ�		
	int				nThreadCnt;		// �۾� ������ ����

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