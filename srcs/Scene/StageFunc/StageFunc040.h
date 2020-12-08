/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ステージ固有の処理
 * @author SPATZ.
 * @data   2015/08/26 22:06:24
 */
//---------------------------------------------------
#ifndef __STAGE_FUNC_040_H__
#define __STAGE_FUNC_040_H__

#include "StageFuncBase.h"

/*=====================================*
  class
 *=====================================*/
class StageFunc040 : public StageFuncBase {
public:
	/* 定数 */

	/* 関数 */
	StageFunc040();
	~StageFunc040();

	void		SetupBeforeLoad(void);
	void		SetupAfterLoad(int& rPhase);
	void		Update(int counter);
	void		SetPatternLight(int pattern);
	void		SetPatternObject(int pattern){}

	void		SetTornadeMorphNumDefault(void);
	void		SetTornadeMorphNumEvent(void);

private:

	void		destroy(void);

	/* 変数 */
public:
	int		mEffHndTornade;					// 
	//	int		mEffHndSky;
	int		mEffHndCrystal;
	int		mEffHndAura;
	
	DWORD	mDistPlayerToTornadeRate;
	float	mTornadeShapeRate;
	bool	mIsChangeShapeTornade;			// 竜巻の形状変化フラグ
	SHORT	mTornadeBattleCount;			// 竜巻戦のカウンタ
	
	/*
	BYTE	mSlapCntPlayer;					// ビンタ回数
	BYTE	mSlapCntEnemy;					// ビンタ回数
	CHAR	mRafiaTalkIndex;				// ラフィアトーク番号
	*/
};

#endif // __STAGE_FUNC_040_H__

/**** end of file ****/


