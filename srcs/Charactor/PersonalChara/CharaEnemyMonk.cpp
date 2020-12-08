/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * •“¹‰Æƒtƒ@ƒCƒ‹
 * @author SPATZ.
 * @data   2012/05/25 00:27:32
 */
//---------------------------------------------------
#include "CharaEnemyMonk.h"
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
CharaEnemyMonk::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
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
CharaEnemyMonk::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	float dist = VSize(vec);
	
	int rand;
	bool result = false;

	if (dist < mBodySize * 1.5f) {
		rand = GetRand(6);

		if (p_player->GetIsAttack()) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_4;
			result = true;
		} else if ((rand == 0) ||
				   (rand == 1)) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
			result = true;
		} else if ((rand == 2) ||
				   (rand == 3)) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_2;
			result = true;
		} else if (rand == 4) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_3;
			result = true;
		}
	} else if (dist < mBodySize * 6.0f) {
		rand = GetRand(30);
		if (rand == 0) {
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
CharaEnemyMonk::Update(void)
{
	CharaEnemy::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMonk::phaseEnterStateAttack1(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "‰E’†Žw‚R");
	this->GetFramePosFromName(&weapon_btm_pos, "‰EŽèŽñ");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_MONK_ATTACK_1, false, 0.3f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMonk::phaseStateAttack1(void)
{
	static float sStart = 4.0f;
	static float sEnd = 9.0f;

	bool is_prev_attack;
	is_prev_attack = mIsAttack;
	if ((GetMotionNowTime() >= sStart) &&
		(GetMotionNowTime() <= sEnd)) {
		mIsAttack = true;

		CharaBase* p_player;
		p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		/* í“¬•s”\ŽÒ‚ÆŠù‚ÉUŒ‚‚³‚ê‚½ŽÒ‚ðœ‚­ */
		if ((p_player->GetIsRetire() == false) &&
			(p_player->IsDamage() == false)) {

			// “G‚Ìƒm[ƒh‚ÆƒLƒƒƒ‰‚Ì‰~’Œ‚Ì“–‚½‚è”»’è
			CollisionManager::tSphereWork sphere;
			CollisionManager::tPillarWork piller;
			this->GetFramePosFromName(&sphere.mPosition, "‰E’†Žw‚R");
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
	this->GetFramePosFromName(&weapon_top_pos, "‰E’†Žw‚R");
	this->GetFramePosFromName(&weapon_btm_pos, "‰EŽèŽñ");
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
CharaEnemyMonk::phaseEnterStateAttack2(void)
{
	//	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_3);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "¶‚Â‚Üæ");
	this->GetFramePosFromName(&weapon_btm_pos, "¶‘«");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_MONK_ATTACK_2, false, 0.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMonk::phaseStateAttack2(void)
{
	static float sStart = 24.0f;
	static float sEnd = 49.0f;

	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	if (this->EqualPhaseFrame(24.0f)) {
		SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
		SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
	}
	if (this->EqualPhaseFrame(43.0f)) {
		SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_3);
		SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
	}

	if ((GetMotionNowTime() >= sStart) &&
		(GetMotionNowTime() <= sEnd)) {
		mIsAttack = true;

		CharaBase* p_player;
		p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		/* í“¬•s”\ŽÒ‚ÆŠù‚ÉUŒ‚‚³‚ê‚½ŽÒ‚ðœ‚­ */
		if ((p_player->GetIsRetire() == false) &&
			(p_player->IsDamage() == false)) {

			// “G‚Ìƒm[ƒh‚ÆƒLƒƒƒ‰‚Ì‰~’Œ‚Ì“–‚½‚è”»’è
			CollisionManager::tSphereWork sphere;
			CollisionManager::tPillarWork piller;
			this->GetFramePosFromName(&sphere.mPosition, "¶‚Â‚Üæ");
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
				if (GetMotionNowTime() >= 43.0f) {
					// ‚Q”­–Ú‚Í‚Ó‚Á‚ÆƒoƒX
					p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
				} else {
					p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, false, eATTACK_TYPE_ENEMY_NORMAL);
				}
			}

		}

	} else {
		mIsAttack = false;
	}
	PRINT_SCREEN(GetColor(255, 255, 0), "NowTime : %.2f\n", GetMotionNowTime());

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "¶‚Â‚Üæ");
	this->GetFramePosFromName(&weapon_btm_pos, "¶‘«");
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
CharaEnemyMonk::phaseEnterStateAttack3(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_2);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "¶’†Žw‚R");
	this->GetFramePosFromName(&weapon_btm_pos, "¶ŽèŽñ");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_MONK_ATTACK_3, false, 0.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMonk::phaseStateAttack3(void)
{
	static float sStart = 10.0f;
	static float sEnd = 25.0f;

	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	float move_speed = 0.24f * (100 - GetMotionTimePercent());
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);

	if (this->EqualPhaseFrame(10.0f)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	}

	if ((GetMotionNowTime() >= sStart) &&
		(GetMotionNowTime() <= sEnd)) {
		mIsAttack = true;

		CharaBase* p_player;
		p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		/* í“¬•s”\ŽÒ‚ÆŠù‚ÉUŒ‚‚³‚ê‚½ŽÒ‚ðœ‚­ */
		if ((p_player->GetIsRetire() == false) &&
			(p_player->IsDamage() == false)) {

			// “G‚Ìƒm[ƒh‚ÆƒLƒƒƒ‰‚Ì‰~’Œ‚Ì“–‚½‚è”»’è
			CollisionManager::tSphereWork sphere;
			CollisionManager::tPillarWork piller;
			this->GetFramePosFromName(&sphere.mPosition, "¶’†Žw‚R");
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
	this->GetFramePosFromName(&weapon_top_pos, "¶’†Žw‚R");
	this->GetFramePosFromName(&weapon_btm_pos, "¶ŽèŽñ");
	mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

	if (GetIsEndMotion()) {
		mBladeEffect.Stop();
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
 * ‰ñ”ð
 */
//---------------------------------------------------
void
CharaEnemyMonk::phaseEnterStateAttack4(void)
{
	//	StartMotionEx(&sMotionParam[eMT_AWAY_BACK]);
	StartMotion(eMT_MONK_AWAY, false, 0.75f, NULL);

	mBladeEffect.Stop();

	SoundManager::GetSndComponent()->PlaySound(eSE_AWAY);
	//	mIsAway = true;
}

//---------------------------------------------------
/**
 * ‰ñ”ð
 */
//---------------------------------------------------
void
CharaEnemyMonk::phaseStateAttack4(void)
{
	VECTOR dir_vec;
	float move_speed = kCHARA_AWAY_SPEED * (float)(100 - GetMotionTimePercent());

	dir_vec.x = sin(mRotation.y + Utility::DegToRad(0.0f));
	dir_vec.y = 0.0f;
	dir_vec.z = cos(mRotation.y + Utility::DegToRad(0.0f));

	mMoveVec = VScale(dir_vec, move_speed);
	
	/*
	if (CHECK_TRIG_JOY(JOY_INPUT_ATTACK)) {
		ChangeState(eCHARA_STATE_ATTACK_THRUST);
	}
	*/

	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

 /**** end of file ****/
