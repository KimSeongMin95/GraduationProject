// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerSocketInGame.h"

unsigned int WINAPI CallMainThread(LPVOID p)
{
	cServerSocketInGame* pOverlappedEvent = (cServerSocketInGame*)p;
	pOverlappedEvent->StartServer();

	return 0;
}

unsigned int WINAPI CallWorkerThread(LPVOID p)
{
	cServerSocketInGame* pOverlappedEvent = (cServerSocketInGame*)p;
	pOverlappedEvent->WorkerThread();

	return 0;
}



cServerSocketInGame::cServerSocketInGame()
{
	bIsServerOn = false;

	// 스레드 구동가능
	bAccept = true;
	bWorkerThread = true;
	nThreadCnt = 0;

	InitializeCriticalSection(&csClients);

	//// 패킷 함수 포인터에 함수 지정
	//fnProcess[EPacketType::LOGIN].funcProcessPacket = Login;
}

cServerSocketInGame::~cServerSocketInGame()
{
	// 서버 종료는 여기서 처리
	CloseServer();

	DeleteCriticalSection(&csClients);
}

bool cServerSocketInGame::Initialize()
{
	/// 안정성을 보장하기 위하여, 작동주인 서버를 닫아줍니다.
	CloseServer();

	// 콘솔을 새로 만듭니다.
	AllocMyConsole();

	printf_s("[INFO] <cServerSocketInGame::Initialize()>\n");

	WSADATA wsaData;

	// winsock 2.2 버전으로 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)\n");
		return false;
	}

	// 소켓 생성
	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (ListenSocket == INVALID_SOCKET)
	{
		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (ListenSocket == INVALID_SOCKET)\n");
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
		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (bind(...) == SOCKET_ERROR)\n");
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	// 수신 대기열 생성
	if (listen(ListenSocket, 5) == SOCKET_ERROR)
	{
		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (listen(ListenSocket, 5) == SOCKET_ERROR)\n");
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	////////////////////
	// 메인 스레드 시작
	////////////////////
	printf_s("[INFO] <cServerSocketInGame::Initialize()> Start main thread\n");
	unsigned int threadId;

	// _beginthreadex()는 ::CloseHandle을 내부에서 호출하지 않기 때문에, 스레드 종료시 사용자가 직접 CloseHandle()해줘야 합니다.
	// 스레드가 종료되면 _endthreadex()가 자동호출됩니다.
	hMainHandle = (HANDLE*)_beginthreadex(NULL, 0, &CallMainThread, this, CREATE_SUSPENDED, &threadId);
	if (hMainHandle == NULL)
	{
		printf_s("[ERROR] <cServerSocketInGame::Initialize()> if (hMainHandle == NULL)\n");
		return false;
	}
	ResumeThread(hMainHandle);

	bIsServerOn = true;

	return true;
}

void cServerSocketInGame::StartServer()
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
	{
		printf_s("[ERROR] <cServerSocketInGame::StartServer()> if (!CreateWorkerThread())\n");
		return;
	}

	printf_s("[INFO] <cServerSocketInGame::StartServer()> Server started.\n");

	// 클라이언트 접속을 받음
	while (bAccept)
	{
		//printf_s("[INFO] <cServerSocketInGame::StartServer()> before WSAAccept(...)\n");
		clientSocket = WSAAccept(ListenSocket, (struct sockaddr*)& clientAddr, &addrLen, NULL, NULL);
		//printf_s("[INFO] <cServerSocketInGame::StartServer()> after WSAAccept(...)\n");

		if (clientSocket == INVALID_SOCKET)
		{
			printf_s("[ERROR] <cServerSocketInGame::StartServer()> if (clientSocket == INVALID_SOCKET)\n");
			return;
		}
		else
		{
			printf_s("[INFO] <cServerSocketInGame::StartServer()> Success WSAAccept, SocketID: %d\n", int(clientSocket));
		}

		SocketInfo = new stSOCKETINFO();
		SocketInfo->socket = clientSocket;
		SocketInfo->recvBytes = 0;
		SocketInfo->sendBytes = 0;
		SocketInfo->dataBuf.len = MAX_BUFFER;
		SocketInfo->dataBuf.buf = SocketInfo->messageBuffer;
		flags = 0;


		EnterCriticalSection(&csClients);
		printf_s("[[INFO] <cServerSocketInGame::StartServer()> Clients.size(): %d\n", (int)Clients.size());
		Clients[clientSocket] = SocketInfo;
		printf_s("[[INFO] <cServerSocketInGame::StartServer()> Clients.size(): %d\n", (int)Clients.size());
		LeaveCriticalSection(&csClients);


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
			printf_s("[ERROR] <cServerSocketInGame::StartServer()> if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)\n");
			return;
		}
	}

}

