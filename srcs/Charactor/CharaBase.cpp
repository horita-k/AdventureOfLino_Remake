/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * プレイヤー管理クラス
 * @author SPATZ.
 * @data   2011/11/27 11:19:40
 */
//---------------------------------------------------
#include <math.h>

#include "CharaBase.h"
#include "AppLib/Basic/Basic.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Effect/EffectManager.h"
#include "../Gimmick/GimmickBase.h"

#include "AppLib/Collision/CollisionManager.h"
#include "DataDefine/MotionDef.h"
#include "DataDefine/VoiceDef.h"
#include "DataDefine/EffectDef.h"
#include "Utility/Utility.h"
#include "CharaInfo.h"

#ifdef _DEBUG
#include "Scene/TotalInfo.h"
#endif // _DEBUG

#include "PersonalChara/CharaEnemyEx.h"

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaBase::CharaBase()
{
	memset(mCharaName, 0x00, sizeof(mCharaName));
	mFlinchMax	 = 0;
	mEarthMaterialIndex = 0;

	// initialize
	initialize();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaBase::~CharaBase()
{
	GraphicManager::GetInstance()->RemoveDrawShadow(&mShadow);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaBase::Create(void)
{
	ModelPmd::Create();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaBase::Setup(bool isDefaultMotioon/*=false*/)
{
	ModelPmd::Setup(isDefaultMotioon);

	initialize();
	mIsActive = true;

	// モデルの大きさから自動的に BodySize と BodyHeight を割り当てる
	setupBodyHeightSize();

	ChangeState(eCHARA_STATE_IDLE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaBase::Update(void)
{
	if (VSize(VSub(mPrevPosition, mPosition)) != 0.0f) { 
		mPrevPosition = mPosition;
	}

	mGravity += mConstGravitySpeed;
	mMoveVec.y += mGravity;

	/*=====================================*
	 * ↓　新コリジョン
	 *=====================================*/
	// 他のオブジェクトとの当たり判定
	updateAllObject();

	// 攻撃の当たり判定
	updateAttackAllObject();

	/*=====================================*
	 * コリジョン関連
	 *=====================================*/
	// 地面との当たり判定
	updateNewEarthCollision();
	// 壁との当たり判定・改
	if (updateSideCollision(false) ) {
		// 地面との当たり判定
		updateNewEarthCollision();
		// 壁との当たり判定・改
		updateSideCollision(true);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaBase::Destroy(void)
{
	ModelPmd::Destroy();
}

//---------------------------------------------------
/**
   リソース関連付け
 */
//---------------------------------------------------
void
CharaBase::LinkCharaResource(const char* charaName)
{
	// キャラの名前を覚えさせておく
	memset(mCharaName, 0x00, sizeof(mCharaName));
	strcpy_s(mCharaName, sizeof(mCharaName), charaName);

	// モデルのリソース関連付け
	ModelBase::LinkModel(charaName);

	// サウンドのリソース関連付け
	
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaBase::EnableShadow(void)
{
	mShadow.EnableShadow();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaBase::DisableShadow(void)
{
	mShadow.DisableShadow();
}

//---------------------------------------------------
/**
   ダメージを食らった時の処理
 */
//---------------------------------------------------
void
CharaBase::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	APP_ASSERT(pDamageDownVec != NULL, "pDamageDownVec is NULL");

	if (isSetMode == false) {
		mHitpoint += -attackPower;
	} else {
		mHitpoint = attackPower;
	}
	mIsDamage = true;

	VECTOR eff_pos;
	if (pEffPos == NULL) {
		eff_pos = mPosition;
		eff_pos.y += mBodyHeight * 0.5f;
	} else {
		eff_pos = *pEffPos;
	}

	const float size = 40.0f;
	int eff_handle = EffectManager::GetInstance()->Play(eEF_HIT,
														&eff_pos,
														&VGet(size, size, size),
														0.0f);
	EffectManager::GetInstance()->Stop(eff_handle, 0.75f);
	
	if (mHitpoint > 0) {
		mFlinchCount += (60 * attackPower);
		if (mFlinchMax <= mFlinchCount) {
			mFlinchCount = 0;
			if (isDamageDown == true) {
				// 吹っ飛ばされ状態へ遷移
				// 吹っ飛ばされに対してキャラの向きを変える
				SetRotationToTarget(&VSub(mPosition, *pDamageDownVec));
				mJumpMoveVec.mDamageDown = *pDamageDownVec;

				mBladeEffect.Stop();
				ChangeState(eCHARA_STATE_DOWN_START);
			} else {
				// ダメージ状態へ遷移
				mBladeEffect.Stop();
				ChangeState(eCHARA_STATE_DAMAGE);
			}
		}
	} else {
		/* 戦闘不能の状態にする */
		mBladeEffect.Stop();

		SetRotationToTarget(&VSub(mPosition, *pDamageDownVec));
		mJumpMoveVec.mDamageDown = *pDamageDownVec;
		
		ChangeState(eCHARA_STATE_RETIRE_START);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaBase::GetBodyCapsule(ShapeCapsule* pCapsule)
{
	pCapsule->mPosA	= mPosition;
	pCapsule->mPosB	= mPosition;
	pCapsule->mPosB.y += mBodyHeight;
	pCapsule->mRadius= mBodySize;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaBase::SetPositionOnCollision(void)
{
	VECTOR vec_start, vec_end, hitpos, normal;

	vec_start	= VAdd(mPosition, VGet(0.0f, (10.0f * ALL_SCALE), 0.0f));
	vec_end		= VAdd(mPosition, mMoveVec);
	
	for (int iI = 0; iI < 5; iI ++) {

		vec_end.y	+= -200.0f;

		if (CollisionManager::GetInstance()->CheckEarthCollision(vec_start, vec_end, &hitpos, &normal) != NOT_ON_COLLISION) {
			
			mPosition = hitpos;
			mNormalVec = normal; // 現在の法線を取得
			mGravity = 0.0f;
			ChangeState(eCHARA_STATE_IDLE);
			mIsLand	= true;

			return;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CharaBase::GetIsRetire(void)
{
	if (mHitpoint > 0) {
		return false;
	}
	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CharaBase::CheckEnableAttack(void)
{
	if ((mHitpoint > 0) && (mIsDamage == false)) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaBase::AttachOcarina(ModelPmd* pAttachModel)
{
	ModelPmd* p_subweapon = pAttachModel;
	p_subweapon->SetVisible(true);		// 常に表示


	MATRIX mtx, tmp;
	// 直接マトリックスを触る
	mtx = MGetIdent();
	// 拡縮行列
	tmp = MGetIdent();
	mtx = MMult(mtx, tmp);

	tmp = MGetRotX(Utility::DegToRad(-30.0f) );
	mtx = MMult(mtx, tmp);

	MATRIX node_mtx;
	GetFrameMatrixFromName(&node_mtx, "左親指２");
	Utility::ExtractRotationMatrix(&tmp, &node_mtx, 1.0f);
	mtx = MMult(mtx, tmp);

	// 移動行列
	VECTOR node_pos;
	Utility::ExtractMatrixPosition(&node_pos, &node_mtx);

	tmp = MGetIdent();
	tmp = MGetTranslate(node_pos);
	mtx = MMult(mtx, tmp);

	p_subweapon->SetMatrixType(ModelBase::eMATRIX_TYPE_MATRIX);
	p_subweapon->Matrix() = mtx;
}

//---------------------------------------------------
/**
 // 攻撃の当たり判定
 */
//---------------------------------------------------
void
CharaBase::updateAttackAllObject(void)
{
	if (mIsAttack == false) return;

	UpdateAttackChara();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaBase::updateNewEarthCollision(void)
{
	VECTOR normal		= ZERO_VEC;
	bool is_wallnormal	= false;
	float dot			= 0.0f;
	int hit_frame_index = 0;

	static float sCapsuleHeight = 1.0f;

	mEarthState = (eEarthState)-1;
	// コリジョン用のラインを用意
	VECTOR vec_start, vec_end, hitpos;
	vec_start	= VAdd(mPosition, VGet(0.0f, ((mBodySize * 0.8f) + sCapsuleHeight), 0.0f));
	vec_end		= VAdd(mPosition, mMoveVec);
	
	/*=====================================*
	 * 各判定処理を行う
	 *=====================================*/
	static const float kSLOPE_NORMAL_VEC = 0.5f;

	INT8 on_coll_index = CollisionManager::GetInstance()->CheckEarthCollisionMulti(vec_start, vec_end, &hitpos, &normal, &hit_frame_index, &mEarthMaterialIndex);

	// ラインとマップの当たり判定を行う
	if ((on_coll_index != NOT_ON_COLLISION) && (normal.y >= kSLOPE_NORMAL_VEC)) { //<! 傾斜判定
		mEarthState = eEARTH_STATE_LINE;
		mOnCollisionIndex = on_coll_index;
	}
	else if ((on_coll_index != NOT_ON_COLLISION) && ((normal.y < kSLOPE_NORMAL_VEC) && (normal.y >= 0.0f))) {
		mEarthState = eEARTH_STATE_SLOPE;
		mOnCollisionIndex = on_coll_index;

	}
	// カプセルとマップとの当たり判定を行う
	else if ((mShadow.IsRenderShadow() == true) && (mGravity <= 0.0f)) {
		normal = mShadow.NormalVec();
		hitpos = VAdd(mPosition, mMoveVec);
		dot = VDot(normal, mMoveVec);

		if ((normal.y < kSLOPE_NORMAL_VEC) && (normal.y >= 0.0f) && (dot < 0.0f)) { 
			mEarthState = eEARTH_STATE_SLOPE;
		} 
		else {
			mEarthState = eEARTH_STATE_CAPSULE;
		}
	}
	else {
		// 空中の場合
		mEarthState = eEARTH_STATE_SKY;
	}

	/* 各ステータスに応じて処理を行う */
	switch (mEarthState) {
	// カプセルの上に乗っている場合 
	case eEARTH_STATE_CAPSULE:

		// 床の場合
		mMoveVec.y = 0.0f;
		mPosition = VAdd(mPosition, mMoveVec);
		
		mNormalVec = mShadow.NormalVec();

		// 『空中』⇒『地面』の場合、着地の遷移へ
		if (mIsLand == false) {
			if (mIsDamage == false) ChangeState(eCHARA_STATE_JUMP_DOWN);
			processLandingFunc();
			mIsLand = true;
		}
		mGravity = 0.0f;

		break;

	// ラインの上に乗っている場合
	case eEARTH_STATE_LINE:
		// ラインの当たった場所に設定させる
		mPosition = hitpos;
		mNormalVec = normal;
		mMoveVec.y = 0.0f;
		
		// 『空中』⇒『地面』の場合、着地の遷移へ
		if (mIsLand == false) {
			if (mIsDamage == false) ChangeState(eCHARA_STATE_JUMP_DOWN);
			processLandingFunc();
			mIsLand = true;
		}
		mGravity = 0.0f;

		break;

	// ラインの上かつ傾斜の場合
	case eEARTH_STATE_SLOPE:
		{
			float slope_speed = (mGravity * (1.0f - normal.y));
			VECTOR side_cross;
			VECTOR slope_vec;
			VECTOR new_pos;
			side_cross = VCross(normal, sUpVec);
			slope_vec = VCross(side_cross, normal);
			
			// ベクトルの大きさを調整
			APP_ASSERT((VSize(slope_vec) > 0.0f),
					   "slope_vec is Invalid value \n");
			slope_vec = VScale(VNorm(slope_vec), slope_speed);


			// 新しい場所はラインの当たった場所 ＋ slope_vec 
			new_pos = VAdd(hitpos, slope_vec);
			mNormalVec = normal;

			if (CollisionManager::GetInstance()->CheckEarthCollisionMulti(hitpos, new_pos, &hitpos, &normal, &hit_frame_index, &mEarthMaterialIndex) != NOT_ON_COLLISION) {
				new_pos = hitpos;
				mNormalVec = normal;
			}

			// 流れに押し戻される
			dot = VDot(slope_vec, VSub(new_pos, mPosition));
			PRINT_CONSOLE("dot = %.2f \n", dot);

			if (dot > 0.0f) {
				if (mCharaState != eCHARA_STATE_SLOPE_DOWN) {
					if (mIsDamage == false) ChangeState(eCHARA_STATE_SLOPE_DOWN);				
				}
			}
			mPosition = new_pos;		// 移動座標を更新
			mMoveVec.y = 0.0f;
			mIsLand = true;
		}
		break;

	// 空中の場合の処理
	case eEARTH_STATE_SKY:
		mPosition = VAdd(mPosition, mMoveVec);

		mNormalVec = VGet(0.0f, 1.0f, 0.0f);

		if (mIsLand == true) {
			if (mGravity < kGRAVITY_SPEED * 16.0f) { 
				if (mIsDamage == false) ChangeState(eCHARA_STATE_JUMP_DOWN);
				mIsLand = false;
			}
		}
		break;

	default:
		break;
	};

	if ((mEarthState == eEARTH_STATE_LINE) ||
		(mEarthState == eEARTH_STATE_CAPSULE)) {

		GimmickBase* p_gimmick = NULL;
		p_gimmick = (GimmickBase *)GimmickBase::GetGimmickBaseFromEarthCollIndex(mOnCollisionIndex);
		if (p_gimmick != NULL) {
			if (normal.y >= kSLOPE_NORMAL_VEC) { //<! 傾斜判定
				p_gimmick->SetCharaPositionToFrameMatrix(this, hit_frame_index);
			}
			else {
				mPosition = VAdd(mPosition, VScale(normal, 10.0f) );
			}
#ifdef _DEBUG
			PRINT_SCREEN(GetColor(0, 0, 0), "gimmick normal vec = {%.2f, %.2f, %.2f } ", normal.x, normal.y, normal.z);
			RENDER_LINE(&mPosition,
						&VAdd(mPosition, VScale(normal, 500.0f)), 
						COLOR_GREEN);
			
#endif // _DEBUG
			mIsOnGimmick = TRUE;
		}
		else {
			mIsOnGimmick = FALSE;
		}
	}
}

//---------------------------------------------------
/**
	// 壁との当たり判定・改
 */
//---------------------------------------------------
bool
CharaBase::updateSideCollision(bool is2nd)
{
	static float sDiffPos = (20.0f * ALL_SCALE);
	ShapeCapsule capsule;

	/*=====================================*
	 *こっからキャラの当たり判定
	 *=====================================*/
	capsule.mRadius = mBodySize * 0.8f;

	// 移動量が多い場合はカプセルのサイズを大きくする
	VECTOR tmp_vec = mMoveVec;
	tmp_vec.y = 0.0f;
	float tmp_speed = VSize(tmp_vec);
	if (capsule.mRadius < tmp_speed) {
		capsule.mRadius = tmp_speed;
	}

	capsule.mPosA = mPosition;
	capsule.mPosA.y += (mBodySize + sDiffPos);
	capsule.mPosB = mPosition;
	capsule.mPosB.y += (mBodyHeight - capsule.mRadius + mMoveVec.y);

	VECTOR normal;

	if (CollisionManager::GetInstance()->CheckSideCollision(&capsule, &mMoveVec, &normal)) {
 		if (mIsLand == true) {

			/* 地面上で壁に当たった場合の処理 */
			if (is2nd == true) {
				mPosition = mPrevPosition;
				mMoveVec = ZERO_VEC;
			}
			else {
				// 壁ずりベクトルを求める
				normal = VNorm(normal);
				mMoveVec = VScale(VSub(mMoveVec, VScale(normal, VDot(mMoveVec, normal)) ), 0.8f);
				mPosition = mPrevPosition;
			}
			mGravity = 0.0f;
			mJumpMoveVec.mDash = ZERO_VEC;

			return true;
		}
		else {
			/* 空中で壁に当たった場合の処理 */
			/* 頭に当たったと判定 */
			if (normal.y < -0.9f) {
				if (mCharaState != eCHARA_STATE_JUMP_DOWN) {
					mPosition.x = mPrevPosition.x;
					mPosition.z = mPrevPosition.z;
					mMoveVec = ZERO_VEC;
					mJumpMoveVec.mDash = ZERO_VEC;
					mNormalVec = sUpVec;
					mGravity = 0.0f;
					ChangeState(eCHARA_STATE_JUMP_DOWN);
				}
			}
			else if (normal.y < -0.1) {
				if (mCharaState != eCHARA_STATE_FALL_DOWN) {
					mPosition.x = mPrevPosition.x;
					mPosition.z = mPrevPosition.z;
					mMoveVec = ZERO_VEC;
					mJumpMoveVec.mDash = ZERO_VEC;
					mNormalVec = sUpVec;
					mGravity = 0.0f;
					ChangeState(eCHARA_STATE_FALL_DOWN);
				}
			}
			else {
				/* 横に当たったと判定 */
				mPosition.x = mPrevPosition.x;
				mPosition.z = mPrevPosition.z;
				mMoveVec.x = 0.0f;
				mMoveVec.z = 0.0f;
				mJumpMoveVec.mDash.x = 0.0f;
				mJumpMoveVec.mDash.z = 0.0f;
				mNormalVec = sUpVec;
			}
		}
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaBase::eTurnType
CharaBase::updateTargetRotateFromPad(void)
{
	eTurnType turn_type = eTURN_TYPE_NONE;
	VECTOR vec;
	vec = VSub(CameraManager::GetInstance()->Target(), CameraManager::GetInstance()->Position());
	float cam_angle = atan2(vec.x, vec.z);

	if (CHECK_PRESS_JOY(JOY_INPUT_UP)) {
		if (CHECK_PRESS_JOY(JOY_INPUT_LEFT)) {
			mTargetRotY = cam_angle + Utility::DegToRad(135.0f);
			turn_type = eTURN_TYPE_UP_LEFT;
		} else if (CHECK_PRESS_JOY(JOY_INPUT_RIGHT)) {
			mTargetRotY = cam_angle + Utility::DegToRad(225.0f);
			turn_type = eTURN_TYPE_UP_RIGHT;
		} else {
			mTargetRotY = cam_angle + Utility::DegToRad(180.0f);
			turn_type = eTURN_TYPE_UP;
		}
	} else if (CHECK_PRESS_JOY(JOY_INPUT_DOWN)) {
		if (CHECK_PRESS_JOY(JOY_INPUT_LEFT)) {
			mTargetRotY = cam_angle + Utility::DegToRad(45.0f);
			turn_type = eTURN_TYPE_DOWN_LEFT;
		} else if (CHECK_PRESS_JOY(JOY_INPUT_RIGHT)) {
			mTargetRotY = cam_angle + Utility::DegToRad(315.0f);
			turn_type = eTURN_TYPE_DOWN_RIGHT;
		} else {
			mTargetRotY = cam_angle + Utility::DegToRad(0.0f);
			turn_type = eTURN_TYPE_DOWN;
		}
	} else if (CHECK_PRESS_JOY(JOY_INPUT_LEFT)) {
		mTargetRotY = cam_angle + Utility::DegToRad(90.0f);
		turn_type = eTURN_TYPE_LEFT;
	} else if (CHECK_PRESS_JOY(JOY_INPUT_RIGHT)) {
		mTargetRotY = cam_angle + Utility::DegToRad(270.0f);
		turn_type = eTURN_TYPE_RIGHT;
	}

	return turn_type;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaBase::changeStateStatus(void)
{
	mIsAttack = false;
	mIsDamage = false;
	mIsReserveComboInput = false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaBase::AddHitPoint(int addValue)
{
	mHitpoint += addValue;
	if (mHitpoint > mMaxHitpoint)	mHitpoint = mMaxHitpoint;
	if (mHitpoint < 0)				mHitpoint = 0;
}

//---------------------------------------------------
/**
   
 */
//---------------------------------------------------
void
CharaBase::initialize(void)
{
	mRunSpeed	= kCHARA_RUN_SPEED;
	mCharaState = 0;
	mGravity	= 0.0f;
	mConstGravitySpeed = kGRAVITY_SPEED;
	mNormalVec	= ZERO_VEC;
	mTargetRotY	= 0.0f;
	mTurnType	= eTURN_TYPE_UP;
	mMoveVec	= ZERO_VEC;
	mJumpMoveVec.mDash = ZERO_VEC;
	mIsActive	= false;
	mIsLand		= false;
	mIsSlide	= false;
	mIsAttack	= false;
	mIsDamage	= false;
	mMaxHitpoint= 0;
	mHitpoint	= 0;
	mAttackPower= 0;
	mIsReserveComboInput	= false;
	mIsOnGimmick=FALSE;
	mFlinchCount = 0;
	mPrevCliffVec= ZERO_VEC;
	//	mFlinchMax	 = 0.0f;	// maxはここでは初期化しない
	mLastEarthPos			= ZERO_VEC;
	mEarthState = (eEarthState)-1;
	mOnCollisionIndex = -1;
}

/**** end of file ****/

