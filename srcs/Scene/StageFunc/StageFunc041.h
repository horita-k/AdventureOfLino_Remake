/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ステージ固有の処理
 * @author SPATZ.
 * @data   2015/08/26 22:06:24
 */
//---------------------------------------------------
#ifndef __STAGE_FUNC_041_H__
#define __STAGE_FUNC_041_H__

#include "StageFuncBase.h"

#include "Charactor/CharaGuardian.h"
#include "AppLib/Graphic/EffekseerManager.h"

/*=====================================*
  class
 *=====================================*/
class StageFunc041 : public StageFuncBase {
public:
	/* 定数 */

	static const BYTE kGUARDIAN_NUM = 3;

	enum {
		eBG_TYPE_GIMMICK_2F = 13,
		eBG_TYPE_GIMMICK_3F,
		eBG_TYPE_GIMMICK_4F,
		eBG_TYPE_GIMMICK_5F,
		eBG_TYPE_GIMMICK_BIG_KEY,
	};


	/* 関数 */
	StageFunc041();
	~StageFunc041();

	void		SetupBeforeLoad(void);
	void		SetupAfterLoad(int& rPhase);
	void		Update(int counter);
	void		SetPatternLight(int pattern);
	void		SetPatternObject(int pattern){}

	void		ProcessShowMapBefore(void);
	void		ProcessShowMapAfter(void);

	void		RevertGuardian(void);

	void		ReviseAllModel(VECTOR& rMoveVec);

private:

	void		destroy(void);

	/* 変数 */
public:
	float	mLightCounter;					// ライトの点滅カウンタ
	BOOL	mIsOnAllSwitch;					// 全てのスイッチを押したか？

	CharaGuardian* mpGuardian;

	EFK_HANDLE	mEfkLightPillar;
	VECTOR		mLightPillarPos;


};

#endif // __STAGE_FUNC_041_H__

/**** end of file ****/


