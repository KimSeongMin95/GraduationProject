#pragma once

#include "../NetworkComponent/NetworkHeader.h"
#include "../NetworkComponent/Packet.h"
#include "../NetworkComponent/Console.h"

class CPlayerPacket
{
public:
	CPlayerPacket()
	{
		Message = "Message";
		PosX = 0.0f;
		PosY = 0.0f;
		PosZ = 0.0f;
	}
	CPlayerPacket(string Message, float PosX, float PosY, float PosZ)
		: Message(Message), PosX(PosX), PosY(PosY), PosZ(PosZ) {}
	~CPlayerPacket() {}

public:
	string Message;

	float PosX;
	float PosY;
	float PosZ;

public:
	// Send
	friend ostream& operator<<(ostream& Stream, CPlayerPacket& ref)
	{
		Stream << CPacket::ReplaceCharInString(ref.Message, ' ', '_') << ' ';
		Stream << ref.PosX << ' ';
		Stream << ref.PosY << ' ';
		Stream << ref.PosZ << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, CPlayerPacket& ref)
	{
		Stream >> ref.Message;
		ref.Message = CPacket::ReplaceCharInString(ref.Message, '_', ' ');
		Stream >> ref.PosX;
		Stream >> ref.PosY;
		Stream >> ref.PosZ;

		return Stream;
	}

	// Print
	void PrintInfo(const char* c_str)
	{
		CONSOLE_LOG("%s Message: %s, PosX: %f, PosX: %f, PosX: %f \n", c_str, Message.c_str(), PosX, PosY, PosZ);
	}
};