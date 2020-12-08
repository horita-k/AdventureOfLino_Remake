/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ミノタウロス・アーマークラス
 * @author kenichi-horita.
 * @data   2013/07/21 11:56:23
 */
//---------------------------------------------------
#include "CharaEnemyMinoArmer.h"
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
CharaEnemyMinoArmer::CharaEnemyMinoArmer()
{
	mStatePhase = 0;
	mStateCount = 0;

	mEfsShock	= -1;
	mEfsHndSmoke= -1;

	mShockScaleSize	= 0;
	mIsJumpAttack	= FALSE;		
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyMinoArmer::~CharaEnemyMinoArmer() 
{
	//	SoundManager::GetSndComponent()->SetVolume(eSE_BURST, 255);
	if (mEfsShock != -1) {
		EffectManager::GetInstance()->Stop(mEfsShock< 0.5f);
		mEfsShock = -1;
	}

	if (mEfsHndSmoke != -1) {
		EffectManager::GetInstance()->Stop(mEfsHndSmoke, 0.5f);
		mEfsHndSmoke = -1;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinoArmer::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	mFlinchMax = 250;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinoArmer::Update(void) 
{
	CharaEnemy::Update();

	if (mEfsShock != -1) {
		mShockScaleSize += 25.0f;
		EffectManager::GetInstance()->Scale(mEfsShock) = VGet(mShockScaleSize, 100.0f, mShockScaleSize);

		CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
		/* 戦闘不能者と既に攻撃された者を除く */
		if ((p_player->GetIsRetire() == false) &&
			(p_player->IsDamage() == false)) {

			VECTOR posA, posB, vec;
			posA = p_player->Position();
			posA.y = 0.0f;
			posB = EffectManager::GetInstance()->Position(mEfsShock);
			posB.y = 0.0f;
		
			float dist, shock_size, total_out_size, total_in_size;
			vec = VSub(posA, posB);
			dist = VSize(vec);
			shock_size = mShockScaleSize * 0.8f;
			total_out_size	= p_player->BodySize() + shock_size;
			total_in_size	= total_out_size - 50.0f;

			// 当たり判定
			if ((dist < total_out_size) &&
				(dist > total_in_size)) {
				float player_pos_y, eff_pos_y;
				player_pos_y= p_player->Position().y;
				eff_pos_y	= EffectManager::GetInstance()->Position(mEfsShock).y;

				// 高さをチェック
				if ((player_pos_y + p_player->BodyHeight() >= eff_pos_y) &&
					(player_pos_y <= eff_pos_y + 100.0f) ) {
				
					int attack_power = 1;
					vec = VNorm(vec);
					p_player->ProcessDamage(attack_power, false, &p_player->Position(), &vec, true, eATTACK_TYPE_BREAK_GUARD);
				}
			}
		}

		// 終了判定
		if (mShockScaleSize > 3000.0f) {
			EffectManager::GetInstance()->Stop(mEfsShock, 0.5f);
			mEfsShock = -1;
			mShockScaleSize = 0.0f;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinoArmer::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	CharaEnemy::ProcessDamage(attackPower, isSetMode, pEffPos, pDamageDownVec, false, attackType);
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyMinoArmer::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	float scale_value = 20.0f;
	int rnd = 0;

	scale_value = 4.0f;
	if (VSize(vec) < mBodySize + (mBodySize * (GetRand(100) * 0.01f * scale_value) ) ) {
		rnd = GetRand(5);
		switch (rnd) {
		case 0:	case 1:	*pAttackCharaState = eCHARA_STATE_ATTACK_1;	break;
		case 2:			*pAttackCharaState = eCHARA_STATE_JUMP_UP;	break;
		case 3:			*pAttackCharaState = eCHARA_STATE_ATTACK_2;	break;
		default:		*pAttackCharaState = eCHARA_STATE_ATTACK_3;	break;
		};
		return true;
	}
	scale_value = 20.0f;
	if (VSize(vec) < mBodySize + (mBodySize * (GetRand(100) * 0.01f * scale_value) ) ) {
		rnd = GetRand(50);
		switch (rnd) {
		case 0:
			*pAttackCharaState = eCHARA_STATE_ATTACK_2;	
			return true;
		case 1:
			*pAttackCharaState = eCHARA_STATE_JUMP_UP;	
			return true;
		default:		
			break;
		};
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinoArmer::updateAttackCollisionLocal(int attackPower)
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
			p_player->ProcessDamage(attackPower, false, NULL, &damage_down_vec, true, eATTACK_TYPE_BREAK_GUARD);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinoArmer::updateAttack1Collision(void)
{
	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "ハンマー先A");
	this->GetFramePosFromName(&weapon_btm_pos, "ハンマー");

	CharaBase* p_player;
	p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	/* 戦闘不能者と既に攻撃された者を除く */
	if ((p_player->GetIsRetire() == false) &&
		(p_player->IsDamage() == false)) {

		ShapeCapsule capsuleA;
		p_player->GetBodyCapsule(&capsuleA);

		ShapeCapsule capsuleB;
		this->GetFramePosFromName(&capsuleB.mPosA, "ハンマー先B");
		this->GetFramePosFromName(&capsuleB.mPosB, "ハンマー先C");
		capsuleB.mRadius = 120.0f;

		ShapeCapsule capsuleC;
		capsuleC.mPosA	= weapon_top_pos;
		capsuleC.mPosB	= weapon_btm_pos;
		capsuleC.mRadius= 60.0f;

		ShapeLine line;
		line.mStart = weapon_top_pos;
		line.mEnd	= weapon_btm_pos;

		//		if ((CollisionManager::Check_LineToCapsule(&line, &capsuleA)) ||
		if ((CollisionManager::Check_CapsuleToCapsule(&capsuleA, &capsuleC)) ||
			(CollisionManager::Check_CapsuleToCapsule(&capsuleA, &capsuleB)) ) {

			SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			int attack_power = 4;
			VECTOR damage_down_vec;
			damage_down_vec = VSub(p_player->Position(), mPosition);
			damage_down_vec = VNorm(damage_down_vec);
			p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_BREAK_GUARD);
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
CharaEnemyMinoArmer::phaseEnterStateIdle(void)
{
	mIsJumpAttack = FALSE;
	CharaEnemy::phaseEnterStateIdle();

	if (mEfsHndSmoke != -1) {
		EffectManager::GetInstance()->Stop(mEfsHndSmoke, 0.5f);
		mEfsHndSmoke = -1;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinoArmer::phaseEnterStateJumpUp(void)
{
	// 重力を変更
	mConstGravitySpeed = (kGRAVITY_SPEED * 1.6f);

	mIsReserveComboInput = false;
	mIsAttack = false;
	mIsDamage = false;

	// ジャンプ力高めに設定
	//   	mGravity += (kJUMP_GRAVITY * 1.4f);
	mGravity += (kJUMP_GRAVITY * 1.8f);

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

	StartMotion(eMT_MINO_ARMER_ATTACK_JUMP, false, 0.5f);

	mIsLand = false;

	mIsJumpAttack = TRUE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinoArmer::phaseStateJumpUp(void)
{
	CharaEnemy::phaseStateJumpUp();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinoArmer::phaseEnterStateJumpDown(void)
{
	if (mIsJumpAttack == FALSE) {
		CharaEnemy::phaseEnterStateJumpDown();
	}
	else {
		mStateCount = 0;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinoArmer::phaseStateJumpDown(void)
{
	if (mIsJumpAttack == FALSE) {
		CharaEnemy::phaseStateJumpDown();
	}
	else {
		if (mStateCount == 20) {
			// 重力を変更
			//		mConstGravitySpeed = (kGRAVITY_SPEED * 4.8f);
			mConstGravitySpeed = (kGRAVITY_SPEED * 6.0f);
			// 垂直に落ちる
			mJumpMoveVec.mDash = ZERO_VEC;

			SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);

		}
		else if (mStateCount > 20) {
			CharaEnemy::phaseStateJumpDown();
		
			int attack_power = 4;
			updateAttackCollisionLocal(attack_power);
		}
		else {
			mConstGravitySpeed = 0;
			mGravity = 0.0f;
		}
		mStateCount ++;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinoArmer::phaseEnterStateJumpEnd(void)
{
	if (mIsJumpAttack == FALSE) {
		CharaEnemy::phaseEnterStateJumpEnd();
	}
	else {
		const float size = 80.0f;
		VECTOR effpos;
		effpos = VAdd(mPosition, mMoveVec);
		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,
															&effpos,
															&VGet(size, size, size),
															0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 0.5f);

		SoundManager::GetSndComponent()->SetVolume(eSE_BURST, 255);
		SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
		CameraManager::GetInstance()->StartShakeCamera(3.0f, 5.0f, 60.0f);

		EFK_HANDLE efk_handle = NULL;
		efk_handle =  EffekseerManager::GetInstance()->Play(eEFK_SHOCK, &mPosition, 30.0f);
		EffekseerManager::GetInstance()->SetSpeed(efk_handle, 1.2f);

		if (mEfsShock == -1) {
			mShockScaleSize = 300.0f;
			mEfsShock = EffectManager::GetInstance()->Play(eEF_AURA, &mPosition, mShockScaleSize);
		}

		mStateCount = 0;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinoArmer::phaseStateJumpEnd(void)
{
	if (mIsJumpAttack == FALSE) {
		CharaEnemy::phaseStateJumpEnd();
	}
	else {
		if (mStateCount > 60) {
			ChangeState(eCHARA_STATE_IDLE);
		}
		mStateCount ++;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinoArmer::phaseEnterStateAttack1(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "ハンマー");
	this->GetFramePosFromName(&weapon_btm_pos, "ハンマー先A");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_MINO_ARMER_ATTACK_1, false, 0.45f);

	mStatePhase = 0;
	mStateCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinoArmer::phaseStateAttack1(void)
{
	static const float sStart = 18.0f;
	static const float sEnd   = 26.0f;

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "ハンマー先A");
	this->GetFramePosFromName(&weapon_btm_pos, "ハンマー");

	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	if (this->EqualPhaseFrame(sStart)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}
	if (EqualPhaseFrame(22.0f)) {
		const float size = 50.0f;
		VECTOR effpos;
		effpos = weapon_top_pos;
		effpos.y = mPosition.y;
		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,&effpos,size, 0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 2.0f);
		SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
		CameraManager::GetInstance()->StartShakeCamera(1.0f, 2.0f, 30.0f);
	}

	if ((GetMotionNowTime() >= sStart) && (GetMotionNowTime() <= sEnd)) {
		mIsAttack = true;
		updateAttack1Collision();
	}
	else {
		mIsAttack = false;
	}

	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {
		mBladeEffect.Stop();
		if (GetRand(1) == 0) {
			ChangeState(eCHARA_STATE_IDLE);
		} 
		else {
			ChangeState(eCHARA_STATE_ATTACK_3);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinoArmer::phaseEnterStateAttack2(void)
{
	//	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	//	SoundManager::GetSndComponent()->PlaySound(eSE_PUT_SWORD);
	SoundManager::GetSndComponent()->PlaySound(eSE_MONSTER_VOICE);
	CameraManager::GetInstance()->StartShakeCamera(1.0f, 3.0f, 30.0f);

	StartMotion(eMT_MINO_ARMER_ATTACK_2_A, false, 1.3f);

	if (mEfsHndSmoke == -1) {
		mEfsHndSmoke = EffectManager::GetInstance()->Play(eEF_WHITE_TAIL, &mPosition, 10.0f, 0.5f);
	}

	mStatePhase = 0;
	mStateCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinoArmer::phaseStateAttack2(void)
{
	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "ハンマー先A");
	this->GetFramePosFromName(&weapon_btm_pos, "ハンマー");

	CharaBase* p_player;
	p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	switch (mStatePhase) {
	case 0:
		if (GetIsMotionBlend()) {
			SetRotationToTarget(&p_player->Position());
		}

		if (GetIsEndMotion()) {
			//			SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_2);

			SoundManager::GetSndComponent()->StopSound(eSE_MONSTER_VOICE);
			
			SoundManager::GetSndComponent()->PlaySound(eSE_ROLLING);
			SoundManager::GetSndComponent()->PlaySound(eSE_BURST);

			CameraManager::GetInstance()->StartShakeCamera(2.0f, 5.0f, 60.0f);

			EffectManager::GetInstance()->Scale(mEfsHndSmoke) = VGet(200.0f, 200.0f, 200.0f);
			EffectManager::GetInstance()->SetFloat(mEfsHndSmoke, "ColorRate",			0.36f);
			EffectManager::GetInstance()->SetFloat(mEfsHndSmoke, "FireRatio",			0.45f);
			EffectManager::GetInstance()->SetFloat(mEfsHndSmoke, "particleSpread",		30.0f);
			EffectManager::GetInstance()->SetFloat(mEfsHndSmoke, "particleSystemHeight",30.0f);

			StartMotion(eMT_MINO_ARMER_ATTACK_2_B, true, 0.9f);

			mStatePhase ++; mStateCount = 0;
		}
		break;

	case 1:
		{
			float move_speed = kCHARA_RUN_SPEED * 1.3f;
			mMoveVec.x += -(sin(mRotation.y) * move_speed);
			mMoveVec.z += -(cos(mRotation.y) * move_speed);

			
			/* 戦闘不能者と既に攻撃された者を除く */
			if ((p_player->GetIsRetire() == false) &&
				(p_player->IsDamage() == false)) {
				
				ShapeCapsule capsuleA, capsuleB;
				p_player->GetBodyCapsule(&capsuleA);
				this->GetBodyCapsule(&capsuleB);
				if (CollisionManager::Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
						
					SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
					int attack_power = 2;
					VECTOR damage_down_vec;
					damage_down_vec = VSub(p_player->Position(), mPosition);
					damage_down_vec = VNorm(damage_down_vec);
					p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_BREAK_GUARD);
				}
				else {
					mIsAttack = true;
					updateAttack1Collision();
				}
			}

			// 指定時間経過した場合
			if (mStateCount > 60) {
				ChangeState(eCHARA_STATE_IDLE);
			}
		}
		break;

	default:
		break;
	};

	if (mEfsHndSmoke != -1) {
		// 煙エフェクトの処理
		VECTOR posA = mPosition;
		posA.y += 50.0f;
		EffectManager::GetInstance()->Position(mEfsHndSmoke) = posA;
		EffectManager::GetInstance()->SetFloat(mEfsHndSmoke, "particleSpeed", 0.5f);
	}

	mStateCount ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinoArmer::phaseEnterStateAttack3(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "ハンマー");
	this->GetFramePosFromName(&weapon_btm_pos, "ハンマー先A");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_MINO_ARMER_ATTACK_3, false, 0.45f);

	mStatePhase = 0;
	mStateCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinoArmer::phaseStateAttack3(void)
{
	static const float sStart = 1.0f;
	static const float sEnd   = 16.0f;

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "ハンマー先A");
	this->GetFramePosFromName(&weapon_btm_pos, "ハンマー");

	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	if (this->EqualPhaseFrame(sStart)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}

	if ((GetMotionNowTime() >= sStart) && (GetMotionNowTime() <= sEnd)) {
		mIsAttack = true;
		updateAttack1Collision();
	}
	else {
		mIsAttack = false;
	}

	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {
		mBladeEffect.Stop();
		if (GetRand(1) == 0) {
			ChangeState(eCHARA_STATE_IDLE);
		} 
		else {
			ChangeState(eCHARA_STATE_ATTACK_1);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinoArmer::phaseEnterStateRetireEnd(void)
{
	if (mEfsShock != -1) {
		EffectManager::GetInstance()->Stop(mEfsShock< 0.5f);
		mEfsShock = -1;
	}

	if (mEfsHndSmoke != -1) {
		EffectManager::GetInstance()->Stop(mEfsHndSmoke, 0.5f);
		mEfsHndSmoke = -1;
	}

	CharaEnemy::phaseEnterStateRetireEnd();
}

 /**** end of file ****/
