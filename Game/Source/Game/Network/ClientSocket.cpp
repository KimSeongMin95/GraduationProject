
#include "ClientSocket.h"

#include "Runtime/Core/Public/GenericPlatform/GenericPlatformAffinity.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"


/////////////////////////////////////
// FRunnable override 함수
/////////////////////////////////////
bool cClientSocket::Init()
{
	return true;
}

uint32 cClientSocket::Run()
{
	//// 초기 init 과정을 기다림
	//FPlatformProcess::Sleep(0.03);


	// 수신 버퍼 스트림
	char recvBuffer[MAX_BUFFER + 1];

	// recv while loop 시작
	// StopTaskCounter 클래스 변수를 사용해 Thread Safety하게 해줌
	while (StopTaskCounter.GetValue() == 0)
	{
		// Alertable Wait 상태를 만들기 위해
		SleepEx(1, true);

		// 수신 바이트 길이
		int nRecvLen = 0;

		/*
		FIONREAD
		네트웍 입력 버퍼에서 기다리고 있는, 소켓 s로부터 읽을 수 있는 데이터의 크기(amount)를 얻어내는데 사용됩니다. 
		argp 매개변수는 데이터의 크기를 의미하는 unsigned long 형태로 포인트 합니다. 
		다시 말하자면, 만약 s 매개변수가 연결지향형(stream oriented) 소켓(예:SOCK_STREAM) 일 경우, 
		FIONREAD 컴맨드에 의한 ioctlsocket 함수의 호출은 recv 함수의 호출로 읽을 수 있는
		데이터의 크기(amount)를 반환하게 되는거죠. 만약 소켓이 메시지 지향형(message oriented) 소켓(예:SOCK_DGRAM) 일 경우 
		FIONREAD 컴맨드는 소켓에 큐된 첫 번째 데이터그램의 크기를 반환 합니다.
		*/
		u_long amount = 0;
		if (ioctlsocket(ServerSocket, FIONREAD, &amount) == -1)
		{
			printf_s("[ERROR] if (ioctlsocket(ServerSocket, FIONREAD, &amount) == -1) \n");
			continue;
		}

		// recv 버퍼에 데이터가 4바이트 미만으로 쌓여있는 상황이면 recv 하지 않습니다.
		if (amount < 4)
		{
			// recv 버퍼에 데이터가 0바이트 존재하면 아직 아무 패킷도 받지 않은것이므로 회문합니다.
			if (amount == 0)
				continue;

			printf_s("\n\n\n\n\n\n\n\n\n\n [ERROR] amount: %d \n\n\n\n\n\n\n\n\n\n\n", (int)amount);
			continue;
		}

		// 수신
		nRecvLen = recv(ServerSocket, (CHAR*)&recvBuffer, MAX_BUFFER, 0);
		recvBuffer[nRecvLen] = '\0';
		

		/////////////////////////////////////////////
		//// (임시) 패킷 하나만 잘림 없이 전송되는 경우 바로 실행 (잘려오는 경우 여기서 에러가 발생할 수 있기 때문에 임시로 한 것이므로 조심!)
		/////////////////////////////////////////////
		//if (ProcessDirectly(recvBuffer, nRecvLen) == true)
		//{
		//	continue;
		//}


		///////////////////////////////////////////
		// recvQueue에 수신한 데이터를 적재
		///////////////////////////////////////////
		PushRecvBufferInQueue(recvBuffer, nRecvLen);

		/**************************************************************************/

		char dataBuffer[MAX_BUFFER + 1];
		dataBuffer[MAX_BUFFER] = '\0';

		///////////////////////////////////////////
		// 수신한 데이터를 저장하는 큐에서 데이터를 획득
		///////////////////////////////////////////
		GetDataInRecvQueue(dataBuffer);

		// 버퍼 길이가 4미만이면
		if (strlen(dataBuffer) < 4)
		{
			printf_s("\t if (strlen(dataBuffer) < 4): %d \n", (int)strlen(dataBuffer));

			// dataBuffer의 남은 데이터를 newBuffer에 복사합니다.
			char* newBuffer = new char[MAX_BUFFER + 1];
			CopyMemory(newBuffer, &dataBuffer, strlen(dataBuffer));
			newBuffer[strlen(dataBuffer)] = '\0';

			// 다시 큐에 데이터를 집어넣고
			RecvQueue.push(newBuffer);
		}
		// 버퍼 길이가 4이상 MAX_BUFFER + 1 미만이면
		else if (strlen(dataBuffer) < MAX_BUFFER + 1)
		{
			printf_s("\t else if (strlen(dataBuffer) < MAX_BUFFER + 1): %d \n", (int)strlen(dataBuffer));

			int idxOfStartInPacket = 0;
			int lenOfDataBuffer = (int)strlen(dataBuffer);

			while (idxOfStartInPacket < lenOfDataBuffer)
			{
				printf_s("\t idxOfStartInPacket: %d \n", idxOfStartInPacket);
				printf_s("\t lenOfDataBuffer: %d \n", lenOfDataBuffer);

				// 버퍼 길이가 4이하면 아직 패킷이 전부 수신되지 않은것이므로
				if ((lenOfDataBuffer - idxOfStartInPacket) < 4)
				{
					printf_s("\t if (lenOfDataBuffer - idxOfStartInPacket < 4): %d \n", lenOfDataBuffer - idxOfStartInPacket);

					// dataBuffer의 남은 데이터를 remainingBuffer에 복사합니다.
					char* newBuffer = new char[MAX_BUFFER + 1];
					CopyMemory(newBuffer, &dataBuffer[idxOfStartInPacket], strlen(&dataBuffer[idxOfStartInPacket]));
					newBuffer[strlen(&dataBuffer[idxOfStartInPacket])] = '\0';

					// 다시 큐에 데이터를 집어넣고
					RecvQueue.push(newBuffer);

					// 반복문을 종료합니다.
					break;
				}

				char sizeBuffer[5]; // [1234\0]
				CopyMemory(sizeBuffer, &dataBuffer[idxOfStartInPacket], 4); // 앞 4자리 데이터만 sizeBuffer에 복사합니다.
				sizeBuffer[4] = '\0';

				stringstream sizeStream;
				sizeStream << sizeBuffer;
				int sizeOfPacket = 0;
				sizeStream >> sizeOfPacket;

				printf_s("\t sizeOfPacket: %d \n", sizeOfPacket);
				printf_s("\t strlen(&dataBuffer[idxOfStartInPacket]): %d \n", (int)strlen(&dataBuffer[idxOfStartInPacket]));

				// 필요한 데이터 사이즈가 버퍼에 남은 데이터 사이즈보다 크면 아직 패킷이 전부 수신되지 않은것이므로
				if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket]))
				{
					printf_s("\t if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket])) \n");

					// dataBuffer의 남은 데이터를 remainingBuffer에 복사합니다.
					char* newBuffer = new char[MAX_BUFFER + 1];
					CopyMemory(newBuffer, &dataBuffer[idxOfStartInPacket], strlen(&dataBuffer[idxOfStartInPacket]));
					newBuffer[strlen(&dataBuffer[idxOfStartInPacket])] = '\0';

					// 다시 큐에 데이터를 집어넣고
					RecvQueue.push(newBuffer);

					// 반복문을 종료합니다.
					break;;
				}

				// 패킷을 자르면서 임시 버퍼에 복사합니다.
				char cutBuffer[MAX_BUFFER + 1];
				CopyMemory(cutBuffer, &dataBuffer[idxOfStartInPacket], sizeOfPacket);
				cutBuffer[idxOfStartInPacket + sizeOfPacket] = '\0';

				///////////////////////////////////////////
				// 패킷을 처리합니다.
				///////////////////////////////////////////
				ProcessReceivedPacket(cutBuffer);

				idxOfStartInPacket += sizeOfPacket;
			}
		}

		// 게임클라이언트를 종료하면 남아있던 WSASend(...)를 다 보내기 위해 Alertable Wait 상태로 만듭니다.
		SleepEx(1, true);
	}

	return 0;
}

