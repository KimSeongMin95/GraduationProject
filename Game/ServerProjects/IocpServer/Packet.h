#pragma once

#include <iostream>
#include <map>

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



	/** �÷��̾ ������ �����ϸ� -> (���Ŀ� ����) �÷��̾ MainScreenWidget���� Online ��ư�� ���� �����ϸ�
	Client:
		Send [ACCEPT_PLAYER]: ���� �˸�
		Recv [ACCEPT_PLAYER]: �ش� Ŭ���̾�Ʈ�� SocketID
	Server:
		Recv [ACCEPT_PLAYER]: O
		Send [ACCEPT_PLAYER]: �ش� Ŭ���̾�Ʈ�� SocketID
	*/
	ACCEPT_PLAYER,

	/** OnlineWidget���� CreateWaitingRoom ��ư�� ���� ������ �����ϸ�
	Client:
		Send [CREATE_WAITING_ROOM]: ������ ������ �⺻ ����
		Recv []: X
	Server:
		Recv [CREATE_WAITING_ROOM]: Games[InfoOfGame.Leader] = InfoOfGame;�� ���� ������ ����
		Send []: X
	*/
	CREATE_WAITING_ROOM,

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

struct stInfoOfGame
{
	string State;
	string Title;
	int Leader = 0; // ������ SocketID
	int Stage = 1;
	int MaxOfNum = 100;
	int CurOfNum = 0;

	string IPv4OfLeader;
	std::map<int, bool> SocketIDOfPlayers;
};












class cCharacter {
public:
	cCharacter();
	~cCharacter();

	// ���� ���̵�
	int		SessionId;

	// ��ġ
	float	X;
	float	Y;
	float	Z;

	// ȸ����
	float	Yaw;
	float	Pitch;
	float	Roll;

	// �ӵ�
	float VX;
	float VY;
	float VZ;

	// �Ӽ�
	bool	IsAlive;
	float	HealthValue;
	bool	IsAttacking;

	friend ostream& operator<<(ostream& stream, cCharacter& info)
	{
		stream << info.SessionId << endl;
		stream << info.X << endl;
		stream << info.Y << endl;
		stream << info.Z << endl;
		stream << info.VX << endl;
		stream << info.VY << endl;
		stream << info.VZ << endl;
		stream << info.Yaw << endl;
		stream << info.Pitch << endl;
		stream << info.Roll << endl;
		stream << info.IsAlive << endl;
		stream << info.HealthValue << endl;
		stream << info.IsAttacking << endl;

		return stream;
	}

	friend istream& operator>>(istream& stream, cCharacter& info)
	{
		stream >> info.SessionId;
		stream >> info.X;
		stream >> info.Y;
		stream >> info.Z;
		stream >> info.VX;
		stream >> info.VY;
		stream >> info.VZ;
		stream >> info.Yaw;
		stream >> info.Pitch;
		stream >> info.Roll;
		stream >> info.IsAlive;
		stream >> info.HealthValue;
		stream >> info.IsAttacking;

		return stream;
	}
};

class cCharactersInfo
{
public:
	cCharactersInfo();
	~cCharactersInfo();

	map<int, cCharacter> players;

	friend ostream& operator<<(ostream& stream, cCharactersInfo& info)
	{
		stream << info.players.size() << endl;
		for (auto& kvp : info.players)
		{
			stream << kvp.first << endl;
			stream << kvp.second << endl;
		}

		return stream;
	}

	friend istream& operator>>(istream& stream, cCharactersInfo& info)
	{
		int nPlayers = 0;
		int SessionId = 0;
		cCharacter Player;
		info.players.clear();

		stream >> nPlayers;
		for (int i = 0; i < nPlayers; i++)
		{
			stream >> SessionId;
			stream >> Player;
			info.players[SessionId] = Player;
		}

		return stream;
	}
};

class CommonClass
{
public:
	CommonClass();
	~CommonClass();
};