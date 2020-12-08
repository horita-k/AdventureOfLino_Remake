/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * �M�M�l�u���t�@�C��
 * @author SPATZ.
 * @data   2011/11/27 11:15:20
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_MAGICIAN_H__
#define __CHARA_ENEMY_MAGICIAN_H__

#include "Charactor/CharaEnemy.h"

#include "AppLib/Graphic/ModelPmd.h"
#include "AppLib/Graphic/EffekseerManager.h"

/*=====================================*
 * class
 *=====================================*/
class CharaEnemyMagician : public CharaEnemy {
 public:

	/*�֐�*/
	CharaEnemyMagician()		{}
	~CharaEnemyMagician()		{}

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

	void		phaseEnterStateDownStart(void);
	void		phaseEnterStateDamage(void);

	/* �L�����̍U���X�e�[�^�X�J�ڂ̃`�F�b�N�֐� */
	bool		checkChangeAttackState(int* pAttackCharaState);

	ModelPmd	mWeaponModel;

	VECTOR		sThunderVec;

	int			mStateCount;
	int			mStatePhase;

	EFK_HANDLE	mEfkHandle;
	int			mEfsHandle;

};

#endif // __CHARA_ENEMY_MAGICIAN_H__

/**** end of file ****/

