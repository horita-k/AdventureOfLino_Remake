/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * オブジェクトベースクラス
 * @author SPATZ.
 * @data   2012/04/14 19:35:52
 */
//---------------------------------------------------
#include "GimmickBase.h"
#include "GimmickExtend.h"

#include "AppLib/Basic/Basic.h"
#include "DataDefine/MotionDef.h"
#include "../Charactor/CharaPlayer.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Collision/CollisionManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Effect/EffectManager.h"
//#include "AppLib/Graphic/EffekseerManager.h"

#include "Scene/ActionTaskManager.h"
#include "Utility/Utility.h"
#include "../Scene/ExternDefine.h"
#include "../Scene/CommandFunctions.h"
#include "DataDefine/VoiceDef.h"
#include "DataDefine/EffectDef.h"

#include <algorithm>
#include <iostream>
#include <map>

static const WORD kCHECK_PLAY_SOUND_DIST = 3600;

/*=====================================*
 * class GimmickExtend_Ship
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_Ship::GimmickExtend_Ship()
{
	mIsMove		= false;
	mPeriod		= 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Ship::extendSetup(void)
{
	mPeriod		= 0.0f;
	mTargetVec	= ZERO_VEC;
	mSpeed		= 0.0f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Ship::extendUpdate(void)
{
	mSpeed *= 0.999f;

	if (isOnNodeModel() == true) {
		if (CHECK_PRESS_JOY(JOY_INPUT_AWAY)) {
			CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
			mTargetVec.x = sin(p_player->Rotation().y + Utility::DegToRad(180.0f));
			mTargetVec.y = 0.0f;
			mTargetVec.z = cos(p_player->Rotation().y + Utility::DegToRad(180.0f));

			mSpeed += 0.1f;
			if (mSpeed > 16.0f) {
				mSpeed = 16.0f;
			}
		}

		VECTOR vec;
		vec.x = sin(mRotation.y + Utility::DegToRad(180.0f));
		vec.y = 0.0f;
		vec.z = cos(mRotation.y + Utility::DegToRad(180.0f));

		vec = VScale(VAdd(VScale(vec, 49.0f), VScale(mTargetVec, 1.0f)), 0.02f);

		mRotation.y = atan2(vec.x, vec.z) + PHI_F;
	} else {
		mSpeed *= 0.98f;
	}

	mPosition.x += sin(mRotation.y + Utility::DegToRad(180.0f)) * mSpeed;
	mPosition.z += cos(mRotation.y + Utility::DegToRad(180.0f)) * mSpeed;

	// 船の高さ調節
	mPeriod += 0.05f;
	ModelBase* p_model = CollisionManager::GetInstance()->GetEarthModel();
	int model_handle = p_model->GetModelHandle();
	int mesh_index = MV1GetFrameMesh(model_handle, eBG_TYPE_WATER, 0);
	float base_pos_y = MV1GetMeshMinPosition(model_handle, mesh_index).y + p_model->Position().y;
	mPosition.y = base_pos_y - (sin(mPeriod) * 22.0f) - (44.0f * p_model->Scale().x);
	
	if (mPeriod > DX_TWO_PI) {
		mPeriod -= (float)DX_TWO_PI;
	}
}

/*=====================================*
 * class GimmickExtend_Motion
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_Motion::GimmickExtend_Motion()
{
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Motion::extendSetup(void)
{
	int motin_index = 0;
	bool is_loop = false;
	float speed = 0.0f;

	motin_index = (int)mpExtendBuffer[eCSVLINE_MOTION_NO];
	is_loop = (mpExtendBuffer[eCSVLINE_MOTION_LOOP] == 0) ? false : true;
	speed = mpExtendBuffer[eCSVLINE_MOTION_SPEED];
	
	this->StartMotionForcibly(motin_index, is_loop, speed);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Motion::extendUpdate(void)
{
	return;
}

/*=====================================*
 * class GimmickExtend_Roll
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_Roll::GimmickExtend_Roll()
{
	mGimmickNodeType = eGMK_NODE_TYPE_DATA;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Roll::extendSetup(void)
{
	// 開始イベントの設定
	mWakeupEvent = (CHAR)mpExtendBuffer[eCSVLINE_WAKEUP_EVENT];

	SetMatrixType(ModelBase::eMATRIX_TYPE_MATRIX);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Roll::extendUpdate(void)
{
	//<! 開始イベントのチェック
	//<! 開始イベントのフラグが無効の場合、ギミックを動作させずに終了する
	if (mWakeupEvent != -1){
		unsigned long event_kind, event_flag;
		event_kind = ConvertPlaceIndex_TO_EventDefine(getStageIndex());
		if (event_kind != eEVENT_ERROR) {
			event_flag = mWakeupEvent;
			if (TotalInfo::GetInstance()->GetEventFlagEnable(event_kind, event_flag) == false) {
				return;
			}
		}
	}

	VECTOR roll_vec;
	roll_vec.x	= mpExtendBuffer[GimmickExtend_Roll::eCSVLINE_ROLLVEC_X];
	roll_vec.y	= mpExtendBuffer[GimmickExtend_Roll::eCSVLINE_ROLLVEC_Y];
	roll_vec.z	= mpExtendBuffer[GimmickExtend_Roll::eCSVLINE_ROLLVEC_Z];

	mRotation = VAdd(mRotation, roll_vec);

	MATRIX tmp;
	MATRIX mtx = MGetIdent();

	// 拡縮行列
	tmp = MGetScale(VGet(mScale.x, mScale.y, mScale.z));
	mtx = MMult(mtx, tmp);

	VECTOR dir_vec;
	dir_vec.x = sin(mRotation.y + PHI_F);
	dir_vec.y = 0.0f;
	dir_vec.z = cos(mRotation.y + PHI_F);

	VECTOR up_vec;
	up_vec.x = sin(mRotation.z + PHI_F) * dir_vec.z;
	up_vec.y = cos(mRotation.z + PHI_F);
	up_vec.z = sin(mRotation.z + PHI_F) * dir_vec.x;

	// 回転行列
	Utility::MTXLookAtMatrix(&tmp, (VECTOR *)&ZERO_VEC, &dir_vec, &up_vec);
	mtx = MMult(mtx, tmp);
	// 移動行列
	tmp = MGetTranslate(mPosition);
	mtx = MMult(mtx, tmp);

	this->Matrix() = mtx;



}

/*=====================================*
 * class GimmickExtend_RollDamage
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_RollDamage::GimmickExtend_RollDamage()
{
	mGimmickNodeType = eGMK_NODE_TYPE_DATA;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_RollDamage::extendSetup(void)
{
	GimmickExtend_Roll::extendSetup();

	// このギミックは当たり判定が不要のために外す
	CollisionManager::GetInstance()->RemoveEarthCollision(mModelHandle, -1);
	CollisionManager::GetInstance()->RemoveWallCollision(mModelHandle, -1);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_RollDamage::extendUpdate(void)
{
	GimmickExtend_Roll::extendUpdate();


	// 壁とプレイヤーの当たり判定
	MV1_COLL_RESULT_POLY_DIM HitResDim = {0};

	ShapeCapsule capsule;
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	if (p_player->GetIsRetire()) return;		// 戦闘不能者も除く
	if (p_player->IsDamage()) return;			// 既に攻撃された者も除く

	p_player->GetBodyCapsule(&capsule);

	// プレイヤーの直下に存在する地面のポリゴンを取得
	HitResDim = MV1CollCheck_Capsule(mModelHandle, -1, capsule.mPosA, capsule.mPosB, 
									 capsule.mRadius  );
	VECTOR down_vec;
	for (int iI = 0 ; iI < HitResDim.HitNum ; iI ++) {

		down_vec.x = sin(mRotation.y + PHI_F) * 2.0f;
		down_vec.y = 1.0f;
		down_vec.z = cos(mRotation.y + PHI_F) * 2.0f;
		down_vec = VNorm(down_vec);
		p_player->ProcessDamage(1, false, NULL, &down_vec, true, eATTACK_TYPE_BREAK_GUARD);

		break;
	}

	// 検出した地面ポリゴン情報の後始末
	if (HitResDim.HitNum > 0) {
		MV1CollResultPolyDimTerminate( HitResDim ) ;
	}
}


/*=====================================*
 * GimmickExtend_Clock
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_Clock::GimmickExtend_Clock()
{
	mGimmickNodeType = eGMK_NODE_TYPE_DATA;

	mCounter = 0.0f;

	// 表示用のモデルをロードさせる
	ResourceModel::GetInstance()->LoadGimmickResource(RES_MDL_CLOCK_VISIBLE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Clock::DeleteModel(void)
{
	GraphicManager::GetInstance()->ReserveRemoveModel(&mVisibleModel);
	CollisionManager::GetInstance()->RemoveEarthCollision(mVisibleModel.GetModelHandle(), -1);

	mVisibleModel.SetAlpha(0.0f);
	mVisibleModel.DeleteModel();

	GimmickBase::DeleteModel();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Clock::extendSetup(void)
{
	SetMatrixType(ModelBase::eMATRIX_TYPE_MATRIX);

	mCounter = 0.0f;

	if (mVisibleModel.GetModelHandle() == -1) {
		mVisibleModel.LinkModel(RES_MDL_CLOCK_VISIBLE);
		mVisibleModel.Setup();
	}
	GraphicManager::GetInstance()->EntryDrawModel(&mVisibleModel);
	CollisionManager::GetInstance()->EntryEarthCollision(mVisibleModel.GetModelHandle(), -1);
	mVisibleModel.StartMotionForcibly(0, true, 1.0f);

	getOriginalPosition(&mVisibleModel.Position());
	getOriginalRotation(&mVisibleModel.Rotation());
	getOriginalScale(&mVisibleModel.Scale());

	mUpVec = UP_VEC;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Clock::extendUpdate(void)
{
#ifdef _DEBUG
	mVisibleModel.Position() = mPosition;
	mVisibleModel.Rotation().y = mRotation.y;
	mVisibleModel.Scale() = mScale;
#endif // _DEBUG

	/**** 回転量の計算 ****/
	VECTOR roll_vec = {0};
	roll_vec.x	= mpExtendBuffer[GimmickExtend_Clock::eCSVLINE_ROLLVEC_X];
	roll_vec.y	= mpExtendBuffer[GimmickExtend_Clock::eCSVLINE_ROLLVEC_Y];
	roll_vec.z	= mpExtendBuffer[GimmickExtend_Clock::eCSVLINE_ROLLVEC_Z];

	roll_vec.x = roll_vec.x * cos(mCounter);

	VECTOR vecZ, crossX, crossZ;
	vecZ.x = sin(mRotation.y + PHI_F);
	vecZ.y = 0.0f;
	vecZ.z = cos(mRotation.y + PHI_F);

	crossX = VCross(mUpVec, vecZ);
	crossZ = VCross(mUpVec, crossX);

	mUpVec = VAdd(mUpVec, VScale(crossX, roll_vec.x));
	mUpVec = VNorm(mUpVec);

	mCounter += mpExtendBuffer[GimmickExtend_Clock::eCSVLINE_PERIOD];

	/**** 行列の計算 ****/
	{
		MATRIX mtx, tmp;
		mtx = MGetIdent();
		
		// 拡縮行列
		tmp = MGetScale(VGet(mScale.x, mScale.y, mScale.z));
		mtx = MMult(mtx, tmp);

		// 回転行列
		Utility::MTXLookAtMatrix(&tmp, &VAdd(vecZ, mPosition), &mPosition, &mUpVec);
		mtx = MMult(mtx, tmp);
		
		// 移動行列
		tmp = MGetTranslate(mPosition);
		mtx = MMult(mtx, tmp);
		
		mMatrix = mtx;
	}
}

