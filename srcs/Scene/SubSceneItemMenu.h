/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * アイテムメニュー選択
 * @author SPATZ.
 * @data   2015/07/13 23:38:22
 */
//---------------------------------------------------
#ifndef __ITEM_MENU_SELECT_H__
#define __ITEM_MENU_SELECT_H__

#include "AppLib/Graphic/ModelBase.h"
#include "SceneBase.h"
#include "SceneInfo.h"
#include "AppLib/Graphic/LayoutBase.h"
#include "AppLib/Graphic/ModelMap.h"
#include "AppLib/Graphic/LayoutNumber.h"

/*=====================================*
  class
 *=====================================*/
class SubSceneItemMenu : public SubSceneBase {
public:

	static const BYTE kPLATE_LAYOUT_NUM = 3;
	/* 定数 */
	enum {
		ePHASE_NONE = 0,

		ePHASE_ITEM_ENTER,
		ePHASE_ITEM_DOING,
		ePHASE_ITEM_LEVE,

		ePHASE_COSPLAY_ENTER,
		ePHASE_COSPLAY_DOING,
		ePHASE_COSPLAY_LEVE,
		
		ePHASE_KEY_ENTER,
		ePHASE_KEY_DOING,
		ePHASE_KEY_LEVE,

		ePHASE_FINISH,

		ePHASE_MAX,
	};

	enum {
		eMENU_KIND_ITEM = 0,
		eMENU_KIND_COSPLAY,
		eMENU_KIND_KEY,

		eMENU_KIND_MAX,
	};

	/* 関数 */
	SubSceneItemMenu();
	~SubSceneItemMenu();

	void		Create(void)	{}
	void		Create(BYTE defMenuKind, int lyt_num);
	bool		Update(void);
	void		Destroy(void);
	void		ChangePhase(BYTE setPhase);

	eItemKind	GetSelectWeapon(void)			{ return mSelectWeapon; }
	void		SetDefMenuKind(BYTE& rDefMenuKind);

private:
	/*=====================================*
	 * phase
	 *=====================================*/
	typedef void (SubSceneItemMenu::*PHASE_FUNC)(void);
	static PHASE_FUNC mPhaseFunc[ePHASE_MAX];

	void		phaseNone(void);
	void		phaseItemEnter(void);
	void		phaseItemDoing(void);
	void		phaseItemLeave(void);
	void		phaseCosplayEnter(void);
	void		phaseCosplayDoing(void);
	void		phaseCosplayLeave(void);
	void		phaseKeyEnter(void);
	void		phaseKeyDoing(void);
	void		phaseKeyLeave(void);

	void		phaseFinish(void);

	void		updateSelect(void);

	
	/* 変数 */
	int			mRingLytNum;

	BYTE		mItemNum;
	int			mNowItemIndex;
	int			mNextItemIndex;
	int			mRotCnt;
	bool		mIsRight;
	float		mIconDist;

	BYTE		mMenuKind;
	BOOL		mIsAlertMessage;

	// アイテム選択時のアイテム数レイアウト
	LayoutNumber*	mpSelectItemNumLayoutList;

	LayoutBase	mInfoLayout;
	LayoutBase  mPlateLayout[kPLATE_LAYOUT_NUM];
	LayoutBase	mSquareLayout;
	LayoutBase	mMsgLayout;
	eItemKind	mSelectWeapon;

	LayoutBase		mHeartPieceLayout;
	LayoutNumber	mHeartPieceNumLayout;

};

#endif // __ITEM_MENU_SELECT_H__

/**** end of file ****/


