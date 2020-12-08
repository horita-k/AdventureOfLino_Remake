/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * Counter.h
 * カウンタ計測クラス
 * @author SPATZ.
 * @data   2011/04/02 16:35:28	
 */
//---------------------------------------------------
#ifndef __COUNTER_H__
#define __COUNTER_H__

/*=====================================*
 * include
 *=====================================*/

#ifdef _DEBUG
#define START_COUNTER()			Counter::Start()
#define CONTINUE_COUNTER(lbl)	Counter::Continue(lbl)
#define STOP_COUNTER()			Counter::Stop()
#else 
#define START_COUNTER()			// 空定義
#define CONTINUE_COUNTER(lbl)	// 空定義
#define STOP_COUNTER()			// 空定義
#endif // _DEBUG

#ifdef _DEBUG

/*=====================================*
 * class
 *=====================================*/
class Counter
{
 public:
	enum eCountType {
		eCOUNT_TYPE_LOAD_EACH,
		eCOUNT_TYPE_LOAD_ALL,
		eCOUNT_TYPE_MAIN,

		eCOUNT_TYPE_MAX,
	};

	/* 関数 */
	Counter();

	static void		UpdateCounter(void);

	static void		StartCounter(eCountType type);
	static void		StopCounter(eCountType type);
	
	static LONGLONG	GetCountNanoSec(eCountType type);
	static float	GetCountSec(eCountType type);

	static void		Start(void);
	static void		Continue(char* label);
	static void		Stop(void);

 private:
	/* 変数 */
	struct tCountWork {
		LONGLONG	mValue;
		bool		mIsStart;
	};
	static tCountWork mCountWork[eCOUNT_TYPE_MAX];
	static int		mPeriod;
};

#endif // _DEBUG

#endif // __COUNTER_H__

/**** end of file ****/


