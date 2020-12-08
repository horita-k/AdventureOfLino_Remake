/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ステージ固有の処理
 * @author SPATZ.
 * @data   2015/08/26 22:06:24
 */
//---------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <d3dx9math.h>
#include "StageFunc047.h"
#include "AppLib/Effect/EffectManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Resource/ResourceManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "AppLib/Resource/ResourceLayout.h"
#include "DataDefine/EffectDef.h"
#include "DataDefine/VoiceDef.h"
#include "Gimmick/GimmickBase.h"
#include "Gimmick/GimmickExtend.h"
#include "../CommandFunctions.h"
#include "../ExternDefine.h"		//<! Extern 宣言は全てここに集約する
#include "../SceneMainGame.h"

/*=====================================*
 * static
 *=====================================*/

/*=====================================*
 * public
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
StageFunc047::StageFunc047()
{
	mEffHndSky = -1;

	memset(&mStaffroll, 0, sizeof(mStaffroll));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
StageFunc047::~StageFunc047()
{
	destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc047::SetupBeforeLoad(void)
{
	/**** ベースクラス関数を自クラス処理後にコール ****/
	StageFuncBase::SetupBeforeLoad();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc047::SetupAfterLoad(int& rPhase)
{
	int model_handle = sModelMap.GetModelHandle();

	/**** ベースクラス関数を自クラス処理前にコール ****/
	StageFuncBase::SetupAfterLoad(rPhase);

	mpTitleLog		= NULL;
			
	spBGInfo->mMusicName = BGM_SKYSHIP;

	mEffHndSky = EffectManager::GetInstance()->Play(eEF_SKY, &sPlayer.Position(), 0.1f, 0.0f);
	EffectManager::GetInstance()->Position(mEffHndSky)	= VGet(0.0f, -2000.0f, 0.0f);
	EffectManager::GetInstance()->Scale(mEffHndSky)		= VGet(10.0f, 10.0f, 10.0f);

	if (IS_END_EVT(eEVENT_47, eEVENT_47_WELCOME_AIRSHIP) == false) {
		sTaskInfo.mTaskType		= ActionTaskManager::eTASK_EVENT;
		sTaskInfo.mTaskValueA	= (BYTE)eEVENT_47;
		sTaskInfo.mTaskValueB	= (BYTE)eEVENT_47_WELCOME_AIRSHIP;
		rPhase = SceneMainGame::ePHASE_EVENT_BEFORE;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc047::Update(int counter)
{
	TotalInfo::GetInstance()->UpdateCompass();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc047::DestroyBuf(void)
{
	if (mpTitleLog != NULL) {
		GraphicManager::GetInstance()->RemoveDrawLayout(mpTitleLog);
		delete mpTitleLog;
		mpTitleLog = NULL;
	}

	STOP_EFFECT(mEffHndSky);
}

/*=====================================*
 * private
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc047::destroy(void)
{
	if (mStaffroll.mpCsv != NULL) {
		mStaffroll.mpCsv->DeleteFile();
		delete mStaffroll.mpCsv;
		mStaffroll.mpCsv = NULL;
	}
}

/**** end of file ****/


