#pragma once

#include <iostream>
#include <map>
#include <sstream>


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

	/** 플레이어가 OnlineWidget에서 LOGIN 하면 
	Client:
		Send [LOGIN]: MyInfo에 ID를 저장후 송신
		Recv [LOGIN]: cInfoOfPlayer
	Server:
		Recv [LOGIN]: 받은 cInfoOfPlayer에 IPv4Addr, SocketByServer, PortByServer를 저장하고 InfoOfClients에 삽입
		Send [LOGIN]: cInfoOfPlayer
	*/
	LOGIN,

	/** OnlineGameWidget에서 (Create Game)버튼을 눌러 대기방을 생성하면
	Client:
		Send [CREATE_GAME]: MyInfoOfGame.Leader에 MyInfo를 저장하고 송신
		Recv []: X
	Server:
		Recv [CREATE_GAME]: 받은 cInfoOfGame를 InfoOfGames에 삽입
		Send []: X
	*/
	CREATE_GAME,




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
	
	/** 방장이 아닌 대기방인 플레이어가 대기방에서 나가면
	Client:
		Send [EXIT_WAITING_ROOM]: Exit한 대기방 나감 알림
		Recv []: X
	Server:
		Recv [EXIT_WAITING_ROOM]: Games.SocketIDOfPlayers에 해당 클라이언트의 SocketID 삭제
		Send [PLAYER_EXITED_WAITING_ROOM] to 대기방의 다른 플레이어들: 해당 클라이언트 SocketID
	*/
	EXIT_WAITING_ROOM,

	/** 대기방의 플레이어가 나가면
	Client:
		Send []: X
		Recv [PLAYER_EXITED_WAITING_ROOM]: 대기방에서 나간 다른 플레이어의 SocketID
	Server:
		Recv []: X
		Send []: EXIT_WAITING_ROOM에서 송신
	*/
	PLAYER_EXITED_WAITING_ROOM,


	/** 대기방에 들어오면 실제로 플레이어가 존재하는지 확인
	Client:
		Send [CHECK_PLAYER_IN_WAITING_ROOM]: SocketIDLeader와 mapPlayers의 모든 keyValue인 socketID
		Recv [CHECK_PLAYER_IN_WAITING_ROOM]: 실제로 존재하지 않는 플레이어의 socketID들
	Server:
		Recv [CHECK_PLAYER_IN_WAITING_ROOM]: SocketIDLeader와 mapPlayers의 모든 keyValue인 socketID
		Send [CHECK_PLAYER_IN_WAITING_ROOM]: Games.at(socketIDOfLeader).SocketIDOfPlayers에 실제로 존재하지 않는 플레이어의 socketID들
	*/
	CHECK_PLAYER_IN_WAITING_ROOM,


	
	DESTROY_WAITING_ROOM, // 방장이 대기방을 종료할 때: FIND_GAMES와 방장을 제외한 대기방인 플레이어들에게 브로드캐스팅 해야 함.

	START_WAITING_ROOM,   // 방장이 대기방에서 게임을 시작할 때: 방장을 제외한 대기방인 플레이어들에게 브로드캐스팅 해야 함.

	JOIN_PLAYING_GAME,	  // 어떤 플레이어가 진행중인 게임에 들어올 때: 해당 게임의 플레이어들과 FIND_GAMES인 플레이어들에게 브로드캐스팅 해야 함.

	START_PLAYING_GAME, // 플레이중인 게임 시작하기

	EXIT_GAME,		  // 플레이어가 게임을 종료할 때: 
};

class GAME_API cInfoOfPlayer
{
public:
	string ID;
	string IPv4Addr; // IP 번호
	int SocketByServer; // Online에서 서버로부터 부여된 소켓 번호
	int SocketByLeader; // Game에서 방장으로부터 부여된 소켓 번호
	int PortByServer; // Online에서 서버로부터 부여된 소켓 번호
	int PortByLeader; // Game에서 방장으로부터 부여된 소켓 번호

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

		// 초기화
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
	int nMax; // 최대 플레이어 수 제한

	cInfoOfPlayer Leader; // 방장
	cInfoOfPlayers Players; // 방장을 제외한 참가자들

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

