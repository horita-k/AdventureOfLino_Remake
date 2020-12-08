/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 画像ベースクラス
 * @author SPATZ.
 * @data   2012/02/18 17:56:09
 */
//---------------------------------------------------
#ifndef __LAYOUT_BASE_H__
#define __LAYOUT_BASE_H__

#include "DxLib.h"

/*=====================================*
 * class
 *=====================================*/
class LayoutBase
{
 public:

	static void			SetupLayoutInfo(float aspectRate, float lytScaleRateX, float lytScaleRateY);

	LayoutBase();
	~LayoutBase();

	virtual void		Create(void);
	virtual void		Setup(void);
	virtual void		Update(void);
	virtual void		Destroy(void);

	virtual void		LinkLayout(const char* fileName);
	virtual void		UnlinkLayout(void);
	void				Delete(void);

	void				SetPosition(int x, int y);
	void				GetPosition(int* x, int* y);
	void				SetPositionByCentering(int x, int y);
	void				GetPositionByCentering(int* x, int* y);
	void				PadRightPos(void);

	void				SetSize(int width, int height);
	void				GetSize(int* pWidth, int* pHeight);
	void				AddSize(int addWidth, int addHeight, int maxWidth, int maxHeight);
	void				SetAlpha(BYTE alpha);
	BYTE				GetAlpha(void)							{ return mAlpha; }
	void				SetColor(BYTE r, BYTE g, BYTE b)		{ mRed	= r; mGreen	= g; mBlue = b;	}
	float&				Rotation(void)							{ return mRotation; }
	void				SetIsFlip(bool flg)						{ mIsFlip = flg; }
	void				SettingContinuDraw(int num, int x, int y);
	void				StartFade(unsigned char targetAlpha, float second);
	bool				IsFade(void)							{ return mFadeAlpha.mIsActive; }

	struct tContinueDrawWork {
		int				mNum;
		int				mIntervalX;
		int				mIntervalY;
	};

	struct tFadeAlphaWork {
		bool			mIsActive;
		unsigned char	mTargetAlpha;
		int				mAddValue;
	};

 protected:
	int					mLayoutHandle;
	int					mPositionX;
	int					mPositionY;
	float				mRotation;

	BYTE				mAlpha;
	BYTE				mRed;
	BYTE				mGreen;
	BYTE				mBlue;

	bool				mIsFlip;

	tContinueDrawWork	mContinueDraw;
	tFadeAlphaWork		mFadeAlpha;

	int					mHalfRateWidth;
	int					mHalfRateHeight;
	
};

#endif // __LAYOUT_BASE_H__

 /**** end of file ****/

