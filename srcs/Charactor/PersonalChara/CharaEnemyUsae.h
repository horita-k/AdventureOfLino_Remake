/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ����������t�@�C��
 * @author SPATZ.
 * @data   2015/12/31 10:04:48
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_USAE_H__
#define __CHARA_ENEMY_USAE_H__

#include "Charactor/CharaEnemy.h"
#include "Object/ObjectHand.h"

#include "AppLib/Graphic/ModelPmd.h"

/*=====================================*
 * class
 *=====================================*/
class CharaEnemyUsae : public CharaEnemy {
 public:

	enum {
		// 10
		eMT_EXTEND_ATTACK_1 = 11,		// �R�A�p���`
		eMT_EXTEND_ATTACK_2,			// ���яR��
		eMT_EXTEND_ATTACK_3,			// ����
		eMT_EXTEND_ATTACK_4,			// ���߂͂ߔg
		eMT_EXTEND_ATTACK_5,			// ���C��
		
		eMT_EXTEND_MAX,
	};


	/*�֐�*/
	CharaEnemyUsae();
	~CharaEnemyUsae()		{}

	void		Setup(const char* enemyName, int maxHitpoint, const char* layoutName);
	void		Update(void);

	void	ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);

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
	void		phaseEnterStateAttack3(void);
	void		phaseStateAttack3(void);
	void		phaseEnterStateAttack4(void);
	void		phaseStateAttack4(void);
	void		phaseEnterStateAttack5(void);
	void		phaseStateAttack5(void);

	void		phaseEnterStateDamage(void);
	void		phaseEnterStateRetireEnd(void);

	/* �L�����̍U���X�e�[�^�X�J�ڂ̃`�F�b�N�֐� */
	bool		checkChangeAttackState(int* pAttackCharaState);
	void		phaseStateWaitAttack(void);

	void		processNoMoveAction(void);

	int			mStateCount;
	int			mStatePhase;

	EFK_HANDLE	mEfkHandle;
	EFK_HANDLE	mEfkFlame;
	DWORD		mCannonCount;

	VECTOR		mEffectPos;
	VECTOR		mEffectMoveVec;
};

#endif // __CHARA_ENEMY_USAE_H__

/**** end of file ****/

