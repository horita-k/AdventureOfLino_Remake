/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * アイテムメニュー選択
 * @author SPATZ.
 * @data   2015/07/13 23:38:22
 */
//---------------------------------------------------
#ifndef __SUB_SCENE_FLYING_MOVE_H__
#define __SUB_SCENE_FLYING_MOVE_H__

#include "AppLib/Graphic/ModelBase.h"
#include "SceneBase.h"
#include "SceneInfo.h"
#include "AppLib/Graphic/LayoutBase.h"
#include "AppLib/Graphic/ModelMap.h"
#include "AppLib/Graphic/LayoutNumber.h"
#include "AppLib/Graphic/EffekseerManager.h"

/*=====================================*
  class
 *=====================================*/
class SubSceneFlyingMove : public SubSceneBase {
public:

	static const float  kCAM_DIST;
	static const VECTOR kCAM_TAR;
	static const float  kCAM_POS_Y_1ST;
	static const float  kCAM_POS_Y_2ND;
	static const WORD  kCAM_ROT_Y_1ST;
	static const WORD  kCAM_ROT_Y_2ND;

	/* 定数 */
	enum {
		ePHASE_NONE = 0,
		ePHASE_COME_POPPO,
		ePHASE_CATCH_POPPO,
		ePHASE_WAI_FADE,
		ePHASE_TALK_POPPO,
		ePHASE_SELECT_DEST,
		ePHASE_CONFIRM_DEST,
		ePHASE_TALK_POPPO_2ND,
		ePHASE_FLY_AWAY,
		ePHASE_END,

		ePHASE_MAX,
	};

	/* 関数 */
	SubSceneFlyingMove();
	~SubSceneFlyingMove();

	void		Create(void);
	bool		Update(void);
	void		Destroy(void);
	void		ChangePhase(BYTE setPhase);

	static BOOL CheckFlyingMoveEnable(UCHAR stageIndex);

private:
	/*=====================================*
	 * phase
	 *=====================================*/
	typedef void (SubSceneFlyingMove::*PHASE_FUNC)(void);
	static PHASE_FUNC mPhaseFuncEnter[ePHASE_MAX];
	static PHASE_FUNC mPhaseFuncUpdate[ePHASE_MAX];
	static PHASE_FUNC mPhaseFuncLeave[ePHASE_MAX];

	void		phaseDummy(void);

	//	void		phasePlayOcarinaEnter(void);
	void		phaseComePoppoEnter(void);
	void		phaseCatchPoppoEnter(void);
	void		phaseWaitFadeEnter(void);
	void		phaseTalkPoppoEnter(void);
	void		phaseSelectDestEnter(void);
	void		phaseConfirmDestEnter (void);
	void		phaseTalkPopo2ndEnter (void);
	void		phaseFlyAwayEnter (void);
	void		phaseEndEnter (void);

	//	void		phasePlayOcarinaUpdate(void);
	void		phaseComePoppoUpdate(void);
	void		phaseCatchPoppoUpdate(void);
	void		phaseWaitFadeUpdate(void);
	void		phaseTalkPoppoUpdate(void);
	void		phaseSelectDestUpdate(void);
	void		phaseConfirmDestUpdate (void);
	void		phaseTalkPopo2ndUpdate (void);
	void		phaseFlyAwayUpdate (void);
	void		phaseEndUpdate (void);

	//	void		phasePlayOcarinaLeave(void);
	void		phaseComePoppoLeave(void);
	void		phaseCatchPoppoLeave(void);
	void		phaseWaitFadeLeave(void);
	void		phaseTalkPoppoLeave(void);
	void		phaseSelectDestLeave(void);
	void		phaseConfirmDestLeave (void);
	void		phaseTalkPopo2ndLeave (void);
	void		phaseFlyAwayLeave (void);
	void		phaseEndLeave (void);

	void		updatePositionOnPoppo(void);

	/* 変数 */
public:
	static		BYTE sSelectDestIndex;
	
private:
	DWORD		mPhaseCounter;

	ModelPmd	mPoppoModel;
	ModelPmd	mLinoModel;
	VECTOR		mPoppoStartPos;
	VECTOR		mPoppoEndPos;
	USHORT		mNowFrame;
	USHORT		mMaxFrame;
	int			mEffHndSky[2];
	EFK_HANDLE	mEfkBooster[2];

	SHORT		mCamVecRotY;

	float		mPoppoPeriod;

	LayoutBase	mBaseLayout;




};

void DrawFontFunc_SelectDest(void);

#endif // __SUB_SCENE_FLYING_MOVE_H__

/**** end of file ****/


