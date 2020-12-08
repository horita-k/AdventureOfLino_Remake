/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 敵クラス
 * @author SPATZ.
 * @data   2011/11/27 11:15:20
 */
//---------------------------------------------------
#ifndef __CHARA_GUARDIAN_H__
#define __CHARA_GUARDIAN_H__

#include "CharaBase.h"
#include "Scene/TotalInfo.h"
#include "../DataDefine/EnemyDef.h"
#include "../DataDefine/MotionDef.h"

#define GUARDIAN_DEF_SCALE		(CHARA_DEF_SCALE)

static const BYTE kGUARDIAN_SPHERE_ADD_MATERIAL_TABLE[] = {
	3, 12, 13, 21, 27, 28
};

/*=====================================*
 * class
 *=====================================*/
class CharaGuardian : public CharaBase {
 public:

	/* 定義 */
	enum {
		// 20
		eCHARA_STATE_NOTHING = eCHARA_BASE_STATE_MAX,
		eCHARA_STATE_ATTACK_1,

		eCHARA_STATE_EMULATE_MAX,

		eCHARA_STATE_SLEEP = eCHARA_STATE_EMULATE_MAX,
		eCHARA_STATE_BOOTDOWN,
		eCHARA_STATE_CHANGE_LINO,

		eCHARA_STATE_MAX,
	};

	enum {
		eMT_SLEEP		= eMT_ATTACK_2,
		eMT_BOOTUP		= eMT_ATTACK_3,
		eMT_BOOTDOWN	= eMT_ATTACK_ROLL,
	};

	/*関数*/
	CharaGuardian();
	virtual ~CharaGuardian();
	void			Setup(void);
	virtual	void	Destroy(void);

	virtual void	Update(void);
	void			ChangeState(int setState);
	virtual	void	ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);
	virtual void	UpdateAttackChara(void)		{}
	char*			GetModelName(void);
	void			ChangeBody2Player(void);


protected:
	/*=====================================*
	 * phaseState
	 *=====================================*/
	virtual void		phaseEnterStateIdle(void);
	virtual void		phaseStateIdle(void);
	virtual void		phaseEnterStateTurn(void);
	virtual void		phaseStateTurn(void);
	virtual void		phaseEnterStateMove(void);
	virtual void		phaseStateMove(void);
	virtual void		phaseEnterStateJumpUp(void);
	virtual void		phaseStateJumpUp(void);
	virtual void		phaseEnterStateJumpDown(void);
	virtual void		phaseStateJumpDown(void);
	virtual void		phaseEnterStateJumpEnd(void);
	virtual void		phaseStateJumpEnd(void);
	virtual void		phaseEnterStateDamage(void);
	virtual void		phaseStateDamage(void);
	virtual void		phaseEnterStateRetireStart(void);
	virtual void		phaseStateRetireStart(void);
	virtual void		phaseEnterStateRetireDoing(void);
	virtual void		phaseStateRetireDoing(void);
	virtual void		phaseEnterStateRetireEnd(void);
	virtual void		phaseStateRetireEnd(void);
	virtual void		phaseEnterStateSlopeDown(void);
	virtual void		phaseStateSlopeDown(void);
	virtual void		phaseEnterStateRefuseInput(void);
	virtual void		phaseStateRefuseInput(void);
	virtual void		phaseEnterStateDownStart(void);
	virtual void		phaseStateDownStart(void);
	virtual void		phaseEnterStateDownDoing(void);
	virtual void		phaseStateDownDoing(void);
	virtual void		phaseEnterStateDownEnd(void);
	virtual void		phaseStateDownEnd(void);
	virtual void		phaseEnterStateFallDown(void);
	virtual void		phaseStateFallDown(void);

	/* 拡張ステータス */
	virtual void		phaseEnterStateAttack1(void);
	virtual void		phaseStateAttack1(void);
	virtual void		phaseEnterStateSleep(void);
	virtual void		phaseStateSleep(void);
	virtual void		phaseEnterStateBootDown(void);
	virtual void		phaseStateBootDown(void);
	virtual void		phaseEnterStateChangeLino(void);
	virtual void		phaseStateChangeLino(void);

	/* キャラの攻撃ステータス遷移のチェック関数 */
	virtual void		processLandingFunc(void);
	void				updateLastEarthPos(void);					// mLastEarthPos更新処理


	/*変数*/
	typedef void (CharaGuardian::*PHASE_STATE)(void);
	static PHASE_STATE mPhaseEnterState[eCHARA_STATE_MAX];
	static PHASE_STATE mPhaseState[eCHARA_STATE_MAX];

private:
	// 真似フラグ
	BOOL				mEmulateFlag;

};

#endif // __CHARA_GUARDIAN_H__

/**** end of file ****/

