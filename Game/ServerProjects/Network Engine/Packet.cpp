#include "pch.h"

#include "Packet.h"


////////////////////////
// IOCP CompletionKey
////////////////////////
CCompletionKey::CCompletionKey()
{
	socket = 0;
	IPv4Addr = "0.0.0.0";
	Port = 0;
}


////////////////////////
// IOCP OverlappedMsg
////////////////////////
COverlappedMsg::COverlappedMsg()
{
	ZeroMemory(&overlapped, sizeof(OVERLAPPED));
	overlapped.hEvent = NULL; // IOCP에서는 overlapped.hEvent를 꼭 NULL로 해줘야 한다고 합니다.
	ZeroMemory(messageBuffer, MAX_BUFFER);
	dataBuf.len = MAX_BUFFER;
	dataBuf.buf = messageBuffer;
	recvBytes = 0;
	sendBytes = 0;
}

void COverlappedMsg::Initialize()
{
	ZeroMemory(&overlapped, sizeof(OVERLAPPED));
	overlapped.hEvent = NULL; // IOCP에서는 overlapped.hEvent를 꼭 NULL로 해줘야 한다고 합니다.
	ZeroMemory(messageBuffer, MAX_BUFFER);
	dataBuf.len = MAX_BUFFER;
	dataBuf.buf = messageBuffer;
	recvBytes = 0;
	sendBytes = 0;
}


////////////////////////
// IOCP CPacket
////////////////////////
unsigned int CPacket::Number = 1; // 1부터 시작합니다.

map<string, unsigned int> CPacket::Types;

map<unsigned int, LPVOID> CPacket::PPFsOfServer;
map<unsigned int, LPVOID> CPacket::PPFsOfClient;


CPacket::CPacket()
{

}

void CPacket::RegisterTypeAndStaticFunc(string Name, ENetworkComponentType NCT, void(*Function)(class CNetworkComponent*, stringstream&, SOCKET))
{
	unsigned int key = 0;

	// 패킷타입명이 기존에 존재하지 않으면
	if (Types.find(Name) == Types.end())
	{
		key = Number;
		Types.emplace(Name, Number);
		Number++;
	}
	// 이미 존재하면 키 값을 받아옵니다.
	else
	{
		key = Types.at(Name);
	}

	switch (NCT)
	{
	case ENetworkComponentType::NCT_Server:
	{
		PPFsOfServer.emplace(key, Function);
		break;
	}
	case ENetworkComponentType::NCT_Client:
	{
		PPFsOfClient.emplace(key, Function);
		break;
	}
	default:
	{
		CONSOLE_LOG("[Error] <Packet::RegisterTypeAndStaticFunc(...)> default: \n");
		break;
	}
	}
}

unsigned int CPacket::GetNumberOfType(string Name)
{
	if (Types.find(Name) != Types.end())
	{
		return Types.at(Name);
	}

	return -1;
}

void CPacket::ProcessPacketOfServer(unsigned int Type, class CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket)
{
	if (PPFsOfServer.find(Type) != PPFsOfServer.end())
	{
		PacketProcessingFunc func = (PacketProcessingFunc)PPFsOfServer.at(Type);
		func(NC, RecvStream, Socket);
	}
	else
	{
		CONSOLE_LOG("[ERROR] <Packet::ProcessPacketOfServer(...)> Invalid packet type. \n");
	}
}

void CPacket::ProcessPacketOfClient(unsigned int Type, class CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket)
{
	if (PPFsOfClient.find(Type) != PPFsOfClient.end())
	{
		PacketProcessingFunc func = (PacketProcessingFunc)PPFsOfClient.at(Type);
		func(NC, RecvStream, Socket);
	}
	else
	{
		CONSOLE_LOG("[ERROR] <Packet::ProcessPacketOfClient(...)> Invalid packet type. \n");
	}
}