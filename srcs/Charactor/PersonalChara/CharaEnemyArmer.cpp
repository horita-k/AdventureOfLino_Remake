/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ギギネブラファイル
 * @author kenichi-horita.
 * @data   2013/07/21 11:56:23
 */
//---------------------------------------------------
#include "CharaEnemyArmer.h"
#include "Charactor/CharaInfo.h"
#include "AppLib/Basic/Basic.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Effect/EffectManager.h"
#include "AppLib/Input/InputManager.h"
#include "AppLib/Sound/SoundManager.h"

#include "AppLib/Collision/CollisionManager.h"
#include "DataDefine/MotionDef.h"
#include "DataDefine/VoiceDef.h"
#include "DataDefine/EffectDef.h"
#include "Scene/ActionTaskManager.h"
#include "Utility/Utility.h"


//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyArmer::~CharaEnemyArmer() 
{
	GraphicManager::GetInstance()->RemoveDrawShadow(&mAnotherShadow);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyArmer::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	mStateCount = 0;

	mAnotherShadow.Setup(this, Shadow::eUPDATE_TYPE_ALWAYS);
	mAnotherShadow.DisableShadow();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyArmer::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	CharaEnemy::ProcessDamage(attackPower, isSetMode, pEffPos, pDamageDownVec, false, attackType);
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyArmer::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);

	float dist = VSize(vec);
	
	bool result = false;

	if (dist < mBodySize * 4.5f) {
		*pAttackCharaState = eCHARA_STATE_JUMP_UP;
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyArmer::updateAttackCollisionLocal(int attackPower)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	/* 戦闘不能者と既に攻撃された者を除く */
	if ((p_player->GetIsRetire() == false) &&
		(p_player->IsDamage() == false)) {

		ShapeCapsule capsuleA;
		ShapeCapsule capsuleB;
		capsuleA.mPosA	= p_player->Position();
		capsuleA.mPosB	= p_player->Position();
		capsuleA.mPosB.y += p_player->BodyHeight();
		capsuleA.mRadius= p_player->BodySize();

		capsuleB.mPosA	= this->Position();
		capsuleB.mPosB	= this->Position();
		capsuleB.mPosB.y += this->BodyHeight();
		capsuleB.mRadius= this->BodySize();
			
		if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
			SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			VECTOR damage_down_vec;
			VECTOR vecA, vecB;
			vecA = mPosition;
			vecA.y = 0.0f;
			vecB = p_player->Position();
			vecB.y = 0.0f;
			damage_down_vec = VSub(vecB, vecA);
			damage_down_vec = VNorm(damage_down_vec);
			p_player->ProcessDamage(attackPower, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
		}
	}
}

