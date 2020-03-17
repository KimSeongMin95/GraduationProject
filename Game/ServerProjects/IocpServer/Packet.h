#pragma once

// ��Ƽ����Ʈ ���� ���� define
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// winsock2 ����� ���� �Ʒ� �ڸ�Ʈ �߰�
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <iostream>
#include <process.h>
#include <sstream>
#include <algorithm>
#include <string>

#include <iostream>
#include <map>
#include <queue>
#include <deque>
#include <sstream>

using namespace std;

// �����ʹ� ũ��� �ִ� 4095���� �������� '\0'�� ���� ������ ���� ����
#define	MAX_BUFFER		4096
#define SERVER_PORT		8000

// IOCP ���� ����ü
struct stSOCKETINFO
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;
	SOCKET			socket;

	char			messageBuffer[MAX_BUFFER];
	int				recvBytes;
	int				sendBytes; // WSASend�� ������ �������� ����Ʈ ũ��
	int				sentBytes; // WSASend�� ���۵� �������� ����Ʈ ũ��

	string			IPv4Addr; // ���� Ŭ���̾�Ʈ�� IP �ּ�
	int				Port;	  // ���� Ŭ���̾�Ʈ�� Port �ּ�
};

//// ���� ���������� Custom�̹Ƿ� ��ȿ���� ������ �� �����Ƿ� ���� ���߿� ����غ� ��.
//template <typename T>
//class cThreadSafeMap
//{
//private:
//	std::map<SOCKET, T> m;
//	CRITICAL_SECTION cs;
//
//public:
//	cThreadSafeMap() { InitializeCriticalSection(&cs); }
//	~cThreadSafeMap() { DeleteCriticalSection(&cs); }
//};

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

	/** ����Ŭ���̾�Ʈ�� �ڽ��� �������� Pioneer ������ ������ ���Ӽ����� �ش� Pioneer�� ������ �ٸ� Pioneer���� ������ ��ε�ĳ����
	Game Client:
		Recv [INFO_OF_PIONEER]:
		Send [INFO_OF_PIONEER]:
	Game Server:
		Recv [INFO_OF_PIONEER]:
		Send [INFO_OF_PIONEER]:
	*/
	INFO_OF_PIONEER
};


class cInfoOfPlayer
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
		Stream << ReplaceCharInString(Info.ID, ' ', '_') << endl;
		Stream << ReplaceCharInString(Info.IPv4Addr, ' ', '_') << endl;
		Stream << Info.SocketByMainServer << endl;
		Stream << Info.SocketByGameServer << endl;
		Stream << Info.PortOfMainClient << endl;
		Stream << Info.PortOfGameServer << endl;
		Stream << Info.PortOfGameClient << endl;
		Stream << Info.LeaderSocketByMainServer << endl;

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
	void PrintInfo(const char* Space = "    ", const char* Space2 = "")
	{
		printf_s("%s%s<cInfoOfPlayer> ID: %s, IPv4Addr: %s, SocketByMainServer: %d, SocketByGameServer: %d, PortOfMainClient: %d, PortOfGameServer: %d, PortOfGameClient: %d, LeaderSocketByMainServer: %d\n", 
			Space, Space2, ID.c_str(), IPv4Addr.c_str(), SocketByMainServer, SocketByGameServer, PortOfMainClient, PortOfGameServer, PortOfGameClient, LeaderSocketByMainServer);
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

class cInfoOfPlayers
{
public:
	std::map<int, cInfoOfPlayer> Players;

public:
	cInfoOfPlayers() {}
	~cInfoOfPlayers() {}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfPlayers& Info)
	{
		Stream << Info.Players.size() << endl;
		for (auto& kvp : Info.Players)
		{
			Stream << kvp.first << endl;
			Stream << kvp.second << endl;
		}

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfPlayers& Info)
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
	void PrintInfo(const char* Space = "    ", const char* Space2 = "")
	{
		for (auto& kvp : Players)
		{
			printf_s("%s%skey: %d, ", Space, Space2, kvp.first);
			kvp.second.PrintInfo();
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


class cInfoOfGame
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
		Stream << ReplaceCharInString(Info.State, ' ', '_') << endl;
		Stream << ReplaceCharInString(Info.Title, ' ', '_') << endl;
		Stream << Info.Stage << endl;
		Stream << Info.nMax << endl;
		Stream << Info.Leader << endl;
		Stream << Info.Players << endl;

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
	void PrintInfo(const char* Space = "    ", const char* Space2 = "    ")
	{
		printf_s("%s<cInfoOfGame> Start\n", Space);
		printf_s("%s%sState: %s, Title: %s, Stage: %d, nMax: %d\n", Space, Space2, State.c_str(), Title.c_str(), Stage, nMax);
		Leader.PrintInfo(Space, Space2);
		Players.PrintInfo(Space, Space2);
		printf_s("%s<cInfoOfGame> End\n", Space);
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

