#pragma once

#define WIN32_LEAN_AND_MEAN

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
#include <WS2tcpip.h> // For: inet_pton()
#include <process.h>
#include <iostream>

#include <map>
#include <queue>
#include <deque>

#include <sstream>
#include <algorithm>
#include <string>

#include <stdarg.h> // For: �������� �Լ�
/*** �������� ��� ���� : End ***/


// put this at the bottom of the .h file
// UE4: disallow windows platform types
// this was enabled at the top of the file
#include "PostWindowsApi.h"
#include "HideWindowsPlatformTypes.h"


using namespace std;

#define	MAX_BUFFER 4096


/*** Console for log : Start ***/
class cMyConsole
{
private:
	FILE* fp_console = nullptr;

public:
	void AllocConsole()
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME

		// �̹� �Ҵ�Ǿ� ������ �ܼ��� �� �Ҵ����� �ʽ��ϴ�.
		if (fp_console)
			return;

		if (::AllocConsole())
		{
			freopen_s(&fp_console, "CONOUT$", "w", stdout);

			//CheckBuildConriguration();
		}
#endif
	}

	void FreeConsole()
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		// �̹� �Ҵ�Ǿ� ���� ���� �Ҹ��ŵ�ϴ�.
		if (fp_console)
		{
			fclose(fp_console);
			fp_console = nullptr;

			::FreeConsole();
		}
#endif
	}

	void CheckBuildConriguration()
	{
		if (!fp_console)
			return;

		printf_s("\n\n\n\n\n");

		char config[16];
		switch (FApp::GetBuildConfiguration())
		{
		case 0:
			strcpy_s(config, "Unknown");
			break;
		case 1:
			strcpy_s(config, "Debug");
			break;
		case 2:
			strcpy_s(config, "DebugGame");
			break;
		case 3:
			strcpy_s(config, "Development");
			break;
		case 4:
			strcpy_s(config, "Shipping");
			break;
		case 5:
			strcpy_s(config, "Test");
			break;
		}

		printf_s("GetBuildConfiguration: %s \n", config);
		printf_s("UE_BUILD_DEBUG: %d \n", UE_BUILD_DEBUG);
		printf_s("UE_BUILD_DEVELOPMENT: %d \n", UE_BUILD_DEVELOPMENT);
		printf_s("UE_BUILD_SHIPPING: %d \n", UE_BUILD_SHIPPING);
		printf_s("UE_GAME: %d \n", UE_GAME);
		printf_s("UE_EDITOR: %d \n", UE_EDITOR);
		printf_s("WITH_EDITOR: %d \n", WITH_EDITOR);
		printf_s("UE_BUILD_SHIPPING_WITH_EDITOR: %d \n", UE_BUILD_SHIPPING_WITH_EDITOR);
		printf_s("UE_SERVER: %d \n", UE_SERVER);
		printf_s("UE_BUILD_TEST: %d \n", UE_BUILD_TEST);
		printf_s("\n\n\n\n\n");

	}

	static cMyConsole* GetSingleton()
	{
		static cMyConsole console;
		return &console;
	}

	static void Log(const char* format, ...)
	{
#if UE_BUILD_DEVELOPMENT && UE_GAME
		char buff[MAX_BUFFER];

		va_list arglist;
		va_start(arglist, format);
		vsprintf_s(buff, format, arglist);
		va_end(arglist);

		printf_s(buff);
#endif
	}
};
#define CONSOLE_LOG cMyConsole::Log
/*** Console for log : End ***/


// IOCP CompletionKey
struct stCompletionKey
{
	SOCKET			socket;

	string			IPv4Addr; // ���� Ŭ���̾�Ʈ�� IP �ּ�
	int				Port;	  // ���� Ŭ���̾�Ʈ�� Port �ּ�
};

// IOCP OverlappedMsg
struct stOverlappedMsg
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;

	char			messageBuffer[MAX_BUFFER];
	int				recvBytes;
	int				sendBytes; // WSASend�� ������ �������� ����Ʈ ũ��
};


template <typename T>
class GAME_API cThreadSafetyQueue
{
private:
	std::queue<T> q;
	CRITICAL_SECTION cs;

public:
	cThreadSafetyQueue() { InitializeCriticalSection(&cs); }
	~cThreadSafetyQueue() { DeleteCriticalSection(&cs); }

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
		std::queue<T> copiedQ = q;
		LeaveCriticalSection(&cs);
		return copiedQ;
	}

	std::queue<T> copy_clear()
	{
		EnterCriticalSection(&cs);
		std::queue<T> copiedQ = q;
		while (q.empty() == false)
			q.pop();
		LeaveCriticalSection(&cs);
		return copiedQ;
	}
};


enum EPacketType
{
	/** ��Ŷ ����
	int PacketSize;
	int EPacketType;
	... Data;
	*/

	// �ּ� [1 1 ] ������� Ÿ�Ա��� 4����Ʈ?
	// �ִ� [4096(MAX_BUFFER) 1 ] ����� 4����Ʈ?

	/** ����
	Main Client:
		Send [EPacketType]:
		Recv [EPacketType]:
	Main Server:
		Recv [EPacketType]:
		Send [EPacketType]:
	*/


	///////////////////////////////////////////
	// Main Server / Main Clients
	///////////////////////////////////////////

	/** �÷��̾ OnlineWidget���� LOGIN �ϸ�
	Main Client:
		Send [LOGIN]: �ӽ� ��ü�� cInfoOfPlayer infoOfPlayer�� ID�� ������ �۽�
		Recv [LOGIN]: ���� cInfoOfPlayer�� MyInfo�� ����
	Main Server:
		Recv [LOGIN]: ���� cInfoOfPlayer�� IPv4Addr, SocketByServer, PortByServer�� �����ϰ� InfoOfClients�� ����
		Send [LOGIN]: cInfoOfPlayer
	*/
	LOGIN,

	/** OnlineGameWidget���� (Create Game)��ư�� ���� ������ �����ϸ�
	Main Client:
		Send [CREATE_GAME]: MyInfoOfGame.Leader�� MyInfo�� �����ϰ� �۽�
		Recv []: X
	Main Server:
		Recv [CREATE_GAME]: ���� cInfoOfGame�� InfoOfGames�� ����
		Send []: X
	*/
	CREATE_GAME,

	/** �÷��̾ MainScreenWidget���� Online ��ư�� ���� �����ϸ�
	Main Client:
		Send [FIND_GAMES]: ���� ���� �䱸
		Recv [FIND_GAMES]: ��� ������� ����
	Main Server:
		Recv [FIND_GAMES]: O
		Send [FIND_GAMES]: InfoOfGames�� ���ԵǾ� �ִ� ��� cInfoOfGame�� ����
	*/
	FIND_GAMES,

	/** ���� ���� ����� */
	WAITING_GAME,

