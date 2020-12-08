/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ステージ固有の処理
 * @author SPATZ.
 * @data   2015/08/26 22:06:24
 */
//---------------------------------------------------
#ifndef __STAGE_FUNC_043_H__
#define __STAGE_FUNC_043_H__

#include "StageFuncBase.h"

/*=====================================*
  class
 *=====================================*/
class StageFunc043 : public StageFuncBase {
public:
	/* 定数 */

	enum {
		eSTAGE_STATE_SHOW_WALL = 0,		// 壁表示
		eSTAGE_STATE_FADEOUT_WALL,		// 壁フェードアウト
		eSTAGE_STATE_HIDE_WALL,			// 壁非表示
		eSTAGE_STATE_FADEIN_WALL,		// 壁フェードイン

		eSTAGE_STATE_MAX,
	};


	/* 関数 */
	StageFunc043();
	~StageFunc043();

	void		SetupBeforeLoad(void);
	void		SetupAfterLoad(int& rPhase);
	void		Update(int counter);
	void		SetPatternLight(int pattern);
	void		SetPatternObject(int pattern);

	void		StartFadeInWall(void);
	void		StartFadeOutWall(void);

private:

	void		destroy(void);
	void		updateStageState(void);


	/* 変数 */
public:
	int		mEffHndCrystal;
	int		mEffHndAura;
	float	mTransH;
	float	mTransV;

	BYTE	mStageState;
	float	mFadeWallAlpha;


};

#endif // __STAGE_FUNC_043_H__

/**** end of file ****/


