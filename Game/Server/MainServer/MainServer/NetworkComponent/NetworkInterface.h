/**************************************************************************
* 저작자(author) && 저작권자(Copyright holder): 김성민(Kim Seongmin)
* Current Affiliation(20/07/23): 홍익대학교 세종캠퍼스 게임소프트웨어 전공 4학년
* NetworkComponent: IOCP 모델을 사용하여 직접 개발한 TCP 네트워크 엔진입니다.
* E-mail: ksm950310@naver.com
* License: X (누구나 자유롭게 사용하셔도 좋습니다.)
* Github: https://github.com/KimSeongMin95/GraduationProject
* Caution: 코드에 버그가 존재할 수 있습니다. 이 점 유의하시기 바랍니다.
* (코드가 마음에 들으시면, 제가 게임 프로그래머로 취업할 수 있게 연락 부탁드립니다.)
* (일단 클라이언트 프로그래머를 지망하고 있습니다. 감사합니다!)
***************************************************************************/

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