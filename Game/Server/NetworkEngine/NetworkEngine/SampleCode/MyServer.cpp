
#include "MyPacketHeader.h"
#include "MyPackets.h"
#include "../NetworkComponent/Console.h"
#include "../NetworkComponent/NetworkComponent.h"

#include "MyServer.h"

CNetworkComponent* CMyServer::Server;

map<SOCKET, CInfoOfClient> CMyServer::InfoOfClients;
CRITICAL_SECTION CMyServer::csInfoOfClients;


CMyServer::CMyServer()
{
	InitializeCriticalSection(&csInfoOfClients);


	Server = new CNetworkComponent(ENetworkComponentType::NCT_Server);
	if (Server)
	{
		Server->RegisterHeaderAndStaticFunc((unsigned int)ETempHeader::Login, Login);
		Server->RegisterHeaderAndStaticFunc((unsigned int)ETempHeader::Move, Move);

		Server->RegisterConCBF(ConnectCBF);
		Server->RegisterDisconCBF(DisconnectCBF);

		while (Server->Initialize("127.0.0.1", 8000) == false);
	}
}
CMyServer::~CMyServer()
{
	if (Server)
	{
		delete Server;
		Server = nullptr;
	}

	DeleteCriticalSection(&csInfoOfClients);
}

void CMyServer::ConnectCBF(class CCompletionKey CompletionKey)
{
	CONSOLE_LOG("[Start] <CMyServer::ConnectCBF(...)> \n");

	CompletionKey.PrintInfo("\t <CMyServer::ConnectCBF(...)>");

	CONSOLE_LOG("[End] <CMyServer::ConnectCBF(...)> \n");
}
void CMyServer::DisconnectCBF(class CCompletionKey CompletionKey)
{
	CONSOLE_LOG("[Start] <CMyServer::DisconnectCBF(...)> \n");

	CInfoOfClient infoOfClient;
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(CompletionKey.socket) != InfoOfClients.end())
	{
		infoOfClient = InfoOfClients.at(CompletionKey.socket);
		InfoOfClients.erase(CompletionKey.socket);
	}
	else
	{
		LeaveCriticalSection(&csInfoOfClients);
		return;
	}
	LeaveCriticalSection(&csInfoOfClients);


	CPacket packet((unsigned int)ETempHeader::Exit);
	packet.GetData() << infoOfClient;
	infoOfClient.PrintInfo("\t <CMyServer::DisconnectCBF(...)>");

	EnterCriticalSection(&csInfoOfClients);
	for (auto kvp : InfoOfClients)
	{

		if (kvp.first == CompletionKey.socket)
			continue;

		Server->Send(packet, kvp.first);
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

			CPacket packet((unsigned int)ETempHeader::Reject);
			Server->Send(packet, Socket);

			CONSOLE_LOG("\t <CMyServer::Login(...)> Reject. \n");
			CONSOLE_LOG("[End] <CMyServer::Login(...)> \n");
			return;
		}
	}
	InfoOfClients[Socket] = infoOfClient;
	LeaveCriticalSection(&csInfoOfClients);

	CPacket packet((unsigned int)ETempHeader::Accept);
	Server->Send(packet, Socket);


	CPacket packet1((unsigned int)ETempHeader::Create);
	EnterCriticalSection(&csInfoOfClients);
	for (auto kvp : InfoOfClients)
	{
		packet1.GetData() << kvp.second;
	}
	LeaveCriticalSection(&csInfoOfClients);
	Server->Send(packet1, Socket);


	CPacket packet2((unsigned int)ETempHeader::Create);
	packet2.GetData() << infoOfClient;
	EnterCriticalSection(&csInfoOfClients);
	for (auto kvp : InfoOfClients)
	{
		if (kvp.first == Socket)
			continue;

		Server->Send(packet2, kvp.first);
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


	CPacket packet((unsigned int)ETempHeader::Move);
	packet.GetData() << infoOfClient;

	EnterCriticalSection(&csInfoOfClients);
	for (auto kvp : InfoOfClients)
	{

		if (kvp.first == Socket)
			continue;

		Server->Send(packet, kvp.first);
	}
	LeaveCriticalSection(&csInfoOfClients);

	CONSOLE_LOG("[End] <CMyServer::Move(...)> \n");
}

CMyServer* CMyServer::GetSingleton()
{
	static CMyServer myServer;
	return &myServer;
}

