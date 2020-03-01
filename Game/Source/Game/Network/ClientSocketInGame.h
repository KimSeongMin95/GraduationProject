// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"

#include "Packet.h"


/**
 * 게임 서버와 접속 및 패킷 처리를 담당하는 클래스 (게임 클라이언트)
 */
class GAME_API cClientSocketInGame
{
private:
	SOCKET	ServerSocket;			// 서버와 연결할 소켓	
	char 	recvBuffer[MAX_BUFFER];	// 수신 버퍼 스트림	

	bool	bAccept;				// 요청 동작 플래그 (메인 스레드)
	CRITICAL_SECTION csAccept;		// 크리티컬 섹션
	HANDLE	hMainHandle;			// 메인 스레드 핸들	

	bool bIsInitialized;
	bool bIsConnected;
	bool bIsClientSocketOn;

protected:
	

public:
	/////////////////////////////////////
	// cClientSocket
	/////////////////////////////////////
	cClientSocketInGame();
	~cClientSocketInGame();

	// 소켓 등록 및 설정
	bool InitSocket();

	// 서버와 연결
	bool Connect(const char * pszIP, int nPort);

	// 소켓 종료
	void CloseSocket();

	// 스레드 시작 및 종료
	bool BeginMainThread();
	void RunMainThread();


	// 싱글턴 객체 가져오기
	static cClientSocketInGame* GetSingleton()
	{
		static cClientSocketInGame ins;
		return &ins;
	}

	bool IsInitialized() { return bIsInitialized; }
	bool IsConnected() { return bIsConnected; }
	bool IsClientSocketOn() { return bIsClientSocketOn; }

	/////////////////////////////////////
	// 서버와 통신
	/////////////////////////////////////
};
