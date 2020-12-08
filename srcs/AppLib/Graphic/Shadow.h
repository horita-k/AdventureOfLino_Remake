/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
#ifndef __SHADOW_H__
#define __SHADOW_H__

/*=====================================*
 * include
 *=====================================*/
#include "dxlib.h"

//class CharaBase;
class ModelCollision;

/*=====================================*
 * class
 *=====================================*/
class Shadow {
	
 public:

	enum eUpdateType {
		eUPDATE_TYPE_ALWAYS = 0,
		eUPDATE_TYPE_ONCE,

		eUPDATE_TYPE_MAX,
	};

	static const int kRIPPLE_NUM = 4;			// 表示する水面の数
	static const int kBACKUP_POLYGON_NUM = 8;	// Vertexデータの最大保持数 (８つ以上ポリゴンを含む場合は捨てる)

	struct tRippleWork {
		VECTOR		mVector[kBACKUP_POLYGON_NUM][3];
		VECTOR		mCharaOldPos;
		VECTOR		mNormal[kBACKUP_POLYGON_NUM];
		float		mSpeed;
		int			mVtxNum;
		int			mCount;
	};

	Shadow();
	~Shadow();

	void		Setup(ModelCollision* pChara, eUpdateType updateType);
	void		Update(void);
	void		UpdateNotRender(void);

	void		EnableShadow(void);
	void		DisableShadow(void);

	bool		IsRenderShadow(void)			{ return mIsRenderShadow; }
	VECTOR&		NormalVec(void)					{ return mNormalVec; }

	void		SetupOnceDrawShadow(void);
	void		SetUpdateType(eUpdateType updateType)			{ mUpdateType = updateType; }
	void		SetShadowVector(VECTOR moveVec);

	void		SetShadowHeightOffset(float offset)				{ mShadowHeightOffset = offset; }

	static	void EnableShadowMap(BOOL bEnable)					{ sEnableShadowMap = bEnable; }

 private:

	void		initialize(void);

	void		updateShadowVertexInfo(VERTEX3D* pVertex, MV1_COLL_RESULT_POLY* pHitRes, float shadowSize, VECTOR* pCharaPos);
	/* 変数 */
	typedef void (Shadow::*UPDATE_FUNC)(void);
	static UPDATE_FUNC mUpdateFunc[eUPDATE_TYPE_MAX];
	void		updateFuncAlways(void);
	void		updateFuncOnce(void);

	ModelCollision*		mpChara;
	bool				mIsRenderShadow;
	VECTOR				mNormalVec;
	eUpdateType			mUpdateType;

	// 波紋情報
	float				mRippleCreateCount;		// 波紋生成カウント
	tRippleWork*		mpRippleWork;			// 波紋情報構造体
	int					mRippleIndex;			// 波紋のインデックス

	// 一度だけ影を描画する為の構造体
	tRippleWork			mOnceDrawWork;
	float				mShadowHeightOffset;
	static BYTE			sEnableShadowMap;

};

#endif // __SHADOW_H__

 /**** end of file ****/
