/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * counter.cpp
 * パス解決クラス
 * @author SPATZ.
 * @data   2011/04/02 16:42:45
 */
//---------------------------------------------------
/*=====================================*
 * include
 *=====================================*/
#include <windows.h>
#include <stdio.h>

#include "Basic.h"
#include "Counter.h"

#ifdef _DEBUG

/*=====================================*
 * static
 *=====================================*/
/*static*/ Counter::tCountWork Counter::mCountWork[Counter::eCOUNT_TYPE_MAX];
/*static*/ int Counter::mPeriod = 0;

/*=====================================*
 * class
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
Counter::Counter()
{
	memset(mCountWork, 0x00, sizeof(mCountWork));
	mPeriod = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
Counter::UpdateCounter(void)
{
	mPeriod ++;
	if (mPeriod > 60) {
		mPeriod = 0;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
Counter::StartCounter(eCountType type)
{
	tCountWork* p_work = &mCountWork[type];

	p_work->mValue		= GetNowHiPerformanceCount();
	p_work->mIsStart	= true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
Counter::StopCounter(eCountType type)
{
	tCountWork* p_work = &mCountWork[type];

	LONGLONG now_vale = GetNowHiPerformanceCount();
	p_work->mValue		= now_vale - p_work->mValue;
	p_work->mIsStart	= false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
LONGLONG
Counter::GetCountNanoSec(eCountType type)
{
	tCountWork* p_work = &mCountWork[type];

	if (p_work->mIsStart == false) {
		return p_work->mValue;
	}
	return -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
float
Counter::GetCountSec(eCountType type)
{
	LONGLONG sec = GetCountNanoSec(type);

	return (float)sec / (1024.0f * 1024.0f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
Counter::Start(void)
{
	StartCounter(eCOUNT_TYPE_LOAD_EACH);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
Counter::Continue(char* label)
{
	StopCounter(eCOUNT_TYPE_LOAD_EACH);
	if (mPeriod == 0) {
		PRINT_CONSOLE("%s=%lld\t", label, GetCountNanoSec(eCOUNT_TYPE_LOAD_EACH));
	}
	StartCounter(eCOUNT_TYPE_LOAD_EACH);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
Counter::Stop(void)
{
	StopCounter(eCOUNT_TYPE_LOAD_EACH);
}

#endif // _DEBUG

/**** end of file ****/