	/** �÷��̾ Join ��ư���� ���ӹ濡 ����
	Main Client:
		Send [JOIN_ONLINE_GAME]: Join�� ���ӹ� Leader�� SocketID�� ������ MyInfo
		Recv [WAITING_GAME]: ���� cInfoOfGame�� MyInfoOfGame�� ����
	Main Server:
		Recv [JOIN_ONLINE_GAME]: InfoOfClients�� LeaderSocketByMainServer�� �����ϰ� InfoOfGames�� Players�� �ش� Ŭ���̾�Ʈ ����
		Send [WAITING_GAME] to ����: �ش� ���ӹ��� cInfoOfGame
		Send [WAITING_GAME] to ���ӹ��� �ٸ� �÷��̾��: �ش� ���ӹ��� cInfoOfGame
	*/
	JOIN_ONLINE_GAME,

	/** ������ ���濡�� Back ��ư�� ���� ������ �����ϸ�
	Main Client:
		Send [DESTROY_WAITING_ROOM]: O, InitMyInfoOfGame();
		Recv [DESTROY_WAITING_ROOM]: ���� ���� �˸��� �ڷΰ��� ��ư Ȱ��ȭ, MyInfo�� Ư�� ��������� �ʱ�ȭ, InitMyInfoOfGame();
	Main Server:
		Recv [DESTROY_WAITING_ROOM] by ����: InfoOfClients�� �ش� Ŭ���̾�Ʈ�� Ư�� ��������� �ʱ�ȭ, InfoOfGames.erase(pSocketInfo->socket);
		Send [DESTROY_WAITING_ROOM] to �÷��̾��(���� ����): O
	*/
	DESTROY_WAITING_GAME,

	/** ������ �ƴ� ������ �÷��̾ ���濡�� ������
	Main Client:
		Send [EXIT_WAITING_ROOM]: O, MyInfo�� Ư�� ��������� �ʱ�ȭ, InitMyInfoOfGame();
		Recv [WAITING_GAME]: ���� cInfoOfGame
	Main Server:
		Recv [EXIT_WAITING_ROOM]: InfoOfClients�� �ش� Ŭ���̾�Ʈ�� Ư�� ��������� �ʱ�ȭ, InfoOfGames�� Players�� �ش� Ŭ���̾�Ʈ ����
		Send [WAITING_GAME] to ����: �ش� ������ cInfoOfGame
		Send [WAITING_GAME] to �÷��̾��(�ش� Ŭ���̾�Ʈ ������): �ش� ������ cInfoOfGame
	*/
	EXIT_WAITING_GAME,

	/** ������ ���濡�� Title�̳� Stage�� Maximum�� �����ϸ�
	Main Client:
		Send [MODIFY_WAITING_GAME]: cInfoOfGame(Title, Stage, Maximum)
		Recv [MODIFY_WAITING_GAME]: cInfoOfGame(Title, Stage, Maximum)
	Main Server:
		Recv [MODIFY_WAITING_GAME]: InfoOfGames�� cInfoOfGame(Title, Stage, Maximum) ����
		Send [MODIFY_WAITING_GAME] to �÷��̾��(���� ����): cInfoOfGame(Title, Stage, Maximum)
	*/
	MODIFY_WAITING_GAME,

	/** ������ ���濡�� ������ �����ϸ�
	Main Client:
		Send [START_WAITING_GAME]: O
		Recv [START_WAITING_GAME]: O
	Main Server:
		Recv [START_WAITING_GAME] by ����: O
		Send [START_WAITING_GAME] to �÷��̾��(���� ����): O
	*/
	START_WAITING_GAME,


	/** ������ ���濡�� ������ �����ϰ� CountStartedGame()���� ���� ���� �ʱ�ȭ�� �����ϸ�
	Main Client:
		Send [ACTIVATE_GAME_SERVER]: MyInfo�� PortOfGameServer�� �����ϰ� MyInfo�� ����, MyInfoOfGame.State = string("Playing");
		Recv [WAITING_GAME]: ���� cInfoOfGame
	Main Server:
		Recv [ACTIVATE_GAME_SERVER] by ����: ������ cInfoOfPlayer�� InfoOfClients�� InfoOfGames�� Leader�� ����
		Send [WAITING_GAME] to ����: �ش� ������ cInfoOfGame
		Send [WAITING_GAME] to �÷��̾��: �ش� ������ cInfoOfGame
	*/
	ACTIVATE_GAME_SERVER,

	/** �����ڰ� ���� Ŭ���̾�Ʈ�� ���� ������ �����Ű�� ���� ���� ���� ������ ��û
	Main Client:
		Send [REQUEST_INFO_OF_GAME_SERVER]: O
		Recv [REQUEST_INFO_OF_GAME_SERVER]: cInfoOfPlayer�� �ް� IPv4Addr�� PortOfGameServer�� ȹ��
	Main Server:
		Recv [REQUEST_INFO_OF_GAME_SERVER]: O
		Send [REQUEST_INFO_OF_GAME_SERVER]: cInfoOfPlayer infoOfPlayer = InfoOfClients.at(leaderSocket);�ϰ� infoOfPlayer ����
	*/
	REQUEST_INFO_OF_GAME_SERVER,


	///////////////////////////////////////////
	// Game Server / Game Clients
	///////////////////////////////////////////

	/** ���Ӽ����� ����Ŭ���̾�Ʈ�� ����Ǿ��ٸ� 
	Game Client:
		Recv [CONNECTED]: (����4) ���� cInfoOfPlayer�� ClientSocket::MyInfo�� ����
		Send [CONNECTED]: (����1) ClientSocket::MyInfo�� ����
	Game Server:
		Recv [CONNECTED]: (����2) ���� cInfoOfPlayer�� SocketByGameServer, PortOfGameServer, PortOfGameClient�� �����ϰ� InfoOfClients�� ����
		Send [CONNECTED]: (����3) InfoOfClients�� ������ cInfoOfPlayer
	*/
	CONNECTED,

	/** ���Ӽ����� ����Ǹ�
	Game Client:
		Recv [DISCONNECTED]:
		Send [X]:
	Game Server:
		Recv [X]:
		Send [DISCONNECTED]:
	*/
	DISCONNECT,

	/** Ŭ���̾�Ʈ�� �����ð����� ScoreBoard ������ ��û
	Game Client:
		Recv [SCORE_BOARD]: 
		Send [SCORE_BOARD]: 
	Game Server:
		Recv [SCORE_BOARD]: 
		Send [SCORE_BOARD]: 
	*/
	SCORE_BOARD,

	/** ���Ӽ����� SpaceShip ������ ��� ����Ŭ���̾�Ʈ�鿡�� ����
	Game Client:
		Recv [SPACE_SHIP]:
		Send [X]:
	Game Server:
		Recv [X]:
		Send [SPACE_SHIP]:
	*/
	SPACE_SHIP,

	/** Ŭ���̾�Ʈ�� �������°� �Ǹ�
	Game Client:
		Recv []:
		Send [OBSERVATION]:
	Game Server:
		Recv [OBSERVATION]:
		Send []:
	*/
	OBSERVATION,


	/** PioneerManager::SpawnPioneer(...) ȣ��Ǹ�
	Game Client:
		Recv [SPAWN_PIONNER]:
		Send []:
	Game Server:
		Recv []:
		Send [SPAWN_PIONNER]:
	*/
	SPAWN_PIONEER,

