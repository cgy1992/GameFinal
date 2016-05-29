#ifndef __THREAD_POOL_INTERFACE_H__
#define __THREAD_POOL_INTERFACE_H__

#include <thread>
#include <functional>
#include <set>
#include "gfTypes.h"
#include "IAsyncTask.h"

namespace gf
{
	//struct SBlock

	typedef std::vector<std::function<void()> > SJobFunctionSet;

	class IThreadPool
	{
	public:
		IThreadPool(u32 maxThreadNum) 
			:mMaxThreadNum(maxThreadNum) 
			, mStopped(false)
		{

		}

		virtual void start() = 0;

		virtual void run() = 0;

		virtual void stop() = 0;

		//virtual void addTask(IAsyncTask* task) = 0;

		virtual void addJob(std::function<void()> func) = 0;

		virtual void addJobs(SJobFunctionSet& jobs, std::function<void()> cb) = 0;

		//virtual void addJobs(std::vector<SJob>& jobs, std::function<void()> cb) = 0;

		virtual u32 getMaxThreadNum() const { return mMaxThreadNum; }

		virtual u32 getCurrentThreadNum() const = 0;

		virtual bool isStopped() const { return mStopped; }

	protected:
		bool		mStopped;
		u32			mMaxThreadNum;

	};
}


#endif

