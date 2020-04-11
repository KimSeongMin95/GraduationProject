#pragma once

//// 멀티바이트 집합 사용시 define
//#define _WINSOCK_DEPRECATED_NO_WARNINGS

// winsock2 사용을 위해 아래 코멘트 추가
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h> // For: inet_pton()

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <process.h>
#include <sstream>
#include <algorithm>
#include <string>

#include <map>
#include <queue>
#include <deque>

using namespace std;

#include "MyConsole.h"


// 데이터는 크기는 최대 4095여야 마지막에 '\0'가 들어가서 오류가 나지 않음
#define	MAX_BUFFER	4096


////////////////////////
// IOCP CompletionKey
////////////////////////
class CCompletionKey
{
public:
	SOCKET			socket;

	string			IPv4Addr; // 메인 클라이언트의 IP 주소
	int				Port;	  // 메인 클라이언트의 Port 주소

public:
	CCompletionKey();

};


////////////////////////
// IOCP OverlappedMsg
////////////////////////
class COverlappedMsg
{
public:
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;

	char			messageBuffer[MAX_BUFFER];
	int				recvBytes;
	int				sendBytes; // WSASend로 전송할 데이터의 바이트 크기

public:
	COverlappedMsg();

	void Initialize();
};


////////////////////////
// IOCP CPacket
////////////////////////
enum ENetworkComponentType
{
	NCT_None,
	NCT_Server,
	NCT_Client
};

//// 패킷 처리 함수 포인터 타입 선언
typedef void* (*PacketProcessingFunc)(class CNetworkComponent*, stringstream&, SOCKET);

class CPacket
{
public:
	// 패킵타입명에 대응하는 실제 Packet에 사용될 값입니다.
	static unsigned int Number;

	// 패킷타입명에 대응하는 Number를 저장합니다.
	static map<string, unsigned int> Types;

	// typedef로 선언한 PacketProcessingFunc형으로 변환하여 사용해야 합니다.
	static map<unsigned int, LPVOID> PPFsOfServer;
	static map<unsigned int, LPVOID> PPFsOfClient;


public:
	CPacket();

	/** 패킷타입명과 전역멤버함수를 등록합니다.
	전역멤버함수에 사용하는 변수는 동기화를 꼭 해주어야 합니다. */
	static void RegisterTypeAndStaticFunc(
		string Name, 
		ENetworkComponentType NCT, 
		void(*Function)(class CNetworkComponent*, 
		stringstream&, 
		SOCKET)
	);

	// 패킷타입명에 해당하는 값을 가져옵니다.
	static unsigned int GetNumberOfType(string Name);

	// 패킷에 해당하는 함수를 실행합니다.
	static void ProcessPacketOfServer(unsigned int Type, class CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket);
	static void ProcessPacketOfClient(unsigned int Type, class CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket);
};







//// 쓰면 간단해지나 Custom이므로 유효성을 보장할 수 없으므로 아주 나중에 사용해볼 것.
//template <typename T>
//class Map_TS
//{
//private:
//	std::map<SOCKET, T> m;
//	CRITICAL_SECTION cs;
//
//public:
//	Map_TS() { InitializeCriticalSection(&cs); }
//	~Map_TS() { DeleteCriticalSection(&cs); }
//};


