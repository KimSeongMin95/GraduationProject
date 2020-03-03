#pragma once

#define WIN32_LEAN_AND_MEAN

////////////////////////////////////////////////////////////////
// 情軒杖殖遭亀 Windows.h研 紫遂馬奄 凶庚拭 naming 中宜聖 杷馬奄 是背
// AllowWindowsPlatformTypes.h引 prewindowsapi.h研 識情馬壱
// 制亀酔綜 伯希級聖 識情廃 及拭
// PostWindowsApi.h人 HideWindowsPlatformTypes.h研 識情背醤 桔艦陥.
////////////////////////////////////////////////////////////////

// put this at the top of your .h file above #includes
// UE4: allow Windows platform types to avoid naming collisions
// must be undone at the bottom of this file!
#include "AllowWindowsPlatformTypes.h"
#include "prewindowsapi.h"

//printf_s("ししししししし\n");


/*** 制亀酔綜 伯希 識情 : Start ***/
#pragma comment(lib, "ws2_32.lib") // winsock2 紫遂聖 是背 蓄亜

#include <WinSock2.h>
#include <WS2tcpip.h> // For: inet_pron()
#include <process.h>
#include <iostream>

#include <map>
#include <queue>

#include <sstream>
#include <algorithm>
#include <string>
/*** 制亀酔綜 伯希 識情 : End ***/


// put this at the bottom of the .h file
// UE4: disallow windows platform types
// this was enabled at the top of the file
#include "PostWindowsApi.h"
#include "HideWindowsPlatformTypes.h"


using namespace std;


#define	MAX_BUFFER 4096


// 社掴 搭重 姥繕端
struct stSOCKETINFO
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;
	SOCKET			socket;
	char			messageBuffer[MAX_BUFFER];
	int				recvBytes;
	int				sendBytes;

	string			IPv4Addr; // 惟績 適虞戚情闘税 IP 爽社
	int				Port;	  // 惟績 適虞戚情闘税 Port 爽社
};


template <typename T>
class GAME_API cThreadSafetyQueue
{
private:
	std::queue<T> q;
	CRITICAL_SECTION cs;

public:
	cThreadSafetyQueue() { InitializeCriticalSection(&cs); }
	~cThreadSafetyQueue() { DeleteCriticalSection(&cs); }

	bool empty()
	{
		EnterCriticalSection(&cs);
		bool result = q.empty();
		LeaveCriticalSection(&cs);
		return result;
	}

	void push(T element)
	{
		EnterCriticalSection(&cs);
		q.push(element);
		LeaveCriticalSection(&cs);
	}

	T front()
	{
		EnterCriticalSection(&cs);
		T result = q.front();
		LeaveCriticalSection(&cs);
		return result;
	}

	void pop()
	{
		EnterCriticalSection(&cs);
		q.pop();
		LeaveCriticalSection(&cs);
	}

	T front_pop()
	{
		EnterCriticalSection(&cs);
		T result = q.front();
		q.pop();
		LeaveCriticalSection(&cs);
		return result;
	}

	T back()
	{
		EnterCriticalSection(&cs);
		T result = q.back();
		LeaveCriticalSection(&cs);
		return result;
	}

	void clear()
	{
		EnterCriticalSection(&cs);
		while (q.empty() == false)
			q.pop();
		LeaveCriticalSection(&cs);
	}

	size_t size()
	{
		EnterCriticalSection(&cs);
		size_t result = q.size();
		LeaveCriticalSection(&cs);
		return result;
	}

	std::queue<T> copy()
	{
		EnterCriticalSection(&cs);
		std::queue<T> copyQ = q;
		LeaveCriticalSection(&cs);
		return copyQ;
	}
};


