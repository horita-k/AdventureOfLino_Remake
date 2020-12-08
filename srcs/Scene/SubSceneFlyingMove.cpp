/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * アイテムメニュー選択
 * @author SPATZ.
 * @data   2015/07/13 23:38:22
 */
//---------------------------------------------------
#include "SceneManager.h"
#include "SceneMainGame.h"
#include "SubSceneFlyingMove.h"
#include "CommandFunctions.h"

#include "AppLib/Basic/Basic.h"
#include "AppLib/Basic/Counter.h"
#include "AppLib/Input/InputManager.h"

#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Resource/ResourceManager.h"
#include "AppLib/Resource/ResourceLayout.h"
#include "AppLib/Effect/EffectManager.h"

#include "Scene/EventManager.h"
#include "Gimmick/GimmickBase.h"

#include "DataDefine/VoiceDef.h"
#include "DataDefine/MotionDef.h"
#include "DataDefine/EffectDef.h"

#include "ExternDefine.h"		//<! Extern 宣言は全てここに集約する

#include "dxlib.h"
#include "Utility/Utility.h"

/*=====================================*
 * フェーズ関数
 *=====================================*/
/* static */SubSceneFlyingMove::PHASE_FUNC
SubSceneFlyingMove::mPhaseFuncEnter[SubSceneFlyingMove::ePHASE_MAX] = {
	&SubSceneFlyingMove::phaseDummy,
	&SubSceneFlyingMove::phaseComePoppoEnter,
	&SubSceneFlyingMove::phaseCatchPoppoEnter,
	&SubSceneFlyingMove::phaseWaitFadeEnter,
	&SubSceneFlyingMove::phaseTalkPoppoEnter,
	&SubSceneFlyingMove::phaseSelectDestEnter,
	&SubSceneFlyingMove::phaseConfirmDestEnter,
	&SubSceneFlyingMove::phaseTalkPopo2ndEnter,
	&SubSceneFlyingMove::phaseFlyAwayEnter,
	&SubSceneFlyingMove::phaseEndEnter,
};

/* static */SubSceneFlyingMove::PHASE_FUNC
SubSceneFlyingMove::mPhaseFuncUpdate[SubSceneFlyingMove::ePHASE_MAX] = {
	&SubSceneFlyingMove::phaseDummy,
	&SubSceneFlyingMove::phaseComePoppoUpdate,
	&SubSceneFlyingMove::phaseCatchPoppoUpdate,
	&SubSceneFlyingMove::phaseWaitFadeUpdate,
	&SubSceneFlyingMove::phaseTalkPoppoUpdate,
	&SubSceneFlyingMove::phaseSelectDestUpdate,
	&SubSceneFlyingMove::phaseConfirmDestUpdate,
	&SubSceneFlyingMove::phaseTalkPopo2ndUpdate,
	&SubSceneFlyingMove::phaseFlyAwayUpdate,
	&SubSceneFlyingMove::phaseEndUpdate,
};

/* static */SubSceneFlyingMove::PHASE_FUNC
SubSceneFlyingMove::mPhaseFuncLeave[SubSceneFlyingMove::ePHASE_MAX] = {
	&SubSceneFlyingMove::phaseDummy,
	&SubSceneFlyingMove::phaseComePoppoLeave,
	&SubSceneFlyingMove::phaseCatchPoppoLeave,
	&SubSceneFlyingMove::phaseWaitFadeLeave,
	&SubSceneFlyingMove::phaseTalkPoppoLeave,
	&SubSceneFlyingMove::phaseSelectDestLeave,
	&SubSceneFlyingMove::phaseConfirmDestLeave,
	&SubSceneFlyingMove::phaseTalkPopo2ndLeave,
	&SubSceneFlyingMove::phaseFlyAwayLeave,
	&SubSceneFlyingMove::phaseEndLeave,
};

/*=====================================*
 * 定数 *
 *=====================================*/
/*static*/
BYTE SubSceneFlyingMove::sSelectDestIndex = 0;

struct tFlyingMoveDestInfo {
	const char* mDestName;
	BYTE		mStageIndex;
	BYTE		mPlaceIndex;
	
	BYTE		mExistEventValueA;
	BYTE		mExistEventValueB;
};

