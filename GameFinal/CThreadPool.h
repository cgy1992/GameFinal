#ifndef __THREAD_POOL_CLASS_H__
#define __THREAD_POOL_CLASS_H__

#include "IThreadPool.h"
#include "BlockQueue.h"
#include <vector>
#include <atomic>
#include <condition_variable>

namespace gf
{
	class CThreadPoolWorker;
	class CJobSet;

	struct SJobWrapper
	{
		std::function<void()>		Procedure;
		CJobSet*					JobSetPtr;
		SJobWrapper()
			:Procedure(nullptr)
			, JobSetPtr(nullptr)
		{

		}
	};

	class CThreadPool : public IThreadPool
	{
	public:
		CThreadPool(u32 maxThreadNum);
		~CThreadPool();

		virtual void start();

		virtual void stop();

		virtual void run();

		//virtual void addTask(IAsyncTask* task);

		virtual void addJob(std::function<void()> func);

		virtual void addJobs(SJobFunctionSet& jobs, std::function<void()> cb);

		virtual u32 getCurrentThreadNum() const;

		void moveToIdleQueue(CThreadPoolWorker* worker);

	private:

		void addJob(SJobWrapper& job);

		std::thread							mMainThread;
		std::vector<CThreadPoolWorker*>		mWorkers;
		BlockQueue<CThreadPoolWorker*>		mIdleWorkers;
		//BlockQueue<IAsyncTask*>				mTasks;
		//BlockQueue<SJob>					mJobs;
		BlockQueue<SJobWrapper>				mJobs;

		mutable std::mutex					m_mutex;
		std::condition_variable				m_have_idle_worker_cond;
	};
}


#endif

