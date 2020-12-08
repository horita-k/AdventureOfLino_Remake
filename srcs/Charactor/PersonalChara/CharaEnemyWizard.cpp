/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * �M�M�l�u���t�@�C��
 * @author SPATZ.
 * @data   2012/05/25 00:27:32
 */
//---------------------------------------------------
#include "CharaEnemyWizard.h"
#include "../CharaPlayer.h"
#include "Charactor/CharaInfo.h"
#include "AppLib/Basic/Basic.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Effect/EffectManager.h"
#include "AppLib/Input/InputManager.h"
#include "AppLib/Sound/SoundManager.h"

#include "AppLib/Resource/ResourceModel.h"

#include "AppLib/Collision/CollisionManager.h"
#include "DataDefine/MotionDef.h"
#include "DataDefine/BGDef.h"
#include "DataDefine/VoiceDef.h"
#include "DataDefine/EffectDef.h"
#include "Scene/ActionTaskManager.h"
#include "Scene/SceneMainGame.h"
#include "Utility/Utility.h"

#include "Scene/CommandFunctions.h"

/*=====================================*
 * define
 *=====================================*/
static const float kENEGY_BALL_HIT_DUST_EFF_SIZE = 20.0f;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWizard::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// �G�x�[�X�N���X��Setup�K�v
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	mFlinchMax = 250;

	mEfkHandle = NULL;
	for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
		mEfsHandle[iI] = -1;
	}
	mStatePhase = 0;
	mStateCount = 0;
	sThunderPos = ZERO_VEC;
	sThunderVec = ZERO_VEC;

	mIsBarrier			= false;
	mProtectEffIndex	= -1;
	mProtectWaitCount	= 0;
	mIsRandomWarp		= false;
}

//---------------------------------------------------
/**
 * �L�����̍U���X�e�[�^�X�J�ڂ̃`�F�b�N�֐�
 */
//---------------------------------------------------
bool
CharaEnemyWizard::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	float dist = VSize(vec);
	
	int rand;
	bool result = false;

	if (dist > mBodySize * 36.0f) {
		mIsRandomWarp = false;
		*pAttackCharaState = eCHARA_STATE_ATTACK_4;
		result = true;
	} else if (dist < mBodySize * 16.0f) {
		if ((mIsBarrier == false) &&
			(mProtectWaitCount == 0)) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_5;
			  result = true;
		} else {
			rand = GetRand(9);//10


			// �o���A�L��ƂȂ��Ŕ�������ς���
			if (mIsBarrier == true) {

				switch (rand){
				case 0 :case 1:case 2:case 3:
					*pAttackCharaState = eCHARA_STATE_ATTACK_1;result = true;break;
				case 4:
					*pAttackCharaState = eCHARA_STATE_ATTACK_2;result = true;break;
				case 5 :case 6:
					*pAttackCharaState = eCHARA_STATE_ATTACK_3;result = true;break;
				case 7 :case 8:
					*pAttackCharaState = eCHARA_STATE_ATTACK_7;result = true;break;
				case 9:
					mIsRandomWarp = true;
					*pAttackCharaState = eCHARA_STATE_ATTACK_4;result = true;break;
				default:
					break;
				};

			} else {
				switch (rand){
				case 0 :
					*pAttackCharaState = eCHARA_STATE_ATTACK_1;result = true;break;
				case 1 :case 2: case 3:
					*pAttackCharaState = eCHARA_STATE_ATTACK_2;result = true;break;
				case 4 :case 5:
					*pAttackCharaState = eCHARA_STATE_ATTACK_3;result = true;break;
				case 6 :case 7: case 8:
					*pAttackCharaState = eCHARA_STATE_ATTACK_7;result = true;break;
				case 9:
					mIsRandomWarp = true;
					*pAttackCharaState = eCHARA_STATE_ATTACK_4;result = true;break;
				default:
					break;
				};

			}

		}
	}
	return result;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWizard::processNoMoveAction(void)
{
	ChangeState(eCHARA_STATE_ATTACK_4);
}

