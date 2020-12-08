/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * シーン統括マネージャクラス
 * @author SPATZ.
 * @data   2011/11/21 00:28:49
 */
//---------------------------------------------------
#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__

#include "../AppLib/Graphic/LayoutBase.h"
#include "SceneBase.h"

/*=====================================*
 * define
 *=====================================*/
#define FADE_BOARD()	(SceneManager::GetInstance()->GetFadeLayout())

/* フェード関連 */
#define FADE_IN(sec)	(FADE_BOARD()->StartFade(0, sec))
#define FADE_OUT(sec)	(FADE_BOARD()->StartFade(255, sec))
#define FADE_VAL(sec, val)		(FADE_BOARD()->StartFade(val, sec))
#define IS_FADE()		(FADE_BOARD()->IsFade())
#define IS_BLACK_FADE()	((FADE_BOARD()->GetAlpha() == 255) ? true : false)

#define FADE_COL_WHITE()		(FADE_BOARD()->SetColor(255, 255, 255))
#define FADE_COL_BLACK()		(FADE_BOARD()->SetColor(0, 0, 0))
#define FADE_COL_BLUE()			(FADE_BOARD()->SetColor(200, 220, 255))
#define FADE_COL_ORANGE()		(FADE_BOARD()->SetColor(255, 128, 0))

/* カウンタ関連 */
#define SEC2FRAME(second)						(second * 60.0f)
#define EQUAL_PHASE_SECOND(second)				(((SEC2FRAME(second) >= mPhaseCounter) && (SEC2FRAME(second) < mPhaseCounter + kADD_PHASE_COUNTER_VALUE)) ? true : false)
#define BETWEEN_PHASE_SECOND(secMin, secMax)	(((SEC2FRAME(secMin) <= mPhaseCounter) && (SEC2FRAME(secMax) > mPhaseCounter) ) ? true : false)

#define EQUAL_PHASE_FRAME(second)				(((second >= mPhaseCounter) && (second < mPhaseCounter + kADD_PHASE_COUNTER_VALUE)) ? true : false)
#define BETWEEN_PHASE_FRAME(secMin, secMax)		(((secMin <= mPhaseCounter) && (secMax > mPhaseCounter) ) ? true : false)

static const float		kADD_PHASE_COUNTER_VALUE = 1.0f;

/*=====================================*
 * class
 *=====================================*/
class SceneManager
{
 public:
	enum {
		eSCENE_TYPE_GAME = 0,
		eSCENE_TYPE_MOVIE,

		eSCENE_TYPE_MAX,
	};

	enum {
		ePHASE_LOAD_RESOURCE = 0,
		ePHASE_SELECT_MENU,
		ePHASE_DEBUG_DOING,

		ePHASE_MAX,
	};

	enum eLoadImage{
		eLOAD_IMAGE_KEY = 0,
		eLOAD_IMAGE_JOY,

		eLOAD_IMAGE_MAX,
	};

	static SceneManager* const GetInstance(void);

	void		Create(void);
	void		Setup(void);
	void		Update(void);
	void		Destroy(void);

	bool		CheckMainLoop(void);
	LayoutBase*	GetFadeLayout(void);
	LayoutBase*	GetInfoLayout(void);
	void		RequestChangeTask(int setTask);

	void		ChangeLoadingImage(eLoadImage loadImage);

	void		EntryDebugTask(void);
	void		RemoveDebugTask(void);

private:
	SceneManager();
	~SceneManager();

	int					mSceneType;

	int					mNowPhase;
	int					mNowMenu;
	int					mMenuArg;
	SceneBase*			mpMainSceneInst;

	int					mRequestChangeTask;

	typedef bool(SceneManager::*CheckLoopFunc)(void);
	static CheckLoopFunc mCheckLoopFunc[eSCENE_TYPE_MAX];

	bool	checkLoopFuncGame(void);
	bool	checkLoopFuncMovie(void);

	typedef void(SceneManager::*UpdateFunc)(void);
	static UpdateFunc mUpdateFunc[eSCENE_TYPE_MAX];

	void	updateFuncGame(void);
	void	updateFuncMovie(void);

	/* 関数ポインタ */
	typedef void(SceneManager::*PhaseFunc)(void);
	static PhaseFunc mPhaseFunc[ePHASE_MAX];

	void changePhase(int setPhase);
	void phaseLoadResource(void);
	void phaseSelectMenu(void);
	void phaseDebugDoing(void);

};

#endif // __MODEL_PMD_H__

 /**** end of file ****/