	/** Pioneer�� ������
	Game Client:
		Recv [DIED_PIONEER]:
		Send [DIED_PIONEER]:
	Game Server:
		Recv [DIED_PIONEER]:
		Send [DIED_PIONEER]:
	*/
	DIED_PIONEER,

	/** ����Ŭ���̾�Ʈ�� �ڽ��� �������� Pioneer�� �ִϸ��̼� ������ ������ ���Ӽ����� �ش� Pioneer�� ������ �ٸ� Pioneer���� ������ ��ε�ĳ����
	Game Client:
		Recv [INFO_OF_PIONEER_ANIMATION]:
		Send [INFO_OF_PIONEER_ANIMATION]:
	Game Server:
		Recv [INFO_OF_PIONEER_ANIMATION]:
		Send [INFO_OF_PIONEER_ANIMATION]:
	*/
	INFO_OF_PIONEER_ANIMATION,

	/** ����Ŭ���̾�Ʈ�� �������� Pioneer�� �����Ϸ��� ��û�� ������
	Game Client:
		Recv [POSSESS_PIONEER]:
		Send [POSSESS_PIONEER]:
	Game Server:
		Recv [POSSESS_PIONEER]:
		Send [POSSESS_PIONEER]: �̹� �������� �������̰ų� ������ 0�� �����ϴ�.
	*/
	POSSESS_PIONEER,

	/** POSSESS_PIONEER�� ��ε�ĳ�������� ����
	Game Client:
		Recv [INFO_OF_PIONEER_SOCKET]:
		Send []:
	Game Server:
		Recv []:
		Send [INFO_OF_PIONEER_SOCKET]: 
	*/
	INFO_OF_PIONEER_SOCKET, 

	/** 
	Game Client:
		Recv [INFO_OF_PIONEER_STAT]:
		Send [INFO_OF_PIONEER_STAT]:
	Game Server:
		Recv [INFO_OF_PIONEER_STAT]:
		Send [INFO_OF_PIONEER_STAT]:
	*/
	INFO_OF_PIONEER_STAT,

	/**
	Game Client:
		Recv [INFO_OF_PROJECTILE]:
		Send [INFO_OF_PROJECTILE]:
	Game Server:
		Recv [INFO_OF_PROJECTILE]:
		Send [INFO_OF_PROJECTILE]:
	*/
	INFO_OF_PROJECTILE, 


	/**
	Game Client:
		Recv [INFO_OF_RESOURCES]:
		Send []:
	Game Server:
		Recv []:
		Send [INFO_OF_RESOURCES]: ���� �ڿ����� �����ϴ�.
	*/
	INFO_OF_RESOURCES,



	/** �ǹ��� �Ǽ��ϸ�
	Game Client:
		Recv [INFO_OF_BUILDING_SPAWN]: �Ǽ��� �մϴ�.
		Send [INFO_OF_BUILDING_SPAWN]: �Ǽ� ��û�� �������� �����ϴ�.
	Game Server:
		Recv [INFO_OF_BUILDING_SPAWN]: �Ǽ� ��û�� �ʿ��� �ڿ��� �����ϸ� �Ǽ��ϰ�
		Send [INFO_OF_BUILDING_SPAWN]: ��� �÷��̾�� �Ǽ��϶�� ����մϴ�.
	*/
	INFO_OF_BUILDING_SPAWN,

	/** 
	Game Client:
		Recv [INFO_OF_BUILDING_STAT]: 
		Send [INFO_OF_BUILDING_STAT]: 
	Game Server:
		Recv [INFO_OF_BUILDING_STAT]: 
		Send [INFO_OF_BUILDING_STAT]: 
	*/
	INFO_OF_BUILDING_STAT,


	/** ����Ŭ���̾�Ʈ�� ���� ������
	Game Client:
		Recv [INFO_OF_BUILDING]:
		Send [INFO_OF_BUILDING]:
	Game Server:
		Recv [INFO_OF_BUILDING]:
		Send [INFO_OF_BUILDING]: 
	*/
	INFO_OF_BUILDING,

	/** 
	Game Client:
		Recv [DESTROY_BUILDING]:
		Send []:
	Game Server:
		Recv []:
		Send [DESTROY_BUILDING]:
	*/
	DESTROY_BUILDING,


	/** EnemyManager::SpawnEnemy(...) ȣ��Ǹ�
	Game Client:
		Recv [SPAWN_ENEMY]:
		Send []:
	Game Server:
		Recv []:
		Send [SPAWN_ENEMY]:
	*/
	SPAWN_ENEMY,

	/** 
	Game Client:
		Recv [INFO_OF_ENEMY_ANIMATION]:
		Send [INFO_OF_ENEMY_ANIMATION]: ��û
	Game Server:
		Recv [INFO_OF_ENEMY_ANIMATION]: ��û Ȯ��
		Send [INFO_OF_ENEMY_ANIMATION]:
	*/
	INFO_OF_ENEMY_ANIMATION,

	/**
	Game Client:
		Recv [INFO_OF_ENEMY_STAT]:
		Send [INFO_OF_ENEMY_STAT]: ��û
	Game Server:
		Recv [INFO_OF_ENEMY_STAT]: ��û Ȯ��
		Send [INFO_OF_ENEMY_STAT]:
	*/
	INFO_OF_ENEMY_STAT,

	/** 
	Game Client:
		Recv [DESTROY_ENEMY]:
		Send []:
	Game Server:
		Recv []:
		Send [DESTROY_ENEMY]:
	*/
	DESTROY_ENEMY
};


class GAME_API cInfoOfPlayer
{
public:
	string ID;
	string IPv4Addr; // IP ��ȣ

	int SocketByMainServer; // ���� �����κ��� �ο��� Ŭ���̾�Ʈ�� ���� ��ȣ
	int SocketByGameServer; // ���� ������ �����ϴ� �������κ��� �ο��� ���� ��ȣ

	int PortOfMainClient; // ���� Ŭ���̾�Ʈ�� ���� ������ ����� ��Ʈ ��ȣ
	int PortOfGameServer; // �������κ��� ������ ���� ������ ��Ʈ ��ȣ
	int PortOfGameClient; // ���� Ŭ���̾�Ʈ�� ���� ������ ����� ��Ʈ ��ȣ

