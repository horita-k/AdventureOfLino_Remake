/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * プレイヤー管理クラス
 * @author SPATZ.
 * @data   2012/02/05 21:50:51
 */
//---------------------------------------------------
#include <math.h>

#include "CharaPlayer.h"
#include "CharaEnemy.h"
#include "CharaGuardian.h"
#include "../Gimmick/GimmickBase.h"
#include "AppLib/Basic/Basic.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Effect/EffectManager.h"
#include "AppLib/Input/InputManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "Scene/ActionTaskManager.h"
#include "Scene/SceneInfo.h"
#include "Scene/SceneMainGame.h"
#include "Scene/CommandFunctions.h"
#include "AppLib/Graphic/EffekseerManager.h"
#include "AppLib/Collision/CollisionManager.h"
#include "AppLib/Resource/ResourceLayout.h"
#include "AppLib/Resource/ResourceModel.h"
#include "DataDefine/MotionDef.h"
#include "DataDefine/VoiceDef.h"
#include "DataDefine/EffectDef.h"
#include "DataDefine/CharaDef.h"
#include "Utility/Utility.h"
#include "CharaInfo.h"

#include "Charactor/CharaNavi.h"

#include "AppLib/Basic/Counter.h"

#include <map>

extern CharaNavi		sNavi;

static VECTOR			sHookHitStartPos = ZERO_VEC;
static VECTOR			sHookHitEndPos = ZERO_VEC;
static const float		kHOOK_REVERSE_SPEED = 0.03f;
static int				sEffWindIndex = -1;
static bool				sIsAnalogInputRun = false;
static int				sIdleMotionIndex = eMT_IDLE;
static VECTOR			sHookShotTargetVec	= ZERO_VEC;
static VECTOR			sBoomerangTargetVec = ZERO_VEC;
static int				sResetCameraBeforeState = -1;
static eFootstepKind	sFootstepKind = eFOOTSTEP_KIND_DEFAULT; /* 0 or 1 */

enum eFootSoundType {
	eFOOT_SND_STEP_A = 0,
	eFOOT_SND_STEP_B,
	eFOOT_SND_LANDING,

	eFOOT_SND_MAX,
};
// 足音用テーブル
static const int sFootSoundTable[eFOOTSTEP_KIND_MAX][eFOOT_SND_MAX] = {
	{eSE_FOOT_STEP_A,		eSE_FOOT_STEP_B,		eSE_LANDING,},				// デフォルト
	{eSE_FOOT_STEP_WATER_A,	eSE_FOOT_STEP_WATER_B,	eSE_LANDING_WATER,},		// 水
	{eSE_FOOT_STEP_GRASS_A,	eSE_FOOT_STEP_GRASS_B,	eSE_LANDING_GRASS,},		// 草
	{eSE_FOOT_STEP_SAND_A,	eSE_FOOT_STEP_SAND_B,	eSE_LANDING_SAND,},			// 砂
	{eSE_FOOT_STEP_SOIL_A,	eSE_FOOT_STEP_SOIL_B,	eSE_LANDING_SOIL,},			// 土
	{eSE_COMICAL,			eSE_COMICAL,			eSE_COMICAL,},				// コミカル
	{eSE_FOOT_STEP_ROBOT,	eSE_FOOT_STEP_ROBOT,	eSE_FOOT_STEP_ROBOT,},		// ロボット
};

// 最大HP
static int sMaxHitpointTemp = (3 * 4);

static tBoomerangWork sBoomerang = { eBOOMERANG_STATE_NONE, ZERO_VEC, ZERO_VEC, -1 };

enum eDirection {
	eDIRECTION_FRONT = 0,
	eDIRECTION_RIGHT,
	eDIRECTION_BACK,
	eDIRECTION_LEFT,

	eDIRECTION_MAX,
};
static const int sAwayJoyUpTable[eDIRECTION_MAX] = {
	CharaPlayer::eCHARA_STATE_AWAY_FRONT,
	CharaPlayer::eCHARA_STATE_AWAY_LEFT,
	CharaPlayer::eCHARA_STATE_AWAY_BACK,
	CharaPlayer::eCHARA_STATE_AWAY_RIGHT,
};
static const int sAwayJoyRightTable[eDIRECTION_MAX] = {
	CharaPlayer::eCHARA_STATE_AWAY_RIGHT,
	CharaPlayer::eCHARA_STATE_AWAY_FRONT,
	CharaPlayer::eCHARA_STATE_AWAY_LEFT,
	CharaPlayer::eCHARA_STATE_AWAY_BACK,
};
static const int sAwayJoyDownTable[eDIRECTION_MAX] = {
	CharaPlayer::eCHARA_STATE_AWAY_BACK,
	CharaPlayer::eCHARA_STATE_AWAY_RIGHT,
	CharaPlayer::eCHARA_STATE_AWAY_FRONT,
	CharaPlayer::eCHARA_STATE_AWAY_LEFT,
};
static const int sAwayJoyLeftTable[eDIRECTION_MAX] = {
	CharaPlayer::eCHARA_STATE_AWAY_LEFT,
	CharaPlayer::eCHARA_STATE_AWAY_BACK,
	CharaPlayer::eCHARA_STATE_AWAY_RIGHT,
	CharaPlayer::eCHARA_STATE_AWAY_FRONT,
};

static const float kARCHERY_INPUT_VALUE[CameraManager::eARCHERY_CAM_TYPE_MAX] = {
	0.10f, 
	1.0f, 
	1.2f,
};
static const float kArcheryNormalDist	= 5000.0f;		// 弓通常時の最大飛距離
static const float kArcheryScopeDist	= 15000.0f;		// 弓スコープ時の最大飛距離
static const float sThrustAttackVec		= 0.77f;		// 突き攻撃の移動量
static const float sSmashAttackVec		= 0.77f * 0.65f;// スマッシュ攻撃の移動量

static const char* strHookShotHead		= "ヘッド";
static const char* strHookShotBottom	= "フックショット";
static const float kHOOKSHOT_LENGTH		= 300.0f * (10.6f);	// 左辺はモーションの距離の値を設定 (10.6fは固定)
static const USHORT kHOOKSHOT_UV_MAX	= 108 * 4;			// モデルのUVの値を設定 

static const SHORT kPIYO_PIYO_GAUGE_MAX	= 10000;		// ピヨピヨゲーム最大数

static const BYTE kMP_CONSUME_THUNDER	= 30;			// 魔法消費量
static const BYTE kMP_CONSUME_FLAME		= 25;			// 魔法消費量
static const BYTE kMP_CONSUME_TORNADE	= 20;			// 魔法消費量
static const BYTE kMP_CONSUME_ROD		= 8;			// 魔法消費量

/*=====================================*
 * サブウェポン定義
 *=====================================*/
// 爆弾定義
static ObjectHand_Bomb			sPlayerBomb[kPLAYER_BOMB_NUM];
static bool						sIsPlayBomb = false;

// 弓矢定義
static ObjectWeapon_Arrow		sPlayerArrow[kPLAYER_ARROW_NUM];
static BYTE						sUsePlayerArrowIndex = 0;

// 天使の翼
static ModelPmd					sAngelWing;		// 天使の翼
static ModelPmd					sHookshotChain;	// フックショットのチェーン部分

// コピーロッド定義
static ObjectWeapon_CopyRod		*spPlayerCopyRod = NULL;

/*=====================================*
 * static
 *=====================================*/
/*static*/
CharaPlayer::PHASE_STATE 
CharaPlayer::mPhaseEnterState[CharaPlayer::eCHARA_STATE_MAX] = {
	&CharaPlayer::phaseEnterStateIdle,
	&CharaPlayer::phaseEnterStateTurn,
	&CharaPlayer::phaseEnterStateMove,
	&CharaPlayer::phaseEnterStateJumpUp,
	&CharaPlayer::phaseEnterStateJumpDown,
	&CharaPlayer::phaseEnterStateJumpEnd,
	&CharaPlayer::phaseEnterStateDamage,
	&CharaPlayer::phaseEnterStateRetireStart,
	&CharaPlayer::phaseEnterStateRetireDoing,
	&CharaPlayer::phaseEnterStateRetireEnd,
	&CharaPlayer::phaseEnterStateSlopeDown,
	&CharaPlayer::phaseEnterStateRefuseInput,	//		eCHARA_STATE_RESERVE_1,
	&CharaPlayer::phaseEnterStateDownStart,		//		eCHARA_STATE_RESERVE_2,
	&CharaPlayer::phaseEnterStateDownDoing,		//		eCHARA_STATE_RESERVE_3,
	&CharaPlayer::phaseEnterStateDownEnd,		//		eCHARA_STATE_RESERVE_4,
	&CharaPlayer::phaseEnterStateFallDown,		//		eCHARA_STATE_RESERVE_5,
	NULL, //		eCHARA_STATE_RESERVE_6,
	NULL, //		eCHARA_STATE_RESERVE_7,
	NULL, //		eCHARA_STATE_RESERVE_8,
	NULL, //		eCHARA_STATE_RESERVE_9,
	&CharaPlayer::phaseEnterStateNothing,
	&CharaPlayer::phaseEnterStateAttack1,
	&CharaPlayer::phaseEnterStateAttack2,
	&CharaPlayer::phaseEnterStateAttack3,
	&CharaPlayer::phaseEnterStateAttack4,
	&CharaPlayer::phaseEnterStateAttackThrust,
	&CharaPlayer::phaseEnterStateAttackRollingStart,
	&CharaPlayer::phaseEnterStateAttackRolling,
	&CharaPlayer::phaseEnterStateHookShotStart,
	&CharaPlayer::phaseEnterStateHookShotting,
	&CharaPlayer::phaseEnterStateHookShotEnd,
	&CharaPlayer::phaseEnterStateHookMoving,
	&CharaPlayer::phaseEnterStateAwayRight,
	&CharaPlayer::phaseEnterStateAwayLeft,
	&CharaPlayer::phaseEnterStateAwayBack,
	&CharaPlayer::phaseEnterStatePutSword,
	&CharaPlayer::phaseEnterStateBoomerangStart,
	&CharaPlayer::phaseEnterStateAwayFront,
	&CharaPlayer::phaseEnterStateAttackJump,
	&CharaPlayer::phaseEnterStateSleep,
	&CharaPlayer::phaseEnterStateSmile,
	&CharaPlayer::phaseEnterStateWaitResetCamera,
	&CharaPlayer::phaseEnterStateLibShieldStart,
	&CharaPlayer::phaseEnterStateRaiseIdle,
	&CharaPlayer::phaseEnterStateRaiseTurn,
	&CharaPlayer::phaseEnterStateRaiseMove,
	&CharaPlayer::phaseEnterStateRaiseThrow,
	&CharaPlayer::phaseEnterStateRaisePut,
	&CharaPlayer::phaseEnterStateBombStart,
	&CharaPlayer::phaseEnterStateArcheryStart,
	&CharaPlayer::phaseEnterStateArcheryCharge,
	&CharaPlayer::phaseEnterStateArcheryShoot,
	&CharaPlayer::phaseEnterStateArcheryEnd,

	&CharaPlayer::phaseEnterStateAttackSmashStart,
	&CharaPlayer::phaseEnterStateAttackSmashDoing,
	&CharaPlayer::phaseEnterStateAttackSmashEnd,
	&CharaPlayer::phaseEnterStateLookAround,

	&CharaPlayer::phaseEnterStateAttackRear,
	&CharaPlayer::phaseEnterStateAttackRollingKick,

	&CharaPlayer::phaseEnterStateGuardStagger,
	&CharaPlayer::phaseEnterStatePiyoPiyo,
	&CharaPlayer::phaseEnterStateGutsPose,
	&CharaPlayer::phaseEnterStateMagicThunder,
	&CharaPlayer::phaseEnterStateMagicFlame,
	&CharaPlayer::phaseEnterStateMagicTornade,
	&CharaPlayer::phaseEnterStatePlayOcarina,
	&CharaPlayer::phaseEnterStateCopyRod,
	&CharaPlayer::phaseEnterStateAfterEvent,
};

/*static*/
CharaPlayer::PHASE_STATE 
CharaPlayer::mPhaseState[CharaPlayer::eCHARA_STATE_MAX] = {
	&CharaPlayer::phaseUpdateStateIdle,
	&CharaPlayer::phaseUpdateStateTurn,
	&CharaPlayer::phaseUpdateStateMove,
	&CharaPlayer::phaseUpdateStateJumpUp,
	&CharaPlayer::phaseUpdateStateJumpDown,
	&CharaPlayer::phaseUpdateStateJumpEnd,
	&CharaPlayer::phaseUpdateStateDamage,
	&CharaPlayer::phaseUpdateStateRetireStart,
	&CharaPlayer::phaseUpdateStateRetireDoing,
	&CharaPlayer::phaseUpdateStateRetireEnd,
	&CharaPlayer::phaseUpdateStateSlopeDown,
	&CharaPlayer::phaseUpdateStateRefuseInput,		//		eCHARA_STATE_RESERVE_1,
	&CharaPlayer::phaseUpdateStateDownStart,			//		eCHARA_STATE_RESERVE_2,
	&CharaPlayer::phaseUpdateStateDownDoing,			//		eCHARA_STATE_RESERVE_3,
	&CharaPlayer::phaseUpdateStateDownEnd,			//		eCHARA_STATE_RESERVE_4,
	&CharaPlayer::phaseUpdateStateFallDown,			//		eCHARA_STATE_RESERVE_5,
	NULL, //		eCHARA_STATE_RESERVE_6,
	NULL, //		eCHARA_STATE_RESERVE_7,
	NULL, //		eCHARA_STATE_RESERVE_8,
	NULL, //		eCHARA_STATE_RESERVE_9,
	&CharaPlayer::phaseUpdateStateNothing,
	&CharaPlayer::phaseUpdateStateAttack1,
	&CharaPlayer::phaseUpdateStateAttack2,
	&CharaPlayer::phaseUpdateStateAttack3,
	&CharaPlayer::phaseUpdateStateAttack4,
	&CharaPlayer::phaseUpdateStateAttackThrust,
	&CharaPlayer::phaseUpdateStateAttackRollingStart,
	&CharaPlayer::phaseUpdateStateAttackRolling,
	&CharaPlayer::phaseUpdateStateHookShotStart,
	&CharaPlayer::phaseUpdateStateHookShotting,
	&CharaPlayer::phaseUpdateStateHookShotEnd,
	&CharaPlayer::phaseUpdateStateHookMoving,
	&CharaPlayer::phaseUpdateStateAwayRight,
	&CharaPlayer::phaseUpdateStateAwayLeft,
	&CharaPlayer::phaseUpdateStateAwayBack,
	&CharaPlayer::phaseUpdateStatePutSword,
	&CharaPlayer::phaseUpdateStateBoomerangStart,
	&CharaPlayer::phaseUpdateStateAwayFront,
	&CharaPlayer::phaseUpdateStateAttackJump,
	&CharaPlayer::phaseUpdateStateSleep,
	&CharaPlayer::phaseUpdateStateSmile,
	&CharaPlayer::phaseUpdateStateWaitResetCamera,
	&CharaPlayer::phaseUpdateStateLibShieldStart,
	&CharaPlayer::phaseUpdateStateRaiseIdle,
	&CharaPlayer::phaseUpdateStateRaiseTurn,
	&CharaPlayer::phaseUpdateStateRaiseMove,
	&CharaPlayer::phaseUpdateStateRaiseThrow,
	&CharaPlayer::phaseUpdateStateRaisePut,
	&CharaPlayer::phaseUpdateStateBombStart,
	&CharaPlayer::phaseUpdateStateArcheryStart,
	&CharaPlayer::phaseUpdateStateArcheryCharge,
	&CharaPlayer::phaseUpdateStateArcheryShoot,
	&CharaPlayer::phaseUpdateStateArcheryEnd,

	&CharaPlayer::phaseUpdateStateAttackSmashStart,
	&CharaPlayer::phaseUpdateStateAttackSmashDoing,
	&CharaPlayer::phaseUpdateStateAttackSmashEnd,

	&CharaPlayer::phaseUpdateStateLookAround,

	&CharaPlayer::phaseUpdateStateAttackRear,
	&CharaPlayer::phaseUpdateStateAttackRollingKick,

	&CharaPlayer::phaseUpdateStateGuardStagger,
	&CharaPlayer::phaseUpdateStatePiyoPiyo,
	&CharaPlayer::phaseUpdateStateGutsPose,
	&CharaPlayer::phaseUpdateStateMagicThunder,
	&CharaPlayer::phaseUpdateStateMagicFlame,
	&CharaPlayer::phaseUpdateStateMagicTornade,
	&CharaPlayer::phaseUpdateStatePlayOcarina,
	&CharaPlayer::phaseUpdateStateCopyRod,
	&CharaPlayer::phaseUpdateStateAfterEvent,
};

/*static*/
CharaPlayer::PHASE_STATE 
CharaPlayer::mPhaseLeaveState[CharaPlayer::eCHARA_STATE_MAX] = {
	&CharaPlayer::phaseLeaveStateIdle,
	&CharaPlayer::phaseLeaveStateTurn,
	&CharaPlayer::phaseLeaveStateMove,
	&CharaPlayer::phaseLeaveStateJumpUp,
	&CharaPlayer::phaseLeaveStateJumpDown,
	&CharaPlayer::phaseLeaveStateJumpEnd,
	&CharaPlayer::phaseLeaveStateDamage,
	&CharaPlayer::phaseLeaveStateRetireStart,
	&CharaPlayer::phaseLeaveStateRetireDoing,
	&CharaPlayer::phaseLeaveStateRetireEnd,
	&CharaPlayer::phaseLeaveStateSlopeDown,
	&CharaPlayer::phaseLeaveStateRefuseInput,	//		eCHARA_STATE_RESERVE_1,
	&CharaPlayer::phaseLeaveStateDownStart,		//		eCHARA_STATE_RESERVE_2,
	&CharaPlayer::phaseLeaveStateDownDoing,		//		eCHARA_STATE_RESERVE_3,
	&CharaPlayer::phaseLeaveStateDownEnd,		//		eCHARA_STATE_RESERVE_4,
	&CharaPlayer::phaseLeaveStateFallDown,		//		eCHARA_STATE_RESERVE_5,
	NULL, //		eCHARA_STATE_RESERVE_6,
	NULL, //		eCHARA_STATE_RESERVE_7,
	NULL, //		eCHARA_STATE_RESERVE_8,
	NULL, //		eCHARA_STATE_RESERVE_9,
	&CharaPlayer::phaseLeaveStateNothing,
	&CharaPlayer::phaseLeaveStateAttack1,
	&CharaPlayer::phaseLeaveStateAttack2,
	&CharaPlayer::phaseLeaveStateAttack3,
	&CharaPlayer::phaseLeaveStateAttack4,
	&CharaPlayer::phaseLeaveStateAttackThrust,
	&CharaPlayer::phaseLeaveStateAttackRollingStart,
	&CharaPlayer::phaseLeaveStateAttackRolling,
	&CharaPlayer::phaseLeaveStateHookShotStart,
	&CharaPlayer::phaseLeaveStateHookShoting,
	&CharaPlayer::phaseLeaveStateHookShotEnd,
	&CharaPlayer::phaseLeaveStateHookMoving,
	&CharaPlayer::phaseLeaveStateAwayRight,
	&CharaPlayer::phaseLeaveStateAwayLeft,
	&CharaPlayer::phaseLeaveStateAwayBack,
	&CharaPlayer::phaseLeaveStatePutSword,
	&CharaPlayer::phaseLeaveStateBoomerangStart,
	&CharaPlayer::phaseLeaveStateAwayFront,
	&CharaPlayer::phaseLeaveStateAttackJump,
	&CharaPlayer::phaseLeaveStateSleep,
	&CharaPlayer::phaseLeaveStateSmile,
	&CharaPlayer::phaseLeaveStateWaitResetCamera,
	&CharaPlayer::phaseLeaveStateLibShieldStart,
	&CharaPlayer::phaseLeaveStateRaiseIdle,
	&CharaPlayer::phaseLeaveStateRaiseTurn,
	&CharaPlayer::phaseLeaveStateRaiseMove,
	&CharaPlayer::phaseLeaveStateRaiseThrow,
	&CharaPlayer::phaseLeaveStateRaisePut,
	&CharaPlayer::phaseLeaveStateBombStart,
	&CharaPlayer::phaseLeaveStateArcheryStart,
	&CharaPlayer::phaseLeaveStateArcheryCharge,
	&CharaPlayer::phaseLeaveStateArcheryShoot,
	&CharaPlayer::phaseLeaveStateArcheryEnd,

	&CharaPlayer::phaseLeaveStateAttackSmashStart,
	&CharaPlayer::phaseLeaveStateAttackSmashDoing,
	&CharaPlayer::phaseLeaveStateAttackSmashEnd,

	&CharaPlayer::phaseLeaveStateLookAround,
	&CharaPlayer::phaseLeaveStateAttackRear,
	&CharaPlayer::phaseLeaveStateAttackRollingKick,

	&CharaPlayer::phaseLeaveStateGuardStagger,
	&CharaPlayer::phaseLeaveStatePiyoPiyo,
	&CharaPlayer::phaseLeaveStateGutsPose,
	&CharaPlayer::phaseLeaveStateMagicThunder,
	&CharaPlayer::phaseLeaveStateMagicFlame,
	&CharaPlayer::phaseLeaveStateMagicTornade,
	&CharaPlayer::phaseLeaveStatePlayOcarina,
	&CharaPlayer::phaseLeaveStateCopyRod,
	&CharaPlayer::phaseLeaveStateAfterEvent,
};

//---------------------------------------------------
/**
 *  爆弾を持たせる
 */
//---------------------------------------------------
#define RAISE_ATTACH_MODEL_MACRO()										\
	if (mpObjectHand != NULL) {											\
		VECTOR tmpA, tmpB;												\
		GetFramePosFromName(&tmpA, "右手首");							\
		GetFramePosFromName(&tmpB, "左手首");							\
		mpObjectHand->Position() = VScale(VAdd(tmpA, tmpB), 0.5f);		\
		mpObjectHand->Rotation().y = mRotation.y;						\
	}
#define WALK_SOUND_SECOND_A		(29.0f);		// 歩きAの足音のタイミング
#define WALK_SOUND_SECOND_B		(13.0f);		// 歩きBの足音のタイミング
#define RUN_SOUND_SECOND_A		(15.0f);		// 走りAの足音のタイミング
#define RUN_SOUND_SECOND_B		(30.0f);		// 走りBの足音のタイミング
#define IS_COSPLAY				((mCosplayIndex == 0) ? FALSE : TRUE)

/*=====================================*
 * 足音更新マクロ
 *=====================================*/
