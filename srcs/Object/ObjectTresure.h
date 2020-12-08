/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * オブジェクトベースクラス
 * @author SPATZ.
 * @data   2012/10/01 23:27:16
 */
//---------------------------------------------------
#ifndef __OBJECT_TRESURE_H__
#define __OBJECT_TRESURE_H__

#include "ObjectBase.h"

/*=====================================*
 * class ObjectTresure
 *=====================================*/
class ObjectTresure : public ObjectBase {
 public:

	/* 関数 */
	ObjectTresure();
	~ObjectTresure();

	void		Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg);
	void		Update(void);

	virtual	bool		CheckDamageCollNormal(tCheckDamageWork* pParam);
	virtual	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);
	void				SetUsed(void);
	
	virtual void		StartCheckEvent(void);

 private:
	int					getTreasureKindFromObjectKind(int objectKind);
};

/*=====================================*
 * class ObjectTresureCrystal
 *=====================================*/
class ObjectTresureCrystal : public ObjectTresure {

public :
	/* 関数 */
	ObjectTresureCrystal();
	~ObjectTresureCrystal();

	void				StartCheckEvent(void);
	void				SetCrystalEvent(ULONG taskValue);


private:
	ULONG				mTaskValue;
};


#endif // __OBJECT_TRESURE_H__

 /**** end of file ****/

