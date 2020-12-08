/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ライブラリの色々融合したテスト
 * @author SPATZ.
 * @data   2011/11/21 00:55:18
 */
//---------------------------------------------------
#ifndef __SCENE_TITLE_H__
#define __SCENE_TITLE_H__

#include "AppLib/Graphic/ModelBase.h"
#include "SceneBase.h"
#include "SceneInfo.h"
#include "AppLib/Graphic/LayoutBase.h"
#include "AppLib/Graphic/ModelMap.h"

/*=====================================*
  class
 *=====================================*/
class SceneTitle : public SceneBase {
public:
	/* 定数 */
	enum {
		ePHASE_LOAD_BEFORE,
		ePHASE_LOADING,

		ePHASE_LOG_SETUP,
		ePHASE_LOG_BEFORE,
		ePHASE_LOG,

		ePHASE_SETUP,
		ePHASE_TITLE_BEFORE,
		ePHASE_TITLE,

		ePHASE_MAX,
	};

	/* 関数 */
	SceneTitle();
	~SceneTitle();

	/*=====================================*
	 * useful method
	 *=====================================*/
	void		Create(int debugArg);
	void		Update(void);

	int			GetLoadingPercent(void);

private:

	void		destroy(void);

	/*=====================================*
	 * phase
	 *=====================================*/
	typedef void (SceneTitle::*PHASE_FUNC)(void);
	static PHASE_FUNC mPhaseFunc[ePHASE_MAX];

	void		phaseLoadBefore(void);
	void		phaseLoading(void);
	void		phaseLogoSetup(void);
	void		phaseLogoBefore(void);
	void		phaseLogo(void);
	void		phaseSetup(void);
	void		phaseTitleBefore(void);
	void		phaseTitle(void);
	
	/* 変数 */
	BOOL		mIsNext;

	LayoutBase	mTitleLog;

	LayoutBase	mLayoutPressAnyKey;
	LayoutBase	mLayoutSpatz;

	VECTOR		mLayoutBoardPos;
	float		mBoardMoveY;
	int			mEfsParticle;

	BOOL		mIsExistSaveData;

};

#endif // __SCENE_TITLE_H__

/**** end of file ****/


