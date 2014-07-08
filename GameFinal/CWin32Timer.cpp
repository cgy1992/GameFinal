#include "stdafx.h"
#include "CWin32Timer.h"
#include <mmsystem.h>

namespace gf
{
	CWin32Timer::CWin32Timer()
		: mBaseTime(0)
		, mPausedTime(0)
		, mPrevTime(0)
		, mCurrTime(0)
		, mStopped(false)
	{

	}


	void CWin32Timer::reset()
	{
		mCurrTime = timeGetTime();

		mBaseTime = mCurrTime;
		mPrevTime = mCurrTime;
		mStopTime = 0;
		mStopped = false;
	}

	// Returns the total time elapsed since Reset() was called, NOT counting any
	// time when the clock is stopped.
	u32 CWin32Timer::getElapseTime() const
	{
		// If we are stopped, do not count the time that has passed since we stopped.
		// Moreover, if we previously already had a pause, the distance 
		// mStopTime - mBaseTime includes paused time, which we do not want to count.
		// To correct this, we can subtract the paused time from mStopTime:  
		//
		//                     |<--paused time-->|
		// ----*---------------*-----------------*------------*------------*------> time
		//  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime

		if (mStopped)
		{
			return (mStopTime - mPausedTime) - mBaseTime;
		}

		// The distance mCurrTime - mBaseTime includes paused time,
		// which we do not want to count.  To correct this, we can subtract 
		// the paused time from mCurrTime:  
		//
		//  (mCurrTime - mPausedTime) - mBaseTime 
		//
		//                     |<--paused time-->|
		// ----*---------------*-----------------*------------*------> time
		//  mBaseTime       mStopTime        startTime     mCurrTime

		else
		{
			return (mCurrTime - mPausedTime) - mBaseTime;
		}
	}

	u32 CWin32Timer::tick()
	{
		if (mStopped)
		{
			return 0;
		}

		mCurrTime = timeGetTime();
		// Time difference between this frame and the previous.
		u32 deltaTime = mCurrTime - mPrevTime;

		// Prepare for next frame.
		mPrevTime = mCurrTime;
		// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
		// processor goes into a power save mode or we get shuffled to another
		// processor, then mDeltaTime can be negative.
		if (deltaTime < 0)
		{
			deltaTime = 0;
		}

		return deltaTime;
	}

	void CWin32Timer::start()
	{

		u32 startTime = timeGetTime();
		// Accumulate the time elapsed between stop and start pairs.
		//
		//                     |<-------d------->|
		// ----*---------------*-----------------*------------> time
		//  mBaseTime       mStopTime        startTime     

		if (mStopped)
		{
			mPausedTime += (startTime - mStopTime);

			mPrevTime = startTime;
			mStopTime = 0;
			mStopped = false;
		}
	}

	void CWin32Timer::stop()
	{
		if (!mStopped)
		{
			u32 currTime = timeGetTime();

			mStopTime = currTime;
			mStopped = true;
		}
	}

}

