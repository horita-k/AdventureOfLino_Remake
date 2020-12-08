/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ギギネブラファイル
 * @author SPATZ.
 * @data   2012/05/25 00:27:32
 */
//---------------------------------------------------
#include "CharaEnemyDragon.h"
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

static const tCapsuleWork sCapsuleWork[] = {
	/*
	{"頭",		"舌５",		120.0f,},
	{"尻尾１",	"尻尾先",	60.0f,},
	{"左腕",	"左手先",	60.0f,},
	{"右腕",	"右手先",	60.0f,},
	{"上半身",	"首Ａ",		200.0f,},
	*/
	{"下あご",	"下あご先",	30.0f,},
};
static const int kCAPSULE_NUM_DRAGON = sizeof(sCapsuleWork) / sizeof(tCapsuleWork);

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDragon::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	mFlinchMax = 250;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CharaEnemyDragon::CheckDamageCollNormal(tCheckDamageWork* pParam)
{
	return ModelCollision::CheckDamageCollNormal(pParam);

	/*
	ShapeCapsule capsule;
	ShapeLine	line;

	line.mStart = pParam->mWeaponBtmPos;
	line.mEnd	= pParam->mWeaponTopPos;

	for (int iI = 0; iI < kCAPSULE_NUM_DRAGON; iI ++) {
		this->GetFramePosFromName(&capsule.mPosA, sCapsuleWork[iI].mPosA);
		this->GetFramePosFromName(&capsule.mPosB, sCapsuleWork[iI].mPosB);
		capsule.mRadius = sCapsuleWork[iI].mRadius;
#ifdef _DEBUG
		GraphicManager::GetInstance()->DbgRenderCapsule(&capsule);
#endif // _DEBUG
		if (CollisionManager::GetInstance()->Check_LineToCapsule(&capsule, &line)) {
			return true;
		}
	}

	return false;
	*/
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyDragon::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	float dist = VSize(vec);
	
	int rand;
	bool result = false;

	if (dist < mBodySize * 1.5f) {
		float rate = mBodySize / dist;
		//		rand = GetRand((int)((1.0f - rate) * 30.0f) + 1);
		rand = GetRand(6);

		switch (rand) {
		case 0:	case 1:
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
			result = true;
			break;

		case 2:	case 3:
			*pAttackCharaState = eCHARA_STATE_ATTACK_2;
			result = true;
			break;
		case 4:	case 5:
			*pAttackCharaState = eCHARA_STATE_ATTACK_3;
			result = true;
			break;

		};
		/*
		if (rand > 2) {
			//			*pAttackCharaState = eCHARA_STATE_ATTACK_3;
			*pAttackCharaState = eCHARA_STATE_ATTACK_2;
			result = true;
		} else if (rand == 2) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_2;
			result = true;
		} else if (rand == 3) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
			result = true;
		}
		*/

	} else if (dist < mBodySize * 4.0f) {
		rand = GetRand(40);
		if (rand == 0) {
			//			*pAttackCharaState = eCHARA_STATE_ATTACK_4;
			*pAttackCharaState = eCHARA_STATE_ATTACK_5;
			result = true;
		}
	}
	return result;
}

