/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 画像ベースクラス
 * @author SPATZ.
 * @data   2012/02/18 17:56:36
 */
//---------------------------------------------------
#include "DxLib.h"

#include "AppLib/Basic/Basic.h"
#include "CameraManager.h"

#include "ShadowMap.h"

#include "DataDefine/ItemDef.h"

/*static*/int				ShadowMap::mIsActive				= TRUE;
/*static*/BYTE				ShadowMap::mShadowMapType			= 0;
/*static*/CALLBACK_BASIC	ShadowMap::mBeforeCreateCallback	= Basic::DummyCallback;
/*static*/CALLBACK_BASIC	ShadowMap::mAfterCreateCallback		= Basic::DummyCallback;

static int	sSmHandle	= -1;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ShadowMap::Create(void)
{
	if (mIsActive == FALSE) return;

	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ShadowMap::Setup(void)
{
	if (mIsActive == FALSE) return;

	sSmHandle = MakeShadowMap( 4096, 4096 );

	SetShadowMapLightDirection( sSmHandle, VGet( 0.5f, -0.5f, 0.5f ) ) ;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ShadowMap::Update(void)
{
	if (mIsActive == FALSE) return;
	if (mShadowMapType == eSHADOWMAP_TYPE_NONE) return;

	VECTOR min_pos, max_pos;
	VECTOR cam_tar = CameraManager::GetInstance()->Target();

	min_pos = VAdd(cam_tar, VGet( -1000.0f, -1000.0f, -1000.0f ) );
	max_pos = VAdd(cam_tar, VGet( 1000.0f, 1000.0f, 1000.0f ) );

	SetShadowMapDrawArea( sSmHandle, min_pos, max_pos);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ShadowMap::Destroy(void)
{
	// シャドウマップの削除
	DeleteShadowMap( sSmHandle );
	sSmHandle = -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ShadowMap::CreateShadowMap(std::vector<ModelBase *>& rList)
{
	if (mIsActive == FALSE) return;
	if (mShadowMapType == eSHADOWMAP_TYPE_NONE) return;

	// シャドウマップ生成前コールバック関数
	mBeforeCreateCallback(NULL);

	// シャドウマップへの描画の準備
	ShadowMap_DrawSetup( sSmHandle );	

	// モデルの書き込み開始
	std::vector<ModelBase *>::iterator ite;
	for (ite = rList.begin(); ite != rList.end(); ite ++) {
		(*ite)->Draw();
	}

	// シャドウマップへの描画を終了
	ShadowMap_DrawEnd();

	// シャドウマップ生成後コールバック関数
	mAfterCreateCallback(NULL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ShadowMap::RenderBegin(void)
{
	if (mIsActive == FALSE) return;
	if (mShadowMapType == eSHADOWMAP_TYPE_NONE) return;

	// 描画に使用するシャドウマップを設定
	SetUseShadowMap( 0, sSmHandle );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ShadowMap::RenderEnd(void)
{
	if (mIsActive == FALSE) return;
	if (mShadowMapType == eSHADOWMAP_TYPE_NONE) return;

	// 描画に使用するシャドウマップの設定を解除
	SetUseShadowMap( 0, -1 ) ;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
ShadowMap::PauseShadowMap(void)
{
	mIsActive = FALSE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
ShadowMap::ResumeShadowMap(void)
{
	mIsActive = TRUE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
ShadowMap::SetShadowMapType(eShadowMapType type)
{
	mShadowMapType = type;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/ShadowMap::eShadowMapType
ShadowMap::GetShadowMapType(void)
{
	return (eShadowMapType)mShadowMapType;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
ShadowMap::SetBeforeCreateCallback(CALLBACK_BASIC callback)
{
	if (callback == NULL) {
		mBeforeCreateCallback = Basic::DummyCallback;
	}
	else {
		mBeforeCreateCallback = callback;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
ShadowMap::SetAfterCreateCallback(CALLBACK_BASIC callback)
{
	if (callback == NULL) {
		mAfterCreateCallback = Basic::DummyCallback;
	}
	else {
		mAfterCreateCallback = callback;
	}
}

 /**** end of file ****/
