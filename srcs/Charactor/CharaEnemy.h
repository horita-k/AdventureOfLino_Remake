/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 敵クラス
 * @author SPATZ.
 * @data   2011/11/27 11:15:20
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_H__
#define __CHARA_ENEMY_H__

#include "CharaBase.h"
#include "Scene/TotalInfo.h"
#include "../DataDefine/EnemyDef.h"

struct tCapsuleWork {
	const char* mPosA;
	const char* mPosB;
	float		mRadius;
};

/*=====================================*
 * class
 *=====================================*/
class CharaEnemy : public CharaBase {
 public:

	/* 定義 */
	enum {
		eCHARA_STATE_ATTACK_1 = eCHARA_BASE_STATE_MAX,
		eCHARA_STATE_ATTACK_2,
		eCHARA_STATE_ATTACK_3,
		eCHARA_STATE_ATTACK_4,
		eCHARA_STATE_ATTACK_5,
		eCHARA_STATE_ATTACK_6,
		eCHARA_STATE_WAIT_ATTACK,
		eCHARA_STATE_APPEAR,
		eCHARA_STATE_ATTACK_7,
		eCHARA_STATE_ATTACK_8,
		eCHARA_STATE_ATTACK_9,

		eCHARA_STATE_MAX,
	};

	/*関数*/
	/*=====================================*
	 * static method
	 *=====================================*/
	static CharaEnemy* CreateInstance(USHORT enemyKind);
	static void SetStopAllEnemyFlag(BOOL isSet);

	CharaEnemy();
	virtual ~CharaEnemy();
	virtual void	Setup(const char* modelName, int maxHitpoint, const char* enemyName);
	virtual void	Update(void);
	void			ChangeState(int setState);
	virtual	void	ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);
	void			ProcessAttackToPlayer(const float startFrame, const float endFrame, const char* nodeName, float nodeSize, int attackPower, bool isDamageDown);

	void		PermitAcceptAI(bool isPermit);

	void		UpdateAttackChara(void);
	void		SetMoveSpeed(float speed)		{ mMoveSpeed = speed; }
	char*		GetModelName(void)				{ return mpEnemyName; }
	void		AppearEnemy(SHORT waitCount);
	void		SetItemInfo(int itemKind, int getPercent);
	void		GetItemInfo(int* pItemKind, int* pGetPercent);
	BOOL		GetIsEnemy(void)				{ return TRUE; }

	// private:
protected:
	//public:
	/*=====================================*
	 * phaseState
	 *=====================================*/
	virtual void		phaseEnterStateIdle(void);
	virtual void		phaseStateIdle(void);
	virtual void		phaseEnterStateTurn(void);
	virtual void		phaseStateTurn(void);
	virtual void		phaseEnterStateMove(void);
	virtual void		phaseStateMove(void);
	virtual void		phaseEnterStateJumpUp(void);
	virtual void		phaseStateJumpUp(void);
	virtual void		phaseEnterStateJumpDown(void);
	virtual void		phaseStateJumpDown(void);
	virtual void		phaseEnterStateJumpEnd(void);
	virtual void		phaseStateJumpEnd(void);
	virtual void		phaseEnterStateDamage(void);
	virtual void		phaseStateDamage(void);
	virtual void		phaseEnterStateRetireStart(void);
	virtual void		phaseStateRetireStart(void);
	virtual void		phaseEnterStateRetireDoing(void);
	virtual void		phaseStateRetireDoing(void);
	virtual void		phaseEnterStateRetireEnd(void);
	virtual void		phaseStateRetireEnd(void);
	virtual void		phaseEnterStateSlopeDown(void);
	virtual void		phaseStateSlopeDown(void);
	virtual void		phaseEnterStateRefuseInput(void);
	virtual void		phaseStateRefuseInput(void);
	virtual void		phaseEnterStateDownStart(void);
	virtual void		phaseStateDownStart(void);
	virtual void		phaseEnterStateDownDoing(void);
	virtual void		phaseStateDownDoing(void);
	virtual void		phaseEnterStateDownEnd(void);
	virtual void		phaseStateDownEnd(void);
	virtual void		phaseEnterStateFallDown(void);
	virtual void		phaseStateFallDown(void);

	/* 拡張ステータス */
	virtual void		phaseEnterStateAttack1(void);
	virtual void		phaseStateAttack1(void);
	virtual void		phaseEnterStateAttack2(void);
	virtual void		phaseStateAttack2(void);
	virtual void		phaseEnterStateAttack3(void);
	virtual void		phaseStateAttack3(void);
	virtual void		phaseEnterStateAttack4(void);
	virtual void		phaseStateAttack4(void);
	virtual void		phaseEnterStateAttack5(void);
	virtual void		phaseStateAttack5(void);
	virtual void		phaseEnterStateAttack6(void);
	virtual void		phaseStateAttack6(void);
	virtual void		phaseEnterStateWaitAttack(void);
	virtual void		phaseStateWaitAttack(void);
	virtual void		phaseEnterStateAppear(void);
	virtual void		phaseStateAppear(void);
	virtual void		phaseEnterStateAttack7(void);
	virtual void		phaseStateAttack7(void);
	virtual void		phaseEnterStateAttack8(void)	{}
	virtual void		phaseStateAttack8(void)			{}
	virtual void		phaseEnterStateAttack9(void)	{}
	virtual void		phaseStateAttack9(void)			{}

	/* キャラの攻撃ステータス遷移のチェック関数 */
	virtual bool		checkChangeAttackState(int* pAttackCharaState);
	virtual void		processNoMoveAction(void);
	void				processTurnTargetRotY(float turnMoveSpeed, int nextCharaState);
	virtual void		processLandingFunc(void);
	void				updateLastEarthPos(void);					// mLastEarthPos更新処理

	/*変数*/
	typedef void (CharaEnemy::*PHASE_STATE)(void);
	PHASE_STATE mPhaseEnterState[eCHARA_STATE_MAX];
	PHASE_STATE mPhaseState[eCHARA_STATE_MAX];

	static PHASE_STATE sPhaseEnterEnemyState[eCHARA_STATE_MAX];
	static PHASE_STATE sPhaseEnemyState[eCHARA_STATE_MAX];

protected:
	VECTOR				mMoveHistory[8];
	float				mMoveSpeed;
	int					mReserveCharaState;		/* 予約用キャラステータス */
	char*				mpEnemyName;			/* このクラスではアドレスのみを管理で実体は管理しない */
	SHORT				mAppearWaitCount;
	SHORT				mAppearWaitMax;

	struct tItemInfo {
		int				mItemKind;
		int				mGetPercent;
	} mItemInfo;

	static BOOL			sIsStopAllEnemy;

};

#endif // __CHARA_ENEMY_H__

/**** end of file ****/

