/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * グラフィック統括マネージャクラス
 * @author SPATZ.
 * @data   2011/11/20 23:20:14
 */
//---------------------------------------------------
#ifndef __GRAPHIC_MANAGER_H__
#define __GRAPHIC_MANAGER_H__

#include <vector>
#include "DxLib.h"
#include "ModelBase.h"
#include "ModelMap.h"
#include "LayoutBase.h"
#include "BillboardBase.h"
#include "Shadow.h"
#include "Shape.h"

#ifdef _DEBUG
#define PRINT_SCREEN(color, str, ...)								\
	{																\
		char cB[128] = {0};											\
		sprintf_s(cB, sizeof(cB), str, __VA_ARGS__);				\
		GraphicManager::GetInstance()->DbgRenderString(cB, color);		\
}
#else
#define PRINT_SCREEN(color, str, ... ) // 空実装
#endif // _DEBUG

// 色定義
#define COLOR_RED						GetColor(255, 0, 0)
#define COLOR_GREEN						GetColor(0, 255, 0)
#define COLOR_BLUE						GetColor(0, 0, 255)
#define COLOR_WHITE						GetColor(255, 255, 255)
#define COLOR_BLACK						GetColor(0, 0, 0)
#define COLOR_YELLOW					GetColor(255, 255, 0)

// 直線描画関数
#define RENDER_LINE(pos, tar, color)	GraphicManager::GetInstance()->DbgRenderLine(pos, tar, color)

static const int kDRAW_STRING_LEN = 64;

typedef		void (*FUNC_DRAW_FONT)(void);

enum ePLightNo {
	ePLIGHT_NO_00 = 0,	// ポイントライト①
	eSLIGHT_NO_00,		// スポットライト①

	ePLIGHT_NO_01,		// ポイントライト②

	
	ePLIGHT_NO_MAX,
};

/*=====================================*
 * class
 *=====================================*/
class GraphicManager
{
 public:
	static const int kMESSAGE_LINE_NUM = 4;

	enum eStringType {
		eSTRING_TYPE_DEBUG = 0,
		eSTRING_TYPE_NAME,
		eSTRING_TYPE_MESSAGE_0,
		eSTRING_TYPE_MESSAGE_1,
		eSTRING_TYPE_MESSAGE_2,
		eSTRING_TYPE_MESSAGE_3,

		eSTRING_TYPE_MAX,
	};

	struct tRenderStringWork {
		char	mString[kDRAW_STRING_LEN];
		int		mColor;
		eStringType		mStringType;
	};
	
	enum eFogPattern {
		eFOG_PATTERN_MIST = 0,
		eFOG_PATTERN_DARK,
		eFOG_PATTERN_WATER,
		eFOG_PATTERN_PURPLE,

		eFOG_PATTERN_MAX,
	};

	struct tFogParam {
		BOOL mIsEnable;
		BOOL mIsActive;
		int mColR;
		int mColG;
		int mColB;
		float mNear;
		float mFar;
	};

	// フォント種別
	enum eFontType {
		eFONT_TYPE_DEFAULT = 0,
		eFONT_TYPE_ENDING,

		eFONT_TYPE_MAX,
	};

	// スクリーン種別
	enum eScreenType {
		eSCREEN_TYPE_FULL = 0,
		eSCREEN_TYPE_HALF,

		eSCREEN_TYPE_MAX,
	};

	// レイアウトプライオリティ
	enum eSystemLayoutPriority {
		eSYS_LAYOUT_PRIORITY_FIRST,				// 最低
		eSYS_LAYOUT_PRIORITY_FADE_BEFORE,		// フェード板より下
		eSYS_LAYOUT_PRIORITY_FADE_AFTER,		// フェード板より上
		eSYS_LAYOUT_PRIORITY_LAST,				// 最高

		eSYS_LAYOUT_PRIORITY_MAX,
	};

	static GraphicManager* const GetInstance(void);

	void		Create(void);
	void		Setup(void);
	void		Update(void);
	
	void		PostUpdate(void);

	void		Destroy(void);

	void		Refresh(void);
	
	// フォントデータの生成
	void		CreateFontData(int fontSize);
	void		CreateEndingFontData(void);
	void		DeleteEndingFontData(void);

	void		SetFontType(GraphicManager::eFontType fontType) ;

	// モデル
	void		EntryDrawModel(ModelBase* pModelBase);
	void		InsertDrawModel(ModelBase* pModelBase, ModelBase* pTargetModelBase);
	void		RemoveDrawModel(ModelBase* pModelBase);
	void		RemoveDrawModelAll(void);
	void		ReserveEntryModel(ModelBase* pModel);
	void		ReserveRemoveModel(ModelBase* pModel);
	BOOL		IsUpdateDrawModelVector(void)	{ return mIsUpdateDrawModelVector; }

