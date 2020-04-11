#pragma once

//// ��Ƽ����Ʈ ���� ���� define
//#define _WINSOCK_DEPRECATED_NO_WARNINGS

// winsock2 ����� ���� �Ʒ� �ڸ�Ʈ �߰�
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


// �����ʹ� ũ��� �ִ� 4095���� �������� '\0'�� ���� ������ ���� ����
#define	MAX_BUFFER	4096


////////////////////////
// IOCP CompletionKey
////////////////////////
class CCompletionKey
{
public:
	SOCKET			socket;

	string			IPv4Addr; // ���� Ŭ���̾�Ʈ�� IP �ּ�
	int				Port;	  // ���� Ŭ���̾�Ʈ�� Port �ּ�

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
	int				sendBytes; // WSASend�� ������ �������� ����Ʈ ũ��

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

//// ��Ŷ ó�� �Լ� ������ Ÿ�� ����
typedef void* (*PacketProcessingFunc)(class CNetworkComponent*, stringstream&, SOCKET);

class CPacket
{
public:
	// ��ŵŸ�Ը� �����ϴ� ���� Packet�� ���� ���Դϴ�.
	static unsigned int Number;

	// ��ŶŸ�Ը� �����ϴ� Number�� �����մϴ�.
	static map<string, unsigned int> Types;

	// typedef�� ������ PacketProcessingFunc������ ��ȯ�Ͽ� ����ؾ� �մϴ�.
	static map<unsigned int, LPVOID> PPFsOfServer;
	static map<unsigned int, LPVOID> PPFsOfClient;


public:
	CPacket();

	/** ��ŶŸ�Ը�� ��������Լ��� ����մϴ�.
	��������Լ��� ����ϴ� ������ ����ȭ�� �� ���־�� �մϴ�. */
	static void RegisterTypeAndStaticFunc(
		string Name, 
		ENetworkComponentType NCT, 
		void(*Function)(class CNetworkComponent*, 
		stringstream&, 
		SOCKET)
	);

	// ��ŶŸ�Ը� �ش��ϴ� ���� �����ɴϴ�.
	static unsigned int GetNumberOfType(string Name);

	// ��Ŷ�� �ش��ϴ� �Լ��� �����մϴ�.
	static void ProcessPacketOfServer(unsigned int Type, class CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket);
	static void ProcessPacketOfClient(unsigned int Type, class CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket);
};







//// ���� ���������� Custom�̹Ƿ� ��ȿ���� ������ �� �����Ƿ� ���� ���߿� ����غ� ��.
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
//	string IPv4Addr; // IP ��ȣ
//
//	int SocketByMainServer; // ���� �����κ��� �ο��� Ŭ���̾�Ʈ�� ���� ��ȣ
//	int SocketByGameServer; // ���� ������ �����ϴ� �������κ��� �ο��� ���� ��ȣ
//
//	int PortOfMainClient; // ���� Ŭ���̾�Ʈ�� ���� ������ ����� ��Ʈ ��ȣ
//	int PortOfGameServer; // �������κ��� ������ ���� ������ ��Ʈ ��ȣ
//	int PortOfGameClient; // ���� Ŭ���̾�Ʈ�� ���� ������ ����� ��Ʈ ��ȣ
//
//	int LeaderSocketByMainServer; // ���� �����κ��� �ο��� ������ ���� ��ȣ
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
//		// �ʱ�ȭ
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
//	int nMax; // �ִ� �÷��̾� �� ����
//
//	cInfoOfPlayer Leader; // ����
//	cInfoOfPlayers Players; // ������ ������ �����ڵ�
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