/*=====================================*
 * extend
 *=====================================*/

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDragon::Update(void)
{
	CharaEnemy::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDragon::phaseEnterStateJumpUp(void)
{
	StartMotionEx(&sMotionParam[eMT_JUMP_START]);

	mIsReserveComboInput = false;
	mIsAttack = false;
	mIsDamage = false;

	// ジャンプ力高めに設定
	mGravity += (kJUMP_GRAVITY * 1.0f);

	mIsLand = false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDragon::phaseEnterStateJumpEnd(void)
{
	const float size = 40.0f;
	VECTOR effpos;
	effpos = VAdd(mPosition, mMoveVec);

	int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,
														&effpos,
														&VGet(size, size, size),
														0.0f);
	EffectManager::GetInstance()->Stop(eff_handle, 2.0f);
	SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
	
	//	StartMotionEx(&sMotionParam[eMT_JUMP_END]);
	StartMotion(eMT_JUMP_END, false, 0.75f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDragon::phaseEnterStateAttack1(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
	//	StartMotion(eMT_DRAGON_ATTACK_1, false, 0.625f);
	StartMotion(eMT_DRAGON_ATTACK_1, false, 0.525f);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "下あご");
	this->GetFramePosFromName(&weapon_btm_pos, "下あご先");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDragon::phaseStateAttack1(void)
{
	static float sStart = 20.0f;
	static float sEnd = 30.0f;

	if (this->EqualPhaseFrame(20.0f)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}

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
		
			this->GetFramePosFromName(&capsuleB.mPosA, "下あご");
			this->GetFramePosFromName(&capsuleB.mPosB, "下あご先");
			capsuleB.mRadius = 30.0f;
			capsuleB.mRadius *= (mScale.x / DRAGON_SCALE);
#ifdef _DEBUG			
			GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 1;
				attack_power *= (int)(mScale.x / DRAGON_SCALE);
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), capsuleB.mPosA);
				damage_down_vec.y = 0.0f;
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
			}
		}
	} else {
		mIsAttack = false;
	}

	PRINT_SCREEN(GetColor(255, 255, 0), "NowTime : %.2f\n", GetMotionNowTime());

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "下あご");
	this->GetFramePosFromName(&weapon_btm_pos, "下あご先");
	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDragon::phaseEnterStateAttack2(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
	//	StartMotion(eMT_DRAGON_ATTACK_2, false, 0.625f);
	StartMotion(eMT_DRAGON_ATTACK_2, false, 0.525f);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "下あご");
	this->GetFramePosFromName(&weapon_btm_pos, "下あご先");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDragon::phaseStateAttack2(void)
{
	static float sStart = 20.0f;
	static float sEnd = 30.0f;

	if (this->EqualPhaseFrame(20.0f)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}

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
		
			this->GetFramePosFromName(&capsuleB.mPosA, "下あご");
			this->GetFramePosFromName(&capsuleB.mPosB, "下あご先");
			capsuleB.mRadius = 30.0f;
			capsuleB.mRadius *= (mScale.x / DRAGON_SCALE);
#ifdef _DEBUG			
			GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 1;
				attack_power *= (int)(mScale.x / DRAGON_SCALE);
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), capsuleB.mPosA);
				damage_down_vec.y = 0.0f;
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
			}
		}
	} else {
		mIsAttack = false;
	}

	PRINT_SCREEN(GetColor(255, 255, 0), "NowTime : %.2f\n", GetMotionNowTime());

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "下あご");
	this->GetFramePosFromName(&weapon_btm_pos, "下あご先");
	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDragon::phaseEnterStateAttack3(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_3);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "尾1");
	this->GetFramePosFromName(&weapon_btm_pos, "尾先");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_ATTACK_3, false, 0.23f);
}

//---------------------------------------------------
/**
 * ローリング攻撃
 */
//---------------------------------------------------
void
CharaEnemyDragon::phaseStateAttack3(void)
{
	static float sStart = 0.0f;
	static float sEnd = 25.0f;

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
		
			this->GetFramePosFromName(&capsuleB.mPosA, "尾1");
			this->GetFramePosFromName(&capsuleB.mPosB, "尾先");
			capsuleB.mRadius = 60.0f;
			capsuleB.mRadius *= (mScale.x / DRAGON_SCALE);
			
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 2;
				attack_power *= (int)(mScale.x / DRAGON_SCALE);
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), capsuleB.mPosB);
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
			}
		}

	} else {
		mIsAttack = false;
	}

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "尾1");
	this->GetFramePosFromName(&weapon_btm_pos, "尾先");
	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

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
CharaEnemyDragon::phaseEnterStateAttack4(void)
{
	CharaBase* p_player;
	p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	/*
	mJumpMoveVec.mDash = VSub(p_player->Position(), this->mPosition);
	mJumpMoveVec.mDash.y = 0.0f;
	mJumpMoveVec.mDash = VScale(VNorm(mJumpMoveVec.mDash), (kCHARA_RUN_SPEED * 1.5f)); 
	*/
	mJumpMoveVec.mDash.x = sin(mRotation.y + Utility::DegToRad(180.0f)) * (kCHARA_RUN_SPEED * 1.5f);
	mJumpMoveVec.mDash.z = cos(mRotation.y + Utility::DegToRad(180.0f)) * (kCHARA_RUN_SPEED * 1.5f);

	mJumpMoveVec.mDash.x *= (mScale.x / DRAGON_SCALE);
	mJumpMoveVec.mDash.z *= (mScale.x / DRAGON_SCALE);

	//	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_ROLLING);

	//	StartMotion(eMT_DRAGON_ATTACK_4, false, 0.25f); 
	StartMotion(eMT_DRAGON_ATTACK_4, false, 0.5f); 
}

//---------------------------------------------------
/**
 * 突進
 */
