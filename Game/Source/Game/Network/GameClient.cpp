﻿
#include "GameClient.h"


/*** 직접 정의한 헤더 전방 선언 : Start ***/
#include "Network/MainClient.h"

#include "Character/Pioneer.h"
/*** 직접 정의한 헤더 전방 선언 : End ***/


unsigned int WINAPI CallMainThreadIncGameClient(LPVOID p)
{
	cGameClient* pOverlappedEvent = (cGameClient*)p;
	pOverlappedEvent->RunMainThread();

	return 0;
}


/////////////////////////////////////
// cGameClient
/////////////////////////////////////
cGameClient::cGameClient()
{
	CONSOLE_LOG("[START] <cGameClient::cGameClient()>\n");


	ServerSocket = NULL;

	bAccept = true;
	hMainHandle = NULL;

	bIsInitialized = false;
	bIsConnected = false;
	bIsClientSocketOn = false;

	bServerOn = false;

	InitializeCriticalSection(&csAccept);

	InitializeCriticalSection(&csServerOn);


	// Ping 시간 측정
	StartTime = FDateTime::UtcNow();
	InitializeCriticalSection(&csPing);
	EnterCriticalSection(&csPing);
	Ping = 0;
	LeaveCriticalSection(&csPing);


	InitializeCriticalSection(&csMyInfoOfScoreBoard);
	EnterCriticalSection(&csMyInfoOfScoreBoard);
	MyInfoOfScoreBoard = cInfoOfScoreBoard();
	LeaveCriticalSection(&csMyInfoOfScoreBoard);


	InitializeCriticalSection(&csPossessedID);
	EnterCriticalSection(&csPossessedID);
	PossessedID = 0;
	LeaveCriticalSection(&csPossessedID);


	CONSOLE_LOG("[END] <cGameClient::cGameClient()>\n");
}
cGameClient::~cGameClient()
{
	Close();

	DeleteCriticalSection(&csAccept);

	DeleteCriticalSection(&csServerOn);

	DeleteCriticalSection(&csPing);

	DeleteCriticalSection(&csMyInfoOfScoreBoard);

	DeleteCriticalSection(&csPossessedID);
}

bool cGameClient::Initialize()
{
	/// 안정성을 보장하기 위하여, 작동중인 소켓을 닫아줍니다.
	Close();

	if (bIsInitialized == true)
	{

		CONSOLE_LOG("[INFO] <cGameClient::Initialize()> if (bIsInitialized == true)\n");

		return true;
	}


	CONSOLE_LOG("\n\n/********** cGameClient **********/\n");
	CONSOLE_LOG("[INFO] <cGameClient::Initialize()>\n");


	WSADATA wsaData;

	// 윈속 버전을 2.2로 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{

		CONSOLE_LOG("[ERROR] <cGameClient::Initialize()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)\n");

		return false;
	}

	// TCP 소켓 생성	
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET)
	{
		WSACleanup();


		CONSOLE_LOG("[ERROR] <cGameClient::Initialize()> if (ServerSocket == INVALID_SOCKET)\n");

		return false;
	}

	SetSockOpt(ServerSocket, 1048576, 1048576);

	bIsInitialized = true;

	return true;
}

