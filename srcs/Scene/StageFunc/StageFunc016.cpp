/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ステージ固有の処理
 * @author SPATZ.
 * @data   2015/08/26 22:06:24
 */
//---------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <d3dx9math.h>
#include "StageFunc016.h"
#include "AppLib/Effect/EffectManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Resource/ResourceManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "AppLib/Resource/ResourceLayout.h"
#include "DataDefine/EffectDef.h"
#include "DataDefine/VoiceDef.h"

#include "Gimmick/GimmickBase.h"
#include "Gimmick/GimmickExtend.h"
#include "../CommandFunctions.h"
#include "../ExternDefine.h"		//<! Extern 宣言は全てここに集約する
#include "../SceneMainGame.h"

/*=====================================*
 * static
 *=====================================*/

/*=====================================*
 * public
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
StageFunc016::StageFunc016()
{
	mGateTimerCnt	= 0;
	mGateTimerMax	= 0;
	//	mpGateObject	= NULL;
	mListGateObject.clear();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
StageFunc016::~StageFunc016()
{
	destroy();
}

//---------------------------------------------------
/**
   
 */
//---------------------------------------------------
void
StageFunc016::SetupBeforeLoad(void)
{
	tStageInfo info = {0};

	info.mFileIndex = eRES_STG_FOREST_SUB_1;
	info.mIsCollision = false;
	sSubStageInfoList.push_back(info);

	info.mFileIndex = eRES_STG_FOREST_SUB_2;
	info.mIsCollision = false;
	sSubStageInfoList.push_back(info);

	info.mFileIndex = eRES_STG_FOREST_SUB_3;
	info.mIsCollision = true;
	sSubStageInfoList.push_back(info);

	/* NPCの配置を変更する */
	if (IS_END_EVT(eEVENT_16, eEVENT_16_RESCUE_USACHU_AFTER)) {
		//<! NPCのセリフを変更
		CharaNpc::SetCsvSubNo(1);
	}

	/**** ベースクラス関数を自クラス処理後にコール ****/
	StageFuncBase::SetupBeforeLoad();
}

//---------------------------------------------------
/**
   
 */
