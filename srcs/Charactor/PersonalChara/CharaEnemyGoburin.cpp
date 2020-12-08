/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ギギネブラファイル
 * @author SPATZ.
 * @data   2012/05/25 00:27:32
 */
//---------------------------------------------------
#include "CharaEnemyGoburin.h"
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
CharaEnemyGoburin::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	mFlinchMax = (SHORT)(((mScale.x / GOBLIN_SCALE) - 1.0f) * 30.0f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*
bool
CharaEnemyGoburin::CheckDamageCollNormal(void* pParam)
{
	tCheckDamageWork* p_work = (tCheckDamageWork *)pParam;

	ShapeCapsule capsule;
	ShapeLine	line;

	line.mStart = p_work->mWeaponBtmPos;
	line.mEnd	= p_work->mWeaponTopPos;

	for (int iI = 0; iI < kCAPSULE_NUM_GOBURIN; iI ++) {
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

	ShapeBoard board;
	this->GetFramePosFromName(&board.mPosA, "左ひじ");
	this->GetFramePosFromName(&board.mPosB, "左手首");
	this->GetFramePosFromName(&board.mPosC, "左足首");
	this->GetFramePosFromName(&board.mPosD, "左腕");
#ifdef _DEBUG
		GraphicManager::GetInstance()->DbgRenderBoard(&board);
#endif // _DEBUG
	if (CollisionManager::GetInstance()->Check_LineToBoard(&board, &line)) {
		return true;
	}
	this->GetFramePosFromName(&board.mPosA, "右ひじ");
	this->GetFramePosFromName(&board.mPosB, "右手首");
	this->GetFramePosFromName(&board.mPosC, "右足首");
	this->GetFramePosFromName(&board.mPosD, "右腕");
#ifdef _DEBUG
		GraphicManager::GetInstance()->DbgRenderBoard(&board);
#endif // _DEBUG
	if (CollisionManager::GetInstance()->Check_LineToBoard(&board, &line)) {
		return true;
	}

	return false;
}
*/

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyGoburin::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	if (VSize(vec) < mBodySize + (mBodySize * (GetRand(200) * 0.020f))) {

		*pAttackCharaState = eCHARA_STATE_ATTACK_1;
		return true;
	}
	return false;
}

/*=====================================*
 * extend
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyGoburin::phaseEnterStateAttack1(void)
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
CharaEnemyGoburin::phaseStateAttack1(void)
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

 /**** end of file ****/
