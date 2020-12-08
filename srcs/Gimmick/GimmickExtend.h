/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ギミッククラス
 * @author SPATZ.
 * @data   2014/04/05 17:13:58
 */
//---------------------------------------------------
#ifndef __GIMMICK_EXTEND_H__
#define __GIMMICK_EXTEND_H__

#include "dxlib.h"
#include "AppLib/Graphic/ModelCollision.h"
#include "GimmickBase.h"
#include "AppLib/Resource/CsvAccesser.h"
#include "Object/ObjectWeapon.h"

/*=====================================*
 * Class GimmickExtend_Ship
 *=====================================*/
class GimmickExtend_Ship : public GimmickBase {
public:
	/* 関数 */
	GimmickExtend_Ship();

private:
	void	extendSetup(void);
	void	extendUpdate(void);

	/* 変数 */
	UINT	mIsMove;

	float	mPeriod;
	VECTOR	mTargetVec;
	float	mSpeed;
};

/*=====================================*
 * Class GimmickExtend_Motion
 *=====================================*/
class GimmickExtend_Motion : public GimmickBase {
public:
	enum {
		eCSVLINE_MOTION_NO = 0,
		eCSVLINE_MOTION_LOOP,
		eCSVLINE_MOTION_SPEED,

		eCSVLINE_MAX,
	};

	/* 関数 */
	GimmickExtend_Motion();

private:
	void	extendSetup(void);
	void	extendUpdate(void);

	/* 変数 */
	UINT	mIsMove;

	float	mPeriod;
	VECTOR	mTargetVec;
	float	mSpeed;
};

/*=====================================*
 * GimmickExtend_Roll
 *=====================================*/
class GimmickExtend_Roll : public GimmickBase {
public:
	enum {
		eCSVLINE_ROLLVEC_X = 0,
		eCSVLINE_ROLLVEC_Y,
		eCSVLINE_ROLLVEC_Z,
		eCSVLINE_WAKEUP_EVENT,

		eCSVLINE_MAX,
	};

	/* 関数 */
	GimmickExtend_Roll();

protected:
	void		extendSetup(void);
	void		extendUpdate(void);
};

/*=====================================*
 * GimmickExtend_RollDamage
 *=====================================*/
class GimmickExtend_RollDamage : public GimmickExtend_Roll {
public:
	enum {
		eCSVLINE_ROLLVEC_X = 0,
		eCSVLINE_ROLLVEC_Y,
		eCSVLINE_ROLLVEC_Z,
		eCSVLINE_WAKEUP_EVENT,

		eCSVLINE_MAX,
	};

	/* 関数 */
	GimmickExtend_RollDamage();

private:
	void		extendSetup(void);
	void		extendUpdate(void);
};

/*=====================================*
 * GimmickExtend_Clock
 *=====================================*/
class GimmickExtend_Clock : public GimmickBase {
public:
	enum {
		eCSVLINE_ROLLVEC_X = 0,
		eCSVLINE_ROLLVEC_Y,
		eCSVLINE_ROLLVEC_Z,
		eCSVLINE_PERIOD,

		eCSVLINE_MAX,
	};

	/* 関数 */
	GimmickExtend_Clock();
	void		DeleteModel(void);

private:
	void		extendSetup(void);
	void		extendUpdate(void);

	float		mCounter;
	ModelPmd	mVisibleModel;
	VECTOR		mUpVec;
};

/*=====================================*
 * GimmickExtend_Lift
 *=====================================*/
class GimmickExtend_Lift : public GimmickBase {
public:
	enum {
		eCSVLINE_TARGET_POS_X = 0,
		eCSVLINE_TARGET_POS_Y,
		eCSVLINE_TARGET_POS_Z,
		eCSVLINE_MAX_SPEED,
		eCSVLINE_WAKEUP_EVENT,

		eCSVLINE_MAX,
	};

	enum {
		eLIFT_STATE_NONE = 0,
		eLIFT_STATE_MOVE_GO,
		eLIFT_STATE_MOVE_STOP,
		eLIFT_STATE_MOVE_RETURN,
		eLIFT_STATE_END,
		
		eLIFT_STATE_MAX,
	};

