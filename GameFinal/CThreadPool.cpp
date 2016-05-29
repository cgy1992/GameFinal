#include "stdafx.h"
#include "CThreadPool.h"
#include "CThreadPoolWorker.h"
#include "CJobSet.h"

namespace gf
{
	static void thread_proc(CThreadPool* pool) {
		pool->run();
	}

	CThreadPool::CThreadPool(u32 maxThreadNum)
		:IThreadPool(maxThreadNum)
	{
		
	}

	CThreadPool::~CThreadPool()
	{

	}

	void CThreadPool::start()
	{
		mStopped = false;
		mMainThread = std::thread(thread_proc, this);
	}

	void CThreadPool::stop()
	{
		if (!mStopped) {
			mStopped = true;
			// m_have_idle_worker_cond.notify_all();
			mMainThread.join();

			for (u32 i = 0; i < mWorkers.size(); i++) {
				CThreadPoolWorker* worker = mWorkers[i];
				worker->join();
				delete worker;
			}
		}
	}

	void CThreadPool::run()
	{
		while (!mStopped || mJobs.size() > 0) {
			std::unique_lock<std::mutex> lk(m_mutex);
			//m_have_idle_worker_cond.wait(lk, [this](){
			//	return mStopped;
			//});
			// while (mIdleWorkers.size() == 0 || mStopped)

			m_have_idle_worker_cond.wait(lk, [this](){
				return mIdleWorkers.size() > 0;
			});

			if (!mJobs.empty()) {
				SJobWrapper job = mJobs.pop();
				CThreadPoolWorker* worker = mIdleWorkers.pop();
				worker->setJob(job);
			}
		}
	}

	void CThreadPool::addJob(SJobWrapper& job)
	{
		std::lock_guard<std::mutex>	lk(m_mutex);
		if (!mIdleWorkers.empty()) {
			CThreadPoolWorker* worker = mIdleWorkers.pop();
			worker->setJob(job);
		}
		else if (mWorkers.size() < mMaxThreadNum) {
			mWorkers.push_back(new CThreadPoolWorker());
			CThreadPoolWorker* worker = mWorkers[mWorkers.size() - 1];
			worker->start(this);
			worker->setJob(job);
		}
		else {
			// add the task to the task queue
			mJobs.push(job);
		}
	}

	void CThreadPool::addJob(std::function<void()> func)
	{
		if (mStopped)
			return;

		SJobWrapper job;
		job.Procedure = func;

		this->addJob(job);
	}

	void CThreadPool::addJobs(SJobFunctionSet& jobs, std::function<void()> cb)
	{
		if (mStopped)
			return;

		CJobSet* pJobSet = new CJobSet(jobs.size(), cb);
		for (auto it = jobs.begin(); it != jobs.end(); it++) {
			SJobWrapper job;
			job.Procedure = *it;
			job.JobSetPtr = pJobSet;

			this->addJob(job);
		}
	}

	/*
	void CThreadPool::addTask(IAsyncTask* task)
	{
		if (mStopped)
			return;

		// if have idle worker, just dispatch to it
		std::lock_guard<std::mutex>	lk(m_mutex);
		if (!mIdleWorkers.empty()) {
			CThreadPoolWorker* worker = mIdleWorkers.pop();
			worker->setTask(task);
		}
		else if (mWorkers.size() < mMaxThreadNum) {
			mWorkers.push_back(new CThreadPoolWorker());
			CThreadPoolWorker* worker = mWorkers[mWorkers.size() - 1];
			worker->start(this);
			worker->setTask(task);
		}
		else {
			// add the task to the task queue
			mTasks.push(task);
		}
	}
	*/

	void CThreadPool::moveToIdleQueue(CThreadPoolWorker* worker)
	{
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			mIdleWorkers.push(worker);
		}
		
		m_have_idle_worker_cond.notify_all();
	}

	u32 CThreadPool::getCurrentThreadNum() const
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		return mWorkers.size();
	}

	

}

