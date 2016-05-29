#include "stdafx.h"
#include "CThreadPool.h"
#include "CThreadPoolWorker.h"
#include "CJobSet.h"

namespace gf
{
	static void thread_proc(CThreadPoolWorker* worker) {
		//printf("worker thread begins");
		worker->run();
	}

	void CThreadPoolWorker::start(CThreadPool* pool)
	{
		this->mThreadPool = pool;
		mStopped = false;
		mThread = std::thread(thread_proc, this);
	}

	void CThreadPoolWorker::join()
	{
		printf("worker is stopping\n");
		mStopped = true;
		if (mThread.joinable()) {
			m_have_task_cond.notify_all();
			mThread.join();
		}
	}

	void CThreadPoolWorker::run()
	{
		while (!mStopped) {
			std::unique_lock<std::mutex> lk(m_mutex);
			m_have_task_cond.wait(lk, [this]{
				return mHaveJob || mStopped;
			});

			if (mHaveJob) {
				mCurrentJob.Procedure();

				// if the job belongs to a group.
				if (mCurrentJob.JobSetPtr) {
					CJobSet* jobSet = mCurrentJob.JobSetPtr;
					if (jobSet->mark_and_test()) {
						jobSet->callback();
						delete jobSet;
					}
				}
			}

			mHaveJob = false;
			mThreadPool->moveToIdleQueue(this);
		}
	}

	void CThreadPoolWorker::setJob(SJobWrapper& job)
	{
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			mCurrentJob = job;
			mHaveJob = true;
		}
		m_have_task_cond.notify_all();
	}

	/*
	void CThreadPoolWorker::setTask(IAsyncTask* task)
	{
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			mTask = task;
		}
		m_have_task_cond.notify_all();
	}
	*/



}