/*=====================================*
 * extend
 *=====================================*/

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWizard::Update(void)
{
	CharaEnemy::Update();

	if (mProtectEffIndex != -1) {
		VECTOR eff_pos;
		eff_pos = mPosition;
		eff_pos.y += (mBodyHeight * 0.5f);
		EffectManager::GetInstance()->Position(mProtectEffIndex) = eff_pos;
	}

	if (mEfkProtect != -1) {
		VECTOR eff_pos;
		eff_pos = mPosition;
		EffekseerManager::GetInstance()->SetPosition(mEfkProtect, &eff_pos);
	}

	mProtectWaitCount --;
	if (mProtectWaitCount < 0) {
		mProtectWaitCount = 0;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWizard::Destroy(void)
{
	STOP_EFFEKSEER(mEfkHandle, 0.0f);
	STOP_EFFEKSEER(mEfkProtect, 0.0f);

	for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
		STOP_EFFECT(mEfsHandle[iI]);
	}

	CharaEnemy::Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWizard::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	if (pEffPos == NULL) {
		pEffPos = &mPosition;
	}

	//	mIsBarrier = true; /* for debug */
	if (mIsBarrier == true) {

		CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		COLOR_F color;
		color.r = 10.0f;
		color.g = 10.0f;
		color.b = 20.0f;
		color.a = 10.0f;
		this->SetDiffuse(color);
		this->SetSpecular(color);

		EFK_HANDLE handle;
		VECTOR eff_pos;
		eff_pos = mPosition;
		eff_pos.y += (mBodyHeight * 0.5f);
		eff_pos = VScale(VAdd(*pEffPos, eff_pos), 0.5f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_SHIELD, &eff_pos, &VScale(mScale, 0.8f));
		EffekseerManager::GetInstance()->SetRotationAxisY(handle, (p_player->Rotation().y + (DX_PI/2.0f)));

		SoundManager::GetSndComponent()->PlaySound(eSE_SHIELD);
		mIsDamage = true;
	} else {
		CharaEnemy::ProcessDamage(attackPower, isSetMode, pEffPos, pDamageDownVec, isDamageDown, attackType);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWizard::phaseEnterStateAttack1(void)
{
	//	const float size = 5200.0f;
	const float size = 4500.0f;

	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_3);
	StartMotion(eMT_KNIGHT_ATTACK_1, false, 0.6f);

	StartJoypadVibration(DX_INPUT_PAD1, 500, 2000);

	SoundManager::GetSndComponent()->PlaySound(eSE_MAGIC);

	GraphicManager::tFogParam fogParam;
	fogParam.mIsEnable = TRUE;
	fogParam.mIsActive = TRUE;
	fogParam.mColR = 10;
	fogParam.mColG = 10;
	fogParam.mColB = 10;
	fogParam.mNear = 10.0f;
	fogParam.mFar = 10000.0f;
	//	fogParam.mFar = 20.0f;
	GraphicManager::GetInstance()->ChangeFogParam(0.5f, &fogParam);

	VECTOR eff_pos;
	eff_pos = VAdd(mPosition, mMoveVec);
	mEfkHandle = EffekseerManager::GetInstance()->Play(eEFK_MAGIC_SQUARE, &eff_pos, &VScale(mScale, 3.0f));

	VECTOR weapon_top_pos;
	this->GetFramePosFromName(&weapon_top_pos, "�E���");
	mEfsHandle[0] = EffectManager::GetInstance()->Play(eEF_LIGHTNING,
													   &weapon_top_pos,
													   &VGet(size, size, size),
													   0.0f);

	CameraManager::GetInstance()->StartVibeCamera(1.6f, 3.2f, 0.8f);

	mStatePhase = 0;
	mStateCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWizard::phaseStateAttack1(void)
{
	//	static const float kTHUNDER_EFF_SIZE = 1200.0f;
	static const float kTHUNDER_EFF_SIZE = 800.0f;

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR weapon_top_pos;
	int	thunder_charge_count = 0;
	int percent = 0;

	this->GetFramePosFromName(&weapon_top_pos, "�E���");

	if (mEfsHandle[0] != -1) {
		EffectManager::GetInstance()->Position(mEfsHandle[0]) = weapon_top_pos;
	}

	switch (mStatePhase) {
	case 0:
		// �L�����̕�������������
		SetRotationToTarget(&p_player->Position());

		// �g�o�ɂ�Ĕ��ˑ��x�𑁂�����
		percent = ( (mHitpoint * 100)  / mMaxHitpoint );
		//		thunder_charge_count = (int)((float)percent * 1.2f);
		thunder_charge_count = 60 + (int)((float)percent * 0.6f);
		if (mStateCount > thunder_charge_count) {
			mStatePhase ++; mStateCount = 0;
		}
		break;

	case 1:
		StartJoypadVibration(DX_INPUT_PAD1, 1000, 3000);
		StartMotion(eMT_KNIGHT_ATTACK_2, false, 0.6f);
		CameraManager::GetInstance()->StartVibeCamera(1.0f, 5.2f, 1.8f);
		SoundManager::GetSndComponent()->PlaySound(eSE_THUNDER);
		GraphicManager::tFogParam fogParam;
		fogParam.mIsEnable = TRUE;
		fogParam.mIsActive = TRUE;
		fogParam.mColR = 255;
		fogParam.mColG = 255;
		fogParam.mColB = 255;
		fogParam.mNear = 10.0f;
		fogParam.mFar = 1000.0f;
		GraphicManager::GetInstance()->ChangeFogParam(0.1f, &fogParam);

		EffekseerManager::GetInstance()->Stop(mEfkHandle);

		if (mEfsHandle[0] != -1) {
			EffectManager::GetInstance()->Stop(mEfsHandle[0], 0.5f);
			mEfsHandle[0] = -1;
		}
		mEfsHandle[0] = EffectManager::GetInstance()->Play(eEF_LIGHTNING_EX,
														   &weapon_top_pos,
														   &VGet(kTHUNDER_EFF_SIZE, kTHUNDER_EFF_SIZE, kTHUNDER_EFF_SIZE),
														   0.0f);
		EffectManager::GetInstance()->SetFloat(mEfsHandle[0], "ThunderRand", 400.0f);
		//		EffectManager::GetInstance()->SetFloat(mEfsHandle[0], "ThunderRand", 200.0f);

		sThunderPos = weapon_top_pos;
		sThunderVec = VSub(p_player->Position(), mPosition);
		sThunderVec = VScale(VNorm(sThunderVec), 100.0f/*speed*/);
		
		mStatePhase ++; mStateCount = 0;
		break;

	case 2:
		VECTOR next_th_pos, hitpos, normal;
		next_th_pos = VAdd(sThunderPos, sThunderVec);
		
		/**** �G�t�F�N�g�̕\���X�V ****/
		FLOAT4 target;
		target.x = next_th_pos.x;
		target.y = next_th_pos.y;
		target.z = next_th_pos.z;
		target.w = 1.0f;
		EffectManager::GetInstance()->SetVec4(mEfsHandle[0], "TgtPos", &target);

		/**** �����蔻�� ****/
		if (p_player->IsDamage() == false) {
			bool is_frank_guard = false;

			ShapeCapsule capsuleA, capsuleB;
			// �J�~�i���̌`��
			capsuleB.mPosA = next_th_pos;
			capsuleB.mPosB = weapon_top_pos;
			capsuleB.mRadius = 150.0f;
#ifdef _DEBUG
			GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG
			if (p_player->GetState() == CharaPlayer::eCHARA_STATE_LIB_SHIELD) {
				// �t�����N�\���o�b�W�Ƃ̓����蔻��
				p_player->GetFramePosFromName(&capsuleA.mPosA, "�E���w�P");
				capsuleA.mPosB = capsuleA.mPosA;
				float size = VSize(sThunderVec) * 1.2f;
				capsuleA.mPosB.x += (sin(p_player->Rotation().y + Utility::DegToRad(180.0f)) * size);
				capsuleA.mPosB.z += (cos(p_player->Rotation().y + Utility::DegToRad(180.0f)) * size);
				capsuleA.mRadius = 60.0f;
#ifdef _DEBUG
				GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleA);
#endif // #ifdef _DEBUG				
				if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
					is_frank_guard = true;

					SoundManager::GetSndComponent()->PlaySound(eSE_REFLECT);

					//					const float size = 100.0f;
					const float size = 50.0f;
					int eff_handle;
					eff_handle = EffectManager::GetInstance()->Play(eEF_HIT,
																	&capsuleA.mPosA,
																	&VGet(size, size, size),
																	0.0f);
					EffectManager::GetInstance()->Stop(eff_handle, 0.75f);

					mStatePhase = 4;
				}
			}

			if (is_frank_guard == false) {
				// �L�����Ƃ̓����蔻��
				capsuleA.mPosA	= p_player->Position();
				capsuleA.mPosB	= p_player->Position();
				capsuleA.mPosB.y += p_player->BodyHeight();
				capsuleA.mRadius= p_player->BodySize();

				if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
					SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
					int attack_power = 8;
					VECTOR damage_down_vec;
					damage_down_vec = VSub(p_player->Position(), capsuleB.mPosA);
					damage_down_vec.y = 0.0f;
					damage_down_vec = VNorm(damage_down_vec);
					p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
				}
			}

		}
		
		/**** �I������ ****/
		if ((CollisionManager::GetInstance()->CheckWallCollision(sThunderPos, next_th_pos, &hitpos, &normal)) ||
			(CollisionManager::GetInstance()->CheckEarthCollision(sThunderPos, next_th_pos, &hitpos, &normal) != NOT_ON_COLLISION)) {

			if (mEfsHandle[0] != -1) {
				EffectManager::GetInstance()->Stop(mEfsHandle[0], 1.0f);
				mEfsHandle[0] = -1;
			}
			mStatePhase ++; mStateCount = 0;
		}

		// ���̍��W���X�V
		sThunderPos = next_th_pos;

		break;

	case 3:
		/* �I������ */
		if (mStateCount == 60) {
			GraphicManager::GetInstance()->RevertFogParam(1.0f);
			
			ChangeState(eCHARA_STATE_IDLE);
		}
		break;

		/* �J�~�i���𒵂˕Ԃ����Ƃ��̏��� */
	case 4:
		if (mStateCount >= 10) {
			if (mEfsHandle[0] != -1) {
				EffectManager::GetInstance()->Stop(mEfsHandle[0], 0.5f);
				mEfsHandle[0] = -1;
			}
			ChangeState(eCHARA_STATE_ATTACK_6);
		}
		break;
		

	};

	mStateCount ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWizard::phaseEnterStateAttack2(void)
{
	StartMotion(eMT_KNIGHT_ATTACK_1, false, 0.6f);
	SoundManager::GetSndComponent()->PlaySound(eSE_MAGIC);

	VECTOR eff_pos;

	// �����w
	eff_pos = mPosition;
	mEfkHandle = EffekseerManager::GetInstance()->Play(eEFK_MAGIC_SQUARE, &eff_pos, &VScale(mScale, 3.0f));

	for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
		if (mEfsHandle[iI] != -1) {
			EffectManager::GetInstance()->Stop(mEfsHandle[iI], 0.0f);
			mEfsHandle[iI] = -1;
		}
	}

	mStatePhase = 0;
	mStateCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWizard::phaseStateAttack2(void)
{
	static float sAccel = 0.0f;
	static const float sThrowTime = 45.0f;
	static const float size = 100.0f;
	VECTOR eff_vec = ZERO_VEC;
	int* pEfsHandle = NULL;

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	EFK_HANDLE handle;
	VECTOR eff_pos;
	VECTOR next_pos;
	VECTOR weapon_top_pos;
	VECTOR hitpos, normal;
	int eff_handle = -1;

	this->GetFramePosFromName(&weapon_top_pos, "�E���w�R");

	switch (mStatePhase) {
	case 0:
		SetRotationToTarget(&p_player->Position());

		//		if (this->GetIsMotionBlend() == false) {
		//		if (this->GetIsEndMotion()) {
		if (mStateCount > 10) {
			
			SoundManager::GetSndComponent()->PlaySound(eSE_DARK);
			// �g��
			eff_pos = weapon_top_pos;
			mEfsHandle[0] = EffectManager::GetInstance()->Play(eEF_ENERGY_BALL_R,
															   &eff_pos,
															   &VGet(size, size, size),
															   1.0f);
			mStatePhase ++;mStateCount = 0;
		}
		break;

	case 1:
		SetRotationToTarget(&p_player->Position());
		if (mStateCount >= 60) {
			SoundManager::GetSndComponent()->PlaySound(eSE_BOOMERANG_RUN);
			SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);

			StartMotion(eMT_KNIGHT_ATTACK_2, false, 1.0f);
			mStatePhase ++;mStateCount = 0;
		}
		break;

	case 2:
		SetRotationToTarget(&p_player->Position());

		if (mEfsHandle[0] != -1) {
			EffectManager::GetInstance()->Position(mEfsHandle[0]) = weapon_top_pos;
		}
		if (mStateCount > 1) {
			VECTOR tar_pos;
			VECTOR vec;
			tar_pos = p_player->Position();
			tar_pos.y += (p_player->BodyHeight() * 0.3f);
			vec = VSub(tar_pos, weapon_top_pos);
			//<! �L������菭���O���^�[�Q�b�g�Ƃ��ē�����
			tar_pos = VAdd(weapon_top_pos, VScale(VNorm(vec), (VSize(vec) * 0.9f)));

			float vo, accel, dist;
			dist = VSize(VSub(tar_pos, weapon_top_pos));

			accel = (-2.0f * dist) / pow(sThrowTime, 2);
			vo = -1.0f * accel * sThrowTime;
			sAccel = accel;

			// �g���𓊂���
			if (mEfsHandle[0] != -1) {
				EffectManager::GetInstance()->Position(mEfsHandle[0]) = weapon_top_pos;
			}
			sThunderVec = VSub(tar_pos, weapon_top_pos);
			sThunderVec = VScale(VNorm(sThunderVec), vo);
			mStatePhase ++;mStateCount = 0;
		}
		break;

	case 3:
		/* �g���ړ����� */
		if (mEfsHandle[0] != -1) {
			eff_pos = EffectManager::GetInstance()->Position(mEfsHandle[0]);
		}
		sThunderVec = VAdd(sThunderVec, VScale(VNorm(sThunderVec), sAccel)); // �����x�X�V
		next_pos = VAdd(eff_pos, sThunderVec); // ���W�X�V
		if (mEfsHandle[0] != -1) {
			EffectManager::GetInstance()->Position(mEfsHandle[0]) = next_pos; // ���f
		}

		/* �ǔ��� */
		if ((mStateCount > sThrowTime) ||
			(CollisionManager::GetInstance()->CheckWallCollision(eff_pos, next_pos, &hitpos, &normal)) ||
			(CollisionManager::GetInstance()->CheckEarthCollision(eff_pos, next_pos, &hitpos, &normal) != NOT_ON_COLLISION)) {

			/* ���􏈗� */
			SoundManager::GetSndComponent()->PlaySound(eSE_SPREAD);

			// �ł����z���~������
			if (mEfsHandle[0] != -1) {
				EffectManager::GetInstance()->Stop(mEfsHandle[0], 0.5f);
				mEfsHandle[0] = -1;
			}

			mEfsHandle[0] = EffectManager::GetInstance()->Play(eEF_ENERGY_BALL_R, &eff_pos, &VGet(size*0.5f, size*0.5f, size*0.5f), 0.0f);
			mEfsHandle[1] = EffectManager::GetInstance()->Play(eEF_ENERGY_BALL_R, &eff_pos, &VGet(size*0.5f, size*0.5f, size*0.5f), 0.0f);
			mEfsHandle[2] = EffectManager::GetInstance()->Play(eEF_ENERGY_BALL_R, &eff_pos, &VGet(size*0.5f, size*0.5f, size*0.5f), 0.0f);
			mEfsHandle[3] = EffectManager::GetInstance()->Play(eEF_ENERGY_BALL_R, &eff_pos, &VGet(size*0.5f, size*0.5f, size*0.5f), 0.0f);

			// ��x������������
			sThunderVec.y = 0.0f;
			sThunderVec = VScale(VNorm(sThunderVec), 60.0f);

			// �j��G�t�F�N�g
			handle = EffekseerManager::GetInstance()->Play(eEFK_SPREAD, &eff_pos, &VScale(mScale, 0.5f));
			EffekseerManager::GetInstance()->SetRotationAxisY(handle, mRotation.y);

			mStatePhase ++;mStateCount = 0;
		}
		break;

	case 4:
		/* �����̈ړ����� */
		sThunderVec = VAdd(sThunderVec, VScale(VNorm(sThunderVec), -0.5f)); // �����x�X�V

		pEfsHandle = &mEfsHandle[0];
		if (*pEfsHandle != -1) {
			//			eff_vec = sThunderVec;
			eff_vec = VScale(sThunderVec, -1.0f);
			eff_pos = EffectManager::GetInstance()->Position(*pEfsHandle);
			next_pos = VAdd(eff_pos, eff_vec); // ���W�X�V
			EffectManager::GetInstance()->Position(*pEfsHandle) = next_pos; // ���f
			if (CollisionManager::GetInstance()->CheckWallCollision(eff_pos, next_pos, &hitpos, &normal)) {
				EffectManager::GetInstance()->Stop(*pEfsHandle, 0.5f);
				*pEfsHandle = -1;
				eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,&eff_pos,&VGet(kENEGY_BALL_HIT_DUST_EFF_SIZE, kENEGY_BALL_HIT_DUST_EFF_SIZE, kENEGY_BALL_HIT_DUST_EFF_SIZE),0.0f);

				EffectManager::GetInstance()->Stop(eff_handle, 0.5f);

				SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
			}
		}
		
		pEfsHandle = &mEfsHandle[1];
		if (*pEfsHandle != -1) {
			//			eff_vec = VScale(sThunderVec, -1.0f);
			eff_vec = VScale(VGet(0.0f, 1.0f, 0.0f), VSize(sThunderVec));
			eff_pos = EffectManager::GetInstance()->Position(*pEfsHandle);
			next_pos = VAdd(eff_pos, eff_vec); // ���W�X�V
			EffectManager::GetInstance()->Position(*pEfsHandle) = next_pos; // ���f
			if (CollisionManager::GetInstance()->CheckWallCollision(eff_pos, next_pos, &hitpos, &normal)) {
				EffectManager::GetInstance()->Stop(*pEfsHandle, 0.5f);
				*pEfsHandle = -1;
				eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,&eff_pos,&VGet(kENEGY_BALL_HIT_DUST_EFF_SIZE, kENEGY_BALL_HIT_DUST_EFF_SIZE, kENEGY_BALL_HIT_DUST_EFF_SIZE),0.0f);
				EffectManager::GetInstance()->Stop(eff_handle, 0.5f);

				SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
			}
		}

		pEfsHandle = &mEfsHandle[2];
		if (*pEfsHandle != -1) {
			eff_vec = VCross(sThunderVec, VGet(0.0f, 1.0f, 0.0f));
			eff_pos = EffectManager::GetInstance()->Position(*pEfsHandle);
			next_pos = VAdd(eff_pos, eff_vec); // ���W�X�V
			EffectManager::GetInstance()->Position(*pEfsHandle) = next_pos; // ���f
			if (CollisionManager::GetInstance()->CheckWallCollision(eff_pos, next_pos, &hitpos, &normal)) {
				EffectManager::GetInstance()->Stop(*pEfsHandle, 0.5f);
				*pEfsHandle = -1;

				eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,&eff_pos,&VGet(kENEGY_BALL_HIT_DUST_EFF_SIZE, kENEGY_BALL_HIT_DUST_EFF_SIZE, kENEGY_BALL_HIT_DUST_EFF_SIZE),0.0f);
				EffectManager::GetInstance()->Stop(eff_handle, 0.5f);

				SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
			}
		}
		
		pEfsHandle = &mEfsHandle[3];
		if (*pEfsHandle != -1) {
			eff_vec = VCross(sThunderVec, VGet(0.0f, -1.0f, 0.0f));
			eff_pos = EffectManager::GetInstance()->Position(*pEfsHandle);
			next_pos = VAdd(eff_pos, eff_vec); // ���W�X�V
			EffectManager::GetInstance()->Position(*pEfsHandle) = next_pos; // ���f
			if (CollisionManager::GetInstance()->CheckWallCollision(eff_pos, next_pos, &hitpos, &normal)) {
				EffectManager::GetInstance()->Stop(*pEfsHandle, 0.5f);
				*pEfsHandle = -1;

				eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,&eff_pos,&VGet(kENEGY_BALL_HIT_DUST_EFF_SIZE, kENEGY_BALL_HIT_DUST_EFF_SIZE, kENEGY_BALL_HIT_DUST_EFF_SIZE),0.0f);
				EffectManager::GetInstance()->Stop(eff_handle, 0.5f);

				SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
			}
		}

		if (mStateCount > sThrowTime) {

			float now_scale = 0.0f;
			for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
				if (mEfsHandle[iI] != -1) {
					now_scale = EffectManager::GetInstance()->Scale(mEfsHandle[iI]).x;
					now_scale *= 0.9f;
					EffectManager::GetInstance()->Scale(mEfsHandle[iI]) = VGet(now_scale, now_scale, now_scale);
					if (now_scale < 0.1f) {
						//<! �I���V�[�P���X�� 
						mStatePhase = 5;mStateCount = 0;
					}
				}
			}
			/* �e���S�ď��������ꍇ */
			if (now_scale == 0.0f) {
				//<! �I���V�[�P���X�� 
				mStatePhase = 5;mStateCount = 0;
			}


		}
		break;

	case 5:
		/**** �I������ ****/
		for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
			if (mEfsHandle[iI] != -1) {
				EffectManager::GetInstance()->Stop(mEfsHandle[iI], 0.5f);
				mEfsHandle[iI] = -1;
			}
		}
		ChangeState(eCHARA_STATE_IDLE);
		return;

	default:
		break;
	};

	/* �����蔻�菈�� */
	if (p_player->IsDamage() == false) {

		for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
			if (mEfsHandle[iI] == -1) {
				continue; // �I��
			}
			float eff_scale;

			eff_pos = EffectManager::GetInstance()->Position(mEfsHandle[iI]);
			eff_scale = EffectManager::GetInstance()->Scale(mEfsHandle[iI]).x;

			ShapeCapsule capsuleA = ShapeCapsule(p_player->Position(), p_player->Position(), p_player->BodySize());
			ShapeCapsule capsuleB = ShapeCapsule(eff_pos, eff_pos, eff_scale);
			capsuleA.mPosB.y += p_player->BodyHeight();
			capsuleB.mPosB.y += 1.0f;
#ifdef _DEBUG
			GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 3;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), capsuleB.mPosA);
				damage_down_vec.y = 0.0f;
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);

