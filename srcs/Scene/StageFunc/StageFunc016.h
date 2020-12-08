/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ステージ固有の処理
 * @author SPATZ.
 * @data   2015/08/26 22:06:24
 */
//---------------------------------------------------
#ifndef __STAGE_FUNC_016_H__
#define __STAGE_FUNC_016_H__

#include <vector>
#include "StageFuncBase.h"
#include "Object/ObjectSwitch.h"

/*=====================================*
  class
 *=====================================*/
class StageFunc016 : public StageFuncBase {
public:
	/* 定数 */

	/* 関数 */
	StageFunc016();
	~StageFunc016();

	void		SetupBeforeLoad(void);
	void		SetupAfterLoad(int& rPhase);

	void		Update(int counter);

	void		SetPatternLight(int pattern);
	void		SetPatternObject(int pattern);

	void		StartGateTimer(WORD timerCnt);

private:
	void		destroy(void);

	/* 変数 */
	WORD			mGateTimerCnt;
	WORD			mGateTimerMax;

	//	ObjectSwitch*	mpGateObject;
	std::vector<ObjectSwitch *>	mListGateObject;

};

#endif // __STAGE_FUNC_016_H__

/**** end of file ****/


