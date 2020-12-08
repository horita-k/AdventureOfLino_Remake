/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ハトフォン
 * @author SPATZ.
 * @data   2013/01/20 12:36:28
 */
//---------------------------------------------------
#include "CharaEnemyHatophon.h"
#include "Charactor/CharaPlayer.h"
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
#include "Scene/SceneInfo.h"
#include "Utility/Utility.h"

static const tCapsuleWork sCapsuleWork[] = {
	{"頭先",	"首",		30.0f,},
	{"尾",		"尾４",		24.0f,},
	{"左ひじ",	"左中指２",	24.0f,},
	{"右ひじ",	"右中指２",	24.0f,},
};
static const int kCAPSULE_NUM_HATOPHON = sizeof(sCapsuleWork) / sizeof(tCapsuleWork);

// ハトフォンがボスであるかどうかのフラグ
#define IS_NOT_BOSS_FLAG()	(TotalInfo::GetInstance()->GetIsFieldStartNoConvert(eFIELD_PRISM) ? true : false)

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyHatophon::CharaEnemyHatophon()
{
	mStateCount = 0;
	mStatePhase = 0;
	mEffIndex = -1;
	mEfkHandle = NULL;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHatophon::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	mFlinchMax = 1390;

	mPhaseState[eCHARA_STATE_JUMP_END]		= (CharaEnemy::PHASE_STATE)&CharaEnemyHatophon::phaseStateJumpEndStandard;
	mPhaseEnterState[eCHARA_STATE_JUMP_END] = (CharaEnemy::PHASE_STATE)&CharaEnemyHatophon::phaseEnterStateJumpEndStandard;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CharaEnemyHatophon::CheckDamageCollNormal(void* pParam)
{
	tCheckDamageWork* p_work = (tCheckDamageWork *)pParam;

	ShapeCapsule capsule;
	ShapeLine	line;

	line.mStart = p_work->mWeaponBtmPos;
	line.mEnd	= p_work->mWeaponTopPos;

	for (int iI = 0; iI < kCAPSULE_NUM_HATOPHON; iI ++) {
		this->GetFramePosFromName(&capsule.mPosA, sCapsuleWork[iI].mPosA);
		this->GetFramePosFromName(&capsule.mPosB, sCapsuleWork[iI].mPosB);
		capsule.mRadius = sCapsuleWork[iI].mRadius;
#ifdef _DEBUG
		GraphicManager::GetInstance()->DbgRenderCapsule(&capsule);
#endif // _DEBUG
		if (CollisionManager::GetInstance()->Check_LineToCapsule(&line, &capsule)) {
			return true;
		}
	}

	ShapeBoard board;
	this->GetFramePosFromName(&board.mPosA, "左ひじ");
	this->GetFramePosFromName(&board.mPosB, "左中指２");
	this->GetFramePosFromName(&board.mPosC, "左足首");
	this->GetFramePosFromName(&board.mPosD, "左腕");
#ifdef _DEBUG
		GraphicManager::GetInstance()->DbgRenderBoard(&board);
#endif // _DEBUG
	if (CollisionManager::GetInstance()->Check_LineToBoard(&line, &board)) {
		return true;
	}
	this->GetFramePosFromName(&board.mPosA, "右ひじ");
	this->GetFramePosFromName(&board.mPosB, "右中指２");
	this->GetFramePosFromName(&board.mPosC, "右足首");
	this->GetFramePosFromName(&board.mPosD, "右腕");
#ifdef _DEBUG
		GraphicManager::GetInstance()->DbgRenderBoard(&board);
#endif // _DEBUG
	if (CollisionManager::GetInstance()->Check_LineToBoard(&line, &board)) {
		return true;
	}

	return false;
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyHatophon::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	float dist = VSize(vec);
	
	int rand;
	bool result = false;

	if (dist < mBodySize * 1.5f) {
		float rate = mBodySize / dist;
		rand = GetRand((int)((1.0f - rate) * 30.0f) + 1);

		if (rand > 2) {
			int pattern = GetRand(2);
			switch (pattern) {
			case 0:
				*pAttackCharaState = eCHARA_STATE_ATTACK_1;
				result = true;
				break;
			case 1:
				*pAttackCharaState = eCHARA_STATE_ATTACK_2;
				result = true;
				break;
			default:
				*pAttackCharaState = eCHARA_STATE_ATTACK_3;
				result = true;
				break;
			}
		}
		//	} else if (dist > mBodySize * 4.0f) { 
	} else if (dist > mBodySize * 9.0f) {
		rand = GetRand(40);
		//		if (rand == 0) {
		if (rand != 0) {
			//			*pAttackCharaState = eCHARA_STATE_ATTACK_5;
			*pAttackCharaState = eCHARA_STATE_JUMP_UP;
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
CharaEnemyHatophon::Update(void)
{
	CharaEnemy::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHatophon::phaseEnterStateJumpUp(void)
{
	StartMotionEx(&sMotionParam[eMT_JUMP_START]);

	mIsReserveComboInput = false;
	mIsAttack = false;
	mIsDamage = false;

	// ジャンプ力高めに設定
	mGravity += (kJUMP_GRAVITY * 1.4f);

	mIsLand = false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHatophon::phaseEnterStateJumpEndStandard(void)
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
	
	StartMotion(eMT_JUMP_END, false, 0.75f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHatophon::phaseEnterStateJumpEndDarts(void)
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

	StartMotion(eMT_HATOPHON_ATTACK_4, false, 0.75f, NULL, 1.0f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHatophon::phaseStateJumpEndStandard(void)
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
CharaEnemyHatophon::phaseStateJumpEndDarts(void)
{
	static float sWaitCount = 0.0f;

	//	mJumpMoveVec.mLanding = VScale(mJumpMoveVec.mLanding, 0.95f);
	//	mMoveVec = mJumpMoveVec.mLanding;

	//	if (GetIsEndMotion()) {
		//		ChangeState(eCHARA_STATE_IDLE);
	//	}	
	sWaitCount += 1.0f;
	if (sWaitCount > 120.0f) {

		sWaitCount = 0.0f;
		mPhaseState[eCHARA_STATE_JUMP_END]		= (CharaEnemy::PHASE_STATE)&CharaEnemyHatophon::phaseStateJumpEndStandard;
		mPhaseEnterState[eCHARA_STATE_JUMP_END] = (CharaEnemy::PHASE_STATE)&CharaEnemyHatophon::phaseEnterStateJumpEndStandard;
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
 * 標準の関数だとプレイヤーの方に向いてしまう為
 */
//---------------------------------------------------
void
CharaEnemyHatophon::phaseEnterStateJumpDown(void)
{
	StartMotionEx(&sMotionParam[eMT_JUMP_DOING]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHatophon::phaseStateJumpUp(void)
{
	CharaEnemy::phaseStateJumpUp();

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (mGravity < 0) {
		SetRotationToTarget(&p_player->Position());
		ChangeState(eCHARA_STATE_ATTACK_4);
	}

	//	ChangeState(eCHARA_STATE_ATTACK_4);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHatophon::phaseEnterStateAttack1(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	/*
	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "右中指２");
	this->GetFramePosFromName(&weapon_btm_pos, "右ひじ");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);
	*/

	StartMotion(eMT_HATOPHON_ATTACK_1, false, 0.625f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHatophon::phaseStateAttack1(void)
{
	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	if (EqualPhaseFrame(45.0f)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}

	if (BetweenPhaseFrame(45.0f, 51.0f)) {

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
		
			this->GetFramePosFromName(&capsuleB.mPosA, "右ひじ");
			this->GetFramePosFromName(&capsuleB.mPosB, "右中指２");
			capsuleB.mRadius = 45.0f;
			
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 2;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), mPosition);
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
			}

			//			mBladeEffect.Update(&capsuleB.mPosA, &capsuleB.mPosB);
		}

	} else {
		mIsAttack = false;
	}

	PRINT_SCREEN(GetColor(255, 255, 0), "NowTime : %.2f\n", GetMotionNowTime());

	if (GetIsEndMotion()) {
		//		mBladeEffect.Stop();
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHatophon::phaseEnterStateAttack2(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	/*
	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "右中指２");
	this->GetFramePosFromName(&weapon_btm_pos, "右ひじ");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);
	*/

	StartMotion(eMT_HATOPHON_ATTACK_2, false, 0.625f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHatophon::phaseStateAttack2(void)
{
	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	if (EqualPhaseFrame(45.0f)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}

	if (BetweenPhaseFrame(45.0f, 51.0f)) {
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
		
			this->GetFramePosFromName(&capsuleB.mPosA, "左ひじ");
			this->GetFramePosFromName(&capsuleB.mPosB, "左中指２");
			capsuleB.mRadius = 45.0f;
			
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 2;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), mPosition);
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
			}

			//			mBladeEffect.Update(&capsuleB.mPosA, &capsuleB.mPosB);
		}
	} else {
		mIsAttack = false;
	}

	PRINT_SCREEN(GetColor(255, 255, 0), "NowTime : %.2f\n", GetMotionNowTime());

	if (GetIsEndMotion()) {
		//		mBladeEffect.Stop();
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHatophon::phaseEnterStateAttack3(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	/*
	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "首1");
	this->GetFramePosFromName(&weapon_btm_pos, "頭先");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);
	*/

	StartMotion(eMT_ATTACK_3, false, 0.25f); 
}

//---------------------------------------------------
/**
 * ローリング攻撃
 */
//---------------------------------------------------
void
CharaEnemyHatophon::phaseStateAttack3(void)
{
	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	if (EqualPhaseFrame(24.0f)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}

	if (EqualPhaseFrame(27.0f)) {
		const float size = 40.0f;
		VECTOR effpos;
		effpos = VAdd(mPosition, mMoveVec);
		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,
															&effpos,
															&VGet(size, size, size),
															0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 2.0f);
		SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
	}

	if (BetweenPhaseFrame(24.0f, 27.0f)) {

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
		
			this->GetFramePosFromName(&capsuleB.mPosA, "首1");
			this->GetFramePosFromName(&capsuleB.mPosB, "頭先");
			capsuleB.mRadius = 60.0f;
			
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 2;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), capsuleB.mPosB);
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
			}

			//			mBladeEffect.Update(&capsuleB.mPosA, &capsuleB.mPosB);
		}

	} else {
		mIsAttack = false;
	}

	PRINT_SCREEN(GetColor(255, 255, 0), "NowTime : %.2f\n", GetMotionNowTime());

	if (GetIsEndMotion()) {
		//		mBladeEffect.Stop();
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHatophon::phaseEnterStateAttack4(void)
{
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
	speed *= 1.1f; // ちょっとだけ増やす
	
	mJumpMoveVec.mDash = VScale(VNorm(vec), speed);


	mPhaseState[eCHARA_STATE_JUMP_END]		= (CharaEnemy::PHASE_STATE)&CharaEnemyHatophon::phaseStateJumpEndDarts;
	mPhaseEnterState[eCHARA_STATE_JUMP_END] = (CharaEnemy::PHASE_STATE)&CharaEnemyHatophon::phaseEnterStateJumpEndDarts;

	SoundManager::GetSndComponent()->PlaySound(eSE_ROLLING);

	StartMotion(eMT_HATOPHON_ATTACK_4, false, 0.5f); 
}

//---------------------------------------------------
/**
 * 突進
 */
//---------------------------------------------------
void
CharaEnemyHatophon::phaseStateAttack4(void)
{
	mMoveVec = mJumpMoveVec.mDash;

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
		
		this->GetFramePosFromName(&capsuleB.mPosA, "首1");
		this->GetFramePosFromName(&capsuleB.mPosB, "頭先");
		capsuleB.mRadius = 70.0f;
		
		if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
			
			SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			int attack_power = 2;
			VECTOR damage_down_vec;
			damage_down_vec = VSub(p_player->Position(), mPosition);
			damage_down_vec = VCross(VGet(0.0f, 1.0f, 0.0f), damage_down_vec);
			damage_down_vec = VNorm(damage_down_vec);
			p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);

		}
	}

	PRINT_SCREEN(GetColor(255, 255, 0), "NowTime : %.2f\n", GetMotionNowTime());

	//	if (GetIsEndMotion() || mIsLand == false) {
	if (GetIsEndMotion() || mIsLand == true) {
		
		mMoveVec = ZERO_VEC;
		mIsAttack = false;
		mJumpMoveVec.mDash = ZERO_VEC;
		ChangeState(eCHARA_STATE_IDLE);
	}
}

static int sBurstEffHandle =-1;
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHatophon::phaseEnterStateRetireDoing(void)
{
	// 武道会の場合の処理
	//	if (sDebugArenaFlag) {
	if (IS_NOT_BOSS_FLAG()) {

		CharaEnemy::phaseEnterStateRetireDoing();
		return;
	}

	if (mHitpoint != kSYSTEM_DAMAGE_VALUE) {

		VECTOR eff_pos = mPosition;
		mEffIndex = EffectManager::GetInstance()->Play(eEF_CHARGE,
														  &eff_pos,
														  24.0f,
														  0.0f);
		EffectManager::GetInstance()->SetFloat(mEffIndex, "particleSpeed", -2.6f);

		SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
		CameraManager::GetInstance()->StartVibeCamera(3.0f, 6.0f, 8.0f);

		GraphicManager::GetInstance()->RemoveDrawShadow(&mShadow);

		CharaPlayer* p_mine = (CharaPlayer *)CollisionManager::GetInstance()->GetBodyCollisionAddress(0);
		p_mine->PermitAcceptInput(false);
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
		TotalInfo::GetInstance()->DisableLockonLayout();
		
		ActionTaskManager::tTaskInfo info;
		info.mTaskType = ActionTaskManager::eTASK_ENEMY_ITEM;
		info.mTaskValueA = (long long)this;
		info.mTaskValueB = 1;
		PRINT_CONSOLE("NotifyAction -> eTASK_ENEMY_ITEM \n");
		ActionTaskManager::GetInstance()->NotifyAction(&info);
	}

	//	SoundManager::GetSndComponent()->PlaySound(eSE_ERASE_MODEL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHatophon::phaseStateRetireDoing(void)
{
	// 武道会の場合の処理
	//	if (sDebugArenaFlag) {
	if (IS_NOT_BOSS_FLAG()) {
		CharaEnemy::phaseStateRetireDoing();
		return;
	}
	// システムダメージの場合、以降の処理を行わない
	if (mHitpoint == kSYSTEM_DAMAGE_VALUE) {
		CharaEnemy::phaseStateRetireDoing();
		return;
	}

	float alpha = this->GetAlpha();

	switch (mStatePhase) {
	case 0:

		alpha += -0.01f;
		if (alpha <= 0.4f) {
			
			alpha = 0.4f;
		}
		if (mStateCount > 150) {

			if (mEffIndex != -1) {
				EffectManager::GetInstance()->Stop(mEffIndex, 1.0f);
				mEffIndex = -1;
			}

			VECTOR eff_pos = mPosition;
			mEfkHandle = EffekseerManager::GetInstance()->Play(eEFK_BOSS_DEATH,
															   &eff_pos,
															   &VScale(VGet(12.0f, 12.0f, 12.0f), 3.0f));
			EffekseerManager::GetInstance()->SetSpeed(mEfkHandle, 0.8f);

			SoundManager::GetSndComponent()->PlaySound(eSE_BREAK_BARRIER);
			SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
			CameraManager::GetInstance()->StartShakeCamera(3.0f, 5.0f, 60.0f);

			mStatePhase ++; mStateCount = 0;
		}

		break;

	case 1:
		alpha += -0.03f;
		if (alpha <= 0.1f) {
			alpha = 0.0002f;
			mScale = ZERO_VEC;
		}
		if (EffekseerManager::GetInstance()->IsPlay(mEfkHandle) == false) {
			alpha = 0.0f;
			ChangeState(eCHARA_STATE_RETIRE_END);
		}
		break;
	default:
		break;
	}
	mStateCount ++;

	this->SetAlpha(alpha);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHatophon::phaseEnterStateRetireEnd(void)
{
	// 武道会の場合の処理
	//	if (sDebugArenaFlag) {
	if (IS_NOT_BOSS_FLAG()) {
		CharaEnemy::phaseEnterStateRetireEnd();
		return;
	}

	GraphicManager::GetInstance()->ReserveRemoveModel((ModelBase *)this);
	CollisionManager::GetInstance()->RemoveBodyCollision((ModelBase *)this);
	CollisionManager::GetInstance()->RemoveAttackCollision((ModelBase *)this);

	this->SetAlpha(0.0f);

	ModelPmd::DeleteModel();
	mIsActive = false;

	GraphicManager::GetInstance()->RemoveDrawShadow(&mShadow);

#ifdef _DEBUG
	//	ChangeState(eCHARA_STATE_JUMP_DOWN);
#endif // _DEBUG

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHatophon::phaseStateRetireEnd(void)
{
	// 武道会の場合の処理
	//	if (sDebugArenaFlag) {
	if (IS_NOT_BOSS_FLAG()) {
		CharaEnemy::phaseStateRetireEnd();
		return;
	}
}


//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*
void
CharaEnemyHatophon::phaseStateWaitAttack(void)
{
	int rand = GetRand(40);

	if (rand > 4) {
		ChangeState(eCHARA_STATE_ATTACK_4);
		return;
	}

	if (rand == 0) {
		ChangeState(eCHARA_STATE_ATTACK_1);
	} else if (rand == 1) {
		ChangeState(eCHARA_STATE_ATTACK_2);
	} else if (rand == 2) {
		ChangeState(eCHARA_STATE_ATTACK_3);
	} else if (rand == 3) {
		ChangeState(eCHARA_STATE_ATTACK_4);
	}
}
*/

#ifdef _DEBUG
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHatophon::debugFunc(void)
{
	ShapeCapsule capsuleB;
	for (int iI = 0; iI < kCAPSULE_NUM_HATOPHON; iI ++) {
		this->GetFramePosFromName(&capsuleB.mPosA, sCapsuleWork[iI].mPosA);
		this->GetFramePosFromName(&capsuleB.mPosB, sCapsuleWork[iI].mPosB);
		capsuleB.mRadius = sCapsuleWork[iI].mRadius;
		GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
	}
}

#endif // _DEBUG

 /**** end of file ****/
