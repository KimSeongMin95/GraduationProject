#include "pch.h"

#include "NetworkComponent.h"

#include "Server.h"
#include "Client.h"


/////////////////////
// 기본
/////////////////////
CNetworkComponent::CNetworkComponent(ENetworkComponentType NCT /*= NetworkComponentType::NCT_None*/, class CPacket* PacketClass /*= nullptr*/)
{
	this->NCT = NCT;

	// 서버 or 클라이언트 설정
	switch (NCT)
	{
	case ENetworkComponentType::NCT_Server:
	{
		//Server = CServer::GetSingleton();
		Server = new CServer();
		if (Server)
			Server->SetNetworkComponent(this);
		break;
	}
	case ENetworkComponentType::NCT_Client:
	{
		//Client = CClient::GetSingleton();
		Client = new CClient();
		if (Client)
			Client->SetNetworkComponent(this);
		break;
	}
	default:
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::CNetworkComponent(...)> default: \n");
		break;
	}
	}

	// 패킷 클래스 설정
	if (Packet)
	{
		this->Packet = PacketClass;
	}
	else
	{
		Packet = new CBasicPacket();
	}
}
CNetworkComponent::~CNetworkComponent()
{
	Close();


	if (Packet)
	{
		delete Packet;
		Packet = nullptr;
	}
	if (Server)
	{
		delete Server;
		Server = nullptr;
	}
	if (Client)
	{
		delete Client;
		Client = nullptr;
	}
}


/////////////////////
// 패킷 인터페이스 상속
/////////////////////
void CNetworkComponent::RegisterTypeAndStaticFunc(string Name, void(*Function)(class CNetworkComponent*, stringstream&, SOCKET))
{
	if (!Packet)
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::RegisterTypeAndStaticFunc(...)> if (!Packet) \n");
		return;
	}

	Packet->RegisterTypeAndStaticFunc(Name, Function);
}
unsigned int CNetworkComponent::GetNumberOfType(string Name)
{
	if (!Packet)
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::GetNumberOfType(...)> if (!Packet) \n");
		return -1;
	}

	return Packet->GetNumberOfType(Name);
}
void CNetworkComponent::ProcessPacket(unsigned int Type, class CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket)
{
	if (!Packet)
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::ProcessPacket(...)> if (!Packet) \n");
		return;
	}

	Packet->ProcessPacket(Type, NC, RecvStream, Socket);
}


/////////////////////
// 서버 & 클라 공통
/////////////////////
bool CNetworkComponent::Initialize(const char* IPv4, USHORT Port)
{
	switch (NCT)
	{
	case ENetworkComponentType::NCT_Server:
	{
		if (Server)
			return Server->Initialize(IPv4, Port);
		break;
	}
	case ENetworkComponentType::NCT_Client:
	{
		if (Client)
			return Client->Initialize(IPv4, Port);
		break;
	}
	default:
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::Initialize(...)> default: \n");
		break;
	}
	}

	return false;
}

void CNetworkComponent::Close()
{
	switch (NCT)
	{
	case ENetworkComponentType::NCT_Server:
	{
		if (Server)
			Server->CloseServer();
		break;
	}
	case ENetworkComponentType::NCT_Client:
	{
		if (Client)
			Client->CloseClient();
		break;
	}
	default:
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::Close()> default: \n");
		break;
	}
	}
}

void CNetworkComponent::Send(stringstream& SendStream, SOCKET Socket /*= 0*/)
{
	switch (NCT)
	{
	case ENetworkComponentType::NCT_Server:
	{
		if (Server)
		{
			if (Server->IsServerOn())
				Server->Send(SendStream, Socket);
		}
		break;
	}
	case ENetworkComponentType::NCT_Client:
	{
		if (Client)
		{
			if (Client->IsClientOn())
				Client->Send(SendStream);
		}
		break;
	}
	default:
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::Send(...)> default: \n");
		break;
	}
	}
}

void CNetworkComponent::SendHugeData(stringstream& SendStream, SOCKET Socket /*= 0*/)
{
	switch (NCT)
	{
	case ENetworkComponentType::NCT_Server:
	{
		if (Server)
		{
			if (Server->IsServerOn())
				Server->SendHugeData(SendStream, Socket);
		}
		break;
	}
	case ENetworkComponentType::NCT_Client:
	{
		if (Client)
		{
			if (Client->IsClientOn())
				Client->SendHugeData(SendStream);
		}
		break;
	}
	default:
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::SendHugeData(...)> default: \n");
		break;
	}
	}
}

bool CNetworkComponent::IsNetworkOn()
{
	switch (NCT)
	{
	case ENetworkComponentType::NCT_Server:
	{
		if (Server)
		{
			return Server->IsServerOn();
		}
		break;
	}
	case ENetworkComponentType::NCT_Client:
	{
		if (Client)
		{
			return Client->IsClientOn();
		}
		break;
	}
	default:
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::IsNetworkOn()> default: \n");
		break;
	}
	}

	return false;
}


/////////////////////
// only 서버
/////////////////////
void CNetworkComponent::RegisterCBF(void (*Connect)(CCompletionKey) /*= nullptr*/, void (*Disconnect)(CCompletionKey) /*= nullptr*/)
{
	switch (NCT)
	{
	case ENetworkComponentType::NCT_Server:
	{
		if (Server)
		{
			this->Connect = (NC_CallBackFunc)Connect;
			this->Disconnect = (NC_CallBackFunc)Disconnect;
		}
		break;
	}
	default:
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::RegisterCBF(...)> default: \n");
		break;
	}
	}
}

CCompletionKey CNetworkComponent::GetCompletionKey(SOCKET Socket)
{
	switch (NCT)
	{
	case ENetworkComponentType::NCT_Server:
	{
		if (Server)
		{
			return Server->GetCompletionKey(Socket);
		}
		break;
	}
	default:
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::GetCompletionKey(...)> default: \n");
		break;
	}
	}

	return CCompletionKey();
}

void CNetworkComponent::ExecuteConnectCBF(CCompletionKey CompletionKey)
{
	switch (NCT)
	{
	case ENetworkComponentType::NCT_Server:
	{
		if (Server)
		{
			if (Connect)
				Connect(CompletionKey);
		}
		break;
	}
	default:
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::ExecuteConnectCBF(...)> default: \n");
		break;
	}
	}
}

void CNetworkComponent::ExecuteDisconnectCBF(CCompletionKey CompletionKey)
{
	switch (NCT)
	{
	case ENetworkComponentType::NCT_Server:
	{
		if (Server)
		{
			if (Disconnect)
				Disconnect(CompletionKey);
		}
		break;
	}
	default:
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::ExecuteDisconnectCBF(...)> default: \n");
		break;
	}
	}
}

void CNetworkComponent::Broadcast(stringstream& SendStream)
{
	switch (NCT)
	{
	case ENetworkComponentType::NCT_Server:
	{
		if (Server)
		{
			return Server->Broadcast(SendStream);
		}
		break;
	}
	default:
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::Broadcast(...)> default: \n");
		break;
	}
	}
}

void CNetworkComponent::BroadcastExceptOne(stringstream& SendStream, SOCKET Except)
{
	switch (NCT)
	{
	case ENetworkComponentType::NCT_Server:
	{
		if (Server)
		{
			return Server->BroadcastExceptOne(SendStream, Except);
		}
		break;
	}
	default:
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::BroadcastExceptOne(...)> default: \n");
		break;
	}
	}
}


/////////////////////
// only 클라
/////////////////////
