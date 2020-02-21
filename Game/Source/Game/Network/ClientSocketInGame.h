// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"

#include "Runtime/Core/Public/HAL/Runnable.h"

#include "Packet.h"

/**
 * 게임 서버와 접속 및 패킷 처리를 담당하는 클래스 (게임 클라이언트)
 */
class GAME_API cClientSocketInGame : public FRunnable 
{
private:
	SOCKET	ServerSocket;				// 서버와 연결할 소켓	
	char 	recvBuffer[MAX_BUFFER];		// 수신 버퍼 스트림	

	// FRunnable Thread members	
	FRunnableThread* Thread;
	FThreadSafeCounter StopTaskCounter;

	bool bIsConnected;

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
	cClientSocketInGame();
	virtual ~cClientSocketInGame();

	// 소켓 등록 및 설정
	bool InitSocket();

	// 서버와 연결
	bool Connect(const char * pszIP, int nPort);

	// 소켓 종료
	void CloseSocket();

	// 스레드 시작 및 종료
	bool StartListen();
	void StopListen();

	// 싱글턴 객체 가져오기
	static cClientSocketInGame* GetSingleton()
	{
		static cClientSocketInGame ins;
		return &ins;
	}

	void SetConnected(bool bConnected) { bIsConnected = bConnected; }
	bool IsConnected() { return bIsConnected; }
	/////////////////////////////////////
	// 서버와 통신
	/////////////////////////////////////
};
