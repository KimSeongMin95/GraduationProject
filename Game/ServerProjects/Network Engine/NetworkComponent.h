#pragma once

#include "Packet.h"


// 클라이언트 접속 / 종료시 실행할 콜백함수의 형을 선언합니다.
typedef void* (*NC_CallBackFunc)(CCompletionKey);

class CNetworkComponent : public CPacket
{
public:
	/////////////////////
	// 기본
	/////////////////////
	/** 서버 or 클라이언트를 선택하고 
	패킷 클래스를 기본을 사용하거나 직접 패킷 인터페이스 클래스를 상속받아 정의한 클래스를 사용합니다. */
	CNetworkComponent(
		ENetworkComponentType NCT = ENetworkComponentType::NCT_None, 
		class CPacket* PacketClass = nullptr);
	~CNetworkComponent();


private:
	ENetworkComponentType NCT;

	class CPacket* Packet = nullptr;
	class CServer* Server = nullptr;
	class CClient* Client = nullptr;


	/////////////////////
	// only 서버
	/////////////////////
	NC_CallBackFunc Connect = nullptr; // 클라이언트가 접속할 때 실행할 콜백함수입니다.
	NC_CallBackFunc Disconnect = nullptr; // 클라이언트가 종료할 때 실행할 콜백함수입니다.


public:
	/////////////////////
	// 패킷 인터페이스 상속
	/////////////////////
	virtual void RegisterTypeAndStaticFunc(string Name, void (*Function)(class CNetworkComponent*, stringstream&, SOCKET)) final;
	virtual unsigned int GetNumberOfType(string Name) final;
	virtual void ProcessPacket(unsigned int Type, class CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket) final;


	/////////////////////
	// 서버 & 클라 공통
	/////////////////////
	bool Initialize(const char* IPv4, USHORT Port);
	void Close();
	void Send(stringstream& SendStream, SOCKET Socket = 0); // 0이면 클라이언트
	void SendHugeData(stringstream& SendStream, SOCKET Socket = 0); // 0이면 클라이언트
	bool IsNetworkOn();

	
	/////////////////////
	// only 서버
	/////////////////////
	void RegisterCBF(void (*Connect)(CCompletionKey) = nullptr, void (*Disconnect)(CCompletionKey) = nullptr); // 콜백함수를 등록합니다.
	CCompletionKey GetCompletionKey(SOCKET Socket);
	void ExecuteConnectCBF(CCompletionKey CompletionKey); // 클라이언트가 접속할 때 실행합니다.
	void ExecuteDisconnectCBF(CCompletionKey CompletionKey); // 클라이언트가 종료할 때 실행합니다.
	void Broadcast(stringstream& SendStream);
	void BroadcastExceptOne(stringstream& SendStream, SOCKET Except);


	/////////////////////
	// only 클라
	/////////////////////

};