#define UPDATE_FOOTSTEP_KIND()	\
	{																										\
	if ((sFootstepKind != eFOOTSTEP_KIND_WATER) &&															\
		(sFootstepKind != eFOOTSTEP_KIND_GUARDIAN)) {														\
			for (int iI = 0; iI < mpFootstepRelateInfo->mRelateNum; iI ++) {								\
				if (mpFootstepRelateInfo->mpRelateInfo[iI].mMaterialIndex == this->mEarthMaterialIndex) {	\
					sFootstepKind = mpFootstepRelateInfo->mpRelateInfo[iI].mFootstepKind;					\
					break;																					\
				}																							\
			}																								\
		}																									\
	}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaPlayer::CharaPlayer()
{
	// initialize
	mMaxRunSpeed		= kCHARA_RUN_SPEED;
	
	mRunSpeed			= mMaxRunSpeed;
	mpSubweaponFunc		= &CharaPlayer::subweaponFuncNone;
	mIsPermitInput		= true;
	mIsAway				= false;
	mIsSubAttack		= false;
	mIsJumpAttack		= false;
	mIsLeaveWeapon		= false;
	mpLockonChara		= NULL;
	mWingJumpState		= eWING_JUMP_STATE_NONE;

	memset(&mItemWork, 0x00, sizeof(mItemWork));
	mItemWork.mItemSubWeapon = eITEM_KIND_NOWEAPON;

	mCameraGeneralVec	= ZERO_VEC;
	mpObjectHand		= NULL;

	mpFootstepRelateInfo= NULL;
	mEfkHnd.uAngelWing	= -1;
	mActionLimit		= eACTION_NG;
	mCosplayIndex		= eWINMENU_COSPLAY_NONE;
	mPiyoPiyoValue		= kPIYO_PIYO_GAUGE_MAX;
	mpCheckPointPtr		= NULL;
	mIsSoakWater		= FALSE;
	mUsingRecoverItemKind = eITEM_KIND_NONE;
	mCount.uSleep		= 0;
	mMagicPoint			= 100;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaPlayer::~CharaPlayer()
{
	if (spPlayerCopyRod != NULL) {
		spPlayerCopyRod->DeleteModel();
		delete spPlayerCopyRod;
		spPlayerCopyRod = NULL;
	}

	sAngelWing.DeleteModel();
	sHookshotChain.DeleteModel();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::ReviseModel(VECTOR* pMoveVec)
{
	mPosition = VSub(mPosition, *pMoveVec);
	this->ForciblySetPositionByCentering(true);

	mPrevPosition = VSub(mPrevPosition, *pMoveVec);
	mLastEarthPos = VSub(mLastEarthPos, *pMoveVec);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::SetLoadParameter(int maxHP, int nowHP, UCHAR nowMP)
{
	sMaxHitpointTemp = maxHP;
	mHitpoint = nowHP;
	mMagicPoint = nowMP;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::Setup(bool isDefaultMotioon, int stageIndex)
{
	int now_hitpoint = mHitpoint; // baseの initialize 前のHPを退避

	CharaBase::Setup(isDefaultMotioon);
	mShadow.Setup(this, Shadow::eUPDATE_TYPE_ALWAYS);

	// トゥーンシェーダを表示する
	EnableToonShader(0.04f, 0.015f);
	
	//<! Add : DxLibバージョン3_10aから3_12e移行対応
	MV1SetUseVertDifColor( mModelHandle, FALSE);
	//	MV1SetUseVertSpcColor( mModelHandle, FALSE);
	MV1SetUseVertSpcColor( mModelHandle, TRUE);
	//<! End : DxLibバージョン3_10aから3_12e移行対応

	mMaxHitpoint = sMaxHitpointTemp;
	mHitpoint = now_hitpoint;
	if (mHitpoint == 0) {
		mHitpoint = mMaxHitpoint;
	}
	/* ロックオンキャラをリセット */
	mpLockonChara = NULL;

	/* 手持ちモデルをリセット */
	mpObjectHand = NULL;

	/* 手放しフラグを無効にする */
	mIsLeaveWeapon	= false;

	/* マテリアルタイプをすべて標準タイプへ変更 */
	float mul_rate = 2.0f;

	mMaxRunSpeed		= kCHARA_RUN_SPEED;

	(this->*mPhaseEnterState[mCharaState])();

	/* キャラの服の設定を行う */
	mItemWork.mItemCloth = eITEM_CLOTH_NORMAL;

	int cloth_type;
	cloth_type = 0;

	for (int iI = 0; iI < kPLAYER_BOMB_NUM; iI ++) {
		sPlayerBomb[iI].State() = ObjectHand::eHAND_STATE_DISAPPEAR;
	}

	// 足音テーブルを追加する
	// まず、ダミーテーブルを設定する
	mpFootstepRelateInfo = &sFootstepRelateManageTable[sFootstepRelateMangerTableNum];
	for (int iI = 0; iI < sFootstepRelateMangerTableNum; iI ++) {
		if (stageIndex == sFootstepRelateManageTable[iI].mPlaceIndex) {
			mpFootstepRelateInfo = &sFootstepRelateManageTable[iI];
			break;
		}
	}	

	// 弓矢の設定
	ObjectWeapon_Arrow* p_arrow = NULL;
	for (int iI = 0; iI < kPLAYER_ARROW_NUM; iI ++) {
		p_arrow = &sPlayerArrow[iI];
		if (p_arrow->GetModelHandle() == -1) {
			p_arrow->Setup(eOBJECT_KIND_ARROW, (eItemKind)0, (VECTOR *)&ZERO_VEC, 0, true);
			p_arrow->SetMatrixType(ModelBase::eMATRIX_TYPE_MATRIX);
		}
	}

	// 天使の翼の設定
	if (sAngelWing.GetModelHandle() == -1) {
		sAngelWing.LinkModel(RES_MDL_ANGEL_WING);
		sAngelWing.Setup();
		sAngelWing.SetMeshVisible(false, 5);	// 鞄は非表示にする
	}

	// フックショットの設定
	if (sHookshotChain.GetModelHandle() == -1) {
		sHookshotChain.LinkModel(RES_MDL_LONGHOOK);
		sHookshotChain.Setup();
	}

	mPiyoPiyoValue		= kPIYO_PIYO_GAUGE_MAX;

	// スフィアテクスチャ番号の指定
	if (IS_COSPLAY == FALSE) {
		mSphereTexIndex = 2;
	}
	// ガーディアンの場合
	else if (mCosplayIndex == eWINMENU_COSPLAY_GUARDIAN) {
		mSphereTexIndex = 1;
	}
	else {
		mSphereTexIndex = -1;
	}

	this->SetMatrixType(eMATRIX_TYPE_TRS);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::Draw(void)
{
	ModelBase::Draw();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::Update(void)
{
	// ロックオンを行える距離
	static float sLockonCheckLength = (5000.0f * ALL_SCALE); // 5000だと少し短いかも...？

	float dot = 0.05f;
	float width = 0.05f;

	/* アナログジョイでの走り判定 */
	int value_x, value_y, len;
	sIsAnalogInputRun = false;
	GetJoypadAnalogInput(&value_x, &value_y, DX_INPUT_PAD1);

	len = (int)sqrt((double)((value_x * value_x) + (value_y * value_y)));
	if (len > 950) {	// (950 / 1000)
		sIsAnalogInputRun = true;
	}

	/* サブウェポン時に実行される関数 */
	(this->*mpSubweaponFunc)();

	ModelPmd::Update();
	mMoveVec = ZERO_VEC;
	(this->*mPhaseState[mCharaState])();

	if (mIsPermitInput == true) {

		/**** ロックオンの処理 ****/
		if (mpLockonChara == NULL) {
			if (CHECK_TRIG_JOY(JOY_INPUT_LOCKON)) {

				ModelCollision* p_mine = this;
				ModelCollision* p_your;
				float nearest = 0.0f;
				float length;

				// まず、敵のモデルのみをチェックする
				for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
					p_your = (ModelCollision *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
					if (p_your->GetIsEnemy() == FALSE) continue;
					if (p_your->GetIsRetire()) continue;

					length = VSize(VSub(p_your->Position(), p_mine->Position()));
			
					if (length < sLockonCheckLength) {
						if (mpLockonChara == NULL) {
							CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_LOCKON);
							CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_LOW, true);

							nearest = length;
							mpLockonChara = p_your;
						} 
						else {
							if (nearest > length) {
								nearest = length;
								mpLockonChara = p_your;
							}
						}
					}
				}

				// まず、敵がいない場合オブジェクトのみをチェックする
				if (mpLockonChara == NULL) {
					for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
						p_your = (ModelCollision *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
						if (p_your->GetIsEnemy() == TRUE) continue;
						if (p_mine == p_your) continue;
						if (p_your->GetIsRetire()) continue;
						if (mpObjectHand == p_your) continue;	// 手持ちモデルも判定しない

						length = VSize(VSub(p_your->Position(), p_mine->Position()));
			
						if (length < sLockonCheckLength) {
							if (mpLockonChara == NULL) {
								CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_LOCKON);
								CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_LOW, true);

								nearest = length;
								mpLockonChara = p_your;
							} 
							else {
								if (nearest > length) {
									nearest = length;
									mpLockonChara = p_your;
								}
							}
						}
					}
				}
			}
		}
		else {
			if ((mCharaState != eCHARA_STATE_TURN) && 
				(mCharaState != eCHARA_STATE_MOVE) &&

				(mCharaState != eCHARA_STATE_RAISE_TURN) && 
				(mCharaState != eCHARA_STATE_RAISE_MOVE) &&

				(mCharaState != eCHARA_STATE_AWAY_FRONT) &&
				(mIsLand == true)) {

				/* キャラを少しずつ敵の方に向かせる */
				VECTOR vec, lockon_pos;
				float target_rot;
				mpLockonChara->GetLockonPosition(&lockon_pos);
				vec = VSub(lockon_pos, this->Position());
				target_rot = atan2(vec.x, vec.z) + DX_PI;
				Utility::CorrectAngleFrom000To360(&target_rot);

				float diff_rot = target_rot - mRotation.y;
				if (kTURN_MOVE_ROT >= abs(diff_rot)) {
					mRotation.y = target_rot;
				} else {
					bool is_plus = (diff_rot >= 0.0f) ? true : false;
					if (abs(diff_rot) > Utility::DegToRad(180.0f)) {
						is_plus ^= true;
					}
					mRotation.y += (is_plus) ? kTURN_MOVE_ROT : -kTURN_MOVE_ROT;
				}			
				Utility::CorrectAngleFrom000To360(&mRotation.y);
			}

			if (mpLockonChara->GetIsRetire()) {
				DisableLockonChara();
			}
			if (CHECK_TRIG_JOY(JOY_INPUT_LOCKON)) {
				DisableLockonChara();
			}
		}

		/**** 回避時の処理 ****/
		if ((mIsDamage == false) && 
			((mEarthState == eEARTH_STATE_LINE) || (mEarthState == eEARTH_STATE_CAPSULE)) 
			&& (mIsAway == false) && (mIsSubAttack == false) && (mCharaState != eCHARA_STATE_PIYOPIYO)) {

			// 回転回避
			if (CHECK_PRESS_JOY(JOY_INPUT_AWAY)) {
				// カメラとキャラの内積を求める
				VECTOR cam_vec = VSub(CameraManager::GetInstance()->Target(), CameraManager::GetInstance()->Position());
				VECTOR player_vec;
				float dot = 0.0f;
				eDirection direction = eDIRECTION_FRONT;
				cam_vec.y = 0;
				cam_vec = VNorm(cam_vec);
				player_vec.x = sin(mRotation.y + Utility::DegToRad(180.0f));
				player_vec.y = 0.0f;
				player_vec.z = cos(mRotation.y + Utility::DegToRad(180.0f));
				dot = VDot(cam_vec, player_vec);

				if (dot >= 0.7f) {
					direction = eDIRECTION_FRONT;
				} else if (dot <= -0.7f) {
					direction = eDIRECTION_BACK;
				} else {
					player_vec.x = sin(mRotation.y + Utility::DegToRad(90.0f));
					player_vec.z = cos(mRotation.y + Utility::DegToRad(90.0f));
					dot = VDot(cam_vec, player_vec);

					if (dot > 0.5f) {
						direction = eDIRECTION_RIGHT;
					} else {
						direction = eDIRECTION_LEFT;
					}
				}

				if (CHECK_PRESS_JOY(JOY_INPUT_UP)) {
					ChangeState(sAwayJoyUpTable[direction]);
				} else if (CHECK_PRESS_JOY(JOY_INPUT_DOWN)) {
					ChangeState(sAwayJoyDownTable[direction]);
				} else if (CHECK_PRESS_JOY(JOY_INPUT_RIGHT)) {
					ChangeState(sAwayJoyRightTable[direction]);
				} else if (CHECK_PRESS_JOY(JOY_INPUT_LEFT)) {
					ChangeState(sAwayJoyLeftTable[direction]);
				}
			}

			// ジャンプ回避
			if (CHECK_TRIG_JOY(JOY_INPUT_JUMP)) {
				mJumpMoveVec.mDash = ZERO_VEC;
				ChangeState(eCHARA_STATE_JUMP_UP);
				
				if (mpLockonChara != NULL) {
					CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_LOCKON);
				}
			}
			
		}
	}
	
	VECTOR prev_pos_tmp = mPrevPosition;
	
	CharaBase::Update();
	
#ifdef _DEBUG
	if (CHECK_PRESS_KEY(DBG_KEY_WARP_LASTPOS)) {
		mGravity = 0.0f;
		mPosition = mLastEarthPos;
		mGravity = 0.0f;
		mMoveVec = ZERO_VEC;
	}
#endif // _DEBUG
	
	if (mIsPermitInput == true) {
		// 水の上にいるかの処理
		updateWaterCollision();

		// mLastEarthPos更新処理		
		updateLastEarthPos();

		// 翼ジャンプの処理
		updateWingJump();

		// ピヨピヨゲージの処理
		if (mPiyoPiyoValue > 0) {
			addPiyoPiyoGauge(5);
		}
		float piyopiyo_rate = (float)mPiyoPiyoValue / (float)kPIYO_PIYO_GAUGE_MAX;
		float magic_rate	= (float)mMagicPoint / 100.0f;
		TotalInfo::GetInstance()->UpdatePiyoPiyoGaugeByRate(piyopiyo_rate, magic_rate);

		// アクションタスク(イベント)の更新の判定
		VECTOR vec_start, vec_end;
		GimmickBase* p_gimmick = NULL;
		p_gimmick = (GimmickBase *)GimmickBase::GetGimmickBaseFromEarthCollIndex(mOnCollisionIndex);
		if (p_gimmick != NULL) {
			vec_start	= prev_pos_tmp;
			vec_end		= VAdd(mPosition, VGet(0.0f, 10.0f, 0.0f));
		}
		else {
			vec_start	= VAdd(mPosition, VGet(0.0f, 10.0f, 0.0f));
			vec_end		= VAdd(mPosition, mMoveVec);
			vec_end.y += -mBodySize;
		}
		ActionTaskManager::GetInstance()->UpdateActionTask(&vec_start, &vec_end);
	}	

#ifdef _DEBUG
	PRINT_SCREEN(GetColor(255, 255, 0), "CharaPlayer::mCharaState : %d", mCharaState);
	PRINT_SCREEN(GetColor(255, 255, 0), "CharaPlayer::Position : %04.2f, %04.2f, %04.2f", mPosition.x, mPosition.y, mPosition.z);
	PRINT_SCREEN(GetColor(255, 255, 0), "CharaPlayer::mEarthMaterialIndex : %d", mEarthMaterialIndex);

	if (CHECK_TRIG_KEY(DBG_KEY_CURE_PLAYER)) {
		mHitpoint = mMaxHitpoint;
		
		AddNowMagicPoint(9999);
		
		ChangeState(eCHARA_STATE_IDLE);
		// テスト
		ModelPmd::tAttachWork* p_attach = GetAttachAddr(eATTACH_MODEL_SUB_WEAPON);
		ModelPmd* p_pmd = (ModelPmd *)p_attach->mpAttachedModel;
		int motion_num = MV1GetAnimNum(p_pmd->GetModelHandle());
		float blend_rate;
		for (int iI = 0; iI < motion_num; iI ++) {
			blend_rate = MV1GetAttachAnimBlendRate(p_pmd->GetModelHandle(), iI);
			PRINT_CONSOLE("blend[%d] : %.2f\n", iI, blend_rate);
		}			
	}

	if (CHECK_PRESS_KEY(DBG_KEY_SKYAWAY)) {
		TotalInfo::GetInstance()->AddMoney(10);

		mGravity = 0.0f;
		mPosition.y += (100.0f * ALL_SCALE);
	}

	PRINT_SCREEN(COLOR_RED, "mJumpMoveVec.mDash = %.2f, %.2f, %.2f.", 
				 mJumpMoveVec.mDash.x,
				 mJumpMoveVec.mDash.y,
				 mJumpMoveVec.mDash.z);

#endif // _DEBUG	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::ChangeState(int setState)
{
	if (setState == mCharaState) return;
	if (mIsPermitInput == false) return;

	APP_ASSERT_PRINT((setState > -1) && (setState < eCHARA_STATE_MAX),
					 "Select setState %d is Invalid Value\n", setState);

	int prev_state = mCharaState;
	mCharaState = setState;
	(this->*mPhaseLeaveState[prev_state])();

	(this->*mPhaseEnterState[mCharaState])();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::UpdateAttackChara(void)
{
	switch (mCharaState) {
	case eCHARA_STATE_ATTACK_ROOLING:
		updateAttackRolling();
		break;
	case eCHARA_STATE_ATTACK_JUMP:
		updateAttackJump();
		break;
	case eCHARA_STATE_ATTACK_ROLING_KICK:
		updateAttackRollingKick();
		break;

	default:
		updateAttackSmallEnemy();
		break;
	};
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::updateAttackSmallEnemy(void)
{
	ModelCollision* p_mine = this;
	ModelCollision* p_your = NULL;

	VECTOR weapon_btm_pos, weapon_top_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);
	weapon_btm_pos.x = this->mPosition.x;		// 近すぎるとあたらない為
	weapon_btm_pos.z = this->mPosition.z;		// 近すぎるとあたらない為

	CharaEnemy::tCheckDamageWork work;
	work.mPlayerPos = p_mine->Position();
	work.mWeaponTopPos = weapon_top_pos;
	work.mWeaponBtmPos = weapon_btm_pos;

	for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
		p_your = (ModelCollision *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
		if (p_mine == p_your) continue;			// 自分はのぞく
		if (p_your->GetIsRetire()) continue;	// 戦闘不能者も除く
		if (p_your->IsDamage()) continue;		// 既に攻撃された者も除く

		if (p_your->CheckDamageCollNormal(&work)) {
			SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);

			VECTOR damage_down_vec;
			damage_down_vec = VSub(p_your->Position(), mPosition);
			damage_down_vec = VNorm(damage_down_vec);

			switch (mCharaState) {
			case eCHARA_STATE_ATTACK_4:
			case eCHARA_STATE_ATTACK_THRUST:
				// アタック４の場合は吹っ飛ばす
				p_your->ProcessDamage(mAttackPower, false, &weapon_top_pos, &damage_down_vec, true, eATTACK_TYPE_NORMAL);
				break;

				// 背面斬りの場合
			case eCHARA_STATE_ATTACK_REAR:
				{
					EFK_HANDLE efk_handle = NULL;
					efk_handle =  EffekseerManager::GetInstance()->Play(eEFK_SHOCK, &mPosition, 15.0f);
					EffekseerManager::GetInstance()->SetSpeed(efk_handle, 1.2f);
					efk_handle = EffekseerManager::GetInstance()->Play(eEFK_BREAK_BARRIER, &weapon_top_pos, 6.0f);
					EffekseerManager::GetInstance()->SetSpeed(efk_handle, 2.0f);

					p_your->ProcessDamage(mAttackPower, false, &weapon_top_pos, &damage_down_vec, true, eATTACK_TYPE_NORMAL);
				}
				break;

			case eCHARA_STATE_ATTACK_SMASH_DOING:
			case eCHARA_STATE_ATTACK_SMASH_END:
				{
					// スマッシュ斬りの場合
					EFK_HANDLE efk_handle = NULL;
					efk_handle =  EffekseerManager::GetInstance()->Play(eEFK_SHOCK, &mPosition, 15.0f);
					EffekseerManager::GetInstance()->SetSpeed(efk_handle, 1.2f);
					efk_handle = EffekseerManager::GetInstance()->Play(eEFK_BREAK_BARRIER, &weapon_top_pos, 6.0f);
					EffekseerManager::GetInstance()->SetSpeed(efk_handle, 2.0f);
					p_your->ProcessDamage(mAttackPower, false, &weapon_top_pos, &damage_down_vec, true, eATTACK_TYPE_NORMAL);
				}
				break;

			default:
				// 通常のダメージ
				p_your->ProcessDamage(mAttackPower, false, &weapon_top_pos, &damage_down_vec, false, eATTACK_TYPE_NORMAL);
				break;

			};

			CameraManager::GetInstance()->StartVibeCamera(0.1f, 2.2f, 0.6f);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::updateAttackRolling(void)
{
	ModelCollision* p_mine = this;
	ModelCollision* p_your = NULL;
	VECTOR weapon_btm_pos, weapon_top_pos;
	VECTOR vec;
	float weaponsize;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);
	weapon_btm_pos.x = this->mPosition.x;		// 近すぎるとあたらない為
	weapon_btm_pos.z = this->mPosition.z;		// 近すぎるとあたらない為
	vec = VSub(weapon_top_pos, weapon_btm_pos);
	weaponsize = VSize(vec);

	for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
		p_your = (ModelCollision *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
		if (p_mine == p_your) continue;			// 自分はのぞく
		if (p_your->GetIsRetire()) continue;	// 戦闘不能者も除く
		if (p_your->IsDamage()) continue;		// 既に攻撃された者も除く

		tCheckDamageWork work = {p_mine->Position(), weapon_top_pos, weapon_btm_pos, weaponsize};

		if (p_your->CheckDamageCollRolling(&work)) {
			SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);

			VECTOR damage_down_vec;
			damage_down_vec = VSub(p_your->Position(), mPosition);
			damage_down_vec = VNorm(damage_down_vec);
			p_your->ProcessDamage(mAttackPower, false, &weapon_top_pos, &damage_down_vec, true, eATTACK_TYPE_ROLLING);

			CameraManager::GetInstance()->StartVibeCamera(0.8f, 1.5f, 0.6f);
		}
	}

	// 剣での攻撃も実施
	updateAttackSmallEnemy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::updateAttackRollingKick(void)
{
	ModelCollision* p_mine = this;
	ModelCollision* p_your = NULL;

	ShapeCapsule capsuleA, capsuleB;
	GetFramePosFromName(&capsuleA.mPosA, "右つま先ＩＫ");
	GetFramePosFromName(&capsuleA.mPosB, "右ひざ");
	capsuleA.mRadius = 52.0f;

	for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
		p_your = (ModelCollision *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
		if (p_mine == p_your) continue;			// 自分はのぞく
		if (p_your->GetIsRetire()) continue;	// 戦闘不能者も除く
		if (p_your->IsDamage()) continue;		// 既に攻撃された者も除く

		p_your->GetBodyCapsule(&capsuleB);

		if (CollisionManager::Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {

			SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);

			VECTOR damage_down_vec;
			damage_down_vec = VSub(p_your->Position(), mPosition);
			damage_down_vec = VNorm(damage_down_vec);

			// 通常のダメージ
			p_your->ProcessDamage(mAttackPower, false, &capsuleA.mPosA, &damage_down_vec, false, eATTACK_TYPE_NORMAL);

			CameraManager::GetInstance()->StartVibeCamera(0.1f, 2.2f, 0.6f);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::updateAttackJump(void)
{
	ModelCollision* p_mine = this;
	ModelCollision* p_your = NULL;

	VECTOR weapon_btm_pos, weapon_top_pos;
	GetFramePosFromName(&weapon_btm_pos, "右足");
	GetFramePosFromName(&weapon_top_pos, "右つま先");
	
	CharaEnemy::tCheckDamageWork work;
	work.mPlayerPos = p_mine->Position();
	work.mWeaponTopPos = weapon_top_pos;
	work.mWeaponBtmPos = weapon_btm_pos;

	for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
		p_your = (ModelCollision *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
		if (p_mine == p_your) continue;			// 自分はのぞく
		if (p_your->GetIsRetire()) continue;	// 戦闘不能者も除く
		if (p_your->IsDamage()) continue;		// 既に攻撃された者も除く

		if (p_your->CheckDamageCollJump(&work)) {

			mIsJumpAttack = false;

			SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);

			VECTOR damage_down_vec;
			damage_down_vec = VSub(p_your->Position(), mPosition);
			damage_down_vec = VNorm(damage_down_vec);
			if ((mCharaState == eCHARA_STATE_ATTACK_4) ||
				(mCharaState == eCHARA_STATE_ATTACK_THRUST)) {
				// アタック４の場合は吹っ飛ばす
				p_your->ProcessDamage(mAttackPower, false, &weapon_top_pos, &damage_down_vec, true, eATTACK_TYPE_JUMP);
			} else {
				// 通常のダメージ
				p_your->ProcessDamage(mAttackPower, false, &weapon_top_pos, &damage_down_vec, false, eATTACK_TYPE_JUMP);
			}
			CameraManager::GetInstance()->StartVibeCamera(0.1f, 2.2f, 0.6f);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::updateWaterCollision(void)
{
	VECTOR vec_start, vec_end, hitpos, normal;

	// 少しだけ下げる
	float check_height = (mBodyHeight * 0.9f);

	vec_end		= mPosition;
	vec_start	= vec_end;
	vec_start.y += (check_height * 2.0f);

	mIsSoakWater = FALSE;

	// 水モデルと線分の当たり判定
	if (CollisionManager::GetInstance()->CheckWaterCollision(vec_start, vec_end, &hitpos, &normal)) {
		float dist;
		dist = VSize(VSub(vec_end, hitpos));
		PRINT_SCREEN(GetColor(255, 255, 0), "dist : %.2f\n", dist);

		if (dist > (check_height * 0.6f * ALL_SCALE)) {
			// 水に体が浸かっている
			mIsSoakWater = TRUE;
		} 

		// 溺れチェック
		if (dist > (check_height * ALL_SCALE)) {
			// 溺れる
			ActionTaskManager::tTaskInfo info;
			info.mTaskType = ActionTaskManager::eTASK_FALLDOWN;
			info.mTaskValueA = 0;
			info.mTaskValueB = 0;
			ActionTaskManager::GetInstance()->NotifyAction(&info);
		}
		sFootstepKind = eFOOTSTEP_KIND_WATER;
	}
	else {
		sFootstepKind = eFOOTSTEP_KIND_DEFAULT;
	}
}

//---------------------------------------------------
/**
 * ジョイパッドとマウスの両方から武器のターゲットの向きを求める
 */
//---------------------------------------------------
void
CharaPlayer::updateWeaponTargetVecFromInput(VECTOR* pTargetVec, float scaleV, float scaleH,  bool isOnMouseMiddle/*=false*/)
{
	VECTOR vecZ, crossX, crossY, addVec;
	addVec = ZERO_VEC;
	vecZ = VNorm(*pTargetVec);

	crossX = VNorm(VCross(vecZ, VGet(0.0f, 1.0f, 0.0f)));
	crossY = VNorm(VCross(vecZ, crossX));
		
	int joy_on_num = 0;
	if (CHECK_PRESS_JOY(JOY_INPUT_RIGHT)) {
		addVec = VScale(crossX, (-1 * scaleH));
		joy_on_num ++;
	} else if (CHECK_PRESS_JOY(JOY_INPUT_LEFT)) {
		addVec = VScale(crossX, (1 * scaleH));
		joy_on_num ++;
	}
	if (CHECK_PRESS_JOY(JOY_INPUT_DOWN)) {
		addVec = VAdd(addVec, VScale(crossY, (-1 * scaleV)));
		joy_on_num ++;
	} else if (CHECK_PRESS_JOY(JOY_INPUT_UP)) {
		addVec = VAdd(addVec, VScale(crossY, (1 * scaleV)));
		joy_on_num ++;
	}
	if (joy_on_num == 2) {
		addVec = VScale(addVec, 0.5f);
	}

	if (VSize(addVec) > 0.0f) {
		if (sIsAnalogInputRun == true) {
			*pTargetVec = VAdd(*pTargetVec, VScale(addVec, 0.025f));
		} else {
			*pTargetVec = VAdd(*pTargetVec, VScale(addVec, 0.01f));
		}
	}

	// マウスでの操作
	if ((CHECK_PRESS_MOUSE(MOUSE_INPUT_MIDDLE)) || 
		(isOnMouseMiddle == true && CHECK_PRESS_MOUSE(MOUSE_INPUT_RIGHT) == true)) {
		addVec = ZERO_VEC;
		int mouse_x, mouse_y;
		InputManager::GetInstance()->GetMouseMoveValue(&mouse_x, &mouse_y);
		addVec = VAdd(addVec, VScale(VScale(crossX, (float)-mouse_x), 0.003f * scaleH));
		addVec = VAdd(addVec, VScale(VScale(crossY, (float)-mouse_y), 0.003f * scaleV));
		if (VSize(addVec) > 0.0f) {
			*pTargetVec = VAdd(*pTargetVec, addVec);
		}
	}
}

//---------------------------------------------------
/**
   未使用の弓矢のポインタを取得する
 */
//---------------------------------------------------
ObjectWeapon_Arrow* 
CharaPlayer::getUnusedArrowPtr(void)
{
	return &sPlayerArrow[sUsePlayerArrowIndex];
}

//---------------------------------------------------
/**
 * 弓の弦とプレイヤーの指をアタッチさせる
 */
//---------------------------------------------------
void
CharaPlayer::updateAttachArcheryStringToPlayerFinger(ModelPmd* pArchery)
{
	VECTOR finger_pos, bow_pos;
	this->GetFramePosFromName(&finger_pos, "左中指１");
	finger_pos.y -= 3.0f;		// 補正
	pArchery->GetFramePosFromName(&bow_pos, "弓中央");

	VECTOR moved_vec;
	GetMovedVec(&moved_vec);
	bow_pos = VAdd(bow_pos, moved_vec);

	pArchery->SetFrameUserMatrix("弦", &finger_pos, &finger_pos, &bow_pos, &mScale);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::processWingJump(void)
{
	this->ResumeAttach(eATTACH_MODEL_ANGEL_WING);
	GraphicManager::GetInstance()->ReserveEntryModel(&sAngelWing);
 
	VECTOR eff_pos;
	this->GetFramePosFromName(&eff_pos, "リュック");
	mEfkHnd.uAngelWing = EffekseerManager::GetInstance()->Play(eEFK_BOSS_DEATH, &eff_pos, 6.5f, 16.0f);
	EffekseerManager::GetInstance()->SetSpeed(mEfkHnd.uAngelWing, 1.2f);
	sAngelWing.SetAlpha(0.1f);
	sAngelWing.Scale() = VGet(0.1f, 0.1f, 0.1f);
	sAngelWing.StartMotion(1, true, 0.5f);

	SoundManager::GetSndComponent()->PlaySound(eSE_WINGS);

	mWingJumpState = eWING_JUMP_STATE_APPEAR;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::updateWingJump(void)
{
	switch (mWingJumpState) {
	case eWING_JUMP_STATE_NONE:
		/* nop */
		break;
		
	case eWING_JUMP_STATE_APPEAR:
		{
			// 羽エフェクトを追従
		   VECTOR eff_pos;
		   this->GetFramePosFromName(&eff_pos, "リュック");
		   EffekseerManager::GetInstance()->SetPosition(mEfkHnd.uAngelWing, &eff_pos);

		   // 翼モデルをフェードイン
		   float alpha = sAngelWing.GetAlpha();
		   alpha *= 1.1f;
		   sAngelWing.SetAlpha(alpha);
		   sAngelWing.Scale() = VGet(alpha, alpha, alpha);

		   // 着地した場合はフェードアウトさせる
		   if (mIsLand) {
			   sAngelWing.StartMotion(0, true, 0.5f, NULL, 0.03f);

			   STOP_EFFEKSEER(mEfkHnd.uAngelWing, 0.1f);

			   mWingJumpState = eWING_JUMP_STATE_DISAPPEAR;
		   }
		}
		break;

	case eWING_JUMP_STATE_DISAPPEAR:
		{
		   // 翼モデルをフェードアウト
		   float alpha = sAngelWing.GetAlpha();
		   alpha *= 0.93f;
		   sAngelWing.SetAlpha(alpha);
		   sAngelWing.Scale() = VGet(alpha, alpha, alpha);

		   if (alpha < 0.1f) {
			   this->PauseAttach(eATTACH_MODEL_ANGEL_WING);
			   GraphicManager::GetInstance()->ReserveRemoveModel(&sAngelWing);
			   mWingJumpState = eWING_JUMP_STATE_NONE;
		   }
		}
		break;

	default:
		break;
	};
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::updateHookshotAttach(ModelPmd* pHookshotMdl)
{
	VECTOR head, bottom;
	sHookshotChain.GetFramePosFromName(&head, strHookShotHead);
	sHookshotChain.GetFramePosFromName(&bottom, strHookShotBottom);

	float stretch_rate = ((VSize(VSub(head, bottom))) / kHOOKSHOT_LENGTH);	// MAX伸び率を求める

	MV1SetFrameTextureAddressTransform(sHookshotChain.GetModelHandle(),
									   0,
									   0.0f, 0.0f,
									   1.0f, 
									   ((float)kHOOKSHOT_UV_MAX * stretch_rate),
									   0.0f, 0.0f, 0.0f);
	
	sHookshotChain.Matrix() = pHookshotMdl->Matrix();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::changeStateFunc(int charaState, PHASE_STATE phaseEnter, PHASE_STATE phaseUpdate, PHASE_STATE phaseLeave)
{
	mPhaseEnterState[charaState]	= phaseEnter;
	mPhaseState[charaState]			= phaseUpdate;
	mPhaseLeaveState[charaState]	= phaseLeave;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CharaPlayer::checkShieldHit(VECTOR* pAttackDirVec)
{
	if (mCharaState == CharaPlayer::eCHARA_STATE_LIB_SHIELD) {

		float dot;
		VECTOR tmp;
		tmp.x = sin(mRotation.y + Utility::DegToRad(180.0f));
		tmp.y = 0.0f;
		tmp.z = cos(mRotation.y + Utility::DegToRad(180.0f));
		dot = VDot(VNorm(*pAttackDirVec), tmp);
		
		if (dot < 0.3f) {
			return true;
		}
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::updateLastEarthPos(void)
{
	// ラインの上に乗っている場合、かつ、ギミックに乗っていない、
	// かつ、入力許可状態でかつ、水に浸かっていない場合
	if ((mEarthState == eEARTH_STATE_LINE)	&&
		(mIsOnGimmick == FALSE)				&&
		(mIsPermitInput == true)			&&
		(mIsSoakWater == FALSE)) {

		// mLastEarthPosを更新
		mLastEarthPos = mPosition;

		PRINT_SCREEN(COLOR_WHITE, "!!!! CharaPlayer::updateLastEarthPos() ");
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::processCancelArchery(void)
{
	if ((mItemWork.mItemSubWeapon != eITEM_KIND_ARCHERY) ||
		(mCosplayIndex == eWINMENU_COSPLAY_TITANIS) ||
		(mCosplayIndex == eWINMENU_COSPLAY_GUARDIAN)
		) {
		return;
	}

	// 回避ボタンで終了
	TotalInfo::GetInstance()->DisableLockonLayout();
		
	ModelPmd::tAttachWork* p_attach = GetAttachAddr(eATTACH_MODEL_SUB_WEAPON);
	ModelPmd* p_pmd = (ModelPmd *)p_attach->mpAttachedModel;
	this->ResetFrameUserMatrix("上半身");
	p_pmd->ResetFrameUserMatrix("弦");
	p_pmd->StartMotion(0, false, 1.0f);
	// モデルの挿入先を派生クラスで設定させる
	ObjectWeapon_Arrow* p_arrow = getUnusedArrowPtr();
	GraphicManager::GetInstance()->ReserveRemoveModel(p_arrow);

	mIsReserveComboInput = false;

	sNavi.EnableNaviCursor();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectWeapon_Arrow*
CharaPlayer::GetArrowPtr(int index)
{
	APP_ASSERT_PRINT((index < kPLAYER_ARROW_NUM),
					 "Invalid arow index %d \n", index);
	return &sPlayerArrow[index];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::SetupAttachAngelWing(void)
{
	int node_index = this->GetFrameIndexFromName("リュック");
	this->AttachModel(&sAngelWing, ModelPmd::eATTACHED_MODEL_MOTION, node_index);
	this->PauseAttach(eATTACH_MODEL_ANGEL_WING);

	sAngelWing.StartMotion(0, true, 0.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::RevertCosplay(void)
{
	mCosplayIndex = eWINMENU_COSPLAY_NONE;

	DisableLockonChara();
	TotalInfo::GetInstance()->DisableLockonLayout();

	// 直前のLeave関数を明示的にコールする
	(this->*mPhaseLeaveState[mCharaState])();

	// モデルファイル読み込みの処理
	DeleteModel();
	LinkModel(Name_Lino);
	Setup(false, SceneMainGame::GetStageIndex());
	
	ResumeAttach(CharaPlayer::eATTACH_MODEL_SWORD);
	SetActionLimit(SceneMainGame::GetStageActionLimit(),
				   ItemWork().mItemSubWeapon);

	// 現在装備しているサブウェポンを設定させる
	changeSubWeapon(ItemWork().mItemSubWeapon);

	ChangeAttachPattern(eATTACH_PATTERN_NORMAL);

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
BYTE&
CharaPlayer::CosplayIndex(void)
{
	return mCosplayIndex; 
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
CharaPlayer::GetFootstepKind(void)
{
	return sFootstepKind;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::addPiyoPiyoGauge(SHORT addValue)
{
	if (addValue > 0) {
		mPiyoPiyoValue += addValue;
		if (mPiyoPiyoValue > kPIYO_PIYO_GAUGE_MAX) {
			mPiyoPiyoValue = kPIYO_PIYO_GAUGE_MAX;
		}
	}
	else {
		// ヒヨコのピアスを持っている場合
		if (TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_CHICK_PIERCE)) {
			addValue = ( (addValue * 3) / 4);
		}
		mPiyoPiyoValue += addValue;
		if (mPiyoPiyoValue < 0) {
			mPiyoPiyoValue = 0;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::GetMovedVec(VECTOR* pMovedVec)
{
	*pMovedVec = VSub(mPosition, mPrevPosition);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
BOOL
CharaPlayer::CheckEnableMagic(eItemKind magicItem)
{
	BOOL is_enable = FALSE;

	switch (magicItem) {
	case eITEM_KIND_MAGIC_THUNDER:
		if (mMagicPoint > kMP_CONSUME_THUNDER) {
			is_enable = TRUE;
		}
		break;
	case eITEM_KIND_MAGIC_FLAME:
		if (mMagicPoint > kMP_CONSUME_FLAME) {
			is_enable = TRUE;
		}
		break;
	case eITEM_KIND_MAGIC_TORNADE:
		if (mMagicPoint > kMP_CONSUME_TORNADE) {
			is_enable = TRUE;
		}
		break;
	default:
		break;
	};

	return is_enable;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::AddNowMagicPoint(SHORT addValue)
{
	SHORT temp_mp = (SHORT)mMagicPoint;

	// 回復の場合
	if (addValue > 0) {
		temp_mp += addValue;
		if (temp_mp > 100) {
			temp_mp = 100;
		}
	}
	// 消費の場合
	else {
		// 妖精の腕輪を持っている場合
		if (TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_FAIRY_BRACELET)) {
			addValue /= 2;
		}
		temp_mp += addValue;
		if (temp_mp < 0) {
			temp_mp = 0;
		}
	}

	mMagicPoint = (BYTE)temp_mp;
}

//---------------------------------------------------
/**
 // アタッチパターンを変更
 */
//---------------------------------------------------
void
CharaPlayer::ChangeAttachPattern(eAttachPattern pattern)
{
	PRINT_CONSOLE("Trace : CharaPlayer::ChangeAttachPattern() : %d \n", pattern);

	ModelPmd* p_sword		= (ModelPmd *)GetAttachAddr(eATTACH_MODEL_SWORD)->mpAttachedModel;
	ModelPmd* p_shield		= (ModelPmd *)GetAttachAddr(eATTACH_MODEL_SHIELD)->mpAttachedModel;
	ModelPmd* p_subweapon	= (ModelPmd *)GetAttachAddr(eATTACH_MODEL_SUB_WEAPON)->mpAttachedModel;
	int shield_frame = -1;
	

	BOOL is_sword_equip		= FALSE;
	BOOL is_shield_equip	= FALSE;

	switch (pattern) {
	case eATTACH_PATTERN_NORMAL:		//	両手空き・剣と盾
		is_sword_equip = TRUE;
		is_shield_equip= TRUE;

		if (IsSubWeaponUsing() == false) {
			p_subweapon->SetVisible(false);
			p_subweapon->StartMotion(0, true, 0.5f);
			ResumeAttach(eATTACH_MODEL_SUB_WEAPON);
		}
		break;

	case eATTACH_PATTERN_BOOMERANG:		//	ブーメラン
		is_sword_equip = TRUE;
		is_shield_equip= FALSE;

		p_subweapon->SetVisible(true);
		p_subweapon->StartMotion(0, true, 0.5f);
		ResumeAttach(eATTACH_MODEL_SUB_WEAPON);
		break;

	case eATTACH_PATTERN_RAISE_HAND:	// 物を持っている
		is_sword_equip = FALSE;
		is_shield_equip= FALSE;

		p_subweapon->SetVisible(false);

		break;

	case eATTACH_PATTERN_ARCHERY:		//	弓
		is_sword_equip = FALSE;
		is_shield_equip= FALSE;

		p_subweapon->SetVisible(true);
		p_subweapon->StartMotion(0, false, 1.0f);
		p_subweapon->ResetFrameUserMatrix("弦");
		ResumeAttach(eATTACH_MODEL_SUB_WEAPON);
		break;

	case eATTACH_PATTERN_HOOKSHOT:		//	フックショット
		is_sword_equip = TRUE;
		is_shield_equip= FALSE;

		p_subweapon->SetVisible(true);
		p_subweapon->StartMotion(0, false, 0.5f);
		MV1SetMeshVisible(p_subweapon->GetModelHandle(), 4, FALSE);

		break;

	case eATTACH_PATTERN_SWORD:			//	剣のみ(魔法)
		is_sword_equip = TRUE;
		is_shield_equip= FALSE;

		p_subweapon->SetVisible(false);
		break;

	case eATTACH_PATTERN_HAND:			//	素手
		is_sword_equip = FALSE;
		is_shield_equip= FALSE;

		p_subweapon->SetVisible(false);
		break;

	case eATTACH_PATTERN_OCARINA:		//	オカリナ
		is_sword_equip = FALSE;
		is_shield_equip= FALSE;

		PauseAttach(eATTACH_MODEL_SUB_WEAPON);
		p_subweapon->SetVisible(true);
		break;

	case eATTACH_PATTERN_COPY_ROD:		//	コピーロッド
		is_sword_equip = FALSE;
		is_shield_equip= FALSE;

		p_subweapon->SetVisible(true);
		//		p_subweapon->StartMotion(0, true, 0.5f);
		ResumeAttach(eATTACH_MODEL_SUB_WEAPON);
		break;

	default:
		break;
	}

	// コスプレ時の対応
	if (IS_COSPLAY) {
		p_sword->SetVisible(false);
		p_shield->SetVisible(false);
	}
	// コスプレ以外
	else {

		// 剣の処理
		if (TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_MASTERSWORD)) {
			if ((is_sword_equip) && (mActionLimit == eACTION_OK)) {
				// 剣を左手に持っているパターン
				p_sword->SetVisible(true);
				MV1SetMeshVisible(mModelHandle, kSHEATH_FRAME_INDEX, TRUE);
				MV1SetMeshVisible(mModelHandle, kHILT_FRAME_INDEX, FALSE);		// 剣の柄を非表示
			}
			else {
				// 剣を鞘にしまっているケース
				p_sword->SetVisible(false);
				MV1SetMeshVisible(mModelHandle, kSHEATH_FRAME_INDEX, TRUE);
				MV1SetMeshVisible(mModelHandle, kHILT_FRAME_INDEX, TRUE);	// 剣の柄を表示
			}
		}
		else {
			// 剣そのものを持っていないケース
			p_sword->SetVisible(false);
			MV1SetMeshVisible(mModelHandle, kSHEATH_FRAME_INDEX, FALSE);
			MV1SetMeshVisible(mModelHandle, kHILT_FRAME_INDEX, FALSE);
		}

		// 盾の処理
		if ((is_shield_equip) && (mActionLimit == eACTION_OK) && (TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_LIB_SHIELD))) {
			// 盾を右手に持っているケース
			p_shield->SetVisible(true);
		}
		else {
			// 盾を持っていないケース
			p_shield->SetVisible(false);
		}

	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::UnvisibleWeaponArrow(void)
{
	for (UCHAR iI = 0; iI < kPLAYER_ARROW_NUM; iI ++) {
		GraphicManager::GetInstance()->ReserveRemoveModel(&sPlayerArrow[iI]);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::SetUsingRecoverItemKind(eItemKind itemKind)
{
	// 使用中回復アイテムフラグを更新
	mUsingRecoverItemKind = itemKind;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::ProcessFallDown(float addHeight)
{
	mGravity = 0.0f;

	SetMatrixType(eMATRIX_TYPE_TRS);
	mPosition		= mLastEarthPos;
	mPrevPosition	= mPosition;

	mGravity = 0.0f;
	mMoveVec = ZERO_VEC;
	mJumpMoveVec.mDash = ZERO_VEC;
	mOnCollisionIndex = -1;

	mHitpoint += -2; // ダメージ２

	ChangeState(eCHARA_STATE_IDLE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::ProcessRecovery(void)
{
	mGravity = 0.0f;
	SetMatrixType(ModelBase::eMATRIX_TYPE_TRS);
	mMoveVec = ZERO_VEC;
	mJumpMoveVec.mDash = ZERO_VEC;
	mOnCollisionIndex = -1;

	/* エフェクトの削除 */
	if (sEffWindIndex != -1) {
		EffectManager::GetInstance()->Stop(sEffWindIndex);
		sEffWindIndex = -1;
	}
	mBladeEffect.Stop();

	// アタッチ関連のリセット
	ModelPmd::tAttachWork* p_attach = GetAttachAddr(eATTACH_MODEL_SUB_WEAPON);
	if (p_attach->mAttachedModelType == ModelPmd::eATTACHED_MODEL_MOTION) {
		ModelPmd* p_pmd = (ModelPmd *)p_attach->mpAttachedModel;
		p_pmd->StartMotion(0, false, 0.0f, NULL, kHOOK_REVERSE_SPEED);
	}
	
	// 
	if (mpSubweaponFunc == &CharaPlayer::subweaponFuncBoomerang) {
		ModelPmd::tAttachWork* p_attach = GetAttachAddr(eATTACH_MODEL_SUB_WEAPON);
		ModelPmd* p_pmd = (ModelPmd *)p_attach->mpAttachedModel;
		p_pmd->Position() = mPosition;
		sBoomerang.mMoveVec = ZERO_VEC;
		sBoomerang.mState = eBOOMERANG_STATE_REVERSE_NO_DAMAGE;
		PRINT_CONSOLE("Line : %d \n", __LINE__);
	}
	
	TotalInfo::GetInstance()->DisableLockonLayout();

	// 影を有効に
	this->EnableShadow();

	// ラスト地面座標を更新
	mLastEarthPos = mPosition;

	TotalInfo::GetInstance()->AddMoney(-10);

	ChangeState(eCHARA_STATE_IDLE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::PermitAcceptInput(bool isPermit)
{
	if (isPermit == true) {
		if (mIsPermitInput == false) {

			// 許可
			mIsPermitInput = true;
			ChangeState(eCHARA_STATE_AFTER_EVENT);
		}
	}
	else { /* if (isPermit == false) */
		if (mIsPermitInput == true) {

			mBladeEffect.Stop(); // ブレードエフェクト破棄
			// 禁止
			ChangeState(eCHARA_STATE_REFUSE_INPUT);
			mIsPermitInput = false;

			DisableLockonChara();
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::DisableLockonChara(void)
{
	if (mpLockonChara != NULL) {
		mpLockonChara = NULL;
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
		CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_LOW, true);
	}
}

//---------------------------------------------------
/**
 // ダメージを食らった時の処理
 */
//---------------------------------------------------
void
CharaPlayer::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	if ((attackType == eATTACK_TYPE_FLAME) ||
		(attackType == eATTACK_TYPE_FLAME_BREAK_GUARD) ||
		(attackType == eATTACK_TYPE_BOMB)) {
		// 炎の指輪を持っている場合
		if (TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_FLAME_RING)) {
			attackPower /= 2;
		}
	}

	// ガード時の処理
	if ((attackType != eATTACK_TYPE_BREAK_GUARD) &&
		(attackType != eATTACK_TYPE_FLAME_BREAK_GUARD)) {
		if (checkShieldHit(pDamageDownVec)) {
			VECTOR eff_pos;
			if (pEffPos == NULL) {
				GetFramePosFromName(&eff_pos, "左中指１");
			}
			else {
				eff_pos = *pEffPos;
			}
			EFK_HANDLE handle = EffekseerManager::GetInstance()->Play(eEFK_SHIELD, &eff_pos, &VScale(mScale, 0.8f));
			EffekseerManager::GetInstance()->SetRotationAxisY(handle, (mRotation.y + (DX_PI/2.0f)));

			SoundManager::GetSndComponent()->PlaySound(eSE_SHIELD);

			addPiyoPiyoGauge((-400 * attackPower));
			
			ChangeState(eCHARA_STATE_GUARD_STAGGER);
			return;
		}
	}

	if (IS_COSPLAY) {
		if (mCosplayIndex == eWINMENU_COSPLAY_TITANIS) {
			revertVehicle();
			RevertCosplay();
			EffekseerManager::GetInstance()->Play(eEFK_GETKEYITEM, &mPosition, 10.0f);
		}
		else if (mCosplayIndex == eWINMENU_COSPLAY_GUARDIAN) {

			// マジックアーマーは敵の攻撃を受け付けない
			VECTOR eff_pos;
			if (pEffPos == NULL)	GetFramePosFromName(&eff_pos, "左中指１");
			else					eff_pos = *pEffPos;
			EFK_HANDLE handle = EffekseerManager::GetInstance()->Play(eEFK_SHIELD, &eff_pos, &VScale(mScale, 1.2f));
			EffekseerManager::GetInstance()->SetRotationAxisY(handle, (mRotation.y + (DX_PI/2.0f)));
			SoundManager::GetSndComponent()->PlaySound(eSE_SHIELD);
			return;
		}
		else {
			RevertCosplay();
			EffekseerManager::GetInstance()->Play(eEFK_GETKEYITEM, &mPosition, 10.0f);
		}
	}
	PRINT_CONSOLE("mCharaState : %d \n", mCharaState);

	/* 画面の振動を処理 */
	if (pDamageDownVec != NULL) {
		CameraManager::GetInstance()->StartVibeCamera(0.7f, 2.4f, 0.8f);
	} else {
		CameraManager::GetInstance()->StartVibeCamera(0.2f, 3.2f, 0.8f);
	}
	CharaBase::ProcessDamage(attackPower, isSetMode, pEffPos, pDamageDownVec, isDamageDown, attackType);

	mIsReserveComboInput = false;
	mIsAttack = false;
	eCamState cam = CameraManager::GetInstance()->GetCameraState();
	if ((cam == eCAM_STATE_HOOKSHOT) ||
		(cam == eCAM_STATE_BOOMERANG) ){
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
	} else if (cam = eCAM_STATE_ARCHERY) {
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
		if (mpLockonChara != NULL) {
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_LOCKON);
		}
	}

	mIsSubAttack		= false;
	mIsJumpAttack		= false;

	/* エフェクトの削除 */
	InputManager::GetInstance()->StartVibe();
	if (sEffWindIndex != -1) {
		EffectManager::GetInstance()->Stop(sEffWindIndex);
		sEffWindIndex = -1;
	}
	mBladeEffect.Stop();

	ModelPmd::tAttachWork* p_attach = GetAttachAddr(eATTACH_MODEL_SUB_WEAPON);
	if (p_attach->mAttachedModelType == ModelPmd::eATTACHED_MODEL_MOTION) {
		ModelPmd* p_pmd = (ModelPmd *)p_attach->mpAttachedModel;
		p_pmd->StartMotion(0, false, 0.0f, NULL, 1.0f);
	}
	TotalInfo::GetInstance()->DisableLockonLayout();

	// 影を有効に
	this->EnableShadow();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::ProcessBurstJump(VECTOR* pJumpVec)
{
	SoundManager::GetSndComponent()->PlaySound(eSE_BOYOYON);
	StartMotionEx(&sMotionParam[eMT_JUMP_START]);

	mBladeEffect.Stop();

	mGravity += (kJUMP_GRAVITY * 20.0f);
	mConstGravitySpeed = kGRAVITY_SPEED;

	mTargetRotY = mRotation.y;

	if (mpObjectHand != NULL) {
		unprotectAllEnemyDamageFlag();
	}

	mJumpMoveVec.mDash.x = pJumpVec->x;
	mJumpMoveVec.mDash.y = 0.0f;
	mJumpMoveVec.mDash.z = pJumpVec->z;

	mIsLand = false;
	mEarthState = eEARTH_STATE_SKY;

	mCharaState = eCHARA_STATE_JUMP_UP;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::ProcessTechniqualJump(VECTOR* pJumpVec)
{
	mBladeEffect.Stop();

	mGravity += (kJUMP_GRAVITY * 5.0f);
	mConstGravitySpeed = kGRAVITY_SPEED;

	mTargetRotY = mRotation.y;

	if (mpObjectHand != NULL) {
		unprotectAllEnemyDamageFlag();
	}

	mJumpMoveVec.mDash.x = pJumpVec->x;
	mJumpMoveVec.mDash.y = 0.0f;
	mJumpMoveVec.mDash.z = pJumpVec->z;

	mIsLand = false;
	mEarthState = eEARTH_STATE_SKY;

	mCharaState = eCHARA_STATE_JUMP_UP;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::ProcessRideVehicle(void)
{
	mCosplayIndex = eWINMENU_COSPLAY_TITANIS;
	SceneMainGame::ChangePlayerModel(NAME_Titanis, FALSE);

	TotalInfo::GetInstance()->DisableLockonLayout();

	ModelPmd* p_attach_mdl = NULL;
	p_attach_mdl = (ModelPmd *)this->GetAttachAddr(eATTACH_MODEL_SUB_WEAPON)->mpAttachedModel;

	/* sGimmick オブジェクトの再設定 */
	p_attach_mdl->DeleteModel();
	p_attach_mdl->LinkModel(Name_Lino);
	p_attach_mdl->SetVisible(true);

	p_attach_mdl->Setup();
	p_attach_mdl->Scale() = CHARA_DEF_SCALE;
	p_attach_mdl->StartMotion(eMT_RIDE_VEHICLE, true, 0.5f);
	p_attach_mdl->EnableToonShader(0.04f, 0.015f);
	p_attach_mdl->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);
	
	GraphicManager::GetInstance()->EntryDrawModel(p_attach_mdl);	// 描画エントリー

	int node_index = this->GetFrameIndexFromName("乗り位置");
	/* サブウェポンをアタッチ */
	this->AttachModel(p_attach_mdl, 
					  ModelPmd::eATTACHED_MODEL_MOTION,
					  node_index);
	this->ChangeAttachModelFrameIndex(eATTACH_MODEL_SUB_WEAPON, node_index);
	
	ResumeAttach(eATTACH_MODEL_SUB_WEAPON);
	MV1SetVisible(p_attach_mdl->GetModelHandle(), TRUE);

	SoundManager::GetInstance()->PlayBGM(BGM_RIDE_VEHICLE);
	
	// 最大速度を上げる
	mMaxRunSpeed		= kCHARA_RUN_SPEED * 1.6f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::ProcessOperateMagicArmer(void)
{
	mCosplayIndex = eWINMENU_COSPLAY_GUARDIAN;
	SceneMainGame::ChangePlayerModel(NAME_Guardian, FALSE);
	StartMotion(CharaGuardian::eMT_SLEEP, true, 0.5f, NULL, 1.0f);

	MV1SetMaterialSphereMapBlendTypeAll(mModelHandle,
										DX_MATERIAL_BLENDTYPE_MODULATE); // 乗算
	for (int iI = 0; iI < (sizeof(kGUARDIAN_SPHERE_ADD_MATERIAL_TABLE) / sizeof(BYTE)); iI ++) {
		MV1SetMaterialSphereMapBlendType(mModelHandle,
										 kGUARDIAN_SPHERE_ADD_MATERIAL_TABLE[iI],
										 DX_MATERIAL_BLENDTYPE_NONE); // 加算
	}


	mBodySize *= 0.5f;

	TotalInfo::GetInstance()->DisableLockonLayout();

	PauseAttach(eATTACH_MODEL_SUB_WEAPON);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::SetActionLimit(const int actionLimit, eItemKind subWeaponKind/*=eITEM_KIND_NONE*/)
{
	PRINT_CONSOLE("Trace : CharaPlayer::SetActionLimit() \n");

	if (subWeaponKind == eITEM_KIND_NONE) {
		subWeaponKind = mItemWork.mItemSubWeapon;
	}

	changeStateFunc(eCHARA_STATE_IDLE,
					&CharaPlayer::phaseEnterStateIdle,	&CharaPlayer::phaseUpdateStateIdle, &CharaPlayer::phaseLeaveStateIdle);
	changeStateFunc(eCHARA_STATE_MOVE,
					&CharaPlayer::phaseEnterStateMove,	&CharaPlayer::phaseUpdateStateMove, &CharaPlayer::phaseLeaveStateMove);
	changeStateFunc(eCHARA_STATE_JUMP_UP,
					&CharaPlayer::phaseEnterStateJumpUp,	&CharaPlayer::phaseUpdateStateJumpUp, &CharaPlayer::phaseLeaveStateJumpUp);
	changeStateFunc(eCHARA_STATE_AWAY_FRONT, 
					&CharaPlayer::phaseEnterStateAwayFront,	&CharaPlayer::phaseUpdateStateAwayFront, &CharaPlayer::phaseLeaveStateAwayFront);
	changeStateFunc(eCHARA_STATE_AWAY_BACK, 
					&CharaPlayer::phaseEnterStateAwayBack,	&CharaPlayer::phaseUpdateStateAwayBack,  &CharaPlayer::phaseLeaveStateAwayBack);
	changeStateFunc(eCHARA_STATE_AWAY_RIGHT, 
					&CharaPlayer::phaseEnterStateAwayRight,	&CharaPlayer::phaseUpdateStateAwayRight, &CharaPlayer::phaseLeaveStateAwayRight);
	changeStateFunc(eCHARA_STATE_AWAY_LEFT, 
					&CharaPlayer::phaseEnterStateAwayLeft,	&CharaPlayer::phaseUpdateStateAwayLeft,  &CharaPlayer::phaseLeaveStateAwayLeft);

	switch (actionLimit) {
		/* □攻撃あり */
	case eACTION_OK:
		changeStateFunc(eCHARA_STATE_ATTACK_1,
						&CharaPlayer::phaseEnterStateAttack1,&CharaPlayer::phaseUpdateStateAttack1,  &CharaPlayer::phaseLeaveStateAttack1);
		changeStateFunc(eCHARA_STATE_ATTACK_2,
						&CharaPlayer::phaseEnterStateAttack2,&CharaPlayer::phaseUpdateStateAttack2,  &CharaPlayer::phaseLeaveStateAttack2);
		changeStateFunc(eCHARA_STATE_ATTACK_ROOLING_START,
						&CharaPlayer::phaseEnterStateAttackRollingStart,&CharaPlayer::phaseUpdateStateAttackRollingStart,&CharaPlayer::phaseLeaveStateAttackRollingStart);
		mItemWork.mItemSword = eITEM_SWORD_MASTERSWORD;
		break;

		/* □攻撃なし */
	case eACTION_NG:
		changeStateFunc(eCHARA_STATE_ATTACK_1,
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
		changeStateFunc(eCHARA_STATE_ATTACK_2,
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
		changeStateFunc(eCHARA_STATE_ATTACK_ROOLING_START,
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
		mItemWork.mItemSword = eITEM_SWORD_NONE;
		break;

	default:
		break;
	};

	TotalInfo::GetInstance()->VisibleBattleIcon();

	//<! ここをなんとかしたい
	if (subWeaponKind == eITEM_KIND_COPY_ROD) {
		if (spPlayerCopyRod == NULL) {
			spPlayerCopyRod = new ObjectWeapon_CopyRod();
			APP_ASSERT((spPlayerCopyRod != NULL),
					   "Failed new ObjectWeapon_CopyRod() \n");
			spPlayerCopyRod->Setup(eOBJECT_KIND_DUMMY_OBJ, (eItemKind)0, (VECTOR *)&ZERO_VEC, 0, true);
		}
	}
	else {
		if (spPlayerCopyRod != NULL) {
			spPlayerCopyRod->DeleteModel();
			delete spPlayerCopyRod;
			spPlayerCopyRod = NULL;
		}
	}

	if (subWeaponKind == eITEM_KIND_NOWEAPON) {
		/* サブウェポンなし */
		mItemWork.mItemSubWeapon	= eITEM_KIND_NOWEAPON;
		changeStateFunc(eCHARA_STATE_SUBWEAPON_START,
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);

	}
	else {
		mItemWork.mItemSubWeapon	= subWeaponKind;
		/* サブウェポンあり */
		switch (subWeaponKind) {
		case eITEM_KIND_LONGHOOK:
			changeStateFunc(eCHARA_STATE_SUBWEAPON_START,
							&CharaPlayer::phaseEnterStateHookShotStart,	&CharaPlayer::phaseUpdateStateHookShotStart, &CharaPlayer::phaseLeaveStateHookShotStart);
			break;
		case eITEM_KIND_BOOMERANG:
			changeStateFunc(eCHARA_STATE_SUBWEAPON_START,
							&CharaPlayer::phaseEnterStateBoomerangStart, &CharaPlayer::phaseUpdateStateBoomerangStart, &CharaPlayer::phaseLeaveStateBoomerangStart);
			break;
		case eITEM_KIND_PIKOHUM:
			// ダミー実装
			changeStateFunc(eCHARA_STATE_SUBWEAPON_START,
							&CharaPlayer::phaseEnterStateBoomerangStart, &CharaPlayer::phaseUpdateStateBoomerangStart, &CharaPlayer::phaseLeaveStateBoomerangStart);
			break;
		case eITEM_KIND_LIB_SHIELD:
			changeStateFunc(eCHARA_STATE_SUBWEAPON_START,
							&CharaPlayer::phaseEnterStateLibShieldStart, &CharaPlayer::phaseUpdateStateLibShieldStart, &CharaPlayer::phaseLeaveStateLibShieldStart);
			break;
			// 爆弾
		case eITEM_KIND_BOMB:
			changeStateFunc(eCHARA_STATE_SUBWEAPON_START,
							&CharaPlayer::phaseEnterStateBombStart,	&CharaPlayer::phaseUpdateStateBombStart, &CharaPlayer::phaseLeaveStateBombStart);
			
			TotalInfo::GetInstance()->AddItemNum(eSUBWEAPON_NUM_BOMB, 0);		// アイテム数の更新
			break;
			// 弓矢
		case eITEM_KIND_ARCHERY:
			changeStateFunc(eCHARA_STATE_SUBWEAPON_START,
							&CharaPlayer::phaseEnterStateArcheryStart, &CharaPlayer::phaseUpdateStateArcheryStart, &CharaPlayer::phaseLeaveStateArcheryStart);
			TotalInfo::GetInstance()->AddItemNum(eSUBWEAPON_NUM_ARROW, 0);		// アイテム数の更新
			break;

			// ロッド
		case eITEM_KIND_COPY_ROD:
			changeStateFunc(eCHARA_STATE_SUBWEAPON_START,
							&CharaPlayer::phaseEnterStateCopyRod, &CharaPlayer::phaseUpdateStateCopyRod, &CharaPlayer::phaseLeaveStateCopyRod);
			break;

			// オカリナ
		case eITEM_KIND_OCARINA:
			changeStateFunc(eCHARA_STATE_SUBWEAPON_START,
							&CharaPlayer::phaseEnterStatePlayOcarina, &CharaPlayer::phaseUpdateStatePlayOcarina, &CharaPlayer::phaseLeaveStatePlayOcarina);
			break;

		default:
			break;
		};
	}

	// トロッコに乗っている間
	GimmickBase* p_gimmick = (GimmickBase *)GimmickBase::GetGimmickBaseFromEarthCollIndex(mOnCollisionIndex);
	if ((p_gimmick != NULL) &&
		(p_gimmick->GetKind() == eGIMMICK_KIND_TROLLEY_EX) ) {
		
		changeStateFunc(eCHARA_STATE_MOVE,
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
		changeStateFunc(eCHARA_STATE_JUMP_UP,
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
	}
	// コスプレ状態がマジックアーマーの場合
	else if (mCosplayIndex == eWINMENU_COSPLAY_GUARDIAN) {
		
		changeStateFunc(eCHARA_STATE_IDLE,
						&CharaPlayer::phaseEnterStateIdle,	&CharaPlayer::phaseUpdateStateIdleCaseGuardian, &CharaPlayer::phaseLeaveStateIdle);
		changeStateFunc(eCHARA_STATE_MOVE,
						&CharaPlayer::phaseEnterStateMoveCaseGuardian,	&CharaPlayer::phaseUpdateStateMoveCaseGuardian, &CharaPlayer::phaseLeaveStateMove);
		changeStateFunc(eCHARA_STATE_JUMP_UP,
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
		changeStateFunc(eCHARA_STATE_ATTACK_ROOLING_START,
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
		changeStateFunc(eCHARA_STATE_SUBWEAPON_START,
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
		changeStateFunc(eCHARA_STATE_AWAY_FRONT, 
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
		changeStateFunc(eCHARA_STATE_AWAY_BACK, 
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
		changeStateFunc(eCHARA_STATE_AWAY_RIGHT, 
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
		changeStateFunc(eCHARA_STATE_AWAY_LEFT, 
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);

		changeStateFunc(eCHARA_STATE_ATTACK_1,
						&CharaPlayer::phaseEnterStateAttack1CaseGuardian,&CharaPlayer::phaseUpdateStateAttack1CaseGuardian,  &CharaPlayer::phaseLeaveStateAttack1);
		changeStateFunc(eCHARA_STATE_ATTACK_2,
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
	}
	// コスプレ状態の場合に動きを制限させる
	else if (IS_COSPLAY) {
		changeStateFunc(eCHARA_STATE_ATTACK_1,
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
		changeStateFunc(eCHARA_STATE_ATTACK_ROOLING_START,
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
		changeStateFunc(eCHARA_STATE_SUBWEAPON_START,
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
		changeStateFunc(eCHARA_STATE_AWAY_FRONT, 
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
		changeStateFunc(eCHARA_STATE_AWAY_BACK, 
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
		changeStateFunc(eCHARA_STATE_AWAY_RIGHT, 
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
		changeStateFunc(eCHARA_STATE_AWAY_LEFT, 
						&CharaPlayer::phaseEnterStateNothing,	&CharaPlayer::phaseUpdateStateNothing, &CharaPlayer::phaseLeaveStateNothing);
	}

	mActionLimit = actionLimit;

	// アタッチパターンを変更する
	ChangeAttachPattern(eATTACH_PATTERN_NORMAL);
}

//---------------------------------------------------
/**
 * 汎用カメラベクトル取得
 */
//---------------------------------------------------
void
CharaPlayer::GetCameraGeneralVec(VECTOR* pOut)
{
	*pOut = mCameraGeneralVec;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::AddMaxHitpoint(int addValue)
{
	mMaxHitpoint += addValue;
	sMaxHitpointTemp = mMaxHitpoint;
}

//---------------------------------------------------
/**
   現在サブウェポンを使用しているか？
 */
//---------------------------------------------------
bool
CharaPlayer::IsSubWeaponUsing(void)
{
	if (mpSubweaponFunc == &CharaPlayer::subweaponFuncNone) {
		return false;
	}
	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
tBoomerangWork*
CharaPlayer::GetBoomerangWork(void)
{
	return &sBoomerang;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::SetObjectHandModel(ObjectHand* pObjectHand)	
{
	mpObjectHand = pObjectHand; 
}

//---------------------------------------------------
/**
   敵キャラの mIsDamage のフラグを解除する（攻撃可にする）
*/
//---------------------------------------------------
void
CharaPlayer::unprotectAllEnemyDamageFlag(void)
{
	ModelCollision* p_mine = this;
	ModelCollision* p_your = NULL;

	for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
		p_your = (ModelCollision *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
		if (p_mine == p_your) continue;			// 自分はのぞく
		if (p_your->GetIsRetire()) continue;	// 戦闘不能者も除く
		
		// 攻撃可にする
		p_your->IsDamage() = false;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::getWeaponTopAndBtmPos(VECTOR* pTopPos, VECTOR* pBtmPos)
{
	ModelPmd::tAttachWork* p_attach = GetAttachAddr(0); // 0 が武器の番号と決め付けて処理
	int model_handle = p_attach->mpAttachedModel->GetModelHandle();
	VECTOR weapon_top_pos;
	VECTOR weapon_btm_pos = MV1GetFramePosition(model_handle, 0);
	VECTOR mesh_01_pos;
	mesh_01_pos = MV1GetMeshMaxPosition(model_handle, kWEAPON_TOP_NODE_INDEX);
	MATRIX mtx = MGetTranslate(mesh_01_pos);
	MATRIX frame_mtx = MV1GetFrameLocalWorldMatrix(model_handle, kWEAPON_TOP_NODE_INDEX);
	mtx = MMult(mtx, frame_mtx);
	weapon_top_pos.x = mtx.m[3][0];
	weapon_top_pos.y = mtx.m[3][1];
	weapon_top_pos.z = mtx.m[3][2];

	*pTopPos = weapon_top_pos;
	*pBtmPos = weapon_btm_pos;
}

/*=====================================*
 * mPhaseState
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateIdle(void)
{
	if (this->GetIsRetire()) {
		ChangeState(eCHARA_STATE_RETIRE_START);
		return;
	}
	else if (mPiyoPiyoValue <= 0) {
		ChangeState(eCHARA_STATE_PIYOPIYO);
		return;
	}

	mIsReserveComboInput = false;
	mIsAttack = false;
	mIsDamage = false;
	
	mIsAway				= false;
	mIsSubAttack		= false;
	mIsJumpAttack		= false;

	mJumpMoveVec.mDash = ZERO_VEC;

	// エフェクト終了
	if (sEffWindIndex != -1) {
		EffectManager::GetInstance()->Stop(sEffWindIndex);
		sEffWindIndex = -1;
	}
	mBladeEffect.Stop();

	mCount.uSleep = 0;

	if (mIsLeaveWeapon == true) {

		// アタッチパターンを変更
		ChangeAttachPattern(eATTACH_PATTERN_NORMAL);

		mIsLeaveWeapon = false;
	}

	/* アイドルモーションの指定 */
	if (( (mHitpoint * 100)  / mMaxHitpoint ) < 35) {
		// 瀕死モーション
		sIdleMotionIndex = eMT_WEEKLY;
	}
	else {
		if (mActionLimit == eACTION_NG) {
			// 非戦闘モーション
			sIdleMotionIndex = eMT_IDLE_NOWEAPON;
		}
		else {
			// 戦闘モーション
			sIdleMotionIndex = eMT_IDLE;
		}
	}

	StartMotionEx(&sMotionParam[sIdleMotionIndex]);
}

//---------------------------------------------------
/**
 * ※ phaseUpdateStateRaiseIdle が処理をコピーしている為、
 *	どちらかを編集する際は注意すること！
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateIdle(void)
{
	eTurnType turn_type = eTURN_TYPE_NONE;

	mCount.uSleep ++;

	turn_type = updateTargetRotateFromPad();
	mTurnType = turn_type;

	if (CHECK_TRIG_JOY(JOY_INPUT_ATTACK)) {
		ChangeState(eCHARA_STATE_ATTACK_1);

		CHECK_TRIG_JOY(JOY_INPUT_ATTACK);

	}
	else if (CHECK_PRESS_JOY(JOY_INPUT_ATTACK)) {
		mCount.uSleep = 0;
		ChangeState(eCHARA_STATE_ATTACK_ROOLING_START);
	}
	else if (CHECK_TRIG_JOY(JOY_INPUT_SUB_WEAPON)) {
		mCount.uSleep = 0;
		ChangeState(eCHARA_STATE_SUBWEAPON_START);
	}
	else if (CHECK_TRIG_JOY(JOY_INPUT_JUMP)) {

		mJumpMoveVec.mDash = ZERO_VEC;
		mCount.uSleep = 0;
		ChangeState(eCHARA_STATE_JUMP_UP);
	}
	else if (turn_type != eTURN_TYPE_NONE) {
		Utility::CorrectAngleFrom000To360(&mTargetRotY);
		mCount.uSleep = 0;
		ChangeState(eCHARA_STATE_TURN);
	}
	
	if (mCount.uSleep > (5.0f * 60.0f)) {
		mCount.uSleep = 0;
		if (sIdleMotionIndex != eMT_WEEKLY) {
			ChangeState(eCHARA_STATE_SLEEP);
		}
	}	

	static BYTE sPushCamRestFrame = 0;

	if (mIsPermitInput == true) {

		// カメラリセット
		if (CHECK_TRIG_JOY(JOY_INPUT_CAM_RESET)) {
			mpLockonChara = NULL;
			CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_MIDDLE, true);
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_RESET);
			sResetCameraBeforeState = mCharaState;
			ChangeState(eCHARA_STATE_WAIT_RESET_CAMERA);

			sPushCamRestFrame = 0;
		}
		else if (CHECK_PRESS_JOY(JOY_INPUT_CAM_RESET)) {

			sPushCamRestFrame ++;
			if (sPushCamRestFrame > 10) {
			
				ChangeState(eCHARA_STATE_LOOK_AROUND);
				sPushCamRestFrame = 0;
			}
		}

		if ((TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_LIB_SHIELD)) &&
			(CHECK_TRIG_JOY(JOY_INPUT_AWAY)) &&
			(mActionLimit == eACTION_OK) ) {
			ChangeState(eCHARA_STATE_LIB_SHIELD);
		}
	}
	
#ifdef _DEBUG
	if (mpLockonChara != NULL) {
		PRINT_SCREEN(GetColor(255, 255, 0), "Lockon !\n");
	}
#endif // _DEBUG

	for (int iI = 0; iI < 6; iI ++) {
		if (CHECK_TRIG_KEY(eKEY_0 + iI)) {
			if (TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_SMILE_0 + iI)) {
				StartMotionEx(&sMotionParam[(eMT_SMILE_00 + iI)]);
				ChangeState(eCHARA_STATE_SMILE);
				break;
			}
		}
	}

	if (CHECK_TRIG_KEY(eKEY_6)) {
		if (TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_DANCE)) {
			StartMotionEx(&sMotionParam[eMT_SMILE_06]);
			ChangeState(eCHARA_STATE_SMILE);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateTurn(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateTurn(void)
{
	/* キャラクターの向きを処理する */
	Utility::CorrectAngleFrom000To360(&mRotation.y);

	float diff_rot = mTargetRotY - mRotation.y;
	eTurnType turn_type = eTURN_TYPE_NONE;
	
	if (kTURN_MOVE_ROT >= abs(diff_rot)) {
		mRotation.y = mTargetRotY;

		turn_type = updateTargetRotateFromPad();
		mTurnType = turn_type;

		if (turn_type != eTURN_TYPE_NONE) {
			ChangeState(eCHARA_STATE_MOVE);
		} else {
			ChangeState(eCHARA_STATE_IDLE);
		}

	} else {
		bool is_plus = (diff_rot >= 0.0f) ? true : false;
		if (abs(diff_rot) > Utility::DegToRad(180.0f)) {
			is_plus ^= true;
		}
		mRotation.y += (is_plus) ? kTURN_MOVE_ROT : -kTURN_MOVE_ROT;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateMove(void)
{
	mRunSpeed = kCHARA_WALK_SPEED;
	if (CHECK_PRESS_JOY(JOY_INPUT_RUN) || sIsAnalogInputRun) {
		StartMotion(eMT_RUN, true, 1.0f, NULL, 0.05f);

	} else {
		StartMotionEx(&sMotionParam[eMT_WALK]);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateMove(void)
{
	eTurnType turn_type = eTURN_TYPE_NONE;
	VECTOR org_vec, org_cross_vec, side_vec;

	// キャラクタの向き
	turn_type = updateTargetRotateFromPad();
	Utility::CorrectAngleFrom000To360(&mTargetRotY);

	org_vec = VSub(CameraManager::GetInstance()->Target(), CameraManager::GetInstance()->Position());
	org_vec.y = 0.0f;
	org_vec = VNorm(org_vec);
	org_cross_vec = VCross(sUpVec, org_vec);

	// 攻撃に遷移
	if (CHECK_TRIG_JOY(JOY_INPUT_ATTACK)) {
		ChangeState(eCHARA_STATE_ATTACK_1);
		return;
	}
	// ジャンプに遷移
	else if (CHECK_TRIG_JOY(JOY_INPUT_JUMP)) {
		mJumpMoveVec.mDash = ZERO_VEC;
		if (CHECK_PRESS_JOY(JOY_INPUT_RUN) || sIsAnalogInputRun) {
			mJumpMoveVec.mDash.x = sin(mRotation.y + DX_PI) * mRunSpeed;
			mJumpMoveVec.mDash.z = cos(mRotation.y + DX_PI) * mRunSpeed;

			// 重力が重ければ横の移動量も減らす
			mJumpMoveVec.mDash = VScale(mJumpMoveVec.mDash, (-0.98f / kGRAVITY_SPEED));
		}
		ChangeState(eCHARA_STATE_JUMP_UP);
		return;

	}
	// 待機に遷移
	else if (turn_type == eTURN_TYPE_NONE) {
		ChangeState(eCHARA_STATE_IDLE);
		return;
	}

	// 入力された十字ーが現在の向きと反対かその隣の場合はターンステートへ
	if ((turn_type == ((mTurnType + 3) % 8) ) ||
		(turn_type == ((mTurnType + 4) % 8) ) ||
		(turn_type == ((mTurnType + 5) % 8) ) ) {
		mTurnType = turn_type;
		ChangeState(eCHARA_STATE_IDLE);
		return;
	}

	if (mTurnType != turn_type) {
		mRunSpeed -= (kCHARA_ADD_MOVE_SPEED * 10.0f);
		if (mRunSpeed <= kCHARA_WALK_SPEED) {
			mRunSpeed = kCHARA_WALK_SPEED;
		}
	}

	/**** 十字キーに応じて移動処理 ****/
	// ターン状態を更新
	mTurnType = turn_type;


	VECTOR dir_vec = ZERO_VEC;
	if (CHECK_PRESS_JOY(JOY_INPUT_UP)) {
		dir_vec = VAdd(dir_vec, org_vec);
	}
	else if (CHECK_PRESS_JOY(JOY_INPUT_DOWN)) {
		dir_vec = VAdd(dir_vec, VScale(org_vec, -1));
	}
	if (CHECK_PRESS_JOY(JOY_INPUT_RIGHT)) {
		dir_vec = VAdd(dir_vec, org_cross_vec);
	}
	else if (CHECK_PRESS_JOY(JOY_INPUT_LEFT)) {
		dir_vec = VAdd(dir_vec, VScale(org_cross_vec, -1));
	}

	dir_vec = VNorm(dir_vec);
	Utility::CorrectAngleFrom000To360(&mRotation.y);
	mTargetRotY = atan2(dir_vec.x, dir_vec.z) + DX_PI;

	float diff_rot = mTargetRotY - mRotation.y;
	if (kTURN_MOVE_ROT >= abs(diff_rot)) {
		mRotation.y = mTargetRotY;
	}
	else {
		bool is_plus = (diff_rot >= 0.0f) ? true : false;
		if (abs(diff_rot) > Utility::DegToRad(180.0f)) {
			is_plus ^= true;
		}
		mRotation.y += (is_plus) ? kTURN_MOVE_ROT * 0.4f : -kTURN_MOVE_ROT * 0.4f;
	}

	side_vec = VCross(sUpVec, dir_vec);
	dir_vec = VCross(side_vec, mNormalVec);
	
	dir_vec.y += mGravity * 0.1f;
	dir_vec = VNorm(dir_vec);

#ifdef _DEBUG
	RENDER_LINE(&mPosition, &VAdd(mPosition, VScale(dir_vec, 300.0f)), COLOR_RED);
#endif // _DEBUG
	
	// 足音
	float sound_secA = 0.0f;
	float sound_secB = 0.0f;
	if ((CHECK_PRESS_JOY(JOY_INPUT_RUN) || sIsAnalogInputRun) &&
		(GetMotionIndex() != eMT_WALK)) {

		/* 走りの処理 */
		mRunSpeed += kCHARA_ADD_MOVE_SPEED;
		if (mRunSpeed >= mMaxRunSpeed) {
			mRunSpeed = mMaxRunSpeed;
		}
		StartMotion(eMT_RUN, true, 1.0f, NULL, 0.05f);

		sound_secA = RUN_SOUND_SECOND_A;
		sound_secB = RUN_SOUND_SECOND_B;

		mMoveVec = VScale(dir_vec, mRunSpeed);

	}
	else {
		/* 歩きの処理 */
		mRunSpeed = kCHARA_WALK_SPEED;
		StartMotionEx(&sMotionParam[eMT_WALK]);
		mMoveVec = VScale(dir_vec, mRunSpeed);
		if ((CHECK_PRESS_JOY(JOY_INPUT_RUN) || sIsAnalogInputRun) &&
			(GetIsMotionBlend() == false)) {
			StartMotion(eMT_RUN, true, 1.0f, NULL, 0.05f);

		}

		sound_secA = WALK_SOUND_SECOND_A;
		sound_secB = WALK_SOUND_SECOND_B;
	}

	// 走り・歩きでの足音を鳴らす
	if ((mMotion.mNowTime <= sound_secA) && (mMotion.mNowTime + mMotion.mSpeed > sound_secA)) {
		UPDATE_FOOTSTEP_KIND();
		SoundManager::GetSndComponent()->PlaySound(sFootSoundTable[sFootstepKind][eFOOT_SND_STEP_A]);
	}
	if ((mMotion.mNowTime <= sound_secB) && (mMotion.mNowTime + mMotion.mSpeed > sound_secB)) {
		UPDATE_FOOTSTEP_KIND();
		SoundManager::GetSndComponent()->PlaySound(sFootSoundTable[sFootstepKind][eFOOT_SND_STEP_B]);
	}

	// カメラリセット
	if (CHECK_TRIG_JOY(JOY_INPUT_CAM_RESET)) {
		mpLockonChara = NULL;
		CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_MIDDLE, true);
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_RESET);
		sResetCameraBeforeState = mCharaState;
		ChangeState(eCHARA_STATE_WAIT_RESET_CAMERA);
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateMove(void) 
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateJumpUp(void)
{
	// うるさいので封印
	StartMotionEx(&sMotionParam[eMT_JUMP_START]);

	mBladeEffect.Stop();

	mGravity += kJUMP_GRAVITY;
	mConstGravitySpeed = kGRAVITY_SPEED;

	mTargetRotY = mRotation.y;

	if (mpObjectHand != NULL) {
		unprotectAllEnemyDamageFlag();
		mpObjectHand->ProcessThrow((VECTOR *)&ZERO_VEC);
	}

	GimmickBase* p_gimmick = NULL;
	p_gimmick = (GimmickBase *)GimmickBase::GetGimmickBaseFromEarthCollIndex(mOnCollisionIndex);
	if (p_gimmick != NULL) {
		mOnCollisionIndex = -1;
	}
	
	mIsLand = false;
	mEarthState = eEARTH_STATE_SKY;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateJumpUp(void)
{
	VECTOR dir_vec = ZERO_VEC;
	VECTOR org_vec, org_cross_vec;
	org_vec = VSub(CameraManager::GetInstance()->Target(), CameraManager::GetInstance()->Position());
	org_vec.y = 0.0f;
	org_vec = VNorm(org_vec);
	org_cross_vec = VCross(sUpVec, org_vec);

	mMoveVec = mJumpMoveVec.mDash;

	if (VSize(mJumpMoveVec.mDash) == 0.0f) {
		// ジャンプ前に移動が無い場合

		if (CHECK_PRESS_JOY(JOY_INPUT_UP)) {
			dir_vec = VAdd(dir_vec, org_vec);
		} else if (CHECK_PRESS_JOY(JOY_INPUT_DOWN)) {
			dir_vec = VAdd(dir_vec, VScale(org_vec, -1));
		}
		if (CHECK_PRESS_JOY(JOY_INPUT_RIGHT)) {
			dir_vec = VAdd(dir_vec, org_cross_vec);
		} else if (CHECK_PRESS_JOY(JOY_INPUT_LEFT)) {
			dir_vec = VAdd(dir_vec, VScale(org_cross_vec, -1));
		}
		if (VSize(dir_vec) > 0.0f) {
			dir_vec = VNorm(dir_vec);
			mMoveVec = VAdd(mMoveVec, VScale(dir_vec, kCHARA_WALK_SPEED));
		}

	} else {
		// ジャンプ前に移動が有る場合

		/* キャラクターの向きを処理する */
		Utility::CorrectAngleFrom000To360(&mRotation.y);
		Utility::CorrectAngleFrom000To360(&mTargetRotY);

		float diff_rot = mTargetRotY - mRotation.y;
		eTurnType turn_type = eTURN_TYPE_NONE;
	
		if (kTURN_MOVE_ROT >= abs(diff_rot)) {
			mRotation.y = mTargetRotY;

			turn_type = updateTargetRotateFromPad();
			mTurnType = turn_type;

			if (turn_type != eTURN_TYPE_NONE) {
				dir_vec.x = -(sin(mRotation.y));
				dir_vec.y = 0.0f;
				dir_vec.z = -(cos(mRotation.y));
				dir_vec = VNorm(dir_vec);
				mMoveVec = VAdd(mMoveVec, VScale(dir_vec, (2.0f * kCHARA_WALK_SPEED)));
			}
		} else {
			bool is_plus = (diff_rot >= 0.0f) ? true : false;
			if (abs(diff_rot) > Utility::DegToRad(180.0f)) {
				is_plus ^= true;
			}
			mRotation.y += (is_plus) ? kTURN_MOVE_ROT : -kTURN_MOVE_ROT;
		}
	}

	if (mIsJumpAttack == false) {
		if (CHECK_PRESS_JOY(JOY_INPUT_ATTACK)) {

			ChangeState(eCHARA_STATE_ATTACK_JUMP);
			return;
		}
	}

	if (mGravity < 0) {
		mConstGravitySpeed = kGRAVITY_SPEED;
		ChangeState(eCHARA_STATE_JUMP_DOWN);
	} else {

		//<! ジャンプの大きさを変える処理の追加
		if (CHECK_PRESS_JOY(JOY_INPUT_JUMP) == false) {
			mGravity *= 0.85f;
			mConstGravitySpeed *= 0.95f;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateJumpDown(void)
{
	if (mpObjectHand != NULL) {
		unprotectAllEnemyDamageFlag();
		mpObjectHand->ProcessThrow((VECTOR *)&ZERO_VEC);
	}
	StartMotion(eMT_JUMP_DOING, true, 0.5f, NULL, 0.05f);

	mJumpBugCount = 0;
}
	
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateJumpDown(void)
{
	phaseUpdateStateJumpUp(); // 上昇中と同じ処理

	if (IS_COSPLAY == FALSE) {
		// 天使の翼を入手済み
		if (TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_ANGEL_WING)) {
			if (mWingJumpState == eWING_JUMP_STATE_NONE) {
				if (CHECK_TRIG_JOY(JOY_INPUT_JUMP) ) {
					processWingJump();
					mGravity = 0.0f;
					mJumpMoveVec.mDash = VScale(mJumpMoveVec.mDash, 0.5f);
					ChangeState(eCHARA_STATE_JUMP_UP);
					StartMotion(eMT_JUMP_DOING, false, 0.5f, NULL, 0.05f);

					mConstGravitySpeed *= 0.5f;		// ChangeState(eCHARA_STATE_JUMP_UP)の後に設定
					return;
				}
			}
		}
	}

	if ((mEarthState == eEARTH_STATE_LINE) || (mEarthState == eEARTH_STATE_CAPSULE)) {
		mJumpMoveVec.mLanding.x = mMoveVec.x;
		mJumpMoveVec.mLanding.y = 0.0f;
		mJumpMoveVec.mLanding.z = mMoveVec.z;

		UPDATE_FOOTSTEP_KIND();
		SoundManager::GetSndComponent()->PlaySound(sFootSoundTable[sFootstepKind][eFOOT_SND_LANDING]);
		StartMotionEx(&sMotionParam[eMT_JUMP_END]);
		ChangeState(eCHARA_STATE_JUMP_END);
	}

	if (abs(mGravity) <= 0.01f) {
		if (mJumpBugCount ++ > 60) {
#ifdef _DEBUG
			SoundManager::GetSndComponent()->PlaySound(eSE_DECIDE_KEY);
#endif // _DEBUG
			PRINT_CONSOLE("Recover Jump Bug \n");
			ChangeState(eCHARA_STATE_IDLE);
		}
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateJumpEnd(void)
{
	mIsJumpAttack	= false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateJumpEnd(void)
{
	mJumpMoveVec.mLanding = VScale(mJumpMoveVec.mLanding, 0.95f);
	mMoveVec = mJumpMoveVec.mLanding;

	if (CHECK_TRIG_JOY(JOY_INPUT_JUMP)) {
		mJumpMoveVec.mDash = mMoveVec;
		ChangeState(eCHARA_STATE_JUMP_UP);
		return;
	}

	int motion_index = GetMotionIndex();

	if (motion_index == eMT_JUMP_END) {
		if (GetIsEndMotion()) {
			StartMotion(sIdleMotionIndex, true, 0.5f);
		}
	} else if (motion_index == sIdleMotionIndex) {
		if (GetIsMotionBlend() == false) {
			ChangeState(eCHARA_STATE_IDLE);
		}
	}
}
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateDamage(void)
{
	StartMotionEx(&sMotionParam[eMT_DAMAGE]);
	SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_DAMAGE);

	if (mpObjectHand != NULL) {
		unprotectAllEnemyDamageFlag();
		mpObjectHand->ProcessThrow((VECTOR *)&ZERO_VEC);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateDamage(void)
{
	if (GetMotionIndex() != eMT_DAMAGE) {
		mIsDamage = false;
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateRetireStart(void)
{
	DisableLockonChara();

	StartMotionEx(&sMotionParam[eMT_DOWN_START]);
	SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_DOWN);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateRetireStart(void)
{
	VECTOR move_vec;
	float move_speed = 0.5f * (float)(100 - GetMotionTimePercent());
	move_vec = VScale(mJumpMoveVec.mDamageDown, move_speed);
	mMoveVec = move_vec;
	if (GetIsEndMotion() && mIsLand && (GetIsMotionBlend() == false)) {
		ChangeState(eCHARA_STATE_RETIRE_DOING);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateRetireDoing(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateRetireDoing(void)
{
	ChangeState(eCHARA_STATE_RETIRE_END);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateRetireEnd(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateRetireEnd(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateSlopeDown(void)
{
	StartMotionEx(&sMotionParam[eMT_JUMP_DOING]);

	mJumpBugCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateSlopeDown(void)
{
	if ((mEarthState == eEARTH_STATE_LINE) ||
		(mEarthState == eEARTH_STATE_CAPSULE)) {
		mJumpMoveVec.mLanding = ZERO_VEC;
		StartMotionEx(&sMotionParam[eMT_JUMP_END]);
		ChangeState(eCHARA_STATE_JUMP_END);
	}

	if (abs(mGravity) <= 0.01f) {
		if (mJumpBugCount ++ > 60) {
#ifdef _DEBUG
			SoundManager::GetSndComponent()->PlaySound(eSE_DECIDE_KEY);
#endif // _DEBUG
			PRINT_CONSOLE("Recover Jump Bug \n");
			ChangeState(eCHARA_STATE_IDLE);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateRefuseInput(void)
{
	StartMotionEx(&sMotionParam[sIdleMotionIndex]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateRefuseInput(void)
{
	// 何もうけつけない
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateDownStart(void)
{
	SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_DAMAGE);
	StartMotionEx(&sMotionParam[eMT_DOWN_START]);

	if (mpObjectHand != NULL) {
		unprotectAllEnemyDamageFlag();
		mpObjectHand->ProcessThrow((VECTOR *)&ZERO_VEC);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateDownStart(void)
{
	VECTOR move_vec;
	float move_speed = 0.5f * (float)(100 - GetMotionTimePercent());

	move_vec = VScale(mJumpMoveVec.mDamageDown, move_speed);

	mMoveVec = move_vec;

	if (GetIsEndMotion() && mIsLand) {
		ChangeState(eCHARA_STATE_DOWN_DOING);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateDownDoing(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateDownDoing(void)
{
	ChangeState(eCHARA_STATE_DOWN_END);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateDownEnd(void)
{
	StartMotion(eMT_DOWN_END, false, 0.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateDownEnd(void)
{
	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
 * 垂直直下のフェーズ
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateFallDown(void)
{
	StartMotionEx(&sMotionParam[eMT_JUMP_DOING]);
}

//---------------------------------------------------
/**
 * 垂直直下のフェーズ
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateFallDown(void)
{
	mMoveVec = mJumpMoveVec.mDash;

	if (mIsLand == true) {
		mJumpMoveVec.mLanding.x = mMoveVec.x;
		mJumpMoveVec.mLanding.y = 0.0f;
		mJumpMoveVec.mLanding.z = mMoveVec.z;

		UPDATE_FOOTSTEP_KIND();
		SoundManager::GetSndComponent()->PlaySound(sFootSoundTable[sFootstepKind][eFOOT_SND_LANDING]);

		StartMotionEx(&sMotionParam[eMT_JUMP_END]);
		ChangeState(eCHARA_STATE_JUMP_END);
	}
}

/*=====================================*
 * 拡張ステータス
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateNothing(void)
{
	if (mCharaState == eCHARA_STATE_SUBWEAPON_START) {
		if (IS_COSPLAY) {

			// 水の上にいる場合はコスプレ解除できない様にする
			if (sFootstepKind == eFOOTSTEP_KIND_WATER) {

				ActionTaskManager::tTaskInfo info;
				info.mTaskType		= ActionTaskManager::eTASK_NAVI_EX;
				info.mTaskValueA	= (long)strNaviTalk_CannotCosplay;
				info.mTaskValueB	= NULL;
				ActionTaskManager::GetInstance()->NotifyAction(&info);

				SoundManager::GetSndComponent()->PlaySound(eSE_NG);
			}
			else {
				if (mCosplayIndex == eWINMENU_COSPLAY_TITANIS) {
					revertVehicle();
				}
				else if (mCosplayIndex == eWINMENU_COSPLAY_GUARDIAN) {
					STOP_EFFEKSEER(mEfkHnd.uMagic, 0.0f);
					revertGuardian();
				}
				RevertCosplay();
				EffekseerManager::GetInstance()->Play(eEFK_GETKEYITEM, &mPosition, 10.0f);
				// 『えいっ！』
				SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_ATTACK_2);
			}
		}
	}

	eTurnType turn_type = eTURN_TYPE_NONE;
	turn_type = updateTargetRotateFromPad();
	if (turn_type == eTURN_TYPE_NONE) {
		ChangeState(eCHARA_STATE_IDLE);
	} else {
		ChangeState(eCHARA_STATE_MOVE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateNothing(void)
{
	// 何もしない
	ChangeState(eCHARA_STATE_IDLE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateNothing(void) 
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateAttack1(void)
{
	SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	unprotectAllEnemyDamageFlag();

	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotionEx(&sMotionParam[eMT_ATTACK_1]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateAttack1(void)
{
	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);

	float move_speed = 0.12f * (100 - GetMotionTimePercent());
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	// コンボ入力受付
	if (CHECK_TRIG_JOY(JOY_INPUT_ATTACK)) {
		mIsReserveComboInput = true;
	}	

	bool is_prev_attack;
	is_prev_attack = mIsAttack;
	if ((GetMotionNowTime() >= 7.0f) &&
		(GetMotionNowTime() <= 11.0f)) {
		mAttackPower = 1;
		mIsAttack = true;
	} else {
		mIsAttack = false;
	}

	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if ((ModelPmd::GetMotionTimePercent() > 80) &&
		(mIsReserveComboInput == true)) {
		ChangeState(eCHARA_STATE_ATTACK_2);
	} else if (ModelPmd::GetIsEndMotion()) {
		mBladeEffect.Stop();
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateAttack2(void)
{
	SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_ATTACK_2);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
	StartMotionEx(&sMotionParam[eMT_ATTACK_2]);

	unprotectAllEnemyDamageFlag();
	mIsReserveComboInput = false;

	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);
	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateAttack2(void)
{
	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);

	float move_speed = 0.003f * (100 - GetMotionTimePercent());
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	if (CHECK_TRIG_JOY(JOY_INPUT_ATTACK)) {
		mIsReserveComboInput = true;
	}	

	bool is_prev_attack;
	is_prev_attack = mIsAttack;
	if ((GetMotionNowTime() >= 1.0f) &&
		(GetMotionNowTime() <= 7.0f)) {
		mAttackPower = 1;
		mIsAttack = true;
	} else {
		mIsAttack = false;
	}

	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if ((ModelPmd::GetMotionTimePercent() > 80) &&
		(mIsReserveComboInput == true)) {
		ChangeState(eCHARA_STATE_ATTACK_3);
	} else if (ModelPmd::GetIsEndMotion()) {
		mBladeEffect.Stop();
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateAttack3(void)
{
	SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_ATTACK_3);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
	StartMotionEx(&sMotionParam[eMT_ATTACK_3]);

	unprotectAllEnemyDamageFlag();
	mIsReserveComboInput = false;

	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);
	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateAttack3(void)
{
	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);

	float move_speed = 0.24f * (100 - GetMotionTimePercent());
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	if (CHECK_TRIG_JOY(JOY_INPUT_ATTACK)) {
		mIsReserveComboInput = true;
	}	

	bool is_prev_attack;
	is_prev_attack = mIsAttack;
	if ((GetMotionNowTime() >= 8.0f) &&
		(GetMotionNowTime() <= 12.0f)) {
		mAttackPower = 1;
		mIsAttack = true;
	} else {
		mIsAttack = false;
	}

	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if ((ModelPmd::GetMotionTimePercent() > 80) &&
		(mIsReserveComboInput == true)) {
		ChangeState(eCHARA_STATE_ATTACK_4);
	} else if (ModelPmd::GetIsEndMotion()) {
		mBladeEffect.Stop();
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateAttack4(void)
{
	SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_ATTACK_4);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
	StartMotionEx(&sMotionParam[eMT_ATTACK_4]);

	unprotectAllEnemyDamageFlag();

	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);
	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateAttack4(void)
{
	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);

	float move_speed = 0.01f * (100 - GetMotionTimePercent());
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	bool is_prev_attack;
	is_prev_attack = mIsAttack;
	if ((GetMotionNowTime() >= 8.0f) &&
		(GetMotionNowTime() <= 14.0f)) {
		mAttackPower = 1;
		mIsAttack = true;
	} else {
		mIsAttack = false;
	}

	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {
		mBladeEffect.Stop();
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateAttackThrust(void)
{
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
	StartMotionEx(&sMotionParam[eMT_ATTACK_THRUST]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateAttackThrust(void)
{
	static const float sCheckTime = 14.0f;

	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);

	if (this->EqualPhaseFrame(sCheckTime)) {
		SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_ATTACK_4);
		unprotectAllEnemyDamageFlag();
		mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);
	}
	float motion_now_time = this->GetMotionNowTime();

	if (motion_now_time > sCheckTime) {
		float motion_max_time = this->GetMotionTotalTime();
		float percent = (100.0f - ((motion_now_time - sCheckTime) / (motion_max_time - sCheckTime) * 100.0f));
		float move_speed = sThrustAttackVec * ((percent * percent) / 100.0f);
		mMoveVec.x += -(sin(mRotation.y) * move_speed);
		mMoveVec.z += -(cos(mRotation.y) * move_speed);

		mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);
	}

	bool is_prev_attack;
	is_prev_attack = mIsAttack;
	if (this->BetweenPhaseFrame(sCheckTime, 50.0f)) {
		mAttackPower = 1;
		mIsAttack = true;
	} else {
		mIsAttack = false;
	}

	if (ModelPmd::GetIsEndMotion()) {
		mBladeEffect.Stop();
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateAttackRollingStart(void)
{
	SoundManager::GetSndComponent()->PlaySound(eSE_CHARGE);
	StartMotion(eMT_ATTACK_4, false, 0.0f);

	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);

	float size = 120.0f;
	sEffWindIndex = EffectManager::GetInstance()->Play(eEF_THUNDER,
													   &weapon_top_pos,
													   &VGet(size, size, size),
													   10.0f);
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateAttackRollingStart(void)
{
	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsMotionBlend()) {
		EffectManager::GetInstance()->Position(sEffWindIndex) = weapon_top_pos;
	} else {
		VECTOR pos, vec;
		float size, eff_size;
		pos = EffectManager::GetInstance()->Position(sEffWindIndex);
		vec = VSub(weapon_btm_pos, pos);
		size = VSize(vec);
		eff_size = 120.0f - size;
		EffectManager::GetInstance()->Scale(sEffWindIndex) = VGet(eff_size, eff_size, eff_size);
		//		vec = VScale(vec, 0.1f);
		vec = VScale(vec, 0.05f);
		pos = VAdd(pos, vec);

		EffectManager::GetInstance()->Position(sEffWindIndex) = pos;

		StartJoypadVibration(DX_INPUT_PAD1, 500, 10);

		if (CHECK_PRESS_JOY(JOY_INPUT_ATTACK) == false) {

			PRINT_CONSOLE("size : %.2f\n", size);

			if (size < 20.0f) {
				ChangeState(eCHARA_STATE_ATTACK_ROOLING);
			}
			else {
				ChangeState(eCHARA_STATE_IDLE);
			}
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateAttackRollingStart(void) 
{
	if (sEffWindIndex != -1) {
		EffectManager::GetInstance()->Stop(sEffWindIndex, 0.0f);
		sEffWindIndex = -1;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateAttackRolling(void)
{
	SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_ATTACK_4);
	SoundManager::GetSndComponent()->PlaySound(eSE_ROLLING);
	StartMotion(eMT_ATTACK_ROLL, false,	0.75f,	NULL, 0.25f);

	sEffWindIndex = EffectManager::GetInstance()->Play(eEF_WIND,
													   &mPosition,
													   &VGet(120.0f, 120.0f, 120.0f),
													   0.2f);
	
	unprotectAllEnemyDamageFlag();

	addPiyoPiyoGauge(-2000);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateAttackRolling(void)
{
	bool is_prev_attack;
	is_prev_attack = mIsAttack;
	if ((GetMotionNowTime() >= 0.0f) &&
		(GetMotionNowTime() <= 24.0f)) {
#ifdef _DEBUG
		mAttackPower = 10000;
#else
		mAttackPower = 4;
#endif // 

		mIsAttack = true;
	} else {
		mIsAttack = false;
	}

	if (GetIsEndMotion()) {
		EffectManager::GetInstance()->Stop(sEffWindIndex);
		sEffWindIndex = -1;
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateAttackRolling(void) 
{
	return;
}

/*=====================================*
 * HookShot 関連
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateHookShotStart(void)
{
	sHookshotChain.StartMotionForcibly(0, false, 0.5f, NULL, 1.0f);

	SoundManager::GetSndComponent()->PlaySound(eSE_HOOK_START);

	unprotectAllEnemyDamageFlag();

	mIsSubAttack = true;
	StartMotionEx(&sMotionParam[eMT_HOOK_SHOT]);

	sHookShotTargetVec.x = (sin(mRotation.y) * 100.0f);
	sHookShotTargetVec.y = 0.0f;
	sHookShotTargetVec.z = (cos(mRotation.y) * 100.0f);

	this->DisableShadow();

	int chain_mdl_handle = sHookshotChain.GetModelHandle();
	for (int iI = 0; iI < MV1GetMeshNum(chain_mdl_handle); iI ++) {
		if (iI == 4) continue;
		// オリジナルモデルのタイヤを非表示にする
		MV1SetMeshVisible(chain_mdl_handle, iI, FALSE);
	}
	sHookshotChain.SetMatrixType(ModelBase::eMATRIX_TYPE_MATRIX);
	GraphicManager::GetInstance()->ReserveEntryModel(&sHookshotChain);

	// アタッチパターンを変更
	ChangeAttachPattern(eATTACH_PATTERN_HOOKSHOT);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateHookShotStart(void)
{
	ModelPmd::tAttachWork* p_attach = GetAttachAddr(eATTACH_MODEL_SUB_WEAPON);
	ModelPmd* p_pmd = (ModelPmd *)p_attach->mpAttachedModel;
	
	if (this->GetIsMotionBlend() == false) {

		bool result;

		if (mpLockonChara != NULL) {
			sHookShotTargetVec = VSub(mPosition, mpLockonChara->Position());
		}
		else {
			// ジョイパッドとマウスの両方から武器のターゲットの向きを求める
			updateWeaponTargetVecFromInput(&sHookShotTargetVec, 100.0f, 100.0f);
		}

		VECTOR hookshot_targetpos = VAdd(sHookShotTargetVec, mPosition);

		MATRIX mtx, tmp;

		// 直接マトリックスを触る
		this->SetMatrixType(ModelBase::eMATRIX_TYPE_MATRIX);
	
		mtx = MGetIdent();
		// 拡縮行列
		tmp = MGetIdent();
		tmp = MGetScale(VGet(mScale.x, mScale.y, mScale.z));
		mtx = MMult(mtx, tmp);
		// 回転行列
		tmp = MGetIdent();
		Utility::MTXLookAtMatrix(&tmp,
								 &mPosition,
								 &hookshot_targetpos,
								 &VGet(0.0f, 1.0f, 0.0f));
		mtx = MMult(mtx, tmp);
		// 移動行列
		tmp = MGetIdent();
		tmp = MGetTranslate(mPosition);
		mtx = MMult(mtx, tmp);

		this->Matrix() = mtx;

		VECTOR weapon_btm_pos, weapon_top_pos;
		result = p_pmd->GetFramePosFromName(&weapon_btm_pos, strHookShotBottom);
		APP_ASSERT(result, "Failed ModelPmd::GetFramePosFromName\n");
		result = p_pmd->GetFramePosFromName(&weapon_top_pos, strHookShotHead);
		APP_ASSERT(result, "Failed ModelPmd::GetFramePosFromName\n");

		weapon_top_pos = VAdd(VScale(VSub(weapon_top_pos, weapon_btm_pos), 74.0f), weapon_btm_pos);
#ifdef _DEBUG
		GraphicManager::GetInstance()->DbgRenderLine(&weapon_top_pos, &weapon_btm_pos);
#endif // _DEBUG

		BOOL is_hit = FALSE;
		VECTOR hitpos, normal;
		// 壁か地面にフックショットに当たった場合
		if ((CollisionManager::GetInstance()->CheckEarthCollision(weapon_btm_pos, weapon_top_pos, &hitpos, &normal) != NOT_ON_COLLISION) ||
			(CollisionManager::GetInstance()->CheckWallCollision(weapon_btm_pos, weapon_top_pos, &hitpos, &normal))) {
			is_hit = TRUE;
		}

		TotalInfo::GetInstance()->EnableLockonLayout(&weapon_top_pos, is_hit);

		if (CHECK_PRESS_JOY(JOY_INPUT_SUB_WEAPON) == false) {
			TotalInfo::GetInstance()->DisableLockonLayout();
			ChangeState(eCHARA_STATE_HOOK_SHOTTING);
			return;
		}
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_HOOKSHOT);
	}

	// チェーンモデルをアタッチ
	updateHookshotAttach(p_pmd);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateHookShotStart(void) 
{
	// 次のモーション以外の場合
	if (mCharaState != eCHARA_STATE_HOOK_SHOTTING) {

		VECTOR vec = VNorm(sHookShotTargetVec);
		mRotation.y = atan2(vec.x, vec.z);

		this->SetMatrixType(ModelBase::eMATRIX_TYPE_TRS);

		GraphicManager::GetInstance()->ReserveRemoveModel(&sHookshotChain);

		CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_HIGH, true);
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);

		TotalInfo::GetInstance()->DisableLockonLayout();
		
		// アタッチパターンを変更
		ChangeAttachPattern(eATTACH_PATTERN_NORMAL);
	}

	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateHookShotting(void)
{
	SoundManager::GetSndComponent()->PlaySound(eSE_HOOK_SHOTTING);

	mAttackPower = 1;

	ModelPmd::tAttachWork* p_attach = GetAttachAddr(eATTACH_MODEL_SUB_WEAPON);
	ModelPmd* p_pmd = (ModelPmd *)p_attach->mpAttachedModel;
	p_pmd->StartMotion(1, false, 0.3f, NULL, 0.05f);
	sHookshotChain.StartMotion(1, false, 0.3f, NULL, 0.05f);
	
	// チェーンモデルをアタッチ
	updateHookshotAttach(p_pmd);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateHookShotting(void)
{
	CharaBase* p_mine = this;
	CharaBase* p_your = NULL;
	bool is_end_hook = false;

	ModelPmd::tAttachWork* p_attach = GetAttachAddr(eATTACH_MODEL_SUB_WEAPON); // 1 がフックショットの番号と決め付けて処理
	int model_handle = p_attach->mpAttachedModel->GetModelHandle();
	VECTOR weapon_btm_pos, weapon_top_pos;
	
	ModelPmd* p_pmd = (ModelPmd *)p_attach->mpAttachedModel;
	bool result;

	result = p_pmd->GetFramePosFromName(&weapon_btm_pos, strHookShotBottom);
	APP_ASSERT(result, "Failed ModelPmd::GetFramePosFromName\n");

	result = p_pmd->GetFramePosFromName(&weapon_top_pos, strHookShotHead);
	APP_ASSERT(result, "Failed ModelPmd::GetFramePosFromName\n");

	for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
		p_your = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
		if (p_mine == p_your) continue;			// 自分はのぞく
		if (p_your->GetIsRetire()) continue;	// 戦闘不能者も除く
		if (p_your->IsDamage()) continue;		// 既に攻撃された者も除く

		tCheckDamageWork param;
		param.mPlayerPos = p_mine->Position();
		param.mWeaponBtmPos = weapon_btm_pos;
		param.mWeaponTopPos = weapon_top_pos;

		if (p_your->CheckDamageCollLonghook(&param)) {
			// 本来の処理
			SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			
			VECTOR eff_pos = weapon_top_pos;
			VECTOR damage_down_vec;
			damage_down_vec = VSub(p_your->Position(), mPosition);
			damage_down_vec = VNorm(damage_down_vec);
			p_your->ProcessDamage(mAttackPower, false, &eff_pos, &damage_down_vec, false, eATTACK_TYPE_LONGHOOK);
			is_end_hook = true;
		}
	}

	VECTOR hitpos, normal;

	float stretch_rate = ((VSize(VSub(weapon_top_pos, weapon_btm_pos))) / kHOOKSHOT_LENGTH);	// MAX伸び率を求める
	float blend_value = 0.02f + (0.06f * (1.0f - stretch_rate));

	// 壁か地面にフックショットに当たった場合
	if ((CollisionManager::GetInstance()->CheckEarthCollision(weapon_btm_pos, weapon_top_pos, &hitpos, &normal) != NOT_ON_COLLISION) ||
		(CollisionManager::GetInstance()->CheckWallCollision(weapon_btm_pos, weapon_top_pos, &hitpos, &normal))) {

		int eff_handle;
		eff_handle = EffectManager::GetInstance()->Play(eEF_DUST, &hitpos, 30.0f, 0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 0.75f);

		SoundManager::GetSndComponent()->PlaySound(eSE_HIT_ARROW);

		/* テスト的な処理・全壁・床にひっつく仕様 */
		// フックショットにつられて動く
		sHookHitStartPos= Position();
		sHookHitEndPos	= weapon_top_pos;

		mJumpMoveVec.mDash.x = blend_value;// とりあｘ軸に
		p_pmd->StartMotion(0, false, 0.0f, NULL, blend_value);
		sHookshotChain.StartMotion(0, false, 0.0f, NULL, blend_value);

		SoundManager::GetSndComponent()->StopSound(eSE_HOOK_SHOTTING);
		SoundManager::GetSndComponent()->PlaySound(eSE_HOOK_SHOTTING);

		StartMotionEx(&sMotionParam[eMT_HOOK_MOVE]);
			
		CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_HIGH, true);
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);

		ChangeState(eCHARA_STATE_HOOK_MOVING);
		return;

	}

	// フックショットが伸びきった場合
	if (p_pmd->GetIsEndMotion()) {
		is_end_hook = true;

	}

	if (is_end_hook) {
		this->EnableShadow();

		mJumpMoveVec.mDash.x = blend_value;// とりあｘ軸に
		p_pmd->StartMotion(0, false, 0.0f, NULL, blend_value);
		sHookshotChain.StartMotion(0, false, 0.0f, NULL, blend_value);

		SoundManager::GetSndComponent()->StopSound(eSE_HOOK_SHOTTING);
		SoundManager::GetSndComponent()->PlaySound(eSE_HOOK_SHOTTING);

		ChangeState(eCHARA_STATE_HOOK_SHOT_END);
	}

	// チェーンモデルをアタッチ
	updateHookshotAttach(p_pmd);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateHookShoting(void) 
{
	// 次のモーション以外の場合
	if (mCharaState != eCHARA_STATE_HOOK_SHOT_END) {
		VECTOR vec = VNorm(sHookShotTargetVec);
		mRotation.y = atan2(vec.x, vec.z);

		this->SetMatrixType(ModelBase::eMATRIX_TYPE_TRS);
		GraphicManager::GetInstance()->ReserveRemoveModel(&sHookshotChain);

	}

	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateHookShotEnd(void)
{
	ModelPmd::tAttachWork* p_attach = GetAttachAddr(eATTACH_MODEL_SUB_WEAPON);
	ModelPmd* p_pmd = (ModelPmd *)p_attach->mpAttachedModel;

	// チェーンモデルをアタッチ
	updateHookshotAttach(p_pmd);

	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateHookShotEnd(void)
{
	ModelPmd::tAttachWork* p_attach = GetAttachAddr(eATTACH_MODEL_SUB_WEAPON);
	ModelPmd* p_pmd = (ModelPmd *)p_attach->mpAttachedModel;
	VECTOR vec;

	// モーションブレンドの終了まで待つ
	if (p_pmd->GetIsMotionBlend() == false) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HOOK_END);

		vec = VNorm(sHookShotTargetVec);
		mRotation.y = atan2(vec.x, vec.z); // Mtx指定からTRS指定時にMtx時のy軸の角度を設定してやる
		this->SetMatrixType(ModelBase::eMATRIX_TYPE_TRS);
		
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
		CameraManager::GetInstance()->ResetCameraPosY();

		ChangeState(eCHARA_STATE_IDLE);
	}

	// チェーンモデルをアタッチ
	updateHookshotAttach(p_pmd);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateHookShotEnd(void)
{
	// 次のモーション以外の場合
	if (mCharaState != eCHARA_STATE_HOOK_MOVING) {
		VECTOR vec = VNorm(sHookShotTargetVec);

		mRotation.y = atan2(vec.x, vec.z);

		this->SetMatrixType(ModelBase::eMATRIX_TYPE_TRS);
		GraphicManager::GetInstance()->ReserveRemoveModel(&sHookshotChain);

		// アタッチパターンを変更
		ChangeAttachPattern(eATTACH_PATTERN_NORMAL);
	}

	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateHookMoving(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateHookMoving(void)
{
	bool result = false;
	VECTOR weapon_top_pos;

	CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_HIGH, true);

	ModelPmd::tAttachWork* p_attach = GetAttachAddr(eATTACH_MODEL_SUB_WEAPON);
	ModelPmd* p_pmd = (ModelPmd *)p_attach->mpAttachedModel;
	result = p_pmd->GetFramePosFromName(&weapon_top_pos, strHookShotHead);
	APP_ASSERT(result, "Failed ModelPmd::GetFramePosFromName\n");

	VECTOR vec;
	vec = VScale(VSub(sHookHitEndPos, sHookHitStartPos), mJumpMoveVec.mDash.x); // とりあで設定したｘ軸から
	PRINT_CONSOLE("mJumpMoveVec.mDash.x : %.2f \n", mJumpMoveVec.mDash.x);
	mGravity = 0.0f;
	mMoveVec = vec;

	mMatrix.m[3][0] = mPosition.x + mMoveVec.x;
	mMatrix.m[3][1] = mPosition.y + mMoveVec.y;
	mMatrix.m[3][2] = mPosition.z + mMoveVec.z;

	if (CHECK_TRIG_JOY(JOY_INPUT_JUMP)) {
		mJumpMoveVec.mDash = VScale(vec, 0.25f);
		mJumpMoveVec.mDash.y *= 1.2f; // ちょっとだけＹ軸に浮かす

		vec = VNorm(sHookShotTargetVec);
		mRotation.y = atan2(vec.x, vec.z); // Mtx指定からTRS指定時にMtx時のy軸の角度を設定してやる
		this->SetMatrixType(ModelBase::eMATRIX_TYPE_TRS);

		this->EnableShadow();

		ChangeState(eCHARA_STATE_JUMP_UP);
	}

	// モーションブレンドの終了まで待つ
	if (p_pmd->GetIsMotionBlend() == false) {

		vec = VNorm(sHookShotTargetVec);
		mRotation.y = atan2(vec.x, vec.z); // Mtx指定からTRS指定時にMtx時のy軸の角度を設定してやる
		this->SetMatrixType(ModelBase::eMATRIX_TYPE_TRS);

		SoundManager::GetSndComponent()->PlaySound(eSE_HOOK_END);

		p_pmd->StartMotionForcibly(0, true, 0.5f, NULL, kHOOK_REVERSE_SPEED);
		sHookshotChain.StartMotionForcibly(0, true, 0.5f, NULL, kHOOK_REVERSE_SPEED);

		this->EnableShadow();

		ChangeState(eCHARA_STATE_JUMP_DOWN);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateHookMoving(void)
{
	GraphicManager::GetInstance()->ReserveRemoveModel(&sHookshotChain);

	// アタッチパターンを変更
	ChangeAttachPattern(eATTACH_PATTERN_NORMAL);

	return;
}

/*=====================================*
 * 回避関連
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateAwayRight(void)
{
	StartMotionEx(&sMotionParam[eMT_AWAY_RIGHT]);
	mBladeEffect.Stop();

	SoundManager::GetSndComponent()->PlaySound(eSE_AWAY);

	if (mpObjectHand != NULL) {
		unprotectAllEnemyDamageFlag();
		mpObjectHand->ProcessThrow((VECTOR *)&ZERO_VEC);
	}

	addPiyoPiyoGauge(-800);

	mIsAway = true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateAwayRight(void)
{
	VECTOR dir_vec;
	float move_speed = kCHARA_AWAY_SPEED * (float)(100 - GetMotionTimePercent());

	dir_vec.x = sin(mRotation.y + Utility::DegToRad(-90.0f));
	dir_vec.y = 0.0f;
	dir_vec.z = cos(mRotation.y + Utility::DegToRad(-90.0f));

	mMoveVec = VScale(dir_vec, move_speed);
	
	if ((mpLockonChara != NULL) &&(CHECK_TRIG_JOY(JOY_INPUT_ATTACK))) {
		if (ModelPmd::GetMotionTimePercent() < 60) {
			ChangeState(eCHARA_STATE_ATTACK_REAR);
		}
	}

	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}

	if (mpLockonChara != NULL) {
		CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_HIGH, true);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateAwayRight(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateAwayLeft(void)
{
	StartMotionEx(&sMotionParam[eMT_AWAY_LEFT]);
	mBladeEffect.Stop();

	SoundManager::GetSndComponent()->PlaySound(eSE_AWAY);

	if (mpObjectHand != NULL) {
		unprotectAllEnemyDamageFlag();
		mpObjectHand->ProcessThrow((VECTOR *)&ZERO_VEC);
	}

	addPiyoPiyoGauge(-800);

	mIsAway = true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateAwayLeft(void)
{
	VECTOR dir_vec;
	float move_speed = kCHARA_AWAY_SPEED * (float)(100 - GetMotionTimePercent());

	dir_vec.x = sin(mRotation.y + Utility::DegToRad(90.0f));
	dir_vec.y = 0.0f;
	dir_vec.z = cos(mRotation.y + Utility::DegToRad(90.0f));

	mMoveVec = VScale(dir_vec, move_speed);
	
	if ((mpLockonChara != NULL) &&(CHECK_TRIG_JOY(JOY_INPUT_ATTACK))) {
		if (ModelPmd::GetMotionTimePercent() < 60) {
			ChangeState(eCHARA_STATE_ATTACK_REAR);
		}
	}

	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}

	if (mpLockonChara != NULL) {
		CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_HIGH, true);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateAwayLeft(void) 
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateAwayBack(void)
{
	StartMotionEx(&sMotionParam[eMT_AWAY_BACK]);
	mBladeEffect.Stop();

	SoundManager::GetSndComponent()->PlaySound(eSE_AWAY);

	if (mpObjectHand != NULL) {
		unprotectAllEnemyDamageFlag();
		mpObjectHand->ProcessThrow((VECTOR *)&ZERO_VEC);
	}

	addPiyoPiyoGauge(-800);

	mIsAway = true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateAwayBack(void)
{
	VECTOR dir_vec;
	float move_speed = kCHARA_AWAY_SPEED * (float)(100 - GetMotionTimePercent());

	dir_vec.x = sin(mRotation.y + Utility::DegToRad(0.0f));
	dir_vec.y = 0.0f;
	dir_vec.z = cos(mRotation.y + Utility::DegToRad(0.0f));

	mMoveVec = VScale(dir_vec, move_speed);
	
	if (CHECK_TRIG_JOY(JOY_INPUT_ATTACK)) {
		ChangeState(eCHARA_STATE_ATTACK_ROLING_KICK);
	}

	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}

	if (mpLockonChara != NULL) {
		CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_HIGH, true);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateAwayBack(void) 
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStatePutSword(void)
{
	StartMotionEx(&sMotionParam[eMT_PUT_SWORD]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStatePutSword(void)
{
	const float sound_secA = 18.0f;

	// 走り・歩きでの足音を鳴らす
	if (GetMotionIndex() == eMT_PUT_SWORD) {
		if ((mMotion.mNowTime <= sound_secA) && (mMotion.mNowTime + mMotion.mSpeed > sound_secA)) {
			
			SoundManager::GetSndComponent()->PlaySound(eSE_PUT_SWORD);
			this->SetActionLimit(eACTION_NG);

			ChangeAttachPattern(eATTACH_PATTERN_NORMAL);

			// SetActionLimit()でアイコンが一時的に表示されるため
			TotalInfo::GetInstance()->HideBattleIcon();
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateBoomerangStart(void)
{
	tMotionParam motion = {eMT_BOOMERANG, false, 0.0f, NULL};
	this->StartMotionEx(&motion);

	sBoomerangTargetVec.x = -(sin(mRotation.y));
	sBoomerangTargetVec.y = 0.0f;
	sBoomerangTargetVec.z = -(cos(mRotation.y));

	// アタッチパターンを変更
	ChangeAttachPattern(eATTACH_PATTERN_BOOMERANG);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateBoomerangStart(void)
{
	static const  float kBOOMERANG_RELEASE_FRAME = 3.5f;

	ModelPmd::tAttachWork* p_attach = GetAttachAddr(eATTACH_MODEL_SUB_WEAPON);
	ModelPmd* p_pmd = (ModelPmd *)p_attach->mpAttachedModel;

	// 構えまでブレンドで待つ
	if ((mMotion.mSpeed == 0.0f) && (mMotion.mIsBlend == false)) {

		if (mpLockonChara == NULL) {
			// ジョイパッドとマウスの両方から武器のターゲットの向きを求める
			updateWeaponTargetVecFromInput(&sBoomerangTargetVec, -0.5f, 0.5f);

			mRotation.y = atan2(sBoomerangTargetVec.x, sBoomerangTargetVec.z) + DX_PI;

			CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_HIGH, true);

			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_BOOMERANG);

		} else {
			/* ロックオンあり時の処理 */
			VECTOR tmpA, tmpB;
			tmpA = mPosition;
			tmpA.y += (mBodyHeight * 0.5f);
			mpLockonChara->GetLockonPosition(&tmpB);
			tmpB.y += (mpLockonChara->BodyHeight() * 0.5f);
			sBoomerangTargetVec = VNorm(VSub(tmpB, tmpA));

			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_LOCKON);
		}

		VECTOR end_pos;
		end_pos = mPosition;
		end_pos.y += (mBodyHeight * 0.5f);
		end_pos = VAdd(end_pos, VScale(sBoomerangTargetVec, 2678.668f));
		TotalInfo::GetInstance()->EnableLockonLayout(&end_pos);

		mCameraGeneralVec = end_pos;	// カメラベクトルを指定

		Utility::CorrectAngleFrom000To360(&mRotation.y);

		// キーを離すと発射
		if (CHECK_PRESS_JOY(JOY_INPUT_SUB_WEAPON) == false) {

			SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_ATTACK_1);
			SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
			mMotion.mSpeed = 0.5f;

			TotalInfo::GetInstance()->DisableLockonLayout();

			unprotectAllEnemyDamageFlag();
		}
	}

	// ブーメランの投げ開始 
	if ((mMotion.mNowTime <= kBOOMERANG_RELEASE_FRAME) && (mMotion.mNowTime + mMotion.mSpeed > kBOOMERANG_RELEASE_FRAME)) {
		p_pmd->StartMotion(1, true, 4.0f);

		PauseAttach(eATTACH_MODEL_SUB_WEAPON);
		MATRIX mtx = p_pmd->Matrix();
		p_pmd->Position() = VTransform(ZERO_VEC, p_pmd->Matrix());
		p_pmd->Scale() = this->mScale;

		sBoomerang.mState = eBOOMERANG_STATE_FOWARD_NO_DAMAGE;
		PRINT_CONSOLE("Line : %d \n", __LINE__);
		sBoomerang.mMoveVec = VNorm(sBoomerangTargetVec);
		sBoomerang.mMoveVec = VScale(sBoomerang.mMoveVec, 80.0f);
		sBoomerang.mRevVec = VScale(sBoomerang.mMoveVec, -0.015f);
		mSubweaponBladeEffect.Play(&p_pmd->Position(), &VAdd(p_pmd->Position(), sBoomerang.mMoveVec));

		/* サブウェポン実行関数を登録 */
		mpSubweaponFunc		= &CharaPlayer::subweaponFuncBoomerang;
	}

	// モーション終了の待ち
	if (GetIsEndMotion()) {
		if (sBoomerang.mState != eBOOMERANG_STATE_NONE) {
			mPhaseEnterState[eCHARA_STATE_SUBWEAPON_START]	= &CharaPlayer::phaseEnterStateNothing;
			mPhaseState[eCHARA_STATE_SUBWEAPON_START]		= &CharaPlayer::phaseUpdateStateNothing;
		}
		PRINT_CONSOLE("Line : %d \n", __LINE__);

		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateBoomerangStart(void) 
{
	// アタッチパターンを変更
	ChangeAttachPattern(eATTACH_PATTERN_NORMAL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateAwayFront(void)
{
	StartMotionEx(&sMotionParam[eMT_AWAY_FRONT]);
	mBladeEffect.Stop();

	SoundManager::GetSndComponent()->PlaySound(eSE_AWAY);

	if (mpObjectHand != NULL) {
		unprotectAllEnemyDamageFlag();
		mpObjectHand->ProcessThrow((VECTOR *)&ZERO_VEC);
	}

	addPiyoPiyoGauge(-800);

	mIsAway = true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateAwayFront(void)
{
	VECTOR dir_vec;
	float move_speed = kCHARA_AWAY_SPEED * (float)(100 - GetMotionTimePercent());
	if (sIsAnalogInputRun) {
		move_speed += (mRunSpeed / 3.0f);
	}

	if (mpLockonChara != NULL) {
		// ロックオン中は
		VECTOR vecZ, crossX;
		VECTOR tmp;
		
		VECTOR lockon_chara;
		mpLockonChara->GetLockonPosition(&lockon_chara);
		
		vecZ = VNorm(VSub(lockon_chara, mPosition));
		crossX = VNorm(VCross(vecZ, sUpVec));
		tmp = ZERO_VEC;
		if (CHECK_PRESS_JOY(JOY_INPUT_LEFT)) {
			tmp = VScale(crossX, -1);
		} else if (CHECK_PRESS_JOY(JOY_INPUT_RIGHT)) {
			tmp = crossX;
		}
		if (CHECK_PRESS_JOY(JOY_INPUT_DOWN)) {
			tmp = VAdd(tmp, vecZ);
		} else if (CHECK_PRESS_JOY(JOY_INPUT_UP)) {
			tmp = VAdd(tmp, VScale(vecZ, -1));
		}
		if (VSize(tmp) > 0.0f) {
			tmp = VNorm(tmp);
			mRotation.y = atan2(tmp.x, tmp.z);
		}
	}

	dir_vec.x = sin(mRotation.y + Utility::DegToRad(180.0f));
	dir_vec.y = 0.0f;
	dir_vec.z = cos(mRotation.y + Utility::DegToRad(180.0f));

	mMoveVec = VScale(dir_vec, move_speed);
	
	if (CHECK_TRIG_JOY(JOY_INPUT_ATTACK)) {
		ChangeState(eCHARA_STATE_ATTACK_SMASH_START);
	}

	if (CHECK_TRIG_JOY(JOY_INPUT_JUMP)) {
		ChangeState(eCHARA_STATE_JUMP_UP);
	}

	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}

	if (mpLockonChara != NULL) {
		CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_HIGH, true);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateAwayFront(void) 
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateAttackJump(void)
{
	unprotectAllEnemyDamageFlag();

	mIsJumpAttack = true;

	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	if (mCosplayIndex == eWINMENU_COSPLAY_TITANIS) {
		mGravity *= 0.8f;

		StartMotion(eMT_ATTACK_JUMP, false, 1.75f);
	}
	else {
		mGravity *= 0.5f;

		SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_ATTACK_1);
		SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

		StartMotionEx(&sMotionParam[eMT_ATTACK_JUMP]);
	}

	addPiyoPiyoGauge(-900);

	PRINT_CONSOLE("mAttackJump\n");
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateAttackJump(void)
{
	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);

	mMoveVec = VScale(mJumpMoveVec.mDash, 0.5f);
	mGravity *= 0.5f;

	bool is_prev_attack;
	is_prev_attack = mIsAttack;
	if ((GetMotionNowTime() >= 0.0f) &&
		(GetMotionNowTime() <= 20.0f)) {
		mAttackPower = 1;
		mIsAttack = true;
	} else {
		mIsAttack = false;
	}
	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (ModelPmd::GetIsEndMotion()) {
		mBladeEffect.Stop();
		ChangeState(eCHARA_STATE_JUMP_DOWN);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateSleep(void)
{
	StartMotion(eMT_IDLE_SLEEP, true, 0.75f, NULL, 0.03f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateSleep(void)
{
	eTurnType turn_type = eTURN_TYPE_NONE;

	turn_type = updateTargetRotateFromPad();
	if ((CHECK_TRIG_ANY_JOY()) ||
		(turn_type != eTURN_TYPE_NONE)) {
		ChangeState(eCHARA_STATE_IDLE);
		return;
	}

	phaseUpdateStateIdle();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateSmile(void)
{
	// アタッチパターンを変更
	ChangeAttachPattern(eATTACH_PATTERN_HAND);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateSmile(void)
{
	for (int iI = 0; iI < 6; iI ++) {
		if (CHECK_TRIG_KEY(eKEY_0 + iI)) {
			if (TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_SMILE_0 + iI)) {
				StartMotionEx(&sMotionParam[(eMT_SMILE_00 + iI)]);
				ChangeState(eCHARA_STATE_SMILE);
				break;
			}
		}
	}

	eTurnType turn_type = eTURN_TYPE_NONE;
	turn_type = updateTargetRotateFromPad();
	if ((CHECK_TRIG_ANY_JOY()) ||
		(turn_type != eTURN_TYPE_NONE)) {
		ChangeState(eCHARA_STATE_IDLE);
	}

	if (GetIsEndMotion()) {
		int motion_index = GetMotionIndex();
		unsigned long check_bit = (1 << (motion_index - eMT_SMILE_00 + 1));
		if (((motion_index >= eMT_SMILE_06) && (motion_index <= eMT_SMILE_08)) &&
			(TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_DANCE)) ) {
			StartMotionEx(&sMotionParam[(motion_index + 1)]);
			ChangeState(eCHARA_STATE_SMILE);
		} else {
			ChangeState(eCHARA_STATE_IDLE);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateSmile(void) 
{
	// アタッチパターンを変更
	ChangeAttachPattern(eATTACH_PATTERN_NORMAL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateWaitResetCamera(void)
{
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateWaitResetCamera(void)
{
	if (CameraManager::GetInstance()->GetCameraState() != eCAM_STATE_LOOK_AROUND) {
		ChangeState(sResetCameraBeforeState);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateLibShieldStart(void)
{
	if (mCharaState != eCHARA_STATE_LIB_SHIELD) {
		ChangeState(eCHARA_STATE_LIB_SHIELD);
		return;
	}

	unprotectAllEnemyDamageFlag();

	StartMotion(eMT_SHIELD_GUARD, true, 0.25f);

	ModelPmd* p_attach = (ModelPmd *)this->GetAttachAddr(eATTACH_MODEL_SHIELD)->mpAttachedModel;
	if (MV1GetMeshVisible(p_attach->GetModelHandle(), LIBSHIELD_LIGHT_MESH_IDX) == TRUE) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HOLY);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateLibShieldStart(void)
{
	if (this->GetIsMotionBlend() == false) {

		if (CHECK_PRESS_JOY(JOY_INPUT_AWAY) == false) {

			ChangeState(eCHARA_STATE_IDLE);
			return;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateRaiseIdle(void)
{
	APP_ASSERT((mpObjectHand != NULL), "mpObjectHand is NULL \n");

	ModelBase* p_model = NULL;
	StartMotion(eMT_RAISE_IDLE, true, 0.25f);

	// アタッチパターンを変更
	ChangeAttachPattern(eATTACH_PATTERN_RAISE_HAND);

	mIsLeaveWeapon = true;		// 武器手放しフラグを有効にする

	// 爆弾を持たせるマクロ
	RAISE_ATTACH_MODEL_MACRO();

	// 持ってるモノをロックオンしてる場合はロックオン解除
	if (mpObjectHand == mpLockonChara) {
		DisableLockonChara();
	}

	mpObjectHand->Scale() = this->mScale;
}

//---------------------------------------------------
/**
 * ※ phaseUpdateStateIdle の処理をコピーしている為、
 *	どちらかを編集する際は注意すること！
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateRaiseIdle(void)
{
	APP_ASSERT((mpObjectHand != NULL), "mpObjectHand is NULL \n");

	// 爆弾を持たせるマクロ
	RAISE_ATTACH_MODEL_MACRO();

	eTurnType turn_type = eTURN_TYPE_NONE;

	turn_type = updateTargetRotateFromPad();
	mTurnType = turn_type;

	if (CHECK_TRIG_JOY(JOY_INPUT_ATTACK)) {
		// 投げる処理
		ChangeState(eCHARA_STATE_RAISE_THROW);
	} else if (CHECK_TRIG_JOY(JOY_INPUT_SUB_WEAPON)) {
		// nop
	} else if (CHECK_TRIG_JOY(JOY_INPUT_AWAY)) {
		
		ChangeState(eCHARA_STATE_RAISE_PUT);

	} else if (turn_type != eTURN_TYPE_NONE) {
		Utility::CorrectAngleFrom000To360(&mTargetRotY);
		ChangeState(eCHARA_STATE_RAISE_TURN);
	}

	// カメラリセット
	if (CHECK_TRIG_JOY(JOY_INPUT_CAM_RESET)) {
		mpLockonChara = NULL;
		CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_MIDDLE, true);
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_RESET);
		sResetCameraBeforeState = mCharaState;
		ChangeState(eCHARA_STATE_WAIT_RESET_CAMERA);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateRaiseIdle(void) 
{
	// 次のモーション以外の場合
	if ((mCharaState != eCHARA_STATE_RAISE_TURN)  &&
		(mCharaState != eCHARA_STATE_RAISE_THROW) &&
		(mCharaState != eCHARA_STATE_RAISE_PUT)   &&
		(mCharaState != eCHARA_STATE_WAIT_RESET_CAMERA)   &&
		(mCharaState != eCHARA_STATE_RAISE_MOVE)) {
		mpObjectHand->ProcessThrow((VECTOR *)&ZERO_VEC);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateRaiseTurn(void)
{
	// 爆弾を持たせるマクロ
	RAISE_ATTACH_MODEL_MACRO();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateRaiseTurn(void)
{
	APP_ASSERT((mpObjectHand != NULL), "mpObjectHand is NULL \n");

	// 爆弾を持たせるマクロ
	RAISE_ATTACH_MODEL_MACRO();

	/* キャラクターの向きを処理する */
	Utility::CorrectAngleFrom000To360(&mRotation.y);

	float diff_rot = mTargetRotY - mRotation.y;
	eTurnType turn_type = eTURN_TYPE_NONE;
	
	if (kTURN_MOVE_ROT >= abs(diff_rot)) {
		mRotation.y = mTargetRotY;

		turn_type = updateTargetRotateFromPad();
		mTurnType = turn_type;

		if (turn_type != eTURN_TYPE_NONE) {
			ChangeState(eCHARA_STATE_RAISE_MOVE);
		} else {
			ChangeState(eCHARA_STATE_RAISE_TURN);
		}

	} else {
		bool is_plus = (diff_rot >= 0.0f) ? true : false;
		if (abs(diff_rot) > Utility::DegToRad(180.0f)) {
			is_plus ^= true;
		}
		mRotation.y += (is_plus) ? kTURN_MOVE_ROT : -kTURN_MOVE_ROT;
	}

	if (CHECK_TRIG_JOY(JOY_INPUT_ATTACK)) {
		// 投げる処理
		ChangeState(eCHARA_STATE_RAISE_THROW);
	} else if (CHECK_TRIG_JOY(JOY_INPUT_SUB_WEAPON)) {
		// nop
	} else {

	}

	if (CHECK_TRIG_JOY(JOY_INPUT_CAM_RESET)) {
		mpLockonChara = NULL;
		CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_MIDDLE, true);
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_RESET);
		sResetCameraBeforeState = mCharaState;
		ChangeState(eCHARA_STATE_WAIT_RESET_CAMERA);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateRaiseTurn(void) 
{
	// 次のモーション以外の場合
	if ((mCharaState != eCHARA_STATE_RAISE_IDLE)  &&
		(mCharaState != eCHARA_STATE_RAISE_THROW) &&
		(mCharaState != eCHARA_STATE_RAISE_PUT)   &&
		(mCharaState != eCHARA_STATE_WAIT_RESET_CAMERA)   &&
		(mCharaState != eCHARA_STATE_RAISE_MOVE)) {
		mpObjectHand->ProcessThrow((VECTOR *)&ZERO_VEC);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateRaiseMove(void)
{
	// 爆弾を持たせるマクロ
	RAISE_ATTACH_MODEL_MACRO();

	mRunSpeed = kCHARA_WALK_SPEED;
	StartMotionEx(&sMotionParam[eMT_RAISE_WALK]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateRaiseMove(void)
{
	// 爆弾を持たせるマクロ
	RAISE_ATTACH_MODEL_MACRO();

	eTurnType turn_type = eTURN_TYPE_NONE;

	VECTOR dir_vec = ZERO_VEC;
	VECTOR org_vec, org_cross_vec, side_vec;

	// キャラクタの向き
	turn_type = updateTargetRotateFromPad();
	Utility::CorrectAngleFrom000To360(&mTargetRotY);

	org_vec = VSub(CameraManager::GetInstance()->Target(), CameraManager::GetInstance()->Position());
	org_vec.y = 0.0f;
	org_vec = VNorm(org_vec);
	org_cross_vec = VCross(sUpVec, org_vec);

	if (CHECK_TRIG_JOY(JOY_INPUT_ATTACK)) {
		// 投げる処理
		ChangeState(eCHARA_STATE_RAISE_THROW);
		return;
	} else if (turn_type != eTURN_TYPE_NONE) {
		// 押された向きが変わっている又は隣合う向きでない場合、ターンのステータスへ
		int diff = abs(mTurnType - turn_type);

		if (mTurnType != turn_type) {
			mRunSpeed -= (kCHARA_ADD_MOVE_SPEED * 10.0f);
			if (mRunSpeed <= kCHARA_WALK_SPEED) {
				mRunSpeed = kCHARA_WALK_SPEED;
			}

		}
		mTurnType = turn_type;

		if (CHECK_PRESS_JOY(JOY_INPUT_UP)) {
			dir_vec = VAdd(dir_vec, org_vec);
		} else if (CHECK_PRESS_JOY(JOY_INPUT_DOWN)) {
			dir_vec = VAdd(dir_vec, VScale(org_vec, -1));
		}
		if (CHECK_PRESS_JOY(JOY_INPUT_RIGHT)) {
			dir_vec = VAdd(dir_vec, org_cross_vec);
		} else if (CHECK_PRESS_JOY(JOY_INPUT_LEFT)) {
			dir_vec = VAdd(dir_vec, VScale(org_cross_vec, -1));
		}
		dir_vec = VNorm(dir_vec);
		Utility::CorrectAngleFrom000To360(&mRotation.y);
		mTargetRotY = atan2(dir_vec.x, dir_vec.z) + DX_PI;
		float diff_rot = mTargetRotY - mRotation.y;
		if (kTURN_MOVE_ROT >= abs(diff_rot)) {
			mRotation.y = mTargetRotY;
		} else {
			bool is_plus = (diff_rot >= 0.0f) ? true : false;
			if (abs(diff_rot) > Utility::DegToRad(180.0f)) {
				is_plus ^= true;
			}
			mRotation.y += (is_plus) ? kTURN_MOVE_ROT * 0.5f : -kTURN_MOVE_ROT * 0.5f;
		}
	} else {
		ChangeState(eCHARA_STATE_RAISE_IDLE);
		return;
	}

	side_vec = VCross(sUpVec, dir_vec);
	dir_vec = VCross(side_vec, mNormalVec);
	
	// 足音
	float sound_secA = 0.0f;
	float sound_secB = 0.0f;

	if ((CHECK_PRESS_JOY(JOY_INPUT_RUN) || sIsAnalogInputRun) &&
		(GetMotionIndex() != eMT_WALK)) {
		/* 走りの処理 */
		mRunSpeed += kCHARA_ADD_MOVE_SPEED;
		if (mRunSpeed >= (kCHARA_WALK_SPEED * 2.0f)) {
			mRunSpeed = (kCHARA_WALK_SPEED * 2.0f);
		}
		StartMotion(eMT_RAISE_WALK, true, 1.0f, NULL);
		SetMotionSpeed(1.0f);
	} else {
		/* 歩きの処理 */
		mRunSpeed = kCHARA_WALK_SPEED;
		StartMotion(eMT_RAISE_WALK, true, 0.5f, NULL);
		SetMotionSpeed(0.5f);
	}
	mMoveVec = VScale(dir_vec, mRunSpeed);
	sound_secA = WALK_SOUND_SECOND_A;
	sound_secB = WALK_SOUND_SECOND_B;

	// 走り・歩きでの足音を鳴らす
	if ((mMotion.mNowTime <= sound_secA) && (mMotion.mNowTime + mMotion.mSpeed > sound_secA)) {
		UPDATE_FOOTSTEP_KIND();
		SoundManager::GetSndComponent()->PlaySound(sFootSoundTable[sFootstepKind][eFOOT_SND_STEP_A]);
	}
	if ((mMotion.mNowTime <= sound_secB) && (mMotion.mNowTime + mMotion.mSpeed > sound_secB)) {
		UPDATE_FOOTSTEP_KIND();
		SoundManager::GetSndComponent()->PlaySound(sFootSoundTable[sFootstepKind][eFOOT_SND_STEP_B]);
	}

	// カメラリセット
	if (CHECK_TRIG_JOY(JOY_INPUT_CAM_RESET)) {
		mpLockonChara = NULL;
		CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_MIDDLE, true);
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_RESET);
		sResetCameraBeforeState = mCharaState;
		ChangeState(eCHARA_STATE_WAIT_RESET_CAMERA);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateRaiseMove(void) 
{
	// 次のモーション以外の場合
	if ((mCharaState != eCHARA_STATE_RAISE_IDLE)  &&
		(mCharaState != eCHARA_STATE_RAISE_THROW) &&
		(mCharaState != eCHARA_STATE_RAISE_PUT)   &&
		(mCharaState != eCHARA_STATE_WAIT_RESET_CAMERA)   &&
		(mCharaState != eCHARA_STATE_RAISE_TURN)) {
		mpObjectHand->ProcessThrow((VECTOR *)&ZERO_VEC);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateRaiseThrow(void)
{
	// 爆弾を持たせるマクロ
	RAISE_ATTACH_MODEL_MACRO();

	unprotectAllEnemyDamageFlag();

	StartMotion(eMT_RAISE_THROW, false, 0.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateRaiseThrow(void)
{
	if (this->EqualPhaseFrame(17.0f)) {
		// 爆弾投げ開始
		ModelPmd::tAttachWork* p_attach = GetAttachAddr(eATTACH_MODEL_SUB_WEAPON);
		ModelPmd* p_pmd = (ModelPmd *)p_attach->mpAttachedModel;

		PauseAttach(eATTACH_MODEL_SUB_WEAPON);
		MATRIX mtx = p_pmd->Matrix();
		p_pmd->Position() = VTransform(ZERO_VEC, p_pmd->Matrix());
		p_pmd->Scale() = this->mScale;
		
		VECTOR throw_vec = {0};
		if (mpLockonChara == NULL) {
			throw_vec.x = sin(mRotation.y + Utility::DegToRad(180.0f));
			throw_vec.y = 0.0f;
			throw_vec.z = cos(mRotation.y + Utility::DegToRad(180.0f));
			throw_vec = VScale(throw_vec, (mMaxRunSpeed * 1.2f));
			throw_vec.y = (kJUMP_GRAVITY * 0.3f);
		}
		else {
			VECTOR lockon_chara;
			mpLockonChara->GetLockonPosition(&lockon_chara);
			throw_vec = VSub(lockon_chara, mPosition);
			throw_vec = VScale(VNorm(throw_vec), (mMaxRunSpeed * 1.0f));
		}

		unprotectAllEnemyDamageFlag();

		APP_ASSERT((mpObjectHand != NULL),
					"mpObjectHand is NULL");
		mpObjectHand->ProcessThrow(&throw_vec);

		SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	} else if (this->BetweenPhaseFrame(0.0f, 17.0f)) {
		
		// 爆弾を持たせるマクロ
		RAISE_ATTACH_MODEL_MACRO();
	}

	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateRaisePut(void)
{
	RAISE_ATTACH_MODEL_MACRO();

	CollisionManager::GetInstance()->RemoveBodyCollision(mpObjectHand);

	StartMotion(eMT_RAISE_PUT, false, 0.75f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateRaisePut(void)
{
	if (this->EqualPhaseFrame(14.0f)) {
		CollisionManager::GetInstance()->EntryBodyCollision(mpObjectHand);

		unprotectAllEnemyDamageFlag();

		float set_size;
		set_size = (mBodySize * 0.5f) + (mpObjectHand->BodySize() * 0.5f);
		mpObjectHand->Position().x += sin(mRotation.y + Utility::DegToRad(180.0f)) * set_size;
		mpObjectHand->Position().z += cos(mRotation.y + Utility::DegToRad(180.0f)) * set_size;
		
		mpObjectHand->SetPositionOnCollision();
		mpObjectHand->Position().y += 0.1f;
		mpObjectHand->ProcessThrow((VECTOR *)&ZERO_VEC);

	} else if (this->BetweenPhaseFrame(0.0f, 14.0f)) {
		RAISE_ATTACH_MODEL_MACRO();
	} else if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateRaisePut(void) 
{
	if (mpObjectHand != NULL) {
		mpObjectHand->ProcessThrow((VECTOR *)&ZERO_VEC);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateBombStart(void)
{
	int iI = 0;
	bool is_find = false;

	// アイテム数のチェック
	if (TotalInfo::GetInstance()->GetItemNum(eSUBWEAPON_NUM_BOMB) == 0) {
		SoundManager::GetSndComponent()->PlaySound(eSE_NG);
		ChangeState(eCHARA_STATE_IDLE);
		return;
	}

	for (iI = 0; iI < kPLAYER_BOMB_NUM; iI ++) {
		if (sPlayerBomb[iI].State() == ObjectHand::eHAND_STATE_DISAPPEAR) {
			mpObjectHand = &sPlayerBomb[iI];
			PRINT_CONSOLE("Raise Bomb Index = %d \n", iI);

			((ObjectHand_Bomb *)mpObjectHand)->Setup(eOBJECT_KIND_BOMB, (eItemKind)0, (VECTOR *)&ZERO_VEC, 0, true);
			ChangeState(eCHARA_STATE_RAISE_IDLE);
			is_find = true;

			TotalInfo::GetInstance()->AddItemNum(eSUBWEAPON_NUM_BOMB, -1);

			break;
		}
	}

	if (is_find == false) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateBombStart(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateArcheryStart(void)
{
	// アイテム数のチェック
	if (TotalInfo::GetInstance()->GetItemNum(eSUBWEAPON_NUM_ARROW) == 0) {
		SoundManager::GetSndComponent()->PlaySound(eSE_NG);
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_RESET);
		ChangeState(eCHARA_STATE_IDLE);
		return;
	}

	ObjectWeapon_Arrow* p_arrow = getUnusedArrowPtr();

	// プレイヤーの設定
	unprotectAllEnemyDamageFlag();
	// 前回のステータスによって変える
	if (this->mMotion.mIndex == sIdleMotionIndex) {

		if (mMatrixType == eMATRIX_TYPE_MATRIX) {
			MATRIX mtx;
			Utility::ExtractRotationMatrix(&mtx, &mMatrix, 1.0f);
			sBoomerangTargetVec = VNorm(VTransform(VGet(0.0f, 0.0f, -1.0f), mtx));
		}
		else { 
			sBoomerangTargetVec.x = -(sin(mRotation.y));
			sBoomerangTargetVec.y = 0.0f;
			sBoomerangTargetVec.z = -(cos(mRotation.y));
		}
	}	
	this->StartMotion(eMT_TAKEBACK_ARCHERY, true, 0.2f);
	this->mIsLeaveWeapon = true;								// 武器手放しフラグを有効にする

	// 弓モデルの初期設定

	// 弓の描画開始
	p_arrow->ProcessSetArchery();

	// アタッチパターンを変更
	ChangeAttachPattern(eATTACH_PATTERN_ARCHERY);

	sNavi.DisableNaviCursor();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateArcheryStart(void)
{
	ModelPmd* p_archery = (ModelPmd *)GetAttachAddr(eATTACH_MODEL_SUB_WEAPON)->mpAttachedModel;
	ObjectWeapon_Arrow* p_arrow = getUnusedArrowPtr();
	float input_value = 0.0f;
	int archery_cam_type = 0;
	float end_length = kArcheryNormalDist;
	
	VECTOR node_pos, crossX, crossY, node_dir;
	MATRIX tmp_mtx;

	if (mMatrixType == eMATRIX_TYPE_MATRIX) {
		VECTOR rot_mtx_vec;
		ModelBase::GetRotationVecFromMatrix(&rot_mtx_vec);
		// トロッコの向きがある程度傾いたら弓矢を解除
		if (rot_mtx_vec.y > 0.6f) {
			ChangeState(eCHARA_STATE_IDLE);
			return;
		}
	}

	this->GetFramePosFromName(&node_pos, "上半身");
	if (mpLockonChara == NULL){
		archery_cam_type = CameraManager::GetInstance()->GetArcheryCamType();
		input_value = kARCHERY_INPUT_VALUE[archery_cam_type];
		// ジョイパッドとマウスの両方から武器の向きを求める
		updateWeaponTargetVecFromInput(&sBoomerangTargetVec, -input_value, input_value, true);
		// スコープモードの場合は飛距離を延ばす
		if (archery_cam_type == CameraManager::eARCHERY_CAM_TYPE_FAR) {
			end_length = kArcheryScopeDist;
		}
	} else {
		// ベクトルに対してキャラを回転させる
		VECTOR lockon_pos;
		mpLockonChara->GetLockonPosition(&lockon_pos);
		lockon_pos.y += (mpLockonChara->BodyHeight() * 0.5f);
		sBoomerangTargetVec = VSub(lockon_pos, node_pos);

		end_length = VSize(sBoomerangTargetVec);
		sBoomerangTargetVec = VNorm(sBoomerangTargetVec);
	}
		
	// ベクトルに対してキャラを回転させる
	mRotation.y = atan2(sBoomerangTargetVec.x, sBoomerangTargetVec.z) + DX_PI;
	Utility::CorrectAngleFrom000To360(&mRotation.y);
		
	// 武器のベクトルから『上半身』のUserMatrixを設定する
	node_pos.x += sin(mRotation.y + Utility::DegToRad(180.0f + 12.0f));
	node_pos.z += cos(mRotation.y + Utility::DegToRad(180.0f + 12.0f));
	crossX = VCross(sBoomerangTargetVec, VGet(0.0f, 1.0f, 0.0f));
	crossY = VCross(crossX, sBoomerangTargetVec);
	tmp_mtx = MGetRotY(Utility::DegToRad(90.0f));
	node_dir = VTransform(crossY, tmp_mtx);
	this->SetFrameUserRotation("上半身", &node_pos, &node_dir, 0.75f);

	// 弓の弦とプレイヤーの指をアタッチさせる
	updateAttachArcheryStringToPlayerFinger(p_archery);

	// 弓を弦に持っていく
	MATRIX string_mtx;
	p_archery->GetFrameMatrixFromName(&string_mtx, "弦");
	p_arrow->Matrix() = string_mtx;
	
	// 標的の終点を求める
	VECTOR bow_pos, end_pos;
	p_archery->GetFramePosFromName(&bow_pos, "弓中央");
	end_pos = VAdd(bow_pos, VScale(sBoomerangTargetVec, end_length));
	VECTOR hitpos, normal;
	if ((CollisionManager::GetInstance()->CheckEarthCollision(bow_pos, end_pos, &hitpos, &normal) != NOT_ON_COLLISION) ||
		(CollisionManager::GetInstance()->CheckWallCollision(bow_pos, end_pos, &hitpos, &normal))) {
		end_pos = hitpos;

		PRINT_SCREEN(GetColor(255, 255, 0), "Archery Hit");
	}
	TotalInfo::GetInstance()->EnableLockonLayout(&end_pos);
	ShapeLine line;
	line.mStart = bow_pos;
	line.mEnd	= end_pos;
	line.mColor = COLOR_WHITE;

	// カメラ制御
	CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_ARCHERY);
	mCameraGeneralVec = end_pos;	// カメラベクトルを指定

	if (CHECK_TRIG_JOY(JOY_INPUT_ATTACK)) {
		mIsReserveComboInput = true;
	}

	// 使用する弓が射撃中なら入力を無効にする
	if (p_arrow->GetIsRetire() == false) {
		mIsReserveComboInput = false;
		// 使用する弓矢は次の弓矢を選択する
		sUsePlayerArrowIndex = ((sUsePlayerArrowIndex + 1) % kPLAYER_ARROW_NUM);
	}

	// フェーズ遷移処理
	if ((mIsReserveComboInput == true) && (GetIsMotionBlend() == false)) {
		mIsReserveComboInput = false;
		ChangeState(eCHARA_STATE_ARCHERY_CHARGE);
	}
	else if ((CHECK_TRIG_JOY(JOY_INPUT_SUB_WEAPON)) ||
			 (CHECK_TRIG_JOY(JOY_INPUT_AWAY))) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void CharaPlayer::phaseLeaveStateArcheryStart(void) 
{
	// 次のモーション以外の場合
	if (mCharaState != eCHARA_STATE_ARCHERY_CHARGE) {

		processCancelArchery();
		SoundManager::GetSndComponent()->PlaySound(eSE_PUT_SWORD);

		if (mpLockonChara != NULL) {
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_LOCKON);
		}
		else {
			CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_HIGH, true);
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
			CameraManager::GetInstance()->ResetCameraPosY();
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateArcheryCharge(void)
{
	ModelPmd* p_archery = (ModelPmd *)GetAttachAddr(eATTACH_MODEL_SUB_WEAPON)->mpAttachedModel;

	this->StartMotion(eMT_CHARGE_ARCHERY, true, 0.5f);
	p_archery->StartMotion(1, false, 0.5f);
	
	SoundManager::GetSndComponent()->PlaySound(eSE_PULL_ARCHERY);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateArcheryCharge(void)
{
	ModelPmd* p_archery = (ModelPmd *)GetAttachAddr(eATTACH_MODEL_SUB_WEAPON)->mpAttachedModel;
	ObjectWeapon_Arrow* p_arrow = getUnusedArrowPtr();
	int archery_cam_type = 0;
	float end_length = kArcheryNormalDist;
	VECTOR node_pos;

	if (mMatrixType == eMATRIX_TYPE_MATRIX) {
		VECTOR rot_mtx_vec;
		ModelBase::GetRotationVecFromMatrix(&rot_mtx_vec);
		// トロッコの向きがある程度傾いたら弓矢を解除
		if (rot_mtx_vec.y > 0.6f) {
			ChangeState(eCHARA_STATE_IDLE);
			return;
		}
	}

	this->GetFramePosFromName(&node_pos, "上半身");
	if (mpLockonChara != NULL){
		// ベクトルに対してキャラを回転させる
		VECTOR lockon_pos;
		mpLockonChara->GetLockonPosition(&lockon_pos);
		
		lockon_pos.y += (mpLockonChara->BodyHeight() * 0.5f);
		sBoomerangTargetVec = VSub(lockon_pos, node_pos);

		end_length = VSize(sBoomerangTargetVec);
		sBoomerangTargetVec = VNorm(sBoomerangTargetVec);
	}

	archery_cam_type = CameraManager::GetInstance()->GetArcheryCamType();
	// スコープモードの場合は飛距離を延ばす
	if (archery_cam_type == CameraManager::eARCHERY_CAM_TYPE_FAR) {
		end_length = kArcheryScopeDist;
	}

	// 弓の弦とプレイヤーの指をアタッチさせる
	updateAttachArcheryStringToPlayerFinger(p_archery);

	// 弓を弦に持っていく
	MATRIX string_mtx;
	p_archery->GetFrameMatrixFromName(&string_mtx, "弦");
	p_arrow->Matrix() = string_mtx;
	
	// 標的の終点を求める
	VECTOR bow_pos, end_pos;
	p_archery->GetFramePosFromName(&bow_pos, "弓中央");
	end_pos = VAdd(bow_pos, VScale(sBoomerangTargetVec, end_length));
	VECTOR hitpos, normal;
	if ((CollisionManager::GetInstance()->CheckEarthCollision(bow_pos, end_pos, &hitpos, &normal) != NOT_ON_COLLISION) ||
		(CollisionManager::GetInstance()->CheckWallCollision(bow_pos, end_pos, &hitpos, &normal))) {
		end_pos = hitpos;
	}
	TotalInfo::GetInstance()->EnableLockonLayout(&end_pos);
	ShapeLine line;
	line.mStart = bow_pos;
	line.mEnd	= end_pos;
	line.mColor = COLOR_WHITE;

	CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_ARCHERY);
	mCameraGeneralVec = end_pos;	// カメラベクトルを指定

	if ((CHECK_TRIG_JOY(JOY_INPUT_SUB_WEAPON)) ||
		(CHECK_TRIG_JOY(JOY_INPUT_AWAY))) {

		ChangeState(eCHARA_STATE_IDLE);
	} 
	// 攻撃ボタンを離すと弓を発射
	else if (CHECK_PRESS_JOY(JOY_INPUT_ATTACK) == false) {

		// アイテム数の消費
		TotalInfo::GetInstance()->AddItemNum(eSUBWEAPON_NUM_ARROW, -1);

		SoundManager::GetSndComponent()->PlaySound(eSE_SHOOT_ARCHERY);

		// 矢の射撃処理
		VECTOR vec, top_pos;
		vec = VScale(sBoomerangTargetVec, 100.0f);
		top_pos = VAdd(bow_pos, vec);
#ifdef _DEBUG
		RENDER_LINE(&bow_pos, &top_pos, COLOR_BLUE);
#endif // _DEBUG
		p_arrow->ProcessShoot(&bow_pos, &top_pos);
		
		// 弓のモーションを設定
		p_archery->ResetFrameUserMatrix("弦");
		p_archery->StartMotion(2, false, 1.8f, NULL, 0.9f);

		VECTOR crossX;
		crossX = VCross(sBoomerangTargetVec, sUpVec);

		float dot, rad;
		VECTOR tmp;
		tmp.x = sin(mRotation.y + Utility::DegToRad(180.0f));
		tmp.y = 0.0f;
		tmp.z = cos(mRotation.y + Utility::DegToRad(180.0f));
		dot = VDot(VNorm(sBoomerangTargetVec), tmp);
		rad = acos(dot);

		EFK_HANDLE efk_handle;
		efk_handle = EffekseerManager::GetInstance()->Play(eEFK_SHOCK_THIN, &bow_pos, 3.0f);
		EffekseerManager::GetInstance()->SetRotationAxis(efk_handle, &crossX, (Utility::DegToRad(-90.0f) + rad));

		// 使用する弓矢は次の弓矢を選択する
		sUsePlayerArrowIndex = ((sUsePlayerArrowIndex + 1) % kPLAYER_ARROW_NUM);

		this->StartMotion(eMT_SHOOT_ARCHERY, false, 2.4f, NULL, 0.9f);

		PRINT_CONSOLE("Process Shoot　\n");

		ChangeState(eCHARA_STATE_ARCHERY_SHOOT);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void CharaPlayer::phaseLeaveStateArcheryCharge(void)
{
	// 次のモーション以外の場合
	if (mCharaState != eCHARA_STATE_ARCHERY_SHOOT) {

		processCancelArchery();
		SoundManager::GetSndComponent()->PlaySound(eSE_PUT_SWORD);

		if (mpLockonChara != NULL) {
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_LOCKON);
		}
		else {
			CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_HIGH, true);
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
			CameraManager::GetInstance()->ResetCameraPosY();
		}
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateArcheryShoot(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateArcheryShoot(void)
{
	ModelPmd* p_archery = (ModelPmd *)GetAttachAddr(eATTACH_MODEL_SUB_WEAPON)->mpAttachedModel;
	float input_value = 0.0f;
	int archery_cam_type = 0;
	float end_length = kArcheryNormalDist;

	VECTOR node_pos, crossX, crossY, node_dir;
	MATRIX tmp_mtx;

	this->GetFramePosFromName(&node_pos, "上半身");
	if (mpLockonChara == NULL){
		archery_cam_type = CameraManager::GetInstance()->GetArcheryCamType();
		input_value = kARCHERY_INPUT_VALUE[archery_cam_type];
		// ジョイパッドとマウスの両方から武器の向きを求める
		updateWeaponTargetVecFromInput(&sBoomerangTargetVec, -input_value, input_value, true);
		// スコープモードの場合は飛距離を延ばす
		if (archery_cam_type == CameraManager::eARCHERY_CAM_TYPE_FAR) {
			end_length = kArcheryScopeDist;
		}
	} else {
		// ベクトルに対してキャラを回転させる
		VECTOR lockon_pos;
		mpLockonChara->GetLockonPosition(&lockon_pos);
		
		lockon_pos.y += (mpLockonChara->BodyHeight() * 0.5f);
		sBoomerangTargetVec = VSub(lockon_pos, node_pos);

		end_length = VSize(sBoomerangTargetVec);
		sBoomerangTargetVec = VNorm(sBoomerangTargetVec);
	}
	// ベクトルに対してキャラを回転させる
	mRotation.y = atan2(sBoomerangTargetVec.x, sBoomerangTargetVec.z) + DX_PI;
	Utility::CorrectAngleFrom000To360(&mRotation.y);
		
	// 武器のベクトルから『上半身』のUserMatrixを設定する
	node_pos.x += sin(mRotation.y + Utility::DegToRad(180.0f + 12.0f));
	node_pos.z += cos(mRotation.y + Utility::DegToRad(180.0f + 12.0f));
	crossX = VCross(sBoomerangTargetVec, VGet(0.0f, 1.0f, 0.0f));
	crossY = VCross(crossX, sBoomerangTargetVec);
	tmp_mtx = MGetRotY(Utility::DegToRad(90.0f));
	node_dir = VTransform(crossY, tmp_mtx);
	this->SetFrameUserRotation("上半身", &node_pos, &node_dir, 0.75f);




	// 標的の終点を求める
	VECTOR bow_pos, end_pos;
	p_archery->GetFramePosFromName(&bow_pos, "弓中央");
	end_pos = VAdd(bow_pos, VScale(sBoomerangTargetVec, end_length));
	VECTOR hitpos, normal;
	if ((CollisionManager::GetInstance()->CheckEarthCollision(bow_pos, end_pos, &hitpos, &normal) != NOT_ON_COLLISION) ||
		(CollisionManager::GetInstance()->CheckWallCollision(bow_pos, end_pos, &hitpos, &normal))) {
		end_pos = hitpos;
	}
	TotalInfo::GetInstance()->EnableLockonLayout(&end_pos);
#ifdef _DEBUG
	RENDER_LINE(&bow_pos, &end_pos, COLOR_RED);
#endif // _DEBUG

	// カメラ制御
	CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_ARCHERY);
	mCameraGeneralVec = end_pos;	// カメラベクトルを指定

	
	if (CHECK_TRIG_JOY(JOY_INPUT_ATTACK)) {
		mIsReserveComboInput = true;
	}

	// フェーズ遷移処理
	if (ModelPmd::GetMotionTimePercent() > 80) {

		StartMotion(eMT_TAKEBACK_ARCHERY, true, 0.75f);
		ChangeState(eCHARA_STATE_ARCHERY_START);

	}
	else {
		if ((CHECK_TRIG_JOY(JOY_INPUT_AWAY)) ||
			(CHECK_TRIG_JOY(JOY_INPUT_JUMP))) {
			ChangeState(eCHARA_STATE_IDLE);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void CharaPlayer::phaseLeaveStateArcheryShoot(void) 
{
	// 次のモーション以外の場合
	if (mCharaState != eCHARA_STATE_ARCHERY_START) {

		processCancelArchery();
		SoundManager::GetSndComponent()->PlaySound(eSE_PUT_SWORD);

		if (mpLockonChara != NULL) {
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_LOCKON);
		}
		else {
			CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_HIGH, true);
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
			CameraManager::GetInstance()->ResetCameraPosY();
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateArcheryEnd(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateArcheryEnd(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void CharaPlayer::phaseLeaveStateArcheryEnd(void) {}

//---------------------------------------------------
/**
 * スマッシュ斬り①
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateAttackSmashStart(void)
{
	StartMotion(eMT_ATTACK_SMASH_START, false, 0.3f, NULL, 0.08f);

	SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_ATTACK_4);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateAttackSmashStart(void)
{
	static float sCheckTime = 4.0f;

	float motion_now_time = this->GetMotionNowTime();

	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);
	weapon_top_pos = VAdd(weapon_top_pos, mMoveVec);
	weapon_btm_pos = VAdd(weapon_btm_pos, mMoveVec);

	if (this->EqualPhaseFrame(sCheckTime)) {
		mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);
		unprotectAllEnemyDamageFlag();
	} else if (motion_now_time > sCheckTime) {
		mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);
	}

	mMoveVec.x += -(sin(mRotation.y) * sSmashAttackVec * 100.0f);
	mMoveVec.z += -(cos(mRotation.y) * sSmashAttackVec * 100.0f);

	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	float dist = 0.0f;
	if (mpLockonChara != NULL) {
		VECTOR lockon_chara;
		mpLockonChara->GetLockonPosition(&lockon_chara);
		dist = VSize(VSub(lockon_chara, mPosition));
		dist -= mpLockonChara->BodySize() + mBodySize;
	}

	// 目標と近づいた場合に振りかざしをする
	if ( ( (mpLockonChara != NULL) && ( dist < 50.0f) && (motion_now_time > 3.0f) ) ||
		 (motion_now_time > 4.0f) ) {
		
		// モーションが指定以上の場合
		SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
		StartMotion(eMT_ATTACK_SMASH_DOING, false, 1.5f);
		SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

		ChangeState(eCHARA_STATE_ATTACK_SMASH_DOING);
	}
}

//---------------------------------------------------
/**
 * スマッシュ斬り②
 */
//---------------------------------------------------
void		
CharaPlayer::phaseEnterStateAttackSmashDoing(void)
{
	unprotectAllEnemyDamageFlag();

	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);
	weapon_top_pos = VAdd(weapon_top_pos, mMoveVec);
	weapon_btm_pos = VAdd(weapon_btm_pos, mMoveVec);

	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void		
CharaPlayer::phaseUpdateStateAttackSmashDoing(void)
{
	mMoveVec = mJumpMoveVec.mDash;

	mMoveVec.x += -(sin(mRotation.y) * sSmashAttackVec * 100.0f);
	mMoveVec.z += -(cos(mRotation.y) * sSmashAttackVec * 100.0f);

	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);
	weapon_top_pos = VAdd(weapon_top_pos, mMoveVec);
	weapon_btm_pos = VAdd(weapon_btm_pos, mMoveVec);

	if (this->EqualPhaseFrame(12.0f)) {
		SoundManager::GetSndComponent()->PlaySound(sFootSoundTable[sFootstepKind][eFOOT_SND_LANDING]);
	}

	mIsAttack = true;
	mAttackPower = 2;

	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {
		StartMotion(eMT_ATTACK_SMASH_END, false, 0.3f); 
		ChangeState(eCHARA_STATE_ATTACK_SMASH_END);
	}
}

//---------------------------------------------------
/**
 * スマッシュ斬り③
 */
//---------------------------------------------------
void		
CharaPlayer::phaseEnterStateAttackSmashEnd(void)
{
	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);
	weapon_top_pos = VAdd(weapon_top_pos, mMoveVec);
	weapon_btm_pos = VAdd(weapon_btm_pos, mMoveVec);

	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	mJumpMoveVec.mLanding.x = mMoveVec.x;
	mJumpMoveVec.mLanding.y = 0.0f;
	mJumpMoveVec.mLanding.z = mMoveVec.z;

	CameraManager::GetInstance()->StartVibeCamera(0.5f, 4.0f, 5.0f);

	UPDATE_FOOTSTEP_KIND();
	ChangeState(eCHARA_STATE_ATTACK_SMASH_END);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void		
CharaPlayer::phaseUpdateStateAttackSmashEnd(void)
{
	mJumpMoveVec.mLanding = VScale(mJumpMoveVec.mLanding, 0.90f);
	mMoveVec = mJumpMoveVec.mLanding;

	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);
	weapon_top_pos = VAdd(weapon_top_pos, mMoveVec);
	weapon_btm_pos = VAdd(weapon_btm_pos, mMoveVec);

	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion() ) {

		mBladeEffect.Stop();
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateLookAround(void)
{
	unprotectAllEnemyDamageFlag();

	CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_LOW, true);
	CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_LOOK_AROUND);
	this->SetVisible(false);


	if (mMotion.mIndex == eMT_IDLE) {
		sBoomerangTargetVec.x = sin(mRotation.y + Utility::DegToRad(210.0f));
		sBoomerangTargetVec.y = 0.0f;
		sBoomerangTargetVec.z = cos(mRotation.y + Utility::DegToRad(210.0f));
	}
	else {
		sBoomerangTargetVec.x = sin(mRotation.y + Utility::DegToRad(180.0f));
		sBoomerangTargetVec.y = 0.0f;
		sBoomerangTargetVec.z = cos(mRotation.y + Utility::DegToRad(180.0f));
	}
	
	// ベクトルに対してキャラを回転させる
	mRotation.y = -(atan2(sBoomerangTargetVec.x, sBoomerangTargetVec.z) + DX_PI);
	Utility::CorrectAngleFrom000To360(&mRotation.y);
}

//---------------------------------------------------
/**
 * 見回す処理 (HookShotを参考)
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateLookAround(void)
{
	VECTOR node_pos;

	if (this->GetIsMotionBlend() == false) {

		float input_value = 1.5f;

		VECTOR tmp_pos = VNorm(sBoomerangTargetVec);

		// ジョイパッドとマウスの両方から武器の向きを求める
		updateWeaponTargetVecFromInput(&tmp_pos, -input_value, input_value, true);
		
		// 首の入力制限
		if ((tmp_pos.y < 0.95f) && (tmp_pos.y > -0.95f)) {
			sBoomerangTargetVec = tmp_pos;
		}

		// ベクトルに対してキャラを回転させる
		mRotation.y = atan2(sBoomerangTargetVec.x, sBoomerangTargetVec.z) + DX_PI;
		Utility::CorrectAngleFrom000To360(&mRotation.y);

		this->GetFramePosFromName(&node_pos, "首");

		VECTOR node_dir;
		node_dir = VAdd(node_pos, sBoomerangTargetVec);

		SetFrameUserRotation("首", &node_dir, 0.5f);

		if ((CHECK_TRIG_JOY(JOY_INPUT_AWAY)) ||
			(CHECK_TRIG_JOY(JOY_INPUT_ATTACK)) ||
			(CHECK_TRIG_JOY(JOY_INPUT_CAM_RESET)) ) {
			
			ChangeState(eCHARA_STATE_IDLE);
			return;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateLookAround(void)
{
	this->SetVisible(true);

	ResetFrameUserMatrix("首");

	CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_MIDDLE, true);
	CameraManager::GetInstance()->ResetCameraPosY();
	CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);

	VECTOR vec;
	float rot;
	vec = VSub( CameraManager::GetInstance()->Target(), 
				CameraManager::GetInstance()->Position() );
	rot = atan2(vec.x, vec.z) + DX_PI;
	Utility::CorrectAngleFrom000To360(&rot);
	
	mRotation.y = rot;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateAttackRear(void)
{
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
	SoundManager::GetSndComponent()->PlaySound(eSE_ROLLING);

	unprotectAllEnemyDamageFlag();

	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_ATTACK_REAR, false, 0.75f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateAttackRear(void)
{
	mAttackPower = 2;
	mIsAttack = true;

	VECTOR weapon_top_pos, weapon_btm_pos;
	getWeaponTopAndBtmPos(&weapon_top_pos, &weapon_btm_pos);

	float move_speed = 0.18f * (100 - GetMotionTimePercent());

	if (BetweenPhaseFrame(0.0f, 6.0f)) {
		mMoveVec.x += -(sin(mRotation.y) * move_speed);
		mMoveVec.z += -(cos(mRotation.y) * move_speed);
	}
	else {
		mMoveVec.x += (sin(mRotation.y) * move_speed);
		mMoveVec.z += (cos(mRotation.y) * move_speed);
	}

	if (EqualPhaseFrame(6.0f)) {
		mGravity += 30.0f;
		mConstGravitySpeed = kGRAVITY_SPEED;

		SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_ATTACK_3);
	}

	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (ModelPmd::GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_JUMP_DOWN);
	}

	CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_MIDDLE, true);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateAttackRear(void)
{
	mIsAttack = false;
	mBladeEffect.Stop();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateAttackRollingKick(void)
{
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
	SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_BOOMERANG_RUN);

	static float sTmp = 1.1f;
	StartMotion(eMT_ATTACK_ROLLING_KICK, false, sTmp);

	addPiyoPiyoGauge(-300);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateAttackRollingKick(void)
{
	VECTOR weapon_top_pos, weapon_btm_pos;
	GetFramePosFromName(&weapon_top_pos, "右つま先ＩＫ");
	GetFramePosFromName(&weapon_btm_pos, "右ひざ");

	float move_speed = 0.43f * (float)(100 - GetMotionTimePercent());
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	if (EqualPhaseFrame(15.0f)) {
		mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);
	}
	if (BetweenPhaseFrame(15.0f, 30.0f)) {
		mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);
		mAttackPower = 1;
		mIsAttack = true;
	}
	else {
		mIsAttack = false;
	}

	if (ModelPmd::GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateAttackRollingKick(void)
{
	mBladeEffect.Stop();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateGuardStagger(void)
{
	StartMotion(eMT_SHIELD_GUARD_DOING, false, 1.75f);

	mIsDamage = true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateGuardStagger(void)
{
	float move_speed = -0.24f * (100 - GetMotionTimePercent());
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	if (GetIsEndMotion()) {
		if (mPiyoPiyoValue <= 0) {
			ChangeState(eCHARA_STATE_PIYOPIYO);
		}
		else if (CHECK_PRESS_JOY(JOY_INPUT_AWAY) == false) {
			StartMotion(eMT_IDLE, true, 0.5f, NULL, 0.05f);
			ChangeState(eCHARA_STATE_IDLE);
		}
		else {
			ChangeState(eCHARA_STATE_LIB_SHIELD);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateGuardStagger(void)
{
	mIsDamage = false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStatePiyoPiyo(void)
{
	mIsAttack = false;
	mIsDamage = false;

	SoundManager::GetSndComponent()->PlaySound(eSE_PIYOPIYO);

	VECTOR effpos = mPosition;
	effpos.y += mBodyHeight;
	mEfkHnd.uAngelWing = EffekseerManager::GetInstance()->Play(eEFK_PIYOPIYO, &effpos, 5.0f);
	EffekseerManager::GetInstance()->SetSpeed(mEfkHnd.uAngelWing, 0.70f);

	this->DisableLockonChara();
	
	StartMotion(eMT_SHIELD_PIYOPIYO, false, 0.75f, NULL, 0.3f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStatePiyoPiyo(void)
{
	if (GetMotionIndex() == eMT_SHIELD_PIYOPIYO) {
		if (GetIsEndMotion()) {
			StartMotion(eMT_SHIELD_PIYOPIYO_2ND, true, 0.80f, NULL, 1.0f);
		}
	}

	if (SoundManager::GetSndComponent()->IsPlay(eSE_PIYOPIYO) == false) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStatePiyoPiyo(void)
{
	if (SoundManager::GetSndComponent()->IsPlay(eSE_PIYOPIYO)) {
		SoundManager::GetSndComponent()->StopSound(eSE_PIYOPIYO);
	}
	
	STOP_EFFEKSEER(mEfkHnd.uAngelWing, 0.0f);

	addPiyoPiyoGauge(kPIYO_PIYO_GAUGE_MAX/2);	// 半快に
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateGutsPose(void)
{
	SetMotionSpeed(0.0f);

	SoundManager::GetSndComponent()->PlaySound(eSE_COMICAL);

	mEfkHnd.uAngelWing = EffekseerManager::GetInstance()->Play(eEFK_PORTION, &mPosition, 20.0f);
	EffekseerManager::GetInstance()->SetSpeed(mEfkHnd.uAngelWing, 1.2f);

	mCount.uGutsPose = 0;

	mIsPermitInput = false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateGutsPose(void)
{
	if (mCount.uGutsPose == 36) {
		StartMotion(eMT_GUTS_POSE, false, 0.35f);
		// アタッチパターンを変更
		ChangeAttachPattern(eATTACH_PATTERN_HAND);

		switch (mUsingRecoverItemKind) {
		case eITEM_KIND_PORTION:
			SoundManager::GetSndComponent()->PlaySound(eSE_HOLY);
			EffekseerManager::GetInstance()->Play(eEFK_HEAL, &mPosition, 10.0f);
			AddHitPoint(12);//4x3
			break;
		case eITEM_KIND_BIN_MAGENTA:
			SoundManager::GetSndComponent()->PlaySound(eSE_HOLY);
			EffekseerManager::GetInstance()->Play(eEFK_HEAL, &mPosition, 20.0f);
			AddHitPoint(40);//4x10
			break;
		case eITEM_KIND_BIN_CYAN:
			SoundManager::GetSndComponent()->PlaySound(eSE_SPREAD);
			EffekseerManager::GetInstance()->Play(eEFK_HEAL, &mPosition, 18.0f);
			AddNowMagicPoint(9999);
			addPiyoPiyoGauge(9999);

			break;
		case eITEM_KIND_BIN_YELLOW:
			SoundManager::GetSndComponent()->PlaySound(eSE_HOLY);
			EffekseerManager::GetInstance()->Play(eEFK_HEAL, &mPosition, 40.0f);
			AddHitPoint(9999); // 全回復
			AddNowMagicPoint(9999); // 全回復

			break;
		default:
			// 想定されていない使用中回復アイテムフラグ
			APP_ASSERT_PRINT((NULL),
							 "Invalid mUsingRecoverItemKind : %d \n", mUsingRecoverItemKind);
			break;
		};

		float piyopiyo_rate = (float)mPiyoPiyoValue / (float)kPIYO_PIYO_GAUGE_MAX;
		float magic_rate	= (float)mMagicPoint / 100.0f;
		TotalInfo::GetInstance()->UpdatePiyoPiyoGaugeByRate(piyopiyo_rate, magic_rate);

		// 使用中回復アイテムフラグをリセット
		mUsingRecoverItemKind = eITEM_KIND_NONE;

	}

	if (mMotion.mIndex == eMT_GUTS_POSE) {
		if (GetIsEndMotion()) {
			mIsPermitInput = true;
			ChangeState(eCHARA_STATE_IDLE);
		}
	}
	mCount.uGutsPose ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateGutsPose(void)
{
	mIsPermitInput = true;

	mCount.uGutsPose = 0;

	STOP_EFFEKSEER(mEfkHnd.uAngelWing, 0.0f);

	// アタッチパターンを変更
	ChangeAttachPattern(eATTACH_PATTERN_NORMAL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateMagicThunder(void)
{
	StartMotion(eMT_MAGIC_ARIA_START, false, 0.5f);

	mCount.uMagic = 0;

	mIsAttack = true;

	// アタッチパターンを変更
	ChangeAttachPattern(eATTACH_PATTERN_SWORD);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateMagicThunder(void)
{
	const float length = 900.0f;
	VECTOR thunder_eff_pos;
	thunder_eff_pos = mPosition;
	thunder_eff_pos.x += sin(mRotation.y + DX_PI) * length;
	thunder_eff_pos.z += cos(mRotation.y + DX_PI) * length;

	VECTOR check_top, check_btm;
	check_top = VGet(thunder_eff_pos.x, 
					 thunder_eff_pos.y + 3000.0f,
					 thunder_eff_pos.z);
	check_btm = VGet(thunder_eff_pos.x, 
					 thunder_eff_pos.y - 3000.0f,
					 thunder_eff_pos.z);
	VECTOR normal;
	CollisionManager::GetInstance()->CheckEarthCollision(check_top, check_btm, &thunder_eff_pos, &normal);
	
	thunder_eff_pos.y += 100.0f;

	// ステップ1：魔法を唱え始め
	if (mCount.uMagic == 30) {
		SoundManager::GetSndComponent()->PlaySound(eSE_MAGIC);
		EffekseerManager::GetInstance()->Play(eEFK_MAGIC_SQUARE, &mPosition, &VScale(mScale, 2.0f));

		GraphicManager::tFogParam fogParam;
		fogParam.mIsEnable = TRUE;
		fogParam.mIsActive = TRUE;
		fogParam.mColR = 10;
		fogParam.mColG = 10;
		fogParam.mColB = 10;
		fogParam.mNear = 10.0f;
		fogParam.mFar = 50000.0f;
		GraphicManager::GetInstance()->ChangeFogParam(0.5f, &fogParam);
	}
	// ステップ2：エフェクト開始
	else if (mCount.uMagic == 120) {

		StartMotion(eMT_MAGIC_ARIA_DOING, true, 0.75f);
		mEfkHnd.uMagic = EffekseerManager::GetInstance()->Play(eEFK_LIGHTNING_STRIKE,
															   &thunder_eff_pos,
															   70.0f);
		
		SoundManager::GetSndComponent()->PlaySound(eSE_THUNDER);

		AddNowMagicPoint(-kMP_CONSUME_THUNDER);
	}
	// ステップ3：エフェクト着弾
	else if (mCount.uMagic == 144) {

		/* 攻撃の処理 */
		CharaBase* p_your = NULL;
		for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
			p_your = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
			if (this == p_your) continue;			// 自分はのぞく
			/* 戦闘不能者と既に攻撃された者を除く */
			if (p_your->CheckEnableAttack() == false)  continue;
			
			tCheckDamageWork work = {thunder_eff_pos, ZERO_VEC, ZERO_VEC, 1400.0f};
			if (p_your->CheckDamageCollRolling(&work)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				
				int attack_power = 12;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_your->Position(), thunder_eff_pos);
				damage_down_vec.y = 0.0f;
				damage_down_vec = VNorm(damage_down_vec);
				p_your->ProcessDamage(attack_power, false, &p_your->Position(), &damage_down_vec, true, eATTACK_TYPE_THUNDER);
			}
		}

		SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
		CameraManager::GetInstance()->StartVibeCamera(2.0f, 8.0f, 10.0f);
	}
	// ステップ4：終了
	else if (mCount.uMagic == 270) {

		ChangeState(eCHARA_STATE_IDLE);
	}

	mCount.uMagic ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateMagicThunder(void)
{
	mCount.uMagic = 0;

	unprotectAllEnemyDamageFlag();

	GraphicManager::GetInstance()->RevertFogParam(1.0f);

	STOP_EFFEKSEER(mEfkHnd.uMagic, 0.0f);

	mIsAttack = false;

	// アタッチパターンを変更
	ChangeAttachPattern(eATTACH_PATTERN_NORMAL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateMagicFlame(void)
{
	StartMotion(eMT_MAGIC_ARIA_START, false, 0.5f);

	mCount.uMagic = 0;

	mIsAttack = true;

	// アタッチパターンを変更
	ChangeAttachPattern(eATTACH_PATTERN_SWORD);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateMagicFlame(void)
{
	const float length = 900.0f;

	// ステップ1：魔法を唱え始め
	if (mCount.uMagic == 30) {
		SoundManager::GetSndComponent()->PlaySound(eSE_MAGIC);
		EffekseerManager::GetInstance()->Play(eEFK_MAGIC_SQUARE, &mPosition, &VScale(mScale, 2.0f));

		GraphicManager::tFogParam fogParam;
		fogParam.mIsEnable = TRUE;
		fogParam.mIsActive = TRUE;
		fogParam.mColR = 10;
		fogParam.mColG = 10;
		fogParam.mColB = 10;
		fogParam.mNear = 10.0f;
		fogParam.mFar = 50000.0f;
		GraphicManager::GetInstance()->ChangeFogParam(0.5f, &fogParam);
	}
	// ステップ2：エフェクト開始
	else if (mCount.uMagic == 31) {

		VECTOR eff_pos;
		GetFramePosFromName(&eff_pos, "左中指１");

		StartMotion(eMT_MAGIC_ARIA_DOING, true, 0.75f);
		mEfkHnd.uMagic = EffekseerManager::GetInstance()->Play(eEFK_REAL_FLAME,
															   &eff_pos,
															   150.0f);
		EffekseerManager::GetInstance()->SetSpeed(mEfkHnd.uMagic, 1.0f);

		SoundManager::GetSndComponent()->PlaySound(eSE_FLAME_SHOOT);

	}
	// ステップ3：エフェクト着弾
	else if (mCount.uMagic == 150) {

		VECTOR eff_pos = mPosition;
		eff_pos.y += 50;

		EffekseerManager::GetInstance()->SetPosition(mEfkHnd.uMagic, &eff_pos);

		/* 攻撃の処理 */
		CharaBase* p_your = NULL;
		for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
			p_your = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
			if (this == p_your) continue;			// 自分はのぞく
			/* 戦闘不能者と既に攻撃された者を除く */
			if (p_your->CheckEnableAttack() == false)  continue;
			
			tCheckDamageWork work = {eff_pos, ZERO_VEC, ZERO_VEC, 2200.0f};
			if (p_your->CheckDamageCollRolling(&work)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				
				int attack_power = 12;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_your->Position(), eff_pos);
				damage_down_vec.y = 0.0f;
				damage_down_vec = VNorm(damage_down_vec);
				p_your->ProcessDamage(attack_power, false, &p_your->Position(), &damage_down_vec, true, eATTACK_TYPE_NORMAL);
			}
		}

		AddNowMagicPoint(-kMP_CONSUME_FLAME);

		SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
		CameraManager::GetInstance()->StartVibeCamera(2.0f, 8.0f, 10.0f);
	}
	// ステップ4：終了
	else if (mCount.uMagic == 300) {

		ChangeState(eCHARA_STATE_IDLE);
	}

	mCount.uMagic ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateMagicFlame(void)
{
	mCount.uMagic = 0;

	unprotectAllEnemyDamageFlag();

	GraphicManager::GetInstance()->RevertFogParam(1.0f);

	STOP_EFFEKSEER(mEfkHnd.uMagic, 0.0f);

	SoundManager::GetSndComponent()->StopSound(eSE_FLAME_SHOOT);

	mIsAttack = false;

	// アタッチパターンを変更
	ChangeAttachPattern(eATTACH_PATTERN_NORMAL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateMagicTornade(void)
{
	StartMotion(eMT_MAGIC_ARIA_START, false, 0.5f);

	mCount.uMagic = 0;

	SoundManager::GetSndComponent()->PlaySound(eSE_TORNADE);

	mIsAttack = true;

	// アタッチパターンを変更
	ChangeAttachPattern(eATTACH_PATTERN_SWORD);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateMagicTornade(void)
{
	const float length = 900.0f;
	VECTOR thunder_eff_pos;
	thunder_eff_pos = mPosition;

	// ステップ1：魔法を唱え始め
	if (mCount.uMagic == 30) {
		SoundManager::GetSndComponent()->PlaySound(eSE_MAGIC);
		EffekseerManager::GetInstance()->Play(eEFK_MAGIC_SQUARE, &mPosition, &VScale(mScale, 2.0f));
	}
	// ステップ2：エフェクト開始
	else if (mCount.uMagic == 31) {

		StartMotion(eMT_MAGIC_ARIA_DOING, true, 0.75f);
		mEfkHnd.uMagic = EffekseerManager::GetInstance()->Play(eEFK_SENPU_AREA,
															   &thunder_eff_pos,
															   120.0f);
		
		CameraManager::GetInstance()->StartVibeCamera(3.0f, 6.0f, 8.0f);

		AddNowMagicPoint(-kMP_CONSUME_TORNADE);
	}
	// ステップ3：エフェクト着弾
	else if ((mCount.uMagic > 31) &&
			 (mCount.uMagic < 160) ) {

		if ((mCount.uMagic % 13) == 0) {
			
			unprotectAllEnemyDamageFlag();

			/* 攻撃の処理 */
			CharaBase* p_your = NULL;
			for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
				p_your = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
				if (this == p_your) continue;			// 自分はのぞく
				/* 戦闘不能者と既に攻撃された者を除く */
				if (p_your->CheckEnableAttack() == false)  continue;
			
				tCheckDamageWork work = {thunder_eff_pos, ZERO_VEC, ZERO_VEC, 620.0f};
				if (p_your->CheckDamageCollRolling(&work)) {
					SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				
					int attack_power = 1;
					VECTOR damage_down_vec;
					damage_down_vec = VSub(p_your->Position(), thunder_eff_pos);
					damage_down_vec.y = 0.0f;
					damage_down_vec = VNorm(damage_down_vec);
					p_your->ProcessDamage(attack_power, false, &p_your->Position(), &damage_down_vec, false, eATTACK_TYPE_NORMAL);
				}
			}
		}

	}
	// ステップ4：終了
	else if (mCount.uMagic == 160) {

		ChangeState(eCHARA_STATE_IDLE);
	}

	mCount.uMagic ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateMagicTornade(void)
{
	mCount.uMagic = 0;

	unprotectAllEnemyDamageFlag();

	STOP_EFFEKSEER(mEfkHnd.uMagic, 1.0f);

	SoundManager::GetSndComponent()->StopSound(eSE_TORNADE);

	mIsAttack = false;

	// アタッチパターンを変更
	ChangeAttachPattern(eATTACH_PATTERN_NORMAL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStatePlayOcarina(void)
{
	// 唄を１つでも覚えているか？
	BOOL is_song = FALSE;
	for (UCHAR iI = 0; iI < eSONG_TYPE_MAX; iI ++) {
		if (TotalInfo::GetInstance()->IsSongType(iI) == TRUE) {
			is_song = TRUE;
			break;
		}
	}

	// バトル中はオカリナが吹けない
	if (SceneMainGame::GetIsBattle() == true) {
		SoundManager::GetSndComponent()->PlaySound(eSE_NG);
		SceneMainGame::StartNaviTalk(strNaviTalk_CannotOcarinaEnemy);
		ChangeState(eCHARA_STATE_IDLE);
	}
	// ギミックの上では吹けない
	else if (mIsOnGimmick == TRUE) {
		SoundManager::GetSndComponent()->PlaySound(eSE_NG);
		SceneMainGame::StartNaviTalk(strNaviTalk_CannotOcarinaGimmick);
		ChangeState(eCHARA_STATE_IDLE);
	}
	// 唄を覚えていない場合
	else if (is_song == FALSE) {
		SoundManager::GetSndComponent()->PlaySound(eSE_NG);
		SceneMainGame::StartNaviTalk(strNaviTalk_CannotOcarinaNoSong);
		ChangeState(eCHARA_STATE_IDLE);
	}
	else {
		ActionTaskManager::tTaskInfo info = {ActionTaskManager::eTASK_PLAY_OCARINA, 0, 0};
		ActionTaskManager::GetInstance()->NotifyAction(&info);
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStatePlayOcarina(void)
{

	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStatePlayOcarina(void)
{
	// アタッチパターンを変更
	ChangeAttachPattern(eATTACH_PATTERN_NORMAL);

	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateCopyRod(void)
{
	// アイテム数のチェック
	if (mMagicPoint <= kMP_CONSUME_ROD) {
		SoundManager::GetSndComponent()->PlaySound(eSE_NG);
		ChangeState(eCHARA_STATE_IDLE);
		return;
	}

	mMagicPoint -= kMP_CONSUME_ROD;

	tMotionParam motion = {eMT_COPY_ROD, false, 0.0f, NULL};
	this->StartMotionEx(&motion);

	sBoomerangTargetVec.x = -(sin(mRotation.y));
	sBoomerangTargetVec.y = 0.0f;
	sBoomerangTargetVec.z = -(cos(mRotation.y));

	// アタッチパターンを変更
	ChangeAttachPattern(eATTACH_PATTERN_COPY_ROD);

	MATRIX node_mtx;
	ModelPmd* p_pmd = (ModelPmd *)GetAttachAddr(eATTACH_MODEL_SUB_WEAPON)->mpAttachedModel;
	p_pmd->GetFrameMatrixFromName(&node_mtx, "剣先");
	spPlayerCopyRod->ProcessCreate(&node_mtx);

	SoundManager::GetSndComponent()->PlaySound(eSE_MAGIC);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateCopyRod(void)
{
	static const  float kBOOMERANG_RELEASE_FRAME = 3.5f;

	ModelPmd::tAttachWork* p_attach = GetAttachAddr(eATTACH_MODEL_SUB_WEAPON);
	ModelPmd* p_pmd = (ModelPmd *)p_attach->mpAttachedModel;

	// 構えまでブレンドで待つ
	if ((mMotion.mSpeed == 0.0f) && (mMotion.mIsBlend == false)) {

		if (mpLockonChara == NULL) {
			// ジョイパッドとマウスの両方から武器のターゲットの向きを求める
			updateWeaponTargetVecFromInput(&sBoomerangTargetVec, -0.5f, 0.5f);
			mRotation.y = atan2(sBoomerangTargetVec.x, sBoomerangTargetVec.z) + DX_PI;
			CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_HIGH, true);
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_BOOMERANG);
		}
		else {
			/* ロックオンあり時の処理 */
			VECTOR tmpA, tmpB;
			tmpA = mPosition;
			tmpA.y += (mBodyHeight * 0.5f);
			mpLockonChara->GetLockonPosition(&tmpB);
			tmpB.y += (mpLockonChara->BodyHeight() * 0.5f);
			sBoomerangTargetVec = VNorm(VSub(tmpB, tmpA));
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_LOCKON);
		}

		VECTOR end_pos;
		end_pos = mPosition;
		end_pos.y += (mBodyHeight * 0.5f);
		//		end_pos = VAdd(end_pos, VScale(sBoomerangTargetVec, 2678.668f));
		end_pos = VAdd(end_pos, VScale(sBoomerangTargetVec, kArcheryScopeDist));
		TotalInfo::GetInstance()->EnableLockonLayout(&end_pos);

		mCameraGeneralVec = end_pos;	// カメラベクトルを指定

		Utility::CorrectAngleFrom000To360(&mRotation.y);

		// キーを離すと発射
		if (CHECK_PRESS_JOY(JOY_INPUT_SUB_WEAPON) == false) {

			SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_ATTACK_1);
			SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
			mMotion.mSpeed = 0.35f;

			TotalInfo::GetInstance()->DisableLockonLayout();

			unprotectAllEnemyDamageFlag();
		}
	}

	const float kCOPY_ROD_RELEASE_FRAME = 3.5f;

	if (EqualPhaseFrame(kCOPY_ROD_RELEASE_FRAME)) {
		VECTOR move_vec;
		float speed = 60.0f;
		VECTOR node_pos;
		p_pmd->GetFramePosFromName(&node_pos, "剣先");
		move_vec = VScale(VNorm(VSub(mCameraGeneralVec, node_pos)), speed);
		spPlayerCopyRod->ProcessShoot(&move_vec, 10.0f, 50.0f, mpLockonChara);
	}
	else if (BetweenPhaseFrame(0.0f, kCOPY_ROD_RELEASE_FRAME)) {
		PRINT_SCREEN(COLOR_RED, "Attach Copy Rod");

		MATRIX node_mtx;
		p_pmd->GetFrameMatrixFromName(&node_mtx, "剣先");
		spPlayerCopyRod->SetCopyRodMatrix(&node_mtx);
	}




	// モーション終了の待ち
	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateCopyRod(void) 
{
	if (spPlayerCopyRod->State() == ObjectWeapon_CopyRod::eWEAPON_COPY_ROD_STATE_ATTACH) {
		TotalInfo::GetInstance()->DisableLockonLayout();
		spPlayerCopyRod->ProcessDisappear();
	}

	// アタッチパターンを変更
	ChangeAttachPattern(eATTACH_PATTERN_NORMAL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateAfterEvent(void)
{
	mIsAway = true;		// キャンセルボタン押下後のジャンプ回避(すいません、使いまわしです。。。)
	StartMotionEx(&sMotionParam[sIdleMotionIndex]);

	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateAfterEvent(void)
{
	if ((!(CHECK_PRESS_JOY(JOY_INPUT_ATTACK))) &&
		(!(CHECK_PRESS_JOY(JOY_INPUT_JUMP)))) {

		mIsAway = false;		// キャンセルボタン押下後のジャンプ回避(すいません、使いまわしです。。。)

		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseLeaveStateAfterEvent(void) 
{
	return;
}

//---------------------------------------------------

/**
	
 */
//---------------------------------------------------
void CharaPlayer::phaseLeaveStateIdle(void) {}
void CharaPlayer::phaseLeaveStateTurn(void) {}
void CharaPlayer::phaseLeaveStateJumpUp(void) {}
void CharaPlayer::phaseLeaveStateJumpDown(void) {}
void CharaPlayer::phaseLeaveStateJumpEnd(void) {}
void CharaPlayer::phaseLeaveStateDamage(void) {}
void CharaPlayer::phaseLeaveStateRetireStart(void) {}
void CharaPlayer::phaseLeaveStateRetireDoing(void) {}
void CharaPlayer::phaseLeaveStateRetireEnd(void) {}
void CharaPlayer::phaseLeaveStateSlopeDown(void) {}
void CharaPlayer::phaseLeaveStateRefuseInput(void) {}
void CharaPlayer::phaseLeaveStateDownStart(void) {}
void CharaPlayer::phaseLeaveStateDownDoing(void) {}
void CharaPlayer::phaseLeaveStateDownEnd(void) {}
void CharaPlayer::phaseLeaveStateFallDown(void) {}
void CharaPlayer::phaseLeaveStateAttack1(void) {}
void CharaPlayer::phaseLeaveStateAttack2(void) {}
void CharaPlayer::phaseLeaveStateAttack3(void) {}
void CharaPlayer::phaseLeaveStateAttack4(void) {}
void CharaPlayer::phaseLeaveStateAttackThrust(void) {}
void CharaPlayer::phaseLeaveStatePutSword(void) {}
void CharaPlayer::phaseLeaveStateAttackJump(void) {}
void CharaPlayer::phaseLeaveStateSleep(void) {}
void CharaPlayer::phaseLeaveStateWaitResetCamera(void) {}
void CharaPlayer::phaseLeaveStateLibShieldStart(void) {}
void CharaPlayer::phaseLeaveStateRaiseThrow(void) {}
void CharaPlayer::phaseLeaveStateBombStart(void) {}

void CharaPlayer::phaseLeaveStateAttackSmashStart(void) {}
void CharaPlayer::phaseLeaveStateAttackSmashDoing(void) {}
void CharaPlayer::phaseLeaveStateAttackSmashEnd(void) {}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateIdleCaseGuardian(void)
{
	CharaPlayer::phaseUpdateStateIdle();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateMoveCaseGuardian(void)
{
	StartMotionEx(&sMotionParam[eMT_WALK]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateMoveCaseGuardian(void)
{
	mRunSpeed = kCHARA_RUN_SPEED * 0.5f;

	sFootstepKind = eFOOTSTEP_KIND_GUARDIAN;

	CharaPlayer::phaseUpdateStateMove();

	// 足音の演出
	if (EqualPhaseFrame(2) || EqualPhaseFrame(18)) {
		CameraManager::GetInstance()->StartVibeCamera(0.4f, 3.0f, 20.0f);

	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseEnterStateAttack1CaseGuardian(void)
{
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	unprotectAllEnemyDamageFlag();

	StartMotion(eMT_ATTACK_1, false, 0.75f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::phaseUpdateStateAttack1CaseGuardian(void)
{
	static const float sSphereSize	= 150.0f;

	float move_speed = 0.08f * (100 - GetMotionTimePercent());
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);
	
	if (BetweenPhaseFrame(37.0f, 50.0f)) {
		
		mIsAttack = true;

		// 敵のノードとキャラの円柱の当たり判定
		CollisionManager::tPillarWork piller;
		CollisionManager::tSphereWork sphere;
		this->GetBodyFrameSphere(&sphere, "右手首", sSphereSize);
		VECTOR damage_down_vec = ZERO_VEC;

		ModelCollision* p_your = NULL;
		for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
			p_your = (ModelCollision *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
			if (p_your == this) continue;			// 自分はのぞく
			if (p_your->GetIsRetire()) continue;	// 戦闘不能者も除く
			if (p_your->IsDamage()) continue;		// 既に攻撃された者も除く

			/* 戦闘不能者と既に攻撃された者を除く */
			if (p_your->CheckEnableAttack() == true) {
				p_your->GetBodyPillar(&piller);
				if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {

					damage_down_vec = VNorm(VSub(p_your->Position(), mPosition));
					p_your->ProcessDamage(12, false, &sphere.mPosition, &damage_down_vec, true, eATTACK_TYPE_GUARDIAN);
					SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				}
			}
		}

		GimmickBase* p_gimmick = NULL;
		for (int iI = 0; iI < GimmickBase::GetGimmickBaseNum(); iI ++) {
			p_gimmick = (GimmickBase *)GimmickBase::GetGimmickBase(iI);

			if (p_gimmick->GetKind() == eGIMMICK_KIND_PRESS_WALL) {

				// 壁とプレイヤーの当たり判定
				MV1_COLL_RESULT_POLY_DIM HitResDim = {0};

				// プレイヤーの直下に存在する地面のポリゴンを取得
				HitResDim = MV1CollCheck_Capsule(p_gimmick->GetModelHandle(), -1, 
												 sphere.mPosition,
												 VGet(sphere.mPosition.x, sphere.mPosition.y + 1.0f, sphere.mPosition.z),
												 sphere.mSize);
				if (HitResDim.HitNum > 0) {
					damage_down_vec = VNorm(VSub(p_gimmick->Position(), mPosition));
					p_gimmick->ProcessDamage(12, false, &sphere.mPosition, &damage_down_vec, true, eATTACK_TYPE_GUARDIAN);

					MV1CollResultPolyDimTerminate( HitResDim ) ;
				}
			}
		}

	}
	else {
		mIsAttack = false;
	}

	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

/*=====================================*
 * サブウェポンの処理
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::subweaponFuncNone(void)
{
	return; /* nop */
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaPlayer::subweaponFuncBoomerang(void)
{
	ModelPmd::tAttachWork* p_attach = GetAttachAddr(eATTACH_MODEL_SUB_WEAPON);
	ModelPmd* p_pmd = (ModelPmd *)p_attach->mpAttachedModel;

	if (CameraManager::GetInstance()->GetCameraState() == eCAM_STATE_BOOMERANG) {
		int value_x, value_y;
		value_x = value_y = 0;
		InputManager::GetInstance()->GetJoyAnalogRight(&value_x, &value_y);
		if ((CHECK_PRESS_MOUSE(MOUSE_INPUT_RIGHT)) ||
			(value_x != 0) || (value_y != 0)) {
			CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_HIGH, true);

			// カメラを戻す
			if (mpLockonChara == NULL) {
				CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
			} 
			else {
				CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_LOCKON);
			}
		}
	}

	/* ブーメランの軌道処理 */
	if ((sBoomerang.mState == eBOOMERANG_STATE_FOWARD_NO_DAMAGE) ||
		(sBoomerang.mState == eBOOMERANG_STATE_FOWARD_DAMAGED)) {

		sBoomerang.mMoveVec = VAdd(sBoomerang.mMoveVec, sBoomerang.mRevVec);
		p_pmd->Position() = VAdd(p_pmd->Position(), sBoomerang.mMoveVec);

		if (VSize(sBoomerang.mMoveVec) <= VSize(sBoomerang.mRevVec)) {
			unprotectAllEnemyDamageFlag();
			sBoomerang.mState = eBOOMERANG_STATE_REVERSE_NO_DAMAGE;
			PRINT_CONSOLE("Line : %d \n", __LINE__);
		}
	}
	else {
		VECTOR vec;
		VECTOR node_pos;
		GetFramePosFromName(&node_pos, "右手首");
		vec = VSub(node_pos, p_pmd->Position());
		vec = VNorm(vec);
		float size;
		size = VSize(sBoomerang.mMoveVec);
		size += (80.0f * 0.015f);
		sBoomerang.mMoveVec = VScale(vec, size);
		p_pmd->Position() = VAdd(p_pmd->Position(), sBoomerang.mMoveVec);
		// ブーメランとキャラが近い時
		if (VSize(VSub(p_pmd->Position(), node_pos)) < VSize(sBoomerang.mMoveVec)) {
			SoundManager::GetSndComponent()->PlaySound(eSE_HOOK_END);
			p_pmd->StartMotion(0, true, 0.5f);

			// カメラを戻す
			if (mpLockonChara == NULL) {
				CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
			} else {
				CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_LOCKON);
			}

			CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_LOW, true);
			CameraManager::GetInstance()->ResetCameraPosY();

			mpSubweaponFunc		= &CharaPlayer::subweaponFuncNone;
			mPhaseEnterState[eCHARA_STATE_SUBWEAPON_START]	= &CharaPlayer::phaseEnterStateBoomerangStart;
			mPhaseState[eCHARA_STATE_SUBWEAPON_START]		= &CharaPlayer::phaseUpdateStateBoomerangStart;

			mSubweaponBladeEffect.Stop();

			sBoomerang.mState = eBOOMERANG_STATE_NONE;

			// アタッチパターンを変更
			ChangeAttachPattern(eATTACH_PATTERN_NORMAL);

			// 終了
			return;
		}
	}
	VECTOR tipA;
	p_pmd->GetFramePosFromName(&tipA, "端1");
	mSubweaponBladeEffect.Update(&tipA, &VSub(tipA, sBoomerang.mMoveVec));

	/* 風きり音をキャラとブーメランの位置によって立体的に再生 */
	if (SoundManager::GetSndComponent()->IsPlay(eSE_BOOMERANG_RUN) == false) {
		static const float kRATE_DIST = (2500.0f * ALL_SCALE);
		float dist = VSize(VSub(p_pmd->Position(), this->mPosition));
		int volume;
		volume = (int)(((kRATE_DIST - dist) / kRATE_DIST) * 255);
		if (volume < 0) volume = 0;

		SoundManager::GetSndComponent()->SetVolume(eSE_BOOMERANG_RUN, volume);
		SoundManager::GetSndComponent()->PlaySound(eSE_BOOMERANG_RUN);
	}

	/* 壁か地面にブーメランが当たった場合 */
	VECTOR weapon_top_pos = p_pmd->Position();
	VECTOR weapon_btm_pos = VSub(weapon_top_pos, sBoomerang.mMoveVec);
	VECTOR hitpos, normal;
	if ((sBoomerang.mState == eBOOMERANG_STATE_FOWARD_NO_DAMAGE) ||
		(sBoomerang.mState == eBOOMERANG_STATE_FOWARD_DAMAGED)) {
		if ((CollisionManager::GetInstance()->CheckEarthCollision(weapon_btm_pos, weapon_top_pos, &hitpos, &normal) != NOT_ON_COLLISION) ||
			(CollisionManager::GetInstance()->CheckWallCollision(weapon_btm_pos, weapon_top_pos, &hitpos, &normal))) {
			unprotectAllEnemyDamageFlag();
			sBoomerang.mState = eBOOMERANG_STATE_REVERSE_NO_DAMAGE;
			PRINT_CONSOLE("Line : %d \n", __LINE__);

			sBoomerang.mMoveVec = ZERO_VEC;
		}
	}

	/* 攻撃の処理 */
	CharaBase* p_mine = this;
	CharaBase* p_your = NULL;
	for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
		p_your = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
		if (p_mine == p_your) continue;			// 自分はのぞく
		if (p_your->GetIsRetire()) continue;	// 戦闘不能者も除く
		if (p_your->IsDamage()) continue;		// 既に攻撃された者も除く


		tCheckDamageWork param;
		param.mPlayerPos = p_pmd->Position();
		param.mWeaponTopPos = weapon_top_pos;
		param.mWeaponBtmPos = weapon_btm_pos;

		if (p_your->CheckDamageCollBoomerang(&param)) {
			SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			mAttackPower = 1;

			if (sBoomerang.mState == eBOOMERANG_STATE_FOWARD_NO_DAMAGE) {
				sBoomerang.mState = eBOOMERANG_STATE_FOWARD_DAMAGED;
				PRINT_CONSOLE("Line : %d \n", __LINE__);

			} else if (sBoomerang.mState == eBOOMERANG_STATE_REVERSE_NO_DAMAGE) {
				sBoomerang.mState = eBOOMERANG_STATE_REVERSE_DAMAGED;
				PRINT_CONSOLE("Line : %d \n", __LINE__);

			}

			// 本来の処理
			VECTOR eff_pos = p_pmd->Position();
			VECTOR damage_down_vec;
			damage_down_vec = VSub(p_your->Position(), p_pmd->Position());
			damage_down_vec = VNorm(damage_down_vec);
			p_your->ProcessDamage(mAttackPower, false, &eff_pos, &damage_down_vec, false, eATTACK_TYPE_BOOMERANG);
		}
	}
}

//---------------------------------------------------
/**
 * リノ専用の描画関数
 * 前髪の透過をなんとかする為
 */
//---------------------------------------------------
void DrawLinoFunc(int model_handle)
{
	static const BYTE kRATE_DRAW_FRAME = 23;

	int mesh_num = MV1GetMeshNum(model_handle);

	for (int iI = 0; iI < kRATE_DRAW_FRAME; iI ++) {
		MV1DrawMesh(model_handle, iI);
	}
	for (int iI = kRATE_DRAW_FRAME+1; iI < mesh_num; iI ++) {
		MV1DrawMesh(model_handle, iI);
	}
	MV1DrawMesh(model_handle, kRATE_DRAW_FRAME);
}

//---------------------------------------------------
/**
 * きつーね専用の描画関数
 */
//---------------------------------------------------
void DrawKitsuneFunc(int model_handle)
{
	static const BYTE kRATE_DRAW_FRAME = 18;

	int mesh_num = MV1GetMeshNum(model_handle);

	for (int iI = 0; iI < kRATE_DRAW_FRAME; iI ++) {
		MV1DrawMesh(model_handle, iI);
	}
	for (int iI = kRATE_DRAW_FRAME+1; iI < mesh_num; iI ++) {
		MV1DrawMesh(model_handle, iI);
	}
	MV1DrawMesh(model_handle, kRATE_DRAW_FRAME);
}

//---------------------------------------------------
/**
 * エルダ専用の描画関数
 */
//---------------------------------------------------
void DrawEldaFunc(int model_handle)
{
	static int target_mesh = 8;

	int mesh_num = MV1GetMeshNum(model_handle);
	for (int iI = 0; iI < target_mesh; iI ++) {
		MV1DrawMesh(model_handle, iI);
	}
	for (int iI = target_mesh+1; iI < mesh_num; iI ++) {
		MV1DrawMesh(model_handle, iI);
	}

	// αテスト判定の値は100が理想 (実験済み)
#ifdef LIB_VER_3_12e
	MV1SetMaterialDrawAlphaTestAll(model_handle, TRUE, DX_CMP_GREATER, 100);
#else 
	SetDrawAlphaTest(DX_CMP_GREATER, 100);
#endif // (LIB_VER == 3_12e)

	MV1DrawMesh(model_handle, target_mesh);

#ifdef LIB_VER_3_12e
	MV1SetMaterialDrawAlphaTestAll(model_handle, FALSE, DX_CMP_GREATER, 0);
#else 
	SetDrawAlphaTest( -1, 0 ) ;
#endif 
}

//---------------------------------------------------
/**
 * リノ専用の描画関数
 * 前髪の透過をなんとかする為
 */
//---------------------------------------------------
void DrawUsamiFunc(int model_handle)
{
	int mesh_num = MV1GetMeshNum(model_handle);

	for (int iI = 0; iI < 19; iI ++) {
		MV1DrawMesh(model_handle, iI);
	}
	for (int iI = 19+1; iI < 23; iI ++) {
		MV1DrawMesh(model_handle, iI);
	}
	for (int iI = 23+1; iI < mesh_num; iI ++) {
		MV1DrawMesh(model_handle, iI);
	}

	MV1DrawMesh(model_handle, 23);
	MV1DrawMesh(model_handle, 19);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void DrawDemonFunc(int model_handle)
{
	MV1DrawMesh(model_handle, 0);
	MV1DrawMesh(model_handle, 1);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void DrawSentoFunc(int model_handle)
{
#ifdef LIB_VER_3_12e
	MV1SetMaterialDrawAlphaTestAll(model_handle, TRUE, DX_CMP_GREATER, 100);
#else
	SetDrawAlphaTest(DX_CMP_GREATER, 100);
#endif 

	MV1DrawMesh(model_handle, 0);

#ifdef LIB_VER_3_12e
	MV1SetMaterialDrawAlphaTestAll(model_handle, FALSE, DX_CMP_GREATER, 0);
#else
	SetDrawAlphaTest( -1, 0 ) ;
#endif 
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void DrawRafiaFunc(int model_handle)
{
	int mesh_num = MV1GetMeshNum(model_handle);

	for (int iI = 0; iI < 19; iI ++) {
		MV1DrawMesh(model_handle, iI);
	}
	MV1DrawMesh(model_handle, 21);

	for (int iI = 25; iI < mesh_num; iI ++) {
		MV1DrawMesh(model_handle, iI);
	}
}

/**** end of file ****/

