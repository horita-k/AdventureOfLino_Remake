/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * カメラ統括マネージャクラス
 * @author SPATZ.
 * @data   2011/11/21 00:05:54
 */
//---------------------------------------------------
#ifndef __CAMERA_MANAGER_H__
#define __CAMERA_MANAGER_H__

#include <vector>
#include "DxLib.h"
#include "ModelBase.h"
#include "Charactor/CharaBase.h"

static const float kDEFAULT_PERSPECTIVE_FOV	= (60.0f / 180.0f * DX_PI);

#define DEFAULT_NEAR_CLIP		(10.0f * ALL_SCALE)
#define DEFAULT_FAR_CLIP		(150000.0f * ALL_SCALE)

enum eCamState {
	eCAM_STATE_DUMMY = 0,
	eCAM_STATE_NORMAL,
	eCAM_STATE_LOCKON,
	eCAM_STATE_HOOKSHOT,
	eCAM_STATE_BOOMERANG,
	eCAM_STATE_MESSAGE,
	eCAM_STATE_SHOW_MAP,
	eCAM_STATE_RESET,
	eCAM_STATE_ARCHERY,
	eCAM_STATE_LOOK_AROUND,
	eCAM_STATE_TROLLEY,
	
	eCAM_STATE_MAX,
};

/*=====================================*
 * class
 *=====================================*/
class CameraManager
{
 public:

	enum {
		eCAMERA_NORMAL = 0,
		eCAMERA_DELAY,
		eCAMERA_DELAY_TO_NORMAL,
		eCAMERA_MOVE,
		eCAMERA_SHAKE,

		eCAMERA_MAX,
	};

	enum eDelayType {
		eDELAY_SPEED_HIGH = 0,
		eDELAY_SPEED_MIDDLE,
		eDELAY_SPEED_LOW,
		eDELAY_SPEED_VERY_LOW,

		eDELAY_SPEED_MAX,
	};

	enum eCamDistType {
		eCAM_DIST_TYPE_NORMAL = 0,
		eCAM_DIST_TYPE_ARCHERY,

		eCAM_DIST_TYPE_MAX,
	};

	enum eArcheryCamType {
		eARCHERY_CAM_TYPE_FAR,
		eARCHERY_CAM_TYPE_NORMAL,
		eARCHERY_CAM_TYPE_NEAR,

		eARCHERY_CAM_TYPE_MAX,
	};

	struct tPatternTrolleyParam {
		VECTOR mVector;
		VECTOR mUp;
		float  mSpeedRate;
		VECTOR mPrevVector;

		float	mCamLength;
		float	mCamWidth;
		float	mCamHeight;
	};

	static CameraManager* const GetInstance(void);

	void		Create(void);
	void		Setup(void);
	void		Update(void);
	void		Destroy(void);

	VECTOR&		Position()		{ return mPosition; }
	VECTOR&		Target()		{ return mTarget; }
	VECTOR&		Up()			{ return mUp; }
	float&		Length()		{ return mLength; }
	float&		Fov()			{ return mFov; }

	void		StartMoveCamera(float setSec, VECTOR* pEndPos, VECTOR* pEndTar);
	bool		IsMoveCamera(void)				{ return (mCamraType == eCAMERA_MOVE) ? true : false; }
	void		StartShakeCamera(float setSec, float ampWidth, float ampSpeed);
	bool		IsShakeCamera(void)				{ return (mCamraType == eCAMERA_SHAKE) ? true : false; }
	void		StartDeleyCamera(eDelayType type, bool isReturnsNormalModeCaseValueSmall);

	void		StartVibeCamera(float setSec = 0.3f, 
								float setSize = 0.005f, 
								float setSpeed = 0.6f);
	bool		IsVibeCamera(void);
	void		StopVibeCamera(void);

	void		RevertCamera(void);

	// カメラの位置とターゲットの距離を求める
	float		GetCameraDist(void);

	// 遠近法 / 正射影 の設定
	void		SetPerspectiveCamera(float fov);
	void		SetOrthoCamera(float size);

	// クリッピング
	void		SetDefaultNearFarClip(void);
	void		SetNearClip(float value);
	void		SetFarClip(float value);
	void		RevertNearFarClip(void);
	
