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
#include "StageFunc028.h"
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
StageFunc028::StageFunc028()
{
	mEffHndCrystal	= -1;
	mEffHndAura		= -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
StageFunc028::~StageFunc028()
{
	destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc028::SetupBeforeLoad(void)
{
	/**** ベースクラス関数を自クラス処理後にコール ****/
	StageFuncBase::SetupBeforeLoad();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc028::SetupAfterLoad(int& rPhase)
{
	/**** ベースクラス関数を自クラス処理前にコール ****/
	StageFuncBase::SetupAfterLoad(rPhase);

	// フォグの切り替え
	if (IS_END_EVT(eEVENT_28, eEVENT_28_BTL_TREASURE_A)) {
		GraphicManager::tFogParam fogParam;
		fogParam.mIsEnable = TRUE;
		fogParam.mIsActive = TRUE;
		fogParam.mColR = 0;
		fogParam.mColG = 0;
		fogParam.mColB = 0;
		fogParam.mNear = 100.0f;
		fogParam.mFar = 8000.0f;
		GraphicManager::GetInstance()->ChangeFogParam(1.0f, &fogParam);
		GraphicManager::GetInstance()->BackupFogParam();
	}
	else {
		GraphicManager::GetInstance()->EnableFogPattern(200, 100, 150, 100.0f, 24000.0f);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc028::Update(int counter)
{
	TotalInfo::GetInstance()->UpdateCompass();
}

/*=====================================*
 * private
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc028::destroy(void)
{
	if (mEffHndCrystal != -1) {
		EffectManager::GetInstance()->Stop(mEffHndCrystal, 0.0f);
		mEffHndCrystal = -1;
	}
	if (mEffHndAura != -1) {
		EffectManager::GetInstance()->Stop(mEffHndAura, 0.0f);
		mEffHndAura = -1;
	}
}

/**** end of file ****/


