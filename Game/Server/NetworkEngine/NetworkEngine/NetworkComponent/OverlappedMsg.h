#pragma once

#include "NetworkHeader.h"
#include "NetworkConfig.h"

class COverlappedMsg final
{
public:
	COverlappedMsg();

public:
	WSAOVERLAPPED	Overlapped;
	WSABUF			DataBuf;

	char			MessageBuffer[MAX_BUFFER];
	int				RecvBytes;
	int				SendBytes;

public:
	void Initialize();
};