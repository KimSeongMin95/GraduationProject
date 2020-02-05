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