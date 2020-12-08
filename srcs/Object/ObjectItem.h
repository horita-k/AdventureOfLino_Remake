/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * オブジェクトベースクラス
 * @author SPATZ.
 * @data   2012/10/01 23:27:16
 */
//---------------------------------------------------
#ifndef __OBJECT_ITEM_H__
#define __OBJECT_ITEM_H__

#include "ObjectBase.h"

/*=====================================*
 * class
 *=====================================*/
class ObjectItem : public ObjectBase {
 public:

	/* 関数 */
	ObjectItem(bool isDisappear=true);
	~ObjectItem();

	virtual void		Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg);
	virtual void		Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg, bool isReserve);
	virtual void		Update(void);

	virtual	bool		CheckDamageCollNormal(tCheckDamageWork* pParam)		{return false;}
	virtual	void		ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType) {}

	bool				GetIsActive(void)				{ return mIsActive; }
	void				SetUsed(void);

 private:
	/*変数*/
	bool		mIsActive;
	bool		mIsDisapper;

	eItemKind	convObjectKind_To_ItemKind(int objectKind);
};

/*=====================================*
 * ObjectItem_CheckPoint
 * チェックポイントクラス
 *=====================================*/
class ObjectItem_CheckPoint : public ObjectItem {
public:

	/* 関数 */
	ObjectItem_CheckPoint();
	~ObjectItem_CheckPoint();

	void		Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg);
	void		Update(void);
	void		SetVisible(bool isVisible, int frameIndex=-1);

	/* 変数 */
	int					mEffHndAura;
};


#endif // __OBJECT_ITEM_H__

 /**** end of file ****/

