/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * モデルベースクラス
 * @author SPATZ.
 * @data   2011/11/20 22:51:41	
 */
//---------------------------------------------------
#include "AppLib/Basic/Basic.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "AppLib/Resource/ResourceLayout.h"

#include "ModelBase.h"

#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Collision/CollisionManager.h"
#include "Utility/Utility.h"

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ModelBase::ModelBase()
{
	// initialize
	mModelHandle		= -1;
	mPosition			= ZERO_VEC;
	mRotation			= ZERO_VEC;
	mScale				= VGet(1.0f, 1.0f, 1.0f);
	mMatrix				= MGetIdent();
	mMatrixType			= eMATRIX_TYPE_TRS;
	mSphereTexIndex		= -1;

	mIsSetup			= false;

	mDrawFunc			= NULL;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ModelBase::~ModelBase()
{
	DeleteModel();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::Create(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::Setup(void)
{
	// トゥーンシェーダ無効
	DisableToonShader();

	MV1SetUseZBuffer(mModelHandle, TRUE);
	MV1SetWriteZBuffer(mModelHandle, TRUE);

	this->SetAlpha(1.0f);

	mIsSetup = true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::Calc(void)
{
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::Draw(void)
{
	ModelBase::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::Update(void)
{
	if (mMatrixType == eMATRIX_TYPE_TRS) {
		MV1SetPosition(mModelHandle, mPosition);
		MV1SetRotationXYZ(mModelHandle, mRotation);
		MV1SetScale(mModelHandle, mScale);
	} else if (mMatrixType == eMATRIX_TYPE_MATRIX) {
		MV1SetMatrix(mModelHandle, mMatrix);
	}

	/* キャラ固有の描画関数があるかチェック */
	if (mDrawFunc == NULL) {
		int frame_num = MV1GetFrameNum(mModelHandle);
		for (int iI = 0; iI < frame_num; iI ++) {
			MV1DrawFrame(mModelHandle, iI);
		}
	} else {
		mDrawFunc(mModelHandle);
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::ForciblySetPositionByCentering(bool isUpdateMatrix/*=false*/)
{
	if (mMatrixType == eMATRIX_TYPE_TRS) {
		MV1SetPosition(mModelHandle, mPosition);
		MV1SetRotationXYZ(mModelHandle, mRotation);
		MV1SetScale(mModelHandle, VScale(mScale, ALL_SCALE));
	}
	else if (mMatrixType == eMATRIX_TYPE_MATRIX) {

		MATRIX mtx, tmp;

		// マトリクスを更新させて座標を強制設定
		if (isUpdateMatrix) {
			mtx = MGetIdent();

			// 回転行列
			Utility::ExtractRotationMatrix(&tmp, &mMatrix, 1.0f);
			mtx = MMult(mtx, tmp);

			// 移動行列
			tmp = MGetTranslate(mPosition);
			mtx = MMult(mtx, tmp);

			mMatrix = mtx;
		}

		MV1SetMatrix(mModelHandle, mMatrix);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::Destroy(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::LoadModel(const char* fileName)
{
	if (mModelHandle == -1) {
		mModelHandle = MV1LoadModel(fileName);
		APP_ASSERT_PRINT(mModelHandle != -1,
						 "Failed ResourceModel::GetCloneHandle : %s\n", fileName);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::LinkModel(const char* fileName)
{
	mModelHandle = ResourceModel::GetInstance()->GetCloneHandle(fileName);
	APP_ASSERT_PRINT(mModelHandle != -1,
					 "Failed ResourceModel::GetCloneHandle : %s\n", fileName);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::DeleteModel(void)
{
	if (mModelHandle != -1) {
		MV1DeleteModel(mModelHandle);
		mModelHandle = -1;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::SetMatrixType(eMatrixType type)
{
	if (type == eMATRIX_TYPE_TRS) {
		MV1SetMatrix(mModelHandle, MGetIdent());
	}

	mMatrixType = type; 
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ModelBase::eMatrixType
ModelBase::GetMatrixType(void)
{
	return mMatrixType;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::SetDiffuse(COLOR_F material)
{
	MV1SetDifColorScale(mModelHandle, material) ;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::SetEmissive(COLOR_F material)
{
	MV1SetEmiColorScale(mModelHandle, material);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::SetAmbient(COLOR_F material)
{
	MV1SetAmbColorScale(mModelHandle, material);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::SetSpecular(COLOR_F material)
{
	MV1SetSpcColorScale(mModelHandle, material);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::SetVisible(bool isVisible, int frameIndex/*=-1*/)
{
	int visible = FALSE;
	if (isVisible) visible = TRUE;

	if (frameIndex == -1) {
		MV1SetVisible(mModelHandle, visible);
	} else {
		MV1SetFrameVisible(mModelHandle, frameIndex, visible);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::SetMeshVisible(bool isVisible, int meshIndex)
{
	int visible = FALSE;
	if (isVisible) visible = TRUE;

	MV1SetMeshVisible(mModelHandle, meshIndex, visible);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
float
ModelBase::GetAlpha(void)
{
	mAlpha = MV1GetOpacityRate(mModelHandle);
	return mAlpha;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::SetAlpha(float rate)
{
	mAlpha = rate;
	MV1SetOpacityRate(mModelHandle, mAlpha);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
float
ModelBase::GetAlpha(int frameIndex)
{
	return MV1GetFrameOpacityRate(mModelHandle, frameIndex);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::SetAlpha(int frameIndex, float rate)
{
	MV1SetFrameOpacityRate(mModelHandle, frameIndex, rate);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::EnableToonShader(float dot/*=0.04f*/, float width/*=0.04f*/)
{
	//	float dot = 0.04f;
	//	float width = 0.04f;


	// トゥーンシェーディングを表示する
	for (int iI = 0; iI < MV1GetMaterialNum(mModelHandle); iI ++) {
		MV1SetMaterialOutLineDotWidth(mModelHandle, iI, dot);
		MV1SetMaterialOutLineWidth(mModelHandle, iI, width);
		MV1SetMaterialOutLineColor(mModelHandle ,iI,  GetColorF( 0.0f, 0.0f, 0.0f, 1.0f ) );
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::DisableToonShader(void)
{
	float dot = 0.0f;
	float width = 0.0f;
	// トゥーンシェーディングを非表示にする
	for (int iI = 0; iI < MV1GetMaterialNum(mModelHandle); iI ++) {
		MV1SetMaterialOutLineDotWidth(mModelHandle, iI, dot);
		MV1SetMaterialOutLineWidth(mModelHandle, iI, width);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::SetEnableEmissive(bool isEnable)
{
	if (isEnable) {
		// 自己発光を有効にする
		MV1SetEmiColorScale(mModelHandle, GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else {
		// 自己発光を抑制する
		MV1SetEmiColorScale(mModelHandle, GetColorF(0.0f, 0.0f, 0.0f, 0.0f));
	}
}

//---------------------------------------------------
/**
   指定のフレームから指定のマテリアル名を検索する。
   見つかった場合はフレームインデックスを、
   見つからない場合は-1を返す
 */
//---------------------------------------------------
int
ModelBase::SearchMaterialName(int frameIndex, const char* strMaterialName)
{
	const char* mat_name;
	int mesh_idx, mat_idx;
	int mesh_num = MV1GetFrameMeshNum(mModelHandle, frameIndex);
	if (mesh_num == -1) {
		return -1;
	}
	
	for (int iI = 0; iI < mesh_num; iI ++) {
		mesh_idx = MV1GetFrameMesh(mModelHandle, frameIndex, iI);
		mat_idx = MV1GetMeshMaterial(mModelHandle, mesh_idx);
		mat_name = MV1GetMaterialName(mModelHandle, mat_idx);

		if (strcmp(mat_name, strMaterialName) == 0) {
			return mesh_idx;
		}
	}
	return -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
ModelBase::GetIncludeMaterialNameNum(int frameIndex, const char* strMaterialName)
{
	const char* mat_name;
	int mesh_idx, mat_idx;
	int mesh_num = MV1GetFrameMeshNum(mModelHandle, frameIndex);
	int include_num = 0;

	if (mesh_num == -1) {
		return -1;
	}
	
	for (int iI = 0; iI < mesh_num; iI ++) {
		mesh_idx = MV1GetFrameMesh(mModelHandle, frameIndex, iI);
		mat_idx = MV1GetMeshMaterial(mModelHandle, mesh_idx);
		mat_name = MV1GetMaterialName(mModelHandle, mat_idx);

		unsigned int iJ, materialNameLength;
		materialNameLength = strlen(strMaterialName);
		if (strlen(mat_name) < materialNameLength) {
			continue;
		}
		for (iJ = 0; iJ < materialNameLength; iJ ++) {
			if (mat_name[iJ] != strMaterialName[iJ]) {
				break;
			}
		}
		if (iJ == materialNameLength) {
			include_num ++;
		}
	}
	return include_num;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::GetMeshMinPosition(VECTOR* pOutPos, int mesh_index)
{
	*pOutPos = VScale(MV1GetMeshMinPosition(mModelHandle, mesh_index), mScale.x);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::GetMeshMaxPosition(VECTOR* pOutPos, int mesh_index)
{
	*pOutPos = VScale(MV1GetMeshMaxPosition(mModelHandle, mesh_index), mScale.x);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::GetMeshMdlPosition(VECTOR* pOutPos, int mesh_index)
{
	VECTOR min, max;
	min = VScale(MV1GetMeshMinPosition(mModelHandle, mesh_index), mScale.x);
	max = VScale(MV1GetMeshMaxPosition(mModelHandle, mesh_index), mScale.x);
	
	*pOutPos = VScale(VAdd(min, max), 0.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::SetPositionOnCollision(void)
{
	VECTOR vec_start, vec_end, hitpos, normal;

	vec_start	= VAdd(mPosition, VGet(0.0f, (10.0f * ALL_SCALE), 0.0f));
	vec_end		= mPosition;

	for (int iI = 0; iI < 5; iI ++) {
		vec_end.y	+= -200.0f;

		if (CollisionManager::GetInstance()->CheckEarthCollision(vec_start, vec_end, &hitpos, &normal) != NOT_ON_COLLISION) {
			mPosition = hitpos;
			return;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::SetRotationToTarget(VECTOR* pTargetPos)
{
	VECTOR vec;

	vec = VSub(*pTargetPos, this->mPosition);
	this->mRotation.y = atan2(vec.x, vec.z) + DX_PI;
}

//---------------------------------------------------
/**
 * 指定のテクスチャ番号のテクスチャを変更する	
 */
//---------------------------------------------------
void
ModelBase::ChangeTexture(BYTE texIndex, char* texName, BOOL isLoad/*=FALSE*/)
{
	int graph_handle = -1;
	if (isLoad) {
		ResourceLayout::GetInstance()->LoadLayoutResource(texName);
	}
	graph_handle = ResourceLayout::GetInstance()->GetHandle(texName);
	MV1SetTextureGraphHandle(mModelHandle, texIndex, graph_handle, TRUE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::ChangeSphereTexAll(int texIndex)
{
	for (int iI = 0; iI < MV1GetMaterialNum(mModelHandle); iI ++) {
		MV1SetMaterialSphereMapTexture(mModelHandle, iI, texIndex);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::RevertSphereTexAll(void)
{
	ChangeSphereTexAll(mSphereTexIndex);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::GetRotationVecFromMatrix(VECTOR* pRotVec)
{
	APP_ASSERT((mMatrixType == eMATRIX_TYPE_MATRIX),
			   "Matrix type incorrect case \n");

	MATRIX mtx;
	
	Utility::ExtractRotationMatrix(&mtx, &mMatrix, 1.0f);
	*pRotVec = VNorm(VTransform(VGet(0.0f, 0.0f, -1.0f), mtx));
}

//---------------------------------------------------
/**
   TRSの情報を元にマトリックスの値を更新
 */
//---------------------------------------------------
void
ModelBase::UpdateMatrixFromTRS(void)
{
	MATRIX tmp;
	mMatrix = MGetIdent();

	// 拡縮行列
	tmp = MGetScale(VGet(mScale.x, mScale.y, mScale.z));
	mMatrix = MMult(mMatrix, tmp);

	// 回転行列
	tmp = MGetRotX(mRotation.x);
	mMatrix = MMult(mMatrix, tmp);

	tmp = MGetRotY(mRotation.y);
	mMatrix = MMult(mMatrix, tmp);

	tmp = MGetRotZ(mRotation.z);
	mMatrix = MMult(mMatrix, tmp);

	// 移動行列
	tmp = MGetTranslate(mPosition);
	mMatrix = MMult(mMatrix, tmp);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelBase::UpdateMatrixFromRotY_Normal(float setRotY, VECTOR& rNormal)
{
	MATRIX mtx, tmp;
	mtx = MGetIdent();
	// 拡縮行列
	tmp = MGetIdent();
	tmp = MGetScale(VGet(mScale.x, mScale.y, mScale.z));
	mtx = MMult(mtx, tmp);
	
	VECTOR vecZ, crossZ, crossX;
	vecZ.x = sin(setRotY + DX_PI);
	vecZ.y = 0.0f;
	vecZ.z = cos(setRotY + DX_PI);
	crossX = VCross(vecZ, UP_VEC);
	crossZ = VCross(crossX, rNormal);

	// 回転行列
	tmp = MGetIdent();
	Utility::MTXLookAtMatrix(&tmp,(VECTOR *)&ZERO_VEC,
							 &crossZ,
							 &rNormal);
	mtx = MMult(mtx, tmp);
	
	// 移動行列
	tmp = MGetIdent();
	tmp = MGetTranslate(mPosition);
	mtx = MMult(mtx, tmp);

	mMatrix = mtx;

}

 /**** end of file ****/
