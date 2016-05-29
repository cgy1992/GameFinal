#ifndef __JOB_SET_CLASS_H__
#define __JOB_SET_CLASS_H__

#include "IThreadPool.h"
#include <vector>
#include <atomic>

namespace gf
{
	class CJobSet
	{
	public:
		CJobSet(int jobNum, std::function<void()> cb)
			: mJobNum(jobNum)
			, mUnfinishedJobNumPtr(new std::atomic<int>(jobNum))
			, mCallback(cb)
		{

		}

		bool mark_and_test() {
			int val = std::atomic_fetch_sub(mUnfinishedJobNumPtr, 1);
			return val == 1;
		}

		void callback() {
			mCallback();
		}

	private:
		int										mJobNum;
		std::atomic<int>*						mUnfinishedJobNumPtr;
		std::function<void()>					mCallback;
	};
}

#endif
