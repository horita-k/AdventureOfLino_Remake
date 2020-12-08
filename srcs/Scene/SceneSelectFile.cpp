/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ライブラリの色々融合したテスト
 * @author SPATZ.
 * @data   2011/11/21 00:58:44
 */
//---------------------------------------------------
#include <math.h>

#include <d3dx9math.h>

#include "SceneManager.h"
#include "SceneSelectFile.h"
#include "Scene/EventManager.h"
#include "TotalInfo.h"
#include "CommandFunctions.h"

#include "AppLib/Basic/Basic.h"
#include "AppLib/Basic/Counter.h"
#include "AppLib/Input/InputManager.h"

#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Resource/ResourceManager.h"
#include "AppLib/Resource/ResourceLayout.h"
#include "AppLib/Resource/ResourceModel.h"

#include "AppLib/Graphic/RenderBloom.h"

#include "DataDefine/VoiceDef.h"

#include "dxlib.h"
#include "Utility/Utility.h"

/*=====================================*
 * フェーズ関数
 *=====================================*/
/* static */SceneSelectFile::PHASE_FUNC
SceneSelectFile::mPhaseFunc[SceneSelectFile::ePHASE_MAX] = {
	&SceneSelectFile::phaseLoadBefore,
	&SceneSelectFile::phaseLoading,
	&SceneSelectFile::phaseSetup,
	
	&SceneSelectFile::phaseEventBefore,
	&SceneSelectFile::phaseEvent,

	&SceneSelectFile::phaseSelectFileBefore,
	&SceneSelectFile::phaseSelectFile,
	&SceneSelectFile::phaseSelectCommand,

	&SceneSelectFile::phaseCheckStart,
	&SceneSelectFile::phaseCheckCopy,
	&SceneSelectFile::phaseCheckDelete,

	&SceneSelectFile::phaseActStart,
	&SceneSelectFile::phaseActCopy,
	&SceneSelectFile::phaseActDelete,
};

void DrawSaveDataFunc(void);

/*=====================================*
 * 画像ファイル一覧
 *=====================================*/
#define RES_LYT_INFO_SELECT_FILE		"Info_SelectFile.png"
#define RES_LYT_INFO_SELECT_LINO		"Info_SelectLino.png"
#define RES_LYT_INFO_SELECT_START		"Info_SelectStart.png"
#define RES_LYT_INFO_SELECT_COPY		"Info_SelectCopy.png"
#define RES_LYT_INFO_SELECT_DELETE		"Info_SelectDelete.png"
#define RES_LYT_INFO_SELECT_RETURN		"Info_SelectReturn.png"

/*=====================================*
 * モデルファイル一覧
 *=====================================*/
/*
// 本モデル
#define RES_MDL_BOOK_OPEN				"Book/Mdl.mv1"	
#define RES_MDL_BOOK_CLOSE				"Book/Sub.mv1"	
*/

/*=====================================*
 * static 
 *=====================================*/
static const tLayoutSetting sSelectLayoutTable[SceneSelectFile::eLYT_MAX] = {
	// 左
	{RES_LYT_INFO_SELECT_FILE,			160, 70,		192, 96,		0,},
	{RES_LYT_INFO_1,					250, 70,		96,	 96,		0,},
	{RES_LYT_INFO_SELECT_LINO,			150, 120,		192, 48,		0,},
	// 右
	{RES_LYT_BALLOON,					470, 100,		256, 64,		0,},
	{RES_LYT_BALLOON,					470, 180,		256, 64,		0,},
	{RES_LYT_BALLOON,					470, 260,		256, 64,		0,},
	{RES_LYT_BALLOON,					470, 340,		256, 64,		0,},
	{RES_LYT_INFO_SELECT_START,			470, 100+6,		184, 46,		0,},
	{RES_LYT_INFO_SELECT_COPY,			470, 180+6,		184, 46,		0,},
	{RES_LYT_INFO_SELECT_DELETE,		470, 260+6,		184, 46,		0,},
	{RES_LYT_INFO_SELECT_RETURN,		470, 340+6,		184, 46,		0,},
};

/*=====================================*
 * define
 *=====================================*/
enum {
	eFILE_INDEX_NEWGAME = 0,
	eFILE_INDEX_CONTINUE,

	eFILE_INDEX_MAX,
};

/*=====================================*
 * static
 *=====================================*/
// 自身のポインタ
static SceneSelectFile* spSelfInstance = NULL;