bool cGameClient::Connect(const char * pszIP, int nPort)
{
	if (bIsInitialized == false)
	{

		CONSOLE_LOG("[INFO] <cGameClient::Connect(...)> if (bIsInitialized == false)\n");

		return false;
	}

	if (bIsConnected == true)
	{

		CONSOLE_LOG("[INFO] <cGameClient::Connect(...)> if (bIsConnected == true)\n");

		return true;
	}


	CONSOLE_LOG("[START] <cGameClient::Connect(...)>\n");


	// 접속할 서버 정보를 저장할 구조체
	SOCKADDR_IN stServerAddr;

	stServerAddr.sin_family = AF_INET;
	// 접속할 서버 포트 및 IP
	stServerAddr.sin_port = htons(nPort);
	//stServerAddr.sin_addr.s_addr = inet_addr(pszIP); // Waring: C4996 심각도	코드	설명	프로젝트	파일	줄	비표시 오류(Suppression) 상태, 경고 C4996	'inet_addr': Use inet_pton() or InetPton() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings	Game
	inet_pton(AF_INET, pszIP, &stServerAddr.sin_addr.s_addr);

	if (connect(ServerSocket, (sockaddr*)&stServerAddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{

		CONSOLE_LOG("[ERROR] <cGameClient::Connect(...)> if (connect(...) == SOCKET_ERROR)\n");

		return false;
	}

	CONSOLE_LOG("\t Connect() Success.\n");


	SendConnected();

	bIsConnected = true;


	CONSOLE_LOG("[END] <cGameClient::Connect(...)>\n");


	return true;
}


void cGameClient::Close()
{

	CONSOLE_LOG("[START] <cGameClient::Close()>\n");



	// 게임클라이언트를 종료하기 전에 조종하던 Pioneer가 죽게끔 알립니다.
	if (bIsClientSocketOn)
	{
		EnterCriticalSection(&csPossessedID);
		int possessedID = PossessedID;
		PossessedID = 0;
		LeaveCriticalSection(&csPossessedID);

		SendDiedPioneer(possessedID);
	}


	// 게임클라이언트를 종료하면 남아있던 WSASend(...)를 다 보내기 위해 Alertable Wait 상태로 만듭니다.
	SleepEx(1, true);

	EnterCriticalSection(&csAccept);
	bAccept = false;
	LeaveCriticalSection(&csAccept);

	StartTime = FDateTime::UtcNow();
	EnterCriticalSection(&csPing);
	Ping = 0;
	LeaveCriticalSection(&csPing);

	if (bIsInitialized == false)
	{

		CONSOLE_LOG("[END] <cGameClient::Close()> if (bIsInitialized == false)\n");

		return;
	}
	bIsInitialized = false;

	if (ServerSocket != NULL && ServerSocket != INVALID_SOCKET)
	{
		closesocket(ServerSocket);
		ServerSocket = NULL;


		CONSOLE_LOG("\t closesocket(ServerSocket);\n");

	}

	WSACleanup();

	if (bIsConnected == false)
	{

		CONSOLE_LOG("[END] <cGameClient::Close()> if (bIsConnected == false)\n");

		return;
	}
	bIsConnected = false;


	if (bIsClientSocketOn == false)
	{

		CONSOLE_LOG("[END] <cGameClient::Close()> if (bIsClientSocketOn == false)\n");

		return;
	}
	bIsClientSocketOn = false;


	////////////////////
	// 메인 스레드 종료
	////////////////////
	if (hMainHandle != NULL && hMainHandle != INVALID_HANDLE_VALUE)
	{
		DWORD result = WaitForSingleObject(hMainHandle, 10000);

		// hMainHandle이 signal이면
		if (result == WAIT_OBJECT_0)
		{
			CloseHandle(hMainHandle);


			CONSOLE_LOG("\t CloseHandle(hMainHandle);\n");

		}
		else if (result == WAIT_TIMEOUT)
		{

			CONSOLE_LOG("[ERROR] WaitForSingleObject(...) result: WAIT_TIMEOUT\n");


			TerminateThread(hMainHandle, 0);
			CloseHandle(hMainHandle);


			CONSOLE_LOG("\t TerminateThread(hMainHandle, 0); CloseHandle(hMainHandle);\n");

		}
		else
		{

			CONSOLE_LOG("[ERROR] WaitForSingleObject(...) failed: %d\n", (int)GetLastError());

		}

		hMainHandle = NULL;
	}


	////////////////////
	// RecvDeque 초기화
	////////////////////
	while (RecvDeque.empty() == false)
	{
		if (RecvDeque.front())
		{
			delete[] RecvDeque.front();
			RecvDeque.front() = nullptr;
		}
		RecvDeque.pop_front();
	}


	//////////////////////
	//// 멤버변수들 초기화
	//////////////////////
	EnterCriticalSection(&csMyInfoOfScoreBoard);
	MyInfoOfScoreBoard = cInfoOfScoreBoard();
	LeaveCriticalSection(&csMyInfoOfScoreBoard);

	EnterCriticalSection(&csPossessedID);
	PossessedID = 0;
	LeaveCriticalSection(&csPossessedID);

	tsqScoreBoard.clear();
	tsqSpaceShip.clear();
	tsqSpawnPioneer.clear();
	tsqDiedPioneer.clear();
	tsqInfoOfPioneer_Animation.clear();
	tsqPossessPioneer.clear();
	tsqInfoOfPioneer_Socket.clear();
	tsqInfoOfPioneer_Stat.clear();
	tsqInfoOfProjectile.clear();
	tsqInfoOfResources.clear();
	tsqInfoOfBuilding_Spawn.clear();
	tsqInfoOfBuilding.clear();
	tsqInfoOfBuilding_Stat.clear();
	tsqDestroyBuilding.clear();
	tsqSpawnEnemy.clear();
	tsqInfoOfEnemy_Animation.clear();
	tsqInfoOfEnemy_Stat.clear();
	tsqDestroyEnemy.clear();
	tsqExp.clear();


	CONSOLE_LOG("[END] <cGameClient::Close()>\n");

}

void CALLBACK SendCompletionRoutineBycGameClient(
	IN DWORD dwError,
	IN DWORD cbTransferred,
	IN LPWSAOVERLAPPED lpOverlapped,
	IN DWORD dwFlags)
{

	//CONSOLE_LOG("[START] <cGameClient::CompletionROUTINE(...)> \n");



	if (dwError != 0)
	{

		CONSOLE_LOG("[ERROR] <cGameClient::CompletionROUTINE(...)> Fail to WSASend(...) : %d\n", WSAGetLastError());

	}

	//CONSOLE_LOG("[INFO] <cGameClient::CompletionROUTINE(...)> Success to WSASend(...)\n");


	stOverlappedMsg* overlappedMsg = (stOverlappedMsg*)lpOverlapped;
	if (overlappedMsg)
	{
		// 전송할 데이터 사이즈와 전송된 데이터 사이즈가 다르면
		if (overlappedMsg->sendBytes != cbTransferred)
		{

			CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
			CONSOLE_LOG("[ERROR] <cGameClient::CompletionROUTINE(...)> if (overlappedMsg->sendBytes != cbTransferred) \n");
			CONSOLE_LOG("[ERROR] <cGameClient::CompletionROUTINE(...)> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);
			CONSOLE_LOG("[ERROR] <cGameClient::CompletionROUTINE(...)> cbTransferred: %d \n", (int)cbTransferred);
			CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");

		}

		delete overlappedMsg;

		//CONSOLE_LOG("\t delete overlappedMsg; \n");

	}



	//CONSOLE_LOG("[END] <cGameClient::CompletionROUTINE(...)> \n");

}

void cGameClient::Send(stringstream& SendStream)
{
	// 참고: https://driftmind.tistory.com/50
	//WSAWaitForMultipleEvents(1, &event, TRUE, WSA_INFINITE, FALSE); // IO가 완료되면 event가 시그널 상태가 됩니다.
	//WSAGetOverlappedResult(hSocket, &overlapped, (LPDWORD)&sendBytes, FALSE, NULL);


	/////////////////////////////
	// 소켓 유효성 검증
	/////////////////////////////
	if (ServerSocket == NULL || ServerSocket == INVALID_SOCKET)
	{

		CONSOLE_LOG("[ERROR] <cGameClient::Send(...)> if (ServerSocket == NULL || ServerSocket == INVALID_SOCKET) \n");

		return;
	}

	//CONSOLE_LOG("[START] <cGameClient::Send(...)> \n");



	stringstream finalStream;
	if (AddSizeInStream(SendStream, finalStream) == false)
	{

		CONSOLE_LOG("\n\n\n\n\n [ERROR] <cGameClient::Send(...)> if (AddSizeInStream(SendStream, finalStream) == false) \n\n\n\n\n\n");

		return;
	}

	DWORD	dwFlags = 0;

	stOverlappedMsg* overlappedMsg = new stOverlappedMsg();
	
	memset(&(overlappedMsg->overlapped), 0, sizeof(OVERLAPPED));
	overlappedMsg->overlapped.hEvent = &overlappedMsg; // CompletionRoutine 기반이므로 overlapped.hEvent를 활용
	CopyMemory(overlappedMsg->messageBuffer, (CHAR*)finalStream.str().c_str(), finalStream.str().length());
	overlappedMsg->messageBuffer[finalStream.str().length()] = '\0';
	overlappedMsg->dataBuf.len = finalStream.str().length();
	overlappedMsg->dataBuf.buf = overlappedMsg->messageBuffer;
	overlappedMsg->recvBytes = 0;
	overlappedMsg->sendBytes = overlappedMsg->dataBuf.len;


	//CONSOLE_LOG("[INFO] <cGameClient::Send(...)> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);



	////////////////////////////////////////////////
	// (임시) 패킷 사이즈와 실제 길이 검증용 함수
	////////////////////////////////////////////////
	VerifyPacket(overlappedMsg->messageBuffer, true);


	int nResult = WSASend(
		ServerSocket, // s: 연결 소켓을 가리키는 소켓 지정 번호
		&(overlappedMsg->dataBuf), // lpBuffers: WSABUF(:4300)구조체 배열의 포인터로 각각의 WSABUF 구조체는 버퍼와 버퍼의 크기를 가리킨다.
		1, // dwBufferCount: lpBuffers에 있는 WSABUF(:4300)구조체의 개수
		NULL, // lpNumberOfBytesSent: 함수의 호출로 전송된 데이터의 바이트 크기를 넘겨준다. 만약 매개 변수 lpOverlapped가 NULL이 아니라면, 이 매개 변수의 값은 NULL로 해야 한다. 그래야 (잠재적인)잘못된 반환을 피할 수 있다.
		dwFlags,// dwFlags: WSASend 함수를 어떤 방식으로 호출 할것인지를 지정한다.
		&(overlappedMsg->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)구조체의 포인터다. 비 (overlapped)중첩 소켓에서는 무시된다.
		SendCompletionRoutineBycGameClient // lpCompletionRoutine: 데이터 전송이 완료 되었을 때 호출할 완료 루틴 (completion routine)의 포인터. 비 중첩 소켓에서는 무시 된다.
	);

	if (nResult == 0)
	{

		//CONSOLE_LOG("[INFO] <cGameClient::Send(...)> Success to WSASend(...) \n");

	}
	if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{

			CONSOLE_LOG("[ERROR] <cGameClient::Send(...)> Fail to WSASend(...) : %d \n", WSAGetLastError());


			delete overlappedMsg;
			overlappedMsg = nullptr;


			CONSOLE_LOG("[ERROR] <cGameClient::Send(...)> delete overlappedMsg; \n");


			/// 서버소켓을 닫아도 되는지 아직 확인이 안되었습니다.
			///Close();

			// 서버 연결끊김 확인
			EnterCriticalSection(&csServerOn);
			bServerOn = false;
			LeaveCriticalSection(&csServerOn);
		}
		else
		{

			//CONSOLE_LOG("[INFO] <cGameClient::Send(...)> WSASend: WSA_IO_PENDING \n");

		}
	}

	//send(ServerSocket, (CHAR*)finalStream.str().c_str(), finalStream.str().length(), 0);



	//CONSOLE_LOG("[END] <cGameClient::Send(...)>\n");

}


///////////////////////////////////////////
// 소켓 버퍼 크기 변경
///////////////////////////////////////////
void cGameClient::SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf)
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


	CONSOLE_LOG("[START] <cGameClient::SetSockOpt(...)> \n");



	int optval;
	int optlen = sizeof(optval);

	// 성공시 0, 실패시 -1 반환
	if (getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, &optlen) == 0)
	{

		CONSOLE_LOG("\t Socket: %d, getsockopt SO_SNDBUF: %d \n", (int)Socket, optval);

	}
	if (getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, &optlen) == 0)
	{

		CONSOLE_LOG("\t Socket: %d, getsockopt SO_RCVBUF: %d \n", (int)Socket, optval);

	}

	optval = SendBuf;
	if (setsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, sizeof(optval)) == 0)
	{

		CONSOLE_LOG("\t Socket: %d, setsockopt SO_SNDBUF: %d \n", (int)Socket, optval);

	}
	optval = RecvBuf;
	if (setsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, sizeof(optval)) == 0)
	{

		CONSOLE_LOG("\t Socket: %d, setsockopt SO_RCVBUF: %d \n", (int)Socket, optval);

	}

	if (getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, &optlen) == 0)
	{

		CONSOLE_LOG("\t Socket: %d, getsockopt SO_SNDBUF: %d \n", (int)Socket, optval);

	}
	if (getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, &optlen) == 0)
	{

		CONSOLE_LOG("\t Socket: %d, getsockopt SO_RCVBUF: %d \n", (int)Socket, optval);

	}



	CONSOLE_LOG("[END] <cGameClient::SetSockOpt(...)> \n");

}


