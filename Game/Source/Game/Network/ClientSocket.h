#pragma once

#define WIN32_LEAN_AND_MEAN

#include "CoreMinimal.h"

// winsock2 사용을 위해 아래 코멘트 추가
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>
#include <string>

#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformAffinity.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"

#include "Packet.h"

using namespace std;

#define	MAX_BUFFER		4096
#define SERVER_PORT		8000
#define SERVER_IP		"127.0.0.1"
#define MAX_CLIENTS		1000

// 소켓 통신 구조체
struct stSOCKETINFO
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;
	SOCKET			socket;
	char			messageBuffer[MAX_BUFFER];
	int				recvBytes;
	int				sendBytes;
};

/**
 * 서버와 접속 및 패킷 처리를 담당하는 클래스
 */
class GAME_API ClientSocket : public FRunnable
{
public:
	ClientSocket();
	virtual ~ClientSocket();

	// 소켓 등록 및 설정
	bool InitSocket();

	// 서버와 연결
	bool Connect(const char * pszIP, int nPort);

	// 소켓 종료
	void CloseSocket();

	//////////////////////////////////////////////////////////////////////////
	// 서버와 통신
	//////////////////////////////////////////////////////////////////////////
	void SendAcceptPlayer();
	void RecvAcceptPlayer(stringstream& RecvStream);

	void SendCreateWaitingRoom(const FText State, const FText Title, int Stage, int MaxOfNum);

	void SendFindGames();
	stInfoOfGame RecvFindGames(stringstream& RecvStream);

	//////////////////////////////////////////////////////////////////////////	

	// 
	void SetMainScreenGameMode(class AMainScreenGameMode* pMainScreenGameMode);

	// FRunnable Thread members	
	FRunnableThread* Thread;
	FThreadSafeCounter StopTaskCounter;

	// FRunnable override 함수
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();

	// 스레드 시작 및 종료
	bool StartListen();
	void StopListen();

	// 싱글턴 객체 가져오기
	static ClientSocket* GetSingleton()
	{
		static ClientSocket ins;
		return &ins;
	}

private:
	SOCKET	ServerSocket;				// 서버와 연결할 소켓	
	char 	recvBuffer[MAX_BUFFER];		// 수신 버퍼 스트림	

	class AMainScreenGameMode* MainScreenGameMode = nullptr;

	int		SocketID;					// 서버에서의 해당 클라이언트 소켓 ID
};
