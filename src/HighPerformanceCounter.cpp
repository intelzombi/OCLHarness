


#include "HighPerformanceCounter.h"

#pragma warning( disable : 4995 )
#include <intrin.h>
#pragma warning( default : 4995 )

#define WINCPUID_STATIC
//#include <WinCPUID.h>

CHPCounter::CHPCounter() : dwpTAMask(0)
{
    eTimerType = QUERYPERF_TIMER;

    //rdtsc init
    //CPUINFO_T WinCPUID_Info = CPUINFO_T_INIT_VAL;
    //WinCPUID_Init(1000, &WinCPUID_Info);
    //iFreq = WinCPUID_Info.llCPUFreq;
    iFreq *= 1000;
    i64StartValue = 0;
    i64LastReadValue = 0;

    //query performance counter init
	ilStartValue.u.HighPart = 0;
	ilStartValue.u.LowPart  = 0;
	iLastReadValue.u.LowPart   = 0;
	iLastReadValue.u.HighPart  = 0;
    LARGE_INTEGER ilProcFrequency = {0, 0};
    QueryPerformanceFrequency(&ilProcFrequency);
    dFrequency  = double(ilProcFrequency.QuadPart);

    //set affinity one time on startup
    DWORD_PTR oldmask = ::SetThreadAffinityMask( ::GetCurrentThread(), 0 );

}

CHPCounter::~CHPCounter(){}


__int64	CHPCounter::GetFrequency()
{

    __int64 iRetval=0;

    if (eTimerType == RDTSC_TIMER)
    {
        //CPUINFO_T WinCPUID_Info = CPUINFO_T_INIT_VAL;
        //WinCPUID_Init(1000, &WinCPUID_Info);
        //iRetval = WinCPUID_Info.llCPUFreq;
        iRetval *= 2500*1000;
    }
    else
    {
        LARGE_INTEGER	proc_freq;
        if (!QueryPerformanceFrequency(&proc_freq)) 
        {
            iRetval = 0;
        }
        else
        {
            iRetval = proc_freq.QuadPart;
        }
    }

    return iRetval;
}

DWORD_PTR	CHPCounter::SetCounterThreadAffinityMask( DWORD_PTR dwpThreadAffinityMask )
{
	DWORD_PTR   oldMask = dwpTAMask;

	dwpTAMask   = dwpThreadAffinityMask;

	return  oldMask;
}

void CHPCounter::StartCounter(void)
{
    if (eTimerType == RDTSC_TIMER)
    {
        i64StartValue = __rdtsc();
    }
    else
    {
        ::QueryPerformanceCounter(&ilStartValue);
    }
}

double CHPCounter::StopCounter()
{
    if (eTimerType == RDTSC_TIMER)
    {
        i64LastReadValue = __rdtsc();
		
        return		static_cast<double>( (i64LastReadValue - i64StartValue) );
    }
    else
    {
        ::QueryPerformanceCounter(&iLastReadValue);
        return		static_cast<double>( (iLastReadValue.QuadPart - ilStartValue.QuadPart) );
    }
} 

double CHPCounter::GetLastTimeInterval() const
{
    if (eTimerType == RDTSC_TIMER)
    {
        return (( i64LastReadValue - i64StartValue) / (double)iFreq);
    }
    else
    {
        return (( iLastReadValue.QuadPart - ilStartValue.QuadPart) / dFrequency);
    }
}

LONGLONG	CHPCounter::GetCounterValueLastRead() const
{
    if (eTimerType == RDTSC_TIMER)
    {
        return  i64LastReadValue;
    }
    else
    {
	    return  iLastReadValue.QuadPart;
    }
}

bool CHPCounter::SetCounterType( eTimerTypes eval )
{
    bool bRetval = false;

    if ( (eval == RDTSC_TIMER) || (eval == QUERYPERF_TIMER) )
    {
        eTimerType = eval;
        bRetval = true;
    }
    return bRetval;
}