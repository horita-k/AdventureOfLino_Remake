/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ロアナファイル
 * @author SPATZ.
 * @data   2012/05/25 00:27:32
 */
//---------------------------------------------------
#include "CharaEnemyLoana.h"
#include "Charactor/CharaInfo.h"
#include "Charactor/CharaPlayer.h"
#include "AppLib/Basic/Basic.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Effect/EffectManager.h"
#include "AppLib/Graphic/EffekseerManager.h"
#include "AppLib/Input/InputManager.h"
#include "AppLib/Sound/SoundManager.h"

#include "AppLib/Collision/CollisionManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "DataDefine/VoiceDef.h"
#include "DataDefine/EffectDef.h"
#include "Scene/ActionTaskManager.h"
#include "Utility/Utility.h"

static const char* strAttackSword_Top = "剣先";
static const char* strAttackSword_Btm = "剣";

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	mFlinchMax	= 250;
	mEfkHandle	= NULL;

//	ResourceModel::GetInstance()->LoadStageResource("dummy.mv1", false);
	mSonicWave.Setup(eOBJECT_KIND_DUMMY_OBJ, (eItemKind)0, (VECTOR *)&ZERO_VEC, 0, true);
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyLoana::checkChangeAttackState(int* pAttackCharaState)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);

	int rand_value = GetRand(100);
	float rand_rate = (float)rand_value * 0.01f;
	float dist = VSize(vec);


	if (dist < (mBodySize * 1.0f) + ((mBodySize * 1.0f) * rand_rate) ) {

		if ((dist < 1500.0f) && (p_player->GetLockonChara() != NULL) && 
			((p_player->GetState() == CharaPlayer::eCHARA_STATE_AWAY_RIGHT) || 
			 (p_player->GetState() == CharaPlayer::eCHARA_STATE_AWAY_LEFT))) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_8;
		}

		if (p_player->GetIsAttack()) {
			// ガード
			*pAttackCharaState = eCHARA_STATE_ATTACK_6;
			//			*pAttackCharaState = eCHARA_STATE_ATTACK_8;
			return true;
		}

		int rand_2nd = GetRand(1);
		switch (rand_2nd) {
		case 0:
			*pAttackCharaState = eCHARA_STATE_ATTACK_4;
			return true;
		case 1:
			*pAttackCharaState = eCHARA_STATE_ATTACK_5;
			return true;
		default:
			break;
		}
	}
	else if (dist < (mBodySize * 2.0f) + ((mBodySize * 2.0f) * rand_rate) ) {

		if (p_player->GetIsAttack()) {
			// ガード
			*pAttackCharaState = eCHARA_STATE_ATTACK_6;
			//			*pAttackCharaState = eCHARA_STATE_ATTACK_8;
			return true;
		}

		if (GetRand(10) == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_2;
			return true;
		}
	}
	else if (dist < (mBodySize * 4.0f) + ((mBodySize * 2.0f) * rand_rate) ) {
		if (GetRand(10) == 1) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_3;
			return true;
		}
	}
	else if (dist < (mBodySize * 8.0f) + ((mBodySize * 2.0f) * rand_rate) ) {
		if (GetRand(10) == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
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
CharaEnemyLoana::phaseStateWaitAttack(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	float dist = VSize(VSub(p_player->Position(), mPosition));

	if ((dist < 1500.0f) && (p_player->GetLockonChara() != NULL) && 
		((p_player->GetState() == CharaPlayer::eCHARA_STATE_AWAY_RIGHT) || 
		 (p_player->GetState() == CharaPlayer::eCHARA_STATE_AWAY_LEFT))) {
		ChangeState(eCHARA_STATE_ATTACK_8);
		mReserveCharaState = -1;
		return;
	}
	
	switch (mReserveCharaState) {
	case eCHARA_STATE_ATTACK_3:
	case eCHARA_STATE_ATTACK_6:
	case eCHARA_STATE_ATTACK_8:
		ChangeState(mReserveCharaState);
		mReserveCharaState = -1;
		break;
		
	case eCHARA_STATE_ATTACK_1:
	case eCHARA_STATE_ATTACK_2:
	case eCHARA_STATE_ATTACK_4:
	case eCHARA_STATE_ATTACK_5:
		if (GetRand(10) == 0) {
			ChangeState(mReserveCharaState);
			mReserveCharaState = -1;
		}
		break;

	default:
		APP_ASSERT(NULL, "Invalid case \n");
		break;
	};

}

/*=====================================*
 * extend
 *=====================================*/

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::Update(void)
{
	CharaEnemy::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	if (checkShieldHit(pDamageDownVec)) {
		VECTOR eff_pos;
		if (pEffPos == NULL) {
			GetFramePosFromName(&eff_pos, "左中指１");
		}
		else {
			eff_pos = *pEffPos;
		}
		EFK_HANDLE handle = EffekseerManager::GetInstance()->Play(eEFK_SHIELD, &eff_pos, &VScale(mScale, 0.8f));
		EffekseerManager::GetInstance()->SetRotationAxisY(handle, (mRotation.y + (PHI_F/2.0f)));

		SoundManager::GetSndComponent()->PlaySound(eSE_SHIELD);

		ChangeState(eCHARA_STATE_ATTACK_7);

		return;
	}

	int prev_state = mCharaState;

	CharaEnemy::ProcessDamage(attackPower, isSetMode, pEffPos, pDamageDownVec, isDamageDown, attackType);

	// ダメージでステータスが変わったら
	if (prev_state != mCharaState) {
		mSonicWave.ProcessDisappear();

		if (mEfkHandle != NULL) {
			EffekseerManager::GetInstance()->Stop(mEfkHandle, 0.0f);
			mEfkHandle = NULL;
		}

	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::phaseEnterStateAttack1(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttackSword_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttackSword_Btm);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_EXTEND_ATTACK_1, false, 0.6f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::phaseStateAttack1(void)
{
	static const float sStart		= 16.0f;
	static const float sEnd			= 40.0f;
	static const float sEffScale	= 0.3f;

	VECTOR weapon_top_pos, weapon_btm_pos;
	GetFramePosFromName(&weapon_top_pos, strAttackSword_Top);
	GetFramePosFromName(&weapon_btm_pos, strAttackSword_Btm);

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (EqualPhaseFrame(1)) {
		mEfkHandle = EffekseerManager::GetInstance()->Play(eEFK_CANNON_LASER_CHARGE, &weapon_top_pos, 9.0f);
		EffekseerManager::GetInstance()->SetSpeed(mEfkHandle, 1.5f);
	}
	else if (EqualPhaseFrame(sStart)) {
		MATRIX weapon_top_mtx;
		GetFrameMatrixFromNameByScale(&weapon_top_mtx, strAttackSword_Top, sEffScale);

		// ソニックウェーブ生成
		mSonicWave.ProcessCreateSonick(&weapon_top_mtx);
	}
	else if (EqualPhaseFrame(24.0f)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_CANNON_LASER);
	}
	else if (EqualPhaseFrame(29.0f)) {
		static const float shoot_speed = 20.0f;
		VECTOR direction;
		direction.x = sin(mRotation.y + PHI_F);
		direction.y = 0.0f;
		direction.z = cos(mRotation.y + PHI_F);
		direction = VScale(direction, shoot_speed);

		mSonicWave.ProcessShoot(&direction, sEffScale*10.0f, 50.0f);
	}
	
	if (BetweenPhaseFrame(sStart, 29.0f)) {
		MATRIX weapon_top_mtx;
		GetFrameMatrixFromNameByScale(&weapon_top_mtx, strAttackSword_Top, sEffScale);

		// ソニックウェーブのエフェクトアタッチ
		mSonicWave.SetSonicWaveMatrix(&weapon_top_mtx);
	}

	if (BetweenPhaseFrame(sStart, sEnd)) {

		float move_speed = 0.08f * GetMotionTimePercent();
		mMoveVec.x += -(sin(mRotation.y) * move_speed);
		mMoveVec.z += -(cos(mRotation.y) * move_speed);

		/* 戦闘不能者と既に攻撃された者を除く */
		if (p_player->CheckEnableAttack() == true) {

			CharaEnemy::tCheckDamageWork work;
			work.mPlayerPos		= mPosition;
			work.mWeaponTopPos	= weapon_top_pos;
			// 近すぎると当たらない為
			work.mWeaponBtmPos.x= mPosition.x;
			work.mWeaponBtmPos.y= weapon_btm_pos.y;
			work.mWeaponBtmPos.z= mPosition.z;
			
			// Hit?
			if (p_player->CheckDamageCollNormal(&work)) {
				VECTOR damage_down_vec = VNorm(VSub(p_player->Position(), mPosition));
				p_player->ProcessDamage(4, false, NULL, &damage_down_vec, false, eATTACK_TYPE_ENEMY_NORMAL);
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			}
		}
		mIsAttack = true;
	}
	else {
		mIsAttack = false;
	}

	if (mEfkHandle != NULL) {
		if (EffekseerManager::GetInstance()->IsPlay(mEfkHandle)) {
			EffekseerManager::GetInstance()->SetPosition(mEfkHandle, &weapon_btm_pos);
		}
		else {
			mEfkHandle = NULL;
		}
	}

	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {
		mBladeEffect.Stop();
		ChangeState(eCHARA_STATE_IDLE);
	}
}

/*=====================================*
 * phaseEnterStateAttack2
 * 突き
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::phaseEnterStateAttack2(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_2);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttackSword_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttackSword_Btm);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_EXTEND_ATTACK_2, false, 0.75f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::phaseStateAttack2(void)
{
	static const float sStart		= 12.0f;
	static const float sEnd			= 32.0f;

	VECTOR weapon_top_pos, weapon_btm_pos;
	GetFramePosFromName(&weapon_top_pos, strAttackSword_Top);
	GetFramePosFromName(&weapon_btm_pos, strAttackSword_Btm);

	float move_speed = 12.0f * sin( ((float)GetMotionTimePercent() / 100.0f) * (PHI_F * 2.0f) );
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (BetweenPhaseFrame(sStart, sEnd)) {

		/* 戦闘不能者と既に攻撃された者を除く */
		if (p_player->CheckEnableAttack() == true) {

			CharaEnemy::tCheckDamageWork work;
			work.mPlayerPos		= mPosition;
			work.mWeaponTopPos	= weapon_top_pos;
			// 近すぎると当たらない為
			work.mWeaponBtmPos.x= mPosition.x;
			work.mWeaponBtmPos.y= weapon_btm_pos.y;
			work.mWeaponBtmPos.z= mPosition.z;
			
			// Hit?
			if (p_player->CheckDamageCollNormal(&work)) {
				VECTOR damage_down_vec = VNorm(VSub(p_player->Position(), mPosition));
				p_player->ProcessDamage(2, false, NULL, &damage_down_vec, false, eATTACK_TYPE_ENEMY_NORMAL);
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			}
		}
		mIsAttack = true;
	}
	else {
		mIsAttack = false;
	}

	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {
		mBladeEffect.Stop();
		ChangeState(eCHARA_STATE_ATTACK_3);
	}
}

/*=====================================*
 * phaseEnterStateAttack3
 * ジャンプ斬り
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::phaseEnterStateAttack3(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_2);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttackSword_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttackSword_Btm);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_EXTEND_ATTACK_3, false, 0.9f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::phaseStateAttack3(void)
{
	static const float sStart		= 25.0f;
	static const float sEnd			= 42.0f;
	static const float sEffScale	= 0.3f;

	VECTOR weapon_top_pos, weapon_btm_pos;
	GetFramePosFromName(&weapon_top_pos, strAttackSword_Top);
	GetFramePosFromName(&weapon_btm_pos, strAttackSword_Btm);

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (EqualPhaseFrame(sStart)) {
		// ソニックウェーブ生成
		MATRIX weapon_top_mtx;
		GetFrameMatrixFromNameByScale(&weapon_top_mtx, strAttackSword_Top, sEffScale);
		mSonicWave.ProcessCreateSonick(&weapon_top_mtx);
	}
	else if (EqualPhaseFrame(sEnd-5.0f)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_CANNON_LASER);
	}
	else if (EqualPhaseFrame(sEnd)) {
		static const float shoot_speed = 20.0f;
		VECTOR direction;
		direction.x = sin(mRotation.y + PHI_F);
		direction.y = 0.0f;
		direction.z = cos(mRotation.y + PHI_F);
		direction = VScale(direction, shoot_speed);

		mSonicWave.ProcessShoot(&direction, sEffScale*10.0f, 50.0f);

		const float size = 30.0f;
		VECTOR effpos;
		effpos = weapon_top_pos;
		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,&effpos,size, 0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 2.0f);
		SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
		CameraManager::GetInstance()->StartShakeCamera(1.0f, 2.0f, 30.0f);
	}

	if (BetweenPhaseFrame(sStart, sEnd)) {

		// ソニックウェーブのエフェクトアタッチ
		MATRIX weapon_top_mtx;
		GetFrameMatrixFromNameByScale(&weapon_top_mtx, strAttackSword_Top, sEffScale);
		mSonicWave.SetSonicWaveMatrix(&weapon_top_mtx);

		//		float move_speed = 0.50f * (100 -  GetMotionTimePercent());
		float dist = VSize(VSub(p_player->Position(), mPosition));
		dist -= mBodySize;

		float move_speed = dist * 0.1f;
		if (move_speed > 100.0f) {
			move_speed = 100.0f; // 速度制限
		}
		mMoveVec.x += -(sin(mRotation.y) * move_speed);
		mMoveVec.z += -(cos(mRotation.y) * move_speed);

		mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

		/* 戦闘不能者と既に攻撃された者を除く */
		if (p_player->CheckEnableAttack() == true) {

			CharaEnemy::tCheckDamageWork work;
			work.mPlayerPos		= mPosition;
			work.mWeaponTopPos	= weapon_top_pos;
			// 近すぎると当たらない為
			work.mWeaponBtmPos.x= mPosition.x;
			work.mWeaponBtmPos.y= weapon_btm_pos.y;
			work.mWeaponBtmPos.z= mPosition.z;
			
			// Hit?
			if (p_player->CheckDamageCollNormal(&work)) {
				VECTOR damage_down_vec = VNorm(VSub(p_player->Position(), mPosition));
				p_player->ProcessDamage(4, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);

				EFK_HANDLE efk_handle = EffekseerManager::GetInstance()->Play(eEFK_BREAK_BARRIER, &weapon_top_pos, 6.0f);
				EffekseerManager::GetInstance()->SetSpeed(efk_handle, 1.0f);
			}
		}
		mIsAttack = true;
	}
	else {
		mIsAttack = false;
	}

	if (GetIsEndMotion()) {
		mBladeEffect.Stop();
		ChangeState(eCHARA_STATE_IDLE);
	}

}