	int LeaderSocketByMainServer; // ���� �����κ��� �ο��� ������ ���� ��ȣ

public:
	cInfoOfPlayer()
	{
		ID = "NULL";
		IPv4Addr = "NULL";
		SocketByMainServer = 0;
		SocketByGameServer = 0;
		PortOfMainClient = 0;
		PortOfGameServer = 0;
		PortOfGameClient = 0;
		LeaderSocketByMainServer = 0;
	}
	~cInfoOfPlayer()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfPlayer& Info)
	{
		Stream << ReplaceCharInString(Info.ID, ' ', '_') << ' ';
		Stream << ReplaceCharInString(Info.IPv4Addr, ' ', '_') << ' ';
		Stream << Info.SocketByMainServer << ' ';
		Stream << Info.SocketByGameServer << ' ';
		Stream << Info.PortOfMainClient << ' ';
		Stream << Info.PortOfGameServer << ' ';
		Stream << Info.PortOfGameClient << ' ';
		Stream << Info.LeaderSocketByMainServer << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfPlayer& Info)
	{
		Stream >> Info.ID;
		Info.ID = ReplaceCharInString(Info.ID, '_', ' ');
		Stream >> Info.IPv4Addr;
		Info.IPv4Addr = ReplaceCharInString(Info.IPv4Addr, '_', ' ');
		Stream >> Info.SocketByMainServer;
		Stream >> Info.SocketByGameServer;
		Stream >> Info.PortOfMainClient;
		Stream >> Info.PortOfGameServer;
		Stream >> Info.PortOfGameClient;
		Stream >> Info.LeaderSocketByMainServer;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfPlayer> ID: %s, IPv4Addr: %s, SocketByMainServer: %d, SocketByGameServer: %d, PortOfMainClient: %d, PortOfGameServer: %d, PortOfGameClient: %d, LeaderSocketByMainServer: %d\n", TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID.c_str(), IPv4Addr.c_str(), SocketByMainServer, SocketByGameServer, PortOfMainClient, PortOfGameServer, PortOfGameClient, LeaderSocketByMainServer);
	}

	// Convert
	static string ReplaceCharInString(string str, char before, char after)
	{
		string result = str;
		for (int i = 0; i < result.size(); i++)
		{
			if (result.at(i) == before)
				result.at(i) = after;
		}
		return result;
	}
};

class GAME_API cInfoOfPlayers
{
public:
	std::map<int, cInfoOfPlayer> Players;

public:
	cInfoOfPlayers() {}
	~cInfoOfPlayers() {}

	// Send
	friend ostream& operator<<(ostream &Stream, cInfoOfPlayers& Info)
	{
		Stream << Info.Players.size() << ' ';
		for (auto& kvp : Info.Players)
		{
			Stream << kvp.first << ' ';
			Stream << kvp.second << ' ';
		}

		return Stream;
	}

	// Recv
	friend istream &operator>>(istream &Stream, cInfoOfPlayers& Info)
	{
		int nPlayers = 0;
		int socketID = 0;
		cInfoOfPlayer Player;

		// �ʱ�ȭ
		Info.Players.clear();

		Stream >> nPlayers;
		for (int i = 0; i < nPlayers; i++)
		{
			Stream >> socketID;
			Stream >> Player;
			Info.Players[socketID] = Player;
		}

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		for (auto& kvp : Players)
		{
			CONSOLE_LOG("%s%skey: %d, ", TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), kvp.first);
			kvp.second.PrintInfo(Space, Space2);
		}
	}

	size_t Size()
	{
		return Players.size();
	}

	void Add(int SocketID, cInfoOfPlayer InfoOfPlayer)
	{
		Players[SocketID] = InfoOfPlayer;
	}

	void Remove(int SocketID)
	{
		Players.erase(SocketID);
	}
};


class GAME_API cInfoOfGame
{
public:
	string State;
	string Title;
	int Stage;
	int nMax; // �ִ� �÷��̾� �� ����

	cInfoOfPlayer Leader; // ����
	cInfoOfPlayers Players; // ������ ������ �����ڵ�

public:
	cInfoOfGame() 
	{
		State = "Waiting";
		Title = "Let's_go_together!";
		Stage = 1;
		nMax = 100;
	}
	~cInfoOfGame() {}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfGame& Info)
	{
		Stream << ReplaceCharInString(Info.State, ' ', '_') << ' ';
		Stream << ReplaceCharInString(Info.Title, ' ', '_') << ' ';
		Stream << Info.Stage << ' ';
		Stream << Info.nMax << ' ';
		Stream << Info.Leader << ' ';
		Stream << Info.Players << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfGame& Info)
	{
		Stream >> Info.State;
		Info.State = ReplaceCharInString(Info.State, '_', ' ');
		Stream >> Info.Title;
		Info.Title = ReplaceCharInString(Info.Title, '_', ' ');
		Stream >> Info.Stage;
		Stream >> Info.nMax;
		Stream >> Info.Leader;
		Stream >> Info.Players;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T("    "))
	{
		CONSOLE_LOG("%s<cInfoOfGame> Start\n", TCHAR_TO_ANSI(Space));
		CONSOLE_LOG("%s%sState: %s, Title: %s, Stage: %d, nMax: %d\n", TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), State.c_str(), Title.c_str(), Stage, nMax);
		Leader.PrintInfo(Space, Space2);
		Players.PrintInfo(Space, Space2);
		CONSOLE_LOG("%s<cInfoOfGame> End\n", TCHAR_TO_ANSI(Space));
	}

	// Convert
	static string ReplaceCharInString(string str, char before, char after)
	{
		string result = str;
		for (int i = 0; i < result.size(); i++)
		{
			if (result.at(i) == before)
				result.at(i) = after;
		}
		return result;
	}
};




class GAME_API cInfoOfScoreBoard
{
public:
	int Ping;
	string ID;
	string State;
	int Level;
	int Kill;
	int Death;

public:
	cInfoOfScoreBoard()
	{
		Ping = 0;
		ID = "NULL";
		State = "Observing";
		Level = 1;
		Kill = 0;
		Death = 0;
	}
	~cInfoOfScoreBoard()
	{

	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfScoreBoard& Info)
	{
		Stream << Info.Ping << ' ';
		Stream << ReplaceCharInString(Info.ID, ' ', '_') << ' ';
		Stream << ReplaceCharInString(Info.State, ' ', '_') << ' ';
		Stream << Info.Level << ' ';
		Stream << Info.Kill << ' ';
		Stream << Info.Death << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfScoreBoard& Info)
	{
		Stream >> Info.Ping;
		Stream >> Info.ID;
		Info.ID = ReplaceCharInString(Info.ID, '_', ' ');
		Stream >> Info.State;
		Info.State = ReplaceCharInString(Info.State, '_', ' ');
		Stream >> Info.Level;
		Stream >> Info.Kill;
		Stream >> Info.Death;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfScoreBoard> Ping: %d, ID: %s, State: %s, Level: %d, Kill: %d, Death: %d \n", TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), Ping, ID.c_str(), State.c_str(), Level, Kill, Death);
	}

	// Convert
	static string ReplaceCharInString(string str, char before, char after)
	{
		string result = str;
		for (int i = 0; i < result.size(); i++)
		{
			if (result.at(i) == before)
				result.at(i) = after;
		}
		return result;
	}

	// For Sort()
	bool operator<(cInfoOfScoreBoard& other) const
	{
		if (this->Kill == other.Kill)
			return this->Level > other.Level;
		else
			return this->Kill > other.Kill;
	}
};


class GAME_API cInfoOfSpaceShip
{
public:
	int State;

	float LocX;
	float LocY;
	float LocZ;

