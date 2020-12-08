/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 爆弾オークファイル
 * @author SPATZ.
 * @data   2015/12/28 14:46:44
 */
//---------------------------------------------------
#include "../CharaPlayer.h"
#include "CharaEnemyOakBomb.h"
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

static const char* strAttack01_Top = "右中指３";

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyOakBomb::CharaEnemyOakBomb()
{
	mpObjectHand = NULL;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyOakBomb::~CharaEnemyOakBomb()
{
	for (int iI = 0; iI < kOAK_BOMB_NUM; iI ++) {
		mObjectBomb[iI].ProcessDisappear();
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyOakBomb::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	for (int iI = 0; iI < kOAK_BOMB_NUM; iI ++) {
		mObjectBomb[iI].State() = ObjectHand::eHAND_STATE_DISAPPEAR;
		mObjectBomb[iI].Position() = mPosition;
	}

	//	mpObjectHand = NULL;

	mpObjectHand = &mObjectBomb[0];
	PRINT_CONSOLE("Raise Bomb Index = %d \n", 0);

	((ObjectHand_Bomb *)mpObjectHand)->Setup(eOBJECT_KIND_BOMB, (eItemKind)0, (VECTOR *)&ZERO_VEC, 0, true);
	mpObjectHand->SetMasterChara(this);
	
	int node_index = this->GetFrameIndexFromName("右中指３");
	AttachModel(mpObjectHand, ModelPmd::eATTACHED_MODEL_MOTION,	node_index);

	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	mFlinchMax = 250;
}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyOakBomb::checkChangeAttackState(int* pAttackCharaState)
{
	if (mpObjectHand == NULL) {
		*pAttackCharaState = eCHARA_STATE_ATTACK_2;
		return true;
	}

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);

	int rand_value = GetRand(100);
	float rand_rate = (float)rand_value * 0.01f;
	float dist = VSize(vec);

	if (dist < (mBodySize * 1.0f) + ((mBodySize * 15.0f) * rand_rate) ) {
		*pAttackCharaState = eCHARA_STATE_ATTACK_1;
		return true;
	}
	
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyOakBomb::phaseStateWaitAttack(void)
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
CharaEnemyOakBomb::processNoMoveAction(void)
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
CharaEnemyOakBomb::phaseEnterStateIdle(void)
{
	CharaEnemy::phaseEnterStateIdle();

	if (mpObjectHand == NULL) {
		for (int iI = 0; iI < kOAK_BOMB_NUM; iI ++) {
			if (mObjectBomb[iI].State() == ObjectHand::eHAND_STATE_DISAPPEAR) {
				mpObjectHand = &mObjectBomb[iI];
				PRINT_CONSOLE("Raise Bomb Index = %d \n", iI);

				((ObjectHand_Bomb *)mpObjectHand)->Setup(eOBJECT_KIND_BOMB, (eItemKind)0, (VECTOR *)&ZERO_VEC, 0, true);
				mpObjectHand->SetMasterChara(this);

				ChangeAttachModelPtr(0, mpObjectHand);
				ResumeAttach(0);

				break;
			}
		}
	}
	

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyOakBomb::phaseEnterStateAttack1(void)
{
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);

	StartMotion(eMT_KNIGHT_ATTACK_2, false, 0.24f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyOakBomb::phaseStateAttack1(void)
{
	if (EqualPhaseFrame(6.0f)) {

		int rand_value = GetRand(5);
		
		VECTOR throw_vec = {0};
		float set_rot = (mRotation.y + DX_PI) + (Utility::DegToRad(3.0f) * rand_value) - Utility::DegToRad(9.0f);

		CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
		float dist = VSize(VSub(p_player->Position(), mPosition) );
		float rate = dist / (mBodySize * 10.0f);
		if (rate > 1.0f) {
			rate = 1.0f;
		}

		throw_vec.x = sin(set_rot);
		throw_vec.y = 0.0f;
		throw_vec.z = cos(set_rot);
		throw_vec = VScale(throw_vec, (22.0f * rate));
		throw_vec.y = 16.0f;

		PauseAttach(0);

		APP_ASSERT((mpObjectHand != NULL), "mpObjectHand is NULL \n");
		Utility::ExtractMatrixPosition(&mpObjectHand->Position(), &mpObjectHand->Matrix() );
		mpObjectHand->ProcessThrow(&throw_vec);
	}

	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyOakBomb::phaseEnterStateAttack2(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyOakBomb::phaseStateAttack2(void)
{
	for (int iI = 0; iI < kOAK_BOMB_NUM; iI ++) {
		if (mObjectBomb[iI].State() == ObjectHand::eHAND_STATE_DISAPPEAR) {
			ChangeState(eCHARA_STATE_IDLE);
			break;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyOakBomb::phaseEnterStateDamage(void)
{
	mIsAttack = false;
	StartMotion(eMT_DAMAGE, false, 1.5f);
	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_DAMAGE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyOakBomb::phaseEnterStateRetireEnd(void)
{
	for (int iI = 0; iI < kOAK_BOMB_NUM; iI ++) {
		mObjectBomb[iI].ProcessDisappear();
	}

	CharaEnemy::phaseEnterStateRetireEnd();
}

 /**** end of file ****/