void cClientSocket::Stop()
{
	// thread safety 변수를 조작해 while loop 가 돌지 못하게 함
	StopTaskCounter.Increment();
}

void cClientSocket::Exit()
{

}


/////////////////////////////////////
// cClientSocket
/////////////////////////////////////
cClientSocket::cClientSocket()
{
	printf_s("[START] <cClientSocket::cClientSocket()>\n");


	ServerSocket = NULL;

	Thread = nullptr;
	StopTaskCounter.Reset();

	bIsInitialized = false;
	bIsConnected = false;

	InitializeCriticalSection(&csMyInfo);
	EnterCriticalSection(&csMyInfo);
	MyInfo = cInfoOfPlayer();
	LeaveCriticalSection(&csMyInfo);

	InitializeCriticalSection(&csMyInfoOfGame);
	EnterCriticalSection(&csMyInfoOfGame);
	MyInfoOfGame = cInfoOfGame();
	LeaveCriticalSection(&csMyInfoOfGame);


	printf_s("[END] <cClientSocket::cClientSocket()>\n");
}

cClientSocket::~cClientSocket()
{
	CloseSocket();

	DeleteCriticalSection(&csMyInfo);
	DeleteCriticalSection(&csMyInfoOfGame);
}

bool cClientSocket::InitSocket()
{
	/// 안정성을 보장하기 위하여, 작동중인 소켓을 닫아줍니다.
	CloseSocket();

	if (bIsInitialized == true)
	{
		printf_s("[INFO] <cClientSocket::InitSocket()> if (bIsInitialized == true)\n");
		return true;
	}

	printf_s("\n\n/********** cClientSocket **********/\n");
	printf_s("[INFO] <cClientSocket::InitSocket()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocket::InitSocket()>"));

	WSADATA wsaData;

	// 윈속 버전을 2.2로 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
	{
		printf_s("[ERROR] <cClientSocket::InitSocket()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <cClientSocket::InitSocket()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)"));
		return false;
	}

	// TCP 소켓 생성	
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET) 
	{
		WSACleanup();

		printf_s("[ERROR] <cClientSocket::InitSocket()> if (ServerSocket == INVALID_SOCKET)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <cClientSocket::InitSocket()> if (ServerSocket == INVALID_SOCKET)"));
		return false;
	}

	SetSockOpt(ServerSocket, 1048576, 1048576);

	bIsInitialized = true;

	return true;
}

