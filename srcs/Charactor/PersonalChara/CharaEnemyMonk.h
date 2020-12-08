/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 武道家ファイル
 * @author SPATZ.
 * @data   2011/11/27 11:15:20
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_MONK_H__
#define __CHARA_ENEMY_MONK_H__

#include "Charactor/CharaEnemy.h"

/*=====================================*
 * class
 *=====================================*/
class CharaEnemyMonk : public CharaEnemy {
 public:

	/*関数*/
	CharaEnemyMonk()		{}
	~CharaEnemyMonk()		{}

	void		Setup(const char* enemyName, int maxHitpoint, const char* layoutName);
	void		Update(void);

 private:
	/*=====================================*
	 * phaseState
	 *=====================================*/
	/* extend */
	void		phaseEnterStateAttack1(void);
	void		phaseStateAttack1(void);
	void		phaseEnterStateAttack2(void);
	void		phaseStateAttack2(void);
	void		phaseEnterStateAttack3(void);
	void		phaseStateAttack3(void);
	void		phaseEnterStateAttack4(void);
	void		phaseStateAttack4(void);

	void		phaseEnterStateAwayBack(void);
	void		phaseStateAwayBack(void);

	/* キャラの攻撃ステータス遷移のチェック関数 */
	bool		checkChangeAttackState(int* pAttackCharaState);
};

#endif // __CHARA_ENEMY_H__

/**** end of file ****/

