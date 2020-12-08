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

#include "StageFuncCommon.h"

#include "AppLib/Effect/EffectManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Graphic/CameraManager.h"

#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/RenderBloom.h"

#include "AppLib/Resource/ResourceManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "AppLib/Resource/ResourceLayout.h"

#include "DataDefine/EffectDef.h"

#include "Gimmick/GimmickBase.h"
#include "Gimmick/GimmickExtend.h"

#include "../CommandFunctions.h"
#include "../SceneMainGame.h"
#include "../ExternDefine.h"		//<! Extern 宣言は全てここに集約する

/*=====================================*
 * static
 *=====================================*/
// 前回のステージ番号リスト
static std::vector<int>		sPrevStageIndexList;

/*=====================================*
 * public
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
StageFuncCommon::StageFuncCommon()
{
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
StageFuncCommon::~StageFuncCommon()
{
	destroy();
}

//---------------------------------------------------
/**
   
 */
//---------------------------------------------------
void
StageFuncCommon::SetupBeforeLoad(void)
{
	tStageInfo info = {0};
	int event_kind, event_flag;

	switch (sStageIndex) {
	case 1: // CaveOfLake
		if (TotalInfo::GetInstance()->GetEventFlagEnable(eEVENT_01, eEVENT_01_END_BATTLE_FRES)) {
			sModelMap.SetVisible(false, eBG_TYPE_EXT_00);
			spBGInfo->mActionLimit = eACTION_OK;			
		}
		break;

	case 9:
		info.mFileIndex = eRES_STG_REVELNEAR2ND_EARTH;
		info.mIsCollision = true;
		sSubStageInfoList.push_back(info);
		break;

	case 10: // Revelnear2ndEarth
		info.mFileIndex = eRES_STG_REVELNEAR2ND_HORBOR;
		info.mIsCollision = true;
		sSubStageInfoList.push_back(info);

		if (IS_END_EVT(eEVENT_35, eEVENT_35_AFTER_GET_GREEN_CRYSTAL)) {
			//<! NPCのセリフを変更
			CharaNpc::SetCsvSubNo(1);
		}

		break;

	case 11: // Revelnear2ndHorbor
		info.mFileIndex = eRES_STG_REVELNEAR2ND_EARTH;
		info.mIsCollision = true;
		sSubStageInfoList.push_back(info);

		if (IS_END_EVT(eEVENT_11, eEVENT_11_WAKEUP_TAKORUKA) ) {
			//<! NPCのセリフを変更
			CharaNpc::SetCsvSubNo(1);
		}

		break;

	case 13: // 貴族街
		info.mFileIndex = eRES_STG_REVELNEAR2ND_EARTH;
		info.mIsCollision = true;
		sSubStageInfoList.push_back(info);

		info.mFileIndex = eRES_STG_REVELNEAR2ND_MERGE;
		info.mIsCollision = false;
		sSubStageInfoList.push_back(info);

		if (IS_END_EVT(eEVENT_29, eEVENT_29_COME_FRES) ) {
			//<! NPCのセリフを変更
			CharaNpc::SetCsvSubNo(1);
		}

		break;

	case 14:
		info.mFileIndex = eRES_STG_VUAL_SUB;
		info.mIsCollision = true;
		sSubStageInfoList.push_back(info);

		ResourceModel::GetInstance()->LoadItemResource(RES_MDL_BOOK_CLOSE);
		ResourceModel::GetInstance()->LoadItemResource(RES_MDL_COPY_ROD);

		//<! NPCのセリフを変更
		if (IS_END_EVT(eEVENT_35, eEVENT_35_END_BETA_GAME)) {
			CharaNpc::SetCsvSubNo(3);
		}
		else if (IS_END_EVT(eEVENT_14, eEVENT_14_ABOUT_SULT_MINE) ) {
			CharaNpc::SetCsvSubNo(2);
		}
		else if (IS_END_EVT(eEVENT_14, eEVENT_14_WILL_TALK)) {
			CharaNpc::SetCsvSubNo(1);
		}
		break;

	case 21: // Revelnear2ndMerge
		info.mFileIndex = eRES_STG_REVELNEAR2ND_EARTH;
		info.mIsCollision = true;
		sSubStageInfoList.push_back(info);

		info.mFileIndex = eRES_STG_REVELNEAR2ND_RICHCITY;
		info.mIsCollision = false;
		sSubStageInfoList.push_back(info);

		event_kind = eEVENT_21;
		event_flag = eEVENT_21_NAVI_EXPLAIN;
		if (TotalInfo::GetInstance()->GetEventFlagEnable(event_kind, event_flag) == false) {
			sStagePlaceIndex = 2;
		}

		if (IS_END_EVT(eEVENT_35, eEVENT_35_AFTER_GET_GREEN_CRYSTAL)) {
			//<! NPCのセリフを変更
			CharaNpc::SetCsvSubNo(2);
		}
		//<! １面をクリアしたかを判定
		//		if (TotalInfo::GetInstance()->GetIsFieldStartNoConvert(eFIELD_PRISM)) {
		//<! NPCのセリフを変更
		else if (IS_END_EVT(eEVENT_29, eEVENT_29_COME_FRES)) {
			//<! NPCのセリフを変更
			CharaNpc::SetCsvSubNo(1);
		}
		break;

	case 22: // Kitchen
		ResourceModel::GetInstance()->LoadItemResource(RES_MDL_OCARINA);

		if (IS_END_EVT(eEVENT_21, eEVENT_21_NAVI_EXPLAIN) ) {
			//<! NPCのセリフを変更
			CharaNpc::SetCsvSubNo(2);
		}
		break;

	case 25:
		//<! NPCのセリフを変更
		if (IS_END_EVT(eEVENT_29, eEVENT_29_COME_FRES)) {
			CharaNpc::SetCsvSubNo(1);
		}
		break;

	case 24:	// 地下水路
		//<! １面をクリアしたかを判定
		if (TotalInfo::GetInstance()->GetIsFieldStartNoConvert(eFIELD_PRISM)) {
			//<! NPCのセリフを変更
			CharaNpc::SetCsvSubNo(1);
		}
		break;

	case 35:
		// 飛空艇墜落イベント越していたら
		if (IS_END_EVT(eEVENT_47, eEVENT_47_CRASH_AIRSHIP) ) {
			//<! NPCのセリフを変更
			CharaNpc::SetCsvSubNo(1);
		}
		break;

	case 39:
//		ResourceModel::GetInstance()->LoadGimmickResource(RES_MDL_BARRIER);

		info.mFileIndex = eRES_STG_SULT_MANE;
		info.mIsCollision = false;
		sSubStageInfoList.push_back(info);

		break;

	case 49:
		info.mFileIndex = eRES_STG_HARAPPA_SUB;
		info.mIsCollision = false;
		sSubStageInfoList.push_back(info);

		break;

	case 81:
		break;

	default:
		break;
	}

	/**** ベースクラス関数を自クラス処理後にコール ****/
	StageFuncBase::SetupBeforeLoad();
}

