#pragma once

#include "../NetworkComponent/NetworkHeader.h"
#include "../NetworkComponent/Packet.h"
#include "../NetworkComponent/Console.h"

class CInfoOfClient
{
public:
	CInfoOfClient()
	{
		ID = "ID";
		PosX = 0.0f;
		PosY = 0.0f;
		PosZ = 0.0f;
	}
	~CInfoOfClient()
	{
	}

public:
	string ID;

	float PosX;
	float PosY;
	float PosZ;

public:
	// Send
	friend ostream& operator<<(ostream& Stream, CInfoOfClient& ref)
	{
		Stream << CPacket::ReplaceCharInString(ref.ID, ' ', '_') << ' ';
		Stream << ref.PosX << ' ';
		Stream << ref.PosY << ' ';
		Stream << ref.PosZ << ' ';

		return Stream;
	}

	// Recv
	friend istream& operator>>(istream& Stream, CInfoOfClient& ref)
	{
		Stream >> ref.ID;
		ref.ID = CPacket::ReplaceCharInString(ref.ID, '_', ' ');
		Stream >> ref.PosX;
		Stream >> ref.PosY;
		Stream >> ref.PosZ;

		return Stream;
	}

	// Print
	void PrintInfo(const char* c_str)
	{
		CONSOLE_LOG("%s ID: %s, PosX: %f, PosX: %f, PosX: %f \n", c_str, ID.c_str(), PosX, PosY, PosZ);
	}
};