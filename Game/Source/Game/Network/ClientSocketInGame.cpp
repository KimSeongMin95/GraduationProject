
#include "ClientSocketInGame.h"

#include "Runtime/Core/Public/GenericPlatform/GenericPlatformAffinity.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"


/////////////////////////////////////
// FRunnable override 함수
/////////////////////////////////////
bool cClientSocketInGame::Init()
{
	return true;
}

uint32 cClientSocketInGame::Run()
{
	//// 초기 init 과정을 기다림
	//FPlatformProcess::Sleep(0.03);

	// recv while loop 시작
	// StopTaskCounter 클래스 변수를 사용해 Thread Safety하게 해줌
	while (StopTaskCounter.GetValue() == 0)
	{
		stringstream RecvStream;

		int PacketType;
		int nRecvLen = recv(ServerSocket, (CHAR*)&recvBuffer, MAX_BUFFER, 0);

		if (nRecvLen > 0)
		{
			// 패킷 처리
			RecvStream << recvBuffer;
			RecvStream >> PacketType;

			/////////////////////////////
			// 필수!!!!: recvBuffer 초기화
			/////////////////////////////
			memset(recvBuffer, 0, MAX_BUFFER);

			//switch (PacketType)
			//{
			//case EPacketType::LOGIN:
			//{
			//	RecvLogin(RecvStream);
			//}
			//break;


			//default:
			//{

			//}
			//break;
			//}
		}
	}
	return 0;
}

void cClientSocketInGame::Stop()
{
	// thread safety 변수를 조작해 while loop 가 돌지 못하게 함
	StopTaskCounter.Increment();
}

void cClientSocketInGame::Exit()
{

}


/////////////////////////////////////
// cClientSocketInGame
/////////////////////////////////////
cClientSocketInGame::cClientSocketInGame()
	:StopTaskCounter(0)
{
	bIsConnected = false;

	//InitializeCriticalSection(&csMyInfo);
	//InitializeCriticalSection(&csMyInfoOfGame);
}

cClientSocketInGame::~cClientSocketInGame()
{
	CloseSocket();

	//DeleteCriticalSection(&csMyInfo);
	//DeleteCriticalSection(&csMyInfoOfGame);
}

bool cClientSocketInGame::InitSocket()
{
	printf_s("\n\n/********** cClientSocket **********/\n");
	printf_s("[INFO] <cClientSocketInGame::InitSocket()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocketInGame::InitSocket()>"));

	WSADATA wsaData;

	// 윈속 버전을 2.2로 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf_s("[ERROR] <cClientSocketInGame::InitSocket()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <cClientSocketInGame::InitSocket()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)"));
		return false;
	}

	// TCP 소켓 생성	
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET)
	{
		printf_s("[ERROR] <cClientSocketInGame::InitSocket()> if (ServerSocket == INVALID_SOCKET)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <cClientSocketInGame::InitSocket()> if (ServerSocket == INVALID_SOCKET)"));
		return false;
	}

	return true;
}

bool cClientSocketInGame::Connect(const char * pszIP, int nPort)
{
	printf_s("[INFO] <cClientSocketInGame::Connect(...)>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocketInGame::Connect(...)>"));

	// 접속할 서버 정보를 저장할 구조체
	SOCKADDR_IN stServerAddr;

	stServerAddr.sin_family = AF_INET;
	// 접속할 서버 포트 및 IP
	stServerAddr.sin_port = htons(nPort);
	//stServerAddr.sin_addr.s_addr = inet_addr(pszIP); // Waring: C4996 심각도	코드	설명	프로젝트	파일	줄	비표시 오류(Suppression) 상태, 경고 C4996	'inet_addr': Use inet_pton() or InetPton() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings	Game
	inet_pton(AF_INET, pszIP, &stServerAddr.sin_addr.s_addr);

	if (connect(ServerSocket, (sockaddr*)&stServerAddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		printf_s("[ERROR] <cClientSocketInGame::Connect(...)> if (connect(...) == SOCKET_ERROR)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <cClientSocketInGame::Connect(...)> if (connect(...) == SOCKET_ERROR)"));
		return false;
	}

	bIsConnected = true;

	return true;
}

void cClientSocketInGame::CloseSocket()
{
	printf_s("[INFO] <cClientSocketInGame::CloseSocket()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocketInGame::CloseSocket()>"));

	bIsConnected = false;

	closesocket(ServerSocket);
	WSACleanup();
}

bool cClientSocketInGame::StartListen()
{
	printf_s("[INFO] <cClientSocketInGame::StartListen()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocketInGame::StartListen()>"));

	if (Thread != nullptr)
		return false;

	// 스레드 시작
	Thread = FRunnableThread::Create(this, TEXT("cClientSocketInGame"), 0, TPri_BelowNormal);

	return (Thread != nullptr);
}

void cClientSocketInGame::StopListen()
{
	printf_s("[INFO] <cClientSocketInGame::StopListen()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocketInGame::StopListen()>"));

	// 스레드 종료
	Stop();
	if (Thread)
	{
		Thread->WaitForCompletion();
		Thread->Kill();
		delete Thread;
		Thread = nullptr;
	}
	StopTaskCounter.Reset();
}


/////////////////////////////////////
// 서버와 통신
/////////////////////////////////////