enum EPacketType
{
	/** 竺誤
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

	/** 巴傾戚嬢亜 OnlineWidget拭辞 LOGIN 馬檎
	Main Client:
		Send [LOGIN]: 績獣 梓端昔 cInfoOfPlayer infoOfPlayer拭 ID研 煽舌板 勺重
		Recv [LOGIN]: 閤精 cInfoOfPlayer研 MyInfo拭 企脊
	Main Server:
		Recv [LOGIN]: 閤精 cInfoOfPlayer拭 IPv4Addr, SocketByServer, PortByServer研 煽舌馬壱 InfoOfClients拭 諮脊
		Send [LOGIN]: cInfoOfPlayer
	*/
	LOGIN,

	/** OnlineGameWidget拭辞 (Create Game)獄動聖 喚君 企奄号聖 持失馬檎
	Main Client:
		Send [CREATE_GAME]: MyInfoOfGame.Leader拭 MyInfo研 煽舌馬壱 勺重
		Recv []: X
	Main Server:
		Recv [CREATE_GAME]: 閤精 cInfoOfGame研 InfoOfGames拭 諮脊
		Send []: X
	*/
	CREATE_GAME,

	/** 巴傾戚嬢亜 MainScreenWidget拭辞 Online 獄動聖 喚君 遭脊馬檎
	Main Client:
		Send [FIND_GAMES]: 企奄号 舛左 推姥
		Recv [FIND_GAMES]: 乞窮 企奄号級税 舛左
	Main Server:
		Recv [FIND_GAMES]: O
		Send [FIND_GAMES]: InfoOfGames拭 諮脊鞠嬢 赤澗 乞窮 cInfoOfGame税 舛左
	*/
	FIND_GAMES,

	/** 企奄号 舛左 痕井獣 */
	WAITING_GAME,

	/** 巴傾戚嬢亜 Join 獄動生稽 企奄号拭 級嬢亜檎
	Main Client:
		Send [JOIN_WAITING_ROOM]: Join廃 企奄号 Leader税 SocketID研 企脊廃 MyInfo
		Recv [WAITING_GAME]: 閤精 cInfoOfGame聖 MyInfoOfGame拭 企脊
	Main Server:
		Recv [JOIN_WAITING_ROOM]: InfoOfClients拭 LeaderSocketByMainServer研 企脊馬壱 InfoOfGames税 Players拭 背雁 適虞戚情闘 諮脊
		Send [WAITING_GAME] to 号舌: 背雁 企奄号税 cInfoOfGame
		Send [WAITING_GAME] to 企奄号税 陥献 巴傾戚嬢級: 背雁 企奄号税 cInfoOfGame
	*/
	JOIN_WAITING_GAME,

	/** 号舌戚 企奄号拭辞 Back 獄動聖 喚君 企奄号聖 曽戟馬檎
	Main Client:
		Send [DESTROY_WAITING_ROOM]: O, InitMyInfoOfGame();
		Recv [DESTROY_WAITING_ROOM]: 企奄号 曽戟 硝顕拭 及稽亜奄 獄動 醗失鉢, MyInfo税 働舛 呉獄痕呪級 段奄鉢, InitMyInfoOfGame();
	Main Server:
		Recv [DESTROY_WAITING_ROOM] by 号舌: InfoOfClients税 背雁 適虞戚情闘税 働舛 呉獄痕呪級 段奄鉢, InfoOfGames.erase(pSocketInfo->socket);
		Send [DESTROY_WAITING_ROOM] to 巴傾戚嬢級(号舌 薦須): O
	*/
	DESTROY_WAITING_GAME,

	/** 号舌戚 焼観 企奄号昔 巴傾戚嬢亜 企奄号拭辞 蟹亜檎
	Main Client:
		Send [EXIT_WAITING_ROOM]: O, MyInfo税 働舛 呉獄痕呪級 段奄鉢, InitMyInfoOfGame();
		Recv [WAITING_GAME]: 企奄号 cInfoOfGame
	Main Server:
		Recv [EXIT_WAITING_ROOM]: InfoOfClients税 背雁 適虞戚情闘税 働舛 呉獄痕呪級 段奄鉢, InfoOfGames税 Players拭 背雁 適虞戚情闘 薦暗
		Send [WAITING_GAME] to 号舌: 背雁 企奄号税 cInfoOfGame
		Send [WAITING_GAME] to 巴傾戚嬢級(背雁 適虞戚情闘 耕匂敗): 背雁 企奄号税 cInfoOfGame
	*/
	EXIT_WAITING_GAME,

