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
StageFunc040::StageFunc040()
{
	mEffHndTornade = -1;
	mEffHndCrystal = -1;
	mEffHndAura = -1;
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

	// 必要なモデルのロード
	ResourceManager::GetInstance()->LoadCharaResource(NAME_Usaida);

	if (IS_END_EVT(eEVENT_40, eEVENT_40_WAKEUP_BABEL) == false) {
		info.mFileIndex = eRES_STG_HARAPPA_WHITE_PLACE;
		info.mIsCollision = true;
		sSubStageInfoList.push_back(info);

		info.mFileIndex = eRES_STG_DUMMY;
		info.mIsCollision = true;
		sSubStageInfoList.push_back(info);
	}
	else {
		info.mFileIndex = eRES_STG_DUMMY;
		info.mIsCollision = true;
		sSubStageInfoList.push_back(info);

		info.mFileIndex = eRES_STG_HARAPPA_MAIN;
		info.mIsCollision = true;
		sSubStageInfoList.push_back(info);
	}
	
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
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_SKYDOME_HARAPPA);
	int graph_handle = ResourceLayout::GetInstance()->GetHandle(RES_LYT_SKYDOME_HARAPPA);
	MV1SetTextureGraphHandle(sModelXfile[eMD_SKYDOME].GetModelHandle(), 0,graph_handle, FALSE);

	// ステージバッファ
	STOP_EFFECT(mEffHndTornade);
	STOP_EFFECT(mEffHndCrystal);
	STOP_EFFECT(mEffHndAura);
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
		rPhase = SceneMainGame::ePHASE_EVENT_BEFORE;
	}
			
	if (IS_END_EVT(eEVENT_40, eEVENT_40_DISAPPER_TORNADE) == false) {
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
		
		// モーフ数
		SetTornadeMorphNumDefault();
	}

	if (IS_END_EVT(eEVENT_40, eEVENT_40_WAKEUP_BABEL) == false) {
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
	}

	ObjectBase* p_enemy = search1stFindObjectKind(eOBJECT_KIND_GETDOWN);
	APP_ASSERT( (p_enemy != NULL),
				"Not Founc search1stFindObjectKind(eOBJECT_KIND_GETDOWN) \n");
	
	if ((IS_END_EVT(eEVENT_40, eEVENT_40_TORNADE_EVENT) == true) &&
		(IS_END_EVT(eEVENT_40, eEVENT_40_BATTLE_USIDERMAN) == false) ) {
		p_enemy->SetVisible(true);
		spNpc = CharaNpc::RecreateNpcInstance(1, spNpc, sNpcNum, sStageIndex, spBGInfo->mScale);
		//		p_enemy->SetAlpha(1.0f);
	}
	else {
		p_enemy->SetVisible(false);
		spNpc = CharaNpc::RecreateNpcInstance(0, spNpc, sNpcNum, sStageIndex, spBGInfo->mScale);
		//		p_enemy->SetAlpha(0.0f);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc040::Update(int counter)
{
	// エフェクト変動テスト
	struct tTornadeInfo {
		const char* mStrParamName;
		float		mMaxValue;
		float		mMinValue;
	};

	// コンパスの表示
	TotalInfo::GetInstance()->UpdateCompass();

	int effindex = mEffHndTornade;

	/**** うさいだーまん戦の間 ****/
	if ( (TotalInfo::GetInstance()->GetEventFlagEnable(eEVENT_40, eEVENT_40_BATTLE_USIDERMAN) == true ) &&
		 (TotalInfo::GetInstance()->GetEventFlagEnable(eEVENT_40, eEVENT_40_DISAPPER_TORNADE) == false ) ) {

		if (mTornadeBattleCount > 0) {

			// カウンタ描画のハンドラを設定する
			GraphicManager::GetInstance()->EntryDrawFontFunc(DrawFontFunc_BattleUsiderManCounter);

			ObjectBase* p_enemy = search1stFindObjectKind(eOBJECT_KIND_GETDOWN);
			APP_ASSERT( (p_enemy != NULL),
						"Not Founc search1stFindObjectKind(eOBJECT_KIND_GETDOWN) \n");

			VECTOR pos;	float rot;
			getPlayerPosRotFromBGModel(&pos, &rot, 3);
			p_enemy->Position() = pos;

			p_enemy->Position().x += sin( (float)mTornadeBattleCount * 0.002f) * 1000;
			p_enemy->Position().z += cos( (float)mTornadeBattleCount * 0.002f) * 1000;
		
			mTornadeBattleCount --;

			// 時間切れの場合
			if (mTornadeBattleCount == 0) {
				ActionTaskManager::tTaskInfo param;
				param.mTaskType = ActionTaskManager::eTASK_EVENT;
				param.mTaskValueA = eEVENT_40;
				param.mTaskValueB = eEVENT_40_FAILED_USIDERMAN;
				ActionTaskManager::GetInstance()->NotifyAction(&param);
			}
			else if (mTornadeBattleCount == (19 * 60) ) {
				SceneMainGame::StartNaviTalk(strNaviTalk_BattleUsidermanA);
			}
			else if (mTornadeBattleCount == (14 * 60) ) {
				SceneMainGame::StartNaviTalk(strNaviTalk_BattleUsidermanB);
			}
		}
	}

	/**** 竜巻が再生している間 ****/
	if ((IS_END_EVT(eEVENT_40, eEVENT_40_DISAPPER_TORNADE) == false) &&
		( (effindex != -1) && (EffectManager::GetInstance()->IsPlay(effindex) ) ) ) {

		//			const float morph_num_min	= 0.001f;
		//			const float morph_num_max	= 0.100f;

		tTornadeInfo sTornadeInfo[] = {
			{"morph_MinWidth",	0.0500f,	-0.55f},
			{"morph_MinWidth",	0.0500f,	-0.28f},
			{"morph_MaxWidth",	0.0900f,	0.200f},
			{"morph_Height",	1.0000f,	0.135f},
			{"morph_Si",		1.0000f,	0.700f},
			{"morph_Si_p",		1.0000f,	0.700f},
			{"morph_Rot_m",		1.4000f,	0.800f},
		};
		const BYTE tornade_info_num = (sizeof(sTornadeInfo) / sizeof(tTornadeInfo) );

		tTornadeInfo* p_info = NULL;
		float diff, param_value;
		const float shape_move_speed = 0.002f;
		bool is_down_move = false;


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
		//		EffectManager::GetInstance()->SetFloat(effindex, "morph_Num",		0.02f);

		// 弓を竜巻から弾く処理
		float dist;
		VECTOR pos_a, pos_b, pos_c;
		pos_a = EffectManager::GetInstance()->Position(effindex);
		pos_a.y = 0.0f;
		pos_c = sPlayer.Position();
		pos_c.y = 0.0f;

		VECTOR vec;
		VECTOR ch_vec = VCross( VNorm(VSub(pos_c, pos_a)), UP_VEC);
		VECTOR crossX;
		float  arrow_speed = 0.0f;

		for (int iI = 0; iI < kPLAYER_ARROW_NUM; iI ++) {
			ObjectWeapon_Arrow* p_arrow = sPlayer.GetArrowPtr(iI);
			if (p_arrow->State() == ObjectWeapon_Arrow::eWEAPON_ARROW_STATE_SHOOTING) {

				pos_b = p_arrow->Position();
				pos_b.y = 0.0f;

				vec = VSub(pos_b, pos_a);
				dist = VSize(vec);
				if (dist < 20000.0f) {
					float dot = VDot(VNorm(vec), ch_vec);
					crossX = VNorm(VCross(VNorm(vec), UP_VEC));
					arrow_speed = (2.0f * mTornadeShapeRate) + 0.01f;
					
					if (dot > 0.0f) {
						p_arrow->AddArrowMoveVec((VECTOR&)VScale(VNorm(crossX), arrow_speed ) );
					}
					else {
						p_arrow->AddArrowMoveVec((VECTOR&) VScale(VNorm(crossX), -arrow_speed ) );
					}
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

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc040::SetTornadeMorphNumDefault(void)
{
	// モーフ数
	EffectManager::GetInstance()->SetFloat(mEffHndTornade, "morph_Num", 0.02f);
	mIsChangeShapeTornade = false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc040::SetTornadeMorphNumEvent(void)
{
	// モーフ数
	EffectManager::GetInstance()->SetFloat(mEffHndTornade, "morph_Num", 0.05f);
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


