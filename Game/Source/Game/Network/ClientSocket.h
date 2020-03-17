#pragma once



#include "CoreMinimal.h"

#include "Runtime/Core/Public/HAL/Runnable.h"

#include "Packet.h"


/**
 * 메인 서버와 접속 및 패킷 처리를 담당하는 클래스 (메인 클라이언트)
 */
class GAME_API cClientSocket : public FRunnable
{
private:
	SOCKET	ServerSocket;				// 서버와 연결할 소켓	
	

	// FRunnable Thread members	
	FRunnableThread* Thread = nullptr;
	FThreadSafeCounter StopTaskCounter;

	bool bIsInitialized;
	bool bIsConnected;

	deque<char*> RecvDeque;

	class cInfoOfPlayer MyInfo;
	CRITICAL_SECTION csMyInfo;

	class cInfoOfGame MyInfoOfGame;
	CRITICAL_SECTION csMyInfoOfGame;

protected:
	/////////////////////////////////////
	// FRunnable override 함수
	/////////////////////////////////////
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();

public:
	/////////////////////////////////////
	// cClientSocket
	/////////////////////////////////////
	cClientSocket();
	~cClientSocket();

	// 소켓 등록 및 설정
	bool InitSocket();

	// 서버와 연결
	bool Connect(const char * pszIP, int nPort);

	// 소켓 종료
	void CloseSocket();

	// 송신
	void Send(stringstream& SendStream);


	///////////////////////////////////////////
	// 소켓 버퍼 크기 변경
	///////////////////////////////////////////
	void SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf);

	///////////////////////////////////////////
	// stringstream의 맨 앞에 size를 추가
	///////////////////////////////////////////
	bool AddSizeInStream(stringstream& DataStream, stringstream& FinalStream);

	///////////////////////////////////////////
	// recvDeque에 수신한 데이터를 적재
	///////////////////////////////////////////
	void PushRecvBufferInDeque(char* RecvBuffer, int RecvLen);

	///////////////////////////////////////////
	// 수신한 데이터를 저장하는 덱에서 데이터를 획득
	///////////////////////////////////////////
	void GetDataInRecvDeque(char* DataBuffer);

	///////////////////////////////////////////
	// 패킷을 처리합니다.
	///////////////////////////////////////////
	void ProcessReceivedPacket(char* DataBuffer);


	// 스레드 시작 및 종료
	bool StartListen();
	void StopListen();

	// 싱글턴 객체 가져오기
	static cClientSocket* GetSingleton()
	{
		static cClientSocket ins;
		return &ins;
	}

	bool IsInitialized() { return bIsInitialized; }
	bool IsConnected() { return bIsConnected; }


	///////////////////////////////////////////
	// Main Server / Main Clients
	///////////////////////////////////////////
	void SendLogin(const FText ID);
	void RecvLogin(stringstream& RecvStream);

	void SendCreateGame();

	void SendFindGames();
	void RecvFindGames(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfGame> tsqFindGames;

	void SendJoinOnlineGame(int SocketIDOfLeader);

	void RecvWaitingGame(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfGame> tsqWaitingGame;

	void SendDestroyWaitingGame();
	void RecvDestroyWaitingGame(stringstream& RecvStream);
	cThreadSafetyQueue<bool> tsqDestroyWaitingGame;

	void SendExitWaitingGame();

	void SendModifyWaitingGame();
	void RecvModifyWaitingGame(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfGame> tsqModifyWaitingGame;

	void SendStartWaitingGame();
	void RecvStartWaitingGame(stringstream& RecvStream);
	cThreadSafetyQueue<bool> tsqStartWaitingGame;


	///////////////////////////////////////////
	// Game Server / Game Clients
	///////////////////////////////////////////
	void SendActivateGameServer(int PortOfGameServer);

	void SendRequestInfoOfGameServer();
	void RecvRequestInfoOfGameServer(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfPlayer> tsqRequestInfoOfGameServer;


	/////////////////////////////////////
	// Set-Get
	/////////////////////////////////////
	void SetMyInfo(cInfoOfPlayer& InfoOfPlayer);
	cInfoOfPlayer CopyMyInfo();
	void InitMyInfo();

	void SetMyInfoOfGame(cInfoOfGame& InfoOfGame);
	cInfoOfGame CopyMyInfoOfGame();
	void InitMyInfoOfGame();
};
