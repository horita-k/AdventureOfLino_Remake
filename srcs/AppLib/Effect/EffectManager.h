/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * エフェクト管理クラス
 * @author SPATZ.
 * @data   2014/01/28 01:01:45	
 */
//---------------------------------------------------
#ifndef __EFFECT_MANAGER_H__
#define __EFFECT_MANAGER_H__

#include <d3dx9.h>
#include "DxLib.h"

#include "EffectNode.h"
#include "EffectResource.h"
#include "../Graphic/EffectType.h"

/*=====================================*
 * define
 *=====================================*/
#define STOP_EFFECT(hnd)								\
	if (hnd != -1) {									\
		EffectManager::GetInstance()->Stop(hnd, 0.0f);	\
		hnd = -1;										\
	}

/*=====================================*
 * class
 *=====================================*/
class EffectManager 
{
public:
	static const int	kEFFECT_NODE_NUM		= 40;	// エフェクトノード数
	static BYTE			sEffectUpdateSpeed;				// エフェクト更新スピード

	enum eEffectQuality {
		eQUALITY_LOW = 0,
		eQUALITY_MIDDLE,
		eQUALITY_HIGH,

		eQUALITY_MAX,
	};

	/* 関数 */
	static				EffectManager* const GetInstance(void);
	static	void		SetEffectUpdateSpeed(BYTE setSpeed);

	BOOL				Create(int resorceNum);
	void				Update(void);
	void				Destroy(void);

	void				LoadResource(int eEffID, const char* effectName, const char* modelName);
	void				UnloadResource(int eEffID);
	void				DeleteEffect(void);

	int					Play(int eResHandle, VECTOR* pPos, VECTOR* pScale, float fadeSec=0.5f);
	int					Play(int eResHandle, VECTOR* pPos, float allScale, float fadeSec=0.5f);
	void				Stop(int eEffectHandle, float fadeSec=0.5f);
	bool				IsPlay(int eEffectHandle);
	void				StopAll(void);
	void				StopAllSystem(void);
	
	VECTOR&				Position(int eEffectHandle);
	VECTOR&				Rotation(int eEffectHandle);
	VECTOR&				Scale(int eEffectHandle);
	MATRIX&				Matrix(int eEffectHandle);

	void				SetMatrixType(int eEffectHandle, EffectNode::eMatrixType type);

	void				SetAlpha(int eEffectHandle, float fAlpha);

	void				SetFloat(int eEffectHandle, const char* pName, const float fValue);
	void				SetFloat2(int eEffectHandle, const char* pName, const float* pValue);
	void				SetVec4(int eEffectHandle, const char* pName, const FLOAT4* pVec4);
	void				SetVec4(int eEffectHandle, const char* pName, const VECTOR* pVec3);

	void				GetFloat(int eEffectHandle, const char* pName, float* pValue);
	void				GetVec4(int eEffectHandle, const char* pName, FLOAT4* pVec4);

	LPDIRECT3DVERTEXBUFFER9 GetVertexBuffer(void)		{ return mpVertexBuffer; }
	LPDIRECT3DSURFACE9		GetOrgRenderTarget(void)	{ return mpOrgRenderTarget; }
	LPDIRECT3DSURFACE9		GetOrgDepthStencil(void)	{ return mpOrgDepthStencil; }

private:
	EffectManager();
	~EffectManager();

	/* 変数 */
	EffectResource*				mpResource;
	BYTE						mResourceNum;
	EffectNode					mEffectList[kEFFECT_NODE_NUM];

	// 頂点バッファ
	LPDIRECT3DVERTEXBUFFER9		mpVertexBuffer;
	LPDIRECT3DSURFACE9			mpOrgRenderTarget;
	LPDIRECT3DSURFACE9			mpOrgDepthStencil;
};

#endif // __EFFECT_MANAGER_H__

/**** end of file ****/
