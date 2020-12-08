/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ロアナファイル
 * @author SPATZ.
 * @data   2015/12/19 18:31:50
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_LOANA_H__
#define __CHARA_ENEMY_LOANA_H__

#include "Charactor/CharaEnemy.h"
#include "DataDefine/MotionDef.h"

#include "AppLib/Graphic/EffekseerManager.h"
#include "Object/ObjectWeapon.h"

/*=====================================*
 * class
 *=====================================*/
class CharaEnemyLoana : public CharaEnemy {
 public:

	enum {
		// 10
		eMT_EXTEND_ATTACK_1 = eMT_STANDARD_MAX,	// 風の刃
		eMT_EXTEND_ATTACK_2,					// 突き
		eMT_EXTEND_ATTACK_3,					// ジャンプ斬り
		eMT_EXTEND_ATTACK_4,					// 通常連撃
		eMT_EXTEND_ATTACK_5,					// 回転2連
		eMT_EXTEND_ATTACK_6,					// ガード
		eMT_EXTEND_ATTACK_7,					// ガード弾き
		eMT_EXTEND_ATTACK_8,					// 回避
		
		eMT_EXTEND_MAX,
	};

	/*関数*/
	CharaEnemyLoana()		{}
	~CharaEnemyLoana()		{}

	void		Setup(const char* enemyName, int maxHitpoint, const char* layoutName);
	void		Update(void);
	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);

 private:
	/*=====================================*
	 * phaseState
	 *=====================================*/
	/* extend */

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

	void		phaseStateWaitAttack(void);

	/* キャラの攻撃ステータス遷移のチェック関数 */
	bool		checkChangeAttackState(int* pAttackCharaState);

	bool		checkShieldHit(VECTOR *pAttackDirVec);

	/* 変数 */
	BOOL		mIsDashAttack;
	EFK_HANDLE	mEfkHandle;

	ObjectWeapon_SonicWave		mSonicWave;
};

#endif // __CHARA_ENEMY_LOANA_H__

/**** end of file ****/