void cServerSocketInGame::CloseServer()
{
	bIsServerOn = false;

	// 작동중인 스레드를 강제로 종료
	// 주의: 메모리누수가 발생합니다.
	TerminateThread(hMainHandle, NULL);
	for (int i = 0; i < nThreadCnt; i++)
		TerminateThread(hWorkerHandle[i], NULL);
	nThreadCnt = 0; // 주의!: 무조건 다시 0으로 초기화해줘야 handled exception이 발생하지 않습니다.

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

	closesocket(ListenSocket);
	WSACleanup();

	FreeMyConsole();
}

bool cServerSocketInGame::CreateWorkerThread()
{
	unsigned int threadCount = 0;
	unsigned int threadId;

	// 시스템 정보 가져옴
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	printf_s("[INFO] <cServerSocketInGame::CreateWorkerThread()> num of CPU: %d\n", (int)sysInfo.dwNumberOfProcessors);

	nThreadCnt = sysInfo.dwNumberOfProcessors;

	// thread handler 선언
	// 동적 배열 할당 [상수가 아니어도 됨]
	hWorkerHandle = new HANDLE[nThreadCnt];

	// thread 생성
	for (int i = 0; i < nThreadCnt; i++)
	{
		hWorkerHandle[i] = (HANDLE*)_beginthreadex(
			NULL, 0, &CallWorkerThread, this, CREATE_SUSPENDED, &threadId
		);
		if (hWorkerHandle[i] == NULL)
		{
			printf_s("[ERROR] <cServerSocketInGame::CreateWorkerThread()> if (hWorkerHandle[i] == NULL)\n");
			return false;
		}
		ResumeThread(hWorkerHandle[i]);

		threadCount++;
	}
	printf_s("[INFO] <cServerSocketInGame::CreateWorkerThread()> Start Worker %d Threads\n", threadCount);

	return true;
}

