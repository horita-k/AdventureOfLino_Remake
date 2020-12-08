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
#include "StageFunc036.h"
#include "AppLib/Effect/EffectManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Resource/ResourceManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "AppLib/Resource/ResourceLayout.h"
#include "DataDefine/EffectDef.h"
#include "Gimmick/GimmickBase.h"
#include "Gimmick/GimmickExtend.h"
#include "DataDefine/VoiceDef.h"

#include "../CommandFunctions.h"
#include "../ExternDefine.h"		//<! Extern 宣言は全てここに集約する
#include "../SceneMainGame.h"

/*=====================================*
 * static
 *=====================================*/
static const BYTE kCHANGE_TOON_LIGHT_NUM = 5;	// ライトがこの定義以上の場合にフォン⇒シェーダに変更
static const VECTOR kFLAME_DEFAULT_POS = VGet(2.5f, 1400.0f, 2668.0f);

/*=====================================*
 * public
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
StageFunc036::StageFunc036()
{
	mEffHndTornade	= -1;
	mEffHndFlame	= -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
StageFunc036::~StageFunc036()
{
	destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc036::SetupBeforeLoad(void)
{
	// Actionlimit を変更させる
	spBGInfo->mActionLimit = eACTION_OK;

	tStageInfo info = {0};
	info.mFileIndex = eRES_STG_SULT_MANE_TROLLEY;
	info.mIsCollision = false;
	sSubStageInfoList.push_back(info);

	/**** ベースクラス関数を自クラス処理後にコール ****/
	StageFuncBase::SetupBeforeLoad();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc036::SetupAfterLoad(int& rPhase)
{
	unsigned long event_kind;
	unsigned long event_flag;
	int model_handle = sModelMap.GetModelHandle();

	/**** ベースクラス関数を自クラス処理前にコール ****/
	StageFuncBase::SetupAfterLoad(rPhase);

	if (IS_END_EVT(eEVENT_36, eEVENT_36_DIRECTION_LIGHT_ON)) {
		/* ライトの切り替え */
		SetPatternLight(eLIGHT_PATTEN_FLAME);
		spBGInfo->mMusicName = BGM_DUNGEON_F_2;
	}
	else {
		/* ライトの切り替え */
		SetPatternLight(eLIGHT_PATTEN_NAVI_LIGHT);
		spBGInfo->mMusicName = BGM_DUNGEON_F;
	}

	spSubModelMap[0].SetVisible(false);
	
	/* オブジェクト表示の切り替え */
	this->SetPatternObject(StageFunc036::eOBJECT_PATTERN_NORMAL);

	if (IS_END_EVT(eEVENT_36, eEVENT_36_BOSS_END)) {

		/* モブクロックを操作し、存在しない場所にもっていく */
		for (int iI = 0; iI < sNpcNum; iI ++) {
			if (strcmp(CharaNpc::GetNpcResourceName(iI), NAME_MobClockW) == 0) {
				spNpc[iI].Position().y += 10000.0f;
			}
		}

	}

	event_kind = eEVENT_36;
	event_flag = eEVENT_36_WELCOME_MINE;
	if (TotalInfo::GetInstance()->GetEventFlagEnable(event_kind, event_flag) == false) {
		sTaskInfo.mTaskType		= ActionTaskManager::eTASK_EVENT;
		sTaskInfo.mTaskValueA	= (BYTE)event_kind;
		sTaskInfo.mTaskValueB	= (BYTE)event_flag;
		rPhase = SceneMainGame::ePHASE_EVENT_BEFORE;
	}

	sModelXfile[eMD_SKYDOME	].Scale() = ZERO_VEC;
	
	CollisionManager::GetInstance()->EntryEarthCollision(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_2F);
	CollisionManager::GetInstance()->EntryEarthCollision(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_3F);
	CollisionManager::GetInstance()->EntryEarthCollision(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_4F);
	CollisionManager::GetInstance()->EntryEarthCollision(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_5F);

	GimmickExtend_TrolleyEx::SetupTrolleyExCollision(sModelMap.GetModelHandle(),
													 (eBG_TYPE_GIMMICK+11) );



	// 毒霧の処理
	VECTOR eff_pos; float rot;
	getPlayerPosRotFromBGModel(&eff_pos, &rot, 6);
	
	int effindex = EffectManager::GetInstance()->Play(eEF_TORNADE, &eff_pos, 0.1f, 0.0f);
	mEffHndTornade = effindex;

	FLOAT4 smoke_color;
	smoke_color.x = 4.0f;
	smoke_color.y = 0.0f;
	smoke_color.z = 4.0f;
	smoke_color.w = 0.8f;
	EffectManager::GetInstance()->SetVec4(effindex, "SmokeColor", &smoke_color);
	EffectManager::GetInstance()->SetFloat(effindex, "morph_Anm",		0.0106f);
	EffectManager::GetInstance()->SetFloat(effindex, "morph_Height",	0.5f);
	EffectManager::GetInstance()->SetFloat(effindex, "morph_H",			0.0f);
	EffectManager::GetInstance()->SetFloat(effindex, "morph_S",			0.5f);
	EffectManager::GetInstance()->SetFloat(effindex, "morph_B",			-0.01f);
	EffectManager::GetInstance()->SetFloat(effindex, "morph_g_Sip",		-0.0135f);
	EffectManager::GetInstance()->SetFloat(effindex, "morph_Rot_p",		0.9f);
	EffectManager::GetInstance()->SetFloat(effindex, "morph_Rot_m",		-0.2f);
	EffectManager::GetInstance()->SetFloat(effindex, "morph_Num",		0.01f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc036::Update(int counter)
{
	/* コンパスの表示 */
	TotalInfo::GetInstance()->UpdateCompass();

	// 開始イベント未終了の場合はライトの設定を行わない
	if (TotalInfo::GetInstance()->GetEventFlagEnable(eEVENT_36, eEVENT_36_WELCOME_MINE) == true) {

		//<! 戦闘中の場合
		if ( (TotalInfo::GetInstance()->GetEventFlagEnable(eEVENT_36, eEVENT_36_BOSS_START) == true) &&
			 (TotalInfo::GetInstance()->GetEventFlagEnable(eEVENT_36, eEVENT_36_BOSS_END) == false) ) {
			//			int eff_index = getEventFunctionEffectHandle();
			// エフェクトが再生されている場合のみ座標関連の設定を行う。
			if (mEffHndFlame != -1) {
				VECTOR pos;
				sppEnemy[0]->GetFramePosFromName(&pos, "腰");
				EffectManager::GetInstance()->Position(mEffHndFlame)= pos;
				EffectManager::GetInstance()->Scale(mEffHndFlame)	= VGet(1800.0f, 1800.0f, 1800.0f);
			}
		}
		//<! 灯りなしでナビォライト起動中の場合
		else if (TotalInfo::GetInstance()->GetEventFlagEnable(eEVENT_36, eEVENT_36_DIRECTION_LIGHT_ON) == false)  {
			VECTOR light_pos = sNavi.Position();
			light_pos.y += (19.406f * sNavi.Scale().x);
			GraphicManager::GetInstance()->SetPLightPosition(&light_pos, ePLIGHT_NO_01);
		}
		//<! 灯りありの場合
		else if	(TotalInfo::GetInstance()->GetEventFlagEnable(eEVENT_36, eEVENT_36_DIRECTION_LIGHT_ON_1ST) == true) {

			if (mEffHndFlame != -1) {
				EffectManager::GetInstance()->Position(mEffHndFlame) = kFLAME_DEFAULT_POS;
			}

			// 炎が揺らいでる様な演出
			float amp = sin(counter * 0.2f);
			GraphicManager::GetInstance()->SetPLightRangeAtten(22340.0f, 
															   0.01000002f + (0.005f * amp),
															   0.000006f + (0.0000005f * amp),
															   0.0f, ePLIGHT_NO_00);
		}
	}
	

	/* 毒ガスの処理 */
	INT8 coll_index = 0;
	int  on_frame_index = 0;
	coll_index = sPlayer.GetOnCollisionIndex();
	on_frame_index = CollisionManager::GetInstance()->GetEarthModelFrameIndex(coll_index);
	if (on_frame_index == eBG_TYPE_GIMMICK_2F) {

//		EffectManager::GetInstance()->SetFloat(mEffHndTornade, "morph_Num", 0.01f);
		EffectManager::GetInstance()->SetFloat(mEffHndTornade, "morph_Num", 0.0075f);
	}
	else {
		EffectManager::GetInstance()->SetFloat(mEffHndTornade, "morph_Num", 0.0f);
	}

	GimmickExtend_TrolleyEx::UpdateTrolleyExCollision();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc036::SetPatternLight(int pattern)
{
	GraphicManager * const p_graphic = GraphicManager::GetInstance();

	switch (pattern) {
	case StageFunc036::eLIGHT_PATTEN_NAVI_LIGHT:
		{
			// 自己発光を抑制する
			sModelMap.SetEnableEmissive(false);
			int model_handle = -1;
			COLOR_F color;
			ModelBase* p_model = NULL;
			color = GetColorF(0.0f, 0.0f, 0.0f, 1.0f);
			for (int iI = 0; iI < sObjectBaseNum; iI ++) {
				p_model = sppObjectBase[iI];
				p_model->SetEnableEmissive(false);
				model_handle = p_model->GetModelHandle();
				color = GetColorF(0.2f, 0.2f, 0.2f, 1.0f);
				MV1SetDifColorScale(model_handle, color);

				MV1SetUseVertDifColor( model_handle, FALSE);
				MV1SetUseVertSpcColor( model_handle, TRUE);

				MV1SetAmbColorScale(model_handle, color);
				MV1SetEmiColorScale(model_handle, color);
				color = GetColorF(1000.0f, 1000.0f, 1000.0f,1.0f);
				MV1SetSpcColorScale(model_handle, color);
			}
			
			// ディレクションライト無効
			p_graphic->SetEnableDirectionLight(false);
			// ポイントライト有効
			p_graphic->GetInstance()->SetEnablePLight(false, ePLIGHT_NO_00); // 炎のライトを無効

			p_graphic->SetEnablePLight(true, ePLIGHT_NO_01);
			p_graphic->SetPLightRangeAtten(6000.0f, 0.02f, 0.0f, 0.00000007f, ePLIGHT_NO_01);
			p_graphic->SetPLightDiffuse(COL_BYTE_2_FLOAT(52), 
										COL_BYTE_2_FLOAT(54),
										COL_BYTE_2_FLOAT(237), 0.0f, ePLIGHT_NO_01);
			p_graphic->SetPLightSpecular(COL_BYTE_2_FLOAT(5), 
										 COL_BYTE_2_FLOAT(10),
										 COL_BYTE_2_FLOAT(5), 0.0f, ePLIGHT_NO_01);
			p_graphic->SetPLightAmbient(COL_BYTE_2_FLOAT(63), 
										COL_BYTE_2_FLOAT(63),
										COL_BYTE_2_FLOAT(63), 0.0f, ePLIGHT_NO_01);
			// オリジナルシェーダー有効
			p_graphic->SetEnableOriginalShader(true);

			// NearClipの値を変更する
			CameraManager::GetInstance()->SetNearClip(20.0f);

			// ナビォライトを有効にする
			sNavi.EnableNaviLight(true);

			// フォグを無効にする
			GraphicManager::GetInstance()->DisableFogPattern();

			// 隠し通路などを表示する
			MV1SetFrameVisible(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 3), TRUE);

			// ギミックのマテリアルを「フォンシェーダ」に変更する
			GimmickBase* p_gimmick = NULL;
			for (int iI = 0; iI < GimmickBase::GetGimmickBaseNum(); iI ++) {
				p_gimmick = GimmickBase::GetGimmickBase(iI);
				model_handle = p_gimmick->GetModelHandle();
				
				MV1SetMaterialTypeAll(model_handle, DX_MATERIAL_TYPE_NORMAL);
				MV1SetAmbColorScale(model_handle, GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ));
			}
		}
		break;

	case StageFunc036::eLIGHT_PATTEN_FLAME_1ST:
	case StageFunc036::eLIGHT_PATTEN_FLAME:
		{
			const float effsize = 1200.0f;
			if (mEffHndFlame == -1) {
				mEffHndFlame = EffectManager::GetInstance()->Play(eEF_FLAME,
																  (VECTOR *)&kFLAME_DEFAULT_POS,
																  &VGet(effsize, effsize, effsize),
																  (4.5f));
				SoundManager::GetSndComponent()->PlaySound(eSE_FLAME_SHOOT);
			}

			// オブジェクトのライトの切り替え
			int model_handle = -1;
			COLOR_F color;
			ModelBase* p_model = NULL;
			color = GetColorF(1.0f, 1.0f, 1.0f, 1.0f);
			for (int iI = 0; iI < sObjectBaseNum; iI ++) {
				p_model = sppObjectBase[iI];
				p_model->SetEnableEmissive(false);
				model_handle = p_model->GetModelHandle();
				MV1SetDifColorScale(model_handle, color);
				MV1SetUseVertDifColor(model_handle, TRUE);
				MV1SetAmbColorScale(model_handle, color);
				MV1SetEmiColorScale(model_handle, color);
				MV1SetSpcColorScale(model_handle, color);
			}

			GraphicManager::GetInstance()->SetEnablePLight(false, ePLIGHT_NO_01); // ナビォライトを無効

			GraphicManager::GetInstance()->SetEnableDirectionLight(false);
			GraphicManager::GetInstance()->SetEnablePLight(true, ePLIGHT_NO_00);
			GraphicManager::GetInstance()->SetEnableOriginalShader(false);

			p_graphic->SetPLightDiffuse(COL_BYTE_2_FLOAT(255), 
										COL_BYTE_2_FLOAT(128),
										COL_BYTE_2_FLOAT(0), 0.0f, ePLIGHT_NO_00);
			p_graphic->SetPLightSpecular(COL_BYTE_2_FLOAT(5), 
										 COL_BYTE_2_FLOAT(10),
										 COL_BYTE_2_FLOAT(5), 0.0f, ePLIGHT_NO_00);
			p_graphic->SetPLightAmbient(COL_BYTE_2_FLOAT(63), 
										COL_BYTE_2_FLOAT(63),
										COL_BYTE_2_FLOAT(63), 0.0f, ePLIGHT_NO_00);

			// 初回のライト設定値
			if (pattern == StageFunc036::eLIGHT_PATTEN_FLAME_1ST) {
				p_graphic->SetPLightRangeAtten(11340.0f, 0.09f, 0.00001f, 0.0f, ePLIGHT_NO_00);
				p_graphic->SetPLightPosition(&VSub(VGet(2.5f, 11000.0f, 2668.0f), 
												   sReviseDiffPos), ePLIGHT_NO_00);
			}
			// ２回目以降のライト設定値
			else if (pattern == StageFunc036::eLIGHT_PATTEN_FLAME) {
				GraphicManager::GetInstance()->SetPLightRangeAtten(22340.0f, 0.01000002f,0.000006f, 0.0f, ePLIGHT_NO_00);
				p_graphic->SetPLightPosition(&VSub(VGet(2.5, 17500.0f, 2668.0f), sReviseDiffPos), ePLIGHT_NO_00);
			}
			
			// 隠し通路などを非表示する
			MV1SetFrameVisible(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 3), FALSE);
			
			// NearClipの値を変更する
			CameraManager::GetInstance()->SetDefaultNearFarClip();

			// ナビォライトを無効にする
			sNavi.EnableNaviLight(false);

			// ギミックのマテリアルを「トゥーンシェーダ」に変更する
			GimmickBase* p_gimmick = NULL;
			for (int iI = 0; iI < GimmickBase::GetGimmickBaseNum(); iI ++) {
				p_gimmick = GimmickBase::GetGimmickBase(iI);
				model_handle = p_gimmick->GetModelHandle();
				
				MV1SetMaterialTypeAll(model_handle, DX_MATERIAL_TYPE_TOON_2);
				MV1SetAmbColorScale(model_handle, GetColorF( 1.0f, 1.0f, 1.0f, 1.0f ));
			}
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
StageFunc036::SetPatternObject(int pattern)
{
	switch (pattern) {
		// 通常のオブジェクト
	case StageFunc036::eOBJECT_PATTERN_NORMAL:
		// バリア更新処理
		StageFuncBase::setVisibleBarrierFromByEventFlag((eBG_TYPE_GIMMICK + 0), 
														eEVENT_36, eEVENT_36_CLEAR_BARRIER_0);
	
		// バリア更新処理
		StageFuncBase::setVisibleBarrierFromByEventFlag((eBG_TYPE_GIMMICK + 1), 
														eEVENT_36, eEVENT_36_CLEAR_BARRIER_1);

		// バリア更新処理
		StageFuncBase::setVisibleBarrierFromByEventFlag((eBG_TYPE_GIMMICK + 2), 
														eEVENT_36, eEVENT_36_CLEAR_BARRIER_2);

		// バリア更新処理
		StageFuncBase::setVisibleBarrierFromByEventFlag((eBG_TYPE_GIMMICK + 6), 
														eEVENT_36, eEVENT_36_CLEAR_BARRIER_3);

		// 透明な床追加
		CollisionManager::GetInstance()->EntryEarthCollision(sModelMap.GetModelHandle(),
															 (eBG_TYPE_GIMMICK + 3));
		
		// 骨・床
		// 骨・壁
		StageFuncBase::setVisibleEarthGimmickCollision((eBG_TYPE_GIMMICK + 4), TRUE);
		StageFuncBase::setVisibleWallGimmickCollision( (eBG_TYPE_GIMMICK + 5), TRUE);

		// 網Ｂの追加(ショボーンスイッチを塞ぐ壁)
		//		StageFuncBase::setVisibleWallGimmickCollision( (eBG_TYPE_GIMMICK + 6), TRUE);

		// 透明な床・ボス前追加
		StageFuncBase::setVisibleEarthGimmickCollision( (eBG_TYPE_GIMMICK + 7), FALSE);

		// 透明な壁・ボス前追加
		StageFuncBase::setVisibleWallGimmickCollision( (eBG_TYPE_GIMMICK + 8), FALSE);

		// 浮島の処理
		StageFuncBase::setVisibleWallGimmickCollision( (eBG_TYPE_GIMMICK + 9), TRUE);

		// 毒霧の対応
		sModelMap.EntryBarrier((eBG_TYPE_GIMMICK + 10));
		break;


		// ボス戦のオブジェクト
	case StageFunc036::eOBJECT_PATTERN_BOSS:
		// 透明な床追加
		StageFuncBase::setVisibleEarthGimmickCollision((eBG_TYPE_GIMMICK + 3), FALSE);

		// 骨・床
		// 骨・壁
		StageFuncBase::setVisibleEarthGimmickCollision((eBG_TYPE_GIMMICK + 4), FALSE);
		StageFuncBase::setVisibleWallGimmickCollision( (eBG_TYPE_GIMMICK + 5), FALSE);

		// 網Ｂの追加(ショボーンスイッチを塞ぐ壁)
		//		StageFuncBase::setVisibleWallGimmickCollision( (eBG_TYPE_GIMMICK + 6), FALSE);

		// 透明な床・ボス前追加
		StageFuncBase::setVisibleEarthGimmickCollision( (eBG_TYPE_GIMMICK + 7), TRUE);

		// 透明な壁・ボス前追加
		StageFuncBase::setVisibleWallGimmickCollision( (eBG_TYPE_GIMMICK + 8), TRUE);

		// 浮島の処理
		StageFuncBase::setVisibleWallGimmickCollision( (eBG_TYPE_GIMMICK + 9), FALSE);

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
StageFunc036::DestroyBuf(void)
{
	GimmickExtend_TrolleyEx::DestroyTrolleyExCollision();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc036::NotifyCreateEnemy(int enemyIndex)
{
	//	int model_handle = -1;

	/*
	// ライトありの場合
	if (IS_END_EVT(eEVENT_36, eEVENT_36_DIRECTION_LIGHT_ON)) {
		model_handle = sppEnemy[enemyIndex]->GetModelHandle();
		MV1SetMaterialTypeAll(model_handle, DX_MATERIAL_TYPE_TOON_2);
		MV1SetAmbColorScale(model_handle, GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
	}
	// ライトなしの場合
	else {
		model_handle = sppEnemy[enemyIndex]->GetModelHandle();
		MV1SetMaterialTypeAll(model_handle, DX_MATERIAL_TYPE_NORMAL);
		MV1SetAmbColorScale(model_handle, GetColorF(-0.4f, -0.4f, -0.1f, 0.0f) );
	}
	*/

	/*
	// クリスタルのリストを取得する
	std::vector<ObjectBase *> crystal_list;
	int total_crystal = 0;
	getCrystalList(crystal_list, total_crystal);

	if (crystal_list.size() >= kCHANGE_TOON_LIGHT_NUM) {
		model_handle = sppEnemy[enemyIndex]->GetModelHandle();
		MV1SetMaterialTypeAll(model_handle, DX_MATERIAL_TYPE_TOON_2);
		MV1SetAmbColorScale(model_handle, GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else {
		model_handle = sppEnemy[enemyIndex]->GetModelHandle();
		MV1SetMaterialTypeAll(model_handle, DX_MATERIAL_TYPE_TOON);
		MV1SetAmbColorScale(model_handle, GetColorF(-0.4f, -0.4f, -0.1f, 0.0f) );
	}
	*/

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc036::ProcessShowMapBefore(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	INT8 coll_index = 0;
	int  on_frame_index = 0;
	coll_index = p_player->GetOnCollisionIndex();

	on_frame_index = CollisionManager::GetInstance()->GetEarthModelFrameIndex(coll_index);

	//	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_EARTH, FALSE);
	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_EARTH, TRUE);
	sModelMap.SetAlpha(eBG_TYPE_EARTH, 0.1f);
	
	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_2F, FALSE);
	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_3F, FALSE);
	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_4F, FALSE);
	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_5F, FALSE);

	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_REVERSE, FALSE);
	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_AROUND_OUT, FALSE);

	switch (on_frame_index) {
	case eBG_TYPE_GIMMICK_5F:
		MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_5F, TRUE);
		break;
	case eBG_TYPE_GIMMICK_4F:
		MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_4F, TRUE);
		break;
	case eBG_TYPE_GIMMICK_3F:
		MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_3F, TRUE);
		break;
	case eBG_TYPE_GIMMICK_2F:
		MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_2F, TRUE);
		break;
	default:
		sModelMap.SetAlpha(eBG_TYPE_EARTH, 1.0f);
		break;
	};
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc036::ProcessShowMapAfter(void)
{
	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_EARTH, TRUE);
	sModelMap.SetAlpha(eBG_TYPE_EARTH, 1.0f);

	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_2F, TRUE);
	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_3F, TRUE);
	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_4F, TRUE);
	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_5F, TRUE);

	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_REVERSE,	TRUE);
	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_AROUND_OUT, TRUE);

	if (IS_END_EVT(eEVENT_36, eEVENT_36_DIRECTION_LIGHT_ON)) {
		/* ライトの切り替え */
		SetPatternLight(eLIGHT_PATTEN_FLAME);
	}
	else {
		/* ライトの切り替え */
		SetPatternLight(eLIGHT_PATTEN_NAVI_LIGHT);
	}
}

/*=====================================*
 * private
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc036::destroy(void)
{
	/*
	if (mEffHndTornade != -1) {
		EffectManager::GetInstance()->Stop(mEffHndTornade, 0.0f);
		mEffHndTornade = -1;
	}
	*/

	STOP_EFFECT(mEffHndTornade);
	STOP_EFFECT(mEffHndFlame);
}

/**** end of file ****/


