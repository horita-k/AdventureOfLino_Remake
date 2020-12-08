/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * エフェクト管理クラス
 * @author SPATZ.
 * @data   2014/01/28 01:01:45	
 */
//---------------------------------------------------
#include "EffectManager.h"
#include "AppLib/Basic/Basic.h"
#include "AppLib/Resource/ResourceLayout.h"

#include "../../DataDefine/EffectDef.h"

/*=====================================*
 * define
 *=====================================*/
/*static*/BYTE EffectManager::sEffectUpdateSpeed = 1;	// エフェクト更新スピード

/*=====================================*
 * public:
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
EffectManager::EffectManager()
{
	mpResource = NULL;
	mResourceNum = 0;
	memset(mEffectList, 0, sizeof(mEffectList));

	mpVertexBuffer		= NULL;
	mpOrgRenderTarget	= NULL;
	mpOrgDepthStencil	= NULL;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
EffectManager::~EffectManager()
{
	Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/EffectManager* const 
EffectManager::GetInstance(void)
{
	static EffectManager sInstance;
	return &sInstance;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
EffectManager::SetEffectUpdateSpeed(BYTE setSpeed)
{
	sEffectUpdateSpeed = setSpeed;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
BOOL
EffectManager::Create(int resorceNum)
{
	if (mResourceNum != 0) {
		return TRUE;
	}

	mResourceNum = resorceNum;

	mpResource = new EffectResource[mResourceNum];
	APP_ASSERT_PRINT((mpResource != NULL),
					 "Failed new EffectResource[%d] \n", mResourceNum);

//	auto d3dDevice = ((LPDIRECT3DDEVICE9)GetUseDirect3DDevice9());
	auto ptr = GetUseDirect3DDevice9();
	auto d3dDevice = ((IDirect3DDevice9 *)ptr);
	if (d3dDevice == NULL) return FALSE;

	d3dDevice->CreateVertexBuffer(4*sizeof(MY_VERTEX),
									 0, 
									 D3DFVF_VERTEX, 
									 D3DPOOL_DEFAULT,
									 &mpVertexBuffer, 
									 NULL);
	APP_ASSERT(mpVertexBuffer != NULL,
			   "Failed CreateVertexBuffer() \n");

	GET_D3DDEV()->GetRenderTarget(0, &mpOrgRenderTarget);
	APP_ASSERT(mpOrgRenderTarget != NULL,
			   "Failed GetRenderTarget() \n");

	GET_D3DDEV()->GetDepthStencilSurface(&mpOrgDepthStencil);
	APP_ASSERT(mpOrgDepthStencil != NULL,
			   "Failed GetDepthStencilSurface() \n");

	return FALSE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectManager::Update(void)
{
	for (DWORD iI = 0; iI < kEFFECT_NODE_NUM; iI ++) {
		if (mEffectList[iI].IsPlay() == true) {
			mEffectList[iI].Update();
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectManager::Destroy(void)
{
	StopAllSystem();

	if (mpResource != NULL) {
		delete[] mpResource;
		mpResource = NULL;
	}

	mResourceNum = 0;

	SAFE_RELEASE(mpVertexBuffer);

	SAFE_RELEASE(mpOrgRenderTarget);
	SAFE_RELEASE(mpOrgDepthStencil);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectManager::LoadResource(int eEffID, const char* effectName, const char* modelName)
{
	APP_ASSERT_PRINT((eEffID < mResourceNum),
					 "eEffID %d is Invalid value \n", eEffID);

	mpResource[eEffID].Create(effectName, modelName);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectManager::UnloadResource(int eEffID)
{
	APP_ASSERT_PRINT((eEffID < mResourceNum),
					 "eEffID %d is Invalid value \n", eEffID);

	mpResource[eEffID].Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectManager::DeleteEffect(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
EffectManager::Play(int eResHandle, VECTOR* pPos, VECTOR* pScale, float fadeSec/*=0.5f*/)
{
	APP_ASSERT_PRINT(eResHandle < mResourceNum,
					 "eResHandle %d is Invalid Value\n", eResHandle);

	APP_ASSERT((pPos != NULL),	"pPos is NULL");
	APP_ASSERT((pScale != NULL),"pScale is NULL");

	int iI = 0;
	for (iI = 0; iI < EffectManager::kEFFECT_NODE_NUM; iI ++) {
		// 使用していないリストを探す
		if (mEffectList[iI].IsPlay() == false) {

			mEffectList[iI].Play(&mpResource[eResHandle], fadeSec);

			// 座標・拡縮を設定
			mEffectList[iI].Position() = *pPos;
			mEffectList[iI].Scale() = * pScale;

			return iI;
		}
	}
	APP_ASSERT_PRINT(NULL,
					 "Faild EffectManager::Play(), no empty buffer\n");
	return -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
EffectManager::Play(int eResHandle, VECTOR* pPos, float allScale, float fadeSec/*=0.5f*/)
{
	return Play(eResHandle, pPos, &VGet(allScale, allScale, allScale), fadeSec);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectManager::Stop(int eEffectHandle, float fadeSec/*=0.5f*/)
{
	APP_ASSERT_PRINT( (unsigned)eEffectHandle < kEFFECT_NODE_NUM,
					 "eEffectHandle %d is Invalid Value\n", eEffectHandle);

	if (mEffectList[eEffectHandle].IsPlay() == true) {
		mEffectList[eEffectHandle].Stop(fadeSec);

		mEffectList[eEffectHandle].SetMatrixType(EffectNode::eMATRIX_TYPE_TRS);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
EffectManager::IsPlay(int eEffectHandle)
{
	APP_ASSERT_PRINT( (unsigned)eEffectHandle < kEFFECT_NODE_NUM,
					 "eEffectHandle %d is Invalid Value\n", eEffectHandle);

	return mEffectList[eEffectHandle].IsPlay();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectManager::StopAll(void)
{
	APP_ASSERT(NULL,
			   "Not Support EffectManager::StopAll() \n");
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectManager::StopAllSystem(void)
{
	for (int iI = 0; iI < kEFFECT_NODE_NUM; iI ++) {
		mEffectList[iI].Stop(0.0f);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
VECTOR&
EffectManager::Position(int eEffectHandle)
{
	APP_ASSERT_PRINT( (unsigned)eEffectHandle < kEFFECT_NODE_NUM,
					 "eEffectHandle %d is Invalid Value\n", eEffectHandle);

	return mEffectList[eEffectHandle].Position();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
VECTOR&
EffectManager::Rotation(int eEffectHandle)
{
	APP_ASSERT_PRINT( (unsigned)eEffectHandle < kEFFECT_NODE_NUM,
					 "eEffectHandle %d is Invalid Value\n", eEffectHandle);

	return mEffectList[eEffectHandle].Rotation();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
VECTOR&
EffectManager::Scale(int eEffectHandle)
{
	APP_ASSERT_PRINT( (unsigned)eEffectHandle < kEFFECT_NODE_NUM,
					 "eEffectHandle %d is Invalid Value\n", eEffectHandle);

	return mEffectList[eEffectHandle].Scale();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
MATRIX&
EffectManager::Matrix(int eEffectHandle)
{
	APP_ASSERT_PRINT( (unsigned)eEffectHandle < kEFFECT_NODE_NUM,
					 "eEffectHandle %d is Invalid Value\n", eEffectHandle);
	
	return mEffectList[eEffectHandle].Matrix();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectManager::SetMatrixType(int eEffectHandle, EffectNode::eMatrixType type)
{
	APP_ASSERT_PRINT( (unsigned)eEffectHandle < kEFFECT_NODE_NUM,
					 "eEffectHandle %d is Invalid Value\n", eEffectHandle);
	
	return mEffectList[eEffectHandle].SetMatrixType(type);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectManager::SetAlpha(int eEffectHandle, float fAlpha)
{
	APP_ASSERT_PRINT( (unsigned)eEffectHandle < kEFFECT_NODE_NUM,
					 "eEffectHandle %d is Invalid Value\n", eEffectHandle);
	
	mEffectList[eEffectHandle].SetAlpha(fAlpha);
}


//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectManager::SetFloat(int eEffectHandle, const char* pName, const float fValue)
{
	APP_ASSERT_PRINT( (unsigned)eEffectHandle < kEFFECT_NODE_NUM,
					 "eEffectHandle %d is Invalid Value\n", eEffectHandle);

	mEffectList[eEffectHandle].SetFloat(pName, fValue);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectManager::SetFloat2(int eEffectHandle, const char* pName, const float* pValue)
{
	APP_ASSERT_PRINT( (unsigned)eEffectHandle < kEFFECT_NODE_NUM,
					  "eEffectHandle %d is Invalid Value\n", eEffectHandle);

	mEffectList[eEffectHandle].SetFloat2(pName, pValue);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectManager::SetVec4(int eEffectHandle, const char* pName, const FLOAT4* pVec4)
{
	APP_ASSERT_PRINT( (unsigned)eEffectHandle < kEFFECT_NODE_NUM,
					 "eEffectHandle %d is Invalid Value\n", eEffectHandle);

	mEffectList[eEffectHandle].SetVec4(pName, pVec4);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectManager::SetVec4(int eEffectHandle, const char* pName, const VECTOR* pVec3)
{
	APP_ASSERT_PRINT( (unsigned)eEffectHandle < kEFFECT_NODE_NUM,
					 "eEffectHandle %d is Invalid Value\n", eEffectHandle);

	FLOAT4 vec4;
	vec4.x = pVec3->x;
	vec4.y = pVec3->y;
	vec4.z = pVec3->z;
	vec4.w = 1.0f;
	
	mEffectList[eEffectHandle].SetVec4(pName, &vec4);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectManager::GetFloat(int eEffectHandle, const char* pName, float* pValue)
{
	APP_ASSERT_PRINT( (unsigned)eEffectHandle < kEFFECT_NODE_NUM,
					 "eEffectHandle %d is Invalid Value\n", eEffectHandle);

	mEffectList[eEffectHandle].GetFloat(pName, pValue);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectManager::GetVec4(int eEffectHandle, const char* pName, FLOAT4* pVec4)
{
	APP_ASSERT_PRINT( (unsigned)eEffectHandle < kEFFECT_NODE_NUM,
					 "eEffectHandle %d is Invalid Value\n", eEffectHandle);

	mEffectList[eEffectHandle].GetVec4(pName, pVec4);
}

/*=====================================*
 * private:
 *=====================================*/

/**** end of file ****/

