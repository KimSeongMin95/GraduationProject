/***********************************************************************
* 저작자(author) && 저작권자(Copyright holder): 김성민(Kim Seongmin)
* 현재 소속(20/07/23): 홍익대학교 세종캠퍼스 게임소프트웨어 전공 4학년
* E-mail: ksm950310@naver.com
* License: X, 자유롭게 사용하셔도 좋습니다.
* 설명: 직접 개발한 IOCP 기반의 TCP 네트워크 엔진입니다.
* (코드가 마음에 들으시면, 제가 게임 프로그래머로 취업할 수 있게 연락 부탁드립니다.)
* (일단 클라이언트 프로그래머를 지망하고 있습니다.)
************************************************************************/

#pragma once

#include "NetworkInterface.h"

enum ENetworkComponentType
{
	NCT_None,
	NCT_Server,
	NCT_Client
};

class CNetworkComponent sealed : private INetworkInterface
{
public:
	CNetworkComponent(ENetworkComponentType NCT = ENetworkComponentType::NCT_None);
	virtual ~CNetworkComponent();

private:
	ENetworkComponentType NCT;

	std::unique_ptr<INetworkInterface> Network = nullptr;

public:
	/////////////////////////////////////////////////////////
	// INetworkInterface virtual Functions (서버 & 클라 공통)
	/////////////////////////////////////////////////////////
	virtual bool Initialize(const char* const IPv4, const USHORT& Port) final;
	virtual bool IsNetworkOn() final;
	virtual CCompletionKey GetCompletionKey(const SOCKET& Socket = NULL) final;
	virtual void Close() final;
	virtual void RegisterHeaderAndStaticFunc(const uint16_t& PacketHeader, void(*StaticFunc)(stringstream&, const SOCKET&)) final;
	virtual void RegisterConCBF(void(*StaticCBFunc)(CCompletionKey)) final;
	virtual void RegisterDisconCBF(void(*StaticCBFunc)(CCompletionKey)) final;
	virtual void Send(CPacket& Packet, const SOCKET& Socket = NULL) final;

	/////////////////////////////////////////////////////////
	// INetworkInterface virtual Functions (서버)
	/////////////////////////////////////////////////////////
	virtual void Broadcast(CPacket& Packet) final;
	virtual void BroadcastExceptOne(CPacket& Packet, const SOCKET& Except) final;
};