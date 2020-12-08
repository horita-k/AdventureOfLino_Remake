/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 敵クラス
 * @author SPATZ.
 * @data   2012/02/05 21:50:51
 */
//---------------------------------------------------
#include <math.h>

#include "CharaGuardian.h"
#include "CharaPlayer.h"
#include "AppLib/Basic/Basic.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Input/InputManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Effect/EffectManager.h"

#include "AppLib/Collision/CollisionManager.h"
#include "Scene/TotalInfo.h"
#include "Scene/ActionTaskManager.h"
#include "DataDefine/MotionDef.h"
#include "DataDefine/VoiceDef.h"
#include "DataDefine/EffectDef.h"
#include "DataDefine/CharaDef.h"
#include "Utility/Utility.h"
#include "CharaInfo.h"

/*=====================================*
 * static
 *=====================================*/
/*static*/
CharaGuardian::PHASE_STATE 
CharaGuardian::mPhaseEnterState[CharaGuardian::eCHARA_STATE_MAX] = {
	&CharaGuardian::phaseEnterStateIdle,
	&CharaGuardian::phaseEnterStateTurn,
	&CharaGuardian::phaseEnterStateMove,
	&CharaGuardian::phaseEnterStateJumpUp,
	&CharaGuardian::phaseEnterStateJumpDown,
	&CharaGuardian::phaseEnterStateJumpEnd,
	&CharaGuardian::phaseEnterStateDamage,
	&CharaGuardian::phaseEnterStateRetireStart,
	&CharaGuardian::phaseEnterStateRetireDoing,
	&CharaGuardian::phaseEnterStateRetireEnd,
	&CharaGuardian::phaseEnterStateSlopeDown,
	&CharaGuardian::phaseEnterStateRefuseInput,	//		eCHARA_STATE_RESERVE_1,
	&CharaGuardian::phaseEnterStateDownStart,		//		eCHARA_STATE_RESERVE_2,
	&CharaGuardian::phaseEnterStateDownDoing,		//		eCHARA_STATE_RESERVE_3,
	&CharaGuardian::phaseEnterStateDownEnd,		//		eCHARA_STATE_RESERVE_4,
	&CharaGuardian::phaseEnterStateFallDown,		//		eCHARA_STATE_RESERVE_5,
	NULL, //		eCHARA_STATE_RESERVE_6,
	NULL, //		eCHARA_STATE_RESERVE_7,
	NULL, //		eCHARA_STATE_RESERVE_8,
	NULL, //		eCHARA_STATE_RESERVE_9,
	NULL, //		eCHARA_STATE_NOTHING,
	&CharaGuardian::phaseEnterStateAttack1,
	&CharaGuardian::phaseEnterStateSleep,
	&CharaGuardian::phaseEnterStateBootDown,
	&CharaGuardian::phaseEnterStateChangeLino,
};

/*static*/
CharaGuardian::PHASE_STATE 
CharaGuardian::mPhaseState[CharaGuardian::eCHARA_STATE_MAX] = {
	&CharaGuardian::phaseStateIdle,
	&CharaGuardian::phaseStateTurn,
	&CharaGuardian::phaseStateMove,
	&CharaGuardian::phaseStateJumpUp,
	&CharaGuardian::phaseStateJumpDown,
	&CharaGuardian::phaseStateJumpEnd,
	&CharaGuardian::phaseStateDamage,
	&CharaGuardian::phaseStateRetireStart,
	&CharaGuardian::phaseStateRetireDoing,
	&CharaGuardian::phaseStateRetireEnd,
	&CharaGuardian::phaseStateSlopeDown,
	&CharaGuardian::phaseStateRefuseInput,			//		eCHARA_STATE_RESERVE_1,
	&CharaGuardian::phaseStateDownStart,			//		eCHARA_STATE_RESERVE_2,
	&CharaGuardian::phaseStateDownDoing,			//		eCHARA_STATE_RESERVE_3,
	&CharaGuardian::phaseStateDownEnd,				//		eCHARA_STATE_RESERVE_4,
	&CharaGuardian::phaseStateFallDown,			//		eCHARA_STATE_RESERVE_5,
	NULL, //		eCHARA_STATE_RESERVE_6,
	NULL, //		eCHARA_STATE_RESERVE_7,
	NULL, //		eCHARA_STATE_RESERVE_8,
	NULL, //		eCHARA_STATE_RESERVE_9,
	NULL, //		eCHARA_STATE_NOTHING,
	&CharaGuardian::phaseStateAttack1,
	&CharaGuardian::phaseStateSleep,
	&CharaGuardian::phaseStateBootDown,
	&CharaGuardian::phaseStateChangeLino,
};