	/** 号舌戚 企奄号拭辞 Title戚蟹 Stage蟹 Maximum聖 呪舛馬檎
	Main Client:
		Send [MODIFY_WAITING_GAME]: cInfoOfGame(Title, Stage, Maximum)
		Recv [MODIFY_WAITING_GAME]: cInfoOfGame(Title, Stage, Maximum)
	Main Server:
		Recv [MODIFY_WAITING_GAME]: InfoOfGames拭 cInfoOfGame(Title, Stage, Maximum) 旋遂
		Send [MODIFY_WAITING_GAME] to 巴傾戚嬢級(号舌 薦須): cInfoOfGame(Title, Stage, Maximum)
	*/
	MODIFY_WAITING_GAME,

	/** 号舌戚 企奄号拭辞 惟績聖 獣拙馬檎
	Main Client:
		Send [START_WAITING_GAME]: O
		Recv [START_WAITING_GAME]: O
	Main Server:
		Recv [START_WAITING_GAME] by 号舌: O
		Send [START_WAITING_GAME] to 巴傾戚嬢級(号舌 薦須): O
	*/
	START_WAITING_GAME,


	/** 号舌戚 企奄号拭辞 惟績聖 獣拙馬壱 CountStartedGame()拭辞 惟績 辞獄 段奄鉢拭 失因馬檎
	Main Client:
		Send [ACTIVATE_GAME_SERVER]: MyInfo拭 PortOfGameServer研 煽舌馬壱 MyInfo研 穿勺, MyInfoOfGame.State = string("Playing");
		Recv []: X
	Main Server:
		Recv [ACTIVATE_GAME_SERVER] by 号舌: 呪重廃 cInfoOfPlayer研 InfoOfClients人 InfoOfGames税 Leader拭 旋遂
		Send []: X
	*/
	ACTIVATE_GAME_SERVER,

	/** 凧亜切亜 惟績 適虞戚情闘研 惟績 辞獄人 尻衣獣徹奄 是背 惟績 辞獄 舛左研 推短
	Main Client:
		Send [REQUEST_INFO_OF_GAME_SERVER]: O
		Recv [REQUEST_INFO_OF_GAME_SERVER]: cInfoOfPlayer研 閤壱 IPv4Addr人 PortOfGameServer研 塙究
	Main Server:
		Recv [REQUEST_INFO_OF_GAME_SERVER]: O
		Send [REQUEST_INFO_OF_GAME_SERVER]: cInfoOfPlayer infoOfPlayer = InfoOfClients.at(leaderSocket);馬壱 infoOfPlayer 穿勺
	*/
	REQUEST_INFO_OF_GAME_SERVER,


	JOIN_PLAYING_GAME,	// 嬢恐 巴傾戚嬢亜 遭楳掻昔 惟績拭 級嬢臣 凶: 背雁 惟績税 巴傾戚嬢級引 FIND_GAMES昔 巴傾戚嬢級拭惟 崎稽球蝶什特 背醤 敗.

	START_PLAYING_GAME, // 巴傾戚掻昔 惟績 獣拙馬奄

	EXIT_PLAYING_GAME,	// 巴傾戚嬢亜 遭楳掻昔 惟績聖 曽戟拝 凶: 

	///////////////////////////////////////////
	// Game Server / Game Clients
	///////////////////////////////////////////
};


class GAME_API cInfoOfPlayer
{
public:
	string ID;
	string IPv4Addr; // IP 腰硲

	int SocketByMainServer; // 五昔 辞獄稽採斗 採食吉 適虞戚情闘税 社掴 腰硲
	int SocketByGameServer; // 惟績 辞獄研 姥疑馬澗 号舌生稽採斗 採食吉 社掴 腰硲

