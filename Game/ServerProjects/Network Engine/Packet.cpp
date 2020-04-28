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

void CCompletionKey::PrintInfo()
{
	CONSOLE_LOG("\t socket: %d, IPv4Addr: %s, Port: %d \n", socket, IPv4Addr.c_str(), Port);
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
// IOCP CBasicPacket
////////////////////////
/** 기본 클래스 */
CBasicPacket::CBasicPacket()
{
	Number = 1;

	InitializeCriticalSection(&cs);
}
CBasicPacket::~CBasicPacket()
{
	DeleteCriticalSection(&cs);
}

void CBasicPacket::RegisterTypeAndStaticFunc(string Name, void(*Function)(class CNetworkComponent*, stringstream&, SOCKET))
{
	unsigned int key = 0;

	EnterCriticalSection(&cs);
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

	PPFs.emplace(key, Function);
	LeaveCriticalSection(&cs);
}

unsigned int CBasicPacket::GetNumberOfType(string Name)
{
	unsigned int num = 0;

	EnterCriticalSection(&cs);
	if (Types.find(Name) != Types.end())
	{
		num = Types.at(Name);

		LeaveCriticalSection(&cs);
		return num;
	}
	LeaveCriticalSection(&cs);

	return -1;
}

void CBasicPacket::ProcessPacket(unsigned int Type, class CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket)
{
	PacketProcessingFunc func;

	EnterCriticalSection(&cs);
	if (PPFs.find(Type) != PPFs.end())
	{
		func = (PacketProcessingFunc)PPFs.at(Type);
	}
	else
	{
		CONSOLE_LOG("[ERROR] <Packet::ProcessPacketOfServer(...)> Invalid packet type. \n");
		LeaveCriticalSection(&cs);
		return;
	}
	LeaveCriticalSection(&cs);

	// 처리 실행
	func(NC, RecvStream, Socket);
}