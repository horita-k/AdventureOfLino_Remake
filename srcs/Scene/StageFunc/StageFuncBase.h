/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ステージ固有の処理
 * @author SPATZ.
 * @data   2015/08/26 22:06:24
 */
//---------------------------------------------------
#ifndef __STAGE_FUNC_H__
#define __STAGE_FUNC_H__

#include "Charactor/CharaGuardian.h"

/*=====================================*
  class
 *=====================================*/
class StageFuncBase {
public:
	/* 定数 */

	/* 関数 */
	StageFuncBase();
	virtual ~StageFuncBase();

	static StageFuncBase* CreateInstance(int stageIndex);

	virtual void		SetupBeforeLoad(void);
	virtual void		SetupAfterLoad(int& rPhase);

	virtual void		Update(int counter)=0;

	static void			UpdateLightInfo(void);

	virtual void		SetPatternLight(int pattern)=0;
	virtual void		SetPatternObject(int pattern)=0;
	virtual void		DestroyBuf(void){}
	virtual void		NotifyCreateEnemy(int enemyIndex){}
	virtual void		NotifyMovePlaceWhileFadeout(void) {}
	
	virtual void		ProcessShowMapBefore(void) {}
	virtual void		ProcessShowMapAfter(void)  {}

	virtual void		ReviseAllModel(VECTOR& rMoveVec)	{}

	virtual CharaGuardian* GetGuardianPtr(void) { return NULL; }
	

private:

	void				destroy(void);

protected:
	void				setVisibleEarthGimmickCollision(int gimmickFrame, BOOL isVisible);
	void				setVisibleWallGimmickCollision(int gimmickFrame, BOOL isVisible);
	void				setVisibleBarrierFromByEventFlag(int gimmickFrame, int eventA, int eventB, BOOL isScroll=TRUE);

	/* 変数 */
};

#endif // __STAGE_FUNC_H__

/**** end of file ****/


