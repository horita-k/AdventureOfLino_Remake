/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ステージ固有の処理
 * @author SPATZ.
 * @data   2015/08/26 22:06:24
 */
//---------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <d3dx9math.h>
#include "StageFunc052.h"
#include "AppLib/Effect/EffectManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Resource/ResourceManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "AppLib/Resource/ResourceLayout.h"
#include "DataDefine/EffectDef.h"
#include "Gimmick/GimmickBase.h"
#include "Gimmick/GimmickExtend.h"
#include "DataDefine/VoiceDef.h"
#include "DataDefine/CharaDef.h"

#include "../CommandFunctions.h"
#include "../ExternDefine.h"		//<! Extern 宣言は全てここに集約する

/*=====================================*
 * static
 *=====================================*/

/*=====================================*
 * public
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
StageFunc052::StageFunc052()
{
	mpGuardian = NULL;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
StageFunc052::~StageFunc052()
{
	destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc052::SetupBeforeLoad(void)
{
	/**** ベースクラス関数を自クラス処理後にコール ****/
	StageFuncBase::SetupBeforeLoad();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc052::SetupAfterLoad(int& rPhase)
{
	/**** ベースクラス関数を自クラス処理前にコール ****/
	StageFuncBase::SetupAfterLoad(rPhase);

	ResourceManager::GetInstance()->LoadCharaResource(NAME_Guardian);

	for (int iI = eBG_TYPE_GIMMICK_1F; iI <= eBG_TYPE_GIMMICK_6F; iI ++) {
		CollisionManager::GetInstance()->EntryEarthCollision(sModelMap.GetModelHandle(), iI);
	}

	// 該当のフォグ設定
	GraphicManager::GetInstance()->EnableFogPattern(0,128,192,10.0f,40000.0f);

	SetPatternObject(0);

	MV1SetupCollInfo(sModelMap.GetModelHandle(),
					 eBG_TYPE_GIMMICK_LASER_1F, 8, 8, 8);
	MV1SetupCollInfo(sModelMap.GetModelHandle(),
					 eBG_TYPE_GIMMICK_LASER_3F, 8, 8, 8);


	// イベント終了時に表示される宝箱があれば、表示される
	for (int iI = 0; iI < sObjectBaseNum; iI ++) {
		if (sppObjectBase[iI]->GetObjectType() == ObjectBase::eOBJECT_TYPE_SWITCH) {
			ObjectSwitch* p_switch = (ObjectSwitch *)sppObjectBase[iI];
			if (p_switch->GetSwitchKind() == ObjectSwitch::eSWITCH_KIND_CRYSTAL) {
				p_switch->SetUnused();
			}
		}
	}

	this->RevertGuardian();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc052::Update(int counter)
{
	BOOL is_damage = FALSE;

	MV1RefreshCollInfo(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_LASER_1F);
	MV1RefreshCollInfo(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_LASER_3F);

	/* コンパスの表示 */
	TotalInfo::GetInstance()->UpdateCompass();

	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	// 壁とプレイヤーの当たり判定
	MV1_COLL_RESULT_POLY_DIM HitResDim = {0};

	{

		ShapeCapsule capsule;
		if (p_player->GetIsRetire()) return;		// 戦闘不能者も除く
		if (p_player->IsDamage()) return;			// 既に攻撃された者も除く

		p_player->GetBodyCapsule(&capsule);
		capsule.mRadius *= 0.4f;		// 少し小さくする

		// プレイヤーの直下に存在する地面のポリゴンを取得
		HitResDim = MV1CollCheck_Capsule(sModelMap.GetModelHandle(),
										eBG_TYPE_GIMMICK_LASER_1F,
										capsule.mPosA, capsule.mPosB, 
										capsule.mRadius);

		// 検出した地面ポリゴン情報の後始末
		if (HitResDim.HitNum > 0) {
			is_damage = TRUE;

			MV1CollResultPolyDimTerminate( HitResDim ) ;
		}
		else {

			/*
			if (MV1GetFrameVisible(sModelMap.GetModelHandle(),
								   eBG_TYPE_GIMMICK_LASER_3F) == TRUE) {
			*/
			if (!(IS_END_EVT(eEVENT_52, eEVENT_52_CLEAR_LASER_3F))) {

				// プレイヤーの直下に存在する地面のポリゴンを取得
				HitResDim = MV1CollCheck_Capsule(sModelMap.GetModelHandle(),
												 eBG_TYPE_GIMMICK_LASER_3F,
												 capsule.mPosA, capsule.mPosB, 
												 capsule.mRadius);

				// 検出した地面ポリゴン情報の後始末
				if (HitResDim.HitNum > 0) {
					is_damage = TRUE;
					
					MV1CollResultPolyDimTerminate( HitResDim ) ;
				}
			}
		}
	}

	if (is_damage == TRUE) {
		VECTOR down_vec;
		for (int iI = 0 ; iI < HitResDim.HitNum ; iI ++) {
			/*
			down_vec.x = sin(p_player->Rotation().y);
			down_vec.y = 1.0f;
			down_vec.z = cos(p_player->Rotation().y);
			*/
			down_vec = VNorm(VScale(p_player->MoveVec(), -1));
			p_player->ProcessDamage(1, false, NULL, &down_vec, true, eATTACK_TYPE_BREAK_GUARD);

			break;
		}
	}



}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc052::SetPatternLight(int pattern)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc052::SetPatternObject(int pattern)
{
	int model_handle = sModelMap.GetModelHandle();

	switch (pattern) {
	case 0:
		// バリア更新処理
		StageFuncBase::setVisibleBarrierFromByEventFlag(eBG_TYPE_GIMMICK_BARRIER_1,
														eEVENT_52, 
														eEVENT_52_CLEAR_BARRIER_1);

		// 複数フラグにてバリアの表示を更新
		if ((IS_END_EVT(eEVENT_52, eEVENT_52_CLEAR_BARRIER_2A) == false) ||
			(IS_END_EVT(eEVENT_52, eEVENT_52_CLEAR_BARRIER_2B) == false) ) {
			CollisionManager::GetInstance()->EntryWallCollision(model_handle, eBG_TYPE_GIMMICK_BARRIER_2);
			sModelMap.EntryBarrier(eBG_TYPE_GIMMICK_BARRIER_2);
			MV1SetFrameVisible(model_handle, eBG_TYPE_GIMMICK_BARRIER_2, TRUE);
		}
		else {
			CollisionManager::GetInstance()->RemoveWallCollision(model_handle, eBG_TYPE_GIMMICK_BARRIER_2);
			MV1SetFrameVisible(model_handle, eBG_TYPE_GIMMICK_BARRIER_2, FALSE);
		}

		StageFuncBase::setVisibleBarrierFromByEventFlag(eBG_TYPE_GIMMICK_BARRIER_3A,
														eEVENT_52, 
														eEVENT_52_CLEAR_BARRIER_3A);

		StageFuncBase::setVisibleBarrierFromByEventFlag(eBG_TYPE_GIMMICK_BARRIER_3B,
														eEVENT_52, 
														eEVENT_52_CLEAR_BARRIER_3B);

		// 複数フラグにてバリアの表示を更新
		if ((IS_END_EVT(eEVENT_52, eEVENT_52_CLEAR_BARRIER_3C1) == false) ||
			(IS_END_EVT(eEVENT_52, eEVENT_52_CLEAR_BARRIER_3C2) == false) ) {
			CollisionManager::GetInstance()->EntryWallCollision(model_handle, eBG_TYPE_GIMMICK_BARRIER_3C);
			sModelMap.EntryBarrier(eBG_TYPE_GIMMICK_BARRIER_3C);
			MV1SetFrameVisible(model_handle, eBG_TYPE_GIMMICK_BARRIER_3C, TRUE);
		}
		else {
			CollisionManager::GetInstance()->RemoveWallCollision(model_handle, eBG_TYPE_GIMMICK_BARRIER_3C);
			MV1SetFrameVisible(model_handle, eBG_TYPE_GIMMICK_BARRIER_3C, FALSE);
		}

		StageFuncBase::setVisibleBarrierFromByEventFlag(eBG_TYPE_GIMMICK_BARRIER_4A,
														eEVENT_52, 
														eEVENT_52_CLEAR_BARRIER_4F_A);

		StageFuncBase::setVisibleBarrierFromByEventFlag(eBG_TYPE_GIMMICK_BARRIER_4B,
														eEVENT_52, 
														eEVENT_52_CLEAR_BARRIER_4F_B);

		StageFuncBase::setVisibleBarrierFromByEventFlag(eBG_TYPE_GIMMICK_BARRIER_4C,
														eEVENT_52,
														eEVENT_52_CLEAR_BARRIER_4F_C);

		
		if (IS_END_EVT(eEVENT_52, eEVENT_52_CLEAR_LASER_3F)) {
			MV1SetFrameVisible(model_handle, eBG_TYPE_GIMMICK_LASER_3F, FALSE);
		}

		break;
		
	default:
		break;
	};
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc052::NotifyCreateEnemy(int enemyIndex)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc052::ProcessShowMapBefore(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	int  on_frame_index = 0;
	on_frame_index = CollisionManager::GetInstance()->GetEarthModelFrameIndex( p_player->GetOnCollisionIndex() );
	
	for (int iI = eBG_TYPE_GIMMICK_1F; iI <= eBG_TYPE_GIMMICK_6F; iI ++) {
		MV1SetFrameVisible(sModelMap.GetModelHandle(), iI, FALSE);
	}

	if ( (on_frame_index >= eBG_TYPE_GIMMICK_1F) && (on_frame_index <= eBG_TYPE_GIMMICK_6F) ) {
		MV1SetFrameVisible(sModelMap.GetModelHandle(), on_frame_index, TRUE);
	}
	else {
		sModelMap.SetAlpha(eBG_TYPE_EARTH, 1.0f);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc052::ProcessShowMapAfter(void)
{
	for (int iI = eBG_TYPE_GIMMICK_1F; iI <= eBG_TYPE_GIMMICK_6F; iI ++) {
		MV1SetFrameVisible(sModelMap.GetModelHandle(), iI, TRUE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc052::RevertGuardian(void)
{
	static const BYTE sGuardianPlaceTable[kGUARDIAN_NUM] = {31};

	if (mpGuardian != NULL) {
		for (int iI = 0; iI < kGUARDIAN_NUM; iI ++) {
			mpGuardian[iI].Destroy();
			mpGuardian[iI].DeleteModel();
		}
		delete[] mpGuardian;
		mpGuardian = NULL;

	}

	mpGuardian = new CharaGuardian[kGUARDIAN_NUM];
	APP_ASSERT((mpGuardian != NULL), "Failed new CharaGuardian \n");

	VECTOR pos; float rot;
	for (int iI = 0; iI < kGUARDIAN_NUM; iI ++) {

		mpGuardian[iI].LinkCharaResource(NAME_Guardian);
		getPlayerPosRotFromBGModel(&pos, &rot, sGuardianPlaceTable[iI]);
		mpGuardian[iI].Position()	= pos;
		mpGuardian[iI].Rotation().y	= rot;
		mpGuardian[iI].Scale() = GUARDIAN_DEF_SCALE;
		mpGuardian[iI].Setup();
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc052::ReviseAllModel(VECTOR& rMoveVec)
{
	if (mpGuardian != NULL) {
		for (int iI = 0; iI < kGUARDIAN_NUM; iI ++) {
			mpGuardian[iI].Position() = VSub(mpGuardian[iI].Position(), rMoveVec);
		}
	}
}

/*=====================================*
 * private
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
StageFunc052::destroy(void)
{
	if (mpGuardian != NULL) {
		for (int iI = 0; iI < kGUARDIAN_NUM; iI ++) {
			mpGuardian[iI].Destroy();
			mpGuardian[iI].DeleteModel();
		}
		delete[] mpGuardian;
		mpGuardian = NULL;
	}



	for (int iI = eBG_TYPE_GIMMICK_1F; iI <= eBG_TYPE_GIMMICK_6F; iI ++) {
		CollisionManager::GetInstance()->RemoveEarthCollision(sModelMap.GetModelHandle(), iI);
	}

	MV1TerminateCollInfo(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_LASER_1F);
	MV1TerminateCollInfo(sModelMap.GetModelHandle(), eBG_TYPE_GIMMICK_LASER_3F);


}

/**** end of file ****/


