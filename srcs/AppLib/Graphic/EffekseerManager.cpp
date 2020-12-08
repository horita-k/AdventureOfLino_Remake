/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * Effekseer機能管理クラス
 * @author SPATZ.
 * @data   2013/06/15 17:11:30
 */
//---------------------------------------------------
//#include "AppLib/Graphic/CameraManager.h"
//#include "AppLib/Resource/ResourceLayout.h"
#include "EffekseerManager.h"
#include "AppLib/Basic/Basic.h"

#include "EffekseerForDXLib.h"

/*=====================================*

 *=====================================*/
#define EFFEKSER_SPRITE_MAX	(1000)

#define CONVERT_MTX_44_TO_43(p_out43, p_in44)			\
	{													\
		p_out43->Value[0][0] = p_in44->m[0][0];			\
		p_out43->Value[0][1] = p_in44->m[0][1];			\
		p_out43->Value[0][2] = p_in44->m[0][2];			\
		p_out43->Value[1][0] = p_in44->m[1][0];			\
		p_out43->Value[1][1] = p_in44->m[1][1];			\
		p_out43->Value[1][2] = p_in44->m[1][2];			\
		p_out43->Value[2][0] = p_in44->m[2][0];			\
		p_out43->Value[2][1] = p_in44->m[2][1];			\
		p_out43->Value[2][2] = p_in44->m[2][2];			\
		p_out43->Value[2][3] = p_in44->m[2][3];			\
		p_out43->Value[3][0] = p_in44->m[3][0];			\
		p_out43->Value[3][1] = p_in44->m[3][1];			\
		p_out43->Value[3][2] = p_in44->m[3][2];			\
	}

//---------------------------------------------------
/**
   
 */
