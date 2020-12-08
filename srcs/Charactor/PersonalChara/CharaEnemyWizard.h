/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ギギネブラファイル
 * @author SPATZ.
 * @data   2011/11/27 11:15:20
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_WIZARD_H__
#define __CHARA_ENEMY_WIZARD_H__

#include "Charactor/CharaEnemy.h"

#include "AppLib/Graphic/ModelPmd.h"
#include "AppLib/Graphic/EffekseerManager.h"

/*=====================================*
 * class
 *=====================================*/
class CharaEnemyWizard : public CharaEnemy {
 public:

	static const int kWIZARD_EFF_NUM = 4;

	enum eRollballState {
		eROLLBALL_STATE_PENDING,
		eROLLBALL_STATE_CREATING,
		eROLLBALL_STATE_STREET,

		eROLLBALL_STATE_MAX,
	};

	/*関数*/
	CharaEnemyWizard()		{}
	~CharaEnemyWizard()		{}

	void		Setup(const char* enemyName, int maxHitpoint, const char* layoutName);
	void		Update(void);

	void		Destroy(void);

	void	ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);

 private:
	/*=====================================*
	 * phaseState
	 *=====================================*/
	/* extend */
	void		phaseEnterStateAttack1(void);	// カミナリ魔法
	void		phaseStateAttack1(void);
	void		phaseEnterStateAttack2(void);	// 分散魔法
	void		phaseStateAttack2(void);
	void		phaseEnterStateAttack3(void);	// 回転魔法
	void		phaseStateAttack3(void);
	void		phaseEnterStateAttack4(void);	// 瞬間移動
	void		phaseStateAttack4(void);
	void		phaseEnterStateAttack5(void);	// プロテクト開始
	void		phaseStateAttack5(void);
	void		phaseEnterStateAttack6(void);	// カミナリ跳ね返されているステータス
	void		phaseStateAttack6(void);
	void		phaseEnterStateAttack7(void);	// 波状攻撃
	void		phaseStateAttack7(void);

	void		phaseEnterStateRetireEnd(void);

	void		phaseEnterStateDownStart(void);
	void		phaseEnterStateDamage(void);

	//	bool		getPlayerPosRotFromBGModel(VECTOR* pOutPos, float* pOutRot, int stagePlaceIndex);

	/* キャラの攻撃ステータス遷移のチェック関数 */
	bool		checkChangeAttackState(int* pAttackCharaState);
	void		processNoMoveAction(void);

	ModelPmd	mWeaponModel;

	EFK_HANDLE	mEfkHandle;
	int			mEfsHandle[kWIZARD_EFF_NUM];
	EFK_HANDLE	mEfkProtect;

	VECTOR		sThunderPos;
	VECTOR		sThunderVec;

	int			mStateCount;
	int			mStatePhase;

	bool		mIsBarrier;
	int			mProtectEffIndex;
	short		mProtectWaitCount;

	eRollballState		mRollballState[kWIZARD_EFF_NUM];
	VECTOR		mEnegyBallVec[kWIZARD_EFF_NUM];

	bool		mIsRandomWarp;

#ifdef _DEBUG
	/* debug func */
	//	void		debugFunc(void);
#endif // _DEBUG
};

#endif // __CHARA_ENEMY_H__

/**** end of file ****/

