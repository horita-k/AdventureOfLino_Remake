/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * リソース画像クラス
 * @author SPATZ.
 * @data   2012/02/18 17:17:20
 */
//---------------------------------------------------
#include "ResourceLayout.h"
#include "dxlib.h"
#include "AppLib/Basic/Basic.h"

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/ResourceLayout* const
ResourceLayout::GetInstance(void)
{
	static ResourceLayout sInstance;
	return &sInstance;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ResourceLayout::LoadLayoutResource(const char* layoutName)
{
	// 多重読み込み禁止
	if (CheckLoadComponent(layoutName) == true) {
		return;
	}

	int layout_handle = -1;

	// モデルの読み込み
	char layout_name[128] = {0};
	sprintf_s(layout_name, sizeof(layout_name), "data/Texture/%s", layoutName);

	// モデル読み込み
	layout_handle = LoadGraph(layout_name);
	APP_ASSERT_PRINT(layout_handle != -1,
					 "Failed MV1LoadLayout : %s\n", layout_name);

	// 名前用ワークを生成
	char* p_name = NULL;
	size_t size = strlen(layoutName);
	p_name = new char[(size + 1)];
	memset(p_name, 0x00, (size + 1));
	strcpy_s(p_name, (size + 1), layoutName);
	p_name[size] = '\0';

	tComponentWork work;
	work.mName	= p_name;
	work.mHandle= layout_handle;

	mComponentList.push_back(work);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ResourceLayout::CheckLoadComponent(const char* cmpName)
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
ResourceLayout::GetHandle(const char* charaName)
{
	for (unsigned int iI = 0; iI < mComponentList.size(); iI ++) {
		if (strcmp(mComponentList.at(iI).mName, charaName) == 0) {
			return mComponentList.at(iI).mHandle;
		}
	}
	// 指定のモデルハンドルが見つからなかった
	return -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ResourceLayout::UnloadResource(char* resourceName)
{
	std::vector<tComponentWork>::iterator ite;
	for (ite = mComponentList.begin(); ite != mComponentList.end(); ite ++) {
		if (strcmp((*ite).mName, resourceName) == 0) {
			if ((*ite).mHandle != -1) {

				delete[] (*ite).mName;
				(*ite).mName = NULL;

				
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
ResourceLayout::ResourceLayout()
{
	mComponentList.clear();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ResourceLayout::~ResourceLayout()
{
	int ret;

	for (unsigned int iI = 0; iI < mComponentList.size(); iI ++) {

		delete[] mComponentList.at(iI).mName;
		mComponentList.at(iI).mName = NULL;

		ret = DeleteGraph(mComponentList.at(iI).mHandle);
		mComponentList.at(iI).mHandle = -1;
	}
}

 /**** end of file ****/

