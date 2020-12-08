/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ギギネブラファイル
 * @author SPATZ.
 * @data   2012/05/25 00:27:32
 */
//---------------------------------------------------
#include "CharaEnemyFresbelk.h"
#include "Charactor/CharaInfo.h"
#include "AppLib/Basic/Basic.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Effect/EffectManager.h"
#include "AppLib/Input/InputManager.h"
#include "AppLib/Sound/SoundManager.h"

#include "AppLib/Graphic/EffekseerManager.h"

#include "AppLib/Collision/CollisionManager.h"
#include "DataDefine/MotionDef.h"
#include "DataDefine/VoiceDef.h"
#include "DataDefine/EffectDef.h"
#include "Scene/ActionTaskManager.h"
#include "Utility/Utility.h"

#include "AppLib/Resource/ResourceModel.h"

#include "DataDefine/CharaDef.h"

static float sCrushAccel = -1.28f;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyFresbelk::CharaEnemyFresbelk()
{
	mIsSkyDiveAttack= FALSE;
	mIsChangeModel	= FALSE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	mRushVecZ	= ZERO_VEC;
	mRushCrossX	= ZERO_VEC;
	mEffHndSmoke= -1;

	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	mFlinchMax = 250;

	if (mWeaponModel.GetModelHandle() == -1) {
		mWeaponModel.LinkModel(RES_MDL_LANCE);
		mWeaponModel.Setup();
	}
	this->AttachModel(&mWeaponModel,
					  ModelPmd::eATTACHED_MODEL_NORMAL,
					  this->GetFrameIndexFromName("右中指１"));
	GraphicManager::GetInstance()->EntryDrawModel(&mWeaponModel);

	ResourceModel::GetInstance()->LoadCharaResource(NAME_FresbelkFly);

	mConstGravitySpeed = kGRAVITY_SPEED;
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyFresbelk::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	float dist = VSize(vec);
	
	int rand;
	bool result = false;

	/***/
	//	*pAttackCharaState = eCHARA_STATE_ATTACK_6;
	//	return true;
	/***/

	if (dist < mBodySize * 1.5f) {
		rand = GetRand(6);

		if (p_player->GetIsAttack()) {
			// 回避へ
			*pAttackCharaState = eCHARA_STATE_ATTACK_4;
			result = true;
		}
		else if (rand == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
			result = true;
		}
		else if (rand == 1) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_2;
			result = true;
		}
		else if (rand == 2) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_3;
			result = true;
		}
	}
	else if (dist < (mBodySize * 8.0f)) {
		rand = GetRand(50);
		if ((rand >= 0) && (rand <= 1)) {
			mIsSkyDiveAttack = FALSE;
			*pAttackCharaState = eCHARA_STATE_JUMP_UP;
			result = true;
		}
		else if ((rand >= 2) && (rand <= 3)) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_2;
			result = true;
		}
	}
	else if ((dist > (mBodySize * 4.0f)) && (dist < (mBodySize * 15.0f))) {
		if (mIsSkyDiveAttack == FALSE) {
			rand = GetRand(100);
			if ((rand >= 0)) {
				mIsSkyDiveAttack = TRUE;
				*pAttackCharaState = eCHARA_STATE_JUMP_UP;
				result = true;
			}
		}
	}

	if (result == true) {
		if (*pAttackCharaState != eCHARA_STATE_JUMP_UP) {
			mIsSkyDiveAttack = FALSE;
		}
	}

	return result;
}


