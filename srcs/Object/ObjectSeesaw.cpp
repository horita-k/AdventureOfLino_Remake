/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * オブジェクトベースクラス
 * @author SPATZ.
 * @data   2012/04/14 19:35:52
 */
//---------------------------------------------------
#include "ObjectSeesaw.h"
#include "AppLib/Basic/Basic.h"
#include "DataDefine/MotionDef.h"
#include "../Charactor/CharaPlayer.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Collision/CollisionManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "Scene/ActionTaskManager.h"
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
ObjectSeesaw::ObjectSeesaw()
{
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectSeesaw::~ObjectSeesaw()
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectSeesaw::DeleteModel(void)
{
	// mModelHandle が解放される前に行う
	MV1TerminateCollInfo(mModelHandle, 0);

	ModelPmd::DeleteModel();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectSeesaw::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg)
{
	ObjectBase::Setup(objectIdx, itemIdx, pSetPos, rotDeg);
	mObjectType = eOBJECT_TYPE_SWITCH;

	// モデルの挿入先を派生クラスで設定させる
	mpEntryTargetModel = CollisionManager::GetInstance()->GetEarthModel();
	this->InsertDrawModelSelf();

	CollisionManager::GetInstance()->EntryEarthCollision(mModelHandle, 0);

	//	this->StartMotion(0, false, 0.5f);
	// モーフモデルに設定
	SetupMorphType(0.5f);
	StartMorph(1, 2, 1.0f);

	MV1SetupCollInfo(mModelHandle,
					 0,
					 8, 8, 8);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectSeesaw::Update(void)
{
	bool is_onplayer = false;
	static const float sCapsuleHeight = 1.0f;
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	// モデルコリジョンのリフレッシュ
	MV1RefreshCollInfo(mModelHandle, 0);

	// コリジョン用のラインを用意
	VECTOR vec_start, vec_end;
	MV1_COLL_RESULT_POLY result_work;

	VECTOR near_pos, far_pos, jump_vec;
	float near_dist, far_dist, near_far_length, rate;
	this->GetFramePosFromName(&near_pos, "手前");
	this->GetFramePosFromName(&far_pos,  "奥");
	near_far_length = VSize(VSub(near_pos, far_pos));
	near_far_length /= 2.0f;

	/**** プレイヤーの処理 ****/
	vec_start	= VAdd(p_player->Position(), VGet(0.0f, ((mBodySize * 0.8f) + sCapsuleHeight), 0.0f));
	vec_end		= VAdd(p_player->Position(), p_player->MoveVec());
	vec_end.y += -10.0f;
	result_work = MV1CollCheck_Line(mModelHandle, 0, vec_start,	vec_end);
	if (result_work.HitFlag == 1) {
		// 二点との距離の近さで傾きを変える
		near_dist		= VSize(VSub(p_player->Position(), near_pos));
		far_dist		= VSize(VSub(p_player->Position(), far_pos));
		if (near_dist < far_dist) {
			rate = (1.0f - (near_dist / near_far_length));
			StartMorph(1, 2, -0.5f * pow(rate, 4));
			jump_vec = VScale(VNorm(VSub(far_pos, near_pos)), 20.0f);
		} else {
			rate = (1.0f - (far_dist / near_far_length));
			StartMorph(1, 2, 0.5f * pow(rate, 4));
			jump_vec = VScale(VNorm(VSub(near_pos, far_pos)), 20.0f);
		}
		is_onplayer = true;
	}

	/**** モノの処理 ****/
	ObjectBase::eObjectType object_type = (ObjectBase::eObjectType)0;
	UCHAR hand_state = 0;
	for (int iI = 0; iI < sObjectBaseNum; iI ++) {
		object_type = sppObjectBase[iI]->GetObjectType();
		if (object_type == ObjectBase::eOBJECT_TYPE_HAND) {
			hand_state = ((ObjectHand *)sppObjectBase[iI])->State();
			if (hand_state == ObjectHand::eHAND_STATE_THROWING) {

				// シーソーに乗っける事のできるオブジェクトの判定
				BYTE hand_type = ((ObjectHand *)sppObjectBase[iI])->GetHandType();
				if ((hand_type == ObjectHand::eHAND_TYPE_STONE) ||
					(hand_type == ObjectHand::eHAND_TYPE_SUNOHARA) ||
					(hand_type == ObjectHand::eHAND_TYPE_USACHU) ) {

					vec_start = sppObjectBase[iI]->Position();
					vec_end = VAdd(vec_start, ((ObjectHand *)sppObjectBase[iI])->MoveVec());
					vec_end.y += -10.0f;
					
					// 水モデルと線分の当たり判定
					//				if (CollisionManager::GetInstance()->CheckWaterCollision(vec_start, vec_end, &hitpos, &normal)) {
					result_work = MV1CollCheck_Line(mModelHandle, 0, vec_start,	vec_end);
					if (result_work.HitFlag == 1) {

						if (is_onplayer == true) {
							p_player->ProcessBurstJump(&jump_vec);
						}

						// 二点との距離の近さで傾きを変える
						near_dist	= VSize(VSub(sppObjectBase[iI]->Position(), near_pos));
						far_dist	= VSize(VSub(sppObjectBase[iI]->Position(), far_pos));
						if (near_dist < far_dist) {
							rate = (1.0f - (near_dist / near_far_length));
							StartMorph(1, 2, -0.5f * pow(rate, 4));
						} else {
							rate = (1.0f - (far_dist / near_far_length));
							StartMorph(1, 2, 0.5f * pow(rate, 4));
						}

						break;
					}
				}
			}
		}
	}



	ObjectBase::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectSeesaw::SetUsed(void)
{
	
}

/**** end of file ****/

