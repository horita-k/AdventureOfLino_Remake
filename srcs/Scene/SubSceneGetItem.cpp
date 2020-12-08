/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * アイテム取得関連
 * @author SPATZ.
 * @data   2016/07/17 18:16:45
 */
//---------------------------------------------------
#include "SceneManager.h"
#include "SceneMainGame.h"
#include "SubSceneGetItem.h"
#include "EventManager.h"

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

#include "StageFunc/StageFuncDef.h"

#include "DataDefine/VoiceDef.h"
#include "DataDefine/MotionDef.h"
#include "DataDefine/EffectDef.h"
#include "ExternDefine.h"		//<! Extern 宣言は全てここに集約する

#include "dxlib.h"
#include "Utility/Utility.h"

/*=====================================*
 * static
 *=====================================*/
static const float length = -100.0f;
static const float kADD_FRAME_COUNTER = 1.0f;
static const VECTOR kCAM_DIFF = VGet(0.0f, 94.8f, 130.0f );

/*=====================================*
 * フェーズ関数
 *=====================================*/
/* static */SubSceneGetItem::PHASE_FUNC
SubSceneGetItem::mPhaseFunc[SubSceneGetItem::ePHASE_MAX] = {
	&SubSceneGetItem::phaseNone,
	&SubSceneGetItem::phaseBigTreasureEnter,
	&SubSceneGetItem::phaseBigTreasureUpdate,
	&SubSceneGetItem::phaseBigTreasureLeave,
	&SubSceneGetItem::phaseSmallTreasureEnter,
	&SubSceneGetItem::phaseSmallTreasureUpdate,
	&SubSceneGetItem::phaseSmallTreasureLeave,
 	&SubSceneGetItem::phaseCrystalEnter,
	&SubSceneGetItem::phaseCrystalUpdate,
	&SubSceneGetItem::phaseCrystalLeave,
	&SubSceneGetItem::phaseCrystalMoving,
	&SubSceneGetItem::phaseFinish,
};

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
SubSceneGetItem::SubSceneGetItem()
{
	mPhase = ePHASE_NONE;
	mPhaseCounter = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
SubSceneGetItem::~SubSceneGetItem()
{

}

//---------------------------------------------------
/**
   作成
 */
//---------------------------------------------------
void
SubSceneGetItem::Create(void)
{
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
SubSceneGetItem::Update(void)
{
	(this->*mPhaseFunc[mPhase])();

	if (mPhase == ePHASE_FINISH) {
		return true;
	}


#ifdef _DEBUG
	static unsigned char sWaitCnt = 0;
	/* 60秒間に１回CSVファイルが更新されているかを確認し、
	   更新されていればCSV処理を実施する */
	if ((sWaitCnt % 6) == 0) {
		// GameTestTool との連携を行う関数
		updateGameTestTool();
	}
	sWaitCnt ++;
#endif // _DEBUG

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneGetItem::Destroy(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneGetItem::ChangePhase(BYTE setPhase)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneGetItem::phaseNone(void)
{
	int treasure_kind = sTaskInfo.mTaskValueB;
	switch (treasure_kind) {
	case eTREASURE_KIND_BIG:
		mPhase = ePHASE_BIG_TREASURE_ENTER;
		break;
	case eTREASURE_KIND_SMALL:
	case eTREASURE_KIND_FALLEN:
		mPhase = ePHASE_SMALL_TREASURE_ENTER;
		break;
	case eTREASURE_KIND_CRYSTAL:
		mPhase = ePHASE_CRYSTAL_ENTER;
		break;
	default:
		break;
	};
}

/*=====================================*
 * Big Treasure
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneGetItem::phaseBigTreasureEnter(void)
{
	HIDE_BATTLE_ICON();
	TotalInfo::GetInstance()->HideBattleIcon();

	/* 初期設定 */
	ObjectBase* p_item = (ObjectTresure *)sTaskInfo.mTaskValueA;

	sPlayer.RevertCosplay();

	float rot;
	rot = p_item->Rotation().y;
	sPlayer.Position() = p_item->Position();
	sPlayer.Position().x += sin(rot) * length;
	sPlayer.Position().z += cos(rot) * length;
	sPlayer.SetRotationToTarget(&p_item->Position());

	sPlayer.ChangeAttachPattern(CharaPlayer::eATTACH_PATTERN_HAND);

	sPlayer.StartMotion(eMT_KICK_TREASURE, false, 0.5f);
	p_item->StartMotion(eMT_TRESURE_EVENT, false, 0.5f);

	EventManager::GetInstance()->LoadCameraFile("data/Camera/TakeTresure.vmd");
	EventManager::GetInstance()->EnableOffsetCamera(sPlayer.Position().x,
													sPlayer.Position().y,
													sPlayer.Position().z,
													sPlayer.Rotation().y);
	EventManager::GetInstance()->StartEvent();

	// EventManager::StartEvent() よってオブジェクトを非表示にされる為、再表示
	p_item->SetVisible(true);

	SetVolumeMusic(0);
	SoundManager::GetSndComponent()->PlaySound(eSE_TREASURE);

	mPhase = ePHASE_BIG_TREASURE_UPDATE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneGetItem::phaseBigTreasureUpdate(void)
{
	ObjectBase* p_item = (ObjectTresure *)sTaskInfo.mTaskValueA;

	eItemKind item_index = (eItemKind)p_item->GetItemKind();
	const tItemDetailInfo * p_item_detail = GetItemDetailInfo(item_index);

	if (EventManager::GetInstance()->EqualPhaseFrame(208.0f + 0.0f)) {
		MSG_TYPE_TELOP();
		MSG_FADE_IN(0.3f);
	}
	else if (EventManager::GetInstance()->EqualPhaseFrame(208.0f + 2.0f)) {
		ModelBase* p_model = NULL;
		p_model = &mGettedItemModel;
		p_model->LinkModel(p_item_detail->mModelName);
		p_model->DisableToonShader();
		GraphicManager::GetInstance()->EntryDrawModel(p_model);
		p_model->Position() = CameraManager::GetInstance()->Target();
		p_model->Rotation() = ZERO_VEC;
		p_model->Position().y += (sPlayer.BodyHeight() * 0.3f);
		p_model->SetRotationToTarget(&CameraManager::GetInstance()->Position());
		p_model->Scale() = sPlayer.Scale();
		
		/* 各アイテムごとの特別処理 */
		settingModelByItemKind(item_index);

	}
	else if (EventManager::GetInstance()->BetweenPhaseFrame(208.0f + 2.0f, 208.0f + 182.0f)) {
		char tmp[256] = {0};
		const char *msg2, *msg3, *msg4;
		sprintf_s(tmp, sizeof(tmp), "%sをてにいれた！", p_item_detail->mDispName);
		msg2			= p_item_detail->mExplain0;
		msg3			= p_item_detail->mExplain1;
		msg4			= p_item_detail->mExplain2;
		RENDER_MESSAGE_COL("", tmp, msg2, msg3, msg4, GetColor(0, 0, 0));

		if (CHECK_TRIG_ANY_JOY()) {
			mPhase = ePHASE_BIG_TREASURE_LEAVE;
		}
	}
	else if (EventManager::GetInstance()->EqualPhaseFrame(208.0f + 183.0f)) {
		mPhase = ePHASE_BIG_TREASURE_LEAVE;
	}

	EventManager::GetInstance()->Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneGetItem::phaseBigTreasureLeave(void)
{
	ObjectBase* p_item = (ObjectTresure *)sTaskInfo.mTaskValueA;

	/* 終了設定 */
	TotalInfo::GetInstance()->EnableItemFlag((eItemKind)p_item->GetItemKind() );

	/* アイテム取得更新処理 */
	updateGetItem((eItemKind)p_item->GetItemKind());
	p_item->SetUsed();

	EventManager::GetInstance()->EndEvent();

	GraphicManager::GetInstance()->RemoveDrawModel(&mGettedItemModel);

	SetVolumeMusic(255);					// ボリュームを戻す
	VISIBLE_BATTLE_ICON();					// アイコンを表示させる
	TotalInfo::GetInstance()->VisibleBattleIcon();

	sPlayer.PermitAcceptInput(true);		// 主人公の入力禁止を解除

	sPlayer.ResumeAttach(CharaPlayer::eATTACH_MODEL_SWORD);
	sPlayer.ResumeAttach(CharaPlayer::eATTACH_MODEL_SUB_WEAPON);

	CameraManager::GetInstance()->LoadTemporaryCamWork();
	CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
	// 画角を戻す
	CameraManager::GetInstance()->SetPerspectiveCamera(kDEFAULT_PERSPECTIVE_FOV);

	sPlayer.ChangeAttachPattern(CharaPlayer::eATTACH_PATTERN_NORMAL);

	// 全ての敵の動きを有効にする
	CharaEnemy::SetStopAllEnemyFlag(FALSE);

	MSG_FADE_OUT(0.2f);

	mPhase = ePHASE_FINISH;
}

/*=====================================*
 * Smalle Treasure
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneGetItem::phaseSmallTreasureEnter(void)
{
	HIDE_BATTLE_ICON();
	TotalInfo::GetInstance()->HideBattleIcon();

	/* 初期設定 */
	ObjectBase* p_item = (ObjectTresure *)sTaskInfo.mTaskValueA;
	eItemKind item_index = (eItemKind)p_item->GetItemKind();
	const tItemDetailInfo * p_item_detail = GetItemDetailInfo(item_index);

	float rot;
	rot = p_item->Rotation().y;
	sPlayer.Position() = p_item->Position();
	sPlayer.Position().x += sin(rot) * length;
	sPlayer.Position().z += cos(rot) * length;
	sPlayer.SetRotationToTarget(&p_item->Position());

	sPlayer.ChangeAttachPattern(CharaPlayer::eATTACH_PATTERN_HAND);

	sPlayer.StartMotion(eMT_KICK_TREASURE_SHORT, false, 0.5f);
	p_item->StartMotion(eMT_TRESURE_EVENT_SHORT, false, 0.5f);

	CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_DUMMY);

	SetVolumeMusic(100);

	ModelBase* p_model = NULL;
	p_model = &mGettedItemModel;
	p_model->LinkModel(p_item_detail->mModelName);
	p_model->DisableToonShader();
	//	p_model->Scale() = sPlayer.Scale();
	p_model->Scale() = ZERO_VEC;
	GraphicManager::GetInstance()->EntryDrawModel(p_model);
	
	mPhaseCounter = 0;
	mPhase = ePHASE_SMALL_TREASURE_UPDATE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneGetItem::phaseSmallTreasureUpdate(void)
{
	ModelBase* p_model = NULL;
	p_model = &mGettedItemModel;

	ObjectBase* p_item = (ObjectTresure *)sTaskInfo.mTaskValueA;

	eItemKind item_index = (eItemKind)p_item->GetItemKind();
	const tItemDetailInfo * p_item_detail = GetItemDetailInfo(item_index);

	if (equalPhaseFrame(10.0f)) {
		MSG_TYPE_TELOP();
		MSG_FADE_IN(0.3f);
	}
	else if (equalPhaseFrame(12.0f)) {

		p_model->Scale() = sPlayer.Scale();

		/* 各アイテムごとの特別処理 */
		settingModelByItemKind(item_index);
	}
	else if (equalPhaseFrame(24.0f)) {
		SoundManager::GetSndComponent()->PlaySound(eSE_GETITEM);
	}
	else if (betweenPhaseFrame(12.0f, 192.0f)) {
		char tmp[256] = {0};
		const char *msg2, *msg3, *msg4;
		sprintf_s(tmp, sizeof(tmp), "%sをてにいれた！", p_item_detail->mDispName);
		msg2			= p_item_detail->mExplain0;
		msg3			= p_item_detail->mExplain1;
		msg4			= p_item_detail->mExplain2;

		if (item_index == eITEM_KIND_HEART_PIECE) {
			if ((TotalInfo::GetInstance()->GetItemNum(eSUBWEAPON_NUM_HEART_PIECE) % 4) == 3) {
				static const char* strHeart1Up = "４つ集まったのでハートが１つ増えるぞ！";
				msg4 = (char *)strHeart1Up;
			}
		}

		RENDER_MESSAGE_COL("", tmp, msg2, msg3, msg4, GetColor(0, 0, 0));

		if (CHECK_TRIG_ANY_JOY()) {
			mPhase = ePHASE_SMALL_TREASURE_LEAVE;
		}
	}
	else if (equalPhaseFrame(193.0f)) {
		mPhase = ePHASE_SMALL_TREASURE_LEAVE;
	}


	VECTOR tmpA, tmpB, hand_pos;
	sPlayer.GetFramePosFromName(&tmpA, "右手首");
	sPlayer.GetFramePosFromName(&tmpB, "左手首");
	hand_pos = VScale(VAdd(tmpA, tmpB), 0.5f);

	p_model->Position() = sPlayer.Position();
	p_model->Position().y = hand_pos.y + 40.0f;
	p_model->SetRotationToTarget(&CameraManager::GetInstance()->Position());

	VECTOR ch_top_pos = sPlayer.Position();
	ch_top_pos.y += sPlayer.BodyHeight();

	VECTOR cam_tar;
	if (p_model->Position().y > ch_top_pos.y) {
		cam_tar = p_model->Position();
	}
	else {
		cam_tar = ch_top_pos;
	}
	// カメラの座標を更新
	cam_tar = VScale(VAdd(cam_tar, ch_top_pos), 0.5f);
	CameraManager::GetInstance()->Target() = cam_tar;

	MATRIX mtx = MGetRotY(p_item->Rotation().y + PHI_F);
	VECTOR diff = VTransform(kCAM_DIFF, mtx);

	CameraManager::GetInstance()->Position() = VAdd(CameraManager::GetInstance()->Target(),	diff);
	CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_LOW, true);

	mPhaseCounter += (DWORD)kADD_FRAME_COUNTER;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneGetItem::phaseSmallTreasureLeave(void)
{
	ObjectBase* p_item = (ObjectTresure *)sTaskInfo.mTaskValueA;

	/* 終了設定 */
	TotalInfo::GetInstance()->EnableItemFlag((eItemKind)p_item->GetItemKind() );

	/* アイテム取得更新処理 */
	updateGetItem((eItemKind)p_item->GetItemKind());
	p_item->SetUsed();

	//	EventManager::GetInstance()->EndEvent();

	GraphicManager::GetInstance()->RemoveDrawModel(&mGettedItemModel);

	SetVolumeMusic(255);					// ボリュームを戻す
	VISIBLE_BATTLE_ICON();					// アイコンを表示させる
	TotalInfo::GetInstance()->VisibleBattleIcon();

	sPlayer.PermitAcceptInput(true);		// 主人公の入力禁止を解除

	sPlayer.ResumeAttach(CharaPlayer::eATTACH_MODEL_SWORD);
	sPlayer.ResumeAttach(CharaPlayer::eATTACH_MODEL_SUB_WEAPON);

	CameraManager::GetInstance()->LoadTemporaryCamWork();
	CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
	// 画角を戻す
	CameraManager::GetInstance()->SetPerspectiveCamera(kDEFAULT_PERSPECTIVE_FOV);

	sPlayer.ChangeAttachPattern(CharaPlayer::eATTACH_PATTERN_NORMAL);

	// 全ての敵の動きを有効にする
	CharaEnemy::SetStopAllEnemyFlag(FALSE);

	MSG_FADE_OUT(0.2f);

	mPhase = ePHASE_FINISH;
}

/*=====================================*
 * Crystall 
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneGetItem::phaseCrystalEnter(void)
{
	HIDE_BATTLE_ICON();
	TotalInfo::GetInstance()->HideBattleIcon();

	/* 初期設定 */
	ObjectBase* p_item = (ObjectTresure *)sTaskInfo.mTaskValueA;
	eItemKind item_index = (eItemKind)p_item->GetItemKind();
	const tItemDetailInfo * p_item_detail = GetItemDetailInfo(item_index);

	float rot;
	rot = p_item->Rotation().y;
	sPlayer.Position() = p_item->Position();
	sPlayer.Position().x += sin(rot) * length;
	sPlayer.Position().z += cos(rot) * length;
	sPlayer.SetRotationToTarget(&p_item->Position());

	sPlayer.ChangeAttachPattern(CharaPlayer::eATTACH_PATTERN_HAND);

	sPlayer.StartMotion(eMT_KICK_TREASURE_SHORT, false, 0.5f);
	p_item->StartMotion(eMT_TRESURE_EVENT_SHORT, false, 0.5f);

	CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_DUMMY);

	GraphicManager::GetInstance()->RevertFogParam(0.5f);

	int* p_eff = NULL;
	switch (sStageIndex) {
	case 28: p_eff = &((StageFunc028 *)spStageFunc)->mEffHndAura;	break;
	case 43: p_eff = &((StageFunc043 *)spStageFunc)->mEffHndAura;	break;
	default:													break;
	};
	APP_ASSERT((p_eff != NULL), "p_eff is NULL \n");
	if (*p_eff != -1) {
		EffectManager::GetInstance()->Stop(*p_eff, 0.0f);
		*p_eff = -1;
	}

	SetVolumeMusic(0);

	ModelBase* p_model = NULL;
	p_model = &mGettedItemModel;
	p_model->LinkModel(p_item_detail->mModelName);
	p_model->DisableToonShader();
	p_model->Scale() = sPlayer.Scale();
	GraphicManager::GetInstance()->EntryDrawModel(p_model);
	
	mPhaseCounter = 0;
	mPhase = ePHASE_CRYSTAL_UPDATE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneGetItem::phaseCrystalUpdate(void)
{
	ModelBase* p_model = NULL;
	p_model = &mGettedItemModel;

	ObjectBase* p_item = (ObjectTresure *)sTaskInfo.mTaskValueA;

	eItemKind item_index = (eItemKind)p_item->GetItemKind();
	const tItemDetailInfo * p_item_detail = GetItemDetailInfo(item_index);

	if (equalPhaseFrame(10.0f)) {
		MSG_TYPE_TELOP();
		MSG_FADE_IN(0.3f);
		SoundManager::GetSndComponent()->PlaySound(eSE_GET_KEYITEM);
	}
	else if (equalPhaseFrame(12.0f)) {
		
		/* 各アイテムごとの特別処理 */
		//		settingModelByItemKind(item_index);

		int graph_handle = -1;
		int event_index = ConvertPlaceIndex_TO_EventDefine(sStageIndex);
		APP_ASSERT_PRINT((event_index != eEVENT_ERROR),
						 "Select event_index %d is invalid value\n", event_index);
		switch (event_index) {
		case eEVENT_28:
			graph_handle = ResourceLayout::GetInstance()->GetHandle(RES_LYT_CRYSTAL_GREEN);
			break;
		case eEVENT_43:
			graph_handle = ResourceLayout::GetInstance()->GetHandle(RES_LYT_CRYSTAL_YELLOW);
			break;
		default:
			APP_ASSERT(NULL, "Not Support getItem crystal\n");
			break;
		};
		MV1SetTextureGraphHandle(p_model->GetModelHandle(), 0, graph_handle, FALSE);

	}
	else if (betweenPhaseFrame(12.0f, 492.0f)) {
		char tmp[256] = {0};
		const char *msg2, *msg3, *msg4;
		sprintf_s(tmp, sizeof(tmp), "%sをてにいれた！", p_item_detail->mDispName);
		msg2			= p_item_detail->mExplain0;
		msg3			= p_item_detail->mExplain1;
		msg4			= p_item_detail->mExplain2;
		RENDER_MESSAGE_COL("", tmp, msg2, msg3, msg4, GetColor(0, 0, 0));

		/*
		if (CHECK_TRIG_ANY_JOY()) {
			mPhase = ePHASE_CRYSTAL_LEAVE;
		}
		*/
	}
	else if (equalPhaseFrame(493.0f)) {
		mPhase = ePHASE_CRYSTAL_LEAVE;
	}


	VECTOR tmpA, tmpB, hand_pos;
	sPlayer.GetFramePosFromName(&tmpA, "右手首");
	sPlayer.GetFramePosFromName(&tmpB, "左手首");
	hand_pos = VScale(VAdd(tmpA, tmpB), 0.5f);

	p_model->Position() = sPlayer.Position();
	p_model->Position().y = hand_pos.y - 20.0f;
	p_model->SetRotationToTarget(&CameraManager::GetInstance()->Position());


	int* p_eff = NULL;
	switch (sStageIndex) {
	case 28: p_eff = &((StageFunc028 *)spStageFunc)->mEffHndCrystal;	break;
	case 43: p_eff = &((StageFunc043 *)spStageFunc)->mEffHndCrystal;	break;
	default:															break;
	};
	APP_ASSERT((p_eff != NULL), "p_eff is NULL \n");
	if (*p_eff != -1) {
		VECTOR eff_pos = p_model->Position();
		eff_pos.y += 80.0f;
		EffectManager::GetInstance()->Position(*p_eff) = eff_pos;
	}

	VECTOR ch_top_pos = sPlayer.Position();
	ch_top_pos.y += sPlayer.BodyHeight();

	VECTOR cam_tar;
	if (p_model->Position().y > ch_top_pos.y) {
		cam_tar = p_model->Position();
	}
	else {
		cam_tar = ch_top_pos;
	}
	// カメラの座標を更新
	cam_tar = VScale(VAdd(cam_tar, ch_top_pos), 0.5f);
	CameraManager::GetInstance()->Target() = cam_tar;

	MATRIX mtx = MGetRotY(p_item->Rotation().y + PHI_F);
	VECTOR diff = VTransform(kCAM_DIFF, mtx);

	CameraManager::GetInstance()->Position() = VAdd(CameraManager::GetInstance()->Target(),	diff);
	CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_LOW, true);

	mPhaseCounter += (DWORD)kADD_FRAME_COUNTER;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneGetItem::phaseCrystalLeave(void)
{
	ObjectBase* p_item = (ObjectTresure *)sTaskInfo.mTaskValueA;

	/* 終了設定 */
	TotalInfo::GetInstance()->EnableItemFlag((eItemKind)p_item->GetItemKind() );

	/* アイテム取得更新処理 */
	updateGetItem((eItemKind)p_item->GetItemKind());
	p_item->SetUsed();

	GraphicManager::GetInstance()->RemoveDrawModel(&mGettedItemModel);

	VISIBLE_BATTLE_ICON();					// アイコンを表示させる
	TotalInfo::GetInstance()->VisibleBattleIcon();

	sPlayer.PermitAcceptInput(true);		// 主人公の入力禁止を解除
	sPlayer.ResumeAttach(CharaPlayer::eATTACH_MODEL_SWORD);
	sPlayer.ResumeAttach(CharaPlayer::eATTACH_MODEL_SUB_WEAPON);
	sPlayer.ChangeAttachPattern(CharaPlayer::eATTACH_PATTERN_NORMAL);

	CameraManager::GetInstance()->LoadTemporaryCamWork();
	CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
	CameraManager::GetInstance()->SetPerspectiveCamera(kDEFAULT_PERSPECTIVE_FOV);

	// 全ての敵の動きを有効にする
	CharaEnemy::SetStopAllEnemyFlag(FALSE);

	MSG_FADE_OUT(0.2f);
	
	
	int* p_eff = NULL;
	switch (sStageIndex) {
	case 28: p_eff = &((StageFunc028 *)spStageFunc)->mEffHndAura;	break;
	case 43: p_eff = &((StageFunc043 *)spStageFunc)->mEffHndAura;	break;
	default:														break;
	};
	APP_ASSERT((p_eff != NULL), "p_eff is NULL \n");
	if (*p_eff != -1) {
		EffectManager::GetInstance()->Stop(*p_eff, 0.0f);
		*p_eff = -1;
	}


	/* クリスタルの間に移動させる */
	ActionTaskManager::tTaskInfo param;
	param.mTaskType = ActionTaskManager::eTASK_MOVE;
	param.mTaskValueA = 38;
	param.mTaskValueB = 0;
	ActionTaskManager::GetInstance()->NotifyAction(&param);

	SoundManager::GetSndComponent()->PlaySound(eSE_MAGIC);
	EffekseerManager::GetInstance()->Play(eEFK_DISAPPER, &sPlayer.Position(), &VGet(12.0f, 12.0f, 12.0f));

	mPhase = ePHASE_CRYSTAL_MOVING;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneGetItem::phaseCrystalMoving(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneGetItem::phaseFinish(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SubSceneGetItem::settingModelByItemKind(eItemKind itemKind)
{
	ModelBase* p_model = &mGettedItemModel;
		
	/* 各アイテムごとの特別処理 */
	switch (itemKind) {
	case eITEM_KIND_LIB_SHIELD:
		p_model->Rotation().y += Utility::DegToRad(180.0f);
		MV1SetMeshVisible(p_model->GetModelHandle(), LIBSHIELD_LIGHT_MESH_IDX, FALSE);
		break;
	case eITEM_KIND_ANGEL_WING:
		{
			// カメラと反対方向を向かせる
			VECTOR vec = VAdd(p_model->Position(), 
							  VSub(p_model->Position(), CameraManager::GetInstance()->Position() ) );
			p_model->SetRotationToTarget(&vec);
		}
		break;

	case eITEM_KIND_CARD_K:
	case eITEM_KIND_CARD_Q:
	case eITEM_KIND_CARD_J:
		p_model->Rotation().x += Utility::DegToRad(90.0f);
		break;
			
	default:
		break;
	};

	// プレートモデルかどうかをプレート名取得から判定し、
	// プレートの場合はプレートテクスチャを貼る
	char* plate_name = GetItemPlateName(itemKind);
	// プレート設定の場合の処理
	if (plate_name != NULL) {
		p_model->ChangeTexture(0, (char *)RES_LYT_PLATE_COMMON, TRUE);
		p_model->ChangeTexture(2, plate_name, TRUE);
	}
	else {
		char* card_name = GetItemCardName(itemKind);
		// カード設定の場合の処理
		if (card_name != NULL) {
			p_model->ChangeTexture(0, card_name, FALSE);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
SubSceneGetItem::equalPhaseFrame(float frame)
{
	return ( ( (frame >= mPhaseCounter) && (frame < mPhaseCounter + kADD_FRAME_COUNTER)) ? true : false);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
SubSceneGetItem::betweenPhaseFrame(float frameMin, float frameMax)
{
	return (( ( frameMin <= mPhaseCounter) && (frameMax > mPhaseCounter) ) ? true : false);
}

/**** end of file ****/


