#pragma once

#include "NetworkInterface.h"

class CClient final : public INetworkInterface
{
public:
	CClient();
	virtual ~CClient();

private:
	CProcessingFuncPtr  ProcFuncs[MAX_HEADER]; // 수신한 패킷을 처리하는 함수 포인터
	CCallBackFuncPtr	ConCBF;				   // 서버에 접속하면 실행할 콜백함수 포인터
	CCallBackFuncPtr	DisconCBF;			   // 서버와 접속이 종료되면 실행할 콜백함수 포인터

	SOCKET				ServerSocket;		   // 서버에 연결할 소켓	

	bool				bAccept;			   // 클라이언트 구동 확인용
	CRITICAL_SECTION	csAccept;			   // 클라이언트 구동 확인 동기화

	HANDLE				hClientThreadHandle;   // 클라이언트 스레드 핸들

	CCompletionKey		Server;				   // 서버 정보
	CRITICAL_SECTION	csServer;			   // 서버 정보 동기화

	deque<unique_ptr<char[]>> RecvDeque;	   // 수신한 모든 데이터를 적재합니다.

private:
	/////////////////////////////////////////
	// INetworkInterface virtual Functions
	/////////////////////////////////////////
	// 클라이언트의 초기화와 클라이언트 스레드 생성을 시도합니다.
	virtual bool Initialize(const char* const IPv4, const USHORT& Port) final;

	// 클라이언트의 정상적인 구동을 확인합니다.
	virtual bool IsNetworkOn() final;

	// 서버의 CCompletionKey를 획득힙ㄴ;디/
	virtual CCompletionKey GetCompletionKey(const SOCKET& Socket = NULL) final;

	// 클라이언트의 구동을 종료합니다.
	virtual void Close() final;

	// 패킷의 헤더에 대응하여 실행할 정적 함수를 등록합니다.
	virtual void RegisterHeaderAndStaticFunc(const uint16_t& PacketHeader, void(*StaticFunc)(stringstream&, const SOCKET&)) final;

	// 서버에 접속하면 실행할 정적 콜백함수를 등록합니다.
	virtual void RegisterConCBF(void(*StaticCBFunc)(CCompletionKey)) final;

	// 서버와 접속을 종료하면 실행할 정적 콜백함수를 등록합니다.
	virtual void RegisterDisconCBF(void(*StaticCBFunc)(CCompletionKey)) final;

	// 서버에게 패킷을 송신합니다.
	virtual void Send(CPacket& Packet, const SOCKET& Socket = NULL) final;

	////////////////////
	// Main Functions
	////////////////////
	// 클라이언트 스레드를 생성합니다.
	bool CreateClientThread();

	// WSASend(...)함수로 패킷인 OverlappedMsg를 비동기 중첩 방식으로 송신합니다. 
	void Send(COverlappedMsg* OverlappedMsg);

	///////////////////
	// Sub Functions
	///////////////////
	// RecvDeque의 뒷부분에 수신한 데이터를 적재합니다.
	void LoadUpReceivedDataToRecvDeque(const char* const RecvBuffer, const int& RecvLen);

	// RecvDeque으로부터 패킷들을 획득합니다.
	void GetPacketsFromRecvDeque(char* const BufOfPackets);

	// 획득한 패킷들을 패킷으로 분할하고 최종적으로 패킷을 처리합니다.
	void DividePacketsAndProcessThePacket(const char* const BufOfPackets);

	// 패킷을 처리합니다.
	void ProcessThePacket(const char* const BufOfPacket);

	// 송신하려는 데이터의 크기를 조절합니다. 
	void SetSizeOfDataForSend(const uint32_t& IdxOfStart, uint32_t& IdxOfEnd, const uint16_t& MaxSizeOfData, const char* const C_StrOfData);

	// 비동기 중첩 방식으로 송신할 OverlappedMsg을 동적할당하고 획득합니다.
	COverlappedMsg* GetOverlappedMsgForSend(const string& StrOfLengthAndHeader, const size_t& LenOfLengthAndHeader, const char* const C_StrOfData, const uint32_t& IdxOfStart, const uint16_t& sizeOfData);

public:
	/////////////////
	// Thread Call
	/////////////////
	// 클라이언트 스레드를 실행합니다.
	void RunClientThread();
};