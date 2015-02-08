#include "stdafx.h"
#include "CWin32Timer.h"
#include <mmsystem.h>

namespace gf
{
	CWin32Timer::CWin32Timer()
		: mSecondsPerCount(0.0)
		//, mDeltaTime(-1.0f)
		, mBaseTime(0)
		, mPausedTime(0)
		, mPrevTime(0)
		, mCurrTime(0)
		, mStopped(false)
	{
		__int64 countsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
		mSecondsPerCount = 1.0 / (double)countsPerSec;
	}


	void CWin32Timer::reset()
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mBaseTime = currTime;
		mPrevTime = currTime;
		mStopTime = 0;
		mStopped = false;
	}

	// Returns the total time elapsed since Reset() was called, NOT counting any
	// time when the clock is stopped.
	f32 CWin32Timer::getElapseTime() const
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
			return (float)(((mStopTime - mPausedTime) - mBaseTime)*mSecondsPerCount);
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
			return (float)(((mCurrTime - mPausedTime) - mBaseTime)*mSecondsPerCount);
		}
	}

	f32 CWin32Timer::tick()
	{
		if (mStopped)
		{
			return 0;
		}

		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		mCurrTime = currTime;

		// Time difference between this frame and the previous.
		double deltaTime = (mCurrTime - mPrevTime)*mSecondsPerCount;

		// Prepare for next frame.
		mPrevTime = mCurrTime;

		// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
		// processor goes into a power save mode or we get shuffled to another
		// processor, then mDeltaTime can be negative.
		if (deltaTime < 0.0)
		{
			deltaTime = 0.0;
		}

		return deltaTime;
	}

	void CWin32Timer::start()
	{
		__int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);


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
			__int64 currTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

			mStopTime = currTime;
			mStopped = true;
		}
	}

	f32 CWin32Timer::getTime() const
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		return static_cast<f32>(currTime * mSecondsPerCount);
	}

}

