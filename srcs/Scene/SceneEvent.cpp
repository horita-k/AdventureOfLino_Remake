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
#include "SceneEvent.h"

#include "AppLib/Basic/Basic.h"
#include "AppLib/Basic/Counter.h"
#include "AppLib/Input/InputManager.h"

#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Resource/ResourceManager.h"
#include "AppLib/Resource/ResourceLayout.h"

#include "AppLib/Graphic/ModelPmd.h"

#include "DataDefine/VoiceDef.h"

#include "EventManager.h"

#include "dxlib.h"
#include "Utility/Utility.h"

/*=====================================*
 * フェーズ関数
 *=====================================*/
/* static */SceneEvent::PHASE_FUNC
SceneEvent::mPhaseFunc[SceneEvent::ePHASE_MAX] = {
	&SceneEvent::phaseLoadBefore,
	&SceneEvent::phaseLoading,
	&SceneEvent::phaseSetup,
	&SceneEvent::phaseEventBefore,
	&SceneEvent::phaseEvent,
};

static ModelPmd sModelPmd;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
SceneEvent::SceneEvent()
{
	mPhase = ePHASE_LOAD_BEFORE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
SceneEvent::~SceneEvent()
{
	FADE_OUT(0.0f);

	destroy();

	StopMusic();
}

//---------------------------------------------------
/**
   作成
 */
//---------------------------------------------------
void
SceneEvent::Create(int debugArg)
{
	SceneBase::Create(debugArg);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneEvent::Update(void)
{
	int phase = mPhase;

	(this->*mPhaseFunc[mPhase])();

	mPhaseCounter += (kADD_PHASE_COUNTER_VALUE / 2.0f);
	/*
	if ((CHECK_TRIG_KEY(eKEY_UP)) ||
		(CHECK_PRESS_KEY(eKEY_UP) && (CHECK_PRESS_KEY(eKEY_SPACE))))  {
		mPhaseCounter += (kADD_PHASE_COUNTER_VALUE / 2.0f);
	}
	if ((CHECK_TRIG_KEY(eKEY_DOWN)) ||
		(CHECK_PRESS_KEY(eKEY_DOWN) && (CHECK_PRESS_KEY(eKEY_SPACE))))  {
		mPhaseCounter += -(kADD_PHASE_COUNTER_VALUE / 2.0f);
	}
	*/

	if (phase != mPhase) {
		mPhaseCounter = 0.0f;
	}

	PRINT_SCREEN(GetColor(255, 255, 0), "mPhaseCounter : %.1f \n", mPhaseCounter);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
SceneEvent::GetLoadingPercent(void)
{
	return -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneEvent::destroy(void)
{
	GraphicManager::GetInstance()->RemoveDrawLayout(&mLayoutBack);
	GraphicManager::GetInstance()->RemoveDrawLayout(&mLayoutMain);
	GraphicManager::GetInstance()->RemoveDrawLayout(&mLayoutPressAnyKey);
	GraphicManager::GetInstance()->RemoveDrawLayout(&mLayoutSpatz);

	GraphicManager::GetInstance()->RemoveDrawModel(&mModelMap);
	mModelMap.DeleteModel();

	GraphicManager::GetInstance()->RemoveDrawModel(&sModelPmd);
	sModelPmd.DeleteModel();

	EventManager::GetInstance()->EndEvent();

}

/*=====================================*
 * phase
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneEvent::phaseLoadBefore(void)
{
	mPhase = SceneEvent::ePHASE_LOADING;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneEvent::phaseLoading(void)
{
	ResourceLayout::GetInstance()->LoadLayoutResource("TitleMain.png");
	ResourceLayout::GetInstance()->LoadLayoutResource("TitleBack.png");
	ResourceLayout::GetInstance()->LoadLayoutResource("TitlePressAnyKey.png");
	ResourceLayout::GetInstance()->LoadLayoutResource("TitleSpatz.png");

	mModelMap.LoadModel("data/model/BG/batokin/000_batokin.mqo");
	sModelPmd.LoadModel("data/Chara/Lino/Model/Model.mv1");
	static const char* sFileName = "data/Camera/BatokinCam.vmd";

	EventManager::GetInstance()->LoadCameraFile(sFileName);

	mPhase = SceneEvent::ePHASE_SETUP;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneEvent::phaseSetup(void)
{
	LayoutBase* p_layout;

	p_layout = &mLayoutBack;
	p_layout->LinkLayout("TitleBack.png");
	p_layout->SetPositionByCentering(320+240, 180);
	p_layout->SetSize(480, 360);
	p_layout->SetAlpha(0);
	GraphicManager::GetInstance()->EntryDrawLayout(p_layout);

	p_layout = &mLayoutMain;
	p_layout->LinkLayout("TitleMain.png");
	p_layout->SetPositionByCentering(320, 180);
	p_layout->SetSize(480, 360);
	p_layout->SetAlpha(0);
	GraphicManager::GetInstance()->EntryDrawLayout(p_layout);

	p_layout = &mLayoutPressAnyKey;
	p_layout->LinkLayout("TitlePressAnyKey.png");
	p_layout->SetPositionByCentering(320, 360);
	p_layout->SetSize(256, 64);
	p_layout->SetAlpha(0);
	GraphicManager::GetInstance()->EntryDrawLayout(p_layout);

	p_layout = &mLayoutSpatz;
	p_layout->LinkLayout("TitleSpatz.png");
	p_layout->SetPositionByCentering(320, 420);
	p_layout->SetSize(200, 40);
	p_layout->SetAlpha(0);
	GraphicManager::GetInstance()->EntryDrawLayout(p_layout);

	float map_scale = 10.0f;
	mModelMap.Setup(map_scale);
	mModelMap.Position()= ZERO_VEC;
	mModelMap.Scale()	= VGet(map_scale, map_scale, map_scale);
	GraphicManager::GetInstance()->EntryDrawModel(&mModelMap);

	float chara_scale = 10.0f;
	sModelPmd.Setup();
	sModelPmd.Position() = ZERO_VEC;
	sModelPmd.Scale() = VGet(chara_scale, chara_scale, chara_scale);
	sModelPmd.EnableToonShader();
	GraphicManager::GetInstance()->EntryDrawModel(&sModelPmd);
	
	mPhase = SceneEvent::ePHASE_EVENT_BEFORE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneEvent::phaseEventBefore(void)
{
	FADE_IN(1.0f);

	//	PlayMusic("data/ogg/Xenogears.ogg", DX_PLAYTYPE_LOOP);
	PlayMusic("data/ogg/Acoustic05.ogg", DX_PLAYTYPE_LOOP);

	//	sModelPmd.StartMotion(2, true, 0.5f);
	sModelPmd.StartMotion(28, true, 0.5f);

	mPhase = SceneEvent::ePHASE_EVENT;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneEvent::phaseEvent(void)
{
	EventManager::GetInstance()->Update();
}

/**** end of file ****/


