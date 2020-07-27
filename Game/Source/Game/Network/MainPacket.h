#pragma once

#include "Network/NetworkComponent/NetworkHeader.h"
#include "Network/NetworkComponent/Packet.h"
#include "Network/NetworkComponent/Console.h"

class CPlayerPacket
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
	CPlayerPacket()
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
	~CPlayerPacket()
	{
	}

	// Send
	friend ostream& operator<<(ostream& Stream, CPlayerPacket& Info)
	{
		Stream << CPacket::ReplaceCharInString(Info.ID, ' ', '_') << ' ';
		Stream << CPacket::ReplaceCharInString(Info.IPv4Addr, ' ', '_') << ' ';
		Stream << Info.SocketByMainServer << ' ';
		Stream << Info.SocketByGameServer << ' ';
		Stream << Info.PortOfMainClient << ' ';
		Stream << Info.PortOfGameServer << ' ';
		Stream << Info.PortOfGameClient << ' ';
		Stream << Info.LeaderSocketByMainServer << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, CPlayerPacket& Info)
	{
		Stream >> Info.ID;
		Info.ID = CPacket::ReplaceCharInString(Info.ID, '_', ' ');
		Stream >> Info.IPv4Addr;
		Info.IPv4Addr = CPacket::ReplaceCharInString(Info.IPv4Addr, '_', ' ');
		Stream >> Info.SocketByMainServer;
		Stream >> Info.SocketByGameServer;
		Stream >> Info.PortOfMainClient;
		Stream >> Info.PortOfGameServer;
		Stream >> Info.PortOfGameClient;
		Stream >> Info.LeaderSocketByMainServer;

		return Stream;
	}

	void PrintInfo(const char* Space = "    ", const char* Space2 = "")
	{
		CONSOLE_LOG("%s%s<CPlayerPacket> ID: %s, IPv4Addr: %s, SocketByMainSeCrver: %d, SocketByGameServer: %d, PortOfMainClient: %d, PortOfGameServer: %d, PortOfGameClient: %d, LeaderSocketByMainServer: %d \n",
			Space, Space2, ID.c_str(), IPv4Addr.c_str(), SocketByMainServer, SocketByGameServer, PortOfMainClient, PortOfGameServer, PortOfGameClient, LeaderSocketByMainServer);
	}
};

class CPlayersPacket
{
public:
	std::map<int, CPlayerPacket> Players;

public:
	CPlayersPacket() {}
	~CPlayersPacket() {}

	// Send
	friend ostream& operator<<(ostream& Stream, CPlayersPacket& Info)
	{
		Stream << Info.Players.size() << ' ';
		for (auto& kvp : Info.Players)
		{
			Stream << kvp.first << ' ';
			Stream << kvp.second << ' ';
		}

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, CPlayersPacket& Info)
	{
		int nPlayers = 0;
		int socketID = 0;
		CPlayerPacket Player;

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

	void PrintInfo(const char* Space = "    ", const char* Space2 = "")
	{
		for (auto& kvp : Players)
		{
			CONSOLE_LOG("%s%skey: %d, ", Space, Space2, kvp.first);
			kvp.second.PrintInfo();
		}
	}

	size_t Size()
	{
		return Players.size();
	}

	void Add(int SocketID, CPlayerPacket InfoOfPlayer)
	{
		Players[SocketID] = InfoOfPlayer;
	}

	void Remove(int SocketID)
	{
		Players.erase(SocketID);
	}
};


class CGamePacket
{
public:
	string State;
	string Title;
	int Stage;
	int nMax; // 최대 플레이어 수 제한

	CPlayerPacket Leader; // 방장
	CPlayersPacket Players; // 방장을 제외한 참가자들

public:
	CGamePacket()
	{
		State = "대기중";
		Title = "함께_재밌게_게임을_해보아요!";
		Stage = 1;
		nMax = 30;
	}
	~CGamePacket() {}

	// Send
	friend ostream& operator<<(ostream& Stream, CGamePacket& Info)
	{
		Stream << CPacket::ReplaceCharInString(Info.State, ' ', '_') << ' ';
		Stream << CPacket::ReplaceCharInString(Info.Title, ' ', '_') << ' ';
		Stream << Info.Stage << ' ';
		Stream << Info.nMax << ' ';
		Stream << Info.Leader << ' ';
		Stream << Info.Players << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, CGamePacket& Info)
	{
		Stream >> Info.State;
		Info.State = CPacket::ReplaceCharInString(Info.State, '_', ' ');
		Stream >> Info.Title;
		Info.Title = CPacket::ReplaceCharInString(Info.Title, '_', ' ');
		Stream >> Info.Stage;
		Stream >> Info.nMax;
		Stream >> Info.Leader;
		Stream >> Info.Players;

		return Stream;
	}

	void PrintInfo(const char* Space = "    ", const char* Space2 = "    ")
	{
		CONSOLE_LOG("%s<CGamePacket> Start \n", Space);
		CONSOLE_LOG("%s%sState: %s, Title: %s, Stage: %d, nMax: %d \n", Space, Space2, State.c_str(), Title.c_str(), Stage, nMax);
		Leader.PrintInfo(Space, Space2);
		Players.PrintInfo(Space, Space2);
		CONSOLE_LOG("%s<CGamePacket> End \n", Space);
	}
};
