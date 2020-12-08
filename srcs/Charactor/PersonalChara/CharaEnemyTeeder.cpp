/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * てぃーだー(戦車)管理クラス
 * @author SPATZ.
 * @data   2014/11/17 01:10:48
 */
//---------------------------------------------------
#include "CharaEnemyTeeder.h"
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

/*=====================================*
 * 各ノード
 *=====================================*/
/*static*/const char* CharaEnemyTeeder::strAttackTopNode	= "主砲先";
/*static*/const char* CharaEnemyTeeder::strAttackBtmNode	= "主砲";

/*static*/const char* CharaEnemyTeeder::strAttackFrontNode	= "攻撃前";
/*static*/const char* CharaEnemyTeeder::strAttackBackNode	= "攻撃後";

/*static*/const char* CharaEnemyTeeder::strHeadNode			= "頭";

/*static*//*const*/float CharaEnemyTeeder::judge_dist_rush_end = 10000.0f;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyTeeder::CharaEnemyTeeder()
{
	/* initialize value */
	mUVScrollValue		= 0.0f;
	mEffHndSmoke		= -1;
	mStatePhase			= 0;
	mStateCount			= 0;
	mCannonTargetPos	= ZERO_VEC;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyTeeder::~CharaEnemyTeeder()
{
	Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyTeeder::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	if ((int)mScale.x == (int)TEEGER_SCALE) {
		mFlinchMax = 320;
	} 
	else {
		mFlinchMax = 0;
	}

	/* 
	 * mTireModel はタイヤのUVアニメーションを有効にするために、
	 * 通常モデルのクローンを作成し、タイヤのみを表示したモデルである。
	 */
	if (mTireModel.GetModelHandle() == -1) {
		mTireModel.LinkModel((char *)GetEnemyWorkTable(eENEMY_KIND_TEEGER)->mResourceName);
		mTireModel.Setup(false);
	}

	GraphicManager::GetInstance()->EntryDrawModel(&mTireModel);

	int tire_mdl_handle = mTireModel.GetModelHandle();
	for (int iI = 0; iI < MV1GetMeshNum(tire_mdl_handle); iI ++) {
		if (iI == 4) continue;
		// オリジナルモデルのタイヤを非表示にする
		MV1SetMeshVisible(tire_mdl_handle, iI, FALSE);
	}

	// オリジナルモデルのタイヤを非表示にする
	MV1SetMeshVisible(mModelHandle, 4, FALSE);

	// ミサイルのセットアップ
	for (int iI = 0; iI < kTEEDER_MISSILE_NUM; iI ++) {
		mMissileObject[iI].Setup(eOBJECT_KIND_MISSILE, (eItemKind)0, (VECTOR *)&ZERO_VEC, 0, this, true);
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyTeeder::Update(void)
{
	CharaEnemy::Update();

	mTireModel.Position() = this->mPosition;
	mTireModel.Rotation() = this->mRotation;
	mTireModel.Scale()	  = this->mScale;

	// ２つのモーションの同期をとる
	if (this->mMotion.mIndex != mTireModel.GetMotionIndex() ){
		mTireModel.StartMotion(mMotion.mIndex,
							   mMotion.mIsLoop,
							   mMotion.mSpeed,
							   mMotion.mpNextMotion,
							   mMotion.mBlendSpeed);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyTeeder::Destroy(void)
{
	GraphicManager::GetInstance()->ReserveRemoveModel(&mTireModel);

	mTireModel.SetAlpha(0.0f);
	mTireModel.DeleteModel();

	for (int iI = 0; iI < kTEEDER_MISSILE_NUM; iI ++) {
		//		GraphicManager::GetInstance()->ReserveRemoveModel(&mMissileObject[iI]);
		GraphicManager::GetInstance()->RemoveDrawModel(&mMissileObject[iI]);
	}

	CharaEnemy::Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyTeeder::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	if ((attackType == eATTACK_TYPE_COPY_ROD) || (attackType == eATTACK_TYPE_THUNDER)) {
		attackPower *= 2;
	}
	CharaEnemy::ProcessDamage(attackPower, isSetMode, pEffPos, pDamageDownVec, isDamageDown, attackType);
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyTeeder::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	//	float scale_value = 4.0f;
	float scale_value = 20.0f;

	scale_value = 4.0f;
	if (VSize(vec) < mBodySize + (mBodySize * (GetRand(100) * 0.01f * scale_value) ) ) {
		if (GetRand(1) == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
		} else {
			*pAttackCharaState = eCHARA_STATE_ATTACK_2;
		}
		return true;
	}

	scale_value = 20.0f;
	if (VSize(vec) < mBodySize + (mBodySize * (GetRand(100) * 0.01f * scale_value) ) ) {
		if (GetRand(1) == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_3;
		} else {
			*pAttackCharaState = eCHARA_STATE_ATTACK_4;
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
CharaEnemyTeeder::phaseEnterStateIdle(void)
{
	// 煙エフェクトの処理
	if (mEffHndSmoke != -1) {
		EffectManager::GetInstance()->Stop(mEffHndSmoke, 1.0f);
		mEffHndSmoke = -1;
	}
	
	mRotation.x = 0.0f;
	mRotation.z = 0.0f;

	CharaEnemy::phaseEnterStateIdle();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyTeeder::phaseStateTurn(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR head_pos, aim_pos;

	this->GetFramePosFromName(&head_pos, strHeadNode);
	aim_pos = p_player->Position();
	aim_pos.y = head_pos.y;
	this->SetFrameUserRotation( (char *)strHeadNode, &aim_pos, 0.10f);

	this->ResetFrameUserMatrix( (char *)strAttackBtmNode);

	CharaEnemy::phaseStateTurn();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyTeeder::phaseStateMove(void)
{
	CharaEnemy::phaseStateMove();

	// タイヤのUVスクロールアニメーションの更新
	updateTireUVAnimation(0.15f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyTeeder::phaseEnterStateAttack1(void)
{
	resetAllNode();

	SoundManager::GetVcComponent(mCharaName)->SetFrequency(eVC_ATTACK_1,
														   (int)(44100 * (((TEEGER_SCALE / mScale.x) - 1.0f) * 0.5f)));
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	//	StartMotion(eMT_TEEDER_ATTACK_1, false, 0.60f, NULL);
	StartMotion(eMT_TEEDER_ATTACK_1, false, 0.5f, NULL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyTeeder::phaseStateAttack1(void)
{
	//	CharaEnemy::phaseStateAttack1();

	float move_speed = 0.0f;
	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	static const float sStart	= 20.0f;
	static const float sEnd		= 25.0f;

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttackTopNode);
	this->GetFramePosFromName(&weapon_btm_pos, strAttackBtmNode);

#ifdef _DEBUG
	ShapeCapsule capsule_draw;
	capsule_draw.mPosA = weapon_top_pos;
	capsule_draw.mPosB = weapon_btm_pos;
	capsule_draw.mRadius = 20.0f;
	GraphicManager::GetInstance()->DbgRenderCapsule(&capsule_draw);
#endif // _DEBUG

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (this->EqualPhaseFrame(sStart) ) {
		// テールエフェクト開始
		mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
		this->SetMotionSpeed(0.75f);
	} else if (this->BetweenPhaseFrame(sStart, sEnd) ) {

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
				int attack_power = (int)(3.0f * (mScale.x / TEEGER_SCALE));
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), mPosition);
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, false, eATTACK_TYPE_ENEMY_NORMAL);
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
CharaEnemyTeeder::phaseEnterStateAttack2(void)
{
	resetAllNode();

	SoundManager::GetVcComponent(mCharaName)->SetFrequency(eVC_ATTACK_1,
														   (int)(44100 * (((TEEGER_SCALE / mScale.x) - 1.0f) * 0.5f)));
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	//	StartMotion(eMT_TEEDER_ATTACK_2, false, 1.2f, NULL);
	StartMotion(eMT_TEEDER_ATTACK_2, false, 0.8f, NULL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyTeeder::phaseStateAttack2(void)
{
	//	CharaEnemy::phaseStateAttack1();

	float move_speed = 0.0f;
	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	static const float sStart	= 45.0f;
	static const float sEnd		= 84.0f;

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttackTopNode);
	this->GetFramePosFromName(&weapon_btm_pos, strAttackBtmNode);

#ifdef _DEBUG
	ShapeCapsule capsule_draw;
	capsule_draw.mPosA = weapon_top_pos;
	capsule_draw.mPosB = weapon_btm_pos;
	capsule_draw.mRadius = 20.0f;
	GraphicManager::GetInstance()->DbgRenderCapsule(&capsule_draw);
#endif // _DEBUG

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (this->EqualPhaseFrame(sStart) ) {
		// テールエフェクト開始
		mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
		this->SetMotionSpeed(1.2f);
	} else if (this->BetweenPhaseFrame(sStart, sEnd) ) {

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
				int attack_power = (int)(2.0f * (mScale.x / TEEGER_SCALE));
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), mPosition);
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
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
 * 突進攻撃
 */
//---------------------------------------------------
void
CharaEnemyTeeder::phaseEnterStateAttack3(void)
{
	SoundManager::GetSndComponent()->SetFrequency(eSE_MOTOR_START,
												  (int)(44100 * (TEEGER_SCALE / mScale.x)));
	SoundManager::GetSndComponent()->SetVolume(eSE_MOTOR_START, getMotorVolume() );
	SoundManager::GetSndComponent()->PlaySound(eSE_MOTOR_START);

	CameraManager::GetInstance()->StartVibeCamera(1.0f, 5.0f, 10.0f);

	VECTOR effpos;
	this->GetFramePosFromName(&effpos, strAttackBackNode);
	mEffHndSmoke = EffectManager::GetInstance()->Play(eEF_REALFIRE, &effpos, 180.0f, 1.0f);
	EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "ColorRate",			0.20f);
	EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "FireRatio",			0.45f);
	EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "particleSpeed",		10000.0f);
	EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "particleSpread",		10.0f);
	EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "particleSystemHeight",30.0f);

	StartMotion(eMT_TEEDER_ATTACK_3, false, 0.75f, NULL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyTeeder::phaseStateAttack3(void)
{
	CharaBase* p_player;
	p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	float uv_anim_speed = 0.0f;
	VECTOR weapon_top_pos, weapon_btm_pos;
	static const float rush_attack_size = 220.0f;
	static const float rush_top_speed = 150.0f;

	this->GetFramePosFromName(&weapon_top_pos, strAttackFrontNode);
	this->GetFramePosFromName(&weapon_btm_pos, strAttackBackNode);

	/**** 突進溜めのモーションの処理 ****/
	if (GetMotionIndex() == eMT_TEEDER_ATTACK_3) {

		uv_anim_speed = (0.175f * (GetMotionTimePercent() / 100.0f) );

		// モーター音の管理
		if (SoundManager::GetSndComponent()->IsPlay(eSE_MOTOR_START) ){
			SoundManager::GetSndComponent()->SetFrequency(eSE_MOTOR_START,
														  (int)(44100 * (TEEGER_SCALE / mScale.x)));
			SoundManager::GetSndComponent()->SetVolume(eSE_MOTOR_START, getMotorVolume() );
		}

		// プレイヤーの方向に向かせる
		CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
		VECTOR vec = VSub(p_player->Position(), mPosition);
		mTargetRotY = atan2(vec.x, vec.z) + PHI_F;
		//		float turn_move_speed = 10.0f * kTURN_MOVE_ROT / mBodySize;
		float turn_move_speed = 10.0f * kTURN_MOVE_ROT / TEEGER_BODY_SIZE;
		processTurnTargetRotY(turn_move_speed, eCHARA_STATE_ATTACK_3);

		// 次のモーションへ移行判定処理
		if (GetIsEndMotion() == true) {
			StartMotion(eMT_TEEDER_ATTACK_4, true, 0.5f, NULL);
			mJumpMoveVec.mDash = ZERO_VEC;
			mIsAttack = true;
			SoundManager::GetSndComponent()->StopSound(eSE_MOTOR_START);

			CameraManager::GetInstance()->StartVibeCamera(2.0f, 3.0f, 10.0f);
		}
	}

	/**** 突進のモーションの処理 ****/
	if (GetMotionIndex() == eMT_TEEDER_ATTACK_4) {

		uv_anim_speed = 0.175f;

		// プレイヤーの方向に向かせる
		CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
		VECTOR vec = VSub(p_player->Position(), mPosition);
		mTargetRotY = atan2(vec.x, vec.z) + PHI_F;
		//		float turn_move_speed = 30.0f * kTURN_MOVE_ROT / mBodySize;
		//		float turn_move_speed = 30.0f * kTURN_MOVE_ROT / mBodySize;
		float turn_move_speed = 30.0f * kTURN_MOVE_ROT / TEEGER_BODY_SIZE;
		processTurnTargetRotY(turn_move_speed, eCHARA_STATE_ATTACK_3);

		// 移動量に関する処理
		if (VSize(mJumpMoveVec.mDash) < 1.0f) {
			// 移動量が無い場合は初速度を設定する
			float start_speed = 0.7f;
			mJumpMoveVec.mDash.x = sin(mRotation.y + Utility::DegToRad(180.0f)) * (kCHARA_RUN_SPEED * start_speed);
			mJumpMoveVec.mDash.z = cos(mRotation.y + Utility::DegToRad(180.0f)) * (kCHARA_RUN_SPEED * start_speed);
		}
		else if (VSize(mJumpMoveVec.mDash) < rush_top_speed) {
			// 突進の速度が最大値に満たない場合は加速値を上げる
			float speed = VSize(mJumpMoveVec.mDash) * 1.03f;
			VECTOR vecA, vecB;
			vecA = VNorm(mJumpMoveVec.mDash);
			vecB.x = sin(mRotation.y + Utility::DegToRad(180.0f) );
			vecB.y = 0.0f;
			vecB.z = cos(mRotation.y + Utility::DegToRad(180.0f) );

			vecA = VScale(vecA, 9.0f );
			vecB = VScale(vecB, 1.0f );
			mJumpMoveVec.mDash = VScale(VAdd(vecA, vecB), (0.1f * speed) );
		}
		mMoveVec = mJumpMoveVec.mDash;

		// 攻撃の処理
		if ( (p_player->GetIsRetire() == false) && (p_player->IsDamage() == false) ) {
			ShapeCapsule capsuleA, capsuleB;
			p_player->GetBodyCapsule(&capsuleA);
			capsuleB.mPosA = weapon_top_pos;
			capsuleB.mPosB = weapon_btm_pos;
			capsuleB.mRadius = rush_attack_size;
		
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB) ) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = (int)(5.0f * (mScale.x / TEEGER_SCALE));
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), mPosition);
				damage_down_vec = VCross(VGet(0.0f, 1.0f, 0.0f), damage_down_vec);
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_BREAK_GUARD);
			}
		}

		// モーター音の管理
		if (SoundManager::GetSndComponent()->IsPlay(eSE_MOTOR_RUNNING) == false){
			SoundManager::GetSndComponent()->PlaySound(eSE_MOTOR_RUNNING);

			EffekseerManager::GetInstance()->Play(eEFK_SHOCK_THIN, &mPosition, 100.0f);
		}
		if (SoundManager::GetSndComponent()->IsPlay(eSE_MOTOR_RUNNING) ){
			SoundManager::GetSndComponent()->SetFrequency(eSE_MOTOR_RUNNING,
														  (int)(44100 * (TEEGER_SCALE / mScale.x)));
			SoundManager::GetSndComponent()->SetVolume(eSE_MOTOR_RUNNING, getMotorVolume() );
		}

		// 終了判定処理
		bool is_end = false;
		float dist_to_player = 0.0f;
		dist_to_player = VSize(VSub(mPosition, p_player->Position() ) );


		if ( (GetIsMotionBlend() == false) && (VSize(VSub(mPosition, mPrevPosition) ) == 0.0f ) ) {
			is_end = true;

			int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST, &mPosition, 100.0f, 0.0f);
			EffectManager::GetInstance()->Stop(eff_handle, 2.0f);
			CameraManager::GetInstance()->StartVibeCamera(1.0f, 8.0f, 10.0f);
			SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
		}
		if (dist_to_player > judge_dist_rush_end) {
			is_end = true;
		}
		if (is_end) {
			SoundManager::GetSndComponent()->StopSound(eSE_MOTOR_START);
			SoundManager::GetSndComponent()->StopSound(eSE_MOTOR_RUNNING);

			// 煙エフェクトの処理
			if (mEffHndSmoke != -1) {
				EffectManager::GetInstance()->Stop(mEffHndSmoke, 1.0f);
				mEffHndSmoke = -1;
			}
			
			ChangeState(eCHARA_STATE_IDLE);
		}
	}

#ifdef _DEBUG
	/*
	ShapeCapsule capsule_draw;
	capsule_draw.mPosA = weapon_top_pos;
	capsule_draw.mPosB = weapon_btm_pos;
	capsule_draw.mRadius = rush_attack_size;
	GraphicManager::GetInstance()->DbgRenderCapsule(&capsule_draw);
	*/
#endif // _DEBUG

	// 煙エフェクトの処理
	if (mEffHndSmoke != -1) {
		//		EffectManager::GetInstance()->Position(mEffHndSmoke) = mPosition;
		VECTOR effpos;
		this->GetFramePosFromName(&effpos, strAttackBackNode);
		EffectManager::GetInstance()->Position(mEffHndSmoke) = effpos;
		EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "particleSpeed", 0.5f);
	}

	// タイヤのUVスクロールアニメーションの更新
	updateTireUVAnimation(uv_anim_speed);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyTeeder::phaseEnterStateAttack4(void)
{
	mStatePhase = 0; mStateCount = 0;
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyTeeder::phaseStateAttack4(void)
{
	CharaBase*	p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR		eff_pos;
	bool		is_set_cannon = false;
	EFK_HANDLE	efk_hdl;

	// "センター"のノードの傾きを計算する
	VECTOR user_rot_body_pos;
	float dist_to_target;
	dist_to_target = VSize(VSub(p_player->Position(), mPosition) );
	user_rot_body_pos = mPosition;
	user_rot_body_pos.x += (sin(mRotation.y + Utility::DegToRad(180.0f) ) * dist_to_target);
	user_rot_body_pos.z += (cos(mRotation.y + Utility::DegToRad(180.0f) ) * dist_to_target);

	switch (mStatePhase) {
	case 0:
		SoundManager::GetVcComponent(mCharaName)->SetFrequency(eVC_ATTACK_1,
															   (int)(44100 * (((TEEGER_SCALE / mScale.x) - 1.0f) * 0.5f)));
		SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
		is_set_cannon = true;
		mStatePhase ++; mStateCount = 0;
		break;

	case 1:
	case 3:
	case 5:
		updateRotateCannon();

		mCannonTargetPos = VAdd(p_player->Position(), p_player->MoveVec() );
		mCannonTargetPos.y += p_player->BodyHeight();

		this->GetFramePosFromName(&eff_pos, strAttackTopNode);

		if (mStateCount == 40) {

			efk_hdl = EffekseerManager::GetInstance()->Play(eEFK_EXPLOSION_BOMB, &eff_pos, 5.0f, 0.0f);
			EffekseerManager::GetInstance()->SetSpeed(efk_hdl, 0.75f);
		}

		if (mStateCount > 45) {
			CameraManager::GetInstance()->StartVibeCamera(0.7f, 5.0f, 10.0f);
			SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
			
			VECTOR weapon_top_pos, weapon_btm_pos;
			VECTOR move_vec;
			float missile_speed = 54;
			this->GetFramePosFromName(&weapon_top_pos, strAttackTopNode);
			this->GetFramePosFromName(&weapon_btm_pos, strAttackBtmNode);
			move_vec = VScale(VNorm(VSub(weapon_top_pos, weapon_btm_pos) ), missile_speed);
			
			//			mMissileObject[(mStatePhase / 2)].ProcessShoot(eff_pos, move_vec);
			//			eff_pos = VScale(VAdd(weapon_top_pos, weapon_btm_pos), 0.5f);
			eff_pos = VAdd(weapon_top_pos, VScale(VNorm(VSub(weapon_btm_pos, weapon_top_pos) ), 30.0f) );
			mMissileObject[(mStatePhase / 2)].ProcessShoot(eff_pos, move_vec);

			mStatePhase ++; mStateCount = 0;
		}
		break;

	case 2:
	case 4:
	case 6:
		// "センター"の傾きを上に持ってくる
		user_rot_body_pos.y += 400.0f;

		if (mStateCount > 5) {
			is_set_cannon = true;
			mStatePhase ++; mStateCount = 0;
		}
		break;

	default:
		break;
	};

	this->SetFrameUserRotation("センター", &user_rot_body_pos, 0.10f);
	mTireModel.SetFrameUserRotation("センター", &user_rot_body_pos, 0.10f);

	mStateCount ++;

	// 終了判定
	if (mStatePhase == 7) {
		mStatePhase =0; mStateCount = 0;
		ChangeState(eCHARA_STATE_IDLE);

		this->ResetFrameUserMatrix("センター");
		mTireModel.ResetFrameUserMatrix("センター");

		is_set_cannon = false;
	}

	if (is_set_cannon) {
		mCannonTargetPos = VAdd(p_player->Position(), p_player->MoveVec() );
		mCannonTargetPos.y += p_player->BodyHeight();
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyTeeder::updateRotateCannon()
{
	VECTOR head_pos, cannon_pos, aim_pos;
	float dist_to_target;

	// 頭を水平に動かす
	this->GetFramePosFromName(&head_pos, strHeadNode);
	aim_pos = mCannonTargetPos;
	aim_pos.y = head_pos.y;
	this->SetFrameUserRotation( (char *)strHeadNode, &aim_pos, 0.10f);

	// 主砲を縦に動かす
	this->GetFramePosFromName(&cannon_pos, strAttackBtmNode);
	dist_to_target = VSize(VSub(mCannonTargetPos, cannon_pos) );
	aim_pos = cannon_pos;
	aim_pos.x += (sin(mRotation.y + Utility::DegToRad(180.0f) ) * dist_to_target);
	aim_pos.y = mCannonTargetPos.y;
	aim_pos.z += (cos(mRotation.y + Utility::DegToRad(180.0f) ) * dist_to_target);

	this->SetFrameUserRotation( (char *)strAttackBtmNode, &aim_pos, 0.15f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyTeeder::resetAllNode(void)
{
	this->ResetFrameUserMatrix( (char *)strHeadNode);
	this->ResetFrameUserMatrix( (char *)strAttackBtmNode);
	this->ResetFrameUserMatrix("センター");
	mTireModel.ResetFrameUserMatrix("センター");
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyTeeder::phaseEnterStateDownStart(void)
{
	resetAllNode();

	if (SoundManager::GetSndComponent()->IsPlay(eSE_MOTOR_START) ){
		SoundManager::GetSndComponent()->StopSound(eSE_MOTOR_START);
	}
	if (SoundManager::GetSndComponent()->IsPlay(eSE_MOTOR_RUNNING) ){
		SoundManager::GetSndComponent()->StopSound(eSE_MOTOR_RUNNING);
	}

	CharaEnemy::phaseEnterStateDownStart();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyTeeder::phaseEnterStateRetireEnd(void)
{
	// 煙エフェクトの処理
	if (mEffHndSmoke != -1) {
		EffectManager::GetInstance()->Stop(mEffHndSmoke, 1.0f);
		mEffHndSmoke = -1;
	}

	GraphicManager::GetInstance()->ReserveRemoveModel(&mTireModel);

	mTireModel.SetAlpha(0.0f);
	mTireModel.DeleteModel();

	CharaEnemy::phaseEnterStateRetireEnd();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CharaEnemyTeeder::updateSideCollision(bool is2nd)		// 壁との当たり判定・改
{
	CharaBase::updateSideCollision(is2nd);

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyTeeder::updateTireUVAnimation(float scrollSpeed)
{
	mUVScrollValue += scrollSpeed;
	MV1SetFrameTextureAddressTransform(mTireModel.GetModelHandle(),
									   0,
									   mUVScrollValue,
									   0.0f,
									   1.0f, 
									   1.0f,
									   0.0f,
									   0.0f,
									   0.0f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
CharaEnemyTeeder::getMotorVolume(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	float dist_to_player = VSize(VSub(mPosition, p_player->Position() ) );
	int volume = (255 - (int)( (dist_to_player / (judge_dist_rush_end / 1.2f) ) * 255.0f) );
	if (volume < 0)		volume = 0;
	if (volume > 255)	volume = 255;

	return volume;
}


 /**** end of file ****/
