/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * オブジェクトベースクラス
 * @author SPATZ.
 * @data   2012/04/14 19:35:52
 */
//---------------------------------------------------
#include "ObjectSwitch.h"
#include "AppLib/Basic/Basic.h"
#include "DataDefine/MotionDef.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Collision/CollisionManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "AppLib/Sound/SoundManager.h"
#include "Scene/ActionTaskManager.h"
#include "DataDefine/VoiceDef.h"
#include "Utility/Utility.h"
#include "../Scene/ExternDefine.h"


//#include "CharaInfo.h"

/*=====================================*
 * static
 *=====================================*/

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectSwitch::ObjectSwitch(int eventPlaceIndex, BYTE objectKind)
{
	mEventPlaceIndex = eventPlaceIndex;

	switch (objectKind) {
	case eOBJECT_KIND_MOGU_SWITCH:
		mSwitchKind = eSWITCH_KIND_MOGU;
		break;
	case eOBJECT_KIND_FACE_SWITCH:
	case eOBJECT_KIND_FACE_SWITCH_BIG:
		mSwitchKind = eSWITCH_KIND_FACE;
		break;
	case eOBJECT_KIND_CRYSTAL_SWITCH:
	case eOBJECT_KIND_CRYSTAL_SWITCH_BIG:
		mSwitchKind = eSWITCH_KIND_CRYSTAL;
		break;
	case eOBJECT_KIND_GETDOWN:
		mSwitchKind = eSWITCH_KIND_GETDOWN;
		break;
	default:
		APP_ASSERT_PRINT(NULL, 
						 "Not Support Object Kind : %d \n", objectKind);
		break;
	};
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectSwitch::~ObjectSwitch()
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectSwitch::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg)
{
	ObjectBase::Setup(objectIdx, itemIdx, pSetPos, rotDeg);
	mObjectType = eOBJECT_TYPE_SWITCH;

	// モデルの挿入先を派生クラスで設定させる
	mpEntryTargetModel = CollisionManager::GetInstance()->GetEarthModel();
	this->InsertDrawModelSelf();

	CollisionManager::GetInstance()->EntryBodyCollision(this);
	CollisionManager::GetInstance()->EntryAttackCollision(this);

	if (mSwitchKind == eSWITCH_KIND_GETDOWN) {
		//		this->StartMotion(12, true, 0.5f);
		this->EnableToonShader(0.005f, 0.005f);
		this->StartMotion(10, true, 0.5f);
	} else {
		this->StartMotion(0, false, 0.5f);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectSwitch::Update(void)
{
	ObjectBase::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ObjectSwitch::CheckDamageCollNormal(tCheckDamageWork* pParam)
{
	return ModelCollision::CheckDamageCollNormal(pParam);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectSwitch::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	bool is_success;
	float blend_speed = 0.1f;

	ActionTaskManager::tTaskInfo info;
	info.mTaskType = ActionTaskManager::eTASK_EVENT;
	info.mTaskValueA = mEventPlaceIndex;

	switch (mSwitchKind) {
	case eSWITCH_KIND_MOGU:
	case eSWITCH_KIND_CRYSTAL:
		/* トグルモード */
		if (this->GetMotionIndex() == 0) {
			info.mTaskValueB = (mItemKind + 0);
			mIsUsed = true;
		} else {
			info.mTaskValueB = (mItemKind + 1);
			mIsUsed = false;
		}
		
		if (mSwitchKind) {
			blend_speed = 1.0f;
			SoundManager::GetSndComponent()->PlaySound(eSE_PRESS_KEY);
		}
		break;

	case eSWITCH_KIND_FACE:
		/* ワンショットモード */
		if (this->GetMotionIndex() == 0) {
			info.mTaskValueB = (mItemKind + 0);
			mIsUsed = true;
		} else {
			// 既にスイッチが押されている場合は何もしない
			return;
		}
		break;

	case eSWITCH_KIND_GETDOWN:
		/* ワンショットモード */
		if (this->GetMotionIndex() == 10) {
			info.mTaskValueB = (mItemKind + 0);
			mIsUsed = true;
		} else {
			// 既にスイッチが押されている場合は何もしない
			return;
		}
		break;

	default:
		break;

	};

	is_success = ActionTaskManager::GetInstance()->NotifyAction(&info);
	if (is_success == true) {
		if (this->GetMotionIndex() == 0) {
			this->StartMotion(1, false, 0.5f, NULL, blend_speed);
		} else {
			this->StartMotion(0, false, 0.5f, NULL, blend_speed);
		}
		mIsDamage = true;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectSwitch::SetUsed(void)
{
	mIsUsed = true;
	this->StartMotion(1, false, 0.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectSwitch::SetUnused(void)
{
	mIsUsed = false;
	this->StartMotion(0, false, 0.5f);
}

/**** end of file ****/