/*=====================================*
 * define
 *=====================================*/

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaGuardian::CharaGuardian()
{
	//	mEmulateFlag = TRUE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaGuardian::~CharaGuardian()
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::Setup(void)
{
	CharaBase::Setup();
	
	mMaxHitpoint= 99;
	mHitpoint	= mMaxHitpoint;

	mShadow.Setup(this, Shadow::eUPDATE_TYPE_ALWAYS);


	(this->*mPhaseEnterState[mCharaState])();

	this->SetPositionOnCollision();
	
	GraphicManager::GetInstance()->EntryDrawModel(this);			// 描画エントリー
	CollisionManager::GetInstance()->EntryBodyCollision(this);	// 当たり判定エントリー
	CollisionManager::GetInstance()->EntryAttackCollision(this);	// 当たり判定エントリー

	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	if (p_player->CosplayIndex() == eWINMENU_COSPLAY_GUARDIAN) {
		ChangeState(eCHARA_STATE_CHANGE_LINO);
	}
	else {
		ChangeState(eCHARA_STATE_SLEEP);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::Destroy(void)
{
	GraphicManager::GetInstance()->RemoveDrawModel(this);	// 描画エントリー
	CollisionManager::GetInstance()->RemoveBodyCollision(this);	// 当たり判定エントリー
	CollisionManager::GetInstance()->RemoveAttackCollision(this);	// 当たり判定エントリー

	CharaBase::Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::Update(void)
{
	ModelPmd::Update();
	mMoveVec = ZERO_VEC;

	(this->*mPhaseState[mCharaState])();

	CharaBase::Update();
	
	// mLastEarthPos更新処理		
	updateLastEarthPos();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::ChangeState(int setState)
{
	if (setState == mCharaState) return;
	if (mIsActive == false) return;

	mCharaState = setState;

	APP_ASSERT_PRINT((setState > -1) && (setState < eCHARA_STATE_MAX),
					  "Select setState %d is Invalid Value\n", mCharaState);
	(this->*mPhaseEnterState[mCharaState])();
}

//---------------------------------------------------
/**
   ダメージを食らった時の処理
 */
//---------------------------------------------------
void
CharaGuardian::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	// 落下時
	if (isSetMode == true) {
		CharaBase::ProcessDamage(attackPower, isSetMode, pEffPos, pDamageDownVec, isDamageDown, attackType);
	}
	else {
		if (pEffPos == NULL) {
			pEffPos = &mPosition;
		}
		// シールドエフェクト
		EFK_HANDLE handle = EffekseerManager::GetInstance()->Play(eEFK_SHIELD, pEffPos, &VScale(mScale, 0.8f));
		EffekseerManager::GetInstance()->SetRotationAxisY(handle, (mRotation.y + (PHI_F/2.0f)));

		SoundManager::GetSndComponent()->PlaySound(eSE_SHIELD);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
char*
CharaGuardian::GetModelName(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	if (p_player->CosplayIndex() == eWINMENU_COSPLAY_GUARDIAN) {
		return (char *)"リノ=ライト";
	}
	else {
		return (char*)"マジックアーマー";
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::ChangeBody2Player(void)
{
	/* マジックアーマー操り移行処理 */
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	// マジックアーマー操作関数
	p_player->ProcessOperateMagicArmer();
	p_player->Scale() = GUARDIAN_DEF_SCALE;
			
	// マジックアーマーの表示をリノに変更
	this->DeleteModel();
	this->LinkModel(Name_Lino);
	this->Setup();
	this->Scale() = CHARA_DEF_SCALE;

	// ２つのモデルの位置を交換する
	VECTOR tmp_pos = p_player->Position();
	p_player->Position() = mPosition;
	mPosition = tmp_pos;

	// ２つのモデルの回転を交換する
	float tmp_rot = p_player->Rotation().y;
	p_player->Rotation().y = mRotation.y;
	mRotation.y = tmp_rot;
}

//---------------------------------------------------
/**
 * 着地した時点でコールされる関数
 * (mGravity の値は0ではない状態)
 */
//---------------------------------------------------
void
CharaGuardian::processLandingFunc(void)
{
	float sRetireGravityValue = -60.0f;

	// ある程度高い所から落ちた場合、リタイアさせる
	if (mGravity < sRetireGravityValue) {
		this->ProcessDamage(-9997, true, (VECTOR *)&ZERO_VEC, (VECTOR *)&ZERO_VEC, false, eATTACK_TYPE_NORMAL); // 擬似的に敵を倒す
		SoundManager::GetSndComponent()->PlaySound(eSE_FALL_STONE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::updateLastEarthPos(void)
{
	// ラインの上に乗っている場合、かつ、ギミックに乗っていない場合
	if ((mEarthState == eEARTH_STATE_LINE) &&
		(mIsOnGimmick == FALSE)) {
		
		// mLastEarthPosを更新
		mLastEarthPos = mPosition;
	}
}

/*=====================================*
 * mPhaseState
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateIdle(void)
{
	StartMotionEx(&sMotionParam[eMT_IDLE]);

	mIsAttack = false;
	mEmulateFlag = TRUE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateIdle(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateTurn(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateTurn(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	mRotation.y = p_player->Rotation().y;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateMove(void)
{
	StartMotionEx(&sMotionParam[eMT_WALK]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateMove(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	mRotation.y = p_player->Rotation().y;

	float movevec_sca = VSize(p_player->MoveVec() );

	PRINT_SCREEN(COLOR_RED, "movevec_sca : %.2f \n", movevec_sca);
	PRINT_SCREEN(COLOR_RED, "runspeed    : %.2f \n", p_player->GetRunSpeed());

	if (movevec_sca < (p_player->GetRunSpeed() * 0.8f)) {

		mMoveVec.x = sin(mRotation.y + PHI_F);
		mMoveVec.y = 0.0f;
		mMoveVec.z = cos(mRotation.y + PHI_F);
		
		mMoveVec = VScale(mMoveVec, p_player->GetRunSpeed());
	}
	else {
		mMoveVec = VScale(VNorm(p_player->MoveVec()), p_player->GetRunSpeed());
	}

	float move_speed = VSize(mMoveVec);
	SetMotionSpeed( (move_speed / kCHARA_WALK_SPEED) * 0.8f);

	if (EqualPhaseFrame(2) || EqualPhaseFrame(18)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_FOOT_STEP_ROBOT);

		CameraManager::GetInstance()->StartVibeCamera(0.4f, 2.0f, 10.0f);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateJumpUp(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateJumpUp(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateJumpDown(void)
{
}
	
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateJumpDown(void)
{
	phaseStateJumpUp(); // 上昇中と同じ処理
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateJumpEnd(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateJumpEnd(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	mMoveVec = p_player->MoveVec();
	mRotation.y = p_player->Rotation().y;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateDamage(void)
{
	mIsAttack = false;
	StartMotionEx(&sMotionParam[eMT_DAMAGE]);

	mEmulateFlag = FALSE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateDamage(void)
{
	if (GetMotionIndex() != eMT_DAMAGE) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateRetireStart(void)
{
	mIsAttack = false;

	StartMotionEx(&sMotionParam[eMT_DOWN_START]);

	mFlinchCount = 0;

	mEmulateFlag = FALSE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateRetireStart(void)
{
	if (mFlinchCount > 120) {
		ChangeState(eCHARA_STATE_RETIRE_DOING);
		return;
	}
	else if ( (mFlinchCount % 10) == 0) {

		// 爆発処理を行う(小さい爆発)
		SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
		VECTOR eff_pos = mPosition;
		eff_pos.y += (mBodyHeight * 0.5f);

		eff_pos.x += (GetRand(10) - 5) * 30.0f;
		eff_pos.y += (GetRand(10) - 5) * 30.0f;
		eff_pos.z += (GetRand(10) - 5) * 30.0f;

		EffekseerManager::GetInstance()->Play(eEFK_EXPLOSION_BOMB, &eff_pos, 7.0f);

	}
	mFlinchCount ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateRetireDoing(void)
{
	// 爆発処理を行う
	SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
	VECTOR eff_pos = mPosition;
	eff_pos.y += (mBodyHeight * 0.5f);
	EFK_HANDLE efk_hdl = EffekseerManager::GetInstance()->Play(eEFK_EXPLOSION_BOMB, &eff_pos, 15.0f);
	EffekseerManager::GetInstance()->SetSpeed(efk_hdl, 0.75f);
	CameraManager::GetInstance()->StartShakeCamera(0.6f, 3.0f, 80.0f);

	mFlinchCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateRetireDoing(void)
{
	float alpha = this->GetAlpha();

	alpha += -0.03f;
	if (alpha <= 0.0f) {
		alpha = 0.0f;
		ChangeState(eCHARA_STATE_RETIRE_END);
	}
	this->SetAlpha(alpha);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateRetireEnd(void)
{
	GraphicManager::GetInstance()->ReserveRemoveModel((ModelBase *)this);
	CollisionManager::GetInstance()->RemoveBodyCollision((ModelBase *)this);
	CollisionManager::GetInstance()->RemoveAttackCollision((ModelBase *)this);

	this->SetAlpha(0.0f);
	ModelPmd::DeleteModel();
	mIsActive = false;

	GraphicManager::GetInstance()->RemoveDrawShadow(&mShadow);

	ActionTaskManager::tTaskInfo info;
	info.mTaskType		= ActionTaskManager::eTASK_NAVI_EX;
	info.mTaskValueA	= (long)strNaviTalk_BreakedMagicArmer;
	info.mTaskValueB	= NULL;
	ActionTaskManager::GetInstance()->NotifyAction(&info);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateRetireEnd(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateSlopeDown(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateSlopeDown(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateRefuseInput(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateRefuseInput(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateDownStart(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateDownStart(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateDownDoing(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateDownDoing(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateDownEnd(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateDownEnd(void)
{
}

//---------------------------------------------------
/**
 * 垂直直下のフェーズ
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateFallDown(void)
{
}

//---------------------------------------------------
/**
 * 垂直直下のフェーズ
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateFallDown(void)
{
}

/*=====================================*
 * 拡張ステータス
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateAttack1(void)
{
	SoundManager::GetSndComponent()->PlaySound(eSE_SWING_SWORD);
	StartMotion(eMT_ATTACK_1, false, 0.75f);

	mEmulateFlag = FALSE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateAttack1(void)
{
	static const float sSphereSize	= 150.0f;

	float move_speed = 0.08f * (100 - GetMotionTimePercent());
	mMoveVec.x += -(sin(mRotation.y) * move_speed);
	mMoveVec.z += -(cos(mRotation.y) * move_speed);
	
	if (BetweenPhaseFrame(37.0f, 50.0f)) {
		
		mIsAttack = true;

		// 敵のノードとキャラの円柱の当たり判定
		CollisionManager::tPillarWork piller;
		CollisionManager::tSphereWork sphere;
		this->GetBodyFrameSphere(&sphere, "右手首", sSphereSize);
		VECTOR damage_down_vec = ZERO_VEC;

		CharaBase* p_your = NULL;
		for (int iI = 0; iI < CollisionManager::GetInstance()->GetAttackCollisionNum(); iI ++) {
			p_your = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionAddress(iI);
			if (p_your == this) continue;			// 自分はのぞく
			if (p_your->GetIsRetire()) continue;	// 戦闘不能者も除く
			if (p_your->IsDamage()) continue;		// 既に攻撃された者も除く

			/* 戦闘不能者と既に攻撃された者を除く */
			if (p_your->CheckEnableAttack() == true) {
				p_your->GetBodyPillar(&piller);
				if (CollisionManager::Check_SphereToPillar(&sphere, &piller)) {
					damage_down_vec = VNorm(VSub(p_your->Position(), mPosition));
					p_your->ProcessDamage(12, false, &sphere.mPosition, &damage_down_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
					SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);
				}
			}
		}
	}
	else {
		mIsAttack = false;
	}

	if (GetIsEndMotion()) {
		ChangeState(eCHARA_STATE_IDLE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateSleep(void)
{
	StartMotion(CharaGuardian::eMT_SLEEP, true, 0.5f);

	MV1SetMaterialSphereMapBlendTypeAll(mModelHandle,
										DX_MATERIAL_BLENDTYPE_MODULATE); // 乗算
	for (int iI = 0; iI < (sizeof(kGUARDIAN_SPHERE_ADD_MATERIAL_TABLE) / sizeof(BYTE)); iI ++) {
		MV1SetMaterialSphereMapBlendType(mModelHandle,
										 kGUARDIAN_SPHERE_ADD_MATERIAL_TABLE[iI],
										 DX_MATERIAL_BLENDTYPE_NONE); // 加算
	}

	mEmulateFlag = FALSE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateSleep(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateBootDown(void)
{
	StartMotion(CharaGuardian::eMT_BOOTDOWN, false, 0.5f);

	SoundManager::GetSndComponent()->PlaySound(eSE_CANCEL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateBootDown(void)
{
	if (GetIsMotionBlend() == false) {
		ChangeState(eCHARA_STATE_SLEEP);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseEnterStateChangeLino(void)
{
	EnableToonShader(0.04f, 0.02f);
	SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);
	

	StartMotion(eMT_OCARINA_PLAY_DOING, true, 0.75f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaGuardian::phaseStateChangeLino(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	// オカリナのアタッチ処理
	ModelPmd* p_subweapon = (ModelPmd *)p_player->GetAttachAddr(CharaPlayer::eATTACH_MODEL_SUB_WEAPON)->mpAttachedModel;
	AttachOcarina(p_subweapon);

	// マジックアーマーの方にリノを向かせる
	SetRotationToTarget(&p_player->Position());
}


/**** end of file ****/