/*=====================================*
 * extend
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyArmer::phaseEnterStateIdle(void)
{
	mAnotherShadow.DisableShadow();

	CharaEnemy::phaseEnterStateIdle();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyArmer::phaseStateIdle(void)
{
	eTurnType turn_type = eTURN_TYPE_NONE;
	turn_type = updateTargetRotateFromPad();
	mTurnType = turn_type;

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	if (p_player->GetIsRetire() == false) {
		VECTOR vec = VSub(p_player->Position(), mPosition);
		mTargetRotY = atan2(vec.x, vec.z) + PHI_F;
		ChangeState(eCHARA_STATE_TURN);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyArmer::phaseEnterStateMove(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyArmer::phaseStateMove(void)
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
	StartMotion(eMT_WALK, true, 0.75f, NULL);
	mMoveVec = VScale(dir_vec, mRunSpeed);

	/* 攻撃のステータスに遷移できるかを判定し、できる場合は遷移する */
	int next_state;
	if (checkChangeAttackState(&next_state)) {
		mReserveCharaState = next_state;
		ChangeState(eCHARA_STATE_WAIT_ATTACK);
	}

	float total_time = GetMotionTotalTime();
	float speed_time = 0.75f;
	if (this->BetweenPhaseFrame( (total_time - speed_time), total_time) ) {
		const float size = 20.0f;
		VECTOR effpos;
		effpos = VAdd(mPosition, mMoveVec);
		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,
															&effpos,
															&VGet(size, size, size),
															0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 0.5f);

		SoundManager::GetSndComponent()->SetVolume(eSE_BURST, 64);
		SoundManager::GetSndComponent()->PlaySound(eSE_BURST);

	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyArmer::phaseEnterStateJumpUp(void)
{
	// 重力を変更
	mConstGravitySpeed = (kGRAVITY_SPEED * 1.6f);

	mIsReserveComboInput = false;
	mIsAttack = false;
	mIsDamage = false;

	// ジャンプ力高めに設定
	mGravity += (kJUMP_GRAVITY * 1.1f);

	//<! ジャンプの最高点がキャラの真上になる様に移動量を計算する
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	float time, speed, dist;
	VECTOR vec;
	vec = VSub(p_player->Position(), mPosition);
	dist = VSize(vec);
	time = - mGravity / mConstGravitySpeed;		// v = vo + at より
	speed = dist / time;						// x = v * t より
	speed *= 0.9f; // 補正のため少し
	mJumpMoveVec.mDash = VScale(VNorm(vec), speed);

	SoundManager::GetSndComponent()->PlaySound(eSE_ROLLING);

	mAnotherShadow.EnableShadow();
	mAnotherShadow.SetShadowHeightOffset(-650.0f);

	mIsLand = false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyArmer::phaseStateJumpUp(void)
{
	CharaEnemy::phaseStateJumpUp();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyArmer::phaseEnterStateJumpDown(void)
{
	mStateCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyArmer::phaseStateJumpDown(void)
{
	if (mStateCount == 30) {
		mConstGravitySpeed = (kGRAVITY_SPEED * 6.0f);
		// 垂直に落ちる
		mJumpMoveVec.mDash = ZERO_VEC;

		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);

	} else if (mStateCount > 20) {
		CharaEnemy::phaseStateJumpDown();
		
		int attack_power = 2;
		updateAttackCollisionLocal(attack_power);
	} else {
		mConstGravitySpeed = 0;
		mGravity = 0.0f;
	}

	mStateCount ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyArmer::phaseEnterStateJumpEnd(void)
{
	const float size = 35.0f;
	VECTOR effpos;
	effpos = VAdd(mPosition, mMoveVec);
	int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,
														&effpos,
														&VGet(size, size, size),
														0.0f);
	EffectManager::GetInstance()->Stop(eff_handle, 0.5f);

	SoundManager::GetSndComponent()->SetVolume(eSE_BURST, 255);
	SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyArmer::phaseEnterStateAttack1(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
	StartMotion(eMT_GOBURIN_ATTACK_1, false, 0.625f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyArmer::phaseStateAttack1(void)
{
	static float sStart = 12.0f;
	static float sEnd = 17.0f;

	float move_speed = 0.16f * (100 - GetMotionTimePercent());
	move_speed *= (mScale.x / GOBLIN_SCALE);
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	bool is_prev_attack;
	is_prev_attack = mIsAttack;
	if ((GetMotionNowTime() >= sStart) &&
		(GetMotionNowTime() <= sEnd)) {
		mIsAttack = true;

		/****/
		CharaBase* p_player;
		p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		/* 戦闘不能者と既に攻撃された者を除く */
		if ((p_player->GetIsRetire() == false) &&
			(p_player->IsDamage() == false)) {

			ShapeCapsule capsuleA;
			ShapeCapsule capsuleB;
			capsuleA.mPosA	= p_player->Position();
			capsuleA.mPosB	= p_player->Position();
			capsuleA.mPosB.y += p_player->BodyHeight();
			capsuleA.mRadius= p_player->BodySize();
		
			this->GetFramePosFromName(&capsuleB.mPosA, "右腕");
			this->GetFramePosFromName(&capsuleB.mPosB, "右中指３");
			capsuleB.mRadius = 60.0f;
			capsuleB.mRadius *= (mScale.x / GOBLIN_SCALE);
			
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 2;
				attack_power *= (int)(mScale.x / GOBLIN_SCALE);
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), mPosition);
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
			}
		}
	} else {
		mIsAttack = false;
	}

	PRINT_SCREEN(GetColor(255, 255, 0), "NowTime : %.2f\n", GetMotionNowTime());

	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyArmer::phaseEnterStateRetireEnd(void)
{
	CharaEnemy::phaseEnterStateRetireEnd();

	GraphicManager::GetInstance()->RemoveDrawShadow(&mAnotherShadow);
}

 /**** end of file ****/