//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseStateWaitAttack(void)
{
	switch (mReserveCharaState) {
	case eCHARA_STATE_JUMP_UP:
		ChangeState(mReserveCharaState);
		mReserveCharaState = -1;
		break;

	default:
		if (GetRand(10) == 0) {
			ChangeState(mReserveCharaState);
			mReserveCharaState = -1;
		}
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
CharaEnemyFresbelk::Update(void)
{
	CharaEnemy::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	if (mIsChangeModel == TRUE) {
		const tEnemyWork* p_work =  GetEnemyWorkTable(eENEMY_KIND_FRESBELK);
		this->DeleteModel();
		this->LinkModel(NAME_Fresbelk);
		int now_hp = mHitpoint;
		this->Setup(NAME_Fresbelk, mMaxHitpoint, p_work->mEnemyName);
		mHitpoint = now_hp;
		this->StartMotion(0, true, 0.5f);
		this->SetPositionOnCollision();
		mIsChangeModel = FALSE;
	}

	CharaEnemy::ProcessDamage(attackPower, isSetMode, pEffPos, pDamageDownVec, isDamageDown, attackType);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseEnterStateIdle(void)
{
	// 煙エフェクトの処理
	if (mEffHndSmoke != -1) {
		EffectManager::GetInstance()->Stop(mEffHndSmoke, 0.0f);
		mEffHndSmoke = -1;
	}

	// 通常の重力に戻す
	mConstGravitySpeed = kGRAVITY_SPEED;

	CharaEnemy::phaseEnterStateIdle();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseEnterStateJumpEndStandard(void)
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
	
	// 通常の重力に戻す
	mConstGravitySpeed = kGRAVITY_SPEED;

	StartMotion(eMT_JUMP_END, false, 0.75f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseEnterStateJumpEndDarts(void)
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
	
	mGravity = 0.0f;
	mMoveVec = ZERO_VEC;
	mJumpMoveVec.mDash = ZERO_VEC;

	// 通常の重力に戻す
	mConstGravitySpeed = kGRAVITY_SPEED;

	StartMotion(eMT_FRESBELK_ATTACK_5, false, 0.75f, NULL, 1.0f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseStateJumpEndStandard(void)
{
	CharaEnemy::phaseStateJumpEnd();

	/*
	mJumpMoveVec.mLanding = VScale(mJumpMoveVec.mLanding, 0.95f);
	mMoveVec = mJumpMoveVec.mLanding;

	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}	
	*/
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseStateJumpEndDarts(void)
{
	static float sWaitCount = 0.0f;

	//	mJumpMoveVec.mLanding = VScale(mJumpMoveVec.mLanding, 0.95f);
	//	mMoveVec = mJumpMoveVec.mLanding;

	//	if (GetIsEndMotion()) {
		//		ChangeState(eCHARA_STATE_IDLE);
	//	}	
	sWaitCount += 1.0f;
	if (sWaitCount > 40.0f) {

		sWaitCount = 0.0f;
		mPhaseState[eCHARA_STATE_JUMP_END]		= (CharaEnemy::PHASE_STATE)&CharaEnemyFresbelk::phaseStateJumpEndStandard;
		mPhaseEnterState[eCHARA_STATE_JUMP_END] = (CharaEnemy::PHASE_STATE)&CharaEnemyFresbelk::phaseEnterStateJumpEndStandard;
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
 * 標準の関数だとプレイヤーの方に向いてしまう為
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseEnterStateJumpDown(void)
{
	StartMotionEx(&sMotionParam[eMT_JUMP_DOING]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseEnterStateJumpUp(void)
{
	StartMotionEx(&sMotionParam[eMT_JUMP_START]);

	mIsReserveComboInput = false;
	mIsAttack = false;
	mIsDamage = false;

	// ジャンプ力高めに設定
	mGravity += (kJUMP_GRAVITY * 1.1f);

	mIsLand = false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseStateJumpUp(void)
{
	CharaEnemy::phaseStateJumpUp();

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (mGravity < 0) {
		SetRotationToTarget(&p_player->Position());
		if (mIsSkyDiveAttack == TRUE) {
			ChangeState(eCHARA_STATE_ATTACK_6);
		}
		else {
			ChangeState(eCHARA_STATE_ATTACK_5);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseEnterStateAttack1(void)
{
	VECTOR weapon_top_pos, weapon_btm_pos;
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣尾");
	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_FRESBELK_ATTACK_1, false, 0.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseStateAttack1(void)
{
	static float sStart = 15.0f;
	static float sEnd = 23.0f;

	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	if (this->EqualPhaseFrame(sStart)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}
	if (this->EqualPhaseFrame(sEnd)) {
		const float size = 56.0f;
		VECTOR effpos;
		effpos = VAdd(mPosition, mMoveVec);
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
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, false, eATTACK_TYPE_ENEMY_NORMAL);
			}
		}

	} else {
		mIsAttack = false;
	}
	PRINT_SCREEN(GetColor(255, 255, 0), "NowTime : %.2f\n", GetMotionNowTime());

	VECTOR weapon_top_pos, weapon_btm_pos;
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣尾");
	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");
	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {
		if (GetRand(1) == 0) {
			mBladeEffect.Stop();
			ChangeState(eCHARA_STATE_IDLE);
		} else {
			mBladeEffect.Stop();
			ChangeState(eCHARA_STATE_ATTACK_2);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseEnterStateAttack2(void)
{

	VECTOR weapon_top_pos, weapon_btm_pos;
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣尾");
	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_FRESBELK_ATTACK_2, false, 0.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseStateAttack2(void)
{
	static float sStart = 3.0f;
	static float sEnd = 18.0f;

	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	if (this->EqualPhaseFrame(sStart)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}

	float move_speed = 0.24f * (100 - GetMotionTimePercent());
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

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
			
				int attack_power = 2;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), mPosition);
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, false, eATTACK_TYPE_ENEMY_NORMAL);
			}
		}
	} 
	else {
		mIsAttack = false;
	}
	PRINT_SCREEN(GetColor(255, 255, 0), "NowTime : %.2f\n", GetMotionNowTime());

	VECTOR weapon_top_pos, weapon_btm_pos;
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣尾");
	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");
	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {
		if (GetRand(1) == 0) {
			mBladeEffect.Stop();
			ChangeState(eCHARA_STATE_IDLE);
		} else {
			mBladeEffect.Stop();
			ChangeState(eCHARA_STATE_ATTACK_3);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseEnterStateAttack3(void)
{
	VECTOR weapon_top_pos, weapon_btm_pos;
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣尾");
	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_FRESBELK_ATTACK_3, false, 0.4f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseStateAttack3(void)
{
	static float sStart = 3.0f;
	static float sEnd = 25.0f;

	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	if (this->EqualPhaseFrame(sStart)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
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

	VECTOR weapon_top_pos, weapon_btm_pos;
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣尾");
	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");
	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {
		mBladeEffect.Stop();
		if (GetRand(3) == 0) {
			ChangeState(eCHARA_STATE_ATTACK_1);
		} else {
			ChangeState(eCHARA_STATE_IDLE);
		}
	}
}

//---------------------------------------------------
/**
 * 回避
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseEnterStateAttack4(void)
{
	StartMotion(eMT_MONK_AWAY, false, 0.75f, NULL);

	mBladeEffect.Stop();

	SoundManager::GetSndComponent()->PlaySound(eSE_AWAY);
}

//---------------------------------------------------
/**
 * 回避
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseStateAttack4(void)
{
	VECTOR dir_vec;
	float move_speed = kCHARA_AWAY_SPEED * (float)(100 - GetMotionTimePercent());

	dir_vec.x = sin(mRotation.y + Utility::DegToRad(0.0f));
	dir_vec.y = 0.0f;
	dir_vec.z = cos(mRotation.y + Utility::DegToRad(0.0f));

	mMoveVec = VScale(dir_vec, move_speed);
	

	if (GetIsEndMotion()) {
		if (GetRand(0)) {
			ChangeState(eCHARA_STATE_ATTACK_2);
		}
		else {
			mIsSkyDiveAttack = TRUE;
			ChangeState(eCHARA_STATE_JUMP_UP);
		}

		/*
		int rand;
		rand = GetRand(6);
		if (rand >= 5) {
			// 突きへ
			ChangeState(eCHARA_STATE_ATTACK_2);
		} else {
			ChangeState(eCHARA_STATE_IDLE);
		}
		*/
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseEnterStateAttack5(void)
{
	// 落下速度を速める
	mConstGravitySpeed = (kGRAVITY_SPEED * 2.0f);

	CharaBase* p_player;
	p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	float time, height, gravity;
	float a, b, c;
	bool result;
	height = p_player->Position().y - this->mPosition.y;
	gravity = mConstGravitySpeed;
	a = 1;		b = 0;		c = (-2 * height) / gravity;
	result = Utility::GetFormulaOfTheSolution(&time, a, b, c);

	if (result == false) {
		ChangeState(eCHARA_STATE_JUMP_DOWN);
		return;
	}

	VECTOR tmpA, tmpB, vec;
	tmpA = p_player->Position();
	tmpA.y = 0.0f;
	tmpB = this->mPosition;
	tmpB.y = 0.0f;
	vec = VSub(tmpA, tmpB);
	float dist = VSize(vec);
	float speed = dist / time;
	speed *= 1.2f; // ちょっとだけ増やす
	
	mJumpMoveVec.mDash = VScale(VNorm(vec), speed);

	mPhaseState[eCHARA_STATE_JUMP_END]		= (CharaEnemy::PHASE_STATE)&CharaEnemyFresbelk::phaseStateJumpEndDarts;
	mPhaseEnterState[eCHARA_STATE_JUMP_END] = (CharaEnemy::PHASE_STATE)&CharaEnemyFresbelk::phaseEnterStateJumpEndDarts;

	SoundManager::GetSndComponent()->PlaySound(eSE_ROLLING);

	StartMotion(eMT_FRESBELK_ATTACK_5, false, 0.5f); 
}

//---------------------------------------------------
/**
 * 突進
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseStateAttack5(void)
{
	mMoveVec = mJumpMoveVec.mDash;

	mIsAttack = true;

	/****/
	CharaBase* p_player;
	p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	/* 戦闘不能者と既に攻撃された者を除く */
	if ((p_player->GetIsRetire() == false) &&
		(p_player->IsDamage() == false)) {

		bool is_damage = false;

		ShapeCapsule capsule;
		ShapeLine line;
		/*
		capsule.mRadius = p_player->BodySize() * 0.8f;
		capsule.mPosA = p_player->Position();
		capsule.mPosA.y += capsule.mRadius;
		capsule.mPosB = p_player->Position();
		capsule.mPosB.y += p_player->BodyHeight() - capsule.mRadius;
		*/
		p_player->GetBodyCapsule(&capsule);

		mWeaponModel.GetFramePosFromName(&line.mStart, "剣先");
		mWeaponModel.GetFramePosFromName(&line.mEnd, "剣尾");
		
		if (CollisionManager::Check_LineToCapsule(&line, &capsule)) {
			is_damage = true;
		} 
		else {
			CollisionManager::tSphereWork sphere;
			sphere.mPosition	= VAdd(mPosition, mMoveVec);
			sphere.mSize		= mBodySize * 1.1f;

			VECTOR vec;
			float dist;
			if (p_player->CheckBodyCollision(sphere, vec, dist)) {
				is_damage = true;
			}
		}

		if (is_damage) {
			SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			int attack_power = 4;
			VECTOR damage_down_vec;
			damage_down_vec = VSub(p_player->Position(), mPosition);
			//			damage_down_vec = VCross(VGet(0.0f, 1.0f, 0.0f), damage_down_vec);
			damage_down_vec = VNorm(damage_down_vec);
			p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
		}
	}

	PRINT_SCREEN(GetColor(255, 255, 0), "NowTime : %.2f\n", GetMotionNowTime());

	if (GetIsEndMotion() || mIsLand == true) {
		
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
CharaEnemyFresbelk::phaseEnterStateAttack6(void)
{
	const tEnemyWork* p_work =  GetEnemyWorkTable(eENEMY_KIND_FRESBELK);

	this->DeleteModel();
	this->LinkModel(NAME_FresbelkFly);
	int now_hp = mHitpoint;
	this->Setup(NAME_FresbelkFly, mMaxHitpoint, p_work->mEnemyName);
	mHitpoint = now_hp;
	this->StartMotion(0, true, 0.5f);
	mIsChangeModel = TRUE;

	mCharaState = eCHARA_STATE_ATTACK_6;

	mPrevPosition = ZERO_VEC;
	updateCrushParameter();
	mRollingCrushCnt = 0;

	VECTOR eff_pos;
	eff_pos = mPosition;
	EFK_HANDLE efk_handle = NULL;
	efk_handle = EffekseerManager::GetInstance()->Play(eEFK_BOSS_DEATH, &eff_pos, 6.5f, 16.0f);
	EffekseerManager::GetInstance()->SetSpeed(efk_handle, 1.2f);

	SoundManager::GetSndComponent()->PlaySound(eSE_CANNON_LASER);

	mConstGravitySpeed = 0.0f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseStateAttack6(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

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

	mAccel += sCrushAccel;

	float dist_y = p_player->Position().y - mPosition.y;
	if (abs(dist_y) > 20.0f) {
		dist_y *= 0.05f;
		mMoveVec.y += dist_y;
	}

	mRotation.y = atan2(mMoveVec.x, mMoveVec.z) + PHI_F;



	/* 戦闘不能者と既に攻撃された者を除く */
	if (p_player->CheckEnableAttack() == true) {

		CollisionManager::tSphereWork sphere;
		sphere.mPosition	= VAdd(mPosition, mMoveVec);
		sphere.mSize		= mBodySize * 1.3f;

		VECTOR vec;
		float dist;
		if (p_player->CheckBodyCollision(sphere, vec, dist)) {
			VECTOR damage_down_vec = VNorm(VSub(p_player->Position(), mPosition));
			p_player->ProcessDamage(4, false, NULL, &damage_down_vec, true, eATTACK_TYPE_BREAK_GUARD);
			SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
		}
	}

	if (mRollingCrushCnt > 1) {

		// 煙エフェクトの処理
		if (mEffHndSmoke != -1) {
			EffectManager::GetInstance()->Stop(mEffHndSmoke, 0.0f);
			mEffHndSmoke = -1;
		}

		const tEnemyWork* p_work =  GetEnemyWorkTable(eENEMY_KIND_FRESBELK);
		this->DeleteModel();
		this->LinkModel(NAME_Fresbelk);
		int now_hp = mHitpoint;
		this->Setup(NAME_Fresbelk, mMaxHitpoint, p_work->mEnemyName);
		mHitpoint = now_hp;
		this->StartMotion(0, true, 0.5f);
		this->SetPositionOnCollision();
		mIsChangeModel = FALSE;
	
		mRollingCrushCnt = 0;

		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST, &mPosition, 100.0f, 0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 2.0f);
		CameraManager::GetInstance()->StartVibeCamera(1.0f, 8.0f, 10.0f);
		SoundManager::GetSndComponent()->PlaySound(eSE_BURST);

		ChangeState(eCHARA_STATE_IDLE);

	}
	else {
		float maxv, nowv;
		maxv = VSize(mMoveVec);
		nowv = VSize(VSub(mPosition, mPrevPosition));

		//		if (VSize(VSub(mPosition, mPrevPosition) ) == 0.0f ) {
		if ((maxv * 0.5f) > nowv) {

			mPrevPosition = ZERO_VEC;
			updateCrushParameter();
			mRollingCrushCnt ++;
		}
	}

	// 煙エフェクトの処理
	if (mEffHndSmoke != -1) {
		VECTOR effpos = mPosition;
		effpos.y += (mBodyHeight * 0.35f);
		EffectManager::GetInstance()->Position(mEffHndSmoke) = effpos;
		EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "particleSpeed", 0.5f);
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::phaseEnterStateRetireEnd(void)
{
	GraphicManager::GetInstance()->ReserveRemoveModel(&mWeaponModel);
	mWeaponModel.SetAlpha(0.0f);
	mWeaponModel.DeleteModel();

	CharaEnemy::phaseEnterStateRetireEnd();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyFresbelk::updateCrushParameter(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	bool  result = false;
	//	VECTOR vec = VSub(p_player->Position(), mPosition);

	VECTOR pos_a, pos_b, vec;
	pos_b = p_player->Position();
	pos_b.y = 0.0f;
	pos_a = mPosition;
	pos_a.y = 0.0f;
	vec = VSub(pos_b, pos_a);

	float dist = VSize(vec);
	float a, b, c;
	float time;
	//	float accel = kGRAVITY_SPEED;
	float accel = sCrushAccel;

	mAccel	= 0.0f;
	mVy0	= 30.0f;

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


 /**** end of file ****/