/*=====================================*
 * phaseEnterStateAttack4
 * 通常連撃
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::phaseEnterStateAttack4(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttackSword_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttackSword_Btm);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_EXTEND_ATTACK_4, false, 0.75f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::phaseStateAttack4(void)
{
	static const float sStart		= 8.0f;
	static const float sEnd			= 60.0f;

	VECTOR weapon_top_pos, weapon_btm_pos;
	GetFramePosFromName(&weapon_top_pos, strAttackSword_Top);
	GetFramePosFromName(&weapon_btm_pos, strAttackSword_Btm);

	float move_speed = 0.07f * GetMotionTimePercent();
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if ((EqualPhaseFrame(20.0f) || EqualPhaseFrame(48.0f)) ) {
		p_player->IsDamage() = false;
	}

	if (BetweenPhaseFrame(sStart, sEnd)) {

		/* 戦闘不能者と既に攻撃された者を除く */
		if (p_player->CheckEnableAttack() == true) {

			CharaEnemy::tCheckDamageWork work;
			work.mPlayerPos		= mPosition;
			work.mWeaponTopPos	= weapon_top_pos;
			// 近すぎると当たらない為
			work.mWeaponBtmPos.x= mPosition.x;
			work.mWeaponBtmPos.y= weapon_btm_pos.y;
			work.mWeaponBtmPos.z= mPosition.z;
			
			// Hit?
			if (p_player->CheckDamageCollNormal(&work)) {
				VECTOR damage_down_vec = VNorm(VSub(p_player->Position(), mPosition));
				p_player->ProcessDamage(2, false, NULL, &damage_down_vec, false, eATTACK_TYPE_ENEMY_NORMAL);
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			}
		}
		mIsAttack = true;
	}
	else {
		mIsAttack = false;
	}

	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {
		mBladeEffect.Stop();
		if (GetRand(1) == 0) {
			ChangeState(eCHARA_STATE_IDLE);
		} else {
			ChangeState(eCHARA_STATE_ATTACK_5);
		}
	}
}

