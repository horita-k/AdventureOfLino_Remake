/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * コリジョンありモデル
 * @author SPATZ.
 * @data   2012/12/29 19:02:56
 */
//---------------------------------------------------
/*=====================================*
 * include
 *=====================================*/
#include "AppLib/Basic/Basic.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "DataDefine/BGDef.h"

#include "ModelCollision.h"
#include "Charactor/CharaInfo.h"

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ModelCollision::ModelCollision()
{
	mIsDamage = false;
	mMoveVec = ZERO_VEC;
	mBodySize = 0.0f;
	mBodyHeight = 0.0f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ModelCollision::~ModelCollision()
{
	Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelCollision::Setup(void)
{
	ModelPmd::Setup();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelCollision::Update(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelCollision::Destroy(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelCollision::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelCollision::GetBodyPillar(CollisionManager::tPillarWork* pPillar)
{
	pPillar->mPosition	= mPosition;
	pPillar->mSize		= mBodySize;
	pPillar->mHeight	= mBodyHeight;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ModelCollision::GetBodyFrameSphere(CollisionManager::tSphereWork* pSphere, const char* strFrameName, float sphereSize)
{
	GetFramePosFromName(&pSphere->mPosition, strFrameName);
	pSphere->mSize = sphereSize;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ModelCollision::CheckBodyCollision(CollisionManager::tSphereWork& rOtherSphere, VECTOR& rVec, float& rDist)
{
	CollisionManager::tSphereWork my_sphere;
	my_sphere.mPosition = mPosition;
	my_sphere.mSize		= mBodySize;

	// 円と円の辺り判定
	if (CollisionManager::GetInstance()->Check_SphereToSphere(&rOtherSphere, &my_sphere, &rVec, &rDist) ) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ModelCollision::CheckEnableAttack(void)
{
	APP_ASSERT(NULL, "Invalid Method \n");

	return false;
}

//---------------------------------------------------
/**
 // 他のオブジェクトとの当たり判定
 */
//---------------------------------------------------
void
ModelCollision::updateAllObject(void)
{
	ModelCollision* p_mine = this;
	ModelCollision* p_your = NULL;

	VECTOR moved_pos = VAdd(p_mine->Position(), mMoveVec);
	VECTOR vec;
	float dist = 0.0f;
	CollisionManager::tSphereWork workA;
	VECTOR target_pos;

	workA.mPosition	= moved_pos;
	workA.mSize		= this->mBodySize;
	for (int iI = 0; iI < CollisionManager::GetInstance()->GetBodyCollisionNum(); iI ++) {
		p_your = (ModelCollision *)CollisionManager::GetInstance()->GetBodyCollisionAddress(iI);
		if (p_mine == p_your) continue;				// 自分はのぞく
		if (p_your->GetIsRetire()) continue;	// 戦闘不能者も除く

		if (p_your->CheckBodyCollision(workA, vec, dist)) {
			mMoveVec.x = 0.0f;
			mMoveVec.z = 0.0f;
			vec.y = 0.0f;
			target_pos = VAdd(moved_pos, VScale(VNorm(vec), -dist));
			mMoveVec = VSub(target_pos, mPosition);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ModelCollision::checkLine2CharaBoard(tCheckDamageWork* pParam)
{
	VECTOR vec, cross, lockon_pos;
	GetLockonPosition(&lockon_pos);
	vec = VSub(pParam->mPlayerPos, lockon_pos);
	vec = VScale(VNorm(vec), mBodySize);
	cross = VCross(vec, sUpVec);

	ShapeBoard board;
	board.mPosA = VAdd(lockon_pos, VAdd(vec, cross));
	board.mPosB = VAdd(lockon_pos, VSub(vec, cross));
	board.mPosC = board.mPosB;
	board.mPosC.y += mBodyHeight;
	board.mPosD = board.mPosA;
	board.mPosD.y += mBodyHeight;

	ShapeLine line;
	line.mStart	= pParam->mWeaponTopPos;
	line.mEnd	= pParam->mWeaponBtmPos;

	if (CollisionManager::Check_LineToBoard(&line, &board)) {
		return true;
	}	
	return false;
}

//---------------------------------------------------
/**
 * モデルの大きさから自動的に BodySize と BodyHeight を割り当てる
 */
//---------------------------------------------------
void
ModelCollision::setupBodyHeightSize(void)
{
	int mesh_num;
	VECTOR max_pos = ZERO_VEC;
	VECTOR min_pos = ZERO_VEC;
	VECTOR tmp_pos;
	
	mesh_num = MV1GetMeshNum(mModelHandle);

	for (int iI = 0; iI < mesh_num; iI ++) {
		GetMeshMaxPosition(&tmp_pos, iI);
		if (tmp_pos.x > max_pos.x) max_pos.x = tmp_pos.x;
		if (tmp_pos.y > max_pos.y) max_pos.y = tmp_pos.y;
		if (tmp_pos.z > max_pos.z) max_pos.z = tmp_pos.z;

		GetMeshMinPosition(&tmp_pos, iI);
		if (tmp_pos.x < min_pos.x) min_pos.x = tmp_pos.x;
		if (tmp_pos.y < min_pos.y) min_pos.y = tmp_pos.y;
		if (tmp_pos.z < min_pos.z) min_pos.z = tmp_pos.z;
	}
	mBodyHeight = max_pos.y - min_pos.y;
	mBodySize = ((max_pos.x - min_pos.x) + (max_pos.z - min_pos.z)) / 2.0f;
	mBodySize *= 0.5f;
}

//---------------------------------------------------
/**
   
 */
//---------------------------------------------------
bool ModelCollision::CheckDamageCollNormal(tCheckDamageWork* pParam)
{
	return checkLine2CharaBoard(pParam);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool ModelCollision::CheckDamageCollRolling(tCheckDamageWork* pParam)
{
	// 球と球の判定
	VECTOR lockon_pos;
	GetLockonPosition(&lockon_pos);
	float dist = VSize(VSub(lockon_pos, pParam->mPlayerPos));
	// 攻撃があたったかの判定
	if (dist <= (pParam->mWeaponLength + mBodySize)) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool ModelCollision::CheckDamageCollJump(tCheckDamageWork* pParam)
{
	return checkLine2CharaBoard(pParam);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool ModelCollision::CheckDamageCollLonghook(tCheckDamageWork* pParam)
{
	return checkLine2CharaBoard(pParam);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool ModelCollision::CheckDamageCollBoomerang(tCheckDamageWork* pParam)
{
	return checkLine2CharaBoard(pParam);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool ModelCollision::CheckDamageCollBomb(ShapeCapsule* pParam)
{
	ShapeCapsule capsule;
	VECTOR lockon_pos;
	GetLockonPosition(&lockon_pos);
	capsule.mPosA	= lockon_pos;
	capsule.mPosB	= lockon_pos;
	capsule.mPosB.y += mBodyHeight;
	capsule.mRadius= mBodySize;

	if (CollisionManager::Check_CapsuleToCapsule(pParam, &capsule)) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool ModelCollision::CheckDamageCollHand(ShapeCapsule* pParam)
{
	ShapeCapsule capsule;
	VECTOR lockon_pos;
	GetLockonPosition(&lockon_pos);
	capsule.mPosA	= lockon_pos;
	capsule.mPosB	= lockon_pos;
	capsule.mPosB.y += mBodyHeight;
	capsule.mRadius= mBodySize;

	if (CollisionManager::Check_CapsuleToCapsule(pParam, &capsule)) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool ModelCollision::CheckDamageCollArrow(tCheckDamageWork* pParam)
{
	return checkLine2CharaBoard(pParam);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool ModelCollision::CheckDamageCollEnemyNormal(tCheckDamageWork* pParam)
{
	return false;

}


/**** end of file ****/