	/* 関数 */
	GimmickExtend_Lift();
	void		ReviseOnPosition(VECTOR& rMoveVec);

protected:
	void		extendSetup(void);
	void		extendUpdate(void);
};

/*=====================================*
 * GimmickExtend_LiftAuto
 * 乗らなくても自動で動いているリフト
 *=====================================*/
class GimmickExtend_LiftAuto : public GimmickBase {
public:

	enum {
		eCSVLINE_TARGET_POS_X = 0,
		eCSVLINE_TARGET_POS_Y,
		eCSVLINE_TARGET_POS_Z,
		eCSVLINE_MAX_SPEED,
		eCSVLINE_WAKEUP_EVENT,

		eCSVLINE_MAX,
	};

	enum {
		eLIFT_STATE_NONE = 0,
		eLIFT_STATE_MOVE_GO,
		eLIFT_STATE_MOVE_STOP,
		eLIFT_STATE_MOVE_RETURN,
		eLIFT_STATE_END,
		
		eLIFT_STATE_MAX,
	};

	/* 関数 */
	GimmickExtend_LiftAuto();
	void		ReviseOnPosition(VECTOR& rMoveVec);

protected:
	void		extendSetup(void);
	void		extendUpdate(void);

private:
	//	int			getSoundVolume(void);
};

/*=====================================*
 * GimmickExtend_LiftLoop
 *=====================================*/
class GimmickExtend_LiftLoop : public GimmickExtend_Lift {

public:
	/* 関数 */
	GimmickExtend_LiftLoop()	{}

private:
	void		extendSetup(void);
	void		extendUpdate(void);
};

/*=====================================*
 * GimmickExtend_Trolley
 *=====================================*/
class GimmickExtend_Trolley : public GimmickBase {
public: 	
	enum {
		eCSVLINE_TARGET_POS_X = 0,
		eCSVLINE_TARGET_POS_Y,
		eCSVLINE_TARGET_POS_Z,
		eCSVLINE_MAX_SPEED,

		eCSVLINE_MAX,
	};

	enum {
		eTROLLEY_STATE_NONE = 0,
		eTROLLEY_STATE_MOVE_GO,
		eTROLLEY_STATE_MOVE_STOP,
		eTROLLEY_STATE_MOVE_RETURN,
		eTROLLEY_STATE_END,
		
		eTROLLEY_STATE_MAX,
	};

	GimmickExtend_Trolley();
	~GimmickExtend_Trolley();
	void		ReviseOnPosition(VECTOR& rMoveVec);

private:
	void		extendSetup(void);
	void		extendUpdate(void);

	float		mTrolleySpeed;
	USHORT		mTrolleyCount;
};

/*=====================================*
 * GimmickExtend_Tower
 *=====================================*/
class GimmickExtend_Tower : public GimmickBase {
public: 	
	static const BYTE kEFFECT_NUM = 4;

	enum {
		eCSVLINE_TOWER = 0,
		eCSVLINE_MAX,
	};
	GimmickExtend_Tower();
	~GimmickExtend_Tower();
	void		SetEnableActive(BOOL isActive)	{ mIsActive = isActive; }
	void		SetColor(COLOR_F& color);

private:
	void		extendSetup(void);
	void		extendUpdate(void);

	int			mEfsTail[kEFFECT_NUM];
	BOOL		mIsActive;
};

/*=====================================*
 * GimmickExtend_Hole
 *=====================================*/
class GimmickExtend_Hole : public  GimmickBase {
public: 	
	enum {
		eCSVLINE_MOVE_STAGE_NO = 0,
		eCSVLINE_MOVE_PLACE_NO,
		eCSVLINE_FIELDINFO_FLAG_A,
		eCSVLINE_FIELDINFO_FLAG_B,

		eCSVLINE_MAX,
	};
	enum {
		eSTATE_NOT_BREAK = 0,
		eSTATE_BREAKING,
		eSTATE_BREAKED,
		eSTATE_END,

		eSTATE_MAX,
	};

	GimmickExtend_Hole();
	~GimmickExtend_Hole();
	void		Update(void);
	void		Draw(void);
	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType=eATTACK_TYPE_NORMAL);
	void		BreakHole(void)				{ mState = eSTATE_BREAKING; }

