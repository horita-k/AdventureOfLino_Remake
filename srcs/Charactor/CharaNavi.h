/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ナビォクラス
 * @author SPATZ.
 * @data   2013/04/02 21:32:13
 */
//---------------------------------------------------
#ifndef __CHARA_NAVI_H__
#define __CHARA_NAVI_H__

#include "CharaNPC.h"

/*=====================================*
 * class
 *=====================================*/
class CharaNavi : public CharaNpc {
 public:

	/* 定義 */

	/* 関数 */
	CharaNavi();
	~CharaNavi();

	void		Update(void);
	void		EnableNaviCursor(void);
	void		DisableNaviCursor(void);

	void		EnableNaviLight(bool isEnable);

 private:
	/*=====================================*
	 * phaseState
	 *=====================================*/
	BYTE		mEnableNaviLight;

};

#endif // __CHARA_NAVI_H__

 /**** end of file ****/

