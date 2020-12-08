/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * コリジョン統括マネージャクラス
 * @author SPATZ.
 * @data   2011/11/21 00:05:54
 */
//---------------------------------------------------
#ifndef __COLLISION_MANAGER_H__
#define __COLLISION_MANAGER_H__

#include <vector>
#include "dxlib.h"
#include "AppLib/Graphic/ModelBase.h"
#include "AppLib/Graphic/Shape.h"

/*=====================================*
 * class
 *=====================================*/
#define NOT_ON_COLLISION		(-1)

class CollisionManager
{
public:
	static const int kCHECK_COLLISION_MAX = 50;

	struct tCheckCollisionWork {
		int		mModelHandle;
		SHORT	mFrameIndex;
	};

	// 球
	struct tSphereWork {
		VECTOR	mPosition;	
		float	mSize;
	};

	// 円柱
	struct tPillarWork {
		VECTOR	mPosition;
		float	mSize;
		float	mHeight;
	};

	static CollisionManager* const GetInstance(void);

	void		Create(void);
	void		Setup(const ModelBase* pEarthModel, const ModelBase* pWallModel);
	void		Update(void);
	void		Destroy(void);
	void		Refresh(void);
	void		RefreshModel(int modelHandle, int frameIndex);

	/*=====================================*
	 * 地形の当たり判定
	 *=====================================*/
	void		EntryEarthCollision(int modelHandle, int frameIndex);
	void		EntryWallCollision(int modelHandle, int frameIndex);
	bool		RemoveEarthCollision(int modelHandle, int frameIndex);
	bool		RemoveWallCollision(int modelHandle, int frameIndex);

	// 地面との当たり判定
	INT8		CheckEarthCollision(const VECTOR& rVecStart, const VECTOR& rVecEnd, VECTOR* pOutHitPos, VECTOR* pOutNormal);
	INT8		CheckEarthCollision(const VECTOR& rVecStart, const VECTOR& rVecEnd, VECTOR* pOutHitPos, VECTOR* pOutNormal, int* pOutFrameIndex, BYTE* pOutMat);
	INT8		CheckEarthCollisionMulti(const VECTOR& rVecStart, const VECTOR& rVecEnd, VECTOR* pOutHitPos, VECTOR* pOutNormal, int* pOutFrameIndex, BYTE* pOutMat);
	INT8		CheckEarthCollisionNearest(const VECTOR& rVecStart, const VECTOR& rVecEnd, VECTOR* pOutHitPos );
	INT8		CheckEarthCollisionExcept(int exceptModelHandle, const VECTOR& rVecStart, const VECTOR& rVecEnd, VECTOR* pOutHitPos, VECTOR* pOutNormal );

	// 壁との当たり判定
	bool		CheckWallCollision(const VECTOR& rVecStart, const VECTOR& rVecEnd, VECTOR* pOutHitPos, VECTOR* pOutNormal);
	bool		CheckWallCollision(const VECTOR& rVecStart, const VECTOR& rVecEnd, VECTOR* pOutHitPos, int* pOutCollIndex);
	bool		CheckWallCollisionNearest(const VECTOR& rVecStart, const VECTOR& rVecEnd, VECTOR* pOutHitPos, int* pOutCollIndex);
	bool		CheckWallCollisionExcept(int exceptModelHandle, const VECTOR& rVecStart, const VECTOR& rVecEnd, VECTOR* pOutHitPos, VECTOR* pOutNormal);

	bool		CheckTaskCollision(const VECTOR& rVecStart, const VECTOR& rVecEnd, void* pParam, VECTOR* pOutNormal);
	bool		CheckSideCollision(ShapeCapsule* pCapsule, VECTOR* pMoveVec, VECTOR* pOutNormal);

	bool		CheckWaterCollision(const VECTOR& rVecStart, const VECTOR& rVecEnd, VECTOR* pOutHitPos, VECTOR* pOutNormal);

	int			GetEarthModelNum(void);
	int			GetEarthModelHandle(int modelIndex);
	int			GetEarthModelFrameIndex(int modelIndex);
	ModelBase*	GetEarthModel(void);

	int			GetWallModelNum(void);
	int			GetWallModelHandle(int modelIndex);
	int			GetWallModelFrameIndex(int modelIndex);
	ModelBase*	GetWallModel(void);

	/*=====================================*
	 * 体の当たり判定
	 *=====================================*/
	void		EntryBodyCollision(ModelBase* pModel);
	void		RemoveBodyCollision(ModelBase* pModel);
	void		RemoveBodyCollisionAll(void);
	int			GetBodyCollisionNum(void);
	ModelBase*	GetBodyCollisionAddress(int modelIndex);

	/*=====================================*
	 * 攻撃の当たり判定
	 *=====================================*/
	void		EntryAttackCollision(ModelBase* pModel);
	void		RemoveAttackCollision(ModelBase* pModel);
	void		RemoveAttackCollisionAll(void);
	int			GetAttackCollisionNum(void);
	ModelBase*	GetAttackCollisionAddress(int modelIndex);
	ModelBase*	GetAttackCollisionPlayerAddress(void);

	/*=====================================*
	 * 形状の当たり判定汎用関数
	 *=====================================*/
	static bool Check_SphereToSphere(tSphereWork* pSphereA, tSphereWork* pSphereB, VECTOR* pOutVec, float* pOutDist);
	static bool	Check_SphereToPillar(tSphereWork* pSphere, tPillarWork* pPiller);
	static bool	Check_LineToCapsule(ShapeLine* pLine, ShapeCapsule* pCapsule);
	static bool	Check_CapsuleToCapsule(ShapeCapsule* pCapsuleA, ShapeCapsule* pCapsuleB);
	static bool Check_LineToBoard(ShapeLine* pLine, ShapeBoard* pBoard);

	static bool Check_LineToLine(VECTOR& rSeg1Pos, VECTOR& rSeg1Vec, VECTOR& rSeg2Pos,VECTOR& rSeg2Vec, VECTOR& rOutPos);

private:
	CollisionManager();
	~CollisionManager();

	void		initialize(void);

	void		entryVector(std::vector<ModelBase *>* pVec, ModelBase* pModel);
	void		removeVector(std::vector<ModelBase *>* pVec, ModelBase* pModel);
	void		removeVectorAll(std::vector<ModelBase *>* pVec);
	int			getVectorNum(std::vector<ModelBase *>* pVec);
	ModelBase*	getVectorAddress(std::vector<ModelBase *>* pVec, int modelIndex);

	/*=====================================*
	 * 変数
	 *=====================================*/
	ModelBase*							mpEarthModel;
	ModelBase*							mpWallModel;
	tCheckCollisionWork					mEarthCollWork[kCHECK_COLLISION_MAX];
	int									mEarthCollNum;
	tCheckCollisionWork					mWallCollWork[kCHECK_COLLISION_MAX];
	int									mWallCollNum;
	std::vector<ModelBase *>			mBodyCollisionVector;		// 当たり用モデル
	std::vector<ModelBase *>			mAttackCollisionVector;		// 当たり用モデル

};

#endif // __COLLISION_MANAGER_H__

/**** end of file ****/
