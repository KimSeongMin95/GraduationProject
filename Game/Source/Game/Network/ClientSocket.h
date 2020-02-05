#pragma once

#define WIN32_LEAN_AND_MEAN

#include "CoreMinimal.h"

// winsock2 ����� ���� �Ʒ� �ڸ�Ʈ �߰�
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
	stInfoOfGame RecvFindGames(stringstream& RecvStream);

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
