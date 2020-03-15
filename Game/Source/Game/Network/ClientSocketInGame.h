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

	queue<char*> RecvQueue;

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

	///////////////////////////////////////////
	// Basic Functions
	///////////////////////////////////////////
	void AddSizeInStream(stringstream& DataStream, stringstream& FinalStream);

	void SetSockOpt(SOCKET& Socket, int SendBuf, int RecvBuf);


	/////////////////////////////////////
	// ������ ���
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
};
