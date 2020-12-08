/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * プレイヤーベース
 * @author SPATZ.
 * @data   2011/11/27 11:15:20
 */
//---------------------------------------------------
#ifndef __CHARA_BASE_H__
#define __CHARA_BASE_H__

#include "../AppLib/Graphic/ModelCollision.h"
#include "AppLib/Graphic/Shadow.h"
#include "Object/BladeEffect.h"

class ObjectHand;


/*=====================================*
 * class
 *=====================================*/
class CharaBase : public ModelCollision {
 public:

	/*定義*/
	enum {
		// 0
		eCHARA_STATE_IDLE = 0,
		eCHARA_STATE_TURN,
		eCHARA_STATE_MOVE,
		eCHARA_STATE_JUMP_UP,
		eCHARA_STATE_JUMP_DOWN,
		// 5
		eCHARA_STATE_JUMP_END,
		eCHARA_STATE_DAMAGE,
		eCHARA_STATE_RETIRE_START,
		eCHARA_STATE_RETIRE_DOING,
		eCHARA_STATE_RETIRE_END,
		// 10
		eCHARA_STATE_SLOPE_DOWN,
		eCHARA_STATE_REFUSE_INPUT,		//	eCHARA_STATE_RESERVE_1,
		eCHARA_STATE_DOWN_START,		//	eCHARA_STATE_RESERVE_2,
		eCHARA_STATE_DOWN_DOING,		//	eCHARA_STATE_RESERVE_3,
		eCHARA_STATE_DOWN_END,			//	eCHARA_STATE_RESERVE_4,
		// 15
		eCHARA_STATE_FALL_DOWN,			//		eCHARA_STATE_RESERVE_5,
		eCHARA_STATE_RESERVE_6,
		eCHARA_STATE_RESERVE_7,
		eCHARA_STATE_RESERVE_8,
		eCHARA_STATE_RESERVE_9,
		// 20
		eCHARA_BASE_STATE_MAX,
	};

	enum eTurnType {
		eTURN_TYPE_NONE = -1,

		eTURN_TYPE_UP = 0,
		eTURN_TYPE_UP_RIGHT,
		eTURN_TYPE_RIGHT,
		eTURN_TYPE_DOWN_RIGHT,
		eTURN_TYPE_DOWN,
		eTURN_TYPE_DOWN_LEFT,
		eTURN_TYPE_LEFT,
		eTURN_TYPE_UP_LEFT,
		
		eTURN_TYPE_MAX,
	};

	enum eEarthState {
		eEARTH_STATE_LINE = 0,
		eEARTH_STATE_SLOPE,
		eEARTH_STATE_CAPSULE,
		eEARTH_STATE_SKY,

		eEARTH_STATE_MAX,
	};

	/*関数*/
	CharaBase();
	virtual ~CharaBase();

	void				Create(void);
	void				Setup(bool isDefaultMotioon=false);
	virtual void		Update(void);
	virtual void		Destroy(void);

	void				LinkCharaResource(const char* charaName);

	virtual void		ChangeState(int setState)=0;
	virtual	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);
	virtual void		UpdateAttackChara(void)=0;

	virtual void		SetObjectHandModel(ObjectHand* pObjectHand) { };
	virtual ObjectHand*	GetObjectHandModel(void)					{ return NULL; }

	int					GetState(void)					{ return mCharaState; }
	int					GetMaxHitpoint(void)			{ return mMaxHitpoint; }
	int					GetHitpoint(void)				{ return mHitpoint; }
	void				SetHitpoint(int point)			{ mHitpoint = point; }
	void				AddHitPoint(int addValue);

	bool				GetIsActive(void)				{ return mIsActive; }
	VECTOR&				MoveVec(void)					{ return mMoveVec; }
	bool				GetIsAttack(void)				{ return mIsAttack; }
	bool				GetIsLand(void)					{ return mIsLand; }
	void				SetNowGravity(float value) 		{ mGravity = value; }
	float				GetNowGravity(void)				{ return mGravity; }
	void				SetGravitySpeed(float speed)	{ mConstGravitySpeed = speed; }
	float				GetConstGravitySpeed(void)		{ return mConstGravitySpeed; }
	BYTE&				EarthMaterialIndex(void)		{ return mEarthMaterialIndex; }
	void				SetJumpMoveVec(VECTOR* pMoveMove) { mJumpMoveVec.mDash = *pMoveMove; }
	float				GetRunSpeed(void)				{ return mRunSpeed; }

	void				EnableShadow(void);
	void				DisableShadow(void);
	void				SetShadowUpdateType(Shadow::eUpdateType updateType)		{ mShadow.SetUpdateType(updateType); }

	void				GetBodyCapsule(ShapeCapsule* pCapsule);

	void				SetPositionOnCollision(void);
	bool				GetIsRetire(void);
	INT8				GetOnCollisionIndex(void)		{ return mOnCollisionIndex; }
	bool				CheckEnableAttack(void);
	void				AttachOcarina(ModelPmd* pAttachModel);

protected:
	void				updateAttackAllObject(void);	// 攻撃の当たり判定
	virtual bool		updateSideCollision(bool is2nd);// 壁との当たり判定・改
	void				updateNewEarthCollision(void);	// 新・地面との当たり判定

	eTurnType			updateTargetRotateFromPad(void);
	void				changeStateStatus(void);
	void				initialize(void);

	virtual void		processLandingFunc(void) {}
	virtual void		updateLastEarthPos(void)=0;		// mLastEarthPos更新処理

	/*=====================================*
	 * 変数
	 *=====================================*/
	DWORD				mCharaState;
	float				mGravity;		
	float				mConstGravitySpeed;
public:
	VECTOR				mNormalVec;
protected:
	float				mTargetRotY;
	int					mTurnType;
	union {
		VECTOR			mDash;
		VECTOR			mLanding;
		VECTOR			mDamageDown;
	} mJumpMoveVec;
	bool				mIsLand;
	bool				mIsSlide;

	bool				mIsActive;
	bool				mIsAttack;
	bool				mIsReserveComboInput;
	BOOL				mIsOnGimmick;

	float				mRunSpeed;
	VECTOR				mPrevPosition;
	VECTOR				mLastEarthPos;

	int					mMaxHitpoint;
	int					mHitpoint;
	int					mAttackPower;

	char				mCharaName[16];

	SHORT				mFlinchCount;			// ダメージ状態になるカウンタ
	SHORT				mFlinchMax;				// ダメージ状態になるカウンタの上限値

	Shadow				mShadow;
	BladeEffect			mBladeEffect;

	VECTOR				mPrevCliffVec;			// 以前の崖際ベクトル
	eEarthState			mEarthState;
	
	BYTE				mEarthMaterialIndex;
	INT8				mOnCollisionIndex;

};

#endif // __CHARA_BASE_H__

/**** end of file ****/

