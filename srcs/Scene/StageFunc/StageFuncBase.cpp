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

#include "StageFuncDef.h"

#include "AppLib/Effect/EffectManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Graphic/CameraManager.h"

#include "AppLib/Graphic/GraphicManager.h"

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
// 前回のステージ番号リスト
static std::vector<int>		sPrevStageIndexList;

/*=====================================*
 * public
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
StageFuncBase::StageFuncBase()
{
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
StageFuncBase::~StageFuncBase()
{
	destroy();
}

//---------------------------------------------------
/**
   
 */
//---------------------------------------------------
void
StageFuncBase::SetupBeforeLoad(void)
{
	/* 前回ロードしたステージとの差分をチェックする */
	bool is_exist = false;
	for (unsigned int iI = 0; iI < sPrevStageIndexList.size(); iI ++) {
		is_exist = false;
		if (sPrevStageIndexList.at(iI) == spBGInfo->mFileIndex) {
			is_exist = true;
		}
		else {
			for (unsigned int iJ = 0; iJ < sSubStageInfoList.size(); iJ ++) {
				if (sPrevStageIndexList.at(iI) == sSubStageInfoList.at(iJ).mFileIndex ) {
					is_exist = true;
					break;
				}
			}
		}

		// 前回と今回で重複するデータが無い場合、このタイミングでステージモデルを破棄する
		if (is_exist == false) {
			PRINT_CONSOLE("diff stage model : delete %d stage index \n", sPrevStageIndexList.at(iI));
			ResourceModel::GetInstance()->UnloadResource((char *)sResourceStageNameTable[sPrevStageIndexList.at(iI)]);
		}
	}
	sPrevStageIndexList.clear();

	// オブジェクトの出現位置を設定させる
	sDissappearLength	= 12000.0f;

	kGRAVITY_SPEED		= -0.98f;
}

//---------------------------------------------------
/**
   
 */
//---------------------------------------------------
void
StageFuncBase::SetupAfterLoad(int& rPhase)
{
	// とりあえず、フォグを無効にする
	GraphicManager::GetInstance()->DisableFogPattern();
	// ライトもデフォルトに戻す
	GraphicManager::GetInstance()->SetEnableDirectionLight(true);
	GraphicManager::GetInstance()->DisablePLightAll();
	GraphicManager::GetInstance()->SetEnableOriginalShader(false);
	GraphicManager::GetInstance()->SetDefaultDirectionLight();

	// ブルームのデフォルト設定を行う
	setDefaultBloomSetting();

	/*
	// ステージバッファが既に確保されいる場合、多重メモリ確保を防ぐため一度破棄する
	if (spStageWorkBuf != NULL) {
		delete spStageWorkBuf;
		spStageWorkBuf = NULL;
	}
	*/
}

/*=====================================*
 * static
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
StageFuncBase::UpdateLightInfo(void)
{
	switch (sStageIndex) {
	case 16:
		break;

	default:
		break;
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
StageFuncBase::destroy(void)
{
	/* 前回のステージリストを挿入する */
	sPrevStageIndexList.clear();
	sPrevStageIndexList.push_back(spBGInfo->mFileIndex);
	for (unsigned int iI = 0; iI < sSubStageInfoList.size(); iI ++) {
		sPrevStageIndexList.push_back( sSubStageInfoList.at(iI).mFileIndex );
	}
}

//---------------------------------------------------
/**
 * Stagefunc のインスタンスを生成
 */
//---------------------------------------------------
/*static*/StageFuncBase*
StageFuncBase::CreateInstance(int stageIndex)
{
	StageFuncBase* p_instance = NULL;

	switch (stageIndex) {
	case 16:	p_instance = new StageFunc016();	break;
	case 28:	p_instance = new StageFunc028();	break;
	case 29:	p_instance = new StageFunc029();	break;
	case 36:	p_instance = new StageFunc036();	break;
	case 40:	p_instance = new StageFunc040();	break;
	case 41:	p_instance = new StageFunc041();	break;
	case 42:	p_instance = new StageFunc042();	break;
	case 43:	p_instance = new StageFunc043();	break;
	case 46:	p_instance = new StageFunc046();	break;
	case 47:	p_instance = new StageFunc047();	break;
	case 48:	p_instance = new StageFunc048();	break;
	case 52:	p_instance = new StageFunc052();	break;
	default:	p_instance = new StageFuncCommon();	break;
	};
	
	return p_instance;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFuncBase::setVisibleEarthGimmickCollision(int gimmickFrame, BOOL isVisible)
{
	int model_handle = sModelMap.GetModelHandle();

	if (isVisible) {
		CollisionManager::GetInstance()->EntryEarthCollision(model_handle, gimmickFrame);
		MV1SetFrameVisible(model_handle, gimmickFrame, TRUE);
	}
	else {
		CollisionManager::GetInstance()->RemoveEarthCollision(model_handle, gimmickFrame);
		MV1SetFrameVisible(model_handle, gimmickFrame, FALSE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFuncBase::setVisibleWallGimmickCollision(int gimmickFrame, BOOL isVisible)
{
	int model_handle = sModelMap.GetModelHandle();

	if (isVisible) {
		CollisionManager::GetInstance()->EntryWallCollision(model_handle, gimmickFrame);
		MV1SetFrameVisible(model_handle, gimmickFrame, TRUE);
	}
	else {
		CollisionManager::GetInstance()->RemoveWallCollision(model_handle, gimmickFrame);
		MV1SetFrameVisible(model_handle, gimmickFrame, FALSE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFuncBase::setVisibleBarrierFromByEventFlag(int gimmickFrame, int eventA, int eventB, BOOL isScroll/*=TRUE*/)
{
	int model_handle = sModelMap.GetModelHandle();

	// フラグが立っていない場合はバリアを表示
	if (IS_END_EVT(eventA, eventB) == false) {
		CollisionManager::GetInstance()->EntryWallCollision(model_handle, gimmickFrame);
		if (isScroll == TRUE) {
			sModelMap.EntryBarrier(gimmickFrame);
		}
		MV1SetFrameVisible(model_handle, gimmickFrame, TRUE);
	}
	else {
		CollisionManager::GetInstance()->RemoveWallCollision(model_handle, gimmickFrame);
		MV1SetFrameVisible(model_handle, gimmickFrame, FALSE);
	}
}

/**** end of file ****/


