/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * オブジェクトベースクラス
 * @author SPATZ.
 * @data   2012/04/14 19:35:52
 */
//---------------------------------------------------
#include "ObjectWeapon.h"
#include "AppLib/Basic/Basic.h"
#include "DataDefine/MotionDef.h"
#include "../Charactor/CharaPlayer.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/Shape.h"
#include "AppLib/Graphic/EffekseerManager.h"
#include "AppLib/Effect/EffectManager.h"

#include "AppLib/Collision/CollisionManager.h"
#include "AppLib/Resource/ResourceModel.h"

#include "AppLib/Sound/SoundManager.h"

#include "Scene/ActionTaskManager.h"
#include "Scene/SceneInfo.h"
#include "Scene/CommandFunctions.h"

#include "Gimmick/GimmickBase.h"

#include "Utility/Utility.h"

#include "DataDefine/EffectDef.h"
#include "DataDefine/VoiceDef.h"

//#include "CharaInfo.h"

/*=====================================*
 * static
 *=====================================*/
/*=====================================*
 * class ObjectWeapon
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectWeapon::ObjectWeapon()
{
	mState		= 0;
	mMoveVec	= ZERO_VEC;
	mGravity	= 0.0f;
	mWeaponType = eWEAPON_TYPE_ERROR;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectWeapon::~ObjectWeapon()
{
	if (GraphicManager::GetInstance()->IsUpdateDrawModelVector()) {
		GraphicManager::GetInstance()->ReserveRemoveModel(this);
	}
	else {
		GraphicManager::GetInstance()->RemoveDrawModel(this);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg)
{
	ObjectBase::Setup(objectIdx, itemIdx, pSetPos, rotDeg);
	mObjectType = eOBJECT_TYPE_WEAPON;

	//	this->StartMotion(0, false, 0.5f);

	mState		= 0;
	mMoveVec	= ZERO_VEC;
	mGravity	= 0.0f;

	mWeaponType = eWEAPON_TYPE_ERROR;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon::Update(void)
{
	ObjectBase::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ObjectWeapon::GetIsRetire(void)
{
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon::SetUsed(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
UCHAR&
ObjectWeapon::State(void)
{
	return mState;
}

/*=====================================*
 * class ObjectWeapon_Arrow
 *=====================================*/
