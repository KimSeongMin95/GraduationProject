#pragma once

#include "NetworkHeader.h"
#include "NetworkConfig.h"

template <typename T>
class CThreadSafetyQueue sealed
{
public:
	CThreadSafetyQueue();
	~CThreadSafetyQueue();

private:
	std::queue<T> q;
	CRITICAL_SECTION cs;

public:
	bool empty();

	void push(T element);

	T front();

	void pop();

	T front_pop();

	T back();

	void clear();

	uint16_t size();

	std::queue<T> copy();

	std::queue<T> copy_clear();
};

template <typename T>
CThreadSafetyQueue<T>::CThreadSafetyQueue()
{
	while (InitializeCriticalSectionAndSpinCount(&cs, SPIN_COUNT) == false);
}
template <typename T>
CThreadSafetyQueue<T>::~CThreadSafetyQueue()
{
	DeleteCriticalSection(&cs);
}

template <typename T>
bool CThreadSafetyQueue<T>::empty()
{
	EnterCriticalSection(&cs);
	bool result = q.empty();
	LeaveCriticalSection(&cs);
	return result;
}

template <typename T>
void CThreadSafetyQueue<T>::push(T element)
{
	EnterCriticalSection(&cs);
	q.push(element);
	LeaveCriticalSection(&cs);
}

template <typename T>
T CThreadSafetyQueue<T>::front()
{
	EnterCriticalSection(&cs);
	T result = q.front();
	LeaveCriticalSection(&cs);
	return result;
}

template <typename T>
void CThreadSafetyQueue<T>::pop()
{
	EnterCriticalSection(&cs);
	q.pop();
	LeaveCriticalSection(&cs);
}

template <typename T>
T CThreadSafetyQueue<T>::front_pop()
{
	EnterCriticalSection(&cs);
	T result = q.front();
	q.pop();
	LeaveCriticalSection(&cs);
	return result;
}

template <typename T>
T CThreadSafetyQueue<T>::back()
{
	EnterCriticalSection(&cs);
	T result = q.back();
	LeaveCriticalSection(&cs);
	return result;
}

template <typename T>
void CThreadSafetyQueue<T>::clear()
{
	EnterCriticalSection(&cs);
	while (q.empty() == false)
		q.pop();
	LeaveCriticalSection(&cs);
}

template <typename T>
uint16_t CThreadSafetyQueue<T>::size()
{
	EnterCriticalSection(&cs);
	uint16_t result = q.size();
	LeaveCriticalSection(&cs);
	return result;
}

template <typename T>
std::queue<T> CThreadSafetyQueue<T>::copy()
{
	EnterCriticalSection(&cs);
	std::queue<T> copiedQ = q;
	LeaveCriticalSection(&cs);
	return copiedQ;
}

template <typename T>
std::queue<T> CThreadSafetyQueue<T>::copy_clear()
{
	EnterCriticalSection(&cs);
	std::queue<T> copiedQ = q;
	while (q.empty() == false)
		q.pop();
	LeaveCriticalSection(&cs);
	return copiedQ;
}