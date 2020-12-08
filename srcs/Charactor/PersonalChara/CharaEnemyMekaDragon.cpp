/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * メカドラゴン
 * @author SPATZ.
 * @data   2015/02/07 19:15:09
 */
//---------------------------------------------------
#include "CharaEnemyMekaDragon.h"
#include "Charactor/CharaInfo.h"
#include "Charactor/CharaPlayer.h"
#include "AppLib/Basic/Basic.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Effect/EffectManager.h"
#include "AppLib/Input/InputManager.h"
#include "AppLib/Sound/SoundManager.h"

#include "AppLib/Collision/CollisionManager.h"
#include "DataDefine/MotionDef.h"
#include "DataDefine/VoiceDef.h"
#include "DataDefine/EffectDef.h"
#include "Scene/ActionTaskManager.h"
#include "Utility/Utility.h"
	
#include "Gimmick/GimmickBase.h"

#include "Scene/CommandFunctions.h"
#include "Scene/SceneMainGame.h"
#include "Scene/StageFunc/StageFunc043.h"

#include "AppLib/Resource/ResourceModel.h"


#ifdef _DEBUG
#include "AppLib/Basic/Counter.h"
#endif // _DEBUG

/*=====================================*
 * 
 *=====================================*/
/*static*/const char* CharaEnemyMekaDragon::strAttackTopNode = "攻撃先";
/*static*/const char* CharaEnemyMekaDragon::strAttackBtmNode = "攻撃元";

/*static*/const char* CharaEnemyMekaDragon::strDamageNodeA = "頭先";
/*static*/const char* CharaEnemyMekaDragon::strDamageNodeB = "頭";

static const tCapsuleWork sCapsuleWork[] = {
	{CharaEnemyMekaDragon::strDamageNodeA, CharaEnemyMekaDragon::strDamageNodeB, 320.0f},
};
static const int kCAPSULE_NUM_REDDRAGON = sizeof(sCapsuleWork) / sizeof(tCapsuleWork);

static const char* strNodeNameArray[11] = {
	"身体１",
	"身体２",
	"身体３",
	"身体４",
	"身体５",
	"身体６",
	"身体７",
	"身体８",
	"身体９",
	"身体１０",
	"尻尾",
};
static const BYTE sNodeNameNum = (sizeof(strNodeNameArray) / sizeof(char *) );

//static const float kLASERA_SCALE_MAX = 50.0f;
static const float kLASER_SCALE_MAX = 8.0f;

/*=====================================*
 * funcSubState
 *=====================================*/