/*=====================================*
 * class GimmickExtend_Lift
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_Lift::GimmickExtend_Lift()
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Lift::ReviseOnPosition(VECTOR& rMoveVec)
{
	GimmickBase::ReviseOnPosition(rMoveVec);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Lift::extendSetup(void)
{
	mState = 0;

	// 進行方向にリフトを向ける (※ただし、真上に移動する場合は向けない)
	VECTOR target_pos, vec;
	target_pos.x	= mpExtendBuffer[eCSVLINE_TARGET_POS_X];
	target_pos.y	= mpExtendBuffer[eCSVLINE_TARGET_POS_Y];
	target_pos.z	= mpExtendBuffer[eCSVLINE_TARGET_POS_Z];
	target_pos = VSub(target_pos, GimmickBase::sRevisePos);

	vec.x = mPosition.x - target_pos.x;
	vec.y = 0.0f;
	vec.z = mPosition.z - target_pos.z;
	if (VSize(vec) > 1.0f) {
		this->SetRotationToTarget(&target_pos);
	}

	// 開始イベントの設定
	mWakeupEvent = (CHAR)mpExtendBuffer[eCSVLINE_WAKEUP_EVENT];

	SetVisible(true);
	if (mWakeupEvent != -1){
		unsigned long event_kind, event_flag;
		event_kind = ConvertPlaceIndex_TO_EventDefine(getStageIndex());
		if (event_kind != eEVENT_ERROR) {
			event_flag = mWakeupEvent;
			if (TotalInfo::GetInstance()->GetEventFlagEnable(event_kind, event_flag) == false) {
				SetVisible(false);
			}
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Lift::extendUpdate(void)
{
	//<! 開始イベントのチェック
	//<! 開始イベントのフラグが無効の場合、ギミックを動作させずに終了する
	if (mWakeupEvent != -1){
		unsigned long event_kind, event_flag;
		event_kind = ConvertPlaceIndex_TO_EventDefine(getStageIndex());
		event_flag = mWakeupEvent;
		if ((event_kind != eEVENT_ERROR) &&
		    (TotalInfo::GetInstance()->GetEventFlagEnable(event_kind, event_flag) == false)) {
			mScale = ZERO_VEC;
			return;
		} else {
			getOriginalScale(&mScale);
			SetVisible(true);
		}
	}

	VECTOR org_pos, org_rot, org_sca, target_pos;
	float max_speed;

	getOriginalPosition(&org_pos);
	getOriginalRotation(&org_rot);
	getOriginalScale(&org_sca);

	target_pos.x	= mpExtendBuffer[GimmickExtend_Lift::eCSVLINE_TARGET_POS_X];
	target_pos.y	= mpExtendBuffer[GimmickExtend_Lift::eCSVLINE_TARGET_POS_Y];
	target_pos.z	= mpExtendBuffer[GimmickExtend_Lift::eCSVLINE_TARGET_POS_Z];
	target_pos = VSub(target_pos, GimmickBase::sRevisePos);

	max_speed		= mpExtendBuffer[GimmickExtend_Lift::eCSVLINE_MAX_SPEED];

	switch (mState) {
	case eLIFT_STATE_NONE:

		this->StartMotion(0, true, 0.5f);
		if (isOnNodeModel() == true) {
			SoundManager::GetSndComponent()->PlaySound(eSE_PRESS_KEY);
			mState = eLIFT_STATE_MOVE_GO;
		}
		break;

	case eLIFT_STATE_MOVE_GO:
		{
			VECTOR vec;
			vec = VSub(target_pos, org_pos);
			vec = VScale(VNorm(vec), max_speed);
			mPosition = VAdd(mPosition, vec);
			mCounter ++;
			if ((mCounter % 8) == 0) {
				SoundManager::GetSndComponent()->SetVolume(eSE_TROLLEY, 255 );
				SoundManager::GetSndComponent()->PlaySound(eSE_TROLLEY);
			}

			if (VSize(VSub(mPosition, target_pos)) < max_speed) {
				mPosition = target_pos;
				mCounter = 0;
				mState = eLIFT_STATE_MOVE_STOP;
			}
		}
		break;
	case eLIFT_STATE_MOVE_STOP:
		mCounter ++;
		if (mCounter > 120) {
			mCounter = 0;
			mState = eLIFT_STATE_MOVE_RETURN;
		}
		break;

	case eLIFT_STATE_MOVE_RETURN:
		{
			VECTOR vec;
			vec = VSub(org_pos, target_pos);
			vec = VScale(VNorm(vec), max_speed);
			mPosition = VAdd(mPosition, vec);
			mCounter ++;
			if ((mCounter % 8) == 0) {
				SoundManager::GetSndComponent()->SetVolume(eSE_TROLLEY, 255 );
				SoundManager::GetSndComponent()->PlaySound(eSE_TROLLEY);
			}

			if (VSize(VSub(mPosition, org_pos)) < max_speed) {
				mPosition = org_pos;
				mState = eLIFT_STATE_END;
				mCounter = 0;
			}
		}
		break;

	case eLIFT_STATE_END:
		if (isOnNodeModel() == false) {
			mState = eLIFT_STATE_NONE;
		}
		break;

	default:
		break;

	};
	//	mRotation = VAdd(mRotation, mGimmickWork.mVecRot);
}

/*=====================================*
 * class GimmickExtend_LiftAuto
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_LiftAuto::GimmickExtend_LiftAuto()
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_LiftAuto::ReviseOnPosition(VECTOR& rMoveVec)
{
	GimmickBase::ReviseOnPosition(rMoveVec);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_LiftAuto::extendSetup(void)
{
	mState = 0;

	// 進行方向にリフトを向ける (※ただし、真上に移動する場合は向けない)
	VECTOR target_pos, vec;
	target_pos.x	= mpExtendBuffer[eCSVLINE_TARGET_POS_X];
	target_pos.y	= mpExtendBuffer[eCSVLINE_TARGET_POS_Y];
	target_pos.z	= mpExtendBuffer[eCSVLINE_TARGET_POS_Z];
	target_pos = VSub(target_pos, GimmickBase::sRevisePos);

	vec.x = mPosition.x - target_pos.x;
	vec.y = 0.0f;
	vec.z = mPosition.z - target_pos.z;

	// 開始イベントの設定
	mWakeupEvent = (CHAR)mpExtendBuffer[eCSVLINE_WAKEUP_EVENT];

	SetVisible(true);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_LiftAuto::extendUpdate(void)
{
	//<! 開始イベントのチェック
	//<! 開始イベントのフラグが無効の場合、ギミックを動作させずに終了する
	if (mWakeupEvent != -1){
		unsigned long event_kind, event_flag;
		event_kind = ConvertPlaceIndex_TO_EventDefine(getStageIndex());
		event_flag = mWakeupEvent;
		if ((event_kind != eEVENT_ERROR) &&
		    (TotalInfo::GetInstance()->GetEventFlagEnable(event_kind, event_flag) == false)) {
			return;
		}
	}

	VECTOR org_pos, org_rot, org_sca, target_pos;
	float max_speed;

	getOriginalPosition(&org_pos);
	getOriginalRotation(&org_rot);
	getOriginalScale(&org_sca);

	target_pos.x	= mpExtendBuffer[GimmickExtend_LiftAuto::eCSVLINE_TARGET_POS_X];
	target_pos.y	= mpExtendBuffer[GimmickExtend_LiftAuto::eCSVLINE_TARGET_POS_Y];
	target_pos.z	= mpExtendBuffer[GimmickExtend_LiftAuto::eCSVLINE_TARGET_POS_Z];
	target_pos = VSub(target_pos, GimmickBase::sRevisePos);

	max_speed		= mpExtendBuffer[GimmickExtend_LiftAuto::eCSVLINE_MAX_SPEED];

	switch (mState) {
	case eLIFT_STATE_NONE:

		this->StartMotion(0, true, 0.5f);

		mCounter ++;
		if (mCounter > 120) {
			mCounter = 0;
			mState = eLIFT_STATE_MOVE_GO;
		}
		break;

	case eLIFT_STATE_MOVE_GO:
		{
			VECTOR vec;
			vec = VSub(target_pos, org_pos);
			vec = VScale(VNorm(vec), max_speed);
			mPosition = VAdd(mPosition, vec);
			mCounter ++;

			if ((mCounter % 8) == 0) {
				int volume = getSoundVolume(kCHECK_PLAY_SOUND_DIST / 1.2f);
				if (volume != 0) {
					SoundManager::GetSndComponent()->SetVolume(eSE_TROLLEY, volume );
					SoundManager::GetSndComponent()->PlaySound(eSE_TROLLEY);
					SoundManager::GetSndComponent()->SetVolume(eSE_TROLLEY, 255 );
				}
			}

			if (VSize(VSub(mPosition, target_pos)) < max_speed) {
				mPosition = target_pos;
				mCounter = 0;
				mState = eLIFT_STATE_MOVE_STOP;
			}
		}
		break;
	case eLIFT_STATE_MOVE_STOP:
		mCounter ++;
		if (mCounter > 120) {
			mCounter = 0;
			mState = eLIFT_STATE_MOVE_RETURN;
		}
		break;

	case eLIFT_STATE_MOVE_RETURN:
		{
			VECTOR vec;
			vec = VSub(org_pos, target_pos);
			vec = VScale(VNorm(vec), max_speed);
			mPosition = VAdd(mPosition, vec);
			mCounter ++;

			if ((mCounter % 8) == 0) {
				int volume = getSoundVolume(kCHECK_PLAY_SOUND_DIST / 1.2f);
				if (volume != 0) {
					SoundManager::GetSndComponent()->SetVolume(eSE_TROLLEY, volume );
					SoundManager::GetSndComponent()->PlaySound(eSE_TROLLEY);
				}
			}

			if (VSize(VSub(mPosition, org_pos)) < max_speed) {
				mPosition = org_pos;
				mState = eLIFT_STATE_END;
				mCounter = 0;
			}
		}
		break;

	case eLIFT_STATE_END:
		mState = eLIFT_STATE_NONE;
		break;

	default:
		break;

	};
}

/*=====================================*
 * GimmickExtend_LiftLoop
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_LiftLoop::extendSetup(void)
{
	GimmickExtend_Lift::extendSetup();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_LiftLoop::extendUpdate(void)
{
	//<! 開始イベントのチェック
	//<! 開始イベントのフラグが無効の場合、ギミックを動作させずに終了する
	if (mWakeupEvent != -1){
		unsigned long event_kind, event_flag;
		event_kind = ConvertPlaceIndex_TO_EventDefine(getStageIndex());
		event_flag = mWakeupEvent;
		if ((event_kind != eEVENT_ERROR) &&
		    (TotalInfo::GetInstance()->GetEventFlagEnable(event_kind, event_flag) == false)) {
			return;
		} else {
			SetVisible(true);
		}
	}

	VECTOR org_pos, org_rot, org_sca, target_pos;
	float max_speed;

	getOriginalPosition(&org_pos);
	getOriginalRotation(&org_rot);
	getOriginalScale(&org_sca);

	target_pos.x	= mpExtendBuffer[GimmickExtend_Lift::eCSVLINE_TARGET_POS_X];
	target_pos.y	= mpExtendBuffer[GimmickExtend_Lift::eCSVLINE_TARGET_POS_Y];
	target_pos.z	= mpExtendBuffer[GimmickExtend_Lift::eCSVLINE_TARGET_POS_Z];
	target_pos = VSub(target_pos, GimmickBase::sRevisePos);

	max_speed		= mpExtendBuffer[GimmickExtend_Lift::eCSVLINE_MAX_SPEED];

	switch (mState) {
	case eLIFT_STATE_NONE:

		this->StartMotion(0, true, 0.5f);
		mState = eLIFT_STATE_MOVE_GO;
		break;

	case eLIFT_STATE_MOVE_GO:
		{
			VECTOR vec;
			vec = VSub(target_pos, org_pos);
			vec = VScale(VNorm(vec), max_speed);
			mPosition = VAdd(mPosition, vec);
			mCounter ++;

			if (VSize(VSub(mPosition, target_pos)) < max_speed) {
				mPosition = org_pos;
				mCounter = 0;
				mState = eLIFT_STATE_NONE;
			}
		}
		break;

	default:
		break;

	};
}

/*=====================================*
 * GimmickExtend_Trolley
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_Trolley::GimmickExtend_Trolley()
{
	mTrolleySpeed = 0.0f;
	mTrolleyCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_Trolley::~GimmickExtend_Trolley()
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Trolley::ReviseOnPosition(VECTOR& rMoveVec)
{
	GimmickBase::ReviseOnPosition(rMoveVec);
}


//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void 
GimmickExtend_Trolley::extendSetup()
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Trolley::extendUpdate()
{
	VECTOR org_pos, target_pos;
	float max_speed;

	getOriginalPosition(&org_pos);

	target_pos.x	= mpExtendBuffer[GimmickExtend_Trolley::eCSVLINE_TARGET_POS_X];
	target_pos.y	= mpExtendBuffer[GimmickExtend_Trolley::eCSVLINE_TARGET_POS_Y];
	target_pos.z	= mpExtendBuffer[GimmickExtend_Trolley::eCSVLINE_TARGET_POS_Z];
	target_pos = VSub(target_pos, GimmickBase::sRevisePos);

	max_speed		= mpExtendBuffer[GimmickExtend_Trolley::eCSVLINE_MAX_SPEED];

	/* トロッコの挙動 */
	switch (mState) {
	case eTROLLEY_STATE_NONE:
		if (isOnNodeModel() == true) {
			mTrolleySpeed = 5.0f;
			mTrolleyCount = 0;
			mState = eTROLLEY_STATE_MOVE_GO;
		}
		break;

	case eTROLLEY_STATE_MOVE_GO:
		{
			VECTOR vec;
			vec = VSub(target_pos, org_pos);

			mTrolleySpeed = mTrolleySpeed * 1.005f;
			if (max_speed < mTrolleySpeed) {
				mTrolleySpeed = max_speed;
			}
			mTrolleyCount += (USHORT)mTrolleySpeed;
			if (mTrolleyCount > 450) {
				SoundManager::GetSndComponent()->SetVolume(eSE_TROLLEY, 255 );
				SoundManager::GetSndComponent()->PlaySound(eSE_TROLLEY);
				SoundManager::GetSndComponent()->SetFrequency(eSE_TROLLEY, 
															  (int)(44100 + (mTrolleySpeed * 100.0f) ) );
					
				mTrolleyCount = 0;
			}

			vec = VScale(VNorm(vec), mTrolleySpeed);
				
			mPosition = VAdd(mPosition, vec);

			if (VSize(VSub(mPosition, target_pos)) < max_speed) {
				mPosition = target_pos;
				mState = eTROLLEY_STATE_MOVE_STOP;
			}
		}
		break;
	case eTROLLEY_STATE_MOVE_STOP:
		mCounter ++;
		if (mCounter > 120) {
			mCounter = 0;
			mState = eTROLLEY_STATE_MOVE_RETURN;
		}
		break;

	case eTROLLEY_STATE_MOVE_RETURN:
		{
			VECTOR vec;
			vec = VSub(org_pos, target_pos);
			vec = VScale(VNorm(vec), max_speed);
			mPosition = VAdd(mPosition, vec);

			if (VSize(VSub(mPosition, org_pos)) < max_speed) {
				mPosition = org_pos;
				mState = eTROLLEY_STATE_END;
			}
		}
		break;

	case eTROLLEY_STATE_END:
		if (isOnNodeModel() == false) {
			mState = eTROLLEY_STATE_NONE;
		}
		break;

	default:
		break;
	};
}

