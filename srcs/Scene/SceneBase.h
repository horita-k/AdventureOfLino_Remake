/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * SceneBase.h
 * デバッグメニューのインターフェース
 * @author SPATZ.
 * @data   2010/02/22 01:29:14
 */
//---------------------------------------------------
#ifndef __SCENE_BASE_H__
#define __SCENE_BASE_H__

#include "../AppLib/Input/InputManager.h"

/*=====================================*
 * class
 *=====================================*/
class SceneBase {
public:

	/* 関数 */
	SceneBase()		{ mPhase = 0; mDebugArg = 0; mIsEndScene = false; mPhaseCounter	= 0.0f;} /* 初期化 */
	virtual ~SceneBase()	{}

	virtual void Create(void)	{}
	virtual void Create(int debugArg)	{ mDebugArg = debugArg; }
	virtual void Update(void)	{}
	virtual void Destroy(void)	{}

	//<! デバッグモード終了判定
	virtual bool CheckEndScene(void) {
		if (
#ifdef _DEBUG
			CHECK_TRIG_KEY(eKEY_BACK) || 
#endif // _DEBUG
			(mIsEndScene == true)) {
			return true;
		}
		return false;
	}

	void SetPhase(int phase)	{ mPhase = phase; }
	int	GetPhase(void)			{ return mPhase; }
	virtual int GetLoadingPercent(void) = 0;

	/* シーンを終了させる */
	static void	EndScene(void)		{ mIsEndScene = true; }
	float&	PhaseCounter(void)	{ return mPhaseCounter; }

private:
	/* 変数 */
protected:
	static int	mPhase;
	int			mDebugArg;
	float		mPhaseCounter;

private:
	static bool		mIsEndScene;
};

/*=====================================*
 * class SubSceneBase
 *=====================================*/
class SubSceneBase {
public:
	/* 関数 */
	SubSceneBase()				{}
	virtual ~SubSceneBase()		{}
	
	virtual void Create(void)=0;
	virtual bool Update(void)=0;
	virtual void Destroy(void)=0;

	virtual void ChangePhase(BYTE setPhase)=0;
	
	/* 変数 */
	BYTE		mPhase;

};


#endif // __SCENE_BASE_H__

/**** end of file ****/


