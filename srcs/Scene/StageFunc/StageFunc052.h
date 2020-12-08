/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ステージ固有の処理
 * @author SPATZ.
 * @data   2015/08/26 22:06:24
 */
//---------------------------------------------------
#ifndef __STAGE_FUNC_052_H__
#define __STAGE_FUNC_052_H__

#include "Charactor/CharaGuardian.h"

#include "StageFuncBase.h"

/*=====================================*
  class
 *=====================================*/
class StageFunc052 : public StageFuncBase {
public:
	/* 定数 */

	static const BYTE kGUARDIAN_NUM = 1;

	enum {
		eBG_TYPE_GIMMICK_1F = 13,
		eBG_TYPE_GIMMICK_2F,
		eBG_TYPE_GIMMICK_3F,
		eBG_TYPE_GIMMICK_4F,
		eBG_TYPE_GIMMICK_5F,
		eBG_TYPE_GIMMICK_6F,

		eBG_TYPE_GIMMICK_BARRIER_1,
		eBG_TYPE_GIMMICK_BARRIER_2,
		eBG_TYPE_GIMMICK_BARRIER_3A,
		eBG_TYPE_GIMMICK_BARRIER_3B,
		eBG_TYPE_GIMMICK_BARRIER_3C,
		eBG_TYPE_GIMMICK_BARRIER_4A,
		eBG_TYPE_GIMMICK_LASER_1F,
		eBG_TYPE_GIMMICK_LASER_3F,
		eBG_TYPE_GIMMICK_BARRIER_4B,
		eBG_TYPE_GIMMICK_BARRIER_4C,

		//		eBG_TYPE_GIMMICK_MAX,
	};

	/* 関数 */
	StageFunc052();
	~StageFunc052();

	void		SetupBeforeLoad(void);
	void		SetupAfterLoad(int& rPhase);
	void		Update(int counter);
	void		SetPatternLight(int pattern);
	void		SetPatternObject(int pattern);

	void		NotifyCreateEnemy(int enemyIndex);

	void		ProcessShowMapBefore(void);
	void		ProcessShowMapAfter(void);

	void		RevertGuardian(void);

	void		ReviseAllModel(VECTOR& rMoveVec);

	CharaGuardian* GetGuardianPtr(void) { return mpGuardian; }

private:

	void		destroy(void);

	/* 変数 */
public:

	CharaGuardian* mpGuardian;
	

};




#endif // __STAGE_FUNC_052_H__

/**** end of file ****/