bool cClientSocket::Connect(const char * pszIP, int nPort)
{
	if (bIsInitialized == false)
	{
		printf_s("[INFO] <cClientSocket::Connect(...)> if (bIsInitialized == false)\n");
		return false;
	}

	if (bIsConnected == true)
	{
		printf_s("[INFO] <cClientSocket::Connect(...)> if (bIsConnected == true)\n");
		return true;
	}

	printf_s("[INFO] <cClientSocket::Connect(...)>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocket::Connect(...)>"));

	// 접속할 서버 정보를 저장할 구조체
	SOCKADDR_IN stServerAddr;

	stServerAddr.sin_family = AF_INET;
	// 접속할 서버 포트 및 IP
	stServerAddr.sin_port = htons(nPort);
	//stServerAddr.sin_addr.s_addr = inet_addr(pszIP); // Waring: C4996 심각도	코드	설명	프로젝트	파일	줄	비표시 오류(Suppression) 상태, 경고 C4996	'inet_addr': Use inet_pton() or InetPton() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings	Game
	inet_pton(AF_INET, pszIP, &stServerAddr.sin_addr.s_addr);

	if (connect(ServerSocket, (sockaddr*)&stServerAddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		printf_s("[ERROR] <cClientSocket::Connect(...)> if (connect(...) == SOCKET_ERROR)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <cClientSocket::Connect(...)> if (connect(...) == SOCKET_ERROR)"));
		return false;
	}

	bIsConnected = true;

	return true;
}

void cClientSocket::CloseSocket()
{
	printf_s("[START] <cClientSocket::CloseSocket()>\n");


	// 게임클라이언트를 종료하면 남아있던 WSASend(...)를 다 보내기 위해 Alertable Wait 상태로 만듭니다.
	SleepEx(1, true);

	if (bIsInitialized == false)
	{
		printf_s("[END] <cClientSocket::CloseSocket()> if (bIsInitialized == false)\n");
		return;
	}
	bIsInitialized = false;

	if (ServerSocket != NULL && ServerSocket != INVALID_SOCKET)
	{
		closesocket(ServerSocket);
		ServerSocket = NULL;

		printf_s("\t closesocket(ServerSocket);\n");
	}

	WSACleanup();

	if (bIsConnected == false)
	{
		printf_s("[END] <cClientSocket::CloseSocket()> if (bIsConnected == false)\n");
		return;
	}
	bIsConnected = false;


	////////////////////
	// 먼저 스레드부터 종료
	////////////////////
	StopListen();


	////////////////////
	// RecvQueue 초기화
	////////////////////
	while (RecvQueue.empty() == false)
	{
		if (RecvQueue.front())
		{
			delete[] RecvQueue.front();
			RecvQueue.front() = nullptr;
		}
		RecvQueue.pop();
	}


	////////////////////
	// 멤버변수들 초기화
	////////////////////
	InitMyInfo();
	InitMyInfoOfGame();

	tsqFindGames.clear();
	tsqWaitingGame.clear();
	tsqDestroyWaitingGame.clear();
	tsqModifyWaitingGame.clear();
	tsqStartWaitingGame.clear();
	tsqRequestInfoOfGameServer.clear();


	printf_s("[END] <cClientSocket::CloseSocket()>\n");
}

