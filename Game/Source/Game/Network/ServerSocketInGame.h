// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Packet.h"


// 패킷 처리 함수 포인터
struct FuncProcess
{
	// RecvStream은 수신한 정보, pSocket은 Overlapped I/O 작업이 발생한 IOCP 소켓 구조체 정보
	void(*funcProcessPacket)(stringstream& RecvStream, SOCKET Socket);
	FuncProcess()
	{
		funcProcessPacket = nullptr;
	}
};

/**
 * 게임 클라이언트와 접속 및 패킷 처리를 담당하는 클래스 (게임 서버)
 */
class GAME_API cServerSocketInGame
{
private:
	FuncProcess	fnProcess[100];	// 패킷 처리 구조체

	static int ServerPort;
	bool bIsServerOn;

protected:
	stSOCKETINFO*	 SocketInfo = nullptr;	  // 소켓 정보
	SOCKET			 ListenSocket;			  // 서버 리슨 소켓
	HANDLE			 hIOCP;					  // IOCP 객체 핸들
	
	bool			 bAccept;				  // 요청 동작 플래그 (메인 스레드)
	CRITICAL_SECTION csAccept;
	HANDLE			 hMainHandle;			  // 메인 스레드 핸들	

	bool			 bWorkerThread;			  // 작업 스레드 동작 플래그 (워커 스레드)
	HANDLE*			 hWorkerHandle = nullptr; // 작업 스레드 핸들		
	DWORD			 nThreadCnt;			  // 작업 스레드 개수

	//HANDLE			 hBroadcastHandle;			  //  브로드캐스팅 전용 핸들	

public:
	/** 게임서버의 임시 소켓 */
	SOCKET SocketID;

	class cClientSocket* ClientSocket = nullptr;

	// WSAAccept한 모든 클라이언트의 new stSOCKETINFO()를 저장, (서버가 닫힐 때 할당 해제용도)
	static map<SOCKET, stSOCKETINFO*> GC_SocketInfo;
	static CRITICAL_SECTION csGC_SocketInfo;

	// WSAAccept한 모든 클라이언트의 new stSOCKETINFO()를 저장, (delete 금지)
	static std::map<SOCKET, stSOCKETINFO*> GameClients;
	static CRITICAL_SECTION csGameClients;

	// 수신한 데이터를 덱에 전부 적재
	static map<SOCKET, deque<char*>*> MapOfRecvDeque;
	static CRITICAL_SECTION csMapOfRecvDeque;


	static unsigned int CountOfSend;
	static CRITICAL_SECTION csCountOfSend;


	// Connected 클라이언트의 InfoOfPlayer 저장
	static std::map<SOCKET, cInfoOfPlayer> InfoOfClients;
	static CRITICAL_SECTION csInfoOfClients;

	static std::map<SOCKET, cInfoOfScoreBoard> InfosOfScoreBoard;
	static CRITICAL_SECTION csInfosOfScoreBoard;

	static std::map<SOCKET, SOCKET> Observers;
	static CRITICAL_SECTION csObservers;


	///////////////////////////////////////////
	// Pioneer 세분화
	///////////////////////////////////////////
	static std::map<int, cInfoOfPioneer_Socket> InfosOfPioneer_Socket;
	static CRITICAL_SECTION csInfosOfPioneer_Socket;

	static std::map<int, cInfoOfPioneer_Animation> InfosOfPioneer_Animation;
	static CRITICAL_SECTION csInfosOfPioneer_Animation;

	static std::map<int, cInfoOfPioneer_Stat> InfosOfPioneer_Stat;
	static CRITICAL_SECTION csInfosOfPioneer_Stat;




public:
	////////////////////////
	// 기본
	////////////////////////
	cServerSocketInGame();
	~cServerSocketInGame();

	// 소켓 등록 및 서버 정보 설정
	bool Initialize();

	// 서버 시작
	void StartServer();

	// 서버 종료
	void CloseServer();

	// 작업 스레드 생성
	bool CreateWorkerThread();

	// 작업 스레드
	void WorkerThread();

	// 클라이언트 접속 종료
	static void CloseSocket(SOCKET Socket);

	// 클라이언트에게 송신
	static void Send(stringstream& SendStream, SOCKET Socket);

