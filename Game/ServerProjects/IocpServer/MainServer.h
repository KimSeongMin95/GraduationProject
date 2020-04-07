// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
class MainServer
{
private:
	FuncProcess	fnProcess[100];	// 패킷 처리 구조체

protected:
	SOCKET			 ListenSocket;			// 서버 리슨 소켓
	HANDLE			 hIOCP;					// IOCP 객체 핸들

	bool			 bAccept;				// 요청 동작 플래그
	CRITICAL_SECTION csAccept;				//
	HANDLE			 hAcceptThreadHandle;	// Accept 스레드 핸들	

	HANDLE*			 hIOThreadHandle;		// IO 스레드 핸들		
	DWORD			 nIOThreadCnt;			// IO 스레드 개수


public:
	// WSAAccept(...)한 모든 클라이언트의 new stCompletionKey()를 저장
	static map<SOCKET, stCompletionKey*> Clients;
	static CRITICAL_SECTION csClients;

	// 수신한 데이터를 덱에 전부 적재
	static map<SOCKET, deque<char*>*> MapOfRecvDeque;
	static CRITICAL_SECTION csMapOfRecvDeque;

	// WSASend(...)를 실행하면 ++, 실행이 완료되거나 실패하면 --
	static unsigned int CountOfSend;
	static CRITICAL_SECTION csCountOfSend;

	/******************************************************/

	// Login한 클라이언트의 InfoOfPlayer 저장
	static std::map<SOCKET, cInfoOfPlayer> InfoOfClients;
	static CRITICAL_SECTION csInfoOfClients;

	// CreateGame한 클라이언트의 cInfoOfGame 저장
	static std::map<SOCKET, cInfoOfGame> InfoOfGames;
	static CRITICAL_SECTION csInfoOfGames;

public:
	////////////////////////
	// 기본
	////////////////////////
	MainServer();
	~MainServer();

	// 
	void SetIPv4AndPort(IN_ADDR& IPv4, USHORT& Port);

	// 초기화 실패시 실행
	void CloseListenSocketAndCleanupWSA();

	// 소켓 등록 및 서버 정보 설정
	bool Initialize();

	// Accept 스레드 생성
	bool CreateAcceptThread();

	// 서버 시작
	void AcceptThread();

	// IO 스레드 생성
	bool CreateIOThread();

	// 작업 스레드
	void IOThread();

	// 클라이언트 접속 종료
	static void CloseSocket(SOCKET Socket, stOverlappedMsg* OverlappedMsg);

	// 서버 종료
	void CloseServer();

	// 클라이언트에게 송신
	static void Send(stringstream& SendStream, SOCKET Socket);

	// 클라이언트 수신 대기
	static void Recv(SOCKET Socket, stOverlappedMsg* ReceivedOverlappedMsg);

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

	////////////////////////////////////////////////
	// 대용량 패킷 분할 
	////////////////////////////////////////////////
	template<typename T>
	static void DivideHugePacket(SOCKET Socket, stringstream& SendStream, EPacketType PacketType, T& queue);

	////////////////////////////////////////////////
	// (임시) 패킷 사이즈와 실제 길이 검증용 함수
	////////////////////////////////////////////////
	static void VerifyPacket(char* DataBuffer, bool send);

	// 싱글턴 객체 가져오기
	static MainServer* GetSingleton()
	{
		static MainServer server;
		return &server;
	}


	////////////////////////
	// 확인
	////////////////////////
	bool IsServerOn();


	////////////////////////
	// Main Server / Min Clients
	////////////////////////
	static void Broadcast(stringstream& SendStream);
	static void BroadcastExceptOne(stringstream& SendStream, SOCKET Except);

	static void Login(stringstream& RecvStream, SOCKET Socket);

	static void CreateGame(stringstream& RecvStream, SOCKET Socket);

	static void FindGames(stringstream& RecvStream, SOCKET Socket);

	static void JoinOnlineGame(stringstream& RecvStream, SOCKET Socket);

	static void DestroyWaitingGame(stringstream& RecvStream, SOCKET Socket);

	static void ExitWaitingGame(stringstream& RecvStream, SOCKET Socket);

	static void ModifyWaitingGame(stringstream& RecvStream, SOCKET Socket);

	static void StartWaitingGame(stringstream& RecvStream, SOCKET Socket);

	///////////////////////////////////////////
	// Main Server / Game Server, Game Clients
	///////////////////////////////////////////
	static void ActivateGameServer(stringstream& RecvStream, SOCKET Socket);

	static void RequestInfoOfGameServer(stringstream& RecvStream, SOCKET Socket);
};
