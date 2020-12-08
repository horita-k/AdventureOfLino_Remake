/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * グラフィック統括マネージャクラス
 * @author SPATZ.
 * @data   2011/11/20 23:25:23
 */
//---------------------------------------------------
#include "AppLib/Basic/Basic.h"
#include "GraphicManager.h"
#include "RenderBloom.h"
#include "../Effect/EffectManager.h"
#include "EffekseerManager.h"
#include "CameraManager.h"
#include "ShadowMap.h"

#include "AppLib/Resource/ResourceLayout.h"

#ifdef _DEBUG
#include "AppLib/Input/InputManager.h"
#include "AppLib/Basic/Counter.h"
#endif // _DEBUG

/*=====================================*
 * macro
 *=====================================*/
 /* グリッド表示を有効にする */
#undef _VISIBLE_GRID

/* ブルーム有効 */
#define _ENABLE_BLOOM

/*=====================================*
 * プロトタイプ
 *=====================================*/
void dummyDrawFunc(void);

/*=====================================*
 * static
 *=====================================*/
#ifdef _DEBUG
static bool sIsRenderDbgString	= true;
static bool sIsRenderDbgGraph	= false;
#endif // _DEBUG

/* 割り込み関数 */
static FUNC_DRAW_FONT spDrawFontFunc   = dummyDrawFunc;
static FUNC_DRAW_FONT spLockonFontFunc = dummyDrawFunc;

// フォントハンドル
static int sFontHandle[GraphicManager::eFONT_TYPE_MAX] = {-1, -1};
static BYTE sNowFontType = GraphicManager::eFONT_TYPE_DEFAULT;


