/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ブレードエフェクト
 * @author SPATZ.
 * @data   2012/06/22 23:55:41
 */
//---------------------------------------------------
#ifndef __BLADEEFFECT_H__
#define __BLADEEFFECT_H__

/*=====================================*
 * include
 *=====================================*/
#include "dxlib.h"

/*=====================================*
 * class
 *=====================================*/
class BladeEffect
{
 public:

	static const int kPREV_NUM = 12;

	/* 関数 */
	BladeEffect();
	~BladeEffect();

	static void	Setup(int attachEffectIndex);

	void		Play(VECTOR* pTopPos, VECTOR* pBtmPos);
	void		Update(VECTOR* pTopPos, VECTOR* pBtmPos);
	void		Stop(void);

 private:
	
	/* 変数 */
	static int	mAttachEffectIndex;

	int			mEffectIndex;

	VECTOR		mPrevTopPos[kPREV_NUM];
	VECTOR		mPrevBtmPos[kPREV_NUM];

	BYTE		mCurrentIndex;
};

#endif // __BLADEEFFECT_H__

 /**** end of file ****/
