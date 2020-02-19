#pragma once

#include <iostream>
#include <map>
#include <sstream>

using namespace std;

enum EPacketType
{
	/** ����
	Client:
		Send [EPacketType]:
		Recv [EPacketType]:
	Server:
		Recv [EPacketType]:
		Send [EPacketType]:
	*/



	/** �÷��̾ OnlineWidget���� LOGIN �ϸ�
	Client:
		Send [LOGIN]: MyInfo�� ID�� ������ �۽�
		Recv [LOGIN]: cInfoOfPlayer
	Server:
		Recv [LOGIN]: ���� cInfoOfPlayer�� IPv4Addr, SocketByServer, PortByServer�� �����ϰ� InfoOfClients�� ����
		Send [LOGIN]: cInfoOfPlayer
	*/
	LOGIN,

	/** OnlineGameWidget���� (Create Game)��ư�� ���� ������ �����ϸ�
	Client:
		Send [CREATE_GAME]: MyInfoOfGame.Leader�� MyInfo�� �����ϰ� �۽�
		Recv []: X
	Server:
		Recv [CREATE_GAME]: ���� cInfoOfGame�� InfoOfGames�� ����
		Send []: X
	*/
	CREATE_GAME,

	/** �÷��̾ MainScreenWidget���� Online ��ư�� ���� �����ϸ�
	Client:
		Send [FIND_GAMES]: ���� ���� �䱸
		Recv [FIND_GAMES]: ��� ������� ����
	Server:
		Recv []: X
		Send [FIND_GAMES]: ��� ������� ����
	*/
	FIND_GAMES,

	WAITING_GAME,

	/** �÷��̾ Join ��ư���� ���濡 ����
	Client:
		Send [JOIN_WAITING_ROOM]: Join�� ���� Leader�� SocketID�� MyInfo
		Recv [JOIN_WAITING_ROOM]: Join�� ���� cInfoOfGame
	Server:
		Recv [JOIN_WAITING_ROOM]: InfoOfGames�� Players�� �ش� Ŭ���̾�Ʈ ����
		Send [JOIN_WAITING_ROOM]: �ش� ������ cInfoOfGame
		Send [PLAYER_JOINED_WAITING_ROOM] to ������ �ٸ� �÷��̾��: �ش� ������ cInfoOfGame
	*/
	JOIN_WAITING_GAME,


	/** ������ ���濡�� Back ��ư�� ���� ������ �����ϸ�
	Client:
		Send [DESTROY_WAITING_ROOM]: O
		Recv [DESTROY_WAITING_ROOM]: ���� ���� �˸��� �ڷΰ��� ��ư Ȱ��ȭ
	Server:
		Recv [DESTROY_WAITING_ROOM] by ����: InfoOfGames.erase(pSocketInfo->socket);
		Send [DESTROY_WAITING_ROOM] to �÷��̾��(���� ����): O
	*/
	DESTROY_WAITING_GAME,

	/** ������ �ƴ� ������ �÷��̾ ���濡�� ������
	Client:
		Send [EXIT_WAITING_ROOM]: Exit�� ���� Leader�� SocketID�� MyInfo
		Recv [WAITING_GAME]: Exit�� ���� cInfoOfGame
	Server:
		Recv [EXIT_WAITING_ROOM]: InfoOfGames�� Players�� �ش� Ŭ���̾�Ʈ ����
		Send [WAITING_GAME] to ����: �ش� ������ cInfoOfGame
		Send [WAITING_GAME] to �÷��̾��(�ش� Ŭ���̾�Ʈ ������): �ش� ������ cInfoOfGame
	*/
	EXIT_WAITING_GAME,

	/** ������ ���濡�� Title�̳� Stage�� Maximum�� �����ϸ�
	Client:
		Send [MODIFY_WAITING_GAME]: cInfoOfGame(Title, Stage, Maximum)
		Recv [MODIFY_WAITING_GAME]: cInfoOfGame(Title, Stage, Maximum)
	Server:
		Recv [MODIFY_WAITING_GAME]: InfoOfGames�� cInfoOfGame(Title, Stage, Maximum) ����
		Send [MODIFY_WAITING_GAME] to �÷��̾��(���� ����): cInfoOfGame(Title, Stage, Maximum)
	*/
	MODIFY_WAITING_GAME,

	/** ������ ���濡�� ������ �����ϸ�
	Client:
		Send [START_WAITING_GAME]: O
		Recv [START_WAITING_GAME]: O
	Server:
		Recv [START_WAITING_GAME] by ����: O
		Send [START_WAITING_GAME] to �÷��̾��(���� ����): O
	*/
	START_WAITING_GAME,




	JOIN_PLAYING_GAME,	  // � �÷��̾ �������� ���ӿ� ���� ��: �ش� ������ �÷��̾��� FIND_GAMES�� �÷��̾�鿡�� ��ε�ĳ���� �ؾ� ��.

	START_PLAYING_GAME, // �÷������� ���� �����ϱ�

	EXIT_PLAYING_GAME,	// �÷��̾ �������� ������ ������ ��: 
};



class cInfoOfPlayer
{
public:
	string ID;
	string IPv4Addr; // IP ��ȣ
	int SocketByServer; // Online���� �����κ��� �ο��� ���� ��ȣ
	int SocketByLeader; // Game���� �������κ��� �ο��� ���� ��ȣ
	int PortByServer; // Online���� �����κ��� �ο��� ���� ��ȣ
	int PortByLeader; // Game���� �������κ��� �ο��� ���� ��ȣ
	int SocketByServerOfLeader; // �����κ��� �ο��� ���� ������ ���� ��ȣ

public:
	cInfoOfPlayer()
	{
		ID = "NULL";
		IPv4Addr = "NULL";
		SocketByServer = 0;
		SocketByLeader = 0;
		PortByServer = 0;
		PortByLeader = 0;
		SocketByServerOfLeader = 0;
	}
	~cInfoOfPlayer()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfPlayer& Info)
	{
		Stream << ReplaceCharInString(Info.ID, ' ', '_') << endl;
		Stream << ReplaceCharInString(Info.IPv4Addr, ' ', '_') << endl;
		Stream << Info.SocketByServer << endl;
		Stream << Info.SocketByLeader << endl;
		Stream << Info.PortByServer << endl;
		Stream << Info.PortByLeader << endl;
		Stream << Info.SocketByServerOfLeader << endl;

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfPlayer& Info)
	{
		Stream >> Info.ID;
		Info.ID = ReplaceCharInString(Info.ID, '_', ' ');
		Stream >> Info.IPv4Addr;
		Info.IPv4Addr = ReplaceCharInString(Info.IPv4Addr, '_', ' ');
		Stream >> Info.SocketByServer;
		Stream >> Info.SocketByLeader;
		Stream >> Info.PortByServer;
		Stream >> Info.PortByLeader;
		Stream >> Info.SocketByServerOfLeader;

		return Stream;
	}

	// Log
	void PrintInfo(const char* Space = "    ", const char* Space2 = "")
	{
		printf_s("%s%s<cInfoOfPlayer> ID: %s, IPv4Addr: %s, SocketByServer: %d, SocketByLeader: %d, PortByServer: %d, PortByLeader: %d, SocketByServerOfLeader: %d\n", 
			Space, Space2, ID.c_str(), IPv4Addr.c_str(), SocketByServer, SocketByLeader, PortByServer, PortByLeader, SocketByServerOfLeader);
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