	// 클라이언트 수신 대기
	static void Recv(SOCKET Socket);

	///////////////////////////////////////////
	// stringstream의 맨 앞에 size를 추가
	///////////////////////////////////////////
	static bool AddSizeInStream(stringstream& DataStream, stringstream& FinalStream);

	///////////////////////////////////////////
	// 소켓 버퍼 크기 변경
	///////////////////////////////////////////
	void SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf);

	///////////////////////////////////////////
	// 수신한 데이터를 저장하는 덱에서 데이터를 획득
	///////////////////////////////////////////
	void GetDataInRecvDeque(deque<char*>* RecvDeque, char* DataBuffer);

	///////////////////////////////////////////
	// 패킷을 처리합니다.
	///////////////////////////////////////////
	void ProcessReceivedPacket(char* DataBuffer, SOCKET Socket);


	// 싱글턴 객체 가져오기
	static cServerSocketInGame* GetSingleton()
	{
		static cServerSocketInGame ins;
		return &ins;
	}


	////////////////////////
	// 확인
	////////////////////////
	bool IsServerOn() { return bIsServerOn; }
	int GetServerPort() { return ServerPort; }


	////////////////////////
	// 통신
	////////////////////////
	static void Broadcast(stringstream& SendStream);
	static void BroadcastExceptOne(stringstream& SendStream, SOCKET Except);

	static void Connected(stringstream& RecvStream, SOCKET Socket);

	static void ScoreBoard(stringstream& RecvStream, SOCKET Socket);

	static void SendSpaceShip(cInfoOfSpaceShip InfoOfSpaceShip);

	static void Observation(stringstream& RecvStream, SOCKET Socket);
	static int SizeOfObservers();
	static void InsertAtObersers(SOCKET Socket);

	static void SendSpawnPioneer(cInfoOfPioneer InfoOfPioneer);
	static void SendSpawnedPioneer(SOCKET Socket);

	static void DiedPioneer(stringstream& RecvStream, SOCKET Socket);
	static cThreadSafetyQueue<int> tsqDiedPioneer;

	static void InfoOfPioneer_Animation(stringstream& RecvStream, SOCKET Socket);
	static cThreadSafetyQueue<cInfoOfPioneer_Animation> tsqInfoOfPioneer_Animation;

	static void PossessPioneer(stringstream& RecvStream, SOCKET Socket);
	bool PossessingPioneer(cInfoOfPioneer_Socket Socket);
	static cThreadSafetyQueue<cInfoOfPioneer_Socket> tsqInfoOfPioneer_Socket;

	////////////////////////////////////////////////
	// (임시) 패킷 사이즈와 실제 길이 검증용 함수
	////////////////////////////////////////////////
	static void VerifyPacket(char* DataBuffer, bool send)
	{
		if (!DataBuffer)
		{
			printf_s("[ERROR] <cServerSocketInGame::VerifyPacket(...)> if (!DataBuffer) \n");
			return;
		}

		int len = (int)strlen(DataBuffer);

		if (len < 4)
		{
			printf_s("[ERROR] <cServerSocketInGame::VerifyPacket(...)> if (len < 4) \n");
			return;
		}

		char buffer[MAX_BUFFER + 1];
		CopyMemory(buffer, DataBuffer, len);
		buffer[len] = '\0';

		for (int i = 0; i < len; i++)
		{
			if (buffer[i] == '\n')
				buffer[i] = '_';
		}

		char sizeBuffer[5]; // [1234\0]
		CopyMemory(sizeBuffer, buffer, 4); // 앞 4자리 데이터만 sizeBuffer에 복사합니다.
		sizeBuffer[4] = '\0';

		stringstream sizeStream;
		sizeStream << sizeBuffer;
		int sizeOfPacket = 0;
		sizeStream >> sizeOfPacket;

		if (sizeOfPacket != len)
		{
			printf_s("\n\n\n\n\n\n\n\n\n\n type: %s \n packet: %s \n sizeOfPacket: %d \n len: %d \n\n\n\n\n\n\n\n\n\n\n", send ? "Send" : "Recv", buffer, sizeOfPacket, len);
		}
	}
};
