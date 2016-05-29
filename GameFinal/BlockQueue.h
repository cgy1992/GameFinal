#ifndef __BLOCK_QUEUE_CLASS_H__
#define __BLOCK_QUEUE_CLASS_H__

#include <deque>
#include <thread>
#include <mutex>
#include <functional>

namespace gf
{
	template<class T>
	class BlockQueue
	{
	public:
		bool empty() const {
			return size() == 0;
		}

		size_t size() const {
			std::lock_guard<std::mutex> guard(m_mutex);
			return m_deque.size();
		}

		void push(T& x) {
			std::lock_guard<std::mutex> guard(m_mutex);
			m_deque.push_back(x);
		}

		void each(std::function<void(T&)> func) {
			std::lock_guard<std::mutex> guard(m_mutex);
			for (auto it = m_deque.begin(); it != m_deque.end(); it++) {
				func(*it);
			}
		}

		T pop() {
			std::lock_guard<std::mutex> guard(m_mutex);
			T x = m_deque.front();
			m_deque.pop_front();
			return x;
		}

	private:
		std::deque<T>				m_deque;
		mutable std::mutex			m_mutex;
	};

}


#endif
