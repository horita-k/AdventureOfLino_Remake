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
#include "StageFunc043.h"
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
StageFunc043::StageFunc043()
{
	mTransH = 0.0f;
	mTransV = 0.0f;

	mStageState = eSTAGE_STATE_SHOW_WALL;
	mFadeWallAlpha = 1.0f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
StageFunc043::~StageFunc043()
{
	destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc043::SetupBeforeLoad(void)
{
	// ステージバッファ
	mEffHndAura	= -1;
	mEffHndCrystal	= -1;

	ResourceModel::GetInstance()->LoadGimmickResource(RES_MDL_HOLE);

	/**** ベースクラス関数を自クラス処理後にコール ****/
	StageFuncBase::SetupBeforeLoad();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc043::SetupAfterLoad(int& rPhase)
{
	int model_handle = sModelMap.GetModelHandle();

	/**** ベースクラス関数を自クラス処理前にコール ****/
	StageFuncBase::SetupAfterLoad(rPhase);

	sModelXfile[eMD_SKYDOME	].Scale() = ZERO_VEC;

	SetPatternLight(0);

	SetPatternObject(0);

	StartFadeInWall();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc043::Update(int counter)
{
	/*=====================================*
	 * コンパスの表示
	 *=====================================*/
	TotalInfo::GetInstance()->UpdateCompass();

	if (SceneMainGame::GetIsBattle() == true) {
		mTransV += -0.010f;
	}
	MV1SetFrameTextureAddressTransform(sModelMap.GetModelHandle(),
									   eBG_TYPE_ALPHA_WALL,
									   mTransH, mTransV,
									   1.0f, 1.0f,
									   0.0f, 0.0f, 0.0f);

	MV1SetFrameTextureAddressTransform(sModelMap.GetModelHandle(),
									   eBG_TYPE_ALPHA_EARTH,
									   mTransH, mTransV,
									   1.0f, 1.0f,
									   0.0f, 0.0f, 0.0f);

	// 壁の表示状態更新
	updateStageState();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc043::SetPatternLight(int pattern)
{

	switch (pattern) {
	case 0:
	case 1:
		{
			// ディレクションライト有効
			GraphicManager::GetInstance()->SetEnableDirectionLight(true);

			RenderBloom::tBloomBrightParam param ={0};
			RenderBloom::SetBloomType(RenderBloom::eBLOOM_TYPE_BRIGHT_GREATER);
			RenderBloom::SetBloomParam(12, 75, 100);
			param.mClipParam = 120;
			param.mPixelColorR = 0;	param.mPixelColorG = 0;	param.mPixelColorB = 0;
			RenderBloom::BloomBrightParam() = param;
		}
		break;

		/*
	case 1:
		{
			// ディレクションライト無効
			GraphicManager::GetInstance()->SetEnableDirectionLight(false);

			GraphicManager::GetInstance()->SetEnablePLight(TRUE);
			GraphicManager::GetInstance()->SetPLightPosition(&VGet(0.000f, -252000.000f, 0.000f) );
			GraphicManager::GetInstance()->SetPLightRangeAtten(309000.000f, 0.13f, (float)3E-07, 0);
			GraphicManager::GetInstance()->SetPLightDiffuse(0.419610f, 0.678430f, 0.713730f, 0.000000f);
			GraphicManager::GetInstance()->SetPLightSpecular(0.000000f, 0.000000f, 0.000000f, 0.000000f);
			GraphicManager::GetInstance()->SetPLightAmbient(0.247060f, 0.427450f, 0.247060f, 0.000000f);
		}
		break;
		*/

	default:
		break;
	};

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc043::SetPatternObject(int pattern)
{
	switch (pattern) {
	case 0:
		CollisionManager::GetInstance()->EntryEarthCollision(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 0));
		sModelMap.SetVisible(true, (eBG_TYPE_GIMMICK+0));

		sModelMap.SetVisible(true, eBG_TYPE_ALPHA_EARTH);
		CollisionManager::GetInstance()->EntryEarthCollision(sModelMap.GetModelHandle(), eBG_TYPE_ALPHA_EARTH);
		sModelMap.SetVisible(true, eBG_TYPE_ALPHA_WALL);
		CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_ALPHA_WALL);

		//		sModelMap.SetVisible(false, (eBG_TYPE_WATER));
		sModelMap.SetAlpha(eBG_TYPE_WATER, (1.0f - mFadeWallAlpha));

		break;

	case 1:
		/*
		sModelMap.SetVisible(false, eBG_TYPE_ALPHA_EARTH);
		CollisionManager::GetInstance()->RemoveEarthCollision(sModelMap.GetModelHandle(), eBG_TYPE_ALPHA_EARTH);
		sModelMap.SetVisible(false, eBG_TYPE_ALPHA_WALL);
		CollisionManager::GetInstance()->RemoveWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_ALPHA_WALL);
		*/

		//		sModelMap.SetVisible(true, (eBG_TYPE_WATER));
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
StageFunc043::StartFadeInWall(void)
{
	if ((mStageState == eSTAGE_STATE_HIDE_WALL) ||
		(mStageState == eSTAGE_STATE_FADEOUT_WALL)) {
		mStageState = eSTAGE_STATE_FADEIN_WALL;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc043::StartFadeOutWall(void)
{
	if ((mStageState == eSTAGE_STATE_SHOW_WALL) ||
		(mStageState == eSTAGE_STATE_FADEIN_WALL)) {
		mStageState = eSTAGE_STATE_FADEOUT_WALL;
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
StageFunc043::destroy(void)
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

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc043::updateStageState(void)
{
	switch (mStageState) {
	case eSTAGE_STATE_SHOW_WALL:
	case eSTAGE_STATE_HIDE_WALL:
		break;

	case eSTAGE_STATE_FADEOUT_WALL:

		mFadeWallAlpha -= 0.02f;
		if (mFadeWallAlpha < 0.0f) {
			mFadeWallAlpha = 0.0f;
			mStageState = eSTAGE_STATE_HIDE_WALL;

			CollisionManager::GetInstance()->RemoveEarthCollision(sModelMap.GetModelHandle(),
																 eBG_TYPE_ALPHA_EARTH);
			CollisionManager::GetInstance()->RemoveWallCollision(sModelMap.GetModelHandle(),
																 eBG_TYPE_ALPHA_WALL);
		}

		sModelMap.SetAlpha(eBG_TYPE_ALPHA_EARTH, mFadeWallAlpha);
		sModelMap.SetAlpha(eBG_TYPE_ALPHA_WALL, mFadeWallAlpha);
		sModelMap.SetAlpha(eBG_TYPE_WATER, (1.0f - mFadeWallAlpha));

		break;

	case eSTAGE_STATE_FADEIN_WALL:

		mFadeWallAlpha += 0.02f;
		if (mFadeWallAlpha > 1.0f) {
			mFadeWallAlpha = 1.0f;
			mStageState = eSTAGE_STATE_SHOW_WALL;
			CollisionManager::GetInstance()->EntryEarthCollision(sModelMap.GetModelHandle(),
																 eBG_TYPE_ALPHA_EARTH);
			CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(),
																eBG_TYPE_ALPHA_WALL);
		}

		sModelMap.SetAlpha(eBG_TYPE_ALPHA_EARTH, mFadeWallAlpha);
		sModelMap.SetAlpha(eBG_TYPE_ALPHA_WALL, mFadeWallAlpha);
		sModelMap.SetAlpha(eBG_TYPE_WATER, (1.0f - mFadeWallAlpha));

		break;
	default:
		break;
	};
}


/**** end of file ****/


