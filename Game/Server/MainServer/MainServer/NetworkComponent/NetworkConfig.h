#pragma once


// ��Ʈ��ũ ȯ�� ����
#define	MAX_BUFFER 4096 // �ۼ��� ���� ũ��
#define MAX_HEADER 256  // ��� ���� �ִ밳��
#define SPIN_COUNT 2048	// CriticalSection�� ���ɶ� ī��Ʈ

// ���� ȯ�� ����
#define BUILD_CONFIG_DEBUG 1
//#define BUILD_CONFIG_RELEASE 1

#ifndef BUILD_CONFIG_DEBUG
#define BUILD_CONFIG_DEBUG 0
#endif
#ifndef BUILD_CONFIG_RELEASE
#define BUILD_CONFIG_RELEASE 0
#endif