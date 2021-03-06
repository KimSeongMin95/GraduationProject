﻿#pragma once

#include "NetworkInterface.h"

enum class ENetworkComponentType : uint16_t
{
	NCT_None,
	NCT_Server,
	NCT_Client
};

class CNetworkComponent final : private INetworkInterface
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