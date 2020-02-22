#pragma once

// 멀티바이트 집합 사용시 define
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// winsock2 사용을 위해 아래 코멘트 추가
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <iostream>
#include <process.h>
#include <sstream>
#include <algorithm>
#include <string>

#include <iostream>
#include <map>
#include <sstream>

using namespace std;

#define	MAX_BUFFER		4096
#define SERVER_PORT		8000

// IOCP 소켓 구조체
struct stSOCKETINFO
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;
	SOCKET			socket;
	char			messageBuffer[MAX_BUFFER];
	int				recvBytes;
	int				sendBytes;

	string			IPv4Addr; // 메인 클라이언트의 IP 주소
	int				Port;	  // 메인 클라이언트의 Port 주소
};

//// 쓰면 간단해지나 Custom이므로 유효성을 보장할 수 없으므로 아주 나중에 사용해볼 것.
//template <typename T>
//class cThreadSafeMap
//{
//private:
//	std::map<SOCKET, T> m;
//	CRITICAL_SECTION cs;
//
//public:
//	cThreadSafeMap() { InitializeCriticalSection(&cs); }
//	~cThreadSafeMap() { DeleteCriticalSection(&cs); }
//};

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


	///////////////////////////////////////////
	// Main Server / Main Clients
	///////////////////////////////////////////

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

	/** 대기방 정보 변경시 */
	WAITING_GAME,

	/** 플레이어가 Join 버튼으로 대기방에 들어가면
	Client:
		Send [JOIN_WAITING_ROOM]: Join한 대기방 Leader의 SocketID와 MyInfo
		Recv [JOIN_WAITING_ROOM]: Join한 대기방 cInfoOfGame
	Server:
		Recv [JOIN_WAITING_ROOM]: InfoOfGames의 Players에 해당 클라이언트 삽입
		Send [JOIN_WAITING_ROOM]: 해당 대기방의 cInfoOfGame
		Send [PLAYER_JOINED_WAITING_ROOM] to 대기방의 다른 플레이어들: 해당 대기방의 cInfoOfGame
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
		Send [MODIFY_WAITING_GAME]: cInfoOfGame(Title, Stage, Maximum)
		Recv [MODIFY_WAITING_GAME]: cInfoOfGame(Title, Stage, Maximum)
	Server:
		Recv [MODIFY_WAITING_GAME]: InfoOfGames에 cInfoOfGame(Title, Stage, Maximum) 적용
		Send [MODIFY_WAITING_GAME] to 플레이어들(방장 제외): cInfoOfGame(Title, Stage, Maximum)
	*/
	MODIFY_WAITING_GAME,

	/** 방장이 대기방에서 게임을 시작하면
	Client:
		Send [START_WAITING_GAME]: O
		Recv [START_WAITING_GAME]: O
	Server:
		Recv [START_WAITING_GAME] by 방장: O
		Send [START_WAITING_GAME] to 플레이어들(방장 제외): O
	*/
	START_WAITING_GAME,


	///////////////////////////////////////////
	// Game Server / Game Clients
	///////////////////////////////////////////

	/** 방장이 대기방에서 게임을 시작하고 CountStartedGame()에서 게임 서버 초기화에 성공하면
	Client:
		Send [ACTIVATE_GAME_SERVER]: MyInfo에 PortOfGameServer를 저장하고 MyInfo를 전송
		Recv []: X
	Server:
		Recv [ACTIVATE_GAME_SERVER] by 방장: 수신한 cInfoOfPlayer를 InfoOfClients와 InfoOfGames의 Leader에 적용
		Send []: X
	*/
	ACTIVATE_GAME_SERVER,

	/** 참가자가 게임 클라이언트를 게임 서버와 연결시키기 위해 게임 서버 정보를 요청
	Client:
		Send [REQUEST_INFO_OF_GAME_SERVER]: MyInfo의 LeaderSocketByMainServer
		Recv [REQUEST_INFO_OF_GAME_SERVER]: cInfoOfPlayer를 받고 IPv4Addr와 PortOfGameServer를 획득
	Server:
		Recv [REQUEST_INFO_OF_GAME_SERVER]: LeaderSocketByMainServer
		Send [REQUEST_INFO_OF_GAME_SERVER]: cInfoOfPlayer infoOfPlayer = InfoOfClients.at((SOCKET)leaderSocketByMainServer);하고 infoOfPlayer 전송
	*/
	REQUEST_INFO_OF_GAME_SERVER,



	JOIN_PLAYING_GAME,	  // 어떤 플레이어가 진행중인 게임에 들어올 때: 해당 게임의 플레이어들과 FIND_GAMES인 플레이어들에게 브로드캐스팅 해야 함.

	START_PLAYING_GAME, // 플레이중인 게임 시작하기

	EXIT_PLAYING_GAME,	// 플레이어가 진행중인 게임을 종료할 때: 
};



