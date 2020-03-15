#pragma once



#include "CoreMinimal.h"

#include "Runtime/Core/Public/HAL/Runnable.h"

#include "Packet.h"


/**
 * ���� ������ ���� �� ��Ŷ ó���� ����ϴ� Ŭ���� (���� Ŭ���̾�Ʈ)
 */
class GAME_API cClientSocket : public FRunnable
{
private:
	SOCKET	ServerSocket;				// ������ ������ ����	
	

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
	// FRunnable override �Լ�
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

	// ���� ��� �� ����
	bool InitSocket();

	// ������ ����
	bool Connect(const char * pszIP, int nPort);

	// ���� ����
	void CloseSocket();

	// �۽�
	void Send(stringstream& SendStream);

	///////////////////////////////////////////
	// ��Ŷ�� ó���մϴ�.
	///////////////////////////////////////////
	void ProcessReceivedPacket(char* DataBuffer);

	// (�ӽ�) ��Ŷ �ϳ��� �߸� ���� ���۵Ǵ� ��� �ٷ� ���� 
	// �߷����� ��� ���⼭ ������ �߻��� �� �־ �����ؾ� ��!
	bool ProcessDirectly(char* RecvBuffer, int RecvLen);

	///////////////////////////////////////////
	// recvQueue�� ������ �����͸� ����
	///////////////////////////////////////////
	void PushRecvBufferInQueue(char* RecvBuffer, int RecvLen);

	///////////////////////////////////////////
	// ������ �����͸� �����ϴ� ť���� �����͸� ȹ��
	///////////////////////////////////////////
	void GetDataInRecvQueue(char* DataBuffer);

	// ������ ���� �� ����
	bool StartListen();
	void StopListen();

	// �̱��� ��ü ��������
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
