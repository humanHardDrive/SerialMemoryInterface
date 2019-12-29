#pragma once

#include <queue>
#include <mutex>

template<class T>
class LockingQ
{
public:
	void push(T& v);

	T& front();
	void pop();
	void clear();

	size_t size();
	bool empty();

private:
	std::queue<T> m_Q;
	std::mutex m_Lock;
};

template<class T>
inline void LockingQ<T>::push(T & v)
{
	std::lock_guard<std::mutex> lock;
	m_Q.push(v);
}

template<class T>
inline T & LockingQ<T>::front()
{
	std::lock_guard<std::mutex> lock;
	return m_Q.front();
}

template<class T>
inline void LockingQ<T>::pop()
{
	std::lock_guard<std::mutex> lock;
	m_Q.pop();
}

template<class T>
inline void LockingQ<T>::clear()
{
	std::lock_guard<std::mutex> lock;
	m_Q = {};
}

template<class T>
inline size_t LockingQ<T>::size()
{
	std::lock_guard<std::mutex> lock;
	return m_Q.size();
}

template<class T>
inline bool LockingQ<T>::empty()
{
	std::lock_guard<std::mutex> lock;
	return m_Q.empty();
}
