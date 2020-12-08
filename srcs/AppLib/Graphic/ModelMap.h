/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * エフェクトベースクラス
 * @author SPATZ.
 * @data   2012/04/29 09:27:17
 */
//---------------------------------------------------
#ifndef __MODEL_MAP_H__
#define __MODEL_MAP_H__

#include <d3dx9.h>
#include "DxLib.h"
#include "ModelBase.h"
#include "EffectType.h"

/*=====================================*
 * class
 *=====================================*/
class ModelMap : public ModelBase
{
 public:

	static const int kBARRIER_NUM = 10;

	ModelMap();
	~ModelMap();

	void				Setup(float scale);
	void				Update(void);
	void				Destroy(void);
	void				DeleteModel(void);

	void				RenderAlphaFrame(void);

	void				VisibleBattleBox(void);
	void				UnvisibleBattleBox(void);

	bool				GetFrameMatrixFromIndex(MATRIX* pOut, int frameIndex);
	bool				GetFramePosFromIndex(VECTOR* pOut, int frameIndex);

	float&				WaterOffsetPos(void)	{ return mWaterOffsetPos; }
	
	void				SetFramePosFromIndex(int frameIndex, VECTOR* pSetPos);
	void				EntryBarrier(int frameIndex);

	float				GetWaveHeight(void)	{ return mWaveHeight; }

	void				DrawShadowMapFrame(void);

 private:
	/* 変数 */
	enum eBattleboxFadeState {
		eBTLBOX_FADE_NONE = 0,
		eBTLBOX_FADE_IN,
		eBTLBOX_FADE_USE,
		eBTLBOX_FADE_OUT,

		eBTLBOX_FADE_MAX,
	};
	struct tBattleboxWork {
		eBattleboxFadeState	mFadeState;
		float				mAlphaRate;		/* 0.0 -> 1.0f */
	} mBattleboxWork;

	float				mWaterOffsetPos;

	int					mBarrierFrameIndex[kBARRIER_NUM];
	int					mBarrierFrameNum;

	float				mWaveHeight;
	BYTE				mBarrierRand;


};

#endif // __MODEL_MAP_H__

 /**** end of file ****/