CharaEnemyMekaDragon::FUNC_SUB_STATE
CharaEnemyMekaDragon::mFuncSubState[CharaEnemyMekaDragon::eSUB_STATE_MAX] = {
	&CharaEnemyMekaDragon::funcSubStateLoiter,
	&CharaEnemyMekaDragon::funcSubStateRushStart,
	&CharaEnemyMekaDragon::funcSubStateRushDoing,
	&CharaEnemyMekaDragon::funcSubStateRushEnd,
	&CharaEnemyMekaDragon::funcSubStateDiveStart,
	&CharaEnemyMekaDragon::funcSubStateDiveDoing,
	&CharaEnemyMekaDragon::funcSubStateDiveEnd,
	&CharaEnemyMekaDragon::funcSubStateComeoutStart,
	&CharaEnemyMekaDragon::funcSubStateComeoutDoing,
};

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyMekaDragon::CharaEnemyMekaDragon()
{
	mStatePhase	= 0;
	mStateCount = 0;

	mEffHndSmoke= -1;
	mEfkHandle = -1;
	mLaserScale = 0.0f;

	mSubState	= eSUB_STATE_LOITER;
	mFlyNum		= 0;

	mAimPos		= ZERO_VEC;
	mTargetAimPos= ZERO_VEC;
	mRotationVec= ZERO_VEC;
	mAimRotPeriod = 0.0f;

	mRushCount	= 0;

	memset(&mBarrierWork, 0, sizeof(mBarrierWork));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyMekaDragon::~CharaEnemyMekaDragon()
{
	//	GraphicManager::GetInstance()->ReserveRemoveModel(&mPiecesModel);
	GraphicManager::GetInstance()->RemoveDrawModel(&mPiecesModel);
	mPiecesModel.SetAlpha(0.0f);
	mPiecesModel.DeleteModel();

	//	GraphicManager::GetInstance()->ReserveRemoveModel(&mHoleModel);
	//	GraphicManager::GetInstance()->RemoveDrawPreUpdateModel(&mHoleModel);
	GraphicManager::GetInstance()->RemoveDrawPreUpdateModel(this);
	mHoleModel.SetAlpha(0.0f);
	mHoleModel.DeleteModel();

	// 煙エフェクトの処理
	if (mEffHndSmoke != -1) {
		EffectManager::GetInstance()->Stop(mEffHndSmoke, 0.0f);
		mEffHndSmoke = -1;
	}

	if (mEfkHandle != -1) {
		EffekseerManager::GetInstance()->Stop(mEfkHandle);
		mEfkHandle = -1;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::PreDraw(void)
{
	// 穴の更新をここで行う
	int hole_model_handle = mHoleModel.GetModelHandle();
	MV1SetPosition(hole_model_handle,	mHoleModel.Position() );
	MV1SetRotationXYZ(hole_model_handle,mHoleModel.Rotation() );
	MV1SetScale(hole_model_handle,		mHoleModel.Scale() );

	MV1DrawMesh(hole_model_handle, 0);
	MV1DrawMesh(hole_model_handle, 1);

	// 自身の更新処理をおこなう
	{
		MATRIX mtx, tmp;
		mtx = MGetIdent();

		// 拡縮行列
		tmp = MGetScale(VGet(mScale.x, mScale.y, mScale.z));
		mtx = MMult(mtx, tmp);

		// 回転行列
		Utility::MTXLookAtMatrix(&tmp, &VAdd(mRotationVec, mPosition), &mPosition, (VECTOR *)&UP_VEC);
		mtx = MMult(mtx, tmp);

		// 移動行列
		tmp = MGetTranslate(mPosition);
		mtx = MMult(mtx, tmp);

		mMatrix = mtx;

		//<! CharaEnemy::Update() を呼ばない様に対応
	}

	MV1DrawMesh(hole_model_handle, 2);
}

//---------------------------------------------------
/**
 * 穴の中に敵が入ってる様に描画するために、
 * ①穴の中の描画 → ②龍のUpdate() → ③穴のふたの描画　
 * の順で処理を更新
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::PreUpdate(void)
{
	// 穴の更新をここで行う
	int hole_model_handle = mHoleModel.GetModelHandle();
	MV1SetPosition(hole_model_handle,	mHoleModel.Position() );
	MV1SetRotationXYZ(hole_model_handle,mHoleModel.Rotation() );
	MV1SetScale(hole_model_handle,		mHoleModel.Scale() );

	MV1DrawMesh(hole_model_handle, 0);
	MV1DrawMesh(hole_model_handle, 1);

	// 自身の更新処理をおこなう
	{
		MATRIX mtx, tmp;
		mtx = MGetIdent();

		// 拡縮行列
		tmp = MGetScale(VGet(mScale.x, mScale.y, mScale.z));
		mtx = MMult(mtx, tmp);

		// 回転行列
		Utility::MTXLookAtMatrix(&tmp, &VAdd(mRotationVec, mPosition), &mPosition, (VECTOR *)&UP_VEC);
		mtx = MMult(mtx, tmp);

		// 移動行列
		tmp = MGetTranslate(mPosition);
		mtx = MMult(mtx, tmp);

		mMatrix = mtx;

		CharaEnemy::Update();
	}

	MV1DrawMesh(hole_model_handle, 2);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::Update(void)
{
	if (mCharaState == eCHARA_STATE_ATTACK_1) {
		mBodySize = 800.0f;
		PRINT_SCREEN(GetColor(255, 255, 0), "Dragon BodySize %.2f", mBodySize);
	}

	if (mBarrierWork.mIsActive) {

		if (mBarrierWork.mIsAppear == FALSE) {
			mBarrierWork.mHeight *= 1.1f;
		}
		else {
			mBarrierWork.mHeight *= 0.95f;
		}

		GimmickBase* p_gimmick = GimmickBase::Search1stFindGimmickKind(eGIMMICK_KIND_BARRIER);
		APP_ASSERT((p_gimmick != NULL),
				   "Failed Search1stFindGimmickKind() \n");
		p_gimmick->Position().y = mHoleModel.Position().y + mBarrierWork.mHeight;

		if (mBarrierWork.mHeight < -2000.0f) {
			p_gimmick->Scale() = ZERO_VEC;
			mBarrierWork.mIsActive = FALSE;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::Setup(const char* enemyName, int maxHitpoint, const char* layoutName)
{
	// 敵ベースクラスのSetup必要
	CharaEnemy::Setup(enemyName, maxHitpoint, layoutName);

	mFlinchMax = 9999;
	mConstGravitySpeed = 0.0f;	// 無重量に設定

	SetMatrixType(ModelBase::eMATRIX_TYPE_MATRIX);
	UpdateMatrixFromTRS();		 // TRSの情報を元にマトリックスの値を更新

	ResourceModel::GetInstance()->LoadItemResource(RES_MDL_PIECES);
	if (mPiecesModel.GetModelHandle() == -1) {
		mPiecesModel.LinkModel(RES_MDL_PIECES);
		mPiecesModel.Setup();
	}
	GraphicManager::GetInstance()->EntryDrawModel(&mPiecesModel);
	mPiecesModel.SetVisible(false);

	ResourceModel::GetInstance()->LoadItemResource(RES_MDL_HOLE);
	if (mHoleModel.GetModelHandle() == -1) {
		mHoleModel.LinkModel(RES_MDL_HOLE);
		mHoleModel.Setup();
	}
	//	GraphicManager::GetInstance()->EntryDrawPreUpdateModel(&mHoleModel);
	GraphicManager::GetInstance()->EntryDrawPreUpdateModel(this); //<! 自身の PreUpdate() で mHoleModel の描画を行う！
	mHoleModel.SetVisible(false);

	int graph_handle;
	graph_handle = MV1GetTextureGraphHandle(mHoleModel.GetModelHandle(), 0);
	MV1SetTextureGraphHandle(mPiecesModel.GetModelHandle(), 0, graph_handle, FALSE);
	
	// マトリックスを元に回転ベクトルを取得
	mRotationVec = VNorm(VTransformSR(VGet(0.0f, 0.0f, -1.0f), mMatrix) );

	mVirtualUpVec = UP_VEC;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	if ((attackType == eATTACK_TYPE_COPY_ROD) || (attackType == eATTACK_TYPE_THUNDER)) {
		attackPower *= 2;
	}
	CharaEnemy::ProcessDamage(attackPower, isSetMode, pEffPos, pDamageDownVec, isDamageDown, attackType);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CharaEnemyMekaDragon::CheckBodyCollision(CollisionManager::tSphereWork& rOtherSphere, VECTOR& rVec, float& rDist)
{
	if (mCharaState == eCHARA_STATE_MOVE) {
		/* 飛んでいる場合 */
		CollisionManager::tSphereWork my_sphere;
		my_sphere.mPosition = mPosition;
		my_sphere.mSize		= mBodySize;
		// 円と円の辺り判定
		if (CollisionManager::GetInstance()->Check_SphereToSphere(&rOtherSphere, &my_sphere, &rVec, &rDist) ) {
			return true;
		}

		my_sphere.mPosition = mHoleModel.Position();
		my_sphere.mSize		= 800.0f;
		// 円と円の辺り判定
		if (CollisionManager::GetInstance()->Check_SphereToSphere(&rOtherSphere, &my_sphere, &rVec, &rDist) ) {
			return true;
		}

	} else {
		/* 着地している場合 */
		CollisionManager::tSphereWork my_sphere;
		my_sphere.mPosition = mHoleModel.Position();
		my_sphere.mSize		= 800.0f;

		// 円と円の辺り判定
		if (CollisionManager::GetInstance()->Check_SphereToSphere(&rOtherSphere, &my_sphere, &rVec, &rDist) ) {
			return true;
		}

		VECTOR node_pos;
		GetFramePosFromName(&node_pos, "頭");
		my_sphere.mPosition = node_pos;
		my_sphere.mPosition.y = mPosition.y;

		float abount_dot = 0.0f;
		CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
		VECTOR player_to_enemy;

		player_to_enemy = VSub(p_player->Position(), node_pos);
		player_to_enemy.y = 0.0f;
		player_to_enemy = VNorm(player_to_enemy);

		// 内積を求める
		abount_dot = VDot(mRotationVec, player_to_enemy);
		//		my_sphere.mSize = (100.0f + (450.0f * abs(abount_dot) ) );
		my_sphere.mSize = (100.0f + (480.0f * abs(abount_dot) ) );

		// 円と円の辺り判定
		if (CollisionManager::GetInstance()->Check_SphereToSphere(&rOtherSphere, &my_sphere, &rVec, &rDist) ) {
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
CharaEnemyMekaDragon::CheckDamageCollNormal(tCheckDamageWork* pParam)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	tCheckDamageWork* p_work = (tCheckDamageWork *)pParam;

	ShapeCapsule capsule;
	ShapeLine	line;

	line.mStart = p_work->mWeaponBtmPos;
	line.mEnd	= p_work->mWeaponTopPos;

	this->GetFramePosFromName(&capsule.mPosA, sCapsuleWork[0].mPosA);
	this->GetFramePosFromName(&capsule.mPosB, sCapsuleWork[0].mPosB);
	capsule.mRadius = sCapsuleWork[0].mRadius;
#ifdef _DEBUG
	GraphicManager::GetInstance()->DbgRenderCapsule(&capsule);
#endif // _DEBUG
	if (CollisionManager::GetInstance()->Check_LineToCapsule(&line, &capsule)) {
		return true;
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CharaEnemyMekaDragon::CheckDamageCollBomb(ShapeCapsule* pParam)
{
	ShapeCapsule capsule;
	this->GetFramePosFromName(&capsule.mPosA, sCapsuleWork[0].mPosA);
	this->GetFramePosFromName(&capsule.mPosB, sCapsuleWork[0].mPosB);
	capsule.mRadius = sCapsuleWork[0].mRadius;

	if (CollisionManager::Check_CapsuleToCapsule(pParam, &capsule)) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::GetLockonPosition(VECTOR* pPosition)
{
	VECTOR node_posA, node_posB;
	GetFramePosFromName(&node_posA, "頭");
	GetFramePosFromName(&node_posB, "頭先");

	*pPosition = VScale(VAdd(node_posA, node_posB), 0.5f);
	pPosition->y = mPosition.y + 100.0f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseStateIdle(void)
{
	// 煙エフェクトの処理
	if (mEffHndSmoke != -1) {
		EffectManager::GetInstance()->Stop(mEffHndSmoke, 0.5f);
		mEffHndSmoke = -1;
	}

	mAimPos = mPosition;

	updateTargetAimPosRandom();
	
	mMoveSpeed = 90.0f * ( ((float)getHitPointRate() * 0.2f) + 1.0f);
	
	ChangeState(eCHARA_STATE_MOVE);		// STATE_TURN は飛ばす
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseEnterStateMove(void)
{
	const tEnemyWork* p_work = GetEnemyWorkTable(eENEMY_KIND_MEKADRAGON);

	// 身体の大きさを変える
	mBodySize	= p_work->mBodySize;
	mBodyHeight = p_work->mBodyHeight;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseStateMove(void)
{
	VECTOR head_pos;
	GetFramePosFromName(&head_pos, "頭");

#ifdef _DEBUG
	ShapeCapsule capsule;
	capsule.mPosA = mAimPos;
	capsule.mPosB = mAimPos;
	capsule.mPosB.y += 1.0f;
	capsule.mRadius = 100.0f;
	GraphicManager::GetInstance()->DbgRenderCapsule(&capsule);
#endif // _DEBUG

	mAimRotPeriod += 0.05f;

	// Sub State
	(this->*mFuncSubState[mSubState])();
	if (mCharaState != eCHARA_STATE_MOVE) {
		return;
	}

	if (mEffHndSmoke != -1) {
		// 煙エフェクトの処理
		VECTOR posA;
		GetFramePosFromName(&posA, "頭");
		EffectManager::GetInstance()->Position(mEffHndSmoke) = posA;
		EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "particleSpeed", 0.5f);
	}

	VECTOR tmp_aim_pos = mAimPos;
	tmp_aim_pos.x += sin(mAimRotPeriod) * 100.0f;
	tmp_aim_pos.y += sin(mAimRotPeriod) * 600.0f;

	VECTOR aim_vec, diff_rot_vec;
	aim_vec = VNorm(VSub(tmp_aim_pos, head_pos) );
	
	float diff_rot_vec_y = aim_vec.y - mRotationVec.y;
	
	float rot_vec_rad;
	rot_vec_rad = atan2(mRotationVec.x, mRotationVec.z) + PHI_F;
	
	MATRIX tmp_mtx;
	tmp_mtx = MGetRotY(-rot_vec_rad);
	
	VECTOR conv_rot_vec, conv_aim_vec;
	conv_rot_vec = VTransform(mRotationVec, tmp_mtx);
	conv_rot_vec.y = 0.0f;
	conv_aim_vec = VTransform(aim_vec, tmp_mtx);
	conv_aim_vec.y = 0.0f;
	diff_rot_vec = VSub(conv_aim_vec, conv_rot_vec);
	updateRevolveMove(diff_rot_vec_y, diff_rot_vec.x, mMoveSpeed);

	PRINT_SCREEN(COLOR_RED, "dist : %.2f \n", VSize(VSub(tmp_aim_pos, head_pos) ) );


	/*=====================================*
	 * 当たり判定の処理
	 *=====================================*/
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	if (p_player->IsDamage() == false) {

		ShapeCapsule capsuleA, capsuleB;
		float laser_len  = (200.0f * ( (float)mStateCount / 10.0f) );
		float laser_size = 12.0f;

		p_player->GetBodyCapsule(&capsuleA);

		int node_index = 0;
		for (int iI = 0; iI < 2; iI ++) {
			node_index = (iI == 0) ? 0 : (sNodeNameNum - 1);	// ノードの先頭か最後尾か
			GetFramePosFromName(&capsuleB.mPosA, strNodeNameArray[node_index]);
			capsuleB.mPosB = capsuleB.mPosA;
			capsuleB.mPosB.y += 1.0f;
			capsuleB.mRadius = 300.0f;
#ifdef _DEBUG
			GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 4;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), capsuleB.mPosA);
				damage_down_vec.y = 0.0f;
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_BREAK_GUARD);
			}
		}
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseEnterStateJumpUp(void)
{
	ChangeState(eCHARA_STATE_IDLE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseStateJumpUp(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseEnterStateJumpDown(void)
{
	ChangeState(eCHARA_STATE_IDLE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseStateJumpDown(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseStateAppear(void)
{
	if (((mAppearWaitMax >= mAppearWaitCount) && 
		 (mAppearWaitMax < mAppearWaitCount + (SHORT)kBASIC_FRAME_COUNTER))) {
		
		mPosition.y += 400.0f;

		VECTOR effpos = VAdd(mPosition, mMoveVec);
		int eff_handle = EffectManager::GetInstance()->Play(eEF_BLACKDUST, &effpos, 50.0f, 0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 2.0f);
		SoundManager::GetSndComponent()->PlaySound(eSE_ENEMY_APPEAR);
		
		mIsLand = false;
		//<! ダメージ状態を解除
		mIsDamage = false;
		this->SetAlpha(1.0f);
	}

	if (mAppearWaitMax < mAppearWaitCount) {
		//		if (mCharaState == eCHARA_STATE_JUMP_UP) {
			ChangeState(eCHARA_STATE_IDLE);
			//		}
	}
	mAppearWaitCount += (SHORT)kBASIC_FRAME_COUNTER;
}

//---------------------------------------------------
/**
 *
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseEnterStateAttack1(void)
{
	StartMotion(eMT_ENE_IDLE, true, 0.5f);
}

//---------------------------------------------------
/**
 *
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseStateAttack1(void)
{
	// ズレ防止の為毎フレ更新
	mPosition = mHoleModel.Position();

	// プレイヤーの方に少しずつ向かせる
	updateRotationToPlayer(0.1f);

	if (GetIsMotionBlend() == false) {

		if ( (mFlyNum == 0) && (getHitPointRate() == eHP_RATE_MIDDLE) ||
			 (mFlyNum == 1) && (getHitPointRate() == eHP_RATE_LOW) ) {
			ChangeState(eCHARA_STATE_ATTACK_7);
		} else {
			CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
			VECTOR vec = VSub(p_player->Position(), mPosition);
			float dist = VSize(vec);

			if (GetRand( (30 - (getHitPointRate() * 10) ) ) == 0) {
				int getrand = GetRand(7);
				switch (getrand) {
				case 0:
					ChangeState(eCHARA_STATE_ATTACK_2);		// 波動砲
					break;
				case 1:
				case 2:
				case 3:
					if (dist < mBodySize * 3.0f) {
						ChangeState(eCHARA_STATE_ATTACK_3);	// 頭攻撃
					} else {
						ChangeState(eCHARA_STATE_ATTACK_2);	// 波動砲
					}
					break;
				case 4:
				case 5:
					ChangeState(eCHARA_STATE_ATTACK_4);		// 左手攻撃
					break;
				case 6:
				case 7:
					ChangeState(eCHARA_STATE_ATTACK_5);		// 右手攻撃
					break;
				default:
					break;
				};
			}
		}

	}
}

//---------------------------------------------------
/**
 * 銃攻撃
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseEnterStateAttack2(void)
{
	if (getHitPointRate() == eHP_RATE_LOW) {
		StartMotion(eMT_ENE_CANNON_LASER, false, 0.5f);
	} else {
		StartMotion(eMT_ENE_CANNON_LASER, false, 0.4f);
	}

	mLaserScale = kLASER_SCALE_MAX;

	//	startRockBarrier();
	CameraManager::GetInstance()->StartShakeCamera(3.0f, 5.0f, 60.0f);

	mStatePhase = 0; mStateCount = 0;
}

//---------------------------------------------------
/**
 * 銃攻撃
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseStateAttack2(void)
{
	// ズレ防止の為毎フレ更新
	mPosition = mHoleModel.Position();

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	MATRIX node_mtx;
	VECTOR node_pos;
	GetFrameMatrixFromName(&node_mtx, "口部レーザー先");
	Utility::ExtractMatrixPosition(&node_pos, &node_mtx);

	ePLightNo light_no = ePLIGHT_NO_00;

	MATRIX mtx;
	{
		MATRIX tmp;
		mtx = MGetIdent();
		// 拡縮行列
		//		tmp = MGetScale(VGet(mLaserScale, mLaserScale, mLaserScale) );
		tmp = MGetScale(VGet(kLASER_SCALE_MAX, mLaserScale, kLASER_SCALE_MAX) );
		mtx = MMult(mtx, tmp);
		tmp = MGetRotY(Utility::DegToRad(90.0f) );
		mtx = MMult(mtx, tmp);
		// 回転行列
		Utility::ExtractRotationMatrix(&tmp, &node_mtx, 1.0f);
		mtx = MMult(mtx, tmp);
		// 移動行列
		tmp = MGetTranslate(node_pos);
		mtx = MMult(mtx, tmp);
	}

	// エフェクトの更新
	if (mEfkHandle != -1) {
		EffekseerManager::GetInstance()->SetMatrix(mEfkHandle, &mtx);
	}

	switch (mStatePhase) {
	case 0:
		// 溜めの開始
		//		if (GetIsEndMotion()) {
		mBodySize *= 1.04f;

		if (EqualPhaseFrame(7.0f) ) {

			mEfkHandle = EffekseerManager::GetInstance()->PlayMtx(eEFK_CANNON_LASER_CHARGE, &mtx);
			if (getHitPointRate() == eHP_RATE_LOW) {
				EffekseerManager::GetInstance()->SetSpeed(mEfkHandle, 1.0f);
			} else {
				EffekseerManager::GetInstance()->SetSpeed(mEfkHandle, 0.75f);
			}
			CameraManager::GetInstance()->StartVibeCamera(1.0f, 4.0f, 6.0f);
			SoundManager::GetSndComponent()->PlaySound(eSE_CANNON_LASER_CHARGE);

			// エミッシブを無効にする
			CollisionManager::GetInstance()->GetEarthModel()->SetEnableEmissive(false);

			// ポイントライト有効
			light_no = ePLIGHT_NO_00;
			GraphicManager::GetInstance()->SetPLightPosition(&node_pos, light_no);
			GraphicManager::GetInstance()->SetEnablePLight(true, light_no);
			GraphicManager::GetInstance()->SetPLightRangeAtten(6000.0f, 0.01f, 0.0f, 0.00000006f, light_no);
			GraphicManager::GetInstance()->SetPLightDiffuse(COL_BYTE_2_FLOAT(52), 
															COL_BYTE_2_FLOAT(54),
															COL_BYTE_2_FLOAT(237), 0.0f, light_no);
			GraphicManager::GetInstance()->SetPLightSpecular(COL_BYTE_2_FLOAT(5), 
															 COL_BYTE_2_FLOAT(10),
															 COL_BYTE_2_FLOAT(205), 0.0f, light_no);
			GraphicManager::GetInstance()->SetPLightAmbient(COL_BYTE_2_FLOAT(63), 
															COL_BYTE_2_FLOAT(63),
															COL_BYTE_2_FLOAT(63), 0.0f, light_no);
			GraphicManager::GetInstance()->SetEnableOriginalShader(true);

			// 光エフェクト再生
			mEffHndSmoke = EffectManager::GetInstance()->Play(eEF_NAVI, &node_pos, 300.0f, 0.0f);
				
			mStatePhase ++; mStateCount = 0;

		}
		break;

	case 1:
		{
			if (((getHitPointRate() == eHP_RATE_LOW) && (mStateCount == 62) ) ||
				((getHitPointRate() != eHP_RATE_LOW) && (mStateCount == 78) )) {

				EffekseerManager::GetInstance()->Stop(mEfkHandle, 0.0f);
				mEfkHandle = EffekseerManager::GetInstance()->PlayMtx(eEFK_CANNON_LASER_SHOOT, &mtx);
			}
			
			// レーザーを吐き出すタイミング
			if (((getHitPointRate() == eHP_RATE_LOW) && (mStateCount == 64) ) ||
				((getHitPointRate() != eHP_RATE_LOW) && (mStateCount == 80) )) {
				SoundManager::GetSndComponent()->PlaySound(eSE_CANNON_LASER);
				SoundManager::GetSndComponent()->SetVolume(eSE_CANNON_LASER_DOING, 255);
				SoundManager::GetSndComponent()->PlaySound(eSE_CANNON_LASER_DOING);
				CameraManager::GetInstance()->StartVibeCamera(10.0f, 7.0f, 10.0f);
				
				EffekseerManager::GetInstance()->SetSpeed(mEfkHandle, 1.2f);

				CollisionManager::GetInstance()->GetEarthModel()->SetEnableEmissive(true);
				mStatePhase ++; mStateCount = 0;
			}
		}
		break;


	case 2:
		/**** 当たり判定 ****/
		if (p_player->IsDamage() == false) {
			ShapeCapsule capsuleA, capsuleB;
			capsuleB.mPosA = node_pos;
			VECTOR rot_vec;
			rot_vec = VNorm(VTransformSR(VGet(0.0f, 0.0f, -1.0f), node_mtx) );
			capsuleB.mPosB = VAdd(capsuleB.mPosA, VScale(rot_vec, 7400.0f) );
			capsuleB.mRadius = 550.0f;

#ifdef _DEBUG
			GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG
			// キャラとの当たり判定
			capsuleA.mPosA	= p_player->Position();
			capsuleA.mPosB	= p_player->Position();
			capsuleA.mPosB.y += p_player->BodyHeight();
			capsuleA.mRadius= p_player->BodySize();

			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				//			if (false) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 4;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), capsuleB.mPosA);
				damage_down_vec.y = 0.0f;
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_BREAK_GUARD);
			}
		}


		if (((getHitPointRate() != eHP_RATE_HIGH) && (GetIsEndMotion()) ) ||
			((getHitPointRate() == eHP_RATE_HIGH) && (EqualPhaseFrame(140.0f)))) {
			StartMotion(eMT_ENE_IDLE, true, 0.5f, NULL, 0.01f);
			mStatePhase ++; mStateCount = 0;
		}
		break;
		
	case 3:
		mLaserScale *= 0.9f;

		{
			float reverse_rate = mLaserScale / kLASER_SCALE_MAX;

			int volume = 0;
			volume = (int)(reverse_rate * 255.0f);
			SoundManager::GetSndComponent()->SetVolume(eSE_CANNON_LASER_DOING, volume);

		}


		



		// 終了判定
		if (mLaserScale < 1.0f) {
			mLaserScale = 0.0f;
			EffekseerManager::GetInstance()->Stop(mEfkHandle, 0.0f);
			mEfkHandle = -1;
			SoundManager::GetSndComponent()->StopSound(eSE_CANNON_LASER_DOING);

			// エフェクト停止
			if (mEffHndSmoke != -1) {
				EffectManager::GetInstance()->Stop(mEffHndSmoke);
				mEffHndSmoke = -1;
			}

			GraphicManager::GetInstance()->SetEnablePLight(TRUE);
			GraphicManager::GetInstance()->SetPLightPosition(&VGet(0.000f, -252000.000f, 0.000f) );
			GraphicManager::GetInstance()->SetPLightRangeAtten(309000.000f, 0.13f, (float)3E-07, 0);
			GraphicManager::GetInstance()->SetPLightDiffuse(0.419610f, 0.678430f, 0.713730f, 0.000000f);
			GraphicManager::GetInstance()->SetPLightSpecular(0.000000f, 0.000000f, 0.000000f, 0.000000f);
			GraphicManager::GetInstance()->SetPLightAmbient(0.247060f, 0.427450f, 0.247060f, 0.000000f);

			//			GraphicManager::GetInstance()->SetEnablePLight(false, eSLIGHT_NO_00);

			GraphicManager::GetInstance()->SetEnableOriginalShader(false);
			GraphicManager::GetInstance()->SetEnableDirectionLight(FALSE);
			CollisionManager::GetInstance()->GetEarthModel()->SetEnableEmissive(true);
			
			//			endRockBarrier();
			CameraManager::GetInstance()->StartShakeCamera(3.0f, 5.0f, 60.0f);

			ChangeState(eCHARA_STATE_ATTACK_1);
		}
		break;

	default:
		break;
	}

	if (mEffHndSmoke != -1) {
		GraphicManager::GetInstance()->SetPLightPosition(&node_pos, light_no);

		//		float scale = 3000.0f;
		float scale = 5000.0f;
		EffectManager::GetInstance()->Position(mEffHndSmoke) = node_pos;
		EffectManager::GetInstance()->Scale(mEffHndSmoke) = VGet(scale, scale, scale);
		EffectManager::GetInstance()->SetAlpha(mEffHndSmoke, 0.2f);
	}

	mStateCount ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseEnterStateAttack3(void)
{
	if (getHitPointRate() == eHP_RATE_HIGH) {
		StartMotion(eMT_ENE_THRUST, false, 0.6f);
	} else {
		StartMotion(eMT_ENE_THRUST, false, 0.7f);
	}

 	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);

	mStatePhase = 0; mStateCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseStateAttack3(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	switch (mStatePhase) {
	case 0:

		if (BetweenPhaseFrame(0.0f, 40.0f) ) {

			// プレイヤーの方に少しずつ向かせる
			updateRotationToPlayer(0.08f);

		} else if (BetweenPhaseFrame(45.0f, 55.0f) ) {
			if (p_player->IsDamage() == false) {
				ShapeCapsule capsuleA, capsuleB;

				GetFramePosFromName(&capsuleB.mPosA, sCapsuleWork[0].mPosA);
				GetFramePosFromName(&capsuleB.mPosB, sCapsuleWork[0].mPosB);
				capsuleB.mRadius = sCapsuleWork[0].mRadius;

#ifdef _DEBUG
				GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG
				// キャラとの当たり判定
				capsuleA.mPosA	= p_player->Position();
				capsuleA.mPosB	= p_player->Position();
				capsuleA.mPosB.y += p_player->BodyHeight();
				capsuleA.mRadius= p_player->BodySize();

				if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
					//			if (false) {
					SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
					int attack_power = 4;
					VECTOR damage_down_vec;
					damage_down_vec = VSub(p_player->Position(), capsuleB.mPosA);
					damage_down_vec.y = 0.0f;
					damage_down_vec = VNorm(damage_down_vec);
					p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, true, eATTACK_TYPE_BREAK_GUARD);
				}
			}
		}

		if (EqualPhaseFrame(53.0f) ) {
			VECTOR effpos;
			GetFramePosFromName(&effpos, strDamageNodeB);
			int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST, &effpos, 100.0f, 0.0f);
			EffectManager::GetInstance()->Stop(eff_handle, 1.0f);

			CameraManager::GetInstance()->StartVibeCamera(2.0f, 8.0f, 10.0f);
			SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
			
		} else if (GetIsEndMotion()) {
			mStatePhase ++; mStateCount = 0;
		}
		break;

	case 1:
		if (mStateCount > 75) {
			StartMotion(eMT_ENE_IDLE, true, 0.5f, NULL, 0.03f);
			mStatePhase ++; mStateCount = 0;
		}
		break;

	case 2:
		if (GetIsMotionBlend() == false) {
			mStatePhase =0; mStateCount = 0;
			ChangeState(eCHARA_STATE_ATTACK_1);
		}
		break;

	default:
		break;
	}

	mStateCount ++;
}

//---------------------------------------------------
/**
 * 左手攻撃
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseEnterStateAttack4(void)
{
	if (getHitPointRate() == eHP_RATE_LOW) {
		StartMotion(eMT_ENE_LEFT_ATTACK, false, 0.5f);
	} else if (getHitPointRate() == eHP_RATE_MIDDLE) {
		StartMotion(eMT_ENE_LEFT_ATTACK, false, 0.45f);
	} else {
		StartMotion(eMT_ENE_LEFT_ATTACK, false, 0.4f);
	}
 	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	mStatePhase = 0; mStateCount = 0;
}

//---------------------------------------------------
/**
 * 左手攻撃
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseStateAttack4(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (EqualPhaseFrame(22.0f)) {
		VECTOR effpos;
		GetFramePosFromName(&effpos, "左指先");
		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST, &effpos, 50.0f, 0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 0.5f);
			
		CameraManager::GetInstance()->StartVibeCamera(0.5, 6.0f, 8.0f);
		SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
	}

	if (BetweenPhaseFrame(18.0f, 22.0f) ) {
		if (p_player->IsDamage() == false) {
			ShapeCapsule capsuleA, capsuleB;

			GetFramePosFromName(&capsuleB.mPosA, "左ひじ");
			GetFramePosFromName(&capsuleB.mPosB, "左指先");
			capsuleB.mRadius = 120.0f;
#ifdef _DEBUG
			GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG
			// キャラとの当たり判定
			capsuleA.mPosA	= p_player->Position();
			capsuleA.mPosB	= p_player->Position();
			capsuleA.mPosB.y += p_player->BodyHeight();
			capsuleA.mRadius= p_player->BodySize();
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 3;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), capsuleB.mPosA);
				damage_down_vec.y = 0.0f;
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, false, eATTACK_TYPE_BREAK_GUARD);
			}
		}
	}

	if (EqualPhaseFrame(25.0f)) {
		VECTOR vec = VSub(p_player->Position(), mPosition);
		float dist = VSize(vec);
		if (dist < mBodySize * 3.0f) {
			if (GetRand(1) == 0) {
				ChangeState(eCHARA_STATE_ATTACK_3);
			}
		}
	}
	if (GetIsEndMotion() ) {
		ChangeState(eCHARA_STATE_ATTACK_1);
	}
}

//---------------------------------------------------
/**
 * 右手攻撃
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseEnterStateAttack5(void)
{
	if (getHitPointRate() == eHP_RATE_LOW) {
		StartMotion(eMT_ENE_RIGHT_ATTACK, false, 0.50f);
	} else if (getHitPointRate() == eHP_RATE_MIDDLE) {
		StartMotion(eMT_ENE_RIGHT_ATTACK, false, 0.45f);
	} else {
		StartMotion(eMT_ENE_RIGHT_ATTACK, false, 0.40f);
	}

 	SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_ATTACK_1);
	mStatePhase = 0; mStateCount = 0;
}

//---------------------------------------------------
/**
 * 右手攻撃
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseStateAttack5(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	if (EqualPhaseFrame(22.0f)) {
		VECTOR effpos;
		GetFramePosFromName(&effpos, "右指先");
		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST, &effpos, 50.0f, 0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 0.5f);
			
		CameraManager::GetInstance()->StartVibeCamera(0.5, 6.0f, 8.0f);
		SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
	}

	if (BetweenPhaseFrame(18.0f, 22.0f) ) {
		if (p_player->IsDamage() == false) {
			ShapeCapsule capsuleA, capsuleB;

			GetFramePosFromName(&capsuleB.mPosA, "右ひじ");
			GetFramePosFromName(&capsuleB.mPosB, "右指先");
			capsuleB.mRadius = 120.0f;
#ifdef _DEBUG
			GraphicManager::GetInstance()->DbgRenderCapsule(&capsuleB);
#endif // #ifdef _DEBUG
			// キャラとの当たり判定
			capsuleA.mPosA	= p_player->Position();
			capsuleA.mPosB	= p_player->Position();
			capsuleA.mPosB.y += p_player->BodyHeight();
			capsuleA.mRadius= p_player->BodySize();
			if (CollisionManager::GetInstance()->Check_CapsuleToCapsule(&capsuleA, &capsuleB)) {
				SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				int attack_power = 3;
				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_player->Position(), capsuleB.mPosA);
				damage_down_vec.y = 0.0f;
				damage_down_vec = VNorm(damage_down_vec);
				p_player->ProcessDamage(attack_power, false, NULL, &damage_down_vec, false, eATTACK_TYPE_BREAK_GUARD);
			}
		}
	}

	if (GetIsEndMotion() ) {
		ChangeState(eCHARA_STATE_ATTACK_1);
	}
}

//---------------------------------------------------
/**
 * 着穴
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseEnterStateAttack6(void)
{
	updateRotationToPlayer(0.9f);

	mStatePhase = 0;
	mStateCount = 0;

	if (mEffHndSmoke != -1) {
		EffectManager::GetInstance()->Stop(mEffHndSmoke, 1.0f);
		mEffHndSmoke = -1;
	}

	StartMotion(eMT_ENE_THRUST, false, 0.1f);
}

//---------------------------------------------------
/**
 * 着穴
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseStateAttack6(void)
{
	switch (mStatePhase) {
	case 0:
		mPosition.y -= 50.0f;
		if (mStateCount > 60) {

			APP_ASSERT_PRINT((SceneMainGame::GetStageIndex() == 43),
							 "Invalid StageIndex : %d \n", SceneMainGame::GetStageIndex() );

			StageFunc043* p_stage = (StageFunc043 *)SceneMainGame::GetStageFunc();
			p_stage->StartFadeInWall();

			SoundManager::GetSndComponent()->PlaySound(eSE_MONSTER_VOICE);
			StartJoypadVibration(DX_INPUT_PAD1, 500, 2000);

			mPosition = mHoleModel.Position();
			for (int iI = 0; iI < sNodeNameNum; iI ++) {
				ResetFrameUserMatrix((char *)strNodeNameArray[iI]);
			}

			CameraManager::GetInstance()->StartVibeCamera(3.0f, 8.0f, 10.0f);
			SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
			
			int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST, &mPosition, 160.0f, 0.0f);
			EffectManager::GetInstance()->Stop(eff_handle, 0.5f);

			mStatePhase ++; mStateCount = 0;
		}
		break;

	case 1:
		// 身体の大きさを変える
		updateRotationToPlayer(0.1f);

		mBodySize	= 800.0f;
		mBodyHeight = 250.0f;
		//		StartMotion(eMT_ENE_IDLE, true, 0.5f, NULL, 1.0f);
		mStatePhase ++; mStateCount = 0;
		break;

	case 2:
		updateRotationToPlayer(0.1f);

		if (mStateCount > 210) {
			ChangeState(eCHARA_STATE_ATTACK_1);
		}
		break;

	default:
		break;
	};


	if ( (GetMotionIndex() == eMT_ENE_THRUST) &&
		 (EqualPhaseFrame(40)) ) {
		StartMotion(eMT_ENE_IDLE, true, 0.5f, NULL, 1.0f);
	}

	mStateCount ++;
}

//---------------------------------------------------
/**
 * 飛び上がり
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseEnterStateAttack7(void)
{
	StageFunc043* p_stage = (StageFunc043 *)SceneMainGame::GetStageFunc();
	p_stage->StartFadeOutWall();

	SoundManager::GetSndComponent()->PlaySound(eSE_MONSTER_VOICE);
	StartMotion(eMT_ENE_THRUST, false, 0.5f);
}

//---------------------------------------------------
/**
 * 飛び上がり
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseStateAttack7(void)
{
	mPosition.y -= 100.0f;

	if (EqualPhaseFrame(40)) {
		mPosition.y -= 1000.0f;
		VECTOR ch_pos; float ch_rot;
		getPlayerPosRotFromBGModel(&ch_pos, &ch_rot, 1);
		VECTOR add_vec;
		
		add_vec = VGet(0.0f, 12000.0f, 0.0f);
		mTargetAimPos = VAdd(ch_pos, add_vec);
		
		mAimPos = mPosition;
		StartMotion(eMT_ENE_FLYING, true, 0.5f);

		mFlyNum ++;

		mSubState = eSUB_STATE_COMEOUT_START;
		//		mSubState = eSUB_STATE_LOITER;
		ChangeState(eCHARA_STATE_MOVE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseEnterStateRetireStart(void)
{
	mIsAttack = false;
	//	StartMotion(eMT_ENE_THRUST, false, 0.5f);
	// プレイヤーとの距離が離れたなどのシステムで倒した場合は悲鳴ボイスを再生
	if (mHitpoint != kSYSTEM_DAMAGE_VALUE) {
		SoundManager::GetVcComponent(mCharaName)->PlaySound(eVC_DOWN);
	}
	else {

		// 煙エフェクトの処理
		if (mEffHndSmoke != -1) {
			EffectManager::GetInstance()->Stop(mEffHndSmoke, 0.0f);
			mEffHndSmoke = -1;
		}

		if (mEfkHandle != -1) {
			EffekseerManager::GetInstance()->Stop(mEfkHandle);
			mEfkHandle = -1;
		}

	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseStateRetireStart(void)
{
	ChangeState(eCHARA_STATE_RETIRE_DOING);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseEnterStateRetireDoing(void)
{
	if (mHitpoint != kSYSTEM_DAMAGE_VALUE) {

		VECTOR eff_pos;
		GetFramePosFromName(&eff_pos, "頭");
		mEffHndSmoke = EffectManager::GetInstance()->Play(eEF_CHARGE,
														  &eff_pos,
														  40.0f,
														  0.0f);
		EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "particleSpeed", -2.6f);

		SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
		CameraManager::GetInstance()->StartVibeCamera(3.0f, 6.0f, 8.0f);

		GraphicManager::GetInstance()->RemoveDrawShadow(&mShadow);

		mHoleModel.Scale() = ZERO_VEC;
		mPiecesModel.Scale() = ZERO_VEC;

		CharaPlayer* p_mine = (CharaPlayer *)CollisionManager::GetInstance()->GetBodyCollisionAddress(0);
		p_mine->PermitAcceptInput(false);
		//		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
		TotalInfo::GetInstance()->DisableLockonLayout();

		ActionTaskManager::tTaskInfo info;
		info.mTaskType = ActionTaskManager::eTASK_ENEMY_ITEM;
		info.mTaskValueA = (long long)this;
		info.mTaskValueB = 1;
		PRINT_CONSOLE("NotifyAction -> eTASK_ENEMY_ITEM \n");
		ActionTaskManager::GetInstance()->NotifyAction(&info);
	}

	mStatePhase = 0;
	mStateCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseStateRetireDoing(void)
{
	// システムダメージの場合、以降の処理を行わない
	if (mHitpoint == kSYSTEM_DAMAGE_VALUE) {
		CharaEnemy::phaseStateRetireDoing();
		return;
	}

	float alpha = this->GetAlpha();

	switch (mStatePhase) {
	case 0:

		alpha += -0.01f;
		if (alpha <= 0.4f) {
			
			alpha = 0.4f;
		}
		if (mStateCount > 150) {

			if (mEffHndSmoke != -1) {
				EffectManager::GetInstance()->Stop(mEffHndSmoke, 1.0f);
				mEffHndSmoke = -1;
			}

			VECTOR eff_pos;
			GetFramePosFromName(&eff_pos, "頭");
			mEfkHandle = EffekseerManager::GetInstance()->Play(eEFK_BOSS_DEATH,
															   &eff_pos,
															   &VScale(VGet(12.0f, 12.0f, 12.0f), 3.0f));
			EffekseerManager::GetInstance()->SetSpeed(mEfkHandle, 0.8f);

			SoundManager::GetSndComponent()->PlaySound(eSE_BREAK_BARRIER);
			SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
			CameraManager::GetInstance()->StartShakeCamera(3.0f, 5.0f, 60.0f);

			mStatePhase ++; mStateCount = 0;
		}

		break;

	case 1:
		alpha += -0.03f;
		if (alpha <= 0.1f) {
			alpha = 0.0002f;
			mScale = ZERO_VEC;
		}
		if (EffekseerManager::GetInstance()->IsPlay(mEfkHandle) == false) {
			alpha = 0.0f;
			ChangeState(eCHARA_STATE_RETIRE_END);
		}
		break;
	default:
		break;
	}
	mStateCount ++;

	this->SetAlpha(alpha);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::phaseEnterStateRetireEnd(void)
{
	GraphicManager::GetInstance()->ReserveRemoveModel((ModelBase *)this);
	CollisionManager::GetInstance()->RemoveBodyCollision((ModelBase *)this);
	CollisionManager::GetInstance()->RemoveAttackCollision((ModelBase *)this);

	this->SetAlpha(0.0f);

	ModelPmd::DeleteModel();
	mIsActive = false;

	GraphicManager::GetInstance()->RemoveDrawShadow(&mShadow);
}

/*=====================================*
 * funcSubState
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::funcSubStateLoiter(void)
{
	// 煙エフェクトの処理
	if (mEffHndSmoke != -1) {
		EffectManager::GetInstance()->Stop(mEffHndSmoke, 1.0f);
		mEffHndSmoke = -1;
	}

	VECTOR vec;
	vec = VSub(mTargetAimPos, mAimPos);
	mAimPos = VAdd(mAimPos, VScale(vec, 0.005f) );
	
	if (VSize(vec) < 600.0f) {
		mSubState = eSUB_STATE_RUSH_START;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::funcSubStateRushStart(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	mTargetAimPos = p_player->Position();
	mAimPos = p_player->Position();
	//	mAimPos.y += 300.0f;
	mAimPos.y += 140.0f;

	mMoveSpeed = 150.0f * ( ((float)getHitPointRate() * 0.2f) + 1.0f);

	mSubState = eSUB_STATE_RUSH_DOING;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::funcSubStateRushDoing(void)
{
	VECTOR ch_pos; float ch_rot;

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	if (getHitPointRate() == eHP_RATE_MIDDLE) {
		mAimPos = VScale(VAdd(VScale(mAimPos, 15.0f), p_player->Position() ), (1.0f / 16.0f) );
	}
	else if (getHitPointRate() == eHP_RATE_LOW) {
		mAimPos = VScale(VAdd(VScale(mAimPos,  7.0f), p_player->Position() ), (1.0f /  8.0f) );
	}
	
	// 下限設定をする
	getPlayerPosRotFromBGModel(&ch_pos, &ch_rot, 1);
	if (mAimPos.y < ch_pos.y) {
		mAimPos.y = ch_pos.y;
	}

	VECTOR vec;
	vec = VSub(mAimPos, mPosition);

	if (VSize(vec) < 500.0f) {

		if ((mRushCount >= 6) ||
			((mRushCount >= 3) && (GetRand(1) == 0))) {
			// ダイブ開始へ
			getPlayerPosRotFromBGModel(&ch_pos, &ch_rot, 1);
			VECTOR add_vec;
			add_vec = VGet(0.0f, 12000.0f, 0.0f);
			mTargetAimPos = VAdd(ch_pos, add_vec);

			mRushCount = 0;

			mSubState = eSUB_STATE_DIVE_START;
		}
		else {
			// ダイブせず
			updateTargetAimPosRandom();
			mSubState = eSUB_STATE_RUSH_END;

			mRushCount ++;
		}
	}

	if (mEffHndSmoke == -1) {

		VECTOR posA, posB, normal, hitpos;
		int hitindex; BYTE mat_index;
		GetFramePosFromName(&posA, "頭");
		posB = posA;
		posB.y -= 500.0f;

		if (CollisionManager::GetInstance()->CheckEarthCollision(posA, posB, &hitpos, &normal, &hitindex, &mat_index) != NOT_ON_COLLISION) {

			mEffHndSmoke = EffectManager::GetInstance()->Play(eEF_WHITE_TAIL, &posA, 400.0f, 0.5f);
			EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "ColorRate",			0.36f);
			EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "FireRatio",			0.45f);
			EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "particleSpread",		30.0f);
			EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "particleSystemHeight",30.0f);

			CameraManager::GetInstance()->StartVibeCamera(2.0f, 8.0f, 10.0f);
			SoundManager::GetSndComponent()->PlaySound(eSE_BURST);

			int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST, &posA, 100.0f, 0.0f);
			EffectManager::GetInstance()->Stop(eff_handle, 0.5f);

		}
	}
	



}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::funcSubStateRushEnd(void)
{
	VECTOR vec;
	vec = VSub(mTargetAimPos, mAimPos);
	//	mAimPos = VAdd(mAimPos, VScale(vec, 0.1f) );
	mAimPos = VAdd(mAimPos, VScale(vec, 0.005f) );
	
	//	if (VSize(vec) < 200.0f) {
	if (VSize(vec) < 4000.0f) {
		mMoveSpeed = 90.0f * ( ((float)getHitPointRate() * 0.2f) + 1.0f);
		mSubState = eSUB_STATE_LOITER;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::funcSubStateDiveStart(void)
{
	VECTOR vec;
	vec = VSub(mTargetAimPos, mAimPos);
	mAimPos = VAdd(mAimPos, VScale(vec, 0.05f) );
	
	if ((VSize(vec) < 20.0f) ||
		(mRotationVec.y > 0.99f)) {

		VECTOR ch_pos; float ch_rot;
		getPlayerPosRotFromBGModel(&ch_pos, &ch_rot, 2);

		VECTOR add_vec;
		//		add_vec = VGet(0.0f, -10000.0f, 0.0f);
		add_vec = VGet(0.0f, 0.0f, 0.0f);
		mTargetAimPos = VAdd(ch_pos, add_vec);

		mRotationVec = VGet(0.0f, -1.0f, 0.0f);
		
		mSubState = eSUB_STATE_DIVE_DOING;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::funcSubStateDiveDoing(void)
{
	VECTOR vec;
	vec = VSub(mTargetAimPos, mAimPos);
	mAimPos = VAdd(mAimPos, VScale(vec, 0.05f) );

	if (mRotationVec.y > 0.99f) {
		mRotationVec.y -= 0.05f;
		mRotationVec = VNorm(mRotationVec);
	}
	
	VECTOR posA, posB, normal, hitpos;
	int hitindex; BYTE mat_index;
	GetFramePosFromName(&posA, "頭");
	posB = posA;
	posB.y -= 1000.0f;
	if (CollisionManager::GetInstance()->CheckEarthCollision(posA, posB, &hitpos, &normal, &hitindex, &mat_index) != NOT_ON_COLLISION) {
		mPosition.y -= 200.0f;
		processOpenHole(posA, hitpos);
		CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
		p_player->DisableLockonChara();
	}

	VECTOR tail_node;
	GetFramePosFromName(&tail_node, (char *)strNodeNameArray[10]);

	if (tail_node.y < mHoleModel.Position().y) {
		mMoveSpeed = 90.0f * ( ((float)getHitPointRate() * 0.2f) + 1.0f);
		ChangeState(eCHARA_STATE_ATTACK_6);
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::funcSubStateDiveEnd(void)
{
	


}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::funcSubStateComeoutStart(void)
{
	VECTOR vec;
	vec = VSub(mTargetAimPos, mAimPos);
	mAimPos = VAdd(mAimPos, VScale(vec, 0.05f) );
	
	VECTOR posA, posB, normal, hitpos;
	int hitindex; BYTE mat_index;
	GetFramePosFromName(&posA, "頭");
	posB = posA;
	posB.y += 1000.0f;
	if (CollisionManager::GetInstance()->CheckEarthCollision(posA, posB, &hitpos, &normal, &hitindex, &mat_index) != NOT_ON_COLLISION) {
		mPosition.y += 200.0f;
		processOpenHole(posA, hitpos);
		CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
		p_player->DisableLockonChara();
	}

	if ((VSize(vec) < 20.0f) ||
		(mRotationVec.y > 0.99f)) {

		mAimPos = mPosition;
		updateTargetAimPosRandom();
		mMoveSpeed = 90.0f * ( ((float)getHitPointRate() * 0.2f) + 1.0f);

		mRotationVec.y = 0.0f;
		mRotationVec = VNorm(mRotationVec);

		mSubState = eSUB_STATE_LOITER;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::funcSubStateComeoutDoing(void)
{

}

//---------------------------------------------------
/**
 * キャラの攻撃ステータス遷移のチェック関数
 */
//---------------------------------------------------
bool
CharaEnemyMekaDragon::checkChangeAttackState(int* pAttackCharaState)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	VECTOR vec = VSub(p_player->Position(), mPosition);
	if (VSize(vec) < (mBodySize * 15.0f) + (mBodySize * (GetRand(100) * 0.010f))) {
		if (GetRand(2) == 0) {
			*pAttackCharaState = eCHARA_STATE_ATTACK_1;
			return true;
		}
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::updateRevolveMove(float revolveRateY, float revolveRateX, float moveSpeed)
{
	static const float sJointLength = -5000.0f;
	static const float sJointValue  = -3000.0f;
	float joint_value_y = sJointValue * (revolveRateY - 0.075f);
	float joint_value_x = sJointValue * revolveRateX;

	int frame_index;
	MATRIX inv_mtx = MGetIdent();
	VECTOR base_vec, target_vec, btm_pos, vec;
	GetFramePosFromName(&base_vec, "身体１");
	btm_pos = VAdd(base_vec, VScale(mRotationVec, sJointLength) );

	VECTOR joint_vec, crossY, crossX;
	
	crossY = VScale(mVirtualUpVec, joint_value_y);
	crossX = VScale(VCross(mRotationVec, mVirtualUpVec), joint_value_x);
	joint_vec = VAdd(crossY, crossX);

	target_vec = VAdd(btm_pos, joint_vec);
	vec = VSub(target_vec, base_vec);
	target_vec = VAdd(base_vec, VScale(vec, -1.0f) );

	SetFrameUserRotation((char *)strNodeNameArray[0], &target_vec, &mVirtualUpVec, 0.1f, &inv_mtx);
	for (int iI = 1; iI < sNodeNameNum; iI ++) {
		target_vec = VAdd(target_vec, joint_vec);
		frame_index = GetFrameIndexFromName(strNodeNameArray[iI]);
		SetFrameUserRotationEx(frame_index, target_vec, mVirtualUpVec, 0.5f, inv_mtx);
	}

	VECTOR head_pos, tail_pos, move_vec;
	GetFramePosFromName(&head_pos, "頭");
	GetFramePosFromName(&tail_pos, "尻尾先");

	VECTOR incline_vec, height_vec;
	incline_vec = VSub(head_pos, tail_pos);
	height_vec = VSub(tail_pos, btm_pos);

	move_vec = VAdd(incline_vec, VScale(height_vec, 2) );

	float rotvec_rate;
	rotvec_rate = 0.050f * (mMoveSpeed / 60.0f);

	mRotationVec = VAdd(VScale(mRotationVec, (1.0f - rotvec_rate)), VScale(VNorm(move_vec), rotvec_rate) );
	mPosition = VAdd(mPosition, VScale(VNorm(move_vec), moveSpeed) );

	// 下限の補正
	VECTOR ch_pos; float ch_rot;
	getPlayerPosRotFromBGModel(&ch_pos, &ch_rot, 2);
	if (mPosition.y < ch_pos.y) {
		mPosition.y = ch_pos.y;
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::updateTargetAimPosRandom(void)
{
	VECTOR ch_pos; float ch_rot;
	getPlayerPosRotFromBGModel(&ch_pos, &ch_rot, 1);
	VECTOR add_vec = VGet(8932.00, 2523.00, 0.00);
	MATRIX tmp_mtx;
	tmp_mtx = MGetRotY( (Utility::DegToRad(90.0f) * (float)GetRand(3) ) );
	add_vec = VTransformSR(add_vec, tmp_mtx);
	mTargetAimPos = VAdd(ch_pos, add_vec);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::updateRotationToPlayer(float rate)
{
	VECTOR next_vec;
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	next_vec = VNorm(VSub(p_player->Position(), mPosition) );
	next_vec.y *= 0.3f; // 上方向にはあまり向いてほしくないので補正をかける
	mRotationVec = VNorm(VAdd(VScale(next_vec, 1.0f), VScale(mRotationVec, 1.0f / rate )));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::processOpenHole(VECTOR& effPos, VECTOR& hitPos)
{
	CameraManager::GetInstance()->StartVibeCamera(4.0f, 10.0f, 16.0f);
	SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
	SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
		
	int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST, &effPos, 160.0f, 0.0f);
	EffectManager::GetInstance()->Stop(eff_handle, 0.5f);

	float ch_rot;
	getPlayerPosRotFromBGModel(&hitPos, &ch_rot, 2);

	mPiecesModel.SetVisible(true);
	mPiecesModel.SetAlpha(1.0f);
	mPiecesModel.Position() = hitPos;
	mPiecesModel.Position().y -= 30.0f;
	mPiecesModel.Scale() = VGet(24.0f, 24.0f, 24.0f);

	mHoleModel.SetVisible(true);
	mHoleModel.Position() = hitPos;
	mHoleModel.Position().y -= 30.0f;
	mHoleModel.Scale() = VGet(24.0f, 24.0f, 24.0f);
	MV1SetMeshVisible(mHoleModel.GetModelHandle(), 3, FALSE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaEnemyMekaDragon::eHpRate
CharaEnemyMekaDragon::getHitPointRate(void)
{
	float rate = (float)mHitpoint / (float)mMaxHitpoint;

	if (rate > 0.65f) {
		return eHP_RATE_HIGH;
	} else if (rate > 0.35f) {
		return eHP_RATE_MIDDLE;
	}
	return eHP_RATE_LOW;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
CharaEnemyMekaDragon::getMotorVolume(void)
{
	static const float judge_dist_rush_end = 20000.0f;

	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	float dist_to_player = VSize(VSub(mPosition, p_player->Position() ) );
	int volume = (255 - (int)( (dist_to_player / (judge_dist_rush_end / 1.2f) ) * 255.0f) );
	if (volume < 0)		volume = 0;
	if (volume > 255)	volume = 255;

	return volume;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::startRockBarrier(void)
{
	mBarrierWork.mIsActive	= TRUE;
	mBarrierWork.mIsAppear	= TRUE;
	mBarrierWork.mHeight	= -2000.0f;

	GimmickBase* p_gimmick = GimmickBase::Search1stFindGimmickKind(eGIMMICK_KIND_BARRIER);
	APP_ASSERT((p_gimmick != NULL),
			   "Failed Search1stFindGimmickKind() \n");
	p_gimmick->Scale() = VGet(10.0f, 20.0f, 10.0f);
	p_gimmick->Rotation().y = atan2(mRotationVec.x, mRotationVec.z);
	p_gimmick->Position() = VAdd(mHoleModel.Position(), VScale(mRotationVec, -1000.0f) );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaEnemyMekaDragon::endRockBarrier(void)
{
	mBarrierWork.mIsActive	= TRUE;
	mBarrierWork.mIsAppear	= FALSE;
	mBarrierWork.mHeight	= -10.0f;
}

 /**** end of file ****/


