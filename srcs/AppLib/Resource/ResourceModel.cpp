/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * リソースモデルクラス
 * @author SPATZ.
 * @data   2011/11/27 15:30:43
 */
//---------------------------------------------------
#include "ResourceModel.h"
#include "dxlib.h"
#include "AppLib/Basic/Basic.h"

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/ResourceModel* const
ResourceModel::GetInstance(void)
{
	static ResourceModel sInstance;
	return &sInstance;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ResourceModel::LoadCharaResource(const char* charaName)
{
	loadResource(charaName, "data/Chara/%s/Model/Model.mv1");
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ResourceModel::LoadItemResource(const char* modelName)
{
	loadResource(modelName, "data/Model/item/%s");
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ResourceModel::LoadGimmickResource(const char* modelName)
{
	loadResource(modelName, "data/Model/item/_GIM/%s");
}
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ResourceModel::LoadModelResource(const char* modelName)
{
	loadResource(modelName, "data/Model/%s");
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ResourceModel::LoadStageResource(const char* stageName, BOOL isExtData)
{
	if (isExtData == FALSE) {
		loadResource(stageName, "data/model/BG/%s");
	}
	else {
		loadResource(stageName, "D:/project/ExternalData/BG/%s");
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ResourceModel::LoadDragMv1Resource(const char* mv1Name)
{
	loadResource(mv1Name, "%s");
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ResourceModel::UnloadResource(const char* resourceName)
{
	unloadResource(resourceName);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ResourceModel::loadResource(const char* resourceName, const char* format)
{
	// 多重読み込み禁止
	if (CheckLoadComponent(resourceName) == true) {
		return;
	}

	char model_root_dir[128] = {0};
	int model_handle = -1;

	// モデルの読み込み
	char model_name[128] = {0};
	sprintf_s(model_root_dir, sizeof(model_root_dir), format, resourceName);

	// モデル読み込み
	model_handle = MV1LoadModel(model_root_dir);
	APP_ASSERT_PRINT(model_handle != -1,
					 "Failed MV1LoadModel : %s\n", model_root_dir);
	PRINT_LOG_PRINT("%s : %d", model_root_dir, model_handle);

	// 名前用ワークを生成
	char* p_name = NULL;
	size_t size = strlen(resourceName);
	p_name = new char[(size + 1)];
	memset(p_name, 0x00, (size + 1));
	strcpy_s(p_name, (size + 1), resourceName);
	p_name[size] = '\0';

	tComponentWork work;
	work.mName	= p_name;
	work.mHandle= model_handle;

	mComponentList.push_back(work);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ResourceModel::unloadResource(const char* resourceName)
{
	std::vector<tComponentWork>::iterator ite;
	for (ite = mComponentList.begin(); ite != mComponentList.end(); ite ++) {
		if (strcmp((*ite).mName, resourceName) == 0) {
			if ( (*ite).mHandle != -1) {

				delete[] (*ite).mName;
				(*ite).mName = NULL;

				MV1DeleteModel( (*ite).mHandle );
				(*ite).mHandle = -1;

				mComponentList.erase(ite);
				break;
			}
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ResourceModel::CheckLoadComponent(const char* cmpName)
{
	for (unsigned int iI = 0; iI < mComponentList.size(); iI ++) {
		if (strcmp(mComponentList.at(iI).mName, cmpName) == 0) {
			return true;
		}
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
ResourceModel::GetCloneHandle(const char* charaName)
{
	for (unsigned int iI = 0; iI < mComponentList.size(); iI ++) {
		if (strcmp(mComponentList.at(iI).mName, charaName) == 0) {
			return MV1DuplicateModel(mComponentList.at(iI).mHandle);
		}
	}
	// 指定のモデルハンドルが見つからなかった
	return -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
char*
ResourceModel::GetResourceName(const char* searchWord, int startIndex)
{
	for (unsigned int iI = startIndex; iI < mComponentList.size(); iI ++) {
		if (strchr(mComponentList.at(iI).mName, (int)searchWord) != NULL) {
			return mComponentList.at(iI).mName;
		}
	}
	return NULL;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ResourceModel::ResourceModel()
{
	mComponentList.clear();
}

//---------------------------------------------------
/**
 * デストラクタ
 */
//---------------------------------------------------
ResourceModel::~ResourceModel()
{
	Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ResourceModel::Destroy(void)
{
	int ret;

	for (unsigned int iI = 0; iI < mComponentList.size(); iI ++) {

		delete[] mComponentList.at(iI).mName;
		mComponentList.at(iI).mName = NULL;

		ret = MV1DeleteModel(mComponentList.at(iI).mHandle);
		mComponentList.at(iI).mHandle = -1;
	}

	mComponentList.clear();
}

 /**** end of file ****/