	float		GetNearClip(void)				{ return mNearClip; }
	float		GetFarClip(void)				{ return mFarClip; }


	// カメラ情報保管
	void		SaveTemporaryCamWork(void);
	void		LoadTemporaryCamWork(void);

	// 弓矢時のカメラ情報
	BYTE		GetArcheryCamType(void)			{ return mArcheryCamType; }

	/*=====================================*
	 * いつかはアプリ側へもっていきたい
	 *=====================================*/
	/* セットパターン用パラメータの設定関数 */
	void		ChangeCameraState(eCamState setState);
	eCamState	GetCameraState(void);
	void		SetPatternCharaBase(ModelCollision* pCharaBase)		{ mpSetPatternCharaBase = pCharaBase; }
	void		SetPatternDist(float dist)						{ mSetPatternDist = dist; }
	void		SetPatternGameBegine(void);
	void		SetPatternCameraByVector(VECTOR& rVector, VECTOR& rUp);

	void		SetTrolleyParam(CameraManager::tPatternTrolleyParam* pParam);
	void		SetPatternArroundPlayer(float camDegY, float camLen, float camPosY, float camTarY);

	void		ResetCameraPosY(void);
	void		SetCamVecZ(VECTOR& rCamVec);

	void		ReviseCamera(VECTOR& rMoveVec);

#ifdef _DEBUG
	void		DumpStatus(void);
#endif // _DEBUG

private:
	typedef void (CameraManager::*CameraFunc)(void);

	static CameraFunc mCamStateFunc[eCAM_STATE_MAX];
	static CameraFunc mCameraFunc[eCAMERA_MAX];

	CameraManager();
	~CameraManager();

	void funcVibe(void);

	void updateCamLength(void);
	void updateCamLengthArchery(void);

	void getCollisionCameraPos(VECTOR* pOutCamPos, VECTOR& rHitPos);

	/*=====================================*
	 * カメラステート
	 *=====================================*/
	void		setPatternDummy(void);
	void		setPatternNormal(void);
	void		setPatternLockon(void);
	void		setPatternHookShot(void);
	void		setPatternBoomerang(void);
	void		setPatternMessage(void);
	void		setPatternShowMap(void);
	void		setPatternReset(void);
	void		setPatternArchery(void);
	void		setPatternLookAround(void);
	void		setPatternTrolley(void);

	/*=====================================*
	 * CameraFunc
	 *=====================================*/
	void		cameraFuncNormal(void);
	void		cameraFuncDelay(void);
	void		cameraFuncDelayToNormal(void);
	void		cameraFuncMove(void);
	void		cameraFuncShake(void);

	/* 変数 */
	struct tShakeWork {
		float	mSec;
		float	mMaxSec;
		float	mAmpWidth;
		float	mAmpSpeed;
	} mShakeWork;

	struct tMoveWork {
		VECTOR	mPositionMoveVec;
		VECTOR	mTargetMoveVec;
		float	mNowSec;
		float	mMaxSec;
	} mMoveWork;

	struct tDelayWork {
		float	mDelaySpeedRate;
		VECTOR	mPrevPosition;
		VECTOR	mPrevTarget;
	} mDelayWork;

	struct tQuakeWork {
		bool	mIsActive;
		float	mCount;

		float	mSetSecond;
		float	mSetSize;
		float	mSetSpeed;

	} mQuakeWork;

	int			mCamraType;

	VECTOR		mCamVecZ;
	VECTOR		mPosition;
	VECTOR		mTarget;
	VECTOR		mUp;
	float		mLength;
	BYTE		mArcheryCamType;

	// クリッピング関係
	float		mNearClip;
	float		mFarClip;

	// テンポラリカメラ情報
	VECTOR		mTmpPos;
	VECTOR		mTmpTar;

	float		mFov;		// 視野角

	/*=====================================*
	 * いつかはアプリ側へもっていきたい
	 *=====================================*/
	eCamState	mCamState;		// カメラステータス
	ModelCollision*	mpSetPatternCharaBase;
	float		mSetPatternDist;

	tPatternTrolleyParam	mPatternTrolleyParam;

};

#endif // __MODEL_PMD_H__

 /**** end of file ****/
