// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Packet.h"


// 패킷 처리 함수 포인터
struct FuncProcess
{
	// RecvStream은 수신한 정보, pSocket은 Overlapped I/O 작업이 발생한 IOCP 소켓 구조체 정보
	void(*funcProcessPacket)(stringstream& RecvStream, stSOCKETINFO* pSocketInfo);
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
	stSOCKETINFO*	SocketInfo = nullptr;	// 소켓 정보
	SOCKET			ListenSocket;			// 서버 리슨 소켓
	HANDLE			hIOCP;					// IOCP 객체 핸들
	
	bool			bAccept;				// 요청 동작 플래그 (메인 스레드)
	HANDLE			hMainHandle;			// 메인 스레드 핸들	

	bool			bWorkerThread;			// 작업 스레드 동작 플래그 (워커 스레드)
	HANDLE*			hWorkerHandle = nullptr;// 작업 스레드 핸들		
	DWORD			nThreadCnt;				// 작업 스레드 개수

public:
	/** 게임서버의 임시 소켓 */
	SOCKET SocketID;

	// WSAAccept한 모든 클라이언트의 new stSOCKETINFO()를 저장
	static std::map<SOCKET, stSOCKETINFO*> GameClients;
	static CRITICAL_SECTION csGameClients;

	// Connected 클라이언트의 InfoOfPlayer 저장
	static std::map<SOCKET, cInfoOfPlayer> InfoOfClients;
	static CRITICAL_SECTION csInfoOfClients;

	// 
	static std::map<SOCKET, cInfoOfScoreBoard> InfosOfScoreBoard;
	static CRITICAL_SECTION csInfosOfScoreBoard;

	class cClientSocket* ClientSocket = nullptr;

	static cThreadSafetyQueue<SOCKET> tsqObserver;

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
	void CloseSocket(stSOCKETINFO* pSocketInfo);

	// 클라이언트에게 송신
	static void Send(stSOCKETINFO* pSocketInfo);

	// 클라이언트 수신 대기
	void Recv(stSOCKETINFO* pSocketInfo);

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
	static void Connected(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void ScoreBoard(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void Observation(stringstream& RecvStream, stSOCKETINFO* pSocket);
};
