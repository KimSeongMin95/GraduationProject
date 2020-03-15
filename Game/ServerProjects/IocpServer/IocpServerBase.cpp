#include "IocpServerBase.h"


map<SOCKET, stSOCKETINFO*> IocpServerBase::Clients;
CRITICAL_SECTION IocpServerBase::csClients;

map<SOCKET, queue<char*>*> IocpServerBase::MapOfRecvQueue;
CRITICAL_SECTION IocpServerBase::csMapOfRecvQueue;

multimap<SOCKET, stSOCKETINFO*> IocpServerBase::SendCollector;
CRITICAL_SECTION IocpServerBase::csSendCollector;

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

	InitializeCriticalSection(&csClients);
	InitializeCriticalSection(&csMapOfRecvQueue);
	InitializeCriticalSection(&csSendCollector);
}

IocpServerBase::~IocpServerBase()
{
	// 크리티컬 섹션들을 제거한다.
	DeleteCriticalSection(&csClients);
	DeleteCriticalSection(&csMapOfRecvQueue);
	DeleteCriticalSection(&csSendCollector);
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
		return;

	printf_s("[INFO] <IocpServerBase::StartServer()> 서버 시작...\n");

	// 클라이언트 접속을 받음
	while (bAccept)
	{
		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*)& clientAddr, &addrLen, NULL, NULL);
	
		if (clientSocket == INVALID_SOCKET)
		{
			printf_s("[ERROR] <IocpServerBase::StartServer()> WSAAccept 실패\n");
			// closesocket(ListenSocket); 하면 여기서 종료됩니다.
			return;
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

		// 동적할당한 소켓 정보를 저장
		EnterCriticalSection(&csClients);
		printf_s("[[INFO] <IocpServerBase::StartServer()> Clients.size(): %d\n", (int)Clients.size());
		Clients[clientSocket] = SocketInfo;
		printf_s("[[INFO] <IocpServerBase::StartServer()> Clients.size(): %d\n", (int)Clients.size());
		LeaveCriticalSection(&csClients);

		// 동적할당한 소켓에 대한 recvQueue를 동적할당하여 저장
		queue<char*>* recvQueue = new queue<char*>();
		EnterCriticalSection(&csMapOfRecvQueue);
		if (MapOfRecvQueue.find(clientSocket) == MapOfRecvQueue.end())
		{
			MapOfRecvQueue.insert(pair<SOCKET, queue<char*>*>(clientSocket, recvQueue));
		}
		LeaveCriticalSection(&csMapOfRecvQueue);

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
				return;
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

void IocpServerBase::CloseSocket(stSOCKETINFO* pSocketInfo)
{
	//
}

void IocpServerBase::Send(stringstream& SendStream, stSOCKETINFO* pSocketInfo)
{
	//
}

void IocpServerBase::Recv(stSOCKETINFO* pSocketInfo)
{
	// DWORD sendBytes;
	DWORD dwFlags = 0;

	// stSOCKETINFO 데이터 초기화
	ZeroMemory(&(pSocketInfo->overlapped), sizeof(OVERLAPPED));
	ZeroMemory(pSocketInfo->messageBuffer, MAX_BUFFER);
	pSocketInfo->dataBuf.len = MAX_BUFFER;
	pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
	pSocketInfo->recvBytes = 0;
	pSocketInfo->sendBytes = 0;
	pSocketInfo->sentBytes = 0;

	// 클라이언트로부터 다시 응답을 받기 위해 WSARecv 를 호출해줌
	int nResult = WSARecv(
		pSocketInfo->socket,
		&(pSocketInfo->dataBuf),
		1,
		(LPDWORD)& (pSocketInfo->recvBytes),
		&dwFlags,
		(LPWSAOVERLAPPED)& (pSocketInfo->overlapped),
		NULL
	);

	if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("[ERROR] WSARecv 실패 : %d\n", WSAGetLastError());
		}
		else
		{
			printf_s("[INFO] <IocpServerBase::Recv(...)> WSARecv: WSA_IO_PENDING \n");
		}
	}
}

