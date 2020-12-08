/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * `‰f‰æ‚oƒtƒ@ƒCƒ‹
 * @author SPATZ.
 * @data   2012/05/25 00:27:32
 */
//---------------------------------------------------
#include "CharaEnemyHongKong.h"
#include "Charactor/CharaInfo.h"
#include "AppLib/Basic/Basic.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Effect/EffectManager.h"
#include "AppLib/Graphic/EffekseerManager.h"
#include "AppLib/Input/InputManager.h"
#include "AppLib/Sound/SoundManager.h"

#include "AppLib/Collision/CollisionManager.h"
#include "DataDefine/VoiceDef.h"
#include "DataDefine/EffectDef.h"
#include "Scene/ActionTaskManager.h"
#include "Utility/Utility.h"

static const char* strAttack01_Top = "¶‘«Žñ";
static const char* strAttack01_Btm = "¶‚Ð‚´";

static const char* strAttack02_Top = "‰E‘«Žñ";
static const char* strAttack02_Btm = "‰E‚Ð‚´";

static const char* strAttack03_Top = "¶ŽèŽñ";
static const char* strAttack03_Btm = "¶‚Ð‚¶";

static const char* strAttack04_Top = "‰EŽèŽñ";
static const char* strAttack04_Btm = "‰E‚Ð‚¶";

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHongKong::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	mIsDashAttack = FALSE;

	// “Gƒx[ƒXƒNƒ‰ƒX‚ÌSetup•K—v
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	mFlinchMax = 250;
}

//---------------------------------------------------
/**
 * ƒLƒƒƒ‰‚ÌUŒ‚ƒXƒe[ƒ^ƒX‘JˆÚ‚Ìƒ`ƒFƒbƒNŠÖ”
 */
