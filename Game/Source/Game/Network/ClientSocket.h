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

	queue<char*> RecvQueue;

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
	// Basic Functions
	///////////////////////////////////////////
	void AddSizeInStream(stringstream& DataStream, stringstream& FinalStream);

	void SetSockOpt(SOCKET& Socket, int SendBuf, int RecvBuf);


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
	// OnlineGameMode
	/////////////////////////////////////




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