///////////////////////////////////////////
// stringstream의 맨 앞에 size를 추가
///////////////////////////////////////////
bool cGameClient::AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
{
	if (DataStream.str().length() == 0)
	{

		CONSOLE_LOG("[ERROR] <cGameClient::AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");

		return false;
	}

	//CONSOLE_LOG("[START] <cGameClient::AddSizeInStream(...)> \n");

	//// ex) DateStream의 크기 : 98
	//CONSOLE_LOG("\t DataStream size: %d\n", (int)DataStream.str().length());
	//CONSOLE_LOG("\t DataStream: %s\n", DataStream.str().c_str());


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


	//CONSOLE_LOG("\t FinalStream size: %d\n", (int)FinalStream.str().length());
	//CONSOLE_LOG("\t FinalStream: %s\n", FinalStream.str().c_str());



	// 전송할 데이터가 최대 버퍼 크기보다 크거나 같으면 전송 불가능을 알립니다.
	// messageBuffer[MAX_BUFFER];에서 마지막에 '\0'을 넣어줘야 되기 때문에 MAX_BUFFER와 같을때도 무시합니다.
	if (FinalStream.str().length() >= MAX_BUFFER)
	{
		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
		CONSOLE_LOG("[ERROR] <cGameClient::AddSizeInStream(...)> if (FinalStream.str().length() > MAX_BUFFER \n");
		CONSOLE_LOG("[ERROR] <cGameClient::AddSizeInStream(...)> FinalStream.str().length(): %d \n", (int)FinalStream.str().length());
		CONSOLE_LOG("[ERROR] <cGameClient::AddSizeInStream(...)> FinalStream.str().c_str(): %s \n", FinalStream.str().c_str());
		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
		return false;
	}


	//CONSOLE_LOG("[END] <v::AddSizeInStream(...)> \n");


	return true;
}


