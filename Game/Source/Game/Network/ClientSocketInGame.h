// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** 언리얼엔진 헤더 선언 : Start ***/
#include "DateTime.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "Packet.h"

#include "CoreMinimal.h"


/**
 * 게임 서버와 접속 및 패킷 처리를 담당하는 클래스 (게임 클라이언트)
 */
class GAME_API cClientSocketInGame
{
private:
	SOCKET	ServerSocket;			// 서버와 연결할 소켓	

	bool	bAccept;				// 요청 동작 플래그 (메인 스레드)
	CRITICAL_SECTION csAccept;		// 크리티컬 섹션
	HANDLE	hMainHandle;			// 메인 스레드 핸들	

	bool bIsInitialized;
	bool bIsConnected;
	bool bIsClientSocketOn;

	class cClientSocket* ClientSocket = nullptr;

	queue<char*> RecvQueue;

	class cInfoOfScoreBoard MyInfoOfScoreBoard;
	CRITICAL_SECTION csMyInfoOfScoreBoard;

	// Ping 시간 측정
	FDateTime StartTime;
	int Ping;
	CRITICAL_SECTION csPing;

protected:
	

public:
	/////////////////////////////////////
	// cClientSocket
	/////////////////////////////////////
	cClientSocketInGame();
	~cClientSocketInGame();

	// 소켓 등록 및 설정
	bool InitSocket();

	// 서버와 연결
	bool Connect(const char * pszIP, int nPort);

	// 소켓 종료
	void CloseSocket();
	
	// 송신
	void Send(stringstream& SendStream);

	///////////////////////////////////////////
	// 패킷을 처리합니다.
	///////////////////////////////////////////
	void ProcessReceivedPacket(char* DataBuffer);

	// (임시) 패킷 하나만 잘림 없이 전송되는 경우 바로 실행 
	// 잘려오는 경우 여기서 에러가 발생할 수 있어서 조심해야 함!
	bool ProcessDirectly(char* RecvBuffer, int RecvLen);

	///////////////////////////////////////////
	// recvQueue에 수신한 데이터를 적재
	///////////////////////////////////////////
	void PushRecvBufferInQueue(char* RecvBuffer, int RecvLen);

	///////////////////////////////////////////
	// 수신한 데이터를 저장하는 큐에서 데이터를 획득
	///////////////////////////////////////////
	void GetDataInRecvQueue(char* DataBuffer);

	// 스레드 시작 및 종료
	bool BeginMainThread();
	void RunMainThread();

	// 싱글턴 객체 가져오기
	static cClientSocketInGame* GetSingleton()
	{
		static cClientSocketInGame ins;
		return &ins;
	}

	bool IsInitialized() { return bIsInitialized; }
	bool IsConnected() { return bIsConnected; }
	bool IsClientSocketOn() { return bIsClientSocketOn; }

	///////////////////////////////////////////
	// Basic Functions
	///////////////////////////////////////////
	void AddSizeInStream(stringstream& DataStream, stringstream& FinalStream);

	void SetSockOpt(SOCKET& Socket, int SendBuf, int RecvBuf);


	/////////////////////////////////////
	// 서버와 통신
	/////////////////////////////////////
	void SendConnected();
	void RecvConnected(stringstream& RecvStream);

	void SendScoreBoard();
	void RecvScoreBoard(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfScoreBoard> tsqScoreBoard;

	void RecvSpaceShip(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfSpaceShip> tsqSpaceShip;

	void SendObservation();

	void RecvSpawnPioneer(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfPioneer> tsqSpawnPioneer;

	void SendDiedPioneer(int ID);
	void RecvDiedPioneer(stringstream& RecvStream);
	cThreadSafetyQueue<int> tsqDiedPioneer; // ID 저장?

	void SendInfoOfPioneer(cInfoOfPioneer InfoOfPioneer);
	void RecvInfoOfPioneer(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfPioneer> tsqInfoOfPioneer;


	/////////////////////////////////////
	// Set-Get
	/////////////////////////////////////
	void SetMyInfoOfScoreBoard(cInfoOfScoreBoard& InfoOfScoreBoard);
	cInfoOfScoreBoard CopyMyInfoOfScoreBoard();
	void InitMyInfoOfScoreBoard();
};
