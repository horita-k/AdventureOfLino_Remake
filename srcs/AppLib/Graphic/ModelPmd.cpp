/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * PMDモデルクラス
 * @author SPATZ.
 * @data   2011/11/20 23:08:14
 */
//---------------------------------------------------
#include "AppLib/Basic/Basic.h"
#include "ModelPmd.h"
#include "Scene/TotalInfo.h"
#include "Utility/Utility.h"

#ifdef _DEBUG
#include "AppLib/Graphic/GraphicManager.h"
#endif // _DEBUG

/*=====================================*
 * static
 *=====================================*/
/*static*/
ModelPmd::PHASE_ANIMATION
ModelPmd::mPhaseAnimation[ModelPmd::eANIMATION_TYPE_MAX] = {
	&ModelPmd::phaseAnimationMotion,
	&ModelPmd::phaseAnimationMorphing,
	&ModelPmd::phaseAnimationNone,
};

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ModelPmd::ModelPmd()
{
	// initialize
	memset(&mMotion, 0x00, sizeof(mMotion));
	mMotion.mpHandle	= NULL;
	mMotion.mIndex		= -1;
	mMotion.mPrevIndex	= -1;
	mMotion.mSpeed		= 0.5f;
	mMotion.mPauseSpeed = 0.0f;
	mMotion.mBlendSpeed = 0.0f;

	this->DeattachModelAll();

	mAnimationType = eANIMATION_TYPE_MOTION;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ModelPmd::~ModelPmd()
{
	Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::Create(void)
{
	ModelBase::Create();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::Setup(bool isDefaultMotioon/*=false*/)
{
	//	if (mIsSetup) return;

	ModelBase::Setup();
	mMotion.mIndex = -1;

	// アニメーションタイプをモーションに初期設定
	mAnimationType = eANIMATION_TYPE_MOTION;

	int anim_num = MV1GetAnimNum(mModelHandle);
	if (anim_num == 0) {
		// アニメーションなしモデルとして適用する
		mAnimationType = eANIMATION_TYPE_NONE;
	} else {
		if (mMotion.mpHandle == NULL) {
			mMotion.mpHandle = new int[anim_num];
			APP_ASSERT( (mMotion.mpHandle != NULL),
						"Failed new mMotion.mpHandle \n");

			for (int iI = 0; iI < anim_num; iI ++) {
				mMotion.mpHandle[iI] = MV1AttachAnim(mModelHandle, iI, -1, FALSE);
				if (mMotion.mpHandle[iI] == -1) {
					PRINT_LOG_PRINT("Attach Error ");
				}
				PRINT_LOG_PRINT("MV1SetAttachAnimBlendRate = {%d, mpHandle[%d] = %d ", mModelHandle, iI, mMotion.mpHandle[iI]);
				MV1SetAttachAnimBlendRate(mModelHandle, mMotion.mpHandle[iI], 0.0f);
			}
		}
	}

	// 	メッシュの頂点ディフューズカラーをマテリアルのディフューズカラーの代わりに使用するかどうかを設定する
	MV1SetUseVertDifColor( mModelHandle, FALSE);
	MV1SetUseVertSpcColor( mModelHandle, TRUE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::Update(void)
{
	ModelBase::Update();

	APP_ASSERT((((unsigned)mAnimationType) < eANIMATION_TYPE_MAX),
			   "Invalid mAnimationType \n");
	(this->*mPhaseAnimation[mAnimationType])();

	// アタッチメントの処理
	for (char iI = 0; iI < kATTACH_MODEL_NUM; iI ++) {
		if (mAttach[iI].mIsActive == TRUE) {
			MATRIX mtx = MV1GetFrameLocalWorldMatrix(mModelHandle, mAttach[iI].mFrameIndex);
			mAttach[iI].mpAttachedModel->Matrix() = mtx;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::Destroy(void)
{
	ModelBase::Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::DeleteModel(void)
{
	int ret = 0;

	if (mModelHandle != -1) {
		int anim_num = MV1GetAnimNum(mModelHandle);
		if (mMotion.mpHandle != NULL) {
			for (int iI = 0; iI < anim_num; iI ++) {
				ret = MV1DetachAnim(mModelHandle, mMotion.mpHandle[iI]);
				APP_ASSERT(ret != -1, 
						   "Failed MV1DetachAnim()\n");
				mMotion.mpHandle[iI] = -1;
			}
		}
	}

	if (mMotion.mpHandle != NULL) {
		delete[] mMotion.mpHandle;
		mMotion.mpHandle = NULL;
	}
	
	mAnimationType = eANIMATION_TYPE_NONE;

	ModelBase::DeleteModel();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::SetupMorphType(float startBlend)
{
	mMotion.mBlendRate = startBlend;

	// モーフィングに設定
	mAnimationType = eANIMATION_TYPE_MORPHING;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::StartMorph(int animIndexA, int animIndexB, float blendSpeed)
{
	if (blendSpeed == mMotion.mBlendSpeed) return;

	APP_ASSERT(mModelHandle != -1,
			   "ModelPmd::mModelHandle is -1\n");

	mMotion.mPrevIndex	= (SHORT)animIndexA;
	mMotion.mIndex		= (SHORT)animIndexB;
	mMotion.mBlendSpeed	= blendSpeed;

	mMotion.mPrevBlendRate		= 0.0f;
	mMotion.mNowTime			= 0.0f;
	mMotion.mTotalTime			= 0.0f;
	mMotion.mIsLoop				= false;
	mMotion.mSpeed				= 0.0f;
	mMotion.mIsEnd				= false;
	mMotion.mIsBlend			= true;
	mMotion.mPauseSpeed			= 0.0f;
	mMotion.mpNextMotion		= NULL;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::StartMotion(int animIndex, bool isLoop, float animSpeed, const tMotionParam* pNextMotion/*=NULL*/, float blendSpeed/*=0.1f*/)
{
	if (mMotion.mIndex == animIndex) return;
	if (animIndex >= MV1GetAnimNum(mModelHandle) ) return;
	
	StartMotionForcibly(animIndex, isLoop, animSpeed, pNextMotion, blendSpeed);
}

//---------------------------------------------------
/**
   同じモーションで強制的に再生させる
 */
//---------------------------------------------------
void
ModelPmd::StartMotionForcibly(int animIndex, bool isLoop, float animSpeed, const tMotionParam* pNextMotion/*=NULL*/, float blendSpeed/*=0.1f*/)
{
	APP_ASSERT(mModelHandle != -1,
			   "ModelPmd::mModelHandle is -1\n");
	APP_ASSERT( (mMotion.mpHandle != NULL),
				"mMotion.mpHandle is NULL \n");
	MV1SetAttachAnimBlendRate(mModelHandle, mMotion.mpHandle[mMotion.mPrevIndex], 0.0f);

	mMotion.mPrevBlendRate	= mMotion.mBlendRate;
	mMotion.mPrevIndex		= mMotion.mIndex;

	mMotion.mBlendRate	= 0.0f;
	mMotion.mIndex		= animIndex;
	mMotion.mNowTime	= 0.0f;
	mMotion.mTotalTime	= MV1GetAttachAnimTotalTime(mModelHandle, mMotion.mpHandle[mMotion.mIndex]);
	mMotion.mIsLoop		= isLoop;
	mMotion.mSpeed		= animSpeed;
	mMotion.mBlendSpeed	= blendSpeed;
	mMotion.mIsEnd		= false;
	mMotion.mIsBlend	= true;
	mMotion.mPauseSpeed	= 0.0f;
	mMotion.mpNextMotion= (tMotionParam *)pNextMotion;
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::StartMotionEx(const tMotionParam* pParam)
{
	StartMotion(pParam->mIndex, pParam->mIsLoop, pParam->mSpeed, pParam->mpNextMotion);
};

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::PauseMotion(void)
{
	mMotion.mPauseSpeed = mMotion.mSpeed;
	mMotion.mSpeed = 0.0f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::ResumeMotion(void)
{
	mMotion.mSpeed = mMotion.mPauseSpeed;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::ReplayMotion(void)
{
	mMotion.mNowTime	= 0.0f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ModelPmd::GetIsEndMotion(void)
{
	if (mMotion.mNowTime >= mMotion.mTotalTime) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
ModelPmd::GetMotionTimePercent(void)
{
	return (int)((mMotion.mNowTime / mMotion.mTotalTime) * 100.0f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::AttachModel(ModelBase* pAttachedModel, eAttachedModelType attachedModelType, int frameIndex)
{
	// 既に登録されているかをチェック
	for (char iI = 0; iI < kATTACH_MODEL_NUM; iI ++) {
		if (mAttach[iI].mpAttachedModel == pAttachedModel) {
			// 既に登録されているモデルの場合は処理を抜ける
			return;
		}
	}
	for (char iI = 0; iI < kATTACH_MODEL_NUM; iI ++) {
		if (mAttach[iI].mpAttachedModel == NULL) {
			pAttachedModel->SetMatrixType(ModelBase::eMATRIX_TYPE_MATRIX);

			mAttach[iI].mIsActive			= TRUE;
			mAttach[iI].mpAttachedModel		= pAttachedModel;
			mAttach[iI].mAttachedModelType	= attachedModelType;
			mAttach[iI].mFrameIndex			= frameIndex;
			mAttachCount ++;
			break;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::ChangeAttachModelFrameIndex(int attachIndex, int frameIndex)
{
	APP_ASSERT_PRINT( (attachIndex < kATTACH_MODEL_NUM),
					  "Invalid attachIndex %d \n", attachIndex);
	APP_ASSERT_PRINT( (frameIndex > 0),
					  "Invalid frameIndex %d \n", frameIndex);

	mAttach[attachIndex].mFrameIndex = frameIndex;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::ChangeAttachModelPtr(int attachIndex, ModelBase* pModel)
{
	APP_ASSERT_PRINT( (attachIndex < kATTACH_MODEL_NUM),
					  "Invalid attachIndex %d \n", attachIndex);
	
	mAttach[attachIndex].mpAttachedModel = pModel;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::DeattachModelAll(void)
{
	for (char iI = 0; iI < kATTACH_MODEL_NUM; iI ++) {
		mAttach[iI].mIsActive			= FALSE;
		mAttach[iI].mpAttachedModel		= NULL;
		mAttach[iI].mFrameIndex			= -1;
	}
	mAttachCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ModelPmd::EqualPhaseFrame(float frame)
{
	return ( ( (frame >= mMotion.mNowTime) && (frame < mMotion.mNowTime + mMotion.mSpeed)) ? true : false);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ModelPmd::BetweenPhaseFrame(float frameMin, float frameMax)
{
	return (( ( (frameMin) <= mMotion.mNowTime) && (frameMax > mMotion.mNowTime) ) ? true : false);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::PauseAttach(int attachIndex)
{
	mAttach[attachIndex].mIsActive = FALSE;
	mAttach[attachIndex].mpAttachedModel->SetMatrixType(ModelBase::eMATRIX_TYPE_TRS);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::ResumeAttach(int attachIndex)
{
	mAttach[attachIndex].mIsActive = true;
	mAttach[attachIndex].mpAttachedModel->SetMatrixType(ModelBase::eMATRIX_TYPE_MATRIX);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
ModelPmd::GetFrameIndexFromName(const char* frameName)
{
	return MV1SearchFrame(mModelHandle, frameName);
}

//---------------------------------------------------
/**
 * 指定フレーム名の行列取得
 */
//---------------------------------------------------
bool
ModelPmd::GetFrameMatrixFromName(MATRIX* pOut, const char* frameName)
{
	if (pOut == NULL) return false;

	int frame_index = this->GetFrameIndexFromName(frameName);

	if (frame_index >= 0) {
		MATRIX mtx = MV1GetFrameLocalWorldMatrix(mModelHandle, frame_index);
		*pOut = mtx;
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
 * 指定フレーム名の行列取得
 */
//---------------------------------------------------
bool
ModelPmd::GetFrameMatrixFromNameByScale(MATRIX* pOut, const char* frameName, float setScale)
{
	if (pOut == NULL) return false;
	
	int frame_index = this->GetFrameIndexFromName(frameName);

	if (frame_index >= 0) {
		MATRIX node_mtx = MV1GetFrameLocalWorldMatrix(mModelHandle, frame_index);

		MATRIX tmp;
		
		*pOut = MGetIdent();

		// 拡大縮小行列
		tmp = MGetScale(VGet(setScale, setScale, setScale));
		*pOut = MMult(*pOut, tmp);

		// 回転行列
		Utility::ExtractRotationMatrix(&tmp, &node_mtx, 1.0f);
		*pOut = MMult(*pOut, tmp);

		// 移動行列
		VECTOR trans_pos;
		trans_pos.x = node_mtx.m[3][0];
		trans_pos.y = node_mtx.m[3][1];
		trans_pos.z = node_mtx.m[3][2];

		tmp = MGetTranslate(trans_pos);
		*pOut = MMult(*pOut, tmp);

		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ModelPmd::GetFrameMatrixFromIndex(MATRIX* pOut, BYTE frameIndex)
{
	MATRIX mtx = MV1GetFrameLocalWorldMatrix(mModelHandle, (int)frameIndex);
	*pOut = mtx;

	return true;
}

//---------------------------------------------------
/**
   指定フレーム名の座標取得
 */
//---------------------------------------------------
bool
ModelPmd::GetFramePosFromName(VECTOR* pOut, const char* frameName)
{
	if (pOut == NULL) return false;

	MATRIX mtx;
	bool ret;

	ret = this->GetFrameMatrixFromName(&mtx, frameName);
	PRINT_ASSERT(ret == true,
				 "Failed GetFramePosFromName : %s\n", frameName);
	pOut->x = mtx.m[3][0];
	pOut->y = mtx.m[3][1];
	pOut->z = mtx.m[3][2];
	
	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ModelPmd::GetFramePosFromIndex(VECTOR* pOut, BYTE frameIndex)
{
	if (pOut == NULL) return false;

	MATRIX mtx;

	this->GetFrameMatrixFromIndex(&mtx, frameIndex);
	pOut->x = mtx.m[3][0];
	pOut->y = mtx.m[3][1];
	pOut->z = mtx.m[3][2];
	
	return true;
}

//---------------------------------------------------
/**
 * 指定のフレーム（ノード）の行列に個別のパラメータを設定する
 */
//---------------------------------------------------
void
ModelPmd::SetFrameUserMatrix(const char* frameName, VECTOR* pTrans, VECTOR* pRotPos, VECTOR* pRotTar, VECTOR* pScale)
{
	int frame_index = 0;
	MATRIX mtx, tmp;

	/**** 指定のフレームを指定のマトリックスの場所に設定させる ****/
	// 指定のマトリックス mtx を生成する
	mtx = MGetIdent();
	/* 拡縮行列の計算 */
	tmp = MGetIdent();
	tmp = MGetScale(VGet(pScale->x, pScale->y, pScale->z));
	mtx = MMult(mtx, tmp);

	/* 回転行列の計算 */
	tmp = MGetIdent();
	Utility::MTXLookAtMatrix(&tmp, pRotTar, pRotPos, &VGet(0.0f, 1.0f, 0.0f));
	mtx = MMult(mtx, tmp);

	/* 移動行列の計算 */
	tmp = MGetIdent();
	tmp = MGetTranslate(*pTrans);
	mtx = MMult(mtx, tmp);

	/* 求めた行列はワールド行列の値だが、設定する関数はローカル変数での指定の為、
	   ワールド行列の逆行列をかけてローカル行列に反映させてから設定する */
	MATRIX world_mtx, inv_mtx;
	world_mtx = MV1GetFrameLocalWorldMatrix(mModelHandle, 0);
	CreateInverseMatrix(&inv_mtx, &world_mtx);
	mtx = MMult(mtx, inv_mtx);	// ワールドからローカルへ変換

	// 指定のフレームに対してローカル行列を設定する
	frame_index = this->GetFrameIndexFromName(frameName);
	MV1SetFrameUserLocalMatrix(mModelHandle, frame_index, mtx);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::SetFrameUserRotation(const char* frameName, VECTOR* pRotTar, float sensitiveRate)
{
	SetFrameUserRotation(frameName, pRotTar, &VGet(0.0f, 1.0f, 0.0f), sensitiveRate, NULL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::SetFrameUserRotation(const char* frameName, VECTOR* pRotTar, VECTOR* pUpVec, float sensitiveRate, MATRIX* pOutParentInvMtx/*=NULL*/)
{
	int frame_index = 0;
	MATRIX target_mtx, tmp, world, set_mtx;
	VECTOR rot_pos;
	MATRIX world_mtx, inv_mtx;

	frame_index = this->GetFrameIndexFromName(frameName);

	MATRIX local_mtx;
	local_mtx = MV1GetFrameLocalMatrix(mModelHandle, frame_index);

	// 指定のマトリックス mtx を生成する
	target_mtx = MGetIdent();
	/* 拡縮行列の計算 */
	tmp = MGetIdent();
	tmp = MGetScale(VGet(mScale.x, mScale.y, mScale.z));
	target_mtx = MMult(target_mtx, tmp);

	/* 回転行列の計算 */
	tmp = MGetIdent();
	world = MV1GetFrameLocalWorldMatrix(mModelHandle, frame_index);
	Utility::ExtractMatrixPosition(&rot_pos, &world);

	// 回転行列の計算
	Utility::MTXLookAtMatrix(&tmp, pRotTar, &rot_pos, pUpVec);
	/* 求めた行列はワールド行列の値だが、設定する関数はローカル変数での指定の為、
	   ワールド行列の逆行列をかけてローカル行列に反映させてから設定する */
	world_mtx = MV1GetFrameLocalWorldMatrix(mModelHandle, 0);
	CreateInverseMatrix(&inv_mtx, &world_mtx);
	tmp = MMult(tmp, inv_mtx);	// ワールドからローカルへ変換
	target_mtx = MMult(target_mtx, tmp);

	if (pOutParentInvMtx != NULL) {
		*pOutParentInvMtx = inv_mtx;
	}

	/* 移動行列の計算 */
	target_mtx.m[3][0] = local_mtx.m[3][0];
	target_mtx.m[3][1] = local_mtx.m[3][1];
	target_mtx.m[3][2] = local_mtx.m[3][2];

	set_mtx = MAdd(MScale(target_mtx,sensitiveRate * 2.0f) , 
				   MScale(local_mtx, (1.0f - sensitiveRate) * 2.0f));
	set_mtx = MScale(set_mtx, 0.5f);

	// 指定のフレームに対してローカル行列を設定する
	MV1SetFrameUserLocalMatrix(mModelHandle, frame_index, set_mtx);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::SetFrameUserRotationEx(int childFrameIndex, VECTOR& rTargetRot, VECTOR& rUpVec, float sensitiveRate, MATRIX& rInvMatrix)
{
	// 親のフレーム番号を取得
	int parent_frame_index = MV1GetFrameParent(mModelHandle, childFrameIndex);
	
	// 親と子のノード間の距離を求める
	float node_length = 30.0f;

	// 親行列の情報を取得
	MATRIX parent_world_mtx, parent_local_mtx;
	parent_world_mtx = MV1GetFrameLocalWorldMatrix(mModelHandle, parent_frame_index);
	parent_local_mtx = MV1GetFrameLocalMatrix(mModelHandle, parent_frame_index);
	
	// 子のワールド行列を親行列の情報から計算して求める
	VECTOR child_world_pos;
	{
		// ワールド座標で親の移動先の座標を求める
		VECTOR parent_world_pos;
		Utility::ExtractMatrixPosition(&parent_world_pos, &parent_world_mtx);

		// ワールドの親ノードの回転ベクトルを求める
		VECTOR parent_world_rot;
		parent_world_rot = VNorm(VTransformSR(VGet(0.0f, 0.0f, 1.0f), parent_world_mtx) );
		
		// 子ノードの位置を求める
		child_world_pos = VAdd(parent_world_pos, 
							   VScale(parent_world_rot, (node_length * mScale.x) ) );
	}
	
	//<! 子のワールド行列を求める
	MATRIX child_world_mtx, tmp_mtx;
	{
		child_world_mtx = MGetIdent();

		// 拡縮
		tmp_mtx = MGetScale(VGet(mScale.x, mScale.y, mScale.z) );
		child_world_mtx = MMult(child_world_mtx, tmp_mtx);

		// 回転
		Utility::MTXLookAtMatrix(&tmp_mtx, &rTargetRot, &child_world_pos, &rUpVec);
		child_world_mtx = MMult(child_world_mtx, tmp_mtx);

		// 移動
		tmp_mtx = MGetTranslate(child_world_pos);
		child_world_mtx = MMult(child_world_mtx, tmp_mtx);
	}

	// 子のローカル行列を求める
	MATRIX child_local_mtx;
	{
		// 計算上、一度ローカル座標にワールド値を設定する
		child_local_mtx = child_world_mtx;

		MATRIX root_mtx, inv_mtx;
		MATRIX calc_mtx;
		calc_mtx = rInvMatrix;
		
		// 親のローカル行列の逆行列を求め、かける
		root_mtx = MV1GetFrameLocalMatrix(mModelHandle, parent_frame_index);
		CreateInverseMatrix(&inv_mtx, &root_mtx);
		calc_mtx = MMult(calc_mtx, inv_mtx);

		child_local_mtx = MMult(child_local_mtx, calc_mtx);

		rInvMatrix = calc_mtx;
	}
	
	MATRIX set_mtx, org_local_mtx;
	{
		org_local_mtx = MV1GetFrameLocalMatrix(mModelHandle, childFrameIndex); 
		
		set_mtx = MAdd(MScale(child_local_mtx, sensitiveRate * 2.0f),
					   MScale(org_local_mtx, (1.0f - sensitiveRate) * 2.0f) );
		set_mtx = MScale(set_mtx, 0.5f);
	}

	MV1SetFrameUserLocalMatrix(mModelHandle, childFrameIndex, set_mtx);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::ResetFrameUserMatrix(const char* frameName)
{
	int frame_index = 0;
	frame_index = this->GetFrameIndexFromName(frameName);
	
	MV1ResetFrameUserLocalMatrix(mModelHandle, frame_index);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::ResetMotion(void)
{
	mMotion.mIndex = 0;
	mMotion.mPrevIndex = 1;
	mMotion.mIsBlend = true;

	mMotion.mBlendRate = 0.1f;
	mMotion.mSpeed = 0.5f;

	int anim_num = MV1GetAnimNum(mModelHandle);
	for (int iI = 0; iI < anim_num; iI ++) {
		MV1SetAttachAnimBlendRate(mModelHandle, mMotion.mpHandle[iI], 0);
	}
	MV1SetAttachAnimBlendRate(mModelHandle, mMotion.mpHandle[mMotion.mIndex], 1);
	MV1SetAttachAnimTime(mModelHandle, 
						 mMotion.mpHandle[mMotion.mIndex], 
						 0);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
ModelPmd::GetAttachedNum(void)
{
	return mAttachCount;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ModelPmd::tAttachWork*
ModelPmd::GetAttachAddr(int attachedIndex)
{
	return &mAttach[attachedIndex];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelPmd::DumpParam(void)
{
	PRINT_LOG_PRINT("mMotion.mPrevBlendRate = %.2f", mMotion.mPrevBlendRate);
	PRINT_LOG_PRINT("mMotion.mPrevIndex = %d ", mMotion.mPrevIndex);
	PRINT_LOG_PRINT("mMotion.mBlendRate = %.2f ",mMotion.mBlendRate );
	PRINT_LOG_PRINT("mMotion.mIndex = %d ", mMotion.mIndex);
	PRINT_LOG_PRINT("mMotion.mNowTime = %.2f ", mMotion.mNowTime);
	PRINT_LOG_PRINT("mMotion.mTotalTime = %.2f ", mMotion.mTotalTime);
	PRINT_LOG_PRINT("mMotion.mIsLoop = %d ", mMotion.mIsLoop);
	PRINT_LOG_PRINT("mMotion.mSpeed = %.2f ", mMotion.mSpeed);
	PRINT_LOG_PRINT("mMotion.mBlendSpeed = %.2f ", mMotion.mBlendSpeed);
	PRINT_LOG_PRINT("mMotion.mIsEnd = %d ", mMotion.mIsEnd);
	PRINT_LOG_PRINT("mMotion.mIsBlend = %d ", mMotion.mIsBlend);
	PRINT_LOG_PRINT("mMotion.mPauseSpeed = %.2f ", mMotion.mPauseSpeed);
	PRINT_LOG_PRINT("mMotion.mpNextMotion = 0x%x ", (unsigned long)mMotion.mpNextMotion);
}

//---------------------------------------------------
/**
 * モーションのアニメーションを処理
 */
//---------------------------------------------------
void
ModelPmd::phaseAnimationMotion(void)
{
	APP_ASSERT((mModelHandle != -1), "mModelHandle is Invalid Value \n");

	// ブレンド関連
	if (mMotion.mIsBlend) {

		mMotion.mBlendRate += mMotion.mBlendSpeed;
		if (mMotion.mBlendRate >= 1.0f) {
			mMotion.mBlendRate = 1.0f;
			mMotion.mIsBlend = false;
		}

		MV1SetAttachAnimBlendRate(mModelHandle, mMotion.mpHandle[mMotion.mIndex],		mMotion.mBlendRate);
		MV1SetAttachAnimBlendRate(mModelHandle, mMotion.mpHandle[mMotion.mPrevIndex],	(1.0f - mMotion.mBlendRate) );
	}

	// アニメーション時間関連
	mMotion.mNowTime += mMotion.mSpeed;
	if (mMotion.mNowTime >= mMotion.mTotalTime) {
		if (mMotion.mIsLoop) {	// ループの場合
			mMotion.mNowTime = mMotion.mNowTime - mMotion.mTotalTime;
		} else {				// ループ以外
			if (mMotion.mpNextMotion != NULL) {
				StartMotionEx(mMotion.mpNextMotion);
			} else {
				mMotion.mNowTime = mMotion.mTotalTime;
				mMotion.mIsEnd = true;
			}
		}
	}
	MV1SetAttachAnimTime(mModelHandle, mMotion.mpHandle[mMotion.mIndex], mMotion.mNowTime);
}

//---------------------------------------------------
/**
 * モーションのアニメーションを処理
 */
//---------------------------------------------------
void
ModelPmd::phaseAnimationMorphing(void)
{
	APP_ASSERT((mModelHandle != -1), "mModelHandle is Invalid Value \n");

	// ブレンド関連
	if (mMotion.mIsBlend) {

		mMotion.mBlendRate += mMotion.mBlendSpeed;
		if (mMotion.mBlendRate >= 1.0f) {
			mMotion.mBlendRate = 1.0f;
			mMotion.mIsBlend = false;
		} else if (mMotion.mBlendRate <= 0.0f) {
			mMotion.mBlendRate = 0.0f;
			mMotion.mIsBlend = false;
		}

		MV1SetAttachAnimBlendRate(mModelHandle, mMotion.mpHandle[mMotion.mIndex], mMotion.mBlendRate);
		MV1SetAttachAnimBlendRate(mModelHandle, mMotion.mpHandle[mMotion.mPrevIndex], 1.0f - mMotion.mBlendRate);
	}
}

//---------------------------------------------------
/**
 * モーションのアニメーションを処理
 */
//---------------------------------------------------
void
ModelPmd::phaseAnimationNone(void)
{
	return; /* nop */
}

 /**** end of file ****/


