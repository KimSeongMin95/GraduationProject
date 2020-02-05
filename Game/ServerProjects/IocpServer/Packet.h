#pragma once

#include <iostream>
#include <map>
#include <list>

using namespace std;

enum EPacketType
{
	ACCEPT_PLAYER,		  // 플레이어가 게임을 실행할 때: 해당 플레이어에게 너의 소켓 ID가 이거라고 답장해야 함.
	CREATE_WAITING_ROOM,  // OnlineWidget에서 CreateWaitingRoom 버튼을 눌러 대기방을 생성할 때: 
	FIND_GAMES,			  // MainScreenWidget에서 Online 버튼을 눌러 게임을 찾을 때: 해당 플레이어에게 모든 게임들을 전송
	MODIFY_WAITING_ROOM,  // 방장이 대기방에서 Title이나 Stage나 MaxOfNum을 변경할 때: FIND_GAMES와 방장을 제외한 대기방인 플레이어들에게 브로드캐스팅 해야 함.
	JOIN_WAITING_ROOM,	  // 어떤 플레이어가 대기방에 들어올 때: FIND_GAMES와 대기방인 플레이어들에게 브로드캐스팅 해야 함.
	JOIN_PLAYING_GAME,	  // 어떤 플레이어가 진행중인 게임에 들어올 때: 해당 게임의 플레이어들과 FIND_GAMES인 플레이어들에게 브로드캐스팅 해야 함.
	DESTROY_WAITING_ROOM, // 방장이 대기방을 종료할 때: FIND_GAMES와 방장을 제외한 대기방인 플레이어들에게 브로드캐스팅 해야 함.
	EXIT_WAITING_ROOM,	  // 방장이 아닌 대기방인 플레이어가 대기방에서 나갈 때: FIND_GAMES와 대기방인 플레이어들에게 브로드캐스팅 해야 함.

	START_WAITING_ROOM,   // 방장이 대기방에서 게임을 시작할 때: 방장을 제외한 대기방인 플레이어들에게 브로드캐스팅 해야 함.
	EXIT_PLAYER,		  // 플레이어가 게임을 종료할 때: 
};

struct stInfoOfGame
{
	string State;
	string Title;
	int Leader = 0; // 방장의 SocketID
	int Stage = 1;
	int MaxOfNum = 100;

	string IPv4OfLeader;
	std::list<int> SocketIDOfPlayers;
};












class cCharacter {
public:
	cCharacter();
	~cCharacter();

	// 세션 아이디
	int		SessionId;

	// 위치
	float	X;
	float	Y;
	float	Z;

	// 회전값
	float	Yaw;
	float	Pitch;
	float	Roll;

	// 속도
	float VX;
	float VY;
	float VZ;

	// 속성
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