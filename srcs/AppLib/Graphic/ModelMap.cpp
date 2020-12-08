/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * エフェクトベースクラス
 * @author SPATZ.
 * @data   2012/04/29 09:42:09
 */
//---------------------------------------------------
/*=====================================*
 * include
 *=====================================*/
#include "AppLib/Basic/Basic.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "DataDefine/BGDef.h"
#include "AppLib/Graphic/GraphicManager.h"

#include "ModelMap.h"

#ifdef _DEBUG
#include "AppLib/Input/InputManager.h"
#include "Scene/TotalInfo.h"
#endif // _DEBUG

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ModelMap::ModelMap()
{
	memset(&mBattleboxWork, 0, sizeof(mBattleboxWork));
	mWaterOffsetPos = 0.0f;

	memset(&mBarrierFrameIndex[0], 0, sizeof(mBarrierFrameIndex));
	mBarrierFrameNum = 0;

	mWaveHeight = 0.0f;
	mBarrierRand= 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ModelMap::~ModelMap()
{
	Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelMap::Setup(float scale)
{
	Destroy();

	ModelBase::Setup();

	mWaterOffsetPos = 0.0f;

	memset(&mBarrierFrameIndex[0], 0, sizeof(mBarrierFrameIndex));
	mBarrierFrameNum = 0;

	// 「壁」フレームからマテリアルのディフューズのアルファが０かをチェックし、
	// ０の場合は非表示とする。
	int mesh_num = MV1GetFrameMeshNum(mModelHandle, eBG_TYPE_WALL);
	int mesh_index, mat_index;
	COLOR_F color;
	for (int iI = 0; iI < mesh_num; iI ++) {
		mesh_index = MV1GetFrameMesh(mModelHandle, eBG_TYPE_WALL, iI);
		mat_index = MV1GetMeshMaterial(mModelHandle, mesh_index);
		color = MV1GetMaterialDifColor(mModelHandle, mat_index);
		if (color.a < 0.05f) {
			MV1SetMeshVisible(mModelHandle, mesh_index, FALSE);
		}
	}

	// （元）ルーフの表示を非表示にする
	MV1SetFrameVisible(mModelHandle, eBG_TYPE_ROOF, FALSE);

	// タスクの表示を非表示にする
	MV1SetFrameVisible(mModelHandle, eBG_TYPE_TASK, FALSE);

	// プレイスの表示を非表示にする
	MV1SetFrameVisible(mModelHandle, eBG_TYPE_PLACE, FALSE);

	// アイテムの表示を非表示にする
	MV1SetFrameVisible(mModelHandle, eBG_TYPE_ITEM, FALSE);

	// バトル用の壁を非表示にする
	MV1SetFrameVisible(mModelHandle, eBG_TYPE_BATTLE_WALL, FALSE);

	MV1SetUseVertDifColor( mModelHandle, TRUE);
	MV1SetUseVertSpcColor( mModelHandle, TRUE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelMap::Update(void)
{
#ifdef _DEBUG
	static BOOL sFlg1 = TRUE;
	static BOOL sFlg2 = TRUE;

	if (CHECK_TRIG_KEY(DBG_KEY_VISIBLE_EARTH)) {
		sFlg1 ^= TRUE;
		MV1SetFrameVisible(mModelHandle, eBG_TYPE_EARTH, sFlg1);
	}
	if (CHECK_TRIG_KEY(DBG_KEY_VISIBLE_WALL)) {
		sFlg2 ^= TRUE;
		MV1SetFrameVisible(mModelHandle, eBG_TYPE_WALL, sFlg2);
	}

#endif // _DEBUG

	if (GraphicManager::GetInstance()->GetStopModeUpdate() == false) {

		/*=====================================*
		 * 水面の処理
		 *=====================================*/
		static const float sCenterU = 50.0f;
		static const float sCenterV = 50.0f;
		static float sTime = 0.0f;
		float water_height = 0.0f;

		/* 波をつけると波紋がうまくいかないのでやめます。。。 */
		const float wave_height = (0.0f * ALL_SCALE);
	
		sTime += 0.001f;

		// ２枚の水面をそれぞれ逆回転でUV座標を回転させる
		static float sRot = 0.0f;
		static float sRotB = 0.0f;
		sRot += 0.000005f;
		sRotB += -0.000003f;
		MV1SetFrameTextureAddressTransform(mModelHandle, eBG_TYPE_WATER, 
										   0.0f, 0.0f,
										   1.0f, 1.0f,
										   sCenterU,
										   sCenterV,
										   sRot);
		MV1SetFrameTextureAddressTransform(mModelHandle, eBG_TYPE_WATER_02, 
										   0.0f, 0.0f,
										   1.0f, 1.0f,
										   sCenterU,
										   sCenterV,
										   sRotB);
		// 上下に水面を揺らす
		mWaveHeight = (sin(sTime * 50.0f) * wave_height);

		water_height = mWaveHeight + mWaterOffsetPos;

		MV1SetFrameUserLocalMatrix(mModelHandle, eBG_TYPE_WATER, 
								   MGetTranslate(VGet(0.0f, 
													  water_height,
													  0.0f ))); 
		MV1SetFrameUserLocalMatrix(mModelHandle, eBG_TYPE_WATER_02,
								   MGetTranslate(VGet(0.0f, 
													  water_height,
													  0.0f ))); 

		/*=====================================*
		 * バトルボックスの処理
		 *=====================================*/
		static float sRotC = 0.0;
		switch (mBattleboxWork.mFadeState) {
		case eBTLBOX_FADE_NONE:
			/* nop */
			break;
		
		case eBTLBOX_FADE_IN:
			mBattleboxWork.mAlphaRate += 0.01f;
			if (mBattleboxWork.mAlphaRate >= 0.5f) {
				mBattleboxWork.mAlphaRate = 0.5f;
				mBattleboxWork.mFadeState = eBTLBOX_FADE_USE;
			}
			this->SetAlpha(eBG_TYPE_BATTLE_WALL, mBattleboxWork.mAlphaRate);
			break;

		case eBTLBOX_FADE_USE:
			sRotC += 0.00001f;
			MV1SetFrameTextureAddressTransform(mModelHandle, eBG_TYPE_BATTLE_WALL,
											   0.0f, 0.0f,
											   1.0f, 1.0f,
											   sCenterU,
											   sCenterV,
											   sRotC);
			break;

		case eBTLBOX_FADE_OUT:
			mBattleboxWork.mAlphaRate += -0.03f;
			if (mBattleboxWork.mAlphaRate <= 0.0f) {
				mBattleboxWork.mAlphaRate = 0.0f;
				MV1SetFrameVisible(mModelHandle, eBG_TYPE_BATTLE_WALL, FALSE);
				mBattleboxWork.mFadeState = eBTLBOX_FADE_NONE;
			}
			this->SetAlpha(eBG_TYPE_BATTLE_WALL, mBattleboxWork.mAlphaRate);
			break;

		default:
			break;
		};

		/*=====================================*
		 * バリアの処理
		 *=====================================*/
		mBarrierRand ++;
		if ((mBarrierRand % 3) == 0) {
			for (int iI = 0; iI < mBarrierFrameNum; iI ++) {
				float setX, setY;
				setX = GetRand(10) * 0.1f;
				setY = GetRand(10) * 0.1f;
				MV1SetFrameTextureAddressTransform(mModelHandle, 
												   mBarrierFrameIndex[iI],
												   setX,
												   setY,
												   1.0f,
												   1.0f,
												   sCenterU,
												   sCenterV,
												   0.0f);
			}
		}
	}

	/*=====================================*
	 * 行列を設定
	 *=====================================*/
	if (mMatrixType == eMATRIX_TYPE_TRS) {
		MV1SetPosition(mModelHandle, mPosition);
		MV1SetRotationXYZ(mModelHandle, mRotation);
		MV1SetScale(mModelHandle, VScale(mScale, ALL_SCALE));
	} else if (mMatrixType == eMATRIX_TYPE_MATRIX) {
		MV1SetMatrix(mModelHandle, mMatrix);
	}


	GraphicManager::GetInstance()->StartOriginalShader();

	MV1DrawFrame(mModelHandle, 	eBG_TYPE_EARTH);
	MV1DrawFrame(mModelHandle, 	eBG_TYPE_WALL);
	MV1DrawFrame(mModelHandle, 	eBG_TYPE_VISIBLE);

	GraphicManager::GetInstance()->EndOriginalShader();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelMap::RenderAlphaFrame(void)
{
	// αテスト判定の値は100が理想 (実験済み)
#ifdef LIB_VER_3_12e
	MV1SetMaterialDrawAlphaTestAll(mModelHandle, TRUE, DX_CMP_GREATER, 100);
#else
	SetDrawAlphaTest(DX_CMP_GREATER, 100);
#endif

	// シェーダー適用モデル開始
	GraphicManager::GetInstance()->StartOriginalShader();

	MV1DrawFrame(mModelHandle, eBG_TYPE_ALPHA_EARTH);
	MV1DrawFrame(mModelHandle, eBG_TYPE_ALPHA_WALL);

#ifdef LIB_VER_3_12e
	MV1SetMaterialDrawAlphaTestAll(mModelHandle, TRUE, DX_CMP_GREATER, 128);
#else
	SetDrawAlphaTest(DX_CMP_GREATER, 128);
#endif

	MV1DrawFrame(mModelHandle, eBG_TYPE_ALPHA_VISIBLE);

	// シェーダー適用モデル終了
	GraphicManager::GetInstance()->EndOriginalShader();

#ifdef LIB_VER_3_12e
	MV1SetMaterialDrawAlphaTestAll(mModelHandle, TRUE, DX_CMP_GREATER, 64);
#else
	SetDrawAlphaTest(DX_CMP_GREATER, 64);
#endif

	MV1DrawFrame(mModelHandle, 	eBG_TYPE_WATER);
	MV1DrawFrame(mModelHandle, 	eBG_TYPE_WATER_02);

	for (int iI = eBG_TYPE_GIMMICK; iI < MV1GetFrameNum(mModelHandle); iI ++) {
		MV1DrawFrame(mModelHandle, iI);
	}

#ifdef LIB_VER_3_12e
	MV1SetMaterialDrawAlphaTestAll(mModelHandle, FALSE, DX_CMP_GREATER, 0);
#else
	SetDrawAlphaTest( -1, 0 ) ;
#endif

	MV1DrawFrame(mModelHandle, 	eBG_TYPE_BATTLE_WALL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelMap::Destroy(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelMap::DeleteModel(void)
{
	ModelBase::DeleteModel();

	Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelMap::VisibleBattleBox(void)
{
	MV1SetFrameVisible(mModelHandle, eBG_TYPE_BATTLE_WALL, TRUE);
	mBattleboxWork.mFadeState = eBTLBOX_FADE_IN;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelMap::UnvisibleBattleBox(void)
{
	mBattleboxWork.mFadeState = eBTLBOX_FADE_OUT;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ModelMap::GetFrameMatrixFromIndex(MATRIX* pOut, int frameIndex)
{
	if (pOut == NULL) return false;

	if (frameIndex >= 0) {
		MATRIX mtx = MV1GetFrameLocalWorldMatrix(mModelHandle, frameIndex);
		*pOut = mtx;
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ModelMap::GetFramePosFromIndex(VECTOR* pOut, int frameIndex)
{
	if (pOut == NULL) return false;

	MATRIX mtx;
	bool ret;

	ret = this->GetFrameMatrixFromIndex(&mtx, frameIndex);
	PRINT_ASSERT(ret == true,
				 "Failed GetFramePosFromName : %d\n", frameIndex);
	pOut->x = mtx.m[3][0];
	pOut->y = mtx.m[3][1];
	pOut->z = mtx.m[3][2];
	
	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelMap::SetFramePosFromIndex(int frameIndex, VECTOR* pSetPos)
{
	MV1SetFrameUserLocalMatrix(mModelHandle,
							   frameIndex,
							   MGetTranslate(*pSetPos));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelMap::EntryBarrier(int frameIndex)
{
	// 多重エントリー防止
	for (int iI = 0; iI < kBARRIER_NUM; iI ++) {
		if (mBarrierFrameIndex[iI] == frameIndex) {
			return;
		}
	}

	APP_ASSERT_PRINT(mBarrierFrameNum + 1 < kBARRIER_NUM,
					 "Failed EntryBarrier, capacity over barrier num (%d / %d) \n", (mBarrierFrameNum + 1), kBARRIER_NUM);
	mBarrierFrameIndex[mBarrierFrameNum] = frameIndex;
	mBarrierFrameNum ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelMap::DrawShadowMapFrame(void)
{
#ifdef LIB_VER_3_12e
	MV1SetMaterialDrawAlphaTestAll(mModelHandle, TRUE, DX_CMP_GREATER, 100);
#else
	SetDrawAlphaTest(DX_CMP_GREATER, 100);
#endif

	/* 描画したくないモデルは非表示に設定する */
	MV1SetFrameVisible(mModelHandle, eBG_TYPE_VISIBLE, FALSE);

	MV1SetFrameVisible(mModelHandle, eBG_TYPE_WATER, FALSE);
	MV1SetFrameVisible(mModelHandle, eBG_TYPE_WATER_02, FALSE);

	MV1SetFrameVisible(mModelHandle, eBG_TYPE_ALPHA_EARTH, FALSE);
	MV1SetFrameVisible(mModelHandle, eBG_TYPE_ALPHA_WALL, FALSE);

	MV1DrawModel(mModelHandle);

	MV1SetFrameVisible(mModelHandle, eBG_TYPE_VISIBLE, TRUE);

	MV1SetFrameVisible(mModelHandle, eBG_TYPE_WATER, TRUE);
	MV1SetFrameVisible(mModelHandle, eBG_TYPE_WATER_02, TRUE);

	MV1SetFrameVisible(mModelHandle, eBG_TYPE_ALPHA_EARTH, TRUE);
	MV1SetFrameVisible(mModelHandle, eBG_TYPE_ALPHA_WALL, TRUE);

#ifdef LIB_VER_3_12e
	MV1SetMaterialDrawAlphaTestAll(mModelHandle, FALSE, DX_CMP_GREATER, 0);
#else
	SetDrawAlphaTest( -1, 0 ) ;
#endif
}

/**** end of file ****/
