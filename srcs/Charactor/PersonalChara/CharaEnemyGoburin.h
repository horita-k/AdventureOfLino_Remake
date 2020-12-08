/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ギギネブラファイル
 * @author SPATZ.
 * @data   2011/11/27 11:15:20
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_GOBURIN_H__
#define __CHARA_ENEMY_GOBURIN_H__

#include "Charactor/CharaEnemy.h"

/*=====================================*
 * class
 *=====================================*/
class CharaEnemyGoburin : public CharaEnemy {
 public:

	/*関数*/
	CharaEnemyGoburin()		{}
	~CharaEnemyGoburin()		{}

	void		Setup(const char* enemyName, int maxHitpoint, const char* layoutName);
	//	void		Update(void);

	/* ダメージ当たり判定チェック関数 */
	//	bool		CheckDamageCollNormal(void* pParam);

 private:
	/*=====================================*
	 * phaseState
	 *=====================================*/
	/* extend */

	void		phaseEnterStateAttack1(void);
	void		phaseStateAttack1(void);

	/* キャラの攻撃ステータス遷移のチェック関数 */
	bool		checkChangeAttackState(int* pAttackCharaState);

#ifdef _DEBUG
	/* debug func */
	//	void		debugFunc(void);
#endif // _DEBUG
};

#endif // __CHARA_ENEMY_H__

/**** end of file ****/

