#include "IocpServerBase.h"


std::map<SOCKET, stSOCKETINFO*> IocpServerBase::Clients;
CRITICAL_SECTION IocpServerBase::csClients;

IocpServerBase::IocpServerBase()
{
	// 멤버 변수 초기화
	bWorkerThread = true;
	bAccept = true;

	InitializeCriticalSection(&csClients);
}

IocpServerBase::~IocpServerBase()
{
	// winsock 의 사용을 끝낸다
	WSACleanup();

	// 다 사용한 객체를 삭제
	if (SocketInfo)
	{
		// 배열 할당 해제
		delete[] SocketInfo;
		SocketInfo = nullptr;
	}

	if (hWorkerHandle)
	{
		// 배열 할당 해제
		delete[] hWorkerHandle;
		hWorkerHandle = nullptr;
	}

	DeleteCriticalSection(&csClients);
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
		printf_s("[ERROR] bind 실패\n");
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	// 수신 대기열 생성
	if (listen(ListenSocket, 5) == SOCKET_ERROR)
	{
		printf_s("[ERROR] listen 실패\n");
		closesocket(ListenSocket);
		WSACleanup();
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
	DWORD recvBytes;
	DWORD flags;

	// Completion Port 객체 생성
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// Worker Thread 생성
	if (!CreateWorkerThread()) 
		return;

	printf_s("[INFO] 서버 시작...\n");

	// 클라이언트 접속을 받음
	while (bAccept)
	{
		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*)& clientAddr, &addrLen, NULL, NULL);
	
		if (clientSocket == INVALID_SOCKET)
		{
			printf_s("[ERROR] WSAAccept 실패\n");
			return;
		}
		else
			printf_s("[INFO] WSAAccept 성공, SocketID: %d\n", int(clientSocket));

		SocketInfo = new stSOCKETINFO();
		SocketInfo->socket = clientSocket;
		SocketInfo->recvBytes = 0;
		SocketInfo->sendBytes = 0;
		SocketInfo->dataBuf.len = MAX_BUFFER;
		SocketInfo->dataBuf.buf = SocketInfo->messageBuffer;
		flags = 0;


		// char *inet_ntoa(struct in_addr adr); // 역으로 네트워크바이트순서로 된 정32비트 정수를 다시 문자열로 돌려주는 함수
		SocketInfo->IPv4Addr = string(inet_ntoa(clientAddr.sin_addr));
		printf_s("[INFO] <IocpServerBase::StartServer()> Client's IP: %s\n", SocketInfo->IPv4Addr.c_str());

		SocketInfo->Port = (int)ntohs(clientAddr.sin_port);
		printf_s("[INFO] <IocpServerBase::StartServer()> Client's Port: %d\n\n", SocketInfo->Port);

		EnterCriticalSection(&csClients);
		Clients[clientSocket] = SocketInfo;
		LeaveCriticalSection(&csClients);


		// 원본은 DWORD로 캐스팅
		//hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (DWORD)SocketInfo, 0);
		hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (ULONG_PTR)SocketInfo, 0);

		// 중첩 소켓을 지정하고 완료시 실행될 함수를 넘겨줌
		int nResult = WSARecv(
			SocketInfo->socket,
			&SocketInfo->dataBuf,
			1,
			&recvBytes,
			&flags,
			&(SocketInfo->overlapped),
			NULL
		);

		if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("[ERROR] IO Pending 실패 : %d\n", WSAGetLastError());
			return;
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
	if (pSocketInfo == nullptr)
	{
		printf_s("[ERROR] <IocpServerBase::CloseSocket(...)>if (pSocketInfo == nullptr)\n");
		return;
	}

	printf_s("[Start] <IocpServerBase::CloseSocket(...)>\n");

	/// Clients에서 제거
	EnterCriticalSection(&csClients);
	printf_s("\t Clients.size(): %d\n", (int)Clients.size());
	Clients.erase(pSocketInfo->socket);
	printf_s("\t Clients.size(): %d\n", (int)Clients.size());
	LeaveCriticalSection(&csClients);

	closesocket(pSocketInfo->socket);
	free(pSocketInfo);
	pSocketInfo = nullptr;

	printf_s("[End] <IocpServerBase::CloseSocket(...)>\n");
}

void IocpServerBase::Send(stSOCKETINFO* pSocketInfo)
{
	//DWORD	sendBytes;
	//DWORD	dwFlags = 0;

	//int nResult = WSASend(
	//	pSocketInfo->socket,
	//	&(pSocketInfo->dataBuf),
	//	1,
	//	&sendBytes,
	//	dwFlags,
	//	NULL,
	//	NULL
	//);

	//if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	//{
	//	printf_s("[ERROR] WSASend 실패 : %d", WSAGetLastError());
	//}
}

void IocpServerBase::Recv(stSOCKETINFO* pSocketInfo)
{
	// DWORD	sendBytes;
	DWORD	dwFlags = 0;

	// stSOCKETINFO 데이터 초기화
	ZeroMemory(&(pSocketInfo->overlapped), sizeof(OVERLAPPED));
	ZeroMemory(pSocketInfo->messageBuffer, MAX_BUFFER);
	pSocketInfo->dataBuf.len = MAX_BUFFER;
	pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
	pSocketInfo->recvBytes = 0;
	pSocketInfo->sendBytes = 0;

	// 클라이언트로부터 다시 응답을 받기 위해 WSARecv 를 호출해줌
	int nResult = WSARecv(
		pSocketInfo->socket,
		&(pSocketInfo->dataBuf),
		1,
		(LPDWORD)& pSocketInfo,
		&dwFlags,
		(LPWSAOVERLAPPED)& (pSocketInfo->overlapped),
		NULL
	);

	if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		printf_s("[ERROR] WSARecv 실패 : %d\n", WSAGetLastError());
	}
}

