
#include "OverlappedMsg.h"

COverlappedMsg::COverlappedMsg()
{
	Initialize();
}

void COverlappedMsg::Initialize()
{
	ZeroMemory(&Overlapped, sizeof(OVERLAPPED));
	Overlapped.hEvent = NULL; // IOCP에서는 overlapped.hEvent를 꼭 NULL로 해줘야 한다고 합니다.
	ZeroMemory(MessageBuffer, MAX_BUFFER);
	DataBuf.len = MAX_BUFFER;
	DataBuf.buf = MessageBuffer;
	RecvBytes = 0;
	SendBytes = 0;
}
