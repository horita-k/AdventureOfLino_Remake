/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 爆弾オークファイル
 * @author SPATZ.
 * @data   2015/12/28 14:46:44
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_OAK_BOMB_H__
#define __CHARA_ENEMY_OAK_BOMB_H__

#include "Charactor/CharaEnemy.h"
#include "Object/ObjectHand.h"

#include "AppLib/Graphic/ModelPmd.h"

/*=====================================*
 * class
 *=====================================*/
class CharaEnemyOakBomb : public CharaEnemy {
 public:

	static const BYTE	kOAK_BOMB_NUM = 3;

	/*関数*/
	CharaEnemyOakBomb();
	~CharaEnemyOakBomb();

	void		Setup(const char* enemyName, int maxHitpoint, const char* layoutName);
	void		SetObjectHandModel(ObjectHand* pObjectHand) { mpObjectHand = pObjectHand; }
	ObjectHand*	GetObjectHandModel(void)					{ return mpObjectHand; }

 private:
	/*=====================================*
	 * phaseState
	 *=====================================*/
	/* extend */
	void		phaseEnterStateIdle(void);
	void		phaseEnterStateAttack1(void);
	void		phaseStateAttack1(void);
	void		phaseEnterStateAttack2(void);
	void		phaseStateAttack2(void);
	void		phaseEnterStateDamage(void);
	void		phaseEnterStateRetireEnd(void);


	/* キャラの攻撃ステータス遷移のチェック関数 */
	bool		checkChangeAttackState(int* pAttackCharaState);
	void		phaseStateWaitAttack(void);

	void		processNoMoveAction(void);

	ObjectHand_Bomb		mObjectBomb[kOAK_BOMB_NUM];
	ObjectHand*			mpObjectHand;
};

#endif // __CHARA_ENEMY_OAK_BOMB_H__

/**** end of file ****/

