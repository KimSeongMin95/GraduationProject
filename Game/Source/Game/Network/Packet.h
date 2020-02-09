#pragma once

#include <iostream>
#include <map>
#include <list>

using namespace std;

enum EPacketType
{
	/** 설명
	Client:
		Send [EPacketType]: 
		Recv [EPacketType]: 
	Server:
		Recv [EPacketType]: 
		Send [EPacketType]: 
	*/


	
	/** 플레이어가 게임을 실행하면 -> (추후에 변경) 플레이어가 MainScreenWidget에서 Online 버튼을 눌러 진입하면
	Client:
		Send [ACCEPT_PLAYER]: 접속 알림
		Recv [ACCEPT_PLAYER]: 해당 클라이언트의 SocketID
	Server:
		Recv [ACCEPT_PLAYER]: O
		Send [ACCEPT_PLAYER]: 해당 클라이언트의 SocketID
	*/
	ACCEPT_PLAYER,

	/** OnlineWidget에서 CreateWaitingRoom 버튼을 눌러 대기방을 생성하면
	Client:
		Send [CREATE_WAITING_ROOM]: 생성한 대기방의 기본 정보
		Recv []: X
	Server:
		Recv [CREATE_WAITING_ROOM]: Games[InfoOfGame.Leader] = InfoOfGame;로 대기방 정보를 저장
		Send []: X
	*/
	CREATE_WAITING_ROOM,

	/** 플레이어가 MainScreenWidget에서 Online 버튼을 눌러 진입하면
	Client:
		Send [FIND_GAMES]: 대기방 정보 요구
		Recv [FIND_GAMES]: 모든 대기방들의 정보
	Server:
		Recv []: X
		Send [FIND_GAMES]: 모든 대기방들의 정보
	*/
	FIND_GAMES,

	/** 방장이 대기방에서 Title이나 Stage나 MaxOfNum을 수정하면
	Client:
		Send [MODIFY_WAITING_ROOM]: Title, Stage, MaxOfNum
		Recv [MODIFY_WAITING_ROOM]: Title, Stage, MaxOfNum
	Server:
		Recv [MODIFY_WAITING_ROOM]: Games에 Title, Stage, MaxOfNum 적용
		Send [MODIFY_WAITING_ROOM] to 대기방 플레이어들(방장x): Title, Stage, MaxOfNum
	*/
	MODIFY_WAITING_ROOM,

	/** 플레이어가 Join 버튼으로 대기방에 들어가면
	Client:
		Send [JOIN_WAITING_ROOM]: Join한 대기방 진입 알림
		Recv [JOIN_WAITING_ROOM]: Join한 대기방의 모든 정보
	Server:
		Recv [JOIN_WAITING_ROOM]: Games.SocketIDOfPlayers에 해당 클라이언트의 SocketID 추가
		Send [JOIN_WAITING_ROOM]: 해당 대기방의 모든 정보
		Send [PLAYER_JOINED_WAITING_ROOM] to 대기방의 다른 플레이어들: 해당 클라이언트 SocketID
	*/
	JOIN_WAITING_ROOM,

	/** 다른 플레이어가 플레이어의 대기방에 들어오면
	Client:
		Send []: X
		Recv [PLAYER_JOINED_WAITING_ROOM]: 대기방에 진입한 다른 플레이어의 SocketID
	Server:
		Recv []: X
		Send []: JOIN_WAITING_ROOM에서 송신
	*/
	PLAYER_JOINED_WAITING_ROOM,
	
	

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
	int CurOfNum = 0;
	int MaxOfNum = 100;

	string IPv4OfLeader;
	std::list<int> SocketIDOfPlayers; // 방장을 제외한 참가자들
};