	int PortOfMainClient; // 五昔 適虞戚情闘亜 五昔 辞獄人 尻衣吉 匂闘 腰硲
	int PortOfGameServer; // 号舌生稽採斗 姥疑吉 惟績 辞獄税 匂闘 腰硲
	int PortOfGameClient; // 惟績 適虞戚情闘亜 惟績 辞獄人 尻衣吉 匂闘 腰硲

	int LeaderSocketByMainServer; // 五昔 辞獄稽採斗 採食吉 号舌税 社掴 腰硲

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
	void PrintInfo(const TCHAR* Space = _T("    "), const TCHAR* Space2 = _T(""))
	{
		printf_s("%s%s<cInfoOfPlayer> ID: %s, IPv4Addr: %s, SocketByMainServer: %d, SocketByGameServer: %d, PortOfMainClient: %d, PortOfGameServer: %d, PortOfGameClient: %d, LeaderSocketByMainServer: %d\n", TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), ID.c_str(), IPv4Addr.c_str(), SocketByMainServer, SocketByGameServer, PortOfMainClient, PortOfGameServer, PortOfGameClient, LeaderSocketByMainServer);
		//UE_LOG(LogTemp, Warning, TEXT("%s%s<cInfoOfPlayer> ID: %s, IPv4Addr: %s, SocketByMainServer: %d, SocketByGameServer: %d, PortOfMainClient: %d, PortOfGameServer: %d, PortOfGameClient: %d, LeaderSocketByMainServer: %d"), Space, Space2, ANSI_TO_TCHAR(ID.c_str()), ANSI_TO_TCHAR(IPv4Addr.c_str()), SocketByMainServer, SocketByGameServer, PortOfMainClient, PortOfGameServer, PortOfGameClient, LeaderSocketByMainServer);
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

		// 段奄鉢
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
			printf_s("%s%skey: %d, ", TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), kvp.first);
			//UE_LOG(LogTemp, Warning, TEXT("%s%skey: %d"), Space, Space2, kvp.first);
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
	int nMax; // 置企 巴傾戚嬢 呪 薦廃

	cInfoOfPlayer Leader; // 号舌
	cInfoOfPlayers Players; // 号舌聖 薦須廃 凧亜切級

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
		printf_s("%s<cInfoOfGame> Start\n", TCHAR_TO_ANSI(Space));
		//UE_LOG(LogTemp, Warning, TEXT("%s<cInfoOfGame> Start"), Space);
		printf_s("%s%sState: %s, Title: %s, Stage: %d, nMax: %d\n", TCHAR_TO_ANSI(Space), TCHAR_TO_ANSI(Space2), State.c_str(), Title.c_str(), Stage, nMax);
		//UE_LOG(LogTemp, Warning, TEXT("%s%sState: %s, Title: %s, Stage: %d, nMax: %d"), Space, Space2, ANSI_TO_TCHAR(State.c_str()), ANSI_TO_TCHAR(Title.c_str()), Stage, nMax);
		Leader.PrintInfo(Space, Space2);
		Players.PrintInfo(Space, Space2);
		//UE_LOG(LogTemp, Warning, TEXT("%s<cInfoOfGame> End"), Space);
		printf_s("%s<cInfoOfGame> End\n", TCHAR_TO_ANSI(Space));

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

/*** Console for log : Start ***/
class CustomLog
{
private:
	static FILE* fp_console;

public:
	static void AllocConsole()
	{
		// 戚耕 拝雁鞠嬢 赤生檎 嬬車聖 希 拝雁馬走 省柔艦陥.
		if (fp_console)
			return;

		if (::AllocConsole())
		{
			freopen_s(&fp_console, "CONOUT$", "w", stdout);
		}
	}

	static void FreeConsole()
	{
		// 戚耕 拝雁鞠嬢 赤聖 凶幻 社瑚獣典艦陥.
		if (fp_console)
		{
			fclose(fp_console);
			::FreeConsole();
		}
	}
	/*** Console for log : End ***/
};

FILE* CustomLog::fp_console = nullptr;