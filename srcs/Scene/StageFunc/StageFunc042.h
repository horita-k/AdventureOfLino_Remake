/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ステージ固有の処理
 * @author SPATZ.
 * @data   2015/08/26 22:06:24
 */
//---------------------------------------------------
#ifndef __STAGE_FUNC_042_H__
#define __STAGE_FUNC_042_H__

#include "StageFuncBase.h"

/*=====================================*
  class
 *=====================================*/
class StageFunc042 : public StageFuncBase {
public:
	/* 定数 */

	/* 関数 */
	StageFunc042();
	~StageFunc042();

	void		SetupBeforeLoad(void);
	void		SetupAfterLoad(int& rPhase);
	void		Update(int counter);
	void		SetPatternLight(int pattern){}
	void		SetPatternObject(int pattern){}

private:

	void		destroy(void);

	/* 変数 */
public:
	int		mEffHndCrystal;
	int		mEffHndAura;
};

#endif // __STAGE_FUNC_042_H__

/**** end of file ****/