//class cInfoOfPlayer
//{
//public:
//	string ID;
//	string IPv4Addr; // IP 번호
//
//	int SocketByMainServer; // 메인 서버로부터 부여된 클라이언트의 소켓 번호
//	int SocketByGameServer; // 게임 서버를 구동하는 방장으로부터 부여된 소켓 번호
//
//	int PortOfMainClient; // 메인 클라이언트가 메인 서버와 연결된 포트 번호
//	int PortOfGameServer; // 방장으로부터 구동된 게임 서버의 포트 번호
//	int PortOfGameClient; // 게임 클라이언트가 게임 서버와 연결된 포트 번호
//
//	int LeaderSocketByMainServer; // 메인 서버로부터 부여된 방장의 소켓 번호
//
//public:
//	cInfoOfPlayer()
//	{
//		ID = "NULL";
//		IPv4Addr = "NULL";
//		SocketByMainServer = 0;
//		SocketByGameServer = 0;
//		PortOfMainClient = 0;
//		PortOfGameServer = 0;
//		PortOfGameClient = 0;
//		LeaderSocketByMainServer = 0;
//	}
//	~cInfoOfPlayer()
//	{
//	}
//
//	// Send
//	friend ostream& operator<<(ostream& Stream, cInfoOfPlayer& Info)
//	{
//		Stream << ReplaceCharInString(Info.ID, ' ', '_') << endl;
//		Stream << ReplaceCharInString(Info.IPv4Addr, ' ', '_') << endl;
//		Stream << Info.SocketByMainServer << endl;
//		Stream << Info.SocketByGameServer << endl;
//		Stream << Info.PortOfMainClient << endl;
//		Stream << Info.PortOfGameServer << endl;
//		Stream << Info.PortOfGameClient << endl;
//		Stream << Info.LeaderSocketByMainServer << endl;
//
//		return Stream;
//	}
//
//	// Recv
//	friend istream& operator>>(istream& Stream, cInfoOfPlayer& Info)
//	{
//		Stream >> Info.ID;
//		Info.ID = ReplaceCharInString(Info.ID, '_', ' ');
//		Stream >> Info.IPv4Addr;
//		Info.IPv4Addr = ReplaceCharInString(Info.IPv4Addr, '_', ' ');
//		Stream >> Info.SocketByMainServer;
//		Stream >> Info.SocketByGameServer;
//		Stream >> Info.PortOfMainClient;
//		Stream >> Info.PortOfGameServer;
//		Stream >> Info.PortOfGameClient;
//		Stream >> Info.LeaderSocketByMainServer;
//
//		return Stream;
//	}
//
//	// Log
//	void PrintInfo(const char* Space = "    ", const char* Space2 = "")
//	{
//		CONSOLE_LOG("%s%s<cInfoOfPlayer> ID: %s, IPv4Addr: %s, SocketByMainServer: %d, SocketByGameServer: %d, PortOfMainClient: %d, PortOfGameServer: %d, PortOfGameClient: %d, LeaderSocketByMainServer: %d\n",
//			Space, Space2, ID.c_str(), IPv4Addr.c_str(), SocketByMainServer, SocketByGameServer, PortOfMainClient, PortOfGameServer, PortOfGameClient, LeaderSocketByMainServer);
//	}
//
//	// Convert
//	static string ReplaceCharInString(string str, char before, char after)
//	{
//		string result = str;
//		for (int i = 0; i < result.size(); i++)
//		{
//			if (result.at(i) == before)
//				result.at(i) = after;
//		}
//		return result;
//	}
//};
//
//class cInfoOfPlayers
//{
//public:
//	std::map<int, cInfoOfPlayer> Players;
//
//public:
//	cInfoOfPlayers() {}
//	~cInfoOfPlayers() {}
//
//	// Send
//	friend ostream& operator<<(ostream& Stream, cInfoOfPlayers& Info)
//	{
//		Stream << Info.Players.size() << endl;
//		for (auto& kvp : Info.Players)
//		{
//			Stream << kvp.first << endl;
//			Stream << kvp.second << endl;
//		}
//
//		return Stream;
//	}
//
//	// Recv
//	friend istream& operator>>(istream& Stream, cInfoOfPlayers& Info)
//	{
//		int nPlayers = 0;
//		int socketID = 0;
//		cInfoOfPlayer Player;
//
//		// 초기화
//		Info.Players.clear();
//
//		Stream >> nPlayers;
//		for (int i = 0; i < nPlayers; i++)
//		{
//			Stream >> socketID;
//			Stream >> Player;
//			Info.Players[socketID] = Player;
//		}
//
//		return Stream;
//	}
//
//	// Log
//	void PrintInfo(const char* Space = "    ", const char* Space2 = "")
//	{
//		for (auto& kvp : Players)
//		{
//			CONSOLE_LOG("%s%skey: %d, ", Space, Space2, kvp.first);
//			kvp.second.PrintInfo();
//		}
//	}
//
//	size_t Size()
//	{
//		return Players.size();
//	}
//
//	void Add(int SocketID, cInfoOfPlayer InfoOfPlayer)
//	{
//		Players[SocketID] = InfoOfPlayer;
//	}
//
//	void Remove(int SocketID)
//	{
//		Players.erase(SocketID);
//	}
//};
//
//
//class cInfoOfGame
//{
//public:
//	string State;
//	string Title;
//	int Stage;
//	int nMax; // 최대 플레이어 수 제한
//
//	cInfoOfPlayer Leader; // 방장
//	cInfoOfPlayers Players; // 방장을 제외한 참가자들
//
//public:
//	cInfoOfGame()
//	{
//		State = "Waiting";
//		Title = "Let's_go_together!";
//		Stage = 1;
//		nMax = 100;
//	}
//	~cInfoOfGame() {}
//
//	// Send
//	friend ostream& operator<<(ostream& Stream, cInfoOfGame& Info)
//	{
//		Stream << ReplaceCharInString(Info.State, ' ', '_') << endl;
//		Stream << ReplaceCharInString(Info.Title, ' ', '_') << endl;
//		Stream << Info.Stage << endl;
//		Stream << Info.nMax << endl;
//		Stream << Info.Leader << endl;
//		Stream << Info.Players << endl;
//
//		return Stream;
//	}
//
//	// Recv
//	friend istream& operator>>(istream& Stream, cInfoOfGame& Info)
//	{
//		Stream >> Info.State;
//		Info.State = ReplaceCharInString(Info.State, '_', ' ');
//		Stream >> Info.Title;
//		Info.Title = ReplaceCharInString(Info.Title, '_', ' ');
//		Stream >> Info.Stage;
//		Stream >> Info.nMax;
//		Stream >> Info.Leader;
//		Stream >> Info.Players;
//
//		return Stream;
//	}
//
//	// Log
//	void PrintInfo(const char* Space = "    ", const char* Space2 = "    ")
//	{
//		CONSOLE_LOG("%s<cInfoOfGame> Start\n", Space);
//		CONSOLE_LOG("%s%sState: %s, Title: %s, Stage: %d, nMax: %d\n", Space, Space2, State.c_str(), Title.c_str(), Stage, nMax);
//		Leader.PrintInfo(Space, Space2);
//		Players.PrintInfo(Space, Space2);
//		CONSOLE_LOG("%s<cInfoOfGame> End\n", Space);
//	}
//
//	// Convert
//	static string ReplaceCharInString(string str, char before, char after)
//	{
//		string result = str;
//		for (int i = 0; i < result.size(); i++)
//		{
//			if (result.at(i) == before)
//				result.at(i) = after;
//		}
//		return result;
//	}
//};
//
