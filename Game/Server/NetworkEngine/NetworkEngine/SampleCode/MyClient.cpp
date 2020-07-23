
#include <random>

#include "../NetworkComponent/Console.h"
#include "../NetworkComponent/NetworkComponent.h"

#include "MyClient.h"

CNetworkComponent* CMyClient::Client;

CInfoOfClient CMyClient::MyInfoOfClient;
CRITICAL_SECTION CMyClient::csMyInfoOfClient;

CThreadSafetyQueue<bool> CMyClient::AcceptTSQ;
CThreadSafetyQueue<bool> CMyClient::RejectTSQ;
CThreadSafetyQueue<CInfoOfClient> CMyClient::CreateTSQ;
CThreadSafetyQueue<CInfoOfClient> CMyClient::MoveTSQ;
CThreadSafetyQueue<CInfoOfClient> CMyClient::ExitTSQ;

CMyClient::CMyClient()
{
	InitializeCriticalSection(&csMyInfoOfClient);

	// 클라이언트 구동
	Client = new CNetworkComponent(ENetworkComponentType::NCT_Client);
	if (Client)
	{
		Client->RegisterHeaderAndStaticFunc((unsigned int)ETempHeader::Accept, RecvAccept);
		Client->RegisterHeaderAndStaticFunc((unsigned int)ETempHeader::Reject, RecvReject);
		Client->RegisterHeaderAndStaticFunc((unsigned int)ETempHeader::Create, RecvCreate);
		Client->RegisterHeaderAndStaticFunc((unsigned int)ETempHeader::Move, RecvMove);
		Client->RegisterHeaderAndStaticFunc((unsigned int)ETempHeader::Exit, RecvExit);

		while (Client->Initialize("127.0.0.1", 8000) == false);
	}
}
CMyClient::~CMyClient()
{
	if (Client)
	{
		delete Client;
		Client = nullptr;
	}

	DeleteCriticalSection(&csMyInfoOfClient);
}

void CMyClient::SendLogin()
{
	CONSOLE_LOG("[Start] <CMyClient::SendLogin()> \n");

	EnterCriticalSection(&csMyInfoOfClient);
	CInfoOfClient infoOfClient = MyInfoOfClient;
	LeaveCriticalSection(&csMyInfoOfClient);

	CPacket packet((unsigned int)ETempHeader::Login);
	packet.GetData() << infoOfClient;
	Client->Send(packet);

	infoOfClient.PrintInfo("\t <CMyClient::SendLogin()>");
	CONSOLE_LOG("[End] <CMyClient::SendLogin()> \n");
}
void CMyClient::RecvAccept(stringstream& RecvStream, const SOCKET& Socket)
{
	AcceptTSQ.push(true);
	CONSOLE_LOG("[Info] <CMyClient::RecvAccept(...)> AcceptTSQ.push(true); \n");
}
void CMyClient::RecvReject(stringstream& RecvStream, const SOCKET& Socket)
{
	RejectTSQ.push(true);
	CONSOLE_LOG("[Info] <CMyClient::RecvReject(...)> RejectTSQ.push(true); \n");
}
void CMyClient::RecvCreate(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Start] <CMyClient::RecvCreate(...)> \n");

	CInfoOfClient infoOfClient;
	while (RecvStream >> infoOfClient)
	{
		CreateTSQ.push(infoOfClient);
		CONSOLE_LOG("[Info] <CMyClient::RecvCreate(...)> CreateTSQ.push(infoOfClient); \n");
		infoOfClient.PrintInfo("\t <CMyClient::RecvCreate(...)>");
	}

	CONSOLE_LOG("[End] <CMyClient::RecvCreate(...)> \n");
}
void CMyClient::SendMove()
{
	CONSOLE_LOG("[Start] <CMyClient::SendMove()> \n");

	EnterCriticalSection(&csMyInfoOfClient);
	CInfoOfClient infoOfClient = MyInfoOfClient;
	LeaveCriticalSection(&csMyInfoOfClient);

	CPacket packet((unsigned int)ETempHeader::Move);
	packet.GetData() << infoOfClient;
	Client->Send(packet);

	infoOfClient.PrintInfo("\t <CMyClient::SendMove()>");
	CONSOLE_LOG("[End] <CMyClient::SendMove()> \n");
}
void CMyClient::RecvMove(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Start] <CMyClient::RecvMove(...)> \n");

	CInfoOfClient infoOfClient;
	RecvStream >> infoOfClient;

	MoveTSQ.push(infoOfClient);
	CONSOLE_LOG("[Info] <CMyClient::RecvMove(...)> MoveTSQ.push(infoOfClient); \n");
	infoOfClient.PrintInfo("\t <CMyClient::RecvMove(...)>");

	CONSOLE_LOG("[End] <CMyClient::RecvMove(...)> \n");
}
void CMyClient::RecvExit(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Start] <CMyClient::RecvExit(...)> \n");

	CInfoOfClient infoOfClient;
	RecvStream >> infoOfClient;

	ExitTSQ.push(infoOfClient);
	CONSOLE_LOG("[Info] <CMyClient::RecvExit(...)> ExitTSQ.push(infoOfClient); \n");
	infoOfClient.PrintInfo("\t <CMyClient::RecvExit(...)>");

	CONSOLE_LOG("[End] <CMyClient::RecvExit(...)> \n");
}

CMyClient* CMyClient::GetSingleton()
{
	static CMyClient myClient;
	return &myClient;
}

void CMyClient::SetID(const char* c_str)
{
	EnterCriticalSection(&csMyInfoOfClient);
	MyInfoOfClient.ID = c_str;
	LeaveCriticalSection(&csMyInfoOfClient);
}

void CMyClient::SetRandomPos()
{
	random_device rd;
	mt19937_64 rand(rd());
	uniform_int_distribution<int> range(0, 10);

	EnterCriticalSection(&csMyInfoOfClient);
	MyInfoOfClient.PosX = (float)range(rand);
	MyInfoOfClient.PosY = (float)range(rand);
	MyInfoOfClient.PosZ = (float)range(rand);
	MyInfoOfClient.PrintInfo("\t <CMyClient::SetRandomPos()>");
	LeaveCriticalSection(&csMyInfoOfClient);
}