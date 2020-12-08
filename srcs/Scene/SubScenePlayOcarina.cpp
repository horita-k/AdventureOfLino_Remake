/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * アイテムメニュー選択
 * @author SPATZ.
 * @data   2015/07/13 23:38:22
 */
//---------------------------------------------------
#include "SceneManager.h"
#include "SceneMainGame.h"
#include "SubScenePlayOcarina.h"
#include "CommandFunctions.h"

#include "AppLib/Basic/Basic.h"
#include "AppLib/Basic/Counter.h"
#include "AppLib/Input/InputManager.h"

#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Resource/ResourceManager.h"
#include "AppLib/Resource/ResourceLayout.h"

#include "DataDefine/VoiceDef.h"
#include "DataDefine/MotionDef.h"

#include "ExternDefine.h"		//<! Extern 宣言は全てここに集約する

#include "dxlib.h"
#include "Utility/Utility.h"

/*=====================================*
 * フェーズ関数
 *=====================================*/
/* static */SubScenePlayOcarina::PHASE_FUNC
SubScenePlayOcarina::mPhaseFunc[SubScenePlayOcarina::ePHASE_MAX] = {
	&SubScenePlayOcarina::phaseNone,

	&SubScenePlayOcarina::phaseSelectEnter,
	&SubScenePlayOcarina::phaseSelectDoing,

	&SubScenePlayOcarina::phasePlayMusic,
	&SubScenePlayOcarina::phaseMessage,
	&SubScenePlayOcarina::phaseMessageLeave,

	&SubScenePlayOcarina::phaseFinish,
};

/*=====================================*
 * 唄名と説明
 *=====================================*/
static const SubScenePlayOcarina::tSongInfo sSongInfoTable[eSONG_TYPE_MAX] =  {
	// 太陽の唄
	{
		eSONG_TYPE_SUN,
		SubScenePlayOcarina::eNEXT_STATE_WAKEUP_EVENT, eSE_OCARINA_SUN, 0.7f, RES_LYT_SONG_SUN,
		//		"太陽の唄",
		{"不思議な音色を奏で、", "物や場所を眠りから覚ますぞ！",}, 
	},
	// 草原の唄
	{
		eSONG_TYPE_WILDNESS,
		SubScenePlayOcarina::eNEXT_STATE_APPEAR_TITANIS, eSE_OCARINA_WILEDNESS, 1.14f, RES_LYT_SONG_WILDNESS,
		//		"草原の唄",
		{"ステージ内にティタニスがいると", "リノの近くまでティタニスを呼ぶ事ができるぞ！",}, 
	},
	// 翼の唄
	{
		eSONG_TYPE_WING,
		SubScenePlayOcarina::eNEXT_STATE_FLYING_MOVE, eSE_OCARINA_WING, 0.63f, RES_LYT_SONG_WING,
		//		"翼の唄",
		{"リノが最初から知っている優しいメロディの曲", "",}, 
	},
	// 操りの唄
	{
		eSONG_TYPE_HANDLING,
		SubScenePlayOcarina::eNEXT_STATE_HANDLING_MAGICARMER, eSE_OCARINA_HANDLING, 1.0f, RES_LYT_SONG_HANDLING,
		//		"操りの唄",
		{"ステージ内にマジックアーマーがいると", "マジックアーマーを操る事ができるぞ！",}, 
	}
};

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
SubScenePlayOcarina::SubScenePlayOcarina()
{
	mPhase				= ePHASE_NONE;

	mNextState			= eNEXT_STATE_GAME;
	mSongWorkList.clear();

	mNextItemIndex		= 0;
	mNowItemIndex		= 0;
	mIconDist			= 0.0f;
	mRotCounter			= 0;
	mCenterPos			= Vec2(0, 0);
	mIsRight			= FALSE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
SubScenePlayOcarina::~SubScenePlayOcarina()
{
	GraphicManager::GetInstance()->RemoveSystemLayout(&mMsgLayout);

	std::vector<tSongWork>::iterator ite;
	for (ite = mSongWorkList.begin(); ite != mSongWorkList.end(); ite ++) {
		GraphicManager::GetInstance()->RemoveSystemLayout( ite->mpLayout  );

		delete ite->mpLayout;
		ite->mpLayout = NULL;
	}
	mSongWorkList.clear();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/SubScenePlayOcarina::tSongInfo* 
SubScenePlayOcarina::GetSongInfoTable(BYTE songType)
{
	return (SubScenePlayOcarina::tSongInfo *)&sSongInfoTable[songType];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubScenePlayOcarina::Create(void)
{
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_SONG_SUN);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_SONG_WILDNESS);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_SONG_WING);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_SONG_HANDLING);

	mSongWorkList.clear();
	for (UCHAR iI = 0; iI < eSONG_TYPE_MAX; iI ++) {
		if (TotalInfo::GetInstance()->IsSongType(iI) == TRUE) {

			tSongWork work = {&sSongInfoTable[iI], NULL};
			
			mSongWorkList.push_back(work);
		}
	}
	APP_ASSERT((mSongWorkList.size() != 0),
		"Empty sSongWorkList.size() \n");

	mCenterPos.x = (int)(320.0f / GraphicManager::GetInstance()->GetAspectRate());
	mCenterPos.y = 180;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
