#ifndef __CWIN32TIMER_H_INCLUDE__
#define __CWIN32TIMER_H_INCLUDE__

#include "ITimer.h"

namespace gf
{
	class CWin32Timer : public ITimer
	{
	public:
		CWin32Timer();

		virtual f32 tick();
		virtual f32 getElapseTime() const;

		virtual void start(); // Call when unpaused.
		virtual void stop();  // Call when paused.
		virtual void reset();

		// get the total time from the computer starting.
		virtual f32 getTime() const;
	
	private:

		double mSecondsPerCount;
	//	double mDeltaTime;

		__int64 mBaseTime;
		__int64 mPausedTime;
		__int64 mStopTime;
		__int64 mPrevTime;
		__int64 mCurrTime;

		bool mStopped;
	};

}

#endif