void CALLBACK SendCompletionRoutineBycClientSocket(
	IN DWORD dwError,
	IN DWORD cbTransferred,
	IN LPWSAOVERLAPPED lpOverlapped,
	IN DWORD dwFlags)
{
	printf_s("[START] <cClientSocket::CompletionROUTINE(...)> \n");

	printf_s("\t cbTransferred: %d \n", (int)cbTransferred);

	stSOCKETINFO* socketInfo = (stSOCKETINFO*)lpOverlapped;
	if (socketInfo)
	{
		delete socketInfo;
		printf_s("\t delete socketInfo; \n");
	}

	if (dwError != 0)
	{
		printf_s("[ERROR] <cClientSocket::CompletionROUTINE(...)> Fail to WSASend(...) : %d\n", WSAGetLastError());
	}
	printf_s("[INFO] <cClientSocket::CompletionROUTINE(...)> Success to WSASend(...)\n");


	printf_s("[END] <cClientSocket::CompletionROUTINE(...)> \n");
}

void cClientSocket::Send(stringstream& SendStream)
{
	// 참고: https://driftmind.tistory.com/50
	//WSAWaitForMultipleEvents(1, &event, TRUE, WSA_INFINITE, FALSE); // IO가 완료되면 event가 시그널 상태가 됩니다.
	//WSAGetOverlappedResult(hSocket, &overlapped, (LPDWORD)&sendBytes, FALSE, NULL);

	printf_s("[START] <cClientSocket::Send(...)>\n");


	DWORD	dwFlags = 0;

	stringstream finalStream;
	AddSizeInStream(SendStream, finalStream);

	stSOCKETINFO* socketInfo = new stSOCKETINFO();
	
	memset(&(socketInfo->overlapped), 0, sizeof(OVERLAPPED));
	socketInfo->overlapped.hEvent = &socketInfo; // CompletionRoutine 기반이므로 overlapped.hEvent를 활용
	CopyMemory(socketInfo->messageBuffer, (CHAR*)finalStream.str().c_str(), finalStream.str().length());
	socketInfo->messageBuffer[finalStream.str().length()] = '\0';
	socketInfo->dataBuf.len = finalStream.str().length();
	socketInfo->dataBuf.buf = socketInfo->messageBuffer;
	socketInfo->socket = NULL;
	socketInfo->recvBytes = 0;
	socketInfo->sendBytes = socketInfo->dataBuf.len;
	socketInfo->sentBytes = 0;

	printf_s("[INFO] <cClientSocket::Send(...)> socketInfo->sendBytes: %d \n", socketInfo->sendBytes);

	int nResult = WSASend(
		ServerSocket, // s: 연결 소켓을 가리키는 소켓 지정 번호
		&(socketInfo->dataBuf), // lpBuffers: WSABUF(:4300)구조체 배열의 포인터로 각각의 WSABUF 구조체는 버퍼와 버퍼의 크기를 가리킨다.
		1, // dwBufferCount: lpBuffers에 있는 WSABUF(:4300)구조체의 개수
		(LPDWORD)& (socketInfo->sentBytes), // lpNumberOfBytesSent: 함수의 호출로 전송된 데이터의 바이트 크기를 넘겨준다. 만약 매개 변수 lpOverlapped가 NULL이 아니라면, 이 매개 변수의 값은 NULL로 해야 한다. 그래야 (잠재적인)잘못된 반환을 피할 수 있다.
		dwFlags,// dwFlags: WSASend 함수를 어떤 방식으로 호출 할것인지를 지정한다.
		&(socketInfo->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)구조체의 포인터다. 비 (overlapped)중첩 소켓에서는 무시된다.
		SendCompletionRoutineBycClientSocket // lpCompletionRoutine: 데이터 전송이 완료 되었을 때 호출할 완료 루틴 (completion routine)의 포인터. 비 중첩 소켓에서는 무시 된다.
	);

	if (nResult == 0)
	{
		printf_s("[INFO] <cClientSocket::Send(...)> Success to WSASend(...) \n");
	}
	if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("[ERROR] <cClientSocket::Send(...)> Fail to WSASend(...) : %d \n", WSAGetLastError());

			delete socketInfo;
			socketInfo = nullptr;
			printf_s("[ERROR] <cClientSocket::Send(...)> delete socketInfo; \n");
		}
		else
		{
			printf_s("[INFO] <cClientSocket::Send(...)> WSASend: WSA_IO_PENDING \n");
		}
	}

	//send(ServerSocket, (CHAR*)finalStream.str().c_str(), finalStream.str().length(), 0);


	printf_s("[END] <cClientSocket::Send(...)>\n");
}


