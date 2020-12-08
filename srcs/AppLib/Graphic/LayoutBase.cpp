/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 画像ベースクラス
 * @author SPATZ.
 * @data   2012/02/18 17:56:36
 */
//---------------------------------------------------
#include "AppLib/Basic/Basic.h"
#include "AppLib/Resource/ResourceLayout.h"
#include "AppLib/Graphic/GraphicManager.h"

#include "LayoutBase.h"

static float sAspectRate	= 0.0f;
static float sLytScaleRateX = 0.0f;
static float sLytScaleRateY = 0.0f;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
LayoutBase::SetupLayoutInfo(float aspectRate, float lytScaleRateX, float lytScaleRateY)
{
	sAspectRate		= aspectRate;
	sLytScaleRateX	= lytScaleRateX;
	sLytScaleRateY	= lytScaleRateY;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
LayoutBase::LayoutBase()
{
	// initialize
	mPositionX	= 0;
	mPositionY	= 0;
	mRotation	= 0.0f;
	mAlpha		= 255;
	mRed		= 255;
	mGreen		= 255;
	mBlue		= 255;

	mIsFlip		= false;

	mContinueDraw.mNum			= 1;
	mContinueDraw.mIntervalX	= 0;
	mContinueDraw.mIntervalY	= 0;

	memset(&mFadeAlpha, 0x00, sizeof(mFadeAlpha));

	mHalfRateWidth = 0;
	mHalfRateHeight = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
LayoutBase::~LayoutBase()
{
	Delete();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutBase::Create(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutBase::Setup(void)
{
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutBase::Update(void)
{
	if (mLayoutHandle == -1) return;
	if ((mAlpha == 0) && (mFadeAlpha.mIsActive == false)) return;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, mAlpha);

	// 描画輝度を赤のみにセット
	SetDrawBright(mRed, mGreen, mBlue);

	if (mFadeAlpha.mIsActive) {
		int work = (int)mAlpha;
		work += mFadeAlpha.mAddValue;
		if (((mFadeAlpha.mAddValue > 0) && (work >= mFadeAlpha.mTargetAlpha)) ||
			((mFadeAlpha.mAddValue < 0) && (work <= mFadeAlpha.mTargetAlpha))) {
			work = (int)mFadeAlpha.mTargetAlpha;
			mFadeAlpha.mIsActive = false;
		}
		mAlpha = (unsigned char)work;
	}

	Vec2 pos;
	Vec2 leUp, riUp, leDw, riDw;
	Vec2 leUpAft, riUpAft, leDwAft, riDwAft;
	Vec2* p_befoer;
	Vec2 tmp;

	for (int iI = 0; iI < mContinueDraw.mNum; iI ++) {

		pos.x = (int)((mPositionX + (mContinueDraw.mIntervalX * iI)) * sLytScaleRateX);
		pos.y = (int)((mPositionY + (mContinueDraw.mIntervalY * iI)) * sLytScaleRateY);

		/* スケーリング */
		leUp = Vec2(-mHalfRateWidth,-mHalfRateHeight);
		riUp = Vec2(mHalfRateWidth,	-mHalfRateHeight);
		leDw = Vec2(-mHalfRateWidth,mHalfRateHeight);
		riDw = Vec2(mHalfRateWidth, mHalfRateHeight);

		if (mRotation != 0.0f) {
			/* 回転 (アフィン変換) */
			p_befoer = &leUp;
			tmp.x = (int)((cos(mRotation) * (float)p_befoer->x) - (sin(mRotation) * (float)p_befoer->y));
			tmp.y = (int)((sin(mRotation) * (float)p_befoer->x) + (cos(mRotation) * (float)p_befoer->y));
			*p_befoer = tmp;

			p_befoer = &riUp;
			tmp.x = (int)((cos(mRotation) * (float)p_befoer->x) - (sin(mRotation) * (float)p_befoer->y));
			tmp.y = (int)((sin(mRotation) * (float)p_befoer->x) + (cos(mRotation) * (float)p_befoer->y));
			*p_befoer = tmp;

			p_befoer = &leDw;
			tmp.x = (int)((cos(mRotation) * (float)p_befoer->x) - (sin(mRotation) * (float)p_befoer->y));
			tmp.y = (int)((sin(mRotation) * (float)p_befoer->x) + (cos(mRotation) * (float)p_befoer->y));
			*p_befoer = tmp;

			p_befoer = &riDw;
			tmp.x = (int)((cos(mRotation) * (float)p_befoer->x) - (sin(mRotation) * (float)p_befoer->y));
			tmp.y = (int)((sin(mRotation) * (float)p_befoer->x) + (cos(mRotation) * (float)p_befoer->y));
			*p_befoer = tmp;
		}

		/* 平行移動 */
		leUp += pos;
		riUp += pos;
		leDw += pos;
		riDw += pos;

		DrawModiGraph(leUp.x, leUp.y,
					  riUp.x, riUp.y,
					  riDw.x, riDw.y,
					  leDw.x, leDw.y,
					  mLayoutHandle, TRUE);
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutBase::Destroy(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutBase::LinkLayout(const char* fileName)
{
	mLayoutHandle = ResourceLayout::GetInstance()->GetHandle(fileName);
	APP_ASSERT_PRINT(mLayoutHandle != -1,
					 "Failed LinkLayout : %s \n", fileName);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutBase::UnlinkLayout(void)
{
	mLayoutHandle = -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutBase::Delete(void)
{
	if (mLayoutHandle != -1) {
		mLayoutHandle = -1;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutBase::SetPosition(int x, int y)
{
	mPositionX = x;
	mPositionY = y;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutBase::GetPosition(int* x, int* y)
{
	*x = mPositionX;
	*y = mPositionY;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutBase::SetPositionByCentering(int x, int y)
{
	// アスペクト比を考慮した値
	mPositionX = (int)( (float)x / sAspectRate );
	mPositionY = y;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutBase::GetPositionByCentering(int* x, int* y)
{
	// アスペクト比を考慮した値
	*x = (int)(mPositionX * sAspectRate);
	*y = (int)mPositionY;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutBase::PadRightPos(void)
{
	int wnd_right_pos;
	wnd_right_pos = (int)(640.0f / sAspectRate);

	int posx, posy, sizex, sizey;
	GetPositionByCentering(&posx, &posy);
	GetSize(&sizex, &sizey);

	int diff_pos = 640 - (posx + (sizex / 2) );
	SetPosition(wnd_right_pos - diff_pos - (sizex / 2),	posy);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutBase::SetAlpha(BYTE alpha)
{
	mAlpha = alpha;

	// フェード処理は停止させる
	mFadeAlpha.mIsActive = false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutBase::SetSize(int width, int height)
{
	mHalfRateWidth = (int)((width / 2) * sLytScaleRateX);
	mHalfRateHeight = (int)((height / 2) * sLytScaleRateY);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutBase::GetSize(int* pWidth, int* pHeight)
{
	*pWidth		= (int)((2.0f * (float)mHalfRateWidth) / sLytScaleRateX);
	*pHeight	= (int)((2.0f * (float)mHalfRateHeight) / sLytScaleRateY);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutBase::AddSize(int addWidth, int addHeight, int maxWidth, int maxHeight)
{
	int width, height;

	width	= (int)((2.0f * (float)mHalfRateWidth) / sLytScaleRateX);
	height	= (int)((2.0f * (float)mHalfRateHeight) / sLytScaleRateY);

	width	+= addWidth;
	if (width > (maxWidth * sLytScaleRateX)) {
		width = (int)(maxWidth * sLytScaleRateX);
	}

	height	+= addHeight;
	if (height >= maxHeight) {
		height = maxHeight;
	}

	mHalfRateWidth = (int)((width / 2) * sLytScaleRateX);
	mHalfRateHeight = (int)((height / 2) * sLytScaleRateY);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutBase::SettingContinuDraw(int num, int x, int y)
{
	if (num < 0) num = 0;
	mContinueDraw.mNum			= num;
	mContinueDraw.mIntervalX	= x;

	mContinueDraw.mIntervalX 	= x;
	mContinueDraw.mIntervalY	= y;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutBase::StartFade(unsigned char targetAlpha, float second)
{
	float frame = second * 60.0f;
	int diff = targetAlpha - mAlpha;

	if (diff != 0) {
		mFadeAlpha.mIsActive	= true;
		mFadeAlpha.mTargetAlpha = targetAlpha;
		mFadeAlpha.mAddValue	= (int)((float)diff / frame);
		if (mFadeAlpha.mAddValue == 0) {
			if (diff > 0) {
				mFadeAlpha.mAddValue = 1;
			} else {
				mFadeAlpha.mAddValue = -1;
			}
		}
	}
}

 /**** end of file ****/