static BYTE		sProgressFieldIndex = 0;
static BYTE		sNowFieldIndex = 0;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
SceneSelectFile::SceneSelectFile()
{
	spSelfInstance = this;		/* 自身のポンタを設定 */

	sProgressFieldIndex	= 0;
	sNowFieldIndex		= 0;

	mIsLoadSuccess = false;

	mPhase = ePHASE_LOAD_BEFORE;
	mIsChangePage		= FALSE;

	TotalInfo::GetInstance()->ClearSaveInfo(mLoadWork[0]);
	TotalInfo::GetInstance()->ClearSaveInfo(mLoadWork[1]);
	TotalInfo::GetInstance()->ClearSaveInfo(mLoadWork[2]);

	mSelectCommand		= 0;

	mpLayoutHandle		= NULL;

	mpSelectLayout		= NULL;
	mPageIndex			= 0;
	mCounter			= 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
SceneSelectFile::~SceneSelectFile()
{
	FADE_COL_BLACK();
	FADE_OUT(0.0f);

	destroy();

	EventManager::GetInstance()->EndEvent();
}

//---------------------------------------------------
/**
   作成
 */
//---------------------------------------------------
void
SceneSelectFile::Create(int debugArg)
{
	SceneBase::Create(debugArg);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::Update(void)
{
	int phase = mPhase;

	(this->*mPhaseFunc[mPhase])();

	mPhaseCounter += kADD_PHASE_COUNTER_VALUE;
	if (phase != mPhase) {
		mPhaseCounter = 0.0f;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::GetTotalPlayTimeNowPage(UCHAR* pPlayTimeArray)
{
	// 出力バッファのサイズは呼び出し側責任
	memcpy(pPlayTimeArray, mLoadWork[mPageIndex].mSavePlayTime, sizeof(mLoadWork[mPageIndex].mSavePlayTime) );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::setLeftPageLayout(int lytHandle)
{
	MV1SetTextureGraphHandle(mBookModel.GetModelHandle(), 1, lytHandle, FALSE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::setRightPageLayout(int lytHandle)
{
	MV1SetTextureGraphHandle(mBookModel.GetModelHandle(), 2, lytHandle, FALSE);
}


//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::fadeInLeftPageLayout(void)
{
	const float fade_sec = 0.25f;

	LayoutBase* p_layout;
	for (int iI = 0; iI < eLYT_LEFT_PAGE_MAX; iI ++) {
		p_layout = (LayoutBase *)&mpSelectLayout[iI];
		p_layout->StartFade(255, fade_sec);
	}

	// ※ TotalInfo::ShowHeartLayout() を参照
	p_layout = TotalInfo::GetInstance()->GetSystemLayout(TotalInfo::eLY_HEART_00);
	p_layout->SetAlpha(0);
	p_layout->StartFade(200, fade_sec);
	p_layout = TotalInfo::GetInstance()->GetSystemLayout(TotalInfo::eLY_HEART_01);
	p_layout->SetAlpha(0);
	p_layout->StartFade(128, fade_sec);
	p_layout = TotalInfo::GetInstance()->GetSystemLayout(TotalInfo::eLY_HEART_NOW);
	p_layout->SetAlpha(0);
	p_layout->StartFade(200, fade_sec);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::fadeOutLeftPageLayout(void)
{
	const float fade_sec = 0.25f;

	LayoutBase* p_layout;
	for (int iI = 0; iI < eLYT_LEFT_PAGE_MAX; iI ++) {
		p_layout = (LayoutBase *)&mpSelectLayout[iI];
		p_layout->StartFade(0, fade_sec);
	}
	p_layout = TotalInfo::GetInstance()->GetSystemLayout(TotalInfo::eLY_HEART_00);
	p_layout->StartFade(0, fade_sec);
	p_layout = TotalInfo::GetInstance()->GetSystemLayout(TotalInfo::eLY_HEART_01);
	p_layout->StartFade(0, fade_sec);
	p_layout = TotalInfo::GetInstance()->GetSystemLayout(TotalInfo::eLY_HEART_NOW);
	p_layout->StartFade(0, fade_sec);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::fadeInRightPageLayout(void)
{
	const float fade_sec = 0.25f;

	LayoutBase* p_layout;
	for (int iI = eLYT_LEFT_PAGE_MAX; iI < eLYT_BALLON_NUM; iI ++) {
		p_layout = (LayoutBase *)&mpSelectLayout[iI];
		p_layout->StartFade(128, fade_sec);
	}
	for (int iI = eLYT_BALLON_NUM; iI < eLYT_MAX; iI ++) {
		p_layout = (LayoutBase *)&mpSelectLayout[iI];
		p_layout->StartFade(255, fade_sec);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::fadeOutRightPageLayout(void)
{
	const float fade_sec = 0.25f;

	LayoutBase* p_layout;
	for (int iI = eLYT_LEFT_PAGE_MAX; iI < eLYT_MAX; iI ++) {
		p_layout = (LayoutBase *)&mpSelectLayout[iI];
		p_layout->StartFade(0, fade_sec);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
BYTE
SceneSelectFile::getProgressFieldIndex(BYTE slotIndex)
{
	BYTE progress_index = 0;

	APP_ASSERT_PRINT( (slotIndex < 3),
					  "slotIndex is Invalid value : %d \n", slotIndex);

	for (int iI = 0; iI < eFIELD_MAX; iI ++) {
		if (mLoadWork[slotIndex].mSaveFieldInfoFlag[iI] & kFIELD_INFO_START_BIT) {
			progress_index = iI;
		}
	}
	return progress_index;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::updatePageLayoutAll(void)
{
	char* layout_back_name = NULL;
	int progres_index = 0;
	for (int iI = 0; iI < 3; iI ++) {
		progres_index = getProgressFieldIndex(iI);
		APP_ASSERT_PRINT( (progres_index < eFIELD_MAX),
						  "Invalide progres_index is %d \n", progres_index);

		layout_back_name = (char *)sFieldWorkTable[ progres_index ].mName[3];
		if (layout_back_name != NULL) {
			// ステージに応じたレイアウトを設定
			mpLayoutHandle[iI] = ResourceLayout::GetInstance()->GetHandle(layout_back_name);
		}
		else {
			// 白紙を設定
			int graph_handle = MV1GetTextureGraphHandle(mBookModel.GetModelHandle(), 0);
			mpLayoutHandle[iI] = graph_handle;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::destroy(void)
{
	LayoutBase* p_layout;

	GraphicManager::GetInstance()->RemoveDrawModel(&mBookModel);
	mBookModel.DeleteModel();

	for (int iI = 0; iI < eLYT_MAX; iI ++) {
		p_layout = (LayoutBase *)&mpSelectLayout[iI];
		GraphicManager::GetInstance()->RemoveDrawLayout(p_layout);
	}

	GraphicManager::GetInstance()->EntryDrawFontFunc(NULL);


	if (mpLayoutHandle != NULL) {
		delete[] mpLayoutHandle;
		mpLayoutHandle = NULL;
	}

	if (mpSelectLayout != NULL) {
		delete[] mpSelectLayout;
		mpSelectLayout = NULL;
	}

	mEventCsv.DeleteFile();

	GraphicManager::GetInstance()->RemoveDrawMapModel(&mModelMap);
	mModelMap.Destroy();


	// 本モデルのアンロード
	ResourceModel::GetInstance()->UnloadResource(RES_MDL_BOOK_OPEN);
	ResourceModel::GetInstance()->UnloadResource(RES_MDL_BOOK_CLOSE);
}

/*=====================================*
 * phase
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::phaseLoadBefore(void)
{
	mPhase = SceneSelectFile::ePHASE_LOADING;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::phaseLoading(void)
{
	TotalInfo::eLoadStatus load_status = TotalInfo::eLOAD_FAILED;
	bool is_conv = false;

	// レイアウトリソースを読み込み
	const char* strBookLayoutTable[] = {
		RES_LYT_INFO_SELECT_BACK_1,
		RES_LYT_INFO_SELECT_BACK_2,
		RES_LYT_INFO_SELECT_BACK_3,
		RES_LYT_INFO_SELECT_BACK_4,
		RES_LYT_INFO_SELECT_BACK_5,
		RES_LYT_INFO_SELECT_BACK_C,
	};
	//	for (int iI = 0; iI < mLayoutNum; iI ++) {
	for (int iI = 0; iI < (sizeof(strBookLayoutTable) / sizeof(char *)) ; iI ++) {
		ResourceLayout::GetInstance()->LoadLayoutResource(strBookLayoutTable[iI]);
	}


	/// 背景バックの LayoutBase の生成
	int mLayoutNum = 3; // 暫定
	mpLayoutHandle = new int[mLayoutNum];
	APP_ASSERT( (mpLayoutHandle != NULL),
				"Failed new mpLayoutHandle \n");
	
	mpSelectLayout = new LayoutBase[eLYT_MAX];
	APP_ASSERT( (mpSelectLayout != NULL),
				"Failed new mpSelectLayout \n");
	float diff;
	float wide_layout_rate = 1.0f;
	if (GraphicManager::GetInstance()->IsWideScreen()) {
		wide_layout_rate = 0.75f;
	}

	LayoutBase* p_layout;
	const tLayoutSetting* p_setting;
	for (int iI = 0; iI < eLYT_MAX; iI ++) {
		p_setting = &sSelectLayoutTable[iI];
		p_layout = (LayoutBase *)&mpSelectLayout[iI];
		// レイアウトのロード
		ResourceLayout::GetInstance()->LoadLayoutResource(p_setting->mStrLayoutName);
		p_layout->LinkLayout(p_setting->mStrLayoutName);

		diff = (320.0f - p_setting->mPosX) * wide_layout_rate;			// 中心に寄せる様にwide 補正をする
		p_layout->SetPositionByCentering((320 - (int)diff), p_setting->mPosY);

		p_layout->SetSize(p_setting->mSizeX, p_setting->mSizeY);
		p_layout->SetAlpha(p_setting->mAlpha);
		GraphicManager::GetInstance()->EntryDrawLayout(p_layout);
	}

	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_INFO_2);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_INFO_3);

	// 本モデルのロード
	ResourceModel::GetInstance()->LoadItemResource(RES_MDL_BOOK_OPEN);
	ResourceModel::GetInstance()->LoadItemResource(RES_MDL_BOOK_CLOSE);
	mBookModel.LinkModel(RES_MDL_BOOK_OPEN);

	ResourceModel::GetInstance()->LoadStageResource((char *)sResourceStageNameTable[eRES_STG_VUAL_SUB],
													false);
	mModelMap.LinkModel(sResourceStageNameTable[eRES_STG_VUAL_SUB]);
	mModelMap.Setup(1.0f);
	mModelMap.Position() = ZERO_VEC;
	mModelMap.Scale() = VGet(1.0f, 1.0f, 1.0f);
	GraphicManager::GetInstance()->EntryDrawMapModel(&mModelMap);

	// セーブデータのロード
	while (true) {
		load_status = TotalInfo::GetInstance()->LoadDataToBuffer(mLoadWork);

		if (load_status == TotalInfo::eLOAD_SUCEESS) {
			mIsLoadSuccess = true;
			break;
		}
		else if (load_status == TotalInfo::eLOAD_FAILED) {
			break;
		}

		APP_ASSERT_PRINT((load_status == TotalInfo::eLOAD_REQUEST_RELOAD),
						 "Invalid load_status : %d \n", load_status);
		is_conv = true;
	}

	if (is_conv) {
		SoundManager::GetSndComponent()->PlaySound(eSE_DECIDE_KEY);
		SYSTEM_MESSAGE("", "セーブデータの変換に成功しました！");
	}

	mPhase = SceneSelectFile::ePHASE_SETUP;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::phaseSetup(void)
{
	// ページ番号の更新
	mPageIndex = 0;

	// ページ番号
	mIsChangePage = FALSE;

	// 本モデルのロード
	mBookModel.Setup();
	mBookModel.Position()	= ZERO_VEC;
	mBookModel.Rotation().y	= PHI_F;
	mBookModel.Scale()		= VGet(14.0f, 3.0f, 12.0f);
	GraphicManager::GetInstance()->EntryDrawModel(&mBookModel);
	mBookModel.StartMotion(eBOOK_MOTION_NONE, true, 1.0f);
	
	TotalInfo::GetInstance()->HideBattleIcon();

	//	setDefaultBloomSetting();

	//	mPhase = SceneSelectFile::ePHASE_SELECT_FILE_BEFORE;
	mPhase = SceneSelectFile::ePHASE_EVENT_BEFORE;

	EventManager::GetInstance()->Setup();

	GraphicManager::GetInstance()->SetDefaultDirectionLight();
	GraphicManager::GetInstance()->SetEnableDirectionLight(true);
	GraphicManager::GetInstance()->SetEnablePLight(false);

	CameraManager::GetInstance()->SetDefaultNearFarClip();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::phaseEventBefore(void)
{
	// メニューを無効、色彩補正は有効に
	TotalInfo::GetInstance()->SetEnableWinMenu(FALSE);
	TotalInfo::GetInstance()->SetEnableColorRevision(FALSE);

	// ブルームのデフォルト設定を行う
	RenderBloom::SetActiveBloom(TRUE);
	setDefaultBloomSetting();

	mEventPhase = 0;

	mPhase = SceneSelectFile::ePHASE_EVENT;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::phaseEvent(void)
{
	const int model_num = 1;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	switch (mEventPhase) {
	case 0:
		FADE_COL_BLACK();
		FADE_OUT(0.5f);
		mEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return;

		// 必要なリソースを読み込む
		EventManager::GetInstance()->LoadCameraFile("data/Camera/selectfile.vmd");

		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, RES_MDL_BOOK_CLOSE, 0,	NULL, true);

		//		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		//		p_model->Scale() = VGet(20.0f, 20.0f, 20.0f);

		mEventCsv.LoadFile("data/CSV/selectfile.csv");
		EventManager::GetInstance()->StartEvent(&mEventCsv);

		startCsvAction2nd();

		FADE_COL_BLACK();
		FADE_IN(0.5f);
		mEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&mEventCsv);

		if (CHECK_TRIG_ANY_JOY()) {
			is_end = true;
		}

		if (is_end) {
			FADE_COL_WHITE();
			FADE_OUT(0.5f);
			mEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return;

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		mBookModel.Position() = p_model->Position();

		EventManager::GetInstance()->EndEvent();
		mEventCsv.DeleteFile();

		mPhase = SceneSelectFile::ePHASE_SELECT_FILE_BEFORE;
		return;
		
	default:
		break;
	}

	EventManager::GetInstance()->Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::phaseSelectFileBefore(void)
{
	// メニュー、及び色彩補正を無効に
	TotalInfo::GetInstance()->SetEnableWinMenu(FALSE);
	TotalInfo::GetInstance()->SetEnableColorRevision(FALSE);

	// 画角を戻す
	CameraManager::GetInstance()->SetPerspectiveCamera(kDEFAULT_PERSPECTIVE_FOV);

	CameraManager::GetInstance()->Target() = mBookModel.Position();
	CameraManager::GetInstance()->Position() = mBookModel.Position();
	CameraManager::GetInstance()->Position().y += 20.0f;
	CameraManager::GetInstance()->Position().z += 1.0f;

	FADE_COL_WHITE();
	FADE_IN(1.0f);

	// ハート表示
	TotalInfo::GetInstance()->ShowHeartLayout(mLoadWork[mPageIndex].mSaveMaxHP,
											  mLoadWork[mPageIndex].mSaveNowHP, TRUE);

	// 左ページを表示
	fadeInLeftPageLayout();

	// 今いるフィールドインデックス
	sNowFieldIndex = ConvertPlaceIndex_TO_FieldInfoDefine(mLoadWork[mPageIndex].mSavePlaceIndex);


	// 現在どこまで進んでいるかを調べる
	sProgressFieldIndex = getProgressFieldIndex(0);

	// ページレイアウトの更新
	updatePageLayoutAll();

	setLeftPageLayout(mpLayoutHandle[0]);
	setRightPageLayout(mpLayoutHandle[1]);

	mCounter = 0;

	mPhase = SceneSelectFile::ePHASE_SELECT_FILE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::phaseSelectFile(void)
{
	if (mCounter != 60) {
		if (++mCounter == 60) {
			// フォント登録
			GraphicManager::GetInstance()->EntryDrawFontFunc(DrawSaveDataFunc);
		}
	}

	if (IS_FADE() == true) {
		return;
	}

	int book_motion = mBookModel.GetMotionIndex();

	if (mIsChangePage == FALSE) {

		// 右キーによるページめくりの処理
		if ((CHECK_TRIG_JOY(JOY_INPUT_RIGHT)) || (CHECK_TRIG_KEY(eKEY_RIGHT))) {
			if (mPageIndex != 2) {
				SoundManager::GetSndComponent()->PlaySound(eSE_CHANGE_PAGE);
				mBookModel.StartMotion(eBOOK_MOTION_NEXT, false, 0.5f, NULL, 1.0f);

				APP_ASSERT_PRINT((mPageIndex + 1 < 3),
								 "Invalid array access mPageIndex : %d \n", mPageIndex);
				setLeftPageLayout(mpLayoutHandle[mPageIndex+0]);
				setRightPageLayout(mpLayoutHandle[mPageIndex+1]);

				fadeOutLeftPageLayout();
				fadeOutRightPageLayout();

				mIsChangePage = TRUE;
			}
		}
		// 左キーによるページめくりの処理
		else if ((CHECK_TRIG_JOY(JOY_INPUT_LEFT)) ||	(CHECK_TRIG_KEY(eKEY_LEFT))) {
			if (mPageIndex != 0) {
				SoundManager::GetSndComponent()->PlaySound(eSE_CHANGE_PAGE);
				mBookModel.StartMotion(eBOOK_MOTION_PREV, false, 0.5f, NULL, 1.0f);

				setLeftPageLayout(mpLayoutHandle[mPageIndex+0]);		// １フレ後に変更させる
				setRightPageLayout(mpLayoutHandle[mPageIndex+0]);

				fadeOutLeftPageLayout();
				fadeOutRightPageLayout();

				mIsChangePage = TRUE;
			}
		}
		else if (CHECK_TRIG_ANY_JOY() ) {
			SoundManager::GetSndComponent()->PlaySound(eSE_PRESS_KEY);
			fadeInRightPageLayout();

			mSelectCommand = 0;

			mPhase = SceneSelectFile::ePHASE_SELECT_COMMAND;
		}

	} 
	else {

		switch (book_motion) {
			// ページめくり終わりの処理
		case eBOOK_MOTION_NONE:
			break;

			// 右にめくっている間の処理
		case eBOOK_MOTION_NEXT:
			if (mBookModel.EqualPhaseFrame(2.0f)) {
				GraphicManager::GetInstance()->EntryDrawFontFunc(NULL);
				TotalInfo::GetInstance()->HideBattleIcon();

				mPageIndex ++;
			}
			break;

			// 左にめくっている間の処理
		case eBOOK_MOTION_PREV:
			// １フレ後に変更させる(ちらつき防止の為)
			if (mBookModel.EqualPhaseFrame(1.0f)) {
				APP_ASSERT_PRINT((mPageIndex - 1 >= 0),
								 "Invalid array access mPageIndex : %d \n", mPageIndex);
				setLeftPageLayout(mpLayoutHandle[mPageIndex-1]);
			}
			else if (mBookModel.EqualPhaseFrame(2.0f)) {

				GraphicManager::GetInstance()->EntryDrawFontFunc(NULL);
				TotalInfo::GetInstance()->HideBattleIcon();

				mPageIndex --;
			}
			break;

		default:
			break;
		}
	}

	// ページめくりモーション終了処理
	if (mBookModel.GetIsEndMotion()) {
		if ((mBookModel.GetMotionIndex() == eBOOK_MOTION_NEXT) ||
			(mBookModel.GetMotionIndex() == eBOOK_MOTION_PREV) ) {

			setLeftPageLayout(mpLayoutHandle[mPageIndex+0]);

			mBookModel.StartMotion(eBOOK_MOTION_NONE, true, 1.0f, NULL, 1.0f);

			// 現在どこまで進んでいるかを調べる
			sProgressFieldIndex = getProgressFieldIndex(mPageIndex);

			// 今いるフィールドインデックス
			sNowFieldIndex = ConvertPlaceIndex_TO_FieldInfoDefine(mLoadWork[mPageIndex].mSavePlaceIndex);

			GraphicManager::GetInstance()->EntryDrawFontFunc(DrawSaveDataFunc);

			TotalInfo::GetInstance()->ShowHeartLayout(mLoadWork[mPageIndex].mSaveMaxHP,
													  mLoadWork[mPageIndex].mSaveNowHP, TRUE);

			// 数字のレイアウトを変更
			switch (mPageIndex) {
			case 0:	mpSelectLayout[1].LinkLayout(RES_LYT_INFO_1); break;
			case 1:	mpSelectLayout[1].LinkLayout(RES_LYT_INFO_2); break;
			case 2:	mpSelectLayout[1].LinkLayout(RES_LYT_INFO_3); break;
			};

			fadeInLeftPageLayout();

			mIsChangePage = FALSE;
		}
	}

#ifdef _DEBUG
	{
		static unsigned char sWaitCnt = 0;
		/* 60秒間に１回CSVファイルが更新されているかを確認し、
		   更新されていればCSV処理を実施する */
		if ((sWaitCnt % 6) == 0) {
			// GameTestTool との連携を行う関数
			updateGameTestTool();
		}
		sWaitCnt ++;
	}
#endif // _DEBUG
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::phaseSelectCommand(void)
{
	if ((CHECK_TRIG_JOY(JOY_INPUT_DOWN)) || (CHECK_TRIG_KEY(eKEY_DOWN))) {
		SoundManager::GetSndComponent()->PlaySound(eSE_SELECT_KEY);
		mSelectCommand = SELECT_ROTATION_ADD(mSelectCommand, eSELECT_CMD_MAX);
	}
	else if ((CHECK_TRIG_JOY(JOY_INPUT_UP)) || (CHECK_TRIG_KEY(eKEY_UP))) {
		SoundManager::GetSndComponent()->PlaySound(eSE_SELECT_KEY);
		mSelectCommand = SELECT_ROTATION_SUB(mSelectCommand, eSELECT_CMD_MAX);
	}
	else if (CHECK_TRIG_JOY(JOY_INPUT_JUMP)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_CANCEL);
		fadeOutRightPageLayout();
		mPhase = SceneSelectFile::ePHASE_SELECT_FILE;
		return;
	}
	else if (CHECK_TRIG_ANY_JOY()) {
		switch (mSelectCommand) {
		case eSELECT_CMD_START:
			mPhase = SceneSelectFile::ePHASE_CHECK_START;
			break;
		case eSELECT_CMD_COPY:
			mPhase = SceneSelectFile::ePHASE_CHECK_COPY;
			break;
		case eSELECT_CMD_DELETE:
			mPhase = SceneSelectFile::ePHASE_CHECK_DELETE;
			break;
		case eSELECT_CMD_RETURN:
			SoundManager::GetSndComponent()->PlaySound(eSE_CANCEL);
			fadeOutRightPageLayout();
			mPhase = SceneSelectFile::ePHASE_SELECT_FILE;
			return;
		default:
			break;
		};
		
		SoundManager::GetSndComponent()->PlaySound(eSE_PRESS_KEY);
		return;
	}

	LayoutBase* p_layout;
	const tLayoutSetting* p_setting;
	for (int iI = eLYT_START; iI < eLYT_MAX; iI ++) {
		p_setting = &sSelectLayoutTable[iI];
		p_layout = (LayoutBase *)&mpSelectLayout[iI];
		p_layout->SetSize(p_setting->mSizeX, p_setting->mSizeY);
	}

	APP_ASSERT_PRINT( ((mSelectCommand + eLYT_START) < eLYT_MAX),
					  "mSelectCommand is Invalid value : %d \n", mSelectCommand);
	p_layout = (LayoutBase *)&mpSelectLayout[(mSelectCommand + eLYT_START)];
	p_layout->SetSize(276, 69);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::phaseCheckStart(void)
{
	Utility::eQuestion qs;

	GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_1, 
												"そんなファイルで大丈夫か？", 
												GetColor(255, 255, 255));
	qs = Utility::ExecuteQuestion();
	if (qs == Utility::eQUESTION_YES) {

		// ゲーム開始画面へ遷移
		GraphicManager::GetInstance()->EntryDrawFontFunc(NULL);

		SoundManager::GetSndComponent()->PlaySound(eSE_DECIDE_KEY);
		TotalInfo::GetInstance()->HideBattleIcon();

		fadeOutLeftPageLayout();
		fadeOutRightPageLayout();

		setLeftPageLayout(mpLayoutHandle[mPageIndex+0]);
		int graph_handle = MV1GetTextureGraphHandle(mBookModel.GetModelHandle(), 0);
		setRightPageLayout(graph_handle);

		// セーブファイルを更新
		TotalInfo::GetInstance()->UpdateLoadDataFromBuffer(mLoadWork[mPageIndex]);
		TotalInfo::GetInstance()->SetPlayintSaveSlot(mPageIndex);

		FADE_COL_BLACK();
		FADE_OUT(3.0f);
		StopMusic();

		mPhase = SceneSelectFile::ePHASE_ACT_START;
	} else if (qs == Utility::eQUESTION_NO) {

		//		GraphicManager::GetInstance()->EntryDrawFontFunc(NULL);
		//		MSG_FADE_OUT(0.4f);

		SoundManager::GetSndComponent()->PlaySound(eSE_CANCEL);
		fadeOutRightPageLayout();
		mPhase = SceneSelectFile::ePHASE_SELECT_FILE;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneSelectFile::phaseCheckCopy(void)
{
	GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_0, 
												"どのファイルにコピーしますか？",
												GetColor(255, 255, 255));
	
	char fileA[10];
	char fileB[10];
	const char* file_tmp = "ファイル";

	static const BYTE sCopyTargetTable[3][2] = {
		{1, 2},		{0, 2},		{0, 1},
	};

	sprintf_s(fileA, sizeof(fileA), "%s%d", file_tmp, sCopyTargetTable[mPageIndex][0]+1);
	sprintf_s(fileB, sizeof(fileB), "%s%d", file_tmp, sCopyTargetTable[mPageIndex][1]+1);

	BYTE selection = Utility::ExecuteSelection3(fileA, fileB, "キャンセル");
	
	BYTE copy_dest_index = 0;
	switch (selection) {
	case 0:
	case 1:
		copy_dest_index = sCopyTargetTable[mPageIndex][selection];

		SoundManager::GetSndComponent()->PlaySound(eSE_DECIDE_KEY);
		memcpy(&mLoadWork[copy_dest_index], 
			   &mLoadWork[mPageIndex], 
			   sizeof(tSaveWork));
		TotalInfo::GetInstance()->saveDataWork(copy_dest_index, mLoadWork[mPageIndex]);

		// ページレイアウトの更新
		updatePageLayoutAll();

		mCounter = 0;
		mPhase = SceneSelectFile::ePHASE_ACT_COPY;
		break;

	case 2:
		SoundManager::GetSndComponent()->PlaySound(eSE_CANCEL);
		fadeOutRightPageLayout();
		mPhase = SceneSelectFile::ePHASE_SELECT_FILE;
		break;
	default:
		break;
	};


}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void 
SceneSelectFile::phaseCheckDelete(void) 
{
	Utility::eQuestion qs;

	GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_1, 
												"ファイルを削除します。よろしいですか？",
												GetColor(255, 255, 255));
	qs = Utility::ExecuteQuestion();
	if (qs == Utility::eQUESTION_YES) {

		SoundManager::GetSndComponent()->PlaySound(eSE_DECIDE_KEY);
		TotalInfo::GetInstance()->ClearSaveInfo(mLoadWork[mPageIndex]);
		TotalInfo::GetInstance()->saveDataWork(mPageIndex, mLoadWork[mPageIndex]);

		// ページレイアウトの更新
		updatePageLayoutAll();
		setLeftPageLayout(mpLayoutHandle[mPageIndex]);

		fadeOutLeftPageLayout();
		fadeOutRightPageLayout();
		GraphicManager::GetInstance()->EntryDrawFontFunc(NULL);

		mCounter = 0;
		mPhase = SceneSelectFile::ePHASE_ACT_DELETE;
		
	} else if (qs == Utility::eQUESTION_NO) {

		SoundManager::GetSndComponent()->PlaySound(eSE_CANCEL);
		fadeOutRightPageLayout();
		mPhase = SceneSelectFile::ePHASE_SELECT_FILE;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void SceneSelectFile::phaseActStart(void)
{
	// レイアウトが非表示になるまで待つ
	if (mpSelectLayout[eLYT_SELECT_FILE].IsFade()) {
		return;
	}

	if (mBookModel.GetMotionIndex() == eBOOK_MOTION_NONE) {
		SoundManager::GetSndComponent()->PlaySound(eSE_CHANGE_PAGE);
		mBookModel.StartMotion(eBOOK_MOTION_NEXT, false, 0.5f, NULL, 1.0f);
	}

	if (mBookModel.GetIsEndMotion()) {
		if (mBookModel.GetMotionIndex() == eBOOK_MOTION_NEXT) {

			int graph_handle = MV1GetTextureGraphHandle(mBookModel.GetModelHandle(), 0);
			setLeftPageLayout(graph_handle);

			mBookModel.StartMotion(eBOOK_MOTION_NONE, true, 1.0f, NULL, 1.0f);
		}
	}

	if (IS_FADE() == false) {
		// 終了
		SceneManager::GetInstance()->RequestChangeTask(eMENU_MERGE);
		this->EndScene();
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void SceneSelectFile::phaseActCopy(void)
{
	GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_0, 
												"コピーしました！",
												GetColor(255, 255, 255));

	if (++mCounter > 90) {
		fadeOutRightPageLayout();

		mCounter = 0;
		mPhase = SceneSelectFile::ePHASE_SELECT_FILE;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void SceneSelectFile::phaseActDelete(void)
{
	GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_0, 
												"削除しました！",
												GetColor(255, 255, 255));

	if (++mCounter > 90) {
		// 現在どこまで進んでいるかを調べる
		sProgressFieldIndex = getProgressFieldIndex(mPageIndex);
		
		// 今いるフィールドインデックス
		sNowFieldIndex = ConvertPlaceIndex_TO_FieldInfoDefine(mLoadWork[mPageIndex].mSavePlaceIndex);

		TotalInfo::GetInstance()->ShowHeartLayout(mLoadWork[mPageIndex].mSaveMaxHP,
												  mLoadWork[mPageIndex].mSaveNowHP, TRUE);

		fadeInLeftPageLayout();
		GraphicManager::GetInstance()->EntryDrawFontFunc(DrawSaveDataFunc);

		mCounter = 0;
		mPhase = SceneSelectFile::ePHASE_SELECT_FILE;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void DrawSaveDataFunc(void)
{
	const int	color = GetColor(0, 0, 0);
	const BYTE	font_size = 30;

	bool is_wide = GraphicManager::GetInstance()->IsWideScreen();

	int left_pos_x		= 40;
	if (is_wide) {
		left_pos_x		= 150;
	}

	UCHAR time_work[3];
	spSelfInstance->GetTotalPlayTimeNowPage(time_work);

	if (!((time_work[eTIME_KIND_HOUR] == 0) && (time_work[eTIME_KIND_MIN] == 0) && (time_work[eTIME_KIND_SEC] == 0))) {
		char time_str[20];
		sprintf_s(time_str, sizeof(time_str), "Time : %02d:%02d:%02d", 
				  time_work[eTIME_KIND_HOUR], time_work[eTIME_KIND_MIN], time_work[eTIME_KIND_SEC]);
		GraphicManager::GetInstance()->DrawFontString(left_pos_x, 80+128-16,
													  time_str, color, font_size);
	}

	GraphicManager::GetInstance()->DrawFontString(left_pos_x, 80+128+32,
												  (char *)sFieldWorkTable[sProgressFieldIndex].mName[0], color, font_size);
	GraphicManager::GetInstance()->DrawFontString(left_pos_x, 80+128+64,		
												  (char *)sFieldWorkTable[sNowFieldIndex].mName[1], color, font_size);
	GraphicManager::GetInstance()->DrawFontString(left_pos_x, 80+128+96,
												  (char *)sFieldWorkTable[sProgressFieldIndex].mName[2], color, font_size);

}

/**** end of file ****/


