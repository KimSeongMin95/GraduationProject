#pragma once

#include "NetworkHeaders.h"

class CPacket sealed
{
private:
	uint16_t Length;   // ��üũ��
	uint16_t Header;   // ���
	stringstream Data; // ������
	char End;		   // ��

public:
	explicit CPacket(uint16_t&& Header);

	uint16_t GetLen();
	uint16_t GetHeader();
	stringstream& GetData();

	// ��Ŷ�� ��üũ�⸦ ���մϴ�.
	void CalculateLen(const uint16_t& SizeOfData);

	// ��Ŷ�� ��üũ�⸦ ���� ��, ��üũ��� ����� string���� ��ȯ�Ͽ� ��ȯ�մϴ�.
	string GetStrOfLengthAndHeader();

	// Ư�� ���ڸ� �ٸ� ���ڷ� ��ü�մϴ�.
	static string ReplaceCharInString(const string& str, char before, char after);
	static string ReplaceCharInString(const char* const c_str, char before, char after);
};

