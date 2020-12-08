/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ライブラリの色々融合したテスト
 * @author SPATZ.
 * @data   2011/11/21 00:58:44
 */
//---------------------------------------------------
#include <stdio.h>
#include <math.h>

#include <d3dx9math.h>

#include "SceneManager.h"
#include "SceneTitle.h"
#include "CommandFunctions.h"

#include "AppLib/Basic/Basic.h"
#include "AppLib/Basic/Counter.h"
#include "AppLib/Input/InputManager.h"

#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Resource/ResourceManager.h"
#include "AppLib/Resource/ResourceLayout.h"

#include "AppLib/Graphic/RenderBloom.h"
#include "AppLib/Effect/EffectManager.h"

#include "DataDefine/VoiceDef.h"
#include "DataDefine/EffectDef.h"

#include "dxlib.h"
#include "Utility/Utility.h"

/*=====================================*
 * フェーズ関数
 *=====================================*/
/* static */SceneTitle::PHASE_FUNC
SceneTitle::mPhaseFunc[SceneTitle::ePHASE_MAX] = {
	&SceneTitle::phaseLoadBefore,
	&SceneTitle::phaseLoading,

	&SceneTitle::phaseLogoSetup,
	&SceneTitle::phaseLogoBefore,
	&SceneTitle::phaseLogo,

	&SceneTitle::phaseSetup,
	&SceneTitle::phaseTitleBefore,
	&SceneTitle::phaseTitle,
};

/*=====================================*
 * tLayoutSetting 
 *=====================================*/
static const tLayoutSetting sLayoutSettingLog[] = {
	// Name						Pos				Size			Alpha
	{"TitleLogo.jpg",			320, 240,		192, 144,		255,},
};

static const tLayoutSetting sLayoutSettingTitle[] = {
	// Name						Pos				Size			Alpha
	{RES_LYT_TITLE_PRESSANYKEY,	318, 420,		268, 62,		0,},
	{RES_LYT_TITLE_SPATZ,		320, 458,		416, 54,		0,},
};
static const float sLayoutScale[] = {
	0.75f,		// TitlePressAnyKey.png
	0.6f,		// TitleSpatz.png
};

/*=====================================*
 * define 
 *=====================================*/
static const float kBOARD_START_POS_Y	= 360.0f;//下
static const float kBOARD_END_POS_Y		= 10.0f;//上
static const float kBOARD_POS_Z			= 220.0f;
static const float kBOARD_LYT_SCALE_Y	= 180.0f;
static const float kBOARD_LYT_SCALE_Z	= -320.0f;
static const float kBOARD_BACK_WIDTH	= 720.0f / 2.0f;
static const float kBOARD_BACK_HEIGHT	= 960.0f / 2.0f;

static const USHORT kMOVE_FRAME_START	= 16;
static const USHORT kMOVE_FRAME_END		= 1240;

static const USHORT kMOVE_FRAME_FADE_OUT= 970;
//static const USHORT kMOVE_FRAME_FADE_IN	= 1360;

