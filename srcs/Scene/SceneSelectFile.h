/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ライブラリの色々融合したテスト
 * @author SPATZ.
 * @data   2011/11/21 00:55:18
 */
//---------------------------------------------------
#ifndef __SCENE_SELECT_FILE_H__
#define __SCENE_SELECT_FILE_H__

#include "AppLib/Graphic/ModelBase.h"
#include "SceneBase.h"
#include "SceneInfo.h"
#include "AppLib/Graphic/LayoutBase.h"
#include "TotalInfo.h"
#include "AppLib/Resource/CsvAccesser.h"
#include "AppLib/Graphic/ModelMap.h"

/*=====================================*
  class
 *=====================================*/
class SceneSelectFile : public SceneBase {
public:
	/* 定数 */
	enum {
		ePHASE_LOAD_BEFORE,
		ePHASE_LOADING,
		ePHASE_SETUP,

		ePHASE_EVENT_BEFORE,
		ePHASE_EVENT,

		ePHASE_SELECT_FILE_BEFORE,
		ePHASE_SELECT_FILE,
		ePHASE_SELECT_COMMAND,

		ePHASE_CHECK_START,
		ePHASE_CHECK_COPY,
		ePHASE_CHECK_DELETE,

		ePHASE_ACT_START,
		ePHASE_ACT_COPY,
		ePHASE_ACT_DELETE,

		ePHASE_MAX,
	};

	// 本のモーション
	enum {
		eBOOK_MOTION_NONE = 0,
		eBOOK_MOTION_NEXT,
		eBOOK_MOTION_PREV,

		eBOOK_MOTION_MAX,
	};

	// レイアウト情報
	enum {
		eLYT_SELECT_FILE = 0,
		eLYT_NUMBER,
		eLYT_LINO,
		eLYT_LEFT_PAGE_MAX,

		eLYT_START_BALLOON	= eLYT_LEFT_PAGE_MAX,
		eLYT_COPY_BALLOON,
		eLYT_DELETE_BALLOON,
		eLYT_RETURN_BALLOON,
		eLYT_BALLON_NUM,

		eLYT_START			= eLYT_BALLON_NUM,
		eLYT_COPY,
		eLYT_DELETE,
		eLYT_RETURN,

		eLYT_MAX,
	};

	enum {
		eSELECT_CMD_START = 0,
		eSELECT_CMD_COPY,
		eSELECT_CMD_DELETE,
		eSELECT_CMD_RETURN,

		eSELECT_CMD_MAX,
	};

	/* 関数 */
	SceneSelectFile();
	~SceneSelectFile();

	/*=====================================*
	 * useful method
	 *=====================================*/
	void		Create(int debugArg);
	void		Update(void);
	int			GetLoadingPercent(void)			{ return -1; }
	void		GetTotalPlayTimeNowPage(UCHAR* pPlayTimeArray);

private:
	void		setLeftPageLayout(int lytHandle);
	void		setRightPageLayout(int lytHandle);

	void		fadeInLeftPageLayout(void);
	void		fadeOutLeftPageLayout(void);
	void		fadeInRightPageLayout(void);
	void		fadeOutRightPageLayout(void);

	BYTE		getProgressFieldIndex(BYTE slotIndex);
	void		updatePageLayoutAll(void);

	void		destroy(void);
	/*=====================================*
	 * phase
	 *=====================================*/
	typedef void (SceneSelectFile::*PHASE_FUNC)(void);
	static PHASE_FUNC mPhaseFunc[ePHASE_MAX];

	void		phaseLoadBefore(void);
	void		phaseLoading(void);
	void		phaseSetup(void);
	void		phaseEventBefore(void);
	void		phaseEvent(void);
	void		phaseSelectFileBefore(void);
	void		phaseSelectFile(void);
	void		phaseSelectCommand(void);

	void		phaseCheckStart(void);
	void		phaseCheckCopy(void);
	void		phaseCheckDelete(void);

	void		phaseActStart(void);
	void		phaseActCopy(void);
	void		phaseActDelete(void);
	
	/* 変数 */
	
	bool		mIsLoadSuccess;
	ModelPmd	mBookModel;
	BOOL		mIsChangePage;
	tSaveWork	mLoadWork[3];
	BYTE		mSelectCommand;

	int*		mpLayoutHandle;
	LayoutBase*	mpSelectLayout;
	BYTE		mPageIndex;
	BYTE		mCounter;

	// イベント関連
	BYTE		mEventPhase;
	CsvAccesser	mEventCsv;
	ModelMap	mModelMap;


};

#endif // __SCENE_SELECT_FILE_H__

/**** end of file ****/


