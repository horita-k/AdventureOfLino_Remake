/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 羊頭のデーモン
 * @author SPATZ.
 * @data   2014/06/19 01:28:51
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_DEMON_H__
#define __CHARA_ENEMY_DEMON_H__

#include "Charactor/CharaEnemy.h"
#include "AppLib/Graphic/EffekseerManager.h"
#include "Object/ObjectWeapon.h"

/*=====================================*
 * static 
 *=====================================*/
static const BYTE kDEMON_METEO_NUM = 8;

/*=====================================*
 * 羊頭のデーモンクラス
 *=====================================*/
class CharaEnemyDemon : public CharaEnemy {
public:
	enum {
		eADVICE_STATE_ADVICE_A = 0,
		eADVICE_STATE_ADVICE_B,
		eADVICE_STATE_ADVICE_C,

		eADVICE_STATE_ADVICE_END,
	};

	CharaEnemyDemon();
	~CharaEnemyDemon();
	
	/* 関数 */
	void		Setup(const char* enemyName, int maxHitpoint, const char* layoutName);
	void		Update(void);
	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);

	bool		CheckDamageCollNormal(tCheckDamageWork* pParam);
	bool		CheckDamageCollArrow(tCheckDamageWork* pParam);

	void		phaseStateIdle(void);
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

	void		phaseStateDamage(void);

	void		phaseEnterStateRetireDoing(void);
	void		phaseStateRetireDoing(void);
	void		phaseEnterStateRetireEnd(void);

	/* キャラの攻撃ステータス遷移のチェック関数 */
	bool		checkChangeAttackState(int* pAttackCharaState);

private:

	/* 変数 */
	int			mStateCount;
	int			mStatePhase;
	int			mThunderEffIndex;
	EFK_HANDLE	mEfkHandle;

	ObjectWeapon_Meteo	mMeteoObject[kDEMON_METEO_NUM];
	ObjectWeapon_MeteoRefrect mRefrectObject;

	int			mMeteoRandIndex[5];
	BYTE		mAdviceState;

};


#endif // __CHARA_ENEMY_DEMON_H__

/**** end of file ****/