void cClientSocket::ProcessReceivedPacket(char* DataBuffer)
{
	if (!DataBuffer)
	{
		printf_s("[ERROR] <cClientSocket::ProcessReceivedPacket(...)> if (!DataBuffer) \n");
		return;
	}

	stringstream recvStream;
	recvStream << DataBuffer;

	// 사이즈 확인
	int sizeOfRecvStream = 0;
	recvStream >> sizeOfRecvStream;
	printf_s("\t sizeOfRecvStream: %d \n", sizeOfRecvStream);

	// 패킷 종류 확인
	int packetType = -1; 
	recvStream >> packetType;
	printf_s("\t packetType: %d \n", packetType);

	/// 오류 확인
	if (sizeOfRecvStream == 0)
	{
		printf_s("[ERROR] <cClientSocket::ProcessReceivedPacket()> recvBuffer: %s \n", DataBuffer);
		return;
	}

	switch (packetType)
	{
	case EPacketType::LOGIN:
	{
		RecvLogin(recvStream);
	}
	break;
	case EPacketType::FIND_GAMES:
	{
		RecvFindGames(recvStream);
	}
	break;
	case EPacketType::WAITING_GAME:
	{
		RecvWaitingGame(recvStream);
	}
	break;
	case EPacketType::DESTROY_WAITING_GAME:
	{
		RecvDestroyWaitingGame(recvStream);
	}
	break;
	case EPacketType::MODIFY_WAITING_GAME:
	{
		RecvModifyWaitingGame(recvStream);
	}
	break;
	case EPacketType::START_WAITING_GAME:
	{
		RecvStartWaitingGame(recvStream);
	}
	break;
	case EPacketType::REQUEST_INFO_OF_GAME_SERVER:
	{
		RecvRequestInfoOfGameServer(recvStream);
	}
	break;

	default:
	{
		printf_s("[ERROR] <cClientSocket::ProcessReceivedPacket()> unknown packet type! PacketType: %d \n", packetType);
		printf_s("[ERROR] <cClientSocket::ProcessReceivedPacket()> recvBuffer: %s \n", DataBuffer);
	}
	break;
	}

}

bool cClientSocket::ProcessDirectly(char* RecvBuffer, int RecvLen)
{
	if (!RecvBuffer)
	{
		printf_s("[ERROR] <cClientSocket::ProcessDirectly(...)> if (!RecvBuffer) \n");
		return false;
	}

	if (RecvLen >= 4) // 한번 더 확인
	{
		char sizeBuffer[5]; // [1234\0]
		CopyMemory(sizeBuffer, RecvBuffer, 4); // 앞 4자리 데이터만 sizeBuffer에 복사합니다.
		sizeBuffer[4] = '\0';

		stringstream sizeStream;
		sizeStream << sizeBuffer;
		int sizeOfPacket = -1;
		sizeStream >> sizeOfPacket;

		if (sizeOfPacket != -1 && sizeOfPacket == RecvLen)
		{
			printf_s("[INFO] <cClientSocket::Run()> if (sizeOfPacket != -1 && sizeOfPacket == nRecvLen) \n");
			printf_s("[INFO] <cClientSocket::Run()> sizeOfPacket: %d, nRecvLen:%d \n", sizeOfPacket, RecvLen);
			ProcessReceivedPacket(RecvBuffer);
			
			return true;
		}
	}

	return false;
}

void cClientSocket::PushRecvBufferInQueue(char* RecvBuffer, int RecvLen)
{
	if (!RecvBuffer)
	{
		printf_s("[ERROR] <cClientSocket::PushRecvBufferInQueue(...)> if (!RecvBuffer) \n");
		return;
	}

	// 데이터가 MAX_BUFFER 그대로 4096개 꽉 채워서 오는 경우가 있기 때문에, 대비하기 위하여 +1로 '\0' 공간을 만들어줍니다.
	char* newBuffer = new char[MAX_BUFFER + 1];
	//ZeroMemory(newBuffer, MAX_BUFFER);
	CopyMemory(newBuffer, RecvBuffer, RecvLen);
	newBuffer[RecvLen] = '\0';

	RecvQueue.push(newBuffer);
}

void cClientSocket::GetDataInRecvQueue(char* DataBuffer)
{
	if (!DataBuffer)
	{
		printf_s("[ERROR] <cClientSocket::GetDataInRecvQueue(...)> if (!DataBuffer) \n");
		return;
	}

	int idxOfStartInQueue = 0;
	int idxOfStartInNextQueue = 0;

	// 큐가 빌 때까지 진행 (buffer가 다 차면 반복문을 빠져나옵니다.)
	while (RecvQueue.empty() == false)
	{
		// dataBuffer를 채우려고 하는 사이즈가 최대로 MAX_BUFFER면 CopyMemory 가능.
		if ((idxOfStartInQueue + strlen(RecvQueue.front())) < MAX_BUFFER + 1)
		{
			CopyMemory(&DataBuffer[idxOfStartInQueue], RecvQueue.front(), strlen(RecvQueue.front()));
			idxOfStartInQueue += (int)strlen(RecvQueue.front());
			DataBuffer[idxOfStartInQueue] = '\0';

			delete[] RecvQueue.front();
			RecvQueue.front() = nullptr;
			RecvQueue.pop();
		}
		else
		{
			// 버퍼에 남은 자리 만큼 꽉 채웁니다.
			idxOfStartInNextQueue = MAX_BUFFER - idxOfStartInQueue;
			CopyMemory(&DataBuffer[idxOfStartInQueue], RecvQueue.front(), idxOfStartInNextQueue);
			DataBuffer[MAX_BUFFER] = '\0';


			// dateBuffer에 복사하고 남은 데이터들을 임시 버퍼에 복사합니다. 
			int lenOfRestInNextQueue = (int)strlen(&RecvQueue.front()[idxOfStartInNextQueue]);
			char tempBuffer[MAX_BUFFER + 1];
			CopyMemory(tempBuffer, &RecvQueue.front()[idxOfStartInNextQueue], lenOfRestInNextQueue);
			tempBuffer[lenOfRestInNextQueue] = '\0';

			// 임시 버퍼에 있는 데이터들을 다시 RecvQueue.front()에 복사합니다.
			CopyMemory(RecvQueue.front(), tempBuffer, strlen(tempBuffer));
			RecvQueue.front()[strlen(tempBuffer)] = '\0';

			break;
		}
	}
}

