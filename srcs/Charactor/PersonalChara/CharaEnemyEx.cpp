/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 敵派生クラス
 * @author SPATZ.
 * @data   2014/01/15 01:59:46
 */
//---------------------------------------------------
#include "CharaEnemyEx.h"
#include "Charactor/CharaInfo.h"
#include "Charactor/CharaPlayer.h"
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

/*=====================================*
 * define
 *=====================================*/

#define RESET_SUB_STATE()						\
	{											\
		mStatePhase = 0;						\
		mStateCount = 0;						\
	}

#define NEXT_SUB_STATE()						\
	{											\
		mStatePhase ++;							\
		mStateCount = 0;						\
	}

/*=====================================*
 * スライムクラス
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemySlime::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);
	mFlinchMax = 150;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemySlime::phaseEnterStateAttack1(void)
{
	int frame_index = 0;

	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
	StartMotion(eMT_ATTACK_1, false, 0.625f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemySlime::phaseStateAttack1(void)
{
	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	static const float sStart	= 30.0f;
	static const float sEnd		= 56.0f;

	if (this->EqualPhaseFrame(sStart)) {
		SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
		SoundManager::GetSndComponent()->PlaySound(eSE_BOYOYON);
	}

	// この関数内で mIsAttack が変更される可能性があるので注意
	CharaEnemy::ProcessAttackToPlayer(sStart, sEnd, "頭", 35.0f, 2, true);

	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemySlime::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	if (VSize(vec) < mBodySize + (mBodySize * (GetRand(200) * 0.010f))) {

		*pAttackCharaState = eCHARA_STATE_ATTACK_1;
		return true;
	}
	return false;
}


/*=====================================*
 * ウッドマンクラス
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWoodman::phaseEnterStateAttack1(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	StartMotion(eMT_ATTACK_1, false, 0.6f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWoodman::phaseStateAttack1(void)
{
	float move_speed = 0.32f * (100 - GetMotionTimePercent());
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	
	static const float sStart	= 4.0f;
	static const float sEnd		= 60.0f;

	// この関数内で mIsAttack が変更される可能性があるので注意
	CharaEnemy::ProcessAttackToPlayer(sStart, sEnd, "頭先", 60.0f, 2, true);

	if (this->EqualPhaseFrame(13.0f)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_BOOMERANG_RUN);
	} else if (this->EqualPhaseFrame(35.0f)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_FALL_STONE);
		CameraManager::GetInstance()->StartShakeCamera(0.4f, 2.0f, 60.0f);
	} else if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWoodman::phaseStateWaitAttack(void)
{
	ChangeState(mReserveCharaState);
	mReserveCharaState = -1;
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyWoodman::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	if (VSize(vec) < (mBodySize * 8.0f) + (mBodySize * (GetRand(200) * 0.05f))) {

		*pAttackCharaState = eCHARA_STATE_ATTACK_1;
		return true;
	}
	return false;
}

/*=====================================*
 * ミノタウロス
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinotaurus::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	mFlinchMax = 250;

	if (mWeaponModel.GetModelHandle() == -1) {
		mWeaponModel.LinkModel(RES_MDL_AX);
		mWeaponModel.Setup();
	}
	this->AttachModel(&mWeaponModel,
					  ModelPmd::eATTACHED_MODEL_NORMAL,
					  this->GetFrameIndexFromName("右手首"));
	GraphicManager::GetInstance()->EntryDrawModel(&mWeaponModel);
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyMinotaurus::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	float dist = VSize(vec);
	
	int rand;
	bool result = false;

	if (dist < mBodySize * 1.5f) {
		rand = GetRand(6);

		if (rand == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
			result = true;
		} else if (rand == 1) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
			result = true;
		} else if (rand == 2) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
			result = true;
		}
	}
	return result;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinotaurus::phaseEnterStateAttack1(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "右手首");
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣先");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_KNIGHT_ATTACK_1, false, 0.45f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMinotaurus::phaseStateAttack1(void)
{
	static float sStart = 15.0f;
	static float sEnd = 21.0f;

	VECTOR weapon_top_pos, weapon_btm_pos;
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣尾");
	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");

	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	if (this->EqualPhaseFrame(sStart)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}
	if (EqualPhaseFrame(23.0f)) {
		const float size = 30.0f;
		VECTOR effpos;
		effpos = weapon_top_pos;
		effpos.y = mPosition.y;
		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,
															&effpos,
															&VGet(size, size, size),
															0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 2.0f);
		SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
	}

	if ((GetMotionNowTime() >= sStart) &&
		(GetMotionNowTime() <= sEnd)) {
		mIsAttack = true;

		CharaBase* p_player;
		p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		/* 戦闘不能者と既に攻撃された者を除く */
		if ((p_player->GetIsRetire() == false) &&
			(p_player->IsDamage() == false)) {

			ShapeCapsule capsule;
			ShapeLine line;
			capsule.mRadius = p_player->BodySize() * 0.8f;
			capsule.mPosA = p_player->Position();
			capsule.mPosA.y += capsule.mRadius;
			capsule.mPosB = p_player->Position();
			capsule.mPosB.y += p_player->BodyHeight() - capsule.mRadius;

			mWeaponModel.GetFramePosFromName(&line.mStart, "剣先");
			mWeaponModel.GetFramePosFromName(&line.mEnd, "剣尾");

			if (CollisionManager::Check_LineToCapsule(&line, &capsule)) {

				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			
				int attack_power = 4;
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
CharaEnemyMinotaurus::phaseEnterStateRetireEnd(void)
{
	GraphicManager::GetInstance()->ReserveRemoveModel(&mWeaponModel);

	mWeaponModel.SetAlpha(0.0f);
	mWeaponModel.DeleteModel();

	CharaEnemy::phaseEnterStateRetireEnd();
}

/*=====================================*
 * スコーピオン
 *=====================================*/
/*static*/const char* CharaEnemyScorpion::strAttackTopNode = "尾節";
/*static*/const char* CharaEnemyScorpion::strAttackBtmNode = "後腹部";
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyScorpion::phaseEnterStateAttack1(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	StartMotion(eMT_ATTACK_1, false, 0.9f, NULL);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttackTopNode);
	this->GetFramePosFromName(&weapon_btm_pos, strAttackBtmNode);

	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyScorpion::phaseStateAttack1(void)
{
	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	static const float sStart	= 28.0f;
	static const float sEnd		= 43.0f;

	// この関数内で mIsAttack が変更される可能性があるので注意
	CharaEnemy::ProcessAttackToPlayer(sStart, sEnd, (char *)strAttackTopNode, 20.0f, 2, true);

	PRINT_SCREEN(GetColor(255, 255, 0), "NowTime : %.2f\n", GetMotionNowTime());
	
	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttackTopNode);
	this->GetFramePosFromName(&weapon_btm_pos, strAttackBtmNode);
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
CharaEnemyScorpion::phaseEnterStateRetireEnd(void)
{
	CharaEnemy::phaseEnterStateRetireEnd();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyScorpion::processNoMoveAction(void)
{
	return;		/* nop */
}

/*=====================================*
 * ロボット・素手タイプ
 *=====================================*/
/*static*/const char* CharaEnemyRobotA::strAttackTopNode = "右ひじ先";
/*static*/const char* CharaEnemyRobotA::strAttackBtmNode = "右ひじ";
/*static*/const char* CharaEnemyRobotA::strAttackTopNodeB = "左ひじ先";
/*static*/const char* CharaEnemyRobotA::strAttackBtmNodeB = "左ひじ";
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRobotA::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyRobotA::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	if (VSize(vec) < (mBodySize * 5.0f) + (mBodySize * (GetRand(100) * 0.010f))) {
		if (GetRand(2) == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_2;
		} else {
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
		}
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRobotA::phaseEnterStateIdle(void)
{
	CharaEnemy::phaseEnterStateIdle();
	mBladeEffectB.Stop();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRobotA::phaseEnterStateAttack1(void)
{
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	StartMotion(eMT_ATTACK_1, false, 0.4f, NULL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRobotA::phaseStateAttack1(void)
{
	float move_speed = 0.0f;
	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	static const float sStart	= 20.0f;
	static const float sEnd		= 25.0f;

	// この関数内で mIsAttack が変更される可能性があるので注意

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttackTopNode);
	this->GetFramePosFromName(&weapon_btm_pos, strAttackBtmNode);

	// 移動量の処理① (攻撃準備中)
	move_speed = 0.04f * (100 - GetMotionTimePercent() );
	mMoveVec.x += (sin(mRotation.y + Utility::DegToRad(180.0f) ) * move_speed);
	mMoveVec.z += (cos(mRotation.y + Utility::DegToRad(180.0f) ) * move_speed);

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (this->EqualPhaseFrame(2.0f) ) {
		// テールエフェクト開始
		mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	} else if (this->BetweenPhaseFrame(sStart, sEnd) ) {

		// 移動量の処理② (攻撃中)
		move_speed = 0.42f * (100 - GetMotionTimePercent() );
		mMoveVec.x += (sin(mRotation.y + Utility::DegToRad(180.0f) ) * move_speed);
		mMoveVec.z += (cos(mRotation.y + Utility::DegToRad(180.0f) ) * move_speed);

		// テールエフェクト更新
		mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

		// 攻撃のあたり判定
		if ( !(p_player->GetIsRetire()) && !(p_player->IsDamage()) ) {
			ShapeCapsule capsuleA, capsuleB;
			p_player->GetBodyCapsule(&capsuleA);
			capsuleB.mPosA = weapon_top_pos;
			capsuleB.mPosB = weapon_btm_pos;
			capsuleB.mRadius = 36.0f;
			if (CollisionManager::Check_CapsuleToCapsule(&capsuleA, &capsuleB) ) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), mPosition);
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(3, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
			}
		}
	}

	PRINT_SCREEN(GetColor(255, 255, 0), "NowTime : %.2f\n", GetMotionNowTime());

	// 終了判定処理
	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
 * 回転アタック
 */
//---------------------------------------------------
void
CharaEnemyRobotA::phaseEnterStateAttack2(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_2);

	StartMotion(eMT_ATTACK_2, false, 0.5f, NULL);
}

//---------------------------------------------------
/**
 * 回転アタック
 */
//---------------------------------------------------
void
CharaEnemyRobotA::phaseStateAttack2(void)
{
	float move_speed = 0.0f;
	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	static const float sStart	= 36.0f;
	static const float sEnd		= 66.0f;

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttackTopNode);
	this->GetFramePosFromName(&weapon_btm_pos, strAttackBtmNode);
	VECTOR weapon_top_posB, weapon_btm_posB;
	this->GetFramePosFromName(&weapon_top_posB, strAttackTopNodeB);
	this->GetFramePosFromName(&weapon_btm_posB, strAttackBtmNodeB);

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();


	if ( (this->EqualPhaseFrame(34.0f)) ||
		 (this->EqualPhaseFrame(42.0f)) ||
		 (this->EqualPhaseFrame(50.0f)) ||
		 (this->EqualPhaseFrame(58.0f)) ) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}

	if (this->EqualPhaseFrame(sStart) ) {
		// テールエフェクト開始
		mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);
		mBladeEffectB.Play(&weapon_top_posB, &weapon_btm_posB);

	} else if (this->BetweenPhaseFrame(sStart, sEnd) ) {
		// テールエフェクト更新
		mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);
		// テールエフェクト更新
		mBladeEffectB.Update(&weapon_top_posB, &weapon_btm_posB);

		// 攻撃のあたり判定
		if ( !(p_player->GetIsRetire()) && !(p_player->IsDamage()) ) {
			bool is_hitA = false;
			bool is_hitB = false;
			ShapeCapsule capsuleA, capsuleB;
			p_player->GetBodyCapsule(&capsuleA);
			capsuleB.mPosA = weapon_top_pos;
			capsuleB.mPosB = weapon_btm_pos;
			capsuleB.mRadius = 20.0f;
			is_hitA = (CollisionManager::Check_CapsuleToCapsule(&capsuleA, &capsuleB) );

			capsuleB.mPosA = weapon_top_posB;
			capsuleB.mPosB = weapon_btm_posB;
			capsuleB.mRadius = 20.0f;
			is_hitB = (CollisionManager::Check_CapsuleToCapsule(&capsuleA, &capsuleB) );

			if ( (is_hitA == true) || (is_hitB == true) ) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), mPosition);
				damage_down_vec = VNorm(damage_down_vec);

				bool is_damage_down = false;
				if (this->BetweenPhaseFrame(50.0f, 66.0f)) {
					is_damage_down = true;
				}
				p_player->ProcessDamage(2, false, NULL, &damage_down_vec, is_damage_down, eATTACK_TYPE_ENEMY_NORMAL);
			}
		}
	}

	PRINT_SCREEN(GetColor(255, 255, 0), "NowTime : %.2f\n", GetMotionNowTime());

	// 終了判定処理
	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRobotA::phaseStateWaitAttack(void)
{
	ChangeState(mReserveCharaState);
	mReserveCharaState = -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRobotA::phaseEnterStateRetireEnd(void)
{
	CharaEnemy::phaseEnterStateRetireEnd();
	mBladeEffectB.Stop();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyRobotB::CharaEnemyRobotB()
{
	mStatePhase = 0;
	mStateCount = 0;
}

//---------------------------------------------------
/**
 * 銃攻撃
 */
//---------------------------------------------------
void
CharaEnemyRobotB::phaseEnterStateAttack2(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_2);

	mStatePhase = 0;
	mStateCount = 0;

	int rnd_value = GetRand(2);
	StartMotion( (eMT_ATTACK_2) + rnd_value, false, 0.5f, NULL);
}

//---------------------------------------------------
/**
 * 銃攻撃
 */
//---------------------------------------------------
void
CharaEnemyRobotB::phaseStateAttack2(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	VECTOR weapon_top_posA, weapon_btm_posA;
	VECTOR weapon_top_posB, weapon_btm_posB;
	this->GetFramePosFromName(&weapon_top_posA, strAttackTopNode);
	this->GetFramePosFromName(&weapon_top_posB, strAttackTopNodeB);
	this->GetFramePosFromName(&weapon_btm_posA, strAttackBtmNode);
	this->GetFramePosFromName(&weapon_btm_posB, strAttackBtmNodeB);

	VECTOR* p_top_pos_tbl[2] = {&weapon_top_posA, &weapon_top_posB};
	VECTOR* p_btm_pos_tbl[2] = {&weapon_btm_posA, &weapon_btm_posB};
	const char* str_top_pos_node[2]= {strAttackTopNode, strAttackTopNodeB};

	switch (mStatePhase) {
	case 0:
		if (GetIsEndMotion() == false) break;
		mStatePhase ++; mStateCount = 0;
		break;

	case 1:
		{
			EFK_HANDLE efk_hdl = NULL;
			VECTOR eff_pos;
			float target_rot;
			VECTOR vecZ, crossX, crossY;

			for (int iI = 0; iI < 2; iI ++) {
				vecZ = VNorm(VSub(*p_top_pos_tbl[iI], *p_btm_pos_tbl[iI]) );
				crossX = VNorm(VCross(vecZ, VGet(0.0f, 1.0f, 0.0f)));
				crossY = VNorm(VCross(vecZ, crossX));

				target_rot = atan2(crossY.x, crossY.z);
				Utility::CorrectAngleFrom000To360(&target_rot);

				this->GetFramePosFromName(&eff_pos, str_top_pos_node[iI]);
				efk_hdl = EffekseerManager::GetInstance()->Play(eEFK_LASER, &eff_pos, 100.0f, 0.0f);

				EffekseerManager::GetInstance()->SetRotationAxis(efk_hdl, &crossY, -target_rot );
			}

			SoundManager::GetSndComponent()->PlaySound(eSE_LASER);

			mStatePhase ++; mStateCount = 0;
		}
		break;
	case 2:

		/* 当たり判定処理 */
		if (p_player->IsDamage() == false) {

			ShapeCapsule capsuleA, capsuleB;
			float laser_len  = (1800.0f * ( (float)mStateCount / 60.0f) );
			float laser_size = 24.0f;

			p_player->GetBodyCapsule(&capsuleA);

			for (int iI = 0; iI < 2; iI ++) {
				capsuleB.mPosA = *p_top_pos_tbl[iI];
				capsuleB.mPosB = VNorm(VSub(*p_top_pos_tbl[iI], *p_btm_pos_tbl[iI]) );
				capsuleB.mPosB = VAdd( VScale(capsuleB.mPosB, laser_len), *p_top_pos_tbl[iI]);
				capsuleB.mRadius = laser_size;
#ifdef _DEBUG
				GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG
				if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
					SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
					int attack_power = 2;
					VECTOR damage_down_vec;
					damage_down_vec = VSub(p_player->Position(), capsuleB.mPosA);
					damage_down_vec.y = 0.0f;
					damage_down_vec = VNorm(damage_down_vec);
					p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
					break;
				}
			}
		}
		if (mStateCount > 60) {
			ChangeState(eCHARA_STATE_IDLE);
		}
		break;

	default:
		break;
	};

	mStateCount ++;

}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyRobotB::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	if (VSize(vec) < (mBodySize * 15.0f) + (mBodySize * (GetRand(100) * 0.010f))) {
		if (GetRand(2) == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_2;
		}
		else {
			*pAttackCharaState = eCHARA_STATE_ATTACK_2;
		}
		return true;
	}
	return false;
}

