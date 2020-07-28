/**************************************************************************
* ������(author) && ���۱���(Copyright holder): �輺��(Kim Seongmin)
* Current Affiliation(20/07/23): ȫ�ʹ��б� ����ķ�۽� ���Ӽ���Ʈ���� ���� 4�г�
* NetworkComponent: IOCP ���� ����Ͽ� ���� ������ TCP ��Ʈ��ũ �����Դϴ�.
* Program: VisualStudio 2019
* E-mail: ksm950310@naver.com
* License: X (������ �����Ӱ� ����ϼŵ� �����ϴ�.)
* Github: https://github.com/KimSeongMin95/GraduationProject
* ����: NetworkComponent ��ü�� �����Ͽ� ������ Ŭ���̾�Ʈ�� ����� �� �ֽ��ϴ�.
* (�ڵ尡 ������ �����ø�, ���� ���� ���α׷��ӷ� ����� �� �ְ� ���� ��Ź�帳�ϴ�.)
* (�ϴ� Ŭ���̾�Ʈ ���α׷��Ӹ� �����ϰ� �ֽ��ϴ�. �����մϴ�!)
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
	friend class CNetworkComponent; // CNetworkComponent������ �������̽� �Լ��� ȣ���� �� �ֵ��� �մϴ�.

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
	// ������ Ȯ���ϰ� �ݽ��ϴ�.
	void CloseSocketWithCheck(SOCKET& Socket);
	void CloseSocketWithCheck(const SOCKET& Socket);

	// ������ �ݰ� winsock ���̺귯���� �����մϴ�.
	void CloseSocketAndWSACleanup(SOCKET& Socket);

	// �ڵ��� Ȯ���ϰ� �ݽ��ϴ�.
	void CloseHandleWithCheck(HANDLE& Handle);

	// ������ ���� ũ�⸦ �����մϴ�.
	void SetSockOpt(const SOCKET& Socket, const int& SizeOfSendBuf, const int& SizeOfRecvBuf);

public:
	static void SetIPv4AndPort(char* IPv4, USHORT& Port);

	static void GetKoreaStandardTime(stringstream& TimeStream);
};

// ������ ��Ŷ�� ó���ϴ� �Լ� ������
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

// Ŭ���̾�Ʈ�� �����ϸ� ������ �ݹ��Լ� ������
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