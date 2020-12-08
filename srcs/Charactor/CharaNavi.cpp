/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * NAVI管理クラス
 * @author SPATZ.
 * @data   2012/04/14 19:35:52
 */
//---------------------------------------------------
#include "CharaNavi.h"
#include "AppLib/Basic/Basic.h"
#include "DataDefine/MotionDef.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Collision/CollisionManager.h"

#include "AppLib/Effect/EffectManager.h"

#include "DataDefine/EffectDef.h"

#include "CharaInfo.h"

/*=====================================*
 * static
 *=====================================*/
static int sEffHandle = -1;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaNavi::CharaNavi()
{
	mEnableNaviLight = FALSE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaNavi::~CharaNavi()
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaNavi::Update(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	// カーソルをナビィ代わりに
	int mouse_x, mouse_y;
	VECTOR world, local;
	static float sRate = 0.96f;
	InputManager::GetInstance()->GetMouseNowPoint(&mouse_x, &mouse_y);

	PRINT_SCREEN(COLOR_YELLOW, "mouse = {%d, %d} ", mouse_x, mouse_y);

	local.x = (float)mouse_x;
	local.y = (float)mouse_y;
	local.z = sRate;
	static float sDiff = (19.406f * mScale.x);
	world = ConvScreenPosToWorldPos(local);

	world.y -= sDiff;
	mPosition = world;
	this->SetRotationToTarget(&p_player->Position());

	CharaNpc::Update();

	VECTOR setpos = mPosition;
	setpos.y += sDiff;
	
	VECTOR vec;
	vec = VSub(CameraManager::GetInstance()->Position(), setpos);
	vec = VScale(VNorm(vec), 50.0f);

	EffectManager::GetInstance()->Position(sEffHandle) = VAdd(setpos, vec);

	MV1SetMeshDrawBlendMode(mModelHandle, 5, DX_BLENDMODE_SUB);
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaNavi::EnableNaviLight(bool isEnable)
{

	mEnableNaviLight = (isEnable == true ? TRUE : FALSE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaNavi::EnableNaviCursor(void)
{
	if (GraphicManager::GetInstance()->IsEntryCursorModel() == TRUE) {
		return;
	}

	// マウスカーソルを非表示
	ShowCursor(FALSE);
	PRINT_CONSOLE("Cursor : Hide \n");

	GraphicManager::GetInstance()->EntryCursorModel(this);

	if (sEffHandle == -1) {

		sEffHandle = EffectManager::GetInstance()->Play(eEF_NAVI,
														&mPosition,
														&VScale(mScale, 1.5f),
														0.0f);
	}

	if (mEnableNaviLight == TRUE) {
		float scale = (mScale.x * 1.5f + 200.0f);
		EffectManager::GetInstance()->Scale(sEffHandle) = VGet(scale, scale, scale);
		EffectManager::GetInstance()->SetAlpha(sEffHandle, 0.2f);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaNavi::DisableNaviCursor(void)
{
	if (GraphicManager::GetInstance()->IsEntryCursorModel() == FALSE) {
		return;
	}

	// マウスカーソルを表示
	ShowCursor(TRUE);
	PRINT_CONSOLE("Cursor : Visible \n");

	GraphicManager::GetInstance()->EntryCursorModel(NULL);

	if (sEffHandle != -1) {
		EffectManager::GetInstance()->Stop(sEffHandle, 0.5f);
		sEffHandle = -1;
	}
}

/**** end of file ****/