/*=====================================*
 * phaseEnterStateAttack5
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::phaseEnterStateAttack5(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttackSword_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttackSword_Btm);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_EXTEND_ATTACK_5, false, 0.6f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::phaseStateAttack5(void)
{
	static const float sStart		= 10.0f;
	static const float sEnd			= 44.0f;

	VECTOR weapon_top_pos, weapon_btm_pos;
	GetFramePosFromName(&weapon_top_pos, strAttackSword_Top);
	GetFramePosFromName(&weapon_btm_pos, strAttackSword_Btm);

	/*
	float move_speed = 0.0f * GetMotionTimePercent();
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);
	*/

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (EqualPhaseFrame(32.0f)) {
		p_player->IsDamage() = false;
	}

	if (BetweenPhaseFrame(sStart, sEnd)) {

		/* 戦闘不能者と既に攻撃された者を除く */
		if (p_player->CheckEnableAttack() == true) {

			CharaEnemy::tCheckDamageWork work;
			work.mPlayerPos		= mPosition;
			work.mWeaponTopPos	= weapon_top_pos;
			// 近すぎると当たらない為
			work.mWeaponBtmPos.x= mPosition.x;
			work.mWeaponBtmPos.y= weapon_btm_pos.y;
			work.mWeaponBtmPos.z= mPosition.z;
			
			// Hit?
			if (p_player->CheckDamageCollNormal(&work)) {
				VECTOR damage_down_vec = VNorm(VSub(p_player->Position(), mPosition));
				p_player->ProcessDamage(1, false, NULL, &damage_down_vec, false, eATTACK_TYPE_ENEMY_NORMAL);
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			}
		}
		mIsAttack = true;
	}
	else {
		mIsAttack = false;
	}

	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {
		mBladeEffect.Stop();
		ChangeState(eCHARA_STATE_IDLE);
	}
}

