#ifndef __THREAD_POOL_WORKER_CLASS_H__
#define __THREAD_POOL_WORKER_CLASS_H__

#include <thread>
#include <condition_variable>

namespace gf
{
	struct SJobWrapper;

	class CThreadPool;

	class CThreadPoolWorker
	{
	public:
		CThreadPoolWorker() 
			:mStopped(false)
			, mHaveJob(false)
		{

		}

		//~CThreadPoolWorker();

		void start(CThreadPool* pool);
		void join();
		void run();

		void setJob(SJobWrapper& job);

		// void setTask(IAsyncTask* task);
		// CThreadPoolWorker(const CThreadPoolWorker&){}
	
	private:
		
		CThreadPoolWorker(const CThreadPoolWorker&);
		void operator=(const CThreadPoolWorker&);
		CThreadPool*				mThreadPool;
		std::thread					mThread;
		std::condition_variable		m_have_task_cond;
		std::mutex					m_mutex;

		//IAsyncTask*					mTask;
		SJobWrapper					mCurrentJob;
		bool						mHaveJob;
		//SJobWrapper*				mCurrentJobPtr;
		bool						mStopped;
	};

}

#endif
