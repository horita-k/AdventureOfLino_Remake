/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ギギネブラファイル
 * @author SPATZ.
 * @data   2011/11/27 11:15:20
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_KNIGHT_H__
#define __CHARA_ENEMY_KNIGHT_H__

#include "Charactor/CharaEnemy.h"

#include "AppLib/Graphic/ModelPmd.h"

/*=====================================*
 * class
 *=====================================*/
class CharaEnemyKnight : public CharaEnemy {
 public:

	/*関数*/
	CharaEnemyKnight()		{}
	~CharaEnemyKnight()		{}

	void		Setup(const char* enemyName, int maxHitpoint, const char* layoutName);
	void		Update(void);
	void		Destroy(void);

 private:
	/*=====================================*
	 * phaseState
	 *=====================================*/
	/* extend */
	void		phaseEnterStateAttack1(void);
	void		phaseStateAttack1(void);

	void		phaseEnterStateRetireEnd(void);


	/* キャラの攻撃ステータス遷移のチェック関数 */
	bool		checkChangeAttackState(int* pAttackCharaState);

	ModelPmd	mWeaponModel;
};

#endif // __CHARA_ENEMY_KNIGHT_H__

/**** end of file ****/