void cServerSocketInGame::WorkerThread()
{
	// 함수 호출 성공 여부
	BOOL	bResult;

	// Overlapped I/O 작업에서 전송된 데이터 크기
	DWORD	recvBytes;

	// Completion Key를 받을 포인터 변수
	stSOCKETINFO* pCompletionKey;

	// I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터	
	stSOCKETINFO* pSocketInfo = nullptr;
	DWORD	dwFlags = 0;

	while (bWorkerThread)
	{
		printf_s("[INFO] <cServerSocketInGame::WorkerThread()> before GetQueuedCompletionStatus(...)\n");
		/**
		 * 이 함수로 인해 쓰레드들은 WaitingThread Queue 에 대기상태로 들어가게 됨
		 * 완료된 Overlapped I/O 작업이 발생하면 IOCP Queue 에서 완료된 작업을 가져와 뒷처리를 함
		 */
		bResult = GetQueuedCompletionStatus(hIOCP,
			&recvBytes,						// 실제로 전송된 바이트
			(PULONG_PTR)& pCompletionKey,	// completion key
			(LPOVERLAPPED*)& pSocketInfo,	// overlapped I/O 객체
			INFINITE						// 대기할 시간
		);
		printf_s("[INFO] <cServerSocketInGame::WorkerThread()> after GetQueuedCompletionStatus(...)\n");

		// 비정상 접속 끊김은 GetQueuedCompletionStatus가 FALSE를 리턴하고 수신바이트 크기가 0입니다.
		if (!bResult && recvBytes == 0)
		{
			printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> if (!bResult && recvBytes == 0)\n");
			CloseSocket(pSocketInfo);
			continue;
		}

		if (!pSocketInfo)
		{
			printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> if (!pSocketInfo)\n");
			continue;
		}

		pSocketInfo->dataBuf.len = recvBytes;

		// 정상 접속 끊김은 GetQueuedCompletionStatus가 TRUE를 리턴하고 수신바이트 크기가 0입니다.
		if (recvBytes == 0)
		{
			printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> if (recvBytes == 0) socketID: %d\n", (int)pSocketInfo->socket);
			CloseSocket(pSocketInfo);
			continue;
		}

		// 패킷 종류
		int PacketType;

		// 클라이언트 정보 역직렬화
		stringstream RecvStream;

		// 문자열인 버퍼 값을 stringstream에 저장합니다.
		RecvStream << pSocketInfo->dataBuf.buf;

		// stringstream에서 PacketType의 자료형인 int형에 해당되는 값만 추출/복사하여 PacketType에 대입합니다.
		RecvStream >> PacketType;

		// 패킷 처리
		// 패킷 처리 함수 포인터인 FuncProcess에 바인딩한 PacketType에 맞는 함수들을 실행합니다.
		if (fnProcess[PacketType].funcProcessPacket != nullptr)
		{
			fnProcess[PacketType].funcProcessPacket(RecvStream, pSocketInfo);
		}
		else
		{
			printf_s("[ERROR] <cServerSocketInGame::WorkerThread()> undefined packet type\n");
		}

		// 클라이언트 대기
		Recv(pSocketInfo);
	}
}

void cServerSocketInGame::CloseSocket(stSOCKETINFO* pSocketInfo)
{
	if (pSocketInfo == nullptr)
	{
		printf_s("[ERROR] <cServerSocketInGame::CloseSocket(...)>if (pSocketInfo == nullptr)\n");
		return;
	}

	printf_s("[INFO] <cServerSocketInGame::CloseSocket(...)>\n");

	/// Clients에서 제거
	EnterCriticalSection(&csClients);
	printf_s("[[INFO] <cServerSocketInGame::CloseSocket(...)> Clients.size(): %d\n", (int)Clients.size());
	Clients.erase(pSocketInfo->socket);
	printf_s("[[INFO] <cServerSocketInGame::CloseSocket(...)> Clients.size(): %d\n", (int)Clients.size());	
	LeaveCriticalSection(&csClients);

	closesocket(pSocketInfo->socket);
	free(pSocketInfo);
	pSocketInfo = nullptr;
}

void cServerSocketInGame::Send(stSOCKETINFO* pSocketInfo)
{
	DWORD	sendBytes;
	DWORD	dwFlags = 0;

	int nResult = WSASend(
		pSocketInfo->socket,
		&(pSocketInfo->dataBuf),
		1,
		&sendBytes,
		dwFlags,
		NULL,
		NULL
	);

	if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		printf_s("[ERROR] <cServerSocketInGame::Send(...)> if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)\n");
	}
}

void cServerSocketInGame::Recv(stSOCKETINFO* pSocketInfo)
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
		printf_s("[ERROR] <cServerSocketInGame::Recv(...)> if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)\n");
	}
}










/*** Console for log : Start ***/
void cServerSocketInGame::AllocMyConsole()
{
	if (AllocConsole())
	{
		freopen_s(&fp_console, "CONOUT$", "w", stdout);
		printf_s("/*** Console Allocated ***/\n");
	}
}

void cServerSocketInGame::FreeMyConsole()
{
	if (fp_console)
		fclose(fp_console);
	FreeConsole();
}
/*** Console for log : End ***/