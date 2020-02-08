#pragma once

#define WIN32_LEAN_AND_MEAN

#include "CoreMinimal.h"

////////////////////////////////////////////////////////////////
// �𸮾����� Windows.h�� ����ϱ� ������ naming �浹�� ���ϱ� ����
// AllowWindowsPlatformTypes.h�� prewindowsapi.h�� �����ϰ�
// �������� ������� ������ �ڿ�
// PostWindowsApi.h�� HideWindowsPlatformTypes.h�� �����ؾ� �˴ϴ�.
////////////////////////////////////////////////////////////////

// put this at the top of your .h file above #includes
// UE4: allow Windows platform types to avoid naming collisions
// must be undone at the bottom of this file!
#include "AllowWindowsPlatformTypes.h"
#include "prewindowsapi.h"


/*** �������� ��� ���� : Start ***/
#pragma comment(lib, "ws2_32.lib") // winsock2 ����� ���� �߰�

#include <WinSock2.h>
#include <WS2tcpip.h> // For: inet_pron()
#include <iostream>
#include <map>
#include <queue>
/*** �������� ��� ���� : End ***/


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

// ���� ��� ����ü
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
 * ������ ���� �� ��Ŷ ó���� ����ϴ� Ŭ����
 */
class GAME_API ClientSocket : public FRunnable
{
public:
	ClientSocket();
	virtual ~ClientSocket();

	// ���� ��� �� ����
	bool InitSocket();

	// ������ ����
	bool Connect(const char * pszIP, int nPort);

	// ���� ����
	void CloseSocket();

	//////////////////////////////////////////////////////////////////////////
	// ������ ���
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

	// FRunnable override �Լ�
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();

	// ������ ���� �� ����
	bool StartListen();
	void StopListen();

	// �̱��� ��ü ��������
	static ClientSocket* GetSingleton()
	{
		static ClientSocket ins;
		return &ins;
	}

private:
	SOCKET	ServerSocket;				// ������ ������ ����	
	char 	recvBuffer[MAX_BUFFER];		// ���� ���� ��Ʈ��	

	class AMainScreenGameMode* MainScreenGameMode = nullptr;

	int		SocketID;					// ���������� �ش� Ŭ���̾�Ʈ ���� ID
};
