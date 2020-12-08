/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * エクシヴィファイル
 * @author SPATZ.
 * @data   2015/12/31 10:04:48
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_EXCIVY_H__
#define __CHARA_ENEMY_EXCIVY_H__

#include "Charactor/CharaEnemy.h"
#include "DataDefine/MotionDef.h"

#include "Object/ObjectHand.h"

#include "AppLib/Graphic/ModelPmd.h"
#include "Object/ObjectWeapon.h"

/*=====================================*
 * class
 *=====================================*/
class CharaEnemyExcivy : public CharaEnemy {
 public:
	/*関数*/
	CharaEnemyExcivy();
	~CharaEnemyExcivy()		{}

	enum {
		// 10
		eMT_EXTEND_ATTACK_1 = eMT_STANDARD_MAX,	// ソードフレイム
		eMT_EXTEND_ATTACK_2,					// 回避
		eMT_EXTEND_ATTACK_3,					// なぎ払い
		eMT_EXTEND_ATTACK_4,					// ガード
		eMT_EXTEND_ATTACK_5,					// ガード弾き

		eMT_EXTEND_ATTACK_6_START,				// スマッシュ斬り・開始
		eMT_EXTEND_ATTACK_7,					// バク転斬り
		eMT_EXTEND_ATTACK_8,					// 突き
		eMT_EXTEND_ATTACK_9,					// 斬り上げ
		
		eMT_EXTEND_MAX,
	};

	void		Setup(const char* enemyName, int maxHitpoint, const char* layoutName);
	void		Update(void);
	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);

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

	void		phaseEnterStateAttack6(void);
	void		phaseStateAttack6(void);

	void		phaseEnterStateAttack7(void);
	void		phaseStateAttack7(void);

	void		phaseEnterStateAttack8(void);
	void		phaseStateAttack8(void);
	void		phaseEnterStateAttack9(void);
	void		phaseStateAttack9(void);


	void		phaseEnterStateDamage(void);
	void		phaseEnterStateRetireEnd(void);

	/* キャラの攻撃ステータス遷移のチェック関数 */
	bool		checkChangeAttackState(int* pAttackCharaState);
	void		phaseStateWaitAttack(void);

	void		processNoMoveAction(void);

	bool		checkShieldHit(VECTOR *pAttackDirVec);

	void		changeWeaponMaterial(bool isChange);
	void		getSonicWaveMatrix(MATRIX& rMatrix, float setScale, bool isVertical);

	ModelPmd	mWeaponModel;
	BOOL		mSmashState;

	ObjectWeapon_SonicWave		mSonicWave;

	int			mEfsFlame;
	EFK_HANDLE	mEfkFlame;
	WORD		mHellFlameCnt;		
	VECTOR		mHellFlamePos;

	EFK_HANDLE	mEfkCharge;
	BOOL		mIsSkyDiveAttack;


};

#endif // __CHARA_ENEMY_EXCIVY_H__

/**** end of file ****/

