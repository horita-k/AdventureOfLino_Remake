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

#include "EffectTest.h"

#include "AppLib/Basic/Basic.h"
#include "AppLib/Basic/Counter.h"
#include "AppLib/Input/InputManager.h"

#include "Scene/ActionTaskManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/ModelPmd.h"
#include "AppLib/Graphic/ModelMap.h"
#include "AppLib/Effect/EffectManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Collision/CollisionManager.h"
#include "DataDefine/VoiceDef.h"
#include "DataDefine/MessageDef.h"
#include "DataDefine/MotionDef.h"
#include "DataDefine/BGDef.h"
#include "DataDefine/EffectDef.h"
#include "Charactor/CharaInfo.h"

#include "Charactor/CharaPlayer.h"
#include "Charactor/CharaEnemy.h"
#include "Charactor/PersonalChara/CharaEnemyGiginebura.h"
#include "Charactor/PersonalChara/CharaEnemyStrategist.h"
#include "Charactor/CharaNPC.h"
#include "AppLib/Resource/ResourceManager.h"
#include "AppLib/Resource/ResourceLayout.h"



#include "dxlib.h"
#include "Utility/Utility.h"

/*=====================================*
 * 各オブジェクトの定義
 *=====================================*/
static CharaPlayer		sPlayer;
static ModelMap			sModelMap;
static ModelBase		sModelSword;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
EffectTest::EffectTest()
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
EffectTest::~EffectTest()
{
	destroy();
}

//---------------------------------------------------
/**
   作成
 */
//---------------------------------------------------
void
EffectTest::Create(int debugArg)
{
	SceneBase::Create(debugArg);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectTest::Update(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
EffectTest::GetLoadingPercent(void)
{
	return -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectTest::destroy(void)
{
	// コリジョンの破棄
	CollisionManager::GetInstance()->Destroy();

	// モデルを描画から外す
	GraphicManager::GetInstance()->RemoveDrawModelAll();

	sPlayer.DeleteModel();

	sModelMap.DeleteModel();

	sModelSword.DeleteModel();

	// エフェクトの破棄
	EffectManager::GetInstance()->Destroy();
}

/**** end of file ****/