	bool bHiddenInGame;
	bool bSimulatePhysics;
	float ScaleOfEngineParticleSystem;
	float AccelerationZ;
	bool bEngine;

public:
	cInfoOfSpaceShip()
	{
		State = 0;

		LocX = 0.0f;
		LocY = 0.0f;
		LocZ = 0.0f;

		bHiddenInGame = false;
		bSimulatePhysics = true;
		ScaleOfEngineParticleSystem = 0.010f;
		AccelerationZ = 980.0f;
		bEngine = false;
	}
	~cInfoOfSpaceShip()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfSpaceShip& Info)
	{
		Stream << Info.State << ' ';
		Stream << Info.LocX << ' ';
		Stream << Info.LocY << ' ';
		Stream << Info.LocZ << ' ';
		Stream << Info.bHiddenInGame << ' ';
		Stream << Info.bSimulatePhysics << ' ';
		Stream << Info.ScaleOfEngineParticleSystem << ' ';
		Stream << Info.AccelerationZ << ' ';
		Stream << Info.bEngine << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfSpaceShip& Info)
	{
		Stream >> Info.State;
		Stream >> Info.LocX;
		Stream >> Info.LocY;
		Stream >> Info.LocZ;
		Stream >> Info.bHiddenInGame;
		Stream >> Info.bSimulatePhysics;
		Stream >> Info.ScaleOfEngineParticleSystem;
		Stream >> Info.AccelerationZ;
		Stream >> Info.bEngine;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfSpaceShip> State: %d, LocX: %f, LocY: %f, LocZ: %f, bHiddenInGame: %s, bSimulatePhysics: %s, ScaleOfEngineParticleSystem: %f, AccelerationZ: %f, bEngine: %s \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), State, LocX, LocY, LocZ, (bHiddenInGame == true) ? "true" : "false", (bSimulatePhysics == true) ? "true" : "false", ScaleOfEngineParticleSystem, AccelerationZ, (bEngine == true) ? "true" : "false");
	}

	void SetInfo(int State_, FVector Location, bool bHiddenInGame_, bool bSimulatePhysics_, float ScaleOfEngineParticleSystem_, float AccelerationZ_, bool bEngine_)
	{
		this->State = State_;

		LocX = Location.X;
		LocY = Location.Y;
		LocZ = Location.Z;

		this->bHiddenInGame = bHiddenInGame_;
		this->bSimulatePhysics = bSimulatePhysics_;
		this->ScaleOfEngineParticleSystem = ScaleOfEngineParticleSystem_;
		this->AccelerationZ = AccelerationZ_;
		this->bEngine = bEngine_;
	}
};


class GAME_API cInfoOfPioneer_Socket
{
public:
	int ID;

	int SocketID;
	string NameOfID;
	//bool bDying; // ���� �ʿ���� �� �մϴ�.

public:
	cInfoOfPioneer_Socket()
	{
		ID = 0;

		SocketID = 0;
		NameOfID = "AI";
		//bDying = false;
	}
	~cInfoOfPioneer_Socket()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfPioneer_Socket& Info)
	{
		Stream << Info.ID << ' ';

		Stream << Info.SocketID << ' ';
		Stream << ReplaceCharInString(Info.NameOfID, ' ', '_') << ' ';
		Stream << Info.NameOfID << ' ';
		//Stream << Info.bDying << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfPioneer_Socket& Info)
	{
		Stream >> Info.ID;

		Stream >> Info.SocketID;
		Stream >> Info.NameOfID;
		Info.NameOfID = ReplaceCharInString(Info.NameOfID, '_', ' ');
		//Stream >> Info.bDying;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfPioneer_Socket> ID: %d, SocketID : %d, NameOfID: %s\n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID, SocketID, NameOfID.c_str());
	}

	// Convert
	static string ReplaceCharInString(string str, char before, char after)
	{
		string result = str;
		for (int i = 0; i < result.size(); i++)
		{
			if (result.at(i) == before)
				result.at(i) = after;
		}
		return result;
	}
};

class GAME_API cInfoOfPioneer_Animation
{
public:
	int ID;

	float RotX;
	float RotY;
	float RotZ;

	float LocX;
	float LocY;
	float LocZ;

	float TargetRotX;
	float TargetRotY;
	float TargetRotZ;


	// �ִϸ��̼� ������
	float VelocityX;
	float VelocityY;
	float VelocityZ;

	bool bHasPistolType;
	bool bHasRifleType;
	bool bHasLauncherType;

	bool bFired;

	// ����
	int IdxOfCurrentWeapon;

	bool bArmedWeapon;

public:
	cInfoOfPioneer_Animation()
	{
		ID = 0;

		RotX = 0.0f;
		RotY = 0.0f;
		RotZ = 0.0f;

		LocX = 0.0f;
		LocY = 0.0f;
		LocZ = 0.0f;

		TargetRotX = 0.0f;
		TargetRotY = 0.0f;
		TargetRotZ = 0.0f;

		VelocityX = 0.0f;
		VelocityY = 0.0f;
		VelocityZ = 0.0f;

		bHasPistolType = false;
		bHasRifleType = false;
		bHasLauncherType = false;

		bFired = false;

		IdxOfCurrentWeapon = 0;

		bArmedWeapon = false;
	}
	~cInfoOfPioneer_Animation()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfPioneer_Animation& Info)
	{
		Stream << Info.ID << ' ';

		Stream << Info.RotX << ' ';
		Stream << Info.RotY << ' ';
		Stream << Info.RotZ << ' ';
		Stream << Info.LocX << ' ';
		Stream << Info.LocY << ' ';
		Stream << Info.LocZ << ' ';
		Stream << Info.TargetRotX << ' ';
		Stream << Info.TargetRotY << ' ';
		Stream << Info.TargetRotZ << ' ';

		Stream << Info.VelocityX << ' ';
		Stream << Info.VelocityY << ' ';
		Stream << Info.VelocityZ << ' ';
		Stream << Info.bHasPistolType << ' ';
		Stream << Info.bHasRifleType << ' ';
		Stream << Info.bHasLauncherType << ' ';
		Stream << Info.bFired << ' ';

		Stream << Info.IdxOfCurrentWeapon << ' ';

		Stream << Info.bArmedWeapon << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfPioneer_Animation& Info)
	{
		Stream >> Info.ID;

		Stream >> Info.RotX;
		Stream >> Info.RotY;
		Stream >> Info.RotZ;
		Stream >> Info.LocX;
		Stream >> Info.LocY;
		Stream >> Info.LocZ;
		Stream >> Info.TargetRotX;
		Stream >> Info.TargetRotY;
		Stream >> Info.TargetRotZ;

		Stream >> Info.VelocityX;
		Stream >> Info.VelocityY;
		Stream >> Info.VelocityZ;
		Stream >> Info.bHasPistolType;
		Stream >> Info.bHasRifleType;
		Stream >> Info.bHasLauncherType;
		Stream >> Info.bFired;

		Stream >> Info.IdxOfCurrentWeapon;

		Stream >> Info.bArmedWeapon;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfPioneer_Animation> ID: %d, RotX: %f, RotY: %f, RotZ: %f, LocX: %f, LocY: %f, LocZ: %f, TargetRotX: %f, TargetRotY: %f, TargetRotZ: %f \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID, RotX, RotY, RotZ, LocX, LocY, LocZ, TargetRotX, TargetRotY, TargetRotZ);
		CONSOLE_LOG("%s%s<cInfoOfPioneer_Animation> VelocityX: %f, VelocityY: %f, VelocityZ: %f, bHasPistolType: %s, bHasRifleType : %s, bHasLauncherType: %s, bFired: %s \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), VelocityX, VelocityY, VelocityZ, (bHasPistolType == true) ? "true" : "false", (bHasRifleType == true) ? "true" : "false", (bHasLauncherType == true) ? "true" : "false", (bFired == true) ? "true" : "false");
		CONSOLE_LOG("%s%s<cInfoOfPioneer_Animation> IdxOfCurrentWeapon: %d, bArmedWeapon: %s \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), IdxOfCurrentWeapon, (bArmedWeapon == true) ? "true" : "false");
	}

	void SetActorTransform(const FTransform& Transform)
	{
		RotX = Transform.GetRotation().Rotator().Pitch;
		RotY = Transform.GetRotation().Rotator().Yaw;
		RotZ = Transform.GetRotation().Rotator().Roll;
	
		LocX = Transform.GetLocation().X;
		LocY = Transform.GetLocation().Y;
		LocZ = Transform.GetLocation().Z;
	}
	
	FTransform GetActorTransform()
	{
		FTransform transform;
		FQuat quat(FRotator(RotX, RotY, RotZ));
		transform.SetRotation(quat);
		transform.SetLocation(FVector(LocX, LocY, LocZ));
	
		return transform;
	}
};

