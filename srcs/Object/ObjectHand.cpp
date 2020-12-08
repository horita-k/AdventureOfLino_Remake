/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * オブジェクトベースクラス
 * @author SPATZ.
 * @data   2012/04/14 19:35:52
 */
//---------------------------------------------------
#include "ObjectHand.h"
#include "AppLib/Basic/Basic.h"
#include "DataDefine/MotionDef.h"
#include "../Charactor/CharaPlayer.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/EffekseerManager.h"
#include "AppLib/Effect/EffectManager.h"

#include "AppLib/Collision/CollisionManager.h"
#include "AppLib/Resource/ResourceModel.h"

#include "AppLib/Sound/SoundManager.h"

#include "Scene/ActionTaskManager.h"
#include "Scene/SceneInfo.h"
#include "Scene/CommandFunctions.h"
#include "Utility/Utility.h"

#include "Gimmick/GimmickBase.h"

#include "DataDefine/EffectDef.h"
#include "DataDefine/VoiceDef.h"

//#include "CharaInfo.h"

/*=====================================*
 * static
 *=====================================*/
/*static*/
ObjectHand::PHASE_STATE
ObjectHand::mPhaseState[ObjectHand::eHAND_STATE_MAX] = {
	&ObjectHand::phaseStateOnEarth,
	&ObjectHand::phaseStateRaise,
	&ObjectHand::phaseStateThrowing,
	&ObjectHand::phaseStateDisappear,
	&ObjectHand::phaseStateOnGimmick,
};

ObjectHand_Bomb::PHASE_BOMB_STATE
ObjectHand_Bomb::mPhaseState[ObjectHand_Bomb::eHAND_BOMB_STATE_MAX] = {
	&ObjectHand::phaseStateOnEarth,
	//	&ObjectHand::phaseStateRaise,
	&ObjectHand_Bomb::phaseBombStateRaise,
	//	&ObjectHand::phaseStateThrowing,
	&ObjectHand_Bomb::phaseBombStateThrowing,
	&ObjectHand::phaseStateDisappear,
	&ObjectHand::phaseStateOnGimmick,
	&ObjectHand_Bomb::phaseBombStateWaitBurst,
	&ObjectHand_Bomb::phaseBombStateBurstting,
};

