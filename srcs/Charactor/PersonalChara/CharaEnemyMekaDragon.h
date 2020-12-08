/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * メカドラゴン
 * @author SPATZ.
 * @data   2015/02/07 19:14:53
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_MEKADRAGON_H__
#define __CHARA_ENEMY_MEKADRAGON_H__

#include "Charactor/CharaEnemy.h"
#include "AppLib/Graphic/EffekseerManager.h"

/*=====================================*
 * リペラコプター
 *=====================================*/
class CharaEnemyMekaDragon : public CharaEnemy {
public:
	static const char* strAttackTopNode;
	static const char* strAttackBtmNode;

	static const char* strDamageNodeA;
	static const char* strDamageNodeB;

	enum {
		eMT_ENE_FLYING = 0,
		eMT_ENE_APPEAR,
		eMT_ENE_IDLE,
		eMT_ENE_CANNON_LASER,
		eMT_ENE_THRUST,
		eMT_ENE_LEFT_ATTACK,
		eMT_ENE_RIGHT_ATTACK,

		eMT_ENE_MAX,
	};

	enum {
		eSUB_STATE_LOITER = 0,
		eSUB_STATE_RUSH_START,
		eSUB_STATE_RUSH_DOING,
		eSUB_STATE_RUSH_END,
		eSUB_STATE_DIVE_START,
		eSUB_STATE_DIVE_DOING,
		eSUB_STATE_DIVE_END,
		eSUB_STATE_COMEOUT_START,
		eSUB_STATE_COMEOUT_DOING,
		
		eSUB_STATE_MAX,
	};

	enum eHpRate {
		eHP_RATE_HIGH,
		eHP_RATE_MIDDLE,
		eHP_RATE_LOW,

		eHP_RATE_MAX,
	};

	CharaEnemyMekaDragon();
	~CharaEnemyMekaDragon();

	void		PreDraw(void);
	void		PreUpdate(void);
	void		Update(void);
	void		Setup(const char* enemyName, int maxHitpoint, const char* layoutName);
	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);

	bool		CheckBodyCollision(CollisionManager::tSphereWork& rOtherSphere, VECTOR& rVec, float& rDist);

	bool CheckDamageCollNormal(tCheckDamageWork* pParam);
	bool CheckDamageCollRolling(tCheckDamageWork* pParam)			{ return CheckDamageCollNormal(pParam); }
	bool CheckDamageCollJump(tCheckDamageWork* pParam)				{ return CheckDamageCollNormal(pParam); }
	bool CheckDamageCollLonghook(tCheckDamageWork* pParam)			{ return CheckDamageCollNormal(pParam); }
	bool CheckDamageCollBoomerang(tCheckDamageWork* pParam)			{ return CheckDamageCollNormal(pParam); }
	bool CheckDamageCollBomb(ShapeCapsule* pParam);
	bool CheckDamageCollHand(ShapeCapsule* pParam)					{ return CheckDamageCollBomb(pParam); }
	bool CheckDamageCollArrow(tCheckDamageWork* pParam)				{ return CheckDamageCollNormal(pParam); }

	void		GetLockonPosition(VECTOR* pPosition);
	
	void		phaseStateIdle(void);
	void		phaseEnterStateMove(void);
	void		phaseStateMove(void);

	void		phaseEnterStateJumpUp(void);
	void		phaseStateJumpUp(void);
	void		phaseEnterStateJumpDown(void);
	void		phaseStateJumpDown(void);
	void		phaseStateAppear(void);
	void		phaseEnterStateAttack1(void);
	void		phaseStateAttack1(void);
	void		phaseEnterStateAttack2(void);
	void		phaseStateAttack2(void);
	void		phaseEnterStateAttack3(void);
	void		phaseStateAttack3(void);

	void		phaseEnterStateAttack4(void);
	void		phaseStateAttack4(void);
	void		phaseEnterStateAttack5(void);
	void		phaseStateAttack5(void);
	void		phaseEnterStateAttack6(void);
	void		phaseStateAttack6(void);
	void		phaseEnterStateAttack7(void);
	void		phaseStateAttack7(void);

	void		phaseEnterStateRetireStart(void);
	void		phaseStateRetireStart(void);
	void		phaseEnterStateRetireDoing(void);
	void		phaseStateRetireDoing(void);
	void		phaseEnterStateRetireEnd(void);

	/*=====================================*
	 * funcSubState
	 *=====================================*/
	void		funcSubStateLoiter(void);
	void		funcSubStateRushStart(void);
	void		funcSubStateRushDoing(void);
	void		funcSubStateRushEnd(void);
	void		funcSubStateDiveStart(void);
	void		funcSubStateDiveDoing(void);
	void		funcSubStateDiveEnd(void);
	void		funcSubStateComeoutStart(void);
	void		funcSubStateComeoutDoing(void);

	typedef void (CharaEnemyMekaDragon::*FUNC_SUB_STATE)(void);
	static FUNC_SUB_STATE mFuncSubState[eSUB_STATE_MAX];

	bool		checkChangeAttackState(int* pAttackCharaState);

	// 空中の時の処理
	void		updateRevolveMove(float revolveRateY, float revolveRateX, float moveSpeed);
	void		updateTargetAimPosRandom(void);

	// 地面の時の処理
	void		updateRotationToPlayer(float rate);

	void		processOpenHole(VECTOR& effPos, VECTOR& hitPos);

	eHpRate		getHitPointRate(void);
	int			getMotorVolume(void);

	void		startRockBarrier(void);
	void		endRockBarrier(void);

private:
	BYTE		mStatePhase;
	WORD		mStateCount;

	BYTE		mSubState;
	BYTE		mFlyNum;

	ModelBase	mHoleModel;
	ModelBase	mPiecesModel;

	VECTOR		mAimPos;
	VECTOR		mTargetAimPos;

	int			mEffHndSmoke;
	EFK_HANDLE	mEfkHandle;
	float		mLaserScale;

	VECTOR		mRotationVec;
	VECTOR		mVirtualUpVec;
	float		mAimRotPeriod;

	struct tBarrierWork {
		BOOL	mIsActive;
		BOOL	mIsAppear;
		float	mHeight;
	} mBarrierWork;

	BYTE		mRushCount;			// ラッシュ回数


};

#endif // __CHARA_ENEMY_MEKADRAGON_H__

/**** end of file ****/

