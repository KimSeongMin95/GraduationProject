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
	// 멤버 변수 초기화
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
	// 크리티컬 섹션들을 제거한다.
	DeleteCriticalSection(&csAccept);

	DeleteCriticalSection(&csGC_SocketInfo);
	DeleteCriticalSection(&csClients);
	DeleteCriticalSection(&csMapOfRecvDeque);
}

bool IocpServerBase::Initialize()
{
	WSADATA wsaData;

	// winsock 2.2 버전으로 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf_s("[ERROR] winsock 초기화 실패\n");
		return false;
	}

	// 소켓 생성
	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (ListenSocket == INVALID_SOCKET)
	{
		WSACleanup();

		printf_s("[ERROR] 소켓 생성 실패\n");
		return false;
	}

	// 서버 정보 설정
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// 소켓 설정
	// boost bind 와 구별짓기 위해 ::bind 사용
	if (::bind(ListenSocket, (struct sockaddr*) & serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;
		WSACleanup();

		printf_s("[ERROR] bind 실패\n");
		return false;
	}

	// 수신 대기열 생성
	if (listen(ListenSocket, 5) == SOCKET_ERROR)
	{
		closesocket(ListenSocket);
		ListenSocket = NULL;
		WSACleanup();

		printf_s("[ERROR] listen 실패\n");
		return false;
	}

	return true;
}

void IocpServerBase::StartServer()
{
	// 클라이언트 정보
	SOCKET clientSocket;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	DWORD flags = 0;
	
	// Completion Port 객체 생성
	// 4번째 인자는 최대 스레드 수로서 0이면 시스템 코어 수에 맞춰집니다.
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// Worker Thread 생성
	if (!CreateWorkerThread())
	{
		printf_s("[ERROR] <IocpServerBase::StartServer()> if (!CreateWorkerThread()) \n");
		return;
	}

	printf_s("[INFO] <IocpServerBase::StartServer()> 서버 시작...\n");

	// 클라이언트 접속을 받음
	while (bAccept)
	{
		// 메인스레드 종료 확인
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
			printf_s("[ERROR] <IocpServerBase::StartServer()> WSAAccept 실패\n");
			// closesocket(ListenSocket); 하면 여기서 종료됩니다.
			continue;
		}
		else
		{
			printf_s("[INFO] <IocpServerBase::StartServer()> WSAAccept 성공, SocketID: %d\n", int(clientSocket));
			
			// 소켓 버퍼 크기 변경
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


		// char *inet_ntoa(struct in_addr adr); // 역으로 네트워크바이트순서로 된 정32비트 정수를 다시 문자열로 돌려주는 함수
		SocketInfo->IPv4Addr = string(inet_ntoa(clientAddr.sin_addr));
		printf_s("[INFO] <IocpServerBase::StartServer()> Client's IP: %s\n", SocketInfo->IPv4Addr.c_str());

		SocketInfo->Port = (int)ntohs(clientAddr.sin_port);
		printf_s("[INFO] <IocpServerBase::StartServer()> Client's Port: %d\n\n", SocketInfo->Port);


		// 동적할당한 소켓 정보를 저장 (서버가 완전히 종료되면 할당 해제)
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

		// 동적할당한 소켓 정보를 저장 (delete 금지)
		EnterCriticalSection(&csClients);
		printf_s("[[INFO] <IocpServerBase::StartServer()> Clients.size(): %d\n", (int)Clients.size());
		Clients[clientSocket] = SocketInfo;
		printf_s("[[INFO] <IocpServerBase::StartServer()> Clients.size(): %d\n", (int)Clients.size());
		LeaveCriticalSection(&csClients);

		// 동적할당한 소켓에 대한 recvDeque을 동적할당하여 저장
		deque<char*>* recvDeque = new deque<char*>();
		EnterCriticalSection(&csMapOfRecvDeque);
		if (MapOfRecvDeque.find(clientSocket) == MapOfRecvDeque.end())
		{
			MapOfRecvDeque.insert(pair<SOCKET, deque<char*>*>(clientSocket, recvDeque));
		}
		LeaveCriticalSection(&csMapOfRecvDeque);


		// SocketInfo를 hIOCP에 등록?
		//hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (DWORD)SocketInfo, 0);
		hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (ULONG_PTR)SocketInfo, 0);

		// CreateIoCompletionPort의 (ULONG_PTR)SocketInfo 인자가 GetQueuedCompletionStatus의 (PULONG_PTR)& pCompletionKey이다.
		// GetQueuedCompletionStatus에서 (LPOVERLAPPED*)& pSocketInfo는 WSASend, WSARecv 함수 호출시 전달되는 WSAOVERLAPPED 구조체 주소 값이다.
		
		// 중첩 소켓을 지정하고 완료시 실행될 함수를 넘겨줌
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
				printf_s("[ERROR] <IocpServerBase::StartServer()> IO Pending 실패 : %d\n", WSAGetLastError());
				
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
	//// 소켓 유효성 검증
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

	//// stSOCKETINFO 데이터 초기화
	//ZeroMemory(&(pSocketInfo->overlapped), sizeof(OVERLAPPED));
	//pSocketInfo->overlapped.hEvent = NULL; // IOCP에서는 overlapped.hEvent를 꼭 NULL로 해줘야 한다고 합니다.
	//ZeroMemory(pSocketInfo->messageBuffer, MAX_BUFFER);
	//pSocketInfo->dataBuf.len = MAX_BUFFER;
	//pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
	//pSocketInfo->recvBytes = 0;
	//pSocketInfo->sendBytes = 0;
	//pSocketInfo->sentBytes = 0;

	//// 클라이언트로부터 다시 응답을 받기 위해 WSARecv 를 호출해줌
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
	//		printf_s("[ERROR] WSARecv 실패 : %d\n", WSAGetLastError());

	//		CloseSocket(pSocketInfo->socket);
	//	}
	//	else
	//	{
	//		printf_s("[INFO] <IocpServerBase::Recv(...)> WSARecv: WSA_IO_PENDING \n");
	//	}
	//}
}


