// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** �𸮾��� ��� ���� : Start ***/
#include "DateTime.h"
/*** �𸮾��� ��� ���� : End ***/

#include "Packet.h"

#include "CoreMinimal.h"


/**
 * ���� ������ ���� �� ��Ŷ ó���� ����ϴ� Ŭ���� (���� Ŭ���̾�Ʈ)
 */
class GAME_API cClientSocketInGame
{
private:
	SOCKET	ServerSocket;			// ������ ������ ����	

	bool	bAccept;				// ��û ���� �÷��� (���� ������)
	CRITICAL_SECTION csAccept;		// ũ��Ƽ�� ����
	HANDLE	hMainHandle;			// ���� ������ �ڵ�	

	bool bIsInitialized;
	bool bIsConnected;
	bool bIsClientSocketOn;

	class cClientSocket* ClientSocket = nullptr;

	deque<char*> RecvDeque;

	bool bServerOn;
	CRITICAL_SECTION csServerOn;

	class cInfoOfScoreBoard MyInfoOfScoreBoard;
	CRITICAL_SECTION csMyInfoOfScoreBoard;

	// Ping �ð� ����
	FDateTime StartTime;
	int Ping;
	CRITICAL_SECTION csPing;

	int PossessedID;

protected:
	

public:
	/////////////////////////////////////
	// cClientSocket
	/////////////////////////////////////
	cClientSocketInGame();
	~cClientSocketInGame();

	// ���� ��� �� ����
	bool InitSocket();

	// ������ ����
	bool Connect(const char * pszIP, int nPort);

	// ���� ����
	void CloseSocket();
	
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
	bool BeginMainThread();
	void RunMainThread();

	// �̱��� ��ü ��������
	static cClientSocketInGame* GetSingleton()
	{
		static cClientSocketInGame ins;
		return &ins;
	}

	bool IsInitialized() { return bIsInitialized; }
	bool IsConnected() { return bIsConnected; }
	bool IsClientSocketOn() { return bIsClientSocketOn; }

	bool IsServerOn();

	/////////////////////////////////////
	// Game Server / Game Clients
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
	cThreadSafetyQueue<int> tsqDiedPioneer; // ID ����?

	void SendInfoOfPioneer_Animation(class APioneer* PioneerOfPlayer);
	void RecvInfoOfPioneer_Animation(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfPioneer_Animation> tsqInfoOfPioneer_Animation;

	void SendPossessPioneer(cInfoOfPioneer_Socket Socket);
	void RecvPossessPioneer(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfPioneer_Socket> tsqPossessPioneer;

	void RecvInfoOfPioneer_Socket(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfPioneer_Socket> tsqInfoOfPioneer_Socket;

	void SendInfoOfPioneer_Stat(class APioneer* PioneerOfPlayer);
	void RecvInfoOfPioneer_Stat(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfPioneer_Stat> tsqInfoOfPioneer_Stat;

	void SendInfoOfProjectile(cInfoOfProjectile InfoOfProjectile);
	void RecvInfoOfProjectile(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfProjectile> tsqInfoOfProjectile;

	void RecvInfoOfResources(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfResources> tsqInfoOfResources;

	void SendInfoOfBuilding_Spawn(cInfoOfBuilding_Spawn InfoOfBuilding_Spawn);
	void RecvInfoOfBuilding_Spawn(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfBuilding_Spawn> tsqInfoOfBuilding_Spawn;

	/////////////////////////////////////
	// Set-Get
	/////////////////////////////////////
	void SetMyInfoOfScoreBoard(cInfoOfScoreBoard& InfoOfScoreBoard);
	cInfoOfScoreBoard CopyMyInfoOfScoreBoard();
	void InitMyInfoOfScoreBoard();




	////////////////////////////////////////////////
	// (�ӽ�) ��Ŷ ������� ���� ���� ������ �Լ�
	////////////////////////////////////////////////
	void VerifyPacket(char* DataBuffer, bool send)
	{
		if (!DataBuffer)
		{
			printf_s("[ERROR] <cClientSocketInGame::VerifyPacket(...)> if (!DataBuffer) \n");
			return;
		}

		int len = (int)strlen(DataBuffer);

		if (len < 4)
		{
			printf_s("[ERROR] <cClientSocketInGame::VerifyPacket(...)> if (len < 4) \n");
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