SubScenePlayOcarina::Update(void)
{
	(this->*mPhaseFunc[mPhase])();

	if (mPhase == ePHASE_FINISH) {
		return true;
	}

#ifdef _DEBUG
	static unsigned char sWaitCnt = 0;
	/* 60秒間に１回CSVファイルが更新されているかを確認し、
	   更新されていればCSV処理を実施する */
	if ((sWaitCnt % 6) == 0) {
		// GameTestTool との連携を行う関数
		updateGameTestTool();
	}
	sWaitCnt ++;
#endif // _DEBUG

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubScenePlayOcarina::Destroy(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubScenePlayOcarina::ChangePhase(BYTE setPhase)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubScenePlayOcarina::phaseNone(void)
{
	sPlayer.StartMotion(eMT_OCARINA_STANCE, true, 0.5f);

	// アタッチパターンを変更
	sPlayer.ChangeAttachPattern(CharaPlayer::eATTACH_PATTERN_OCARINA);

	// メッセージレイアウトの設定
	{
		LayoutBase* p_lyt_balloon;
		p_lyt_balloon = TotalInfo::GetInstance()->GetSystemLayout(TotalInfo::eLY_BALLOON);

		int x, y;
		mMsgLayout.LinkLayout(RES_LYT_BALLOON);
		p_lyt_balloon->GetSize(&x, &y);		
		mMsgLayout.SetSize(x, y);
		p_lyt_balloon->GetPositionByCentering(&x, &y);
		mMsgLayout.SetPositionByCentering(x, y);
		mMsgLayout.SetAlpha(0);
		GraphicManager::GetInstance()->EntrySystemLayout(&mMsgLayout,
														 GraphicManager::eSYS_LAYOUT_PRIORITY_FADE_AFTER);
	}

	mMsgLayout.StartFade(186, 0.3f);

	mPhase = ePHASE_SELECT_ENTER;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubScenePlayOcarina::phaseSelectEnter(void)
{
	ModelPmd* p_subweapon = (ModelPmd *)sPlayer.GetAttachAddr(CharaPlayer::eATTACH_MODEL_SUB_WEAPON)->mpAttachedModel;

	sPlayer.AttachOcarina(p_subweapon);

	LayoutBase* p_layout = NULL;

	for (UCHAR iI = 0; iI < mSongWorkList.size(); iI ++) {
		const tSongInfo* p_info = mSongWorkList.at(iI).mpSongInfo;

		p_layout = new LayoutBase();
		p_layout->LinkLayout(p_info->mLytName);
		p_layout->SetPositionByCentering(-100, -100);
		p_layout->SetSize(60, 60);
		p_layout->SetAlpha(255);
		GraphicManager::GetInstance()->EntrySystemLayout(p_layout,
														 GraphicManager::eSYS_LAYOUT_PRIORITY_FADE_AFTER);
		mSongWorkList.at(iI).mpLayout = p_layout;
	}	

	mNowItemIndex	= ((mNextItemIndex + (mSongWorkList.size() / 2)) % mSongWorkList.size());
	mIconDist		= 300.0f;

	SoundManager::GetSndComponent()->PlaySound(eSE_ITEM_APPEAR);

	mPhase = ePHASE_SELECT_DOING;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubScenePlayOcarina::phaseSelectDoing(void)
{
	static const UCHAR kROTATION_FRAME = 30;

	ModelPmd* p_subweapon = (ModelPmd *)sPlayer.GetAttachAddr(CharaPlayer::eATTACH_MODEL_SUB_WEAPON)->mpAttachedModel;
	sPlayer.AttachOcarina(p_subweapon);

	float theta;
	LayoutBase* p_layout = NULL;

	mIconDist = -20.0f;
	if (mIconDist < 100.0f) {
		mIconDist = 100.0f;
	}

	// 回転が止まっている時の処理
	if (mRotCounter == 0) {
		if (CHECK_TRIG_JOY(JOY_INPUT_LEFT) ) {
			mIsRight = FALSE;
			mNextItemIndex = (UCHAR)SELECT_ROTATION_ADD((unsigned int)mNextItemIndex, mSongWorkList.size());
			SoundManager::GetSndComponent()->PlaySound(eSE_SELECT_KEY);
		}
		else if (CHECK_TRIG_JOY(JOY_INPUT_RIGHT)) {
			mIsRight = TRUE;
			mNextItemIndex = (UCHAR)SELECT_ROTATION_SUB((int)mNextItemIndex, mSongWorkList.size());
			SoundManager::GetSndComponent()->PlaySound(eSE_SELECT_KEY);
		}
	}

	float div_rad = Utility::DegToRad(360.0f / (float)mSongWorkList.size());

	if (mNowItemIndex == mNextItemIndex) {
		for (UCHAR iI = 0; iI < mSongWorkList.size(); iI ++ ) {
			theta = (iI - mNowItemIndex) * div_rad;
			mSongWorkList.at(iI).mpLayout->SetPosition(mCenterPos.x - (int)(sin(theta) * mIconDist * 1.5f),
											 mCenterPos.y - (int)(cos(theta) * mIconDist));
		}
		mRotCounter = 0;
	}
	else {
		for (UCHAR iI = 0; iI < mSongWorkList.size(); iI ++) {

			float now_theta, next_theta, offset_theta;

			now_theta = (iI - mNowItemIndex) * div_rad;
			next_theta = (iI - mNextItemIndex) * div_rad;

			offset_theta = next_theta - now_theta;
			if ((mIsRight == FALSE) && (offset_theta > 0) ) {
				offset_theta += -(PHI_F * 2.0f);
			}
			if ((mIsRight == TRUE) && (offset_theta < 0) ) {
				offset_theta += (PHI_F * 2.0f);
			}
			theta = now_theta + (offset_theta / kROTATION_FRAME) * mRotCounter;
			mSongWorkList.at(iI).mpLayout->SetPosition(mCenterPos.x - (int)(sin(theta) * mIconDist * 1.5f),
											 mCenterPos.y - (int)(cos(theta) * mIconDist));
			mRotCounter ++;
			if (mRotCounter > kROTATION_FRAME) {
				mNowItemIndex = mNextItemIndex;
			}
		}
	}

	const tSongInfo* p_info = mSongWorkList.at(mNextItemIndex).mpSongInfo;

	RENDER_MESSAGE_COL("", GetSongName(p_info->mSongType), p_info->mSongExplain[0], p_info->mSongExplain[1], "", GetColor(0, 0, 0) );

	/**** 終了処理 ****/

	// 決定
	if ((CHECK_TRIG_JOY(JOY_INPUT_SELECT_SUB)) || 
		(CHECK_TRIG_JOY(JOY_INPUT_AWAY)) ||
		(CHECK_TRIG_JOY(JOY_INPUT_ATTACK))) {

		SoundManager::GetInstance()->SetVolumeBGM(0);

		mNextState = p_info->mNextState;
		
		for (UCHAR iI = 0; iI < mSongWorkList.size(); iI ++ ) {
			mSongWorkList.at(iI).mpLayout->StartFade(0, 0.2f);
		}
		mMsgLayout.StartFade(0, 0.2f);

		sPlayer.DisableLockonChara();
		TotalInfo::GetInstance()->DisableLockonLayout();

		// キャラ周辺のカメラの設定
		CameraManager::GetInstance()->SetPatternArroundPlayer(-10.0f,		// camDeg
															  -188.0f,		// camLen
															  155.0f,		// camPosY
															  115.0f);		// camTarY

		sPlayer.StartMotion(eMT_OCARINA_PLAY_START, false, 0.6f);

		mPhase = ePHASE_PLAY_MUSIC;
	}
	// キャンセル
	else if (CHECK_TRIG_JOY(JOY_INPUT_JUMP)) {

		mNextState = eNEXT_STATE_GAME;

		for (UCHAR iI = 0; iI < mSongWorkList.size(); iI ++ ) {
			mSongWorkList.at(iI).mpLayout->SetAlpha(0);
		}
		mMsgLayout.SetAlpha(0);

		mPhase = ePHASE_FINISH;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubScenePlayOcarina::phasePlayMusic(void)
{
	ModelPmd* p_subweapon = (ModelPmd *)sPlayer.GetAttachAddr(CharaPlayer::eATTACH_MODEL_SUB_WEAPON)->mpAttachedModel;
	sPlayer.AttachOcarina(p_subweapon);
	const tSongInfo* p_info = mSongWorkList.at(mNextItemIndex).mpSongInfo;


	if (sPlayer.GetMotionIndex() == eMT_OCARINA_PLAY_START) {
		if (sPlayer.GetIsEndMotion()) {
			sPlayer.StartMotion(eMT_OCARINA_PLAY_DOING, true, p_info->mMotionSpeed);
		}
	}
	else {
		if (sPlayer.EqualPhaseFrame(23.0f)) {
			SoundManager::GetSndComponent()->PlaySound(p_info->mSongSoundIndex);
		}
		else if (sPlayer.BetweenPhaseFrame(23.0f, 9999.0f)) {
			// オカリナの音色がおわったら？
			if (SoundManager::GetSndComponent()->IsPlay(p_info->mSongSoundIndex) == false) {
				sPlayer.StartMotion(eMT_OCARINA_STANCE, true, 0.5f, NULL, 0.05f);
				
				SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
				mMsgLayout.StartFade(186, 0.3f);
				mPhase = ePHASE_MESSAGE;
			}
		}
	}

	// カメラ変更入力があった場合
	int value_x, value_y;
	value_x = value_y = 0;
	InputManager::GetInstance()->GetJoyAnalogRight(&value_x, &value_y);
	if ((CHECK_PRESS_MOUSE(MOUSE_INPUT_RIGHT)) ||
		(value_x != 0) || (value_y != 0)) {
		CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_HIGH, true);
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
	}


}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubScenePlayOcarina::phaseMessage(void)
{
	ModelPmd* p_subweapon = (ModelPmd *)sPlayer.GetAttachAddr(CharaPlayer::eATTACH_MODEL_SUB_WEAPON)->mpAttachedModel;
	sPlayer.AttachOcarina(p_subweapon);

	const tSongInfo* p_info = mSongWorkList.at(mNextItemIndex).mpSongInfo;

	char str[64];

	sprintf_s(str, sizeof(str), "%sを奏でた", GetSongName(p_info->mSongType));
	RENDER_MESSAGE_COL("", "", str, "", "", GetColor(0, 0, 0) );

	if (CHECK_TRIG_ANY_JOY()) {
		SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
		mMsgLayout.StartFade(0, 0.2f);
		mPhase = ePHASE_MESSAGE_LEAVE;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubScenePlayOcarina::phaseMessageLeave(void)
{
	ModelPmd* p_subweapon = (ModelPmd *)sPlayer.GetAttachAddr(CharaPlayer::eATTACH_MODEL_SUB_WEAPON)->mpAttachedModel;
	sPlayer.AttachOcarina(p_subweapon);

	if (mMsgLayout.IsFade() == false) {
		mPhase = ePHASE_FINISH;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubScenePlayOcarina::phaseFinish(void)
{

	return;
}

/**** end of file ****/


