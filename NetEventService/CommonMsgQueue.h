#pragma once
#include <list>
#include <mutex>
#include <condition_variable>

template<typename T>
class CommonMsgQueue
{
private:
	bool _isFull() const
	{
		return m_queue.size() == m_maxSize;
	}

	bool _isEmpty() const
	{
		return m_queue.empty();
	}

public:
	CommonMsgQueue(int maxSize) : m_maxSize(maxSize)
	{
	}

	void put(const T& x)
	{
		std::lock_guard<std::mutex> locker(m_mutex);

		while (_isFull())
		{
			m_notFull.wait(m_mutex);
		}
		m_queue.push_back(x);
		m_notEmpty.notify_one();
	}

	void force_put(const T& x)
	{
		std::lock_guard<std::mutex> locker(m_mutex);

		while (_isFull())
		{
			m_queue.pop_front();
		}
		m_queue.push_back(x);
		m_notEmpty.notify_one();
	}

	//用于线程里调用，可以立即返回，不阻塞线程
	void get(T& x, bool &bIsEmpty)
	{
		std::lock_guard<std::mutex> locker(m_mutex);

		if (_isEmpty())
		{
			bIsEmpty = true;
			return;
		}

		bIsEmpty = false;
		x = m_queue.front();
		m_queue.pop_front();
		m_notFull.notify_one();
	}


	void get(T& x)
	{
		std::lock_guard<std::mutex> locker(m_mutex);

		while (_isEmpty())
		{
			m_notEmpty.wait(m_mutex);
		}

		x = m_queue.front();
		m_queue.pop_front();
		m_notFull.notify_one();
	}


	bool isFull() const
	{
		return _isFull();
	}


private:
	std::list<T> m_queue;
	std::mutex m_mutex;
	std::condition_variable_any m_notEmpty;
	std::condition_variable_any m_notFull;
	int m_maxSize;
};