private:
	void		extendSetup(void);
	void		extendUpdate(void);
};
void DrawHoleFunc(int model_handle);

/*=====================================*
 * GimmickExtend_Wheel
 *=====================================*/
class GimmickExtend_Wheel : public  GimmickBase {
public: 	
	enum {
		eCSVLINE_WHEEL = 0,
		eCSVLINE_MAX,
	};
	GimmickExtend_Wheel();
	~GimmickExtend_Wheel();

private:
	void		extendSetup(void);
	void		extendUpdate(void);
};

/*=====================================*
 * GimmickExtend_Cannon
 *=====================================*/
class GimmickExtend_Cannon : public  GimmickBase {
public: 	
	enum {
		eCSVLINE_CANNON = 0,
		eCSVLINE_MAX,
	};
	enum {
		eSTATE_HIDE = 0,
		eSTATE_ACTIVE,
		eSTATE_BREAKING,
		eSTATE_BREAKED,
		eSTATE_END,
	};

	enum {
		eACTIVE_PHASE_NONE = 0,
		eACTIVE_PHASE_AIM,
		eACTIVE_PHASE_SHOOT,

		eACTIVE_PHASE_MAX,
	};
	
	GimmickExtend_Cannon();
	~GimmickExtend_Cannon();

	void		PreDraw(void);
	void		PreUpdate(void);
	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType=eATTACK_TYPE_NORMAL);

	char*		GetModelName(void)						{ return (char *)"基地砲台"; }
	
private:
	void		extendSetup(void);
	void		extendUpdate(void);
	
	void		funcActivePhase(void);
	void		updateRotateCannon(void);

	/* 変数 */
	BYTE		mActiveJudgeCounter;	// このカウンターはいずれ GimmickBase が引き継いで全員の面倒を見てくれるはず

	CHAR		mHitpoint;
	BYTE		mActivePhase;
	VECTOR		mCannonTargetPos;
	ObjectWeapon_Missile		mMissileObject;
};

/*=====================================*
 * GimmickExtend_LeanFloor
 *=====================================*/
class GimmickExtend_LeanFloor : public  GimmickBase {
public: 	
	enum {
		eCSVLINE_LEAN_RATE = 0,
		eCSVLINE_MAX,
	};

	GimmickExtend_LeanFloor();
	~GimmickExtend_LeanFloor();

private:
	void		extendSetup(void);
	void		extendUpdate(void);

	VECTOR		mLeanVecZ;
	VECTOR		mLeanVecUp;
	float		mLeanValueY;
#ifdef _DEBUG
// デバッグツール用のオリジナルY座標の値を保持
	float sDebugToolOrgRotationY;
#endif // _DEBUG
};

/*=====================================*
 * Class GimmickExtend_Switch
 *=====================================*/
class GimmickExtend_Switch : public GimmickBase {
public:

	enum {
		eCSVLINE_TASK_VALUE = 0,
		eCSVLINE_MAX,
	};

	/* 関数 */
	GimmickExtend_Switch();
	~GimmickExtend_Switch();

	void ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType=eATTACK_TYPE_NORMAL);
private:
	void	extendSetup(void);
	void	extendUpdate(void);

	/* 変数 */
	
};

/*=====================================*
 * Class GimmickExtend_FootSwitch
 *=====================================*/
class GimmickExtend_FootSwitch : public GimmickBase {
public:
	
	enum {
		eCSVLINE_TASK_VALUE = 0,
		eCSVLINE_MAX,
	};
	enum {
		eSTATE_NONE = 0,
		eSTATE_DOWN_SWITCH,
		eSTATE_ON_MODEL_SWITCH,
		eSTATE_UP_SWITCH,

		eSTATE_MAX,
	};

	/* 関数 */
	GimmickExtend_FootSwitch();
	~GimmickExtend_FootSwitch();
	
private:
	void	extendSetup(void);
	void	extendUpdate(void);

	UCHAR		mOffNodeCount;
};

/*=====================================*
 * GimmickExtend_Teto
 * 大動物
 *=====================================*/