bool cClientSocket::StartListen()
{
	printf_s("[INFO] <cClientSocket::StartListen()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocket::StartListen()>"));

	if (Thread)
		return true;

	// 스레드 시작
	Thread = FRunnableThread::Create(this, TEXT("cClientSocket"), 0, TPri_BelowNormal);

	return (Thread != nullptr);
}

void cClientSocket::StopListen()
{
	printf_s("[START] <cClientSocket::StopListen()>\n");

	// 스레드 종료
	Stop();

	if (Thread)
	{
		Thread->WaitForCompletion();
		Thread->Kill();
		delete Thread;
		Thread = nullptr;

		printf_s("\t Thread->WaitForCompletion(); Thread->Kill(); delete Thread;\n");
	}
	StopTaskCounter.Reset();

	printf_s("[END] <cClientSocket::StopListen()>\n");
}


///////////////////////////////////////////
// Basic Functions
///////////////////////////////////////////
void cClientSocket::AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
{
	if (DataStream.str().length() == 0)
	{
		printf_s("[ERROR] <cClientSocket::AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");
		return;
	}
	//printf_s("[START] <cClientSocket::AddSizeInStream(...)> \n");

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

	printf_s("\t FinalStream size: %d\n", (int)FinalStream.str().length());
	//printf_s("\t FinalStream: %s\n", FinalStream.str().c_str());


	//printf_s("[END] <cClientSocket::AddSizeInStream(...)> \n");
}

void cClientSocket::SetSockOpt(SOCKET& Socket, int SendBuf, int RecvBuf)
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

	printf_s("[START] <cClientSocket::SetSockOpt(...)> \n");


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


	printf_s("[END] <cClientSocket::SetSockOpt(...)> \n");
}


/////////////////////////////////////
// Main Server / Main Clients
/////////////////////////////////////
void cClientSocket::SendLogin(const FText ID)
{
	printf_s("[Start] <cClientSocket::SendLogin(...)>\n");


	cInfoOfPlayer infoOfPlayer;

	// ID가 비어있지 않으면 대입
	if (!ID.IsEmpty())
		infoOfPlayer.ID = TCHAR_TO_UTF8(*ID.ToString());

	stringstream sendStream;
	sendStream << EPacketType::LOGIN << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream);

	infoOfPlayer.PrintInfo();


	printf_s("[End] <cClientSocket::SendLogin(...)>\n");
}
void cClientSocket::RecvLogin(stringstream& RecvStream)
{
	printf_s("[Start] <cClientSocket::RecvLogin(...)>\n");


	cInfoOfPlayer infoOfPlayer;

	RecvStream >> infoOfPlayer;

	SetMyInfo(infoOfPlayer);

	infoOfPlayer.PrintInfo();


	printf_s("[End] <cClientSocket::RecvLogin(...)>\n");
}

void cClientSocket::SendCreateGame()
{
	printf_s("[Start] <cClientSocket::SendCreateGame()>\n");


	cInfoOfGame infoOfGame;

	infoOfGame.Leader = CopyMyInfo();

	SetMyInfoOfGame(infoOfGame);

	stringstream sendStream;
	sendStream << EPacketType::CREATE_GAME << endl;
	sendStream << infoOfGame << endl;

	Send(sendStream);

	infoOfGame.PrintInfo();


	printf_s("[End] <cClientSocket::SendCreateGame()>\n");
}