static const char* strNotWent = "？？？";

static const tFlyingMoveDestInfo sDestInfoTable[] = {
	{"リベルニア商業区",			21, 0, eEVENT_21, eEVENT_21_NAVI_EXPLAIN},
	{"リベルニア魔術大学",			21, 3, eEVENT_21, eEVENT_21_NAVI_EXPLAIN},
	{"リベルニア地下水路前",		13, 2, eEVENT_24, eEVENT_24_WELCOME_WATERWAY},
	{"リベルニア城",				29, 42,eEVENT_29, eEVENT_29_COME_FRES},
	{"リベルニア港",				11, 0, eEVENT_14, eEVENT_14_WILL_TALK},
	{"荒野北部",					10, 14,eEVENT_14, eEVENT_14_WILL_TALK},
	{"ジモト村",					35, 0, eEVENT_16, eEVENT_16_WELCOM_FOREST},
	{"忘却の森",					16, 0, eEVENT_16, eEVENT_16_WELCOM_FOREST},
	{"スノレト鉱山前",				10, 12,eEVENT_36, eEVENT_36_WELCOME_MINE},
	{"荒野ハラッパ",				40, 0, eEVENT_40, eEVENT_40_WELCOME_WILDERNESS},
	{"鹿の国",						 8, 0, eEVENT_11, eEVENT_11_WAKEUP_TAKORUKA},
};
static const BYTE kDEST_INFO_NUM = sizeof(sDestInfoTable) / sizeof(tFlyingMoveDestInfo);