//---------------------------------------------------
/*static*/EffekseerManager* const 
EffekseerManager::GetInstance(void)
{
	static EffekseerManager sInstance;
	return &sInstance;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
EffekseerManager::EffekseerManager()
{
	mScreenRate			= 0.0f;
	mpEfkResHandleTable = NULL;
	mEfkResBufferNum	= 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
EffekseerManager::~EffekseerManager()
{
	Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffekseerManager::Create(void)
{
	Effkseer_Init(2000);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffekseerManager::Setup(void)
{
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffekseerManager::Destroy(void)
{
	SAFE_DELETE_ARRAY(mpEfkResHandleTable);

	// Effekseerを終了する。
	Effkseer_End();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
BOOL
EffekseerManager::SetupResourceBufferNum(BYTE bufNum)
{
	if (mEfkResBufferNum != 0) {
		return TRUE;
	}

	mEfkResBufferNum = bufNum;
	mpEfkResHandleTable = new int[mEfkResBufferNum];
	APP_ASSERT_PRINT((mpEfkResHandleTable != NULL),
					 "Failed new mpEfkResHandleTable[%d] \n", mEfkResBufferNum);
	return FALSE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
EffekseerManager::LoadResource(int eEffID, char* effectName)
{
	if (eEffID >= mEfkResBufferNum) {
		return false;
	}

	char file_name[64];
	sprintf_s(file_name, 64, "object/Effekseer/%s", effectName);
	mpEfkResHandleTable[eEffID] = LoadEffekseerEffect(file_name);

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffekseerManager::DeleteEffect(int eEffID)
{
	DeleteEffekseerEffect( mpEfkResHandleTable[eEffID] );
	mpEfkResHandleTable[eEffID] = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------

void
EffekseerManager::Update(VECTOR& rCamPos, VECTOR& rCamTar, VECTOR& rCamUp)
{
	// DXライブラリのカメラとEffekseerのカメラを同期する。
	Effekseer_Sync3DSetting();

	// Effekseerにより再生中のエフェクトを更新する。
	UpdateEffekseer3D();
	
	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer3D();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffekseerManager::Draw(VECTOR& rCamPos, VECTOR& rCamTar, VECTOR& rCamUp)
{
	// DXライブラリのカメラとEffekseerのカメラを同期する。
	Effekseer_Sync3DSetting();

	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer3D();

}

//---------------------------------------------------
/**
 * エフェクト再生
 */
//---------------------------------------------------
EFK_HANDLE
EffekseerManager::Play(int eResHandle, VECTOR* pPos, VECTOR* pScale, float fadeSec/*=0.5f*/)
{
	EFK_HANDLE playingEffectHandle;

	playingEffectHandle = PlayEffekseer3DEffect(mpEfkResHandleTable[eResHandle]);
	
	SetPosPlayingEffekseer3DEffect(playingEffectHandle, pPos->x, pPos->y, pPos->z);
	SetScalePlayingEffekseer3DEffect(playingEffectHandle, pScale->x, pScale->y, pScale->z);

	return playingEffectHandle;
}

//---------------------------------------------------
/**
 * エフェクト再生 (scale 値一括設定)
 */
//---------------------------------------------------
EFK_HANDLE
EffekseerManager::Play(int eResHandle, VECTOR* pPos, float allScale, float fadeSec/*=0.5f*/)
{
	return Play(eResHandle, pPos, &VGet(allScale, allScale, allScale), fadeSec);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
EFK_HANDLE
EffekseerManager::PlayMtx(int eResHandle, MATRIX* pMtx)
{
	// エフェクトの再生
	EFK_HANDLE handle = NULL;
	Effekseer::Matrix43 mtx;

	handle = PlayEffekseer3DEffect(mpEfkResHandleTable[eResHandle]);

	// MATRIX から Effekseer::Matrix43 に変換するマクロ
	Effekseer::Matrix43* p_mtx = &mtx;
	CONVERT_MTX_44_TO_43(p_mtx, pMtx);

	GetEffekseer3DManager()->SetBaseMatrix(handle, mtx);

	return handle;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
EFK_HANDLE
EffekseerManager::PlayMtx(int eResHandle, VECTOR* pTrans, VECTOR* pScale)
{
	EFK_HANDLE handle = NULL;
	Effekseer::Matrix43 mtx, mtx_rot;
	mtx.Indentity();
	mtx_rot.Indentity();

	handle = PlayEffekseer3DEffect(mpEfkResHandleTable[eResHandle]);
	
	mtx.SetSRT(Effekseer::Vector3D(pScale->x, pScale->y, pScale->z),
			   mtx_rot, 
			   Effekseer::Vector3D(pTrans->x, pTrans->y, pTrans->z) );
	GetEffekseer3DManager()->SetBaseMatrix(handle, mtx);

	return handle;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffekseerManager::Stop(EFK_HANDLE eEffectHandle, float fadeSec/*=0.5f*/)
{
	Effekseer::Manager* p_mgr = GetEffekseer3DManager();
	if (p_mgr == NULL) return;

	if (fadeSec == 0.0f) {
		p_mgr->StopEffect(eEffectHandle);
	}
	else {
		p_mgr->StopRoot(eEffectHandle);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
EffekseerManager::IsPlay(EFK_HANDLE eEffectHandle)
{
	return (IsEffekseer3DEffectPlaying(eEffectHandle) == 0) ? true : false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffekseerManager::StopAll(void)
{
	GetEffekseer3DManager()->StopAllEffects();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffekseerManager::SetPosition(EFK_HANDLE eEffectHandle, VECTOR* pSetPos)
{
	SetPosPlayingEffekseer3DEffect(eEffectHandle, pSetPos->x, pSetPos->y, pSetPos->z);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffekseerManager::SetRotationAxisX(EFK_HANDLE eEffectHandle, float rotateX)
{
	GetEffekseer3DManager()->SetRotation(eEffectHandle, Effekseer::Vector3D(1.0f, 0.0f, 0.0f), rotateX);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffekseerManager::SetRotationAxisY(EFK_HANDLE eEffectHandle, float rotateY)
{
	GetEffekseer3DManager()->SetRotation(eEffectHandle, Effekseer::Vector3D(0.0f, 1.0f, 0.0f), rotateY);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffekseerManager::SetRotationAxis(EFK_HANDLE eEffectHandle, VECTOR* pAxisVec, float angle)
{
	Effekseer::Vector3D vec;
	vec.X = pAxisVec->x;
	vec.Y = pAxisVec->y;
	vec.Z = pAxisVec->z;
	GetEffekseer3DManager()->SetRotation(eEffectHandle, vec, angle);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffekseerManager::SetScale(EFK_HANDLE eEffectHandle, VECTOR* pSetSca)
{
	SetScalePlayingEffekseer3DEffect(eEffectHandle, pSetSca->x, pSetSca->y, pSetSca->z);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffekseerManager::SetMatrix(EFK_HANDLE eEffectHandle, MATRIX* pMtx)
{
	// エフェクトの再生
	EFK_HANDLE handle = NULL;
	Effekseer::Matrix43 mtx;

	// MATRIX から Effekseer::Matrix43 に変換するマクロ
	Effekseer::Matrix43* p_mtx = &mtx;
	CONVERT_MTX_44_TO_43(p_mtx, pMtx);

	GetEffekseer3DManager()->SetBaseMatrix(eEffectHandle, mtx);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffekseerManager::SetSpeed(EFK_HANDLE eEffectHandle, float speed)
{
	GetEffekseer3DManager()->SetSpeed(eEffectHandle, speed);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
EffekseerManager::GetUpdateTime(EFK_HANDLE eEffectHandle)
{
	return -1;
}

 /**** end of file ****/