///////////////////////////////////////////
// recvDeque에 수신한 데이터를 적재
///////////////////////////////////////////
void cGameClient::PushRecvBufferInDeque(char* RecvBuffer, int RecvLen)
{
	if (!RecvBuffer)
	{

		CONSOLE_LOG("[ERROR] <cGameClient::PushRecvBufferInQueue(...)> if (!RecvBuffer) \n");

		return;
	}

	// 데이터가 MAX_BUFFER 그대로 4096개 꽉 채워서 오는 경우가 있기 때문에, 대비하기 위하여 +1로 '\0' 공간을 만들어줍니다.
	char* newBuffer = new char[MAX_BUFFER + 1];
	//ZeroMemory(newBuffer, MAX_BUFFER);
	CopyMemory(newBuffer, RecvBuffer, RecvLen);
	newBuffer[RecvLen] = '\0';

	RecvDeque.push_back(newBuffer); // 뒤에 순차적으로 적재합니다.
}


///////////////////////////////////////////
// 수신한 데이터를 저장하는 덱에서 데이터를 획득
///////////////////////////////////////////
void cGameClient::GetDataInRecvDeque(char* DataBuffer)
{
	if (!DataBuffer)
	{

		CONSOLE_LOG("[ERROR] <cGameClient::GetDataInRecvQueue(...)> if (!DataBuffer) \n");

		return;
	}

	int idxOfStartInQueue = 0;
	int idxOfStartInNextQueue = 0;

	// 덱이 빌 때까지 진행 (buffer가 다 차면 반복문을 빠져나옵니다.)
	while (RecvDeque.empty() == false)
	{
		// dataBuffer를 채우려고 하는 사이즈가 최대로 MAX_BUFFER면 CopyMemory 가능.
		if ((idxOfStartInQueue + strlen(RecvDeque.front())) < MAX_BUFFER + 1)
		{
			CopyMemory(&DataBuffer[idxOfStartInQueue], RecvDeque.front(), strlen(RecvDeque.front()));
			idxOfStartInQueue += (int)strlen(RecvDeque.front());
			DataBuffer[idxOfStartInQueue] = '\0';

			delete[] RecvDeque.front();
			RecvDeque.front() = nullptr;
			RecvDeque.pop_front();
		}
		else
		{
			// 버퍼에 남은 자리 만큼 꽉 채웁니다.
			idxOfStartInNextQueue = MAX_BUFFER - idxOfStartInQueue;
			CopyMemory(&DataBuffer[idxOfStartInQueue], RecvDeque.front(), idxOfStartInNextQueue);
			DataBuffer[MAX_BUFFER] = '\0';


			// dateBuffer에 복사하고 남은 데이터들을 임시 버퍼에 복사합니다. 
			int lenOfRestInNextQueue = (int)strlen(&RecvDeque.front()[idxOfStartInNextQueue]);
			char tempBuffer[MAX_BUFFER + 1];
			CopyMemory(tempBuffer, &RecvDeque.front()[idxOfStartInNextQueue], lenOfRestInNextQueue);
			tempBuffer[lenOfRestInNextQueue] = '\0';

			// 임시 버퍼에 있는 데이터들을 다시 RecvDeque.front()에 복사합니다.
			CopyMemory(RecvDeque.front(), tempBuffer, strlen(tempBuffer));
			RecvDeque.front()[strlen(tempBuffer)] = '\0';

			break;
		}
	}
}


///////////////////////////////////////////
// 패킷을 처리합니다.
///////////////////////////////////////////
void cGameClient::ProcessReceivedPacket(char* DataBuffer)
{
	if (!DataBuffer)
	{

		CONSOLE_LOG("[ERROR] <cGameClient::ProcessReceivedPacket(...)> if (!DataBuffer) \n");

		return;
	}

	// Ping
	double gap = (FDateTime::UtcNow() - StartTime).GetTotalMilliseconds();
	if (gap > 10000.0)
		gap = 9999.0;
	else if (gap < 1.0)
		gap = 1.0;

	EnterCriticalSection(&csPing);
	Ping = (int)gap;
	LeaveCriticalSection(&csPing);
	StartTime = FDateTime::UtcNow();


	stringstream recvStream;
	recvStream << DataBuffer;

	// 사이즈 확인
	int sizeOfRecvStream = 0;
	recvStream >> sizeOfRecvStream;

	//CONSOLE_LOG("\t sizeOfRecvStream: %d \n", sizeOfRecvStream);


	// 패킷 종류 확인
	int packetType = -1;
	recvStream >> packetType;

	//CONSOLE_LOG("\t packetType: %d \n", packetType);


	switch (packetType)
	{
	case EPacketType::CONNECTED:
	{
		RecvConnected(recvStream);
	}
	break;
	case EPacketType::SCORE_BOARD:
	{
		RecvScoreBoard(recvStream);
	}
	break;
	case EPacketType::SPACE_SHIP:
	{
		RecvSpaceShip(recvStream);
	}
	break;
	case EPacketType::SPAWN_PIONEER:
	{
		RecvSpawnPioneer(recvStream);
	}
	break;
	case EPacketType::DIED_PIONEER:
	{
		RecvDiedPioneer(recvStream);
	}
	break;
	case EPacketType::INFO_OF_PIONEER_ANIMATION:
	{
		RecvInfoOfPioneer_Animation(recvStream);
	}
	break;
	case EPacketType::POSSESS_PIONEER:
	{
		RecvPossessPioneer(recvStream);
	}
	break;
	case EPacketType::INFO_OF_PIONEER_SOCKET:
	{
		RecvInfoOfPioneer_Socket(recvStream);
	}
	break;
	case EPacketType::INFO_OF_PIONEER_STAT:
	{
		RecvInfoOfPioneer_Stat(recvStream);
	}
	break;
	case EPacketType::INFO_OF_PROJECTILE:
	{
		RecvInfoOfProjectile(recvStream);
	}
	break;
	case EPacketType::INFO_OF_RESOURCES:
	{
		RecvInfoOfResources(recvStream);
	}
	break;
	case EPacketType::INFO_OF_BUILDING_SPAWN:
	{
		RecvInfoOfBuilding_Spawn(recvStream);
	}
	break;
	break;
	case EPacketType::INFO_OF_BUILDING:
	{
		RecvInfoOfBuilding_Spawned(recvStream);
	}
	break;
	case EPacketType::INFO_OF_BUILDING_STAT:
	{
		RecvInfoOfBuilding_Stat(recvStream);
	}
	break;
	case EPacketType::DESTROY_BUILDING:
	{
		RecvDestroyBuilding(recvStream);
	}
	break;
	case EPacketType::SPAWN_ENEMY:
	{
		RecvSpawnEnemy(recvStream);
	}
	break;
	case EPacketType::INFO_OF_ENEMY_ANIMATION:
	{
		RecvInfoOfEnemy_Animation(recvStream);
	}
	break;
	case EPacketType::INFO_OF_ENEMY_STAT:
	{
		RecvInfoOfEnemy_Stat(recvStream);
	}
	break;
	case EPacketType::DESTROY_ENEMY:
	{
		RecvDestroyEnemy(recvStream);
	}
	break;

	default:
	{

		CONSOLE_LOG("[ERROR] <cGameClient::ProcessReceivedPacket()> unknown packet type! PacketType: %d \n", packetType);
		CONSOLE_LOG("[ERROR] <cGameClient::ProcessReceivedPacket()> recvBuffer: %s \n", DataBuffer);

	}
	break;
	}
}


