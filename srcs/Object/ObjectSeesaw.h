/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * シーソークラス
 * @author SPATZ.
 * @data   2014/03/01 20:02:15
 */
//---------------------------------------------------
#ifndef __OBJECT_SEESAW_H__
#define __OBJECT_SEESAW_H__

#include "ObjectBase.h"

/*=====================================*
 * class
 *=====================================*/
class ObjectSeesaw : public ObjectBase {
 public:
	/* 関数 */
	ObjectSeesaw();
	~ObjectSeesaw();

	void		DeleteModel(void);

	void		Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg);
	void		Update(void);

	void		SetUsed(void);

 private:
	/*変数*/

};

#endif // __OBJECT_SEESAW_H__

 /**** end of file ****/

