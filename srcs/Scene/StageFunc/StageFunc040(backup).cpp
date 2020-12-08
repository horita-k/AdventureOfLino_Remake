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
#include "StageFunc040.h"
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
#include "../ExternDefine.h"		//<! Extern 宣言は全てここに集約する

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
StageFunc040::StageFunc040()
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
StageFunc040::~StageFunc040()
{
	destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc040::SetupBeforeLoad(void)
{
	tStageInfo info = {0};

	//<! デバッグ用！、ハラッパのイベントフラグの有効化！
	/*
	  TotalInfo::GetInstance()->SetEventFlag(eEVENT_40, eEVENT_40_WELCOME_WILDERNESS);
	  TotalInfo::GetInstance()->SetEventFlag(eEVENT_40, eEVENT_40_WELCOME_WILDERNESS_2);
	  TotalInfo::GetInstance()->SetEventFlag(eEVENT_40, eEVENT_40_DISAPPER_TORNADE);
	  TotalInfo::GetInstance()->SetEventFlag(eEVENT_40, eEVENT_40_TORNADE_EVENT);
	  TotalInfo::GetInstance()->SetEventFlag(eEVENT_40, eEVENT_40_BATTLE_USIDERMAN);
	  TotalInfo::GetInstance()->SetEventFlag(eEVENT_40, eEVENT_40_WAKEUP_BABEL);

	  if (sStagePlaceIndex == 0) {
	  sStagePlaceIndex = 9;
	  //		sStagePlaceIndex = 1;
	  }
	*/

	info.mFileIndex = eRES_STG_HARAPPA_WHITE_PLACE;
	info.mIsCollision = true;
	sSubStageInfoList.push_back(info);

	// 必要なモデルのロード
	ResourceManager::GetInstance()->LoadCharaResource(NAME_Usaida);
	
	/**** ベースクラス関数を自クラス処理後にコール ****/
	StageFuncBase::SetupBeforeLoad();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc040::SetupAfterLoad(int& rPhase)
{
	unsigned long event_kind;
	unsigned long event_flag;
	int model_handle = sModelMap.GetModelHandle();

	/**** ベースクラス関数を自クラス処理前にコール ****/
	StageFuncBase::SetupAfterLoad(rPhase);

	// スカイドームを変更
	{
		ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_SKYDOME_HARAPPA);
		int graph_handle = ResourceLayout::GetInstance()->GetHandle(RES_LYT_SKYDOME_HARAPPA);
		MV1SetTextureGraphHandle(sModelXfile[eMD_SKYDOME].GetModelHandle(), 0,graph_handle, FALSE);

		// ステージバッファ
		mEffHndTornade				= -1;
		//		mEffHndSky					= -1;
		mEffHndCrystal				= -1;
		mEffHndAura				= -1;
		mDistPlayerToTornadeRate	= 0;
		mTornadeShapeRate			= 1.0f;
		mIsChangeShapeTornade		= false;
		mTornadeBattleCount		= 0;

		event_kind = eEVENT_40;
		event_flag = eEVENT_40_WELCOME_WILDERNESS;
		if (TotalInfo::GetInstance()->GetEventFlagEnable(event_kind, event_flag) == false) {
			sTaskInfo.mTaskType		= ActionTaskManager::eTASK_EVENT;
			sTaskInfo.mTaskValueA	= (BYTE)event_kind;
			sTaskInfo.mTaskValueB	= (BYTE)event_flag;
			rPhase = SceneMergeTest::ePHASE_EVENT_BEFORE;
		}
			
#if 1 // ※消すな！　デバッグの為、一時的に無効！
		// 竜巻エフェクトの発生
		VECTOR eff_pos; float tmp_rot;
		getPlayerPosRotFromBGModel(&eff_pos, &tmp_rot, 2);

		int effindex = EffectManager::GetInstance()->Play(eEF_TORNADE, &eff_pos, 0.1f, 0.0f);
		mEffHndTornade = effindex;

		FLOAT4 smoke_color;
		smoke_color.x = 5.0f;
		smoke_color.y = 4.0f;
		smoke_color.z = 2.0f;
		smoke_color.w = 0.8f;//0.7
		EffectManager::GetInstance()->SetVec4(effindex, "SmokeColor", &smoke_color);
		EffectManager::GetInstance()->SetFloat(effindex, "morph_Anm", 0.06f);
		EffectManager::GetInstance()->SetFloat(effindex, "morph_Height", 2.0f);
		EffectManager::GetInstance()->SetFloat(effindex, "morph_H", 0.0f);
		EffectManager::GetInstance()->SetFloat(effindex, "morph_S", 0.1f);
		EffectManager::GetInstance()->SetFloat(effindex, "morph_B", 0.0f);
		EffectManager::GetInstance()->SetFloat(effindex, "morph_g_Sip", 0.005f);
		EffectManager::GetInstance()->SetFloat(effindex, "morph_Rot_m", 2.0f);
		
		// クリスタル出現の演出
		ObjectBase* p_object =search1stFindObjectKind(eOBJECT_KIND_CRYSTAL);
		APP_ASSERT( (p_object != NULL), 
					"Not Found object of eOBJECT_KIND_CRYSTAL \n");
		  processAppearCrystal(2,				// stagePlaceIndex
							   p_object,		// pCrystalObject
							   1,				// crystalColor : 1 : Yellow
							   FALSE,			// isEfk
							   &mEffHndAura,
							   &mEffHndCrystal);

#endif // 0

		if (sStagePlaceIndex == 1) {
			SetPatternLight(1);
		}
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc040::Update(int counter)
{
	// コンパスの表示
	TotalInfo::GetInstance()->UpdateCompass();

	int effindex = mEffHndTornade;

	/**** うさいだーまん戦の間 ****/
	if ( (TotalInfo::GetInstance()->GetEventFlagEnable(eEVENT_40, eEVENT_40_BATTLE_USIDERMAN) == true ) &&
		 (TotalInfo::GetInstance()->GetEventFlagEnable(eEVENT_40, eEVENT_40_DISAPPER_TORNADE) == false ) ) {

		// カウンタ描画のハンドラを設定する
		GraphicManager::GetInstance()->EntryDrawFontFunc(DrawFontFunc_BattleUsiderManCounter);

		ObjectBase* p_enemy = search1stFindObjectKind(eOBJECT_KIND_GETDOWN);
		APP_ASSERT( (p_enemy != NULL),
					"Not Founc search1stFindObjectKind(eOBJECT_KIND_GETDOWN) \n");

		VECTOR pos;	float rot;
		getPlayerPosRotFromBGModel(&pos, &rot, 3);
		p_enemy->Position() = pos;
		p_enemy->Position().x += sin( (float)mTornadeBattleCount * 0.07f) * 1000;
		p_enemy->Position().z += cos( (float)mTornadeBattleCount * 0.07f) * 1000;
		
		mTornadeBattleCount --;

		if (mTornadeBattleCount == 0) {
			//			sPlayer.ProcessDamage(kSYSTEM_DAMAGE_VALUE, true, NULL, (VECTOR *)&ZERO_VEC, false, eATTACK_TYPE_NORMAL); // 擬似的に敵を倒す
			mTornadeBattleCount = (20 * 60);
		} else if (mTornadeBattleCount == (19 * 60) ) {
			SceneMergeTest::StartNaviTalk(strNaviTalk_BattleUsidermanA);
		} else if (mTornadeBattleCount == (14 * 60) ) {
			SceneMergeTest::StartNaviTalk(strNaviTalk_BattleUsidermanB);
		}
	}

	/**** 竜巻が再生している間 ****/
	if (IS_END_EVT(eEVENT_40, eEVENT_40_DISAPPER_TORNADE) == false) {
		if( (effindex != -1) &&
			(EffectManager::GetInstance()->IsPlay(effindex) ) ) {

#if 0
			const float morph_dist_min = 2000.0f;
			const float morph_dist_max = 40000.0f;
			const float morph_num_min	= 0.02f;
			const float morph_num_max	= 0.20f;
			//			const float morph_num_min	= 0.001f;
			//			const float morph_num_max	= 0.100f;

			const float morph_dist_diff	= morph_dist_max - morph_dist_min;
			const float morph_num_diff	= morph_num_max - morph_num_min;
			float set_morph_num = 0;

			float dist_player_to_tornade;
			dist_player_to_tornade = VSize(VSub(EffectManager::GetInstance()->Position(effindex), 
												sPlayer.Position() ) );
			DWORD dist_player_to_tornade_rate = (DWORD)(dist_player_to_tornade / 8000);

			if ( mDistPlayerToTornadeRate != dist_player_to_tornade_rate) {
				mDistPlayerToTornadeRate = dist_player_to_tornade_rate;
				float dist_rate = ( (dist_player_to_tornade - morph_dist_min) / morph_dist_diff);
				set_morph_num = morph_num_min + (dist_rate * morph_num_diff);
				//			if (set_morph_num > 0.30f) {
				if (set_morph_num > 0.24f) {
					//				set_morph_num = 0.30f;
					set_morph_num = 0.24f;
				}
				EffectManager::GetInstance()->SetFloat(effindex, "morph_Num", set_morph_num);

				PRINT_CONSOLE("=> set_morph_num : %.2f \n", set_morph_num);

			}
//			EffectManager::GetInstance()->SetFloat(effindex, "morph_Num", 0.001f);

			// エフェクト変動テスト
			struct tTornadeInfo {
				const char* mStrParamName;
				float		mMaxValue;
				float		mMinValue;
			};
			tTornadeInfo sTornadeInfo[] = {
				/*
				 * デバッグ版
				 {"morph_MinWidth",	0.0500,	DBGTOOLVALUE00},
				 {"morph_MaxWidth",	0.0900,	DBGTOOLVALUE01},
				 {"morph_Height",	1.0000,	DBGTOOLVALUE02},
				 {"morph_Si",		1.0000,	DBGTOOLVALUE03},
				 {"morph_Si_p",		1.0000,	DBGTOOLVALUE04},
				 {"morph_Rot_m",		1.4000,	DBGTOOLVALUE05},
				*/
				{"morph_MinWidth",	0.0500f,	-0.55f},
				{"morph_MaxWidth",	0.0900f,	0.200f},
				{"morph_Height",	1.0000f,	0.250f},
				{"morph_Si",		1.0000f,	0.700f},
				{"morph_Si_p",		1.0000f,	0.700f},
				{"morph_Rot_m",		1.4000f,	0.800f},
			};
			const BYTE tornade_info_num = (sizeof(sTornadeInfo) / sizeof(tTornadeInfo) );

			tTornadeInfo* p_info = NULL;
			float diff, param_value;
			const float shape_move_speed = 0.002f;
			bool is_down_move = false;
#ifdef _DEBUG
			if (DBGTOOLVALUE07 > 0) {
				mIsChangeShapeTornade = true;
			}
#endif // _DEBUG

			if (mIsChangeShapeTornade == false) {
				mTornadeShapeRate += shape_move_speed;
				if (mTornadeShapeRate > 1.0f) mTornadeShapeRate = 1.0f;
			} else {
				mTornadeShapeRate += -shape_move_speed;
				if (mTornadeShapeRate < 0.0f) mTornadeShapeRate = 0.0f;
			}

			for (int iI = 0; iI < tornade_info_num; iI ++) {
				p_info = &sTornadeInfo[iI];
				diff = p_info->mMaxValue - p_info->mMinValue;
				param_value = p_info->mMinValue + (diff * mTornadeShapeRate);
				EffectManager::GetInstance()->SetFloat(effindex, p_info->mStrParamName,	param_value);
			}
#endif // 0
			/*
			EffectManager::GetInstance()->SetFloat(effindex, "morph_MinWidth",	DBGTOOLVALUE00);
			EffectManager::GetInstance()->SetFloat(effindex, "morph_MaxWidth",	DBGTOOLVALUE01);
			EffectManager::GetInstance()->SetFloat(effindex, "morph_Height",	DBGTOOLVALUE02);
			EffectManager::GetInstance()->SetFloat(effindex, "morph_Si",		DBGTOOLVALUE03);
			EffectManager::GetInstance()->SetFloat(effindex, "morph_Si_p",		DBGTOOLVALUE04);
			EffectManager::GetInstance()->SetFloat(effindex, "morph_Rot_m",		DBGTOOLVALUE05);
			EffectManager::GetInstance()->SetFloat(effindex, "morph_Num",		DBGTOOLVALUE06);
			*/

			EffectManager::GetInstance()->SetFloat(effindex, "morph_MinWidth",	0.05f);
			EffectManager::GetInstance()->SetFloat(effindex, "morph_MaxWidth",	0.09f);
			EffectManager::GetInstance()->SetFloat(effindex, "morph_Height",	1.0f);
			EffectManager::GetInstance()->SetFloat(effindex, "morph_Si",		1.0f);
			EffectManager::GetInstance()->SetFloat(effindex, "morph_Si_p",		1.0f);
			EffectManager::GetInstance()->SetFloat(effindex, "morph_Rot_m",		1.4f);
			EffectManager::GetInstance()->SetFloat(effindex, "morph_Num",		0.02f);
			

		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc040::SetPatternLight(int pattern)
{
	RenderBloom::tBloomBrightParam param ={0};

	switch(pattern) {
		// 通常時
	case 0:
		break;

		// クリスタル入手前の設定
	case 1:
		{
			VECTOR pos; float rot;
			GraphicManager::GetInstance()->SetEnableDirectionLight(TRUE);
			SetLightDifColor(GetColorF(0.364710f, 0.337250f, 0.427450f, 0.000000f) );
			SetLightSpcColor(GetColorF(0.098040f, 0.098040f, 0.098040f, 0.000000f) );
			SetLightAmbColor(GetColorF(0.215690f, 0.180390f, 0.176470f, 0.000000f) );
			GraphicManager::GetInstance()->SetEnablePLight(TRUE);
			getPlayerPosRotFromBGModel(&pos, &rot, 2);
			pos.y += 1088.0f;
			GraphicManager::GetInstance()->SetPLightPosition(&pos);
			/*
			GraphicManager::GetInstance()->SetPLightRangeAtten(5800.000f, 0.3, 4E-06, 1.2E-07);
			GraphicManager::GetInstance()->SetPLightDiffuse(0.149020f, 0.156860f, 0.000000f, 0.000000f);
			GraphicManager::GetInstance()->SetPLightSpecular(0.000000f, 1.000000f, 0.000000f, 0.000000f);
			GraphicManager::GetInstance()->SetPLightAmbient(0.000000f, 0.074510f, 0.435290f, 0.000000f);
			*/
			//			GraphicManager::GetInstance()->SetEnablePLight(TRUE);
			//			GraphicManager::GetInstance()->SetPLightPosition(&VGet(-4422.000f, 1000.000f, 36923.000f) );
			GraphicManager::GetInstance()->SetPLightRangeAtten(5800.000f, 0.3f, (float)4E-06, (float)1.2E-07);
			GraphicManager::GetInstance()->SetPLightDiffuse(0.580390f, 0.270590f, 0.000000f, 0.000000f);
			GraphicManager::GetInstance()->SetPLightSpecular(1.000000f, 1.000000f, 0.000000f, 0.000000f);
			GraphicManager::GetInstance()->SetPLightAmbient(0.164710f, 0.000000f, 0.000000f, 0.000000f);
		}		
		
		break;

	default:
		break;
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
StageFunc040::destroy(void)
{
	return;
}

/**** end of file ****/


