/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ギギネブラファイル
 * @author SPATZ.
 * @data   2011/11/27 11:15:20
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_FRESBELK_H__
#define __CHARA_ENEMY_FRESBELK_H__

#include "Charactor/CharaEnemy.h"

#include "AppLib/Graphic/ModelPmd.h"

/*=====================================*
 * class
 *=====================================*/
class CharaEnemyFresbelk : public CharaEnemy {
 public:

	/*関数*/
	CharaEnemyFresbelk();
	~CharaEnemyFresbelk()		{}

	void		Setup(const char* enemyName, int maxHitpoint, const char* layoutName);
	void		Update(void);
	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);

 private:
	/*=====================================*
	 * phaseState
	 *=====================================*/
	/* extend */

	void		phaseEnterStateIdle(void);

	void		phaseEnterStateJumpEndStandard(void);
	void		phaseEnterStateJumpEndDarts(void);
	void		phaseStateJumpEndStandard(void);
	void		phaseStateJumpEndDarts(void);


	void		phaseEnterStateJumpDown(void);
	void		phaseEnterStateJumpUp(void);
	void		phaseStateJumpUp(void);

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

	//	void		phaseStateWaitAttack(void);

	void		phaseEnterStateRetireEnd(void);

	/* キャラの攻撃ステータス遷移のチェック関数 */
	bool		checkChangeAttackState(int* pAttackCharaState);
	void		phaseStateWaitAttack(void);

	void		processLandingFunc(void)		{}
	void		updateCrushParameter(void);

	ModelPmd	mWeaponModel;

	VECTOR		mRushVecZ;
	VECTOR		mRushCrossX;
	float		mVy0;
	float		mVx;
	float		mAccel;
	float		mRollingCrushCnt;
	int			mEffHndSmoke;
	BOOL		mIsSkyDiveAttack;
	BOOL		mIsChangeModel;

#ifdef _DEBUG
	/* debug func */
	//	void		debugFunc(void);
#endif // _DEBUG
};

#endif // __CHARA_ENEMY_H__

/**** end of file ****/