bool cGameClient::BeginMainThread()
{
	if (bIsClientSocketOn == true)
	{

		CONSOLE_LOG("[INFO] <cGameClient::BeginMainThread()> if (bIsClientSocketOn == true)\n");

		return true;
	}

	////////////////////
	// 메인 스레드 시작
	////////////////////
	EnterCriticalSection(&csAccept);
	bAccept = true;
	LeaveCriticalSection(&csAccept);


	CONSOLE_LOG("[INFO] <cGameClient::BeginMainThread()> Start main thread\n");

	unsigned int threadId;

	// _beginthreadex()는 ::CloseHandle을 내부에서 호출하지 않기 때문에, 스레드 종료시 사용자가 직접 CloseHandle()해줘야 합니다.
	// 스레드가 종료되면 _endthreadex()가 자동호출됩니다.
	hMainHandle = (HANDLE*)_beginthreadex(NULL, 0, &CallMainThreadIncGameClient, this, CREATE_SUSPENDED, &threadId);
	if (hMainHandle == NULL)
	{

		CONSOLE_LOG("[ERROR] <cGameClient::BeginMainThread()> if (hMainHandle == NULL)\n");

		return false;
	}
	ResumeThread(hMainHandle);

	bIsClientSocketOn = true;

	EnterCriticalSection(&csServerOn);
	bServerOn = true;
	LeaveCriticalSection(&csServerOn);

	return true;
}

void cGameClient::RunMainThread()
{
	// 수신 버퍼 스트림 (최대 MAX_BUFFER 사이즈의 데이터를 저장하기 때문에, 마지막 '\0'용 사이즈가 필요)
	char recvBuffer[MAX_BUFFER + 1];

	// recv while loop 시작
	// StopTaskCounter 클래스 변수를 사용해 Thread Safety하게 해줌
	while (true)
	{
		// Alertable Wait 상태를 만들기 위해
		SleepEx(1, true);

		// 임계영역
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			LeaveCriticalSection(&csAccept);
			return;
		}
		LeaveCriticalSection(&csAccept);

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

			CONSOLE_LOG("[ERROR] if (ioctlsocket(ServerSocket, FIONREAD, &amount) == -1) \n");

			continue;
		}

		// recv 버퍼에 데이터가 4바이트 미만으로 쌓여있는 상황이면 recv 하지 않습니다.
		if (amount < 4)
		{
			// recv 버퍼에 데이터가 0바이트 존재하면 아직 아무 패킷도 받지 않은것이므로 회문합니다.
			if (amount == 0)
				continue;


			//CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n [ERROR] amount: %d \n\n\n\n\n\n\n\n\n\n\n", (int)amount);

			continue;
		}

		// 수신
		nRecvLen = recv(ServerSocket, (CHAR*)&recvBuffer, MAX_BUFFER, 0);
		recvBuffer[nRecvLen] = '\0';


		///////////////////////////////////////////
		// recvDeque에 수신한 데이터를 적재
		///////////////////////////////////////////
		PushRecvBufferInDeque(recvBuffer, nRecvLen);

		/**************************************************************************/

		char dataBuffer[MAX_BUFFER + 1];
		dataBuffer[0] = '\0'; // GetDataInRecvDeque(...)를 해도 덱이 비어있는 상태면 오류가 날 수 있으므로 초기화
		dataBuffer[MAX_BUFFER] = '\0';

		///////////////////////////////////////////
		// 수신한 데이터를 저장하는 덱에서 데이터를 획득
		///////////////////////////////////////////
		GetDataInRecvDeque(dataBuffer);


		/////////////////////////////////////////////
		// 1. 데이터 버퍼 길이가 0이면
		/////////////////////////////////////////////
		if (strlen(dataBuffer) == 0)
		{

			//CONSOLE_LOG("\t if (strlen(dataBuffer) == 0) \n");

		}
		/////////////////////////////////////////////
		// 2. 데이터 버퍼 길이가 4미만이면
		/////////////////////////////////////////////
		if (strlen(dataBuffer) < 4)
		{

			//CONSOLE_LOG("\t if (strlen(dataBuffer) < 4): %d \n", (int)strlen(dataBuffer));


			// dataBuffer의 남은 데이터를 newBuffer에 복사합니다.
			char* newBuffer = new char[MAX_BUFFER + 1];
			CopyMemory(newBuffer, &dataBuffer, strlen(dataBuffer));
			newBuffer[strlen(dataBuffer)] = '\0';

			// 다시 덱 앞부분에 적재합니다.
			RecvDeque.push_front(newBuffer);
		}
		/////////////////////////////////////////////
		// 3. 데이터 버퍼 길이가 4이상 MAX_BUFFER + 1 미만이면
		/////////////////////////////////////////////
		else if (strlen(dataBuffer) < MAX_BUFFER + 1)
		{

			//CONSOLE_LOG("\t else if (strlen(dataBuffer) < MAX_BUFFER + 1): %d \n", (int)strlen(dataBuffer));


			int idxOfStartInPacket = 0;
			int lenOfDataBuffer = (int)strlen(dataBuffer);

			while (idxOfStartInPacket < lenOfDataBuffer)
			{

				//CONSOLE_LOG("\t idxOfStartInPacket: %d \n", idxOfStartInPacket);
				//CONSOLE_LOG("\t lenOfDataBuffer: %d \n", lenOfDataBuffer);


				// 버퍼 길이가 4이하면 아직 패킷이 전부 수신되지 않은것이므로
				if ((lenOfDataBuffer - idxOfStartInPacket) < 4)
				{

					//CONSOLE_LOG("\t if (lenOfDataBuffer - idxOfStartInPacket < 4): %d \n", lenOfDataBuffer - idxOfStartInPacket);


					// dataBuffer의 남은 데이터를 remainingBuffer에 복사합니다.
					char* newBuffer = new char[MAX_BUFFER + 1];
					CopyMemory(newBuffer, &dataBuffer[idxOfStartInPacket], strlen(&dataBuffer[idxOfStartInPacket]));
					newBuffer[strlen(&dataBuffer[idxOfStartInPacket])] = '\0';

					// 다시 덱 앞부분에 적재합니다.
					RecvDeque.push_front(newBuffer);

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


				//CONSOLE_LOG("\t sizeOfPacket: %d \n", sizeOfPacket);
				//CONSOLE_LOG("\t strlen(&dataBuffer[idxOfStartInPacket]): %d \n", (int)strlen(&dataBuffer[idxOfStartInPacket]));


				// 필요한 데이터 사이즈가 버퍼에 남은 데이터 사이즈보다 크면 아직 패킷이 전부 수신되지 않은것이므로
				if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket]))
				{

					//CONSOLE_LOG("\t if (sizeOfPacket > strlen(&dataBuffer[idxOfStartInPacket])) \n");


					// dataBuffer의 남은 데이터를 remainingBuffer에 복사합니다.
					char* newBuffer = new char[MAX_BUFFER + 1];
					CopyMemory(newBuffer, &dataBuffer[idxOfStartInPacket], strlen(&dataBuffer[idxOfStartInPacket]));
					newBuffer[strlen(&dataBuffer[idxOfStartInPacket])] = '\0';

					// 다시 덱 앞부분에 적재합니다.
					RecvDeque.push_front(newBuffer);

					// 반복문을 종료합니다.
					break;;
				}

				/// 오류 확인
				if (sizeOfPacket <= 0)
				{

					CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
					CONSOLE_LOG("[ERROR] <MainServer::WorkerThread()> sizeOfPacket: %d \n", sizeOfPacket);
					CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");

					break;
				}

				// 패킷을 자르면서 임시 버퍼에 복사합니다.
				char cutBuffer[MAX_BUFFER + 1];
				CopyMemory(cutBuffer, &dataBuffer[idxOfStartInPacket], sizeOfPacket);
				cutBuffer[sizeOfPacket] = '\0';


				////////////////////////////////////////////////
				// (임시) 패킷 사이즈와 실제 길이 검증용 함수
				////////////////////////////////////////////////
				VerifyPacket(cutBuffer, false);


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
}