/*=====================================*
 * phaseEnterStateAttack6
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::phaseEnterStateAttack6(void)
{
	StartMotion(eMT_EXTEND_ATTACK_6, false, 0.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::phaseStateAttack6(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (p_player->GetIsAttack() == false) {
	//	if ((p_player->GetState() == eCHARA_STATE_IDLE) ||
		//		(p_player->GetState() == eCHARA_STATE_JUMP_DOWN)) {
		if (GetRand(1) == 0) {
			ChangeState(eCHARA_STATE_ATTACK_3);
		}
		else {
			ChangeState(eCHARA_STATE_ATTACK_5);
		}
			
	}
}

/*=====================================*
 * phaseEnterStateAttack7
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::phaseEnterStateAttack7(void)
{
	StartMotion(eMT_EXTEND_ATTACK_7, false, 2.0f);

	mIsDamage = true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::phaseStateAttack7(void)
{
	float move_speed = -0.24f * (100 - GetMotionTimePercent());
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	if (GetIsEndMotion()) {

		mIsDamage = false;
		if (mReserveCharaState != -1) {
			ChangeState(mReserveCharaState);
			mReserveCharaState = -1;
		}
		else {
			ChangeState(eCHARA_STATE_ATTACK_6);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::phaseEnterStateAttack8(void)
{
	StartMotion(eMT_EXTEND_ATTACK_8, false, 0.75f, NULL);

	mBladeEffect.Stop();

	SoundManager::GetSndComponent()->PlaySound(eSE_AWAY);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyLoana::phaseStateAttack8(void)
{
	VECTOR dir_vec;
	float move_speed = kCHARA_AWAY_SPEED * (float)(100 - GetMotionTimePercent());

	dir_vec.x = sin(mRotation.y + Utility::DegToRad(0.0f));
	dir_vec.y = 0.0f;
	dir_vec.z = cos(mRotation.y + Utility::DegToRad(0.0f));

	mMoveVec = VScale(dir_vec, move_speed);
	

	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_ATTACK_3);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CharaEnemyLoana::checkShieldHit(VECTOR *pAttackDirVec)
{
	if ((mCharaState == eCHARA_STATE_ATTACK_6) ||
		(mCharaState == eCHARA_STATE_WAIT_ATTACK)) {

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

 /**** end of file ****/
