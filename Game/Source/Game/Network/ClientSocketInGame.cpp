
#include "ClientSocketInGame.h"

#include "Runtime/Core/Public/GenericPlatform/GenericPlatformAffinity.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"


/////////////////////////////////////
// FRunnable override �Լ�
/////////////////////////////////////
bool cClientSocketInGame::Init()
{
	return true;
}

uint32 cClientSocketInGame::Run()
{
	//// �ʱ� init ������ ��ٸ�
	//FPlatformProcess::Sleep(0.03);

	// recv while loop ����
	// StopTaskCounter Ŭ���� ������ ����� Thread Safety�ϰ� ����
	while (StopTaskCounter.GetValue() == 0)
	{
		stringstream RecvStream;

		int PacketType;
		int nRecvLen = recv(ServerSocket, (CHAR*)&recvBuffer, MAX_BUFFER, 0);

		if (nRecvLen > 0)
		{
			// ��Ŷ ó��
			RecvStream << recvBuffer;
			RecvStream >> PacketType;

			/////////////////////////////
			// �ʼ�!!!!: recvBuffer �ʱ�ȭ
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
	// thread safety ������ ������ while loop �� ���� ���ϰ� ��
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

	// ���� ������ 2.2�� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf_s("[ERROR] <cClientSocketInGame::InitSocket()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)\n");
		//UE_LOG(LogTemp, Error, TEXT("[ERROR] <cClientSocketInGame::InitSocket()> if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)"));
		return false;
	}

	// TCP ���� ����	
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

	// ������ ���� ������ ������ ����ü
	SOCKADDR_IN stServerAddr;

	stServerAddr.sin_family = AF_INET;
	// ������ ���� ��Ʈ �� IP
	stServerAddr.sin_port = htons(nPort);
	//stServerAddr.sin_addr.s_addr = inet_addr(pszIP); // Waring: C4996 �ɰ���	�ڵ�	����	������Ʈ	����	��	��ǥ�� ����(Suppression) ����, ��� C4996	'inet_addr': Use inet_pton() or InetPton() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings	Game
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

	// ������ ����
	Thread = FRunnableThread::Create(this, TEXT("cClientSocketInGame"), 0, TPri_BelowNormal);

	return (Thread != nullptr);
}

void cClientSocketInGame::StopListen()
{
	printf_s("[INFO] <cClientSocketInGame::StopListen()>\n");
	//UE_LOG(LogTemp, Warning, TEXT("[INFO] <cClientSocketInGame::StopListen()>"));

	// ������ ����
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
// ������ ���
/////////////////////////////////////
