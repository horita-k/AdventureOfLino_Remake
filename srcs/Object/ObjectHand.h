/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * オブジェクトベースクラス
 * @author SPATZ.
 * @data   2012/10/01 23:27:16
 */
//---------------------------------------------------
#ifndef __OBJECT_HAND_H__
#define __OBJECT_HAND_H__

#include "ObjectBase.h"
#include "Charactor/CharaBase.h"

/*=====================================*
 * class
 *=====================================*/
class ObjectHand : public ObjectBase {
 public:

	enum {
		eHAND_STATE_ON_EARTH = 0,
		eHAND_STATE_RAISE,
		eHAND_STATE_THROWING,
		eHAND_STATE_DISAPPEAR,
		eHAND_STATE_ON_GIMMICK,
		
		eHAND_STATE_MAX,
	};

	enum {
		eHAND_TYPE_JAR = 0,
		eHAND_TYPE_STONE,
		eHAND_TYPE_LOCK,
		eHAND_TYPE_BOMB,
		eHAND_TYPE_USACHU,
		eHAND_TYPE_SUNOHARA,
		
		eHAND_TYPE_MAX,
		eHAND_TYPE_ERROR,
	};

	/* 関数 */
	ObjectHand();
	~ObjectHand();

	virtual void	Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg);
	virtual void	Update(void);
	bool			GetIsRetire(void);
	bool			CheckDamageCollNormal(tCheckDamageWork* pParam);
	virtual void	ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType=eATTACK_TYPE_NORMAL);
	void			SetNowGravity(float value) 		{ mGravity = value; }
	void			SetUsed(void);
	void			StartCheckEvent(void);
	virtual void	ProcessThrow(VECTOR* pThrowVec);
	UCHAR&			State(void)				{ return mHandState; }
	VECTOR&			MoveVec(void)			{ return mMoveVec; }		
	BYTE			GetHandType(void)		{ return mHandType; }

	void			SetMasterChara(CharaBase* pMaster)	{ mpMaster = pMaster; }
	bool			CheckEnableAttack(void)	{ return false; }

private:
	
	virtual	void processHitCollision(void* pParam)=0;
	virtual void processHitEnemy(void* pParam)=0;
	virtual void processBreakObject(void)=0;
	/*変数*/
protected:
	void		phaseStateOnEarth(void);
	void		phaseStateRaise(void);
	void		phaseStateThrowing(void);
	void		phaseStateDisappear(void);
	void		phaseStateOnGimmick(void);

	typedef void (ObjectHand::*PHASE_STATE)(void);
	static PHASE_STATE mPhaseState[eHAND_STATE_MAX];

	UCHAR		mHandState;
	VECTOR		mMoveVec;
	float		mGravity;
	BYTE		mHandType;
	CharaBase*	mpMaster;

};

/*=====================================*
 * ObjectHand_Jar
 *=====================================*/
class ObjectHand_Jar : public ObjectHand {
public:
	
	void Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg);
	char*			GetModelName(void)		{ return (char *)"つぼ"; }
	/* 関数 */
private:
	void processHitCollision(void* pParam);
	void processHitEnemy(void* pParam);
	void processBreakObject(void);

	/* 変数 */
};

/*=====================================*
 * ObjectHand_Stone
 *=====================================*/
class ObjectHand_Stone : public ObjectHand {
public:
	
	virtual void	Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg);
	virtual void	ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType=eATTACK_TYPE_NORMAL);
	char*			GetModelName(void)		{ return (char *)"石"; }

	/* 関数 */
protected:
	virtual void processHitCollision(void* pParam);
	void processHitEnemy(void* pParam);
	virtual void processBreakObject(void);

	/* 変数 */
};

/*=====================================*
 * ObjectHand_Sunohara
 *=====================================*/
class ObjectHand_Sunohara : public ObjectHand_Stone {
public:
	/* 関数 */
	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType=eATTACK_TYPE_NORMAL);
	char*		GetModelName(void)		{ return (char*)"マスク・ザ・春原"; }
	void		processBreakObject(void)		{}
};

/*=====================================*
 * ObjectHand_Usachu
 *=====================================*/
class ObjectHand_Usachu : public ObjectHand_Stone {
public:
	void Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg);
	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType=eATTACK_TYPE_NORMAL);
	char*		GetModelName(void)		{ return (char*)"うさちゅう"; }

	void		processHitCollision(void* pParam);
	void		processBreakObject(void)		{}
};

/*=====================================*
 * ObjectHand_Lock
 *=====================================*/
class ObjectHand_Lock : public ObjectHand {
public:
	
	/* 関数 */
	void Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg);
	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType=eATTACK_TYPE_NORMAL);
	char*		GetModelName(void)		{ return (char*)"大岩"; }

private:
	void processHitCollision(void* pParam) {}
	void processHitEnemy(void* pParam){}
	void processBreakObject(void);

	/* 変数 */
};

/*=====================================*
 * ObjectHand_Bomb
 *=====================================*/
class ObjectHand_Bomb : public ObjectHand {
public:
	
	enum  {
		eHAND_BOMB_STATE_WAIT_BURST = eHAND_STATE_MAX,
		eHAND_BOMB_STATE_BURSTTING,

		eHAND_BOMB_STATE_MAX,
	};

	ObjectHand_Bomb();
	~ObjectHand_Bomb();

	void Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg, bool isRaise=false);
	void Update(void);
	char*			GetModelName(void)		{ return (char*)"爆弾"; }

	bool		GetIsRetire(void);
	void		ProcessThrow(VECTOR* pThrowVec);
	void		ProcessDisappear(void);

	/* 関数 */
private:

	void processHitCollision(void* pParam);
	void processHitEnemy(void* pParam);
	void processBreakObject(void);

	void		phaseBombStateRaise(void);
	void		phaseBombStateThrowing(void);
	void		phaseBombStateWaitBurst(void);
	void		phaseBombStateBurstting(void);

	/* 変数 */
	typedef void (ObjectHand_Bomb::*PHASE_BOMB_STATE)(void);
	static PHASE_BOMB_STATE mPhaseState[eHAND_BOMB_STATE_MAX];

	USHORT		mBurstTimer;
	BYTE		mReboundNum;
	int			mFireEffIndex;

};

#endif // __OBJECT_HAND_H__

 /**** end of file ****/

