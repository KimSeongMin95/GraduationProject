#pragma once

#include <iostream>
#include <map>
#include <list>

using namespace std;

enum EPacketType
{
	ACCEPT_PLAYER,		  // �÷��̾ ������ ������ ��: �ش� �÷��̾�� ���� ���� ID�� �̰Ŷ�� �����ؾ� ��.
	CREATE_WAITING_ROOM,  // OnlineWidget���� CreateWaitingRoom ��ư�� ���� ������ ������ ��: 
	FIND_GAMES,			  // MainScreenWidget���� Online ��ư�� ���� ������ ã�� ��: �ش� �÷��̾�� ��� ���ӵ��� ����
	MODIFY_WAITING_ROOM,  // ������ ���濡�� Title�̳� Stage�� MaxOfNum�� ������ ��: FIND_GAMES�� ������ ������ ������ �÷��̾�鿡�� ��ε�ĳ���� �ؾ� ��.
	JOIN_WAITING_ROOM,	  // � �÷��̾ ���濡 ���� ��: FIND_GAMES�� ������ �÷��̾�鿡�� ��ε�ĳ���� �ؾ� ��.
	JOIN_PLAYING_GAME,	  // � �÷��̾ �������� ���ӿ� ���� ��: �ش� ������ �÷��̾��� FIND_GAMES�� �÷��̾�鿡�� ��ε�ĳ���� �ؾ� ��.
	DESTROY_WAITING_ROOM, // ������ ������ ������ ��: FIND_GAMES�� ������ ������ ������ �÷��̾�鿡�� ��ε�ĳ���� �ؾ� ��.
	EXIT_WAITING_ROOM,	  // ������ �ƴ� ������ �÷��̾ ���濡�� ���� ��: FIND_GAMES�� ������ �÷��̾�鿡�� ��ε�ĳ���� �ؾ� ��.

	START_WAITING_ROOM,   // ������ ���濡�� ������ ������ ��: ������ ������ ������ �÷��̾�鿡�� ��ε�ĳ���� �ؾ� ��.
	EXIT_PLAYER,		  // �÷��̾ ������ ������ ��: 
};

struct stInfoOfGame
{
	string State;
	string Title;
	int Leader = 0; // ������ SocketID
	int Stage = 1;
	int MaxOfNum = 100;

	string IPv4OfLeader;
	std::list<int> SocketIDOfPlayers;
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