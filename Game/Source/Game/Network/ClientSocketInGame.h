// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"

#include "Runtime/Core/Public/HAL/Runnable.h"

#include "Packet.h"

/**
 * ���� ������ ���� �� ��Ŷ ó���� ����ϴ� Ŭ���� (���� Ŭ���̾�Ʈ)
 */
class GAME_API cClientSocketInGame : public FRunnable 
{
private:
	SOCKET	ServerSocket;				// ������ ������ ����	
	char 	recvBuffer[MAX_BUFFER];		// ���� ���� ��Ʈ��	

	// FRunnable Thread members	
	FRunnableThread* Thread;
	FThreadSafeCounter StopTaskCounter;

	bool bIsConnected;

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
	cClientSocketInGame();
	virtual ~cClientSocketInGame();

	// ���� ��� �� ����
	bool InitSocket();

	// ������ ����
	bool Connect(const char * pszIP, int nPort);

	// ���� ����
	void CloseSocket();

	// ������ ���� �� ����
	bool StartListen();
	void StopListen();

	// �̱��� ��ü ��������
	static cClientSocketInGame* GetSingleton()
	{
		static cClientSocketInGame ins;
		return &ins;
	}

	void SetConnected(bool bConnected) { bIsConnected = bConnected; }
	bool IsConnected() { return bIsConnected; }
	/////////////////////////////////////
	// ������ ���
	/////////////////////////////////////
};
