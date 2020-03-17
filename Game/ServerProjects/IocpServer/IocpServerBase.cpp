#include "IocpServerBase.h"


map<SOCKET, stSOCKETINFO*> IocpServerBase::GC_SocketInfo;
CRITICAL_SECTION IocpServerBase::csGC_SocketInfo;

map<SOCKET, stSOCKETINFO*> IocpServerBase::Clients;
CRITICAL_SECTION IocpServerBase::csClients;

map<SOCKET, deque<char*>*> IocpServerBase::MapOfRecvDeque;
CRITICAL_SECTION IocpServerBase::csMapOfRecvDeque;


IocpServerBase::IocpServerBase()
{
	///////////////////
	// ��� ���� �ʱ�ȭ
	///////////////////
	SocketInfo = nullptr;
	ListenSocket = NULL;
	hIOCP = NULL;

	bAccept = true;

	bWorkerThread = true;
	hWorkerHandle = nullptr;
	nThreadCnt = 0;

	InitializeCriticalSection(&csAccept);

	InitializeCriticalSection(&csGC_SocketInfo);
	InitializeCriticalSection(&csClients);
	InitializeCriticalSection(&csMapOfRecvDeque);
}

IocpServerBase::~IocpServerBase()
{
	// ũ��Ƽ�� ���ǵ��� �����Ѵ�.
	DeleteCriticalSection(&csAccept);

	DeleteCriticalSection(&csGC_SocketInfo);
	DeleteCriticalSection(&csClients);
	DeleteCriticalSection(&csMapOfRecvDeque);
}

bool IocpServerBase::Initialize()
{
	WSADATA wsaData;

	// winsock 2.2 �������� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf_s("[ERROR] winsock �ʱ�ȭ ����\n");
		return false;
	}

	// ���� ����
	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (ListenSocket == INVALID_SOCKET)
	{
		WSACleanup();

		printf_s("[ERROR] ���� ���� ����\n");
		return false;
	}

	// ���� ���� ����
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// ���� ����
	// boost bind �� �������� ���� ::bind ���
	if (::bind(ListenSocket, (struct sockaddr*) & serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;
		WSACleanup();

		printf_s("[ERROR] bind ����\n");
		return false;
	}

	// ���� ��⿭ ����
	if (listen(ListenSocket, 5) == SOCKET_ERROR)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;
		WSACleanup();

		printf_s("[ERROR] listen ����\n");
		return false;
	}

	return true;
}

