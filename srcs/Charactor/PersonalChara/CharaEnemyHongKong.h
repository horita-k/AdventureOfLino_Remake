/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 香港映画Ｐファイル
 * @author SPATZ.
 * @data   2011/11/27 11:15:20
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_HONG_KONG_H__
#define __CHARA_ENEMY_HONG_KONG_H__

#include "Charactor/CharaEnemy.h"
#include "DataDefine/MotionDef.h"

/*=====================================*
 * class
 *=====================================*/
class CharaEnemyHongKong : public CharaEnemy {
 public:

	enum {
		// 10
		eMT_EXTEND_ATTACK_1 = eMT_STANDARD_MAX,
		eMT_EXTEND_ATTACK_2,
		eMT_EXTEND_ATTACK_3,
		eMT_EXTEND_ATTACK_4,
		eMT_EXTEND_ATTACK_5,
		
		eMT_EXTEND_MAX,
	};

	/*関数*/
	CharaEnemyHongKong()		{}
	~CharaEnemyHongKong()		{}

	void		Setup(const char* enemyName, int maxHitpoint, const char* layoutName);
	void		Update(void);

 private:
	/*=====================================*
	 * phaseState
	 *=====================================*/
	/* extend */
	void		phaseEnterStateMove(void);

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

	void		phaseEnterStateAwayBack(void);
	void		phaseStateAwayBack(void);

	void		phaseStateWaitAttack(void);

	/* キャラの攻撃ステータス遷移のチェック関数 */
	bool		checkChangeAttackState(int* pAttackCharaState);

	/* 変数 */
	BOOL		mIsDashAttack;

};

#endif // __CHARA_ENEMY_HONG_KONG_H__

/**** end of file ****/

