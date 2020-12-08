/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * �E�T�C�_�[�܂�t�@�C��
 * @author SPATZ.
 * @data   2015/12/31 10:04:48
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_USIDER_H__
#define __CHARA_ENEMY_USIDER_H__

#include "Charactor/CharaEnemy.h"
#include "Object/ObjectHand.h"
#include "DataDefine/MotionDef.h"

#include "AppLib/Graphic/ModelPmd.h"

/*=====================================*
 * class
 *=====================================*/
class CharaEnemyUsider : public CharaEnemy {

	enum {
		// 10
		eMT_EXTEND_ATTACK_1 = eMT_STANDARD_MAX,	// �Q�b�_��
		
		eMT_EXTEND_MAX,
	};

 public:
	/*�֐�*/
	CharaEnemyUsider();
	~CharaEnemyUsider()		{}

	void		Setup(const char* enemyName, int maxHitpoint, const char* layoutName);
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

	/* �L�����̍U���X�e�[�^�X�J�ڂ̃`�F�b�N�֐� */
	bool		checkChangeAttackState(int* pAttackCharaState);
	void		phaseStateWaitAttack(void);

	void		processNoMoveAction(void);

	void		updateRollingParameter(void);
	int			getMotorVolume(void);

	EFK_HANDLE	mEfkHandle;
	VECTOR		mRushVecZ;
	VECTOR		mRushCrossX;
	int			mEffHndSmoke;
	float		mVy0;
	float		mVx;
	float		mAccel;
	float		mRollingCrushCnt;

};

#endif // __CHARA_ENEMY_USIDER_H__

/**** end of file ****/

