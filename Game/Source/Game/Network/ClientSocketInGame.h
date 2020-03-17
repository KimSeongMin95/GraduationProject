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

	class cInfoOfScoreBoard MyInfoOfScoreBoard;
	CRITICAL_SECTION csMyInfoOfScoreBoard;

	// Ping �ð� ����
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

	void SendInfoOfPioneer(cInfoOfPioneer InfoOfPioneer);
	void RecvInfoOfPioneer(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfPioneer> tsqInfoOfPioneer;


	/////////////////////////////////////
	// Set-Get
	/////////////////////////////////////
	void SetMyInfoOfScoreBoard(cInfoOfScoreBoard& InfoOfScoreBoard);
	cInfoOfScoreBoard CopyMyInfoOfScoreBoard();
	void InitMyInfoOfScoreBoard();




	////////////////////////////////////////////////
	// (�ӽ�) ��Ŷ ������� ���� ���� ������ �Լ�
	////////////////////////////////////////////////
	static void PrintPacket(char* DataBuffer, bool send)
	{
		int len = strlen(DataBuffer);

		char cutBuffer[MAX_BUFFER + 1];
		CopyMemory(cutBuffer, DataBuffer, len);
		cutBuffer[len] = '\0';

		for (int i = 0; i < len; i++)
		{
			if (cutBuffer[i] == '\n')
				cutBuffer[i] = '_';
		}


		char sizeBuffer[5]; // [1234\0]
		CopyMemory(sizeBuffer, cutBuffer, 4); // �� 4�ڸ� �����͸� sizeBuffer�� �����մϴ�.
		sizeBuffer[4] = '\0';

		stringstream sizeStream;
		sizeStream << sizeBuffer;
		int sizeOfPacket = 0;
		sizeStream >> sizeOfPacket;


		if (sizeOfPacket == len)
		{
			printf_s("\n type: %s \n %s \n sizeOfPacket: %d \n len: %d \n", send ? "Send" : "Recv", cutBuffer, sizeOfPacket, len);
		}
		else
		{
			printf_s("\n\n\n\n\n\n\n\n\n\ntype: %s \n %s \n sizeOfPacket: %d \n len: %d \n\n\n\n\n\n\n\n\n\n\n", send ? "Send" : "Recv", cutBuffer, sizeOfPacket, len);
		}
	}
};
