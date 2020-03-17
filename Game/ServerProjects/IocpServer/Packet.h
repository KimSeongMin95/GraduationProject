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
#include <queue>
#include <deque>
#include <sstream>

using namespace std;

// 데이터는 크기는 최대 4095여야 마지막에 '\0'가 들어가서 오류가 나지 않음
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
	int				sendBytes; // WSASend로 전송할 데이터의 바이트 크기
	int				sentBytes; // WSASend로 전송된 데이터의 바이트 크기

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
	/** 패킷 구조
	int PacketSize;
	int EPacketType;
	... Data;
	*/

	// 최소 [1 1 ] 사이즈와 타입까지 4바이트?
	// 최대 [4096(MAX_BUFFER) 1 ] 사이즈만 4바이트?

	/** 설명
	Main Client:
		Send [EPacketType]:
		Recv [EPacketType]:
	Main Server:
		Recv [EPacketType]:
		Send [EPacketType]:
	*/


	///////////////////////////////////////////
	// Main Server / Main Clients
	///////////////////////////////////////////

	/** 플레이어가 OnlineWidget에서 LOGIN 하면
	Main Client:
		Send [LOGIN]: 임시 객체인 cInfoOfPlayer infoOfPlayer에 ID를 저장후 송신
		Recv [LOGIN]: 받은 cInfoOfPlayer를 MyInfo에 대입
	Main Server:
		Recv [LOGIN]: 받은 cInfoOfPlayer에 IPv4Addr, SocketByServer, PortByServer를 저장하고 InfoOfClients에 삽입
		Send [LOGIN]: cInfoOfPlayer
	*/
	LOGIN,

	/** OnlineGameWidget에서 (Create Game)버튼을 눌러 대기방을 생성하면
	Main Client:
		Send [CREATE_GAME]: MyInfoOfGame.Leader에 MyInfo를 저장하고 송신
		Recv []: X
	Main Server:
		Recv [CREATE_GAME]: 받은 cInfoOfGame를 InfoOfGames에 삽입
		Send []: X
	*/
	CREATE_GAME,

	/** 플레이어가 MainScreenWidget에서 Online 버튼을 눌러 진입하면
	Main Client:
		Send [FIND_GAMES]: 대기방 정보 요구
		Recv [FIND_GAMES]: 모든 대기방들의 정보
	Main Server:
		Recv [FIND_GAMES]: O
		Send [FIND_GAMES]: InfoOfGames에 삽입되어 있는 모든 cInfoOfGame의 정보
	*/
	FIND_GAMES,

	/** 대기방 정보 변경시 */
	WAITING_GAME,

	/** 플레이어가 Join 버튼으로 게임방에 들어가면
	Main Client:
		Send [JOIN_ONLINE_GAME]: Join한 게임방 Leader의 SocketID를 대입한 MyInfo
		Recv [WAITING_GAME]: 받은 cInfoOfGame을 MyInfoOfGame에 대입
	Main Server:
		Recv [JOIN_ONLINE_GAME]: InfoOfClients에 LeaderSocketByMainServer를 대입하고 InfoOfGames의 Players에 해당 클라이언트 삽입
		Send [WAITING_GAME] to 방장: 해당 게임방의 cInfoOfGame
		Send [WAITING_GAME] to 게임방의 다른 플레이어들: 해당 게임방의 cInfoOfGame
	*/
	JOIN_ONLINE_GAME,

	/** 방장이 대기방에서 Back 버튼을 눌러 대기방을 종료하면
	Main Client:
		Send [DESTROY_WAITING_ROOM]: O, InitMyInfoOfGame();
		Recv [DESTROY_WAITING_ROOM]: 대기방 종료 알림에 뒤로가기 버튼 활성화, MyInfo의 특정 멤버변수들 초기화, InitMyInfoOfGame();
	Main Server:
		Recv [DESTROY_WAITING_ROOM] by 방장: InfoOfClients의 해당 클라이언트의 특정 멤버변수들 초기화, InfoOfGames.erase(pSocketInfo->socket);
		Send [DESTROY_WAITING_ROOM] to 플레이어들(방장 제외): O
	*/
	DESTROY_WAITING_GAME,

	/** 방장이 아닌 대기방인 플레이어가 대기방에서 나가면
	Main Client:
		Send [EXIT_WAITING_ROOM]: O, MyInfo의 특정 멤버변수들 초기화, InitMyInfoOfGame();
		Recv [WAITING_GAME]: 대기방 cInfoOfGame
	Main Server:
		Recv [EXIT_WAITING_ROOM]: InfoOfClients의 해당 클라이언트의 특정 멤버변수들 초기화, InfoOfGames의 Players에 해당 클라이언트 제거
		Send [WAITING_GAME] to 방장: 해당 대기방의 cInfoOfGame
		Send [WAITING_GAME] to 플레이어들(해당 클라이언트 미포함): 해당 대기방의 cInfoOfGame
	*/
	EXIT_WAITING_GAME,

	/** 방장이 대기방에서 Title이나 Stage나 Maximum을 수정하면
	Main Client:
		Send [MODIFY_WAITING_GAME]: cInfoOfGame(Title, Stage, Maximum)
		Recv [MODIFY_WAITING_GAME]: cInfoOfGame(Title, Stage, Maximum)
	Main Server:
		Recv [MODIFY_WAITING_GAME]: InfoOfGames에 cInfoOfGame(Title, Stage, Maximum) 적용
		Send [MODIFY_WAITING_GAME] to 플레이어들(방장 제외): cInfoOfGame(Title, Stage, Maximum)
	*/
	MODIFY_WAITING_GAME,

	/** 방장이 대기방에서 게임을 시작하면
	Main Client:
		Send [START_WAITING_GAME]: O
		Recv [START_WAITING_GAME]: O
	Main Server:
		Recv [START_WAITING_GAME] by 방장: O
		Send [START_WAITING_GAME] to 플레이어들(방장 제외): O
	*/
	START_WAITING_GAME,


	/** 방장이 대기방에서 게임을 시작하고 CountStartedGame()에서 게임 서버 초기화에 성공하면
	Main Client:
		Send [ACTIVATE_GAME_SERVER]: MyInfo에 PortOfGameServer를 저장하고 MyInfo를 전송, MyInfoOfGame.State = string("Playing");
		Recv [WAITING_GAME]: 대기방 cInfoOfGame
	Main Server:
		Recv [ACTIVATE_GAME_SERVER] by 방장: 수신한 cInfoOfPlayer를 InfoOfClients와 InfoOfGames의 Leader에 적용
		Send [WAITING_GAME] to 방장: 해당 대기방의 cInfoOfGame
		Send [WAITING_GAME] to 플레이어들: 해당 대기방의 cInfoOfGame
	*/
	ACTIVATE_GAME_SERVER,

	/** 참가자가 게임 클라이언트를 게임 서버와 연결시키기 위해 게임 서버 정보를 요청
	Main Client:
		Send [REQUEST_INFO_OF_GAME_SERVER]: O
		Recv [REQUEST_INFO_OF_GAME_SERVER]: cInfoOfPlayer를 받고 IPv4Addr와 PortOfGameServer를 획득
	Main Server:
		Recv [REQUEST_INFO_OF_GAME_SERVER]: O
		Send [REQUEST_INFO_OF_GAME_SERVER]: cInfoOfPlayer infoOfPlayer = InfoOfClients.at(leaderSocket);하고 infoOfPlayer 전송
	*/
	REQUEST_INFO_OF_GAME_SERVER,


	///////////////////////////////////////////
	// Game Server / Game Clients
	///////////////////////////////////////////

	/** 게임서버와 게임클라이언트가 연결되었다면
	Game Client:
		Recv [CONNECTED]: (순서4) 받은 cInfoOfPlayer를 ClientSocket::MyInfo에 저장
		Send [CONNECTED]: (순서1) ClientSocket::MyInfo를 전송
	Game Server:
		Recv [CONNECTED]: (순서2) 받은 cInfoOfPlayer에 SocketByGameServer, PortOfGameServer, PortOfGameClient를 대입하고 InfoOfClients에 삽입
		Send [CONNECTED]: (순서3) InfoOfClients에 삽입한 cInfoOfPlayer
	*/
	CONNECTED,

	/** 게임서버가 종료되면
	Game Client:
		Recv [DISCONNECTED]:
		Send [X]:
	Game Server:
		Recv [X]:
		Send [DISCONNECTED]:
	*/
	DISCONNECT,

	/** 클라이언트가 일정시간마다 ScoreBoard 정보를 요청
	Game Client:
		Recv [SCORE_BOARD]:
		Send [SCORE_BOARD]:
	Game Server:
		Recv [SCORE_BOARD]:
		Send [SCORE_BOARD]:
	*/
	SCORE_BOARD,

	/** 게임서버가 SpaceShip 정보를 계속 게임클라이언트들에게 전송
	Game Client:
		Recv [SPACE_SHIP]:
		Send [X]:
	Game Server:
		Recv [X]:
		Send [SPACE_SHIP]:
	*/
	SPACE_SHIP,

	/** 클라이언트가 관전상태가 되면
	Game Client:
		Recv []:
		Send [OBSERVATION]:
	Game Server:
		Recv [OBSERVATION]:
		Send []:
	*/
	OBSERVATION,


	/** PioneerManager::SpawnPioneer(...) 호출되면
	Game Client:
		Recv [SPAWN_PIONNER]:
		Send []:
	Game Server:
		Recv []:
		Send [SPAWN_PIONNER]:
	*/
	SPAWN_PIONEER,

	/** Pioneer가 죽으면
	Game Client:
		Recv [DIED_PIONEER]:
		Send [DIED_PIONEER]:
	Game Server:
		Recv [DIED_PIONEER]:
		Send [DIED_PIONEER]:
	*/
	DIED_PIONEER,

	/** 게임클라이언트가 자신이 조종중인 Pioneer 정보를 보내면 게임서버는 해당 Pioneer를 제외한 다른 Pioneer들의 정보를 브로드캐스팅
	Game Client:
		Recv [INFO_OF_PIONEER]:
		Send [INFO_OF_PIONEER]:
	Game Server:
		Recv [INFO_OF_PIONEER]:
		Send [INFO_OF_PIONEER]:
	*/
	INFO_OF_PIONEER
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