class GimmickExtend_Teto : public GimmickBase {
public:
	enum {
		eCSVLINE_TARGET_POS_X = 0,
		eCSVLINE_TARGET_POS_Y,
		eCSVLINE_TARGET_POS_Z,
		eCSVLINE_MAX_SPEED,
		eCSVLINE_WAKEUP_EVENT,

		eCSVLINE_MAX,
	};

	enum {
		eLIFT_STATE_NONE = 0,
		eLIFT_STATE_MOVE_GO,
		eLIFT_STATE_MOVE_STOP,
		eLIFT_STATE_MOVE_RETURN,
		eLIFT_STATE_END,
		
		eLIFT_STATE_MAX,
	};

	/* 関数 */
	GimmickExtend_Teto();
	void		ReviseOnPosition(VECTOR& rMoveVec);

protected:
	void		extendSetup(void);
	void		extendUpdate(void);

private:
	//	int			getSoundVolume(void);

	UINT	mIsMove;

	float	mPeriod;
	VECTOR	mTargetVec;
	float	mSpeed;
};

/*=====================================*
 * GimmickExtend_TrolleyEx
 *=====================================*/
class GimmickExtend_TrolleyEx : public GimmickBase {
public: 	
	enum {
		eCSVLINE_MAX_SPEED = 0,
		eCSVLINE_START_SPEED,
		eCSVLINE_ACCEL_RATE,
		
		eCSVLINE_CAM_LENGTH,
		eCSVLINE_CAM_WIDTH,
		eCSVLINE_CAM_HEIGHT,
		eCSVLINE_JUMP_RATE_V,
		eCSVLINE_JUMP_RATE_H,

		eCSVLINE_MAX,
	};
	
	enum {
		eTROLLEY_STATE_NONE = 0,
		eTROLLEY_STATE_MOVE_START,
		eTROLLEY_STATE_MOVE_GO,
		eTROLLEY_STATE_END,
		
		eTROLLEY_STATE_MAX,
	};
	static void	SetupTrolleyExCollision(int modelHandle, int frameIndex);
	static void UpdateTrolleyExCollision(void);
	static void	DestroyTrolleyExCollision(void);

	GimmickExtend_TrolleyEx();
	~GimmickExtend_TrolleyEx();
	void Update(void);
	void ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType=eATTACK_TYPE_NORMAL);
	

private:
	static	int sRouteModelHandle;
	static	int sRouteFrameIndex;

	void		extendSetup(void);
	void		extendUpdate(void);

	void		updateRouteCollision(void);
	void		updateSpotLight(void);

	VECTOR		getRouteVectorByPolygon(VECTOR& rP1, VECTOR& rP2, VECTOR& rP3, VECTOR& rHitPos);
	VECTOR		getCenterPosByPolygon(VECTOR* pVertex);
	void		getPolyCenterPos(int polyIndex, VECTOR* pRotationVec, VECTOR* pStart, VECTOR* pEnd);

	typedef void (GimmickExtend_TrolleyEx::*PHASE_STATE)(void);
	static PHASE_STATE mPhaseState[eTROLLEY_STATE_MAX];
	void		phaseStateNone(void);
	void		phaseStateMoveStart(void);
	void		phaseStateMoveGo(void);
	void		phaseStateEnd(void);

	float		mTrolleySpeed;
	USHORT		mTrolleyCount;

	VECTOR		mMoveVec;
	float		mGravity;
	VECTOR		mNormalVec;
	VECTOR		mRealNormalVec;

	float		mPrevRotY;
	VECTOR		mPrevNormalVec;

	BOOL		mIsOnRoute;
	BOOL		mIsOnPlayer;
	WORD		mNotOnPlayerCount;

	VECTOR		mJumpMoveVec;
};

/*=====================================*
 * Class GimmickExtend_PLight
 *=====================================*/
class GimmickExtend_PLight : public GimmickBase {
public:

	enum {
		eCSVLINE_LIGHT_INDEX = 0,
		eCSVLINE_MAX,
	};

	/* 関数 */
	GimmickExtend_PLight();
	~GimmickExtend_PLight();

private:
	void	extendSetup(void);
	void	extendUpdate(void);
};

/*=====================================*
 * Class GimmickExtend_Barrier
 *=====================================*/
class GimmickExtend_Barrier : public GimmickBase {
public:

