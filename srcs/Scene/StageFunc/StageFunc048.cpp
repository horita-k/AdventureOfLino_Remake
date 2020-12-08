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
#include "StageFunc048.h"
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
StageFunc048::StageFunc048()
{
	// ステージバッファ
	mEffHndSky					= -1;
	mEffHndGravityAura			= -1;

	mIsSlapBattleWin			= FALSE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
StageFunc048::~StageFunc048()
{
	destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc048::SetupBeforeLoad(void)
{
	tStageInfo info = {0};

	info.mFileIndex = eRES_STG_HARAPPA_SUB;
	info.mIsCollision = true;
	sSubStageInfoList.push_back(info);

	/**** ベースクラス関数を自クラス処理後にコール ****/
	StageFuncBase::SetupBeforeLoad();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc048::SetupAfterLoad(int& rPhase)
{
	int model_handle = sModelMap.GetModelHandle();

	/**** ベースクラス関数を自クラス処理前にコール ****/
	StageFuncBase::SetupAfterLoad(rPhase);

	// スカイドームを変更
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_SKYDOME_HARAPPA);
	int graph_handle = ResourceLayout::GetInstance()->GetHandle(RES_LYT_SKYDOME_HARAPPA);
	MV1SetTextureGraphHandle(sModelXfile[eMD_SKYDOME].GetModelHandle(), 0,graph_handle, FALSE);

	// ステージバッファ


	if (mEffHndSky == -1) {
		mEffHndSky = EffectManager::GetInstance()->Play(eEF_SKY, &sPlayer.Position(), 0.1f, 0.0f);
			
		EffectManager::GetInstance()->Scale(mEffHndSky) = VGet(10.0f, 10.0f, 10.0f);
	}

	if (mEffHndGravityAura == -1) {
		VECTOR eff_pos;	float rot;
		getPlayerPosRotFromBGModel(&eff_pos, &rot, 12);

		mEffHndGravityAura = EffectManager::GetInstance()->Play(eEF_AURA,&eff_pos,
																&VGet(300.0f, 100.0f, 300.0f));
	}

	kGRAVITY_SPEED = -0.98f * 2.0f;

	// イベントの設定
	unsigned long event_kind;
	unsigned long event_flag;
	event_kind = eEVENT_48;
	event_flag = eEVENT_48_APPEAR_TOWER;
	if (TotalInfo::GetInstance()->GetEventFlagEnable(event_kind, event_flag) == false) {
		sTaskInfo.mTaskType		= ActionTaskManager::eTASK_EVENT;
		sTaskInfo.mTaskValueA	= (BYTE)event_kind;
		sTaskInfo.mTaskValueB	= (BYTE)event_flag;
		rPhase = SceneMainGame::ePHASE_EVENT_BEFORE;
	}

	if ((IS_END_EVT(eEVENT_38, eEVENT_38_TALK_YODEL) == true) &&
		(IS_END_EVT(eEVENT_48, eEVENT_48_APPEAR_SKYSHIP) == false) ) {
		sTaskInfo.mTaskType		= ActionTaskManager::eTASK_EVENT;
		sTaskInfo.mTaskValueA	= eEVENT_48;
		sTaskInfo.mTaskValueB	= eEVENT_48_APPEAR_SKYSHIP;
		rPhase = SceneMainGame::ePHASE_EVENT_BEFORE;

		// ステージ48, パターンのライトを設定
		SetPatternLight(2);
	}
	else {
		// ステージ48, パターンのライトを設定
		SetPatternLight(1);
	}


}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc048::Update(int counter)
{
	// コンパスの表示
	TotalInfo::GetInstance()->UpdateCompass();

	int effindex = mEffHndSky;
	if (effindex != -1) {
		EffectManager::GetInstance()->Position(effindex).y = sPlayer.Position().y - 3000.0f;
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc048::SetPatternLight(int pattern)
{
	RenderBloom::tBloomBrightParam param ={0};

	switch(pattern) {
	case 0:
		/* nop */
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

		// 塔の上
	case 2:
		{
			// emissive
			sModelMap.SetEnableEmissive(false);

			// fog
			GraphicManager::GetInstance()->EnableFogPattern(255, 255, 255, 10.0f, 180000.0f);

			// bloom
			RenderBloom::SetBloomType(RenderBloom::eBLOOM_TYPE_BRIGHT_GREATER);
			RenderBloom::SetBloomParam(10, 25, 1000);
			param.mClipParam = 190;
			param.mPixelColorR = 0;	param.mPixelColorG = 0;	param.mPixelColorB = 0;
			RenderBloom::BloomBrightParam() = param;
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
StageFunc048::destroy(void)
{
	STOP_EFFECT(mEffHndSky);
	STOP_EFFECT(mEffHndGravityAura);

	return;
}

/**** end of file ****/


