/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 敵赤竜クラス
 * @author SPATZ.
 * @data   2014/01/15 01:59:46
 */
//---------------------------------------------------
#include "CharaEnemyDemon.h"
#include "Charactor/CharaInfo.h"
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
#include "Utility/Utility.h"

/*=====================================*
 * 赤竜クラス
 *=====================================*/
static const tCapsuleWork sCapsuleWork[] = {
	//	{"頭先",	"左目",		220.0f,},
	{"頭",		"頭先",		100.0f,},
	{"口",		"舌５",		120.0f,},
	{"左腕",	"左手首",	120.0f,},
	{"右腕",	"右手首",	120.0f,},
	{"左股間",	"左足首",	120.0f,},
	{"右股間",	"右足首",	120.0f,},
	{"首Ａ",	"尻尾１",	400.0f,},

};
static const int kCAPSULE_NUM_DEMON = sizeof(sCapsuleWork) / sizeof(tCapsuleWork);

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyDemon::CharaEnemyDemon()
{
	mStateCount = 0;
	mStatePhase = 0;
	mThunderEffIndex = -1;
	mEfkHandle = NULL;
	memset(mMeteoRandIndex, 0, sizeof(mMeteoRandIndex));

	mAdviceState = eADVICE_STATE_ADVICE_A;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyDemon::~CharaEnemyDemon()
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	// 固有の描画を指定する
	this->SetDrawFunc((ModelBase::DRAW_FUNC)DrawDemonFunc);
	
	for (int iI = 0; iI < kDEMON_METEO_NUM; iI ++) {
		mMeteoObject[iI].Setup(eOBJECT_KIND_METEO, (eItemKind)0, (VECTOR *)&ZERO_VEC, 0, true);
	}
	mRefrectObject.Setup(eOBJECT_KIND_METEO_REFRECT, (eItemKind)0, (VECTOR *)&ZERO_VEC, 0, true);

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::Update(void)
{
	ShapeCapsule capsuleL, capsuleR;

	this->GetFramePosFromName(&capsuleL.mPosA, "左目");
	capsuleL.mPosB = capsuleL.mPosA;
	capsuleL.mPosB.y += 1.0f;
	capsuleL.mRadius = 160.0f;
#ifdef _DEBUG
	GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleL);
#endif // #ifdef _DEBUG
	this->GetFramePosFromName(&capsuleR.mPosA, "右目");
	capsuleR.mPosB = capsuleR.mPosA;
	capsuleR.mPosB.y += 1.0f;
	capsuleR.mRadius = 160.0f;
#ifdef _DEBUG
	GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleR);
#endif // #ifdef _DEBUG
	ShapeCapsule capsuleB;
	this->GetFramePosFromName(&capsuleB.mPosA, "右ひじ");
	this->GetFramePosFromName(&capsuleB.mPosB, "右中指１");
	capsuleB.mRadius = (mScale.x / 1.0f);
#ifdef _DEBUG
	GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG

	CharaEnemy::Update();
}

//---------------------------------------------------
/**
   ダメージを食らった時の処理
 */
