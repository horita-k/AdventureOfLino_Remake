/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * てぃーだー(戦車)管理クラス
 * @author SPATZ.
 * @data   2014/11/17 01:10:48
 */
//---------------------------------------------------
#ifndef __CHARA_ENEMY_TEEDER_H__
#define __CHARA_ENEMY_TEEDER_H__

#include "Charactor/CharaEnemy.h"
#include "Object/ObjectWeapon.h"

/*=====================================*
 * class
 *=====================================*/
class CharaEnemyTeeder : public CharaEnemy {

	static const BYTE kTEEDER_MISSILE_NUM = 3;
 public:
	/*関数*/
	CharaEnemyTeeder();
	~CharaEnemyTeeder();

	void		Setup(const char* enemyName, int maxHitpoint, const char* layoutName);
	void		Update(void);
	void		Destroy(void);
	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);

 private:
	static const char* strAttackTopNode;
	static const char* strAttackBtmNode;
	static const char* strAttackFrontNode;
	static const char* strAttackBackNode;
	static const char* strHeadNode;

	static /*const*/ float judge_dist_rush_end;

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
	void		phaseEnterStateDownStart(void);
	void		phaseEnterStateRetireEnd(void);
	bool		checkChangeAttackState(int* pAttackCharaState);

	bool		updateSideCollision(bool is2nd);		// 壁との当たり判定・改

	void		updateTireUVAnimation(float scrollSpeed);
	int			getMotorVolume(void);
	void		updateRotateCannon(void);
	void		resetAllNode(void);
	
	ModelPmd	mTireModel;
	float		mUVScrollValue;
	int			mEffHndSmoke;
	BYTE		mStatePhase;
	WORD		mStateCount;		
	VECTOR		mCannonTargetPos;
	
	ObjectWeapon_Missile		mMissileObject[kTEEDER_MISSILE_NUM];
};

#endif // __CHARA_ENEMY_H__

/**** end of file ****/

