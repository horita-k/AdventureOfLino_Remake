/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * モーションファイルの定義	
 * @author SPATZ.
 * @data   2011/11/27 11:43:56
 */
//---------------------------------------------------
#ifndef __MODEL_TYPE_H__
#define __MODEL_TYPE_H__

struct tMotionParam {
	int					mIndex;
	bool				mIsLoop;
	float				mSpeed;
	const tMotionParam*	mpNextMotion;
};

#endif // __MODEL_TYPE_H__

 /**** end of file ****/

