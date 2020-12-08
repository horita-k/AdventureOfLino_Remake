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
#include "StageFunc041.h"
#include "AppLib/Effect/EffectManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/RenderBloom.h"
#include "AppLib/Resource/ResourceManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "AppLib/Resource/ResourceLayout.h"
#include "DataDefine/EffectDef.h"
#include "DataDefine/VoiceDef.h"
#include "Gimmick/GimmickBase.h"
#include "Gimmick/GimmickExtend.h"
#include "../CommandFunctions.h"
#include "../ExternDefine.h"		//<! Extern 宣言は全てここに集約する

#include "Utility/Utility.h"

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
StageFunc041::StageFunc041()
{
	mpGuardian = NULL;

	mEfkLightPillar = -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
StageFunc041::~StageFunc041()
{
	destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc041::SetupBeforeLoad(void)
{
	/**** ベースクラス関数を自クラス処理後にコール ****/
	StageFuncBase::SetupBeforeLoad();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc041::SetupAfterLoad(int& rPhase)
{
	int model_handle = sModelMap.GetModelHandle();

	/**** ベースクラス関数を自クラス処理前にコール ****/
	StageFuncBase::SetupAfterLoad(rPhase);

	mLightCounter			= 0.0f;
	mIsOnAllSwitch			= FALSE;

	// ステージ41のパターン0のライトをセット
	SetPatternLight(0);

	CollisionManager::GetInstance()->EntryEarthCollision(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_2F);
	CollisionManager::GetInstance()->EntryEarthCollision(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_3F);
	CollisionManager::GetInstance()->EntryEarthCollision(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_4F);
	CollisionManager::GetInstance()->EntryEarthCollision(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_5F);

	// 錠前イベント
	if (IS_END_EVT(eEVENT_41, eEVENT_41_BIG_KEY)) {
		sModelMap.SetVisible(false, StageFunc041::eBG_TYPE_GIMMICK_BIG_KEY);
	}
	else {
		sModelMap.SetVisible(true, StageFunc041::eBG_TYPE_GIMMICK_BIG_KEY);
		CollisionManager::GetInstance()->EntryWallCollision(model_handle, StageFunc041::eBG_TYPE_GIMMICK_BIG_KEY);
	}

	//	kGRAVITY_SPEED = -0.98f * 3.0f;

	sModelXfile[eMD_SKYDOME].Scale() = ZERO_VEC;

	// ライトの位置を初期化
	float rot;
	getPlayerPosRotFromBGModel(&mLightPillarPos, &rot, 9);
	mLightPillarPos.y -= 70.0f;

	//	mEfkLightPillar = EffekseerManager::GetInstance()->Play(eEFK_LIGHT_PILLAR, &mLightPillarPos, 100.0f, 0.0f);
	mEfkLightPillar = EffekseerManager::GetInstance()->Play(eEFK_LIGHT_PILLAR, &mLightPillarPos, 1000.0f, 0.0f);
	EffekseerManager::GetInstance()->SetSpeed(mEfkLightPillar, 0.3f);


	//<! ついでにファークリップも変更する
	CameraManager::GetInstance()->SetFarClip(225000.0f);

	this->RevertGuardian();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc041::Update(int counter)
{
	// コンパスの表示
	TotalInfo::GetInstance()->UpdateCompass();

	static const BYTE kBABEL_SWITCH_NUM = 3;

	BYTE babel_switch_index[kBABEL_SWITCH_NUM] = {-1, -1, -1};
	BYTE switch_index = 0;

	/* バベルスイッチが全て押されているかのチェック */
	GimmickBase* p_gimmick = NULL;
	for (int iI = 0; iI < GimmickBase::GetGimmickBaseNum(); iI ++) {
		p_gimmick = GimmickBase::GetGimmickBase(iI);
		if (p_gimmick->GetKind() == eGIMMICK_KIND_BABEL_SWITCH) {

			APP_ASSERT_PRINT((switch_index < kBABEL_SWITCH_NUM),
							 "Invalid Index switch_index : %d \n", switch_index);
			babel_switch_index[switch_index] = iI;
			switch_index ++;
		}	
	}

	COLOR_F color_default, color_bright;
	int model_handle = sModelMap.GetModelHandle();
	int mat_index = -1;

	color_default = GetColorF(0.6f, 0.6f, 0.6f, 1.0f);
	// 発光のカラー値を算出
	color_bright = GetColorF((sin(mLightCounter * 0.9f) * 0.15f) + 0.62f,
							 (sin(mLightCounter * 0.9f) * 0.15f) + 0.62f,
							 (sin(mLightCounter * 0.9f) * 0.15f) + 1.02f,
							 1.0f);
	
	BYTE on_switch_num = 0;
	
	/*
	// ケーブルの発光
	for (int iI = 0; iI < kBABEL_SWITCH_NUM; iI ++) {
		p_gimmick = GimmickBase::GetGimmickBase(babel_switch_index[iI]);
		mat_index = (5 + iI);
		if (p_gimmick->GetState() == GimmickExtend_BabelSwitch::eSTATE_ON_MODEL_SWITCH) {
			color = color_bright;
			MV1SetMaterialDifColor(model_handle, mat_index, color);
			MV1SetMaterialSpcColor(model_handle, mat_index, color);
			MV1SetMaterialAmbColor(model_handle, mat_index, color);
			MV1SetMaterialEmiColor(model_handle, mat_index, color);

			on_switch_num ++;
		}
		else {
			color = GetColorF(0.49f, 0.49f, 0.49f, 1.0f);
			MV1SetMaterialDifColor(model_handle, mat_index, color);
			color = GetColorF(0.00f, 0.00f, 0.00f, 1.0f);
			MV1SetMaterialSpcColor(model_handle, mat_index, color);
			color = GetColorF(0.10f, 0.10f, 0.10f, 1.0f);
			MV1SetMaterialAmbColor(model_handle, mat_index, color);
			color = GetColorF(0.196f, 0.196f, 0.196f, 1.0f);
			MV1SetMaterialEmiColor(model_handle, mat_index, color);
		}
	}
	*/
	
	DWORD field_bit = TotalInfo::GetInstance()->GetFieldInfoFlagBit(41);
	GimmickExtend_Tower* p_tower = NULL;

	p_tower = (GimmickExtend_Tower *)GimmickBase::Search1stFindGimmickKind(eGIMMICK_KIND_TOWER);
	APP_ASSERT((p_tower != NULL),
			   "Failed Search1stFindGimmickKind(eGIMMICK_KIND_TOWER) \n");

	// 全てのスイッチを押下している場合の処理
	if (on_switch_num == kBABEL_SWITCH_NUM) {


		/*
		if (mIsOnAllSwitch == FALSE) {
			// ステージ41のパターン1のライトをセット
			SetPatternLight(1);
			SoundManager::GetSndComponent()->PlaySound(eSE_MACHINE_BOOTUP);
			SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);

			// タワーを有効にする
			p_tower->SetEnableActive(TRUE);
			mIsOnAllSwitch = TRUE;
		}
		
		// タワー自体を発行させる
		p_tower->SetColor(color_bright);

		// 機材みたいな奴の発光
		for (int iI = 0; iI < 2; iI ++) {
			MV1SetMaterialDifColor(model_handle, iI, color_bright);
			MV1SetMaterialSpcColor(model_handle, iI, color_bright);
			MV1SetMaterialAmbColor(model_handle, iI, color_bright);
			MV1SetMaterialEmiColor(model_handle, iI, color_bright);
		}
		*/


	}
	else {
		if (mIsOnAllSwitch == TRUE) {
			// ステージ41のパターン0のライトをセット
			SetPatternLight(0);
			SoundManager::GetSndComponent()->PlaySound(eSE_CANCEL);

			p_tower->SetEnableActive(FALSE);
			// タワー自体を発行させる
			p_tower->SetColor(color_default);

			mIsOnAllSwitch = FALSE;
		}
	}

	// 電球みたいな奴の発光
	/*
	color_bright = GetColorF((sin(mLightCounter * 1.8f) * 0.15f) + 2.02f,
							 (sin(mLightCounter * 1.8f) * 0.15f) + 1.62f,
							 (sin(mLightCounter * 1.8f) * 0.15f) + 1.62f,
							 1.0f);
	*/

	/*
	color_bright = GetColorF((sin(mLightCounter * 1.8f) * 0.05f) + 0.22f,
							 (sin(mLightCounter * 1.8f) * 0.05f) + 0.162f,
							 (sin(mLightCounter * 1.8f) * 0.05f) + 0.162f,
							 1.0f);


	for (int iI = 2; iI < 5; iI ++) {
		MV1SetMaterialDifColor(model_handle, iI, color_bright);
		MV1SetMaterialSpcColor(model_handle, iI, color_bright);
		MV1SetMaterialAmbColor(model_handle, iI, color_bright);
		MV1SetMaterialEmiColor(model_handle, iI, color_bright);
	}
	*/



	/*
	static float sRot = 0.0f;
	sRot += 0.00001f;
	MV1SetFrameTextureAddressTransform(sModelMap.GetModelHandle(), eBG_TYPE_ALPHA_WALL,
									   0, 0, 1, 1,
									   50, 50,
									   sRot);
	*/

	mLightCounter += 0.1f;

	//	kGRAVITY_SPEED = -0.98f * 2.0f;

	// 光の柱
	if ((mEfkLightPillar == -1) ||
		(EffekseerManager::GetInstance()->IsPlay(mEfkLightPillar) == false) ) {
		mEfkLightPillar = EffekseerManager::GetInstance()->Play(eEFK_LIGHT_PILLAR, &mLightPillarPos, 1000.0f, 0.0f);
		EffekseerManager::GetInstance()->SetSpeed(mEfkLightPillar, 0.3f);
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc041::SetPatternLight(int pattern)
{
	return;


	RenderBloom::tBloomBrightParam param ={0};

	switch(pattern) {
		// 非ライトアップ時のライト
	case 0:
		GraphicManager::GetInstance()->SetEnableDirectionLight(TRUE);
		/*
		SetLightDifColor(GetColorF(0.090200f, 0.121570f, 0.113730f, 0.000000f) );
		SetLightSpcColor(GetColorF(0.098040f, 0.098040f, 0.098040f, 0.000000f) );
		SetLightAmbColor(GetColorF(0.203920f, 0.219610f, 0.196080f, 0.000000f) );
		*/
		SetLightDifColor(GetColorF(0.239220f, 0.392160f, 0.380390f, 0.000000f) );
		SetLightSpcColor(GetColorF(0.098040f, 0.098040f, 0.098040f, 0.000000f) );
		SetLightAmbColor(GetColorF(0.376470f, 0.517650f, 0.505880f, 0.000000f) );
		GraphicManager::GetInstance()->SetEnablePLight(TRUE);
		GraphicManager::GetInstance()->SetPLightPosition(&VGet(0.000f, 8300.000f, 0.000f) );
		GraphicManager::GetInstance()->SetPLightRangeAtten(30800.000f, (float)0.1, (float)1E-05, (float)4E-10);
		GraphicManager::GetInstance()->SetPLightDiffuse(0.301960f, 0.498040f, 0.498040f, 0.000000f);
		GraphicManager::GetInstance()->SetPLightSpecular(0.254900f, 0.333330f, 0.329410f, 0.000000f);
		GraphicManager::GetInstance()->SetPLightAmbient(0.231370f, 0.180390f, 0.000000f, 0.000000f);
		
		RenderBloom::SetBloomType(RenderBloom::eBLOOM_TYPE_BRIGHT_LESS);
		RenderBloom::SetBloomParam(4, 255, 100);
		param.mClipParam = 30;
		param.mPixelColorR = 0;	param.mPixelColorG = 0;	param.mPixelColorB = 0;
		RenderBloom::BloomBrightParam() = param;

		break;

		// ライトアップ時のライト
	case 1:
		GraphicManager::GetInstance()->SetEnablePLight(TRUE);
		GraphicManager::GetInstance()->SetPLightPosition(&VGet(0.000f, 8300.000f, 0.000f) );
		GraphicManager::GetInstance()->SetPLightRangeAtten(30800.000f, (float)0.1, (float)1E-05, (float)4E-10);
		GraphicManager::GetInstance()->SetPLightDiffuse(0.360780f, 0.580390f, 0.533330f, 0.000000f);
		GraphicManager::GetInstance()->SetPLightSpecular(0.003920f, 0.074510f, 0.074510f, 0.000000f);
		GraphicManager::GetInstance()->SetPLightAmbient(0.090200f, 0.196080f, 0.149020f, 0.000000f);
		
		RenderBloom::SetBloomType(RenderBloom::eBLOOM_TYPE_BRIGHT_LESS);
		//	RenderBloom::SetBloomParam(12, 55, 200);
		RenderBloom::SetBloomParam(12, 45, 200);
		//	param.mClipParam = 210;
		param.mClipParam = 170;
		RenderBloom::BloomBrightParam() = param;
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
StageFunc041::ProcessShowMapBefore(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	INT8 coll_index = 0;
	int  on_frame_index = 0;
	coll_index = p_player->GetOnCollisionIndex();

	on_frame_index = CollisionManager::GetInstance()->GetEarthModelFrameIndex(coll_index);

	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_EARTH, TRUE);
	sModelMap.SetAlpha(eBG_TYPE_EARTH, 0.1f);
	
	MV1SetFrameVisible(sModelMap.GetModelHandle(), StageFunc041::eBG_TYPE_GIMMICK_2F, FALSE);
	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_3F, FALSE);
	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_4F, FALSE);
	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_5F, FALSE);

	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_VISIBLE, FALSE);

	switch (on_frame_index) {
	case eBG_TYPE_GIMMICK_5F:
		MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_5F, TRUE);
		break;
	case eBG_TYPE_GIMMICK_4F:
		MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_4F, TRUE);
		break;
	case eBG_TYPE_GIMMICK_3F:
	case eBG_TYPE_ALPHA_EARTH:
		MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_3F, TRUE);
		break;
	case eBG_TYPE_GIMMICK_2F:
		MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_2F, TRUE);
		break;
	default:
		sModelMap.SetAlpha(eBG_TYPE_EARTH, 1.0f);
		break;
	};
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc041::ProcessShowMapAfter(void)
{
	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_EARTH, TRUE);
	sModelMap.SetAlpha(eBG_TYPE_EARTH, 1.0f);

	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_2F, TRUE);
	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_3F, TRUE);
	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_4F, TRUE);
	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_5F, TRUE);

	MV1SetFrameVisible(sModelMap.GetModelHandle(), eBG_TYPE_VISIBLE, TRUE);

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc041::RevertGuardian(void)
{
	static const BYTE sGuardianPlaceTable[kGUARDIAN_NUM] = {2, 5, 8};

	if (mpGuardian != NULL) {
		for (int iI = 0; iI < kGUARDIAN_NUM; iI ++) {
			mpGuardian[iI].Destroy();
			mpGuardian[iI].DeleteModel();
		}
		delete[] mpGuardian;
		mpGuardian = NULL;

	}

	mpGuardian = new CharaGuardian[kGUARDIAN_NUM];
	APP_ASSERT((mpGuardian != NULL), "Failed new CharaGuardian \n");

	VECTOR pos; float rot;
	for (int iI = 0; iI < kGUARDIAN_NUM; iI ++) {

		mpGuardian[iI].LinkCharaResource(NAME_Guardian);
		getPlayerPosRotFromBGModel(&pos, &rot, sGuardianPlaceTable[iI]);
		mpGuardian[iI].Position()	= pos;
		mpGuardian[iI].Rotation().y	= rot;
		mpGuardian[iI].Scale() = GUARDIAN_DEF_SCALE;
		mpGuardian[iI].Setup();
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc041::ReviseAllModel(VECTOR& rMoveVec)
{
	if (mpGuardian != NULL) {
		for (int iI = 0; iI < kGUARDIAN_NUM; iI ++) {
			mpGuardian[iI].Position() = VSub(mpGuardian[iI].Position(), rMoveVec);
		}
	}


	if (mEfkLightPillar != -1) {
		/*
		float rot;
		getPlayerPosRotFromBGModel(&mLightPillarPos, &rot, 8);
		mLightPillarPos.y -= 70.0f;
		*/
		mLightPillarPos = VSub(mLightPillarPos, rMoveVec);
		EffekseerManager::GetInstance()->SetPosition(mEfkLightPillar, &mLightPillarPos);
	}

}

/*=====================================*
 * private
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc041::destroy(void)
{
	if (mpGuardian != NULL) {
		for (int iI = 0; iI < kGUARDIAN_NUM; iI ++) {
			mpGuardian[iI].Destroy();
			mpGuardian[iI].DeleteModel();
		}
		delete[] mpGuardian;
		mpGuardian = NULL;
	}

	STOP_EFFEKSEER(mEfkLightPillar, 0.0f);
}

/**** end of file ****/


