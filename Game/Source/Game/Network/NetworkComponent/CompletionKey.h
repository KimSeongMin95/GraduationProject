﻿#pragma once

#include "NetworkHeader.h"

class CCompletionKey final
{
public:
	CCompletionKey();

public:
	SOCKET	Socket;

	string	IPv4Addr; // 메인 클라이언트의 IP 주소
	USHORT	Port;	  // 메인 클라이언트의 Port 주소

public:
	void PrintInfo(const char* c_str);
};