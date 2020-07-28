#pragma once


// 네트워크 환경 설정
#define	MAX_BUFFER 4096 // 송수신 버퍼 크기
#define MAX_HEADER 256  // 헤더 종류 최대개수
#define SPIN_COUNT 2048	// CriticalSection의 스핀락 카운트

// 빌드 환경 설정
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