void cClientSocket::SendFindGames()
{
	printf_s("[Start] <cClientSocket::SendFindGames()>\n");


	stringstream sendStream;
	sendStream << EPacketType::FIND_GAMES << endl;

	Send(sendStream);


	printf_s("[End] <cClientSocket::SendFindGames()>\n");
}
void cClientSocket::RecvFindGames(stringstream& RecvStream)
{
	printf_s("[Start] <cClientSocket::RecvFindGames(...)>\n");


	cInfoOfGame infoOfGame;

	while (RecvStream >> infoOfGame)
	{
		tsqFindGames.push(infoOfGame);
		infoOfGame.PrintInfo();
	}


	printf_s("[End] <cClientSocket::RecvFindGames(...)>\n");
}
     
void cClientSocket::SendJoinOnlineGame(int SocketIDOfLeader)
{
	printf_s("[Start] <cClientSocket::SendJoinWaitingGame(...)>\n");
	printf_s("\t SocketIDOfLeader: %d\n", SocketIDOfLeader);


	cInfoOfPlayer infoOfPlayer = CopyMyInfo();
	infoOfPlayer.LeaderSocketByMainServer = SocketIDOfLeader;
	SetMyInfo(infoOfPlayer);

	stringstream sendStream;
	sendStream << EPacketType::JOIN_ONLINE_GAME << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream);


	printf_s("[End] <cClientSocket::SendJoinWaitingGame(...)>\n");
}

void cClientSocket::RecvWaitingGame(stringstream& RecvStream)
{
	printf_s("[Start] <cClientSocket::RecvJoinWaitingGame(...)>\n");


	cInfoOfGame infoOfGame;

	RecvStream >> infoOfGame;

	SetMyInfoOfGame(infoOfGame);
	
	tsqWaitingGame.push(infoOfGame);

	infoOfGame.PrintInfo();


	printf_s("[End] <cClientSocket::RecvJoinWaitingGame(...)>\n");
}

void cClientSocket::SendDestroyWaitingGame()
{
	printf_s("[Start] <cClientSocket::SendDestroyWaitingGame()>\n");


	// MyInfo의 특정 멤버변수들 초기화
	cInfoOfPlayer infoOfPlayer = CopyMyInfo();
	infoOfPlayer.SocketByGameServer = 0;
	infoOfPlayer.PortOfGameServer = 0;
	infoOfPlayer.PortOfGameClient = 0;
	infoOfPlayer.LeaderSocketByMainServer = 0;
	SetMyInfo(infoOfPlayer);

	InitMyInfoOfGame();

	stringstream sendStream;
	sendStream << EPacketType::DESTROY_WAITING_GAME << endl;

	Send(sendStream);


	printf_s("[End] <cClientSocket::SendDestroyWaitingGame()>\n");
}
void cClientSocket::RecvDestroyWaitingGame(stringstream& RecvStream)
{
	printf_s("[Start] <cClientSocket::RecvDestroyWaitingGame(...)>\n");


	// MyInfo의 특정 멤버변수들 초기화
	cInfoOfPlayer infoOfPlayer = CopyMyInfo();
	infoOfPlayer.SocketByGameServer = 0;
	infoOfPlayer.PortOfGameServer = 0;
	infoOfPlayer.PortOfGameClient = 0;
	infoOfPlayer.LeaderSocketByMainServer = 0;
	SetMyInfo(infoOfPlayer);

	InitMyInfoOfGame();

	tsqDestroyWaitingGame.push(true);


	printf_s("[End] <cClientSocket::RecvDestroyWaitingGame(...)>\n");
}

void cClientSocket::SendExitWaitingGame()
{
	printf_s("[Start] <cClientSocket::SendExitWaitingGame(...)\n");


	// MyInfo의 특정 멤버변수들 초기화
	cInfoOfPlayer infoOfPlayer = CopyMyInfo();
	infoOfPlayer.SocketByGameServer = 0;
	infoOfPlayer.PortOfGameServer = 0;
	infoOfPlayer.PortOfGameClient = 0;
	infoOfPlayer.LeaderSocketByMainServer = 0;
	SetMyInfo(infoOfPlayer);

	InitMyInfoOfGame();

	stringstream sendStream;
	sendStream << EPacketType::EXIT_WAITING_GAME << endl;

	Send(sendStream);


	printf_s("[End] <cClientSocket::SendExitWaitingGame(...)>\n");
}


void cClientSocket::SendModifyWaitingGame()
{
	printf_s("[Start] <cClientSocket::SendModifyWaitingGame()>\n");


	cInfoOfGame infoOfGame = CopyMyInfoOfGame();
	
	stringstream sendStream;
	sendStream << EPacketType::MODIFY_WAITING_GAME << endl;
	sendStream << infoOfGame << endl;

	Send(sendStream);

	infoOfGame.PrintInfo();


	printf_s("[End] <cClientSocket::SendModifyWaitingGame()>\n");
}
void cClientSocket::RecvModifyWaitingGame(stringstream& RecvStream)
{
	printf_s("[Start] <cClientSocket::RecvModifyWaitingGame(...)>\n");


	cInfoOfGame infoOfGame;

	RecvStream >> infoOfGame;

	SetMyInfoOfGame(infoOfGame);

	tsqModifyWaitingGame.push(infoOfGame);

	infoOfGame.PrintInfo();


	printf_s("[End] <cClientSocket::RecvModifyWaitingGame(...)>\n");
}

