/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * アイテム取得関連
 * @author SPATZ.
 * @data   2015/07/13 23:38:22
 */
//---------------------------------------------------
#ifndef __SUB_SCENE_GET_ITEM_H__
#define __SUB_SCENE_GET_ITEM_H__

#include "AppLib/Graphic/ModelBase.h"
#include "SceneBase.h"
#include "SceneInfo.h"
#include "AppLib/Graphic/LayoutBase.h"
#include "AppLib/Graphic/ModelMap.h"
#include "AppLib/Graphic/LayoutNumber.h"

/*=====================================*
  class
 *=====================================*/
class SubSceneGetItem : public SubSceneBase {
public:

	/* 定数 */
	enum {
		ePHASE_NONE = 0,

		ePHASE_BIG_TREASURE_ENTER,
		ePHASE_BIG_TREASURE_UPDATE,
		ePHASE_BIG_TREASURE_LEAVE,

		ePHASE_SMALL_TREASURE_ENTER,
		ePHASE_SMALL_TREASURE_UPDATE,
		ePHASE_SMALL_TREASURE_LEAVE,

		ePHASE_CRYSTAL_ENTER,
		ePHASE_CRYSTAL_UPDATE,
		ePHASE_CRYSTAL_LEAVE,

		ePHASE_CRYSTAL_MOVING,
		
		ePHASE_FINISH,

		ePHASE_MAX,
	};

	/* 関数 */
	SubSceneGetItem();
	~SubSceneGetItem();

	void		Create(void);
	bool		Update(void);
	void		Destroy(void);
	void		ChangePhase(BYTE setPhase);

private:
	/*=====================================*
	 * phase
	 *=====================================*/
	typedef void (SubSceneGetItem::*PHASE_FUNC)(void);
	static PHASE_FUNC mPhaseFunc[ePHASE_MAX];

	void		phaseNone(void);

	void		phaseBigTreasureEnter(void);
	void		phaseBigTreasureUpdate(void);
	void		phaseBigTreasureLeave(void);

	void		phaseSmallTreasureEnter(void);
	void		phaseSmallTreasureUpdate(void);
	void		phaseSmallTreasureLeave(void);

	void		phaseCrystalEnter(void);
	void		phaseCrystalUpdate(void);
	void		phaseCrystalLeave(void);
	void		phaseCrystalMoving(void);

	void		phaseFinish(void);

	void		settingModelByItemKind(eItemKind itemKind);

	bool		equalPhaseFrame(float frame);
	bool		betweenPhaseFrame(float frameMin, float frameMax);
	
	/* 変数 */
	ModelBase	mGettedItemModel;
	DWORD		mPhaseCounter;


};

#endif // __SUB_SCENE_GET_ITEM_H__

/**** end of file ****/


