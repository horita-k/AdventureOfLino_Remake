/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * エクシヴィファイル
 * @author SPATZ.
 * @data   2015/12/31 10:04:48
 */
//---------------------------------------------------
#include "../CharaPlayer.h"
#include "CharaEnemyExcivy.h"
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

#include "AppLib/Resource/ResourceModel.h"

static const char* strAttack01_Top = "右中指３";
static const float sSmashAttackVec		= 0.77f * 0.65f;// スマッシュ攻撃の移動量
//static const float sEffScale	= 0.5f;
static const float sEffScale	= 1.0f;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyExcivy::CharaEnemyExcivy()
{
	mIsSkyDiveAttack = FALSE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	mEfsFlame		= -1;
	mEfkFlame		= NULL;
	mHellFlameCnt	= 0;

	mEfkCharge		= NULL;

	mSmashState		= 0;

	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	mFlinchMax = 250;

	ResourceModel::GetInstance()->LoadItemResource(RES_MDL_DRAGON_SWORD);

	if (mWeaponModel.GetModelHandle() == -1) {
		mWeaponModel.LinkModel(RES_MDL_DRAGON_SWORD);
		mWeaponModel.Setup();
	}
	this->AttachModel(&mWeaponModel,
					  ModelPmd::eATTACHED_MODEL_NORMAL,
					  this->GetFrameIndexFromName("右中指１"));
	GraphicManager::GetInstance()->EntryDrawModel(&mWeaponModel);

//	ResourceModel::GetInstance()->LoadStageResource("dummy.mv1", false);
	mSonicWave.Setup(eOBJECT_KIND_DUMMY_OBJ, (eItemKind)0, (VECTOR *)&ZERO_VEC, 0, true);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::Update(void)
{
	CharaEnemy::Update();

	if (mEfsFlame != -1) {
		MATRIX weapon_top_mtx;
		mWeaponModel.GetFrameMatrixFromNameByScale(&weapon_top_mtx, "剣尾", 80.0f);
		EffectManager::GetInstance()->Matrix(mEfsFlame) = weapon_top_mtx;
	}

	if (mEfkFlame != NULL) {
		CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
		/* 戦闘不能者と既に攻撃された者を除く */
		if (p_player->CheckEnableAttack() == true) {
			//			tCheckDamageWork work = {mHellFlamePos, ZERO_VEC, ZERO_VEC, 440};
			tCheckDamageWork work = {mHellFlamePos, ZERO_VEC, ZERO_VEC, 360};
			
			if (p_player->CheckDamageCollRolling(&work)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);

				int attack_power = 5;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), mHellFlamePos);
				damage_down_vec.y = 0.0f;
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_FLAME);
			}
		}

		mHellFlameCnt --;
		if (mHellFlameCnt == 0) {
			EffekseerManager::GetInstance()->Stop(mEfkFlame, 0.0f);
			mEfkFlame = NULL;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	if (checkShieldHit(pDamageDownVec)) {
		VECTOR eff_pos;
		if (pEffPos == NULL) {
			GetFramePosFromName(&eff_pos, "右中指１");
		}
		else {
			eff_pos = *pEffPos;
		}
		EFK_HANDLE handle = EffekseerManager::GetInstance()->Play(eEFK_SHIELD, &eff_pos, &VScale(mScale, 0.8f));
		EffekseerManager::GetInstance()->SetRotationAxisY(handle, (mRotation.y + (DX_PI/2.0f)));

		SoundManager::GetSndComponent()->PlaySound(eSE_SHIELD);

		ChangeState(eCHARA_STATE_ATTACK_5);

		return;
	}

	int prev_state = mCharaState;

	CharaEnemy::ProcessDamage(attackPower, isSetMode, pEffPos, pDamageDownVec, isDamageDown, attackType);

	// ダメージでステータスが変わったら
	if (prev_state != mCharaState) {
		mSonicWave.ProcessDisappear();

		GraphicManager::GetInstance()->RevertFogParam(1.0f);

		if (mEfkCharge != NULL) {
			EffekseerManager::GetInstance()->Stop(mEfkCharge, 0.0f);
			mEfkCharge = NULL;
		}
	}
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyExcivy::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);

	int rand_value = GetRand(100);
	float rand_rate = (float)rand_value * 0.01f;
	float dist = VSize(vec);

	if (dist < (mBodySize * 1.0f) + ((mBodySize * 2.0f) * rand_rate) ) {

		if (p_player->GetIsAttack()) {
			if (GetRand(1) == 0) {
				// 回避
				*pAttackCharaState = eCHARA_STATE_ATTACK_2;
				return true;
			}
			else {
				// ガード
				*pAttackCharaState = eCHARA_STATE_ATTACK_4;
				return true;
			}
		}
		int rand_2nd = GetRand(1);
		switch (rand_2nd) {
		case 0:
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
			return true;
		case 1:
			*pAttackCharaState = eCHARA_STATE_ATTACK_3;
			mIsSkyDiveAttack = FALSE;
			return true;
		default:
			break;
		}
	}
	else if ((dist > (mBodySize * 4.0f)) &&
			 (dist < (mBodySize * 6.0f) + ((mBodySize * 4.0f) * rand_rate) )) {
		if (GetRand(10) == 1) {

			if (mIsSkyDiveAttack == FALSE) {
				*pAttackCharaState = eCHARA_STATE_ATTACK_6;
				mIsSkyDiveAttack = TRUE;
			}
			else {
				if (GetRand(10) == 1) {
					*pAttackCharaState = eCHARA_STATE_ATTACK_3;
				}
				else {
					*pAttackCharaState = eCHARA_STATE_ATTACK_8;
				}
			}
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
CharaEnemyExcivy::phaseStateWaitAttack(void)
{
	switch (mReserveCharaState) {
	case eCHARA_STATE_ATTACK_2:
	case eCHARA_STATE_ATTACK_4:
	case eCHARA_STATE_ATTACK_5:
	case eCHARA_STATE_ATTACK_6:
	case eCHARA_STATE_ATTACK_7:
		ChangeState(mReserveCharaState);
		mReserveCharaState = -1;
		break;
		
	case eCHARA_STATE_ATTACK_3:
		if (GetRand(30) == 0) {
			ChangeState(mReserveCharaState);
			mReserveCharaState = -1;
		}
		break;

	default:
		if (GetRand(10) == 0) {
			ChangeState(mReserveCharaState);
			mReserveCharaState = -1;
		}
		break;
	};
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::processNoMoveAction(void)
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
CharaEnemyExcivy::phaseEnterStateIdle(void)
{
	CharaEnemy::phaseEnterStateIdle();

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (VSize(VSub(p_player->Position(), mPosition)) < 500.0f) {

		VECTOR veca, vecb;
		veca.x = sin(mRotation.y + DX_PI);
		veca.y = 0.0f;
		veca.z = cos(mRotation.y + DX_PI);

		vecb = VNorm(VSub(p_player->Position(), mPosition));
		float dot = VDot(veca, vecb);
		PRINT_CONSOLE("dot : %.2f \n", dot);
		if (dot < -0.75f) {
			ChangeState(eCHARA_STATE_ATTACK_7);
		}
	}
	
	changeWeaponMaterial(false);

	if (mEfsFlame != -1) {
		EffectManager::GetInstance()->Stop(mEfsFlame, 5.0f);
		mEfsFlame = -1;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::phaseEnterStateAttack1(void)
{
	VECTOR weapon_top_pos, weapon_btm_pos;
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣尾");
	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	//	mEfsFlame = EffectManager::GetInstance()->Play(eEF_FLAME,
	if (mEfsFlame == -1) {
		mEfsFlame = EffectManager::GetInstance()->Play(eEF_FLAME_BOMB,
														  (VECTOR *)&ZERO_VEC,
														  180.0f, 1.0f);
		EffectManager::GetInstance()->SetMatrixType(mEfsFlame, EffectNode::eMATRIX_TYPE_MATRIX);
	}

	changeWeaponMaterial(true);

	GraphicManager::tFogParam fogParam;
	fogParam.mIsEnable = TRUE;
	fogParam.mIsActive = TRUE;
	fogParam.mColR = 10;
	fogParam.mColG = 10;
	fogParam.mColB = 10;
	fogParam.mNear = 10.0f;
	fogParam.mFar = 5000.0f;
	GraphicManager::GetInstance()->ChangeFogParam(0.3f, &fogParam);

	StartMotion(eMT_EXTEND_ATTACK_1, false, 1.0f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::phaseStateAttack1(void)
{
	static float sStart = 34.0f;
	static float sEnd = 54.0f;

	CharaBase* p_player;
	p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	VECTOR weapon_top_pos, weapon_btm_pos;
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣尾");
	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");

	if (this->EqualPhaseFrame(sStart)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}
	if (this->EqualPhaseFrame(52.0f)) {
		const float size = 20.0f;
		VECTOR effpos;
		effpos = weapon_top_pos;
		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,
															&effpos,
															50.0f,
															0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 2.0f);
		CameraManager::GetInstance()->StartVibeCamera(10.0f, 7.0f, 10.0f);

		mEfkFlame = EffekseerManager::GetInstance()->Play(eEFK_HELL_FLAME, &effpos, size);
		mHellFlameCnt = 70;
		mHellFlamePos = effpos;

		SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
	}

	mIsAttack = false;

	if (BetweenPhaseFrame(sStart, sEnd)) {

		/* 戦闘不能者と既に攻撃された者を除く */
		if (p_player->CheckEnableAttack() == true) {

			ShapeCapsule capsule;
			ShapeLine line;
			p_player->GetBodyCapsule(&capsule);

			mWeaponModel.GetFramePosFromName(&line.mStart, "剣先");
			mWeaponModel.GetFramePosFromName(&line.mEnd, "剣尾");

			if (CollisionManager::Check_LineToCapsule(&line, &capsule)) {

				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			
				int attack_power = 4;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), mPosition);
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_FLAME_BREAK_GUARD);
			}
		}
		mIsAttack = true;
	}

	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {

		GraphicManager::GetInstance()->RevertFogParam(2.0f);

		if (mEfsFlame != -1) {
			EffectManager::GetInstance()->Stop(mEfsFlame, 5.0f);
			mEfsFlame = -1;
		}

		changeWeaponMaterial(false);

		mBladeEffect.Stop();
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::phaseEnterStateAttack2(void)
{
	StartMotion(eMT_EXTEND_ATTACK_2, false, 0.75f, NULL);

	mBladeEffect.Stop();

	SoundManager::GetSndComponent()->PlaySound(eSE_AWAY);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::phaseStateAttack2(void)
{
	VECTOR dir_vec;
	float move_speed = kCHARA_AWAY_SPEED * (float)(100 - GetMotionTimePercent());

	dir_vec.x = sin(mRotation.y + Utility::DegToRad(0.0f));
	dir_vec.y = 0.0f;
	dir_vec.z = cos(mRotation.y + Utility::DegToRad(0.0f));

	mMoveVec = VScale(dir_vec, move_speed);
	

	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_ATTACK_6);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::phaseEnterStateAttack3(void)
{
	VECTOR weapon_top_pos, weapon_btm_pos;
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣尾");
	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_EXTEND_ATTACK_3, false, 0.35f);

	SoundManager::GetSndComponent()->PlaySound(eSE_CANNON_LASER_CHARGE);

	MATRIX weapon_top_mtx;
	getSonicWaveMatrix(weapon_top_mtx, 15.0f, false);
	mEfkCharge = EffekseerManager::GetInstance()->Play(eEFK_CANNON_LASER_CHARGE, &weapon_top_pos, 9.0f);
	EffekseerManager::GetInstance()->SetSpeed(mEfkCharge, 1.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::phaseStateAttack3(void)
{
	static float sStart	= 10.0f;
	static float sMiddle= 17.0f;
	static float sEnd	= 25.0f;

	VECTOR weapon_top_pos, weapon_btm_pos;
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣尾");
	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");

	if (this->EqualPhaseFrame(sStart)) {
		MATRIX tmp;
		tmp = MGetIdent();
		mSonicWave.ProcessCreateSonick(&tmp);
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}

	if (EqualPhaseFrame(sMiddle)) {
		static const float shoot_speed = 20.0f;
		VECTOR direction;
		/*
		direction.x = sin(mRotation.y + DX_PI + Utility::DegToRad(15.0f));
		direction.y = 0.0f;
		direction.z = cos(mRotation.y + DX_PI + Utility::DegToRad(15.0f));
		*/
		direction.x = sin(mRotation.y + DX_PI);
		direction.y = -0.02f;
		direction.z = cos(mRotation.y + DX_PI);

		direction = VScale(direction, shoot_speed);

		if (mEfkCharge != NULL) {
			EffekseerManager::GetInstance()->Stop(mEfkCharge, 0.0f);
			mEfkCharge = NULL;
		}

		SoundManager::GetSndComponent()->PlaySound(eSE_CANNON_LASER);
		mSonicWave.ProcessShoot(&direction, sEffScale*10.0f, 180.0f);
	}
	else if (BetweenPhaseFrame(0.0f, sMiddle)) {
		EffekseerManager::GetInstance()->SetPosition(mEfkCharge, &weapon_btm_pos);

		MATRIX weapon_top_mtx;
		getSonicWaveMatrix(weapon_top_mtx, 7.5f, false);
		mSonicWave.SetSonicWaveMatrix(&weapon_top_mtx);
	}

	if (BetweenPhaseFrame(sStart, sEnd)) {
		mIsAttack = true;

		CharaBase* p_player;
		p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		/* 戦闘不能者と既に攻撃された者を除く */
		if (p_player->CheckEnableAttack() == true) {

			ShapeCapsule capsule;
			ShapeLine line;
			p_player->GetBodyCapsule(&capsule);

			mWeaponModel.GetFramePosFromName(&line.mStart, "剣先");
			mWeaponModel.GetFramePosFromName(&line.mEnd, "剣尾");

			if (CollisionManager::Check_LineToCapsule(&line, &capsule)) {

				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			
				int attack_power = 2;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), this->mPosition);
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
CharaEnemyExcivy::phaseEnterStateAttack4(void)
{
	StartMotion(eMT_EXTEND_ATTACK_4, false, 0.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::phaseStateAttack4(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (p_player->GetIsAttack() == false) {
	//	if ((p_player->GetState() == eCHARA_STATE_IDLE) ||
		//		(p_player->GetState() == eCHARA_STATE_JUMP_DOWN)) {
		ChangeState(eCHARA_STATE_ATTACK_3);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::phaseEnterStateAttack5(void)
{
	StartMotion(eMT_EXTEND_ATTACK_5, false, 2.0f);

	mIsDamage = true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::phaseStateAttack5(void)
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
			ChangeState(eCHARA_STATE_ATTACK_4);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::phaseEnterStateAttack6(void)
{
	StartMotion(eMT_EXTEND_ATTACK_6_START, false, 0.9f, NULL, 0.08f);

	// ソニックウェーブ生成
	MATRIX weapon_top_mtx;
	getSonicWaveMatrix(weapon_top_mtx, 7.5f, true);

	mSonicWave.ProcessCreateSonick(&weapon_top_mtx);

	mSmashState = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::phaseStateAttack6(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	static const float sStart	= 13.0f;
	static const float sMiddle	= 25.0f;
	static const float sEnd		= 32.0f;

	VECTOR weapon_top_pos, weapon_btm_pos;
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣尾");
	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");
	
	float dist = 0.0f;
	dist = VSize(VSub(p_player->Position(), mPosition));
	dist -= p_player->BodySize() + mBodySize;
	//	dist -= (mBodySize + p_player->BodySize());
	float move_speed;
	if (BetweenPhaseFrame(8.0f, sEnd)) {
		move_speed = dist * 0.1f;
		if (move_speed > 70.0f) {
			move_speed = 70.0f; // 速度制限
		}
		mMoveVec.x += -(sin(mRotation.y) * move_speed);
		mMoveVec.z += -(cos(mRotation.y) * move_speed);
	}
	else {
		mJumpMoveVec.mLanding = VScale(mJumpMoveVec.mLanding, 0.96f);
		mMoveVec = mJumpMoveVec.mLanding;
	}

	if (EqualPhaseFrame(sStart)) {
		mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);
	}
	else if (EqualPhaseFrame(sMiddle)) {
		// モーションが指定以上の場合
		SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
	}
	else if (EqualPhaseFrame(sEnd)) {
		CameraManager::GetInstance()->StartVibeCamera(0.5f, 4.0f, 5.0f);

		SoundManager::GetSndComponent()->PlaySound(eSE_CANNON_LASER);

		static const float shoot_speed = 20.0f;
		VECTOR direction;
		direction.x = sin(mRotation.y + DX_PI);
		direction.y = 0.0f;
		direction.z = cos(mRotation.y + DX_PI);
		direction = VScale(direction, shoot_speed);

		mSonicWave.ProcessShoot(&direction, sEffScale*10.0f, 100.0f);

		mJumpMoveVec.mLanding.x = mMoveVec.x;
		mJumpMoveVec.mLanding.y = 0.0f;
		mJumpMoveVec.mLanding.z = mMoveVec.z;
	}


	if (BetweenPhaseFrame(sStart, sEnd)) {
		// ソニックウェーブのエフェクトアタッチ
		MATRIX weapon_top_mtx;
		getSonicWaveMatrix(weapon_top_mtx, 7.5f, true);
		mSonicWave.SetSonicWaveMatrix(&weapon_top_mtx);
	}
	/* 戦闘不能者と既に攻撃された者を除く */
	if (BetweenPhaseFrame(sMiddle, sEnd + 8.0f)) {
		if (p_player->CheckEnableAttack() == true) {
			CharaEnemy::tCheckDamageWork work;
			work.mPlayerPos = mPosition;
			work.mWeaponTopPos = weapon_top_pos;
			work.mWeaponBtmPos = weapon_btm_pos;

			if (p_player->CheckDamageCollNormal(&work)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);

				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), mPosition);
				damage_down_vec = VNorm(damage_down_vec);

				// スマッシュ斬りの場合
				EFK_HANDLE efk_handle = NULL;
				efk_handle =  EffekseerManager::GetInstance()->Play(eEFK_SHOCK, &mPosition, 15.0f);
				EffekseerManager::GetInstance()->SetSpeed(efk_handle, 1.2f);
				efk_handle = EffekseerManager::GetInstance()->Play(eEFK_BREAK_BARRIER, &weapon_top_pos, 6.0f);
				EffekseerManager::GetInstance()->SetSpeed(efk_handle, 2.0f);
				p_player->ProcessDamage(3, false, &weapon_top_pos, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
			}		
		}
	}

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
CharaEnemyExcivy::phaseEnterStateAttack7(void)
{
	VECTOR weapon_top_pos, weapon_btm_pos;
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣尾");
	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	mGravity *= 0.5f;

	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	StartMotion(eMT_EXTEND_ATTACK_7, false, 0.30f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::phaseStateAttack7(void)
{
	VECTOR weapon_top_pos, weapon_btm_pos;
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣尾");
	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");

	mMoveVec = VScale(mJumpMoveVec.mDash, 0.5f);
	mGravity *= 0.5f;

	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	if (BetweenPhaseFrame(7.0f, 20.0f)) {
		mIsAttack = true;

		CharaBase* p_player;
		p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		/* 戦闘不能者と既に攻撃された者を除く */
		if (p_player->CheckEnableAttack() == true) {

			CharaEnemy::tCheckDamageWork work;
			work.mPlayerPos = mPosition;
			work.mWeaponTopPos = weapon_top_pos;
			work.mWeaponBtmPos = weapon_btm_pos;

			if (p_player->CheckDamageCollJump(&work)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), mPosition);
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(2, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
			}
		}

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
CharaEnemyExcivy::phaseEnterStateAttack8(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_2);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣尾");
	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_EXTEND_ATTACK_8, false, 0.85f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::phaseStateAttack8(void)
{
	static const float sStart		= 12.0f;
	static const float sEnd			= 30.0f;

	VECTOR weapon_top_pos, weapon_btm_pos;
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣尾");
	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");

	float move_speed = 0.22f * (100 - GetMotionTimePercent());
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
		if (GetRand(2) == 0) {
			ChangeState(eCHARA_STATE_IDLE);
		}
		else {
			ChangeState(eCHARA_STATE_ATTACK_9);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::phaseEnterStateAttack9(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_2);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣尾");
	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_EXTEND_ATTACK_9, false, 0.65f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::phaseStateAttack9(void)
{
	static const float sStart		= 3.0f;
	static const float sEnd			= 18.0f;

	VECTOR weapon_top_pos, weapon_btm_pos;
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣尾");
	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");

	float move_speed = 0.07f * GetMotionTimePercent();
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

		if (GetRand(1) == 0) {
			ChangeState(eCHARA_STATE_ATTACK_1);
		}
		else {
			ChangeState(eCHARA_STATE_IDLE);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::phaseEnterStateDamage(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::phaseEnterStateRetireEnd(void)
{
	GraphicManager::GetInstance()->ReserveRemoveModel(&mWeaponModel);
	mWeaponModel.SetAlpha(0.0f);
	mWeaponModel.DeleteModel();

	GraphicManager::GetInstance()->RevertFogParam(1.0f);

	CharaEnemy::phaseEnterStateRetireEnd();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CharaEnemyExcivy::checkShieldHit(VECTOR *pAttackDirVec)
{
	if ((mCharaState == eCHARA_STATE_ATTACK_4) ||
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

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::changeWeaponMaterial(bool isChange)
{
	COLOR_F color;
	
	if (isChange) {
		color = GetColorF(2.0f, 0.01f, 0.01f, 1.0f);
	}
	else {
		color = GetColorF(0.5f, 0.5f, 0.5f, 1.0f);
	}

	int model_handle = mWeaponModel.GetModelHandle();

	for (int iI = 0; iI < MV1GetMaterialNum(model_handle); iI ++) {
	// マテリアルカラーを変更
		MV1SetMaterialDifColor(model_handle, iI, color);
		MV1SetMaterialSpcColor(model_handle, iI, color);
		MV1SetMaterialAmbColor(model_handle, iI, color);
		MV1SetMaterialEmiColor(model_handle, iI, color);
	}
	MV1SetUseVertDifColor(model_handle, FALSE);
	MV1SetUseVertSpcColor(model_handle, FALSE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyExcivy::getSonicWaveMatrix(MATRIX& rMatrix, float setScale, bool isVertical)
{
	//	VECTOR weapon_top_pos;
	//	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");
	VECTOR middle_pos;

	VECTOR posa, posb;
	VECTOR vecz, vecn;

	mWeaponModel.GetFramePosFromName(&posa, "サイド");
	mWeaponModel.GetFramePosFromName(&posb, "センター");
	vecn = VNorm(VSub(posb, posa) );

	mWeaponModel.GetFramePosFromName(&posa, "剣尾");
	mWeaponModel.GetFramePosFromName(&posb, "剣先");
	vecz = VNorm(VSub(posb, posa) );

	if (isVertical == false) {
		middle_pos = posa;
		vecz = VCross(vecz, vecn);
	}
	else {
		middle_pos = GET_MIDDLE_POS(posa, posb);
		vecz = VScale(vecz, -1.0f);
	}
	
	MATRIX tmp;

	rMatrix = MGetIdent();

	// 拡縮行列
	tmp = MGetScale(VGet(setScale, setScale, setScale));
	rMatrix = MMult(rMatrix, tmp);
	
	// 回転行列
	Utility::MTXLookAtMatrix(&tmp,
							 (VECTOR *)&ZERO_VEC,
							 &vecz,
							 &vecn);

	rMatrix = MMult(rMatrix, tmp);

	// 移動行列
	tmp = MGetTranslate(middle_pos);
	rMatrix = MMult(rMatrix, tmp);
}


 /**** end of file ****/