	// モデル(マップモデル描画前)
	void		EntryDrawPreUpdateModel(ModelBase* pModelBase);
	void		RemoveDrawPreUpdateModel(ModelBase* pModelBase);
	void		RemoveDrawPreUpdateModelAll(void);

	// マップ
	void		EntryDrawMapModel(ModelMap* pModelMap);
	void		RemoveDrawMapModel(ModelMap* pModelMap);
	void		RemoveDrawMapModelAll(void);

	void		EntryCursorModel(ModelBase* pModelBase);
	BYTE		IsEntryCursorModel(void);

	void		EntryAfterDrawModel(ModelBase* pModelBase);

	// レイアウト
	void		EntryDrawLayout(LayoutBase* pLayoutBase);
	void		InsertDrawLayout(LayoutBase* pLayoutBase, LayoutBase* pTargetLayoutBase);
	void		RemoveDrawLayout(LayoutBase* pLayoutBase);
	void		RemoveDrawLayoutAll(void);
	bool		CheckEntryDrawLayout(LayoutBase* pLayoutBase);

	// システムレイアウト

	void		EntrySystemLayout(LayoutBase* pLayoutBase, eSystemLayoutPriority lytPrio=eSYS_LAYOUT_PRIORITY_LAST);
	void		RemoveSystemLayout(LayoutBase* pLayoutBase);
	void		RemoveSystemLayoutAll(void);
	bool		CheckEntrySystemLayout(LayoutBase* pLayoutBase);
	void		AssignFadeboardSystemLayout(LayoutBase* pLayoutBase);

	// ビルボード
	void		EntryDrawBillboard(BillboardBase* pBillboardBase);
	void		RemoveDrawBillboard(BillboardBase* pBillboardBase);
	void		ReserveRemoveBillboard(BillboardBase* pBillboardBase);

	// シャドウ
	void		EntryDrawShadow(Shadow* pShadow);
	void		RemoveDrawShadow(Shadow* pShadow);
	void		RemoveDrawShadowAll(void);
	bool		CheckEntryShadow(Shadow* pShadow);

	// 文字・線形描画
	void		RenderMessage(char* CharaName, char* Message[kMESSAGE_LINE_NUM], int setColor=0);
	void		RenderString(eStringType type, char* setString, int setColor=-1);
	void		RenderString(eStringType type, const char* setString, int setColor=-1);
	void		RenderLine(ShapeLine& rLine);
	// デバッグ用描画関数
#ifdef _DEBUG
	void		DbgRenderString(char *String ,int Color);
	//	void		DbgRenderLine(VECTOR* pStart, VECTOR* pEnd, int Color);
	void		DbgRenderLine(ShapeLine* pLine);
	void		DbgRenderLine(VECTOR* pStart, VECTOR* pEnd);
	void		DbgRenderLine(VECTOR* pStart, VECTOR* pEnd, int color);
	void		DbgRenderCapsule(ShapeCapsule* pCapsule);
	void		DbgRenderBox(ShapeBox* pBox);
	void		DbgRenderBoard(ShapeBoard* pBoard);

	void		DbgSetRenderString(bool isRender);
	void		DbgSetRenderGraph(bool isRender);
#endif // _DEBUG

	float		GetLayoutScaleRateX(void)		{ return mLayoutScaleRateX; }
	float		GetLayoutScaleRateY(void)		{ return mLayoutScaleRateY; }

	// アスペクト比による補正値 (Yを1.0に固定した、Xの値を格納)
	float		GetAspectRate(void)				{ return mAspectRate; }
	bool		IsWideScreen(void)				{ return (mAspectRate == 0.75f) ? true : false; }
	
	// フォグ
	void		EnableFogPattern(eFogPattern pattern);
	void		EnableFogPattern(int setR, int setG, int setB, float setNear, float setFar);
	void		DisableFogPattern(void);
	void		PauseFog(void);
	void		ResumeFog(void);
	void		BackupFogParam(void)			{ mBackupFogParam = mFogParam; }
	void		RevertFogParam(float setSec);
	void		ChangeFogParam(float setSec, tFogParam* pFogParam);

	// ライト
	VECTOR&		LightDirection(void)			{ return mLightDirection; }

	void		SetEnableDirectionLight(bool isEnable);
	bool		GetEnableDirectionLight(void);
	void		SetDefaultDirectionLight(void);

	/* ポイントライト */
	void		SetEnablePLight(bool isEnable, ePLightNo lightIndex=ePLIGHT_NO_00);
	void		DisablePLightAll(void);
	bool		GetEnablePLight(ePLightNo lightIndex=ePLIGHT_NO_00);