/*static*/
ObjectWeapon_Arrow::PHASE_WEAPON_ARROW_STATE
ObjectWeapon_Arrow::mPhaseState[ObjectWeapon_Arrow::eWEAPON_ARROW_STATE_MAX] = {
	&ObjectWeapon_Arrow::phaseArrowStateInArchery,
	&ObjectWeapon_Arrow::phaseArrowStateShooting,
	&ObjectWeapon_Arrow::phaseArrowStateStick,
};
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectWeapon_Arrow::ObjectWeapon_Arrow()
{
	mPosition 	= ZERO_VEC;
	mTopPos 	= ZERO_VEC;
	
	mEffIndex = -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Arrow::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg, bool isRaise/*=false*/)
{
	ObjectWeapon::Setup(objectIdx, itemIdx, pSetPos, rotDeg);
	mWeaponType = eWEAPON_TYPE_ARROW;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Arrow::Update(void)
{
	APP_ASSERT_PRINT((mState < eWEAPON_ARROW_STATE_MAX),
					 "Select mState %d is Invalid value \n", mState);

	(this->*mPhaseState[mState])();

	ObjectWeapon::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ObjectWeapon_Arrow::GetIsRetire(void)
{
	if (mState == eWEAPON_ARROW_STATE_SHOOTING) {
		return false;
	}
	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Arrow::ProcessSetArchery(void)
{
	mScale.x = ObjectBase::GetObjectBaseParam(eOBJECT_KIND_ARROW)->mScale;
	mScale.y = mScale.x;
	mScale.z = mScale.x;

	GraphicManager::GetInstance()->ReserveEntryModel(this);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void 
ObjectWeapon_Arrow::ProcessShoot(VECTOR* pBottomPos, VECTOR* pTopPos)
{
	mScale = VGet(20.0f, 20.0f, 20.0f);

	mPosition 	= *pBottomPos;
	mTopPos		= *pTopPos;
	mGravity	= 0.0f;
	
	mEffIndex = EffectManager::GetInstance()->Play(eEF_ARROW,
												   (VECTOR *)&ZERO_VEC,
												   (VECTOR *)&ZERO_VEC,
												   0.0f);
	EffectManager::GetInstance()->SetMatrixType(mEffIndex, EffectNode::eMATRIX_TYPE_MATRIX);

	MATRIX mtx, tmp;

	mtx = MGetIdent();
	// 拡縮行列
	tmp = MGetIdent();
	tmp = MGetScale(VGet(mScale.x, mScale.y, mScale.z));
	mtx = MMult(mtx, tmp);
	
	VECTOR vecZ, crossY, crossX;
	vecZ = VNorm(VSub(mTopPos, mPosition));
	crossX = VCross(vecZ, VGet(0.0f, 1.0f, 0.0f));
	crossY = VCross(crossX, vecZ);

	// 回転行列
	tmp = MGetIdent();
	Utility::MTXLookAtMatrix(&tmp,
							 &mTopPos,
							 &mPosition,
							 //							 &VGet(0.0f, 1.0f, 0.0f));
							 &crossY);
	mtx = MMult(mtx, tmp);
	
	// 移動行列
	tmp = MGetIdent();
	tmp = MGetTranslate(mPosition);
	mtx = MMult(mtx, tmp);

	mMatrix = mtx;

	mState = eWEAPON_ARROW_STATE_SHOOTING;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Arrow::AddArrowMoveVec(VECTOR& addVec)
{
	mTopPos = VAdd(mTopPos, addVec);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Arrow::phaseArrowStateInArchery(void)
{
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Arrow::phaseArrowStateShooting(void)
{
	mMoveVec = VSub(mTopPos, mPosition);
	mMoveVec = VScale(VNorm(mMoveVec), 160.0f);

	bool is_hit = false;

	VECTOR add_top_pos;
	VECTOR hitpos, normal;

	add_top_pos = VAdd(mTopPos, mMoveVec);

	mGravity += -0.0054f;

	// プレイヤーとの距離が離れすぎた場合、自動で消す
	ModelBase* p_player = CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	if (VSize(VSub(p_player->Position(), add_top_pos) ) > 50000.0f) {
		mState = eWEAPON_ARROW_STATE_STICK;
	}

	INT8 on_earth_idx = -1;
	on_earth_idx = CollisionManager::GetInstance()->CheckEarthCollision(add_top_pos, mTopPos, &hitpos, &normal);
	
	// 壁か地面に矢に当たった場合
	if ((on_earth_idx != NOT_ON_COLLISION) ||
		(CollisionManager::GetInstance()->CheckWallCollision(add_top_pos, mTopPos, &hitpos, &normal))) {
		is_hit = true;
	}
	
	/* 攻撃の処理 */
	ModelBase* p_mine = this;
	CharaBase* p_your = NULL;
	for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
		p_your = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
		if (p_mine == (ModelBase *)p_your) continue;			// 自分はのぞく
		if (p_your == p_player) continue;		// 主人公に対しても当らない
		if (p_your->GetIsRetire()) continue;	// 戦闘不能者も除く
		if (p_your->IsDamage()) continue;		// 既に攻撃された者も除く


		tCheckDamageWork param;
		param.mPlayerPos = mPosition;	//this->mPosition;
		param.mWeaponTopPos = mPosition;
		param.mWeaponBtmPos = VAdd(mPosition, mMoveVec);

		if (p_your->CheckDamageCollArrow(&param)) {
			int attackPower = 1;

			is_hit = true;

			// 本来の処理
			VECTOR eff_pos = mPosition;
			VECTOR damage_down_vec;
			damage_down_vec = VSub(p_your->Position(), mPosition);
			damage_down_vec = VNorm(damage_down_vec);
			p_your->ProcessDamage(attackPower, false, &eff_pos, &damage_down_vec, false, eATTACK_TYPE_ARROW);

			// スケールを０にして隠す
			mScale = ZERO_VEC;
		}
	}

	// 
	if (is_hit) {
		SoundManager::GetSndComponent()->PlaySound(eSE_HIT_ARROW);

		float size = 30.0f;
		if (VSize(VSub(hitpos, p_player->Position())) > 5000.0f) {
			size = 50.0f;
		}
		int eff_handle = EffectManager::GetInstance()->Play(eEF_HIT,
															&hitpos,
															//															&add_top_pos,
															&VGet(size, size, size),
															0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 0.75f);
		EffectManager::GetInstance()->SetMatrixType(mEffIndex, EffectNode::eMATRIX_TYPE_TRS);
		EffectManager::GetInstance()->Stop(mEffIndex, 0.5f);
		mEffIndex = -1;

		// 座標更新
		mPosition 	= VAdd(mPosition, 	mMoveVec);
		mPosition.y += mGravity * 0.5f;
		mTopPos 	= add_top_pos;
		mTopPos.y += mGravity;

		// 突き刺さりすぎるので少し後ろに戻す
		VECTOR vec = VSub(mPosition, mTopPos);
		mPosition	= VAdd(mPosition,	VScale(vec, 0.75f) );
		mTopPos		= VAdd(mTopPos,		VScale(vec, 0.75f) );

		// ギミックに突き刺さった場合は非表示にする
		GimmickBase* p_gimmick = NULL;
		p_gimmick = (GimmickBase *)GimmickBase::GetGimmickBaseFromEarthCollIndex(on_earth_idx);
		if (p_gimmick != NULL) {
			GraphicManager::GetInstance()->ReserveRemoveModel(this);
		}

		mState = eWEAPON_ARROW_STATE_STICK;
	} else {
		// 座標更新
		mPosition 	= VAdd(mPosition, 	mMoveVec);
		mPosition.y += mGravity * 0.5f;
		mTopPos 	= add_top_pos;
		mTopPos.y += mGravity;
	}
	
	
	MATRIX mtx, tmp;

	mtx = MGetIdent();
	// 拡縮行列
	tmp = MGetIdent();
	tmp = MGetScale(VGet(mScale.x, mScale.y, mScale.z));
	mtx = MMult(mtx, tmp);
	
	VECTOR vecZ, crossY, crossX;
	vecZ = VNorm(VSub(mTopPos, mPosition));
	crossX = VCross(vecZ, VGet(0.0f, 1.0f, 0.0f));
	crossY = VCross(crossX, vecZ);

	// 回転行列
	tmp = MGetIdent();
	Utility::MTXLookAtMatrix(&tmp,
							 &mTopPos,
							 &mPosition,
							 //							 &VGet(0.0f, 1.0f, 0.0f));
							 &crossY);
	mtx = MMult(mtx, tmp);
	
	// 移動行列
	tmp = MGetIdent();
	tmp = MGetTranslate(mPosition);
	mtx = MMult(mtx, tmp);

	mMatrix = mtx;
	
	if (mEffIndex != -1) {
		EffectManager::GetInstance()->Matrix(mEffIndex) = mMatrix;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Arrow::phaseArrowStateStick(void)
{
	if (mEffIndex != -1) {
		EffectManager::GetInstance()->Stop(mEffIndex, 0.5f);
		mEffIndex = -1;
	}
}

/*=====================================*
 * class ObjectWeapon_Meteo
 *=====================================*/
/*static*/
ObjectWeapon_Meteo::PHASE_WEAPON_METEO_STATE
ObjectWeapon_Meteo::mPhaseState[ObjectWeapon_Meteo::eWEAPON_METEO_STATE_MAX] = {
	&ObjectWeapon_Meteo::phaseMeteoStateNone,
	&ObjectWeapon_Meteo::phaseMeteoStateFloatUp,
	&ObjectWeapon_Meteo::phaseMeteoStateFloating,
	&ObjectWeapon_Meteo::phaseMeteoStateCharge,
	&ObjectWeapon_Meteo::phaseMeteoStateShooting,
	&ObjectWeapon_Meteo::phaseMeteoStateDisappear,
	&ObjectWeapon_Meteo::phaseMeteoStateBreaked,
};
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectWeapon_Meteo::ObjectWeapon_Meteo()
{
	mEffIndex = -1;
	mIsRefrect = false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectWeapon_Meteo::~ObjectWeapon_Meteo()
{
	// 当たり判定用を解除
	CollisionManager::GetInstance()->RemoveAttackCollision(this);

	// 煙エフェクトの処理
	if (mEffIndex != -1) {
		EffectManager::GetInstance()->Stop(mEffIndex, 1.0f);
		mEffIndex = -1;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Meteo::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg, bool isRaise/*=false*/)
{
	ObjectWeapon::Setup(objectIdx, itemIdx, pSetPos, rotDeg);
	mWeaponType = eWEAPON_TYPE_METEO;

	// 当たり判定用と登録
	CollisionManager::GetInstance()->EntryAttackCollision(this);

	// 自己発光を抑制する
	this->SetEnableEmissive(false);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Meteo::Update(void)
{
	APP_ASSERT_PRINT((mState < eWEAPON_METEO_STATE_MAX),
					 "Select mState %d is Invalid value \n", mState);
	(this->*mPhaseState[mState])();

	ObjectWeapon::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Meteo::ProcessFloat(VECTOR& rStartPos)
{
	mPosition = rStartPos;
	mMoveVec = ZERO_VEC;

	mScale.x = ObjectBase::GetObjectBaseParam(eOBJECT_KIND_METEO)->mScale;
	mScale.y = mScale.x;
	mScale.z = mScale.x;

	GraphicManager::GetInstance()->ReserveEntryModel(this);

	mState = eWEAPON_METEO_STATE_FLOAT_UP;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Meteo::ProcessCharge(void)
{
	// 既に破壊されている場合は即リターン
	if (GetIsRetire()) {
		return;
	}

	mState = eWEAPON_METEO_STATE_CHARGE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void 
ObjectWeapon_Meteo::ProcessShoot(VECTOR& rMoveVec)
{
	// 既に破壊されている場合は即リターン
	if (GetIsRetire()) {
		return;
	}

	mGravity	= 0.0f;
	mMoveVec = rMoveVec;
	mState = eWEAPON_METEO_STATE_SHOOTING;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ObjectWeapon_Meteo::GetIsRetire(void)
{
	if ((mState == eWEAPON_METEO_STATE_DISAPPEAR) ||
		(mState == eWEAPON_METEO_STATE_BREAKED)) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Meteo::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
	{
		//		const float size = 15.0f * 7.5f;
		const float size = 15.0f * 5.0f;
		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,
															&mPosition,
															&VGet(size, size, size),
															0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 5.0f);
	}

	{
		EFK_HANDLE efk_hdl;

		//		efk_hdl = EffekseerManager::GetInstance()->Play(eEFK_BREAK_LOCK, &mPosition, &VScale(mScale, 1.0f));
		efk_hdl = EffekseerManager::GetInstance()->Play(eEFK_BREAK_LOCK, &mPosition, &VScale(mScale, 0.75f));
		EffekseerManager::GetInstance()->SetSpeed(efk_hdl, 2.0f);
	}
		
	GraphicManager::GetInstance()->ReserveRemoveModel(this);
		
	mState = eWEAPON_METEO_STATE_BREAKED;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ObjectWeapon_Meteo::CheckDamageCollArrow(tCheckDamageWork* pParam)
{
	tCheckDamageWork* p_work = (tCheckDamageWork *)pParam;

	ShapeCapsule capsule;
	ShapeLine	line;

	line.mStart = p_work->mWeaponBtmPos;
	line.mEnd	= p_work->mWeaponTopPos;

	capsule.mPosA = mPosition;
	capsule.mPosB = mPosition;
	capsule.mPosB.y += 1.0f;
	capsule.mRadius = 400.0f;

	if (CollisionManager::GetInstance()->Check_LineToCapsule(&line, &capsule)) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Meteo::phaseMeteoStateNone(void)
{
	return;
}


	float end_pos = 12000.0f;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Meteo::phaseMeteoStateFloatUp(void)
{

	mMoveVec.y = (end_pos - mPosition.y) * 0.08f;

	mPosition = VAdd(mPosition, mMoveVec);

	if (abs(mMoveVec.y) < 10.0f) {
		mPosition.y = end_pos;
		mRotation.y = 0.0f;
		mState = eWEAPON_METEO_STATE_FLOATING;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Meteo::phaseMeteoStateFloating(void)
{
	float next_pos;
	next_pos = end_pos + (cos(mRotation.z * 5.0f) * 400.0f) - 400.0f;

	mPosition.y = (next_pos * 1.0f + mPosition.y * 9.0f) * 0.1f;
	
	mRotation.z += 0.02f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Meteo::phaseMeteoStateCharge(void)
{
	float next_pos;
	next_pos = end_pos + (cos(mRotation.z * 5.0f) * 400.0f) - 400.0f;

	mPosition.y = (next_pos * 1.0f + mPosition.y * 9.0f) * 0.1f;
	
	mRotation.z += 0.5f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Meteo::phaseMeteoStateShooting(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	VECTOR add_top_pos;
	VECTOR hitpos, normal;
	bool is_hit = false;

	add_top_pos = VAdd(mPosition, mMoveVec);

	// 壁か地面に当たった場合
	if ((CollisionManager::GetInstance()->CheckEarthCollision(add_top_pos, mPosition, &hitpos, &normal) != NOT_ON_COLLISION) ||
		(CollisionManager::GetInstance()->CheckWallCollision(add_top_pos, mPosition, &hitpos, &normal))) {

		// ドロップアイテム生成
		createDropItem(eOBJECT_KIND_PLATE_ARROW, mPosition);

		is_hit = true;
	}

	CollisionManager::tSphereWork sphere;
	sphere.mPosition = mPosition;
	sphere.mSize = 400.0f;
#ifdef _DEBUG
	ShapeCapsule capsule;
	capsule.mPosA = sphere.mPosition;
	capsule.mPosB = sphere.mPosition;
	capsule.mPosB.y += 1.0f;
	capsule.mRadius = sphere.mSize;
	GraphicManager::GetInstance()->DbgRenderCapsule(&capsule);
#endif // _DEBUG

	CharaBase* p_your = NULL;
	for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
		p_your = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
		if ((ModelCollision *)this == (ModelCollision *)p_your) continue;			// 自分はのぞく
		if (p_your->GetIsRetire()) continue;	// 戦闘不能者も除く
		if (p_your->IsDamage()) continue;		// 既に攻撃された者も除く

		CollisionManager::tPillarWork pillar;
		pillar.mPosition = p_your->Position();
		pillar.mSize = p_your->BodySize();
		pillar.mHeight = p_your->BodyHeight();

		// プレイヤーと岩がぶつかった場合
		if (CollisionManager::Check_SphereToPillar(&sphere, &pillar)) {

			VECTOR damage_down_vec;
			damage_down_vec = VSub(p_your->Position(), mPosition);
			damage_down_vec = VNorm(damage_down_vec);
			p_your->ProcessDamage(2, false, NULL, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);

			is_hit = true;
		}

		if (is_hit) {
			SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
			const float size = 15.0f * 5.0f;
			int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,
																&mPosition,
																&VGet(size, size, size),
																0.0f);
			EffectManager::GetInstance()->Stop(eff_handle, 5.0f);
		
			if (mEffIndex != -1) {
				EffectManager::GetInstance()->Stop(mEffIndex);
				mEffIndex = -1;
			}

			GraphicManager::GetInstance()->ReserveRemoveModel(this);
		
			mState = eWEAPON_METEO_STATE_DISAPPEAR;
			return;
		}
	}

	mRotation.z += 0.6f;

	mPosition = VAdd(mPosition, mMoveVec);
	mMoveVec = VScale(mMoveVec, 1.01f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Meteo::phaseMeteoStateDisappear(void)
{
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Meteo::phaseMeteoStateBreaked(void)
{

}

/*=====================================*

 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectWeapon_MeteoRefrect::ObjectWeapon_MeteoRefrect()
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_MeteoRefrect::Update(void)
{
	ObjectWeapon_Meteo::Update();
	
	if (mEffIndex != -1){
		EffectManager::GetInstance()->Position(mEffIndex) = mPosition;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_MeteoRefrect::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg, bool isRaise/*=false*/)
{
	COLOR_F color = GetColorF(1.0f, 0.01f, 0.5f, 1.0f);

	ObjectWeapon_Meteo::Setup(objectIdx, itemIdx, pSetPos, rotDeg, isRaise);

	// マテリアルカラーを変更
	MV1SetMaterialDifColor(mModelHandle, 0, color);
	MV1SetMaterialSpcColor(mModelHandle, 0, color);
	MV1SetMaterialAmbColor(mModelHandle, 0, color);
	MV1SetMaterialEmiColor(mModelHandle, 0, color);

	MV1SetUseVertDifColor(mModelHandle, FALSE);
	MV1SetUseVertSpcColor(mModelHandle, FALSE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_MeteoRefrect::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	VECTOR move_vec;

//	move_vec = VScale(mMoveVec, -1.0f);
	move_vec = VScale(*pDamageDownVec, 100.0f);
	ProcessShoot(move_vec);
	mIsRefrect = true;

	SoundManager::GetSndComponent()->PlaySound(eSE_REFLECT);
	const float size = 15.0f * 5.0f;
	int eff_handle = EffectManager::GetInstance()->Play(eEF_HIT,
														&mPosition,
														&VGet(size, size, size),
														0.0f);
	EffectManager::GetInstance()->Stop(eff_handle, 0.75f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void 
ObjectWeapon_MeteoRefrect::ProcessStarEffect(void)
{
	mMoveVec = ZERO_VEC;

	mScale.x = ObjectBase::GetObjectBaseParam(eOBJECT_KIND_METEO)->mScale;
	mScale.y = mScale.x;
	mScale.z = mScale.x;

	GraphicManager::GetInstance()->ReserveEntryModel(this);

	static const float size = 1000.0f;
	mEffIndex = EffectManager::GetInstance()->Play(eEF_ENERGY_BALL_R,
												   &mPosition,
												   &VGet(size, size, size),
												   1.0f);
	mState = eWEAPON_METEO_STATE_NONE;
}


/*=====================================*
 * class ObjectWeapon_Missile
 *=====================================*/
/*static*/
ObjectWeapon_Missile::PHASE_WEAPON_MISSILE_STATE
ObjectWeapon_Missile::mPhaseState[ObjectWeapon_Missile::eWEAPON_MISSILE_STATE_MAX] = {
	&ObjectWeapon_Missile::phaseMissileStateNone,
	&ObjectWeapon_Missile::phaseMissileStateShooting,
	&ObjectWeapon_Missile::phaseMissileStateDisappear,
	&ObjectWeapon_Missile::phaseMissileStateBreaked,
};
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectWeapon_Missile::ObjectWeapon_Missile()
{
	mEffHndSmoke = -1;
	mpMaster = NULL;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectWeapon_Missile::~ObjectWeapon_Missile()
{
	// 当たり判定用を解除
	CollisionManager::GetInstance()->RemoveAttackCollision(this);

	// 煙エフェクトの処理
	if (mEffHndSmoke != -1) {
		EffectManager::GetInstance()->Stop(mEffHndSmoke, 1.0f);
		mEffHndSmoke = -1;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Missile::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg, ModelBase* pMaster, bool isRaise/*=false*/)
{
	ObjectWeapon::Setup(objectIdx, itemIdx, pSetPos, rotDeg);
	mWeaponType = eWEAPON_TYPE_MISSILE;

	// 当たり判定用と登録
	CollisionManager::GetInstance()->EntryAttackCollision(this);

	mpMaster = pMaster;
	APP_ASSERT((mpMaster != NULL),
			   "mpMaster is NULL \n");
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Missile::Update(void)
{
	APP_ASSERT_PRINT((mState < eWEAPON_MISSILE_STATE_MAX),
					 "Select mState %d is Invalid value \n", mState);
	(this->*mPhaseState[mState])();

	ObjectWeapon::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Missile::ProcessShoot(VECTOR& rStartPos, VECTOR& rMoveVec)
{
	/*
	// 既に破壊されている場合は即リターン
	if (GetIsRetire()) {
		return;
	}
	*/

	mPosition	= rStartPos;
	mMoveVec	= rMoveVec;
	mGravity	= 0.0f;

	float scale = 5;
	mScale = VGet(scale, scale, scale);

	GraphicManager::GetInstance()->ReserveEntryModel(this);

	// 煙エフェクトの処理
	if (mEffHndSmoke != -1) {
		EffectManager::GetInstance()->Stop(mEffHndSmoke, 1.0f);
		mEffHndSmoke = -1;
	}
	mEffHndSmoke = EffectManager::GetInstance()->Play(eEF_REALFIRE, &mPosition, 70.0f, 0.2f);
	EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "FireRatio",			0.18f);
	EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "particleSpeed",		10000.0f);
	EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "particleSpread",		3.0f);
	EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "particleSystemHeight",0.0f);

	mState = eWEAPON_MISSILE_STATE_SHOOTING;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ObjectWeapon_Missile::GetIsRetire(void)
{
	if ((mState == eWEAPON_MISSILE_STATE_DISAPPEAR) ||
		(mState == eWEAPON_MISSILE_STATE_BREAKED)) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Missile::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
	{
		const float size = 15.0f * 5.0f;
		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,
															&mPosition,
															&VGet(size, size, size),
															0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 5.0f);
	}

	{
		EFK_HANDLE efk_hdl;

		efk_hdl = EffekseerManager::GetInstance()->Play(eEFK_BREAK_LOCK, &mPosition, &VScale(mScale, 0.75f));
		EffekseerManager::GetInstance()->SetSpeed(efk_hdl, 2.0f);
	}
		
	if (mEffHndSmoke != -1) {
		EffectManager::GetInstance()->Stop(mEffHndSmoke);
		mEffHndSmoke = -1;
	}

	GraphicManager::GetInstance()->ReserveRemoveModel(this);
		
	mState = eWEAPON_MISSILE_STATE_BREAKED;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ObjectWeapon_Missile::CheckDamageColl(tCheckDamageWork* pParam)
{
	tCheckDamageWork* p_work = (tCheckDamageWork *)pParam;

	ShapeCapsule capsule;
	ShapeLine	line;

	line.mStart = p_work->mWeaponBtmPos;
	line.mEnd	= p_work->mWeaponTopPos;

	capsule.mPosA = mPosition;
	capsule.mPosB = mPosition;
	capsule.mPosB.y += 1.0f;
	capsule.mRadius = 400.0f;

	if (CollisionManager::GetInstance()->Check_LineToCapsule(&line, &capsule)) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Missile::phaseMissileStateNone(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Missile::phaseMissileStateShooting(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	VECTOR add_top_pos;
	VECTOR hitpos, normal;
	bool is_hit = false;

	add_top_pos = VAdd(mPosition, mMoveVec);

	// 煙エフェクトの処理
	if (mEffHndSmoke != -1) {
		EffectManager::GetInstance()->Position(mEffHndSmoke) = add_top_pos;
		EffectManager::GetInstance()->SetFloat(mEffHndSmoke, "particleSpeed", 1.5f);
	}

	// 壁か地面に当たった場合
	if ((CollisionManager::GetInstance()->CheckEarthCollision(add_top_pos, mPosition, &hitpos, &normal) != NOT_ON_COLLISION) ||
		(CollisionManager::GetInstance()->CheckWallCollision(add_top_pos, mPosition, &hitpos, &normal))) {

		is_hit = true;
	}

	CollisionManager::tSphereWork sphere;
	sphere.mPosition = mPosition;
	sphere.mSize = mScale.x * 6.0f;
#ifdef _DEBUG
	ShapeCapsule capsule;
	capsule.mPosA = sphere.mPosition;
	capsule.mPosB = sphere.mPosition;
	capsule.mPosB.y += 1.0f;
	capsule.mRadius = sphere.mSize;
	GraphicManager::GetInstance()->DbgRenderCapsule(&capsule);
#endif // _DEBUG

	CharaBase* p_your = NULL;
	for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
		p_your = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
		if ((ModelCollision *)this == (ModelCollision *)p_your) continue;			// 自分はのぞく
		if (p_your->GetIsRetire()) continue;	// 戦闘不能者も除く
		if (p_your->IsDamage()) continue;		// 既に攻撃された者も除く
		if (p_your == mpMaster) continue;		// 発射主の場合も除く

		CollisionManager::tPillarWork pillar;
		pillar.mPosition = p_your->Position();
		pillar.mSize = p_your->BodySize();
		pillar.mHeight = p_your->BodyHeight();

		// プレイヤーと岩がぶつかった場合
		if (CollisionManager::Check_SphereToPillar(&sphere, &pillar)) {

			VECTOR damage_down_vec;
			damage_down_vec = VSub(p_your->Position(), mPosition);
			damage_down_vec = VNorm(damage_down_vec);
			p_your->ProcessDamage(3, false, NULL, &damage_down_vec, true, eATTACK_TYPE_MISSILE);

			is_hit = true;
		}

		if (is_hit) {
			SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
			//			SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
			const float size = 15.0f * 5.0f;
			int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,
																&mPosition,
																&VGet(size, size, size),
																0.0f);
			EffectManager::GetInstance()->Stop(eff_handle, 5.0f);

			// 煙エフェクトの処理
			if (mEffHndSmoke != -1) {
				EffectManager::GetInstance()->Stop(mEffHndSmoke, 0.5f);
				mEffHndSmoke = -1;
			}

			GraphicManager::GetInstance()->ReserveRemoveModel(this);
		
			mState = eWEAPON_MISSILE_STATE_DISAPPEAR;
			return;
		}
	}

	mRotation.z += 0.6f;

	mPosition = VAdd(mPosition, mMoveVec);
	mMoveVec = VScale(mMoveVec, 1.01f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Missile::phaseMissileStateDisappear(void)
{
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_Missile::phaseMissileStateBreaked(void)
{

}


/*=====================================*
 * class ObjectWeapon_SonicWave
 *=====================================*/
/*static*/
ObjectWeapon_SonicWave::PHASE_WEAPON_SONIC_WAVE_STATE
ObjectWeapon_SonicWave::mPhaseState[ObjectWeapon_SonicWave::eWEAPON_SONIC_WAVE_STATE_MAX] = {
	&ObjectWeapon_SonicWave::phaseSonicWaveStateAttach,
	&ObjectWeapon_SonicWave::phaseSonicWaveStateShooting,
	&ObjectWeapon_SonicWave::phaseSonicWaveStateDisappear,
};
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectWeapon_SonicWave::ObjectWeapon_SonicWave()
{
	mEfkHandle	= -1;
	mState = eWEAPON_SONIC_WAVE_STATE_ATTACH;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectWeapon_SonicWave::~ObjectWeapon_SonicWave()
{
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
ObjectWeapon_SonicWave::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg, bool isRaise/*=false*/)
{
	ObjectWeapon::Setup(objectIdx, itemIdx, pSetPos, rotDeg);
	mWeaponType = eWEAPON_TYPE_SONIC_WAVE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_SonicWave::Update(void)
{
	APP_ASSERT_PRINT((mState < eWEAPON_SONIC_WAVE_STATE_MAX),
					 "Select mState %d is Invalid value \n", mState);

	(this->*mPhaseState[mState])();

	ObjectWeapon::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_SonicWave::ProcessCreateSonick(MATRIX* pMatrix)
{
	if (mEfkHandle != -1) {
		EffekseerManager::GetInstance()->Stop(mEfkHandle, 0.0f);
		mEfkHandle = -1;
	}
	mEfkHandle = EffekseerManager::GetInstance()->PlayMtx(eEFK_SONICK_WAVE, pMatrix);

	// 自分で追加する
	GraphicManager::GetInstance()->ReserveEntryModel(this);

	mState = eWEAPON_SONIC_WAVE_STATE_ATTACH;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_SonicWave::SetSonicWaveMatrix(MATRIX* pMatrix)
{
	mMatrix = *pMatrix;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_SonicWave::ProcessShoot(VECTOR* pMoveVec, float setScale, float hitSize)
{
	mMoveVec	= *pMoveVec;
	mScale		= VGet(setScale, setScale, setScale);
	mBodySize	= hitSize;

	Utility::ExtractMatrixPosition(&mPosition, &mMatrix);

	mState = eWEAPON_SONIC_WAVE_STATE_SHOOTING;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_SonicWave::ProcessDisappear(void)
{
	if (mEfkHandle != -1) {
		EffekseerManager::GetInstance()->Stop(mEfkHandle, 0.0f);
		mEfkHandle = -1;
	}
	mState = eWEAPON_SONIC_WAVE_STATE_DISAPPEAR;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_SonicWave::phaseSonicWaveStateAttach(void)
{
	EffekseerManager::GetInstance()->SetMatrix(mEfkHandle, &mMatrix);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_SonicWave::phaseSonicWaveStateShooting(void)
{
	VECTOR node_pos;
	Utility::ExtractMatrixPosition(&node_pos, &mMatrix);

	// 敵のノードとキャラの円柱の当たり判定
	CollisionManager::tSphereWork sphere;
	sphere.mPosition= node_pos;
	sphere.mSize	= mBodySize;
		
	CollisionManager::tPillarWork piller;

	// 壁か地面に矢に当たった場合
	VECTOR next_pos, hitpos, normal;
	next_pos = VAdd(node_pos, mMoveVec);
	mMoveVec = VScale(mMoveVec, 1.05f);

	if ((CollisionManager::GetInstance()->CheckEarthCollision(node_pos, next_pos, &hitpos, &normal) != NOT_ON_COLLISION) ||
		(CollisionManager::GetInstance()->CheckWallCollision(node_pos, next_pos, &hitpos, &normal))) {
		
		int eff_handle = EffectManager::GetInstance()->Play(eEF_DUST,&node_pos, &VGet(30.0f, 30.0f, 30.0f),	0.0f);
		EffectManager::GetInstance()->Stop(eff_handle, 0.5f);
		
		SoundManager::GetSndComponent()->PlaySound(eSE_BURST);

		ProcessDisappear();
		return;
	}

	/* 攻撃の処理 */
	ModelBase* p_mine = this;
	CharaBase* p_your = NULL;
	for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
		p_your = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
		if (p_mine == (ModelBase *)p_your) continue;			// 自分はのぞく
		if (p_your->GetIsRetire()) continue;	// 戦闘不能者も除く
		if (p_your->IsDamage()) continue;		// 既に攻撃された者も除く
		p_your->GetBodyPillar(&piller);

		if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {

			if ((p_your->Position().y + p_your->BodyHeight() > sphere.mPosition.y) &&
				(p_your->Position().y < sphere.mPosition.y) ) {

				int attackPower = 2;
				// 本来の処理
				VECTOR eff_pos;
				eff_pos = p_your->Position();
				eff_pos.y = node_pos.y;

				VECTOR damage_down_vec;
				damage_down_vec = VSub(p_your->Position(), node_pos);
				damage_down_vec = VNorm(damage_down_vec);
				p_your->ProcessDamage(attackPower, false, &eff_pos, &damage_down_vec, false, eATTACK_TYPE_ARROW);
			}
		}
	}
	
	/* 描画の処理 */
	{
		MATRIX tmp, mtx;
		mtx = MGetIdent();

		Utility::ExtractRotationMatrix(&tmp, &mMatrix, 1.0f);
		mtx = MMult(mtx, tmp);

		tmp = MGetTranslate(next_pos);
		mtx = MMult(mtx, tmp);

		mMatrix = mtx;

		EffekseerManager::GetInstance()->SetMatrix(mEfkHandle, &mMatrix);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_SonicWave::phaseSonicWaveStateDisappear(void)
{
	GraphicManager::GetInstance()->ReserveRemoveModel(this);
}


/*=====================================*
 * class ObjectWeapon_CopyRod
 *=====================================*/
/*static*/
ObjectWeapon_CopyRod::PHASE_WEAPON_COPY_ROD_STATE
ObjectWeapon_CopyRod::mPhaseState[ObjectWeapon_CopyRod::eWEAPON_COPY_ROD_STATE_MAX] = {
	&ObjectWeapon_CopyRod::phaseCopyRodStateAttach,
	&ObjectWeapon_CopyRod::phaseCopyRodStateShooting,
	&ObjectWeapon_CopyRod::phaseCopyRodStateHittingObj,
	&ObjectWeapon_CopyRod::phaseCopyRodStateDisappear,
};
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectWeapon_CopyRod::ObjectWeapon_CopyRod()
{
	mEfkHandle	= -1;
	mpLockonModel = NULL;
	mState = eWEAPON_COPY_ROD_STATE_ATTACH;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectWeapon_CopyRod::~ObjectWeapon_CopyRod()
{
	/*
	if (mEfkHandle != -1) {
		EffekseerManager::GetInstance()->Stop(mEfkHandle);
		mEfkHandle = -1;
	}
	*/
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_CopyRod::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg, bool isRaise/*=false*/)
{
	ObjectWeapon::Setup(objectIdx, itemIdx, pSetPos, rotDeg);
	mWeaponType = eWEAPON_TYPE_COPY_ROD;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_CopyRod::Update(void)
{
	APP_ASSERT_PRINT((mState < eWEAPON_COPY_ROD_STATE_MAX),
					 "Select mState %d is Invalid value \n", mState);

	(this->*mPhaseState[mState])();

	ObjectWeapon::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_CopyRod::ProcessCreate(MATRIX* pMatrix)
{
	if (mEfkHandle != -1) {
		EffekseerManager::GetInstance()->Stop(mEfkHandle, 0.0f);
		mEfkHandle = -1;
	}
	mEfkHandle = EffekseerManager::GetInstance()->PlayMtx(eEFK_MAGMA, pMatrix);

	// 自分で追加する
	GraphicManager::GetInstance()->ReserveEntryModel(this);

	mState = eWEAPON_COPY_ROD_STATE_ATTACH;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_CopyRod::SetCopyRodMatrix(MATRIX* pMatrix)
{
	mMatrix = *pMatrix;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_CopyRod::ProcessShoot(VECTOR* pMoveVec, float setScale, float hitSize, ModelCollision* pLockonModel/*=NULL*/)
{
	mMoveVec	= *pMoveVec;
	mScale		= VGet(setScale, setScale, setScale);
	mBodySize	= hitSize;

	Utility::ExtractMatrixPosition(&mPosition, &mMatrix);

	mpLockonModel = pLockonModel;

	mState = eWEAPON_COPY_ROD_STATE_SHOOTING;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_CopyRod::ProcessHittingObj(float effScale)
{
	// 魔法エフェクトを削除し、追従用の魔法エフェクトの再生
	if (mEfkHandle != -1) {
		EffekseerManager::GetInstance()->Stop(mEfkHandle, 0.0f);
		mEfkHandle = -1;
	}

	VECTOR eff_pos;
	if (mpLockonModel != NULL) {
		mpLockonModel->GetLockonPosition(&eff_pos);
		eff_pos.y += (mpLockonModel->BodyHeight() * 0.5f);
	}
	else {
		Utility::ExtractMatrixPosition(&eff_pos, &mMatrix);
	}
	//				mEfkHandle = EffekseerManager::GetInstance()->Play(eEFK_HIT_LIGHTNING, &eff_pos, 64.0f);

	SoundManager::GetSndComponent()->PlaySound(eSE_SPREAD);
	mEfkHandle = EffekseerManager::GetInstance()->Play(eEFK_HIT_LIGHTNING, &eff_pos, effScale);
	EffekseerManager::GetInstance()->SetSpeed(mEfkHandle, 1.2f);

	mState = eWEAPON_COPY_ROD_STATE_HITTING_OBJ;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_CopyRod::ProcessDisappear(void)
{
	if (mEfkHandle != -1) {
		EffekseerManager::GetInstance()->Stop(mEfkHandle, 0.0f);
		//		EffekseerManager::GetInstance()->Stop(mEfkHandle, 1.0f);
		mEfkHandle = -1;
	}
	mState = eWEAPON_COPY_ROD_STATE_DISAPPEAR;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_CopyRod::phaseCopyRodStateAttach(void)
{
	EffekseerManager::GetInstance()->SetMatrix(mEfkHandle, &mMatrix);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_CopyRod::phaseCopyRodStateShooting(void)
{
	VECTOR mtx_pos;
	Utility::ExtractMatrixPosition(&mtx_pos, &mMatrix);

	// 敵のノードとキャラの円柱の当たり判定
	CollisionManager::tSphereWork sphere;
	sphere.mPosition= mtx_pos;
	sphere.mSize	= mBodySize;
		
	CollisionManager::tPillarWork piller;

	if (mpLockonModel != NULL) {
		VECTOR lockon_pos;
		mpLockonModel->GetLockonPosition(&lockon_pos);
		lockon_pos.y += (mpLockonModel->BodyHeight() * 0.5f);
		VECTOR vec = VNorm(VSub(lockon_pos, mtx_pos));
		mMoveVec = VScale(vec, VSize(mMoveVec) );
	}
	mMoveVec = VScale(mMoveVec, 1.01f);

	// 壁か地面に矢に当たった場合
	VECTOR next_pos, hitpos, normal;
	next_pos = VAdd(mtx_pos, mMoveVec);

	if ((CollisionManager::GetInstance()->CheckEarthCollision(mtx_pos, next_pos, &hitpos, &normal) != NOT_ON_COLLISION) ||
		(CollisionManager::GetInstance()->CheckWallCollision(mtx_pos, next_pos, &hitpos, &normal))) {
		
		ProcessHittingObj(50.0f);
		
		// カメラを元の状態に戻す
		revertCameraState();

		return;
	}

	/* 攻撃の処理 */
	ModelBase* p_mine = this;
	CharaBase* p_your = NULL;
	for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
		p_your = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
		if (p_mine == (ModelBase *)p_your) continue;			// 自分はのぞく
		if (p_your->GetIsRetire()) continue;	// 戦闘不能者も除く
		if (p_your->IsDamage()) continue;		// 既に攻撃された者も除く
		p_your->GetBodyPillar(&piller);

		if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {

			float eff_scale = (p_your->BodySize() * 0.45f);
			ProcessHittingObj(eff_scale);

			// 爆発後の当たり判定を行う
			ShapeCapsule capsule;
			//			capsule.mPosA	= mPosition;
			capsule.mPosA	= mtx_pos;
			capsule.mPosA.y += mBodySize * 0.5f;
			capsule.mPosB	= capsule.mPosA;
			capsule.mPosB.y += 0.01f;
			capsule.mRadius= 300.0f;
			
			for (int iI = 1; iI <  CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
				p_your = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
				if (p_mine == (ModelBase *)p_your) continue;			// 自分はのぞく
				// 戦闘不能者と既に攻撃された者を除く
				if ((p_your->GetIsRetire() == false) &&
					(p_your->IsDamage() == false)) {
					if (p_your->CheckDamageCollBomb(&capsule)) {
						PRINT_CONSOLE("Hit Bomb to Burst !!!!!!!!!!!!!!!!!!!!! \n");
						int attack_power = 4;
						VECTOR damage_down_vec;
						damage_down_vec = VNorm(VSub(p_your->Position(), mPosition));

						VECTOR eff_pos = p_your->Position();
						eff_pos.y = mtx_pos.y;

						p_your->ProcessDamage(attack_power, false, &eff_pos, &damage_down_vec, true, eATTACK_TYPE_COPY_ROD);
					}
				}
			}

			// カメラを元の状態に戻す
			revertCameraState();

			return;
		}
	}

	// カメラ操作をした場合
	int value_x, value_y;
	InputManager::GetInstance()->GetJoyAnalogRight(&value_x, &value_y);
	if ((CHECK_PRESS_MOUSE(MOUSE_INPUT_RIGHT)) || (value_x != 0) || (value_y != 0)) {
		// カメラを元の状態に戻す
		revertCameraState();
	}
	
	/* 描画の処理 */
	{
		MATRIX tmp, mtx;
		mtx = MGetIdent();
		Utility::ExtractRotationMatrix(&tmp, &mMatrix, 1.0f);
		mtx = MMult(mtx, tmp);
		tmp = MGetTranslate(next_pos);
		mtx = MMult(mtx, tmp);
		mMatrix = mtx;
		EffekseerManager::GetInstance()->SetMatrix(mEfkHandle, &mMatrix);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_CopyRod::phaseCopyRodStateHittingObj(void)
{
	if (mpLockonModel != NULL) {
		VECTOR lockon_pos;
		mpLockonModel->GetLockonPosition(&lockon_pos);
		lockon_pos.y += (mpLockonModel->BodyHeight() * 0.5f);

		EffekseerManager::GetInstance()->SetPosition(mEfkHandle, &lockon_pos);
	}

	if (EffekseerManager::GetInstance()->IsPlay(mEfkHandle) == false) {
		ProcessDisappear();
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_CopyRod::phaseCopyRodStateDisappear(void)
{
	GraphicManager::GetInstance()->ReserveRemoveModel(this);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectWeapon_CopyRod::revertCameraState(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	// カメラを元に戻す
	if (CameraManager::GetInstance()->GetCameraState() == eCAM_STATE_BOOMERANG) {
		// カメラを戻す
		if (p_player->GetLockonChara() == NULL) {
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
		} 
		else {
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_LOCKON);
		}
		CameraManager::GetInstance()->ResetCameraPosY();
		CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_HIGH, true);

	}
}


/**** end of file ****/