///////////////////////////////////////////
// stringstream의 맨 앞에 size를 추가
///////////////////////////////////////////
bool IocpServerBase::AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
{
	if (DataStream.str().length() == 0)
	{
		printf_s("[ERROR] <AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");
		return false;
	}
	//printf_s("[START] <AddSizeInStream(...)> \n");

	// ex) DateStream의 크기 : 98
	//printf_s("\t DataStream size: %d\n", (int)DataStream.str().length());
	//printf_s("\t DataStream: %s\n", DataStream.str().c_str());

	// dataStreamLength의 크기 : 3 [98 ]
	stringstream dataStreamLength;
	dataStreamLength << DataStream.str().length() << endl;

	// lengthOfFinalStream의 크기 : 4 [101 ]
	stringstream lengthOfFinalStream;
	lengthOfFinalStream << (dataStreamLength.str().length() + DataStream.str().length()) << endl;

	// FinalStream의 크기 : 101 [101 DataStream]
	int sizeOfFinalStream = (int)(lengthOfFinalStream.str().length() + DataStream.str().length());
	FinalStream << sizeOfFinalStream << endl;
	FinalStream << DataStream.str(); // 이미 DataStream.str() 마지막에 endl;를 사용했으므로 여기선 다시 사용하지 않습니다.

	//printf_s("\t FinalStream size: %d\n", (int)FinalStream.str().length());
	//printf_s("\t FinalStream: %s\n", FinalStream.str().c_str());


	// 전송할 데이터가 최대 버퍼 크기보다 크거나 같으면 전송 불가능을 알립니다.
	// messageBuffer[MAX_BUFFER];에서 마지막에 '\0'을 넣어줘야 되기 때문에 MAX_BUFFER와 같을때도 무시합니다.
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
// 소켓 버퍼 크기 변경
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

		/* 검증
		1048576B == 1024KB
		TCP에선 send buffer와 recv buffer 모두 1048576 * 256까지 가능.
		*/

	printf_s("[START] <SetSockOpt(...)> \n");


	int optval;
	int optlen = sizeof(optval);

	// 성공시 0, 실패시 -1 반환
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