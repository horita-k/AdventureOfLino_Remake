/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ギギネブラファイル
 * @author SPATZ.
 * @data   2012/05/25 00:27:32
 */
//---------------------------------------------------
#include "CharaEnemyStrategist.h"
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
void
CharaEnemyStrategist::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	mFlinchMax = 150;
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyStrategist::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	float dist = VSize(vec);
	
	int rand;
	bool result = false;

	if (dist < mBodySize * 1.5f) {
		//		float rate = mBodySize / dist;
		//		rand = GetRand((int)((1.0f - rate) * 30.0f) + 1);
		rand = GetRand(6);

		/*
		if (rand > 2) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_3;
			result = true;
		} else if (rand == 2) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_2;
			result = true;
		} else if (rand == 3) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
			result = true;
		}
		*/
		if (rand == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
			result = true;
		} else if (rand == 1) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_2;
			result = true;
		} else if (rand == 2) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_3;
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
CharaEnemyStrategist::Update(void)
{
	CharaEnemy::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyStrategist::phaseEnterStateAttack1(void)
{
	//	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "右手首");
	this->GetFramePosFromName(&weapon_btm_pos, "剣先");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_STRATEGIST_ATTACK_1, false, 0.6f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyStrategist::phaseStateAttack1(void)
{
	static float sStart = 3.0f;
	static float sEnd = 20.0f;

	bool is_prev_attack;
	is_prev_attack = mIsAttack;
	if ((GetMotionNowTime() >= sStart) &&
		(GetMotionNowTime() <= sEnd)) {
		mIsAttack = true;

		CharaBase* p_player;
		p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		/*
		if (p_player->GetIsRetire()) return;		// 戦闘不能者も除く
		if (p_player->IsDamage()) return;			// 既に攻撃された者も除く
		*/
		/* 戦闘不能者と既に攻撃された者を除く */
		if ((p_player->GetIsRetire() == false) &&
			(p_player->IsDamage() == false)) {

			// 敵のノードとキャラの円柱の当たり判定
			CollisionManager::tSphereWork sphere;
			CollisionManager::tPillarWork piller;
			this->GetFramePosFromName(&sphere.mPosition, "右手首");
			sphere.mSize		= 35.0f;

			VECTOR tmpA, tmpB;
			tmpA = sphere.mPosition;
			tmpA.y += sphere.mSize * 0.5f;
			tmpB = sphere.mPosition;
			tmpB.y += -sphere.mSize * 0.5f;
#ifdef _DEBUG
			GraphicManager::GetInstance()->DbgRenderLine(&tmpA, &tmpB);
#endif // #ifdef _DEBUG
			piller.mPosition	= p_player->Position();
			piller.mSize		= p_player->BodySize();
			piller.mHeight		= p_player->BodyHeight();

			if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			
				int attack_power = 1;
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
	this->GetFramePosFromName(&weapon_top_pos, "右手首");
	this->GetFramePosFromName(&weapon_btm_pos, "剣先");
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
CharaEnemyStrategist::phaseEnterStateAttack2(void)
{
	//	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_2);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "右手首");
	this->GetFramePosFromName(&weapon_btm_pos, "剣先");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_STRATEGIST_ATTACK_2, false, 0.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyStrategist::phaseStateAttack2(void)
{
	static float sStart = 3.0f;
	static float sEnd = 20.0f;

	bool is_prev_attack;
	is_prev_attack = mIsAttack;
	if ((GetMotionNowTime() >= sStart) &&
		(GetMotionNowTime() <= sEnd)) {
		mIsAttack = true;

		CharaBase* p_player;
		p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		/*
		if (p_player->GetIsRetire()) return;		// 戦闘不能者も除く
		if (p_player->IsDamage()) return;			// 既に攻撃された者も除く
		*/

		/* 戦闘不能者と既に攻撃された者を除く */
		if ((p_player->GetIsRetire() == false) &&
			(p_player->IsDamage() == false)) {

			// 敵のノードとキャラの円柱の当たり判定
			CollisionManager::tSphereWork sphere;
			CollisionManager::tPillarWork piller;
			this->GetFramePosFromName(&sphere.mPosition, "右手首");
			sphere.mSize		= 35.0f;

			VECTOR tmpA, tmpB;
			tmpA = sphere.mPosition;
			tmpA.y += sphere.mSize * 0.5f;
			tmpB = sphere.mPosition;
			tmpB.y += -sphere.mSize * 0.5f;
#ifdef _DEBUG
			GraphicManager::GetInstance()->DbgRenderLine(&tmpA, &tmpB);
#endif // #ifdef _DEBUG

			piller.mPosition	= p_player->Position();
			piller.mSize		= p_player->BodySize();
			piller.mHeight		= p_player->BodyHeight();

			if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			
				int attack_power = 1;
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
	this->GetFramePosFromName(&weapon_top_pos, "右手首");
	this->GetFramePosFromName(&weapon_btm_pos, "剣先");
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
CharaEnemyStrategist::phaseEnterStateAttack3(void)
{
	//	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_3);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "右手首");
	this->GetFramePosFromName(&weapon_btm_pos, "剣先");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_STRATEGIST_ATTACK_3, false, 0.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyStrategist::phaseStateAttack3(void)
{
	static float sStart = 3.0f;
	static float sEnd = 20.0f;

	bool is_prev_attack;
	is_prev_attack = mIsAttack;
	if ((GetMotionNowTime() >= sStart) &&
		(GetMotionNowTime() <= sEnd)) {
		mIsAttack = true;

		CharaBase* p_player;
		p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		/*
		if (p_player->GetIsRetire()) return;		// 戦闘不能者も除く
		if (p_player->IsDamage()) return;			// 既に攻撃された者も除く
		*/

		/* 戦闘不能者と既に攻撃された者を除く */
		if ((p_player->GetIsRetire() == false) &&
			(p_player->IsDamage() == false)) {

			// 敵のノードとキャラの円柱の当たり判定
			CollisionManager::tSphereWork sphere;
			CollisionManager::tPillarWork piller;
			this->GetFramePosFromName(&sphere.mPosition, "右手首");
			sphere.mSize		= 35.0f;

			VECTOR tmpA, tmpB;
			tmpA = sphere.mPosition;
			tmpA.y += sphere.mSize * 0.5f;
			tmpB = sphere.mPosition;
			tmpB.y += -sphere.mSize * 0.5f;
#ifdef _DEBUG
			GraphicManager::GetInstance()->DbgRenderLine(&tmpA, &tmpB);
#endif // #ifdef _DEBUG

			piller.mPosition	= p_player->Position();
			piller.mSize		= p_player->BodySize();
			piller.mHeight		= p_player->BodyHeight();

			if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			
				int attack_power = 1;
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
	this->GetFramePosFromName(&weapon_top_pos, "右手首");
	this->GetFramePosFromName(&weapon_btm_pos, "剣先");
	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {
		mBladeEffect.Stop();
		ChangeState(eCHARA_STATE_IDLE);
	}
}

 /**** end of file ****/
