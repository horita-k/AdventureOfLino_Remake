/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ブルームレンダリングクラス
 * @author SPATZ.
 * @data   2015/11/23 12:25:41
 */
//---------------------------------------------------
#include "DxLib.h"

#include "AppLib/Basic/Basic.h"
#include "CameraManager.h"

#include "RenderBloom.h"

#include "DataDefine/ItemDef.h"

/*static*/int RenderBloom::mIsActive;
static int sScrHightBright	= -1;
static int sScrDownScale	= -1;
static int sScrGauss		= -1;
static int sScrWindow		= -1;

// ブルーム関連 
static BOOL								mEnableSystemBloom;	// システム側でのブルーム有効フラグ
static BOOL								mIsBloomActive;
static RenderBloom::eBloomType			mBloomType;
static BYTE								mBloomBlendMode;
static WORD								mBloomBlendValue;
static WORD								mBloomGaussValue;
static RenderBloom::tBloomBrightParam	mBloomBrightParam;
static RenderBloom::tBloomHSBParam		mBloomHSBParam;
static RenderBloom::tChangeBloomWork	mChangeBloomWork;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
RenderBloom::Create(void)
{
	mIsActive			= TRUE;
	mEnableSystemBloom	= TRUE;
	mIsBloomActive		= TRUE;

	memset(&mChangeBloomWork, 0, sizeof(mChangeBloomWork));

	mBloomType			= eBLOOM_TYPE_BRIGHT_LESS;
	mBloomBlendMode		= 2;
	mBloomBlendValue	= 255;
	mBloomGaussValue	= 100;
	memset(&mBloomBrightParam, 0, sizeof(mBloomBrightParam));
	mBloomBrightParam.mClipParam = 230;

	memset(&mBloomHSBParam, 0, sizeof(mBloomHSBParam));
	mBloomHSBParam.mHue = 100;
	mBloomHSBParam.mSaturation = 90;
	mBloomHSBParam.mBright = -60;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
RenderBloom::Setup(void)
{
	if (mIsActive == FALSE) return;

	int x, y, depth;
	GetScreenState(&x, &y, &depth);

	sScrHightBright		= MakeScreen(x, y, FALSE);
	sScrDownScale		= MakeScreen((x / 8), (y / 8), FALSE);
	sScrGauss			= MakeScreen((x / 8), (y / 8), FALSE);
	
	SetDrawValidMultiSample(4, 2);
	sScrWindow			= MakeScreen(x, y, FALSE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
RenderBloom::ClearScreen(void)
{
	if (mEnableSystemBloom) {
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();

		SetDrawScreen( sScrWindow );
		CameraManager::GetInstance()->RevertNearFarClip();
	}

	ClearDrawScreen();
}

//---------------------------------------------------
/**
 * ブルーム関連処理
 */
//---------------------------------------------------
void
RenderBloom::Update(void)
{
	if (mEnableSystemBloom == FALSE) return;
	// ブルーム処理

	if (mIsBloomActive == FALSE) {
		// 描画対象を裏画面にする
		SetDrawScreen( DX_SCREEN_BACK ) ;
	
		// 通常の描画結果を描画する
		DrawGraph( 0, 0, sScrWindow, FALSE );
	}
	// ブルーム処理有り
	else {
		tChangeBloomWork* p_change = &mChangeBloomWork;
		if (p_change->mIsActive == TRUE) {
			p_change->mNowSec += (1.0f / 60.0f);
			if (p_change->mNowSec > p_change->mMaxSec) {
				p_change->mIsActive = FALSE;
				mBloomBlendValue = p_change->mBlendValueTarget;
				mBloomGaussValue = p_change->mGaussValueTarget;
			} else {
				float rate = p_change->mNowSec / p_change->mMaxSec;
				mBloomBlendValue = (WORD)((float)(p_change->mBlendValueTarget - p_change->mBlendValueBase) * rate) + p_change->mBlendValueBase;
				mBloomGaussValue = (WORD)((float)(p_change->mGaussValueTarget - p_change->mGaussValueBase) * rate) + p_change->mGaussValueBase;
			}
		}

		if ((mBloomType == eBLOOM_TYPE_BRIGHT_LESS) ||
			(mBloomType == eBLOOM_TYPE_BRIGHT_GREATER)) {

			// 描画結果から高輝度部分のみを抜き出した画像を得る
			GraphFilterBlt(sScrWindow, 
						   sScrHightBright, 
						   DX_GRAPH_FILTER_BRIGHT_CLIP, 
						   (mBloomType == eBLOOM_TYPE_BRIGHT_LESS) ? DX_CMP_LESS : DX_CMP_GREATER,
						   mBloomBrightParam.mClipParam,
						   TRUE,					// クリップしたピクセルを塗りつぶすか
						   GetColor(mBloomBrightParam.mPixelColorR, mBloomBrightParam.mPixelColorG, mBloomBrightParam.mPixelColorB),
						   255) ;					// ピクセルのα値
		}
		else if ((mBloomType == eBLOOM_TYPE_HSB_RELATIVE) ||
				 (mBloomType == eBLOOM_TYPE_HSB_ABSOLUTE)) {
			// 色相・彩度・輝度フィルターを施す
			GraphFilterBlt(sScrWindow, 
						   sScrHightBright,
						   DX_GRAPH_FILTER_HSB, 
						   (mBloomType == eBLOOM_TYPE_HSB_RELATIVE) ? 0 : 1,	// (0:相対 1:絶対)
						   mBloomHSBParam.mHue,
						   mBloomHSBParam.mSaturation,
						   mBloomHSBParam.mBright);
		}

		// 高輝度部分を８分の１に縮小した画像を得る
		GraphFilterBlt(sScrHightBright, sScrDownScale, DX_GRAPH_FILTER_DOWN_SCALE, 8);

		// ８分の１に縮小した画像をガウスフィルタでぼかす
		GraphFilterBlt(sScrDownScale, 
					   sScrGauss, 
					   DX_GRAPH_FILTER_GAUSS, 
					   16,						// 使用ピクセル幅
					   mBloomGaussValue);		// ぼかしパラメータ

		// 描画対象を裏画面にする
		SetDrawScreen( DX_SCREEN_BACK ) ;
	
		int x, y, depth;
		GetScreenState(&x, &y, &depth);

		// 通常の描画結果を描画する
		DrawGraph( 0, 0, sScrWindow, FALSE );

		// 描画モードをバイリニアフィルタリングにする( 拡大したときにドットがぼやけるようにする )
		SetDrawMode( DX_DRAWMODE_BILINEAR ) ;

		// 描画ブレンドモードを加算にする
		SetDrawBlendMode(mBloomBlendMode, mBloomBlendValue);

		// 高輝度部分を縮小してぼかした画像を画面いっぱいに２回描画する( ２回描画するのはより明るくみえるようにするため )
		DrawExtendGraph( 0, 0, x, y, sScrGauss, FALSE ) ;

		// 描画ブレンドモードをブレンド無しに戻す
		SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 255 ) ;

		// 描画モードを二アレストに戻す
		SetDrawMode( DX_DRAWMODE_NEAREST ) ;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
RenderBloom::Destroy(void)
{
	return;
}

/*=====================================*
 * ブルーム
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
RenderBloom::SetEnableSystemBloom(BOOL isEnable)
{
	mEnableSystemBloom = isEnable; 
	if (isEnable == FALSE) {
		SetDrawScreen( DX_SCREEN_BACK ) ;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
BOOL
RenderBloom::GetEnableSystemBloom(void)
{
	return mEnableSystemBloom;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
RenderBloom::SetActiveBloom(BOOL isActive)
{
	mIsBloomActive = isActive;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
RenderBloom::SetBloomType(RenderBloom::eBloomType eType)
{
	mBloomType = eType;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
RenderBloom::eBloomType
RenderBloom::GetBloomType(void)
{
	return mBloomType;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
RenderBloom::SetBloomParam(BYTE blendMode, WORD blendValue, WORD gaussValue)
{
	mBloomBlendMode		= blendMode;
	mBloomBlendValue	= blendValue;
	mBloomGaussValue	= gaussValue;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
RenderBloom::GetBloomParam(BYTE& rBlendMode, WORD& rBlendValue, WORD& rGaussValue)
{
	rBlendMode	= mBloomBlendMode;
	rBlendValue = mBloomBlendValue;
	rGaussValue = mBloomGaussValue;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
RenderBloom::tBloomBrightParam&
RenderBloom::BloomBrightParam(void)
{
	return mBloomBrightParam;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
RenderBloom::tBloomHSBParam&
RenderBloom::BloomHSBParam(void)
{
	return mBloomHSBParam;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
RenderBloom::ChangeBloomParam(float setSec, WORD blendValue, WORD gaussValue)
{
	mChangeBloomWork.mIsActive = TRUE;
	mChangeBloomWork.mBlendValueBase	= mBloomBlendValue;
	mChangeBloomWork.mBlendValueTarget	= blendValue;
	mChangeBloomWork.mGaussValueBase	= mBloomGaussValue;
	mChangeBloomWork.mGaussValueTarget	= gaussValue;
	mChangeBloomWork.mNowSec = 0.0f;
	mChangeBloomWork.mMaxSec = setSec;
}

 /**** end of file ****/
