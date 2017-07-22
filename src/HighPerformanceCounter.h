

#pragma  once

#ifndef __HIGH_PERFORMANCE_COUNTER_H_
#define __HIGH_PERFORMANCE_COUNTER_H_

#include <BaseTsd.h>
#include <Windows.h>

enum eTimerTypes {RDTSC_TIMER, QUERYPERF_TIMER};

class CHPCounter {
public:

	CHPCounter();
	~CHPCounter();

	// returns 0 if the counter query fails
    __int64 GetFrequency();

	// returns previously set affinity mask; 0 if none was set.
	DWORD_PTR	SetCounterThreadAffinityMask( DWORD_PTR dwpThreadAffinityMask );

	// If called without setting the affinity mask, it will default to current thread
	void   StartCounter() ;

	//  record the current count and return the interval count since the counter was started.
	// For explicit counter value call GetLastCount()
	double StopCounter();

	//  return the last time lapse recorded, in seconds; call "StopCounter" first!
	double GetLastTimeInterval() const;

	//	return the last counter value recorded - NOT the interval, aboslute number 
	LONGLONG	GetCounterValueLastRead() const;

    //  Set the kind of timer you want to use (DEFAULT is QueryPerformanceCounter)
    bool SetCounterType(eTimerTypes eval);

private:

    __int64         i64StartValue;
    __int64         i64LastReadValue;
    __int64         iFreq;
	LARGE_INTEGER   ilStartValue;
	LARGE_INTEGER   iLastReadValue;
	double          dFrequency;
	DWORD_PTR	    dwpTAMask;
    eTimerTypes     eTimerType;

};



#endif		// #ifndef   __HIGH_PERFORMANCE_COUNTER_H_
