/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 画像ベースクラス
 * @author SPATZ.
 * @data   2012/02/18 17:56:09
 */
//---------------------------------------------------
#ifndef __SHADOW_MAP_H__
#define __SHADOW_MAP_H__

#include "ModelBase.h"
#include "ModelMap.h"

/*=====================================*
 * class
 *=====================================*/
class ShadowMap
{
 public:

	enum eShadowMapType {
		eSHADOWMAP_TYPE_NONE = 0,
		eSHADOWMAP_TYPE_CHARA_ONLY,
		eSHADOWMAP_TYPE_CHARA_STAGE, // 未サポート

		eSHADOWMAP_TYPE_MAX,
	};

	static void		Create(void);
	static void		Setup(void);
	static void		Update(void);
	static void		Destroy(void);

	static void		CreateShadowMap(std::vector<ModelBase *>& rList);

	static void		RenderBegin(void);
	static void		RenderEnd(void);

	static void		PauseShadowMap(void);
	static void		ResumeShadowMap(void);

	static void		SetShadowMapType(eShadowMapType type);
	static eShadowMapType GetShadowMapType(void);

	static void		SetBeforeCreateCallback(CALLBACK_BASIC callback);
	static void		SetAfterCreateCallback(CALLBACK_BASIC callback);

 private:

	static BOOL				mIsActive;
	static BYTE				mShadowMapType;
	static CALLBACK_BASIC	mBeforeCreateCallback;
	static CALLBACK_BASIC	mAfterCreateCallback;
};

#endif // __SHADOW_MAP_H__

 /**** end of file ****/

