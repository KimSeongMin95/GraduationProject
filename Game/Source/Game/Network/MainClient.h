#pragma once



#include "CoreMinimal.h"

#include "Runtime/Core/Public/HAL/Runnable.h"

#include "Packet.h"


/**
 * ���� ������ ���� �� ��Ŷ ó���� ����ϴ� Ŭ���� (���� Ŭ���̾�Ʈ)
 */
class GAME_API cMainClient : public FRunnable
{
private:
	SOCKET	ServerSocket;				// ������ ������ ����	
	

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
	// FRunnable override �Լ�
	/////////////////////////////////////
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();

public:
	/////////////////////////////////////
	// cMainClient
	/////////////////////////////////////
	cMainClient();
	~cMainClient();

	// ���� ��� �� ����
	bool Initialize();

	// ������ ����
	bool Connect(const char * pszIP, int nPort);

	// ���� ����
	void Close();

	// �۽�
	void Send(stringstream& SendStream);


	///////////////////////////////////////////
	// ���� ���� ũ�� ����
	///////////////////////////////////////////
	void SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf);

	///////////////////////////////////////////
	// stringstream�� �� �տ� size�� �߰�
	///////////////////////////////////////////
	bool AddSizeInStream(stringstream& DataStream, stringstream& FinalStream);

	///////////////////////////////////////////
	// recvDeque�� ������ �����͸� ����
	///////////////////////////////////////////
	void PushRecvBufferInDeque(char* RecvBuffer, int RecvLen);

	///////////////////////////////////////////
	// ������ �����͸� �����ϴ� ������ �����͸� ȹ��
	///////////////////////////////////////////
	void GetDataInRecvDeque(char* DataBuffer);

	///////////////////////////////////////////
	// ��Ŷ�� ó���մϴ�.
	///////////////////////////////////////////
	void ProcessReceivedPacket(char* DataBuffer);


	// ������ ���� �� ����
	bool StartListen();
	void StopListen();

	// �̱��� ��ü ��������
	static cMainClient* GetSingleton()
	{
		static cMainClient mainClient;
		return &mainClient;
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




	////////////////////////////////////////////////
	// (�ӽ�) ��Ŷ ������� ���� ���� ������ �Լ�
	////////////////////////////////////////////////
	void VerifyPacket(char* DataBuffer, bool send)
	{
		if (!DataBuffer)
		{
			printf_s("[ERROR] <cMainClient::VerifyPacket(...)> if (!DataBuffer) \n");
			return;
		}

		int len = (int)strlen(DataBuffer);

		if (len < 4)
		{
			printf_s("[ERROR] <cMainClient::VerifyPacket(...)> if (len < 4) \n");
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
		CopyMemory(sizeBuffer, buffer, 4); // �� 4�ڸ� �����͸� sizeBuffer�� �����մϴ�.
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