void IocpServerBase::StartServer()
{
	// Ŭ���̾�Ʈ ����
	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	DWORD flags = 0;
	
	// Completion Port ��ü ����
	// 4��° ���ڴ� �ִ� ������ ���μ� 0�̸� �ý��� �ھ� ���� �������ϴ�.
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// Worker Thread ����
	if (!CreateWorkerThread())
	{
		printf_s("[ERROR] <IocpServerBase::StartServer()> if (!CreateWorkerThread()) \n");
		return;
	}

	printf_s("[INFO] <IocpServerBase::StartServer()> ���� ����...\n");

	// Ŭ���̾�Ʈ ������ ����
	while (bAccept)
	{
		// ���ν����� ���� Ȯ��
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			bAccept = true;
			printf_s("[INFO] <cServerSocketInGame::StartServer()> if (!bAccept) \n");
			printf_s("[INFO] <cServerSocketInGame::StartServer()> Main Thread is Closeed! \n");
			return;
		}
		LeaveCriticalSection(&csAccept);

		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*)& clientAddr, &addrLen, NULL, NULL);
	
		if (clientSocket == INVALID_SOCKET)
		{
			printf_s("[ERROR] <IocpServerBase::StartServer()> WSAAccept ����\n");
			// closesocket(ListenSocket); �ϸ� ���⼭ ����˴ϴ�.
			continue;
		}
		else
		{
			printf_s("[INFO] <IocpServerBase::StartServer()> WSAAccept ����, SocketID: %d\n", int(clientSocket));
			
			// ���� ���� ũ�� ����
			SetSockOpt(clientSocket, 1048576, 1048576);
		}

		SocketInfo = new stSOCKETINFO();
		memset(&(SocketInfo->overlapped), 0, sizeof(OVERLAPPED));
		//ZeroMemory(&(SocketInfo->overlapped), sizeof(OVERLAPPED));
		memset(&(SocketInfo->messageBuffer), 0, MAX_BUFFER);
		//ZeroMemory(SocketInfo->messageBuffer, MAX_BUFFER);
		SocketInfo->dataBuf.len = MAX_BUFFER;
		SocketInfo->dataBuf.buf = SocketInfo->messageBuffer;
		SocketInfo->socket = clientSocket;
		SocketInfo->recvBytes = 0;
		SocketInfo->sendBytes = 0;
		SocketInfo->sentBytes = 0;

		//flags = 0;


		// char *inet_ntoa(struct in_addr adr); // ������ ��Ʈ��ũ����Ʈ������ �� ��32��Ʈ ������ �ٽ� ���ڿ��� �����ִ� �Լ�
		SocketInfo->IPv4Addr = string(inet_ntoa(clientAddr.sin_addr));
		printf_s("[INFO] <IocpServerBase::StartServer()> Client's IP: %s\n", SocketInfo->IPv4Addr.c_str());

		SocketInfo->Port = (int)ntohs(clientAddr.sin_port);
		printf_s("[INFO] <IocpServerBase::StartServer()> Client's Port: %d\n\n", SocketInfo->Port);


		// �����Ҵ��� ���� ������ ���� (������ ������ ����Ǹ� �Ҵ� ����)
		EnterCriticalSection(&csGC_SocketInfo);
		if (GC_SocketInfo.find(clientSocket) != GC_SocketInfo.end())
		{
			printf_s("\n\n\n\n\n\n\n\n\n\n");
			printf_s("[[INFO] <IocpServerBase::StartServer()> if (GC_SocketInfo.find(clientSocket) != GC_SocketInfo.end()) \n");
			printf_s("\n\n\n\n\n\n\n\n\n\n");

			delete GC_SocketInfo[clientSocket];
			GC_SocketInfo.erase(clientSocket);
		}
		GC_SocketInfo[clientSocket] = SocketInfo;
		LeaveCriticalSection(&csGC_SocketInfo);

		// �����Ҵ��� ���� ������ ���� (delete ����)
		EnterCriticalSection(&csClients);
		printf_s("[[INFO] <IocpServerBase::StartServer()> Clients.size(): %d\n", (int)Clients.size());
		Clients[clientSocket] = SocketInfo;
		printf_s("[[INFO] <IocpServerBase::StartServer()> Clients.size(): %d\n", (int)Clients.size());
		LeaveCriticalSection(&csClients);

		// �����Ҵ��� ���Ͽ� ���� recvDeque�� �����Ҵ��Ͽ� ����
		deque<char*>* recvDeque = new deque<char*>();
		EnterCriticalSection(&csMapOfRecvDeque);
		if (MapOfRecvDeque.find(clientSocket) == MapOfRecvDeque.end())
		{
			MapOfRecvDeque.insert(pair<SOCKET, deque<char*>*>(clientSocket, recvDeque));
		}
		LeaveCriticalSection(&csMapOfRecvDeque);


		// SocketInfo�� hIOCP�� ���?
		//hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (DWORD)SocketInfo, 0);
		hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (ULONG_PTR)SocketInfo, 0);

		// CreateIoCompletionPort�� (ULONG_PTR)SocketInfo ���ڰ� GetQueuedCompletionStatus�� (PULONG_PTR)& pCompletionKey�̴�.
		// GetQueuedCompletionStatus���� (LPOVERLAPPED*)& pSocketInfo�� WSASend, WSARecv �Լ� ȣ��� ���޵Ǵ� WSAOVERLAPPED ����ü �ּ� ���̴�.
		
		// ��ø ������ �����ϰ� �Ϸ�� ����� �Լ��� �Ѱ���
		int nResult = WSARecv(
			SocketInfo->socket,
			&(SocketInfo->dataBuf),
			1,
			(LPDWORD)& SocketInfo->recvBytes,
			&flags,
			&(SocketInfo->overlapped),
			NULL
		);

		if (nResult == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSA_IO_PENDING)
			{
				printf_s("[INFO] <IocpServerBase::StartServer()> WSA_IO_PENDING \n");
			}
			else
			{
				printf_s("[ERROR] <IocpServerBase::StartServer()> IO Pending ���� : %d\n", WSAGetLastError());
				
				delete SocketInfo;
				SocketInfo = nullptr;	

				EnterCriticalSection(&csGC_SocketInfo);
				if (GC_SocketInfo.find(clientSocket) != GC_SocketInfo.end())
				{
					GC_SocketInfo.erase(clientSocket);
				}
				LeaveCriticalSection(&csGC_SocketInfo);

				EnterCriticalSection(&csClients);
				if (Clients.find(clientSocket) != Clients.end())
				{
					printf_s("[[INFO] <IocpServerBase::StartServer()> Clients.size(): %d\n", (int)Clients.size());
					Clients.erase(clientSocket);
					printf_s("[[INFO] <IocpServerBase::StartServer()> Clients.size(): %d\n", (int)Clients.size());
				}
				LeaveCriticalSection(&csClients);

				EnterCriticalSection(&csMapOfRecvDeque);
				if (MapOfRecvDeque.find(clientSocket) != MapOfRecvDeque.end())
				{
					delete MapOfRecvDeque.at(clientSocket);
					MapOfRecvDeque.erase(clientSocket);
				}
				LeaveCriticalSection(&csMapOfRecvDeque);
				
				continue;
			}
		}
		else
		{
			printf_s("[INFO] <IocpServerBase::StartServer()> WSARecv(...) \n");
		}
	}

}

