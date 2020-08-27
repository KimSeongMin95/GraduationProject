#pragma once

#include "NetworkInterface.h"

class CClient final : public INetworkInterface
{
public:
	CClient();
	virtual ~CClient();

private:
	CProcessingFuncPtr  ProcFuncs[MAX_HEADER]; // ������ ��Ŷ�� ó���ϴ� �Լ� ������
	CCallBackFuncPtr	ConCBF;				   // ������ �����ϸ� ������ �ݹ��Լ� ������
	CCallBackFuncPtr	DisconCBF;			   // ������ ������ ����Ǹ� ������ �ݹ��Լ� ������

	SOCKET				ServerSocket;		   // ������ ������ ����	

	bool				bAccept;			   // Ŭ���̾�Ʈ ���� Ȯ�ο�
	CRITICAL_SECTION	csAccept;			   // Ŭ���̾�Ʈ ���� Ȯ�� ����ȭ

	HANDLE				hClientThreadHandle;   // Ŭ���̾�Ʈ ������ �ڵ�

	CCompletionKey		Server;				   // ���� ����
	CRITICAL_SECTION	csServer;			   // ���� ���� ����ȭ

	deque<unique_ptr<char[]>> RecvDeque;	   // ������ ��� �����͸� �����մϴ�.

private:
	/////////////////////////////////////////
	// INetworkInterface virtual Functions
	/////////////////////////////////////////
	// Ŭ���̾�Ʈ�� �ʱ�ȭ�� Ŭ���̾�Ʈ ������ ������ �õ��մϴ�.
	virtual bool Initialize(const char* const IPv4, const USHORT& Port) final;

	// Ŭ���̾�Ʈ�� �������� ������ Ȯ���մϴ�.
	virtual bool IsNetworkOn() final;

	// ������ CCompletionKey�� ȹ������;��/
	virtual CCompletionKey GetCompletionKey(const SOCKET& Socket = NULL) final;

	// Ŭ���̾�Ʈ�� ������ �����մϴ�.
	virtual void Close() final;

	// ��Ŷ�� ����� �����Ͽ� ������ ���� �Լ��� ����մϴ�.
	virtual void RegisterHeaderAndStaticFunc(const uint16_t& PacketHeader, void(*StaticFunc)(stringstream&, const SOCKET&)) final;

	// ������ �����ϸ� ������ ���� �ݹ��Լ��� ����մϴ�.
	virtual void RegisterConCBF(void(*StaticCBFunc)(CCompletionKey)) final;

	// ������ ������ �����ϸ� ������ ���� �ݹ��Լ��� ����մϴ�.
	virtual void RegisterDisconCBF(void(*StaticCBFunc)(CCompletionKey)) final;

	// �������� ��Ŷ�� �۽��մϴ�.
	virtual void Send(CPacket& Packet, const SOCKET& Socket = NULL) final;

	////////////////////
	// Main Functions
	////////////////////
	// Ŭ���̾�Ʈ �����带 �����մϴ�.
	bool CreateClientThread();

	// WSASend(...)�Լ��� ��Ŷ�� OverlappedMsg�� �񵿱� ��ø ������� �۽��մϴ�. 
	void Send(COverlappedMsg* OverlappedMsg);

	///////////////////
	// Sub Functions
	///////////////////
	// RecvDeque�� �޺κп� ������ �����͸� �����մϴ�.
	void LoadUpReceivedDataToRecvDeque(const char* const RecvBuffer, const int& RecvLen);

	// RecvDeque���κ��� ��Ŷ���� ȹ���մϴ�.
	void GetPacketsFromRecvDeque(char* const BufOfPackets);

	// ȹ���� ��Ŷ���� ��Ŷ���� �����ϰ� ���������� ��Ŷ�� ó���մϴ�.
	void DividePacketsAndProcessThePacket(const char* const BufOfPackets);

	// ��Ŷ�� ó���մϴ�.
	void ProcessThePacket(const char* const BufOfPacket);

	// �۽��Ϸ��� �������� ũ�⸦ �����մϴ�. 
	void SetSizeOfDataForSend(const uint32_t& IdxOfStart, uint32_t& IdxOfEnd, const uint16_t& MaxSizeOfData, const char* const C_StrOfData);

	// �񵿱� ��ø ������� �۽��� OverlappedMsg�� �����Ҵ��ϰ� ȹ���մϴ�.
	COverlappedMsg* GetOverlappedMsgForSend(const string& StrOfLengthAndHeader, const size_t& LenOfLengthAndHeader, const char* const C_StrOfData, const uint32_t& IdxOfStart, const uint16_t& sizeOfData);

public:
	/////////////////
	// Thread Call
	/////////////////
	// Ŭ���̾�Ʈ �����带 �����մϴ�.
	void RunClientThread();
};