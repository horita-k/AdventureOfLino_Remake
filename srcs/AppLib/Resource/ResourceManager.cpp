/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * リソース管理クラス
 * @author SPATZ.
 * @data   2011/11/27 15:30:43
 */
//---------------------------------------------------
#include "ResourceManager.h"

#include "ResourceModel.h"
#include "AppLib/Sound/SoundManager.h"
#include "DataDefine/VoiceDef.h"

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/ResourceManager* const
ResourceManager::GetInstance(void)
{
	static ResourceManager sInstance;
	return &sInstance;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ResourceManager::LoadCharaResource(const char* charaName)
{
	// モデル読み込み
	ResourceModel::GetInstance()->LoadCharaResource(charaName);

	// サウンド読み込み
	SoundManager::GetInstance()->LoadVoiceComponent((char *)charaName, eVC_MAX);
}

 /**** end of file ****/

