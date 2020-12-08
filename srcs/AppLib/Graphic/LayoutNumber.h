/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 画像ベースクラス
 * @author SPATZ.
 * @data   2012/02/18 17:56:09
 */
//---------------------------------------------------
#ifndef __LAYOUT_NUMBER_H__
#define __LAYOUT_NUMBER_H__

#include "LayoutBase.h"

/*=====================================*
 * class
 *=====================================*/
class LayoutNumber : public LayoutBase
{
 public:

	LayoutNumber();
	~LayoutNumber();

	virtual void		Create(void);
	virtual void		Setup(void);
	virtual void		Update(void);
	virtual void		Destroy(void);

	void				LinkLayout(int setNumber, const char* fileName);
	void				UnlinkLayout(void);
	void				SetNumber(int number);

private:
	int					mLayoutNumberHandle[10];

};

#endif // __LAYOUT_NUMBER_H__

 /**** end of file ****/

