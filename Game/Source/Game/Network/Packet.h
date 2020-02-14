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





	/** ������ ���濡�� Title�̳� Stage�� MaxOfNum�� �����ϸ�
	Client:
		Send [MODIFY_WAITING_ROOM]: Title, Stage, MaxOfNum
		Recv [MODIFY_WAITING_ROOM]: Title, Stage, MaxOfNum
	Server:
		Recv [MODIFY_WAITING_ROOM]: Games�� Title, Stage, MaxOfNum ����
		Send [MODIFY_WAITING_ROOM] to ���� �÷��̾��(����x): Title, Stage, MaxOfNum
	*/
	MODIFY_WAITING_ROOM,

	/** �÷��̾ Join ��ư���� ���濡 ����
	Client:
		Send [JOIN_WAITING_ROOM]: Join�� ���� ���� �˸�
		Recv [JOIN_WAITING_ROOM]: Join�� ������ ��� ����
	Server:
		Recv [JOIN_WAITING_ROOM]: Games.SocketIDOfPlayers�� �ش� Ŭ���̾�Ʈ�� SocketID �߰�
		Send [JOIN_WAITING_ROOM]: �ش� ������ ��� ����
		Send [PLAYER_JOINED_WAITING_ROOM] to ������ �ٸ� �÷��̾��: �ش� Ŭ���̾�Ʈ SocketID
	*/
	JOIN_WAITING_ROOM,

	/** �ٸ� �÷��̾ �÷��̾��� ���濡 ������
	Client:
		Send []: X
		Recv [PLAYER_JOINED_WAITING_ROOM]: ���濡 ������ �ٸ� �÷��̾��� SocketID
	Server:
		Recv []: X
		Send []: JOIN_WAITING_ROOM���� �۽�
	*/
	PLAYER_JOINED_WAITING_ROOM,
	
	/** ������ �ƴ� ������ �÷��̾ ���濡�� ������
	Client:
		Send [EXIT_WAITING_ROOM]: Exit�� ���� ���� �˸�
		Recv []: X
	Server:
		Recv [EXIT_WAITING_ROOM]: Games.SocketIDOfPlayers�� �ش� Ŭ���̾�Ʈ�� SocketID ����
		Send [PLAYER_EXITED_WAITING_ROOM] to ������ �ٸ� �÷��̾��: �ش� Ŭ���̾�Ʈ SocketID
	*/
	EXIT_WAITING_ROOM,

	/** ������ �÷��̾ ������
	Client:
		Send []: X
		Recv [PLAYER_EXITED_WAITING_ROOM]: ���濡�� ���� �ٸ� �÷��̾��� SocketID
	Server:
		Recv []: X
		Send []: EXIT_WAITING_ROOM���� �۽�
	*/
	PLAYER_EXITED_WAITING_ROOM,


	/** ���濡 ������ ������ �÷��̾ �����ϴ��� Ȯ��
	Client:
		Send [CHECK_PLAYER_IN_WAITING_ROOM]: SocketIDLeader�� mapPlayers�� ��� keyValue�� socketID
		Recv [CHECK_PLAYER_IN_WAITING_ROOM]: ������ �������� �ʴ� �÷��̾��� socketID��
	Server:
		Recv [CHECK_PLAYER_IN_WAITING_ROOM]: SocketIDLeader�� mapPlayers�� ��� keyValue�� socketID
		Send [CHECK_PLAYER_IN_WAITING_ROOM]: Games.at(socketIDOfLeader).SocketIDOfPlayers�� ������ �������� �ʴ� �÷��̾��� socketID��
	*/
	CHECK_PLAYER_IN_WAITING_ROOM,


	
	DESTROY_WAITING_ROOM, // ������ ������ ������ ��: FIND_GAMES�� ������ ������ ������ �÷��̾�鿡�� ��ε�ĳ���� �ؾ� ��.

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
};

