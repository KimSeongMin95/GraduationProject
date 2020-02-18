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

	/** �÷��̾ OnlineWidget���� �α����Ͽ� OnlineGameWidget���� �����ϸ�
	Client:
		Send [FIND_GAMES]: O
		Recv [FIND_GAMES]: InfoOfGames�� ��� cInfoOfGame
	Server:
		Recv []: X
		Send [FIND_GAMES]: InfoOfGames�� ��� cInfoOfGame
	*/
	FIND_GAMES,

	WAITING_GAME, 

	/** �÷��̾ Join ��ư���� ���濡 ����
	Client:
		Send [JOIN_WAITING_GAME]: Join�� ���� Leader�� SocketID�� MyInfo
		Recv [WAITING_GAME]: Join�� ���� cInfoOfGame
	Server:
		Recv [JOIN_WAITING_GAME]: InfoOfGames�� Players�� �ش� Ŭ���̾�Ʈ ����
		Send [WAITING_GAME] to ����: �ش� ������ cInfoOfGame
		Send [WAITING_GAME] to �÷��̾��(�ش� Ŭ���̾�Ʈ ����): �ش� ������ cInfoOfGame
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
	DESTROY_WAITING_ROOM,

	/** ������ �ƴ� ������ �÷��̾ ���濡�� ������
	Client:
		Send [EXIT_WAITING_ROOM]: Exit�� ���� Leader�� SocketID�� MyInfo
		Recv [WAITING_GAME]: Exit�� ���� cInfoOfGame
	Server:
		Recv [EXIT_WAITING_ROOM]: InfoOfGames�� Players�� �ش� Ŭ���̾�Ʈ ����
		Send [WAITING_GAME] to ����: �ش� ������ cInfoOfGame
		Send [WAITING_GAME] to �÷��̾��(�ش� Ŭ���̾�Ʈ ������): �ش� ������ cInfoOfGame
	*/
	EXIT_WAITING_ROOM,



	/** ������ ���濡�� Title�̳� Stage�� MaxOfNum�� �����ϸ�
	Client:
		Send [MODIFY_WAITING_ROOM]: Title, Stage, MaxOfNum
		Recv [MODIFY_WAITING_ROOM]: Title, Stage, MaxOfNum
	Server:
		Recv [MODIFY_WAITING_ROOM]: Games�� Title, Stage, MaxOfNum ����
		Send [MODIFY_WAITING_ROOM] to ���� �÷��̾��(����x): Title, Stage, MaxOfNum
	*/
	MODIFY_WAITING_ROOM,




	

	

	START_WAITING_ROOM,   // ������ ���濡�� ������ ������ ��: ������ ������ ������ �÷��̾�鿡�� ��ε�ĳ���� �ؾ� ��.

	JOIN_PLAYING_GAME,	  // � �÷��̾ �������� ���ӿ� ���� ��: �ش� ������ �÷��̾��� FIND_GAMES�� �÷��̾�鿡�� ��ε�ĳ���� �ؾ� ��.

	START_PLAYING_GAME, // �÷������� ���� �����ϱ�

	EXIT_GAME,		  // �÷��̾ ������ ������ ��: 
};

class GAME_API cInfoOfPlayer
{
public:
	string ID;
	string IPv4Addr; // IP ��ȣ
	int SocketByServer; // Online���� �����κ��� �ο��� ���� ��ȣ
	int SocketByLeader; // Game���� �������κ��� �ο��� ���� ��ȣ
	int PortByServer; // Online���� �����κ��� �ο��� ���� ��ȣ
	int PortByLeader; // Game���� �������κ��� �ο��� ���� ��ȣ

public:
	cInfoOfPlayer()
	{
		ID = "NULL";
		IPv4Addr = "NULL";
		SocketByServer = -1;
		SocketByLeader = -1;
		PortByServer = -1;
		PortByLeader = -1;
	}
	~cInfoOfPlayer()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfPlayer& Info)
	{
		Stream << Info.ID << endl;
		Stream << Info.IPv4Addr << endl;
		Stream << Info.SocketByServer << endl;
		Stream << Info.SocketByLeader << endl;
		Stream << Info.PortByServer << endl;
		Stream << Info.PortByLeader << endl;

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfPlayer& Info)
	{
		Stream >> Info.ID;
		Stream >> Info.IPv4Addr;
		Stream >> Info.SocketByServer;
		Stream >> Info.SocketByLeader;
		Stream >> Info.PortByServer;
		Stream >> Info.PortByLeader;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s%s<cInfoOfPlayer> ID: %s, IPv4Addr: %s, SocketByServer: %d, SocketByLeader: %d, PortByServer: %d, PortByLeader: %d"),
			Space, Space2, ANSI_TO_TCHAR(ID.c_str()), ANSI_TO_TCHAR(IPv4Addr.c_str()), SocketByServer, SocketByLeader, PortByServer, PortByLeader);
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
		Stream << Info.Players.size() << endl;
		for (auto& kvp : Info.Players)
		{
			Stream << kvp.first << endl;
			Stream << kvp.second << endl;
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
			UE_LOG(LogTemp, Warning, TEXT("%s%skey: %d"), Space, Space2, kvp.first);
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
		Stream << Info.State << endl;
		Stream << Info.Title << endl;
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
		Stream >> Info.Title;
		Stream >> Info.Stage;
		Stream >> Info.nMax;
		Stream >> Info.Leader;
		Stream >> Info.Players;

		return Stream;
	}

	// Log
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T("    "))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s<cInfoOfGame> Start"), Space);
		UE_LOG(LogTemp, Warning, TEXT("%s%sState: %s, Title: %s, Stage: %d, nMax: %d"), Space, Space2, ANSI_TO_TCHAR(State.c_str()), ANSI_TO_TCHAR(Title.c_str()), Stage, nMax);
		Leader.PrintInfo(Space, Space2);
		Players.PrintInfo(Space, Space2);
		UE_LOG(LogTemp, Warning, TEXT("%s<cInfoOfGame> End"), Space);
	}
};

