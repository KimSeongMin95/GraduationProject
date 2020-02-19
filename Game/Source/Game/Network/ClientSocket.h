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


template <typename T>
class GAME_API cThreadSafeQueue
{
private:
	std::queue<T> q;
	CRITICAL_SECTION cs;

public:
	cThreadSafeQueue() { InitializeCriticalSection(&cs); }
	~cThreadSafeQueue() { DeleteCriticalSection(&cs); }

	bool empty()
	{
		EnterCriticalSection(&cs);
		bool result = q.empty();
		LeaveCriticalSection(&cs);
		return result;
	}

	void push(T element)
	{
		EnterCriticalSection(&cs);
		q.push(element);
		LeaveCriticalSection(&cs);
	}

	T front()
	{
		EnterCriticalSection(&cs);
		T result = q.front();
		LeaveCriticalSection(&cs);
		return result;
	}

	void pop()
	{
		EnterCriticalSection(&cs);
		q.pop();
		LeaveCriticalSection(&cs);
	}

	T front_pop()
	{
		EnterCriticalSection(&cs);
		T result = q.front();
		q.pop();
		LeaveCriticalSection(&cs);
		return result;
	}

	T back()
	{
		EnterCriticalSection(&cs);
		T result = q.back();
		LeaveCriticalSection(&cs);
		return result;
	}

	void clear()
	{
		EnterCriticalSection(&cs);
		while (q.empty() == false)
			q.pop();
		LeaveCriticalSection(&cs);
	}

	size_t size()
	{
		EnterCriticalSection(&cs);
		size_t result = q.size();
		LeaveCriticalSection(&cs);
		return result;
	}

	std::queue<T> copy()
	{
		EnterCriticalSection(&cs);
		std::queue<T> copyQ = q;
		LeaveCriticalSection(&cs);
		return copyQ;
	}
};


/**
 * ������ ���� �� ��Ŷ ó���� ����ϴ� Ŭ����
 */
class GAME_API cClientSocket : public FRunnable
{
private:
	SOCKET	ServerSocket;				// ������ ������ ����	
	char 	recvBuffer[MAX_BUFFER];		// ���� ���� ��Ʈ��	

	// FRunnable Thread members	
	FRunnableThread* Thread;
	FThreadSafeCounter StopTaskCounter;


	class cInfoOfPlayer MyInfo;
	CRITICAL_SECTION csMyInfo;

	class cInfoOfGame MyInfoOfGame;
	CRITICAL_SECTION csMyInfoOfGame;

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
	cClientSocket();
	virtual ~cClientSocket();

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
	static cClientSocket* GetSingleton()
	{
		static cClientSocket ins;
		return &ins;
	}

	/////////////////////////////////////
	// ������ ���
	/////////////////////////////////////
	void SendLogin(const FText ID);
	void RecvLogin(stringstream& RecvStream);

	void SendCreateGame();

	void SendFindGames();
	void RecvFindGames(stringstream& RecvStream);
	cThreadSafeQueue<cInfoOfGame> tsqFindGames;

	void SendJoinWaitingGame(int SocketIDOfLeader);

	void RecvWaitingGame(stringstream& RecvStream);
	cThreadSafeQueue<cInfoOfGame> tsqWaitingGame;

	void SendDestroyWaitingGame();
	void RecvDestroyWaitingGame(stringstream& RecvStream);
	cThreadSafeQueue<bool> tsqDestroyWaitingGame;

	void SendExitWaitingGame();

	void SendModifyWaitingGame();
	void RecvModifyWaitingGame(stringstream& RecvStream);
	cThreadSafeQueue<cInfoOfGame> tsqModifyWaitingGame;

	void SendStartWaitingGame();
	void RecvStartWaitingGame(stringstream& RecvStream);
	cThreadSafeQueue<bool> tsqStartWaitingGame;
	/*


	bool GetRecvFindGames(stInfoOfGame& InfoOfGame);

	void SendModifyWaitingRoom(const FString Title, int Stage, int MaxOfNum);
	void RecvModifyWaitingRoom(stringstream& RecvStream);
	stInfoOfGame mRecvModifyWaitingRoom;
	CRITICAL_SECTION csRecvModifyWaitingRoom;
	bool GetRecvModifyWaitingRoom(stInfoOfGame& InfoOfGame);

	void SendJoinWaitingRoom(int SocketIDOfLeader);
	void RecvJoinWaitingRoom(stringstream& RecvStream);
	stInfoOfGame mRecvJoinWaitingRoom;
	CRITICAL_SECTION csRecvJoinWaitingRoom;
	bool GetRecvJoinWaitingRoom(stInfoOfGame& InfoOfGame);

	void RecvPlayerJoinedWaitingRoom(stringstream& RecvStream);
	std::queue<int> qRecvPlayerJoinedWaitingRoom;
	CRITICAL_SECTION csRecvPlayerJoinedWaitingRoom;
	bool GetRecvPlayerJoinedWaitingRoom(std::queue<int>& qSocketID);

	void SendExitWaitingRoom(int SocketIDOfLeader);

	void RecvPlayerExitedWaitingRoom(stringstream& RecvStream);
	std::queue<int> qRecvPlayerExitedWaitingRoom;
	CRITICAL_SECTION csRecvPlayerExitedWaitingRoom;
	bool GetRecvPlayerExitedWaitingRoom(std::queue<int>& qSocketID);

	void SendCheckPlayerInWaitingRoom(int SocketIDOfLeader, std::queue<int>& qSocketID);
	void RecvCheckPlayerInWaitingRoom(stringstream& RecvStream);
	std::queue<int> qRecvCheckPlayerInWaitingRoom;
	CRITICAL_SECTION csRecvCheckPlayerInWaitingRoom;
	bool GetRecvCheckPlayerInWaitingRoom(std::queue<int>& qSocketID);

	*/

	/////////////////////////////////////
	// Set-Get
	/////////////////////////////////////
	void SetMyInfo(cInfoOfPlayer& InfoOfPlayer);
	cInfoOfPlayer CopyMyInfo();
	void InitMyInfo();

	void SetMyInfoOfGame(cInfoOfGame& InfoOfGame);
	cInfoOfGame CopyMyInfoOfGame();
	void InitMyInfoOfGame();
};
