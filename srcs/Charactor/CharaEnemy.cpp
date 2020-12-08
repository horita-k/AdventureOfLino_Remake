/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 敵クラス
 * @author SPATZ.
 * @data   2012/02/05 21:50:51
 */
//---------------------------------------------------
#include <math.h>

#include "CharaEnemy.h"
#include "AppLib/Basic/Basic.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Input/InputManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Effect/EffectManager.h"

#include "AppLib/Collision/CollisionManager.h"
#include "Scene/TotalInfo.h"
#include "Scene/ActionTaskManager.h"
#include "DataDefine/MotionDef.h"
#include "DataDefine/VoiceDef.h"
#include "DataDefine/EffectDef.h"
#include "Utility/Utility.h"
#include "CharaInfo.h"

#include "Charactor/PersonalChara/CharaEnemyGiginebura.h"
#include "Charactor/PersonalChara/CharaEnemyStrategist.h"
#include "Charactor/PersonalChara/CharaEnemyDragon.h"
#include "Charactor/PersonalChara/CharaEnemyHatophon.h"
#include "Charactor/PersonalChara/CharaEnemyGoburin.h"
#include "Charactor/PersonalChara/CharaEnemyMonk.h"
#include "Charactor/PersonalChara/CharaEnemyKnight.h"
#include "Charactor/PersonalChara/CharaEnemyWizard.h"
#include "Charactor/PersonalChara/CharaEnemyFresbelk.h"
#include "Charactor/PersonalChara/CharaEnemyArmer.h"
#include "Charactor/PersonalChara/CharaEnemyMagician.h"
#include "Charactor/PersonalChara/CharaEnemyOak.h"
#include "Charactor/PersonalChara/CharaEnemyOakBomb.h"
#include "Charactor/PersonalChara/CharaEnemyEx.h"
#include "Charactor/PersonalChara/CharaEnemyRedDragon.h"
#include "Charactor/PersonalChara/CharaEnemyDemon.h"
#include "Charactor/PersonalChara/CharaEnemyTeeder.h"
#include "Charactor/PersonalChara/CharaEnemyMekaDragon.h"
#include "Charactor/PersonalChara/CharaEnemyMinoArmer.h"
#include "Charactor/PersonalChara/CharaEnemyHongKong.h"
#include "Charactor/PersonalChara/CharaEnemyLoana.h"
#include "Charactor/PersonalChara/CharaEnemyGimuri.h"
#include "Charactor/PersonalChara/CharaEnemyUsae.h"
#include "Charactor/PersonalChara/CharaEnemyUsider.h"
#include "Charactor/PersonalChara/CharaEnemyExcivy.h"

/*=====================================*
 * static 
 *=====================================*/
/*static*/ BOOL	CharaEnemy::sIsStopAllEnemy = FALSE;

/*=====================================*
 * static
 *=====================================*/
/*static*/
CharaEnemy::PHASE_STATE 
CharaEnemy::sPhaseEnterEnemyState[CharaEnemy::eCHARA_STATE_MAX] = {
	&CharaEnemy::phaseEnterStateIdle,
	&CharaEnemy::phaseEnterStateTurn,
	&CharaEnemy::phaseEnterStateMove,
	&CharaEnemy::phaseEnterStateJumpUp,
	&CharaEnemy::phaseEnterStateJumpDown,
	&CharaEnemy::phaseEnterStateJumpEnd,
	&CharaEnemy::phaseEnterStateDamage,
	&CharaEnemy::phaseEnterStateRetireStart,
	&CharaEnemy::phaseEnterStateRetireDoing,
	&CharaEnemy::phaseEnterStateRetireEnd,
	&CharaEnemy::phaseEnterStateSlopeDown,
	&CharaEnemy::phaseEnterStateRefuseInput,	//		eCHARA_STATE_RESERVE_1,
	&CharaEnemy::phaseEnterStateDownStart,		//		eCHARA_STATE_RESERVE_2,
	&CharaEnemy::phaseEnterStateDownDoing,		//		eCHARA_STATE_RESERVE_3,
	&CharaEnemy::phaseEnterStateDownEnd,		//		eCHARA_STATE_RESERVE_4,
	&CharaEnemy::phaseEnterStateFallDown,		//		eCHARA_STATE_RESERVE_5,
	NULL, //		eCHARA_STATE_RESERVE_6,
	NULL, //		eCHARA_STATE_RESERVE_7,
	NULL, //		eCHARA_STATE_RESERVE_8,
	NULL, //		eCHARA_STATE_RESERVE_9,
	&CharaEnemy::phaseEnterStateAttack1,
	&CharaEnemy::phaseEnterStateAttack2,
	&CharaEnemy::phaseEnterStateAttack3,
	&CharaEnemy::phaseEnterStateAttack4,
	&CharaEnemy::phaseEnterStateAttack5,
	&CharaEnemy::phaseEnterStateAttack6,
	&CharaEnemy::phaseEnterStateWaitAttack,
	&CharaEnemy::phaseEnterStateAppear,
	&CharaEnemy::phaseEnterStateAttack7,
	&CharaEnemy::phaseEnterStateAttack8,
	&CharaEnemy::phaseEnterStateAttack9,
};

