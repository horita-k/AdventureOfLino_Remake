/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * コリジョン統括マネージャクラス
 * @author SPATZ.
 * @data   2011/11/27 12:37:00
 */
//---------------------------------------------------
#include "AppLib/Basic/Basic.h"
#include "CollisionManager.h"
#include "Scene/ActionTaskManager.h"
#include "DataDefine/BGDef.h"

#ifdef _DEBUG
#include "../Graphic/GraphicManager.h"
#endif // _DEBUG

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/CollisionManager * const
CollisionManager::GetInstance(void)
{
	static CollisionManager sInstance;
	return &sInstance;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CollisionManager::CollisionManager()
{
	initialize();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CollisionManager::~CollisionManager()
{
	Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CollisionManager::Create(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CollisionManager::Setup(const ModelBase* pEarthModel, const ModelBase* pWallModel)
{
	mpEarthModel= (ModelBase *)pEarthModel;
	mpWallModel	= (ModelBase *)pWallModel;

	for (int iI = 0; iI < kCHECK_COLLISION_MAX; iI ++) {
		mEarthCollWork[iI].mModelHandle = -1;
		mEarthCollWork[iI].mFrameIndex = -1;
	}
	mEarthCollNum = 0;
	
	for (int iI = 0; iI < kCHECK_COLLISION_MAX; iI ++) {
		mWallCollWork[iI].mModelHandle = -1;
		mWallCollWork[iI].mFrameIndex = -1;
	}
	mWallCollNum = 0;

	MV1SetupCollInfo(mpEarthModel->GetModelHandle(), eBG_TYPE_TASK, 8, 8, 8);
	MV1SetupCollInfo(mpEarthModel->GetModelHandle(), eBG_TYPE_BATTLE_WALL, 8, 8, 8);
	MV1SetupCollInfo(mpEarthModel->GetModelHandle(), eBG_TYPE_WATER, 8, 8, 8);

	if (mpWallModel != NULL) {
		MV1SetupCollInfo(mpWallModel->GetModelHandle(), eBG_TYPE_TASK, 8, 8, 8);
		MV1SetupCollInfo(mpWallModel->GetModelHandle(), eBG_TYPE_BATTLE_WALL, 8, 8, 8);
		MV1SetupCollInfo(mpWallModel->GetModelHandle(), eBG_TYPE_WATER, 8, 8, 8);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CollisionManager::Update(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CollisionManager::Destroy(void)
{
	int ret;
	if (mpEarthModel != NULL) {
		for (int iI = 0; iI < mEarthCollNum; iI ++) {
			ret = MV1TerminateCollInfo(mEarthCollWork[iI].mModelHandle, mEarthCollWork[iI].mFrameIndex);
		}
		for (int iI = 0; iI < mWallCollNum; iI ++) {
			ret = MV1TerminateCollInfo(mWallCollWork[iI].mModelHandle, mWallCollWork[iI].mFrameIndex);
		}
		ret = MV1TerminateCollInfo(mpEarthModel->GetModelHandle(), eBG_TYPE_TASK);
		ret = MV1TerminateCollInfo(mpEarthModel->GetModelHandle(), eBG_TYPE_BATTLE_WALL);
		ret = MV1TerminateCollInfo(mpEarthModel->GetModelHandle(), eBG_TYPE_WATER);
	}

	if (mpWallModel != NULL) {
		ret = MV1TerminateCollInfo(mpWallModel->GetModelHandle(), eBG_TYPE_TASK);
		ret = MV1TerminateCollInfo(mpWallModel->GetModelHandle(), eBG_TYPE_BATTLE_WALL);
		ret = MV1TerminateCollInfo(mpWallModel->GetModelHandle(), eBG_TYPE_WATER);
	}

	initialize();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CollisionManager::Refresh(void)
{
	if (mpEarthModel != NULL) {
		for (int iI = 0; iI < mEarthCollNum; iI ++) {
			MV1RefreshCollInfo(mEarthCollWork[iI].mModelHandle, mEarthCollWork[iI].mFrameIndex);
		}
		for (int iI = 0; iI < mWallCollNum; iI ++) {
			MV1RefreshCollInfo(mWallCollWork[iI].mModelHandle, mWallCollWork[iI].mFrameIndex);
		}
		MV1RefreshCollInfo(mpEarthModel->GetModelHandle(), eBG_TYPE_TASK);
		MV1RefreshCollInfo(mpEarthModel->GetModelHandle(), eBG_TYPE_BATTLE_WALL);
		MV1RefreshCollInfo(mpEarthModel->GetModelHandle(), eBG_TYPE_WATER);
	}
	if (mpWallModel != NULL) {
		MV1RefreshCollInfo(mpWallModel->GetModelHandle(), eBG_TYPE_TASK);
		MV1RefreshCollInfo(mpWallModel->GetModelHandle(), eBG_TYPE_BATTLE_WALL);
		MV1RefreshCollInfo(mpWallModel->GetModelHandle(), eBG_TYPE_WATER);
	}
}

//---------------------------------------------------
/**
 * 指定のモデルのフレームのコリジョンを更新する
 */
//---------------------------------------------------
void
CollisionManager::RefreshModel(int modelHandle, int frameIndex)
{
	MV1RefreshCollInfo(modelHandle, frameIndex);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CollisionManager::EntryEarthCollision(int modelHandle, int frameIndex)
{
	// 多重エントリーのチェック
	for (int iI = 0; iI < mEarthCollNum; iI ++) {
		if ((mEarthCollWork[iI].mModelHandle == modelHandle) &&
			(mEarthCollWork[iI].mFrameIndex == frameIndex)) {
			// 既にエントリーされているので終了
			return;
		}
	}

	APP_ASSERT((mEarthCollNum + 1) < kCHECK_COLLISION_MAX,
			"Failed EntryEarthCollision(), over array \n");

	// 不要なコリジョンは挿入させない
	int frame_num = MV1GetFrameMeshNum(modelHandle, frameIndex);
	if (frame_num == 0) {
		return;
	}

	mEarthCollWork[mEarthCollNum].mModelHandle = modelHandle;
	mEarthCollWork[mEarthCollNum].mFrameIndex = frameIndex;
	MV1SetupCollInfo(mEarthCollWork[mEarthCollNum].mModelHandle, 
					 mEarthCollWork[mEarthCollNum].mFrameIndex, 
					 8, 8, 8);

	mEarthCollNum ++;

#ifdef _DEBUG
	PRINT_CONSOLE("%s : mEarthCollNum : %d \n", __FUNCDNAME__, mEarthCollNum);
#endif // _DEBUG
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CollisionManager::EntryWallCollision(int modelHandle, int frameIndex)
{
	// 多重エントリーのチェック
	for (int iI = 0; iI < mWallCollNum; iI ++) {
		if ((mWallCollWork[iI].mModelHandle == modelHandle) &&
			(mWallCollWork[iI].mFrameIndex == frameIndex)) {
			// 既にエントリーされているので終了
			return;
		}
	}

	APP_ASSERT((mWallCollNum + 1) < kCHECK_COLLISION_MAX,
			   "Failed EntryWallCollision(), over array \n");

	// 不要なコリジョンは挿入させない
	int frame_num = MV1GetFrameMeshNum(modelHandle, frameIndex);
	if (frame_num == 0) {
		return;

	}

	mWallCollWork[mWallCollNum].mModelHandle = modelHandle;
	mWallCollWork[mWallCollNum].mFrameIndex = frameIndex;
	MV1SetupCollInfo(mWallCollWork[mWallCollNum].mModelHandle, 
					 mWallCollWork[mWallCollNum].mFrameIndex, 
					 8, 8, 8);

	mWallCollNum ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CollisionManager::RemoveEarthCollision(int modelHandle, int frameIndex)
{
	int serchindex = -1;
	for (int iI = 0; iI < mEarthCollNum; iI ++) {
		if ((mEarthCollWork[iI].mModelHandle == modelHandle) &&
			(mEarthCollWork[iI].mFrameIndex == frameIndex)) {
			serchindex = iI;
			break;
		}
	}
	if (serchindex == -1) {
		return false;
	}

	for (int iI = (serchindex + 1); iI < mEarthCollNum; iI ++) {
		mEarthCollWork[iI - 1].mModelHandle = mEarthCollWork[iI].mModelHandle;
		mEarthCollWork[iI - 1].mFrameIndex = mEarthCollWork[iI].mFrameIndex;
	}
	mEarthCollWork[mEarthCollNum].mModelHandle = -1;
	mEarthCollWork[mEarthCollNum].mFrameIndex = -1;

	mEarthCollNum --;

#ifdef _DEBUG
	PRINT_CONSOLE("%s : mEarthCollNum : %d \n", __FUNCDNAME__, mEarthCollNum);
#endif // _DEBUG

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CollisionManager::RemoveWallCollision(int modelHandle, int frameIndex)
{
	int serchindex = -1;
	for (int iI = 0; iI < mWallCollNum; iI ++) {
		if ((mWallCollWork[iI].mModelHandle == modelHandle) &&
			(mWallCollWork[iI].mFrameIndex == frameIndex)) {
			serchindex = iI;
			break;
		}
	}
	if (serchindex == -1) {
		return false;
	}

	for (int iI = (serchindex + 1); iI < mWallCollNum; iI ++) {
		mWallCollWork[iI - 1].mModelHandle = mWallCollWork[iI].mModelHandle;
		mWallCollWork[iI - 1].mFrameIndex = mWallCollWork[iI].mFrameIndex;
	}
	mWallCollWork[mWallCollNum].mModelHandle = -1;
	mWallCollWork[mWallCollNum].mFrameIndex = -1;

	mWallCollNum --;

	return true;
}

//---------------------------------------------------
/**
 * マップモデルとの当たり判定
 */
//---------------------------------------------------
INT8
CollisionManager::CheckEarthCollision(const VECTOR& rVecStart, const VECTOR& rVecEnd, VECTOR* pOutHitPos, VECTOR* pOutNormal)
{
	MV1_COLL_RESULT_POLY result_work;

	for (int iI = (mEarthCollNum - 1); iI >= 0; iI --) {
		result_work = MV1CollCheck_Line(mEarthCollWork[iI].mModelHandle,
										mEarthCollWork[iI].mFrameIndex,
										rVecStart,
										rVecEnd);
		if (result_work.HitFlag == 1) {
			*pOutHitPos = result_work.HitPosition;
			*pOutNormal = result_work.Normal;

#ifdef _DEBUG
			VECTOR position[3];
			memcpy(&position[0], &result_work.Position[0], sizeof(position));
			GraphicManager::GetInstance()->DbgRenderLine(&position[0], &position[1]);
			GraphicManager::GetInstance()->DbgRenderLine(&position[1], &position[2]);
			GraphicManager::GetInstance()->DbgRenderLine(&position[2], &position[0]);
#endif // _DEBUG

			return (INT8)iI;
		}
	}

	return NOT_ON_COLLISION;
}

//---------------------------------------------------
/**
 * マップモデルとの当たり判定 (マテリアル付加)
 */
//---------------------------------------------------
INT8
CollisionManager::CheckEarthCollision(const VECTOR& rVecStart, const VECTOR& rVecEnd, VECTOR* pOutHitPos, VECTOR* pOutNormal, int* pOutFrameIndex, BYTE* pOutMat)
{
	MV1_COLL_RESULT_POLY result_work;
	for (int iI = (mEarthCollNum - 1); iI >= 0; iI --) {
		result_work = MV1CollCheck_Line(mEarthCollWork[iI].mModelHandle,
										mEarthCollWork[iI].mFrameIndex,
										rVecStart,
										rVecEnd);
		if (result_work.HitFlag == 1) {
			*pOutHitPos = result_work.HitPosition;
			*pOutNormal = result_work.Normal;
			*pOutMat	= (BYTE)result_work.MaterialIndex;
			*pOutFrameIndex = (int)result_work.FrameIndex;

#ifdef _DEBUG
			VECTOR position[3];
			memcpy(&position[0], &result_work.Position[0], sizeof(position));
			GraphicManager::GetInstance()->DbgRenderLine(&position[0], &position[1]);
			GraphicManager::GetInstance()->DbgRenderLine(&position[1], &position[2]);
			GraphicManager::GetInstance()->DbgRenderLine(&position[2], &position[0]);
#endif // _DEBUG

			return (INT8)iI;
		}
	};
	return NOT_ON_COLLISION;
}


//---------------------------------------------------
/**
 * マップモデルとの当たり判定 (複数モデル対応)
 */
//---------------------------------------------------
INT8
CollisionManager::CheckEarthCollisionMulti(const VECTOR& rVecStart, const VECTOR& rVecEnd, VECTOR* pOutHitPos, VECTOR* pOutNormal, int* pOutFrameIndex, BYTE* pOutMat)
{
	MV1_COLL_RESULT_POLY_DIM result_work;

	float max_y = -99999.9f;
	INT8 coll_index = NOT_ON_COLLISION;
	int hit_index;

	for (int iI = (mEarthCollNum - 1); iI >= 0; iI --) {
		hit_index = -1;

		result_work = MV1CollCheck_LineDim(mEarthCollWork[iI].mModelHandle,
										   mEarthCollWork[iI].mFrameIndex,
										   rVecStart,
										   rVecEnd);
		
		for (int iJ = 0; iJ < result_work.HitNum; iJ ++) {
			if (max_y < result_work.Dim[iJ].HitPosition.y) {
				max_y = result_work.Dim[iJ].HitPosition.y;
				hit_index = iJ;
			}
		}

		if (hit_index != -1) {
			*pOutHitPos		= result_work.Dim[hit_index].HitPosition;
			*pOutNormal		= result_work.Dim[hit_index].Normal;
			*pOutMat		= (BYTE)result_work.Dim[hit_index].MaterialIndex;
			*pOutFrameIndex = (int)result_work.Dim[hit_index].FrameIndex;
			coll_index = (INT8)iI;
		}

		// 当たり判定情報の破棄
		if (result_work.HitNum > 0) {
			MV1CollResultPolyDimTerminate(result_work);
		}
	}

	return coll_index;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
INT8
CollisionManager::CheckEarthCollisionNearest(const VECTOR& rVecStart, 
											 const VECTOR& rVecEnd, 
											 VECTOR* pOutHitPos
											 ) 
{
	MV1_COLL_RESULT_POLY_DIM result_poly;

	float nearest_dist = 9999.9f;
	CHAR collision_index = NOT_ON_COLLISION;

	for (int iI = (mEarthCollNum - 1); iI >= 0; iI --) {
		
		result_poly = MV1CollCheck_LineDim(mEarthCollWork[iI].mModelHandle,
										   mEarthCollWork[iI].mFrameIndex,
										   rVecStart,
										   rVecEnd);

		// ヒットした頂点分だけチェック
		for (int iJ = 0; iJ < result_poly.HitNum; iJ ++) {
			// チェック開始点と交点の距離を求める
			float dist = VSize(VSub(rVecStart, result_poly.Dim[iJ].HitPosition));
			if (nearest_dist > dist) {
				nearest_dist = dist;

				*pOutHitPos = result_poly.Dim[iJ].HitPosition;

				collision_index = iI;
			}
		}

		if (result_poly.HitNum > 0) {
			MV1CollResultPolyDimTerminate(result_poly);
		}
	}

	return collision_index;
}


//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
INT8
CollisionManager::CheckEarthCollisionExcept(int exceptModelHandle, const VECTOR& rVecStart, const VECTOR& rVecEnd, VECTOR* pOutHitPos, VECTOR* pOutNormal)
{
	MV1_COLL_RESULT_POLY result_work;

	for (int iI = (mEarthCollNum - 1); iI >= 0; iI --) {

		if (mEarthCollWork[iI].mModelHandle == exceptModelHandle) {
			continue;
		}

		result_work = MV1CollCheck_Line(mEarthCollWork[iI].mModelHandle,
										mEarthCollWork[iI].mFrameIndex,
										rVecStart,
										rVecEnd);
		if (result_work.HitFlag == 1) {
			*pOutHitPos = result_work.HitPosition;
			*pOutNormal = result_work.Normal;
			return (INT8)iI;
		}
	}

	return NOT_ON_COLLISION;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CollisionManager::CheckWallCollision(const VECTOR& rVecStart, const VECTOR& rVecEnd, VECTOR* pOutHitPos, VECTOR* pOutNormal)
{
	MV1_COLL_RESULT_POLY result_work;

	for (int iI = 0; iI < mWallCollNum; iI ++) {
		result_work = MV1CollCheck_Line(mWallCollWork[iI].mModelHandle,
										mWallCollWork[iI].mFrameIndex,
										rVecStart, 
										rVecEnd);
		if (result_work.HitFlag == 1) {
			*pOutHitPos = result_work.HitPosition;
			*pOutNormal = result_work.Normal;
			return true;
		}
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CollisionManager::CheckWallCollision(const VECTOR& rVecStart, const VECTOR& rVecEnd, VECTOR* pOutHitPos, int* pOutCollIndex)
{
	MV1_COLL_RESULT_POLY result_work;

	for (int iI = 0; iI < mWallCollNum; iI ++) {
		result_work = MV1CollCheck_Line(mWallCollWork[iI].mModelHandle,
										mWallCollWork[iI].mFrameIndex,
										rVecStart, 
										rVecEnd);
		if (result_work.HitFlag == 1) {
			*pOutHitPos = result_work.HitPosition;
			*pOutCollIndex = iI;
			return true;
		}
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CollisionManager::CheckWallCollisionNearest(const VECTOR& rVecStart, 
											const VECTOR& rVecEnd, 
											VECTOR* pOutHitPos, 
											int* pOutCollIndex) 
{
	MV1_COLL_RESULT_POLY_DIM result_work;

	float nearest_dist = 9999.9f;
	CHAR collision_index = NOT_ON_COLLISION;

	for (int iI = (mWallCollNum - 1); iI >= 0; iI --) {
		result_work = MV1CollCheck_LineDim(mWallCollWork[iI].mModelHandle,
										   mWallCollWork[iI].mFrameIndex,
										   rVecStart, 
										   rVecEnd);

		// ヒットした頂点分だけチェック
		for (USHORT iJ = 0; iJ < result_work.HitNum; iJ ++) {
			float dist = VSize(VSub(rVecStart, result_work.Dim[iJ].HitPosition));
			if (nearest_dist > dist) {
				nearest_dist = dist;
			}

			*pOutHitPos = result_work.Dim[iJ].HitPosition;
			*pOutCollIndex = iI;
		}

		if (result_work.HitNum > 0) {
			MV1CollResultPolyDimTerminate(result_work);
		}
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CollisionManager::CheckWallCollisionExcept(int exceptModelHandle, const VECTOR& rVecStart, const VECTOR& rVecEnd, VECTOR* pOutHitPos, VECTOR* pOutNormal)
{
	MV1_COLL_RESULT_POLY result_work;

	for (int iI = 0; iI < mWallCollNum; iI ++) {

		if (mWallCollWork[iI].mModelHandle == exceptModelHandle) {
			continue;
		}

		result_work = MV1CollCheck_Line(mWallCollWork[iI].mModelHandle,
										mWallCollWork[iI].mFrameIndex,
										rVecStart, 
										rVecEnd);
		if (result_work.HitFlag == 1) {
			*pOutHitPos = result_work.HitPosition;
			*pOutNormal = result_work.Normal;
			return true;
		}
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CollisionManager::CheckTaskCollision(const VECTOR& rVecStart, const VECTOR& rVecEnd, void* pParam, VECTOR* pOutNormal)
{
	MV1_COLL_RESULT_POLY result_work;

	result_work = MV1CollCheck_Line(mpEarthModel->GetModelHandle(), 
									eBG_TYPE_TASK,
									rVecStart, 
									rVecEnd);

	if (result_work.HitFlag == 1) {
		int material_index = result_work.MaterialIndex;
		char** pp_name = (char **)pParam;
		*pp_name = (char *)MV1GetMaterialName(mpEarthModel->GetModelHandle(),
											  material_index);
		*pOutNormal = result_work.Normal;
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CollisionManager::CheckSideCollision(ShapeCapsule* pCapsule, VECTOR* pMoveVec, VECTOR* pOutNormal)
{
	bool result = false;
	MV1_COLL_RESULT_POLY_DIM HitResDim = {0};
	MV1_COLL_RESULT_POLY *HitRes ;

	for (int iI = 0; iI < mWallCollNum; iI ++) {
		if (result == true) break;

		// プレイヤーの直下に存在する地面のポリゴンを取得
		HitResDim = MV1CollCheck_Capsule(mWallCollWork[iI].mModelHandle,
										 mWallCollWork[iI].mFrameIndex,
										 pCapsule->mPosA,
										 pCapsule->mPosB,
										 pCapsule->mRadius);

		HitRes = HitResDim.Dim ;
		for(int iJ = 0 ; iJ < HitResDim.HitNum ; iJ ++, HitRes ++ ) {
#ifdef _DEBUG
			VECTOR position[3];
			memcpy(&position[0], &HitRes->Position[0], sizeof(position));
			GraphicManager::GetInstance()->DbgRenderLine(&position[0], &position[1]);
			GraphicManager::GetInstance()->DbgRenderLine(&position[1], &position[2]);
			GraphicManager::GetInstance()->DbgRenderLine(&position[2], &position[0]);
#endif // _DEBUG
			
			float vec_dot;
			vec_dot = VDot(HitRes->Normal, *pMoveVec);

			if (vec_dot < 0.0f) {
				*pOutNormal = HitRes->Normal;
				result = true;
				break;
			}
		}
		// 検出した地面ポリゴン情報の後始末
		if (HitResDim.HitNum > 0) {
			MV1CollResultPolyDimTerminate( HitResDim ) ;
		}
	}
	return result;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CollisionManager::CheckWaterCollision(const VECTOR& rVecStart, const VECTOR& rVecEnd, VECTOR* pOutHitPos, VECTOR* pOutNormal)
{
	MV1_COLL_RESULT_POLY result_work;

	result_work = MV1CollCheck_Line(mpEarthModel->GetModelHandle(), 
									eBG_TYPE_WATER,
									rVecStart, 
									rVecEnd);
	if (result_work.HitFlag == 1) {
		*pOutHitPos = result_work.HitPosition;
		*pOutNormal = result_work.Normal;
		return true;
	}
	if (mpWallModel != NULL) {
		result_work = MV1CollCheck_Line(mpWallModel->GetModelHandle(), 
										eBG_TYPE_WATER, 
										rVecStart, 
										rVecEnd);
		if (result_work.HitFlag == 1) {
			*pOutHitPos = result_work.HitPosition;
			*pOutNormal = result_work.Normal;
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
CollisionManager::GetEarthModelNum(void)
{
	return mEarthCollNum;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
CollisionManager::GetEarthModelHandle(int modelIndex)
{
	return mEarthCollWork[modelIndex].mModelHandle;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
CollisionManager::GetEarthModelFrameIndex(int modelIndex)
{
	return mEarthCollWork[modelIndex].mFrameIndex;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ModelBase* 
CollisionManager::GetEarthModel(void)
{
	return mpEarthModel;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
CollisionManager::GetWallModelNum(void)
{
	return mWallCollNum;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
CollisionManager::GetWallModelHandle(int modelIndex)
{
	return mWallCollWork[modelIndex].mModelHandle;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
CollisionManager::GetWallModelFrameIndex(int modelIndex)
{
	return mWallCollWork[modelIndex].mFrameIndex;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ModelBase* 
CollisionManager::GetWallModel(void)
{
	return mpWallModel;
}

/*=====================================*
 * BodyCollision 関連
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CollisionManager::EntryBodyCollision(ModelBase* pModel)
{
	entryVector(&mBodyCollisionVector, pModel);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CollisionManager::RemoveBodyCollision(ModelBase* pModel)
{
	removeVector(&mBodyCollisionVector, pModel);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CollisionManager::RemoveBodyCollisionAll(void)
{
	removeVectorAll(&mBodyCollisionVector);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
CollisionManager::GetBodyCollisionNum(void)
{
	return getVectorNum(&mBodyCollisionVector);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ModelBase*
CollisionManager::GetBodyCollisionAddress(int modelIndex)
{
	return getVectorAddress(&mBodyCollisionVector, modelIndex);
}

/*=====================================*
 * AttackCollision 関連
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CollisionManager::EntryAttackCollision(ModelBase* pModel)
{
	entryVector(&mAttackCollisionVector, pModel);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CollisionManager::RemoveAttackCollision(ModelBase* pModel)
{
	removeVector(&mAttackCollisionVector, pModel);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CollisionManager::RemoveAttackCollisionAll(void)
{
	removeVectorAll(&mAttackCollisionVector);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
CollisionManager::GetAttackCollisionNum(void)
{
	return getVectorNum(&mAttackCollisionVector);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ModelBase*
CollisionManager::GetAttackCollisionAddress(int modelIndex)
{
	return getVectorAddress(&mAttackCollisionVector, modelIndex);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ModelBase*
CollisionManager::GetAttackCollisionPlayerAddress(void)
{
	return getVectorAddress(&mAttackCollisionVector, 0); // ０きめ打ち
}

/*=====================================*
 * 汎用コリジョン関数
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/bool
CollisionManager::Check_SphereToSphere(tSphereWork* pSphereA, tSphereWork* pSphereB, VECTOR* pOutVec, float* pOutDist)
{
	VECTOR vec = VSub(pSphereB->mPosition, pSphereA->mPosition);
	float len = VSize(vec);
	float total_size = pSphereB->mSize + pSphereA->mSize;

	if (len < total_size) {
		*pOutVec = vec;
		*pOutDist = total_size - len;
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
 // 球と円柱の当たり判定
 */
//---------------------------------------------------
/*static*/bool
CollisionManager::Check_SphereToPillar(CollisionManager::tSphereWork* pSphere, 
									 CollisionManager::tPillarWork* pPiller)
{
	VECTOR vec, tmpA, tmpB;
	tmpA = pSphere->mPosition;
	tmpB = pPiller->mPosition;
	tmpA.y = 0.0f;
	tmpB.y = 0.0f;
	
	vec = VSub(tmpA, tmpB);
	if (VSize(vec) < (pSphere->mSize + pPiller->mSize)) {
		if (((pPiller->mPosition.y + pPiller->mHeight) >= (pSphere->mPosition.y - pSphere->mSize)) &&
			(pPiller->mPosition.y <= (pSphere->mPosition.y + pSphere->mSize))) {
			return true;
		}
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/bool
CollisionManager::Check_LineToCapsule(ShapeLine* pLine, ShapeCapsule* pCapsule)
{
	VECTOR tmp = pLine->mEnd;
	tmp.y += 1.0f;

	if (HitCheck_Capsule_Triangle(pCapsule->mPosA,
								  pCapsule->mPosB,
								  pCapsule->mRadius,
								  pLine->mStart,
								  pLine->mEnd,
								  tmp)) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/ bool
CollisionManager::Check_CapsuleToCapsule(ShapeCapsule* pCapsuleA, ShapeCapsule* pCapsuleB)
{
	if (HitCheck_Capsule_Capsule(pCapsuleA->mPosA,
								 pCapsuleA->mPosB,
								 pCapsuleA->mRadius,
								 pCapsuleB->mPosA,
								 pCapsuleB->mPosB,
								 pCapsuleB->mRadius)) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/ bool
CollisionManager::Check_LineToBoard(ShapeLine* pLine, ShapeBoard* pBoard)
{
	HITRESULT_LINE hitresult0, hitresult1;
	hitresult0 = HitCheck_Line_Triangle(pLine->mStart, pLine->mEnd,
										pBoard->mPosA, pBoard->mPosB, pBoard->mPosC);
	hitresult1 = HitCheck_Line_Triangle(pLine->mStart, pLine->mEnd,
										pBoard->mPosC, pBoard->mPosD, pBoard->mPosA);

	if (hitresult0.HitFlag == TRUE || hitresult1.HitFlag == TRUE) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
// 2Dベクトルの外積
float D3DXVec2Cross( VECTOR* v1, VECTOR* v2 ) {
   return v1->x * v2->z - v1->z * v2->x;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/ bool
CollisionManager::Check_LineToLine(VECTOR& rSeg1Pos, VECTOR& rSeg1Vec, VECTOR& rSeg2Pos,VECTOR& rSeg2Vec, VECTOR& rOutPos)
{
	VECTOR vec = VSub(rSeg2Pos, rSeg1Pos);
	float Crs_v1_v2 = D3DXVec2Cross( &rSeg1Vec, &rSeg2Vec );
	if ( Crs_v1_v2 == 0.0f ) {
		// 平行状態
		return false;
	}

	float Crs_v_v1 = D3DXVec2Cross( &vec, &rSeg1Vec );
	float Crs_v_v2 = D3DXVec2Cross( &vec, &rSeg2Vec );

	float t1 = Crs_v_v2 / Crs_v1_v2;
	float t2 = Crs_v_v1 / Crs_v1_v2;

	const float eps_value = 0.00001f;
	if ( t1 + eps_value < 0 || t1 - eps_value > 1 || t2 + eps_value < 0 || t2 - eps_value > 1 ) {
		// 交差していない
		return false;
	}

	rOutPos = VAdd(rSeg1Pos, VScale(rSeg1Vec, t1) );

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CollisionManager::initialize(void)
{
	mpEarthModel	= NULL;
	mpWallModel		= NULL;

	for (int iI = 0; iI < kCHECK_COLLISION_MAX; iI ++) {
		mEarthCollWork[iI].mModelHandle = -1;
		mEarthCollWork[iI].mFrameIndex = -1;
	}
	mEarthCollNum = 0;

	for (int iI = 0; iI < kCHECK_COLLISION_MAX; iI ++) {
		mWallCollWork[iI].mModelHandle = -1;
		mWallCollWork[iI].mFrameIndex = -1;
	}
	mWallCollNum = 0;

	mBodyCollisionVector.clear();
	mAttackCollisionVector.clear();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CollisionManager::entryVector(std::vector<ModelBase *>* pVec, ModelBase* pModel)
{
	std::vector<ModelBase *>::iterator ite;
	for (ite = pVec->begin(); ite != pVec->end(); ite ++) {
		if (*ite == pModel) {
			return;
		}
	}
	pVec->push_back(pModel);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CollisionManager::removeVector(std::vector<ModelBase *>* pVec, ModelBase* pModel)
{
	std::vector<ModelBase *>::iterator ite;
	for (ite = pVec->begin(); ite != pVec->end(); ite ++) {
		if (*ite == pModel) {
			pVec->erase(ite);
			break;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CollisionManager::removeVectorAll(std::vector<ModelBase *>* pVec)
{
	pVec->clear();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
CollisionManager::getVectorNum(std::vector<ModelBase *>* pVec)
{
	return pVec->size();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ModelBase*
CollisionManager::getVectorAddress(std::vector<ModelBase *>* pVec, int modelIndex)
{
	if (pVec->size() == 0) {
		return NULL;
	}
	return pVec->at(modelIndex);
}

 /**** end of file ****/