class GAME_API cInfoOfPioneer_Stat
{
public:
	int ID;

	float HealthPoint;
	float MaxHealthPoint;
	
	float MoveSpeed;
	float AttackSpeed;
	
	float AttackPower;
	
	float SightRange;
	float DetectRange;
	float AttackRange;

	float Exp;
	int Level;


public:
	cInfoOfPioneer_Stat()
	{
		ID = 0;

		HealthPoint = 100.0f;
		MaxHealthPoint = 100.0f;
		
		MoveSpeed = 10.0f;
		AttackSpeed = 1.0f;
		
		AttackPower = 1.0f;
		
		SightRange = 32.0f;
		DetectRange = 32.0f;
		AttackRange = 16.0f;

		Exp = 0.0f;
		Level = 1;
	}
	~cInfoOfPioneer_Stat()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfPioneer_Stat& Info)
	{
		Stream << Info.ID << ' ';

		Stream << Info.HealthPoint << ' ';
		Stream << Info.MaxHealthPoint << ' ';
		Stream << Info.MoveSpeed << ' ';
		Stream << Info.AttackSpeed << ' ';
		Stream << Info.AttackPower << ' ';
		Stream << Info.SightRange << ' ';
		Stream << Info.DetectRange << ' ';
		Stream << Info.AttackRange << ' ';

		Stream << Info.Exp << ' ';
		Stream << Info.Level << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfPioneer_Stat& Info)
	{
		Stream >> Info.ID;

		Stream >> Info.HealthPoint;
		Stream >> Info.MaxHealthPoint;
		Stream >> Info.MoveSpeed;
		Stream >> Info.AttackSpeed;
		Stream >> Info.AttackPower;
		Stream >> Info.SightRange;
		Stream >> Info.DetectRange;
		Stream >> Info.AttackRange;

		Stream >> Info.Exp;
		Stream >> Info.Level;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfPioneer_Stat> ID: %d, HealthPoint: %f, MaxHealthPoint : %f, MoveSpeed: %f, AttackSpeed: %f, AttackPower: %f, SightRange: %f, DetectRange: %f, AttackRange: %f, Exp: %f, Level: %d \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID, HealthPoint, MaxHealthPoint, MoveSpeed, AttackSpeed, AttackPower, SightRange, DetectRange, AttackRange, Exp, Level);
	}

};

// ���� ��ģ��
class GAME_API cInfoOfPioneer
{
public:
	int ID;

	cInfoOfPioneer_Socket Socket;
	cInfoOfPioneer_Animation Animation;
	cInfoOfPioneer_Stat Stat;

public:
	cInfoOfPioneer()
	{
		ID = 0;


	}
	cInfoOfPioneer(int ID, cInfoOfPioneer_Socket Socket, cInfoOfPioneer_Animation Animation, cInfoOfPioneer_Stat Stat)
		: ID(ID), Socket(Socket), Animation(Animation), Stat(Stat)
	{
	}
	~cInfoOfPioneer()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfPioneer& Info)
	{
		Stream << Info.ID << ' ';

		Stream << Info.Socket << ' ';
		Stream << Info.Animation << ' ';
		Stream << Info.Stat << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfPioneer& Info)
	{
		Stream >> Info.ID;

		Stream >> Info.Socket;
		Stream >> Info.Animation;
		Stream >> Info.Stat;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		Socket.PrintInfo();
		Animation.PrintInfo();
		Stat.PrintInfo();
	}

};

class GAME_API cInfoOfProjectile
{
public:
	int ID; // Pioneer::ID;

	int Numbering;

	float ScaleX;
	float ScaleY;
	float ScaleZ;

	float RotX;
	float RotY;
	float RotZ;

	float LocX;
	float LocY;
	float LocZ;

public:
	cInfoOfProjectile()
	{
		ID = 0;

		Numbering = 0;

		ScaleX = 0.0f;
		ScaleY = 0.0f;
		ScaleZ = 0.0f;

		RotX = 0.0f;
		RotY = 0.0f;
		RotZ = 0.0f;

		LocX = 0.0f;
		LocY = 0.0f;
		LocZ = 0.0f;
	}
	~cInfoOfProjectile()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfProjectile& Info)
	{
		Stream << Info.ID << ' ';

		Stream << Info.Numbering << ' ';

		Stream << Info.ScaleX << ' ';
		Stream << Info.ScaleY << ' ';
		Stream << Info.ScaleZ << ' ';

		Stream << Info.RotX << ' ';
		Stream << Info.RotY << ' ';
		Stream << Info.RotZ << ' ';

		Stream << Info.LocX << ' ';
		Stream << Info.LocY << ' ';
		Stream << Info.LocZ << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfProjectile& Info)
	{
		Stream >> Info.ID;

		Stream >> Info.Numbering;

		Stream >> Info.ScaleX;
		Stream >> Info.ScaleY;
		Stream >> Info.ScaleZ;

		Stream >> Info.RotX;
		Stream >> Info.RotY;
		Stream >> Info.RotZ;

		Stream >> Info.LocX;
		Stream >> Info.LocY;
		Stream >> Info.LocZ;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfProjectile> ID: %d, Numbering : %d \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID, Numbering);
		CONSOLE_LOG("%s%s<cInfoOfProjectile> ScaleX: %f, ScaleY : %f, ScaleZ: %f, RotX: %f, RotY : %f, RotZ: %f, LocX: %f, LocY : %f, LocZ: %f \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ScaleX, ScaleY, ScaleZ, RotX, RotY, RotZ, LocX, LocY, LocZ);
	}

	void SetActorTransform(const FTransform& Transform)
	{
		ScaleX = Transform.GetScale3D().X;
		ScaleY = Transform.GetScale3D().Y;
		ScaleZ = Transform.GetScale3D().Z;

		RotX = Transform.GetRotation().Rotator().Pitch;
		RotY = Transform.GetRotation().Rotator().Yaw;
		RotZ = Transform.GetRotation().Rotator().Roll;

		LocX = Transform.GetLocation().X;
		LocY = Transform.GetLocation().Y;
		LocZ = Transform.GetLocation().Z;
	}

	FTransform GetActorTransform()
	{
		FTransform transform;

		transform.SetScale3D(FVector(ScaleX, ScaleY, ScaleZ));
		FQuat quat(FRotator(RotX, RotY, RotZ));
		transform.SetRotation(quat);
		transform.SetLocation(FVector(LocX, LocY, LocZ));

		return transform;
	}
};

