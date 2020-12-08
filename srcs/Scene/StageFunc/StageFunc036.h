/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ステージ固有の処理
 * @author SPATZ.
 * @data   2015/08/26 22:06:24
 */
//---------------------------------------------------
#ifndef __STAGE_FUNC_036_H__
#define __STAGE_FUNC_036_H__

#include "StageFuncBase.h"

/*=====================================*
  class
 *=====================================*/
class StageFunc036 : public StageFuncBase {
public:
	/* 定数 */
	enum {
		eLIGHT_PATTEN_NAVI_LIGHT = 0,
		eLIGHT_PATTEN_FLAME_1ST,
		eLIGHT_PATTEN_FLAME,

		eLIGHT_PATTEN_MAX,
	};

	enum {
		eOBJECT_PATTERN_NORMAL = 0,
		eOBJECT_PATTERN_BOSS,

		eOBJECT_PATTERN_MAX,
	};

	enum {
		eBG_TYPE_GIMMICK_ROUTE = 24,
		eBG_TYPE_GIMMICK_2F,
		eBG_TYPE_GIMMICK_3F,
		eBG_TYPE_GIMMICK_4F,
		eBG_TYPE_GIMMICK_5F,

		eBG_TYPE_GIMMICK_REVERSE,
		eBG_TYPE_GIMMICK_AROUND_OUT,

		//		eBG_TYPE_GIMMICK_MAX,
	};

	/* 関数 */
	StageFunc036();
	~StageFunc036();

	void		SetupBeforeLoad(void);
	void		SetupAfterLoad(int& rPhase);
	void		Update(int counter);
	void		SetPatternLight(int pattern);
	void		SetPatternObject(int pattern);
	void		DestroyBuf(void);

	void		NotifyCreateEnemy(int enemyIndex);

	void		ProcessShowMapBefore(void);
	void		ProcessShowMapAfter(void);

	int			GetEffectFlameIndex(void)		{ return mEffHndFlame; }
	void		SetEffectFlameIndex(int index)	{ mEffHndFlame = index; }

private:

	void		destroy(void);

	/* 変数 */
public:
	int mEffHndTornade;
	int mEffHndFlame;


};




#endif // __STAGE_FUNC_036_H__

/**** end of file ****/