	void		SetPLightPosition(VECTOR* pPosition, ePLightNo lightIndex=ePLIGHT_NO_00);
	void		SetPLightRangeAtten(float range, float atten0, float atten1, float atten2, ePLightNo lightIndex=ePLIGHT_NO_00);
	void		SetPLightDiffuse(float r, float g, float b, float a, ePLightNo lightIndex=ePLIGHT_NO_00);
	void		SetPLightSpecular(float r, float g, float b, float a, ePLightNo lightIndex=ePLIGHT_NO_00);
	void		SetPLightAmbient(float r, float g, float b, float a, ePLightNo lightIndex=ePLIGHT_NO_00);

	void		SetPLightDirection(VECTOR* pDir, ePLightNo lightIndex=ePLIGHT_NO_00);
	void		SetPLightAngle(float outAngle, float inAngle, ePLightNo lightIndex=ePLIGHT_NO_00);


	void		GetPLightPosition(VECTOR* pPosition, ePLightNo lightIndex=ePLIGHT_NO_00);
	void		GetPLightRangeAtten(float* range, float* atten0, float* atten1, float* atten2, ePLightNo lightIndex=ePLIGHT_NO_00);
	void		GetPLightDiffuse(COLOR_F* pColor, ePLightNo lightIndex=ePLIGHT_NO_00);
	void		GetPLightSpecular(COLOR_F* pColor, ePLightNo lightIndex=ePLIGHT_NO_00);
	void		GetPLightAmbient(COLOR_F* pColor, ePLightNo lightIndex=ePLIGHT_NO_00);

	// オリジナルシェーダー
	void		SetEnableOriginalShader(bool isEnable);
	bool		GetEnableOriginalShader(void);
	void		StartOriginalShader(void);
	void		EndOriginalShader(void);

	// 停止モード
	void		SetStopModeUpdate(bool isStop);
	bool		GetStopModeUpdate(void);

	// 汎用コールバック関数
	void		EntryDrawFontFunc(FUNC_DRAW_FONT func);
	void		EntryLockonFontFunc(FUNC_DRAW_FONT func);

	void DrawFontString( int x , int y , char *String , int Color, int size);
	void DrawFontStringCenter( int x , int y , char *String , int Color, int size);

private:
	GraphicManager();
	~GraphicManager();

	void		updateBloomProcess(void);

	void		testBlurInit(void);
	void		testBlurPreUpdate(void);
	void		testBlurUpdate(void);
	void		testBlurDestroy(void);

#ifdef _DEBUG
	void		dumpDrawModelVector(void);
#endif // _DEBUG

	struct tShapeVector
	{
		std::vector<ShapeLine>			mLine;
		std::vector<ShapeCapsule>		mCapsule;
		std::vector<ShapeBox>			mBox;
		std::vector<ShapeBoard>			mBoard;
	};

	// モデル関連
	std::vector<ModelBase *>			mDrawModelVector;		// 描画モデル
	std::vector<ModelBase *>			mDrawPreUpdateModelVector;	// 描画モデル
	std::vector<ModelMap *>				mDrawModelMapVector;	// 描画モデル
	ModelBase*							mpCursorModel;
	ModelBase*							mpAfterEffectModel;		// エフェクト描画後に描画されるモデル
	BYTE								mIsUpdateDrawModelVector;// mDrawModelVector が更新中かどうか

	// レイアウト関連
	std::vector<LayoutBase *>			mDrawLayoutVector;		// 描画レイアウト
	std::vector<LayoutBase *>			mSystemLayoutVector;	// システム用レイアウト
	std::vector<Shadow *>				mDrawShadowVector;		// 描画シャドウ
	std::vector<tRenderStringWork>		mDrawStringVector;		// 描画文字
	std::vector<ShapeLine>				mShapeLineVector;		// 形状描画(本使用)
#ifdef _DEBUG
	std::vector<tShapeVector>			mDbgShapeVector;		// 形状描画(デバッグ用)
#endif // _DEBUG
	LayoutBase*							mpFadeLayout;

	float								mLayoutScaleRateX;
	float								mLayoutScaleRateY;
	float								mAspectRate;

	std::vector<ModelBase *>			sReserveEntryModel;
	std::vector<ModelBase *>			sReserveRemoveModel;


	bool								mIsStopModeUpdate;

	// フォグ関連
	tFogParam							mFogParam;
	tFogParam							mBackupFogParam;

	// ライト関連
	VECTOR								mLightDirection;
	int									mOtherLightHandle[ePLIGHT_NO_MAX];

	// DxLibシェーダー
	int									mVShaderHandle;
	int									mPShaderHandle;
	BYTE								mIsEnableOriginalShader;

	struct tFongChangeWork {
		bool			mIsActive;
		tFogParam		mDiffParam;
		float			mNowSec;
		float			mMaxSec;
	} mFogChangeWork;

};

#endif // __MODEL_PMD_H__

 /**** end of file ****/