class GAME_API cInfoOfResources
{
public:
	float NumOfMineral;
	float NumOfOrganic;
	float NumOfEnergy;

public:
	cInfoOfResources()
	{
		NumOfMineral = 100.0f;
		NumOfOrganic = 50.0f;
		NumOfEnergy = 100.0f;
	}
	~cInfoOfResources()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfResources& Info)
	{
		Stream << Info.NumOfMineral << ' ';
		Stream << Info.NumOfOrganic << ' ';
		Stream << Info.NumOfEnergy << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfResources& Info)
	{
		Stream >> Info.NumOfMineral;
		Stream >> Info.NumOfOrganic;
		Stream >> Info.NumOfEnergy;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfResources> NumOfMineral: %f, NumOfOrganic: %f, NumOfEnergy: %f \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), NumOfMineral, NumOfOrganic, NumOfEnergy);
	}
};


class GAME_API cInfoOfBuilding_Spawn
{
public:
	int ID; // BuildingManager::ID

	int Numbering; // 1~8

	float NeedMineral;
	float NeedOrganicMatter;

	float ScaleX;
	float ScaleY;
	float ScaleZ;

	float RotX;
	float RotY;
	float RotZ;

	float LocX;
	float LocY;
	float LocZ;


	// �ͷ������� ���
	int IdxOfUnderWall;

public:
	cInfoOfBuilding_Spawn()
	{
		ID = 0;

		Numbering = 0;

		NeedMineral = 0.0f;
		NeedOrganicMatter = 0.0f;

		ScaleX = 0.0f;
		ScaleY = 0.0f;
		ScaleZ = 0.0f;

		RotX = 0.0f;
		RotY = 0.0f;
		RotZ = 0.0f;

		LocX = 0.0f;
		LocY = 0.0f;
		LocZ = 0.0f;

		IdxOfUnderWall = 0;
	}
	~cInfoOfBuilding_Spawn()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfBuilding_Spawn& Info)
	{
		Stream << Info.ID << ' ';

		Stream << Info.Numbering << ' ';

		Stream << Info.NeedMineral << ' ';
		Stream << Info.NeedOrganicMatter << ' ';

		Stream << Info.ScaleX << ' ';
		Stream << Info.ScaleY << ' ';
		Stream << Info.ScaleZ << ' ';

		Stream << Info.RotX << ' ';
		Stream << Info.RotY << ' ';
		Stream << Info.RotZ << ' ';

		Stream << Info.LocX << ' ';
		Stream << Info.LocY << ' ';
		Stream << Info.LocZ << ' ';

		Stream << Info.IdxOfUnderWall << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfBuilding_Spawn& Info)
	{
		Stream >> Info.ID;

		Stream >> Info.Numbering;

		Stream >> Info.NeedMineral;
		Stream >> Info.NeedOrganicMatter;

		Stream >> Info.ScaleX;
		Stream >> Info.ScaleY;
		Stream >> Info.ScaleZ;

		Stream >> Info.RotX;
		Stream >> Info.RotY;
		Stream >> Info.RotZ;

		Stream >> Info.LocX;
		Stream >> Info.LocY;
		Stream >> Info.LocZ;

		Stream >> Info.IdxOfUnderWall;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfBuilding_Spawn> ID: %d, Numbering : %d, NeedMineral: %f, NeedOrganicMatter: %f \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID, Numbering, NeedMineral, NeedOrganicMatter);
		CONSOLE_LOG("%s%s<cInfoOfBuilding_Spawn> ScaleX: %f, ScaleY : %f, ScaleZ: %f, RotX: %f, RotY : %f, RotZ: %f, LocX: %f, LocY : %f, LocZ: %f \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ScaleX, ScaleY, ScaleZ, RotX, RotY, RotZ, LocX, LocY, LocZ);
	}

	void SetActorTransform(const FTransform& Transform)
	{
		ScaleX = Transform.GetScale3D().X;
		ScaleY = Transform.GetScale3D().Y;
		ScaleZ = Transform.GetScale3D().Z;

		RotX = Transform.GetRotation().Rotator().Pitch;
		RotY = Transform.GetRotation().Rotator().Yaw;
		RotZ = Transform.GetRotation().Rotator().Roll;

		LocX = Transform.GetLocation().X;
		LocY = Transform.GetLocation().Y;
		LocZ = Transform.GetLocation().Z;
	}

	FTransform GetActorTransform()
	{
		FTransform transform;

		transform.SetScale3D(FVector(ScaleX, ScaleY, ScaleZ));
		FQuat quat(FRotator(RotX, RotY, RotZ));
		transform.SetRotation(quat);
		transform.SetLocation(FVector(LocX, LocY, LocZ));

		return transform;
	}
};


class GAME_API cInfoOfBuilding_Stat
{
public:
	int ID; // BuildingManager::ID

	int BuildingState; // EBuildingState

	float HealthPoint;

public:
	cInfoOfBuilding_Stat()
	{
		ID = 0;

		BuildingState = 1;

		HealthPoint = 100.0f;
	}
	~cInfoOfBuilding_Stat()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfBuilding_Stat& Info)
	{
		Stream << Info.ID << ' ';

		Stream << Info.BuildingState << ' ';

		Stream << Info.HealthPoint << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfBuilding_Stat& Info)
	{
		Stream >> Info.ID;

		Stream >> Info.BuildingState;

		Stream >> Info.HealthPoint;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfBuilding_Stat> ID: %d, BuildingState : %d, HealthPoint: %f \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID, BuildingState, HealthPoint);
	}

};

class GAME_API cInfoOfBuilding
{
public:
	int ID;

	cInfoOfBuilding_Spawn Spawn;
	cInfoOfBuilding_Stat Stat;

public:
	cInfoOfBuilding()
	{
		ID = 0;


	}
	cInfoOfBuilding(int ID, cInfoOfBuilding_Spawn Spawn, cInfoOfBuilding_Stat Stat)
		: ID(ID), Spawn(Spawn), Stat(Stat)
	{
	}
	~cInfoOfBuilding()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfBuilding& Info)
	{
		Stream << Info.ID << ' ';

		Stream << Info.Spawn << ' ';
		Stream << Info.Stat << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfBuilding& Info)
	{
		Stream >> Info.ID;

		Stream >> Info.Spawn;
		Stream >> Info.Stat;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		Spawn.PrintInfo();
		Stat.PrintInfo();
	}

};


class GAME_API cInfoOfEnemy_Spawn
{
public:
	int ID;

