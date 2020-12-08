/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ブレードエフェクト
 * @author SPATZ.
 * @data   2012/06/22 23:55:41
 */
//---------------------------------------------------
/*=====================================*
 * include
 *=====================================*/
#include "AppLib/Basic/Basic.h"
#include "AppLib/Effect/EffectManager.h"
#include "DataDefine/EffectDef.h"

#include "BladeEffect.h"

/*=====================================*
 * static
 *=====================================*/
/*static*/int BladeEffect::mAttachEffectIndex = eEF_MY_TAIL;

static const char* strExtSetPosA[BladeEffect::kPREV_NUM] = {
	"ExtSetPosA[0]", 
	"ExtSetPosA[1]", 
	"ExtSetPosA[2]", 
	"ExtSetPosA[3]", 
	"ExtSetPosA[4]", 
	"ExtSetPosA[5]", 
	"ExtSetPosA[6]", 
	"ExtSetPosA[7]", 
	"ExtSetPosA[8]", 
	"ExtSetPosA[9]", 
	"ExtSetPosA[10]", 
	"ExtSetPosA[11]", 
}; 
static const char* strExtSetPosB[BladeEffect::kPREV_NUM] = {
	"ExtSetPosB[0]", 
	"ExtSetPosB[1]", 
	"ExtSetPosB[2]", 
	"ExtSetPosB[3]", 
	"ExtSetPosB[4]", 
	"ExtSetPosB[5]", 
	"ExtSetPosB[6]", 
	"ExtSetPosB[7]", 
	"ExtSetPosB[8]", 
	"ExtSetPosB[9]", 
	"ExtSetPosB[10]", 
	"ExtSetPosB[11]", 
}; 

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
BladeEffect::BladeEffect()
{
	mEffectIndex = -1;
	memset(&mPrevTopPos, 0x00, sizeof(mPrevTopPos));
	memset(&mPrevBtmPos, 0x00, sizeof(mPrevBtmPos));
	mCurrentIndex = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
BladeEffect::~BladeEffect()
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
BladeEffect::Setup(int attachEffectIndex)
{
	mAttachEffectIndex = attachEffectIndex;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
BladeEffect::Play(VECTOR* pTopPos, VECTOR* pBtmPos)
{
	if (mEffectIndex != -1) {
		//		this->Stop();
		return;
	}
	mEffectIndex = EffectManager::GetInstance()->Play(mAttachEffectIndex,
													  (VECTOR *)&ZERO_VEC,
													  &VGet(10.0f, 10.0f, 10.0f),
													  0.0f);
	for (int iI = 0; iI < kPREV_NUM; iI ++) {
		mPrevTopPos[iI] = *pTopPos;
		mPrevBtmPos[iI] = *pBtmPos;
	}
	mCurrentIndex = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
BladeEffect::Update(VECTOR* pTopPos, VECTOR* pBtmPos)
{
	mPrevTopPos[(mCurrentIndex+1)%kPREV_NUM] = *pTopPos;
	mPrevBtmPos[(mCurrentIndex+1)%kPREV_NUM] = *pBtmPos;

	//		mPrevTopPos[mCurrentIndex] = *pTopPos;
	//		mPrevBtmPos[mCurrentIndex] = *pBtmPos;

	VECTOR prevpos_top = mPrevTopPos[(mCurrentIndex + kPREV_NUM- 1) % kPREV_NUM];
	VECTOR prevpos_btm = mPrevBtmPos[(mCurrentIndex + kPREV_NUM- 1) % kPREV_NUM];

	VECTOR currentpos_top = *pTopPos;
	VECTOR currentpos_btm = *pBtmPos;

	VECTOR midpos_top = GET_MIDDLE_POS(prevpos_top, currentpos_top);
	VECTOR midpos_btm = GET_MIDDLE_POS(prevpos_btm, currentpos_btm);

	float length = VSize(VSub(currentpos_top, currentpos_btm));

	VECTOR midvec = VSub(midpos_top, midpos_btm);
	
	mPrevTopPos[mCurrentIndex] = VAdd(midpos_btm, VScale(VNorm(midvec), length));
	mPrevBtmPos[mCurrentIndex] = midpos_btm;
	
	// mPrevPosition の値を シェーダ側に設定
	if (mEffectIndex != -1) {
		FLOAT4 f4;
		FLOAT4* p_f4 = &f4;
		int cnt;

		EffectManager* const efk_mgr = EffectManager::GetInstance();
		
		cnt = 0;
		for (int iI = mCurrentIndex; iI >= 0; iI --) {
			SET_FLOAT4(p_f4, mPrevTopPos[iI]);
			efk_mgr->SetVec4(mEffectIndex, strExtSetPosA[cnt++], p_f4);
		}
		for (int iI = (kPREV_NUM - 1); iI > mCurrentIndex; iI --) {
			SET_FLOAT4(p_f4, mPrevTopPos[iI]);
			efk_mgr->SetVec4(mEffectIndex, strExtSetPosA[cnt++], p_f4);
		}

		cnt = 0;
		for (int iI = mCurrentIndex; iI >= 0; iI --) {
			SET_FLOAT4(p_f4, mPrevBtmPos[iI]);
			efk_mgr->SetVec4(mEffectIndex, strExtSetPosB[cnt++], p_f4);
		}
		for (int iI = (kPREV_NUM - 1); iI > mCurrentIndex; iI --) {
			SET_FLOAT4(p_f4, mPrevBtmPos[iI]);
			efk_mgr->SetVec4(mEffectIndex, strExtSetPosB[cnt++], p_f4);
		}
	}

	//	mCurrentIndex = (mCurrentIndex + 1) % kPREV_NUM;
	mCurrentIndex = (mCurrentIndex + 2) % kPREV_NUM;

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
BladeEffect::Stop(void)
{
	if (mEffectIndex == -1) {
		return;
	}

	EffectManager::GetInstance()->Stop(mEffectIndex, 0.0f);
	mEffectIndex = -1;
}

 /**** end of file ****/