bool cGameClient::IsServerOn()
{
	EnterCriticalSection(&csServerOn);
	bool result = bServerOn;
	LeaveCriticalSection(&csServerOn);

	return result;
}

/////////////////////////////////////
// Game Server / Game Clients
/////////////////////////////////////
void cGameClient::SendConnected()
{
	CONSOLE_LOG("[Start] <cGameClient::SendConnected()>\n");


	cInfoOfPlayer infoOfPlayer = cMainClient::GetSingleton()->CopyMyInfo();

	stringstream sendStream;
	sendStream << EPacketType::CONNECTED << endl;
	sendStream << infoOfPlayer << endl;

	Send(sendStream);

	infoOfPlayer.PrintInfo();


	EnterCriticalSection(&csMyInfoOfScoreBoard);
	MyInfoOfScoreBoard.ID = infoOfPlayer.ID;
	LeaveCriticalSection(&csMyInfoOfScoreBoard);


	CONSOLE_LOG("[End] <cGameClient::SendConnected()>\n");
}
void cGameClient::RecvConnected(stringstream& RecvStream)
{
	cInfoOfPlayer infoOfPlayer;

	RecvStream >> infoOfPlayer;

	cMainClient::GetSingleton()->SetMyInfo(infoOfPlayer);

	infoOfPlayer.PrintInfo();


	CONSOLE_LOG("[End] <cGameClient::RecvConnected(...)>\n");
}

void cGameClient::SendScoreBoard()
{
	//CONSOLE_LOG("[Start] <cGameClient::SendScoreBoard()>\n");


	EnterCriticalSection(&csMyInfoOfScoreBoard);

	EnterCriticalSection(&csPing);
	MyInfoOfScoreBoard.Ping = Ping;
	LeaveCriticalSection(&csPing);

	cInfoOfScoreBoard infoOfScoreBoard = MyInfoOfScoreBoard;
	LeaveCriticalSection(&csMyInfoOfScoreBoard);


	stringstream sendStream;
	sendStream << EPacketType::SCORE_BOARD << endl;
	sendStream << infoOfScoreBoard << endl;

	Send(sendStream);


	//CONSOLE_LOG("[End] <cGameClient::SendScoreBoard()>\n");
}
void cGameClient::RecvScoreBoard(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cGameClient::RecvScoreBoard(...)>\n");


	cInfoOfScoreBoard infoOfScoreBoard;

	while (RecvStream >> infoOfScoreBoard)
	{
		tsqScoreBoard.push(infoOfScoreBoard);

		//infoOfScoreBoard.PrintInfo();
	}


	//CONSOLE_LOG("[End] <cGameClient::RecvScoreBoard(...)>\n");
}

void cGameClient::RecvSpaceShip(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cGameClient::RecvSpaceShip(...)>\n");


	cInfoOfSpaceShip infoOfSpaceShip;

	RecvStream >> infoOfSpaceShip;

	tsqSpaceShip.push(infoOfSpaceShip);

	//infoOfSpaceShip.PrintInfo();


	//CONSOLE_LOG("[End] <cGameClient::RecvSpaceShip(...)>\n");
}

void cGameClient::SendObservation()
{
	//CONSOLE_LOG("[Start] <cGameClient::SendObservation()>\n");


	stringstream sendStream;
	sendStream << EPacketType::OBSERVATION << endl;

	Send(sendStream);


	//CONSOLE_LOG("[End] <cGameClient::SendObservation()>\n");
}

void cGameClient::RecvSpawnPioneer(stringstream& RecvStream)
{
	CONSOLE_LOG("[Start] <cGameClient::RecvSpawnPioneer(...)>\n");


	cInfoOfPioneer infoOfPioneer;

	RecvStream >> infoOfPioneer;
	
	tsqSpawnPioneer.push(infoOfPioneer);

	infoOfPioneer.PrintInfo();


	CONSOLE_LOG("[End] <cGameClient::RecvSpawnPioneer(...)>\n");
}