//---------------------------------------------------
/**
   
 */
//---------------------------------------------------
void
StageFuncCommon::SetupAfterLoad(int& rPhase)
{
	/**** ベースクラス関数を自クラス処理前にコール ****/
	StageFuncBase::SetupAfterLoad(rPhase);

	unsigned long event_kind;
	unsigned long event_flag;
	int model_handle = sModelMap.GetModelHandle();

	unsigned long object_flag = 0x00;
	unsigned long event_def = ConvertPlaceIndex_TO_EventDefine(sStageIndex);
	if (event_def != eEVENT_ERROR) {
		object_flag = TotalInfo::GetInstance()->GetObjectFlagBit(event_def);
	}
	PRINT_CONSOLE("---- object_flag = 0x%x ---- \n", object_flag);

	switch (sStageIndex) {
	case 9: // Revelnear2ndHorbor
		break;

	case 14:	// 魔術大学
		event_kind = eEVENT_14;
		event_flag = eEVENT_14_WELCOM_COLLEAGE;
		//		event_flag = eEVENT_14_WILL_TALK;
		if (TotalInfo::GetInstance()->GetEventFlagEnable(event_kind, event_flag) == false) {
			sTaskInfo.mTaskType		= ActionTaskManager::eTASK_EVENT;
			sTaskInfo.mTaskValueA	= (BYTE)event_kind;
			sTaskInfo.mTaskValueB	= (BYTE)event_flag;
			rPhase = SceneMainGame::ePHASE_EVENT_BEFORE;
		}
		break;

	case 18:	// cd_gato
		break;

	case 20: // 墓地
		//		attachSkydomeTexture("../model/BG/Skydome/nightsky.jpg");
		ResourceModel::GetInstance()->LoadModelResource("BG/Skydome/sky_night.mv1");
		sModelXfile[eMD_SKYDOME].LinkModel("BG/Skydome/sky_night.mv1");
		sModelXfile[eMD_SKYDOME].Scale() = VGet(600.0f, 600.0f, 600.0f);

		break;

	case 21: // Revelnear2ndMerge

		event_kind = eEVENT_21;
		event_flag = eEVENT_21_NAVI_EXPLAIN;
		if (TotalInfo::GetInstance()->GetEventFlagEnable(event_kind, event_flag) == false) {
			sTaskInfo.mTaskType		= ActionTaskManager::eTASK_EVENT;
			sTaskInfo.mTaskValueA	= (BYTE)event_kind;
			sTaskInfo.mTaskValueB	= (BYTE)event_flag;
			rPhase = SceneMainGame::ePHASE_EVENT_BEFORE;
		}
		CollisionManager::GetInstance()->EntryEarthCollision(model_handle,	(eBG_TYPE_GIMMICK + 0));
		CollisionManager::GetInstance()->EntryWallCollision(model_handle,	(eBG_TYPE_GIMMICK + 1));

		spSubModelMap[0].SetVisible(false, (eBG_TYPE_GIMMICK+0));

		CollisionManager::GetInstance()->EntryWallCollision(model_handle, (eBG_TYPE_GIMMICK + 2));
		sModelMap.EntryBarrier((eBG_TYPE_GIMMICK + 2));
		break;

	case 22: // Kitchen
		event_kind = eEVENT_22;
		event_flag = eEVENT_22_WAKEUP;
		if (TotalInfo::GetInstance()->GetEventFlagEnable(event_kind, event_flag) == false) {
			sTaskInfo.mTaskType		= ActionTaskManager::eTASK_EVENT;
			sTaskInfo.mTaskValueA	= (BYTE)event_kind;
			sTaskInfo.mTaskValueB	= (BYTE)event_flag;
			rPhase = SceneMainGame::ePHASE_EVENT_BEFORE;
		}

		// 最初はポーションを持たせる
		addMyWeapon(eITEM_KIND_PORTION);

		CollisionManager::GetInstance()->EntryEarthCollision(model_handle, (eBG_TYPE_GIMMICK + 0));

		break;

	case 24: // 地下水路
		event_kind = eEVENT_24;
		event_flag = eEVENT_24_WELCOME_WATERWAY;
		if (TotalInfo::GetInstance()->GetEventFlagEnable(event_kind, event_flag) == false) {
			sTaskInfo.mTaskType		= ActionTaskManager::eTASK_EVENT;
			sTaskInfo.mTaskValueA	= (BYTE)event_kind;
			sTaskInfo.mTaskValueB	= (BYTE)event_flag;
			rPhase = SceneMainGame::ePHASE_EVENT_BEFORE;
		}

		// フォグ設定
		GraphicManager::GetInstance()->EnableFogPattern(200,200,200,1000.0f,10750.0f);
		CollisionManager::GetInstance()->EntryEarthCollision(model_handle, (eBG_TYPE_GIMMICK + 0));

		// バリアの処理
		if (!(object_flag & 0x02)) {
			CollisionManager::GetInstance()->EntryWallCollision(model_handle, (eBG_TYPE_GIMMICK + 1));
			sModelMap.EntryBarrier((eBG_TYPE_GIMMICK + 1));
		} else {
			sModelMap.SetAlpha((eBG_TYPE_GIMMICK + 1), 0.0f);
		}
		
		// 波の処理
		if (((sPlayer.Position().y + sPlayer.BodyHeight()) < sEvt24_WaterStopHeight) ||
			(object_flag & 0x01)) {
			// 水の高さの設定
			sModelMap.WaterOffsetPos() = sEvt24_WaterStopHeight;
			sModelMap.SetFramePosFromIndex((eBG_TYPE_GIMMICK + 0), &VGet(0.0f, sEvt24_WaterStopHeight, 0.0f));
			// 直値、注意
			sppObjectBase[0]->SetUsed();	// 使用済みに設定する
		}
		break;

	case 26: // 地底湖
		GraphicManager::GetInstance()->EnableFogPattern(150,230,230,100.0f,23750.0f);
		// バリアの処理
		if (TotalInfo::GetInstance()->GetEventFlagEnable(eEVENT_26, eEVENT_26_CLEAR_BARRIER) == false) {
			CollisionManager::GetInstance()->EntryWallCollision(model_handle, (eBG_TYPE_GIMMICK + 0));
			sModelMap.EntryBarrier((eBG_TYPE_GIMMICK + 0));
		} else {
			sModelMap.SetAlpha((eBG_TYPE_GIMMICK + 0), 0.0f);
		}
		break;

	case 34:
		{
			ResourceModel::GetInstance()->LoadModelResource("BG/Skydome/lostforest.mv1");
			sModelXfile[eMD_SKYDOME].LinkModel("BG/Skydome/lostforest.mv1");
			sModelXfile[eMD_SKYDOME].Scale() = VGet(2.0f, 2.0f, 2.0f);

			GraphicManager::GetInstance()->SetEnableDirectionLight(FALSE);
			GraphicManager::GetInstance()->SetEnablePLight(TRUE);
			GraphicManager::GetInstance()->SetPLightPosition(&VGet(-231.180f, 2544.760f, 10.000f) );
			GraphicManager::GetInstance()->SetPLightRangeAtten(20000.000f, 0.1, 1E-06, 2E-09);
			GraphicManager::GetInstance()->SetPLightDiffuse(0.498040f, 0.498040f, 0.498040f, 0.000000f);
			GraphicManager::GetInstance()->SetPLightSpecular(0.000000f, 0.000000f, 0.000000f, 0.000000f);
			GraphicManager::GetInstance()->SetPLightAmbient(0.501960f, 0.501960f, 0.501960f, 0.000000f);

			RenderBloom::SetBloomType(RenderBloom::eBLOOM_TYPE_HSB_ABSOLUTE);
			RenderBloom::SetBloomParam(11, 195, 10);
			RenderBloom::tBloomHSBParam param = {0};
			param.mHue = 80;param.mSaturation = 225;param.mBright = 115;
			RenderBloom::BloomHSBParam() = param;
		}

		break;

	case 35:
		{
			// 飛空艇墜落イベント越していたら
			if (IS_END_EVT(eEVENT_47, eEVENT_47_CRASH_AIRSHIP) ) {

				event_kind = eEVENT_35;
				event_flag = eEVENT_35_WELCOME_VILLAGE;
				if (TotalInfo::GetInstance()->GetEventFlagEnable(event_kind, event_flag) == false) {
					sTaskInfo.mTaskType		= ActionTaskManager::eTASK_EVENT;
					sTaskInfo.mTaskValueA	= (BYTE)event_kind;
					sTaskInfo.mTaskValueB	= (BYTE)event_flag;
					rPhase = SceneMainGame::ePHASE_EVENT_BEFORE;
				}
			}
			else {

				event_kind = eEVENT_35;
				event_flag = eEVENT_35_AFTER_GET_GREEN_CRYSTAL;
				if ((IS_END_EVT(eEVENT_38, eEVENT_38_TALK_SENTO) == true) &&
					(IS_END_EVT(event_kind, event_flag) == false)) {
			
					sTaskInfo.mTaskType		= ActionTaskManager::eTASK_EVENT;
					sTaskInfo.mTaskValueA	= (BYTE)event_kind;
					sTaskInfo.mTaskValueB	= (BYTE)event_flag;
					rPhase = SceneMainGame::ePHASE_EVENT_BEFORE;
				}

				// 飛空艇非表示
				GimmickBase* p_gimmick = GimmickBase::Search1stFindGimmickKind(eGIMMICK_KIND_AIRSHIP);
				APP_ASSERT((p_gimmick != NULL), "Not found gimmick \n");
				p_gimmick->SetVisible(false);
				p_gimmick->Scale() = ZERO_VEC;
			}


		}
		break;

	case 37:
		{
			{
				GraphicManager * const p_graphic = GraphicManager::GetInstance();

				// 自己発光を抑制する
				sModelMap.SetEnableEmissive(false);

				// ディレクションライト無効
				p_graphic->SetEnableDirectionLight(false);
				// ポイントライト有効
				p_graphic->SetEnablePLight(true);

				// オリジナルシェーダー有効
				p_graphic->SetEnableOriginalShader(true);

				// NearClipの値を変更する
				CameraManager::GetInstance()->SetNearClip(20.0f);

				// フォグを無効にする
				GraphicManager::GetInstance()->DisableFogPattern();
			}

			GraphicManager::GetInstance()->SetPLightRangeAtten(20100.0f, 0.03f, 0.000001f, 0.000000004f);
			GraphicManager::GetInstance()->SetPLightDiffuse(COL_BYTE_2_FLOAT(255), 
																COL_BYTE_2_FLOAT(128),
																COL_BYTE_2_FLOAT(20), 0.0f);
			GraphicManager::GetInstance()->SetPLightSpecular(COL_BYTE_2_FLOAT(21), 
																 COL_BYTE_2_FLOAT(10),
																 COL_BYTE_2_FLOAT(5), 0.0f);
			GraphicManager::GetInstance()->SetPLightAmbient(COL_BYTE_2_FLOAT(255), 
																COL_BYTE_2_FLOAT(128),
																COL_BYTE_2_FLOAT(0), 0.0f);
			
			sModelXfile[eMD_SKYDOME	].Scale() = ZERO_VEC;
			break;

	case 38:
		{

			{
				GraphicManager * const p_graphic = GraphicManager::GetInstance();

				// 自己発光を抑制する
				sModelMap.SetEnableEmissive(false);

				// ディレクションライト無効
				p_graphic->SetEnableDirectionLight(false);
				// ポイントライト有効
				p_graphic->SetEnablePLight(true);

				// オリジナルシェーダー有効
				p_graphic->SetEnableOriginalShader(true);

				// NearClipの値を変更する
				CameraManager::GetInstance()->SetNearClip(20.0f);

				// フォグを無効にする
				GraphicManager::GetInstance()->DisableFogPattern();
			}

			SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);

			GraphicManager::GetInstance()->SetPLightPosition(&VGet(0.0f, 8100.0f, 0.0f));

			GraphicManager::GetInstance()->SetPLightRangeAtten(15900.0f, 0.04f, 0.00000000011f, 0.000000017f);
			GraphicManager::GetInstance()->SetPLightDiffuse(COL_BYTE_2_FLOAT(203),
																COL_BYTE_2_FLOAT(192),
																COL_BYTE_2_FLOAT(192), 0.0f);
			GraphicManager::GetInstance()->SetPLightSpecular(COL_BYTE_2_FLOAT(217), 
																 COL_BYTE_2_FLOAT(228),
																 COL_BYTE_2_FLOAT(234), 0.0f);
			GraphicManager::GetInstance()->SetPLightAmbient(COL_BYTE_2_FLOAT(25), 
																COL_BYTE_2_FLOAT(67),
																COL_BYTE_2_FLOAT(255), 0.0f);
			
			// オリジナルシェーダー無効
			GraphicManager::GetInstance()->SetEnableOriginalShader(false);

			// NearClipの値を変更する
			CameraManager::GetInstance()->SetDefaultNearFarClip();

			// バリアの追加
			sModelMap.EntryBarrier((eBG_TYPE_GIMMICK + 0));

			sModelXfile[eMD_SKYDOME	].Scale() = ZERO_VEC;

			event_kind = eEVENT_38;
			event_flag = eEVENT_38_CRYSTAL_FOREST;


			// 赤竜を倒している
			if (IS_END_EVT(eEVENT_28, eEVENT_28_BTL_TREASURE_A) == true) {

				// 森のクリスタル出現イベント発生？
				if (IS_END_EVT(eEVENT_38, eEVENT_38_CRYSTAL_FOREST) == false) {
					// ０番をクリスタル森とみなし、最初は非表示にする
					sppObjectBase[0]->Scale() = ZERO_VEC;
					((ObjectTresureCrystal *)sppObjectBase[0])->SetCrystalEvent(eEVENT_38_TALK_SENTO);

					// クリスタル出現イベント
					sTaskInfo.mTaskType		= ActionTaskManager::eTASK_EVENT;
					sTaskInfo.mTaskValueA	= (BYTE)event_kind;
					sTaskInfo.mTaskValueB	= (BYTE)eEVENT_38_CRYSTAL_FOREST;
					rPhase = SceneMainGame::ePHASE_EVENT_BEFORE;
				}
				else {
					// ０番をクリスタル森とみなし、最初から光らせておく
					processAppearCrystal(3,					// stagePlaceIndex
										 sppObjectBase[0],	// pCrystalObject
										 0,					// crystalColor : 0 : Green, 1 : Yellow
										 FALSE);			// isEfk
					((ObjectTresureCrystal *)sppObjectBase[0])->SetCrystalEvent(eEVENT_38_TALK_SENTO);
				}
			}
			else {
				// 0番をクリスタル土とみなし、隠す
				sppObjectBase[0]->Position().y -= 10000.0f;
			}

			// アルニャートを倒している？
			if (IS_END_EVT(eEVENT_43, eEVENT_43_BTL_TREASURE_A) == true) {

				// 土のクリスタル出現イベント発生？
				if (IS_END_EVT(eEVENT_38, eEVENT_38_CRYSTAL_EARTH) == false) {

					// １番をクリスタル土とみなし、最初は非表示にする
					sppObjectBase[1]->Scale() = ZERO_VEC;
					((ObjectTresureCrystal *)sppObjectBase[1])->SetCrystalEvent(eEVENT_38_TALK_YODEL);

					// クリスタル出現イベント
					sTaskInfo.mTaskType		= ActionTaskManager::eTASK_EVENT;
					sTaskInfo.mTaskValueA	= (BYTE)event_kind;
					sTaskInfo.mTaskValueB	= (BYTE)eEVENT_38_CRYSTAL_EARTH;
					rPhase = SceneMainGame::ePHASE_EVENT_BEFORE;
				}
				else {
					// 1番をクリスタル森とみなし、最初から光らせておく
					processAppearCrystal(4,					// stagePlaceIndex
										 sppObjectBase[1],	// pCrystalObject
										 1,					// crystalColor : 0 : Green, 1 : Yellow
										 FALSE);			// isEfk
					((ObjectTresureCrystal *)sppObjectBase[0])->SetCrystalEvent(eEVENT_38_TALK_SENTO);
				}
			}
			else {
				// １番をクリスタル土とみなし、隠す
				sppObjectBase[1]->Position().y -= 10000.0f;
			}
		}
		break;

		case 39:
			{
				SetPatternLight(0);

				GimmickExtend_TrolleyEx::SetupTrolleyExCollision(sModelMap.GetModelHandle(),
																 (eBG_TYPE_GIMMICK+0) );
				
				// 自己発光を抑制する
				sModelMap.SetEnableEmissive(false);

				//<! ついでにファークリップも変更する
				CameraManager::GetInstance()->SetFarClip(225000.0f);
				const float skyedome_scale = 1680.0f;
				sModelXfile[eMD_SKYDOME].Scale() = VGet(skyedome_scale, skyedome_scale, skyedome_scale);
				
				spSubModelMap[0].SetVisible(false);

				// オブジェクトの出現位置を設定させる(特別処理)
				sDissappearLength = 24000.0f;

				BYTE sub_place_index = 0;
				float nearest = 99999.0f;
				VECTOR pos; float rot;
				VECTOR ch_pos = sPlayer.Position();
				float dist = 0.0f;
				int cnt = 0;
				for (;;) {
					if (getPlayerPosRotFromBGModel(&pos, &rot, cnt)) {
						dist = VSize(VSub(ch_pos, pos));
						if (nearest > dist) {
							nearest = dist;
							sub_place_index = cnt;
						}
						cnt ++;
					}
					else {
						break;
					}
				}
				if (sub_place_index == 6) {
					SoundManager::GetInstance()->SetVolumeBGM(0);
				}
				else {
					SoundManager::GetInstance()->SetVolumeBGM(kSOUND_VOLUME_DEFAULT);
				}

			}
		}
		break;

	case 45:
		{
			GraphicManager::GetInstance()->SetEnableDirectionLight(TRUE);
			SetLightDifColor(GetColorF(0.949020f, 0.968630f, 0.968630f, 0.000000f) );
			SetLightSpcColor(GetColorF(0.098040f, 0.098040f, 0.098040f, 0.000000f) );
			SetLightAmbColor(GetColorF(0.792160f, 0.831370f, 0.929410f, 0.000000f) );
			GraphicManager::GetInstance()->SetEnablePLight(FALSE);
			GraphicManager::GetInstance()->EnableFogPattern(100,100,100, 10.0f, 18000.0f);
		}
		break;

	case 49:
		sModelXfile[eMD_SKYDOME	].Scale() = ZERO_VEC;

		spSubModelMap[0].Scale() = ZERO_VEC;

		break;

	case 0:
		ResourceModel::GetInstance()->LoadModelResource("BG/Skydome/sky_over.mv1");
		sModelXfile[eMD_SKYDOME	].LinkModel("BG/Skydome/sky_over.mv1");
		sModelXfile[eMD_SKYDOME	].Scale() = VGet(15.0f, 15.0f, 15.0f);
		break;

	case 1:
		ResourceModel::GetInstance()->LoadModelResource("BG/Skydome/sky_over.mv1");
		sModelXfile[eMD_SKYDOME	].LinkModel("BG/Skydome/sky_over.mv1");
		sModelXfile[eMD_SKYDOME	].Scale() = VGet(15.0f, 15.0f, 15.0f);
		break;

	case 60:
		//		attachSkydomeTexture("../model/BG/Skydome/nightsky.jpg");
		ResourceModel::GetInstance()->LoadModelResource("BG/Skydome/sky_red.mv1");
		sModelXfile[eMD_SKYDOME].LinkModel("BG/Skydome/sky_red.mv1");
		sModelXfile[eMD_SKYDOME].Scale() = VGet(600.0f, 600.0f, 600.0f);
		break;

	case 52:
#ifdef _DEBUG
		{
			ResourceManager::GetInstance()->LoadCharaResource(NAME_Guardian);

			VECTOR pos; float rot;
			mGuardian.LinkCharaResource(NAME_Guardian);
			//			getPlayerPosRotFromBGModel(&pos, &rot, 0);
			getPlayerPosRotFromBGModel(&pos, &rot, 2);
			mGuardian.Position()	= pos;
			mGuardian.Rotation().y	= rot;
			mGuardian.Scale() = GUARDIAN_DEF_SCALE;
			mGuardian.Setup();
		}
#endif // _DEBUG
		break;

	case 81:
#ifdef _DEBUG
		{
			ResourceManager::GetInstance()->LoadCharaResource(NAME_Guardian);

			VECTOR pos; float rot;
			mGuardian.LinkCharaResource(NAME_Guardian);
			getPlayerPosRotFromBGModel(&pos, &rot, 0);
			mGuardian.Position()	= pos;
			mGuardian.Rotation().y	= rot;
			mGuardian.Scale() = GUARDIAN_DEF_SCALE;
			mGuardian.Setup();
		}
#endif // _DEBUG
		break;

	case 90:
		{
//			MV1SetMaterialTypeAll(sModelMap.GetModelHandle(), DX_MATERIAL_TYPE_TOON_2);
		}
		break;

	default:

		break;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFuncCommon::Update(int counter)
{
	/*=====================================*
	 * コンパスの表示
	 *=====================================*/
	TotalInfo::GetInstance()->UpdateCompass();

	switch (sStageIndex) {

	case 34:
		{
			// スカイドームのY座標も更新する
			//		sModelXfile[eMD_SKYDOME].Position().y = sPlayer.Position().y;
		
			// スカイドームのY軸は回転させない
			sModelXfile[eMD_SKYDOME].Rotation().y = 0.0f;

			VECTOR light_pos = sPlayer.Position();
			light_pos.y += 5000.0f;
			GraphicManager::GetInstance()->SetPLightPosition(&light_pos);
		}

		break;

	case 37:
	//<! トロッコのライトの処理
		{
			VECTOR light_pos = sPlayer.Position();
			GraphicManager::GetInstance()->SetPLightPosition(&light_pos);
		}
		break;

	case 38:
		{
			float amp = sin(counter * 0.02f);
			GraphicManager::GetInstance()->SetPLightRangeAtten(8800.0f,
															   0.1f,
															   0.0f,
															   0.000000020f + (0.000000012f * amp) );
		}
		break;

	case 39:
		{
			VECTOR light_pos = sPlayer.Position();
			light_pos.y += sPlayer.BodyHeight();
			GraphicManager::GetInstance()->SetPLightPosition(&light_pos);
			//			GraphicManager::GetInstance()->SetPLightPosition(&VSub(VGet(2.5, 17500.0f, 2668.0f), sReviseDiffPos), ePLIGHT_NO_00);

		}
		GimmickExtend_TrolleyEx::UpdateTrolleyExCollision();
		break;

	case 81:
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
StageFuncCommon::SetPatternLight(int pattern)
{
	switch (sStageIndex) {
	case 39:
		switch (pattern) {
		case 0:
			GraphicManager::GetInstance()->SetEnableDirectionLight(FALSE);
			GraphicManager::GetInstance()->SetEnablePLight(TRUE, ePLIGHT_NO_00);
			GraphicManager::GetInstance()->SetEnablePLight(FALSE, ePLIGHT_NO_01);
			GraphicManager::GetInstance()->SetPLightPosition(&VGet(12498.050f, 4580.060f, -11515.960f) );
			GraphicManager::GetInstance()->SetPLightRangeAtten((float)201000.000f, (float)0.08, (float)5E-06, 0);
			GraphicManager::GetInstance()->SetPLightDiffuse(1.000000f, 0.501960f, 0.078430f, 0.000000f);
			GraphicManager::GetInstance()->SetPLightSpecular(0.000000f, 0.000000f, 0.000000f, 0.000000f);
			GraphicManager::GetInstance()->SetPLightAmbient(0.588240f, 0.501960f, 0.000000f, 0.000000f);
			break;

		case 1:
			GraphicManager::GetInstance()->SetEnableDirectionLight(true);
			GraphicManager::GetInstance()->DisablePLightAll();
			GraphicManager::GetInstance()->SetEnableOriginalShader(false);
			GraphicManager::GetInstance()->SetDefaultDirectionLight();
				
			// エミッシブ
			sModelMap.SetEnableEmissive(true);
			break;

		default:
			break;
		};

		break;
	default:
		break;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFuncCommon::SetPatternObject(int pattern)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFuncCommon::DestroyBuf(void)
{
	switch (sStageIndex) {
	case 39:
		GimmickExtend_TrolleyEx::DestroyTrolleyExCollision();
		break;

	case 52:
#ifdef _DEBUG
		mGuardian.Destroy();
		mGuardian.DeleteModel();
#endif // _DEBUG
		break;

	case 81:
#ifdef _DEBUG
		mGuardian.Destroy();
		mGuardian.DeleteModel();
#endif // _DEBUG
		break;

	default:
		break;
	};
}

/*=====================================*
 * private
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFuncCommon::destroy(void)
{
	/* 前回のステージリストを挿入する */
	sPrevStageIndexList.clear();
	sPrevStageIndexList.push_back(spBGInfo->mFileIndex);
	for (unsigned int iI = 0; iI < sSubStageInfoList.size(); iI ++) {
		sPrevStageIndexList.push_back( sSubStageInfoList.at(iI).mFileIndex );
	}
}

/**** end of file ****/


