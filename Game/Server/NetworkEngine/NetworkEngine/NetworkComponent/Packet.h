#pragma once

#include "NetworkHeaders.h"

class CPacket sealed
{
private:
	uint16_t Length;   // 전체크기
	uint16_t Header;   // 헤더
	stringstream Data; // 데이터
	char End;		   // 끝

public:
	explicit CPacket(uint16_t&& Header);

	uint16_t GetLen();
	uint16_t GetHeader();
	stringstream& GetData();

	// 패킷의 전체크기를 구합니다.
	void CalculateLen(const uint16_t& SizeOfData);

	// 패킷의 전체크기를 구한 뒤, 전체크기와 헤더를 string으로 변환하여 반환합니다.
	string GetStrOfLengthAndHeader();

	// 특정 문자를 다른 문자로 교체합니다.
	static string ReplaceCharInString(const string& str, char before, char after);
	static string ReplaceCharInString(const char* const c_str, char before, char after);
};