//---------------------------------------------------
void
StageFunc016::SetupAfterLoad(int& rPhase)
{
	unsigned long event_kind;
	unsigned long event_flag;
	int model_handle = sModelMap.GetModelHandle();

	VECTOR pos;	float rot;

	/**** ベースクラス関数を自クラス処理前にコール ****/
	StageFuncBase::SetupAfterLoad(rPhase);

	SetPatternLight(0);

	// イベントの設定
	event_kind = eEVENT_16;
	event_flag = eEVENT_16_WELCOM_FOREST;
	if (TotalInfo::GetInstance()->GetEventFlagEnable(event_kind, event_flag) == false) {
		sTaskInfo.mTaskType		= ActionTaskManager::eTASK_EVENT;
		sTaskInfo.mTaskValueA	= (BYTE)event_kind;
		sTaskInfo.mTaskValueB	= (BYTE)event_flag;
		rPhase = SceneMainGame::ePHASE_EVENT_BEFORE;
	}
			
	CollisionManager::GetInstance()->EntryWallCollision(model_handle, (eBG_TYPE_GIMMICK + 0));
	sModelMap.EntryBarrier((eBG_TYPE_GIMMICK + 0));

	// 金網の設定
	CollisionManager::GetInstance()->EntryWallCollision(model_handle, (eBG_TYPE_GIMMICK + 1));
	CollisionManager::GetInstance()->EntryWallCollision(model_handle, (eBG_TYPE_GIMMICK + 2));
			
	/* うさちゅうの位置を変更する */
	if (IS_END_EVT(eEVENT_16, eEVENT_16_USAEMON_COME_2) == true) {
		getPlayerPosRotFromBGModel(&pos, &rot, 9);
		ObjectBase* p_object = search1stFindObjectKind(eOBJECT_KIND_USACHU);
		p_object->Position() = pos;
	} else if (IS_END_EVT(eEVENT_16, eEVENT_16_RESCUE_USACHU_AFTER) == true) {
		getPlayerPosRotFromBGModel(&pos, &rot, 13);
		ObjectBase* p_object = search1stFindObjectKind(eOBJECT_KIND_USACHU);
		p_object->Position() = pos;
	}

	/* 池の水を低下させる */
	if ((IS_END_EVT(eEVENT_16, eEVENT_16_USAEMON_COME_2) == true) &&
		(IS_END_EVT(eEVENT_28, eEVENT_28_BTL_TREASURE_A) == false)) {
		sModelMap.WaterOffsetPos() = sEvt24_WaterStopHeight;

		getPlayerPosRotFromBGModel(&pos, &rot, 9);
		EffectManager::GetInstance()->Play(eEF_AURA,&pos,
										   &VGet(300.0f, 100.0f, 300.0f));
	}

	
	// 該当のイベントトリガのスイッチを検索する
	ObjectBase* p_object = NULL;
	for (int iI = 0; iI < sObjectBaseNum; iI ++) {
		p_object = sppObjectBase[iI];
		if (p_object->GetObjectType() == ObjectBase::eOBJECT_TYPE_SWITCH) {
			if (p_object->GetItemKind() == eEVENT_16_OPEN_WIRE_B) {
				mListGateObject.push_back((ObjectSwitch *)p_object);
			}
		}
	}
	//	APP_ASSERT((mpGateObject != NULL),
	//			   "Not Found mpGateObject \n");
	APP_ASSERT((mListGateObject.size() != 0),
			   "Not Found mpGateObject \n");
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc016::Update(int counter)
{
	VECTOR vec_start, vec_end, hitpos, normal;

	TotalInfo::GetInstance()->CrazyCompass();

	// スカイドームのY座標も更新する
	sModelXfile[eMD_SKYDOME].Position().y = sPlayer.Position().y;

	// スカイドームのY軸は回転させない
	sModelXfile[eMD_SKYDOME].Rotation().y = 0.0f;

	// モノが水に落ちたかのチェック
	ObjectBase::eObjectType object_type = (ObjectBase::eObjectType)0;
	UCHAR hand_state = 0;
	for (int iI = 0; iI < sObjectBaseNum; iI ++) {
		object_type = sppObjectBase[iI]->GetObjectType();
		if (object_type == ObjectBase::eOBJECT_TYPE_HAND) {
			hand_state = ((ObjectHand *)sppObjectBase[iI])->State();
			if (hand_state == ObjectHand::eHAND_STATE_THROWING) {
				
				vec_start = sppObjectBase[iI]->Position();
				if (vec_start.x > -500.0f * spBGInfo->mScale) {
					vec_end = VAdd(vec_start, ((ObjectHand *)sppObjectBase[iI])->MoveVec());
					
					// 水モデルと線分の当たり判定
					if (CollisionManager::GetInstance()->CheckWaterCollision(vec_start, vec_end, &hitpos, &normal)) {
						
						CollisionManager::GetInstance()->RemoveBodyCollision(sppObjectBase[iI]);
						GraphicManager::GetInstance()->ReserveRemoveModel(sppObjectBase[iI]);
						sppObjectBase[iI]->DeleteModel();
						((ObjectHand *)sppObjectBase[iI])->State() = ObjectHand::eHAND_STATE_DISAPPEAR;


						ActionTaskManager::tTaskInfo info;
						info.mTaskType = ActionTaskManager::eTASK_EVENT;
						info.mTaskValueA = eEVENT_16;
						// うさちゅうを落とした場合
						if (((ObjectHand *)sppObjectBase[iI])->GetHandType() == ObjectHand::eHAND_TYPE_USACHU) {
							info.mTaskValueB = eEVENT_16_USAEMON_COME_2;
						} 
						// うさちゅう以外を落とした場合
						else {

							if (IS_END_EVT(eEVENT_28, eEVENT_28_BTL_TREASURE_A)) {
								info.mTaskValueB = eEVENT_16_USAEMON_COME_3;
							}
							else if (IS_END_EVT(eEVENT_16, eEVENT_16_USAEMON_COME_1)) {
								info.mTaskValueB = eEVENT_16_USAEMON_COME_1_B;
							} 
							else {
								info.mTaskValueB = eEVENT_16_USAEMON_COME_1;
							}
						}
						ActionTaskManager::GetInstance()->NotifyAction(&info);

						break;
					}
				}
			}
		}
	}

	// タイマーが動作している場合
	if (mGateTimerCnt > 0) {
		mGateTimerCnt --;

		int dev_cnt = 0;
		float rate = (float)mGateTimerCnt / (float)mGateTimerMax;
		if (rate > 0.60f) dev_cnt = 20;
		else if (rate > 0.45f) dev_cnt = 15;
		else if (rate > 0.30f) dev_cnt = 10;
		else if (rate > 0.15f) dev_cnt = 8;
		else dev_cnt = 5;

		if ((mGateTimerCnt % dev_cnt) == 0) {
			SoundManager::GetSndComponent()->PlaySound(eSE_SELECT_KEY);
		}

		if (mGateTimerCnt == 0) {
			ActionTaskManager::tTaskInfo info = {ActionTaskManager::eTASK_EVENT, eEVENT_16, eEVENT_16_CLOSE_WIRE_B};
			ActionTaskManager::GetInstance()->NotifyAction(&info);

			//			APP_ASSERT((mpGateObject != NULL),
			//					   "Not Found mpGateObject \n");
			APP_ASSERT((mListGateObject.size() != 0),
					   "Not Found mpGateObject \n");

			//			mpGateObject->SetUnused();

			for (unsigned int iI = 0; iI < mListGateObject.size(); iI ++) {
				mListGateObject.at(iI)->SetUnused();
			}

		}
	}


}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc016::SetPatternLight(int pattern)
{
	const float fnear = 100.0f;
	static const float sScaleRate = 1.9f;
	static const float sClipRate  = 1.0f;

	float skydome_scale;
	switch (pattern) {

		// 通常時
	case 0:
		{
			ResourceModel::GetInstance()->LoadModelResource("BG/Skydome/sky_forest.mv1");
			sModelXfile[eMD_SKYDOME].LinkModel("BG/Skydome/sky_forest.mv1");

			float ffar = 23200.0f;
			GraphicManager::GetInstance()->EnableFogPattern(150,230,230, fnear, ffar);

			skydome_scale = 170.0f;
			sModelXfile[eMD_SKYDOME].Scale() = VGet(skydome_scale, skydome_scale, skydome_scale);

			//<! ついでにファークリップも変更する
			CameraManager::GetInstance()->SetFarClip(ffar);
		}
		break;

		// 初回イベント時
	case 1:
		{
			ResourceModel::GetInstance()->LoadModelResource("BG/Skydome/sky_forest.mv1");
			sModelXfile[eMD_SKYDOME].LinkModel("BG/Skydome/sky_forest.mv1");

			float ffar = 38000.0f;
			GraphicManager::GetInstance()->EnableFogPattern(150,230,230, fnear, ffar);

			//<! ついでにファークリップも変更する
			static const float rate = DEFAULT_FAR_CLIP / 600.0f;
			skydome_scale = ffar / rate;
			skydome_scale *= sScaleRate;
			sModelXfile[eMD_SKYDOME].Scale() = VGet(skydome_scale, skydome_scale, skydome_scale);
			CameraManager::GetInstance()->SetFarClip(ffar);
		}
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
StageFunc016::StartGateTimer(WORD timerCnt)
{
	mGateTimerMax = timerCnt;
	mGateTimerCnt = timerCnt;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc016::SetPatternObject(int pattern)
{

}

/*=====================================*
 * private
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc016::destroy(void)
{
	mListGateObject.clear();
}

/**** end of file ****/