void cClientSocket::SendStartWaitingGame()
{
	printf_s("[Start] <cClientSocket::SendStartWaitingGame()>\n");


	stringstream sendStream;
	sendStream << EPacketType::START_WAITING_GAME << endl;

	Send(sendStream);


	printf_s("[End] <cClientSocket::SendStartWaitingGame()>\n");
}
void cClientSocket::RecvStartWaitingGame(stringstream& RecvStream)
{
	printf_s("[Start] <cClientSocket::RecvStartWaitingGame(...)>\n");
	

	tsqStartWaitingGame.push(true);


	printf_s("[End] <cClientSocket::RecvStartWaitingGame(...)>\n");
}

void cClientSocket::SendActivateGameServer(int PortOfGameServer)
{
	printf_s("[Start] <cClientSocket::SendActivateGameServer(...)\n");


	cInfoOfPlayer infoOfLeader = CopyMyInfo();
	infoOfLeader.PortOfGameServer = PortOfGameServer;
	SetMyInfo(infoOfLeader);

	cInfoOfGame infoOfGame = CopyMyInfoOfGame();
	infoOfGame.State = string("Playing");
	infoOfGame.Leader = infoOfLeader;
	SetMyInfoOfGame(infoOfGame);

	stringstream sendStream;
	sendStream << EPacketType::ACTIVATE_GAME_SERVER << endl;
	sendStream << infoOfLeader << endl;

	Send(sendStream);
	
	infoOfLeader.PrintInfo();


	printf_s("[End] <cClientSocket::SendActivateGameServer(...)>\n");
}

void cClientSocket::SendRequestInfoOfGameServer()
{
	printf_s("[Start] <cClientSocket::SendRequestInfoOfGameServer()>\n");


	stringstream sendStream;
	sendStream << EPacketType::REQUEST_INFO_OF_GAME_SERVER << endl;

	Send(sendStream);


	printf_s("[End] <cClientSocket::SendRequestInfoOfGameServer()>\n");
}
void cClientSocket::RecvRequestInfoOfGameServer(stringstream& RecvStream)
{
	printf_s("[Start] <cClientSocket::RecvRequestInfoOfGameServer(...)>\n");


	cInfoOfPlayer infoOfLeader;

	RecvStream >> infoOfLeader;

	cInfoOfGame infoOfGame = CopyMyInfoOfGame();
	infoOfGame.State = string("Playing");
	infoOfGame.Leader = infoOfLeader;
	SetMyInfoOfGame(infoOfGame);

	tsqRequestInfoOfGameServer.push(infoOfLeader);

	infoOfLeader.PrintInfo();


	printf_s("[End] <cClientSocket::RecvRequestInfoOfGameServer(...)>\n");
}



/////////////////////////////////////
// Set-Get
/////////////////////////////////////
void cClientSocket::SetMyInfo(cInfoOfPlayer& InfoOfPlayer)
{
	EnterCriticalSection(&csMyInfo);
	MyInfo = InfoOfPlayer;
	LeaveCriticalSection(&csMyInfo);
}
cInfoOfPlayer cClientSocket::CopyMyInfo()
{
	cInfoOfPlayer infoOfPlayer;

	EnterCriticalSection(&csMyInfo);
	infoOfPlayer = MyInfo;
	LeaveCriticalSection(&csMyInfo);

	return infoOfPlayer;
}
void cClientSocket::InitMyInfo()
{
	EnterCriticalSection(&csMyInfo);
	MyInfo = cInfoOfPlayer();
	LeaveCriticalSection(&csMyInfo);
}

void cClientSocket::SetMyInfoOfGame(cInfoOfGame& InfoOfGame)
{
	EnterCriticalSection(&csMyInfoOfGame);
	MyInfoOfGame = InfoOfGame;
	LeaveCriticalSection(&csMyInfoOfGame);
}
cInfoOfGame cClientSocket::CopyMyInfoOfGame()
{
	cInfoOfGame infoOfGame;

	EnterCriticalSection(&csMyInfoOfGame);
	infoOfGame = MyInfoOfGame;
	LeaveCriticalSection(&csMyInfoOfGame);

	return infoOfGame;
}
void cClientSocket::InitMyInfoOfGame()
{
	EnterCriticalSection(&csMyInfoOfGame);
	MyInfoOfGame = cInfoOfGame();
	LeaveCriticalSection(&csMyInfoOfGame);
}
