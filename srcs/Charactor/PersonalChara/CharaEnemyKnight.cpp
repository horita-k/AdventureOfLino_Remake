/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * �M�M�l�u���t�@�C��
 * @author SPATZ.
 * @data   2012/05/25 00:27:32
 */
//---------------------------------------------------
#include "CharaEnemyKnight.h"
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
CharaEnemyKnight::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// �G�x�[�X�N���X��Setup�K�v
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	if (mWeaponModel.GetModelHandle() == -1) {
		mWeaponModel.LinkModel(RES_MDL_SWORD);
		mWeaponModel.Setup();
	}
	this->AttachModel(&mWeaponModel,
					  ModelPmd::eATTACHED_MODEL_NORMAL,
					  this->GetFrameIndexFromName("�E���"));
	GraphicManager::GetInstance()->EntryDrawModel(&mWeaponModel);
}

//---------------------------------------------------
/**
 * �L�����̍U���X�e�[�^�X�J�ڂ̃`�F�b�N�֐�
 */
//---------------------------------------------------
bool
CharaEnemyKnight::checkChangeAttackState(int* pAttackCharaState)
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

/*=====================================*
 * extend
 *=====================================*/

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyKnight::Update(void)
{
	CharaEnemy::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyKnight::Destroy(void)
{
	CharaEnemy::Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyKnight::phaseEnterStateAttack1(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	VECTOR weapon_top_pos, weapon_btm_pos;
	this->GetFramePosFromName(&weapon_top_pos, "�E���");
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "����");
	mBladeEffect.Play(&weapon_top_pos, &weapon_btm_pos);

	StartMotion(eMT_KNIGHT_ATTACK_1, false, 0.35f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyKnight::phaseStateAttack1(void)
{
	static float sStart = 5.0f;
	static float sEnd = 12.0f;

	bool is_prev_attack;
	is_prev_attack = mIsAttack;
	if ((GetMotionNowTime() >= sStart) &&
		(GetMotionNowTime() <= sEnd)) {
		mIsAttack = true;

		CharaBase* p_player;
		p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		/* �퓬�s�\�҂Ɗ��ɍU�����ꂽ�҂����� */
		if ((p_player->GetIsRetire() == false) &&
			(p_player->IsDamage() == false)) {

			// �G�̃m�[�h�ƃL�����̉~���̓����蔻��
			CollisionManager::tSphereWork sphere;
			CollisionManager::tPillarWork piller;
			this->GetFramePosFromName(&sphere.mPosition, "�E���");
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
			
				int attack_power = 2;
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
	this->GetFramePosFromName(&weapon_top_pos, "�E���");
	mWeaponModel.GetFramePosFromName(&weapon_btm_pos, "����");
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
CharaEnemyKnight::phaseEnterStateRetireEnd(void)
{
	GraphicManager::GetInstance()->ReserveRemoveModel(&mWeaponModel);

	mWeaponModel.SetAlpha(0.0f);
	mWeaponModel.DeleteModel();

	CharaEnemy::phaseEnterStateRetireEnd();
}

 /**** end of file ****/
