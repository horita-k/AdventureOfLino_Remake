/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ミノタウロス・アーマークラス
 * @author SPATZ.
 * @data   2015/09/06 12:17:32
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_MINO_ARMER_H__
#define __CHARA_ENEMY_MINO_ARMER_H__

#include "Charactor/CharaEnemy.h"
#include "DataDefine/MotionDef.h"
#include "AppLib/Graphic/EffekseerManager.h"

/*=====================================*
 * class
 *=====================================*/
class CharaEnemyMinoArmer : public CharaEnemy {
 public:

	/* 定義 */
	enum {
		eMT_MINO_ARMER_ATTACK_1 = eMT_STANDARD_MAX,
		eMT_MINO_ARMER_ATTACK_2_A,
		eMT_MINO_ARMER_ATTACK_2_B,
		eMT_MINO_ARMER_ATTACK_3,
		eMT_MINO_ARMER_ATTACK_JUMP,

		eMT_MINO_ARMER_MAX,
	};

	/*関数*/
	CharaEnemyMinoArmer();
	~CharaEnemyMinoArmer();

	void		Setup(const char* enemyName, int maxHitpoint, const char* layoutName);
	void		Update(void);

 private:

	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);

	/*=====================================*
	 * phaseState
	 *=====================================*/
	/* extend */

	void		phaseEnterStateIdle(void);

	void		phaseEnterStateJumpUp(void);
	void		phaseStateJumpUp(void);
	void		phaseEnterStateJumpDown(void);
	void		phaseStateJumpDown(void);
	void		phaseEnterStateJumpEnd(void);
	void		phaseStateJumpEnd(void);

	void		phaseEnterStateAttack1(void);
	void		phaseStateAttack1(void);
	void		phaseEnterStateAttack2(void);
	void		phaseStateAttack2(void);
	void		phaseEnterStateAttack3(void);
	void		phaseStateAttack3(void);

	void		phaseEnterStateRetireEnd(void);

	/* キャラの攻撃ステータス遷移のチェック関数 */
	bool		checkChangeAttackState(int* pAttackCharaState);
	void		processLandingFunc(void)		{}

	void		updateAttackCollisionLocal(int attackPower);
	void		updateAttack1Collision(void);

	/* 変数 */
	int			mStateCount;
	int			mStatePhase;

	int			mEfsShock;
	int			mEfsHndSmoke;

	float		mShockScaleSize;
	BOOL		mIsJumpAttack;
};

#endif // __CHARA_ENEMY_MINO_ARMER_H__

/**** end of file ****/