bool IocpServerBase::CreateWorkerThread()
{
	return false;
}

void IocpServerBase::WorkerThread()
{
	//
}

void IocpServerBase::CloseSocket(SOCKET Socket)
{
	//
}

void IocpServerBase::Send(stringstream& SendStream, SOCKET Socket)
{
	//
}

void IocpServerBase::Recv(SOCKET Socket)
{
	///////////////////////////////
	//// ���� ��ȿ�� ����
	///////////////////////////////
	//EnterCriticalSection(&csClients);
	//if (Clients.find(Socket) == Clients.end())
	//{
	//	printf_s("[ERROR] <IocpServerBase::Recv(...)> if (Clients.find(Socket) == Clients.end()) \n");
	//	LeaveCriticalSection(&csClients);
	//	return;
	//}
	//stSOCKETINFO* pSocketInfo = Clients.at(Socket);
	//if (pSocketInfo->socket == NULL || pSocketInfo->socket == INVALID_SOCKET)
	//{
	//	printf_s("[ERROR] <IocpServerBase::Recv(...)> if (pSocketInfo->socket == NULL || pSocketInfo->socket == INVALID_SOCKET) \n");
	//	LeaveCriticalSection(&csClients);
	//	return;
	//}
	//LeaveCriticalSection(&csClients);


	//// DWORD sendBytes;
	//DWORD dwFlags = 0;

	//// stSOCKETINFO ������ �ʱ�ȭ
	//ZeroMemory(&(pSocketInfo->overlapped), sizeof(OVERLAPPED));
	//pSocketInfo->overlapped.hEvent = NULL; // IOCP������ overlapped.hEvent�� �� NULL�� ����� �Ѵٰ� �մϴ�.
	//ZeroMemory(pSocketInfo->messageBuffer, MAX_BUFFER);
	//pSocketInfo->dataBuf.len = MAX_BUFFER;
	//pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
	//pSocketInfo->recvBytes = 0;
	//pSocketInfo->sendBytes = 0;
	//pSocketInfo->sentBytes = 0;

	//// Ŭ���̾�Ʈ�κ��� �ٽ� ������ �ޱ� ���� WSARecv �� ȣ������
	//int nResult = WSARecv(
	//	pSocketInfo->socket,
	//	&(pSocketInfo->dataBuf),
	//	1,
	//	(LPDWORD)& (pSocketInfo->recvBytes),
	//	&dwFlags,
	//	(LPWSAOVERLAPPED)& (pSocketInfo->overlapped),
	//	NULL
	//);

	//if (nResult == SOCKET_ERROR)
	//{
	//	if (WSAGetLastError() != WSA_IO_PENDING)
	//	{
	//		printf_s("[ERROR] WSARecv ���� : %d\n", WSAGetLastError());

	//		CloseSocket(pSocketInfo->socket);
	//	}
	//	else
	//	{
	//		printf_s("[INFO] <IocpServerBase::Recv(...)> WSARecv: WSA_IO_PENDING \n");
	//	}
	//}
}