//				STOP_EFFECT(mEfsHandle[iI]);
				
				eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,&eff_pos,&VGet(kENEGY_BALL_HIT_DUST_EFF_SIZE, kENEGY_BALL_HIT_DUST_EFF_SIZE, kENEGY_BALL_HIT_DUST_EFF_SIZE),0.0f);
				EffectManager::GetInstance()->Stop(eff_handle, 0.5f);
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
CharaEnemyWizard::phaseEnterStateAttack3(void)
{
	StartMotion(eMT_KNIGHT_ATTACK_1, false, 0.6f);
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_MAGIC);

	SoundManager::GetSndComponent()->PlaySound(eSE_DARK);

	VECTOR eff_pos;

	// �����w
	eff_pos = mPosition;
	mEfkHandle = EffekseerManager::GetInstance()->Play(eEFK_MAGIC_SQUARE, &eff_pos, &VScale(mScale, 3.0f));

	for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
		if (mEfsHandle[iI] != -1) {
			EffectManager::GetInstance()->Stop(mEfsHandle[iI], 0.0f);
			mEfsHandle[iI] = -1;
		}
	}

	mStatePhase = 0;
	mStateCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWizard::phaseStateAttack3(void)
{
	static float sAccel = 0.0f;
	static const float sThrowTime = 45.0f;
	static const float size = 50.0f;
	VECTOR eff_vec = ZERO_VEC;
	int* pEfsHandle = NULL;
	float cycle = 0.0f;

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	VECTOR eff_pos;
	VECTOR next_pos;
	VECTOR hitpos, normal;
	int eff_handle = -1;

	switch (mStatePhase) {
	case 0:
		/* �G�t�F�N�g�S�𐶐� */
		SetRotationToTarget(&p_player->Position());

		if (mStateCount > 30) {
			
			// �g��
			eff_pos = mPosition;
			eff_pos.y += mBodyHeight * 0.5f;

			mEfsHandle[0] = EffectManager::GetInstance()->Play(eEF_ENERGY_BALL_R, &eff_pos, &VGet(size, size, size), 0.0f);
			mEfsHandle[1] = EffectManager::GetInstance()->Play(eEF_ENERGY_BALL_G, &eff_pos, &VGet(size, size, size), 0.0f);
			mEfsHandle[2] = EffectManager::GetInstance()->Play(eEF_ENERGY_BALL_B, &eff_pos, &VGet(size, size, size), 0.0f);
			mEfsHandle[3] = EffectManager::GetInstance()->Play(eEF_ENERGY_BALL,   &eff_pos, &VGet(size, size, size), 0.0f);

			mStatePhase ++;mStateCount = 0;
		}
		break;

	case 1:
		mStatePhase ++;mStateCount = 0;
		break;

	case 2:
		for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
			pEfsHandle = &mEfsHandle[iI];

			// �G�t�F�N�g���������Ă��Ȃ��Ȃ�
			if (*pEfsHandle != -1) {
				//					float dist = 10.0f + (mStateCount * 1.25f);
				float dist = 10.0f + (mStateCount * mStateCount * 0.02f);

				eff_pos = EffectManager::GetInstance()->Position(*pEfsHandle);

				cycle = (mStateCount * 0.1f) + (iI * (DX_PI * 0.5f));
				next_pos = VGet(sin(cycle), 0.0f, cos(cycle));

				next_pos = VScale(next_pos, dist);
				next_pos = VAdd(next_pos, mPosition);
				next_pos.y += (mBodyHeight * 0.5f);

				if ((CollisionManager::GetInstance()->CheckEarthCollision(eff_pos, next_pos, &hitpos, &normal) != NOT_ON_COLLISION) ||
					(CollisionManager::GetInstance()->CheckWallCollision(eff_pos, next_pos, &hitpos, &normal))) {
					EffectManager::GetInstance()->Stop(*pEfsHandle, 0.5f);
					*pEfsHandle = -1;

					eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,&eff_pos,&VGet(kENEGY_BALL_HIT_DUST_EFF_SIZE, kENEGY_BALL_HIT_DUST_EFF_SIZE, kENEGY_BALL_HIT_DUST_EFF_SIZE),0.0f);
					EffectManager::GetInstance()->Stop(eff_handle, 0.5f);

					SoundManager::GetSndComponent()->PlaySound(eSE_BURST);

				} else {
					EffectManager::GetInstance()->Position(*pEfsHandle) = next_pos;
				}
			}
		}

		{
			bool is_end = true;
			for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
				if (mEfsHandle[iI] != -1) {
					is_end = false;
				}
			}
			/* �e���S�ď��������ꍇ */
			if (is_end) {
				//<! �I���V�[�P���X�� 
				mStatePhase = 3;mStateCount = 0;
			}
		}
		break;

	case 3:
		/**** �I������ ****/
		for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
			if (mEfsHandle[iI] != -1) {
				EffectManager::GetInstance()->Stop(mEfsHandle[iI], 0.5f);
				mEfsHandle[iI] = -1;
			}
		}
		ChangeState(eCHARA_STATE_IDLE);
		return;

	default:
		break;
	};

	/* �����蔻�菈�� */
	if (p_player->IsDamage() == false) {

		for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
			if (mEfsHandle[iI] == -1) {
				continue; // �I��
			}

			float eff_scale;

			eff_pos = EffectManager::GetInstance()->Position(mEfsHandle[iI]);
			eff_scale = EffectManager::GetInstance()->Scale(mEfsHandle[iI]).x;

			ShapeCapsule capsuleA = ShapeCapsule(p_player->Position(), p_player->Position(), p_player->BodySize());
			ShapeCapsule capsuleB = ShapeCapsule(eff_pos, eff_pos, eff_scale);
			capsuleA.mPosB.y += p_player->BodyHeight();
			capsuleB.mPosB.y += 1.0f;
#ifdef _DEBUG
			GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 2;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), capsuleB.mPosA);
				damage_down_vec.y = 0.0f;
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);

				STOP_EFFECT(mEfsHandle[iI]);
				
				eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,&eff_pos,&VGet(kENEGY_BALL_HIT_DUST_EFF_SIZE, kENEGY_BALL_HIT_DUST_EFF_SIZE, kENEGY_BALL_HIT_DUST_EFF_SIZE),0.0f);
				EffectManager::GetInstance()->Stop(eff_handle, 0.5f);
			}
		}
	}


	mStateCount ++;
}

