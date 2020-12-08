/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * EnemyDef.h
 * 敵管理ヘッダー
 * @author SPATZ.
 * @data   2012/04/04 14:48:17
 */
//---------------------------------------------------
#ifndef __ENEMY_DEF_H__
#define __ENEMY_DEF_H__

#include "../Charactor/CharaInfo.h"
#include "ItemDef.h"

#define DRAGON_SCALE	(16.0f)
#define GOBLIN_SCALE	(12.0f)
#define TEEGER_SCALE	(24.0f)
#define TEEGER_BODY_SIZE (330.0f)

static const BYTE	kENEMY_ONCE_MAX = 5;
#define				ENEMY_ARRAY_NUM(arrays)			( sizeof(arrays) / (sizeof(eEnemyKind) * kENEMY_ONCE_MAX) )

/*=====================================*
 * 敵の情報
 *=====================================*/
enum eEnemyKind {
	//
	eENEMY_KIND_KILLER = 0,
	eENEMY_KIND_FIGHTER,
	eENEMY_KIND_FRESBELK,
	eENEMY_KIND_GIGINEBURA,
	eENEMY_KIND_STRATEGIST,
	// 5
	eENEMY_KIND_DRAGON,
	eENEMY_KIND_GARGOYLE,
	eENEMY_KIND_ANHEL,
	eENEMY_KIND_HATOPHON,
	eENEMY_KIND_HATOPHON_Z,
	// 10
	eENEMY_KIND_DUCK,
	eENEMY_KIND_GOBULIN,
	eENEMY_KIND_BAT,
	eENEMY_KIND_MINO,
	eENEMY_KIND_BEAR,
	// 15
	eENEMY_KIND_MALON,
	eENEMY_KIND_KNIGHT,
	eENEMY_KIND_DRAGON_B,
	eENEMY_KIND_GOBULIN_B,
	eENEMY_KIND_WIZARD,
	// 20
	eENEMY_KIND_MAGICIAN,
	eENEMY_KIND_ARMER,
	eENEMY_KIND_OAK,
	eENEMY_KIND_WOODMAN,
	eENEMY_KIND_WOODMAN_B,
	// 25
	eENEMY_KIND_SLIME,
	eENEMY_KIND_RABBIT,
	eENEMY_KIND_RED_DRAGON,
	eENEMY_KIND_MINOTAURUS,
	eENEMY_KIND_DEMON,
	// 30
	eENEMY_KIND_SCORPION,
	eENEMY_KIND_ROBOT_A,
	eENEMY_KIND_ROBOT_B,
	eENEMY_KIND_ROBOT_C,
	eENEMY_KIND_TEEGER,
	// 35
	eENEMY_KIND_HELICOPTER,
	eENEMY_KIND_MEKADRAGON,
	eENEMY_KIND_SA_14,
	eENEMY_KIND_TEEGER_M,
	eENEMY_KIND_MINO_ARMER,
	// 40
	eENEMY_KIND_SPIDER,
	eENEMY_KIND_HONGKONG,
	eENEMY_KIND_LOANA,
	eENEMY_KIND_OAK_BOMB,

	eENEMY_KIND_USAEMON,
	eENEMY_KIND_USIDERMAN,
	eENEMY_KIND_GIMURI,
	eENEMY_KIND_EXCIVY,

	eENEMY_KIND_MAX,
};

enum eEnemyTblIdx {
	eENE_TBL_IDX_NONE = 0,
	eENE_TBL_IDX_000,
	eENE_TBL_IDX_001,
	eENE_TBL_IDX_002,
	eENE_TBL_IDX_003,
	eENE_TBL_IDX_004,
	eENE_TBL_IDX_005,
	eENE_TBL_IDX_010,
	eENE_TBL_KARUN_A,
	eENE_TBL_KARUN_BOSS_A,
	eENE_TBL_IDX_016,
	eENE_TBL_IDX_024,
	eENE_TBL_IDX_026,
	eENE_TBL_IDX_ARENA,
	eENE_TBL_IDX_028,
	eENE_TBL_IDX_029,
	eENE_TBL_IDX_036,
	eENE_TBL_IDX_039,
	eENE_TBL_IDX_040,
	eENE_TBL_IDX_041,
	eENE_TBL_IDX_042,
	eENE_TBL_IDX_046,
	eENE_TBL_IDX_TEST,

	eENE_TBL_IDX_MAX,
};

struct tEnemyWork {
	const char*		mResourceName;
	float			mScale;
	float			mBodySize;
	float			mBodyHeight;
	float			mMoveSpeed;
	int				mHitPoint;
	int				mGetItemKind;
	int				mGetItemPercent;
	//	const char*		mLayoutName;
	const char*		mEnemyName;
};

struct tEnemyTableInfo {
	eEnemyKind*		mpEnemyKind;
	const BYTE		mEnemyPatternNum;
};

/*=====================================*
 * 敵のパラメーターテーブル取得関数
 *=====================================*/
const tEnemyWork*		GetEnemyWorkTable(int enemyKind);
const tEnemyTableInfo*	GetEnemyTableInfo(int tableIndex);

void LoadEnemyAttachmentModel(eEnemyKind kind);


#endif //__ENEMY_DEF_H__

 /**** end of file ****/
