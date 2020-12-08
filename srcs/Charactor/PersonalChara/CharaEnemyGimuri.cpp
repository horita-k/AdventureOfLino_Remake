/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ギムリファイル
 * @author SPATZ.
 * @data   2015/12/31 10:04:48
 */
//---------------------------------------------------
#include "../CharaPlayer.h"
#include "CharaEnemyGimuri.h"
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

static const char* strAttack01_Top = "左足首";
static const char* strAttack01_Btm = "左ひざ";

static const char* strAttack02_Top = "右足首";
static const char* strAttack02_Btm = "右ひざ";

static const char* strAttack03_Top = "左手首";
static const char* strAttack03_Btm = "左ひじ";

static const char* strAttack04_Top = "右手首";
static const char* strAttack04_Btm = "右ひじ";

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyGimuri::CharaEnemyGimuri()
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyGimuri::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	mEfkHandle	= NULL;
	mFlinchMax	= 250;
	mCannonCount= 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyGimuri::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	int prev_state = mCharaState;
	
	CharaEnemy::ProcessDamage(attackPower, isSetMode, pEffPos, pDamageDownVec, isDamageDown, attackType);
	
	// ダメージでステータスが変わったら
	if (prev_state != mCharaState) {
		if (mEfkHandle != NULL) {
			EffekseerManager::GetInstance()->Stop(mEfkHandle, 0.0f);
			mEfkHandle = NULL;
		}
	}
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyGimuri::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);

	int rand_value = GetRand(100);
	float rand_rate = (float)rand_value * 0.01f;
	float dist = VSize(vec);

	int rand = GetRand(6);

	if (dist < (mBodySize * 3.0f) + ((mBodySize * 1.0f) * rand_rate) ) {

		if (p_player->GetIsAttack()) {
			// 回避へ
			*pAttackCharaState = eCHARA_STATE_ATTACK_5;
			return true;
		}

		if (rand == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_3;
			return true;
		}
		else if (rand == 1) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_4;
			return true;
		}
	}
	else if (dist < (mBodySize * 8.0f) + ((mBodySize * 4.0f) * rand_rate) ) {
		if (rand_value >= 0 && rand_value < 10) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_6;
			return true;
		}
		if (rand_value >= 10 && rand_value < 20) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
			return true;
		}
	}
	else if (dist < (mBodySize * 16.0f) + ((mBodySize * 2.0f) * rand_rate) ) {
		if (GetRand(16) == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_2;
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
CharaEnemyGimuri::phaseStateWaitAttack(void)
{
	switch (mReserveCharaState) {
	case eCHARA_STATE_ATTACK_1:
	case eCHARA_STATE_ATTACK_2:
	case eCHARA_STATE_ATTACK_5:
	case eCHARA_STATE_ATTACK_6:
		ChangeState(mReserveCharaState);
		mReserveCharaState = -1;
		break;

	default:
		if (GetRand(12) == 0) {
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
CharaEnemyGimuri::processNoMoveAction(void)
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
CharaEnemyGimuri::phaseEnterStateIdle(void)
{
	CharaEnemy::phaseEnterStateIdle();

	mBladeEffect2nd.Stop();

	if (mEfkHandle != NULL) {
		EffekseerManager::GetInstance()->Stop(mEfkHandle);
		mEfkHandle = NULL;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyGimuri::phaseEnterStateMove(void)
{
	StartMotion(eMT_WALK, true, 0.6f, NULL, 1.0f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyGimuri::phaseEnterStateAttack1(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack04_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack04_Btm);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	mEfkHandle = EffekseerManager::GetInstance()->Play(eEFK_CANNON_LASER_CHARGE, &weapon_top_pos, 3.0f);
	EffekseerManager::GetInstance()->SetSpeed(mEfkHandle, 2.0f);

	StartMotion(eMT_EXTEND_ATTACK_1, false, 0.6f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyGimuri::phaseStateAttack1(void)
{
	static const float sStart		= 18.0f;
	static const float sEnd			= 50.0f;
	static const float sSphereSize	= 30.0f;
	
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack04_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack04_Btm);

	// エフェクト処理
	if (mEfkHandle != NULL) {
		if (EffekseerManager::GetInstance()->IsPlay(mEfkHandle)) {
			EffekseerManager::GetInstance()->SetPosition(mEfkHandle, &weapon_top_pos);
		}
		else {
			mEfkHandle = NULL;
		}
	}

	float move_speed = 0.0f;
	if (BetweenPhaseFrame(sStart, sEnd)) {

		move_speed = 0.12f * (100 - GetMotionTimePercent());

		/* 戦闘不能者と既に攻撃された者を除く */
		if (p_player->CheckEnableAttack() == true) {
		
			// 敵のノードとキャラの円柱の当たり判定
			CollisionManager::tPillarWork piller;
			p_player->GetBodyPillar(&piller);

			CollisionManager::tSphereWork sphere;
			this->GetBodyFrameSphere(&sphere, strAttack04_Top, sSphereSize);

			if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {

				EFK_HANDLE efk_handle = NULL;
				efk_handle = EffekseerManager::GetInstance()->Play(eEFK_BREAK_BARRIER, &sphere.mPosition, 6.0f);
				EffekseerManager::GetInstance()->SetSpeed(efk_handle, 1.0f);

				VECTOR damage_down_vec = VNorm(VSub(p_player->Position(), mPosition));
				p_player->ProcessDamage(2, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			}
		}

		mIsAttack = true;
	}
	else {
		move_speed = 0.05f * (100 - GetMotionTimePercent());

		mIsAttack = false;
	}

	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

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
CharaEnemyGimuri::phaseEnterStateAttack2(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack03_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack03_Btm);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	MATRIX weapon_top_mtx;
	GetFrameMatrixFromNameByScale(&weapon_top_mtx, strAttack03_Top, 0.5f);
	mEfkHandle = EffekseerManager::GetInstance()->PlayMtx(eEFK_CANNON_LASER_CHARGE, &weapon_top_mtx);
	EffekseerManager::GetInstance()->SetSpeed(mEfkHandle, 0.5f);

	SoundManager::GetSndComponent()->PlaySound(eSE_CANNON_LASER_CHARGE);

	StartMotion(eMT_EXTEND_ATTACK_2, false, 0.9f);

	mCannonCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyGimuri::phaseStateAttack2(void)
{
	static const float sStart		= 70.0f;
	static const float sEnd			= 87.0f;
	static const float sSphereSize	= 30.0f;
	
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack03_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack03_Btm);

	MATRIX node_mtx;
	VECTOR node_pos;

	GetFrameMatrixFromName(&node_mtx, "左手先");
	Utility::ExtractMatrixPosition(&node_pos, &node_mtx);
	
	MATRIX mtx;
	{
		static const float scale = 0.75f;

		MATRIX tmp;
		mtx = MGetIdent();
		// 拡縮行列
		tmp = MGetScale(VGet(scale, scale, scale) );
		mtx = MMult(mtx, tmp);
		tmp = MGetRotY(Utility::DegToRad(90.0f) );
		mtx = MMult(mtx, tmp);
		// 回転行列
		Utility::ExtractRotationMatrix(&tmp, &node_mtx, 1.0f);
		mtx = MMult(mtx, tmp);
		// 移動行列
		tmp = MGetTranslate(node_pos);
		mtx = MMult(mtx, tmp);
	}


	if (EqualPhaseFrame(78.0f)) {
		if (mEfkHandle != NULL) {
			EffekseerManager::GetInstance()->Stop(mEfkHandle, 0.0f);
			mEfkHandle = NULL;
		}
		mEfkHandle = EffekseerManager::GetInstance()->PlayMtx(eEFK_CANNON_LASER_SHOOT, &mtx);
		EffekseerManager::GetInstance()->SetSpeed(mEfkHandle, 2.0f);

		SoundManager::GetSndComponent()->PlaySound(eSE_CANNON_LASER);
	}

	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (mEfkHandle != NULL) {
		if (EffekseerManager::GetInstance()->IsPlay(mEfkHandle)) {
			EffekseerManager::GetInstance()->SetMatrix(mEfkHandle, &mtx);
		}
		else {
			mEfkHandle = NULL;
		}
	}

	if (BetweenPhaseFrame(78.0f, 9999.0f)) {

		/* 戦闘不能者と既に攻撃された者を除く */
		if (p_player->CheckEnableAttack() == true) {

			ShapeCapsule capsuleA, capsuleB;
			p_player->GetBodyCapsule(&capsuleA);

			capsuleB.mPosA = node_pos;
			VECTOR rot_vec;
			rot_vec = VNorm(VTransformSR(VGet(0.0f, 0.0f, -1.0f), node_mtx) );
			capsuleB.mPosA = VAdd(node_pos, VScale(rot_vec, 150.0f) );
			capsuleB.mPosB = VAdd(node_pos, VScale(rot_vec, 900.0f) );
			capsuleB.mRadius = 150.0f;

#ifdef _DEBUG
			GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG

			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);

				int attack_power = 4;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), capsuleB.mPosA);
				damage_down_vec.y = 0.0f;
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
			}
		}

		mCannonCount ++;
		if (mCannonCount > 100.0f) {
			if (mEfkHandle != NULL) {
				EffekseerManager::GetInstance()->Stop(mEfkHandle, 0.0f);
				mEfkHandle = NULL;
			}
			mBladeEffect.Stop();
			ChangeState(eCHARA_STATE_IDLE);
		}
		mIsAttack = true;
	}
	else {
		mIsAttack = false;
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyGimuri::phaseEnterStateAttack3(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack03_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack03_Btm);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	this->GetFramePosFromName(&weapon_top_pos, strAttack04_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack04_Btm);
	mBladeEffect2nd.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_EXTEND_ATTACK_3, false, 0.6f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyGimuri::phaseStateAttack3(void)
{
	static const float sSphereSize	= 30.0f;
	
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	/* 戦闘不能者と既に攻撃された者を除く */
	if (p_player->CheckEnableAttack() == true) {

		// 敵のノードとキャラの円柱の当たり判定
		CollisionManager::tPillarWork piller;
		p_player->GetBodyPillar(&piller);

		CollisionManager::tSphereWork sphere;
		this->GetBodyFrameSphere(&sphere, strAttack03_Top, sSphereSize);

		if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {
			VECTOR damage_down_vec = VNorm(VSub(p_player->Position(), mPosition));
			p_player->ProcessDamage(1, false, NULL, &damage_down_vec, false, eATTACK_TYPE_ENEMY_NORMAL);
			SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
		}
		else {
			this->GetBodyFrameSphere(&sphere, strAttack04_Top, sSphereSize);
			if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {
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

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack03_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack03_Btm);
	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	this->GetFramePosFromName(&weapon_top_pos, strAttack04_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack04_Btm);
	mBladeEffect2nd.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {
		mBladeEffect.Stop();
		mBladeEffect2nd.Stop();
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyGimuri::phaseEnterStateAttack4(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack01_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack01_Btm);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	this->GetFramePosFromName(&weapon_top_pos, strAttack02_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack02_Btm);
	mBladeEffect2nd.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_EXTEND_ATTACK_4, false, 0.6f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyGimuri::phaseStateAttack4(void)
{
	static const float sStart		= 0.0f;
	static const float sEnd			= 64.0f;
	static const float sSphereSize	= 50.0f;
	
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (BetweenPhaseFrame(sStart, sEnd)) {

		/* 戦闘不能者と既に攻撃された者を除く */
		if (p_player->CheckEnableAttack() == true) {

			// 敵のノードとキャラの円柱の当たり判定
			CollisionManager::tPillarWork piller;
			p_player->GetBodyPillar(&piller);

			CollisionManager::tSphereWork sphere;
			this->GetBodyFrameSphere(&sphere, strAttack01_Top, sSphereSize);

			if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {
				VECTOR damage_down_vec = VNorm(VSub(p_player->Position(), mPosition));
				p_player->ProcessDamage(1, false, NULL, &damage_down_vec, false, eATTACK_TYPE_ENEMY_NORMAL);
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			}
			else {
				this->GetBodyFrameSphere(&sphere, strAttack02_Top, sSphereSize);
				if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {
					VECTOR damage_down_vec = VNorm(VSub(p_player->Position(), mPosition));
					p_player->ProcessDamage(1, false, NULL, &damage_down_vec, false, eATTACK_TYPE_ENEMY_NORMAL);
					SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				}
			}
		}
		mIsAttack = true;
	}
	else {
		mIsAttack = false;
	}

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack01_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack01_Btm);
	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	this->GetFramePosFromName(&weapon_top_pos, strAttack02_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack02_Btm);
	mBladeEffect2nd.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {
		mBladeEffect.Stop();
		mBladeEffect2nd.Stop();

		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyGimuri::phaseEnterStateAttack5(void)
{
	StartMotion(eMT_EXTEND_ATTACK_5, false, 0.75f, NULL);

	SoundManager::GetSndComponent()->PlaySound(eSE_AWAY);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyGimuri::phaseStateAttack5(void)
{
	VECTOR dir_vec;
	float move_speed = (kCHARA_AWAY_SPEED * 0.9f) * (float)(100 - GetMotionTimePercent());

	dir_vec.x = sin(mRotation.y + Utility::DegToRad(0.0f));
	dir_vec.y = 0.0f;
	dir_vec.z = cos(mRotation.y + Utility::DegToRad(0.0f));

	mMoveVec = VScale(dir_vec, move_speed);

	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyGimuri::phaseEnterStateAttack6(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack03_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack03_Btm);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	this->GetFramePosFromName(&weapon_top_pos, strAttack04_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack04_Btm);
	mBladeEffect2nd.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_EXTEND_ATTACK_6, false, 0.65f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyGimuri::phaseStateAttack6(void)
{
	static const float sSphereSize	= 30.0f;
	
	float move_speed = 0.06f * (100 - GetMotionTimePercent());
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	/* 戦闘不能者と既に攻撃された者を除く */
	if (p_player->CheckEnableAttack() == true) {

		// 敵のノードとキャラの円柱の当たり判定
		CollisionManager::tPillarWork piller;
		p_player->GetBodyPillar(&piller);

		CollisionManager::tSphereWork sphere;
		this->GetBodyFrameSphere(&sphere, strAttack03_Top, sSphereSize);

		bool is_hit = false;

		if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {
			is_hit = true;
		}
		else {
			this->GetBodyFrameSphere(&sphere, strAttack04_Top, sSphereSize);
			if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {
				is_hit = true;
			}
		}
		if (is_hit) {
			EFK_HANDLE efk_handle = NULL;
			efk_handle = EffekseerManager::GetInstance()->Play(eEFK_BREAK_BARRIER, &sphere.mPosition, 4.0f);
			EffekseerManager::GetInstance()->SetSpeed(efk_handle, 1.0f);

			VECTOR damage_down_vec = VNorm(VSub(p_player->Position(), mPosition));
			p_player->ProcessDamage(1, false, NULL, &damage_down_vec, false, eATTACK_TYPE_ENEMY_NORMAL);
			SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
		}

		mIsAttack = true;
	}
	else {
		mIsAttack = false;
	}

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack03_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack03_Btm);
	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	this->GetFramePosFromName(&weapon_top_pos, strAttack04_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack04_Btm);
	mBladeEffect2nd.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {
		mBladeEffect.Stop();
		mBladeEffect2nd.Stop();
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyGimuri::phaseEnterStateWaitAttack(void)
{
	StartMotion(eMT_IDLE, true, 0.5f, NULL, 1.0f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyGimuri::phaseEnterStateDamage(void)
{
	CharaEnemy::phaseEnterStateDamage();

	mBladeEffect2nd.Stop();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyGimuri::phaseEnterStateRetireEnd(void)
{
	CharaEnemy::phaseEnterStateRetireEnd();

	mBladeEffect2nd.Stop();
}

 /**** end of file ****/