void cGameClient::SendDiedPioneer(int ID)
{
	CONSOLE_LOG("[Start] <cGameClient::SendDiedPioneer()>\n");


	EnterCriticalSection(&csMyInfoOfScoreBoard);
	MyInfoOfScoreBoard.Death++;
	MyInfoOfScoreBoard.State = "관전중";
	LeaveCriticalSection(&csMyInfoOfScoreBoard);


	stringstream sendStream;
	sendStream << EPacketType::DIED_PIONEER << endl;
	sendStream << ID << endl;

	Send(sendStream);


	CONSOLE_LOG("\t ID: %d\n", ID);

	CONSOLE_LOG("[End] <cGameClient::SendDiedPioneer()>\n");
}
void cGameClient::RecvDiedPioneer(stringstream& RecvStream)
{
	CONSOLE_LOG("[Start] <cGameClient::RecvDiedPioneer(...)>\n");


	int id = 0;

	RecvStream >> id;

	tsqDiedPioneer.push(id);

	CONSOLE_LOG("\t ID: %d\n", id);

	CONSOLE_LOG("[End] <cGameClient::RecvDiedPioneer(...)>\n");
}

void cGameClient::SendInfoOfPioneer_Animation(class APioneer* PioneerOfPlayer)
{
	//CONSOLE_LOG("[Start] <cGameClient::SendInfoOfPioneer_Animation()>\n");


	cInfoOfPioneer_Animation animtion;
	
	if (PioneerOfPlayer)
	{
		animtion = PioneerOfPlayer->GetInfoOfPioneer_Animation();
	}
	
	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PIONEER_ANIMATION << endl;
	sendStream << animtion << endl;

	Send(sendStream);


	//CONSOLE_LOG("[End] <cGameClient::SendInfoOfPioneer_Animation()>\n");
}
void cGameClient::RecvInfoOfPioneer_Animation(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cGameClient::RecvInfoOfPioneer_Animation(...)>\n");


	cInfoOfPioneer_Animation animation;

	while (RecvStream >> animation)
	{
		tsqInfoOfPioneer_Animation.push(animation);

		//animation.PrintInfo();
	}


	//CONSOLE_LOG("[End] <cGameClient::RecvInfoOfPioneer_Animation(...)>\n");
}

void cGameClient::SendPossessPioneer(cInfoOfPioneer_Socket Socket)
{
	//CONSOLE_LOG("[Start] <cGameClient::SendPossessPioneer()>\n");


	stringstream sendStream;
	sendStream << EPacketType::POSSESS_PIONEER << endl;
	sendStream << Socket << endl;

	Send(sendStream);

	Socket.PrintInfo();



	//CONSOLE_LOG("[End] <cGameClient::SendPossessPioneer()>\n");
}
void cGameClient::RecvPossessPioneer(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cGameClient::RecvPossessPioneer(...)>\n");


	cInfoOfPioneer_Socket socket;

	RecvStream >> socket;

	tsqPossessPioneer.push(socket);

	socket.PrintInfo();


	if (socket.ID != 0)
	{
		EnterCriticalSection(&csPossessedID);
		PossessedID = socket.ID;
		LeaveCriticalSection(&csPossessedID);

		EnterCriticalSection(&csMyInfoOfScoreBoard);
		MyInfoOfScoreBoard.State = "진행중";
		LeaveCriticalSection(&csMyInfoOfScoreBoard);
	}


	//CONSOLE_LOG("[End] <cGameClient::RecvPossessPioneer(...)>\n");
}

void cGameClient::RecvInfoOfPioneer_Socket(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cGameClient::RecvInfoOfPioneer_Socket(...)>\n");


	cInfoOfPioneer_Socket socket;

	RecvStream >> socket;

	tsqInfoOfPioneer_Socket.push(socket);

	socket.PrintInfo();


	//CONSOLE_LOG("[End] <cGameClient::RecvInfoOfPioneer_Socket(...)>\n");
}

void cGameClient::SendInfoOfPioneer_Stat(class APioneer* PioneerOfPlayer)
{
	//CONSOLE_LOG("[Start] <cGameClient::SendInfoOfPioneer_Stat()>\n");


	cInfoOfPioneer_Stat stat;

	if (PioneerOfPlayer)
	{
		stat = PioneerOfPlayer->GetInfoOfPioneer_Stat();
	}

	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PIONEER_STAT << endl;
	sendStream << stat << endl;

	Send(sendStream);

	//stat.PrintInfo();


	//CONSOLE_LOG("[End] <cGameClient::SendInfoOfPioneer_Stat()>\n");
}
void cGameClient::RecvInfoOfPioneer_Stat(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cGameClient::RecvInfoOfPioneer_Stat(...)>\n");


	cInfoOfPioneer_Stat stat;

	while (RecvStream >> stat)
	{
		tsqInfoOfPioneer_Stat.push(stat);

		//stat.PrintInfo();
	}


	//CONSOLE_LOG("[End] <cGameClient::RecvInfoOfPioneer_Stat(...)>\n");
}

void cGameClient::SendInfoOfProjectile(cInfoOfProjectile InfoOfProjectile)
{
	//CONSOLE_LOG("[Start] <cGameClient::SendInfoOfProjectile()>\n");


	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_PROJECTILE << endl;
	sendStream << InfoOfProjectile << endl;

	Send(sendStream);

	//InfoOfProjectile.PrintInfo();


	//CONSOLE_LOG("[End] <cGameClient::SendInfoOfProjectile()>\n");
}
void cGameClient::RecvInfoOfProjectile(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cGameClient::RecvInfoOfProjectile(...)>\n");


	cInfoOfProjectile infoOfProjectile;

	RecvStream >> infoOfProjectile;

	tsqInfoOfProjectile.push(infoOfProjectile);

	//infoOfProjectile.PrintInfo();


	//CONSOLE_LOG("[End] <cGameClient::RecvInfoOfProjectile(...)>\n");
}

void cGameClient::RecvInfoOfResources(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cGameClient::RecvInfoOfResources(...)>\n");


	cInfoOfResources infoOfResources;

	if (RecvStream >> infoOfResources)
		tsqInfoOfResources.push(infoOfResources);

	//infoOfResources.PrintInfo();


	//CONSOLE_LOG("[End] <cGameClient::RecvInfoOfResources(...)>\n");
}

