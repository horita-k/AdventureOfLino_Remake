/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * アイテムメニュー選択
 * @author SPATZ.
 * @data   2015/07/13 23:38:22
 */
//---------------------------------------------------
#ifndef __PLAY_OCARINA_H__
#define __PLAY_OCARINA_H__

#include "AppLib/Graphic/ModelBase.h"
#include "SceneBase.h"
#include "SceneInfo.h"
#include "AppLib/Graphic/LayoutBase.h"
#include "AppLib/Graphic/ModelMap.h"
#include "AppLib/Graphic/LayoutNumber.h"

/*=====================================*
  class
 *=====================================*/
class SubScenePlayOcarina : public SubSceneBase {
public:

	struct tSongInfo {
		UCHAR		mSongType;
		UCHAR		mNextState;
		UCHAR		mSongSoundIndex;
		float		mMotionSpeed;
		const char*		mLytName;
		const char*		mSongExplain[2];
	};

	/* 定数 */
	enum {
		ePHASE_NONE = 0,

		ePHASE_SELECT_ENTER,
		ePHASE_SELECT_DOING,

		ePHASE_PLAY_MUSIC,

		ePHASE_MESSAGE,
		ePHASE_MESSAGE_LEAVE,

		ePHASE_FINISH,

		ePHASE_MAX,
	};

	// 演出後のステータス指定
	enum {
		eNEXT_STATE_GAME= 0,
		eNEXT_STATE_WAKEUP_EVENT,
		eNEXT_STATE_APPEAR_TITANIS,
		eNEXT_STATE_FLYING_MOVE,
		eNEXT_STATE_HANDLING_MAGICARMER,

		eNEXT_STATE_MAX,
	};

	/* 関数 */
	SubScenePlayOcarina();
	~SubScenePlayOcarina();

	static tSongInfo* GetSongInfoTable(BYTE songType);

	void		Create(void);
	bool		Update(void);
	void		Destroy(void);
	void		ChangePhase(BYTE setPhase);

	UCHAR		GetNextState(void)						{ return mNextState; }
	void		SetOcarinaSongSelect(BYTE songSelect)	{ mNextItemIndex = songSelect; }// 前回設定値に
	UCHAR		GetOcarinaSongSelect(void)				{ return mNextItemIndex; }

private:


	struct tSongWork {
		const tSongInfo*	mpSongInfo;
		LayoutBase*			mpLayout;
	};

	/*=====================================*
	 * phase
	 *=====================================*/
	typedef void (SubScenePlayOcarina::*PHASE_FUNC)(void);
	static PHASE_FUNC mPhaseFunc[ePHASE_MAX];

	void		phaseNone(void);

	void		phaseSelectEnter(void);
	void		phaseSelectDoing(void);

	void		phasePlayMusic(void);
	void		phaseMessage(void);
	void		phaseMessageLeave(void);

	void		phaseFinish(void);

	
	/* 変数 */
	UCHAR		mNextState;
	//	std::vector<LayoutBase *>mpLayoutList;
	std::vector<tSongWork> mSongWorkList;

	LayoutBase	mMsgLayout;

	UCHAR		mNextItemIndex;
	UCHAR		mNowItemIndex;
	float		mIconDist;
	USHORT		mRotCounter;
	Vec2		mCenterPos;
	BOOL		mIsRight;


};

#endif // __PLAY_OCARINA_H__

/**** end of file ****/


