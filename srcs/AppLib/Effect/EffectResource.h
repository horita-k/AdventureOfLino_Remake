/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * エフェクトリソース管理クラス
 * @author SPATZ.
 * @data   2014/01/28 23:12:38
 */
//---------------------------------------------------
#ifndef __EFFECT_RESOURCE_H__
#define __EFFECT_RESOURCE_H__

#include <d3dx9.h>
#include <windows.h>
#include <vector>

#include "DxLib.h"

#include "../Graphic/EffectType.h"

/*=====================================*
 * class
 *=====================================*/
class EffectResource 
{
public:

	// RENDERCOLORTARGET 識別用の列挙子
	enum {
		eRENDER_COLOR_TARGET_0 = 0,
		eRENDER_COLOR_TARGET_1,
		eRENDER_COLOR_TARGET_2,
		eRENDER_COLOR_TARGET_3,

		eRENDER_COLOR_TARGET_MAX,
	};

	// 描画タイプが Geometory か Buffer か
	enum ePassDrawType {
		ePASS_DRAW_TYPE_GEOMETORY,
		ePASS_DRAW_TYPE_BUFFER,

		ePASS_DRAW_TYPE_MAX,
	};

	// エフェクトのモデルワーク
	struct tModelWork {
		LPD3DXMESH				mpMesh;
		LPD3DXBUFFER			mpAdjucency;
		LPD3DXBUFFER			mpMaterialBuffer;
		LPD3DXBUFFER			mpEffectInstance;
		DWORD					mMaterialsNum;
	};

	// セマンティック情報
	struct tSemanticInfo {
		eSmtKind				mSemKind;		// セマンティク種別の列挙子
		int						mSemIndex;		// .fxのセマンティック番号
	};

	// アノテーション情報
	struct tAnnotationWork {
		const char*				mAnnoName;
		D3DXPARAMETER_TYPE		mParamType;
		void*					mpParam;
	};

	struct tScriptWork {
		eScriptKind				mScriptKind;
		char					mParamString[32];
	};

	/* 関数 */
	EffectResource();
	~EffectResource();

	void				Create(const char* effectName, const char* modelName);
	void				Destroy();

	// セマンティック関連
	int					GetSemanticNum(void);
	int					GetSemanticNum(eSmtKind semanticKind);
	eSmtKind			GetSemanticType(int index);
	D3DXHANDLE			GetSemanticName(eSmtKind semanticKind, int index);

	// スクリプト関連
	int					GetScriptNum(void);
	tScriptWork*		GetScriptWork(int index);

	DWORD				GetMaterialNum(void);
	void				DrawModel(int materialIndex);

	void				GetCloneEffect(ID3DXEffect** ppEffect);
	D3DXHANDLE			GetSemanticHandle(ID3DXEffect* pEffect, int index);
	ePassDrawType		GetPassDrawType(int index);

	bool				IsExistStencilSurface(void);
	void				HoldStencilSurfaceHandle(BYTE& rGetHandle);
	void				ReleaseStencilSurfaceHandle(BYTE& rGetHandle);
	IDirect3DTexture9*	GetRenderColorTargetTexture(BYTE StencilSurfaceHandle, int index);
	IDirect3DSurface9*	GetRenderDepthStencilSurface(BYTE StencilSurfaceHandle);

#ifdef _DEBUG
	void				LogoutEffectInfo(void);
#endif // _DEBUG

private:
	void				initParameters(void);

	bool				loadEff(const char* fileName);
	bool				loadMdl(const char* fileName);
	bool				loadTex(const char* fileName, D3DXHANDLE hParam);

	void				setupTechniqueScript(int techIndex);
	void				setupDrawTypeByPass(int techIndex);

	void				getAnnotationParam(int paramIndex, int annoNum, tAnnotationWork* pAnnoWork, int annoWorkNum);
	DWORD				getD3DFORMAT_FromString(char* format);
	bool				addSemanticInstance(int paramIndex, D3DXPARAMETER_DESC* pParamDesc);

	// エフェクト情報取得関数
	void				getEffectInfo(D3DXEFFECT_DESC* pDesc);
	D3DXHANDLE			getTechInfo(D3DXTECHNIQUE_DESC* pDesc, int techIndex);
	D3DXHANDLE			getTechAnnotationInfo(D3DXPARAMETER_DESC* pDesc, int techIndex);

	D3DXHANDLE			getPassInfo(D3DXPASS_DESC* pDesc, int techIndex, int passIndex);
	D3DXHANDLE			getPassAnnotationInfo(D3DXPARAMETER_DESC* pDesc, int techIndex, int passIndex);

	D3DXHANDLE			getParamInfo(D3DXPARAMETER_DESC* pDesc, int paramIndex);
	D3DXHANDLE			getParamAnnotationInfo(D3DXPARAMETER_DESC* pAnnoDesc, int paramIndex, int annoIndex);

	/*=====================================*
	 * リソース固有のインスタンス生成関数
	 *=====================================*/
	void resourceSMT_RENDERCOLORTARGET(int targetIndex, int paramIndex, D3DXPARAMETER_DESC* pParamDesc, tSemanticInfo* pSmtInfo);
	void resourceSMT_RENDERDEPTHSTENCILTARGET(int paramIndex, D3DXPARAMETER_DESC* pParamDesc, tSemanticInfo* pSmtInfo);


	/* 変数 */
	ID3DXEffect*						mpEffect;
	LPD3DXBUFFER						mpBuffer;
	tModelWork							mModelWork;

	std::vector<IDirect3DTexture9 *>	mTextureList;
	BYTE								mStencilHandleUseBit;
	struct tStencilInfo {
		IDirect3DTexture9*				mpRenderColorTarget[eRENDER_COLOR_TARGET_MAX];
		IDirect3DSurface9*				mpRenderDepthStencilTarget;
	};
	tStencilInfo*						mpStencilInfo;

	std::vector<tSemanticInfo *>		mSemanticList;
	std::vector<tScriptWork *>			mScriptList;
	std::vector<ePassDrawType>			mPassDrawTypeList;

	// エフェクト情報
	BYTE								mParameterNum;
	BYTE								mTechniqueNum;
	BYTE								mFunctionNum;
	BYTE*								mpPassNum;

};

#endif // __EFFECT_RESOURCE_H__

/**** end of file ****/