/*=====================================*
 * リペラコプター
 *=====================================*/
/*static*/const char* CharaEnemyHelicopter::strAttackTopNode = "攻撃先";
/*static*/const char* CharaEnemyHelicopter::strAttackBtmNode = "攻撃元";
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyHelicopter::CharaEnemyHelicopter()
{
	mStatePhase	= 0;
	mStateCount = 0;
	mTmpVec.mEscapePos	= ZERO_VEC;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHelicopter::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);
	mConstGravitySpeed = 0.0f;	// 無重量に設定
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHelicopter::phaseStateIdle(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	if (p_player->GetIsRetire() == false) {
		if (GetRand(10) == 0) {
			VECTOR vec;
			vec = VSub(p_player->Position(), mPosition);

			// 間合いの座標を探す
			float escape_dist	= 500.0f;
			float escape_height	= (100.0f + (50.0f * GetRand(6) ) );

			// プレイヤーに対して回り込む角度
			float escape_rot = p_player->Rotation().y - PHI_F;
			escape_rot += Utility::DegToRad( (15.0f * GetRand(12) ) );
			Utility::CorrectAngleFrom000To360(&escape_rot);

			mTmpVec.mEscapePos = p_player->Position();
			mTmpVec.mEscapePos.x += sin(escape_rot) * escape_dist;
			mTmpVec.mEscapePos.y += escape_height;
			mTmpVec.mEscapePos.z += cos(escape_rot) * escape_dist;

			vec = VSub(mTmpVec.mEscapePos, mPosition);
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
CharaEnemyHelicopter::phaseStateMove(void)
{
	VECTOR dir_vec;
	dir_vec = VSub(mTmpVec.mEscapePos, mPosition);
	if (VSize(dir_vec) > mMoveSpeed) {
		mMoveVec = VScale(VNorm(dir_vec), mMoveSpeed);
	} else {
		mPosition = mTmpVec.mEscapePos;
		ChangeState(eCHARA_STATE_ATTACK_1);
	}

	for (int iI = (8 - 1); iI > 0; iI --) {
		mMoveHistory[iI] = mMoveHistory[iI - 1];
	}
	mMoveHistory[0] = mPosition;
	// 全然移動していない場合の処理
	if (VSize(VSub(mMoveHistory[0], mMoveHistory[7])) < mMoveSpeed * 0.1f) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHelicopter::phaseEnterStateJumpUp(void)
{
	ChangeState(eCHARA_STATE_IDLE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHelicopter::phaseStateJumpUp(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHelicopter::phaseEnterStateJumpDown(void)
{
	ChangeState(eCHARA_STATE_IDLE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHelicopter::phaseStateJumpDown(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHelicopter::phaseStateAppear(void)
{
	if (((mAppearWaitMax >= mAppearWaitCount) && 
		 (mAppearWaitMax < mAppearWaitCount + (SHORT)kBASIC_FRAME_COUNTER))) {
		
		mPosition.y += 400.0f;

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
		ChangeState(eCHARA_STATE_IDLE);
	}
	mAppearWaitCount += (SHORT)kBASIC_FRAME_COUNTER;
}

//---------------------------------------------------
/**
 *
 */
//---------------------------------------------------
void
CharaEnemyHelicopter::phaseEnterStateAttack1(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec;
	
	vec = VSub(p_player->Position(), mPosition);
	mTargetRotY = atan2(vec.x, vec.z) + PHI_F;
}

//---------------------------------------------------
/**
 *
 */
//---------------------------------------------------
void
CharaEnemyHelicopter::phaseStateAttack1(void)
{
	float turn_move_speed = 60.0f * kTURN_MOVE_ROT / mBodySize;
	processTurnTargetRotY(turn_move_speed, eCHARA_STATE_ATTACK_2);
}

//---------------------------------------------------
/**
 * 銃攻撃
 */
//---------------------------------------------------
void
CharaEnemyHelicopter::phaseEnterStateAttack2(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_2);
	mStatePhase = 0;
	mStateCount = 0;

	// 銃口を向けるベクトルを求める
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	mTmpVec.mAimPos = p_player->Position();
	mTmpVec.mAimPos.y += p_player->BodyHeight() * 0.5f;
}

//---------------------------------------------------
/**
 * 銃攻撃
 */
//---------------------------------------------------
void
CharaEnemyHelicopter::phaseStateAttack2(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	VECTOR aim_pos, weapon_top_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttackTopNode);
	aim_pos = mTmpVec.mAimPos;

	EFK_HANDLE efk_hdl = NULL;

	switch (mStatePhase) {
	case 0:
		if (mStateCount > 20) {
			mStatePhase ++; mStateCount = 0;
		}
		break;

	case 1:
		{
			MATRIX mtx, tmp;
			mtx = MGetIdent();
			// 拡縮
			tmp = MGetScale(VGet(50.0f, 50.0f, 50.0f) );
			mtx = MMult(mtx, tmp);
			// 回転
			Utility::MTXLookAtMatrix(&tmp, &aim_pos, &weapon_top_pos, (VECTOR *)&UP_VEC);
			mtx = MMult(mtx, tmp);
			// 移動
			tmp = MGetTranslate(weapon_top_pos);
			mtx = MMult(mtx, tmp);
			
			efk_hdl = EffekseerManager::GetInstance()->PlayMtx(eEFK_LASER, &mtx);

			EffekseerManager::GetInstance()->SetSpeed(efk_hdl, 0.75f);
			SoundManager::GetSndComponent()->PlaySound(eSE_LASER);

			mStatePhase ++; mStateCount = 0;
		}
		break;
	case 2:

		/* 当たり判定処理 */
		if (p_player->IsDamage() == false) {

			ShapeCapsule capsuleA, capsuleB;
			float laser_len  = (200.0f * ( (float)mStateCount / 10.0f) );
			float laser_size = 12.0f;

			p_player->GetBodyCapsule(&capsuleA);
			capsuleB.mPosA = weapon_top_pos;
			capsuleB.mPosB = VNorm(VSub(aim_pos, weapon_top_pos) );
			capsuleB.mPosB = VAdd( VScale(capsuleB.mPosB, laser_len), weapon_top_pos);
			capsuleB.mRadius = laser_size;
#ifdef _DEBUG
			GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 2;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), capsuleB.mPosA);
				damage_down_vec.y = 0.0f;
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
			}
		}
		if (mStateCount > 60) {
			mStatePhase ++; mStateCount = 0;
		}
		break;
		
	case 3:
		if (mStateCount > 60) {
			ChangeState(eCHARA_STATE_IDLE);
		}
		break;

	default:
		break;
	};

	mStateCount ++;

}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyHelicopter::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	if (VSize(vec) < (mBodySize * 15.0f) + (mBodySize * (GetRand(100) * 0.010f))) {
		if (GetRand(2) == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
			return true;
		}
	}
	return false;
}



/*=====================================*
 * SA-14
 *=====================================*/
/*static*/const char* CharaEnemySA_14::strAttackBtmNode = "右mgマズル";
/*static*/const char* CharaEnemySA_14::strAttackTopNode = "右mgマズル先";
/*static*/const char* CharaEnemySA_14::strAttackTopNodeLeft = "左刃1";
/*static*/const char* CharaEnemySA_14::strAttackBtmNodeLeft = "左電池";

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemySA_14::CharaEnemySA_14()
{
	for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
		mEfsHandle[iI] = -1;
	}
	memset(mEffTargetVec, 0, sizeof(mEffTargetVec));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemySA_14::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);
	mFlinchMax = 200;

	int model_handle = -1;
	COLOR_F color;

	for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
		if (mShotModel[iI].GetModelHandle() == -1) {
			mShotModel[iI].LinkModel(RES_MDL_LOCK_CENTER);
			mShotModel[iI].Setup();
		}
		mShotModel[iI].Scale() = ZERO_VEC;

		// マテリアルカラーを変更
		model_handle = mShotModel[iI].GetModelHandle();
		color = GetColorF(0.01f, 0.5f, 1.0f, 1.0f);

		MV1SetMaterialDifColor(model_handle, 0, color);
		MV1SetMaterialSpcColor(model_handle, 0, color);
		MV1SetMaterialAmbColor(model_handle, 0, color);
		MV1SetMaterialEmiColor(model_handle, 0, color);
		MV1SetUseVertDifColor(model_handle, FALSE);
		MV1SetUseVertSpcColor(model_handle, FALSE);

		GraphicManager::GetInstance()->EntryDrawModel(&mShotModel[iI]);
	}
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemySA_14::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);

	if (VSize(vec) < (mBodySize * 15.0f) + (mBodySize * (GetRand(100) * 0.010f))) {
		if (GetRand(160) == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
			return true;
		}
	}
	if (VSize(vec) < (mBodySize * 2.0f) + (mBodySize * (GetRand(100) * 0.010f))) {
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
CharaEnemySA_14::phaseEnterStateIdle(void)
{
	CharaEnemy::phaseEnterStateIdle();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemySA_14::phaseStateMove(void)
{
	CharaEnemy::phaseStateMove();

	if (EqualPhaseFrame(4) || EqualPhaseFrame(20)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_FOOT_STEP_ROBOT);

		CameraManager::GetInstance()->StartVibeCamera(0.4f, 2.0f, 10.0f);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemySA_14::phaseEnterStateAttack1(void)
{
	SoundManager::GetSndComponent()->PlaySound(eSE_CANNON_LASER_CHARGE);

	StartMotion(eMT_ATTACK_1, false, 1.0f, NULL);

	mStatePhase = 0;
	mStateCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemySA_14::phaseStateAttack1(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR weapon_btm_pos, weapon_top_pos;
	EFK_HANDLE handle;

	GetFramePosFromName(&weapon_btm_pos, strAttackBtmNode);
	GetFramePosFromName(&weapon_top_pos, strAttackTopNode);

	switch (mStatePhase) {
	case 0:
		SetRotationToTarget(&p_player->Position());
		for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
			mEfsHandle[iI] = EffectManager::GetInstance()->Play(eEF_NAVI,
																&weapon_btm_pos,
																60.0f,
																1.0f);
		}
		SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
		NEXT_SUB_STATE();

		break;

	case 1:
		for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
			EffectManager::GetInstance()->Position(mEfsHandle[iI]) = weapon_btm_pos;
		}
		if (mStateCount >= 50) {

			float eff_target_len = 43.0f;
			mEffTargetVec[0] = VNorm(VSub(weapon_top_pos, weapon_btm_pos));

			VECTOR crossX;
			crossX = VNorm(VCross(mEffTargetVec[0], UP_VEC));
			
			mEffTargetVec[1] = VAdd(mEffTargetVec[0], crossX);
			mEffTargetVec[2] = VSub(mEffTargetVec[0], crossX);
			
			mEffTargetVec[0] = VScale(mEffTargetVec[0], eff_target_len);
			mEffTargetVec[1] = VScale(mEffTargetVec[1], eff_target_len);
			mEffTargetVec[2] = VScale(mEffTargetVec[2], eff_target_len);

			handle = EffekseerManager::GetInstance()->Play(eEFK_EXPLOSION_BOMB, &weapon_btm_pos, 5.0f, 0.0f);

			for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
				mShotModel[iI].Scale() = VGet(3.0f, 3.0f, 3.0f);
				mShotModel[iI].SetAlpha(0.75f);
			}

			CameraManager::GetInstance()->StartShakeCamera(0.2f, 2.0f, 30.0f);

			SoundManager::GetSndComponent()->PlaySound(eSE_BURST);

			NEXT_SUB_STATE();
		}
		break;

	case 2:
		VECTOR effpos;
		for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
			effpos = EffectManager::GetInstance()->Position(mEfsHandle[iI]);
			effpos = VAdd(effpos, mEffTargetVec[iI]);
			EffectManager::GetInstance()->Position(mEfsHandle[iI]) = effpos;
			mShotModel[iI].Position() = effpos;
			mShotModel[iI].Rotation().z += 0.6f;

			mEffTargetVec[iI] = VScale(mEffTargetVec[iI], 0.99f);
		}		

		if (VSize(mEffTargetVec[0]) < 10.0f) {
			for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
				if (mEfsHandle[iI] != -1) {
					EffectManager::GetInstance()->Stop(mEfsHandle[iI], 0.1f);
					mEfsHandle[iI] = -1;
				}
				mShotModel[iI].Scale() = ZERO_VEC;
			}
			ChangeState(eCHARA_STATE_IDLE);
			return;
		}		

		/* 当たり判定処理 */
		if (p_player->IsDamage() == false) {
			VECTOR eff_pos;
			float eff_scale;
			ShapeCapsule capsuleA, capsuleB;

			for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
				eff_pos = EffectManager::GetInstance()->Position(mEfsHandle[iI]);
				eff_scale = EffectManager::GetInstance()->Scale(mEfsHandle[iI]).x;
		
				capsuleA.mPosA = p_player->Position();
				capsuleA.mPosB = p_player->Position();
				capsuleA.mPosA.y += p_player->BodySize();
				capsuleA.mPosB.y += (p_player->BodyHeight() - p_player->BodySize());
				capsuleA.mRadius = p_player->BodySize();

				capsuleB = ShapeCapsule(eff_pos, eff_pos, eff_scale);
				capsuleB.mPosB.y += 1.0f;
#ifdef _DEBUG
				GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleA);
				GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG
				if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
					SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
					int attack_power = 2;
					VECTOR damage_down_vec;
					damage_down_vec = VSub(p_player->Position(), capsuleB.mPosA);
					damage_down_vec.y = 0.0f;
					damage_down_vec = VNorm(damage_down_vec);
					p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
				}
			}

		}

		break;

	default:
		break;
	};

	mStateCount ++;

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemySA_14::phaseEnterStateAttack2(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_2);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
	StartMotion(eMT_GOBURIN_ATTACK_2, false, 0.4f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemySA_14::phaseStateAttack2(void)
{
	static float sStart = 15.0f;
	static float sEnd = 20.0f;

	VECTOR weapon_btm_pos, weapon_top_pos;
	GetFramePosFromName(&weapon_btm_pos, strAttackTopNodeLeft);
	GetFramePosFromName(&weapon_top_pos, strAttackBtmNodeLeft);

	bool is_prev_attack;

	float move_speed = 0.08f * GetMotionTimePercent();
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	if (EqualPhaseFrame(sStart)) {
		mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}

	is_prev_attack = mIsAttack;
	if (BetweenPhaseFrame(sStart, sEnd)) {

		mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);
		
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
		
			capsuleB.mPosA = weapon_top_pos;
			capsuleB.mPosB = weapon_btm_pos;
			capsuleB.mRadius = 60.0f;
			
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 2;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), mPosition);
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, false, eATTACK_TYPE_ENEMY_NORMAL);
			}
		}
	} else {
		mIsAttack = false;
	}

	PRINT_SCREEN(GetColor(255, 255, 0), "NowTime : %.2f\n", GetMotionNowTime());

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
CharaEnemySA_14::phaseStateWaitAttack(void)
{
	ChangeState(mReserveCharaState);
	mReserveCharaState = -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemySA_14::phaseEnterStateDamage(void)
{
	mBladeEffect.Stop();
	for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
		if (mEfsHandle[iI] != -1) {
			EffectManager::GetInstance()->Stop(mEfsHandle[iI], 0.1f);
			mEfsHandle[iI] = -1;
		}
		mShotModel[iI].Scale() = ZERO_VEC;
	}

	CharaEnemy::phaseEnterStateDamage();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemySA_14::phaseEnterStateDownStart(void)
{
	mBladeEffect.Stop();
	for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
		if (mEfsHandle[iI] != -1) {
			EffectManager::GetInstance()->Stop(mEfsHandle[iI], 0.1f);
			mEfsHandle[iI] = -1;
		}
		mShotModel[iI].Scale() = ZERO_VEC;
	}

	CharaEnemy::phaseEnterStateDownStart();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemySA_14::phaseEnterStateRetireEnd(void)
{
	for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
		GraphicManager::GetInstance()->ReserveRemoveModel(&mShotModel[iI]);
		mShotModel[iI].SetAlpha(0.0f);
		mShotModel[iI].DeleteModel();
	}

	mBladeEffect.Stop();
	for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
		if (mEfsHandle[iI] != -1) {
			EffectManager::GetInstance()->Stop(mEfsHandle[iI], 0.1f);
			mEfsHandle[iI] = -1;
		}
		mShotModel[iI].Scale() = ZERO_VEC;
	}

	CharaEnemy::phaseEnterStateRetireEnd();
}


 /**** end of file ****/