//---------------------------------------------------
bool
CharaEnemyHongKong::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	float dist = VSize(vec);
	
	int rand;
	bool result = false;

	mIsDashAttack = FALSE;

	if (dist < (mBodySize * 3.0f)) {
		rand = GetRand(6);
		if (rand == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_2;
			result = true;
		}
		else if (rand == 1) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_4;
			result = true;
		}
	}
	else if (dist < (mBodySize * 16.0f) && (mMotion.mIndex == eMT_RUN) ) {
		*pAttackCharaState = eCHARA_STATE_ATTACK_5;
		result = true;
		mIsDashAttack = TRUE;
	}
	else if (dist < (mBodySize * 8.0f) ) {
		rand = GetRand(50);
		//		if ((rand >= 0) && (rand <= 1)) {
		if (rand == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_5;
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
CharaEnemyHongKong::Update(void)
{
	CharaEnemy::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHongKong::phaseEnterStateMove(void)
{
	const tEnemyWork* p_work = GetEnemyWorkTable(eENEMY_KIND_HONGKONG);

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	float dist = VSize(vec);

	// ‰“‚³‚É‰ž‚¶‚ÄˆÚ“®—Ê‚ð•Ï‰»‚³‚¹‚é
	if (dist > (mBodySize * 15.0f)) {
		mMoveSpeed = p_work->mMoveSpeed * 2.5f;
		StartMotion(eMT_RUN, true, 0.5f);
	}
	else {
		mMoveSpeed = p_work->mMoveSpeed;
		StartMotionEx(&sMotionParam[eMT_WALK]);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHongKong::phaseEnterStateAttack1(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack01_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack01_Btm);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_EXTEND_ATTACK_1, false, 0.4f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHongKong::phaseStateAttack1(void)
{
	static const float sStart		= 6.0f;
	static const float sEnd			= 10.0f;
	static const float sSphereSize	= 50.0f;

	float move_speed = 0.06f * (100 - GetMotionTimePercent());
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	if (BetweenPhaseFrame(sStart, sEnd)) {

		CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		/* í“¬•s”\ŽÒ‚ÆŠù‚ÉUŒ‚‚³‚ê‚½ŽÒ‚ðœ‚­ */
		if (p_player->CheckEnableAttack() == true) {

			// “G‚Ìƒm[ƒh‚ÆƒLƒƒƒ‰‚Ì‰~’Œ‚Ì“–‚½‚è”»’è
			CollisionManager::tSphereWork sphere;
			this->GetBodyFrameSphere(&sphere, strAttack01_Top, sSphereSize);

			CollisionManager::tPillarWork piller;
			p_player->GetBodyPillar(&piller);

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
	GetFramePosFromName(&weapon_top_pos, strAttack01_Top);
	GetFramePosFromName(&weapon_btm_pos, strAttack01_Btm);
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

/*=====================================*
 * phaseEnterStateAttack2
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHongKong::phaseEnterStateAttack2(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack02_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack02_Btm);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_EXTEND_ATTACK_2, false, 1.0f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHongKong::phaseStateAttack2(void)
{
	static const float sStart		= 5.0f;
	static const float sEnd			= 43.0f;
	static const float sSphereSize	= 50.0f;

	float move_speed = 0.06f * (100 - GetMotionTimePercent());
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	if (BetweenPhaseFrame(sStart, sEnd)) {

		CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		/* í“¬•s”\ŽÒ‚ÆŠù‚ÉUŒ‚‚³‚ê‚½ŽÒ‚ðœ‚­ */
		if (p_player->CheckEnableAttack() == true) {

			// “G‚Ìƒm[ƒh‚ÆƒLƒƒƒ‰‚Ì‰~’Œ‚Ì“–‚½‚è”»’è
			CollisionManager::tSphereWork sphere;
			this->GetBodyFrameSphere(&sphere, strAttack02_Top, sSphereSize);

			CollisionManager::tPillarWork piller;
			p_player->GetBodyPillar(&piller);

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
	GetFramePosFromName(&weapon_top_pos, strAttack02_Top);
	GetFramePosFromName(&weapon_btm_pos, strAttack02_Btm);
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

/*=====================================*
 * phaseEnterStateAttack3
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHongKong::phaseEnterStateAttack3(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack03_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack03_Btm);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_EXTEND_ATTACK_3, false, 0.4f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHongKong::phaseStateAttack3(void)
{
	static const float sSphereSize	= 50.0f;

	float move_speed = 0.06f * (100 - GetMotionTimePercent());
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	const char* top_name = NULL;
	const char* btm_name = NULL;

	if (BetweenPhaseFrame(0, 11)) {
		top_name = strAttack03_Top;
		btm_name = strAttack03_Btm;
	}
	else if (BetweenPhaseFrame(12, 18)) {
		top_name = strAttack04_Top;
		btm_name = strAttack04_Btm;
	}

	if (top_name != NULL) {
		CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		/* í“¬•s”\ŽÒ‚ÆŠù‚ÉUŒ‚‚³‚ê‚½ŽÒ‚ðœ‚­ */
		if (p_player->CheckEnableAttack() == true) {

			// “G‚Ìƒm[ƒh‚ÆƒLƒƒƒ‰‚Ì‰~’Œ‚Ì“–‚½‚è”»’è
			CollisionManager::tSphereWork sphere;
			this->GetBodyFrameSphere(&sphere, top_name, sSphereSize);

			CollisionManager::tPillarWork piller;
			p_player->GetBodyPillar(&piller);

			if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {
				VECTOR damage_down_vec = VNorm(VSub(p_player->Position(), mPosition));
				p_player->ProcessDamage(1, false, NULL, &damage_down_vec, false, eATTACK_TYPE_ENEMY_NORMAL);

				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			}
		}
		VECTOR weapon_top_pos, weapon_btm_pos;
		GetFramePosFromName(&weapon_top_pos, top_name);
		GetFramePosFromName(&weapon_btm_pos, btm_name);
		mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

		mIsAttack = true;
	}
	else {
		mIsAttack = false;
	}


	if (GetIsEndMotion()) {
		if (GetRand(1) == 0) {
			mBladeEffect.Stop();
			ChangeState(eCHARA_STATE_IDLE);
		} else {
			mBladeEffect.Stop();
			ChangeState(eCHARA_STATE_ATTACK_4);
		}
	}
}

/*=====================================*
 * phaseEnterStateAttack4
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHongKong::phaseEnterStateAttack4(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack02_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack02_Btm);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_EXTEND_ATTACK_4, false, 0.8f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHongKong::phaseStateAttack4(void)
{
	static const float sSphereSize	= 50.0f;

	const char* top_name = NULL;
	const char* btm_name = NULL;
	bool is_damage_down	 = false;
	bool is_smash		 = false;

	if (BetweenPhaseFrame(10, 16)) {
		top_name = strAttack02_Top;
		btm_name = strAttack02_Btm;
	}
	else if (BetweenPhaseFrame(22, 30)) {
		top_name = strAttack03_Top;
		btm_name = strAttack03_Btm;
	}
	else if (BetweenPhaseFrame(38, 54)) {
		top_name = strAttack02_Top;
		btm_name = strAttack02_Btm;
		is_damage_down = true;
		is_smash = true;
	}
	
	if (BetweenPhaseFrame(10, 54)) {
		float move_speed = 0.2f * (GetMotionTimePercent());
		mMoveVec.x += -(sin(mRotation.y) * move_speed);
		mMoveVec.z += -(cos(mRotation.y) * move_speed);
	}

	if (EqualPhaseFrame(22)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}

	if (top_name != NULL) {
		CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		/* í“¬•s”\ŽÒ‚ÆŠù‚ÉUŒ‚‚³‚ê‚½ŽÒ‚ðœ‚­ */
		if (p_player->CheckEnableAttack() == true) {

			// “G‚Ìƒm[ƒh‚ÆƒLƒƒƒ‰‚Ì‰~’Œ‚Ì“–‚½‚è”»’è
			CollisionManager::tSphereWork sphere;
			this->GetBodyFrameSphere(&sphere, top_name, sSphereSize);

			CollisionManager::tPillarWork piller;
			p_player->GetBodyPillar(&piller);

			if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {
				int attack_power = 1;
				if (is_smash == true) {
					EFK_HANDLE efk_handle = NULL;
					efk_handle = EffekseerManager::GetInstance()->Play(eEFK_BREAK_BARRIER, &sphere.mPosition, 6.0f);
					EffekseerManager::GetInstance()->SetSpeed(efk_handle, 1.0f);

					attack_power = 3;
				}

				VECTOR damage_down_vec = VNorm(VSub(p_player->Position(), mPosition));
				p_player->ProcessDamage(attack_power, false, &sphere.mPosition, &damage_down_vec, is_damage_down, eATTACK_TYPE_ENEMY_NORMAL);

				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			}
		}
		VECTOR weapon_top_pos, weapon_btm_pos;
		GetFramePosFromName(&weapon_top_pos, top_name);
		GetFramePosFromName(&weapon_btm_pos, btm_name);
		mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

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

/*=====================================*
 * phaseEnterStateAttack5
 * ƒWƒƒƒ“ƒvR‚è
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHongKong::phaseEnterStateAttack5(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, strAttack02_Top);
	this->GetFramePosFromName(&weapon_btm_pos, strAttack02_Btm);
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_EXTEND_ATTACK_5, false, 0.46f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyHongKong::phaseStateAttack5(void)
{
	static const float sSphereSize	= 50.0f;

	const char* top_name = NULL;
	const char* btm_name = NULL;

	if (BetweenPhaseFrame(10, 33)) {
		top_name = strAttack02_Top;
		btm_name = strAttack02_Btm;
	}
	
	float move_speed;
	if (mIsDashAttack == TRUE) {
		move_speed = mMoveSpeed;
	}
	else {
		move_speed = 0.40f * (GetMotionTimePercent());
	}

	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	if (top_name != NULL) {
		CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		/* í“¬•s”\ŽÒ‚ÆŠù‚ÉUŒ‚‚³‚ê‚½ŽÒ‚ðœ‚­ */
		if (p_player->CheckEnableAttack() == true) {

			// “G‚Ìƒm[ƒh‚ÆƒLƒƒƒ‰‚Ì‰~’Œ‚Ì“–‚½‚è”»’è
			CollisionManager::tSphereWork sphere;
			this->GetBodyFrameSphere(&sphere, top_name, sSphereSize);

			CollisionManager::tPillarWork piller;
			p_player->GetBodyPillar(&piller);

			if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {
				VECTOR damage_down_vec = VNorm(VSub(p_player->Position(), mPosition));
				int attack_power = 1;

				EFK_HANDLE efk_handle = NULL;
				efk_handle = EffekseerManager::GetInstance()->Play(eEFK_BREAK_BARRIER, &sphere.mPosition, 6.0f);
				EffekseerManager::GetInstance()->SetSpeed(efk_handle, 1.0f);
				if (mIsDashAttack == TRUE) {
					attack_power = 4;
				}

				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);

				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
			}
		}
		VECTOR weapon_top_pos, weapon_btm_pos;
		GetFramePosFromName(&weapon_top_pos, top_name);
		GetFramePosFromName(&weapon_btm_pos, btm_name);
		mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

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
CharaEnemyHongKong::phaseStateWaitAttack(void)
{
	if ((GetRand(5) == 0) || (mReserveCharaState == eCHARA_STATE_ATTACK_5)) {
		ChangeState(mReserveCharaState);
		mReserveCharaState = -1;
	}
}

 /**** end of file ****/
