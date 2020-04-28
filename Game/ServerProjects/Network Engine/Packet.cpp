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
	overlapped.hEvent = NULL; // IOCP������ overlapped.hEvent�� �� NULL�� ����� �Ѵٰ� �մϴ�.
	ZeroMemory(messageBuffer, MAX_BUFFER);
	dataBuf.len = MAX_BUFFER;
	dataBuf.buf = messageBuffer;
	recvBytes = 0;
	sendBytes = 0;
}

void COverlappedMsg::Initialize()
{
	ZeroMemory(&overlapped, sizeof(OVERLAPPED));
	overlapped.hEvent = NULL; // IOCP������ overlapped.hEvent�� �� NULL�� ����� �Ѵٰ� �մϴ�.
	ZeroMemory(messageBuffer, MAX_BUFFER);
	dataBuf.len = MAX_BUFFER;
	dataBuf.buf = messageBuffer;
	recvBytes = 0;
	sendBytes = 0;
}


////////////////////////
// IOCP CBasicPacket
////////////////////////
/** �⺻ Ŭ���� */
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
	// ��ŶŸ�Ը��� ������ �������� ������
	if (Types.find(Name) == Types.end())
	{
		key = Number;
		Types.emplace(Name, Number);
		Number++;
	}
	// �̹� �����ϸ� Ű ���� �޾ƿɴϴ�.
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

	// ó�� ����
	func(NC, RecvStream, Socket);
}