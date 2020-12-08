/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * オブジェクトベースクラス
 * @author SPATZ.
 * @data   2012/10/01 23:27:16
 */
//---------------------------------------------------
#ifndef __OBJECT_SWITCH_H__
#define __OBJECT_SWITCH_H__

#include "ObjectBase.h"

/*=====================================*
 * class
 *=====================================*/
class ObjectSwitch : public ObjectBase {
 public:

	typedef void (*NOTIFY_FUNC)(void);

	enum {
		eSWITCH_KIND_NONE = -1,
		eSWITCH_KIND_MOGU = 0,
		eSWITCH_KIND_FACE,
		eSWITCH_KIND_CRYSTAL,
		eSWITCH_KIND_GETDOWN,

		eSWITCH_KIND_MAX,
	};

	/* 関数 */
	ObjectSwitch(int eventPlaceIndex, BYTE objectKind);
	~ObjectSwitch();
	char*			GetModelName(void)		{ return (char *)"スイッチ"; }

	void		Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg);
	void		Update(void);
	bool		CheckDamageCollNormal(tCheckDamageWork* pParam);
	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);

	void		SetUsed(void);
	void		SetUnused(void);
	bool		CheckEnableAttack(void)	{ return true; }

	int			GetSwitchKind(void)		{ return mSwitchKind; }


	static		NOTIFY_FUNC sNotifyUpdateCrystalFunc;

 private:
	/*変数*/
	int			mEventPlaceIndex;
	BYTE		mSwitchKind;


};

#endif // __OBJECT_SWITCH_H__

 /**** end of file ****/

