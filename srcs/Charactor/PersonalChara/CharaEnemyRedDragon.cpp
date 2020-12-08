/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ìGê‘ó≥ÉNÉâÉX
 * @author SPATZ.
 * @data   2014/01/15 01:59:46
 */
//---------------------------------------------------
#include "CharaEnemyRedDragon.h"
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
 * ê‘ó≥ÉNÉâÉX
 *=====================================*/
static const tCapsuleWork sCapsuleWork[] = {
	//	{"ì™",		"ì™êÊ",		100.0f,},
	{"ì™",		"ì™êÊ",		220.0f,},
	//	{"å˚",		"ê„ÇT",		120.0f,},
	{"å˚",		"ê„ÇT",		200.0f,},

	{"ç∂òr",	"ç∂éËéÒ",	120.0f,},
	{"âEòr",	"âEéËéÒ",	120.0f,},
	{"ç∂å“ä‘",	"ç∂ë´éÒ",	120.0f,},
	{"âEå“ä‘",	"âEë´éÒ",	120.0f,},
	{"éÒÇ`",	"êKîˆÇP",	400.0f,},

};
static const int kCAPSULE_NUM_REDDRAGON = sizeof(sCapsuleWork) / sizeof(tCapsuleWork);

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyRedDragon::CharaEnemyRedDragon()
{
	mFlameEffIndex	= -1; 
	mChargeEffIndex	= -1;
	mStateCount	= 0;
	mStatePhase	= 0;
	mSndIndex	= -1;
	mIsScaleFlame	= false;
	mFlameCount = 0;
	mIsCritical = false;
	mEfkHandle			= -1;
	mEfkHandlePiyopiyo	= -1;
	mEffHandle		= -1;
	mSndHandle		= -1;
	mAboutPlayerDot = 0.0f;

	mLockonLength	= 0.0f;
	mIsHitHead		= FALSE;
	
	mFireCount		= 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// ìGÉxÅ[ÉXÉNÉâÉXÇÃSetupïKóv
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	mFlinchMax = 9999;		// ê‚ëŒÇ–ÇÈÇ‹Ç»Ç¢
	mBodySize = 0.0f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::Update(void)
{
	mIsHitHead		= FALSE;

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR enemy_rot, player_to_enemy;
	enemy_rot.x = sin(mRotation.y + Utility::DegToRad(180.0f));
	enemy_rot.y = 0.0f;
	enemy_rot.z = cos(mRotation.y + Utility::DegToRad(180.0f));
	
	player_to_enemy = VNorm(VSub(p_player->Position(), mPosition));
	mAboutPlayerDot = VDot(enemy_rot, player_to_enemy);

	static const float sBaseBodySize = 10.0f;

	float add_body_size = 0.0f;
	// É_ÉEÉìíÜ
	if (mCharaState == eCHARA_STATE_ATTACK_5) {
		//		add_body_size = 900.0f;
		add_body_size = 900.0f;
	}
	// âäÇìfÇ¢ÇƒÇ¢ÇÈä‘
	else if (mCharaState == eCHARA_STATE_ATTACK_1) {
		//		add_body_size = 1000.0f;
		add_body_size = 700.0f;
	}
	// ÇªÇÃëº
	else {
		add_body_size = 700.0f;
	}

	//	const tEnemyWork* p_work = GetEnemyWorkTable(eENEMY_KIND_RED_DRAGON);
	mBodySize = (sBaseBodySize + (add_body_size * abs(mAboutPlayerDot)));

	PRINT_SCREEN(GetColor(255, 255, 0), "Dragon BodySize %.2f", mBodySize);

	CharaEnemy::Update();
}

//---------------------------------------------------
/**
   É_ÉÅÅ[ÉWÇêHÇÁÇ¡ÇΩéûÇÃèàóù
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	if (attackPower == kSYSTEM_DAMAGE_VALUE) {
		CharaEnemy::ProcessDamage(attackPower, isSetMode, pEffPos, pDamageDownVec, isDamageDown, attackType);
		return;
	}

	if (mIsHitHead == FALSE) {
		CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
			
		EFK_HANDLE handle = -1;
		handle = EffekseerManager::GetInstance()->Play(eEFK_SHIELD, pEffPos, 5.0f);
		EffekseerManager::GetInstance()->SetRotationAxisY(handle, (p_player->Rotation().y + (PHI_F/2.0f)));
		
		SoundManager::GetSndComponent()->PlaySound(eSE_SHIELD);
		mIsDamage = true;

		return;
	}

	if (mCharaState != eCHARA_STATE_ATTACK_5) {
		if (mIsCritical == true) {
			attackPower *= 4;

			SoundManager::GetSndComponent()->StopSound(eSE_MONSTER_VOICE);
			SoundManager::GetSndComponent()->StopSound(eSE_FLAME_ARIA);
			SoundManager::GetSndComponent()->StopSound(eSE_FLAME_SHOOT);

			if (mFlameEffIndex != -1) {
				EffectManager::GetInstance()->Stop(mFlameEffIndex);
				mFlameEffIndex = -1;
			}
			if (mChargeEffIndex != -1) {
				EffectManager::GetInstance()->Stop(mChargeEffIndex);
				mChargeEffIndex	= -1;
			}
			GraphicManager::GetInstance()->RevertFogParam(1.0f);
		}
	}

	CharaEnemy::ProcessDamage(attackPower, isSetMode, pEffPos, pDamageDownVec, isDamageDown, attackType);

	if (mCharaState != eCHARA_STATE_RETIRE_START) {
		if (mIsCritical == true) {
			ChangeState(eCHARA_STATE_ATTACK_5);
		}
	}
	else if (mCharaState == eCHARA_STATE_RETIRE_START) {
		if (mFlameEffIndex != -1) {
			EffectManager::GetInstance()->Stop(mFlameEffIndex);
			mFlameEffIndex = -1;
		}
		if (mChargeEffIndex != -1) {
			EffectManager::GetInstance()->Stop(mChargeEffIndex);
			mChargeEffIndex	= -1;
		}
		GraphicManager::GetInstance()->RevertFogParam(1.0f);

		SoundManager::GetSndComponent()->StopSound(eSE_MONSTER_VOICE);
		SoundManager::GetSndComponent()->StopSound(eSE_FLAME_ARIA);
		SoundManager::GetSndComponent()->StopSound(eSE_FLAME_SHOOT);
		SoundManager::GetSndComponent()->StopSound(eSE_PIYOPIYO);
		EffekseerManager::GetInstance()->Stop(mEfkHandlePiyopiyo, 0.0f);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CharaEnemyRedDragon::CheckBodyCollision(CollisionManager::tSphereWork& rOtherSphere, VECTOR& rVec, float& rDist)
{
	CollisionManager::tSphereWork my_sphere;
	/*
	getHeadSphere((CollisionManager::tSphereWork &)my_sphere);
	*/
	my_sphere.mPosition = mPosition;
	my_sphere.mSize		= mBodySize;

	// â~Ç∆â~ÇÃï”ÇËîªíË
	if (CollisionManager::GetInstance()->Check_SphereToSphere(&rOtherSphere, &my_sphere, &rVec, &rDist) ) {
		return true;
	}

	getHeadSphere((CollisionManager::tSphereWork &)my_sphere);
	// â~Ç∆â~ÇÃï”ÇËîªíË
	if (CollisionManager::GetInstance()->Check_SphereToSphere(&rOtherSphere, &my_sphere, &rVec, &rDist) ) {
		return true;
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CharaEnemyRedDragon::CheckDamageCollNormal(tCheckDamageWork* pParam)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	tCheckDamageWork* p_work = (tCheckDamageWork *)pParam;

	ShapeCapsule capsule;
	ShapeLine	line;

	mIsCritical = false;
	
	line.mStart = p_work->mWeaponBtmPos;
	line.mEnd	= p_work->mWeaponTopPos;

	for (int iI = 0; iI < kCAPSULE_NUM_REDDRAGON; iI ++) {
		this->GetFramePosFromName(&capsule.mPosA, sCapsuleWork[iI].mPosA);
		this->GetFramePosFromName(&capsule.mPosB, sCapsuleWork[iI].mPosB);
		capsule.mRadius = sCapsuleWork[iI].mRadius;
#ifdef _DEBUG
		GraphicManager::GetInstance()->DbgRenderCapsule(&capsule);
#endif // _DEBUG
		if (CollisionManager::GetInstance()->Check_LineToCapsule(&line, &capsule)) {

			if (iI <= 1) {
				mIsHitHead = TRUE;
			}

			return true;
		}
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CharaEnemyRedDragon::CheckDamageCollJump(tCheckDamageWork* pParam)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	tCheckDamageWork* p_work = (tCheckDamageWork *)pParam;

	ShapeCapsule capsule;
	ShapeLine	line;

	mIsCritical = false;
	
	line.mStart = p_work->mWeaponBtmPos;
	line.mEnd	= p_work->mWeaponTopPos;

	for (int iI = 0; iI < kCAPSULE_NUM_REDDRAGON; iI ++) {
		this->GetFramePosFromName(&capsule.mPosA, sCapsuleWork[iI].mPosA);
		this->GetFramePosFromName(&capsule.mPosB, sCapsuleWork[iI].mPosB);
		capsule.mRadius = sCapsuleWork[iI].mRadius;
#ifdef _DEBUG
		GraphicManager::GetInstance()->DbgRenderCapsule(&capsule);
#endif // _DEBUG
		if (CollisionManager::GetInstance()->Check_LineToCapsule(&line, &capsule)) {

			if (iI <= 1) {
				mIsHitHead = TRUE;

				// ìGÇ™âäÇìfÇ¢ÇƒÇÈèÍçáÇ»ÇÁÉNÉäÉeÉBÉJÉãÇ…Ç∑ÇÈ
				if (mCharaState == eCHARA_STATE_ATTACK_1) {
					mIsCritical = true;
					VECTOR vec;
					vec = VSub(p_player->Position(), this->mPosition);
					vec = VScale(VNorm(vec), 20.0f);
				
					((CharaPlayer *)p_player)->ProcessTechniqualJump(&vec);
				}
			}

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
CharaEnemyRedDragon::GetLockonPosition(VECTOR* pPosition)
{
	float target_len = 0.0f;

	// É_ÉEÉìíÜ
	if (mCharaState == eCHARA_STATE_ATTACK_5) {
		target_len = 960.0f;
	}
	// âäÇìfÇ¢ÇƒÇ¢ÇÈä‘
	else if (mCharaState == eCHARA_STATE_ATTACK_1) {
		target_len = 760.0f;
	}
	// ÇªÇÃëº
	else {
		//		target_len = 760.0f;
		target_len = 0.0f;
	}

	mLockonLength = ((target_len - mLockonLength) * 0.01f) + mLockonLength;

	*pPosition = mPosition;
	//	pPosition->x += -(sin(mRotation.y) * len);
	//	pPosition->z += -(cos(mRotation.y) * len);
	pPosition->x += -(sin(mRotation.y) * mLockonLength);
	pPosition->z += -(cos(mRotation.y) * mLockonLength);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::phaseEnterStateIdle(void)
{
	this->ResetFrameUserMatrix("éÒÇ`");

	const tEnemyWork* p_work = GetEnemyWorkTable(eENEMY_KIND_RED_DRAGON);
	//<!	mBodySize = p_work->mBodySize;

	CharaEnemy::phaseEnterStateIdle();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::phaseStateTurn(void)
{
	//	mBodySize = 200.0f;
	CharaEnemy::phaseStateTurn();
	//<!	const tEnemyWork* p_work = GetEnemyWorkTable(eENEMY_KIND_RED_DRAGON);
	//<!	mBodySize = p_work->mBodySize;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::phaseStateMove(void)
{
	// çUåÇÇÃÉXÉeÅ[É^ÉXÇ…ëJà⁄Ç≈Ç´ÇÈÇ©ÇîªíËÇµÅAÇ≈Ç´ÇÈèÍçáÇÕëJà⁄Ç∑ÇÈ
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
CharaEnemyRedDragon::phaseEnterStateAttack1(void)
{
	// Ç§ÇÈÇ≥Ç¢
	mSndHandle = SoundManager::GetSndComponent()->PlaySound(eSE_MONSTER_VOICE);
	StartMotion(eMT_ATTACK_1, false, 1.2f, NULL);

	//	mBodySize = 1100.0f;
	mBodySize = 800.0f;

	//	CameraManager::GetInstance()->StartShakeCamera(3.0f, 3.0f, 80.0f);
	CameraManager::GetInstance()->StartVibeCamera(3.0f, 6.0f, 8.0f);

	StartJoypadVibration(DX_INPUT_PAD1, 500, 2000);

	GraphicManager::tFogParam fogParam;
	fogParam.mIsEnable = TRUE;
	fogParam.mIsActive = TRUE;
	fogParam.mColR = 10;
	fogParam.mColG = 10;
	fogParam.mColB = 10;
	fogParam.mNear = 10.0f;
	fogParam.mFar = 10000.0f;
	//	fogParam.mFar = 20.0f;
	GraphicManager::GetInstance()->ChangeFogParam(0.1f, &fogParam);

	mStatePhase = 0; mStateCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::phaseStateAttack1(void)
{
	static const float sFlameEffMaxSize = 1200.0f;
	bool is_prev_attack;
	VECTOR effpos, effvecA, effvecB, flame_vec;
	float effsize = 0;
	float effsize_rate;
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	float percent_rate = ((float)mHitpoint / (float)mMaxHitpoint);

	is_prev_attack = mIsAttack;

	float flameout_length = 0.0f;


	//<! ÉLÉÉÉâëÂÇµÇƒéÒí«è]
	VECTOR neck_pos = p_player->Position();
	//	neck_pos.y += -150.0f;
	neck_pos.y += -150.0f;
	//	neck_pos.y += -350.0f;
	if (mStatePhase == 6) {
	} else {
		this->SetFrameUserRotation("éÒÇ`", &neck_pos, 0.1f);
	}

	this->GetFramePosFromName(&effvecA, "ê„ÇS");
	this->GetFramePosFromName(&effvecB, "ê„ÇT");
	flame_vec = VNorm(VSub(effvecB, effvecA));

	this->GetFramePosFromName(&effpos, "ê„ÇS");

	switch (mStatePhase) {
	case 0:
		/* ÉÇÅ[ÉVÉáÉìÇ™èIÇÌÇÈÇ‹Ç≈ë“Ç¬ */
		if (GetIsEndMotion()) {
			mStatePhase ++; mStateCount = 0;
		}
		break;

	case 1:
		/* å˚ÇÃíÜÇ…âäÇî≠ê∂ */
		SoundManager::GetSndComponent()->PlaySound(eSE_FLAME_ARIA);

		//		effsize = 20.0f;
		effsize = 16.0f;

		/*
		if (mChargeEffIndex != -1) {
			EffectManager::GetInstance()->Stop(mChargeEffIndex, 0.0f);
			mChargeEffIndex = -1;
		}
		if (mFlameEffIndex != -1) {
			EffectManager::GetInstance()->Stop(mFlameEffIndex, 0.0f);
			mFlameEffIndex = -1;
		}
		*/

		mChargeEffIndex = EffectManager::GetInstance()->Play(eEF_CHARGE,
													   &effpos,
													   &VGet(effsize, effsize, effsize),
													   0.0f);
		EffectManager::GetInstance()->SetMatrixType(mChargeEffIndex, EffectNode::eMATRIX_TYPE_TRS);
		EffectManager::GetInstance()->SetFloat(mChargeEffIndex, "particleSpeed", -2.0f);

		/* íºê⁄É}ÉgÉäÉbÉNÉXÇêGÇÈ */
		mFlameEffIndex = EffectManager::GetInstance()->Play(eEF_FLAME_BOMB,
													   &effpos,
													   &VGet(effsize, effsize, effsize),
													   0.0f);
		setFlameMatrix(0.01f);

		mStatePhase ++; mStateCount = 0;
		break;

	case 2:
		/* å˚ÇÃíÜÇ≈âäÇ™ëÂÇ´Ç≠Ç»ÇÈ */
		EffectManager::GetInstance()->Position(mChargeEffIndex) = effpos;
		EffectManager::GetInstance()->Scale(mChargeEffIndex) = VScale(EffectManager::GetInstance()->Scale(mChargeEffIndex), 1.004f);

		setFlameMatrix(0.01f);

		//		if (mStateCount > 30.0f) {
		//		if (mStateCount > 45.0f) {
		// ÇgÇoÇ™í·Ç¢íˆó≠ÇﬂÇ™íZÇ≠Ç»ÇÈ
		if (mStateCount > (5.0f + (int)(40.0f * percent_rate) )) {
			mStatePhase ++; mStateCount = 0;
		}
		break;

	case 3:
		/* å˚ÇÃíÜÇÃâäÇè¡Ç∑ */
		EffectManager::GetInstance()->Stop(mChargeEffIndex, 0.5f);
		mChargeEffIndex = -1;

		setFlameMatrix(0.01f);

		CameraManager::GetInstance()->StartShakeCamera(0.5f, 0.0f, 0.0f);

		mStatePhase ++; mStateCount = 0;
		break;

	case 4:
		setFlameMatrix(0.01f);
		if (mStateCount > 15) {
			mStatePhase ++; mStateCount = 0;
		}
		break;

	case 5:

		/* âäÇìfÇ´èoÇ∑ */
		SoundManager::GetSndComponent()->PlaySound(eSE_FLAME_SHOOT);
		mSndIndex = eSE_FLAME_SHOOT;

		StartJoypadVibration(DX_INPUT_PAD1, 1000, 1000);

		GraphicManager::tFogParam fogParam;
		fogParam.mIsEnable = TRUE;
		fogParam.mIsActive = TRUE;
		fogParam.mColR = 255;
		fogParam.mColG = 60;
		fogParam.mColB = 50;
		fogParam.mNear = 10.0f;
		//		fogParam.mFar = 1000.0f;
		fogParam.mFar = 15000.0f;
		GraphicManager::GetInstance()->ChangeFogParam(0.5f, &fogParam);

		setFlameMatrix(0.01f);

		mIsScaleFlame = false;

		{
			int frame_count = (int)((1.0f - percent_rate) * 3.0f);
			mFlameCount = GetRand(frame_count) + 1;
		}
		mStatePhase ++; mStateCount = 0;
		break;

	case 6:

#define FLAME_1_FRAME	60.0f
		//#define FLAME_2_FRAME	90.0f
		//#define FLAME_3_FRAME	110.0f
		//#define FLAME_2_FRAME	110.0f - (30.0f * percent_rate)
		//#define FLAME_3_FRAME	FLAME_2_FRAME + 20.0f

		//		flameout_length = 110.0f - (40.0f * (1.0f - percent_rate));
		flameout_length = 110.0f - (50.0f * (1.0f - percent_rate));

		// âäÇñcÇÁÇ‹Ç∑
		if ((mStateCount >= 0) && (mStateCount < FLAME_1_FRAME)) {
			float size_rate = 0;
			size_rate = (float)mStateCount / FLAME_1_FRAME;
			effsize = (sqrt(size_rate) * sFlameEffMaxSize);
			if (effsize > sFlameEffMaxSize) {
				effsize = sFlameEffMaxSize;
			}
		}

		if (mStateCount >= FLAME_1_FRAME) {
			effsize = sFlameEffMaxSize;
		}
		// âäÇìfÇ´ë±ÇØÇÈ
		if ((mStateCount >= FLAME_1_FRAME) && (mStateCount < flameout_length)) {
			mIsScaleFlame = true;
		}
		// âäÇåpë±Ç©ÇÃÉWÉÉÉbÉW
		if (mStateCount ==  (int)flameout_length) {
			if (mFlameCount <= 0) {
				// âäèIóπ
				mStatePhase ++; mStateCount = 0;
				break;
			}
			mFlameCount --;
		}
		// ÉLÉÉÉâÇÃï˚Ç…å¸Ç©ÇπÇÈ
		if ((mStateCount > flameout_length) && (mStateCount < (flameout_length + 20.0f))) {

			if (mAboutPlayerDot < 0.2f) {
				updateRotationToPlayer(0.08f);
			} else {
				this->SetFrameUserRotation("éÒÇ`", &neck_pos, 0.1f);
			}

			effsize = sFlameEffMaxSize * 0.5f;
		}
		// ç≈èâÉãÅ[ÉvÇ≥ÇπÇÈ
		if (mStateCount >= (flameout_length + 20.0f)) {
			mStateCount = 0;
			break;
		}


		if (CameraManager::GetInstance()->IsVibeCamera() == false) {
			CameraManager::GetInstance()->StartVibeCamera(4.0f, 6.5f, 9.0f);
		}

		effsize_rate = effsize / sFlameEffMaxSize;

		/**** ìñÇΩÇËîªíË ****/
		if (p_player->IsDamage() == false) {

			ShapeCapsule capsuleA, capsuleB;
			//			capsuleB.mPosA = effpos;
			capsuleB.mPosA = VAdd(effpos, VScale(flame_vec, 300.0f)); //<! âäÇÃénì_ÇÕìñÇΩÇËîªíËÇÇ»ÇµÇ…Ç∑ÇÈ
			capsuleB.mPosB = VAdd(effpos, VScale(flame_vec, (1500.0f * effsize_rate)));

			capsuleB.mRadius = 110.0f;
#ifdef _DEBUG
			GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG
			// ÉLÉÉÉâÇ∆ÇÃìñÇΩÇËîªíË
			capsuleA.mPosA	= p_player->Position();
			capsuleA.mPosB	= p_player->Position();
			capsuleA.mPosB.y += p_player->BodyHeight();
			capsuleA.mRadius= p_player->BodySize();

			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				//			if (false) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 4;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), capsuleB.mPosA);
				damage_down_vec.y = 0.0f;
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_FLAME_BREAK_GUARD);
			}
		}

		if (mIsScaleFlame == true) {
			effsize = sFlameEffMaxSize;
		}

		//		effsize *= 0.1f;
		//		setFlameMatrix(effsize);
		{
			MATRIX mtx, tmp;
			/* íºê⁄É}ÉgÉäÉbÉNÉXÇêGÇÈ */
			EffectManager::GetInstance()->SetMatrixType(mFlameEffIndex, EffectNode::eMATRIX_TYPE_MATRIX);
			mtx = MGetIdent();
			// ägèkçsóÒ
			tmp = MGetIdent();
			effsize *= 0.1f;
			tmp = MGetScale(VGet(effsize, effsize, effsize));
			mtx = MMult(mtx, tmp);
			// âÒì]çsóÒ
			tmp = MGetIdent();
			this->GetFrameMatrixFromName(&tmp, "ê„ÇT");
			mtx = MMult(mtx, tmp);
			EffectManager::GetInstance()->Matrix(mFlameEffIndex) = mtx;
		}

		break;

	case 7:
		/* èIóπèàóù */
		//		InputManager::GetInstance()->StopVibe();

		GraphicManager::GetInstance()->RevertFogParam(1.0f);
		CameraManager::GetInstance()->StopVibeCamera();

		SoundManager::GetSndComponent()->StopSound(mSndIndex);
		mSndIndex = -1;

		EffectManager::GetInstance()->Stop(mFlameEffIndex, 0.5f);
		mFlameEffIndex = -1;

		if (mFireCount == 1) {
			ActionTaskManager::tTaskInfo info = {ActionTaskManager::eTASK_NAVI_EX, (long)strNaviTalk_RedDragonAdviceA, NULL};
			ActionTaskManager::GetInstance()->NotifyAction(&info);
		}
		else if (mFireCount == 3) {
			ActionTaskManager::tTaskInfo info = {ActionTaskManager::eTASK_NAVI_EX, (long)strNaviTalk_RedDragonAdviceB, NULL};
			ActionTaskManager::GetInstance()->NotifyAction(&info);
		}
		else if (mFireCount == 9) {
			ActionTaskManager::tTaskInfo info = {ActionTaskManager::eTASK_NAVI_EX, (long)strNaviTalk_RedDragonAdviceC, NULL};
			ActionTaskManager::GetInstance()->NotifyAction(&info);
		}
		mFireCount ++;

		ChangeState(eCHARA_STATE_IDLE); // èIóπ
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
CharaEnemyRedDragon::phaseEnterStateAttack2(void)
{
	StartMotion(eMT_REDDRAGON_ATTACK_2, false, 0.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::phaseStateAttack2(void)
{
	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::phaseEnterStateAttack3(void)
{
	StartMotion(eMT_REDDRAGON_ATTACK_3, false, 0.75f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::phaseStateAttack3(void)
{
	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	if (BetweenPhaseFrame(0.0f, 30.0f)) {
		//		float move_speed = 0.4f * (100 - GetMotionTimePercent());
		float move_speed = 0.3f * (100 - GetMotionTimePercent());
		mMoveVec.x += -(sin(mRotation.y) * move_speed);
		mMoveVec.z += -(cos(mRotation.y) * move_speed);
	}

	if (EqualPhaseFrame(30.0f)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	} else if (EqualPhaseFrame(39.0f)) {
		const float size = 50.0f;
		VECTOR effpos;
		this->GetFramePosFromName(&effpos, "âEñÚéwÇP");
		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,
															&effpos,
															&VGet(size, size, size),
															0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 2.0f);

		CameraManager::GetInstance()->StartVibeCamera(2.5f, 7.0f, 10.0f);

		SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
		SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
	}

	CharaBase* p_player;
	p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (EqualPhaseFrame(50.0f)) {
		if (GetRand(1) == 0) {
			//			SetRotationToTarget(&p_player->Position());
			ChangeState(eCHARA_STATE_ATTACK_4);
		}
		return;
	}

	if (BetweenPhaseFrame(30.0f, 45.0f)) {
		mIsAttack = true;
		/****/

		/* êÌì¨ïsî\é“Ç∆ä˘Ç…çUåÇÇ≥ÇÍÇΩé“ÇèúÇ≠ */
		if ((p_player->GetIsRetire() == false) &&
			(p_player->IsDamage() == false)) {

			ShapeCapsule capsuleA;
			ShapeCapsule capsuleB;
			capsuleA.mPosA	= p_player->Position();
			capsuleA.mPosB	= p_player->Position();
			capsuleA.mPosB.y += p_player->BodyHeight();
			capsuleA.mRadius= p_player->BodySize();
		
			this->GetFramePosFromName(&capsuleB.mPosA, "âEòr");
			this->GetFramePosFromName(&capsuleB.mPosB, "âEñÚéwÇP");
			capsuleB.mRadius = 90.0f;
			
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

#ifdef _DEBUG
	{
		ShapeCapsule capsuleB;
		this->GetFramePosFromName(&capsuleB.mPosA, "âEòr");
		this->GetFramePosFromName(&capsuleB.mPosB, "âEñÚéwÇP");
		capsuleB.mRadius = 120.0f;
		GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
	}
#endif // _DEBUG

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
CharaEnemyRedDragon::phaseEnterStateAttack4(void)	
{
	//	StartMotion(eMT_REDDRAGON_ATTACK_4, false, 0.75f);
	StartMotion(eMT_REDDRAGON_ATTACK_4, false, 1.0f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::phaseStateAttack4(void)
{
	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	if (BetweenPhaseFrame(0.0f, 30.0f)) {
		// ÉvÉåÉCÉÑÅ[ÇÃï˚å¸Ç÷ëÃÇå¸ÇØÇÈ
		updateRotationToPlayer(0.15f);

		//		float move_speed = 0.4f * (100 - GetMotionTimePercent());
		float move_speed = 0.3f * (100 - GetMotionTimePercent());
		mMoveVec.x += -(sin(mRotation.y) * move_speed);
		mMoveVec.z += -(cos(mRotation.y) * move_speed);
	}

	if (EqualPhaseFrame(30.0f)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);
	} else if (EqualPhaseFrame(39.0f)) {
		const float size = 50.0f;
		VECTOR effpos;
		this->GetFramePosFromName(&effpos, "ç∂ñÚéwÇP");
		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,
															&effpos,
															&VGet(size, size, size),
															0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 2.0f);

		CameraManager::GetInstance()->StartVibeCamera(2.5f, 7.0f, 10.0f);

		SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
		SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
	}

	if (BetweenPhaseFrame(30.0f, 45.0f)) {
		mIsAttack = true;
		/****/
		CharaBase* p_player;
		p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		/* êÌì¨ïsî\é“Ç∆ä˘Ç…çUåÇÇ≥ÇÍÇΩé“ÇèúÇ≠ */
		if ((p_player->GetIsRetire() == false) &&
			(p_player->IsDamage() == false)) {

			ShapeCapsule capsuleA;
			ShapeCapsule capsuleB;
			capsuleA.mPosA	= p_player->Position();
			capsuleA.mPosB	= p_player->Position();
			capsuleA.mPosB.y += p_player->BodyHeight();
			capsuleA.mRadius= p_player->BodySize();
		
			this->GetFramePosFromName(&capsuleB.mPosA, "ç∂òr");
			this->GetFramePosFromName(&capsuleB.mPosB, "ç∂ñÚéwÇP");
			capsuleB.mRadius = 90.0f;
			
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

#ifdef _DEBUG
	{
		ShapeCapsule capsuleB;
		this->GetFramePosFromName(&capsuleB.mPosA, "ç∂òr");
		this->GetFramePosFromName(&capsuleB.mPosB, "ç∂ñÚéwÇP");
		capsuleB.mRadius = 120.0f;
		GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
	}
#endif // _DEBUG

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
CharaEnemyRedDragon::phaseEnterStateAttack5(void)	
{
 	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_DAMAGE);

	mStateCount = 0;

	VECTOR eff_pos;
	//	eff_pos = mPosition;
	//	eff_pos.y += (mBodyHeight / 2.0f);
	this->GetFramePosFromName(&eff_pos, "ê„ÇT");
	EffekseerManager::GetInstance()->Play(eEFK_SHOCK,
										  &eff_pos,
										  &VScale(VGet(12.0f, 12.0f, 12.0f), 3.0f));
	SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD_3);


	StartMotion(eMT_REDDRAGON_DOWN_START, false, 0.75f, NULL);
	this->ResetFrameUserMatrix("éÒÇ`");
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::phaseStateAttack5(void)
{
	VECTOR effpos;

	if (GetMotionIndex() == eMT_REDDRAGON_DOWN_START) {
		if (EqualPhaseFrame(6.0f)) {
			const float size = 40.0f;
			this->GetFramePosFromName(&effpos, "ê„ÇT");
			int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,
																&effpos,
																&VGet(size, size, size),
																0.0f);
			EffectManager::GetInstance()->Stop(eff_handle, 2.0f);

			SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
			SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
			
			CameraManager::GetInstance()->StartVibeCamera(2.5f, 7.0f, 10.0f);

		} else if (GetIsEndMotion()) {
			StartMotion(eMT_REDDRAGON_ATTACK_5, true, 0.5f);

			this->GetFramePosFromName(&effpos, "ê„êÊ");
			effpos.y += 160.0f;
			mEfkHandlePiyopiyo = EffekseerManager::GetInstance()->Play(eEFK_PIYOPIYO,
																	   &effpos,
																	   &VScale(VGet(12.0f, 12.0f, 12.0f), 0.75f));

			SoundManager::GetSndComponent()->PlaySound(eSE_PIYOPIYO);
		}
	}

	//	if (mStateCount > 250) {
	if (mStateCount > 300) {

		SoundManager::GetSndComponent()->StopSound(eSE_PIYOPIYO);
		EffekseerManager::GetInstance()->Stop(mEfkHandlePiyopiyo, 0.0f);
		
		ChangeState(eCHARA_STATE_IDLE);
		mStateCount = 0;
		return;
	}
	mStateCount ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::phaseEnterStateAttack6(void)
{
	StartMotion(eMT_REDDRAGON_ATTACK_6, false, 0.6f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::phaseStateAttack6(void)
{
	bool is_prev_attack;
	is_prev_attack = mIsAttack;

	if (EqualPhaseFrame(33.0f)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HEAVY_SWING);

		VECTOR weapon_top_pos, weapon_btm_pos;
		this->GetFramePosFromName(&weapon_top_pos, "ì™êÊ");
		this->GetFramePosFromName(&weapon_btm_pos, "éÒÇ`");
		mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);
	}

	CharaBase* p_player;
	p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (BetweenPhaseFrame(33.0f, 43.0f)) {
		mIsAttack = true;
		/****/

		VECTOR weapon_top_pos, weapon_btm_pos;
		this->GetFramePosFromName(&weapon_top_pos, "ì™êÊ");
		this->GetFramePosFromName(&weapon_btm_pos, "éÒÇ`");
		mBladeEffect.Update(&weapon_top_pos, &weapon_btm_pos);

		/* êÌì¨ïsî\é“Ç∆ä˘Ç…çUåÇÇ≥ÇÍÇΩé“ÇèúÇ≠ */
		if ((p_player->GetIsRetire() == false) &&
			(p_player->IsDamage() == false)) {

			ShapeCapsule capsuleA;
			ShapeCapsule capsuleB;
			capsuleA.mPosA	= p_player->Position();
			capsuleA.mPosB	= p_player->Position();
			capsuleA.mPosB.y += p_player->BodyHeight();
			capsuleA.mRadius= p_player->BodySize();
		
			this->GetFramePosFromName(&capsuleB.mPosA, "ì™êÊ");
			this->GetFramePosFromName(&capsuleB.mPosB, "éÒÇ`");
			capsuleB.mRadius = 90.0f;
			
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

#ifdef _DEBUG
	{
		ShapeCapsule capsuleB;
			this->GetFramePosFromName(&capsuleB.mPosA, "ì™êÊ");
			this->GetFramePosFromName(&capsuleB.mPosB, "éÒÇ`");
			capsuleB.mRadius = 90.0f;
		GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
	}
#endif // _DEBUG

	if (EqualPhaseFrame(43.0f)) {
		mBladeEffect.Stop();
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
CharaEnemyRedDragon::phaseStateWaitAttack(void)
{
	ChangeState(mReserveCharaState);
	mReserveCharaState = -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::phaseEnterStateRetireDoing(void)
{
	if (mHitpoint != kSYSTEM_DAMAGE_VALUE) {

		//		const float size = 75.0f * 2.0f;
		VECTOR eff_pos;
		eff_pos = mPosition;
		eff_pos.y += (mBodyHeight / 2.0f);

		//		const float size = 75.0f * 1.0f;
		const float size = 30.0f;
		mEffHandle = EffectManager::GetInstance()->Play(eEF_CHARGE,
														&eff_pos,
														&VGet(size, size, size),
														0.0f);
		EffectManager::GetInstance()->SetFloat(mEffHandle, "particleSpeed", -2.6f);

		SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
		CameraManager::GetInstance()->StartVibeCamera(3.0f, 6.0f, 8.0f);

		GraphicManager::GetInstance()->RemoveDrawShadow(&mShadow);

		CharaPlayer* p_mine = (CharaPlayer *)CollisionManager::GetInstance()->GetBodyCollisionAddress(0);
		p_mine->PermitAcceptInput(false);

		ActionTaskManager::tTaskInfo info;
		info.mTaskType = ActionTaskManager::eTASK_ENEMY_ITEM;
		info.mTaskValueA = (long long)this;
		info.mTaskValueB = 1;
		PRINT_CONSOLE("NotifyAction -> eTASK_ENEMY_ITEM \n");
		ActionTaskManager::GetInstance()->NotifyAction(&info);
	}

	mStatePhase = 0;
	mStateCount = 0;

	//	SoundManager::GetSndComponent()->PlaySound(eSE_ERASE_MODEL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::phaseStateRetireDoing(void)
{
	// ÉVÉXÉeÉÄÉ_ÉÅÅ[ÉWÇÃèÍçáÅAà»ç~ÇÃèàóùÇçsÇÌÇ»Ç¢
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

			EffectManager::GetInstance()->Stop(mEffHandle, 1.0f);

			const float size = 75.0f * 2.0f;
			VECTOR eff_pos;
			eff_pos = mPosition;
			eff_pos.y += (mBodyHeight / 2.0f);
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

		//		alpha += -0.01f;
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
CharaEnemyRedDragon::phaseEnterStateRetireEnd(void)
{
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
bool
CharaEnemyRedDragon::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	float dist = VSize(vec);
	
	int rand;
	bool result = false;

	if (dist < mBodySize * 1.75f) {
		rand = GetRand(6);

		if (rand == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_6;
			result = true;
		} else if (rand == 1) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_3;
			result = true;
		} else if (rand == 2) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
			result = true;
		}
		//	} else if (dist < (mBodySize * 3.0f)) {
	}
	else if (dist < (mBodySize * 4.0f)) {
		rand = GetRand(45);
		//		if ((rand >= 0) && (rand <= 1)) {
		if (rand <= 1) {
			//		if (rand == 1) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
			result = true;
		} else if (rand == 2) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_3;
			result = true;
		}
	}

	return result;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
#if 0
void
CharaEnemyRedDragon::updateAllObject(void)
{
	ModelCollision* p_mine = this;
	ModelCollision* p_your = NULL;

	/*
	VECTOR moved_pos = VAdd(p_mine->Position(), mMoveVec);
	VECTOR vec;
	float dist = 0.0f;
	//	CollisionManager::tSphereWork workA, workB;
	CollisionManager::tSphereWork workA;

	workA.mPosition	= moved_pos;
	workA.mSize		= this->mBodySize;
	*/

	VECTOR vec;
	float dist = 0.0f;

	/*
	CollisionManager::tSphereWork my_sphere;
	getHeadSphere((CollisionManager::tSphereWork &)my_sphere);
	my_sphere.mPosition = VAdd(my_sphere.mPosition, mMoveVec);

	p_your = (ModelCollision *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	if (p_your->GetIsRetire()) return;	// êÌì¨ïsî\é“Ç‡èúÇ≠

	if (p_your->CheckBodyCollision(my_sphere, vec, dist)) {
		VECTOR target_pos;
		mMoveVec.x = 0.0f;
		mMoveVec.z = 0.0f;
		vec.y = 0.0f;
		//		target_pos = VAdd(moved_pos, VScale(VNorm(vec), -dist));
		target_pos = VAdd(my_sphere.mPosition, VScale(VNorm(vec), -dist));
		mMoveVec = VSub(target_pos, mPosition);
	}
	*/
}
#endif // 0

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::setFlameMatrix(float setSize)
{
	EffectManager::GetInstance()->SetMatrixType(mFlameEffIndex, EffectNode::eMATRIX_TYPE_MATRIX);
	MATRIX mtx, tmp;
	float effsize;

	mtx = MGetIdent();
	// ägèkçsóÒ
	tmp = MGetIdent();
	effsize = setSize;
	tmp = MGetScale(VGet(effsize, effsize, effsize));
	mtx = MMult(mtx, tmp);
	// ägèkçsóÒ
	tmp = MGetIdent();
	tmp = MGetScale(VGet(effsize, effsize, effsize));
	mtx = MMult(mtx, tmp);
	// âÒì]çsóÒ
	tmp = MGetIdent();
	this->GetFrameMatrixFromName(&tmp, "ê„ÇT");
	mtx = MMult(mtx, tmp);
	EffectManager::GetInstance()->Matrix(mFlameEffIndex) = mtx;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::updateRotationToPlayer(float rate)
{
	VECTOR vecA, vecB, vecC;
	vecA.x = sin(mRotation.y + Utility::DegToRad(180.0f));
	vecA.y = 0.0f;
	vecA.z = cos(mRotation.y + Utility::DegToRad(180.0f));
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	vecB = VSub(p_player->Position(), mPosition);
	vecB.y = 0.0f;
	vecB = VNorm(vecB);
	vecC = VAdd(VScale(vecA, (1.0f - rate)), VScale(vecB, rate));
	//	vecC = VScale(vecC, (1.0f / 7.0f));
	mRotation.y = atan2(vecC.x, vecC.z) + PHI_F;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyRedDragon::getHeadSphere(CollisionManager::tSphereWork& rSphere)
{
	/*
	VECTOR node_pos;
	GetFramePosFromName(&node_pos, "ì™");
	rSphere.mPosition = node_pos;
	rSphere.mPosition.y = mPosition.y;

	float abount_dot = 0.0f;
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR player_to_enemy;

	player_to_enemy = VSub(p_player->Position(), node_pos);
	player_to_enemy.y = 0.0f;
	player_to_enemy = VNorm(player_to_enemy);

	// ì‡êœÇãÅÇﬂÇÈ
	VECTOR mRotationVec;
	// É}ÉgÉäÉbÉNÉXÇå≥Ç…âÒì]ÉxÉNÉgÉãÇéÊìæ
	mRotationVec = VNorm(VTransformSR(VGet(0.0f, 0.0f, -1.0f), mMatrix) );

	abount_dot = VDot(mRotationVec, player_to_enemy);
	rSphere.mSize = (100.0f + (400.0f * abs(abount_dot) ) );
	*/
	
	VECTOR node_pos;
	GetFramePosFromName(&node_pos, "ì™");
	rSphere.mPosition = node_pos;
	rSphere.mPosition.y = mPosition.y;
	rSphere.mSize = 360.0f;

}


 /**** end of file ****/

