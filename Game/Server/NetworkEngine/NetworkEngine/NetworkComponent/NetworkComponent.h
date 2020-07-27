/**************************************************************************
* ������(author) && ���۱���(Copyright holder): �輺��(Kim Seongmin)
* Current Affiliation(20/07/23): ȫ�ʹ��б� ����ķ�۽� ���Ӽ���Ʈ���� ���� 4�г�
* NetworkComponent: IOCP ���� ����Ͽ� ���� ������ TCP ��Ʈ��ũ �����Դϴ�.
* E-mail: ksm950310@naver.com
* License: X (������ �����Ӱ� ����ϼŵ� �����ϴ�.)
* Github: https://github.com/KimSeongMin95/GraduationProject
* Caution: �ڵ忡 ���װ� ������ �� �ֽ��ϴ�. �� �� �����Ͻñ� �ٶ��ϴ�.
* (�ڵ尡 ������ �����ø�, ���� ���� ���α׷��ӷ� ����� �� �ְ� ���� ��Ź�帳�ϴ�.)
* (�ϴ� Ŭ���̾�Ʈ ���α׷��Ӹ� �����ϰ� �ֽ��ϴ�. �����մϴ�!)
***************************************************************************/

#pragma once

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
	// INetworkInterface virtual Functions (���� & Ŭ�� ����)
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
	// INetworkInterface virtual Functions (����)
	/////////////////////////////////////////////////////////
	virtual void Broadcast(CPacket& Packet) final;
	virtual void BroadcastExceptOne(CPacket& Packet, const SOCKET& Except) final;
};