//---------------------------------------------------
/**
 * �u�Ԉړ�
 */
//---------------------------------------------------
void
CharaEnemyWizard::phaseEnterStateAttack4(void)
{
	mStatePhase = 0;
	mStateCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWizard::phaseStateAttack4(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	float scale_one = 0.0f;
	EFK_HANDLE handle;
	VECTOR ch_pos;
	float ch_rot;
	bool ret = false;
	VECTOR eff_pos;
	VECTOR eff_scale = VGet(12.0f, 12.0f, 12.0f);

	switch (mStatePhase) {
	case 0:
		SoundManager::GetSndComponent()->PlaySound(eSE_MAGIC);

		eff_pos = mPosition;
		handle = EffekseerManager::GetInstance()->Play(eEFK_DISAPPER, &eff_pos, &eff_scale);
		this->DisableShadow();
		mStatePhase ++;mStateCount = 0;
		break;

	case 1:
		scale_one = mScale.x;
		scale_one *= 0.8f;
		if (scale_one < 0.1f) {
			scale_one = 0.0f;
			mStatePhase ++;mStateCount = 0;
			((CharaPlayer *)p_player)->DisableLockonChara();			// ���b�N�I��������������
		}
		mScale = VGet(scale_one, mScale.y, scale_one);
		break;

	case 2:
		if (mIsRandomWarp == true) {
			/* �����_���ړ� */
			int place_index;
			place_index = (19 + GetRand(9-1));
			getPlayerPosRotFromBGModel(&ch_pos, &ch_rot, place_index);

		} else {
			/* ���m�ɋ߂Â��ړ� */
			int near_index = 0;
			VECTOR player_pos = p_player->Position();
			float near_dist = 99999.9f;
			for (int iI = 19; iI < (19 + 9); iI ++) {
				getPlayerPosRotFromBGModel(&ch_pos, &ch_rot, iI);
				float dist = VSize(VSub(player_pos, ch_pos));
				if (near_dist > dist) {
					near_dist = dist;
					near_index = iI;
				}
			}	
			getPlayerPosRotFromBGModel(&ch_pos, &ch_rot, near_index);
		}
		mPosition = ch_pos;
		this->SetRotationToTarget(&p_player->Position());
		mStatePhase ++;mStateCount = 0;

		break;

	case 3:
		/* ���������҂� */
		if (mStateCount > 30) {
			mStatePhase ++;mStateCount = 0;
		}
		break;

	case 4:
		mScale = VGet(0.1f, mScale.y, 0.1f);

		eff_pos = mPosition;
		handle = EffekseerManager::GetInstance()->Play(eEFK_DISAPPER, &eff_pos, &eff_scale);
		this->EnableShadow();
		mStatePhase ++;mStateCount = 0;
		break;

	case 5:
		scale_one = mScale.x;
		scale_one *= 1.1f;
		if (scale_one > 12.0f) {
			scale_one = 12.0f;
			mStatePhase ++;mStateCount = 0;
		}
		mScale = VGet(scale_one, mScale.y, scale_one);
		break;

	case 6:
		ChangeState(eCHARA_STATE_IDLE);
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
CharaEnemyWizard::phaseEnterStateAttack5(void)
{
	StartMotion(eMT_KNIGHT_ATTACK_3, false, 0.6f);
	SoundManager::GetSndComponent()->PlaySound(eSE_MAGIC);

	STOP_EFFEKSEER(mEfkProtect, 0.0f);

	VECTOR eff_pos;
	eff_pos = mPosition;
	eff_pos.y += (mBodyHeight * 0.02f);
	mEfkProtect = EffekseerManager::GetInstance()->Play(eEFK_PROTECT_START, &eff_pos, &mScale);

	mStatePhase = 0;
	mStateCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWizard::phaseStateAttack5(void)
{
	switch (mStatePhase) {
	case 0:

		if (mStateCount > 90) {
			SoundManager::GetSndComponent()->PlaySound(eSE_SHIELD);

			VECTOR eff_scale;
			eff_scale = VScale(mScale, 10.0f);
			eff_scale.y *= 2.0f;
			mProtectEffIndex = EffectManager::GetInstance()->Play(eEF_NAVI,
																  &mPosition,
																  &eff_scale);
			mIsBarrier = true; /* for debug */

			ActionTaskManager::tTaskInfo info;
			info.mTaskType		= ActionTaskManager::eTASK_NAVI_EX;
			info.mTaskValueA	= (long)strNaviTalk_WizardProtectStart;
			info.mTaskValueB	= NULL;
			ActionTaskManager::GetInstance()->NotifyAction(&info);

			mStatePhase ++;mStateCount = 0;
		}
		break;

	case 1:
		if (mStateCount > 120) {
			ChangeState(eCHARA_STATE_IDLE);
		}
		break;

	default:
		break;
	}

	mStateCount ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWizard::phaseEnterStateAttack6(void)
{
	mStatePhase = 0;
	mStateCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWizard::phaseStateAttack6(void)
{
	//	static const float kTHUNDER_EFF_SIZE = 300.0f;
	//	static const float kTHUNDER_EFF_SIZE = 600.0f;
	static const float kTHUNDER_EFF_SIZE = 400.0f;

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR weapon_top_pos;
	p_player->GetFramePosFromName(&weapon_top_pos, "�E���w�P");

	switch (mStatePhase) {
	case 0:
		if (mStateCount > 10) {
			mStatePhase ++; mStateCount = 0;
		}
		break;

	case 1:

		SoundManager::GetSndComponent()->PlaySound(eSE_THUNDER);
		mEfsHandle[0] = EffectManager::GetInstance()->Play(eEF_LIGHTNING_EX,
														   &weapon_top_pos,
														   &VGet(kTHUNDER_EFF_SIZE, kTHUNDER_EFF_SIZE, kTHUNDER_EFF_SIZE),
														   0.0f);
		//		EffectManager::GetInstance()->SetFloat(mEfsHandle[0], "ThunderRand", 100.0f);
		EffectManager::GetInstance()->SetFloat(mEfsHandle[0], "ThunderRand", 200.0f);

		sThunderPos = weapon_top_pos;
		sThunderVec.x = sin(p_player->Rotation().y + DX_PI);
		sThunderVec.y = 0.0f;
		sThunderVec.z = cos(p_player->Rotation().y + DX_PI);
		sThunderVec = VScale(VNorm(sThunderVec), 5000.0f/*speed*/);

		mStatePhase ++; mStateCount = 0;
		break;

	case 2:
		VECTOR next_th_pos, hitpos, normal;
		next_th_pos = VAdd(sThunderPos, sThunderVec);
		
		/**** �G�t�F�N�g�̕\���X�V ****/
		FLOAT4 target;
		target.x = next_th_pos.x;
		target.y = next_th_pos.y;
		target.z = next_th_pos.z;
		target.w = 1.0f;
		EffectManager::GetInstance()->SetVec4(mEfsHandle[0], "TgtPos", &target);

		// �����蔻��
		
		{
			ShapeCapsule capsuleA, capsuleB;
			// �J�~�i���̌`��
			capsuleB.mPosA = next_th_pos;
			capsuleB.mPosB = weapon_top_pos;
			capsuleB.mRadius = 150.0f;
#ifdef _DEBUG
			GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG

			// �L�����Ƃ̓����蔻��
			capsuleA.mPosA	= mPosition;
			capsuleA.mPosB	= mPosition;
			capsuleA.mPosB.y += mBodyHeight;
			capsuleA.mRadius = mBodySize;
		
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {

				if (mIsBarrier == true) {
					SoundManager::GetSndComponent()->PlaySound(eSE_BREAK_BARRIER);
					EffekseerManager::GetInstance()->Play(eEFK_BREAK_BARRIER, &mPosition, &mScale);
				}

				if (mProtectWaitCount == 0) {
					mProtectWaitCount = (60 * 30);
				}

				if (mProtectEffIndex != -1) {
					EffectManager::GetInstance()->Stop(mProtectEffIndex, 0.5f);
					mProtectEffIndex = -1;
				}

				STOP_EFFEKSEER(mEfkProtect, 0.0f);

				mStatePhase = 4;
				break;
			}
		
			// �I������
			if ((CollisionManager::GetInstance()->CheckWallCollision(sThunderPos, next_th_pos, &hitpos, &normal)) ||
				(CollisionManager::GetInstance()->CheckEarthCollision(sThunderPos, next_th_pos, &hitpos, &normal) != NOT_ON_COLLISION)) {
				if (mEfsHandle[0] != -1) {
					EffectManager::GetInstance()->Stop(mEfsHandle[0], 1.00f);
					mEfsHandle[0] = -1;
				}
				mStatePhase ++; mStateCount = 0;
			}
		}

		// ���̍��W���X�V
		sThunderPos = next_th_pos;

		break;

	case 3:
		/* �I������ */
		if (mStateCount == 60) {

			GraphicManager::GetInstance()->RevertFogParam(1.0f);
			
			ChangeState(eCHARA_STATE_IDLE);
		}
		break;

	case 4:
		if (mStateCount >= 15) {

			int attack_power = 0;
			if (mIsBarrier == true) {
				attack_power = 1;
			} else {
				attack_power = 8;
			}

			mIsBarrier = false;		// �o���A�j��

			mFlinchCount = mFlinchMax; //<! ��΂ɂӂ��Ƃ΂�
			VECTOR damage_down_vec;
			damage_down_vec = VNorm(sThunderVec);
			this->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_NORMAL);
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
CharaEnemyWizard::phaseEnterStateAttack7(void)
{
	StartMotion(eMT_KNIGHT_ATTACK_3, false, 0.6f);
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_2);
	SoundManager::GetSndComponent()->PlaySound(eSE_MAGIC);

	VECTOR eff_pos;

	// �����w
	eff_pos = mPosition;
	mEfkHandle = EffekseerManager::GetInstance()->Play(eEFK_MAGIC_SQUARE, &eff_pos, &VScale(mScale, 3.0f));

	for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
		if (mEfsHandle[iI] != -1) {
			EffectManager::GetInstance()->Stop(mEfsHandle[iI], 0.0f);
			mEfsHandle[iI] = -1;
		}

		mEnegyBallVec[iI] = ZERO_VEC;
		mRollballState[iI] = eROLLBALL_STATE_PENDING;
	}

	mStatePhase = 0;
	mStateCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWizard::phaseStateAttack7(void)
{
	static float sAccel = 0.0f;
	static const float sThrowTime = 45.0f;
	static const float size = 50.0f;
	VECTOR eff_vec = ZERO_VEC;
	int* pEfsHandle = NULL;
	float cycle = 0.0f;

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	VECTOR eff_pos;
	VECTOR next_pos;
	VECTOR hitpos, normal;
	int eff_handle = -1;
	VECTOR target;

	static float startVecSize = 200.0f;

	switch (mStatePhase) {
	case 0:
		/* �G�t�F�N�g�S�𐶐� */
		SetRotationToTarget(&p_player->Position());

		if (mStateCount > 10) {
			
			SoundManager::GetSndComponent()->PlaySound(eSE_DARK);
			/*
			// �g��
			mEfsHandle[2] = EffectManager::GetInstance()->Play(eEF_ENERGY_BALL_B, &eff_pos, &VGet(size, size, size), 0.0f);
			mEfsHandle[3] = EffectManager::GetInstance()->Play(eEF_ENERGY_BALL,   &eff_pos, &VGet(size, size, size), 0.0f);
			*/

			VECTOR vec, cross;
			vec = VNorm(VSub(p_player->Position(), mPosition));
			cross = VCross(vec, VGet(0.0f, 1.0f, 0.0f));

			target = mPosition;
			target.y += (mBodyHeight * 0.6f);

			mRollballState[0] = eROLLBALL_STATE_CREATING;
			eff_pos = VAdd(target, VScale(cross, startVecSize));
			mEfsHandle[0] = EffectManager::GetInstance()->Play(eEF_ENERGY_BALL,   &eff_pos, &VGet(size, size, size), 2.0f);

			mRollballState[1] = eROLLBALL_STATE_CREATING;
			eff_pos = VAdd(target, VScale(VNorm(VAdd(cross, VGet(0.0f, 1.0f, 0.0f))), startVecSize));
			mEfsHandle[1] = EffectManager::GetInstance()->Play(eEF_ENERGY_BALL_R, &eff_pos, &VGet(size, size, size), 2.0f);

			mRollballState[2] = eROLLBALL_STATE_CREATING;
			eff_pos = VAdd(target, VScale(VNorm(VAdd(cross, VGet(0.0f, -1.0f, 0.0f))), -startVecSize));
			mEfsHandle[2] = EffectManager::GetInstance()->Play(eEF_ENERGY_BALL_G, &eff_pos, &VGet(size, size, size), 2.0f);

			mRollballState[3] = eROLLBALL_STATE_CREATING;
			eff_pos = VAdd(target, VScale(cross, -startVecSize));
			mEfsHandle[3] = EffectManager::GetInstance()->Play(eEF_ENERGY_BALL_B, &eff_pos, &VGet(size, size, size), 2.0f);

			mStatePhase ++;mStateCount = 0;
		}
		break;

	case 1:
		mStatePhase ++;mStateCount = 0;
		break;

	case 2:

		VECTOR vec;

		eff_pos = mPosition;
		eff_pos.y += (mBodyHeight * 0.6f);

		for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
			pEfsHandle = &mEfsHandle[iI];

			// �G�t�F�N�g���������Ă��Ȃ��Ȃ�
			if (*pEfsHandle != -1) {

				if (mStateCount == (45 + (iI * 20))) {
					mRollballState[iI] = eROLLBALL_STATE_STREET;
					VECTOR target;
					target = p_player->Position();
					target.y += (p_player->BodyHeight() * 0.6f);

					if (mEfsHandle[iI] != -1) {
						vec = VNorm(VSub(target, EffectManager::GetInstance()->Position(mEfsHandle[iI])));
					}
					vec = VScale(vec, 20.0f);
					mEnegyBallVec[iI] = vec;
				}

				if (mRollballState[iI] == eROLLBALL_STATE_STREET) {
					eff_pos = EffectManager::GetInstance()->Position(*pEfsHandle);
					
					mEnegyBallVec[iI] = VScale(mEnegyBallVec[iI], 1.05f);

					next_pos = VAdd(eff_pos, mEnegyBallVec[iI]);
				

					if ((CollisionManager::GetInstance()->CheckEarthCollision(eff_pos, next_pos, &hitpos, &normal) != NOT_ON_COLLISION) ||
						(CollisionManager::GetInstance()->CheckWallCollision(eff_pos, next_pos, &hitpos, &normal))) {
						EffectManager::GetInstance()->Stop(*pEfsHandle, 0.5f);
						*pEfsHandle = -1;

						eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,&eff_pos,&VGet(kENEGY_BALL_HIT_DUST_EFF_SIZE, kENEGY_BALL_HIT_DUST_EFF_SIZE, kENEGY_BALL_HIT_DUST_EFF_SIZE),0.0f);
						EffectManager::GetInstance()->Stop(eff_handle, 0.5f);

						SoundManager::GetSndComponent()->PlaySound(eSE_BURST);

					} else {
						EffectManager::GetInstance()->Position(*pEfsHandle) = next_pos;
					}

				}

			}
		}

		{
			bool is_end = true;
			for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
				if (mEfsHandle[iI] != -1) {
					is_end = false;
				}
			}
			/* �e���S�ď��������ꍇ */
			if (is_end) {
				//<! �I���V�[�P���X�� 
				mStatePhase = 3;mStateCount = 0;
			}
		}
		break;

	case 3:
		/**** �I������ ****/
		for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
			if (mEfsHandle[iI] != -1) {
				EffectManager::GetInstance()->Stop(mEfsHandle[iI], 0.5f);
				mEfsHandle[iI] = -1;
			}
		}
		ChangeState(eCHARA_STATE_IDLE);
		return;

	default:
		break;
	};

	/* �����蔻�菈�� */
	if (p_player->IsDamage() == false) {

		for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
			if (mEfsHandle[iI] == -1) {
				continue; // �I��
			}

			float eff_scale;

			eff_pos = EffectManager::GetInstance()->Position(mEfsHandle[iI]);
			eff_scale = EffectManager::GetInstance()->Scale(mEfsHandle[iI]).x;

			ShapeCapsule capsuleA = ShapeCapsule(p_player->Position(), p_player->Position(), p_player->BodySize());
			ShapeCapsule capsuleB = ShapeCapsule(eff_pos, eff_pos, eff_scale);
			capsuleA.mPosB.y += p_player->BodyHeight();
			capsuleB.mPosB.y += 1.0f;
#ifdef _DEBUG
			GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 2;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), capsuleB.mPosA);
				damage_down_vec.y = 0.0f;
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);

				STOP_EFFECT(mEfsHandle[iI]);
				
				eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,&eff_pos,&VGet(kENEGY_BALL_HIT_DUST_EFF_SIZE, kENEGY_BALL_HIT_DUST_EFF_SIZE, kENEGY_BALL_HIT_DUST_EFF_SIZE),0.0f);
				EffectManager::GetInstance()->Stop(eff_handle, 0.5f);
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
CharaEnemyWizard::phaseEnterStateRetireEnd(void)
{
	STOP_EFFEKSEER(mEfkHandle, 0.0f);
	STOP_EFFEKSEER(mEfkProtect, 0.0f);

	for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
		STOP_EFFECT(mEfsHandle[iI]);
	}

	GraphicManager::GetInstance()->RevertFogParam(0.1f);

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
CharaEnemyWizard::phaseEnterStateDownStart(void)
{
	for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
		if (mEfsHandle[iI] != -1) {
			EffectManager::GetInstance()->Stop(mEfsHandle[iI], 0.5f);
			mEfsHandle[iI] = -1;
		}
	}
		
	GraphicManager::GetInstance()->RevertFogParam(0.1f);

	CharaEnemy::phaseEnterStateDownStart();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyWizard::phaseEnterStateDamage(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_DAMAGE);

	for (int iI = 0; iI < kWIZARD_EFF_NUM; iI ++) {
		if (mEfsHandle[iI] != -1) {
			EffectManager::GetInstance()->Stop(mEfsHandle[iI], 0.5f);
			mEfsHandle[iI] = -1;
		}
	}
		
	GraphicManager::GetInstance()->RevertFogParam(0.1f);

	CharaEnemy::phaseEnterStateDamage();
}


/**** end of file ****/