//---------------------------------------------------
void
CharaEnemyDemon::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	CharaEnemy::ProcessDamage(attackPower, isSetMode, pEffPos, pDamageDownVec, false/*isDamageDown*/, attackType);

	if (mHitpoint != kSYSTEM_DAMAGE_VALUE) {
		SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_DAMAGE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CharaEnemyDemon::CheckDamageCollNormal(tCheckDamageWork* pParam)
{
	return false;
}


//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CharaEnemyDemon::CheckDamageCollArrow(tCheckDamageWork* pParam)
{
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::phaseStateIdle(void)
{
	if (mThunderEffIndex != -1) {
		EffectManager::GetInstance()->Stop(mThunderEffIndex, 0.5f);
		mThunderEffIndex = -1;
	}

	CharaEnemy::phaseStateIdle();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::phaseStateMove(void)
{
	/* 攻撃のステータスに遷移できるかを判定し、できる場合は遷移する */
	int next_state;
	if (checkChangeAttackState(&next_state)) {
		mReserveCharaState = next_state;
		ChangeState(eCHARA_STATE_WAIT_ATTACK);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::phaseEnterStateAttack1(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	const tMotionParam param = {eMT_DEMON_ATTACK_1, false, 0.5f, NULL,};
	StartMotionEx(&param);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::phaseStateAttack1(void)
{
	static float sStart = 28.0f;
	static float sEnd = 69.0f;

	bool is_prev_attack;

	if (EqualPhaseFrame(40.0f)) {
		const float size = 160.0f;
		VECTOR effpos;
		this->GetFramePosFromName(&effpos, "左中指１");
		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,
															&effpos,
															&VGet(size, size, size),
															0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 2.0f);

		CameraManager::GetInstance()->StartVibeCamera(0.5f, 6.0f, 8.0f);

		SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
		SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
	}

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
		
			this->GetFramePosFromName(&capsuleB.mPosA, "左ひじ");
			this->GetFramePosFromName(&capsuleB.mPosB, "左中指１");
			capsuleB.mRadius = (mScale.x / 1.0f);
			
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 2;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), mPosition);
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_BREAK_GUARD);
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

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::phaseEnterStateAttack2(void)
{
	mStatePhase = 0;
	mStateCount = 0;

	EFK_HANDLE efk_handle;
	VECTOR effpos;
	const float size = 200.0f;
	effpos = mPosition;
	effpos.y += (mBodyHeight * 0.5f);

	efk_handle = EffekseerManager::GetInstance()->Play(eEFK_TELEKINESIS,
													   &effpos,
													   &VGet(size, size, size));
	EffekseerManager::GetInstance()->SetSpeed(efk_handle, 0.2f);

	CameraManager::GetInstance()->StartVibeCamera(3.0f, 6.0f, 8.0f);
	StartJoypadVibration(DX_INPUT_PAD1, 500, 2000);

	StartMotion(eMT_DEMON_UP_ARM, true, 0.5f, NULL, 0.05f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::phaseStateAttack2(void)
{
	VECTOR hornL, hornR;
	this->GetFramePosFromName(&hornL, "左角");
	this->GetFramePosFromName(&hornR, "右角");

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	ObjectWeapon_Meteo* p_meteo = NULL;

	float thunder_scale = 600.0f;

	int meteo_num = 1;
	switch (mHitpoint) {
	case 8:	meteo_num = 2;	break;
	case 6:	meteo_num = 3;	break;
	case 4:	meteo_num = 4;	break;
	case 2:	meteo_num = 5;	break;
	};

	switch (mStatePhase) {
	case 0:
		if (GetIsMotionBlend() == false) {
			mStatePhase ++; mStateCount = 0;
		}
		break;

	case 1:
		{
			// メテオの発射順番をソートして決める
			memset(mMeteoRandIndex, 0, sizeof(mMeteoRandIndex));
			for (int iI = 0; iI < meteo_num;) {
				int get_index = GetRand(meteo_num - 1);
				if (mMeteoRandIndex[get_index] == NULL) {
					mMeteoRandIndex[get_index] = iI;
					iI ++;
				}
			}

		}
		mStatePhase ++; mStateCount = 0;
		break;

	case 2:
		{
			if (mStateCount == 20) {
				SoundManager::GetSndComponent()->PlaySound(eSE_MAGIC);
				SoundManager::GetSndComponent()->PlaySound(eSE_DARK);

				// 魔方陣エフェクトの表示
				VECTOR magic_circle_pos;
				VECTOR rot_vec;
				magic_circle_pos = VScale(VAdd(hornL, hornR), 0.5f);
				EFK_HANDLE efk_handle = EffekseerManager::GetInstance()->Play(eEFK_MAGIC_SQUARE, 
																			   &magic_circle_pos, 
																			   &VGet(200.0f, 200.0f, 200.0f));
				rot_vec = VNorm(VSub(hornL, hornR));
				EffekseerManager::GetInstance()->SetRotationAxis(efk_handle, &rot_vec, Utility::DegToRad(0.0f));
			}

			static const char sMeteRad1[1] = {0};
			static const char sMeteRad2[2] = {-30, 30};
			static const char sMeteRad3[3] = {-45, 0, 45};
			static const char sMeteRad4[4] = {-50, -20, 20, 50};
			static const char sMeteRad5[5] = {-60, -30, 0, 30, 60};
			static const char* spMeteoRadTable[5] = {
				sMeteRad1,
				sMeteRad2,
				sMeteRad3,
				sMeteRad4,
				sMeteRad5,
			};
		
			if (mStateCount == 139) {
				StartMotion(eMT_DEMON_INDICATE_FINGER, false, 1.0f, NULL, 0.1f);
				SoundManager::GetSndComponent()->PlaySound(eSE_BOOMERANG_RUN);
				for (int iI = 0; iI < meteo_num; iI ++) {
					p_meteo = &mMeteoObject[iI];
					p_meteo->ProcessCharge();
				}

				// ナビォアドバイスの処理
				if (mAdviceState == eADVICE_STATE_ADVICE_C) {
					if (mHitpoint != mMaxHitpoint) {
						ActionTaskManager::tTaskInfo info = {ActionTaskManager::eTASK_NAVI_EX, (long)strNaviTalk_DemonAdviceC, NULL};
						ActionTaskManager::GetInstance()->NotifyAction(&info);
						mAdviceState = eADVICE_STATE_ADVICE_END;
					}
				}

			}
			for (int iI = 0; iI < meteo_num; iI ++) {
				if (mStateCount == (10 * (iI + 1))) {
					p_meteo = &mMeteoObject[iI];
					
					VECTOR tmp_pos;
					const char* p_tabletop = spMeteoRadTable[(meteo_num - 1)];
					float set_deg = p_tabletop[mMeteoRandIndex[iI]];
					float set_rad = Utility::DegToRad(set_deg);
					float set_dist = 3000.0f;
					float set_height = 0.0f;

					tmp_pos = mPosition;
					//					tmp_pos = p_player->Position();
					PRINT_CONSOLE("before dist = %.2f, deg = %.2f  \n", set_dist, Utility::RadToDeg(set_rad));
					set_dist = (set_dist / cos(set_rad));
					PRINT_CONSOLE("after  dist = %.2f, deg = %.2f  \n", set_dist, Utility::RadToDeg(set_rad));
					set_rad += mRotation.y;

					tmp_pos.x -= (sin(set_rad) * set_dist);
					tmp_pos.z -= (cos(set_rad) * set_dist);

					p_meteo->ProcessFloat(tmp_pos);

				} else if (mStateCount == (140 + (30 * iI))) {
					p_meteo = &mMeteoObject[iI];

					VECTOR move_vec;
					float move_speed = 55.0f;
					move_vec = VScale(VNorm(VSub(p_player->Position(), p_meteo->Position())), move_speed);
					p_meteo->ProcessShoot(move_vec);

					if (iI == (meteo_num - 1)) {
						mStatePhase ++; mStateCount = 0;
					}
				}
			}
		}

		break;

	case 3:
		{
			int stop_num = 0;
			int break_num = 0;
			for (int iI = 0; iI < meteo_num; iI ++) {
				p_meteo = &mMeteoObject[iI];
				if (p_meteo->State() == ObjectWeapon_Meteo::eWEAPON_METEO_STATE_BREAKED) {
					break_num ++;
				}
				if (p_meteo->GetIsRetire()) {
					stop_num ++;
				}
			}
			if (break_num == meteo_num) {
				mReserveCharaState = eCHARA_STATE_ATTACK_3;
				ChangeState(eCHARA_STATE_WAIT_ATTACK);
			}
			else if (stop_num == meteo_num) {

				if (mAdviceState == eADVICE_STATE_ADVICE_A) {
					ActionTaskManager::tTaskInfo info = {ActionTaskManager::eTASK_NAVI_EX, (long)strNaviTalk_DemonAdviceA, NULL};
					ActionTaskManager::GetInstance()->NotifyAction(&info);
					mAdviceState = eADVICE_STATE_ADVICE_B;
				}
				
				ChangeState(eCHARA_STATE_IDLE);
			}
		}

		break;

	default:
		break;
	};

	mStateCount ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::phaseEnterStateAttack3(void)
{
	mStatePhase = 0;
	mStateCount = 0;

	CameraManager::GetInstance()->StartVibeCamera(3.0f, 6.0f, 8.0f);
	StartJoypadVibration(DX_INPUT_PAD1, 500, 2000);

	StartMotion(eMT_DEMON_UP_ARM, true, 0.5f, NULL, 0.05f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::phaseStateAttack3(void)
{
	VECTOR hornL, hornR;
	this->GetFramePosFromName(&hornL, "左角");
	this->GetFramePosFromName(&hornR, "右角");

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	ObjectWeapon_MeteoRefrect* p_meteo = NULL;
	p_meteo = &mRefrectObject;
	
	float thunder_scale = 600.0f;

	int meteo_num = 1;

	switch (mStatePhase) {
	case 0:
		if (GetIsMotionBlend() == false) {
			mStatePhase ++; mStateCount = 0;
		}
		break;

	case 1:
		mStatePhase ++; mStateCount = 0;
		break;

	case 2:
		{
			if (mStateCount == 20) {
				SoundManager::GetSndComponent()->PlaySound(eSE_MAGIC);
				SoundManager::GetSndComponent()->PlaySound(eSE_DARK);

				// 魔方陣エフェクトの表示
				VECTOR magic_circle_pos;
				VECTOR rot_vec;
				magic_circle_pos = VScale(VAdd(hornL, hornR), 0.5f);
				EFK_HANDLE efk_handle = EffekseerManager::GetInstance()->Play(eEFK_MAGIC_SQUARE, 
																			   &magic_circle_pos, 
																				&VGet(200.0f, 200.0f, 200.0f));
				rot_vec = VNorm(VSub(hornL, hornR));
				EffekseerManager::GetInstance()->SetRotationAxis(efk_handle, &rot_vec, Utility::DegToRad(0.0f));
				
				VECTOR finger_pos;
				GetFramePosFromName(&finger_pos, "右人指３");
				p_meteo->Position() = finger_pos;
				p_meteo->ProcessStarEffect();
			}

			static const char sMeteRad1[1] = {0};
			static const char* spMeteoRadTable[1] = {sMeteRad1};
		
			if (mStateCount == 59) {
				StartMotion(eMT_DEMON_INDICATE_FINGER, false, 1.0f, NULL, 0.1f);
				SoundManager::GetSndComponent()->PlaySound(eSE_BOOMERANG_RUN);

				if ((mAdviceState == eADVICE_STATE_ADVICE_A) ||
					(mAdviceState == eADVICE_STATE_ADVICE_B)) {
					ActionTaskManager::tTaskInfo info = {ActionTaskManager::eTASK_NAVI_EX, (long)strNaviTalk_DemonAdviceB, NULL};
					ActionTaskManager::GetInstance()->NotifyAction(&info);
					mAdviceState = eADVICE_STATE_ADVICE_C;
				}

			}
			if (mStateCount == 60) {
				VECTOR move_vec;
				float move_speed = 55.0f;
				move_vec = VScale(VNorm(VSub(p_player->Position(), p_meteo->Position())), move_speed);
				p_meteo->ProcessShoot(move_vec);
				
				mStatePhase ++; mStateCount = 0;
			}
		}

		break;

	case 3:
		{
			int stop_num = 0;
			p_meteo = &mRefrectObject;
			if (p_meteo->GetIsRetire()) {

				ChangeState(eCHARA_STATE_IDLE);
			}
		}

		break;

	default:
		break;
	};

	mStateCount ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::phaseEnterStateAttack4(void)	
{
	mStatePhase = 0;
	mStateCount = 0;

	// 固有の描画を指定する
	this->SetDrawFunc(NULL);

	const tMotionParam param = {eMT_DEMON_ATTACK_3, false, 0.3f, NULL,};
	StartMotionEx(&param);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::phaseStateAttack4(void)
{
	if (mMotion.mIndex == eMT_DEMON_ATTACK_3) {
		if (EqualPhaseFrame(16.0f)) {
			SoundManager::GetSndComponent()->PlaySound(eSE_PUT_SWORD);
		}
		if (EqualPhaseFrame(45.0f)) {
			SoundManager::GetSndComponent()->PlaySound(eSE_ROLLING);
		}
		if (EqualPhaseFrame(48.0f)) {
			const float size = 340.0f;
			VECTOR effpos;
			this->GetFramePosFromName(&effpos, "右剣先");
			int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST, &effpos, &VGet(size, size, size),	0.0f);
			EffectManager::GetInstance()->Stop(eff_handle, 2.0f);

			CameraManager::GetInstance()->StartVibeCamera(2.0f, 8.0f, 10.0f);
			SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
			SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
		}
		if (BetweenPhaseFrame(45.0f, 48.0f)) {
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
		
				this->GetFramePosFromName(&capsuleB.mPosA, "右ダミー");
				this->GetFramePosFromName(&capsuleB.mPosB, "右剣先");
				capsuleB.mRadius = (mScale.x / 1.5f);
			
				if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
					SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
					int attack_power = 4;
					VECTOR damage_down_vec;
					damage_down_vec = VSub(p_player->Position(), mPosition);
					damage_down_vec = VNorm(damage_down_vec);
					p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_BREAK_GUARD);
				}
			}
		}

		if (this->GetIsEndMotion()) {
			const tMotionParam param = {eMT_DEMON_ATTACK_4, true, 0.5f, NULL,};
			StartMotionEx(&param);
		}
	} else if (mMotion.mIndex == eMT_DEMON_ATTACK_4) {
		if (mStateCount > 90) {
			// 固有の描画を指定する
			this->SetDrawFunc((ModelBase::DRAW_FUNC)DrawDemonFunc);
			ChangeState(eCHARA_STATE_IDLE);
		}
		mStateCount ++;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::phaseEnterStateAttack5(void)	
{
	mStatePhase = 0;
	mStateCount = 0;

	SoundManager::GetSndComponent()->PlaySound(eSE_MONSTER_VOICE);

	const tMotionParam param = {eMT_DEMON_ATTACK_5, false, 0.20f, NULL,};
	StartMotionEx(&param);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::phaseStateAttack5(void)
{
	if (this->EqualPhaseFrame(8.0f)) {
		mStateCount = 30;
		this->SetMotionSpeed(0.025f);
	}

	if (this->BetweenPhaseFrame(8.0f, 9999.90f)) {
		if (mStateCount > 30) {
			const float size = 160.0f;
			VECTOR effpos;
			this->GetFramePosFromName(&effpos, "右中指１");
			int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST, &effpos, &VGet(size, size, size),	0.0f);
			EffectManager::GetInstance()->Stop(eff_handle, 2.0f);

			CameraManager::GetInstance()->StartVibeCamera(0.5f, 6.0f, 8.0f);
			SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
			SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
			mStateCount = 0;
		}
		
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
			this->GetFramePosFromName(&capsuleB.mPosB, "右中指１");
			capsuleB.mRadius = (mScale.x / 1.0f);
			
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 4;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), mPosition);
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_BREAK_GUARD);
			}
		}

		if (GetIsEndMotion()) {
			ChangeState(eCHARA_STATE_IDLE);
		}
		mStateCount ++;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::phaseEnterStateAttack6(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::phaseStateAttack6(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::phaseStateDamage(void)
{
	if (GetMotionIndex() != eMT_DAMAGE) {
		ChangeState(eCHARA_STATE_ATTACK_5);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::phaseEnterStateRetireDoing(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_RESET);
	p_player->processCancelArchery();

	if (mHitpoint != kSYSTEM_DAMAGE_VALUE) {
		if (mThunderEffIndex != -1) {
			EffectManager::GetInstance()->Stop(mThunderEffIndex, 0.75f);
			mThunderEffIndex = -1;
		}

		VECTOR eff_pos;
		this->GetFramePosFromName(&eff_pos, "首");

		const float size = 30.0f * 2.0f;
		mThunderEffIndex = EffectManager::GetInstance()->Play(eEF_CHARGE,
														&eff_pos,
														&VGet(size, size, size),
														0.0f);
		EffectManager::GetInstance()->SetFloat(mThunderEffIndex, "particleSpeed", -2.6f);

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

	mStatePhase = 0;
	mStateCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyDemon::phaseStateRetireDoing(void)
{
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

			if (mThunderEffIndex != -1) {
				EffectManager::GetInstance()->Stop(mThunderEffIndex, 1.0f);
				mThunderEffIndex = -1;
			}

			VECTOR eff_pos;
			this->GetFramePosFromName(&eff_pos, "首");
			mEfkHandle = EffekseerManager::GetInstance()->Play(eEFK_BOSS_DEATH,
															   &eff_pos,
															   &VScale(VGet(12.0f, 12.0f, 12.0f), 16.0f));
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
CharaEnemyDemon::phaseEnterStateRetireEnd(void)
{
	GraphicManager::GetInstance()->ReserveRemoveModel((ModelBase *)this);
	CollisionManager::GetInstance()->RemoveBodyCollision((ModelBase *)this);
	CollisionManager::GetInstance()->RemoveAttackCollision((ModelBase *)this);

	this->SetAlpha(0.0f);

	ModelPmd::DeleteModel();
	mIsActive = false;

	GraphicManager::GetInstance()->RemoveDrawShadow(&mShadow);


}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CharaEnemyDemon::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	float dist = VSize(vec);
	
	int rand;
	bool result = false;

	rand = GetRand(45);
	if ( ( (rand == 0) || (false) ) && (mHitpoint <= 8) ){
		*pAttackCharaState = eCHARA_STATE_ATTACK_1;		// 左手攻撃
		result = true;
	} else if ( (rand == 2) || (false) ){
		*pAttackCharaState = eCHARA_STATE_ATTACK_2;		// 岩攻撃
		result = true;
	} else if ( (rand == 5) && (mHitpoint <= 4) ){
		*pAttackCharaState = eCHARA_STATE_ATTACK_4;
		result = true;
	} else if ( (rand == 6) && (mHitpoint <= 6) ){

	}

	return result;
}

 /**** end of file ****/

