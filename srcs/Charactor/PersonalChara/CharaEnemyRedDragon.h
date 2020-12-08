/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * �G�ԗ��N���X
 * @author SPATZ.
 * @data   2011/11/27 11:15:20
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_REDDRAGON_H__
#define __CHARA_ENEMY_REDDRAGON_H__

#include "Charactor/CharaEnemy.h"
#include "AppLib/Graphic/EffekseerManager.h"

/*=====================================*
 * �ԗ��N���X
 *=====================================*/
class CharaEnemyRedDragon : public CharaEnemy {
public:
	CharaEnemyRedDragon();
	
	/* �֐� */
	void		Setup(const char* enemyName, int maxHitpoint, const char* layoutName);
	void		Update(void);
	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);

	bool		CheckBodyCollision(CollisionManager::tSphereWork& rOtherSphere, VECTOR& rVec, float& rDist);

	bool		CheckDamageCollNormal(tCheckDamageWork* pParam);
	bool		CheckDamageCollJump(tCheckDamageWork* pParam);

	void		GetLockonPosition(VECTOR* pPosition);

	void		phaseEnterStateIdle(void);

	void		phaseStateTurn(void);

	void		phaseStateMove(void);
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
	void		phaseEnterStateAttack6(void);
	void		phaseStateAttack6(void);

	void		phaseStateWaitAttack(void);

	void		phaseEnterStateRetireDoing(void);
	void		phaseStateRetireDoing(void);
	void		phaseEnterStateRetireEnd(void);

	/* �L�����̍U���X�e�[�^�X�J�ڂ̃`�F�b�N�֐� */
	bool		checkChangeAttackState(int* pAttackCharaState);

private:

	//	void		updateAllObject(void);	// ���̃I�u�W�F�N�g�Ƃ̓����蔻��
	void		setFlameMatrix(float setSize);
	void		updateRotationToPlayer(float rate);

	void		getHeadSphere(CollisionManager::tSphereWork& rSphere);

	/* �ϐ� */
	int			mFlameEffIndex;
	int			mChargeEffIndex;
	int			mStateCount;
	int			mStatePhase;
	int			mSndIndex;
	bool		mIsScaleFlame;
	int			mFlameCount;
	bool		mIsCritical;
	//	EFK_HANDLE	mEfkHandle;
	EFK_HANDLE	mEfkHandle;
	EFK_HANDLE	mEfkHandlePiyopiyo;
	int			mEffHandle;
	int			mSndHandle;
	float		mAboutPlayerDot;

	float		mLockonLength;
	BOOL		mIsHitHead;
	BYTE		mFireCount;

};

#endif // __CHARA_ENEMY_REDDRAGON_H__

/**** end of file ****/