	enum {
		eCSVLINE_DUMMY = 0,
		eCSVLINE_MAX,
	};

	/* 関数 */
	GimmickExtend_Barrier();
	~GimmickExtend_Barrier();

	void Update(void);
	void Draw(void);

private:
	void	extendSetup(void);
	void	extendUpdate(void);

	/* 変数 */
	float mRotCounter;

};
void DrawBarrierFunc(int model_handle);

/*=====================================*
 * GimmickExtend_AirShip
 *=====================================*/
class GimmickExtend_AirShip : public GimmickBase {
public:
	enum {
		eCSVLINE_MOVEVEC_X = 0,
		eCSVLINE_MOVEVEC_Y,
		eCSVLINE_MOVEVEC_Z,

		eCSVLINE_WAVE_PERIOD,
		eCSVLINE_WAVE_HEIGHT,

		eCSVLINE_MAX,
	};

	/* 関数 */
	GimmickExtend_AirShip();

private:
	void		extendSetup(void);
	void		extendUpdate(void);

	float		mPeriod;
};

/*=====================================*
 * Class GimmickExtend_FloatBlock
 *=====================================*/
class GimmickExtend_FloatBlock : public GimmickBase {
public:

	enum {
		eCSVLINE_TASK_VALUE = 0,
		eCSVLINE_MAX,
	};

	/* 関数 */
	GimmickExtend_FloatBlock();
	~GimmickExtend_FloatBlock();

	void ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType=eATTACK_TYPE_NORMAL);
private:
	void	extendSetup(void);
	void	extendUpdate(void);

	/* 変数 */
	
};

/*=====================================*
 * GimmickExtend_MagicCube
 *=====================================*/
class GimmickExtend_MagicCube : public  GimmickBase {
public: 	
	enum {
		eCSVLINE_MAGIC_CUBE = 0,
		eCSVLINE_MAX,
	};
	GimmickExtend_MagicCube();
	~GimmickExtend_MagicCube();

private:
	void		extendSetup(void);
	void		extendUpdate(void);
};

/*=====================================*
 * GimmickExtend_PressWall
 *=====================================*/
class GimmickExtend_PressWall : public  GimmickBase {
public: 	
	enum {
		eCSVLINE_UP_TARGET_POS_X = 0,
		eCSVLINE_UP_TARGET_POS_Y,
		eCSVLINE_UP_TARGET_POS_Z,

		eCSVLINE_UP_MOVE_RATE,
		eCSVLINE_DOWN_MOVE_RATE,
		eCSVLINE_MOVE_STOP_COUNT,

		eCSVLINE_MAX,
	};

	enum {
		eSTATE_STOP_DOWN,
		eSTATE_MOVE_UP,
		eSTATE_STOP_UP,
		eSTATE_PRESS_DOWN,

		eSTATE_BREAKING,
		eSTATE_BREAKED,
		eSTATE_END,

		eSTATE_MAX,
	};

	GimmickExtend_PressWall();
	~GimmickExtend_PressWall();

	virtual void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType=eATTACK_TYPE_NORMAL);
	void		ReviseOnPosition(VECTOR& rMoveVec);

protected:
	virtual void		extendSetup(void);
	virtual void		extendUpdate(void);

	void		updateHitPlayer(VECTOR& rMoveVec);

	float		mCenterHeight;

};

/*=====================================*
 * GimmickExtend_PressWallHard
 *=====================================*/
class GimmickExtend_PressWallHard : public  GimmickExtend_PressWall {
public: 	

	GimmickExtend_PressWallHard();
	~GimmickExtend_PressWallHard();

	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType=eATTACK_TYPE_NORMAL);

private:
	void		extendSetup(void);
	void		extendUpdate(void);
};


#if 0
/*=====================================*
 * GimmickExtend_xxx (テンプレート)
 *=====================================*/
class GimmickExtend_xxx : public  GimmickBase {
public: 	
	enum {
		eCSVLINE_XXX = 0,
		eCSVLINE_MAX,
	};
	GimmickExtend_xxx();
	~GimmickExtend_xxx();

private:
	void		extendSetup(void);
	void		extendUpdate(void);
};
#endif // 0


#endif // __GIMMICK_EXTEND_H__

 /**** end of file ****/

