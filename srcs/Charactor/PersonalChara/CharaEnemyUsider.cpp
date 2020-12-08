/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ウサイダーまんファイル
 * @author SPATZ.
 * @data   2015/12/31 10:04:48
 */
//---------------------------------------------------
#include "../CharaPlayer.h"
#include "CharaEnemyUsider.h"
#include "Charactor/CharaInfo.h"
#include "AppLib/Basic/Basic.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Effect/EffectManager.h"
#include "AppLib/Graphic/EffekseerManager.h"
#include "AppLib/Input/InputManager.h"
#include "AppLib/Sound/SoundManager.h"

#include "AppLib/Collision/CollisionManager.h"
#include "DataDefine/MotionDef.h"
#include "DataDefine/VoiceDef.h"
#include "DataDefine/EffectDef.h"
#include "Scene/ActionTaskManager.h"
#include "Utility/Utility.h"

static const char* strAttack01_Top = "右中指３";

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyUsider::CharaEnemyUsider()
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsider::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	mEfkHandle	= NULL;

	mRushVecZ	= ZERO_VEC;
	mRushCrossX	= ZERO_VEC;
	mEffHndSmoke= -1;

	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	mFlinchMax = 120;
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyUsider::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);

	int rand_value = GetRand(100);
	float rand_rate = (float)rand_value * 0.01f;
	float dist = VSize(vec);

	if (dist < (mBodySize * 1.0f) + ((mBodySize * 1500.0f) * rand_rate) ) {
		*pAttackCharaState = eCHARA_STATE_ATTACK_2;
		return true;
	}
	
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsider::phaseStateWaitAttack(void)
{
	ChangeState(mReserveCharaState);
	mReserveCharaState = -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsider::processNoMoveAction(void)
{
	//<! 何もしない
	// nop
//	ChangeState(eCHARA_STATE_IDLE);
}

/*=====================================*
 * extend
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsider::phaseEnterStateIdle(void)
{
	// 煙エフェクトの処理
	if (mEffHndSmoke != -1) {
		EffectManager::GetInstance()->Stop(mEffHndSmoke, 0.0f);
		mEffHndSmoke = -1;
	}

	CharaEnemy::phaseEnterStateIdle();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsider::phaseEnterStateAttack1(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsider::phaseStateAttack1(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsider::phaseEnterStateAttack2(void)
{
	StartMotion(eMT_EXTEND_ATTACK_1, false, 0.55f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsider::phaseStateAttack2(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	// 回転モーション以外は、突進の動きのパラメータの更新を行う
	if ((BetweenPhaseFrame(0.0f,	87.0f ))  ||
		(BetweenPhaseFrame(348.0f,	370.0f))  ||
		(BetweenPhaseFrame(404.0f,	550.0f))  ||
		(BetweenPhaseFrame(810.0f,	834.0f))  ||
		(BetweenPhaseFrame(866.0f,	1012.0f))) {

		// 煙エフェクトの処理
		if (mEffHndSmoke != -1) {
			EffectManager::GetInstance()->Stop(mEffHndSmoke, 1.0f);
			mEffHndSmoke = -1;
		}

		mPrevPosition = ZERO_VEC;
		updateRollingParameter();
		mRollingCrushCnt = 0;
	}
	else {
		
		if (mEffHndSmoke == -1) {
			VECTOR effpos = mPosition;
			mEffHndSmoke = EffectManager::GetInstance()->Play(eEF_WHITE_TAIL, &effpos, 150.0f, 0.5f);
			EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "ColorRate",			0.36f);
			EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "FireRatio",			0.45f);
			EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "particleSpread",		30.0f);
			EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "particleSystemHeight",30.0f);
		}

		// 2軸の移動量を加算
		mMoveVec = VAdd(VScale(mRushVecZ, mVx),
						VScale(mRushCrossX, (mVy0 + mAccel)));
		mAccel += mConstGravitySpeed;

		/* 戦闘不能者と既に攻撃された者を除く */
		if (p_player->CheckEnableAttack() == true) {

			CollisionManager::tSphereWork sphere;
			sphere.mPosition	= VAdd(mPosition, mMoveVec);
			sphere.mSize		= mBodySize * 2.0f;

			VECTOR vec;
			float dist;
			if (p_player->CheckBodyCollision(sphere, vec, dist)) {
				VECTOR damage_down_vec = VNorm(VSub(p_player->Position(), mPosition));
				p_player->ProcessDamage(4, false, NULL, &damage_down_vec, true, eATTACK_TYPE_BREAK_GUARD);
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			}
		}

		if (mRollingCrushCnt > 4) {
			ChangeState(eCHARA_STATE_IDLE);
		}
		else {
			if (VSize(VSub(mPosition, mPrevPosition) ) == 0.0f ) {
				int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST, &mPosition, 100.0f, 0.0f);
				EffectManager::GetInstance()->Stop(eff_handle, 2.0f);
				CameraManager::GetInstance()->StartVibeCamera(1.0f, 8.0f, 10.0f);
				SoundManager::GetSndComponent()->PlaySound(eSE_BURST);

				mPrevPosition = ZERO_VEC;
				updateRollingParameter();
				mRollingCrushCnt ++;
			}
		}
	}

	// 煙エフェクトの処理
	if (mEffHndSmoke != -1) {
		VECTOR effpos = mPosition;
		effpos.y += (mBodyHeight * 0.5f);
		EffectManager::GetInstance()->Position(mEffHndSmoke) = effpos;
		EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "particleSpeed", 0.5f);


		// モーター音の管理
		if (SoundManager::GetSndComponent()->IsPlay(eSE_MOTOR_RUNNING) == false){
			SoundManager::GetSndComponent()->PlaySound(eSE_MOTOR_RUNNING);

			EffekseerManager::GetInstance()->Play(eEFK_SHOCK_THIN, &mPosition, 100.0f);
		}
		if (SoundManager::GetSndComponent()->IsPlay(eSE_MOTOR_RUNNING) ){
			SoundManager::GetSndComponent()->SetVolume(eSE_MOTOR_RUNNING, getMotorVolume() );
		}
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
CharaEnemyUsider::phaseEnterStateDamage(void)
{
	CharaEnemy::phaseEnterStateDamage();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsider::phaseEnterStateRetireEnd(void)
{
	CharaEnemy::phaseEnterStateRetireEnd();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsider::updateRollingParameter(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	bool  result = false;
	VECTOR vec = VSub(p_player->Position(), mPosition);
	float dist = VSize(vec);
	float a, b, c;
	float time;
	float accel = mConstGravitySpeed;

	mAccel	= 0.0f;
	mVy0		= 30.0f;

	a = 1.0f;
	b = (2.0f * mVy0) / accel;
	c = 0.0f;
	
	result = Utility::GetFormulaOfTheSolution(&time, a, b, c);
	APP_ASSERT((result != false), "Failed GetFormulaOfTheSolution()\n");

	mVx = (dist / time);
	mVx *= 1.03f;		// 少しだけ補正

	mRushVecZ	= VNorm(vec);
	int rand_value = GetRand(1);
	switch (rand_value) {
	case 0:
		mRushCrossX	= VCross(mRushVecZ, VGet(0.0f, 1.0f, 0.0f));
		break;
	case 1:
		mRushCrossX	= VCross(mRushVecZ, VGet(0.0f, -1.0f, 0.0f));
		break;
	default:
		break;
	};
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
CharaEnemyUsider::getMotorVolume(void)
{
	static const float judge_dist_rush_end = 10000.0f;

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	float dist_to_player = VSize(VSub(mPosition, p_player->Position() ) );
	int volume = (255 - (int)( (dist_to_player / (judge_dist_rush_end / 1.2f) ) * 255.0f) );
	if (volume < 0)		volume = 0;
	if (volume > 255)	volume = 255;

	return volume;
}

 /**** end of file ****/
