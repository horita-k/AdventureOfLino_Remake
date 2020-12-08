/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * オブジェクトベースクラス
 * @author SPATZ.
 * @data   2012/04/14 19:35:52
 */
//---------------------------------------------------
#include "ObjectItem.h"
#include "../Charactor/CharaPlayer.h"
#include "AppLib/Basic/Basic.h"
#include "DataDefine/MotionDef.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Collision/CollisionManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "AppLib/Resource/ResourceLayout.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Effect/EffectManager.h"

#include "Scene/ActionTaskManager.h"
#include "Utility/Utility.h"
#include "DataDefine/VoiceDef.h"
#include "DataDefine/EffectDef.h"


/*=====================================*
 * static
 *=====================================*/
/* アイテム消失の距離 */
static const float sItemDisappearLength = 12000.0f;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectItem::ObjectItem(bool isDisappear/*=true*/)
{
	mIsDisapper = isDisappear;
	mIsActive = false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectItem::~ObjectItem()
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectItem::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg)
{
	this->Setup(objectIdx, itemIdx, pSetPos, rotDeg, false);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectItem::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg, bool isReserve)
{
	// モデルの挿入先を派生クラスで設定させる
	mpEntryTargetModel = CollisionManager::GetInstance()->GetEarthModel();

	mIsActive = true;

	ObjectBase::Setup(objectIdx, itemIdx, pSetPos, rotDeg);
	mObjectType = eOBJECT_TYPE_ITEM;

	eItemKind item_kind = (eItemKind)0;
	item_kind = convObjectKind_To_ItemKind(mObjectIndex);
	if (item_kind != -1) {
		char* plate_name = GetItemPlateName(item_kind);
		// プレート設定の場合の処理
		if (plate_name != NULL) {
			// 通常プレートの設定
			this->ChangeTexture(0, (char *)RES_LYT_PLATE_COMMON, TRUE);

			this->ChangeTexture(2, plate_name, TRUE);
		}
		else {
			char* card_name = GetItemCardName(item_kind);
			// カード設定の場合の処理
			if (card_name != NULL) {
				this->ChangeTexture(0, card_name, FALSE);
			}
		}
	}

	// モデルの挿入先を派生クラスで設定させる
	mpEntryTargetModel = NULL;
	//	this->InsertDrawModelSelf();

	if (isReserve == false) {
		GraphicManager::GetInstance()->InsertDrawModel(this, mpEntryTargetModel);
	}
	else {
		GraphicManager::GetInstance()->ReserveEntryModel(this);
	}

	mItemKind = item_kind;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectItem::Update(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	float total_size, len;
	bool is_dissappear = false;

	len = VSize(VSub(p_player->Position(), mPosition) );
	APP_ASSERT_PRINT(len > 0.000f,
					 "Failed updateObjectBaseEvent, player & item pos is SamePosition\n");

	total_size = p_player->BodySize() + mBodySize;
	total_size *= 1.6f; // アイテムを取りやすくする
	if ((len > sItemDisappearLength) && (mIsDisapper == true)) {
		is_dissappear = true;
	}
	else if ((len < total_size) &&
			 (mIsUsed == false)) {
		
		// 消滅フラグを有効にする
		is_dissappear = true;

		// アイテム種別を取得
		eItemKind item_kind = (eItemKind)0;
		item_kind = convObjectKind_To_ItemKind(mObjectIndex);
		APP_ASSERT_PRINT((item_kind != -1),
						 "Not support object kind : %d \n", mObjectIndex);

		if (item_kind == eITEM_KIND_HEART_CASE) {
			ActionTaskManager::tTaskInfo info;
			info.mTaskType	= ActionTaskManager::eTASK_GETITEM;
			info.mTaskValueA= (long long)this;
			info.mTaskValueB= eTREASURE_KIND_SMALL;
			ActionTaskManager::GetInstance()->NotifyAction(&info);
		}
		else if (item_kind == eITEM_KIND_HEART_PIECE) {
			ActionTaskManager::tTaskInfo info;
			info.mTaskType	= ActionTaskManager::eTASK_GETITEM;
			info.mTaskValueA= (long long)this;
			info.mTaskValueB= eTREASURE_KIND_SMALL;
			ActionTaskManager::GetInstance()->NotifyAction(&info);
		}
		else {
			GET_ITEM_HANDLER handler = NULL;
			handler = ObjectBase::GetGetItemHandler();
			handler(item_kind);
		}
	}

	// アイテムを動かす
	if (mObjectIndex != eOBJECT_KIND_CHECK_POINT) {
		mRotation.y += 0.06f;
		mPosition.y += -cos(mRotation.y * 0.75f) * 0.5f;
	}
	
	if (is_dissappear == true) {

		mIsUsed = true;
		mIsActive = false;

		PRINT_CONSOLE("[%d] ReserveRemoveModel \n", __LINE__);
		GraphicManager::GetInstance()->ReserveRemoveModel(this);
		ModelPmd::DeleteModel();

		return; /* 即リターン */
	}

	ObjectBase::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectItem::SetUsed(void)
{
	mIsUsed = true;
	mIsActive = false;

	PRINT_CONSOLE("[%d] ReserveRemoveModel \n", __LINE__);
	//	GraphicManager::GetInstance()->ReserveRemoveModel(this);
	GraphicManager::GetInstance()->RemoveDrawModel(this);
		
	ModelPmd::DeleteModel();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
eItemKind
ObjectItem::convObjectKind_To_ItemKind(int objectKind)
{
	eItemKind item_kind = (eItemKind)0;
	
	switch (objectKind) {
	case eOBJECT_KIND_HEART:
		item_kind = eITEM_KIND_HEART;
		break;
	case eOBJECT_KIND_MONEY_1:
		item_kind = eITEM_KIND_MONEY_1;
		break;
	case eOBJECT_KIND_MONEY_5:
		item_kind = eITEM_KIND_MONEY_5;
		break;
	case eOBJECT_KIND_MONEY_10:
		item_kind = eITEM_KIND_MONEY_10;
		break;
	case eOBJECT_KIND_MONEY_20:
		item_kind = eITEM_KIND_MONEY_20;
		break;
	case eOBJECT_KIND_MONEY_50:
		item_kind = eITEM_KIND_MONEY_50;
		break;
	case eOBJECT_KIND_MONEY_100:
		item_kind = eITEM_KIND_MONEY_100;
		break;
	case eOBJECT_KIND_MONEY_200:
		item_kind = eITEM_KIND_MONEY_200;
		break;
	case eOBJECT_KIND_MONEY_400:
		item_kind = eITEM_KIND_MONEY_400;
		break;
	case eOBJECT_KIND_HEART_CASE:
		item_kind = eITEM_KIND_HEART_CASE;
		break;
	case eOBJECT_KIND_PLATE_ARROW:
		item_kind = eITEM_KIND_PLATE_ARROW;
		break;
	case eOBJECT_KIND_PLATE_BOMB:
		item_kind = eITEM_KIND_PLATE_BOMB;
		break;
	case eOBJECT_KIND_HEART_PIECE:
		item_kind = eITEM_KIND_HEART_PIECE;
		break;
	case eOBJECT_KIND_CHECK_POINT:
		// nop.
		break;
	default:
		item_kind = (eItemKind)-1;
		break;
	};

	return item_kind;
}

/*=====================================*
 * ObjectItem_CheckPoint
 * チェックポイントクラス
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectItem_CheckPoint::ObjectItem_CheckPoint()
{
	mEffHndAura = -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectItem_CheckPoint::~ObjectItem_CheckPoint()
{
	if (mEffHndAura != -1) {
		EffectManager::GetInstance()->Stop(mEffHndAura);
		mEffHndAura = -1;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectItem_CheckPoint::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg)
{
	// 基底クラスの Setup
	ObjectItem::Setup(objectIdx, itemIdx, pSetPos, rotDeg);

	//	mEffHndAura = EffectManager::GetInstance()->Play(eEF_AURA, &mPosition, &VGet(300.0f, 100.0f, 300.0f));
	this->SetVisible(true);

	FLOAT4 color;
	color.x = 0.0f;
	color.y = 1.0f;
	color.z = 0.5f;
	color.w = 1.0f;
	EffectManager::GetInstance()->SetVec4(mEffHndAura, "Color", &color);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectItem_CheckPoint::Update(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vecA, vecB;
	float total_size, len, treasure_center;

	vecA = p_player->Position();
	vecA.y = 0.0f;
	vecB = VGet(mPosition.x, 0.0f, mPosition.z);

	len = VSize(VSub(vecA, vecB) );
	//	APP_ASSERT_PRINT(len > 0.000f,
	//					 "Failed updateObjectBaseEvent, player & item pos is SamePosition\n");

	total_size = p_player->BodySize() + mBodySize;
	total_size *= 1.6f; // アイテムを取りやすくする
	treasure_center = mPosition.y + (mBodySize * 0.5f);

	if ((len < total_size) &&
		(p_player->Position().y < treasure_center) &&
		((p_player->Position().y + p_player->BodyHeight() ) ) > treasure_center) {
		if (mIsUsed == false) {
			// チェックポイント更新のイベントを通知する
			ActionTaskManager::tTaskInfo task;
			task.mTaskType		= ActionTaskManager::eTASK_CHECK_POINT;
			task.mTaskValueA	= (long)this;
			task.mTaskValueB	= 0x00;
			
			ActionTaskManager::GetInstance()->NotifyAction(&task);

			mIsUsed = true;
		}
	} else {
		mIsUsed = false;
	}
	//	ObjectBase::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectItem_CheckPoint::SetVisible(bool isVisible, int frameIndex/*=-1*/)
{
	// 表示
	if (isVisible == true) {
		if (mEffHndAura == -1) {
			mEffHndAura = EffectManager::GetInstance()->Play(eEF_AURA, &mPosition, &VGet(300.0f, 100.0f, 300.0f));
		}
	}
	// 非表示
	else {
		if (mEffHndAura != -1) {
			EffectManager::GetInstance()->Stop(mEffHndAura, 0.0f);
			mEffHndAura = -1;
		}
	}
}

/**** end of file ****/

