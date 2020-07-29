
#include "../NetworkComponent/Console.h"
#include "../NetworkComponent/NetworkComponent.h"

#include "MyServer.h"

unique_ptr<CNetworkComponent> CMyServer::Server;

unordered_map<SOCKET, CPlayerPacket> CMyServer::Players;
CRITICAL_SECTION CMyServer::csPlayers;

CMyServer::CMyServer()
{
	// 크리티컬 섹션에 스핀락을 걸고 초기화에 성공할때까지 시도합니다.
	while (InitializeCriticalSectionAndSpinCount(&csPlayers, SPIN_COUNT) == false);

	Server = make_unique<CNetworkComponent>(ENetworkComponentType::NCT_Server);
	if (Server)
	{
		Server->RegisterConCBF(ConnectCBF); // 클라이언트가 접속하면 실행할 정적 콜백함수를 등록합니다.
		Server->RegisterDisconCBF(DisconnectCBF); // 클라이언트가 접속을 종료하면 실행할 정적 콜백함수를 등록합니다.

		Server->RegisterHeaderAndStaticFunc((uint16_t)EMyPacketHeader::Data, Data); // 패킷의 헤더에 대응하여 실행할 정적 함수를 등록합니다.
		Server->RegisterHeaderAndStaticFunc((uint16_t)EMyPacketHeader::BigData, BigData); // 패킷의 헤더에 대응하여 실행할 정적 함수를 등록합니다.
	}
}
CMyServer::~CMyServer()
{
	DeleteCriticalSection(&csPlayers);
}

CMyServer* CMyServer::GetSingleton()
{
	static CMyServer myServer;
	return &myServer;
}

bool CMyServer::Initialize(const char* const IPv4, const USHORT& Port)
{
	if (!Server)
	{
		CONSOLE_LOG("[Error] <CMyServer::Initialize(...)> if (!Server) \n");
		return false;
	}
	/****************************************/

	return Server->Initialize(IPv4, Port);
}
bool CMyServer::IsNetworkOn()
{
	CONSOLE_LOG("[START] <CMyServer::IsNetworkOn()>\n");

	if (!Server)
	{
		CONSOLE_LOG("[Error] <CMyServer::IsNetworkOn())> if (!Server) \n");
		return false;
	}
	/****************************************/

	return Server->IsNetworkOn();
}
void CMyServer::Close()
{
	CONSOLE_LOG("[START] <CMyServer::Close()>\n");

	if (!Server)
	{
		CONSOLE_LOG("[Error] <CMyServer::Close())> if (!Server) \n");
		return;
	}
	/****************************************/

	Server->Close();

	CONSOLE_LOG("[END] <CMyServer::Close()>\n");
}

void CMyServer::ConnectCBF(CCompletionKey CompletionKey)
{
	CONSOLE_LOG("[Start] <CMyServer::ConnectCBF(...)> \n");
	CompletionKey.PrintInfo("\t <CMyServer::ConnectCBF(...)>");

	random_device rd;
	mt19937_64 rand(rd());
	uniform_int_distribution<int> range(0, 10);

	string msg = to_string(CompletionKey.Socket) + string(" is connected.");
	CPlayerPacket player(msg, (float)range(rand), (float)range(rand), (float)range(rand));
	player.PrintInfo("\t <CMyServer::ConnectCBF(...)>");

	EnterCriticalSection(&csPlayers);
	if (Players.find(CompletionKey.Socket) == Players.end())
	{
		Players.emplace(CompletionKey.Socket, player);
	}
	LeaveCriticalSection(&csPlayers);

	CPacket dataPacket((uint16_t)EMyPacketHeader::Data);
	dataPacket.GetData() << player << endl;
	if (Server) Server->Broadcast(dataPacket);

	CONSOLE_LOG("[End] <CMyServer::ConnectCBF(...)> \n");
}
void CMyServer::DisconnectCBF(CCompletionKey CompletionKey)
{
	CONSOLE_LOG("[Start] <CMyServer::DisconnectCBF(...)> \n");
	CompletionKey.PrintInfo("\t <CMyServer::DisconnectCBF(...)>");

	CPlayerPacket player;
	EnterCriticalSection(&csPlayers);
	if (Players.find(CompletionKey.Socket) != Players.end())
	{
		player = Players.at(CompletionKey.Socket);
		player.Message = to_string(CompletionKey.Socket) + string(" is disconnected.");
		player.PrintInfo("\t <CMyServer::DisconnectCBF(...)>");
		Players.erase(CompletionKey.Socket);
	}
	else
	{
		LeaveCriticalSection(&csPlayers);
		return;
	}
	LeaveCriticalSection(&csPlayers);

	CPacket dataPacket((uint16_t)EMyPacketHeader::Data);
	dataPacket.GetData() << player << endl;
	if (Server) Server->BroadcastExceptOne(dataPacket, CompletionKey.Socket);

	CONSOLE_LOG("[End] <CMyServer::DisconnectCBF(...)> \n");
}

void CMyServer::Data(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Start] <CMyServer::Data(...)> \n");

	CPlayerPacket player;
	if (RecvStream >> player)
	{
		player.PrintInfo("\t <CMyServer::Data(...)>");

		CPacket dataPacket((uint16_t)EMyPacketHeader::Data);
		dataPacket.GetData() << player << endl;
		if (Server) Server->Send(dataPacket, Socket);
	}

	CONSOLE_LOG("[End] <CMyServer::Data(...)> \n");
}
void CMyServer::BigData(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Start] <CMyServer::BigData(...)> \n");

	CPlayerPacket player;
	while (RecvStream >> player)
	{
		player.PrintInfo("\t <CMyServer::BigData(...)>");

		CPacket bidDataPacket((uint16_t)EMyPacketHeader::BigData);
		bidDataPacket.GetData() << player << endl;
		if (Server) Server->Send(bidDataPacket, Socket);
	}

	CONSOLE_LOG("[End] <CMyServer::BigData(...)> \n");
}

