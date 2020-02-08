#pragma once

#define WIN32_LEAN_AND_MEAN

#include "CoreMinimal.h"

////////////////////////////////////////////////////////////////
// 언리얼엔진도 Windows.h를 사용하기 때문에 naming 충돌을 피하기 위해
// AllowWindowsPlatformTypes.h과 prewindowsapi.h를 선언하고
// 윈도우즈 헤더들을 선언한 뒤에
// PostWindowsApi.h와 HideWindowsPlatformTypes.h를 선언해야 됩니다.
////////////////////////////////////////////////////////////////

// put this at the top of your .h file above #includes
// UE4: allow Windows platform types to avoid naming collisions
// must be undone at the bottom of this file!
#include "AllowWindowsPlatformTypes.h"
#include "prewindowsapi.h"


/*** 윈도우즈 헤더 선언 : Start ***/
#pragma comment(lib, "ws2_32.lib") // winsock2 사용을 위해 추가

#include <WinSock2.h>
#include <WS2tcpip.h> // For: inet_pron()
#include <iostream>
#include <map>
#include <queue>
/*** 윈도우즈 헤더 선언 : End ***/


// put this at the bottom of the .h file
// UE4: disallow windows platform types
// this was enabled at the top of the file
#include "PostWindowsApi.h"
#include "HideWindowsPlatformTypes.h"


#include "Runtime/Core/Public/HAL/Runnable.h"

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
	void RecvFindGames(stringstream& RecvStream);
	std::queue<stInfoOfGame> qRecvFindGames;
	CRITICAL_SECTION csRecvFindGames;
	bool GetRecvFindGames(stInfoOfGame& InfoOfGame);

	void SendModifyWaitingRoom(const FString Title, int Stage, int MaxOfNum);
	void RecvModifyWaitingRoom(stringstream& RecvStream);
	stInfoOfGame mRecvModifyWaitingRoom;
	CRITICAL_SECTION csRecvModifyWaitingRoom;
	bool GetRecvModifyWaitingRoom(stInfoOfGame& InfoOfGame);

	void SendJoinWaitingRoom(int SocketIDOfLeader);
	void RecvJoinWaitingRoom(stringstream& RecvStream);
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