///////////////////////////////////////////
// stringstream�� �� �տ� size�� �߰�
///////////////////////////////////////////
bool IocpServerBase::AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
{
	if (DataStream.str().length() == 0)
	{
		printf_s("[ERROR] <AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");
		return false;
	}
	//printf_s("[START] <AddSizeInStream(...)> \n");

	// ex) DateStream�� ũ�� : 98
	//printf_s("\t DataStream size: %d\n", (int)DataStream.str().length());
	//printf_s("\t DataStream: %s\n", DataStream.str().c_str());

	// dataStreamLength�� ũ�� : 3 [98 ]
	stringstream dataStreamLength;
	dataStreamLength << DataStream.str().length() << endl;

	// lengthOfFinalStream�� ũ�� : 4 [101 ]
	stringstream lengthOfFinalStream;
	lengthOfFinalStream << (dataStreamLength.str().length() + DataStream.str().length()) << endl;

	// FinalStream�� ũ�� : 101 [101 DataStream]
	int sizeOfFinalStream = (int)(lengthOfFinalStream.str().length() + DataStream.str().length());
	FinalStream << sizeOfFinalStream << endl;
	FinalStream << DataStream.str(); // �̹� DataStream.str() �������� endl;�� ��������Ƿ� ���⼱ �ٽ� ������� �ʽ��ϴ�.

	//printf_s("\t FinalStream size: %d\n", (int)FinalStream.str().length());
	//printf_s("\t FinalStream: %s\n", FinalStream.str().c_str());


	// ������ �����Ͱ� �ִ� ���� ũ�⺸�� ũ�ų� ������ ���� �Ұ����� �˸��ϴ�.
	// messageBuffer[MAX_BUFFER];���� �������� '\0'�� �־���� �Ǳ� ������ MAX_BUFFER�� �������� �����մϴ�.
	if (FinalStream.str().length() >= MAX_BUFFER)
	{
		printf_s("\n\n\n\n\n\n\n\n\n\n");
		printf_s("[ERROR] <AddSizeInStream(...)> if (FinalStream.str().length() > MAX_BUFFER \n");
		printf_s("[ERROR] <AddSizeInStream(...)> FinalStream.str().length(): %d \n", (int)FinalStream.str().length());
		printf_s("[ERROR] <AddSizeInStream(...)> FinalStream.str().c_str(): %s \n", FinalStream.str().c_str());
		printf_s("\n\n\n\n\n\n\n\n\n\n");
		return false;
	}


	//printf_s("[END] <AddSizeInStream(...)> \n");

	return true;
}


///////////////////////////////////////////
// ���� ���� ũ�� ����
///////////////////////////////////////////
void IocpServerBase::SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf)
{
	/*
		The maximum send buffer size is 1,048,576 bytes.
		The default value of the SO_SNDBUF option is 32,767.
		For a TCP socket, the maximum length that you can specify is 1 GB.
		For a UDP or RAW socket, the maximum length that you can specify is the smaller of the following values:
		65,535 bytes (for a UDP socket) or 32,767 bytes (for a RAW socket).
		The send buffer size defined by the SO_SNDBUF option.
		*/

		/* ����
		1048576B == 1024KB
		TCP���� send buffer�� recv buffer ��� 1048576 * 256���� ����.
		*/

	printf_s("[START] <SetSockOpt(...)> \n");


	int optval;
	int optlen = sizeof(optval);

	// ������ 0, ���н� -1 ��ȯ
	if (getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, &optlen) == 0)
	{
		printf_s("\t Socket: %d, getsockopt SO_SNDBUF: %d \n", (int)Socket, optval);
	}
	if (getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, &optlen) == 0)
	{
		printf_s("\t Socket: %d, getsockopt SO_RCVBUF: %d \n", (int)Socket, optval);
	}

	optval = SendBuf;
	if (setsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, sizeof(optval)) == 0)
	{
		printf_s("\t Socket: %d, setsockopt SO_SNDBUF: %d \n", (int)Socket, optval);
	}
	optval = RecvBuf;
	if (setsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, sizeof(optval)) == 0)
	{
		printf_s("\t Socket: %d, setsockopt SO_RCVBUF: %d \n", (int)Socket, optval);
	}

	if (getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, &optlen) == 0)
	{
		printf_s("\t Socket: %d, getsockopt SO_SNDBUF: %d \n", (int)Socket, optval);
	}
	if (getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, &optlen) == 0)
	{
		printf_s("\t Socket: %d, getsockopt SO_RCVBUF: %d \n", (int)Socket, optval);
	}


	printf_s("[END] <SetSockOpt(...)> \n");
}