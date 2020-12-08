/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * うさえもんファイル
 * @author SPATZ.
 * @data   2015/12/31 10:04:48
 */
//---------------------------------------------------
#include "../CharaPlayer.h"
#include "CharaEnemyUsae.h"
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
CharaEnemyUsae::CharaEnemyUsae()
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsae::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	mEfkHandle	= NULL;
	mEfkFlame	= NULL;

	mStatePhase = 0;
	mStateCount = 0;

	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	mFlinchMax = 180;
	mCannonCount= 0;

	mEffectPos = ZERO_VEC;
	mEffectMoveVec = ZERO_VEC;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsae::Update(void)
{
	CharaEnemy::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsae::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	int prev_state = mCharaState;

	CharaEnemy::ProcessDamage(attackPower, isSetMode, pEffPos, pDamageDownVec, isDamageDown, attackType);

	// ダメージでステータスが変わったら
	if (prev_state != mCharaState) {
		if (mEfkHandle != NULL) {
			EffekseerManager::GetInstance()->Stop(mEfkHandle, 0.0f);
			mEfkHandle = NULL;
		}
		GraphicManager::GetInstance()->RevertFogParam(1.0f);

		mConstGravitySpeed = kGRAVITY_SPEED;
	}
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyUsae::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);

	int rand_value = GetRand(100);
	float rand_rate = (float)rand_value * 0.01f;
	float dist = VSize(vec);

	if (dist < (mBodySize * 1.0f) + ((mBodySize * 2.0f) * rand_rate) ) {

		int rand_2nd = GetRand(2);
		switch (rand_2nd) {
		case 0:
		case 1:
			*pAttackCharaState = eCHARA_STATE_ATTACK_1; // ３連パンチ
			return true;
		case 2:
			*pAttackCharaState = eCHARA_STATE_ATTACK_2;	// 跳び蹴り
			return true;
		default:
			break;
		}
	}
	else if (dist < (mBodySize * 4.0f) + ((mBodySize * 2.0f) * rand_rate) ) {
		
		int rand_2nd = GetRand(10);
		if (rand_2nd == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_2;	// 跳び蹴り
			return true;
		}
		else if (rand_2nd == 1) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_3;	// 龍拳
			return true;
		}

	}
	else if (dist < (mBodySize * 8.0f) + ((mBodySize * 4.0f) * rand_rate) ) {
		int rand_2nd = GetRand(20);
		if (rand_2nd == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_4;	// かめはめ波
			return true;
		}
		else if (rand_2nd == 1) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_5;	// 元気玉
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
CharaEnemyUsae::phaseStateWaitAttack(void)
{
	if (GetRand(2) == 0) {
		ChangeState(mReserveCharaState);
		mReserveCharaState = -1;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsae::processNoMoveAction(void)
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
CharaEnemyUsae::phaseEnterStateIdle(void)
{
	CharaEnemy::phaseEnterStateIdle();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsae::phaseEnterStateAttack1(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack03_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack03_Btm);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	//	StartMotion(eMT_EXTEND_ATTACK_1, false, 0.4f);
	StartMotion(eMT_EXTEND_ATTACK_1, false, 0.3f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsae::phaseStateAttack1(void)
{
	static const float sSphereSize	= 50.0f;

	float move_speed = 0.1f * (GetMotionTimePercent());

	const char* top_name = NULL;
	const char* btm_name = NULL;

	bool is_straight = false;

	if (BetweenPhaseFrame(4, 15)) {
		top_name = strAttack03_Top;
		btm_name = strAttack03_Btm;
	}
	else if (BetweenPhaseFrame(25, 28)) {
		top_name = strAttack04_Top;
		btm_name = strAttack04_Btm;
		is_straight = true;
	}
	else {
		move_speed = 0.0f;
	}

	if (EqualPhaseFrame(4)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
	}
	else if (EqualPhaseFrame(25)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}

	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);


	if (top_name != NULL) {
		CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		VECTOR weapon_top_pos, weapon_btm_pos;
		GetFramePosFromName(&weapon_top_pos, top_name);
		GetFramePosFromName(&weapon_btm_pos, btm_name);
		mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

		/* 戦闘不能者と既に攻撃された者を除く */
		if (p_player->CheckEnableAttack() == true) {

			// 敵のノードとキャラの円柱の当たり判定
			CollisionManager::tSphereWork sphere;
			this->GetBodyFrameSphere(&sphere, top_name, sSphereSize);

			CollisionManager::tPillarWork piller;
			p_player->GetBodyPillar(&piller);

			if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {
				VECTOR damage_down_vec = VNorm(VSub(p_player->Position(), mPosition));
				p_player->ProcessDamage(1, false, NULL, &damage_down_vec, is_straight, eATTACK_TYPE_ENEMY_NORMAL);

				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
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

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsae::phaseEnterStateAttack2(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack01_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack01_Btm);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_EXTEND_ATTACK_2, false, 0.75f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsae::phaseStateAttack2(void)
{
	static const float sSphereSize	= 50.0f;

	float move_speed = 0.18f * (100 - GetMotionTimePercent());

	const char* top_name = NULL;
	const char* btm_name = NULL;

	if (BetweenPhaseFrame(8, 14)) {
		top_name = strAttack01_Top;
		btm_name = strAttack01_Btm;
	}

	if (EqualPhaseFrame(4)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}

	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	if (top_name != NULL) {
		CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		VECTOR weapon_top_pos, weapon_btm_pos;
		GetFramePosFromName(&weapon_top_pos, top_name);
		GetFramePosFromName(&weapon_btm_pos, btm_name);
		mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

		/* 戦闘不能者と既に攻撃された者を除く */
		if (p_player->CheckEnableAttack() == true) {

			// 敵のノードとキャラの円柱の当たり判定
			CollisionManager::tSphereWork sphere;
			this->GetBodyFrameSphere(&sphere, top_name, sSphereSize);

			CollisionManager::tPillarWork piller;
			p_player->GetBodyPillar(&piller);

			if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {
				VECTOR damage_down_vec = VNorm(VSub(p_player->Position(), mPosition));
				p_player->ProcessDamage(1, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);

				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
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

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsae::phaseEnterStateAttack3(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack04_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack04_Btm);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_EXTEND_ATTACK_3, false, 0.8f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsae::phaseStateAttack3(void)
{
	static const float sSphereSize	= 65.0f;

	float move_speed = 0.05f * (100 - GetMotionTimePercent());

	const char* top_name = NULL;
	const char* btm_name = NULL;

	if (BetweenPhaseFrame(23, 26)) {
		top_name = strAttack04_Top;
		btm_name = strAttack04_Btm;
	}

	if (EqualPhaseFrame(23)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}

	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	if (top_name != NULL) {
		CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		VECTOR weapon_top_pos, weapon_btm_pos;
		GetFramePosFromName(&weapon_top_pos, top_name);
		GetFramePosFromName(&weapon_btm_pos, btm_name);
		mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

		/* 戦闘不能者と既に攻撃された者を除く */
		if (p_player->CheckEnableAttack() == true) {

			// 敵のノードとキャラの円柱の当たり判定
			CollisionManager::tSphereWork sphere;
			this->GetBodyFrameSphere(&sphere, top_name, sSphereSize);

			CollisionManager::tPillarWork piller;
			p_player->GetBodyPillar(&piller);

			if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {
				VECTOR damage_down_vec = VNorm(VSub(p_player->Position(), mPosition));
				p_player->ProcessDamage(2, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);

				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
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

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsae::phaseEnterStateAttack4(void)
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

	StartMotion(eMT_EXTEND_ATTACK_4, false, 2.2f);

	mCannonCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsae::phaseStateAttack4(void)
{
	static const float kCHARGE_FRAME = 148.0f;
	
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack03_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack03_Btm);

	MATRIX node_mtx;
	VECTOR node_pos;

	//	GetFrameMatrixFromName(&node_mtx, "左手首先");
	//	Utility::ExtractMatrixPosition(&node_pos, &node_mtx);
	GetFramePosFromName(&node_pos, "左手首先");
	
	MATRIX mtx;
	{
		float scale = 0.75f * mScale.x;

		MATRIX tmp;
		mtx = MGetIdent();
		// 拡縮行列
		tmp = MGetScale(VGet(scale, scale, scale) );
		mtx = MMult(mtx, tmp);
		tmp = MGetRotY(Utility::DegToRad(90.0f) );
		mtx = MMult(mtx, tmp);

		// 回転行列
		//		Utility::ExtractRotationMatrix
		node_mtx = MGetRotY(mRotation.y);
		mtx = MMult(mtx, node_mtx);

		// 移動行列
		tmp = MGetTranslate(node_pos);
		mtx = MMult(mtx, tmp);
	}

	if (EqualPhaseFrame(kCHARGE_FRAME)) {
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

	if (BetweenPhaseFrame(0, kCHARGE_FRAME)) {

		// プレイヤーの方向に向かせる
		CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
		VECTOR vec = VSub(p_player->Position(), mPosition);
		mTargetRotY = atan2(vec.x, vec.z) + PHI_F;
		float turn_move_speed = 10.0f * kTURN_MOVE_ROT / TEEGER_BODY_SIZE;
		processTurnTargetRotY(turn_move_speed, eCHARA_STATE_ATTACK_4);
	}
	else if (BetweenPhaseFrame(kCHARGE_FRAME, 9999.0f)) {

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
CharaEnemyUsae::phaseEnterStateAttack5(void)
{
	StartMotion(eMT_EXTEND_ATTACK_5, false, 2.0f);

	mStateCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsae::phaseStateAttack5(void)
{
	static float sAccel = 0.0f;
	static const float size = 500.0f;
	VECTOR eff_vec = ZERO_VEC;
	int* pEfsHandle = NULL;

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	VECTOR next_pos;
	VECTOR weapon_top_pos;
	VECTOR hitpos, normal;

	bool is_end = false;

	{
		VECTOR posa, posb;
		this->GetFramePosFromName(&posa, strAttack03_Top);
		this->GetFramePosFromName(&posb, strAttack04_Top);

		weapon_top_pos = VScale(VAdd(posa, posb), 0.5f);
		weapon_top_pos.y += 300.0f;
	}

	if (EqualPhaseFrame(30.0f)) {
		SetRotationToTarget(&p_player->Position());

		mEffectPos = weapon_top_pos;
		mEfkHandle = EffekseerManager::GetInstance()->Play(eEFK_AURA_FLAME, &mEffectPos, 8.0f);

		SoundManager::GetSndComponent()->PlaySound(eSE_CANNON_LASER_CHARGE);
	}
		
	if (BetweenPhaseFrame(30.0f, 257.0f)) {
		SetRotationToTarget(&p_player->Position());

		EffekseerManager::GetInstance()->SetPosition(mEfkHandle, &weapon_top_pos);
	}

	if (EqualPhaseFrame(257.0f)) {

		SetRotationToTarget(&p_player->Position());

		EffekseerManager::GetInstance()->SetPosition(mEfkHandle, &weapon_top_pos);

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
		mEffectPos = weapon_top_pos;
		EffekseerManager::GetInstance()->SetPosition(mEfkHandle, &mEffectPos);

		mEffectMoveVec = VSub(tar_pos, weapon_top_pos);
		mEffectMoveVec = VScale(VNorm(mEffectMoveVec), vo);

		SoundManager::GetSndComponent()->PlaySound(eSE_CANNON_LASER);
	}

	if (BetweenPhaseFrame(257.0f, 9999.0f)) {

		/* 波動移動処理 */
		mEffectMoveVec = VAdd(mEffectMoveVec, VScale(VNorm(mEffectMoveVec), sAccel)); // 加速度更新
		next_pos = VAdd(mEffectPos, mEffectMoveVec); // 座標更新

		/* 壁判定 */
		if (
			(CollisionManager::GetInstance()->CheckWallCollision(mEffectPos, next_pos, &hitpos, &normal)) ||
			(CollisionManager::GetInstance()->CheckEarthCollision(mEffectPos, next_pos, &hitpos, &normal) != NOT_ON_COLLISION)) {
			// でかい奴を停止させる
			EffekseerManager::GetInstance()->Stop(mEfkHandle, 0.5f);
			mEfkHandle = NULL;
			
			int eff_hnd = EffectManager::GetInstance()->Play(eEF_DUST, &hitpos, 50.0f, 0.0f);
			EffectManager::GetInstance()->Stop(eff_hnd, 0.5f);
			SoundManager::GetSndComponent()->PlaySound(eSE_BURST);

			is_end = true;
		}
		else {
			mEffectPos = next_pos;
			EffekseerManager::GetInstance()->SetPosition(mEfkHandle, &mEffectPos); // 反映
		}
	}

	/* 当たり判定処理 */
	if (p_player->IsDamage() == false) {

		if (mEfkHandle != NULL) {
			float eff_size = 300.0f;
		
			ShapeCapsule capsuleA = ShapeCapsule(p_player->Position(), p_player->Position(), p_player->BodySize());
			ShapeCapsule capsuleB = ShapeCapsule(mEffectPos, mEffectPos, eff_size);
			capsuleA.mPosB.y += p_player->BodyHeight();
			capsuleB.mPosB.y += 1.0f;
#ifdef _DEBUG
			GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				//				int attack_power = 3;
				int attack_power = 6;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), capsuleB.mPosA);
				damage_down_vec.y = 0.0f;
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_FLAME);
			}
		}
	}

	/**** 終了処理 ****/
	if (is_end) {
		if (mEfkHandle != NULL) {
			EffekseerManager::GetInstance()->Stop(mEfkHandle, 0.5f);
			mEfkHandle = NULL;
		}
		ChangeState(eCHARA_STATE_IDLE);
		return;
	}

	mStateCount ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsae::phaseEnterStateDamage(void)
{
	CharaEnemy::phaseEnterStateDamage();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyUsae::phaseEnterStateRetireEnd(void)
{
	CharaEnemy::phaseEnterStateRetireEnd();

	GraphicManager::GetInstance()->RevertFogParam(1.0f);
}


 /**** end of file ****/