/*=====================================*
 * class ObjectHand
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectHand::ObjectHand()
{
	mHandState	= eHAND_STATE_ON_EARTH;
	mMoveVec	= ZERO_VEC;
	mGravity	= 0.0f;
	mHandType = eHAND_TYPE_ERROR;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectHand::~ObjectHand()
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg)
{
	mpMaster = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	ObjectBase::Setup(objectIdx, itemIdx, pSetPos, rotDeg);
	mObjectType = eOBJECT_TYPE_HAND;

	// モデルの挿入先を派生クラスで設定させる
	mpEntryTargetModel = NULL;
	//	this->InsertDrawModelSelf();
	GraphicManager::GetInstance()->ReserveEntryModel(this);

	CollisionManager::GetInstance()->EntryBodyCollision(this);
	CollisionManager::GetInstance()->EntryAttackCollision(this);

	mHandState	= eHAND_STATE_ON_EARTH;
	mMoveVec	= ZERO_VEC;
	mGravity	= 0.0f;

	mHandType = eHAND_TYPE_ERROR;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand::Update(void)
{
	(this->*mPhaseState[mHandState])();

	ObjectBase::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ObjectHand::GetIsRetire(void)
{
	return (mHandState == eHAND_STATE_DISAPPEAR) ? true : false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ObjectHand::CheckDamageCollNormal(tCheckDamageWork* pParam)
{
	return ModelCollision::CheckDamageCollNormal(pParam);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*
void
ObjectHand::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown)
{
	// 破壊処理
	processBreakObject();
}
*/

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType/*=eATTACK_TYPE_NORMAL*/)
{
	if (GetIsRetire() == false) {
		// 破壊処理
		processBreakObject();
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand::SetUsed(void)
{
	return;

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand::StartCheckEvent(void)
{
	if ( (mHandState == eHAND_STATE_ON_EARTH) ||
		 (mHandState == eHAND_STATE_ON_GIMMICK) ) {

		CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
		if (p_player->GetObjectHandModel() == NULL) {

			if ((p_player->CosplayIndex() != eWINMENU_COSPLAY_DABAH) &&
				(p_player->CosplayIndex() != eWINMENU_COSPLAY_BAT) &&
				(p_player->CosplayIndex() != eWINMENU_COSPLAY_SUNOPHONE) ) {

				p_player->SetObjectHandModel(this);
				p_player->ChangeState(CharaPlayer::eCHARA_STATE_RAISE_IDLE);

				mGravity = 0.0f;
				mMoveVec = ZERO_VEC;

				// 現在押されている入力を無効にする
				InputManager::GetInstance()->ResetNowTrigJoy();

				// 攻撃用当たり判定から解除
				CollisionManager::GetInstance()->RemoveAttackCollision(this);

				mHandState = eHAND_STATE_RAISE;

			}
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand::ProcessThrow(VECTOR* pThrowVec)
{
	//	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	mHandState = eHAND_STATE_THROWING;

	mGravity = 0.0f;
	mMoveVec = *pThrowVec;

	/*
	if (p_player->GetObjectHandModel() == this) {
		// 投げた時にプレイヤーの登録ポインタを解除させる
		p_player->SetObjectHandModel(NULL);
	}
	*/
	if (mpMaster->GetObjectHandModel() == this) {
		// 投げた時にプレイヤーの登録ポインタを解除させる
		mpMaster->SetObjectHandModel(NULL);
	}

	if (mMatrixType == eMATRIX_TYPE_MATRIX) {
		Utility::ExtractMatrixPosition(&mPosition, &mMatrix);
		mMatrixType = eMATRIX_TYPE_TRS;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand::phaseStateOnEarth(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand::phaseStateRaise(void)
{
#if 0
	// デバッグ用、カプセル表示
	ShapeCapsule capsuleA;
	capsuleA.mPosA	= this->Position();
	capsuleA.mPosA.y += this->BodySize() * 0.5f;
	capsuleA.mPosB	= capsuleA.mPosA;
	capsuleA.mPosB.y += 0.01f;
	capsuleA.mRadius= this->BodySize();

	GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleA);
#endif // 0
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand::phaseStateThrowing(void)
{
	VECTOR hitpos, normal;

	mGravity += (kGRAVITY_SPEED * 0.15f);
	mMoveVec.y += mGravity;

	// 敵との当たり判定
	processHitEnemy(NULL);

	VECTOR add_vec = VAdd(mPosition, mMoveVec);
	int hit_frame_index;
	BYTE earth_mat_index;
	INT8 on_coll_index = CollisionManager::GetInstance()->CheckEarthCollision(mPosition, 
																			  add_vec,
																			  &hitpos, 
																			  &normal,
																			  &hit_frame_index,
																			  &earth_mat_index);
	// 壁か地面に爆弾が当たった場合
	if ( (on_coll_index != NOT_ON_COLLISION) ||
		 (CollisionManager::GetInstance()->CheckWallCollision(mPosition, add_vec, &hitpos, &normal))) {
		processHitCollision(&normal);
		if (mHandState == eHAND_STATE_ON_EARTH) {
			GimmickBase* p_gimmik = (GimmickBase *)GimmickBase::GetGimmickBaseFromEarthCollIndex(on_coll_index);
			if (p_gimmik != NULL) {
				p_gimmik->SetCharaPositionToFrameMatrix(this, hit_frame_index);
				mHandState = eHAND_STATE_ON_GIMMICK;
			}
		}
	} else {
		mPosition = VAdd(mPosition, mMoveVec);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand::phaseStateDisappear(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand::phaseStateOnGimmick(void)
{
	/*
	VECTOR hitpos, normal;

	mGravity += (kGRAVITY_SPEED * 0.15f);
	mMoveVec.y += mGravity;

	VECTOR add_vec = VAdd(mPosition, mMoveVec);
	int hit_frame_index;
	BYTE earth_mat_index;
	INT8 on_coll_index = CollisionManager::GetInstance()->CheckEarthCollision(mPosition, 
																			  add_vec,
																			  &hitpos, 
																			  &normal,
																			  &hit_frame_index,
																			  &earth_mat_index);

	GimmickBase* p_gimmik =NULL;

	// 地面に当たった場合
	if (on_coll_index != NOT_ON_COLLISION) {
		//		processHitCollision(&normal);
		//		mPosition = hitpos;
		this->SetPositionOnCollision();

		p_gimmik = (GimmickBase *)GimmickBase::GetGimmickBaseFromEarthCollIndex(on_coll_index);
		if (p_gimmik != NULL) {
			p_gimmik->SetCharaPositionToFrameMatrix(this, hit_frame_index);
			mHandState = eHAND_STATE_ON_GIMMICK;
		}
	}

	mGravity = 0.0f;
	*/

	/*
	// 当たっていない場合は通常のコリジョン状態に戻す
	if (p_gimmik == NULL) {
		mHandState = eHAND_STATE_ON_EARTH;
	}
	*/

	//	mGravity += (kGRAVITY_SPEED * 0.15f);
	mGravity += (kGRAVITY_SPEED * 0.1f);
	mMoveVec.y += mGravity;

	VECTOR top_pos = VGet(mPosition.x, 
						  mPosition.y + (mBodySize * 2.0f),
						  mPosition.z);
	VECTOR btm_pos = VAdd(mPosition, mMoveVec);
	int hit_frame_index;
	BYTE earth_mat_index;
	VECTOR hitpos, normal;
	INT8 on_coll_index = CollisionManager::GetInstance()->CheckEarthCollision(btm_pos, 
																			  top_pos,
																			  &hitpos, 
																			  &normal,
																			  &hit_frame_index,
																			  &earth_mat_index);
	GimmickBase* p_gimmik =NULL;
	// 地面に当たった場合
	if (on_coll_index != NOT_ON_COLLISION) {

		mGravity = 0.0f;
		this->SetPositionOnCollision();

		//		this->SetPositionOnCollision();
		p_gimmik = (GimmickBase *)GimmickBase::GetGimmickBaseFromEarthCollIndex(on_coll_index);
		if (p_gimmik != NULL) {
			PRINT_SCREEN(GetColor(255, 0, 0), "on collision node");
			p_gimmik->SetCharaPositionToFrameMatrix(this, hit_frame_index);
		} else {
			mHandState = eHAND_STATE_ON_EARTH;
		}
	} else {
		mPosition = VAdd(mPosition, mMoveVec);
	}
	


}

/*=====================================*
 * class ObjectHand_Jar
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Jar::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg)
{
	ObjectHand::Setup(objectIdx, itemIdx, pSetPos, rotDeg);

	mHandType = eHAND_TYPE_JAR;
}

//---------------------------------------------------
/**
 * コリジョンにあたった時の処理を実行
 */
//---------------------------------------------------
void
ObjectHand_Jar::processHitCollision(void* pParam)
{
	if (VSize(mMoveVec) > abs(kGRAVITY_SPEED)) {
		// 壺がコリジョンに当たった場合は割れる処理を実行
		processBreakObject();
	}
	else {
		// 攻撃用当たり判定に再登録
		CollisionManager::GetInstance()->EntryAttackCollision(this);

		this->SetPositionOnCollision();
		mHandState = eHAND_STATE_ON_EARTH;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Jar::processHitEnemy(void* pParam)
{
	VECTOR tmp;

	tmp = mMoveVec;
	tmp.y = 0.0f;
	if (VSize(tmp) > 0.0f) {
		ModelCollision* p_enemy = NULL;
		int attack_coll_num = CollisionManager::GetInstance()->GetAttackCollisionNum();
		ShapeCapsule capsule;

		this->GetBodyCapsule(&capsule);
		
		for (int iI = 1; iI < attack_coll_num; iI ++) {

			p_enemy = (ModelCollision *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);

			/* 戦闘不能者と既に攻撃された者を除く */
			if ((p_enemy->GetIsRetire() == false) &&
				(p_enemy->IsDamage() == false)) {

				if (p_enemy->CheckDamageCollHand(&capsule)) {

					PRINT_CONSOLE("Hit Jar to enemy !!!!!!!!!!!!!!!!!!!!! \n");
					int attack_power = 4;
					VECTOR damage_down_vec;
					damage_down_vec = VSub(p_enemy->Position(), mPosition);
					damage_down_vec = VNorm(damage_down_vec);
					p_enemy->ProcessDamage(attack_power, false, &mPosition, &damage_down_vec, true, eATTACK_TYPE_HAND);

					processHitCollision(NULL);	// 壺の場合
					return;
				}
			}
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Jar::processBreakObject(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	// 壺が割れる処理
	SoundManager::GetSndComponent()->PlaySound(eSE_BREAK_JAR);

	EFK_HANDLE efk_hdl;
	efk_hdl = EffekseerManager::GetInstance()->Play(eEFK_BREAK_JAR, &mPosition, &VScale(mScale, 1.0f));
	EffekseerManager::GetInstance()->SetSpeed(efk_hdl, 2.5f);

	CollisionManager::GetInstance()->RemoveBodyCollision(this);
	GraphicManager::GetInstance()->ReserveRemoveModel(this);
	ModelPmd::DeleteModel();

	// ドロップアイテム生成
	VECTOR item_pos, vec;
	vec = VSub(mPosition, p_player->Position());
	vec = VScale(VNorm(vec), 40.0f);
	item_pos = VAdd(vec, mPosition);
	int rnd = GetRand(10);

	switch (mObjectIndex) {
	case eOBJECT_KIND_JAR_TYPE_A:
		switch (rnd) {
		case 0:case 1:
			createDropItem(eOBJECT_KIND_MONEY_1, item_pos);		break;
		case 2:
			createDropItem(eOBJECT_KIND_MONEY_10, item_pos);	break;
		case 3: case 4:
			createDropItem(eOBJECT_KIND_HEART, item_pos);		break;
		default:												break;
		};
		break;

	case eOBJECT_KIND_JAR_TYPE_B:
		switch (rnd) {
		case 0:
			createDropItem(eOBJECT_KIND_HEART, item_pos);		break;
		case 1:
			createDropItem(eOBJECT_KIND_MONEY_10, item_pos);	break;
		case 2: case 3: case 4: case 5:
			createDropItem(eOBJECT_KIND_PLATE_BOMB, item_pos);	break;
		default:												break;
		};
		break;

	case eOBJECT_KIND_JAR_TYPE_C:
		switch (rnd) {
		case 0:
			createDropItem(eOBJECT_KIND_MONEY_5, item_pos);		break;
		case 1:
			createDropItem(eOBJECT_KIND_MONEY_10, item_pos);	break;
		case 2:
			createDropItem(eOBJECT_KIND_HEART, item_pos);		break;
		case 3: case 4: case 5: case 6:
			createDropItem(eOBJECT_KIND_PLATE_ARROW, item_pos);	break;
		default:												break;
		};
		break;

	default:
		APP_ASSERT_PRINT((NULL), 
						 "Invalid Object Kind index : %d \n", mObjectIndex);
		break;
	}

	mIsUsed = true;
	ObjectBase::RemoveActiveObjectList(this);

	mHandState = eHAND_STATE_DISAPPEAR;
}

/*=====================================*
 * class ObjectHand_Stone
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Stone::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg)
{
	ObjectHand::Setup(objectIdx, itemIdx, pSetPos, rotDeg);

	// 
	mIsUsed = false;
	mHandType = eHAND_TYPE_STONE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Stone::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType/*=eATTACK_TYPE_NORMAL*/)
{
	if (attackType == eATTACK_TYPE_BOMB) {
		// 破壊処理
		processBreakObject();
	} else {
		SoundManager::GetSndComponent()->PlaySound(eSE_SHIELD);
	}
}

//---------------------------------------------------
/**
 * コリジョンにあたった時の処理を実行
 */
//---------------------------------------------------
void
ObjectHand_Stone::processHitCollision(void* pParam)
{
	VECTOR normal = *(VECTOR *)pParam;

	if (VSize(mMoveVec) > abs(kGRAVITY_SPEED)) {
		// 壺がコリジョンに当たった場合は割れる処理を実行
		//		processBreakObject();

		if (abs(mMoveVec.y) > abs(mGravity * 5.0f)) {
			SoundManager::GetSndComponent()->PlaySound(eSE_FALL_STONE);
		}

		VECTOR tmp;
		tmp = VScale(VNorm(normal), VSize(mMoveVec));
		mMoveVec = VScale(VAdd(mMoveVec, tmp), 0.15f);

		this->ProcessThrow(&mMoveVec);

	} else {
		// 攻撃用当たり判定に再登録
		CollisionManager::GetInstance()->EntryAttackCollision(this);

		this->SetPositionOnCollision();
		mHandState = eHAND_STATE_ON_EARTH;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Stone::processHitEnemy(void* pParam)
{
	VECTOR tmp;

	tmp = mMoveVec;
	tmp.y = 0.0f;
	if (VSize(tmp) > 0.0f) {
		ModelCollision* p_enemy = NULL;
		int attack_coll_num = CollisionManager::GetInstance()->GetAttackCollisionNum();
		ShapeCapsule capsule;

		this->GetBodyCapsule(&capsule);
		
		for (int iI = 1; iI < attack_coll_num; iI ++) {

			p_enemy = (ModelCollision *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);

			/* 戦闘不能者と既に攻撃された者を除く */
			if ((p_enemy->GetIsRetire() == false) &&
				(p_enemy->IsDamage() == false)) {

				if (p_enemy->CheckDamageCollHand(&capsule)) {

					PRINT_CONSOLE("Hit Jar to enemy !!!!!!!!!!!!!!!!!!!!! \n");
				
					int attack_power = 4;
					VECTOR damage_down_vec;
					damage_down_vec = VSub(p_enemy->Position(), mPosition);
					damage_down_vec = VNorm(damage_down_vec);
					p_enemy->ProcessDamage(attack_power, false, &mPosition, &damage_down_vec, true, eATTACK_TYPE_HAND);

					//					processHitCollision(NULL);	// 壺の場合
					processBreakObject();
					return;
				}
			}
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Stone::processBreakObject(void)
{
	// 壺が割れる処理
	//	SoundManager::GetSndComponent()->PlaySound(eSE_BREAK_JAR);

	//	EFK_HANDLE efk_hdl;
	//	efk_hdl = EffekseerManager::GetInstance()->Play(eEFK_BREAK_JAR, &mPosition, &VScale(mScale, 1.0f));
	//	EffekseerManager::GetInstance()->SetSpeed(efk_hdl, 2.5f);


	SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
	const float size = 15.0f;
	int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,
														&mPosition,
														&VGet(size, size, size),
														0.0f);
	EffectManager::GetInstance()->Stop(eff_handle, 5.0f);


	CollisionManager::GetInstance()->RemoveBodyCollision(this);
	GraphicManager::GetInstance()->ReserveRemoveModel(this);
	ModelPmd::DeleteModel();

	mIsUsed = true;
	ObjectBase::RemoveActiveObjectList(this);

	mHandState = eHAND_STATE_DISAPPEAR;
}

/*=====================================*
 * ObjectHand_Sunohara
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Sunohara::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType/*=eATTACK_TYPE_NORMAL*/)
{
	// 何も攻撃を受け付けない
	SoundManager::GetSndComponent()->PlaySound(eSE_SHIELD);
}

/*=====================================*
 * class ObjectHand_Usachu
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Usachu::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg)
{
	ObjectHand_Stone::Setup(objectIdx, itemIdx, pSetPos, rotDeg);
	
	mHandType = eHAND_TYPE_USACHU;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Usachu::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType/*=eATTACK_TYPE_NORMAL*/)
{
	// 何も攻撃を受け付けない
	SoundManager::GetSndComponent()->PlaySound(eSE_SHIELD);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Usachu::processHitCollision(void* pParam)
{
	ObjectHand_Stone::processHitCollision(pParam);
	SoundManager::GetSndComponent()->PlaySound(eSE_COMICAL);
}

/*=====================================*
 * class ObjectHand_Lock
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Lock::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg)
{
	ObjectHand::Setup(objectIdx, itemIdx, pSetPos, rotDeg);

	mHandType = eHAND_TYPE_LOCK;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Lock::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType/*=eATTACK_TYPE_NORMAL*/)
{
	if ((attackType == eATTACK_TYPE_BOMB) ||
		(attackType == eATTACK_TYPE_ARROW)) {
		// 破壊処理
		processBreakObject();
	} else {
		SoundManager::GetSndComponent()->PlaySound(eSE_SHIELD);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Lock::processBreakObject(void)
{
	// 岩が割れる処理
	SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
	const float size = 115.0f;
	VECTOR effpos = mPosition;
	int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,
														&effpos,
														&VGet(size, size, size),
														0.0f);
	EffectManager::GetInstance()->Stop(eff_handle, 5.0f);

	CollisionManager::GetInstance()->RemoveBodyCollision(this);
	GraphicManager::GetInstance()->ReserveRemoveModel(this);
	ModelPmd::DeleteModel();

	mIsUsed = true;
	ObjectBase::RemoveActiveObjectList(this);

	mHandState = eHAND_STATE_DISAPPEAR;
}

/*=====================================*
 * class ObjectHand_Bomb
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectHand_Bomb::ObjectHand_Bomb()
{
	mFireEffIndex = -1;
	mBurstTimer = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectHand_Bomb::~ObjectHand_Bomb()
{
	if (mFireEffIndex != -1) {
		EffectManager::GetInstance()->Stop(mFireEffIndex, 0.0f);
		mFireEffIndex = -1;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Bomb::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg, bool isRaise/*=false*/)
{
	ObjectHand::Setup(objectIdx, itemIdx, pSetPos, rotDeg);

	this->StartMotion(0, false, 0.5f);

	if (isRaise) {
		mHandState = eHAND_STATE_RAISE;
	}
	mHandType = eHAND_TYPE_BOMB;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Bomb::Update(void)
{
	(this->*mPhaseState[mHandState])();

	ObjectBase::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ObjectHand_Bomb::GetIsRetire(void)
{
	if ((mHandState == eHAND_STATE_DISAPPEAR) ||
		(mHandState == eHAND_BOMB_STATE_BURSTTING)) {
		//	if (mHandState == eHAND_STATE_DISAPPEAR) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Bomb::ProcessThrow(VECTOR* pThrowVec)
{
	ObjectHand::ProcessThrow(pThrowVec);

	VECTOR effpos;
	this->GetFramePosFromName(&effpos, "火先");
	if (mFireEffIndex != -1) {
		EffectManager::GetInstance()->Position(mFireEffIndex) = VAdd(effpos, mMoveVec);
	}
	mReboundNum = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Bomb::ProcessDisappear(void)
{
	if (mHandState != eHAND_STATE_DISAPPEAR) {
		CollisionManager::GetInstance()->RemoveAttackCollision(this);

		CollisionManager::GetInstance()->RemoveBodyCollision(this);
		GraphicManager::GetInstance()->ReserveRemoveModel(this);
		//	GraphicManager::GetInstance()->RemoveDrawModel(this);
		ModelPmd::DeleteModel();

		mHandState = eHAND_STATE_DISAPPEAR;
	}

	STOP_EFFECT(mFireEffIndex);
}

//---------------------------------------------------
/**
 * コリジョンにあたった時の処理を実行
 */
//---------------------------------------------------
void
ObjectHand_Bomb::processHitCollision(void* pParam)
{
	VECTOR normal = *(VECTOR *)pParam;

	// 移動量が指定以下の場合は停止させる
	if ((abs(mMoveVec.y) <= abs(mGravity)) ||
		(mReboundNum > 6)) {
		// 通常のアイテムに戻す
		mGravity = 0.0f;
		mMoveVec = ZERO_VEC;
		this->SetPositionOnCollision();

		// 攻撃用当たり判定から再びエントリー
		CollisionManager::GetInstance()->EntryAttackCollision(this);

		//		mHandState = eHAND_STATE_ON_EARTH;
		mBurstTimer = 0;
		mHandState = eHAND_BOMB_STATE_WAIT_BURST;
	}
	else {
		//<! リバウンドする処理

		if (abs(mMoveVec.y) > abs(mGravity * 3.0f)) {
			SoundManager::GetSndComponent()->PlaySound(eSE_COMICAL);
		}

		VECTOR tmp;
		tmp = VScale(VNorm(normal), VSize(mMoveVec));
		mMoveVec = VScale(VAdd(mMoveVec, tmp), 0.55f);

		this->ProcessThrow(&mMoveVec);

		mReboundNum ++;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Bomb::processHitEnemy(void* pParam)
{
	VECTOR tmp;

	tmp = mMoveVec;
	tmp.y = 0.0f;
	if (VSize(tmp) > 0.0f) {
		ModelCollision* p_enemy = NULL;
		int attack_coll_num = CollisionManager::GetInstance()->GetAttackCollisionNum();
		ShapeCapsule capsule;

		this->GetBodyCapsule(&capsule);

		for (int iI = 0; iI < attack_coll_num; iI ++) {
			p_enemy = (ModelCollision *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
			if (p_enemy == mpMaster) continue;
			if (p_enemy == this) continue;

			/* 戦闘不能者と既に攻撃された者を除く */
			if ((p_enemy->GetIsRetire() == false) &&
				(p_enemy->IsDamage() == false)) {

				if (p_enemy->CheckDamageCollBomb(&capsule)) {

					PRINT_CONSOLE("Hit Bomb to Enemy!!!!!!!!!!!!!!!!!!!!! \n");
					
					processBreakObject();

					return;
				}
			}
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Bomb::processBreakObject(void)
{
	// 爆発処理を行う
	SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
	EFK_HANDLE efk_hdl;
	efk_hdl = EffekseerManager::GetInstance()->Play(eEFK_EXPLOSION_BOMB, &mPosition, &VScale(mScale, 0.8f));
//	efk_hdl = EffekseerManager::GetInstance()->Play(eEFK_EXPLOSION_BOMB, &mPosition, 0.8f);
	EffekseerManager::GetInstance()->SetSpeed(efk_hdl, 0.75f);
		
	if (mFireEffIndex != -1) {
		EffectManager::GetInstance()->Stop(mFireEffIndex, 0.0f);
		mFireEffIndex = -1;
	}

	this->SetVisible(false);

	CameraManager::GetInstance()->StartShakeCamera(0.6f, 3.0f, 80.0f);

	mBurstTimer = 0;

	mIsUsed = true;
	ObjectBase::RemoveActiveObjectList(this);

	mHandState = eHAND_BOMB_STATE_BURSTTING;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Bomb::phaseBombStateRaise(void)
{
	VECTOR effpos;
	this->GetFramePosFromName(&effpos, "火先");

	if (mFireEffIndex == -1) {
		const float size = 40.0f;
		mFireEffIndex = EffectManager::GetInstance()->Play(eEF_FLAME_BOMB,
														   &effpos,
														   &VGet(size, size, size),
														   0.0f);
	}
	EffectManager::GetInstance()->Position(mFireEffIndex) = VAdd(effpos, mMoveVec);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Bomb::phaseBombStateThrowing(void)
{
	VECTOR effpos;
	this->GetFramePosFromName(&effpos, "火先");
	if (mFireEffIndex != -1) {
		EffectManager::GetInstance()->Position(mFireEffIndex) = VAdd(effpos, mMoveVec);
	}
	ObjectHand::phaseStateThrowing();

	// プレイヤーとの距離が離れすぎた場合、自動で消す
	if (VSize(VSub(mpMaster->Position(), mPosition) ) > 50000.0f) {
		mHandState = eHAND_STATE_DISAPPEAR;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Bomb::phaseBombStateWaitBurst(void)
{
	static const float kBOMB_BURST_FRAME = 120.0f;

	float now_scale = 0.0f;
	float period = 0.0f;
	COLOR_F color = {1.0f, 1.0f, 1.0f, 1.0f};
	float amp = 0.3f;

	//	static float sPeriodRate = 0.08f;
	static float sPeriodRate = 0.16f;

	period = ((float)mBurstTimer * sPeriodRate);

	float rate = 0.0f;

	rate = ((float)mBurstTimer / 60.0f);

	// ３秒経ったら爆発させる
	if (mBurstTimer >= (int)kBOMB_BURST_FRAME) {
		mBurstTimer = 0;

		processBreakObject();
		return;
	} else {
		period	*= (1.0f + (rate * 0.5f));
		amp		*= (1.0f + (rate * 1.0f));
		color.a = 1.0f + (rate * rate * 100.0f);
		color.g = (float)(kBOMB_BURST_FRAME - mBurstTimer) / kBOMB_BURST_FRAME;
		color.b = color.g;

		if (mFireEffIndex != -1) {
			VECTOR effpos;
			this->GetFramePosFromName(&effpos, "火先");
			EffectManager::GetInstance()->Position(mFireEffIndex) = VAdd(effpos, mMoveVec);
		}
	}

	this->SetDiffuse(color);
	this->SetEmissive(color);
	this->SetAmbient(color);
	this->SetSpecular(color);

	now_scale = mScale.x + (sin(period) * amp);
	//	now_scale += ((float)mBurstTimer * 0.0002f);
	mScale = VGet(now_scale, now_scale, now_scale);

	mBurstTimer ++;

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectHand_Bomb::phaseBombStateBurstting(void)
{
	static const UCHAR sBurstEndFrame = 10;		// 爆発終了時間（フレーム）

	/**** 爆発の当たり判定を行う ****/
	ModelCollision* p_enemy = NULL;
	int attack_coll_num = CollisionManager::GetInstance()->GetAttackCollisionNum();
	ShapeCapsule capsule;

	capsule.mPosA	= mPosition;
	capsule.mPosA.y += mBodySize * 0.5f;
	capsule.mPosB	= capsule.mPosA;
	capsule.mPosB.y += 0.01f;
	capsule.mRadius= (300.0f / (float)sBurstEndFrame * (float)mBurstTimer);

#ifdef _DEBUG
	GraphicManager::GetInstance()->DbgRenderCapsule(&capsule);
#endif // _DEBUG
	
	for (int iI = 0; iI < attack_coll_num; iI ++) {
		p_enemy = (ModelCollision *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);

		if (p_enemy == this) continue;

		// 戦闘不能者と既に攻撃された者を除く
		if ((p_enemy->GetIsRetire() == false) &&
			(p_enemy->IsDamage() == false)) {

			if (p_enemy->CheckDamageCollBomb(&capsule)) {
				PRINT_CONSOLE("Hit Bomb to Burst !!!!!!!!!!!!!!!!!!!!! \n");
				
				int attack_power = 4;
				VECTOR damage_down_vec;
				damage_down_vec = VNorm(VSub(p_enemy->Position(), mPosition));
				p_enemy->ProcessDamage(attack_power, false, &mPosition, &damage_down_vec, true, eATTACK_TYPE_BOMB);
			}
		}
	}

	if (mBurstTimer > sBurstEndFrame) {
		//		ProcessDisappear();

		CollisionManager::GetInstance()->RemoveAttackCollision(this);

		CollisionManager::GetInstance()->RemoveBodyCollision(this);
		GraphicManager::GetInstance()->ReserveRemoveModel(this);
		//		GraphicManager::GetInstance()->RemoveDrawModel(this);
		ModelPmd::DeleteModel();

		mHandState = eHAND_STATE_DISAPPEAR;
	}

	mBurstTimer ++;
}


/**** end of file ****/