void cGameClient::SendInfoOfBuilding_Spawn(cInfoOfBuilding_Spawn InfoOfBuilding_Spawn)
{
	//CONSOLE_LOG("[Start] <cGameClient::SendInfoOfBuilding_Spawn()>\n");


	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_BUILDING_SPAWN << endl;
	sendStream << InfoOfBuilding_Spawn << endl;

	Send(sendStream);

	//InfoOfBuilding_Spawn.PrintInfo();


	//CONSOLE_LOG("[End] <cGameClient::SendInfoOfBuilding_Spawn()>\n");
}
void cGameClient::RecvInfoOfBuilding_Spawn(stringstream& RecvStream)
{
	CONSOLE_LOG("[Start] <cGameClient::RecvInfoOfBuilding_Spawn(...)>\n");


	cInfoOfBuilding_Spawn spawn;

	if (RecvStream >> spawn)
	{
		tsqInfoOfBuilding_Spawn.push(spawn);

		spawn.PrintInfo();
	}


	CONSOLE_LOG("[End] <cGameClient::RecvInfoOfBuilding_Spawn(...)>\n");
}

void cGameClient::RecvInfoOfBuilding_Spawned(stringstream& RecvStream)
{
	CONSOLE_LOG("[Start] <cGameClient::RecvInfoOfBuilding_Spawned(...)>\n");


	cInfoOfBuilding infoOfBuilding;

	if (RecvStream >> infoOfBuilding)
	{
		tsqInfoOfBuilding.push(infoOfBuilding);

		infoOfBuilding.PrintInfo();
	}


	CONSOLE_LOG("[End] <cGameClient::RecvInfoOfBuilding_Spawned(...)>\n");
}

void cGameClient::SendInfoOfBuilding_Stat()
{
	//CONSOLE_LOG("[Start] <cGameClient::SendInfoOfBuilding_Stat()>\n");


	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_BUILDING_STAT << endl;

	Send(sendStream);


	//CONSOLE_LOG("[End] <cGameClient::SendInfoOfBuilding_Stat()>\n");
}
void cGameClient::RecvInfoOfBuilding_Stat(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cGameClient::RecvInfoOfBuilding_Stat(...)>\n");


	cInfoOfBuilding_Stat stat;

	while (RecvStream >> stat)
	{
		tsqInfoOfBuilding_Stat.push(stat);

		//stat.PrintInfo();
	}


	//CONSOLE_LOG("[End] <cGameClient::RecvInfoOfBuilding_Stat(...)>\n");
}

void cGameClient::RecvDestroyBuilding(stringstream& RecvStream)
{
	CONSOLE_LOG("[Start] <cGameClient::RecvDestroyBuilding(...)>\n");


	int id;

	if (RecvStream >> id)
	{
		tsqDestroyBuilding.push(id);

		CONSOLE_LOG("\t id: %d \n", id);
	}


	CONSOLE_LOG("[End] <cGameClient::RecvDestroyBuilding(...)>\n");
}

void cGameClient::RecvSpawnEnemy(stringstream& RecvStream)
{
	CONSOLE_LOG("[Start] <cGameClient::RecvSpawnEnemy(...)>\n");


	cInfoOfEnemy infoOfEnemy;

	RecvStream >> infoOfEnemy;

	tsqSpawnEnemy.push(infoOfEnemy);

	infoOfEnemy.PrintInfo();


	CONSOLE_LOG("[End] <cGameClient::RecvSpawnEnemy(...)>\n");
}

void cGameClient::SendInfoOfEnemy_Animation()
{
	//CONSOLE_LOG("[Start] <cGameClient::SendInfoOfEnemy_Animation()>\n");


	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_ENEMY_ANIMATION << endl;

	Send(sendStream);


	//CONSOLE_LOG("[End] <cGameClient::SendInfoOfEnemy_Animation()>\n");
}
void cGameClient::RecvInfoOfEnemy_Animation(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cGameClient::RecvInfoOfEnemy_Animation(...)>\n");


	cInfoOfEnemy_Animation animation;

	while (RecvStream >> animation)
	{
		tsqInfoOfEnemy_Animation.push(animation);

		//animation.PrintInfo();
	}


	//CONSOLE_LOG("[End] <cGameClient::RecvInfoOfEnemy_Animation(...)>\n");
}

void cGameClient::SendInfoOfEnemy_Stat()
{
	//CONSOLE_LOG("[Start] <cGameClient::SendInfoOfEnemy_Stat()>\n");


	stringstream sendStream;
	sendStream << EPacketType::INFO_OF_ENEMY_STAT << endl;

	Send(sendStream);


	//CONSOLE_LOG("[End] <cGameClient::SendInfoOfEnemy_Stat()>\n");
}
void cGameClient::RecvInfoOfEnemy_Stat(stringstream& RecvStream)
{
	//CONSOLE_LOG("[Start] <cGameClient::RecvInfoOfEnemy_Stat(...)>\n");


	cInfoOfEnemy_Stat stat;

	while (RecvStream >> stat)
	{
		tsqInfoOfEnemy_Stat.push(stat);

		//stat.PrintInfo();
	}


	//CONSOLE_LOG("[End] <cGameClient::RecvInfoOfEnemy_Stat(...)>\n");
}

void cGameClient::RecvDestroyEnemy(stringstream& RecvStream)
{
	CONSOLE_LOG("[Start] <cGameClient::RecvDestroyEnemy(...)>\n");


	int idOfEnemy;
	int idOfPioneer;
	int exp;

	RecvStream >> idOfEnemy;
	RecvStream >> idOfPioneer;
	RecvStream >> exp;

	tsqDestroyEnemy.push(idOfEnemy);

	CONSOLE_LOG("\t idOfEnemy: %d \n", idOfEnemy);
	CONSOLE_LOG("\t idOfPioneer: %d \n", idOfPioneer);
	CONSOLE_LOG("\t exp: %d \n", exp);

	if (idOfPioneer != 0)
	{
		EnterCriticalSection(&csPossessedID);
		int possessedID = PossessedID;
		LeaveCriticalSection(&csPossessedID);

		if (idOfPioneer == possessedID)
		{
			EnterCriticalSection(&csMyInfoOfScoreBoard);
			MyInfoOfScoreBoard.Kill++;
			LeaveCriticalSection(&csMyInfoOfScoreBoard);

			tsqExp.push(exp);
		}
	}

	CONSOLE_LOG("[End] <cGameClient::RecvDestroyEnemy(...)>\n");
}