//---------------------------------------------------
void
CharaEnemyDragon::phaseStateAttack4(void)
{
	mMoveVec = mJumpMoveVec.mDash;

	mIsAttack = true;

	/****/
	CharaBase* p_player;
	p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	/* 戦闘不能者と既に攻撃された者を除く */
	if ((p_player->GetIsRetire() == false) &&
		(p_player->IsDamage() == false)) {

		/*
		CollisionManager::tSphereWork sphereA;
		CollisionManager::tSphereWork sphereB;
		VECTOR vec, outpos;
		float dist;
		sphereA.mPosition = p_player->Position();
		sphereA.mSize = p_player->BodySize();
		sphereB.mPosition = this->mPosition;
		sphereB.mSize = mBodySize;
		*/

		ShapeCapsule capsuleA;
		ShapeCapsule capsuleB;
		capsuleA.mPosA	= p_player->Position();
		capsuleA.mPosB	= p_player->Position();
		capsuleA.mPosB.y += p_player->BodyHeight();
		capsuleA.mRadius= p_player->BodySize();
		
		this->GetFramePosFromName(&capsuleB.mPosA, "下あご");
		this->GetFramePosFromName(&capsuleB.mPosB, "下あご先");
		capsuleB.mRadius = 70.0f;
		capsuleB.mRadius *= (mScale.x / DRAGON_SCALE);
		
		//if (CollisionManager::Check_SphereToSphere(&sphereA, &sphereB, &outpos, &dist)) {
		if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
			
			SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			int attack_power = 1;
			attack_power *= (int)(mScale.x / DRAGON_SCALE);
			VECTOR damage_down_vec;
			damage_down_vec = VSub(p_player->Position(), mPosition);
			damage_down_vec = VCross(VGet(0.0f, 1.0f, 0.0f), damage_down_vec);
			damage_down_vec = VNorm(damage_down_vec);
			p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
		}
	}

	PRINT_SCREEN(GetColor(255, 255, 0), "NowTime : %.2f\n", GetMotionNowTime());

	if (GetIsEndMotion() || mIsLand == false) {
		
		mMoveVec = ZERO_VEC;
		mIsAttack = false;
		mJumpMoveVec.mDash = ZERO_VEC;
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDragon::phaseEnterStateAttack5(void)
{
	CharaBase* p_player;
	p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	//	SoundManager::GetSndComponent()->PlaySound(eSE_ROLLING);

	VECTOR vec = VSub(p_player->Position(), mPosition);
	mTargetRotY = atan2(vec.x, vec.z) + PHI_F;

	//	StartMotion(eMT_DRAGON_ATTACK_4, false, 0.25f); 
	StartMotion(eMT_DRAGON_ATTACK_4, false, 0.5f, NULL, 0.035f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDragon::phaseStateAttack5(void)
{
	/* キャラクターの向きを処理する */
	Utility::CorrectAngleFrom000To360(&mRotation.y);

	float diff_rot = mTargetRotY - mRotation.y;
	eTurnType turn_type = eTURN_TYPE_NONE;
	
	// キャラの回転するスピード　でかい奴ほど遅い
	float turn_move_speed = 60.0f * kTURN_MOVE_ROT / mBodySize;

	if (turn_move_speed >= abs(diff_rot)) {
		mRotation.y = mTargetRotY;

		//		turn_type = updateTargetRotateFromPad();
		//		mTurnType = turn_type;

		//		ChangeState(eCHARA_STATE_MOVE);

	} else {
		bool is_plus = (diff_rot >= 0.0f) ? true : false;
		if (abs(diff_rot) > Utility::DegToRad(180.0f)) {
			is_plus ^= true;
		}
		mRotation.y += (is_plus) ? turn_move_speed : -turn_move_speed;
	}

	//<! ブレンドが終わるまで待つ
	if (this->GetIsMotionBlend() == false) {
		ChangeState(eCHARA_STATE_ATTACK_4);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*
void
CharaEnemyDragon::phaseStateWaitAttack(void)
{
	int rand = GetRand(40);

	if (rand > 4) {
		ChangeState(eCHARA_STATE_ATTACK_4);
		return;
	}

	if (rand == 0) {
		ChangeState(eCHARA_STATE_ATTACK_1);
	} else if (rand == 1) {
		ChangeState(eCHARA_STATE_ATTACK_2);
	} else if (rand == 2) {
		ChangeState(eCHARA_STATE_ATTACK_3);
	} else if (rand == 3) {
		ChangeState(eCHARA_STATE_ATTACK_4);
	}
}
*/

#ifdef _DEBUG
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDragon::debugFunc(void)
{
	ShapeCapsule capsuleB;
	for (int iI = 0; iI < kCAPSULE_NUM_DRAGON; iI ++) {
		this->GetFramePosFromName(&capsuleB.mPosA, sCapsuleWork[iI].mPosA);
		this->GetFramePosFromName(&capsuleB.mPosB, sCapsuleWork[iI].mPosB);
		capsuleB.mRadius = sCapsuleWork[iI].mRadius;
		capsuleB.mRadius *= (mScale.x / DRAGON_SCALE);
		GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
	}
}

#endif // _DEBUG

 /**** end of file ****/
