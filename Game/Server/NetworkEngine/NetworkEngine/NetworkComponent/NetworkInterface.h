
#pragma once

#include "NetworkHeader.h"
#include "NetworkConfig.h"
#include "CompletionKey.h"
#include "OverlappedMsg.h"
#include "Packet.h"

class INetworkInterface
{
public:
	INetworkInterface() {}
	virtual ~INetworkInterface() {}

public:
	friend class CNetworkComponent; // CNetworkComponent에서만 인터페이스 함수를 호출할 수 있도록 합니다.

protected:
	virtual bool Initialize(const char* const IPv4, const USHORT& Port) = 0;
	virtual bool IsNetworkOn() = 0;
	virtual CCompletionKey GetCompletionKey(const SOCKET& Socket = NULL) = 0;
	virtual void Close() = 0;
	virtual void RegisterHeaderAndStaticFunc(const uint16_t& PacketHeader, void(*StaticFunc)(stringstream&, const SOCKET&)) = 0;
	virtual void RegisterConCBF(void(*StaticCBFunc)(CCompletionKey)) = 0;
	virtual void RegisterDisconCBF(void(*StaticCBFunc)(CCompletionKey)) = 0;
	virtual void Send(CPacket& Packet, const SOCKET& Socket = NULL) = 0;

	virtual void Broadcast(CPacket& Packet) {}
	virtual void BroadcastExceptOne(CPacket& Packet, const SOCKET& Except) {}

protected:
	// 소켓을 확인하고 닫습니다.
	void CloseSocketWithCheck(SOCKET& Socket);
	void CloseSocketWithCheck(const SOCKET& Socket);

	// 소켓을 닫고 winsock 라이브러리를 해제합니다.
	void CloseSocketAndWSACleanup(SOCKET& Socket);

	// 핸들을 확인하고 닫습니다.
	void CloseHandleWithCheck(HANDLE& Handle);

	// 소켓의 버퍼 크기를 변경합니다.
	void SetSockOpt(const SOCKET& Socket, const int& SizeOfSendBuf, const int& SizeOfRecvBuf);

public:
	static void SetIPv4AndPort(char* IPv4, USHORT& Port);

	static void GetKoreaStandardTime(stringstream& TimeStream);
};

// 수신한 패킷을 처리하는 함수 포인터
class CProcessingFuncPtr final
{
public:
	CProcessingFuncPtr();

private:
	void(*Func)(stringstream& RecvStream, const SOCKET& Socket);

public:
	void SetFunc(void(*StaticFunc)(stringstream&, const SOCKET&));
	void ExecuteFunc(stringstream& RecvStream, const SOCKET& Socket);
};

// 클라이언트가 접속하면 실행할 콜백함수 포인터
class CCallBackFuncPtr final
{
public:
	CCallBackFuncPtr();

private:
	void(*Func)(CCompletionKey CompletionKey);

public:
	void SetFunc(void(*StaticCBF)(CCompletionKey));
	void ExecuteFunc(CCompletionKey CompletionKey);
};