	int EnemyType;

public:
	cInfoOfEnemy_Spawn()
	{
		ID = 0;

		EnemyType = 0;
	}
	~cInfoOfEnemy_Spawn()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfEnemy_Spawn& Info)
	{
		Stream << Info.ID << ' ';

		Stream << Info.EnemyType << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfEnemy_Spawn& Info)
	{
		Stream >> Info.ID;

		Stream >> Info.EnemyType;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfEnemy_Spawn> ID: %d, EnemyType : %d \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID, EnemyType);
	}
};

class GAME_API cInfoOfEnemy_Animation
{
public:
	int ID;

	float RotX;
	float RotY;
	float RotZ;

	float LocX;
	float LocY;
	float LocZ;

	float TargetRotX;
	float TargetRotY;
	float TargetRotZ;

	// �ִϸ��̼� ������
	float VelocityX;
	float VelocityY;
	float VelocityZ;

	// FSM
	int State;

public:
	cInfoOfEnemy_Animation()
	{
		ID = 0;

		RotX = 0.0f;
		RotY = 0.0f;
		RotZ = 0.0f;

		LocX = 0.0f;
		LocY = 0.0f;
		LocZ = 0.0f;

		TargetRotX = 0.0f;
		TargetRotY = 0.0f;
		TargetRotZ = 0.0f;

		VelocityX = 0.0f;
		VelocityY = 0.0f;
		VelocityZ = 0.0f;

		State = 0;
	}
	~cInfoOfEnemy_Animation()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfEnemy_Animation& Info)
	{
		Stream << Info.ID << ' ';

		Stream << Info.RotX << ' ';
		Stream << Info.RotY << ' ';
		Stream << Info.RotZ << ' ';
		Stream << Info.LocX << ' ';
		Stream << Info.LocY << ' ';
		Stream << Info.LocZ << ' ';
		Stream << Info.TargetRotX << ' ';
		Stream << Info.TargetRotY << ' ';
		Stream << Info.TargetRotZ << ' ';

		Stream << Info.VelocityX << ' ';
		Stream << Info.VelocityY << ' ';
		Stream << Info.VelocityZ << ' ';
		Stream << Info.State << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfEnemy_Animation& Info)
	{
		Stream >> Info.ID;

		Stream >> Info.RotX;
		Stream >> Info.RotY;
		Stream >> Info.RotZ;
		Stream >> Info.LocX;
		Stream >> Info.LocY;
		Stream >> Info.LocZ;
		Stream >> Info.TargetRotX;
		Stream >> Info.TargetRotY;
		Stream >> Info.TargetRotZ;

		Stream >> Info.VelocityX;
		Stream >> Info.VelocityY;
		Stream >> Info.VelocityZ;
		Stream >> Info.State;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfEnemy_Animation> ID: %d, RotX: %f, RotY: %f, RotZ: %f, LocX: %f, LocY: %f, LocZ: %f, TargetRotX: %f, TargetRotY: %f, TargetRotZ: %f \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID, RotX, RotY, RotZ, LocX, LocY, LocZ, TargetRotX, TargetRotY, TargetRotZ);
		CONSOLE_LOG("%s%s<cInfoOfEnemy_Animation> VelocityX: %f, VelocityY: %f, VelocityZ: %f, State: %d \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), VelocityX, VelocityY, VelocityZ, State);
	}

	void SetActorTransform(const FTransform& Transform)
	{
		RotX = Transform.GetRotation().Rotator().Pitch;
		RotY = Transform.GetRotation().Rotator().Yaw;
		RotZ = Transform.GetRotation().Rotator().Roll;

		LocX = Transform.GetLocation().X;
		LocY = Transform.GetLocation().Y;
		LocZ = Transform.GetLocation().Z;
	}

	FTransform GetActorTransform()
	{
		FTransform transform;
		FQuat quat(FRotator(RotX, RotY, RotZ));
		transform.SetRotation(quat);
		transform.SetLocation(FVector(LocX, LocY, LocZ));

		return transform;
	}
};

class GAME_API cInfoOfEnemy_Stat
{
public:
	int ID;

	float HealthPoint;
	float MaxHealthPoint;

	float MoveSpeed;
	float AttackSpeed;

	float AttackPower;

	float SightRange;
	float DetectRange;
	float AttackRange;

public:
	cInfoOfEnemy_Stat()
	{
		ID = 0;

		HealthPoint = 100.0f;
		MaxHealthPoint = 100.0f;

		MoveSpeed = 10.0f;
		AttackSpeed = 1.0f;

		AttackPower = 10.0f;

		SightRange = 32.0f;
		DetectRange = 32.0f;
		AttackRange = 4.0f;
	}
	~cInfoOfEnemy_Stat()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfEnemy_Stat& Info)
	{
		Stream << Info.ID << ' ';

		Stream << Info.HealthPoint << ' ';
		Stream << Info.MaxHealthPoint << ' ';
		Stream << Info.MoveSpeed << ' ';
		Stream << Info.AttackSpeed << ' ';
		Stream << Info.AttackPower << ' ';
		Stream << Info.SightRange << ' ';
		Stream << Info.DetectRange << ' ';
		Stream << Info.AttackRange << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfEnemy_Stat& Info)
	{
		Stream >> Info.ID;

		Stream >> Info.HealthPoint;
		Stream >> Info.MaxHealthPoint;
		Stream >> Info.MoveSpeed;
		Stream >> Info.AttackSpeed;
		Stream >> Info.AttackPower;
		Stream >> Info.SightRange;
		Stream >> Info.DetectRange;
		Stream >> Info.AttackRange;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		CONSOLE_LOG("%s%s<cInfoOfEnemy_Stat> ID: %d, HealthPoint: %f, MaxHealthPoint : %f, MoveSpeed: %f, AttackSpeed: %f, AttackPower: %f, SightRange: %f, DetectRange: %f, AttackRange: %f \n",
			TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID, HealthPoint, MaxHealthPoint, MoveSpeed, AttackSpeed, AttackPower, SightRange, DetectRange, AttackRange);
	}

};

class GAME_API cInfoOfEnemy
{
public:
	int ID;

	cInfoOfEnemy_Spawn Spawn;
	cInfoOfEnemy_Animation Animation;
	cInfoOfEnemy_Stat Stat;

public:
	cInfoOfEnemy()
	{
		ID = 0;


	}
	cInfoOfEnemy(int ID, cInfoOfEnemy_Spawn Spawn, cInfoOfEnemy_Animation Animation, cInfoOfEnemy_Stat Stat)
		: ID(ID), Spawn(Spawn), Animation(Animation), Stat(Stat)
	{
	}
	~cInfoOfEnemy()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfEnemy& Info)
	{
		Stream << Info.ID << ' ';

		Stream << Info.Spawn << ' ';
		Stream << Info.Animation << ' ';
		Stream << Info.Stat << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfEnemy& Info)
	{
		Stream >> Info.ID;

		Stream >> Info.Spawn;
		Stream >> Info.Animation;
		Stream >> Info.Stat;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		Spawn.PrintInfo();
		Animation.PrintInfo();
		Stat.PrintInfo();
	}

};