//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/GraphicManager * const
GraphicManager::GetInstance(void)
{
	static GraphicManager sInstance;
	return &sInstance;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GraphicManager::GraphicManager()
{
	// initialize
	mDrawModelVector.clear();
	mDrawPreUpdateModelVector.clear();
	mDrawModelMapVector.clear();
	mpCursorModel = NULL;
	mpAfterEffectModel = NULL;
	mIsUpdateDrawModelVector = FALSE;
	mDrawLayoutVector.clear();
	mSystemLayoutVector.clear();
	mDrawStringVector.clear();
	mShapeLineVector.clear();
#ifdef _DEBUG
	mDbgShapeVector.clear();
#endif // _DEBUG
	mDrawShadowVector.clear();

	mLayoutScaleRateX = 0.0f;
	mLayoutScaleRateY = 0.0f;
	mAspectRate = 0.0f;

	sReserveEntryModel.clear();
	sReserveRemoveModel.clear();

	memset(&mFogParam, 0, sizeof(mFogParam));
	mFogParam.mFar = 50000.0f;

	memset(&mFogChangeWork, 0, sizeof(mFogChangeWork));

	mLightDirection = ZERO_VEC;
	for (int iI = 0; iI < ePLIGHT_NO_MAX; iI ++) {
		mOtherLightHandle[iI] = -1;
	}

	// DxLibシェーダー
	mVShaderHandle = -1;
	mPShaderHandle = -1;
	mIsEnableOriginalShader = FALSE;

	// ブルーム関連の初期化
	RenderBloom::Create();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GraphicManager::~GraphicManager()
{
	// ライトハンドルを全て削除する
	DeleteLightHandleAll();
	for (int iI = 0; iI < ePLIGHT_NO_MAX; iI ++) {
		mOtherLightHandle[iI] = -1;
	}

	// 作成したフォントデータを削除する

	for (int iI = 0; iI < eFONT_TYPE_MAX; iI ++) {
		if (sFontHandle[iI] != -1) {
			DeleteFontToHandle(sFontHandle[iI]);
			sFontHandle[iI] = -1;
		}
	}

	// DxLibシェーダーの破棄
	if (mVShaderHandle != -1) {
		DeleteShader(mVShaderHandle);
		mVShaderHandle = -1;
	}
	if (mPShaderHandle != -1) {
		DeleteShader(mPShaderHandle);
		mPShaderHandle = -1;
	}

	// シャドウマップの終了
	ShadowMap::Destroy();

	// ブルーム関連の終了
	RenderBloom::Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::Create(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::Setup(void)
{
	// ライト設定
	ePLightNo light_no;

	// ライトハンドルを作成する
	light_no = ePLIGHT_NO_00;
	mOtherLightHandle[light_no] = CreatePointLightHandle(VGet( 0.0f, 0.0f, 0.0f ), 0.0f, 0.0f, 0.0f, 0.0f);
	SetLightEnableHandle(mOtherLightHandle[light_no], FALSE);

	light_no = ePLIGHT_NO_01;
	mOtherLightHandle[light_no] = CreatePointLightHandle(VGet( 0.0f, 0.0f, 0.0f ), 0.0f, 0.0f, 0.0f, 0.0f);
	SetLightEnableHandle(mOtherLightHandle[light_no], FALSE);

	light_no = eSLIGHT_NO_00;
	mOtherLightHandle[light_no] = CreateSpotLightHandle(ZERO_VEC,
														VGet(0.0f, 0.0f, 1.0f),
														DX_PI_F/2.0f, DX_PI_F/4.0f,	0.0f,
														0.0f, 0.0f, 0.0f);
	SetLightEnableHandle(mOtherLightHandle[light_no], FALSE);

	// デフォルトオフだが、一応設定しておく
	SetPLightPosition(&VGet(0.0f, 1000.0f, 0.0f));
	SetPLightRangeAtten(5000.0f,
						0.1f,
						0.0f, 
						0.0f );
	SetPLightDiffuse(0.5f, 0.5f, 0.5f, 0.0f);
	SetPLightSpecular(0.0f, 0.0f, 0.0f, 0.0f);
	SetPLightAmbient(0.25f, 0.25f, 0.25f, 0.0f);

	// ディレクションライトをデフォルト値に設定
	SetDefaultDirectionLight();

	mLightDirection = VNorm(VGet(1.0f, -2.0f, 1.0f));
	SetLightDirection(mLightDirection);

	/* 重力設定 */
	MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
	MV1SetLoadModelPhysicsWorldGravity( -10.0f );

	int x, y, depth;
	GetScreenState(&x, &y, &depth);

	/* 画面比からレイアウトの比率を求める */
	float screen_rate;
	screen_rate = (float)y / (float)x;
	if (screen_rate == 0.5625f) {
		/* 16 : 9 */
		mLayoutScaleRateX = (float)x / 640.0f;
		mLayoutScaleRateY = (float)y / 360.0f;

		mAspectRate = (360.0f / 480.0f);		// Y軸を1にする様にあわせる
	} else if (screen_rate == 0.8f) {
		/* 5 : 4 */
		mLayoutScaleRateX = (float)x / 640.0f;
		mLayoutScaleRateY = (float)y / 512.0f;

		mAspectRate = (512.0f / 480.0f);		// Y軸を1にする様にあわせる
	} else {
		/* 4 : 3 */
		mLayoutScaleRateX = (float)x / 640.0f;
		mLayoutScaleRateY = (float)y / 480.0f;

		mAspectRate = (480.0f / 480.0f);		// Y軸を1にする様にあわせる
	}

	mLayoutScaleRateX *= mAspectRate;
	mLayoutScaleRateY *= mAspectRate;

	// DxLibのシェーダーを使用
	mVShaderHandle = LoadVertexShader("object/DxLibShader/PointLightShaderVS.vso");
	mPShaderHandle = LoadPixelShader("object/DxLibShader/PointLightShaderPS.pso");
	
	// シャドウマップの生成
	ShadowMap::Setup();

	// ブルーム関連のセットアップ
	RenderBloom::Setup();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::CreateFontData(int fontSize)
{
#define FONT_NAME		("HG丸ｺﾞｼｯｸM-PRO")

	const int font_thin = 5;

	// フォントを生成する
	if (sFontHandle[eFONT_TYPE_DEFAULT] == -1) {
		sFontHandle[eFONT_TYPE_DEFAULT] = CreateFontToHandle(FONT_NAME,						// 作成するフォント名( NULL にするとデフォルトのフォント )
															 fontSize,						// フォントのサイズ( およそドット数 -1:デフォルトのサイズ )
															 font_thin,						// フォントの太さ( 0 ～　9 -1:デフォルトの太さ )
															 DX_FONTTYPE_ANTIALIASING_4X4);	// フォントのタイプ
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::CreateEndingFontData(void)
{
#define ENDING_FONT_NAME		("SimSun")

	const int font_thin = 5;
	int font_size = GetFontSizeToHandle( sFontHandle[eFONT_TYPE_DEFAULT] ) / 2;

	// フォントを生成する
	if (sFontHandle[eFONT_TYPE_ENDING] == -1) {
		sFontHandle[eFONT_TYPE_ENDING] = CreateFontToHandle(ENDING_FONT_NAME,						// 作成するフォント名( NULL にするとデフォルトのフォント )
															font_size,						// フォントのサイズ( およそドット数 -1:デフォルトのサイズ )
															font_thin,						// フォントの太さ( 0 ～　9 -1:デフォルトの太さ )
															DX_FONTTYPE_ANTIALIASING_4X4);	// フォントのタイプ
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::DeleteEndingFontData(void)
{
	if (sFontHandle[eFONT_TYPE_ENDING] != -1) {
		DeleteFontToHandle(sFontHandle[eFONT_TYPE_ENDING]);
		sFontHandle[eFONT_TYPE_ENDING] = -1;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::SetFontType(GraphicManager::eFontType fontType) 
{
	sNowFontType = fontType;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::Update(void)
{
	// シャドウマップの更新
	ShadowMap::Update();

	// シャドウマップ描画開始
	ShadowMap::RenderBegin();

	/**** マップ描画前のモデル描画 ****/
	{
		std::vector<ModelBase *>::iterator ite;
		if (mIsStopModeUpdate == false) {
			for (ite = mDrawPreUpdateModelVector.begin(); ite != mDrawPreUpdateModelVector.end(); ite ++) {
				(*ite)->PreUpdate();
			}
		} else {
			for (ite = mDrawPreUpdateModelVector.begin(); ite != mDrawPreUpdateModelVector.end(); ite ++) {
				(*ite)->PreDraw();
			}
		}
	}

	// シャドウマップ描画終了
	ShadowMap::RenderEnd();

	// シャドウマップ描画開始
	ShadowMap::RenderBegin();

	/**** マップ描画 ****/
	{
		std::vector<ModelMap *>::iterator ite;
		for (ite = mDrawModelMapVector.begin(); ite != mDrawModelMapVector.end(); ite ++) {
			(*ite)->Update();

		}
	}
	// シャドウマップ描画終了
	ShadowMap::RenderEnd();


	/**** モデル描画 ****/
	mIsUpdateDrawModelVector = TRUE;
	{
		std::vector<ModelBase *>::iterator ite;
		if (mIsStopModeUpdate == false) {
			for (ite = mDrawModelVector.begin(); ite != mDrawModelVector.end(); ite ++) {
				(*ite)->Update();
			}
		} else {
			for (ite = mDrawModelVector.begin(); ite != mDrawModelVector.end(); ite ++) {
				(*ite)->Draw();
			}
		}
	}
	mIsUpdateDrawModelVector = FALSE;


	// シャドウマップ描画終了
	ShadowMap::RenderBegin();

	/**** マップ(アルファ部分)描画 ****/
	{
		std::vector<ModelMap *>::iterator ite;
		for (ite = mDrawModelMapVector.begin(); ite != mDrawModelMapVector.end(); ite ++) {
			(*ite)->RenderAlphaFrame();
		}
	}
	// シャドウマップ描画終了
	ShadowMap::RenderEnd();


	/**** 影描画 ****/
	// shadow
	if (ShadowMap::GetShadowMapType() == ShadowMap::eSHADOWMAP_TYPE_NONE) {
		std::vector<Shadow *>::iterator ite;
		for (ite = mDrawShadowVector.begin(); ite != mDrawShadowVector.end(); ite ++) {
			(*ite)->Update();
		}
	} 
	else {
		std::vector<Shadow *>::iterator ite;
		for (ite = mDrawShadowVector.begin(); ite != mDrawShadowVector.end(); ite ++) {
			(*ite)->UpdateNotRender();
		}
	}

	/*** カーソルモデル (navi) 描画 ****/
	if (mpCursorModel != NULL) {
		mpCursorModel->Update();
	}

	// Bloomが有効の場合のみ以下の処理をおこなう
	if (RenderBloom::GetEnableSystemBloom() == TRUE) {

		/**** エフェクト描画 ****/
		SetFogEnable(FALSE);

		// 描画不具合を直す為のダミー描画、なんじゃそりゃ
		VERTEX3D Vertex[ 3 ] ;
		int graph_handle = ResourceLayout::GetInstance()->GetHandle("Shadow.tga");
		DrawPolygon3D( Vertex, 1, graph_handle, TRUE ) ;

		// effect
		EffectManager::GetInstance()->Update();
		SetFogEnable(mFogParam.mIsEnable & mFogParam.mIsActive);

		/*** エフェクト描画後に描画されるモデル (こんなやり方嫌だ) ****/
		if (mpAfterEffectModel != NULL) {
			mpAfterEffectModel->Update();
		}

		/**** Effekseer 描画 ****/
		SetFogEnable(FALSE);

		if (mIsStopModeUpdate == false) {
			EffekseerManager::GetInstance()->Update(CameraManager::GetInstance()->Position(),
													CameraManager::GetInstance()->Target(),
													CameraManager::GetInstance()->Up());
		}
		else {
			EffekseerManager::GetInstance()->Draw(CameraManager::GetInstance()->Position(),
													CameraManager::GetInstance()->Target(),
													CameraManager::GetInstance()->Up());
		}

	}


	// シャドウマップ作成開始
	{
		SetFogEnable(FALSE);
		ShadowMap::CreateShadowMap(mDrawModelVector);
		// フォグ情報を元に戻す
		SetFogEnable(mFogParam.mIsEnable & mFogParam.mIsActive);
	}


	/**** フォグの変化処理 ****/
	if (mFogChangeWork.mIsActive == true) {
		mFogParam.mColR += mFogChangeWork.mDiffParam.mColR;
		mFogParam.mColG += mFogChangeWork.mDiffParam.mColG;
		mFogParam.mColB += mFogChangeWork.mDiffParam.mColB;
		mFogParam.mNear += mFogChangeWork.mDiffParam.mNear;
		mFogParam.mFar  += mFogChangeWork.mDiffParam.mFar;

		if (mFogParam.mNear > mFogParam.mFar) {
			mFogParam.mNear = mFogParam.mFar - 0.1f;
		}
		mFogChangeWork.mNowSec += (1.0f / 60.0f);
		if (mFogChangeWork.mMaxSec <= mFogChangeWork.mNowSec) {
			mFogChangeWork.mIsActive = false;

			mFogParam.mIsEnable = mFogChangeWork.mDiffParam.mIsEnable;
			mFogParam.mIsActive = mFogChangeWork.mDiffParam.mIsActive;
		} else {
			// フォグを更新する
			SetFogColor(mFogParam.mColR, mFogParam.mColG, mFogParam.mColB);
			SetFogStartEnd(mFogParam.mNear, mFogParam.mFar);
		}
	}

	// フォグ情報を元に戻す
	SetFogEnable(mFogParam.mIsEnable & mFogParam.mIsActive);

	/**** Bloom関連更新 ****/
	RenderBloom::Update();

	// バイリニア法に設定
	SetDrawMode(DX_DRAWMODE_BILINEAR);

	/**** レイアウト描画 ****/
	{
		std::vector<LayoutBase *>::iterator ite;
		for (ite = mDrawLayoutVector.begin(); ite != mDrawLayoutVector.end(); ite ++) {
			(*ite)->Update();
		}
	}

	/**** システムレイアウト描画 ****/
	{
		std::vector<LayoutBase *>::iterator ite;
		for (ite = mSystemLayoutVector.begin(); ite != mSystemLayoutVector.end(); ite ++)
		{
			(*ite)->Update();
		}
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	SetDrawBright(255, 255, 255);

	/**** 線形の描画 ****/
	{
		std::vector<ShapeLine>::iterator ite;
		for (ite = mShapeLineVector.begin(); ite != mShapeLineVector.end(); ite ++) {
			(*ite).Draw();
		}
	}

#ifdef _DEBUG
	{
		std::vector<tShapeVector>::iterator ite;
		for (ite = mDbgShapeVector.begin(); ite != mDbgShapeVector.end(); ite ++) {
			{
				std::vector<ShapeCapsule>::iterator sub;
				for (sub = (*ite).mCapsule.begin(); sub != (*ite).mCapsule.end(); sub ++) {
					(*sub).Draw();
				}
			}
			{
				std::vector<ShapeLine>::iterator sub;
				for (sub = (*ite).mLine.begin(); sub != (*ite).mLine.end(); sub ++) {
					(*sub).Draw();
				}
			}
			{
				std::vector<ShapeBox>::iterator sub;
				for (sub = (*ite).mBox.begin(); sub != (*ite).mBox.end(); sub ++) {
					(*sub).Draw();
				}
			}
			{
				std::vector<ShapeBoard>::iterator sub;
				for (sub = (*ite).mBoard.begin(); sub != (*ite).mBoard.end(); sub ++) {
					(*sub).Draw();
				}
			}
		}
	}
	// グリッドを表示する
#ifdef _VISIBLE_GRID
	{
		int width_length = 32;
		int height_length = 32;
		int x, y, depth;
		int color = 0;
		GetScreenState(&x, &y, &depth);
		for (int iI = 0; iI < x; iI += width_length) {
			if (iI % 256 == 0) {
				color = GetColor(255, 0, 0);
			} else {
				color = GetColor(255, 255, 255);
			}
			DrawLine(iI , 0 , iI , y , color);
		}
		for (int iJ = 0; iJ < y; iJ += height_length) {
			if (iJ % 256 == 0) {
				color = GetColor(255, 0, 0);
			} else {
				color = GetColor(255, 255, 255);
			}
			DrawLine(0 , iJ , x , iJ , color);
		}
	}
#endif _VISIBLE_GRID
#endif // _DEBUG

	/**** フォント描画 ****/
	{
		int cnt = 0;
		int font_size = 24;
		std::vector<tRenderStringWork>::iterator ite;
		for (ite = mDrawStringVector.begin(); ite != mDrawStringVector.end(); ite ++) {
			switch ((*ite).mStringType) {
			case eSTRING_TYPE_DEBUG:
				this->DrawFontString(0, (cnt++ * font_size), (*ite).mString, (*ite).mColor, font_size);
				break;

			case eSTRING_TYPE_NAME:
				this->DrawFontStringCenter(132, 304, (*ite).mString, (*ite).mColor, font_size);
				break;
			default:
				this->DrawFontStringCenter(320, (290 + ((int)(*ite).mStringType * (font_size + 4))), (*ite).mString, (*ite).mColor, font_size);
				break;
			} 
		}
		/* 自由に文字を描画するためのハンドラ */
		spDrawFontFunc();

		/* ロックオン専用文字列描画ハンドラ */
		spLockonFontFunc();
	}

	/* 追加予約のモデルを破棄する */
	if (sReserveEntryModel.empty() == false) {
		std::vector<ModelBase *>::iterator ite;
		for (ite = sReserveEntryModel.begin(); ite != sReserveEntryModel.end(); ite ++) {
			GraphicManager::GetInstance()->EntryDrawModel(*ite);
		}
		sReserveEntryModel.clear();	// 全て削除
	}
	/* 削除予約のモデルを破棄する */
	if (sReserveRemoveModel.empty() == false) {
		std::vector<ModelBase *>::iterator ite;
		for (ite = sReserveRemoveModel.begin(); ite != sReserveRemoveModel.end(); ite ++) {
			GraphicManager::GetInstance()->RemoveDrawModel(*ite);
		}
		sReserveRemoveModel.clear();	// 全て削除
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::PostUpdate(void)
{
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::Destroy(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::Refresh(void)
{
	mDrawStringVector.clear();
	mShapeLineVector.clear();
#ifdef _DEBUG
	mDbgShapeVector.clear();
#endif // _DEBUG
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::EntryDrawModel(ModelBase* pModelBase)
{
	std::vector<ModelBase *>::iterator ite;
	for (ite = mDrawModelVector.begin(); ite != mDrawModelVector.end(); ite ++) {
		if (*ite == pModelBase) {
			return;
		}
	}
#ifdef _DEBUG
	APP_ASSERT( (mIsUpdateDrawModelVector == FALSE),
				"mDrawModelVector is Update Now! \n");

#endif // _DEBUG
	mDrawModelVector.push_back(pModelBase);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::InsertDrawModel(ModelBase* pModelBase, ModelBase* pTargetModelBase)
{
#ifdef _DEBUG
	APP_ASSERT( (mIsUpdateDrawModelVector == FALSE),
				"mDrawModelVector is Update Now! \n");
#endif // _DEBUG

	std::vector<ModelBase *>::iterator ite;
	// 既にエントリーされていないかチェック
	for (ite = mDrawModelVector.begin(); ite != mDrawModelVector.end(); ite ++) {
		if (*ite == pModelBase) {
			return;
		}
	}

	for (ite = mDrawModelVector.begin(); ite != mDrawModelVector.end(); ite ++) {
		if (*ite == pTargetModelBase) {
			mDrawModelVector.insert(ite, pModelBase);
			return;
		}
	}

	// もし pTargetModelBase が挿入されていない場合は最後尾に push_back() する
	mDrawModelVector.push_back(pModelBase);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::RemoveDrawModel(ModelBase* pModelBase)
{
#ifdef _DEBUG
	APP_ASSERT( (mIsUpdateDrawModelVector == FALSE),
				"mDrawModelVector is Update Now! \n");
#endif // _DEBUG
	if (mDrawModelVector.size() == 0) return;

	std::vector<ModelBase *>::iterator ite;
	for (ite = mDrawModelVector.begin(); ite != mDrawModelVector.end(); ite ++) {
		if (*ite == pModelBase) {
			mDrawModelVector.erase(ite);
			break;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::RemoveDrawModelAll(void)
{
#ifdef _DEBUG
	APP_ASSERT( (mIsUpdateDrawModelVector == FALSE),
				"mDrawModelVector is Update Now! \n");
#endif // _DEBUG

	mDrawModelVector.clear();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::ReserveEntryModel(ModelBase* pModel)
{
	std::vector<ModelBase *>::iterator ite;
	for (ite = mDrawModelVector.begin(); ite != mDrawModelVector.end(); ite ++) {
		if (*ite == pModel) {
			return;
		}
	}

	sReserveEntryModel.push_back(pModel);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::ReserveRemoveModel(ModelBase* pModel)
{
	std::vector<ModelBase *>::iterator ite;
	for (ite = mDrawModelVector.begin(); ite != mDrawModelVector.end(); ite ++) {
		if (*ite == pModel) {
			sReserveRemoveModel.push_back(pModel);
			break;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::EntryDrawPreUpdateModel(ModelBase* pModelBase)
{
	std::vector<ModelBase *>::iterator ite;
	for (ite = mDrawPreUpdateModelVector.begin(); ite != mDrawPreUpdateModelVector.end(); ite ++) {
		if (*ite == pModelBase) {
			return;
		}
	}
	mDrawPreUpdateModelVector.push_back(pModelBase);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::RemoveDrawPreUpdateModel(ModelBase* pModelBase)
{
	std::vector<ModelBase *>::iterator ite;
	for (ite = mDrawPreUpdateModelVector.begin(); ite != mDrawPreUpdateModelVector.end(); ite ++) {
		if (*ite == pModelBase) {
			mDrawPreUpdateModelVector.erase(ite);
			break;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::RemoveDrawPreUpdateModelAll(void)
{
	mDrawPreUpdateModelVector.clear();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::EntryCursorModel(ModelBase* pModelBase)
{
	mpCursorModel = pModelBase;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
BYTE
GraphicManager::IsEntryCursorModel(void)
{
	return (mpCursorModel == NULL) ? FALSE : TRUE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::EntryAfterDrawModel(ModelBase* pModelBase)
{
	mpAfterEffectModel = pModelBase;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::EntryDrawMapModel(ModelMap* pModelMap)
{
	std::vector<ModelMap *>::iterator ite;
	for (ite = mDrawModelMapVector.begin(); ite != mDrawModelMapVector.end(); ite ++) {
		if (*ite == pModelMap) {
			return;
		}
	}
	mDrawModelMapVector.push_back(pModelMap);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::RemoveDrawMapModel(ModelMap* pModelMap)
{
	std::vector<ModelMap *>::iterator ite;
	for (ite = mDrawModelMapVector.begin(); ite != mDrawModelMapVector.end(); ite ++) {
		if (*ite == pModelMap) {
			mDrawModelMapVector.erase(ite);
			break;
		}
	}
}


//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::RemoveDrawMapModelAll(void)
{
	mDrawModelMapVector.clear();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::EntryDrawLayout(LayoutBase* pLayoutBase)
{
	mDrawLayoutVector.push_back(pLayoutBase);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::InsertDrawLayout(LayoutBase* pLayoutBase, LayoutBase* pTargetLayoutBase)
{
	std::vector<LayoutBase *>::iterator ite;
	// 既にエントリーされていないかチェック
	for (ite = mDrawLayoutVector.begin(); ite != mDrawLayoutVector.end(); ite ++) {
		if (*ite == pLayoutBase) {
			return;
		}
	}

	// pTargetLayoutBase を探しだし、その前に insert() を行う
	for (ite = mDrawLayoutVector.begin(); ite != mDrawLayoutVector.end(); ite ++) {
		if (*ite == pTargetLayoutBase) {
			mDrawLayoutVector.insert(ite, pLayoutBase);
			return;
		}
	}

	// もし pTargetLayoutBase が挿入されていない場合は最後尾に push_back() する
	mDrawLayoutVector.push_back(pLayoutBase);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::RemoveDrawLayout(LayoutBase* pLayoutBase)
{
	std::vector<LayoutBase *>::iterator ite;
	for (ite = mDrawLayoutVector.begin(); ite != mDrawLayoutVector.end(); ite ++) {
		if (*ite == pLayoutBase) {
			mDrawLayoutVector.erase(ite);
			break;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::RemoveDrawLayoutAll(void)
{
	mDrawLayoutVector.clear();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
GraphicManager::CheckEntryDrawLayout(LayoutBase* pLayoutBase)
{
	std::vector<LayoutBase *>::iterator ite;
	for (ite = mDrawLayoutVector.begin(); ite != mDrawLayoutVector.end(); ite ++) {
		if (*ite == pLayoutBase) {
			return true;
		}
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::EntrySystemLayout(LayoutBase* pLayoutBase, eSystemLayoutPriority lytPrio/*=eSYS_LAYOUT_PRIORITY_LAST*/)
{
	std::vector<LayoutBase *>::iterator ite;

	switch (lytPrio) {
	case eSYS_LAYOUT_PRIORITY_FIRST:
		// 最低
		ite = mSystemLayoutVector.begin();
		mSystemLayoutVector.insert(ite, pLayoutBase);
		break;

	case eSYS_LAYOUT_PRIORITY_FADE_BEFORE:
		// フェード板より下
		for (ite = mSystemLayoutVector.begin(); ite != mSystemLayoutVector.end(); ite ++) {
			if (*ite == mpFadeLayout) {
				mSystemLayoutVector.insert(ite, pLayoutBase);
				break;
			}
		}
		break;

	case eSYS_LAYOUT_PRIORITY_FADE_AFTER:
		// フェード板より上
		for (ite = mSystemLayoutVector.begin(); ite != mSystemLayoutVector.end(); ite ++) {
			if (*ite == mpFadeLayout) {
				ite ++;
				mSystemLayoutVector.insert(ite, pLayoutBase);
				break;
			}
		}
		break;

	case eSYS_LAYOUT_PRIORITY_LAST:
		// 最高
		mSystemLayoutVector.push_back(pLayoutBase);
		break;

	default:
		APP_ASSERT(NULL, "Invalid case \n");
		break;
	};
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::RemoveSystemLayout(LayoutBase* pLayoutBase)
{
	if (mSystemLayoutVector.size() == 0) return;

	std::vector<LayoutBase *>::iterator ite;
	for (ite = mSystemLayoutVector.begin(); ite != mSystemLayoutVector.end(); ite ++) {
		if (*ite == pLayoutBase) {
			mSystemLayoutVector.erase(ite);
			break;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::RemoveSystemLayoutAll(void)
{
	mSystemLayoutVector.clear();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
GraphicManager::CheckEntrySystemLayout(LayoutBase* pLayoutBase)
{
	std::vector<LayoutBase *>::iterator ite;
	for (ite = mSystemLayoutVector.begin(); ite != mSystemLayoutVector.end(); ite ++) {
		if (*ite == pLayoutBase) {
			return true;
		}
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::AssignFadeboardSystemLayout(LayoutBase* pLayoutBase)
{
	mpFadeLayout = pLayoutBase;
}

#if 0
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::EntryHitModel(ModelBase* pModelBase)
{
	mHitModelVector.push_back(pModelBase);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::RemoveHitModel(ModelBase* pModelBase)
{
	std::vector<ModelBase *>::iterator ite;
	for (ite = mHitModelVector.begin(); ite != mHitModelVector.end(); ite ++) {
		if (*ite == pModelBase) {
			mHitModelVector.erase(ite);
			break;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::RemoveHitModelAll(void)
{
	mHitModelVector.clear();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
GraphicManager::GetHitModelNum(void)
{
	return mHitModelVector.size();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ModelBase*
GraphicManager::GetHitModelBaseAddr(int modelIndex)
{
	return mHitModelVector.at(modelIndex);
}
#endif /* 0 */

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::EntryDrawShadow(Shadow* pShadow)
{
	mDrawShadowVector.push_back(pShadow);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::RemoveDrawShadow(Shadow* pShadow)
{
	if (mDrawShadowVector.size() == 0) return;

	std::vector<Shadow *>::iterator ite;
	for (ite = mDrawShadowVector.begin(); ite != mDrawShadowVector.end(); ite ++) {
		if (*ite == pShadow) {
			mDrawShadowVector.erase(ite);
			break;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::RemoveDrawShadowAll(void)
{
	mDrawShadowVector.clear();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
GraphicManager::CheckEntryShadow(Shadow* pShadow)
{
	std::vector<Shadow *>::iterator ite;
	for (ite = mDrawShadowVector.begin(); ite != mDrawShadowVector.end(); ite ++) {
		if (*ite == pShadow) {
			return true;
		}
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::RenderMessage(char* CharaName, char* Message[GraphicManager::kMESSAGE_LINE_NUM], int setColor)
{
	int str_len;
	tRenderStringWork work;
	int color = setColor;

	str_len = strlen(CharaName);
	// 使用できる最大文字列を超えている場合
	if (str_len > kDRAW_STRING_LEN - 1) {
		return; // 使用不可
	}
	strcpy_s(work.mString, sizeof(work.mString), CharaName);
	work.mString[str_len] = '\0';
	work.mColor = color;
	work.mStringType = eSTRING_TYPE_NAME;

	mDrawStringVector.push_back(work);

	for (int iI = 0; iI < kMESSAGE_LINE_NUM; iI ++) {
		str_len = strlen(Message[iI]);
		// 使用できる最大文字列を超えている場合
		if (str_len > kDRAW_STRING_LEN - 1) {
			return; // 使用不可
		}
		strcpy_s(work.mString, sizeof(work.mString), Message[iI]);
		work.mString[str_len] = '\0';
		work.mColor = color;
		work.mStringType = eStringType(eSTRING_TYPE_MESSAGE_0 + iI);
		
		mDrawStringVector.push_back(work);
	}
}

void		
GraphicManager::RenderString(eStringType type, const char* setString, int setColor)
{
	RenderString(type, (char *)setString, setColor);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::RenderString(eStringType type, char* setString, int setColor)
{
	int str_len;
	tRenderStringWork work;

	int color;
	color = setColor;

	str_len = strlen(setString);
	// 使用できる最大文字列を超えている場合
	if (str_len > kDRAW_STRING_LEN - 1) {
		return; // 使用不可
	}
	strcpy_s(work.mString, sizeof(work.mString), setString);
	work.mString[str_len] = '\0';
	work.mColor = color;
	work.mStringType = type;
		
	mDrawStringVector.push_back(work);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::RenderLine(ShapeLine& rLine)
{
	mShapeLineVector.push_back(rLine);
}

#ifdef _DEBUG
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::DbgRenderString(char *String ,int Color)
{
	if (sIsRenderDbgString == false) return;

	int str_len = strlen(String);
	// 使用できる最大文字列を超えている場合
	if (str_len > kDRAW_STRING_LEN - 1) {
		return; // 使用不可
	}

	tRenderStringWork work;
	strcpy_s(work.mString, sizeof(work.mString), String);
	work.mString[str_len] = '\0';
	work.mColor = Color;
	work.mStringType = eSTRING_TYPE_DEBUG;

	mDrawStringVector.push_back(work);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::DbgRenderLine(ShapeLine* pLine)
{
	if (sIsRenderDbgGraph == false) return;

	tShapeVector shape;
	shape.mLine.push_back(*pLine);
	mDbgShapeVector.push_back(shape);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::DbgRenderLine(VECTOR* pStart, VECTOR* pEnd)
{
	if (sIsRenderDbgGraph == false) return;

	tShapeVector shape;
	ShapeLine line;
	line.mStart = *pStart;
	line.mEnd	= *pEnd;
	line.mColor = COLOR_YELLOW;

	shape.mLine.push_back(line);
	mDbgShapeVector.push_back(shape);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::DbgRenderLine(VECTOR* pStart, VECTOR* pEnd, int color)
{
	if (sIsRenderDbgGraph == false) return;

	tShapeVector shape;
	ShapeLine line;
	line.mStart = *pStart;
	line.mEnd	= *pEnd;
	line.mColor = color;

	shape.mLine.push_back(line);
	mDbgShapeVector.push_back(shape);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::DbgRenderCapsule(ShapeCapsule* pCapsule)
{
	if (sIsRenderDbgGraph == false) return;

	tShapeVector shape;
	shape.mCapsule.push_back(*pCapsule);
	mDbgShapeVector.push_back(shape);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::DbgRenderBox(ShapeBox* pBox)
{
	if (sIsRenderDbgGraph == false) return;

	tShapeVector shape;
	shape.mBox.push_back(*pBox);
	mDbgShapeVector.push_back(shape);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::DbgRenderBoard(ShapeBoard* pBoard)
{
	if (sIsRenderDbgGraph == false) return;

	tShapeVector shape;
	shape.mBoard.push_back(*pBoard);
	mDbgShapeVector.push_back(shape);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::DbgSetRenderString(bool isRender)
{
	sIsRenderDbgString = isRender;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::DbgSetRenderGraph(bool isRender)
{
	sIsRenderDbgGraph = isRender;
}
#endif // _DEBUG

//---------------------------------------------------
/**
   
 */
//---------------------------------------------------
void
GraphicManager::EnableFogPattern(int setR, int setG, int setB, float setNear, float setFar)
{
	mFogParam.mIsEnable = TRUE;
	mFogParam.mIsActive = TRUE;
	mFogParam.mColR = setR;
	mFogParam.mColG = setG;
	mFogParam.mColB = setB;
	mFogParam.mNear = setNear;
	mFogParam.mFar = setFar;

	SetFogColor(mFogParam.mColR, mFogParam.mColG, mFogParam.mColB);
	SetFogStartEnd(mFogParam.mNear, mFogParam.mFar);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::DisableFogPattern(void)
{
	mFogParam.mIsEnable = FALSE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::PauseFog(void)
{
	mFogParam.mIsActive = FALSE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::ResumeFog(void)
{
	mFogParam.mIsActive = TRUE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::RevertFogParam(float setSec)
{ 
	ChangeFogParam(setSec, &mBackupFogParam);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::ChangeFogParam(float setSec, tFogParam *pFogParam)
{
	if (setSec == 0.0f) return;

	float max_frame = (60.0f * setSec);
	float move_rate = 1.0f / max_frame;

	if (mFogParam.mIsActive == FALSE) {
		if (pFogParam->mIsActive == FALSE) {
			/* 無効 ⇒ 無効 */
			return;		// 終了

		} else if (pFogParam->mIsActive == TRUE) {
			/* 無効 ⇒ 有効 */
			// フォグが無効の場合はNearの値をFarと同等に設定する
			mFogParam.mNear = mFogParam.mFar;

			mFogParam.mIsEnable = TRUE;
			mFogParam.mIsActive = TRUE;
		}
	} else if (mFogParam.mIsActive == TRUE)	{
		if (pFogParam->mIsActive == FALSE) {
			/* 有効 ⇒ 無効 */
			pFogParam->mNear = pFogParam->mFar;
		}
	}

	mFogChangeWork.mIsActive = true;
	mFogChangeWork.mDiffParam.mIsEnable = pFogParam->mIsEnable;
	mFogChangeWork.mDiffParam.mIsActive = pFogParam->mIsActive;
	mFogChangeWork.mDiffParam.mColR = (int)((float)(pFogParam->mColR - mFogParam.mColR) * move_rate);
	mFogChangeWork.mDiffParam.mColG = (int)((float)(pFogParam->mColG - mFogParam.mColG) * move_rate);
	mFogChangeWork.mDiffParam.mColB = (int)((float)(pFogParam->mColB - mFogParam.mColB) * move_rate);
	mFogChangeWork.mDiffParam.mNear = ((pFogParam->mNear - mFogParam.mNear) * move_rate);
	mFogChangeWork.mDiffParam.mFar = ((pFogParam->mFar - mFogParam.mFar) * move_rate);
	mFogChangeWork.mNowSec		= 0.0f;
	mFogChangeWork.mMaxSec		= setSec;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::SetEnableDirectionLight(bool isEnable)
{
	// 通常ライトを有効にする
	SetLightEnable((isEnable == true ? TRUE : FALSE));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
GraphicManager::GetEnableDirectionLight(void)
{
	return (GetLightEnable() == TRUE ? true : false);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::SetDefaultDirectionLight(void)
{
	int ret;
	static float col_d = 0.8f;
	static float col_s = 0.1f;
	static float col_a = 0.5f;

	COLOR_F col;
	col = GetColorF(col_d, col_d, col_d, col_d);
	ret = SetLightDifColor(col);
	col = GetColorF(col_s, col_s, col_s, col_s);
	ret = SetLightSpcColor(col);
	col = GetColorF(col_a, col_a, col_a, col_a);
	ret = SetLightAmbColor(col);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::SetEnablePLight(bool isEnable, ePLightNo lightIndex/*=ePLIGHT_NO_00*/)
{
	// ポイントライトを無効にする
	SetLightEnableHandle(mOtherLightHandle[lightIndex], (isEnable == true ? TRUE : FALSE));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::DisablePLightAll(void)
{
	for (int iI = 0; iI < ePLIGHT_NO_MAX; iI ++) {
		// ポイントライトを無効にする
		SetLightEnableHandle(mOtherLightHandle[iI], FALSE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
GraphicManager::GetEnablePLight(ePLightNo lightIndex/*=ePLIGHT_NO_00*/)
{
	return (GetLightEnableHandle(mOtherLightHandle[lightIndex]) == TRUE ? true : false);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::SetPLightPosition(VECTOR* pPosition, ePLightNo lightIndex/*=ePLIGHT_NO_00*/)
{
	SetLightPositionHandle(mOtherLightHandle[lightIndex], *pPosition);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::SetPLightRangeAtten(float range, float atten0, float atten1, float atten2, ePLightNo lightIndex/*=ePLIGHT_NO_00*/)
{
	SetLightRangeAttenHandle(mOtherLightHandle[lightIndex], range, atten0, atten1, atten2);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::SetPLightDiffuse(float r, float g, float b, float a, ePLightNo lightIndex/*=ePLIGHT_NO_00*/)
{
	SetLightDifColorHandle(mOtherLightHandle[lightIndex], GetColorF(r, g, b, a));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::SetPLightSpecular(float r, float g, float b, float a, ePLightNo lightIndex/*=ePLIGHT_NO_00*/)
{
	SetLightSpcColorHandle(mOtherLightHandle[lightIndex], GetColorF(r, g, b, a));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::SetPLightAmbient(float r, float g, float b, float a, ePLightNo lightIndex/*=ePLIGHT_NO_00*/)
{
	SetLightAmbColorHandle(mOtherLightHandle[lightIndex], GetColorF(r, g, b, a));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::SetPLightDirection(VECTOR* pDir, ePLightNo lightIndex/*=ePLIGHT_NO_00*/)
{
	SetLightDirectionHandle(mOtherLightHandle[lightIndex], *pDir);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::SetPLightAngle(float outAngle, float inAngle, ePLightNo lightIndex/*=ePLIGHT_NO_00*/)
{
	SetLightAngleHandle(mOtherLightHandle[lightIndex], outAngle, inAngle);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::GetPLightPosition(VECTOR* pPosition, ePLightNo lightIndex/*=ePLIGHT_NO_00*/)
{
	*pPosition = GetLightPositionHandle(mOtherLightHandle[lightIndex]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::GetPLightRangeAtten(float* range, float* atten0, float* atten1, float* atten2, ePLightNo lightIndex/*=ePLIGHT_NO_00*/)
{
	GetLightRangeAttenHandle(mOtherLightHandle[lightIndex], range, atten0, atten1, atten2);	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::GetPLightDiffuse(COLOR_F* pColor, ePLightNo lightIndex/*=ePLIGHT_NO_00*/)
{
	*pColor = GetLightDifColorHandle(mOtherLightHandle[lightIndex]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::GetPLightSpecular(COLOR_F* pColor, ePLightNo lightIndex/*=ePLIGHT_NO_00*/)
{
	*pColor = GetLightSpcColorHandle(mOtherLightHandle[lightIndex]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::GetPLightAmbient(COLOR_F* pColor, ePLightNo lightIndex/*=ePLIGHT_NO_00*/)
{
	*pColor = GetLightAmbColorHandle(mOtherLightHandle[lightIndex]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::SetEnableOriginalShader(bool isEnable)
{
	mIsEnableOriginalShader = (isEnable == true ? TRUE : FALSE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
GraphicManager::GetEnableOriginalShader(void)
{
	return (mIsEnableOriginalShader == TRUE ? true : false);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::StartOriginalShader(void)
{
	MV1SetUseOrigShader(mIsEnableOriginalShader);

	SetUseVertexShader(mVShaderHandle);
	SetUsePixelShader(mPShaderHandle);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::EndOriginalShader(void)
{
	MV1SetUseOrigShader(FALSE);

	SetUseVertexShader(-1);
	SetUsePixelShader(-1);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::SetStopModeUpdate(bool isStop)
{
	if (isStop) {
		EffectManager::SetEffectUpdateSpeed(0);
	}
	else {
		EffectManager::SetEffectUpdateSpeed(1);
	}
	mIsStopModeUpdate = isStop; 
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
GraphicManager::GetStopModeUpdate(void)
{
	return mIsStopModeUpdate; 
}

//---------------------------------------------------
/**
 * フォントの自由描画用関数
 */
//---------------------------------------------------
void
GraphicManager::EntryDrawFontFunc(FUNC_DRAW_FONT func)
{
	if (func == NULL) {
		spDrawFontFunc = dummyDrawFunc;
	} else {
		spDrawFontFunc = func;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::EntryLockonFontFunc(FUNC_DRAW_FONT func)
{
	if (func == NULL) {
		spLockonFontFunc = dummyDrawFunc;
	} else {
		spLockonFontFunc = func;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::DrawFontString(int x ,int y , char *String ,int Color, int size)
{
	int string_len = strlen(String);

	x = (int)(x * mLayoutScaleRateX);
	y = (int)(y * mLayoutScaleRateY);

	DrawStringToHandle(x, y, String , Color, sFontHandle[sNowFontType]);
}

//---------------------------------------------------
/**
 * X軸にセンタリングして文字を表示
 */
//---------------------------------------------------
void
GraphicManager::DrawFontStringCenter(int x ,int y ,char *String ,int Color, int size)
{
	static const float sFontSizeTable[GraphicManager::eFONT_TYPE_MAX] = {5.5f, 3.2f};

	int string_len = strlen(String);
	float scale_rate_x = mLayoutScaleRateX / mAspectRate;
	float center_pos_x = (float)x * scale_rate_x;
	center_pos_x -= 16.0f;

	x = (int)(center_pos_x - (sFontSizeTable[sNowFontType] * scale_rate_x * (float)string_len));
	y = (int)(y * mLayoutScaleRateY);

	DrawStringToHandle(x, y, String , Color, sFontHandle[sNowFontType]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void dummyDrawFunc(void)
{

}

/*=====================================*
 * Blur テスト
 *=====================================*/
#if 0
//---------------------------------------------------
/**
 * 画面（バックバッファ）の消去
 */
//---------------------------------------------------
void GraphicsClear(D3DCOLOR color) 
{
	GET_D3DDEV()->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, color, 1, 0 );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void SetRenderTarget(
	LPDIRECT3DTEXTURE9 texture, 
	LPDIRECT3DSURFACE9 depth_stencil
) {
	LPDIRECT3DSURFACE9 rt;
	if (SUCCEEDED(texture->GetSurfaceLevel(0, &rt))) {
		GET_D3DDEV()->SetRenderTarget(0, rt);
		rt->Release();
		GET_D3DDEV()->SetDepthStencilSurface(depth_stencil);
	}
}

//---------------------------------------------------
/**
 * レンダリングターゲットの初期化
 */
//---------------------------------------------------
void ResetRenderTarget() 
{
	GET_D3DDEV()->SetRenderTarget(0, EffectManager::GetInstance()->GetOrgRenderTarget() );
	GET_D3DDEV()->SetDepthStencilSurface(EffectManager::GetInstance()->GetOrgDepthStencil() );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
// テクスチャの描画
void DrawTexture(
	LPDIRECT3DBASETEXTURE9 texture, 
	float x0, float y0, float x1, float y1, 
	float x2, float y2, float x3, float y3, 
	float u0, float v0, float u1, float v1, 
	D3DCOLOR diffuse
) {
	// テクスチャ処理方法の設定
	GET_D3DDEV()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	GET_D3DDEV()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	GET_D3DDEV()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	GET_D3DDEV()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	GET_D3DDEV()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	GET_D3DDEV()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	// テクスチャの設定
	GET_D3DDEV()->SetTexture(0, texture);
	
	// 四角形の描画
	DrawQuad(x0, y0, x1, y1, x2, y2, x3, y3, u0, v0, u1, v1, diffuse);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void DrawTexture(
		LPDIRECT3DBASETEXTURE9 texture, 
		float x0, float y0, float x1, float y1, 
		float u0, float v0, float u1, float v1, 
		D3DCOLOR diffuse
) {
	DrawTexture(
				texture, x0, y0, x1, y0, x0, y1, 
				x1, y1, u0, v0, u1, v1, diffuse);
}


/*=====================================*
 * ブラーテスト関連
 *=====================================*/
static LPDIRECT3DTEXTURE9* WorkTexture = NULL;
static LPDIRECT3DSURFACE9* WorkDepthStencil = NULL;
static int Target = 0;
static D3DCOLOR BGColor;
float ObjYaw, EnvYaw, Alpha, Roll, Scale;
int Width, Height;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::testBlurInit(void)
{
	BGColor = D3DCOLOR_XRGB(128, 128, 128);

	Target = 0;
	ObjYaw = 0;
	EnvYaw = 0;
	Alpha = 1.5f;
	
	Roll = 1.02f;
	Scale = 1.1f;

	// クライアント領域のサイズを取得
	int depth;
	GetScreenState(&Width, &Height, &depth);

	WorkTexture			= new LPDIRECT3DTEXTURE9[4];
	WorkDepthStencil	= new LPDIRECT3DSURFACE9[4];

	for (int i=0, n=4; i<n; i++) {
		GET_D3DDEV()->CreateTexture(Width, Height, 1, D3DUSAGE_RENDERTARGET, 
									D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &WorkTexture[i], NULL);
		GET_D3DDEV()->CreateDepthStencilSurface
			(Width, Height, D3DFMT_D16, 
			 D3DMULTISAMPLE_NONE, 0, TRUE, &WorkDepthStencil[i], NULL);
	}
}

static BYTE sFlg = FALSE;
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::testBlurPreUpdate(void)
{
	if (sFlg == FALSE) return;

	// テクスチャ０にメッシュを描画
	SetRenderTarget(WorkTexture[Target], WorkDepthStencil[Target]);
	GraphicsClear(BGColor);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::testBlurUpdate(void)
{
	if (sFlg == FALSE) return;

	float w = (float)Width;
	float h = (float)Height;

	// テクスチャ１をテクスチャ０上に描画
	float 
		r=Roll*D3DX_PI*2, c=cos(r), s=sin(r), 
		dx=((-w*c+h*s)*Scale+w)/2, 
		dy=((-w*s-h*c)*Scale+h)/2;
	DrawTexture(
		WorkTexture[1-Target], 
		dx, dy, w-1-dy, dx, dy, h-1-dx, w-1-dx, h-1-dy, 
		0, 0, 1, 1, D3DCOLOR_ARGB((int)(255*Alpha), 255, 255, 255));

	// テクスチャ０を画面に描画
	ResetRenderTarget();
	GET_D3DDEV()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	DrawTexture(WorkTexture[Target], 0, 0, w, h, 0, 0, 1, 1, D3DXCOLOR(1, 1, 1, 1));
	GET_D3DDEV()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GraphicManager::testBlurDestroy(void)
{
	for (int iI = 0; iI < 4; iI ++ ){
		SAFE_RELEASE(WorkTexture[iI]);
		SAFE_RELEASE(WorkDepthStencil[iI]);
	}

	delete[] WorkTexture;
	WorkTexture = NULL;

	delete[] WorkDepthStencil;
	WorkDepthStencil = NULL;

}
#endif // 0

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
#ifdef _DEBUG
void
GraphicManager::dumpDrawModelVector(void)
{
	int cnt = 0;

	PRINT_CONSOLE(" * GraphicManager::dumpDrawModelVector() : Start * \n");

	std::vector<ModelBase *>::iterator ite;
	for (ite = mDrawModelVector.begin(); ite != mDrawModelVector.end(); ite ++) {

		PRINT_CONSOLE("* mDrawModelVector[%d] : 0x%08x \n", cnt++, (DWORD)*ite);

	}

	PRINT_CONSOLE(" * GraphicManager::dumpDrawModelVector() : End * \n");

}
#endif // _DEBUG



/**** end of file ****/

