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
#include "StageFunc046.h"
#include "AppLib/Effect/EffectManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/RenderBloom.h"
#include "AppLib/Resource/ResourceManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "AppLib/Resource/ResourceLayout.h"
#include "DataDefine/EffectDef.h"
#include "Gimmick/GimmickBase.h"
#include "Gimmick/GimmickExtend.h"
#include "../CommandFunctions.h"
#include "../ExternDefine.h"		//<! Extern 宣言は全てここに集約する

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
StageFunc046::StageFunc046()
{
	mWaterType = eWATER_TYPE_UP;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
StageFunc046::~StageFunc046()
{
	destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc046::SetupBeforeLoad(void)
{
	if (IS_END_EVT(eEVENT_35, eEVENT_35_END_BETA_GAME) ) {
		//<! NPCのセリフを変更
		CharaNpc::SetCsvSubNo(1);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc046::SetupAfterLoad(int& rPhase)
{
	int model_handle = sModelMap.GetModelHandle();

	if (TotalInfo::GetInstance()->GetEventFlagEnable(eEVENT_46, eEVENT_46_END_ENEMY_BATTLE) == true) {
		sModelMap.SetVisible(false, eBG_TYPE_GIMMICK + 0);
	} else {
		CollisionManager::GetInstance()->EntryWallCollision(model_handle,	(eBG_TYPE_GIMMICK + 0));
	}

	// 該当のフォグ設定
	GraphicManager::GetInstance()->EnableFogPattern(192,192,192,10.0f,30000.0f);

	if (IS_END_EVT(eEVENT_35, eEVENT_35_END_BETA_GAME) ) {
		// 飛空艇非表示
		GimmickBase* p_gimmick = GimmickBase::Search1stFindGimmickKind(eGIMMICK_KIND_AIRSHIP);
		APP_ASSERT((p_gimmick != NULL), "Not found gimmick \n");
		p_gimmick->SetVisible(false);
		p_gimmick->Scale() = ZERO_VEC;
	}

	//	sModelXfile[eMD_SKYDOME	].Scale() = ZERO_VEC;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc046::Update(int counter)
{
	/*=====================================*
	 * コンパスの表示
	 *=====================================*/
	TotalInfo::GetInstance()->UpdateCompass();

	float kWATER_LOWER_HEIGHT = -150.0f;

	switch (mWaterType) {
	case eWATER_TYPE_DOWN_GOING:
		//		sModelMap.WaterOffsetPos() += -0.04f;
		sModelMap.WaterOffsetPos() += -0.08f;
		if (sModelMap.WaterOffsetPos() < kWATER_LOWER_HEIGHT) {
			sModelMap.WaterOffsetPos() = kWATER_LOWER_HEIGHT;
			mWaterType = eWATER_TYPE_DOWN;
		}
		break;

	case eWATER_TYPE_UP_GOING:
		//		sModelMap.WaterOffsetPos() += 0.04f;
		sModelMap.WaterOffsetPos() += 0.08f;
		if (sModelMap.WaterOffsetPos() > 0.0f) {
			sModelMap.WaterOffsetPos() = 0.0f;
			mWaterType = eWATER_TYPE_UP;
		}
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
StageFunc046::SetPatternLight(int pattern)
{

	switch (pattern) {
	case 0:
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
StageFunc046::SetPatternObject(int pattern)
{
	switch (pattern) {
	case 0:
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
StageFunc046::SetWaterType(StageFunc046::eWaterType type)
{
	mWaterType = type;
}


/*=====================================*
 * private
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc046::destroy(void)
{
	return;
}

/**** end of file ****/


