
#include <random>

#include "../NetworkComponent/Console.h"
#include "../NetworkComponent/NetworkComponent.h"

#include "MyClient.h"

unique_ptr<CNetworkComponent> CMyClient::Client;

CThreadSafetyQueue<CPlayerPacket> CMyClient::tsqPlayerPacket;

CMyClient::CMyClient()
{
	Client = make_unique<CNetworkComponent>(ENetworkComponentType::NCT_Client);
	if (Client)
	{
		Client->RegisterConCBF(ConnectCBF); // 서버에 접속하면 실행할 정적 콜백함수를 등록합니다.
		Client->RegisterDisconCBF(DisconnectCBF); // 서버와 접속을 종료하면 실행할 정적 콜백함수를 등록합니다.

		Client->RegisterHeaderAndStaticFunc((uint16_t)EMyPacketHeader::Data, RecvData); // 패킷의 헤더에 대응하여 실행할 정적 함수를 등록합니다.
		Client->RegisterHeaderAndStaticFunc((uint16_t)EMyPacketHeader::BigData, RecvBigData); // 패킷의 헤더에 대응하여 실행할 정적 함수를 등록합니다.
	}
}
CMyClient::~CMyClient()
{
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
	CompletionKey.PrintInfo("\t <CMyClient::DisconnectCBF(...)>");

	CONSOLE_LOG("[End] <CMyClient::DisconnectCBF(...)> \n");
}

void CMyClient::SendData()
{
	CONSOLE_LOG("[Start] <CMyClient::SendData()> \n");

	CPlayerPacket player("Hello world.", 1, 2, 3);
	player.PrintInfo("\t <CMyClient::SendData()>");

	CPacket dataPacket((uint16_t)EMyPacketHeader::Data);
	dataPacket.GetData() << player << endl;
	if (Client) Client->Send(dataPacket);

	CONSOLE_LOG("[End] <CMyClient::SendData()> \n");
}
void CMyClient::RecvData(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Start] <CMyClient::RecvData(...)> \n");

	CPlayerPacket player;
	if (RecvStream >> player)
	{
		tsqPlayerPacket.push(player);
		player.PrintInfo("\t <CMyClient::RecvData(...)>");
	}

	CONSOLE_LOG("[End] <CMyClient::RecvData(...)> \n");
}
void CMyClient::SendBigData()
{
	CONSOLE_LOG("[Start] <CMyClient::SendBigData()> \n");

	random_device rd;
	mt19937_64 rand(rd());
	uniform_int_distribution<int> range(0, 10);

	CPlayerPacket player;
	CPacket dataPacket((uint16_t)EMyPacketHeader::BigData);

	for (int i = 0; i < 26; i++)
	{
		player.Message = "Hello world.";
		player.PosX = (float)range(rand);
		player.PosY = (float)range(rand);
		player.PosZ = (float)range(rand);
		player.PrintInfo("\t <CMyClient::SendBigData()>");

		dataPacket.GetData() << player << endl;
	}
	if (Client) Client->Send(dataPacket);

	CONSOLE_LOG("[End] <CMyClient::SendBigData()> \n");
}
void CMyClient::RecvBigData(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Start] <CMyClient::RecvBigData(...)> \n");

	CPlayerPacket player;
	while (RecvStream >> player)
	{
		tsqPlayerPacket.push(player);
		player.PrintInfo("\t <CMyClient::RecvBigData(...)>");
	}

	CONSOLE_LOG("[End] <CMyClient::RecvBigData(...)> \n");
}