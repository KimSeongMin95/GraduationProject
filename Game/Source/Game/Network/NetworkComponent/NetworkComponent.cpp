
#include "NetworkComponent.h"

#include "Console.h"
#include "Server.h"
#include "Client.h"

CNetworkComponent::CNetworkComponent(ENetworkComponentType NCT /*= NetworkComponentType::NCT_None*/)
{
	this->NCT = NCT;

	// 네트워크를 서버 또는 클라이언트로 초기화합니다.
	switch (NCT)
	{
	case ENetworkComponentType::NCT_Server:
	{
		Network = std::make_unique<CServer>();
		break;
	}
	case ENetworkComponentType::NCT_Client:
	{
		Network = std::make_unique<CClient>();
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

bool CNetworkComponent::Initialize(const char* const IPv4, const USHORT& Port)
{
	if (!Network)
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::Initialize(...)> if (!Network) \n");
		return false;
	}
	/******************************************/

	return Network->Initialize(IPv4, Port);
}

bool CNetworkComponent::IsNetworkOn()
{
	if (!Network)
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::IsNetworkOn()> if (!Network) \n");
		return false;
	}
	/******************************************/

	return Network->IsNetworkOn();
}

CCompletionKey CNetworkComponent::GetCompletionKey(const SOCKET& Socket /*= NULL*/)
{
	if (!Network)
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::GetCompletionKey(...)> if (!Network) \n");
		return CCompletionKey();
	}
	/******************************************/

	return Network->GetCompletionKey(Socket);
}

void CNetworkComponent::Close()
{
	if (!Network)
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::Close()> if (!Network) \n");
		return;
	}
	/******************************************/

	Network->Close();
}

void CNetworkComponent::RegisterHeaderAndStaticFunc(const uint16_t& PacketHeader, void(*StaticFunc)(stringstream&, const SOCKET&))
{
	if (!Network)
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::RegisterHeaderAndStaticFunc(...)> if (!Network) \n");
		return;
	}
	/******************************************/

	Network->RegisterHeaderAndStaticFunc(PacketHeader, StaticFunc);
}

void CNetworkComponent::RegisterConCBF(void(*StaticCBFunc)(CCompletionKey))
{
	if (!Network)
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::RegisterConCBF(...)> if (!Network) \n");
		return;
	}
	/******************************************/

	Network->RegisterConCBF(StaticCBFunc);
}

void CNetworkComponent::RegisterDisconCBF(void(*StaticCBFunc)(CCompletionKey))
{
	if (!Network)
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::RegisterDisconCBF(...)> if (!Network) \n");
		return;
	}
	/******************************************/

	Network->RegisterDisconCBF(StaticCBFunc);
}

void CNetworkComponent::Send(CPacket& Packet, const SOCKET& Socket /*= NULL*/)
{
	if (!Network)
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::Send(...)> if (!Network) \n");
		return;
	}
	/******************************************/

	if (Network->IsNetworkOn())
		Network->Send(Packet, Socket);
}

void CNetworkComponent::Broadcast(CPacket& Packet)
{
	if (!Network)
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::Broadcast(...)> if (!Network) \n");
		return;
	}
	/******************************************/

	if (Network->IsNetworkOn())
		Network->Broadcast(Packet);
}

void CNetworkComponent::BroadcastExceptOne(CPacket& Packet, const SOCKET& Except)
{
	if (!Network)
	{
		CONSOLE_LOG("[Error] <CNetworkComponent::BroadcastExceptOne(...)> if (!Network) \n");
		return;
	}
	/******************************************/

	if (Network->IsNetworkOn())
		Network->BroadcastExceptOne(Packet, Except);
}