
#include "../NetworkComponent/Console.h"
#include "../NetworkComponent/NetworkComponent.h"

#include "MyServer.h"

unique_ptr<CNetworkComponent> CMyServer::Server;

map<SOCKET, CInfoOfClient> CMyServer::InfoOfClients;
CRITICAL_SECTION CMyServer::csInfoOfClients;

CMyServer::CMyServer()
{
	// 크리티컬 섹션에 스핀락을 걸고 초기화에 성공할때까지 시도합니다.
	while (InitializeCriticalSectionAndSpinCount(&csInfoOfClients, SPIN_COUNT) == false);

	Server = make_unique<CNetworkComponent>(ENetworkComponentType::NCT_Server);
	if (Server)
	{
		Server->RegisterConCBF(ConnectCBF);
		Server->RegisterDisconCBF(DisconnectCBF);

		Server->RegisterHeaderAndStaticFunc((uint16_t)EMyPacketHeader::Login, Login);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EMyPacketHeader::Move, Move);
	}
}
CMyServer::~CMyServer()
{
	DeleteCriticalSection(&csInfoOfClients);
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

void CMyServer::ConnectCBF(CCompletionKey CompletionKey)
{
	CONSOLE_LOG("[Start] <CMyServer::ConnectCBF(...)> \n");

	CompletionKey.PrintInfo("\t <CMyServer::ConnectCBF(...)>");

	CONSOLE_LOG("[End] <CMyServer::ConnectCBF(...)> \n");
}
void CMyServer::DisconnectCBF(CCompletionKey CompletionKey)
{
	CONSOLE_LOG("[Start] <CMyServer::DisconnectCBF(...)> \n");

	CInfoOfClient infoOfClient;
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(CompletionKey.Socket) != InfoOfClients.end())
	{
		infoOfClient = InfoOfClients.at(CompletionKey.Socket);
		InfoOfClients.erase(CompletionKey.Socket);
	}
	else
	{
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	LeaveCriticalSection(&csInfoOfClients);


	CPacket packet((uint16_t)EMyPacketHeader::Exit);
	packet.GetData() << infoOfClient << endl;
	infoOfClient.PrintInfo("\t <CMyServer::DisconnectCBF(...)>");

	EnterCriticalSection(&csInfoOfClients);
	for (auto kvp : InfoOfClients)
	{

		if (kvp.first == CompletionKey.Socket)
			continue;

		if (Server) Server->Send(packet, kvp.first);
	}
	LeaveCriticalSection(&csInfoOfClients);

	CONSOLE_LOG("[End] <CMyServer::DisconnectCBF(...)> \n");
}

void CMyServer::Login(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Start] <CMyServer::Login(...)> \n");


	CInfoOfClient infoOfClient;
	RecvStream >> infoOfClient;
	infoOfClient.PrintInfo("\t <CMyServer::Login(...)>");

	EnterCriticalSection(&csInfoOfClients);
	for (auto kvp : InfoOfClients)
	{

		if (infoOfClient.ID.compare(kvp.second.ID) == 0)
		{
			LeaveCriticalSection(&csInfoOfClients);

			CPacket packet((uint16_t)EMyPacketHeader::Reject);
			if (Server) Server->Send(packet, Socket);

			CONSOLE_LOG("\t <CMyServer::Login(...)> Reject. \n");
			CONSOLE_LOG("[End] <CMyServer::Login(...)> \n");
			return;
		}
	}
	InfoOfClients[Socket] = infoOfClient;
	LeaveCriticalSection(&csInfoOfClients);

	CPacket packet((uint16_t)EMyPacketHeader::Accept);
	if (Server) Server->Send(packet, Socket);


	CPacket packet1((uint16_t)EMyPacketHeader::Create);
	EnterCriticalSection(&csInfoOfClients);
	for (auto kvp : InfoOfClients)
	{
		packet1.GetData() << kvp.second << endl;
	}
	LeaveCriticalSection(&csInfoOfClients);
	if (Server) Server->Send(packet1, Socket);


	CPacket packet2((uint16_t)EMyPacketHeader::Create);
	packet2.GetData() << infoOfClient << endl;
	EnterCriticalSection(&csInfoOfClients);
	for (auto kvp : InfoOfClients)
	{
		if (kvp.first == Socket)
			continue;

		if (Server) Server->Send(packet2, kvp.first);
	}
	LeaveCriticalSection(&csInfoOfClients);

	CONSOLE_LOG("[End] <CMyServer::Login(...)> \n");
}
void CMyServer::Move(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Start] <CMyServer::Move(...)> \n");


	CInfoOfClient infoOfClient;
	RecvStream >> infoOfClient;
	infoOfClient.PrintInfo("\t <CMyServer::Move(...)>");


	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	InfoOfClients.at(Socket) = infoOfClient;
	LeaveCriticalSection(&csInfoOfClients);


	CPacket packet((uint16_t)EMyPacketHeader::Move);
	packet.GetData() << infoOfClient << endl;

	EnterCriticalSection(&csInfoOfClients);
	for (auto kvp : InfoOfClients)
	{

		if (kvp.first == Socket)
			continue;

		if (Server) Server->Send(packet, kvp.first);
	}
	LeaveCriticalSection(&csInfoOfClients);

	CONSOLE_LOG("[End] <CMyServer::Move(...)> \n");
}


