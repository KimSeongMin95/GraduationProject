// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"

#include "Packet.h"


/**
 * ���� ������ ���� �� ��Ŷ ó���� ����ϴ� Ŭ���� (���� Ŭ���̾�Ʈ)
 */
class GAME_API cClientSocketInGame
{
private:
	SOCKET	ServerSocket;			// ������ ������ ����	
	char 	recvBuffer[MAX_BUFFER];	// ���� ���� ��Ʈ��	

	bool	bAccept;				// ��û ���� �÷��� (���� ������)
	CRITICAL_SECTION csAccept;		// ũ��Ƽ�� ����
	HANDLE	hMainHandle;			// ���� ������ �ڵ�	

	bool bIsInitialized;
	bool bIsConnected;
	bool bIsClientSocketOn;

	class cInfoOfScoreBoard MyInfoOfScoreBoard;
	CRITICAL_SECTION csMyInfoOfScoreBoard;

	class cClientSocket* ClientSocket = nullptr;

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
	// ������ ���
	/////////////////////////////////////
	void SendConnected();
	void RecvConnected(stringstream& RecvStream);

	void SendScoreBoard();
	void RecvScoreBoard(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfScoreBoard> tsqScoreBoard;


	/////////////////////////////////////
	// Set-Get
	/////////////////////////////////////
	void SetMyInfoOfScoreBoard(cInfoOfScoreBoard& InfoOfScoreBoard);
	cInfoOfScoreBoard CopyMyInfoOfScoreBoard();
	void InitMyInfoOfScoreBoard();
};