static const USHORT kBLINK_FRAME_START	= 1480;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
SceneTitle::SceneTitle()
{
	mIsNext = FALSE;
	mPhase = ePHASE_LOAD_BEFORE;

	mLayoutBoardPos = ZERO_VEC;
	mBoardMoveY		= 0.0f;

	mEfsParticle	= -1;
	mIsExistSaveData= FALSE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
SceneTitle::~SceneTitle()
{
	FADE_OUT(0.0f);

	destroy();

	// エフェクトの破棄
	PRINT_CONSOLE(" EffectManager::Destroy() \n");
	EffectManager::GetInstance()->Destroy();

	//	StopMusic();
}

//---------------------------------------------------
/**
   作成
 */
//---------------------------------------------------
void
SceneTitle::Create(int debugArg)
{
	SceneBase::Create(debugArg);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneTitle::Update(void)
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
int
SceneTitle::GetLoadingPercent(void)
{
	return -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneTitle::destroy(void)
{
	GraphicManager::GetInstance()->RemoveDrawLayout(&mTitleLog);

	GraphicManager::GetInstance()->RemoveDrawLayout(&mLayoutPressAnyKey);
	GraphicManager::GetInstance()->RemoveDrawLayout(&mLayoutSpatz);


	FADE_COL_BLACK();
}

/*=====================================*
 * phase
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneTitle::phaseLoadBefore(void)
{
	StopMusic();

	mIsNext = FALSE;
	mPhase = SceneTitle::ePHASE_LOADING;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneTitle::phaseLoading(void)
{
	ResourceLayout::GetInstance()->LoadLayoutResource("TitleLogo.jpg");
	
	// 他のリソースは別タイミングでロードする


	//	mPhase = SceneTitle::ePHASE_SETUP;
	mPhase = SceneTitle::ePHASE_LOG_SETUP;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneTitle::phaseLogoSetup(void)
{
	const tLayoutSetting* p_setting;
	LayoutBase* p_layout = NULL;
	
	p_setting = &sLayoutSettingLog[0];
	p_layout = &mTitleLog;
	p_layout->LinkLayout(p_setting->mStrLayoutName);
	p_layout->SetPositionByCentering(p_setting->mPosX, p_setting->mPosY);
	p_layout->SetSize(p_setting->mSizeX, p_setting->mSizeY);
	p_layout->SetAlpha(p_setting->mAlpha);
	GraphicManager::GetInstance()->EntryDrawLayout(p_layout);

	TotalInfo::GetInstance()->HideBattleIcon();

	mPhase = SceneTitle::ePHASE_LOG_BEFORE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneTitle::phaseLogoBefore(void)
{
	// メニュー、及び色彩補正を無効に
	TotalInfo::GetInstance()->SetEnableWinMenu(FALSE);
	TotalInfo::GetInstance()->SetEnableColorRevision(FALSE);

	SoundManager::GetInstance()->StopBGM();

	FADE_IN(1.0f);

	// セーブデータの存否チェック
	if (TotalInfo::GetInstance()->LoadDataToBuffer(NULL) == TotalInfo::eLOAD_SUCEESS) {
		mIsExistSaveData = TRUE;
	}

	mPhase = SceneTitle::ePHASE_LOG;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneTitle::phaseLogo(void)
{
	if ((mIsExistSaveData == TRUE) && (CHECK_TRIG_ANY_JOY())) {
		FADE_OUT(0.5f);
		mIsNext = TRUE;
	}

	if (EQUAL_PHASE_SECOND(3.0f)) {
		FADE_OUT(1.0f);
		mIsNext = TRUE;
	}

	if ((mIsNext == TRUE) &&
		(IS_FADE() == false) ) {
		GraphicManager::GetInstance()->RemoveDrawLayout(&mTitleLog);
		mPhase = SceneTitle::ePHASE_SETUP;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneTitle::phaseSetup(void)
{
	// このタイミングでロードを行う
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_TITLE_BACK);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_TITLE_SUB2);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_TITLE_SUB);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_TITLE_MAIN);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_TITLE_PRESSANYKEY);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_TITLE_SPATZ);

	LayoutBase* p_layout = NULL;

	const LayoutBase* spLayoutBaseTable[] = {
		&mLayoutPressAnyKey,
		&mLayoutSpatz,
	};

	const tLayoutSetting* p_setting;
	for (int iI = 0; iI < (sizeof(spLayoutBaseTable) / sizeof(LayoutBase *)); iI ++) {
		p_setting = &sLayoutSettingTitle[iI];
		p_layout = (LayoutBase *)spLayoutBaseTable[iI];
		p_layout->LinkLayout(p_setting->mStrLayoutName);
		p_layout->SetPositionByCentering(p_setting->mPosX, p_setting->mPosY);

		p_layout->SetSize((int)((((float)p_setting->mSizeX * 640.0f) / 800.0f) * sLayoutScale[iI]),
						  (int)((((float)p_setting->mSizeY * 480.0f) / 600.0f) * sLayoutScale[iI]));
		
		p_layout->SetAlpha(p_setting->mAlpha);
		GraphicManager::GetInstance()->EntryDrawLayout(p_layout);
	}

	EffectManager::GetInstance()->Create(eEF_MAX);
	EffectManager::GetInstance()->LoadResource(eEF_FOREST, "Forest/Forest.fx",	"ModelCommon/FlameMdlCmn.x");	//	eEF_FOREST


	// フォグを無効にする
	GraphicManager::GetInstance()->DisableFogPattern();

	CameraManager::GetInstance()->SetPerspectiveCamera(kDEFAULT_PERSPECTIVE_FOV);

	mPhase = SceneTitle::ePHASE_TITLE_BEFORE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneTitle::phaseTitleBefore(void)
{
	mPhase = SceneTitle::ePHASE_TITLE;

	int reso_type = TotalInfo::GetInstance()->GetConfigVideoSize();
	const tDisplaySetting* p_setting = NULL;
	p_setting = &sDisplaySetting[reso_type];

	float res_rate;
	float rate_y;
	float end_board_pos_y;

	res_rate = TotalInfo::GetInstance()->GetConfigResolutionRateFromGraphQuality();
	mLayoutBoardPos.x = p_setting->mResolusionX * res_rate / 2.0f;
	rate_y = (float)p_setting->mResolusionY * res_rate / 360.0f;
	
	end_board_pos_y		= (kBOARD_LYT_SCALE_Y * (rate_y - 1.0f) ) + kBOARD_END_POS_Y;
	mLayoutBoardPos.y	= (kBOARD_LYT_SCALE_Y * (rate_y - 1.0f) ) + kBOARD_START_POS_Y;
	mLayoutBoardPos.z	= (kBOARD_LYT_SCALE_Z * (rate_y - 1.0f) ) + kBOARD_POS_Z;

	// 移動量を求めておく
	mBoardMoveY = (end_board_pos_y - mLayoutBoardPos.y) / (float)(kMOVE_FRAME_END - kMOVE_FRAME_START);

	// 描画設定をバイリニア法に設定
	SetDrawMode(DX_DRAWMODE_BILINEAR);

	// カメラの設定
	CameraManager::GetInstance()->Position()= VGet(0.0f, 2000.0f, 8000.0f);
	CameraManager::GetInstance()->Target()	= ZERO_VEC;
	CameraManager::GetInstance()->Up()		= VGet(0.0f, 1.0f, 0.0f);

	RenderBloom::SetActiveBloom(TRUE);
	RenderBloom::SetBloomType(RenderBloom::eBLOOM_TYPE_BRIGHT_GREATER);
	RenderBloom::SetBloomParam(21, 0, 0);
	RenderBloom::tBloomBrightParam param = {0};
	param.mClipParam = 255;
	RenderBloom::BloomBrightParam() = param;

	FADE_IN(3.0f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneTitle::phaseTitle(void)
{
	LayoutBase* p_layout = NULL;

	int reso_type = TotalInfo::GetInstance()->GetConfigVideoSize();
	const tDisplaySetting* p_setting = &sDisplaySetting[reso_type];
	float res_rate = TotalInfo::GetInstance()->GetConfigResolutionRateFromGraphQuality();
	float rate_y = (float)p_setting->mResolusionY * res_rate / 360.0f;

	// ロゴの最終位置の指定
	const float end_board_pos_y	= (kBOARD_LYT_SCALE_Y * (rate_y - 1.0f) ) + kBOARD_END_POS_Y;

    // ３Ｄ空間上に画像を描画
    DrawModiBillboard3D( mLayoutBoardPos,
						 -kBOARD_BACK_WIDTH,	kBOARD_BACK_HEIGHT,
						 kBOARD_BACK_WIDTH,		kBOARD_BACK_HEIGHT,
						 kBOARD_BACK_WIDTH,		-kBOARD_BACK_HEIGHT,
						 -kBOARD_BACK_WIDTH,	-kBOARD_BACK_HEIGHT,
						 ResourceLayout::GetInstance()->GetHandle(RES_LYT_TITLE_BACK),
						 TRUE);

	if (kMOVE_FRAME_END <= mPhaseCounter) {
	    // ３Ｄ空間上に画像を描画
		VECTOR board_pos;
		int lyt_size_x = (int)(824.0f * 0.25f);
		int lyt_size_y = (int)(844.0f * 0.25f);

		board_pos = VGet((798.0f / 1.25f) * ((float)p_setting->mResolusionX * res_rate / 1280.0f),
						 (542.0f / 1.25f) * ((float)p_setting->mResolusionY * res_rate / 720.0f),
						 mLayoutBoardPos.z);
		DrawModiBillboard3D( board_pos,
							 -lyt_size_x,	 lyt_size_y,	  lyt_size_x,	 lyt_size_y,
							 lyt_size_x,	-lyt_size_y,	 -lyt_size_x,	-lyt_size_y,
							 ResourceLayout::GetInstance()->GetHandle(RES_LYT_TITLE_SUB), TRUE);
		board_pos = VGet(((796.0f * (float)p_setting->mResolusionX * res_rate) / (1.25f * 1280.0f)), 
						 ((535.0f * (float)p_setting->mResolusionY * res_rate) / (1.25f * 720.0f)),
						 mLayoutBoardPos.z);
		DrawModiBillboard3D( board_pos,
							 -lyt_size_x,	 lyt_size_y,	  lyt_size_x,	 lyt_size_y,
							 lyt_size_x,	-lyt_size_y,	 -lyt_size_x,	-lyt_size_y,
							 ResourceLayout::GetInstance()->GetHandle(RES_LYT_TITLE_MAIN), TRUE);

		lyt_size_x = (int)((float)lyt_size_x * 0.8f);
		lyt_size_y = (int)((float)lyt_size_y * 0.8f);
		
		board_pos = VGet((826.0f / 1.25f) * ((float)p_setting->mResolusionX * res_rate / 1280.0f),
						 (523.0f / 1.25f) * ((float)p_setting->mResolusionY * res_rate / 720.0f),
						 mLayoutBoardPos.z);
		DrawModiBillboard3D( board_pos,
							 -lyt_size_x,	 lyt_size_y,	  lyt_size_x,	 lyt_size_y,
							 lyt_size_x,	-lyt_size_y,	 -lyt_size_x,	-lyt_size_y,
							 ResourceLayout::GetInstance()->GetHandle(RES_LYT_TITLE_SUB2), TRUE);
	}

	if (mEfsParticle != -1) {
		VECTOR pos = VAdd(mLayoutBoardPos, VGet(0.0f, -5500.0f, 2100.0f) );
		EffectManager::GetInstance()->Position(mEfsParticle) = pos;

		EffectManager::GetInstance()->SetFloat(mEfsParticle, "particleSpread", 8.0f);
	}
	
	if (EQUAL_PHASE_SECOND(0)) {
		SoundManager::GetInstance()->PlayBGM(BGM_TITLE);

		RenderBloom::ChangeBloomParam(((float)kMOVE_FRAME_FADE_OUT/60.0f), 0, 1000);
	} 
	else if (EQUAL_PHASE_FRAME(180.0f)) {
		mEfsParticle = EffectManager::GetInstance()->Play(eEF_FOREST, 
														  &mLayoutBoardPos,
														  10000.0f, 
														  16.0f);
		EffectManager::GetInstance()->SetFloat(mEfsParticle, "particleSpeed", 0.06f);
	}
	else if (EQUAL_PHASE_FRAME(kMOVE_FRAME_FADE_OUT + 60.0f)) {
		RenderBloom::ChangeBloomParam(3.0f, 255, 1000);
	}
	else if (EQUAL_PHASE_FRAME(kMOVE_FRAME_FADE_OUT + 120.0f)) {
		FADE_COL_WHITE();
		FADE_OUT(2.0f);
	}
	else if (EQUAL_PHASE_FRAME(kMOVE_FRAME_END)) {

		static const float kLAYOUT_FADE_SECOND = 1.6f;

		FADE_COL_WHITE();
		FADE_IN(kLAYOUT_FADE_SECOND);


		RenderBloom::SetBloomParam(21, 255, 1000);
		RenderBloom::ChangeBloomParam(kLAYOUT_FADE_SECOND, 0, 1000);

		STOP_EFFECT(mEfsParticle);
	}
	else if (EQUAL_PHASE_FRAME(1412)) {
		//		mLayoutPressAnyKey.StartFade(200, 1.0f);
		mLayoutSpatz.StartFade(200, 1.0f);
	}

	if (BETWEEN_PHASE_FRAME(kMOVE_FRAME_START, kMOVE_FRAME_END)) {
		mLayoutBoardPos.y += mBoardMoveY;
	}

	// Press Any Key の処理
	// 点滅処理
	if (mPhaseCounter >= kBLINK_FRAME_START) {
		BYTE alpha;
		float blink_cnt = (mPhaseCounter - (float)kBLINK_FRAME_START);

		p_layout = &mLayoutPressAnyKey;
		alpha = p_layout->GetAlpha();
		p_layout->SetAlpha( (int)(sin(blink_cnt * 0.05f) * 127.0f) + 127 );

		if (CHECK_TRIG_ANY_JOY()) {
			SoundManager::GetSndComponent()->PlaySound(eSE_PRESS_KEY);
			SceneManager::GetInstance()->RequestChangeTask(eMENU_SELECT_FILE);
			this->EndScene();
		}
	}
	else if (mPhaseCounter <= kMOVE_FRAME_END) {

		if ((mIsExistSaveData == TRUE) && (CHECK_TRIG_ANY_JOY())) {

			SoundManager::GetSndComponent()->PlaySound(eSE_PRESS_KEY);
			mLayoutSpatz.StartFade(200, 0.5f);

			RenderBloom::SetBloomParam(21, 255, 1000);
			RenderBloom::ChangeBloomParam(1.0f, 0, 1000);

			FADE_IN(0.5f);

			mPhaseCounter = kBLINK_FRAME_START;

			mLayoutBoardPos.y = end_board_pos_y;

			STOP_EFFECT(mEfsParticle);
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

/**** end of file ****/


