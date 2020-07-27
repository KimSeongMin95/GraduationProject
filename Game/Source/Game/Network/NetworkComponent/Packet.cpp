
#include "Packet.h"

CPacket::CPacket(uint16_t&& Header)
{
	Length = 4; // 최소 크기
	this->Header = Header;
	End = (char)3; // ASCII ETX(End of Text)
};

uint16_t CPacket::GetLen()
{
	return Length;
}
uint16_t CPacket::GetHeader()
{
	return Header;
}
stringstream& CPacket::GetData()
{
	return Data;
}

void CPacket::CalculateLen(const uint16_t& SizeOfData)
{
	// 참고: 최대크기의 패킷: [("4095") ("256") (Data)(End)('\0')]
	uint16_t lenOfRemaining = 1 + (uint16_t)to_string(Header).length() + 1 + SizeOfData + 1;

	/* 계산
	1 + 4 = 5
	2 + 30 = 32
	2 + 98 = 100
	3 + 999 = 1002
	*/
	Length = (uint16_t)to_string(lenOfRemaining).length() + lenOfRemaining;

	/* 결과
	1 + 4 = 5
	2 + 30 = 32
	3 + 98 = 101
	4 + 999 = 1003
	*/
	Length = (uint16_t)to_string(Length).length() + lenOfRemaining;
}

string CPacket::GetStrOfLengthAndHeader()
{
	return (to_string(Length) + ' ' + to_string(Header) + ' ');
}

string CPacket::ReplaceCharInString(const string& str, char before, char after)
{
	string result = str;
	for (size_t i = 0; i < result.size(); i++)
	{
		if (result.at(i) == before)
			result.at(i) = after;
	}
	return result;
}
string CPacket::ReplaceCharInString(const char* const c_str, char before, char after)
{
	string result = c_str;
	for (size_t i = 0; i < result.size(); i++)
	{
		if (result.at(i) == before)
			result.at(i) = after;
	}
	return result;
}
