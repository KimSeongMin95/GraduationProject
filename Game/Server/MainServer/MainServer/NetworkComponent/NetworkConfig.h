#pragma once


// ��Ʈ��ũ ȯ�� ����
#define	MAX_BUFFER 4096 // �ۼ��� ���� ũ��
#define MAX_HEADER 256  // ��� ���� �ִ밳��
#define SPIN_COUNT 2048	// CriticalSection�� ���ɶ� ī��Ʈ

// ���� ȯ�� ����
#define BUILD_CONFIG_DEBUG 1
#define BUILD_CONFIG_DEVELOPMENT 0
#define BUILD_CONFIG_SHIPPING 0
#define BUILD_CONFIG_EDITOR 0
#define BUILD_CONFIG_TEST 0

#ifndef BUILD_CONFIG_DEBUG
#define BUILD_CONFIG_DEBUG 0
#endif
#ifndef BUILD_CONFIG_DEVELOPMENT
#define BUILD_CONFIG_DEVELOPMENT 0
#endif
#ifndef BUILD_CONFIG_SHIPPING
#define BUILD_CONFIG_SHIPPING 0
#endif
#ifndef BUILD_CONFIG_EDITOR
#define BUILD_CONFIG_EDITOR 0
#endif
#ifndef BUILD_CONFIG_TEST
#define BUILD_CONFIG_TEST 0
#endif