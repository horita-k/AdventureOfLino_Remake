/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * シーンについての情報を記載
 * @author SPATZ.
 * @data   2011/11/21 00:50:29
 */
//---------------------------------------------------
#ifndef __SCENE_INFO_H__
#define __SCENE_INFO_H__

#include "Scene/ActionTaskManager.h"
#include "DataDefine/MessageDef.h"
#include "DataDefine/EnemyDef.h"

/*=====================================*
 * define
 *=====================================*/
#define _MENU_MERGE_TEST

// 選択中のメニュー
enum {
	eMENU_MERGE,
	eMENU_EFFECT_TEST,
	eMENU_TITLE,
	eMENU_SELECT_FILE,
	eMENU_EVENT,

	eMENU_MAX,
};

static const char* sDebugMenuName[eMENU_MAX] = {
	"Merge Test",
	"EffectTest",
	"Merge TItle",
	"Select File",
	"Event",
};

enum {
	eACTION_OK = 0,
	eACTION_NG,
	/*
	eACTION_LIMIT_ATTACK_NORMAL,
	eACTION_LIMIT_ATTACK_SUB,
	*/

	eACTION_LIMIT_MAX,
};

#if 0
struct tNpcWork {
	BYTE				mNpcNum;
	tNpcInfo*			mpNpcInfo;
};
#endif // 0

// レイアウト設定情報
struct tLayoutSetting {
	const char* mStrLayoutName;
	USHORT		mPosX;
	USHORT		mPosY;
	USHORT		mSizeX;
	USHORT		mSizeY;
	BYTE		mAlpha;
};

/*=====================================*
 * BGの情報
 *=====================================*/
struct tBGInfo {
	BOOL				mIsExtData;
	BYTE				mFileIndex;
	float				mScale;
	const char*			mMusicName;
	eEnemyTblIdx		mEnemyTblIdx;
	int					mActionLimit;
};

struct tStageInfo {
	//	const char* mStageName;
	BYTE		mFileIndex;
	bool		mIsCollision;
};

/*=====================================*
 * macro function
 *=====================================*/
/* メッセージ関連 */
#define MSG_TYPE_TELOP()		TotalInfo::GetInstance()->MsgTypeTelop()
#define MSG_TYPE_MESSAGE()		TotalInfo::GetInstance()->MsgTypeMessage()

//		sLayout[eLY_BALLOON].StartFade(176, sec);
//		sLayout[eLY_BALLOON].StartFade(210, sec);
#define MSG_FADE_IN(sec)		TotalInfo::GetInstance()->MsgFadeIn(sec)
#define MSG_FADE_OUT(sec)		TotalInfo::GetInstance()->MsgFadeOut(sec)
#define MSG_IS_FADE()			TotalInfo::GetInstance()->MsgIsFade()

#define MSG_FADE_IN_PATTERN()	TotalInfo::GetInstance()->MsgFadeIn();	\
	SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
#define MSG_FADE_OUT_PATTERN()	TotalInfo::GetInstance()->MsgFadeOut();	\
	SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);


#define RENDER_MESSAGE_COL(name, msg1, msg2, msg3, msg4, col)	Basic::RenderMessage(name, (char*)msg1, (char*)msg2, (char*)msg3, (char*)msg4, col)
#define RENDER_MESSAGE(name, msg1, msg2, msg3, msg4)			RENDER_MESSAGE_COL(name, msg1, msg2, msg3, msg4, GetColor(0, 0, 0))

/* アイコン関連 */
#define	VISIBLE_BATTLE_ICON()	TotalInfo::GetInstance()->VisibleBattleIcon()
#define HIDE_BATTLE_ICON()		TotalInfo::GetInstance()->HideBattleIcon()

/* バトルフラグ＆タスク関連 */
#define TASKVALUE_TO_BATTLEFLAG(big, little)	(big * 32 + little)
#define BATTLEFLAG_TO_TASKVALUE_A(battleflag)	(battleflag >> 5)
#define BATTLEFLAG_TO_TASKVALUE_B(battleflag)	(battleflag & 0x1F)
#define SET_EVENT_END_FLAG()					(TotalInfo::GetInstance()->SetEventFlag(sTaskInfo.mTaskValueA, sTaskInfo.mTaskValueB))
// イベント終了判定
#define IS_END_EVT(evtA, evtB)					TotalInfo::GetInstance()->GetEventFlagEnable(evtA, evtB)

/* 敵情報取得マクロ */
//#define GET_ENEMY_TBL_INFO()			(&sEnemyArrayTable[spBGInfo->mEnemyTblIdx])
#define GET_ENEMY_TBL_INFO()			(GetEnemyTableInfo(spBGInfo->mEnemyTblIdx))
#define GET_ENEMY_PATTERN_NUM()			(GET_ENEMY_TBL_INFO()->mEnemyPatternNum)
#define GET_ENEMY_KIND(pattern, index)	(*(GET_ENEMY_TBL_INFO()->mpEnemyKind + (pattern * kENEMY_ONCE_MAX) + index))

/*=====================================*
 * define
 *=====================================*/
static const float sEvt24_WaterCheckHeight	= -10.0f;
static const float sEvt24_WaterStopHeight	= -100.0f;
static const int kENEMY_ITEM_NUM			= 5;

// モデル座標更新距離
static const float ALL_MODEL_REVISE_DIST	= 50000.0f;
// スカイドームのデフォルトスケール
static const float kSKYDOME_DEF_SCALE		= 980.0f;

#define EVENT29_DISABLE_OFFSET_X			(20000.0f)

/*=====================================*
 * 各オブジェクトの定義
 *=====================================*/
enum {
	eMD_SKYDOME = 0,
	eMD_SWORD,
	eMD_GETTED_ITEM,
	eMD_LOCKON,
	eMD_SHIELD,

	eMD_MAX,
};

/*
enum {
	eLY_LINO_FACE = 0,
	eLY_SUB_WEAPON,

	eLY_GAUGE_LINE,
	eLY_GAUGE_FRAME,

	eLY_ENEMY_FACE,

	eLY_MAP_ARROW,
	eLY_MAP_POSITION,

	eLY_BALLOON,
	eLY_INFO,

	eLY_DEFAULT_DRAW_MAX,					// 最初から表示されているレイアウトの数

	// 以下のレイアウトが最初は表示されていないレイアウト
	eLY_HAS_MAP = eLY_DEFAULT_DRAW_MAX,		// マップを持っているか？
	eLY_HAS_KEY,							// 鍵を持っているか？

	eLY_MAX,
};
*/

#endif // __SCENE_INFO_H__

/**** end of file ****/