/*static*/const float	SubSceneFlyingMove::kCAM_DIST		= 384.0f;
/*static*/const VECTOR	SubSceneFlyingMove::kCAM_TAR		= VGet(-100.00f, 115.00f, 0.00f);
/*static*/const float	SubSceneFlyingMove::kCAM_POS_Y_1ST	= 44.0f;
/*static*/const float	SubSceneFlyingMove::kCAM_POS_Y_2ND	= 200.0f;
/*static*/const WORD	SubSceneFlyingMove::kCAM_ROT_Y_1ST	= 163;
/*static*/const WORD	SubSceneFlyingMove::kCAM_ROT_Y_2ND	= 0;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
SubSceneFlyingMove::SubSceneFlyingMove()
{
	mPhase = ePHASE_NONE;

	mPhaseCounter = 0;

	mNowFrame = 0;
	mMaxFrame = 0;
	memset(mEffHndSky, 0xFF, sizeof(mEffHndSky));
	memset(mEfkBooster,0xFF, sizeof(mEfkBooster));

	mCamVecRotY = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
SubSceneFlyingMove::~SubSceneFlyingMove()
{
	STOP_EFFECT(mEffHndSky[0]);
	STOP_EFFECT(mEffHndSky[1]);

	STOP_EFFEKSEER(mEfkBooster[0], 0.0f);
	STOP_EFFEKSEER(mEfkBooster[1], 0.0f);

	GraphicManager::GetInstance()->RemoveDrawModel(&mLinoModel);
	mLinoModel.DeleteModel();

	GraphicManager::GetInstance()->RemoveDrawModel(&mPoppoModel);
	mPoppoModel.DeleteModel();

	GraphicManager::GetInstance()->RemoveSystemLayout(&mBaseLayout);
}

//---------------------------------------------------
/**
   作成
 */
//---------------------------------------------------
void
SubSceneFlyingMove::Create(void)
{
	ChangePhase(ePHASE_COME_POPPO);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
SubSceneFlyingMove::Update(void)
{
	(this->*mPhaseFuncUpdate[mPhase])();
	
	mPhaseCounter ++;

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::Destroy(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::ChangePhase(BYTE setPhase)
{
	int prev_phase = mPhase;
	mPhase = setPhase;

	(this->*mPhaseFuncLeave[prev_phase])();

	(this->*mPhaseFuncEnter[mPhase])();

	mPhaseCounter = 0;
}

//---------------------------------------------------
/**
 * ポッポ召還可能かを判定する
 */
//---------------------------------------------------
/*static*/BOOL
SubSceneFlyingMove::CheckFlyingMoveEnable(UCHAR stageIndex)
{
	for (int iI = 0; iI < kDEST_INFO_NUM; iI ++) {
		if (sDestInfoTable[iI].mStageIndex == stageIndex) {
			return TRUE;
		}
	}
	return FALSE;
}

/*=====================================*
 * dummy
 *=====================================*/
void
SubSceneFlyingMove::phaseDummy() {}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseComePoppoEnter(void)
{
	sPlayer.PermitAcceptInput(false);

	CameraManager::GetInstance()->SaveTemporaryCamWork();
	CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_DUMMY);

	ResourceManager::GetInstance()->LoadCharaResource(NAME_Poppo);
	EventManager::GetInstance()->LoadCameraFile("data/Camera/FlyingMove.vmd");
		
	/* 表示モデルの設定 */
	mPoppoModel.LinkModel(NAME_Poppo);
	mPoppoModel.SetVisible(true);
	mPoppoModel.Setup();
	GraphicManager::GetInstance()->ReserveEntryModel(&mPoppoModel);
	mPoppoModel.SetMatrixType(CharaPlayer::eMATRIX_TYPE_TRS);
	mPoppoModel.Position() = ZERO_VEC;
	mPoppoModel.Scale() = CHARA_DEF_SCALE;
	mPoppoModel.StartMotion(2, false, 0.5f);

	mLinoModel.LinkModel(Name_Lino);
	mLinoModel.SetVisible(false);
	mLinoModel.Setup();
	GraphicManager::GetInstance()->ReserveEntryModel(&mLinoModel);
	mLinoModel.SetMatrixType(CharaPlayer::eMATRIX_TYPE_TRS);
	mLinoModel.Position() = ZERO_VEC;
	mLinoModel.Scale() = CHARA_DEF_SCALE;
	mLinoModel.StartMotion(eMT_RIDE_VEHICLE_RUNNING, true, 0.8f);
	mLinoModel.SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
	mLinoModel.EnableToonShader();

		
	sModelXfile[eMD_SKYDOME].Position() = ZERO_VEC;
	sModelXfile[eMD_SKYDOME].Rotation().y= 0.0f;
	sModelXfile[eMD_SKYDOME].Scale()	= VGet(60.0f, 60.0f, 60.0f);
	sModelMap.SetVisible(false);
	for (unsigned int iI = 0; iI < sSubStageInfoList.size(); iI ++) {
		spSubModelMap[iI].SetVisible(false);
	}

	sNavi.DisableNaviCursor();

	TotalInfo::GetInstance()->HideBattleIcon();

	EventManager::GetInstance()->StartEvent();

	visibleObjectAll(false);
	GimmickBase::SetVisibleAll(false);
	for (int iI = 0; iI < sNpcNum; iI ++) {
		spNpc[iI].SetVisible(false);
	}


	SoundManager::GetSndComponent()->PlaySound(eSE_SKY_AWAY);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseComePoppoUpdate(void)
{
	EventManager::GetInstance()->Update();


	if (EventManager::GetInstance()->IsEndCamera()) {
						
		sModelMap.SetVisible(true);
		for (unsigned int iI = 0; iI < sSubStageInfoList.size(); iI ++) {
			spSubModelMap[iI].SetVisible(true);
		}

		sModelXfile[eMD_SKYDOME].Position() = sPlayer.Position();
		sModelXfile[eMD_SKYDOME].Scale()	= VGet(kSKYDOME_DEF_SCALE, kSKYDOME_DEF_SCALE, kSKYDOME_DEF_SCALE);

		// 画角を戻す
		CameraManager::GetInstance()->SetPerspectiveCamera(kDEFAULT_PERSPECTIVE_FOV);

		EventManager::GetInstance()->EndEvent();

		CameraManager::GetInstance()->LoadTemporaryCamWork();
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_DUMMY);
		CameraManager::GetInstance()->SetDefaultNearFarClip();
		sPlayer.PermitAcceptInput(false);
		sPlayer.StartMotionEx(&sMotionParam[eMT_IDLE_NOWEAPON]);

		ChangePhase(ePHASE_CATCH_POPPO);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseComePoppoLeave(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseCatchPoppoEnter(void)
{
	sPlayer.ChangeAttachPattern(CharaPlayer::eATTACH_PATTERN_HAND);

	VECTOR vec_z, cross_x, cam_pos, player_pos;
	
	player_pos = sPlayer.Position();
	cam_pos = CameraManager::GetInstance()->Position();

	vec_z = VNorm(VSub(player_pos, cam_pos) );
	cross_x = VCross(vec_z, UP_VEC);
	
	mPoppoStartPos	= VAdd(player_pos, VScale(cross_x, -3000.0f) );
	mPoppoEndPos	= VAdd(player_pos, VScale(cross_x,  3000.0f) );
	mPoppoStartPos.y += 100.0f;
	mPoppoEndPos.y += 100.0f;

	mPoppoModel.SetRotationToTarget(&player_pos);
	mPoppoModel.StartMotion(3, true, 2.0f);

	mNowFrame = 0;
	mMaxFrame = 120;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseCatchPoppoUpdate(void)
{
	VECTOR vec = VSub(mPoppoEndPos, mPoppoStartPos);
	float  rate = (float)mNowFrame / (float)mMaxFrame;

	mPoppoModel.Position() = VAdd(VScale(vec, rate), mPoppoStartPos);

	mNowFrame ++;

	if (mNowFrame >= mMaxFrame) {
		ChangePhase(ePHASE_WAI_FADE);
		FADE_OUT(1.0f);
	}
	else if (mNowFrame > (mMaxFrame / 2)) {
		updatePositionOnPoppo();
	}
	// キャッチする瞬間
	else if (mNowFrame == (mMaxFrame / 2) ) {
		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();
		mLinoModel.SetVisible(true);

		EffekseerManager::GetInstance()->Play(eEFK_BOSS_DEATH, &mPoppoModel.Position(), 3.0f, 16.0f);
	}
	else {
		mPoppoModel.SetRotationToTarget(&sPlayer.Position());
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseCatchPoppoLeave(void)
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseWaitFadeEnter(void)
{
	mCamVecRotY = kCAM_ROT_Y_1ST;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseWaitFadeUpdate(void)
{
	if (IS_FADE() == false) {

		// レイアウトの設定
		mBaseLayout.LinkLayout(RES_LYT_BALLOON);
		mBaseLayout.SetSize(192, 96);
		mBaseLayout.SetPosition(110, 40);
		mBaseLayout.SetAlpha(0);
		mBaseLayout.Rotation() = PHI_F * 0.5f;

		GraphicManager::GetInstance()->EntrySystemLayout(&mBaseLayout);

		// モデルの設定
		sModelXfile[eMD_SKYDOME].Position() = ZERO_VEC;
		sModelXfile[eMD_SKYDOME].Rotation().y= (PHI_F * 0.5f);
		sModelXfile[eMD_SKYDOME].Scale()	= VGet(180.0f, 180.0f, 180.0f);
		sModelMap.SetVisible(false);
		for (unsigned int iI = 0; iI < sSubStageInfoList.size(); iI ++) {
			spSubModelMap[iI].SetVisible(false);
		}

		VECTOR cam_pos;
		float rot = Utility::DegToRad((float)mCamVecRotY);

		cam_pos.x = kCAM_TAR.x + (sin(rot + PHI_F) * kCAM_DIST);
		cam_pos.y = kCAM_POS_Y_1ST;
		cam_pos.z = kCAM_TAR.z + (cos(rot + PHI_F) * kCAM_DIST);

		CameraManager::GetInstance()->Position() = cam_pos;
		CameraManager::GetInstance()->Target() = (VECTOR &)kCAM_TAR;

		VECTOR eff_pos;
		if (mEffHndSky[0] == -1) {
			eff_pos = ZERO_VEC;
			eff_pos.y += 500.0f;

			mEffHndSky[0] = EffectManager::GetInstance()->Play(eEF_SKY, &eff_pos, 0.1f, 0.0f);
			EffectManager::GetInstance()->Scale(mEffHndSky[0]) = VGet(10.0f, 10.0f, 10.0f);
		}

		if (mEffHndSky[1] == -1) {
			eff_pos = ZERO_VEC;
			mEffHndSky[1] = EffectManager::GetInstance()->Play(eEF_SKY, &eff_pos, 0.1f, 0.0f);
			EffectManager::GetInstance()->Scale(mEffHndSky[1]) = VGet(10.0f, 10.0f, 10.0f);
		}

		// スカイドームの色を変更
		COLOR_F color;
		color = GetColorF(0.1f, 0.2f, 0.6f, 1.0f);
		MV1SetMaterialAmbColor(sModelXfile[eMD_SKYDOME].GetModelHandle(), 0, color);
		
		color = GetColorF(0.6f, 0.6f, 1.0f, 1.0f);
		MV1SetMaterialEmiColor(sModelXfile[eMD_SKYDOME].GetModelHandle(), 0, color);

		// BGM変更
		SoundManager::GetInstance()->PlayBGM(BGM_DEPARTURE);

		// ギミック・オブジェクトの表示切り替え
		visibleObjectAll(false);
		GimmickBase::SetVisibleAll(false);
		for (int iI = 0; iI < sNpcNum; iI ++) {
			spNpc[iI].SetVisible(false);
		}

		FADE_IN(0.5f);
		ChangePhase(ePHASE_TALK_POPPO);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseWaitFadeLeave(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseTalkPoppoEnter(void)
{
	mPoppoModel.Position() = ZERO_VEC;
	mPoppoModel.Position().y = 84.0f;
	mPoppoModel.Rotation().y = PHI_F;

	mPoppoPeriod = 0.0f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseTalkPoppoUpdate(void)
{

	if (mPhaseCounter == 60) {
		MSG_TYPE_MESSAGE();
		SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
		MSG_FADE_IN(0.3f);
	}
	else if (mPhaseCounter >= 80) {
		RENDER_MESSAGE("ぽっぽ", "リノ", "どこに行くの？", "", "");
		if (CHECK_TRIG_ANY_JOY()) {
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			MSG_FADE_OUT(0.2f);
			ChangePhase(ePHASE_SELECT_DEST);
		}
	}
	
	updatePositionOnPoppo();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseTalkPoppoLeave(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseSelectDestEnter(void)
{
	GraphicManager::GetInstance()->EntryDrawFontFunc(DrawFontFunc_SelectDest);
	mBaseLayout.StartFade(190, 0.2f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseSelectDestUpdate(void)
{
	float size = 375.0f / GraphicManager::GetInstance()->GetAspectRate();
	mBaseLayout.SetSize(420, (int)size);

	float pos = 518.0f / GraphicManager::GetInstance()->GetAspectRate();
	mBaseLayout.SetPosition((int)pos, 260);

	if (CHECK_TRIG_JOY(JOY_INPUT_UP)) {
		SubSceneFlyingMove::sSelectDestIndex = SELECT_ROTATION_SUB(SubSceneFlyingMove::sSelectDestIndex, kDEST_INFO_NUM);
		SoundManager::GetSndComponent()->PlaySound(eSE_SELECT_KEY);
	}
	else if (CHECK_TRIG_JOY(JOY_INPUT_DOWN)) {
		SubSceneFlyingMove::sSelectDestIndex = SELECT_ROTATION_ADD(SubSceneFlyingMove::sSelectDestIndex, kDEST_INFO_NUM);
		SoundManager::GetSndComponent()->PlaySound(eSE_SELECT_KEY);
	}
	else if (CHECK_TRIG_ANY_JOY()) {

		if (TotalInfo::GetInstance()->GetEventFlagEnable(sDestInfoTable[sSelectDestIndex].mExistEventValueA,
														 sDestInfoTable[sSelectDestIndex].mExistEventValueB)) {
			SoundManager::GetSndComponent()->PlaySound(eSE_PRESS_KEY);
			ChangePhase(ePHASE_CONFIRM_DEST);
		}
		else {
			SoundManager::GetSndComponent()->PlaySound(eSE_NG);
		}
	}

	updatePositionOnPoppo();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseSelectDestLeave(void)
{
	GraphicManager::GetInstance()->EntryDrawFontFunc(NULL);

	mBaseLayout.StartFade(0, 0.1f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseConfirmDestEnter (void)
{

	MSG_TYPE_MESSAGE();
	SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
	MSG_FADE_IN(0.3f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseConfirmDestUpdate (void)
{
	char str_buf[40];
	sprintf_s(str_buf, sizeof(str_buf), "行き先は%sでいいの？", sDestInfoTable[sSelectDestIndex].mDestName);
	RENDER_MESSAGE("ぽっぽ", str_buf, "", "", "");

	Utility::eQuestion qs;
	qs = Utility::ExecuteQuestion();
	if (qs == Utility::eQUESTION_YES) {
		ChangePhase(ePHASE_TALK_POPPO_2ND);
	}
	if (qs == Utility::eQUESTION_NO) {
		SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
		MSG_FADE_OUT(0.3f);

		ChangePhase(ePHASE_SELECT_DEST);
	}

	updatePositionOnPoppo();
}

void
SubSceneFlyingMove::phaseConfirmDestLeave (void){}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseTalkPopo2ndEnter (void){}
void
SubSceneFlyingMove::phaseTalkPopo2ndUpdate (void)
{
	VECTOR node_pos = ZERO_VEC;

	RENDER_MESSAGE("ぽっぽ", "よっしゃぁ！", "じゃあ行くで～", "", "");

	/* カメラの移動処理 */
	mCamVecRotY -= 2;
	if (mCamVecRotY <= kCAM_ROT_Y_2ND) {
		mCamVecRotY = kCAM_ROT_Y_2ND;
	}
	VECTOR cam_pos;
	float rot = Utility::DegToRad(mCamVecRotY);
	float rate = 1.0f - ((float)mCamVecRotY / (float)kCAM_ROT_Y_1ST);
	cam_pos.x = kCAM_TAR.x + (sin(rot + PHI_F) * kCAM_DIST);
	cam_pos.y = ((float)(kCAM_POS_Y_2ND - kCAM_POS_Y_1ST) * rate) + kCAM_POS_Y_1ST;
	cam_pos.z = kCAM_TAR.z + (cos(rot + PHI_F) * kCAM_DIST);
	CameraManager::GetInstance()->Position() = cam_pos;

	/* エフェクトの再生処理 */
	if (mPhaseCounter == 30) {
		mEfkBooster[0] = EffekseerManager::GetInstance()->Play(eEFK_CANNON_LASER_CHARGE, &node_pos, 10.0f);
		mEfkBooster[1] = EffekseerManager::GetInstance()->Play(eEFK_CANNON_LASER_CHARGE, &node_pos, 10.0f);
		SoundManager::GetSndComponent()->PlaySound(eSE_CANNON_LASER_CHARGE);
	}

	if (mEfkBooster[0] != -1) {
		mPoppoModel.GetFramePosFromName(&node_pos, "ﾌﾞｰｽﾀｰ右先");
		EffekseerManager::GetInstance()->SetPosition(mEfkBooster[0], &node_pos);
	}
	if (mEfkBooster[1] != -1) {
		mPoppoModel.GetFramePosFromName(&node_pos, "ﾌﾞｰｽﾀｰ左先");
		EffekseerManager::GetInstance()->SetPosition(mEfkBooster[1], &node_pos);
	}

	//	if (mPhaseCounter >= 120) {
	if (mPhaseCounter >= 90) {
		MSG_FADE_OUT(0.2f);
		ChangePhase(ePHASE_FLY_AWAY);
	}

	updatePositionOnPoppo();
}

void
SubSceneFlyingMove::phaseTalkPopo2ndLeave (void){}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseFlyAwayEnter (void){}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseFlyAwayUpdate (void)
{
	VECTOR node_pos = ZERO_VEC;

	if (mPhaseCounter == 30) {
		STOP_EFFEKSEER(mEfkBooster[0], 0.5f);
		STOP_EFFEKSEER(mEfkBooster[1], 0.5f);

		// キャノンレーザー
		mEfkBooster[0] = EffekseerManager::GetInstance()->Play(eEFK_CANNON_LASER_SHOOT, &node_pos, 10.0f);
		mEfkBooster[1] = EffekseerManager::GetInstance()->Play(eEFK_CANNON_LASER_SHOOT, &node_pos, 10.0f);

		VECTOR scale = VGet(50.0f, 8.0f, 8.0f);
		
		EffekseerManager::GetInstance()->SetScale(mEfkBooster[0], &scale);
		EffekseerManager::GetInstance()->SetScale(mEfkBooster[1], &scale);

		EffekseerManager::GetInstance()->SetSpeed(mEfkBooster[0], 0.5f);
		EffekseerManager::GetInstance()->SetSpeed(mEfkBooster[1], 0.5f);

		SoundManager::GetSndComponent()->PlaySound(eSE_CANNON_LASER);

		EffekseerManager::GetInstance()->Play(eEFK_BOSS_DEATH, &mPoppoModel.Position(), 5.0f, 16.0f);
	}

	if (mEfkBooster[0] != -1) {
		mPoppoModel.GetFramePosFromName(&node_pos, "ﾌﾞｰｽﾀｰ右先");
		EffekseerManager::GetInstance()->SetPosition(mEfkBooster[0], &node_pos);
		EffekseerManager::GetInstance()->SetRotationAxisY(mEfkBooster[0], (PHI_F*0.5f));
	}
	if (mEfkBooster[1] != -1) {
		mPoppoModel.GetFramePosFromName(&node_pos, "ﾌﾞｰｽﾀｰ左先");
		EffekseerManager::GetInstance()->SetPosition(mEfkBooster[1], &node_pos);
		EffekseerManager::GetInstance()->SetRotationAxisY(mEfkBooster[1], (PHI_F*0.5f));
	}

	if (mPhaseCounter == 56) {
		STOP_EFFEKSEER(mEfkBooster[0], 0.1f);
		STOP_EFFEKSEER(mEfkBooster[1], 0.1f);
	}

	if (mPhaseCounter > 40) {
		mPoppoModel.Position().z += 150.0f;
	}

	if (mPhaseCounter == 250) {
		FADE_OUT(0.3f);
		ChangePhase(ePHASE_END);

	}
	updatePositionOnPoppo();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseFlyAwayLeave (void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseEndEnter (void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseEndUpdate (void)
{
	if (IS_FADE() == false) {
		sStageIndex = -1; //<! 強制的にロードを挟むために、一度値を変更する

		ActionTaskManager::tTaskInfo param;
		param.mTaskType = ActionTaskManager::eTASK_MOVE;
		param.mTaskValueA = sDestInfoTable[sSelectDestIndex].mStageIndex;
		param.mTaskValueB = sDestInfoTable[sSelectDestIndex].mPlaceIndex;
		ActionTaskManager::GetInstance()->NotifyAction(&param);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::phaseEndLeave (void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneFlyingMove::updatePositionOnPoppo(void)
{
	static const float wave_period = 0.06f;
	static const float wave_height = 1.0f;

	float wave_pos = sin(mPoppoPeriod) * wave_height;
	mPoppoModel.Position().y += wave_pos;
		
	mPoppoPeriod += wave_period;
	sModelXfile[eMD_SKYDOME].Rotation().y += 0.00075f;

	mLinoModel.Position() = mPoppoModel.Position();
	mLinoModel.Rotation().y = mPoppoModel.Rotation().y;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void DrawFontFunc_SelectDest(void)
{
	static const BYTE kSTR_SIZE		= 30;
	static const BYTE kSTR_HEIGHT	= 32;

	int color = GetColor(0, 0, 0);

	for (int iI = 0; iI < kDEST_INFO_NUM; iI ++) {
		if (SubSceneFlyingMove::sSelectDestIndex == iI) {
			color = GetColor(255, 0, 0);
		}
		else {
			color = GetColor(0, 0, 0);
		}

		char* dest_name = NULL;
		if (TotalInfo::GetInstance()->GetEventFlagEnable(sDestInfoTable[iI].mExistEventValueA,
														 sDestInfoTable[iI].mExistEventValueB)) {
			dest_name = (char *)sDestInfoTable[iI].mDestName;
		}
		else {
			dest_name = (char *)strNotWent;
		}

		//		GraphicManager::GetInstance()->DrawFontString(496, 80+(kSTR_HEIGHT * iI), (char *)sDestInfoTable[iI].mDestName, color, kSTR_SIZE);
		float posx = 372.0f / GraphicManager::GetInstance()->GetAspectRate();
		GraphicManager::GetInstance()->DrawFontString((int)posx, 80+(kSTR_HEIGHT * iI), dest_name, color, kSTR_SIZE);
	}

}

/**** end of file ****/


