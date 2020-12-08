/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
#ifndef __BILLBOARD_BASE_H__
#define __BILLBOARD_BASE_H__

/*=====================================*
 * class
 *=====================================*/
class BillboardBase {

 public:
	
	/* ä÷êî */
	BillboardBase()		{}
	~BillboardBase()	{}

	virtual void		Update(void) = 0;
};

#endif // __BILLBOARD_BASE_H__

 /**** end of file ****/