class cInfoOfPlayer
{
public:
	string ID;
	string IPv4Addr; // IP 번호

	int SocketByMainServer; // 메인 서버로부터 부여된 클라이언트의 소켓 번호
	int SocketByGameServer; // 게임 서버를 구동하는 방장으로부터 부여된 소켓 번호

	int PortOfMainClient; // 메인 클라이언트가 메인 서버와 연결된 포트 번호
	int PortOfGameServer; // 방장으로부터 구동된 게임 서버의 포트 번호
	int PortOfGameClient; // 게임 클라이언트가 게임 서버와 연결된 포트 번호

	int LeaderSocketByMainServer; // 메인 서버로부터 부여된 방장의 소켓 번호

public:
	cInfoOfPlayer()
	{
		ID = "NULL";
		IPv4Addr = "NULL";
		SocketByMainServer = 0;
		SocketByGameServer = 0;
		PortOfMainClient = 0;
		PortOfGameServer = 0;
		PortOfGameClient = 0;
		LeaderSocketByMainServer = 0;
	}
	~cInfoOfPlayer()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, cInfoOfPlayer& Info)
	{
		Stream << ReplaceCharInString(Info.ID, ' ', '_') << endl;
		Stream << ReplaceCharInString(Info.IPv4Addr, ' ', '_') << endl;
		Stream << Info.SocketByMainServer << endl;
		Stream << Info.SocketByGameServer << endl;
		Stream << Info.PortOfMainClient << endl;
		Stream << Info.PortOfGameServer << endl;
		Stream << Info.PortOfGameClient << endl;
		Stream << Info.LeaderSocketByMainServer << endl;

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, cInfoOfPlayer& Info)
	{
		Stream >> Info.ID;
		Info.ID = ReplaceCharInString(Info.ID, '_', ' ');
		Stream >> Info.IPv4Addr;
		Info.IPv4Addr = ReplaceCharInString(Info.IPv4Addr, '_', ' ');
		Stream >> Info.SocketByMainServer;
		Stream >> Info.SocketByGameServer;
		Stream >> Info.PortOfMainClient;
		Stream >> Info.PortOfGameServer;
		Stream >> Info.PortOfGameClient;
		Stream >> Info.LeaderSocketByMainServer;

		return Stream;
	}

	// Log
	void PrintInfo(const char* Space = "    ", const char* Space2 = "")
	{
		printf_s("%s%s<cInfoOfPlayer> ID: %s, IPv4Addr: %s, SocketByMainServer: %d, SocketByGameServer: %d, PortOfMainClient: %d, PortOfGameServer: %d, PortOfGameClient: %d, LeaderSocketByMainServer: %d\n", 
			Space, Space2, ID.c_str(), IPv4Addr.c_str(), SocketByMainServer, SocketByGameServer, PortOfMainClient, PortOfGameServer, PortOfGameClient, LeaderSocketByMainServer);
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

