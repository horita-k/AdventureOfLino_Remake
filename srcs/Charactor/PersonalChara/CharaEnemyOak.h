/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ギギネブラファイル
 * @author SPATZ.
 * @data   2011/11/27 11:15:20
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_OAK_H__
#define __CHARA_ENEMY_OAK_H__

#include "Charactor/CharaEnemy.h"

#include "AppLib/Graphic/ModelPmd.h"

/*=====================================*
 * class
 *=====================================*/
class CharaEnemyOak : public CharaEnemy {
 public:

	/*関数*/
	CharaEnemyOak()		{}
	~CharaEnemyOak()		{}

	void		Setup(const char* enemyName, int maxHitpoint, const char* layoutName);
	void		Update(void);
	void		Destroy(void);

	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);

 private:
	/*=====================================*
	 * phaseState
	 *=====================================*/
	/* extend */
	void		phaseEnterStateAttack1(void);
	void		phaseStateAttack1(void);

	void		phaseEnterStateDamage(void);
	void		phaseStateDamage(void);

	void		phaseEnterStateRetireEnd(void);


	/* キャラの攻撃ステータス遷移のチェック関数 */
	bool		checkChangeAttackState(int* pAttackCharaState);

	void		processNoMoveAction(void);

	ModelPmd	mWeaponModel;
	unsigned short mDamageCount;

};

#endif // __CHARA_ENEMY_OAK_H__

/**** end of file ****/

