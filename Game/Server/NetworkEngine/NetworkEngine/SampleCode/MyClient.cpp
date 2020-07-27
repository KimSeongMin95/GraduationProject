
#include <random>

#include "../NetworkComponent/Console.h"
#include "../NetworkComponent/NetworkComponent.h"

#include "MyClient.h"

unique_ptr<CNetworkComponent> CMyClient::Client;

CInfoOfClient CMyClient::MyInfoOfClient;
CRITICAL_SECTION CMyClient::csMyInfoOfClient;

CThreadSafetyQueue<bool> CMyClient::AcceptTSQ;
CThreadSafetyQueue<bool> CMyClient::RejectTSQ;
CThreadSafetyQueue<CInfoOfClient> CMyClient::CreateTSQ;
CThreadSafetyQueue<CInfoOfClient> CMyClient::MoveTSQ;
CThreadSafetyQueue<CInfoOfClient> CMyClient::ExitTSQ;

CMyClient::CMyClient()
{
	// 크리티컬 섹션에 스핀락을 걸고 초기화에 성공할때까지 시도합니다.
	while (InitializeCriticalSectionAndSpinCount(&csMyInfoOfClient, SPIN_COUNT) == false);

	Client = make_unique<CNetworkComponent>(ENetworkComponentType::NCT_Client);
	if (Client)
	{
		Client->RegisterConCBF(ConnectCBF);
		Client->RegisterDisconCBF(DisconnectCBF);

		Client->RegisterHeaderAndStaticFunc((uint16_t)EMyPacketHeader::Accept, RecvAccept);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EMyPacketHeader::Reject, RecvReject);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EMyPacketHeader::Create, RecvCreate);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EMyPacketHeader::Move, RecvMove);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EMyPacketHeader::Exit, RecvExit);
	}
}
CMyClient::~CMyClient()
{
	DeleteCriticalSection(&csMyInfoOfClient);
}

CMyClient* CMyClient::GetSingleton()
{
	static CMyClient myClient;
	return &myClient;
}

bool CMyClient::Initialize(const char* const IPv4, const USHORT& Port)
{
	if (!Client)
	{
		CONSOLE_LOG("[Error] <CMyClient::Initialize(...)> if (!Client) \n");
		return false;
	}
	/****************************************/

	return Client->Initialize(IPv4, Port);
}
bool CMyClient::IsNetworkOn()
{
	CONSOLE_LOG("[START] <CMyClient::IsNetworkOn()>\n");

	if (!Client)
	{
		CONSOLE_LOG("[Error] <CMyClient::IsNetworkOn())> if (!Client) \n");
		return false;
	}
	/****************************************/

	return Client->IsNetworkOn();
}
void CMyClient::Close()
{
	CONSOLE_LOG("[START] <CMyClient::Close()>\n");

	if (!Client)
	{
		CONSOLE_LOG("[Error] <CMyClient::Close())> if (!Client) \n");
		return;
	}
	/****************************************/

	Client->Close();

	CONSOLE_LOG("[END] <CMyClient::Close()>\n");
}

void CMyClient::ConnectCBF(CCompletionKey CompletionKey)
{
	CONSOLE_LOG("[Start] <CMyClient::ConnectCBF(...)> \n");

	CompletionKey.PrintInfo("\t <CMyClient::ConnectCBF(...)>");

	CONSOLE_LOG("[End] <CMyClient::ConnectCBF(...)> \n");
}
void CMyClient::DisconnectCBF(CCompletionKey CompletionKey)
{
	CONSOLE_LOG("[Start] <CMyClient::DisconnectCBF(...)> \n");

	CompletionKey.PrintInfo("\t <CMyClient::ConnectCBF(...)>");

	CONSOLE_LOG("[End] <CMyClient::DisconnectCBF(...)> \n");
}

void CMyClient::SendLogin()
{
	CONSOLE_LOG("[Start] <CMyClient::SendLogin()> \n");

	EnterCriticalSection(&csMyInfoOfClient);
	CInfoOfClient infoOfClient = MyInfoOfClient;
	LeaveCriticalSection(&csMyInfoOfClient);

	CPacket packet((uint16_t)EMyPacketHeader::Login);
	packet.GetData() << infoOfClient << endl;
	if (Client) Client->Send(packet);

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

	CPacket packet((uint16_t)EMyPacketHeader::Move);
	packet.GetData() << infoOfClient << endl;
	if (Client) Client->Send(packet);

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