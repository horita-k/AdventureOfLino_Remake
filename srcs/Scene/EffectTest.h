/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ライブラリの色々融合したテスト
 * @author SPATZ.
 * @data   2011/11/21 00:55:18
 */
//---------------------------------------------------
#ifndef __EFFECT_TEST_H__
#define __EFFECT_TEST_H__

#include "AppLib/Graphic/ModelBase.h"
#include "SceneBase.h"
#include "SceneInfo.h"

/*=====================================*
  class
 *=====================================*/
class EffectTest : public SceneBase {
public:
	/* 定数 */

	/* 関数 */
	EffectTest();
	~EffectTest();

	/*=====================================*
	 * useful method
	 *=====================================*/
	void		Create(int debugArg);
	void		Update(void);

	int			GetLoadingPercent(void);

private:

	void		destroy(void);

	
	/* 変数 */

};

#endif // __EFFECT_TEST_H__

/**** end of file ****/


