/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * PMDモデルクラス
 * @author SPATZ.
 * @data   2011/11/20 22:59:08
 */
//---------------------------------------------------
#ifndef __MODEL_PMD_H__
#define __MODEL_PMD_H__

#include "ModelBase.h"
#include "ModelType.h"

/*=====================================*
 * class
 *=====================================*/
class ModelPmd : public ModelBase
{
 public:
	static const char kATTACH_MODEL_NUM		= 4;

	enum eAttachedModelType {
		eATTACHED_MODEL_NORMAL = 0,
		eATTACHED_MODEL_MOTION,

		eATTACHED_MODEL_MAX,
	};

	enum eAnimationType {
		eANIMATION_TYPE_MOTION = 0,
		eANIMATION_TYPE_MORPHING,
		eANIMATION_TYPE_NONE,

		eANIMATION_TYPE_MAX,
	};

	struct tAttachWork {
		ModelBase*			mpAttachedModel;
		eAttachedModelType	mAttachedModelType;
		SHORT				mFrameIndex;
		BYTE				mIsActive;
	};


	ModelPmd();
	virtual ~ModelPmd();

	virtual void		Create(void);
	virtual void		Setup(bool isDefaultMotioon=false);
	virtual void		Update(void);
	virtual void		Destroy(void);

	virtual void		DeleteModel(void);

	void		StartMorph(int animIndexA, int animIndexB, float blendSpeed);
	void		SetupMorphType(float startBlend);

	void		StartMotion(int animIndex, bool isLoop, float animSpeed, const tMotionParam* pNextMotion=NULL, float blendSpeed=0.1f);
	void		StartMotionForcibly(int animIndex, bool isLoop, float animSpeed, const tMotionParam* pNextMotion=NULL, float blendSpeed=0.1f);
	void		StartMotionEx(const tMotionParam* pParam);
	void		PauseMotion(void);
	void		ResumeMotion(void);
	void		ReplayMotion(void);
	bool		GetIsEndMotion(void);

	float		GetMotionNowTime(void)			{ return mMotion.mNowTime; }
	void		SetMotionNowTime(float setTime)	{ mMotion.mNowTime = setTime; }
	float		GetMotionTotalTime(void)		{ return mMotion.mTotalTime; }
	int			GetMotionTimePercent(void);
	int			GetMotionIndex(void)			{ return mMotion.mIndex; }
	bool		GetIsMotionBlend(void)			{ return mMotion.mIsBlend; }
	void		SetMotionSpeed(float speed)		{ mMotion.mSpeed = speed; }

	bool		EqualPhaseFrame(float frame);
	bool		BetweenPhaseFrame(float frameMin, float frameMax);

	void		AttachModel(ModelBase* pAttachedModel, eAttachedModelType attachedModelType, int frameIndex);
	void		ChangeAttachModelFrameIndex(int attachIndex, int frameIndex);
	void		ChangeAttachModelPtr(int attachIndex, ModelBase* pModel);

	void		DeattachModelAll(void);
	void		PauseAttach(int attachIndex);
	void		ResumeAttach(int attachIndex);
	int			GetFrameIndexFromName(const char* frameName);
	bool		GetFrameMatrixFromName(MATRIX* pOut, const char* frameName);
	bool		GetFrameMatrixFromNameByScale(MATRIX* pOut, const char* frameName, float setScale);
	bool		GetFrameMatrixFromIndex(MATRIX* pOut, BYTE frameIndex);
	bool		GetFramePosFromName(VECTOR* pOut, const char* frameName);
	bool		GetFramePosFromIndex(VECTOR* pOut, BYTE frameIndex);

	// ユーザー設定マトリックス
	void		SetFrameUserMatrix(const char* frameName, VECTOR* pTrans, VECTOR* pRotPos, VECTOR* pRotTar, VECTOR* pScale);
	void		SetFrameUserRotation(const char* frameName, VECTOR* pRotTar, float sensitiveRate);
	void		SetFrameUserRotation(const char* frameName, VECTOR* pRotTar, VECTOR* pUpVec, float sensitiveRate, MATRIX* pOutParentInvMtx=NULL);
	void		SetFrameUserRotationEx(int childFrameIndex, VECTOR& rTargetRot, VECTOR& rUpVec, float sensitiveRate, MATRIX& rInvMatrix);
	void		ResetFrameUserMatrix(const char* frameName);

	void		ResetMotion(void);

	// モーフィング関連
	int				GetAttachedNum(void);
	tAttachWork*	GetAttachAddr(int attachedIndex);

	void			DumpParam(void);

	//private:
protected:

	void		phaseAnimationMotion(void);
	void		phaseAnimationMorphing(void);
	void		phaseAnimationNone(void);

	/* 変数 */
	typedef		void (ModelPmd::*PHASE_ANIMATION)(void);
	static		PHASE_ANIMATION mPhaseAnimation[eANIMATION_TYPE_MAX];

protected:

	struct tMotionWork {
		int*	mpHandle;
		float	mBlendRate;
		SHORT	mIndex;

		float	mPrevBlendRate;
		SHORT	mPrevIndex;

		float	mNowTime;
		float	mTotalTime;
		bool	mIsLoop;
		float	mSpeed;
		float	mBlendSpeed;
		bool	mIsEnd;
		bool	mIsBlend;
		float	mPauseSpeed;

		tMotionParam*	mpNextMotion;
	} mMotion;

	tAttachWork			mAttach[kATTACH_MODEL_NUM];

	unsigned char		mAttachCount;
	eAnimationType		mAnimationType;

};

#endif // __MODEL_PMD_H__

 /**** end of file ****/
