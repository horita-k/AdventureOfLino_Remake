/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ギギネブラファイル
 * @author SPATZ.
 * @data   2012/05/25 00:27:32
 */
//---------------------------------------------------
#include "CharaEnemyMagician.h"
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
CharaEnemyMagician::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	if (mWeaponModel.GetModelHandle() == -1) {
		mWeaponModel.LinkModel(RES_MDL_STAFF);
		mWeaponModel.Setup();
	}
	this->AttachModel(&mWeaponModel,
					  ModelPmd::eATTACHED_MODEL_NORMAL,
					  this->GetFrameIndexFromName("右手首"));
	GraphicManager::GetInstance()->EntryDrawModel(&mWeaponModel);

	mEfkHandle = NULL;
	mEfsHandle = -1;

	mStatePhase = 0;
	mStateCount = 0;
	sThunderVec = ZERO_VEC;
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyMagician::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	float dist = VSize(vec);
	
	int rand;
	bool result = false;

	//	if (dist < mBodySize * 1.5f) {
	if (dist < mBodySize * 16.0f) {

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

/*=====================================*
 * extend
 *=====================================*/

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMagician::Update(void)
{
	CharaEnemy::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMagician::Destroy(void)
{
	CharaEnemy::Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMagician::phaseEnterStateAttack1(void)
{
	StartMotion(eMT_KNIGHT_ATTACK_1, false, 0.6f);
	SoundManager::GetSndComponent()->PlaySound(eSE_MAGIC);

	VECTOR eff_pos;
	// 魔方陣
	eff_pos = mPosition;
	mEfkHandle = EffekseerManager::GetInstance()->Play(eEFK_MAGIC_SQUARE, &eff_pos, &VScale(mScale, 2.0f));

	if (mEfsHandle != -1) {
		EffectManager::GetInstance()->Stop(mEfsHandle, 0.0f);
		mEfsHandle = -1;
	}

	mStatePhase = 0;
	mStateCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMagician::phaseStateAttack1(void)
{
	static float sAccel = 0.0f;
	//	static const float sThrowTime = 45.0f;
	static const float sThrowTime = 90.0f;
	static const float size = 100.0f;
	VECTOR eff_vec = ZERO_VEC;
	int* pEfsHandle = NULL;

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	VECTOR eff_pos;
	VECTOR next_pos;
	VECTOR weapon_top_pos;
	VECTOR hitpos, normal;

	mWeaponModel.GetFramePosFromName(&weapon_top_pos, "剣先");

	switch (mStatePhase) {
	case 0:
		SetRotationToTarget(&p_player->Position());

		if (this->EqualPhaseFrame(5.0f)) {
			this->PauseMotion();
			// 波動
			eff_pos = weapon_top_pos;
			mEfsHandle = EffectManager::GetInstance()->Play(eEF_ENERGY_BALL,
															&eff_pos,
															&VGet(size, size, size),
															1.0f);
			mStatePhase ++;mStateCount = 0;
		}
		break;

	case 1:
		EffectManager::GetInstance()->Position(mEfsHandle) = weapon_top_pos;

		SetRotationToTarget(&p_player->Position());
		if (mStateCount >= 60) {
			SoundManager::GetSndComponent()->PlaySound(eSE_DARK);
			this->ResumeMotion();
			mStatePhase ++;mStateCount = 0;
		}
		break;

	case 2:
		SetRotationToTarget(&p_player->Position());

		EffectManager::GetInstance()->Position(mEfsHandle) = weapon_top_pos;
		if (mStateCount > 1) {
			VECTOR tar_pos;
			VECTOR vec;
			tar_pos = VAdd(p_player->Position(), p_player->MoveVec());
			tar_pos.y += (p_player->BodyHeight() * 0.3f);
			vec = VSub(tar_pos, weapon_top_pos);
			//<! キャラより奥へ投げる
			tar_pos = VAdd(weapon_top_pos, VScale(VNorm(vec), (VSize(vec) * 2.0f)));

			float vo;
			vo = 20.0f;
			sAccel = 0.4f;

			// 波動を投げる
			EffectManager::GetInstance()->Position(mEfsHandle) = weapon_top_pos;
			sThunderVec = VSub(tar_pos, weapon_top_pos);
			sThunderVec = VScale(VNorm(sThunderVec), vo);
			mStatePhase ++;mStateCount = 0;
		}
		break;

	case 3:
		if (this->EqualPhaseFrame(11.0f)) {
			this->PauseMotion();
		}
		/* 波動移動処理 */
		eff_pos = EffectManager::GetInstance()->Position(mEfsHandle);
		sThunderVec = VAdd(sThunderVec, VScale(VNorm(sThunderVec), sAccel)); // 加速度更新
		next_pos = VAdd(eff_pos, sThunderVec); // 座標更新
		EffectManager::GetInstance()->Position(mEfsHandle) = next_pos; // 反映

		/* 壁判定 */
		if ((mStateCount > sThrowTime) ||
			(CollisionManager::GetInstance()->CheckWallCollision(eff_pos, next_pos, &hitpos, &normal)) ||
			(CollisionManager::GetInstance()->CheckEarthCollision(eff_pos, next_pos, &hitpos, &normal) != NOT_ON_COLLISION)) {
			// でかい奴を停止させる
			EffectManager::GetInstance()->Stop(mEfsHandle, 0.5f);
			mStatePhase = 4; mStateCount = 0;
		}
		break;

	case 4:
		/**** 終了処理 ****/
		if (mEfsHandle != -1) {
			EffectManager::GetInstance()->Stop(mEfsHandle, 0.5f);
			mEfsHandle = -1;
		}
		ChangeState(eCHARA_STATE_IDLE);
		return;

	default:
		break;
	};

	/* 当たり判定処理 */
	if (p_player->IsDamage() == false) {

		if (mEfsHandle != -1) {
			float eff_scale;
			eff_pos = EffectManager::GetInstance()->Position(mEfsHandle);
			eff_scale = EffectManager::GetInstance()->Scale(mEfsHandle).x;
		
			ShapeCapsule capsuleA = ShapeCapsule(p_player->Position(), p_player->Position(), p_player->BodySize());
			ShapeCapsule capsuleB = ShapeCapsule(eff_pos, eff_pos, eff_scale);
			capsuleA.mPosB.y += p_player->BodyHeight();
			capsuleB.mPosB.y += 1.0f;
#ifdef _DEBUG
			GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				//				int attack_power = 3;
				int attack_power = 1;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), capsuleB.mPosA);
				damage_down_vec.y = 0.0f;
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, false, eATTACK_TYPE_ENEMY_NORMAL);
			}
		}
	}
	mStateCount ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMagician::phaseEnterStateRetireEnd(void)
{
	if (mEfsHandle != -1) {
		EffectManager::GetInstance()->Stop(mEfsHandle, 0.0f);
		mEfsHandle = -1;
	}

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
CharaEnemyMagician::phaseEnterStateDownStart(void)
{
	if (mEfsHandle != -1) {
		EffectManager::GetInstance()->Stop(mEfsHandle, 0.5f);
		mEfsHandle = -1;
	}
		
	//<!	GraphicManager::GetInstance()->RevertFogParam(0.1f);

	CharaEnemy::phaseEnterStateDownStart();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMagician::phaseEnterStateDamage(void)
{
	if (mEfsHandle != -1) {
		EffectManager::GetInstance()->Stop(mEfsHandle, 0.5f);
		mEfsHandle = -1;
	}
		
	//<!	GraphicManager::GetInstance()->RevertFogParam(0.1f);

	CharaEnemy::phaseEnterStateDamage();
}

 /**** end of file ****/
