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

	/** 플레이어가 OnlineWidget에서 로그인하여 OnlineGameWidget으로 진입하면
	Client:
		Send [FIND_GAMES]: O
		Recv [FIND_GAMES]: InfoOfGames의 모든 cInfoOfGame
	Server:
		Recv []: X
		Send [FIND_GAMES]: InfoOfGames의 모든 cInfoOfGame
	*/
	FIND_GAMES,

	WAITING_GAME, 

	/** 플레이어가 Join 버튼으로 대기방에 들어가면
	Client:
		Send [JOIN_WAITING_GAME]: Join한 대기방 Leader의 SocketID와 MyInfo
		Recv [WAITING_GAME]: Join한 대기방 cInfoOfGame
	Server:
		Recv [JOIN_WAITING_GAME]: InfoOfGames의 Players에 해당 클라이언트 삽입
		Send [WAITING_GAME] to 방장: 해당 대기방의 cInfoOfGame
		Send [WAITING_GAME] to 플레이어들(해당 클라이언트 포함): 해당 대기방의 cInfoOfGame
	*/
	JOIN_WAITING_GAME,


	/** 방장이 대기방에서 Back 버튼을 눌러 대기방을 종료하면
	Client:
		Send [DESTROY_WAITING_ROOM]: O
		Recv [DESTROY_WAITING_ROOM]: 대기방 종료 알림에 뒤로가기 버튼 활성화
	Server:
		Recv [DESTROY_WAITING_ROOM] by 방장: InfoOfGames.erase(pSocketInfo->socket);
		Send [DESTROY_WAITING_ROOM] to 플레이어들(방장 제외): O
	*/
	DESTROY_WAITING_GAME,

	/** 방장이 아닌 대기방인 플레이어가 대기방에서 나가면
	Client:
		Send [EXIT_WAITING_ROOM]: Exit한 대기방 Leader의 SocketID와 MyInfo
		Recv [WAITING_GAME]: Exit한 대기방 cInfoOfGame
	Server:
		Recv [EXIT_WAITING_ROOM]: InfoOfGames의 Players에 해당 클라이언트 제거
		Send [WAITING_GAME] to 방장: 해당 대기방의 cInfoOfGame
		Send [WAITING_GAME] to 플레이어들(해당 클라이언트 미포함): 해당 대기방의 cInfoOfGame
	*/
	EXIT_WAITING_GAME,

	/** 방장이 대기방에서 Title이나 Stage나 Maximum을 수정하면
	Client:
		Send [MODIFY_WAITING_GAME]: MyInfoOfGame(Title, Stage, Maximum)
		Recv [MODIFY_WAITING_GAME]: cInfoOfGame(Title, Stage, Maximum)
	Server:
		Recv [MODIFY_WAITING_GAME]: InfoOfGames에 cInfoOfGame(Title, Stage, Maximum) 적용
		Send [MODIFY_WAITING_GAME] to 플레이어들(방장 제외): cInfoOfGame(Title, Stage, Maximum)
	*/
	MODIFY_WAITING_GAME,




	

	

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
	int SocketByServerOfLeader; // 서버로부터 부여된 게임 방장의 소켓 번호

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
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s%s<cInfoOfPlayer> ID: %s, IPv4Addr: %s, SocketByServer: %d, SocketByLeader: %d, PortByServer: %d, PortByLeader: %d, SocketByServerOfLeader: %d"),
			Space, Space2, ANSI_TO_TCHAR(ID.c_str()), ANSI_TO_TCHAR(IPv4Addr.c_str()), SocketByServer, SocketByLeader, PortByServer, PortByLeader, SocketByServerOfLeader);
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
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T("    "))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s<cInfoOfGame> Start"), Space);
		UE_LOG(LogTemp, Warning, TEXT("%s%sState: %s, Title: %s, Stage: %d, nMax: %d"), Space, Space2, ANSI_TO_TCHAR(State.c_str()), ANSI_TO_TCHAR(Title.c_str()), Stage, nMax);
		Leader.PrintInfo(Space, Space2);
		Players.PrintInfo(Space, Space2);
		UE_LOG(LogTemp, Warning, TEXT("%s<cInfoOfGame> End"), Space);
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