/*=====================================*
 * GimmickExtend_Tower
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_Tower::GimmickExtend_Tower()
{
	for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
		mEfsTail[iI] = -1;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_Tower::~GimmickExtend_Tower()
{
	for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
		if (mEfsTail[iI] != -1) {
			EffectManager::GetInstance()->Stop(mEfsTail[iI]);
			mEfsTail[iI] = -1;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Tower::SetColor(COLOR_F& color)
{
	int mat_num = MV1GetMaterialNum(mModelHandle);
	for (int iI = 0; iI < mat_num; iI ++) {
		MV1SetMaterialDifColor(mModelHandle, iI, color);
		MV1SetMaterialSpcColor(mModelHandle, iI, color);
		MV1SetMaterialAmbColor(mModelHandle, iI, color);
		MV1SetMaterialEmiColor(mModelHandle, iI, color);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Tower::extendSetup(void)
{
	mIsActive = FALSE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Tower::extendUpdate(void)
{
	mIsActive = TRUE;

	if (mIsActive == FALSE) return;

	VECTOR org_pos;
	getOriginalPosition(&org_pos);

#ifdef _DEBUG
	if (CHECK_PRESS_KEY(eKEY_1)) {
		for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
			if (mEfsTail[iI] != -1) {
				EffectManager::GetInstance()->Stop(mEfsTail[iI]);
				mEfsTail[iI] = -1;
			}
		}
		StartMotion(0, false, 0.5f);
		mPosition = org_pos;
	}
#endif // _DEBUG
	
	if (isOnNodeModel() == true) {
		StartMotion(1, false, 0.3f);
	}

	static const BYTE sStartFrame = 90;
	static const BYTE sEndFrame = 120;

	float motion_rate		= 0.0f;
	float diff_pos = (140.0f * mScale.x);

	motion_rate = (mMotion.mNowTime - (float)sStartFrame) / (float)(sEndFrame - sStartFrame);

	if (GetMotionIndex() == 1) {

		VECTOR effpos[kEFFECT_NUM];
		GetFramePosFromName(&effpos[0], "eff0");
		GetFramePosFromName(&effpos[1], "eff1");
		GetFramePosFromName(&effpos[2], "eff2");
		GetFramePosFromName(&effpos[3], "eff3");

		if (EqualPhaseFrame(1)) {
			SoundManager::GetSndComponent()->PlaySound(eSE_ELEVATOR);
		}
		else if (EqualPhaseFrame(22)) {
			SoundManager::GetSndComponent()->PlaySound(eSE_FOOT_STEP_ROBOT);

			for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
				mEfsTail[iI] = EffectManager::GetInstance()->Play(eEF_WHITE_TAIL,
															  &effpos[iI],
																  //															  240.0f,
															  100.0f,
															  1.0f);
				EffectManager::GetInstance()->SetFloat(mEfsTail[iI], "ColorRate",			0.36f);
				EffectManager::GetInstance()->SetFloat(mEfsTail[iI], "FireRatio",			0.45f);
				EffectManager::GetInstance()->SetFloat(mEfsTail[iI], "particleSpread",		30.0f);
				EffectManager::GetInstance()->SetFloat(mEfsTail[iI], "particleSystemHeight",-23);
			}
		} 
		else if (EqualPhaseFrame(90)) {
			SoundManager::GetSndComponent()->PlaySound(eSE_ELEVATOR);
			SoundManager::GetSndComponent()->PlaySound(eSE_BURST);

			for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
				EffectManager::GetInstance()->Scale(mEfsTail[iI]) = VGet(160.0f, 160.0f, 160.0f);
			}
		}
		else if (BetweenPhaseFrame(90, 120)) {
			mPosition.y = org_pos.y + (diff_pos * motion_rate);
		}

		if (EqualPhaseFrame(120)) {
			for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
				if (mEfsTail[iI] != -1) {
					EffectManager::GetInstance()->Stop(mEfsTail[iI]);
					mEfsTail[iI] = -1;
				}
			}
		}


		for (int iI = 0; iI < kEFFECT_NUM; iI ++) {
			if (mEfsTail[iI] != -1) {
				EffectManager::GetInstance()->Position(mEfsTail[iI]) = effpos[iI];
			}
		}

	}
}

/*=====================================*
 * GimmickExtend_Hole
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_Hole::GimmickExtend_Hole()
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_Hole::~GimmickExtend_Hole()
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Hole::Update(void)
{
	GimmickBase::Update();

	// 破壊前のモデルを描画
	if (mState == eSTATE_NOT_BREAK) {
		// ひび割れモデル(マップ描画後)を描画する
		MV1DrawMesh(mModelHandle, 3);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Hole::Draw(void)
{
	ModelBase::Draw();

	// 破壊前のモデルを描画
	if (mState == eSTATE_NOT_BREAK) {
		// ひび割れモデル(マップ描画後)を描画する
		MV1DrawMesh(mModelHandle, 3);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Hole::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType/*=eATTACK_TYPE_NORMAL*/)
{
	if (attackType == eATTACK_TYPE_BOMB) {

		SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);
		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,
															&mPosition,
															64.0f,
															0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 5.0f);

		int field_flag_a, field_flag_b;
		field_flag_a = (int)mpExtendBuffer[GimmickExtend_Hole::eCSVLINE_FIELDINFO_FLAG_A];
		field_flag_b = (int)mpExtendBuffer[GimmickExtend_Hole::eCSVLINE_FIELDINFO_FLAG_B];
		if (field_flag_a != -1) {
			TotalInfo::GetInstance()->SetEnableFieldInfoFlag(field_flag_a, field_flag_b);
		}

		mState = eSTATE_BREAKING;

	}
	else {
		SoundManager::GetSndComponent()->PlaySound(eSE_SHIELD);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Hole::extendSetup(void)
{
	mState			= 0;

	this->mBodySize		= 14.0f * mScale.x;
	this->mBodyHeight	= 14.0f * mScale.x;

	this->SetDrawFunc((ModelBase::DRAW_FUNC)DrawHoleFunc);
	CollisionManager::GetInstance()->EntryAttackCollision(this);

	// このギミックは当たり判定が不要のために外す
	CollisionManager::GetInstance()->RemoveEarthCollision(mModelHandle, -1);

	MV1SetMeshVisible(mModelHandle, 2, FALSE);

	// FieldInfoFlag に書き込みがない場合、ずっと開いてる穴と認識する
	int field_flag_a = (int)mpExtendBuffer[GimmickExtend_Hole::eCSVLINE_FIELDINFO_FLAG_A];
	if (field_flag_a == -1) {
		this->BreakHole();
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Hole::extendUpdate(void)
{
	switch (mState) {
	case eSTATE_NOT_BREAK:
		break;

	case eSTATE_BREAKING:
		{
			CollisionManager::GetInstance()->RemoveAttackCollision(this);
			MV1SetMeshVisible(mModelHandle, 2, TRUE);
		}

		mState = eSTATE_BREAKED;
		break;

	case eSTATE_BREAKED:
		{
			CollisionManager::tSphereWork sphereA, sphereB;
			CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

			sphereA.mPosition = p_player->Position();
			sphereA.mPosition.y += (p_player->BodyHeight() * 0.5f);
			sphereA.mSize = p_player->BodySize();

			sphereB.mPosition = mPosition;
			sphereB.mPosition.y += (mBodySize * 0.2f);
			sphereB.mSize = mBodySize;
			VECTOR out; float dist;
			if (CollisionManager::Check_SphereToSphere(&sphereA, &sphereB, &out, &dist) ) {
				ActionTaskManager::tTaskInfo info;
				info.mTaskType = ActionTaskManager::eTASK_MOVE;
				info.mTaskValueA = (int)mpExtendBuffer[GimmickExtend_Hole::eCSVLINE_MOVE_STAGE_NO];
				info.mTaskValueB = (int)mpExtendBuffer[GimmickExtend_Hole::eCSVLINE_MOVE_PLACE_NO];
				ActionTaskManager::GetInstance()->NotifyAction(&info);
			}
		}
		break;
		
	case eSTATE_END:
		break;

	default:
		break;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void DrawHoleFunc(int model_handle)
{
	// α抜き対応の穴モデル(マップ描画前)を描画する
	MV1DrawMesh(model_handle, 0);
	MV1DrawMesh(model_handle, 1);
	MV1DrawMesh(model_handle, 2);
}

/*=====================================*
 * GimmickExtend_Cannon
 *=====================================*/
static const char* strHeadNode		= "砲塔";
static const char* strAttackTopNode = "主砲先";
static const char* strAttackBtmNode = "主砲";
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_Cannon::GimmickExtend_Cannon()
{
	mActiveJudgeCounter = 0;

	mState			= 0;
	mHitpoint		= 0;
	mActivePhase	= 0;
	mCannonTargetPos= ZERO_VEC;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_Cannon::~GimmickExtend_Cannon()
{
	GraphicManager::GetInstance()->RemoveDrawModel(&mMissileObject);

	CollisionManager::GetInstance()->RemoveAttackCollision(this);
	CollisionManager::GetInstance()->RemoveBodyCollision(this);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Cannon::PreDraw(void)
{
	// 既に破壊されている場合は即リターン
	if (mState == eSTATE_END) return;

	// eSTATE_HIDE 以外の場合は GimmickBase::PreUpdate() をコールする
	if (mState != eSTATE_HIDE) {
		GimmickBase::PreDraw();
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Cannon::PreUpdate(void)
{
	// 既に破壊されている場合は即リターン
	if (mState == eSTATE_END) return;

	// 定期的にプレイヤーとの座標を計測する
	if (mActiveJudgeCounter >= 60) {

		VECTOR cam_pos = CameraManager::GetInstance()->Position();
		float dist = VSize(VSub(cam_pos, mPosition) );

		if (mState == eSTATE_HIDE) {
			if (dist <= sDissappearLength) {
				mState = eSTATE_ACTIVE;
			}
		}
		else if (mState == eSTATE_ACTIVE) {
			if (dist >= sDissappearLength) {
				mState = eSTATE_HIDE;
			}
		}
		mActiveJudgeCounter = 0;
	}
	mActiveJudgeCounter ++;

	// eSTATE_HIDE 以外の場合は GimmickBase::PreUpdate() をコールする
	if (mState != eSTATE_HIDE) {
		GimmickBase::PreUpdate();
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Cannon::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType/*=eATTACK_TYPE_NORMAL*/)
{
	if (attackType == eATTACK_TYPE_MISSILE) return;

	mHitpoint -= attackPower;

	VECTOR* p_eff_pos;
	if (pEffPos == NULL) {
		p_eff_pos = &mPosition;
	} else {
		p_eff_pos = pEffPos;
	}

	int eff_handle = EffectManager::GetInstance()->Play(eEF_HIT, p_eff_pos, 30.0f, 0.0f);
	EffectManager::GetInstance()->Stop(eff_handle, 0.75f);

	// HPが０になった場合
	if (mHitpoint <= 0) {
		VECTOR eff_pos = mPosition;
		eff_pos.y += (mBodyHeight * 0.5f);
		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST, &eff_pos, 80.0f, 0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 7.5f);

		CameraManager::GetInstance()->StartVibeCamera(0.7f, 5.0f, 10.0f);
		SoundManager::GetSndComponent()->PlaySound(eSE_BURST);

		// このモデルにロックオンしている場合は解除させる
		CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
		if (p_player->GetLockonChara() == this) {
			p_player->DisableLockonChara();
		}
		
		SoundManager::GetVcComponent(NAME_RobotA)->PlaySound(eVC_DOWN);

		mState = eSTATE_BREAKING;
	}
	else {
		SoundManager::GetVcComponent(NAME_RobotA)->PlaySound(eVC_DAMAGE);
	}

	mIsDamage = true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Cannon::extendSetup(void)
{
	// 既に破壊されている場合は即リターン
	// モデルハンドルは破棄されている場合は破壊されたと判定し、リターン
	if (mModelHandle == -1) {
		mState = eSTATE_END;	// ステータスを終了に設定させる
		return;
	}

	// ボディサイズの取得
	setupBodyHeightSize();
	CollisionManager::GetInstance()->EntryBodyCollision(this);
	CollisionManager::GetInstance()->EntryAttackCollision(this);

	// このギミックは当たり判定が不要のために外す
	CollisionManager::GetInstance()->RemoveEarthCollision(mModelHandle, -1);
	CollisionManager::GetInstance()->RemoveWallCollision(mModelHandle, -1);
	
	mState			= eSTATE_HIDE;
	mHitpoint		= 4;
	mActivePhase	= eACTIVE_PHASE_NONE;
	mCannonTargetPos= ZERO_VEC;

	mMissileObject.Setup(eOBJECT_KIND_MISSILE, (eItemKind)0, (VECTOR *)&ZERO_VEC, 0, this, true);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Cannon::extendUpdate(void)
{
	switch (mState) {
	case eSTATE_HIDE:
		break;

	case eSTATE_ACTIVE:
		funcActivePhase();
		break;

	case eSTATE_BREAKING:
		{
			float alpha = GetAlpha();
			alpha -= 0.1f;
			if (alpha > 0.0f) {
				SetAlpha(alpha);
			} else {
				SetAlpha(0.0f);
				mState = eSTATE_BREAKED;
			}
		}
		break;

	case eSTATE_BREAKED:
		CollisionManager::GetInstance()->RemoveAttackCollision(this);
		CollisionManager::GetInstance()->RemoveBodyCollision(this);
		GraphicManager::GetInstance()->ReserveRemoveModel(this);
		this->SetAlpha(0.0f);
		ModelPmd::DeleteModel();

		mState = eSTATE_END;
		break;

	case eSTATE_END:
		break;

	default:
		break;

	};
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Cannon::funcActivePhase(void)
{
	VECTOR eff_pos;
	EFK_HANDLE	efk_hdl;

	switch (mActivePhase) {
	case eACTIVE_PHASE_NONE:
		{
			CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
			float dist = VSize(VSub(p_player->Position(), mPosition) );
			if (dist < 4000.0f) {

				VECTOR hitpos, normal, node_pos, target_pos;
				this->GetFramePosFromName(&node_pos, strAttackTopNode);

				target_pos = p_player->Position();
				target_pos.y += p_player->BodyHeight();

				// プレイヤーと大砲間にコリジョンが無い場合に起動する
				if ((CollisionManager::GetInstance()->CheckEarthCollisionExcept(mModelHandle, target_pos, node_pos, &hitpos, &normal) == NOT_ON_COLLISION) &&
					(!(CollisionManager::GetInstance()->CheckWallCollisionExcept(mModelHandle, target_pos, node_pos, &hitpos, &normal))) ) {
					mCannonTargetPos = VAdd(p_player->Position(), p_player->MoveVec() );
					mCannonTargetPos.y += p_player->BodyHeight();
					mActivePhase = eACTIVE_PHASE_AIM;
					mCounter = 0;
				}
			}
		}
		break;

	case eACTIVE_PHASE_AIM:
		{
			// 大砲を向ける
			updateRotateCannon();
			if (mCounter == 40) {
				this->GetFramePosFromName(&eff_pos, strAttackTopNode);
				efk_hdl = EffekseerManager::GetInstance()->Play(eEFK_EXPLOSION_BOMB, &eff_pos, 5.0f, 0.0f);
				EffekseerManager::GetInstance()->SetSpeed(efk_hdl, 0.75f);
			}

			if (mCounter > 45) {
				SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
			
				VECTOR weapon_top_pos, weapon_btm_pos;
				VECTOR move_vec;
				const float missile_speed = 54;
				this->GetFramePosFromName(&weapon_top_pos, strAttackTopNode);
				this->GetFramePosFromName(&weapon_btm_pos, strAttackBtmNode);
				move_vec = VScale(VNorm(VSub(weapon_top_pos, weapon_btm_pos) ), missile_speed);
			
				eff_pos = VAdd(weapon_top_pos, VScale(VNorm(VSub(weapon_btm_pos, weapon_top_pos) ), 30.0f) );
				mMissileObject.ProcessShoot(eff_pos, move_vec);

				mActivePhase = eACTIVE_PHASE_SHOOT;
				mCounter = 0;
			}
		}
		break;

	case eACTIVE_PHASE_SHOOT:
		if (mCounter > 90) {
			mActivePhase = eACTIVE_PHASE_NONE;
			mCounter = 0;
		}
		break;

	default:
		break;
	};

	mCounter ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Cannon::updateRotateCannon(void)
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

/*=====================================*
 * GimmickExtend_LeanFloor
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_LeanFloor::GimmickExtend_LeanFloor()
{
	mLeanVecZ  = ZERO_VEC;
	mLeanVecUp = ZERO_VEC;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_LeanFloor::~GimmickExtend_LeanFloor()
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_LeanFloor::extendSetup(void)
{
	SetMatrixType(ModelBase::eMATRIX_TYPE_MATRIX);

	mLeanVecZ.x = sin(mRotation.y + PHI_F);
	mLeanVecZ.y = 0.0f;
	mLeanVecZ.z = cos(mRotation.y + PHI_F);
	mLeanVecUp = UP_VEC;
	mLeanValueY = 0.0f;
	mGimmickNodeType = eGMK_NODE_TYPE_DATA;

#ifdef _DEBUG
	// デバッグツールで、Y軸の角度が変更された場合に即時に反映させる為の処理
	VECTOR tmp;
	getOriginalRotation(&tmp);
	sDebugToolOrgRotationY = tmp.y;
#endif // _DEBUG
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_LeanFloor::extendUpdate(void)
{
	VECTOR org_lean_z;
	VECTOR org_lean_up;
	bool is_reverse = false;

	org_lean_z.x = sin(mRotation.y + PHI_F);
	org_lean_z.y = 0.0f;
	org_lean_z.z = cos(mRotation.y + PHI_F);
	org_lean_up = UP_VEC;
#ifdef _DEBUG
	// デバッグツールで、Y軸の角度が変更された場合に即時に反映させる為の処理
	if ((int)Utility::RadToDeg(mRotation.y) != (int)Utility::RadToDeg(sDebugToolOrgRotationY) ) {
		mLeanVecZ = org_lean_z;
		mLeanVecUp= org_lean_up;

		mLeanValueY = 0.0f;
		sDebugToolOrgRotationY = mRotation.y;
	}
#endif // _DEBUG

	MATRIX mtx, tmp;
	mtx = MGetIdent();

	// 拡縮行列
	tmp = MGetScale(VGet(mScale.x, mScale.y, mScale.z));
	mtx = MMult(mtx, tmp);
	
	// 回転行列
	mLeanValueY  *= 0.96f;
	if (isOnNodeModel() == true) {
#ifdef _DEBUG
		PRINT_SCREEN(GetColor(255, 255, 0), "on lean floar \n");
#endif _DEBUG

		CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
		VECTOR vec;
		float len;
		vec = VSub(p_player->Position(), mPosition);
		len = VSize(vec);
		if (len > 0.0f) {
			len = sqrt(len);
		}
		vec.y = 0.0f;
		float lean_rate = mpExtendBuffer[GimmickExtend_LeanFloor::eCSVLINE_LEAN_RATE];
		vec = VScale(VNorm(vec), lean_rate * len);
		
		mLeanValueY = (cos(mRotation.y) * vec.z) + (sin(mRotation.y) * vec.x);
	}
	else {
		if (abs(mLeanValueY) < 0.0001f) {
			mLeanValueY = 0.00f;
			is_reverse = true;
		}	
	}

	// 傾きを元に戻す場合の処理
	if (is_reverse) {
	//	if (false) {
		mLeanVecZ = VNorm(VAdd(mLeanVecZ,	VScale(org_lean_z,  0.02f) ) );
	} else {
		VECTOR next_vec;
		next_vec = VAdd(mLeanVecZ, VScale(mLeanVecUp, mLeanValueY) );
		if (abs(next_vec.y) < 0.9f) {
			mLeanVecZ = next_vec;
		}
	}
	mLeanVecZ = VNorm(mLeanVecZ);
		
	VECTOR crossX = VCross(mLeanVecZ, mLeanVecUp);
	mLeanVecUp = VCross(crossX, mLeanVecZ);
		
	VECTOR vecZ = VAdd(mLeanVecZ, mPosition);
	Utility::MTXLookAtMatrix(&tmp, &vecZ, &mPosition, &mLeanVecUp);

	mtx = MMult(mtx, tmp);
	
	// 移動行列
	tmp = MGetTranslate(mPosition);
	mtx = MMult(mtx, tmp);

	mMatrix = mtx;
}

/*=====================================*
 * Class GimmickExtend_Switch
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_Switch::GimmickExtend_Switch()
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_Switch::~GimmickExtend_Switch()
{
	CollisionManager::GetInstance()->RemoveAttackCollision(this);
	CollisionManager::GetInstance()->RemoveBodyCollision(this);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Switch::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType/*=eATTACK_TYPE_NORMAL*/)
{
	SoundManager::GetSndComponent()->PlaySound(eSE_BURST);

	ActionTaskManager::tTaskInfo param;
	param.mTaskType = ActionTaskManager::eTASK_EVENT;
	param.mTaskValueA = ConvertPlaceIndex_TO_EventDefine(getStageIndex());
	if (param.mTaskValueA != eEVENT_ERROR) {
		param.mTaskValueB = (int)mpExtendBuffer[GimmickExtend_Switch::eCSVLINE_TASK_VALUE];
		ActionTaskManager::GetInstance()->NotifyAction(&param);
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Switch::extendSetup(void)
{
	// ボディサイズの取得
	setupBodyHeightSize();
	CollisionManager::GetInstance()->EntryBodyCollision(this);
	CollisionManager::GetInstance()->EntryAttackCollision(this);

	// このギミックは当たり判定が不要のために外す
	CollisionManager::GetInstance()->RemoveEarthCollision(mModelHandle, -1);

	SetVisible(false);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Switch::extendUpdate(void)
{

}


/*=====================================*
 * GimmickExtend_FootSwitch
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_FootSwitch::GimmickExtend_FootSwitch()
{
	mOffNodeCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_FootSwitch::~GimmickExtend_FootSwitch()
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_FootSwitch::extendSetup(void)
{
	mGimmickNodeType = eGMK_NODE_TYPE_DATA;
	mOffNodeCount = 0;

	mCounter = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_FootSwitch::extendUpdate(void)
{
	VECTOR org_pos;
	float dist = 0.0f;

	getOriginalPosition(&org_pos);

	switch (mState) {
	case eSTATE_NONE:
		if (isOnNodeModel()) {
			SoundManager::GetSndComponent()->PlaySound(eSE_PRESS_KEY);
			mCounter ++;
			//			StartMotion(1, false, 0.5f);
			mState = eSTATE_DOWN_SWITCH;
		}
		break;

	case eSTATE_DOWN_SWITCH:
		mCounter ++;
		break;

	case eSTATE_ON_MODEL_SWITCH:
		if (isOnNodeModel() == false) {
			mOffNodeCount ++;
		}
		else {
			mOffNodeCount = 0;
		}

		if (mOffNodeCount > 60) {
			mOffNodeCount = 0;
			mState = eSTATE_UP_SWITCH;
		}
		break;

	case eSTATE_UP_SWITCH:
		if ((mCounter % 4) == 0) {
			SoundManager::GetSndComponent()->SetVolume(eSE_TROLLEY, 255 );
			SoundManager::GetSndComponent()->PlaySound(eSE_TROLLEY);
		}
		mCounter --;
		break;

	default:
		break;
	};
	
	if (mCounter > 14) {
		mCounter = 14;
		mState = eSTATE_ON_MODEL_SWITCH;

		// イベント開始
		ActionTaskManager::tTaskInfo param;
		param.mTaskType = ActionTaskManager::eTASK_EVENT;
		param.mTaskValueA = ConvertPlaceIndex_TO_EventDefine(getStageIndex());
		if (param.mTaskValueA != eEVENT_ERROR) {
			param.mTaskValueB = (int)mpExtendBuffer[GimmickExtend_Switch::eCSVLINE_TASK_VALUE];
			ActionTaskManager::GetInstance()->NotifyAction(&param);
		}


	} else if (mCounter <= 0) {
		mCounter = 0;
		mState = eSTATE_NONE;
	}
	dist = (float)mCounter * -0.1f;

	MV1SetFrameUserLocalMatrix(mModelHandle, 1,
							   MGetTranslate(VGet(0.0f, dist, 0.0f)));
	

}

/*=====================================*
 * class GimmickExtend_Teto
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_Teto::GimmickExtend_Teto()
{
	mGimmickNodeType = eGMK_NODE_TYPE_DATA;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Teto::ReviseOnPosition(VECTOR& rMoveVec)
{
	GimmickBase::ReviseOnPosition(rMoveVec);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Teto::extendSetup(void)
{
	mPeriod		= 0.0f;
	mTargetVec	= ZERO_VEC;
	mSpeed		= 0.0f;

	StartMotion(0, true, 0.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Teto::extendUpdate(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	
	mSpeed *= 0.999f;

	if (isOnNodeModel() == true) {
		if (CHECK_PRESS_JOY(JOY_INPUT_AWAY)) {

			StartMotion(2, true, 0.5f);

			mTargetVec.x = sin(p_player->Rotation().y + Utility::DegToRad(180.0f));
			mTargetVec.y = 0.0f;
			mTargetVec.z = cos(p_player->Rotation().y + Utility::DegToRad(180.0f));

			mSpeed += 0.1f;

			static const float kMAX_SPEED = 8.0f;
			if (mSpeed > kMAX_SPEED) {
				mSpeed = kMAX_SPEED;
			}
		}

		VECTOR vec;
		vec.x = sin(mRotation.y + Utility::DegToRad(180.0f));
		vec.y = 0.0f;
		vec.z = cos(mRotation.y + Utility::DegToRad(180.0f));

		vec = VScale(VAdd(VScale(vec, 49.0f), VScale(mTargetVec, 1.0f)), 0.02f);
		
		mRotation.y = atan2(vec.x, vec.z) + PHI_F;
	}
	else {
		mSpeed *= 0.98f;
	}

	VECTOR add_vec;

	add_vec.x = sin(mRotation.y + Utility::DegToRad(180.0f)) * mSpeed;
	add_vec.y = 0.0f;
	add_vec.z = cos(mRotation.y + Utility::DegToRad(180.0f)) * mSpeed;

	mPosition = VAdd(mPosition, add_vec);

	p_player->Position() = VAdd(p_player->Position(), add_vec);

	if (mPeriod > DX_TWO_PI) {
		mPeriod -= (float)DX_TWO_PI;
	}

}

/*=====================================*
 * GimmickExtend_TrolleyEx
 *=====================================*/
GimmickExtend_TrolleyEx::PHASE_STATE
GimmickExtend_TrolleyEx::mPhaseState[GimmickExtend_TrolleyEx::eTROLLEY_STATE_MAX] = {
	&GimmickExtend_TrolleyEx::phaseStateNone,
	&GimmickExtend_TrolleyEx::phaseStateMoveStart,
	&GimmickExtend_TrolleyEx::phaseStateMoveGo,
	&GimmickExtend_TrolleyEx::phaseStateEnd,
};
/*static*/int GimmickExtend_TrolleyEx::sRouteModelHandle= -1;
/*static*/int GimmickExtend_TrolleyEx::sRouteFrameIndex	= -1;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
GimmickExtend_TrolleyEx::SetupTrolleyExCollision(int modelHandle, int frameIndex)
{
	sRouteModelHandle	= modelHandle;
	sRouteFrameIndex	= frameIndex;

	MV1SetupCollInfo(sRouteModelHandle, sRouteFrameIndex, 8, 8, 8);

	// 参照用メッシュ作成
	MV1SetupReferenceMesh(sRouteModelHandle, sRouteFrameIndex, TRUE);

	// スポットライトの設定
	GraphicManager::GetInstance()->SetPLightDiffuse(COL_BYTE_2_FLOAT(255), 
													COL_BYTE_2_FLOAT(128),
													COL_BYTE_2_FLOAT(20), 0.0f, eSLIGHT_NO_00);
	GraphicManager::GetInstance()->SetPLightSpecular(COL_BYTE_2_FLOAT(21), 
													 COL_BYTE_2_FLOAT(10),
													 COL_BYTE_2_FLOAT(5), 0.0f, eSLIGHT_NO_00);
	GraphicManager::GetInstance()->SetPLightAmbient(COL_BYTE_2_FLOAT(255), 
													COL_BYTE_2_FLOAT(128),
													COL_BYTE_2_FLOAT(0), 0.0f, eSLIGHT_NO_00);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
GimmickExtend_TrolleyEx::UpdateTrolleyExCollision(void)
{
	MV1RefreshReferenceMesh(sRouteModelHandle, sRouteFrameIndex, TRUE);

	MV1RefreshCollInfo(sRouteModelHandle, sRouteFrameIndex);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
GimmickExtend_TrolleyEx::DestroyTrolleyExCollision(void)
{
	CollisionManager::GetInstance()->RemoveEarthCollision(sRouteModelHandle, sRouteFrameIndex);

	MV1TerminateCollInfo(sRouteModelHandle, sRouteFrameIndex);
	
	// 参照用メッシュ破棄
	MV1TerminateReferenceMesh(sRouteModelHandle, sRouteFrameIndex, TRUE);

	sRouteModelHandle	= -1;
	sRouteFrameIndex	= -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_TrolleyEx::GimmickExtend_TrolleyEx()
{
	mTrolleySpeed	= 0.0f;
	mTrolleyCount	= 0;

	mMoveVec		= ZERO_VEC;
	mGravity		= 0.0f;
	mNormalVec		= UP_VEC;

	mPrevRotY		= 0.0f;
	mPrevNormalVec	= ZERO_VEC;

	mIsOnRoute		= FALSE;
	mIsOnPlayer		= FALSE;
	mNotOnPlayerCount = 0;

	mJumpMoveVec	= ZERO_VEC;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_TrolleyEx::~GimmickExtend_TrolleyEx()
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_TrolleyEx::Update(void)
{
	GimmickBase::Update();

	if (mIsOnPlayer == TRUE) {
		CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

		VECTOR frame_pos;
		frame_pos = VAdd(mPosition, VScale(mNormalVec, 50.0f) );

		p_player->Position() = frame_pos;

		p_player->UpdateMatrixFromRotY_Normal(p_player->Rotation().y, mNormalVec);

		p_player->MoveVec().x = 0.0f;
		p_player->MoveVec().z = 0.0f;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_TrolleyEx::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec,
									   bool isDamageDown, eAttackType attackType/*=eATTACK_TYPE_NORMAL*/)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	p_player->SetActionLimit(eACTION_OK, p_player->ItemWork().mItemSubWeapon);
	
	SoundManager::GetSndComponent()->PlaySound(eSE_PRESS_KEY);

	mState = eTROLLEY_STATE_MOVE_START;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void 
GimmickExtend_TrolleyEx::extendSetup()
{
	mState			= 0;

	mTrolleySpeed	= 0.0f;
	mTrolleyCount	= 0;

	mMoveVec		= ZERO_VEC;
	mGravity		= 0.0f;
	mNormalVec		= UP_VEC;
	mRealNormalVec	= ZERO_VEC;

	mPrevRotY		= mRotation.y;
	mPrevNormalVec	= mNormalVec;

	mIsOnRoute		= FALSE;
	mIsOnPlayer		= FALSE;
	mNotOnPlayerCount = 0;

	// モデルの大きさから自動的に BodySize と BodyHeight を割り当てる
	setupBodyHeightSize();
	mBodySize	*= 1.2f;
	mBodyHeight *= 1.2f;

	CollisionManager::GetInstance()->EntryAttackCollision(this);

	ModelBase::SetMatrixType(eMATRIX_TYPE_MATRIX);
	mMoveVec = VGet(0.0f, 0.0f, 1.0f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_TrolleyEx::extendUpdate()
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	mGravity = kGRAVITY_SPEED;

	if ((mIsOnRoute == TRUE) && (mIsOnPlayer == TRUE)) {

		// ジャンプ
		if (CHECK_TRIG_JOY(JOY_INPUT_JUMP)) {

			SoundManager::GetSndComponent()->PlaySound(eSE_COMICAL);

			mTrolleySpeed *= 0.9f;

			float jump_rate_v = mpExtendBuffer[GimmickExtend_TrolleyEx::eCSVLINE_JUMP_RATE_V];
			float jump_rate_h = mpExtendBuffer[GimmickExtend_TrolleyEx::eCSVLINE_JUMP_RATE_H];
			float max_speed = mpExtendBuffer[GimmickExtend_TrolleyEx::eCSVLINE_MAX_SPEED];
			float speed_rate = mTrolleySpeed / max_speed;
			
			mGravity += kJUMP_GRAVITY * jump_rate_v * sqrt(speed_rate);

			float dot;
			dot = VDot(VNorm(mMoveVec), mRealNormalVec);

			// 垂直に走っている場合ははっきりジャンプする様にする
			if (mRealNormalVec.y > 0.75f) {
				PRINT_CONSOLE("vertical jump !!!!!!\n");
				mMoveVec = VScale(VNorm(mMoveVec), mTrolleySpeed);
			}
			else if (abs(dot) < 0.5f) {
				PRINT_CONSOLE("jump dot : %.2f \n", dot);

				mMoveVec = VScale(VNorm(mMoveVec), mTrolleySpeed);
				
				mJumpMoveVec = mRealNormalVec;
				mJumpMoveVec.y = 0.0f;
				mJumpMoveVec = VScale(mJumpMoveVec, (jump_rate_h * 1.0f ) );
			}				

			PRINT_CONSOLE("mMoveVec   = %f, %f, %f : %f \n", mMoveVec.x, mMoveVec.y, mMoveVec.z, VSize(mMoveVec));
			PRINT_CONSOLE("mJumpMoveVec = %f, %f, %f : %f \n", mJumpMoveVec.x, mJumpMoveVec.y, mJumpMoveVec.z, VSize(mJumpMoveVec));
		}
	}
	
	mMoveVec.y += mGravity;

	// ステート毎に応じた処理
	(this->*mPhaseState[mState])();

	// マトリックスの更新
	float diff = mRotation.y - mPrevRotY;
	if (diff > PHI_F) {
		mPrevRotY += (PHI_F * 2);
	}
	else if (diff < -PHI_F) {
		mPrevRotY -= (PHI_F * 2);
	}

	mRotation.y = ((mRotation.y - mPrevRotY) * 0.2f) + mPrevRotY;
	Utility::CorrectAngleFrom000To360(&mRotation.y);

	mNormalVec = VScale(VAdd(mNormalVec,
							 VScale(mPrevNormalVec, 9.0f)), 1.0f/10.0f);

	// Y軸回転と法線指定でマトリックスを更新
	ModelBase::UpdateMatrixFromRotY_Normal(mRotation.y, mNormalVec);

	// プレイヤーの回転も補正させる
	if (mIsOnPlayer == TRUE) {
		p_player->Rotation().y += (mRotation.y - mPrevRotY);
		Utility::CorrectAngleFrom000To360(&p_player->Rotation().y);
	}

	/* １フレ前の情報を更新 */
	mPrevRotY		= mRotation.y;
	mPrevNormalVec	= mNormalVec;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_TrolleyEx::phaseStateNone(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (isOnNodeModel() == true) {
		p_player->SetMatrixType(CharaBase::eMATRIX_TYPE_MATRIX);
		mIsOnPlayer = TRUE;
	}

	if (mIsOnPlayer == TRUE) {
		if (p_player->GetState() == CharaBase::eCHARA_STATE_JUMP_UP) {
			p_player->SetMatrixType(CharaBase::eMATRIX_TYPE_TRS);

			GraphicManager::GetInstance()->SetEnablePLight(false, eSLIGHT_NO_00);

			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
			mIsOnPlayer = FALSE;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_TrolleyEx::phaseStateMoveStart(void)
{
	// トロッコ走行条件判定
	mTrolleySpeed = mpExtendBuffer[GimmickExtend_TrolleyEx::eCSVLINE_START_SPEED];
	mTrolleyCount = 0;

	mState = eTROLLEY_STATE_MOVE_GO;

	CollisionManager::GetInstance()->RemoveAttackCollision(this);

	if (mIsOnPlayer == TRUE) {
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_TROLLEY);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_TrolleyEx::phaseStateMoveGo(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (isOnNodeModel() == true) {
		p_player->SetMatrixType(CharaBase::eMATRIX_TYPE_MATRIX);
		mIsOnPlayer = TRUE;
	}

	if (mIsOnPlayer == TRUE) {
		if ((CameraManager::GetInstance()->GetCameraState() != eCAM_STATE_TROLLEY) &&
			(CameraManager::GetInstance()->GetCameraState() != eCAM_STATE_LOOK_AROUND) ) {
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_TROLLEY);
		}

		mNotOnPlayerCount = 0;
	}
	else {
		mNotOnPlayerCount ++;
		if (mNotOnPlayerCount > 900) {
			mState = eTROLLEY_STATE_END;
		}
	}

	// レールとの当たり判定
	updateRouteCollision();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_TrolleyEx::phaseStateEnd(void)
{
	if (isOnNodeModel() == false) {

		if (mTrolleyCount == 0) {
			CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
			p_player->SetActionLimit(eACTION_OK, p_player->ItemWork().mItemSubWeapon);
		}

		mTrolleyCount ++;

		if (mTrolleyCount > 240) {
			int eff_handle;
			eff_handle = EffectManager::GetInstance()->Play(eEF_DUST, &mPosition, 75.0f, 0.0f);
			EffectManager::GetInstance()->Stop(eff_handle, 5.0f);

			this->ReloadInformation(sStageIndex);

			eff_handle = EffectManager::GetInstance()->Play(eEF_DUST, &mPosition, 75.0f, 0.0f);
			EffectManager::GetInstance()->Stop(eff_handle, 5.0f);

			mTrolleyCount = 0;
		}
	}
	else {
		mMoveVec.y += mGravity;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_TrolleyEx::updateRouteCollision(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	BOOL is_break = FALSE;
	VECTOR hitpos, normal;
	static float sCapsuleHeight = 1.0f;
	VECTOR vec_start, vec_end;

	vec_start	= VAdd(mPosition, VGet(0.0f, ((mBodySize * 0.8f) + sCapsuleHeight), 0.0f));

	if (mIsOnRoute) {
		
		// レールに乗っている場合
		vec_end		= mPosition;
		vec_end.y	+= mGravity;
	}
	else {
		// 宙に舞っている場合
		vec_end		= VAdd(mPosition, mMoveVec);
	}

	MV1_COLL_RESULT_POLY result_work;
	result_work = MV1CollCheck_Line(sRouteModelHandle, sRouteFrameIndex, vec_start, vec_end);

	// レールに当たっている場合
	if (result_work.HitFlag == 1) {

		if (mIsOnRoute == FALSE) {
			VECTOR effpos = VAdd(mPosition, mMoveVec);
			int eff_handle = EffectManager::GetInstance()->Play(eEF_HIT, &effpos, 40.0f, 0.0f);
			EffectManager::GetInstance()->Stop(eff_handle, 2.0f);
			SoundManager::GetSndComponent()->PlaySound(eSE_BRAKE);
		}
		mJumpMoveVec = ZERO_VEC;

		mIsOnRoute = TRUE;
		
		// トロッコの音・関連
		mTrolleyCount += (USHORT)mTrolleySpeed;
		if (mTrolleyCount > 250) {
			SoundManager::GetSndComponent()->SetVolume(eSE_TROLLEY, 255 );
			SoundManager::GetSndComponent()->PlaySound(eSE_TROLLEY);
			SoundManager::GetSndComponent()->SetFrequency(eSE_TROLLEY, 
														  (int)(44100 + (mTrolleySpeed * 50.0f) ) );
			mTrolleyCount = 0;
		}

		// 速度に応じた移動ベクトルの作成
		float max_speed = mpExtendBuffer[GimmickExtend_TrolleyEx::eCSVLINE_MAX_SPEED];
		float min_speed = mpExtendBuffer[GimmickExtend_TrolleyEx::eCSVLINE_START_SPEED];
		float accel_rate = mpExtendBuffer[GimmickExtend_TrolleyEx::eCSVLINE_ACCEL_RATE];

		// トロッコの向きのベクトルを作る
		PRINT_SCREEN(COLOR_RED, "mRotation : %.2f \n", mRotation.y);
		VECTOR rotation_vec;
		rotation_vec.x = sin(mRotation.y + PHI_F);
		rotation_vec.y = 0.0f;
		rotation_vec.z = cos(mRotation.y + PHI_F);
		rotation_vec = VNorm(rotation_vec);

		/**** ここから！ ****/
		VECTOR start_pos, end_pos;
		VECTOR route_vec;
		VECTOR next_pos;
		float  remain_speed;
		float  end_dist;

		// ポリゴン番号の中心座標を取得する
		remain_speed = mTrolleySpeed;

		getPolyCenterPos(result_work.PolygonIndex, &rotation_vec, &start_pos, &end_pos);
		route_vec = VNorm(VSub(end_pos, start_pos) );

		end_dist = VSize( VSub(end_pos, mPosition) );
		//		next_pos = VAdd(mPosition, VScale(route_vec, remain_speed) );
		if (end_dist >= remain_speed) {
			next_pos = VAdd(end_pos, VScale(route_vec, (remain_speed - end_dist)));

			//			mPosition = next_pos;
			mMoveVec = VSub(next_pos, mPosition);

			mNormalVec = result_work.Normal;
			mRealNormalVec = mNormalVec;
		}
		else {
			// ポリゴン間をまたぐ場合
			mPosition = end_pos;
			remain_speed -= end_dist;

			MV1_COLL_RESULT_POLY_DIM	HitResDim = {0};
			MV1_COLL_RESULT_POLY*		pHitRes = NULL;
			float	dist;
			float	nearest_dist	= 9999.9f;
			int		nearest_index	= -1;
			int		check_pindex	= -1;	
			HitResDim = MV1CollCheck_Capsule(sRouteModelHandle, sRouteFrameIndex,
											 end_pos, end_pos, max_speed*1.2f);
			
			for (int iI = 0; iI < HitResDim.HitNum; iI ++) {
				pHitRes = &HitResDim.Dim[iI];
				check_pindex = pHitRes->PolygonIndex;
				if ( (result_work.PolygonIndex / 2) == (check_pindex / 2) ) {
					continue;
				}

				for (int pindex = 0; pindex < 3; pindex ++) {
					dist = VSize(VSub(end_pos, pHitRes->Position[pindex]) );
					if (nearest_dist > dist) {
						nearest_dist = dist;
						nearest_index= check_pindex;
						mNormalVec	= pHitRes->Normal;
						mRealNormalVec = mNormalVec;
					}
				}
			}
		

			if (nearest_index != -1) {
				VECTOR tmp_vec;
				tmp_vec = VNorm(VGet(route_vec.x, 0.0f, route_vec.z) );
				getPolyCenterPos(nearest_index, &tmp_vec, &start_pos, &end_pos);
				route_vec = VNorm(VSub(end_pos, start_pos) );
				end_dist = VSize( VSub(end_pos, mPosition) );

				APP_ASSERT((VSize(end_pos) > 0.01f), "Invalid end_dist \n");

				if (end_dist > remain_speed) {
					//					next_pos = VAdd(mPosition, VScale(route_vec, remain_speed) );
					next_pos = VAdd(end_pos, VScale(route_vec, (remain_speed - end_dist)));
					//					mPosition = next_pos;
					mMoveVec = VSub(next_pos, mPosition);
				}
				else {
					PRINT_ASSERT(NULL, "invalid !!!!!!!!!\n ");
				}
			}
			else {
				// 飛び出る処理
				next_pos = VAdd(mPosition, VScale(route_vec, remain_speed) );
				mPosition = next_pos;
			}
		}

		mTrolleySpeed = mTrolleySpeed *  accel_rate * (1.0f + ( route_vec.y * -0.03f));
		if (max_speed < mTrolleySpeed) {
			mTrolleySpeed = max_speed;
		}
		else if (min_speed > mTrolleySpeed) {
			mTrolleySpeed = min_speed;
		}

		// トロッコの向きを壁ずりに合わせる
		mRotation.y = atan2(route_vec.x, route_vec.z) + PHI_F;
		Utility::CorrectAngleFrom000To360(&mRotation.y);

		if (mIsOnPlayer) {
			CameraManager::tPatternTrolleyParam param;
			param.mVector	= VNorm(mMoveVec);
			param.mUp		= mNormalVec;
			param.mSpeedRate= mTrolleySpeed / max_speed;

			param.mCamLength	= mpExtendBuffer[GimmickExtend_TrolleyEx::eCSVLINE_CAM_LENGTH];
			param.mCamWidth		= mpExtendBuffer[GimmickExtend_TrolleyEx::eCSVLINE_CAM_WIDTH];
			param.mCamHeight	= mpExtendBuffer[GimmickExtend_TrolleyEx::eCSVLINE_CAM_HEIGHT];

			CameraManager::GetInstance()->SetTrolleyParam(&param);
		}

		mPosition = VAdd(mPosition, mMoveVec);

		mMoveVec.y	= 0.0f;
		mGravity	= 0.0f;

	}
	// レールに当たっていない場合
	else {

		mIsOnRoute = FALSE;

		if ((CollisionManager::GetInstance()->CheckEarthCollisionExcept(mModelHandle, vec_start, vec_end, &hitpos, &normal) != NOT_ON_COLLISION) ||
			(CollisionManager::GetInstance()->CheckWallCollisionExcept(mModelHandle, vec_start, vec_end, &hitpos, &normal))) {
			is_break = TRUE;
		}
		else {
			mPosition = VAdd(mPosition, VAdd(mMoveVec, mJumpMoveVec));

			VECTOR vecZ, crossX, crossY;
			vecZ = VNorm(mMoveVec);
			crossX = VCross(vecZ, UP_VEC);
			crossY = VCross(crossX, vecZ);
			mNormalVec = crossY;
			mRealNormalVec = mNormalVec;

			if (mIsOnPlayer) {
				float max_speed = mpExtendBuffer[GimmickExtend_TrolleyEx::eCSVLINE_MAX_SPEED];

				CameraManager::tPatternTrolleyParam param;
				param.mVector	= VNorm(mMoveVec);
				param.mUp		= mNormalVec;
				param.mSpeedRate= mTrolleySpeed / max_speed;

				param.mCamLength	= mpExtendBuffer[GimmickExtend_TrolleyEx::eCSVLINE_CAM_LENGTH];
				param.mCamWidth		= mpExtendBuffer[GimmickExtend_TrolleyEx::eCSVLINE_CAM_WIDTH];
				param.mCamHeight	= mpExtendBuffer[GimmickExtend_TrolleyEx::eCSVLINE_CAM_HEIGHT];

				CameraManager::GetInstance()->SetTrolleyParam(&param);
			}


		}
	}


	ModelCollision* p_enemy = NULL;
	int attack_coll_num = CollisionManager::GetInstance()->GetAttackCollisionNum();
	ShapeCapsule capsuleA, capsuleB;
	capsuleA.mPosA	= mPosition;
	capsuleA.mPosB	= mPosition;
	capsuleA.mRadius= mBodySize;

	for (int iI = 1; iI < attack_coll_num; iI ++) {
		p_enemy = (ModelCollision *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);

		/* 戦闘不能者と既に攻撃された者を除く */
		if ((p_enemy->GetIsRetire() == false) &&
			(p_enemy->IsDamage() == false)) {

			capsuleB.mPosA	= p_enemy->Position();
			capsuleB.mPosB	= p_enemy->Position();
			capsuleB.mRadius= p_enemy->BodySize();

			if (CollisionManager::Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				is_break = TRUE;
				hitpos = mPosition;
			}
		}
	}


	if (is_break) {
		if (mIsOnPlayer == TRUE) {
			p_player->SetMatrixType(ModelBase::eMATRIX_TYPE_TRS);
			p_player->Position() = mPosition;

			p_player->SetPositionOnCollision();
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_RESET);
			mIsOnPlayer = FALSE;
			this->ResetOnNodeInfo(p_player);
		}

		SoundManager::GetSndComponent()->PlaySound(eSE_BURST);

		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST, &mPosition, 75.0f, 0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 5.0f);

		EFK_HANDLE efk_hdl;
		efk_hdl = EffekseerManager::GetInstance()->Play(eEFK_BREAK_LOCK, &mPosition, &VScale(mScale, 0.75f));
		EffekseerManager::GetInstance()->SetSpeed(efk_hdl, 2.0f);

		mScale = VGet(0.1f, 0.1f, 0.1f);
		mPosition = hitpos;

		mTrolleyCount = 0;
			
		mState = eTROLLEY_STATE_END;
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_TrolleyEx::updateSpotLight(void)
{
	// ポイントライト有効
	// オリジナルシェーダー有効
	GraphicManager::GetInstance()->SetEnablePLight(true, eSLIGHT_NO_00);
	GraphicManager::GetInstance()->SetPLightPosition(&VGet(mPosition.x,
														   mPosition.y + mBodyHeight,
														   mPosition.z),
													 eSLIGHT_NO_00);
	
	GraphicManager::GetInstance()->SetPLightDirection(&mMoveVec, eSLIGHT_NO_00);

	{
		const float range	= 30100;
		const float atten0	= 0.07f;
		const float atten1	= 0.0f;
		const float atten2	= 0.0f;
		const float outrange= 1.0f;
		const float inrange	= 0.635f;

		GraphicManager::GetInstance()->SetPLightRangeAtten(range, atten0, atten1, atten2, eSLIGHT_NO_00);
		GraphicManager::GetInstance()->SetPLightAngle(outrange, inrange, eSLIGHT_NO_00);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
VECTOR
GimmickExtend_TrolleyEx::getCenterPosByPolygon(VECTOR* pVertex)
{
	static const BYTE kPOINT_NUM = 3;

	VECTOR center_pos = ZERO_VEC;
	VECTOR* p_point[3][2];
	p_point[0][0] = &pVertex[0];
	p_point[0][1] = &pVertex[1];
	p_point[1][0] = &pVertex[1];
	p_point[1][1] = &pVertex[2];
	p_point[2][0] = &pVertex[2];
	p_point[2][1] = &pVertex[0];

	float	len[kPOINT_NUM] = {0};

	for (int iI = 0; iI < kPOINT_NUM; iI ++) {
		len[iI] = VSize( VSub(*p_point[iI][0], *p_point[iI][1]) );
	}

	std::map<float, int> tmp;
	// 乗ってるポリゴンの三辺の長さをソートとして格納
	for (int iI = 0; iI < kPOINT_NUM; iI ++) {
		tmp.insert(std::map<float, int>::value_type(len[iI], iI));
	}

	std::map<float, int>::iterator ite;
	ite = tmp.begin();

	center_pos = VScale(VAdd(*p_point[ite->second][0], 
							 *p_point[ite->second][1]),
						0.5f);

	return center_pos;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
VECTOR
GimmickExtend_TrolleyEx::getRouteVectorByPolygon(VECTOR& rP1, VECTOR& rP2, VECTOR& rP3, VECTOR& rHitPos)
{
	static const BYTE kPOINT_NUM = 3;

	VECTOR* p_point[3][2];
	p_point[0][0] = &rP1;
	p_point[0][1] = &rP2;
	p_point[1][0] = &rP2;
	p_point[1][1] = &rP3;
	p_point[2][0] = &rP3;
	p_point[2][1] = &rP1;

	VECTOR	vec[kPOINT_NUM] = {0};
	float	len[kPOINT_NUM] = {0};

	vec[0] = VSub(rP1, rP2);
	vec[1] = VSub(rP2, rP3);
	vec[2] = VSub(rP3, rP1);
	
	for (int iI = 0; iI < kPOINT_NUM; iI ++) {
		len[iI] = VSize(vec[iI]);
	}

	std::map<float, int> tmp;
	std::map<float, int>::iterator ite;

	// 乗ってるポリゴンの三辺の長さをソートとして格納
	for (int iI = 0; iI < kPOINT_NUM; iI ++) {
		tmp.insert(std::map<float, int>::value_type(len[iI], iI));
	}

	ite = tmp.begin();
	ite++;

	return vec[ite->second];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_TrolleyEx::getPolyCenterPos(int polyIndex, VECTOR* pRotationVec, VECTOR* pStart, VECTOR* pEnd)
{
	int poly_index_a, poly_index_b;
	VECTOR poly_vtx_a[3], poly_vtx_b[3];
	VECTOR center_pos_a, center_pos_b;

	// 現在乗っているポリゴン番号とそのペアのポリゴンを探す
	if (polyIndex % 2 == 0) {
		poly_index_a = polyIndex;
		poly_index_b = polyIndex + 1;
	}
	else {
		poly_index_a = polyIndex - 1;
		poly_index_b = polyIndex;
	}

	// ２つポリゴンに対し、それぞれ、一番短い辺を求める。
	MV1_REF_POLYGONLIST poly_list = MV1GetReferenceMesh(sRouteModelHandle, sRouteFrameIndex, TRUE);
	int vtx_index = 0;
	for (int iI = 0; iI < 3; iI ++) {
		vtx_index = poly_list.Polygons[poly_index_a].VIndex[iI];
		poly_vtx_a[iI] =  poly_list.Vertexs[vtx_index].Position;
	}
	for (int iI = 0; iI < 3; iI ++) {
		vtx_index = poly_list.Polygons[poly_index_b].VIndex[iI];
		poly_vtx_b[iI] =  poly_list.Vertexs[vtx_index].Position;
	}
		
	center_pos_a = getCenterPosByPolygon(poly_vtx_a);
	center_pos_b = getCenterPosByPolygon(poly_vtx_b);

	VECTOR vec;
	vec = VSub(center_pos_b, mPosition);
	if (VSize(vec) == 0.0f) {
		*pEnd	= center_pos_a;
		*pStart	= center_pos_b;
	}
	else {
		float dot = VDot(VNorm(vec), *pRotationVec);
		if (dot < 0) {
			*pEnd	= center_pos_a;
			*pStart	= center_pos_b;
		}
		else {
			*pEnd	= center_pos_b;
			*pStart	= center_pos_a;
		}
	}
}

/*=====================================*
 * Class GimmickExtend_PLight
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_PLight::GimmickExtend_PLight()
{
	

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_PLight::~GimmickExtend_PLight()
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_PLight::extendSetup(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_PLight::extendUpdate(void)
{
	int light_index = 0;
	light_index = (int)mpExtendBuffer[eCSVLINE_LIGHT_INDEX];
	GraphicManager::GetInstance()->SetPLightPosition(&VSub(mPosition, sRevisePos),
													 (ePLightNo)light_index);
}

/*=====================================*
 * Class GimmickExtend_Barrier
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_Barrier::GimmickExtend_Barrier()
{
	mRotCounter = 0.0f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_Barrier::~GimmickExtend_Barrier()
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Barrier::Update(void)
{
	GimmickBase::Update();

	// α有りのモデル描画
	MV1DrawModel(mModelHandle);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Barrier::Draw(void)
{
	GimmickBase::Draw();

	// α有りのモデル描画
	MV1DrawModel(mModelHandle);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Barrier::extendSetup(void)
{
	this->SetDrawFunc((ModelBase::DRAW_FUNC)DrawBarrierFunc);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_Barrier::extendUpdate(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void DrawBarrierFunc(int model_handle)
{
	// α抜きとしては何も表示させない
	return;
}

/*=====================================*
 * class GimmickExtend_AirShip
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_AirShip::GimmickExtend_AirShip()
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_AirShip::extendSetup(void)
{
	mPeriod = 0.0f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_AirShip::extendUpdate(void)
{
	VECTOR move_vec;
	move_vec.x	= mpExtendBuffer[GimmickExtend_AirShip::eCSVLINE_MOVEVEC_X];
	move_vec.y	= mpExtendBuffer[GimmickExtend_AirShip::eCSVLINE_MOVEVEC_Y];
	move_vec.z	= mpExtendBuffer[GimmickExtend_AirShip::eCSVLINE_MOVEVEC_Z];

	float wave_pos;
	float wave_period = mpExtendBuffer[GimmickExtend_AirShip::eCSVLINE_WAVE_PERIOD];
	float wave_height = mpExtendBuffer[GimmickExtend_AirShip::eCSVLINE_WAVE_HEIGHT];
	wave_pos = sin(mPeriod) * wave_height;

	mPosition = VAdd(mPosition, move_vec);
	mPosition.y += wave_pos;

	mPeriod += wave_period;
}

/*=====================================*
 * class GimmickExtend_FloatBlock
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_FloatBlock::GimmickExtend_FloatBlock()
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_FloatBlock::~GimmickExtend_FloatBlock()
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_FloatBlock::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType/*=eATTACK_TYPE_NORMAL*/)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_FloatBlock::extendSetup(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_FloatBlock::extendUpdate(void)
{
	ModelMap* p_model = (ModelMap *)CollisionManager::GetInstance()->GetEarthModel();
	int model_handle = CollisionManager::GetInstance()->GetEarthModelHandle(0);

	int mesh_num = MV1GetFrameMeshNum(model_handle, eBG_TYPE_WATER);
	int mesh_index = MV1GetFrameMesh(model_handle, eBG_TYPE_WATER, 0);
	VECTOR max_pos = VScale(MV1GetMeshMaxPosition(model_handle, mesh_index), p_model->Scale().x);
	
	mPosition.y = (max_pos.y + (p_model->WaterOffsetPos() * p_model->Scale().x));
}

/*=====================================*
 * GimmickExtend_MagicCube
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_MagicCube::GimmickExtend_MagicCube()
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_MagicCube::~GimmickExtend_MagicCube()
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_MagicCube::extendSetup(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_MagicCube::extendUpdate(void)
{
	for (int iI = 0; iI < MV1GetFrameNum(mModelHandle); iI ++) {
		CollisionManager::GetInstance()->RefreshModel(mModelHandle, iI);
	}
}

/*=====================================*
 * GimmickExtend_PressWall
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_PressWall::GimmickExtend_PressWall()
{
	mCenterHeight = 0.0f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_PressWall::~GimmickExtend_PressWall()
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_PressWall::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType/*=eATTACK_TYPE_NORMAL*/)
{
	if (attackType != eATTACK_TYPE_GUARDIAN) {
		// 何も攻撃を受け付けない
		SoundManager::GetSndComponent()->PlaySound(eSE_SHIELD);
		return;
	}

	VECTOR eff_pos = mPosition;
	eff_pos.y += (mBodyHeight * 0.5f);

	float eff_size = mScale.x * 1.6f;

	int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST, &eff_pos, eff_size, 0.0f);
	EffectManager::GetInstance()->Stop(eff_handle, 7.5f);

	CameraManager::GetInstance()->StartVibeCamera(0.7f, 5.0f, 10.0f);
	SoundManager::GetSndComponent()->PlaySound(eSE_BURST);

	mState = eSTATE_BREAKING;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_PressWall::ReviseOnPosition(VECTOR& rMoveVec)
{
	GimmickBase::ReviseOnPosition(rMoveVec);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_PressWall::extendSetup(void)
{
	VECTOR temp_scale = mScale;	// 一時的に保持

	mScale = VGet(1.0f, 1.0f, 1.0f);

	int mesh_num;
	VECTOR max_pos = ZERO_VEC;
	VECTOR min_pos = ZERO_VEC;
	VECTOR tmp_pos;
	
	mesh_num = MV1GetMeshNum(mModelHandle);

	for (int iI = 0; iI < mesh_num; iI ++) {
		GetMeshMaxPosition(&tmp_pos, iI);
		if (tmp_pos.x > max_pos.x) max_pos.x = tmp_pos.x;
		if (tmp_pos.y > max_pos.y) max_pos.y = tmp_pos.y;
		if (tmp_pos.z > max_pos.z) max_pos.z = tmp_pos.z;

		GetMeshMinPosition(&tmp_pos, iI);
		if (tmp_pos.x < min_pos.x) min_pos.x = tmp_pos.x;
		if (tmp_pos.y < min_pos.y) min_pos.y = tmp_pos.y;
		if (tmp_pos.z < min_pos.z) min_pos.z = tmp_pos.z;
	}
	
	mScale = temp_scale;

	mCenterHeight = (min_pos.y + max_pos.y) * 0.5f;
	mCenterHeight *= mScale.y;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_PressWall::extendUpdate(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec_to_player;
	vec_to_player = VSub(p_player->Position(), mPosition);

	int move_stop_count = (int)mpExtendBuffer[GimmickExtend_PressWall::eCSVLINE_MOVE_STOP_COUNT];

	switch (mState) {
	case eSTATE_STOP_DOWN:
		mCounter ++;
		if (mCounter > move_stop_count) {
			mCounter = 0;
			mState = eSTATE_MOVE_UP;
		}
		break;

	case eSTATE_MOVE_UP:
		{
			VECTOR up_target_pos;
			up_target_pos.x = mpExtendBuffer[GimmickExtend_PressWall::eCSVLINE_UP_TARGET_POS_X];
			up_target_pos.y = mpExtendBuffer[GimmickExtend_PressWall::eCSVLINE_UP_TARGET_POS_Y];
			up_target_pos.z = mpExtendBuffer[GimmickExtend_PressWall::eCSVLINE_UP_TARGET_POS_Z];

			up_target_pos = VSub(up_target_pos, sRevisePos);

			VECTOR vec;
			vec = VSub(up_target_pos, mPosition);

			float up_move_rate = mpExtendBuffer[GimmickExtend_PressWall::eCSVLINE_UP_MOVE_RATE];
			VECTOR up_move_vec = VScale(vec, up_move_rate);
			mPosition = VAdd(mPosition, up_move_vec);
			
			float dist = VSize(vec);
			if (dist < 10.0f) {
				mState = eSTATE_STOP_UP;
			}
		}
		break;

	case eSTATE_STOP_UP:
		mCounter ++;
		if (mCounter > move_stop_count) {
			mCounter = 0;
			mState = eSTATE_PRESS_DOWN;
		}
		break;

	case eSTATE_PRESS_DOWN:
		{
			VECTOR org_pos;
			getOriginalPosition(&org_pos);

			VECTOR vec;
			vec = VSub(org_pos, mPosition);

			float down_move_rate = mpExtendBuffer[GimmickExtend_PressWall::eCSVLINE_DOWN_MOVE_RATE];
			VECTOR down_move_vec = VScale(vec, down_move_rate);
			mPosition = VAdd(mPosition, down_move_vec);

			// 壁との当たり判定
			updateHitPlayer(down_move_vec);

			float dist = VSize(vec);
			if (dist < 10.0f) {

				// プレイヤーとの距離が遠い場合は処理しない
				if (VSize(vec_to_player) < (kCHECK_PLAY_SOUND_DIST / 0.8f) ) {
					int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,&mPosition,100.0f,0.0f);
					EffectManager::GetInstance()->Stop(eff_handle, 0.5f);
					CameraManager::GetInstance()->StartVibeCamera(1.0f, 8.0f, 10.0f);

					int volume = getSoundVolume(kCHECK_PLAY_SOUND_DIST / 0.8f);
					if (volume != 0) {
						SoundManager::GetSndComponent()->SetVolume(eSE_BURST, volume );
						SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
						SoundManager::GetSndComponent()->SetVolume(eSE_BURST, kSOUND_VOLUME_DEFAULT );
					}
				}
				mState = eSTATE_STOP_DOWN;
			}
		}
		break;

	case eSTATE_BREAKING:
		{
			float alpha = GetAlpha();
			alpha -= 0.1f;
			if (alpha > 0.0f) {
				SetAlpha(alpha);
			} else {
				SetAlpha(0.0f);
				mState = eSTATE_BREAKED;
			}
		}
		break;

	case eSTATE_BREAKED:
		CollisionManager::GetInstance()->RemoveAttackCollision(this);
		CollisionManager::GetInstance()->RemoveBodyCollision(this);
		GraphicManager::GetInstance()->ReserveRemoveModel(this);
		this->SetAlpha(0.0f);
		ModelPmd::DeleteModel();

		mState = eSTATE_END;
		break;

	case eSTATE_END:
		break;

	default:
		break;
	};
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_PressWall::updateHitPlayer(VECTOR& rMoveVec)
{
	// 壁とプレイヤーの当たり判定
	MV1_COLL_RESULT_POLY_DIM HitResDim = {0};

	ShapeCapsule capsule;
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	if (p_player->GetIsRetire()) return;		// 戦闘不能者も除く
	if (p_player->IsDamage()) return;			// 既に攻撃された者も除く

	p_player->GetBodyCapsule(&capsule);

	// 移動量
	float move_value = VSize(rMoveVec);

	VECTOR center_pos;
	center_pos = mPosition;
	center_pos.y += mCenterHeight;

	VECTOR ch_pos = p_player->Position();
	ch_pos.y += p_player->BodyHeight();

	if (VSize(VSub(ch_pos, center_pos)) < ALL_MODEL_REVISE_DIST) {

		// モデルコリジョンのリフレッシュ
		MV1RefreshCollInfo(mModelHandle, 0);

		MV1_COLL_RESULT_POLY result_work;
		result_work = MV1CollCheck_Line(mModelHandle, 
										0, 
										ch_pos,
										center_pos);
		if (result_work.HitFlag == 0) {
			VECTOR down_vec = VNorm(VSub(ch_pos, center_pos));
			p_player->ProcessDamage(4, false, NULL, &down_vec, true, eATTACK_TYPE_BREAK_GUARD);
		}
	}
}

/*=====================================*
 * GimmickExtend_PressWallHard
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_PressWallHard::GimmickExtend_PressWallHard()
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickExtend_PressWallHard::~GimmickExtend_PressWallHard()
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_PressWallHard::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType/*=eATTACK_TYPE_NORMAL*/)
{
	// 何も攻撃を受け付けない
	SoundManager::GetSndComponent()->PlaySound(eSE_SHIELD);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_PressWallHard::extendSetup(void)
{
	// ベースクラスのメソッドコール
	GimmickExtend_PressWall::extendSetup();

	COLOR_F color = GetColorF(1.0f, 1.0f, 0.0f, 1.0f);

	for (UCHAR iI = 0; iI < MV1GetMaterialNum(mModelHandle); iI ++) {

		// マテリアルカラーを変更
		MV1SetMaterialDifColor(mModelHandle, iI, color);
		MV1SetMaterialSpcColor(mModelHandle, iI, color);
		MV1SetMaterialAmbColor(mModelHandle, iI, color);
		MV1SetMaterialEmiColor(mModelHandle, iI, color);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickExtend_PressWallHard::extendUpdate(void)
{
	// ベースクラスのメソッドコール
	GimmickExtend_PressWall::extendUpdate();
	
	MV1SetUseVertDifColor(mModelHandle, FALSE);
}


/**** end of file ****/

