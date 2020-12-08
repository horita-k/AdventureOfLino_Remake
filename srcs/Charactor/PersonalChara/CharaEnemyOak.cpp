/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ギギネブラファイル
 * @author SPATZ.
 * @data   2012/05/25 00:27:32
 */
//---------------------------------------------------
#include "../CharaPlayer.h"
#include "CharaEnemyOak.h"
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

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyOak::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	if (mWeaponModel.GetModelHandle() == -1) {
		mWeaponModel.LinkModel(RES_MDL_SHIELD);
		mWeaponModel.Setup();
	}
	this->AttachModel(&mWeaponModel,
					  ModelPmd::eATTACHED_MODEL_NORMAL,
					  this->GetFrameIndexFromName("右手首"));
	GraphicManager::GetInstance()->EntryDrawModel(&mWeaponModel);

	mDamageCount = 0;
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyOak::checkChangeAttackState(int* pAttackCharaState)
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
CharaEnemyOak::processNoMoveAction(void)
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
CharaEnemyOak::Update(void)
{
	CharaEnemy::Update();

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	if ((VSize(p_player->MoveVec()) > 1.0f) && 
		(p_player->IsDamage() == false) &&
		(GetIsMotionBlend() == false) &&
		((mCharaState == eCHARA_STATE_IDLE) ||
		 (mCharaState == eCHARA_STATE_TURN) ||
		 (mCharaState == eCHARA_STATE_ATTACK_1) ||
		 (mCharaState == eCHARA_STATE_WAIT_ATTACK) ||
		 (mCharaState == eCHARA_STATE_MOVE)) ) {
		ChangeState(eCHARA_STATE_IDLE);
		this->SetRotationToTarget(&p_player->Position());
		mIsDamage = false;
		mDamageCount = 0;
	}

	if (mDamageCount > 0) {
		mDamageCount --;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyOak::Destroy(void)
{
	CharaEnemy::Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyOak::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	bool is_guard = false;
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	//	APP_ASSERT( (pEffPos != NULL), "pEffPos is NULL");
	if (pEffPos == NULL) {
		pEffPos = &mPosition;
	}

	if ((mDamageCount == 0) &&
		(attackPower != kSYSTEM_DAMAGE_VALUE)) {

		switch (attackType) {
		case eATTACK_TYPE_BOOMERANG:
			// ブーメランの場合
			{
				tBoomerangWork* p_boomerang;
				p_boomerang = ((CharaPlayer *)p_player)->GetBoomerangWork();
				if ((p_boomerang->mState == eBOOMERANG_STATE_FOWARD_NO_DAMAGE) ||
					(p_boomerang->mState == eBOOMERANG_STATE_FOWARD_DAMAGED)) {
					// 盾がブーメランで跳ね返る
					p_boomerang->mState = eBOOMERANG_STATE_REVERSE_NO_DAMAGE;
					mIsDamage = true;
					is_guard = true;
				} 
				else if (p_boomerang->mState == eBOOMERANG_STATE_REVERSE_DAMAGED) {
					is_guard = false;
				}
				else {
					is_guard = true;
				}
			}
			break;

		case eATTACK_TYPE_BOMB:
			// 爆弾の場合
			{
				// 敵の向きと爆弾との位置との内積を求め、背後からの攻撃かを判定する
				VECTOR enemy_vec;
				enemy_vec.x = sin(mRotation.y + PHI_F);
				enemy_vec.y = 0.0f;
				enemy_vec.z = cos(mRotation.y + PHI_F);

				float dot_value = 0.0f;
				dot_value = VDot(enemy_vec, *pDamageDownVec);
				PRINT_CONSOLE("dot_value : %.2f \n", dot_value);
				if (dot_value > 0.0f) {
					is_guard = false;
				}
				else {
					is_guard = true;
				}
			}
			break;

		default:
			is_guard = true;
			break;
		}

		// ガード時の処理
		if (is_guard == true) {
			COLOR_F color;
			color.r = 10.0f;
			color.g = 10.0f;
			color.b = 20.0f;
			color.a = 10.0f;
			this->SetDiffuse(color);
			this->SetSpecular(color);

			EFK_HANDLE handle;
			handle = EffekseerManager::GetInstance()->Play(eEFK_SHIELD, pEffPos, &VScale(mScale, 0.8f));
			EffekseerManager::GetInstance()->SetRotationAxisY(handle, (p_player->Rotation().y + (PHI_F/2.0f)));

			SoundManager::GetSndComponent()->PlaySound(eSE_SHIELD);
			mIsDamage = true;
		}
		else {
			// 攻撃ヒット
			mDamageCount = 200;
			CharaEnemy::ProcessDamage(attackPower, isSetMode, pEffPos, pDamageDownVec, isDamageDown, attackType);
		}
	} 
	else {
		CharaEnemy::ProcessDamage(attackPower, isSetMode, pEffPos, pDamageDownVec, isDamageDown, attackType);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyOak::phaseEnterStateAttack1(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "右手首");
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣先");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_KNIGHT_ATTACK_1, false, 0.6f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyOak::phaseStateAttack1(void)
{
	static float sStart = 12.0f;
	static float sEnd = 20.0f;

	bool is_prev_attack;
	is_prev_attack = mIsAttack;
	if ((GetMotionNowTime() >= sStart) &&
		(GetMotionNowTime() <= sEnd)) {
		mIsAttack = true;

		CharaBase* p_player;
		p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

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
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
			}
		}

	} else {
		mIsAttack = false;
	}
	PRINT_SCREEN(GetColor(255, 255, 0), "NowTime : %.2f\n", GetMotionNowTime());

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "右手首");
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "剣先");
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
CharaEnemyOak::phaseEnterStateDamage(void)
{
	mIsAttack = false;
	//	StartMotionEx(&sMotionParam[eMT_DAMAGE]);
	StartMotion(eMT_DAMAGE, false, 2.00f);
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_DAMAGE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyOak::phaseStateDamage(void)
{
	if (mDamageCount == 0) {
		ChangeState(eCHARA_STATE_IDLE);
	}

	/*
	if (GetMotionIndex() != eMT_DAMAGE) {
		//		mIsDamage = false;
		ChangeState(eCHARA_STATE_IDLE);
	}
	*/
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyOak::phaseEnterStateRetireEnd(void)
{
	GraphicManager::GetInstance()->ReserveRemoveModel(&mWeaponModel);

	mWeaponModel.SetAlpha(0.0f);
	mWeaponModel.DeleteModel();

	CharaEnemy::phaseEnterStateRetireEnd();
}

 /**** end of file ****/
