#include "pch.h"

#include "NetworkComponent.h"

#include "Server.h"
#include "Client.h"


/////////////////////
// 기본
/////////////////////
CNetworkComponent::CNetworkComponent(ENetworkComponentType NCT /*= NetworkComponentType::NCT_None*/)
{
	this->NCT = NCT;

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
}
CNetworkComponent::~CNetworkComponent()
{
	Close();
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
		CONSOLE_LOG("[Error] <CNetworkComponent::CloseNetwork(...)> default: \n");
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
			Server->Send(SendStream, Socket);
		}
		break;
	}
	case ENetworkComponentType::NCT_Client:
	{
		if (Client)
		{
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
			Server->SendHugeData(SendStream, Socket);
		}
		break;
	}
	case ENetworkComponentType::NCT_Client:
	{
		if (Client)
		{
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
		CONSOLE_LOG("[Error] <CNetworkComponent::IsNetworkOn()> default: \n");
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
		CONSOLE_LOG("[Error] <CNetworkComponent::IsNetworkOn()> default: \n");
		break;
	}
	}
}


/////////////////////
// only 클라
/////////////////////