/*static*/
CharaEnemy::PHASE_STATE 
CharaEnemy::sPhaseEnemyState[CharaEnemy::eCHARA_STATE_MAX] = {
	&CharaEnemy::phaseStateIdle,
	&CharaEnemy::phaseStateTurn,
	&CharaEnemy::phaseStateMove,
	&CharaEnemy::phaseStateJumpUp,
	&CharaEnemy::phaseStateJumpDown,
	&CharaEnemy::phaseStateJumpEnd,
	&CharaEnemy::phaseStateDamage,
	&CharaEnemy::phaseStateRetireStart,
	&CharaEnemy::phaseStateRetireDoing,
	&CharaEnemy::phaseStateRetireEnd,
	&CharaEnemy::phaseStateSlopeDown,
	&CharaEnemy::phaseStateRefuseInput,			//		eCHARA_STATE_RESERVE_1,
	&CharaEnemy::phaseStateDownStart,			//		eCHARA_STATE_RESERVE_2,
	&CharaEnemy::phaseStateDownDoing,			//		eCHARA_STATE_RESERVE_3,
	&CharaEnemy::phaseStateDownEnd,				//		eCHARA_STATE_RESERVE_4,
	&CharaEnemy::phaseStateFallDown,			//		eCHARA_STATE_RESERVE_5,
	NULL, //		eCHARA_STATE_RESERVE_6,
	NULL, //		eCHARA_STATE_RESERVE_7,
	NULL, //		eCHARA_STATE_RESERVE_8,
	NULL, //		eCHARA_STATE_RESERVE_9,
	&CharaEnemy::phaseStateAttack1,
	&CharaEnemy::phaseStateAttack2,
	&CharaEnemy::phaseStateAttack3,
	&CharaEnemy::phaseStateAttack4,
	&CharaEnemy::phaseStateAttack5,
	&CharaEnemy::phaseStateAttack6,
	&CharaEnemy::phaseStateWaitAttack,
	&CharaEnemy::phaseStateAppear,
	&CharaEnemy::phaseStateAttack7,
	&CharaEnemy::phaseStateAttack8,
	&CharaEnemy::phaseStateAttack9,
};

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/CharaEnemy*
CharaEnemy::CreateInstance(USHORT enemyKind)
{
	CharaEnemy* p_create_enemy = NULL;

	switch (enemyKind) {
 	case eENEMY_KIND_GIGINEBURA:
		p_create_enemy = new CharaEnemyGiginebura;		break;
	case eENEMY_KIND_STRATEGIST:
		p_create_enemy = new CharaEnemyStrategist;		break;
	case eENEMY_KIND_DRAGON:
	case eENEMY_KIND_DRAGON_B:
		p_create_enemy = new CharaEnemyDragon;			break;
	case eENEMY_KIND_ANHEL:
		APP_ASSERT( (NULL), "Not Support eENEMY_KIND_ANHEL");
	case eENEMY_KIND_HATOPHON:
	case eENEMY_KIND_HATOPHON_Z:
		p_create_enemy = new CharaEnemyHatophon;			break;
	case eENEMY_KIND_GOBULIN:
	case eENEMY_KIND_GOBULIN_B:
		p_create_enemy = new CharaEnemyGoburin;			break;
	case eENEMY_KIND_MALON:
		p_create_enemy = new CharaEnemyMonk;				break;
	case eENEMY_KIND_KNIGHT:
		p_create_enemy = new CharaEnemyKnight;			break;
	case eENEMY_KIND_FRESBELK:
		p_create_enemy = new CharaEnemyFresbelk;			break;
	case eENEMY_KIND_WIZARD:
		p_create_enemy = new CharaEnemyWizard;			break;
	case eENEMY_KIND_ARMER:
		p_create_enemy = new CharaEnemyArmer;				break;
	case eENEMY_KIND_MAGICIAN:
		p_create_enemy = new CharaEnemyMagician;			break;
	case eENEMY_KIND_OAK:
		p_create_enemy = new CharaEnemyOak;				break;
	case eENEMY_KIND_SLIME:
		p_create_enemy = new CharaEnemySlime;				break;
	case eENEMY_KIND_WOODMAN:
		p_create_enemy = new CharaEnemyWoodman;			break;
	case eENEMY_KIND_RED_DRAGON:
		p_create_enemy = new CharaEnemyRedDragon;			break;
	case eENEMY_KIND_MINOTAURUS:
		p_create_enemy = new CharaEnemyMinotaurus;		break;
	case eENEMY_KIND_DEMON:
		p_create_enemy = new CharaEnemyDemon;				break;
	case eENEMY_KIND_SCORPION:
		p_create_enemy = new CharaEnemyScorpion;			break;
	case eENEMY_KIND_ROBOT_A:
		p_create_enemy = new CharaEnemyRobotA;			break;
	case eENEMY_KIND_ROBOT_B:
		p_create_enemy = new CharaEnemyRobotB;			break;
	case eENEMY_KIND_TEEGER:
	case eENEMY_KIND_TEEGER_M:
		p_create_enemy = new CharaEnemyTeeder;			break;
	case eENEMY_KIND_HELICOPTER:
		p_create_enemy = new CharaEnemyHelicopter;		break;
	case eENEMY_KIND_MEKADRAGON:
		p_create_enemy = new CharaEnemyMekaDragon;		break;
	case eENEMY_KIND_SA_14:
		p_create_enemy = new CharaEnemySA_14;				break;
	case eENEMY_KIND_MINO_ARMER:
		p_create_enemy = new CharaEnemyMinoArmer;		break;
	case eENEMY_KIND_HONGKONG:
		p_create_enemy = new CharaEnemyHongKong;		break;
	case eENEMY_KIND_LOANA:
		p_create_enemy = new CharaEnemyLoana;			break;
	case eENEMY_KIND_OAK_BOMB:
		p_create_enemy = new CharaEnemyOakBomb;			break;

	case eENEMY_KIND_GIMURI:
		p_create_enemy = new CharaEnemyGimuri;			break;
	case eENEMY_KIND_USAEMON:
		p_create_enemy = new CharaEnemyUsae;			break;
	case eENEMY_KIND_USIDERMAN:
		p_create_enemy = new CharaEnemyUsider;			break;
	case eENEMY_KIND_EXCIVY:
		p_create_enemy = new CharaEnemyExcivy;			break;


	default:
		p_create_enemy = new CharaEnemy;					break;
	};

	return p_create_enemy;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
CharaEnemy::SetStopAllEnemyFlag(BOOL isSet)
{
	sIsStopAllEnemy = isSet; 
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemy::CharaEnemy()
{
	mMoveSpeed = kCHARA_WALK_SPEED;
	memset(&mMoveHistory, 0x00, sizeof(mMoveHistory));
	mReserveCharaState = -1;
	//	mpLayoutName = NULL;
	mpEnemyName = NULL;
	mAppearWaitCount= 0;
	mAppearWaitMax	= 0;

	memset(&mItemInfo, 0, sizeof(mItemInfo));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemy::~CharaEnemy()
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::Setup(const char* modelName, int maxHitpoint, const char* enemyName)
{
	memcpy(mPhaseEnterState, sPhaseEnterEnemyState, sizeof(sPhaseEnterEnemyState));
	memcpy(mPhaseState, sPhaseEnemyState, sizeof(sPhaseEnemyState));
	
	CharaBase::Setup();
	mShadow.Setup(this, Shadow::eUPDATE_TYPE_ALWAYS);

	mMaxHitpoint= maxHitpoint;
	mHitpoint	= mMaxHitpoint;

	mpEnemyName = (char*)enemyName;

	(this->*mPhaseEnterState[mCharaState])();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::Update(void)
{
	if (mAlpha <= 0.0) return;

	ModelPmd::Update();
	mMoveVec = ZERO_VEC;

	if (sIsStopAllEnemy == FALSE) {
		(this->*mPhaseState[mCharaState])();
	}

	CharaBase::Update();

	// mLastEarthPos更新処理		
	updateLastEarthPos();

	// ダメージ後の怯みカウンタの処理
	mFlinchCount -= 1;
	if (mFlinchCount <= 0) {
		mFlinchCount = 0;
	}

#ifdef _DEBUG
	if (CHECK_PRESS_KEY(DBG_KEY_HEAL_ENEMY)) {
		mHitpoint = mMaxHitpoint;
		ChangeState(eCHARA_STATE_IDLE);
	}
#endif // _DEBUG	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::ChangeState(int setState)
{
	if (setState == mCharaState) return;
	if (mIsActive == false) return;

	mCharaState = setState;

	APP_ASSERT_PRINT((setState > -1) && (setState < eCHARA_STATE_MAX),
					  "Select setState %d is Invalid Value\n", mCharaState);
	(this->*mPhaseEnterState[mCharaState])();
}

//---------------------------------------------------
/**
   ダメージを食らった時の処理
 */
//---------------------------------------------------
void
CharaEnemy::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	CharaBase::ProcessDamage(attackPower, isSetMode, pEffPos, pDamageDownVec, isDamageDown, attackType);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::ProcessAttackToPlayer(const float startFrame, const float endFrame, const char* nodeName, float nodeSize, int attackPower, bool isDamageDown)
{
	bool is_prev_attack;
	is_prev_attack = mIsAttack;
	if ((GetMotionNowTime() >= startFrame) &&
		(GetMotionNowTime() <= endFrame)) {
		mIsAttack = true;

		CharaBase* p_player;
		p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		if (p_player->GetIsRetire()) return;		// 戦闘不能者も除く
		if (p_player->IsDamage()) return;			// 既に攻撃された者も除く

		// 敵のノードとキャラの円柱の当たり判定
		CollisionManager::tSphereWork sphere;
		CollisionManager::tPillarWork piller;
		this->GetFramePosFromName(&sphere.mPosition, nodeName);
		sphere.mPosition = VAdd(sphere.mPosition, mMoveVec);
		sphere.mSize = nodeSize;

		VECTOR tmpA, tmpB;
		tmpA = sphere.mPosition;
		tmpA.y += sphere.mSize * 0.5f;
		tmpB = sphere.mPosition;
		tmpB.y += -sphere.mSize * 0.5f;
#ifdef _DEBUG
		GraphicManager::GetInstance()->DbgRenderLine(&tmpA, &tmpB);
#endif // _DEBUG

		piller.mPosition	= p_player->Position();
		piller.mSize		= p_player->BodySize();
		piller.mHeight		= p_player->BodyHeight();

#ifdef _DEBUG
		ShapeCapsule capsule;
		capsule.mPosA = sphere.mPosition;
		capsule.mPosB = sphere.mPosition;
		capsule.mPosB.y =+ 0.1f;
		capsule.mRadius = sphere.mSize;
		GraphicManager::GetInstance()->DbgRenderCapsule(&capsule);
#endif // _DEBUG

		if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {
			SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			
			VECTOR damage_down_vec;
			damage_down_vec = VSub(p_player->Position(), mPosition);
			damage_down_vec = VNorm(damage_down_vec);
			p_player->ProcessDamage(attackPower, false, NULL, &damage_down_vec, isDamageDown, eATTACK_TYPE_ENEMY_NORMAL);
		}

	} else {
		mIsAttack = false;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::PermitAcceptAI(bool isPermit)
{
	if (isPermit) {
		// 許可
		mPhaseEnterState[eCHARA_STATE_IDLE]			= &CharaEnemy::phaseEnterStateIdle;
		mPhaseState[eCHARA_STATE_IDLE]				= &CharaEnemy::phaseStateIdle;
	} else {
		// 禁止
		mPhaseEnterState[eCHARA_STATE_IDLE]			= &CharaEnemy::phaseEnterStateRefuseInput;
		mPhaseState[eCHARA_STATE_IDLE]				= &CharaEnemy::phaseStateRefuseInput;
	}
	ChangeState(eCHARA_STATE_IDLE);
}

//---------------------------------------------------
/**
	他のキャラとのあたり判定
 */
//---------------------------------------------------
void
CharaEnemy::UpdateAttackChara(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::AppearEnemy(SHORT waitCount)
{
	mAppearWaitCount= 0;
	mAppearWaitMax	= waitCount;
	ChangeState(eCHARA_STATE_APPEAR);

	this->SetAlpha(0.1f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::SetItemInfo(int itemKind, int getPercent)
{
	mItemInfo.mItemKind = itemKind;
	mItemInfo.mGetPercent = getPercent;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::GetItemInfo(int* pItemKind, int* pGetPercent)
{
	*pItemKind = mItemInfo.mItemKind;
	*pGetPercent = mItemInfo.mGetPercent;
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemy::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	if (VSize(vec) < mBodySize + (mBodySize * (GetRand(200) * 0.005f))) {

		*pAttackCharaState = eCHARA_STATE_ATTACK_1;
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*virtual*/void
CharaEnemy::processNoMoveAction(void)
{
	// 全然動いていない場合はジャンプさせる
	mJumpMoveVec.mDash.x = sin(mRotation.y + Utility::DegToRad(180.0f)) * mMoveSpeed;
	mJumpMoveVec.mDash.z = cos(mRotation.y + Utility::DegToRad(180.0f)) * mMoveSpeed;
	ChangeState(eCHARA_STATE_JUMP_UP);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::processTurnTargetRotY(float turnMoveSpeed, int nextCharaState)
{
	/* キャラクターの向きを処理する */
	Utility::CorrectAngleFrom000To360(&mRotation.y);

	float diff_rot = mTargetRotY - mRotation.y;
	eTurnType turn_type = eTURN_TYPE_NONE;
	
	// キャラの回転するスピード　でかい奴ほど遅い

	if (turnMoveSpeed >= abs(diff_rot)) {
		mRotation.y = mTargetRotY;

		turn_type = updateTargetRotateFromPad();
		mTurnType = turn_type;

		ChangeState(nextCharaState);

	} else {
		bool is_plus = (diff_rot >= 0.0f) ? true : false;
		if (abs(diff_rot) > Utility::DegToRad(180.0f)) {
			is_plus ^= true;
		}
		mRotation.y += (is_plus) ? turnMoveSpeed : -turnMoveSpeed;
	}
}

//---------------------------------------------------
/**
 * 着地した時点でコールされる関数
 * (mGravity の値は0ではない状態)
 */
//---------------------------------------------------
void
CharaEnemy::processLandingFunc(void)
{
	static float sRetireGravityValue = -60.0f;

	// ある程度高い所から落ちた場合、リタイアさせる
	if (mGravity < sRetireGravityValue) {

		// 敵が高い時から落ちた時、落ちる前後の距離を比較し、
		// プレイヤーと遠のいた場合のみ消滅し、近づいた場合は消滅させない様にする。
		CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
		float dist_prev, dist_now;
		dist_prev = VSize(VSub(p_player->Position(), mMoveHistory[0]));
		dist_now  = VSize(VSub(p_player->Position(), mPosition));
		if (dist_now > dist_prev) {
			this->ProcessDamage(-9997, true, (VECTOR *)&ZERO_VEC, (VECTOR *)&ZERO_VEC, false, eATTACK_TYPE_NORMAL); // 擬似的に敵を倒す
			SoundManager::GetSndComponent()->PlaySound(eSE_FALL_STONE);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::updateLastEarthPos(void)
{
	// ラインの上に乗っている場合、かつ、ギミックに乗っていない場合
	if ((mEarthState == eEARTH_STATE_LINE) &&
		(mIsOnGimmick == FALSE)) {
		
		// mLastEarthPosを更新
		mLastEarthPos = mPosition;
	}
}

/*=====================================*
 * mPhaseState
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateIdle(void)
{
	if (this->GetIsRetire()) {
		ChangeState(eCHARA_STATE_RETIRE_START);
		return;
	}

	mIsAttack = false;
	mBladeEffect.Stop();
	mJumpMoveVec.mDash = ZERO_VEC;
	StartMotionEx(&sMotionParam[eMT_IDLE]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateIdle(void)
{
	eTurnType turn_type = eTURN_TYPE_NONE;

	turn_type = updateTargetRotateFromPad();
	mTurnType = turn_type;

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	if (p_player->GetIsRetire() == false) {
		if (GetRand(10) == 0) {
			VECTOR vec = VSub(p_player->Position(), mPosition);
			mTargetRotY = atan2(vec.x, vec.z) + PHI_F;
			ChangeState(eCHARA_STATE_TURN);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateTurn(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateTurn(void)
{
	float turn_move_speed = 60.0f * kTURN_MOVE_ROT / mBodySize;
	processTurnTargetRotY(turn_move_speed, eCHARA_STATE_MOVE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateMove(void)
{
	StartMotionEx(&sMotionParam[eMT_WALK]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateMove(void)
{
	eTurnType turn_type = eTURN_TYPE_NONE;

	VECTOR dir_vec = ZERO_VEC;
	VECTOR org_vec, org_cross_vec, side_vec, player_pos;

	// キャラクタの向き
	turn_type = updateTargetRotateFromPad();
	Utility::CorrectAngleFrom000To360(&mTargetRotY);

	org_vec = VSub(CameraManager::GetInstance()->Target(), CameraManager::GetInstance()->Position());
	org_vec.y = 0.0f;
	org_vec = VNorm(org_vec);
	org_cross_vec = VCross(sUpVec, org_vec);

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	player_pos = p_player->Position();
	player_pos.x += GetRand(10) + 5; // 多少の誤差を入れる
	player_pos.z += GetRand(10) + 5; // 多少の誤差を入れる

	VECTOR vec = VSub(player_pos, mPosition);
	// プレイヤーの方向に向く
	mRotation.y = atan2(vec.x, vec.z) + PHI_F;

#ifdef _DEBUG
	GraphicManager::GetInstance()->DbgRenderLine(&mPosition, &VAdd(mPosition, VScale(dir_vec, 60.0f)));
#endif // #ifdef _DEBUG

	dir_vec = VSub(CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress()->Position(), mPosition);
	dir_vec = VNorm(dir_vec);

	side_vec = VCross(sUpVec, dir_vec);
	dir_vec = VCross(side_vec, mNormalVec);
	
#ifdef _DEBUG
	GraphicManager::GetInstance()->DbgRenderLine(&mPosition, &VAdd(mPosition, VScale(mNormalVec, 60.0f)));
	GraphicManager::GetInstance()->DbgRenderLine(&mPosition, &VAdd(mPosition, VScale(side_vec, 60.0f)));
	GraphicManager::GetInstance()->DbgRenderLine(&mPosition, &VAdd(mPosition, VScale(dir_vec, 60.0f)));
#endif // _DEBUG

	mRunSpeed = mMoveSpeed;
	mMoveVec = VScale(dir_vec, mRunSpeed);

	/* 攻撃のステータスに遷移できるかを判定し、できる場合は遷移する */
	int next_state;
	if (checkChangeAttackState(&next_state)) {
		mReserveCharaState = next_state;
		ChangeState(eCHARA_STATE_WAIT_ATTACK);
	}

	for (int iI = (8 - 1); iI > 0; iI --) {
		mMoveHistory[iI] = mMoveHistory[iI - 1];
	}
	mMoveHistory[0] = mLastEarthPos;

	// 全然移動していない場合の処理
	if (VSize(VSub(mMoveHistory[0], mMoveHistory[7])) < mMoveSpeed * 0.1f) {
		if (mIsLand == true) {
			processNoMoveAction();
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateJumpUp(void)
{
	StartMotionEx(&sMotionParam[eMT_JUMP_START]);

	mIsReserveComboInput = false;
	mIsAttack = false;
	mIsDamage = false;

	/* 解の方程式を用いて移動速度を求める */
	// 1. x = Vo * t + 1/2 * a * t2 の式からt(=time)を求める 
	// 2. 1より、 t2 + (2Vo * t / 2) - (2 * x / a) = 0 になる
	// 3. 解の方程式より x = (-b +/- √b2 - 4 * a * c) / 2a を当てはめてとく

	VECTOR player_pos;
	player_pos = CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress()->Position();

	VECTOR tmpA, tmpB;
	float Vo, x, y, g, in_root;
	float a, b, c;
	float t1, t2, t;
	Vo = (kJUMP_GRAVITY * 0.65f);
	y = player_pos.y - mPosition.y;
	g = mConstGravitySpeed;

	a = 1;
	b = (2 * Vo) / g;
	c = (-2 * y) / g;

	in_root = (b * b) - (4 * a * c);
	if (in_root < 0) {
		/* 解の方程式により、解が見当たらない場合はジャンプが足りないという事なので普通にジャンプ */
		mGravity += Vo;
		mJumpMoveVec.mDash.y = 0.0f;
	} else {
		/* 解の方程式より、解大きい方の解に応じて mJumpMoveVec.mDash の値を求めだす */

		t1 = (-b + sqrt(in_root)) / (2 * a) ;
		t2 = (-b - sqrt(in_root)) / (2 * a) ;
		if (t1 > t2)	t = t1;
		else			t = t2;
		tmpA = player_pos;
		tmpA.y = 0.0f;
		tmpB = mPosition;
		tmpB.y = 0.0f;
		x = VSize(VSub(tmpA, tmpB));

		mGravity += Vo;
		mJumpMoveVec.mDash.y = 0.0f;
		mJumpMoveVec.mDash = VScale(VNorm(mJumpMoveVec.mDash), (x / t)); // v = x / t (速・時・距の公式)
	}

	mIsLand = false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateJumpUp(void)
{
	VECTOR org_vec, org_cross_vec;
	org_vec = VSub(CameraManager::GetInstance()->Target(), CameraManager::GetInstance()->Position());
	org_vec.y = 0.0f;
	org_vec = VNorm(org_vec);
	org_cross_vec = VCross(sUpVec, org_vec);

	mMoveVec = mJumpMoveVec.mDash;

	if (mGravity < 0) {
		ChangeState(eCHARA_STATE_JUMP_DOWN);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateJumpDown(void)
{
	eTurnType turn_type = eTURN_TYPE_NONE;

	VECTOR dir_vec = ZERO_VEC;
	VECTOR org_vec, org_cross_vec, side_vec, player_pos;

	// キャラクタの向き
	turn_type = updateTargetRotateFromPad();
	Utility::CorrectAngleFrom000To360(&mTargetRotY);

	org_vec = VSub(CameraManager::GetInstance()->Target(), CameraManager::GetInstance()->Position());
	org_vec.y = 0.0f;
	org_vec = VNorm(org_vec);
	org_cross_vec = VCross(sUpVec, org_vec);

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	player_pos = p_player->Position();

	VECTOR vec = VSub(player_pos, mPosition);
	// プレイヤーの方向に向く
	mRotation.y = atan2(vec.x, vec.z) + PHI_F;

	dir_vec = VSub(CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress()->Position(), mPosition);
	dir_vec = VNorm(dir_vec);

	side_vec = VCross(sUpVec, dir_vec);
	dir_vec = VCross(side_vec, mNormalVec);

	mRunSpeed = mMoveSpeed;
	mMoveVec = VScale(dir_vec, mRunSpeed);
	mJumpMoveVec.mDash = mMoveVec;

	StartMotionEx(&sMotionParam[eMT_JUMP_DOING]);
}
	
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateJumpDown(void)
{
	phaseStateJumpUp(); // 上昇中と同じ処理

	if (mIsLand == true) {
		mJumpMoveVec.mLanding.x = mMoveVec.x;
		mJumpMoveVec.mLanding.y = 0.0f;
		mJumpMoveVec.mLanding.z = mMoveVec.z;

		ChangeState(eCHARA_STATE_JUMP_END);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateJumpEnd(void)
{
	StartMotion(eMT_JUMP_END, false, 0.75f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateJumpEnd(void)
{
	mJumpMoveVec.mLanding = VScale(mJumpMoveVec.mLanding, 0.95f);
	mMoveVec = mJumpMoveVec.mLanding;

	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateDamage(void)
{
	mIsAttack = false;
	StartMotionEx(&sMotionParam[eMT_DAMAGE]);
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_DAMAGE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateDamage(void)
{
	if (GetMotionIndex() != eMT_DAMAGE) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateRetireStart(void)
{
	mIsAttack = false;

	StartMotionEx(&sMotionParam[eMT_DOWN_START]);

	// プレイヤーとの距離が離れたなどのシステムで倒した場合は悲鳴ボイスを再生
	if (mHitpoint != kSYSTEM_DAMAGE_VALUE) {
		SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_DOWN);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateRetireStart(void)
{
	VECTOR move_vec;
	float move_speed = 0.5f * (float)(100 - GetMotionTimePercent());
	move_vec = VScale(mJumpMoveVec.mDamageDown, move_speed);
	mMoveVec = move_vec;
	if (GetIsEndMotion() && (GetIsMotionBlend() == false)) {
		ChangeState(eCHARA_STATE_RETIRE_DOING);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateRetireDoing(void)
{
	if (mHitpoint != kSYSTEM_DAMAGE_VALUE) {

		SoundManager::GetSndComponent()->PlaySound(eSE_ERASE_MODEL);

		ActionTaskManager::tTaskInfo info;
		info.mTaskType = ActionTaskManager::eTASK_ENEMY_ITEM;
		info.mTaskValueA = (long long)this;
		info.mTaskValueB = 0;
		PRINT_CONSOLE("NotifyAction -> eTASK_ENEMY_ITEM \n");

		ActionTaskManager::GetInstance()->NotifyAction(&info);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateRetireDoing(void)
{
	float alpha = this->GetAlpha();

	alpha += -0.03f;
	if (alpha <= 0.0f) {
		alpha = 0.0f;
		ChangeState(eCHARA_STATE_RETIRE_END);
	}
	this->SetAlpha(alpha);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateRetireEnd(void)
{
	GraphicManager::GetInstance()->ReserveRemoveModel((ModelBase *)this);
	CollisionManager::GetInstance()->RemoveBodyCollision((ModelBase *)this);
	CollisionManager::GetInstance()->RemoveAttackCollision((ModelBase *)this);

	mBladeEffect.Stop();

	this->SetAlpha(0.0f);

	ModelPmd::DeleteModel();
	mIsActive = false;

	GraphicManager::GetInstance()->RemoveDrawShadow(&mShadow);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateRetireEnd(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateSlopeDown(void)
{
	StartMotionEx(&sMotionParam[eMT_JUMP_DOING]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateSlopeDown(void)
{
	if ((mEarthState == eEARTH_STATE_LINE) ||
		(mEarthState == eEARTH_STATE_CAPSULE)) {
		mJumpMoveVec.mLanding = ZERO_VEC;
		StartMotionEx(&sMotionParam[eMT_JUMP_END]);
		ChangeState(eCHARA_STATE_JUMP_END);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateRefuseInput(void)
{
	StartMotionEx(&sMotionParam[eMT_IDLE]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateRefuseInput(void)
{
	// 何もうけつけない
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateDownStart(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_DAMAGE);
	StartMotionEx(&sMotionParam[eMT_DOWN_START]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateDownStart(void)
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
CharaEnemy::phaseEnterStateDownDoing(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateDownDoing(void)
{
	ChangeState(eCHARA_STATE_DOWN_END);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateDownEnd(void)
{
	StartMotion(eMT_DOWN_END, false, 0.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateDownEnd(void)
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
CharaEnemy::phaseEnterStateFallDown(void)
{
	StartMotionEx(&sMotionParam[eMT_JUMP_DOING]);
}

//---------------------------------------------------
/**
 * 垂直直下のフェーズ
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateFallDown(void)
{
	mMoveVec = mJumpMoveVec.mDash;

	if (mIsLand == true) {
		mJumpMoveVec.mLanding.x = mMoveVec.x;
		mJumpMoveVec.mLanding.y = 0.0f;
		mJumpMoveVec.mLanding.z = mMoveVec.z;
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
CharaEnemy::phaseEnterStateAttack1(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	StartMotionEx(&sMotionParam[eMT_ATTACK_1]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateAttack1(void)
{
	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	static const float sStart	= 3.0f;
	static const float sEnd		= 10.0f;

	// この関数内で mIsAttack が変更される可能性があるので注意
	CharaEnemy::ProcessAttackToPlayer(sStart, sEnd, "右手首", 35.0f, 1, false);

	PRINT_SCREEN(GetColor(255, 255, 0), "NowTime : %.2f\n", GetMotionNowTime());

	VECTOR weapon_top_pos;
	if ((is_prev_attack == false) && (mIsAttack == true)) {
		this->GetFramePosFromName(&weapon_top_pos, "右手首");
	}
	else if ((is_prev_attack == true) && (mIsAttack == false)) {
		this->GetFramePosFromName(&weapon_top_pos, "右手首");
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
CharaEnemy::phaseEnterStateAttack2(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateAttack2(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateAttack3(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateAttack3(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateAttack4(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateAttack4(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateAttack5(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateAttack5(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateAttack6(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateAttack6(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateWaitAttack(void)
{
	StartMotionEx(&sMotionParam[eMT_IDLE]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateWaitAttack(void)
{
	if (GetRand(30) == 0) {
		ChangeState(mReserveCharaState);
		mReserveCharaState = -1;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateAppear(void)
{
	//<! 着地時にステータスが変わるバグを封じるために、ダメージ状態にする
	mIsDamage = true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateAppear(void)
{
	if (((mAppearWaitMax >= mAppearWaitCount) && 
		 (mAppearWaitMax < mAppearWaitCount + (SHORT)kBASIC_FRAME_COUNTER))) {
		StartMotionEx(&sMotionParam[eMT_JUMP_START]);
		mGravity += (kJUMP_GRAVITY * 0.6f);
		
		VECTOR effpos = VAdd(mPosition, mMoveVec);
		int eff_handle = EffectManager::GetInstance()->Play(eEF_BLACKDUST, &effpos, 50.0f, 0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 2.0f);
		SoundManager::GetSndComponent()->PlaySound(eSE_ENEMY_APPEAR);
		
		mIsLand = false;
		//<! ダメージ状態を解除
		mIsDamage = false;
		this->SetAlpha(1.0f);
	}

	if (mAppearWaitMax < mAppearWaitCount) {
		if (mCharaState == eCHARA_STATE_JUMP_UP) {
			ChangeState(eCHARA_STATE_IDLE);
		}
	}
	mAppearWaitCount += (SHORT)kBASIC_FRAME_COUNTER;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseEnterStateAttack7(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemy::phaseStateAttack7(void)
{
	return;
}

/**** end of file ****/

