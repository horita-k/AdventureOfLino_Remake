/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * NPC管理クラス
 * @author SPATZ.
 * @data   2012/04/14 19:35:52
 */
//---------------------------------------------------
#include "CharaNpc.h"
#include "CharaPlayer.h"
#include "AppLib/Basic/Basic.h"
#include "AppLib/Basic/Path.h"
#include "DataDefine/MotionDef.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Collision/CollisionManager.h"
#include "AppLib/Resource/ResourceManager.h"
#include "CharaInfo.h"
#include "Utility/Utility.h"
#include "DataDefine/CharaDef.h"
#include "DataDefine/messagedef.h"
#include "Scene/ActionTaskManager.h"

/*=====================================*
 * static
 *=====================================*/
/*static*/const char*	CharaNpc::strCsvRootPath = "data/CSV/npc";
/*static*/CsvAccesser	CharaNpc::sStageInfoCsv;
/*static*/float			CharaNpc::sBGScale = 0.0f;
/*static*/BYTE			CharaNpc::sCsvSubNo = 0;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaNpc::CharaNpc()
{
	mNpcIndex = -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
CharaNpc::~CharaNpc()
{
	GraphicManager::GetInstance()->RemoveDrawShadow(&mShadow);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaNpc::Setup(bool isShadow/*=false*/)
{
	ModelCollision::Setup();
	
	// モデルの大きさから自動的に BodySize と BodyHeight を割り当てる
	setupBodyHeightSize();

	if (isShadow) {
		mShadow.Setup(this, Shadow::eUPDATE_TYPE_ALWAYS);
	}
	else {
		mShadow.Setup(this, Shadow::eUPDATE_TYPE_ONCE);
	}

	StartMotionEx(&sMotionParam[eMT_WALK]);

	phaseEnterStateIdle();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaNpc::Update(void)
{
	ModelPmd::Update();
	mMoveVec = ZERO_VEC;
	phaseStateIdle();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
CharaNpc::SetupNpcResourceInfo(int stageIndex)
{
	// ファイル名："data/CSV/npc/stXXX_YY.csv";
	char csv_file[32] = {0};
	sprintf_s(csv_file, sizeof(csv_file), "%s/st%03d_%02d.csv", strCsvRootPath, stageIndex, sCsvSubNo);
	
	sStageInfoCsv.LoadFile(csv_file);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/int
CharaNpc::GetNpcResourceNum(void)
{
	return sStageInfoCsv.GetRowNum();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/char*
CharaNpc::GetNpcResourceName(int npcIndex)
{
	return sStageInfoCsv.GetString(npcIndex, eCSV_FILE_NAME);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/CharaNpc* 
CharaNpc::CreateNpcInstance(float bgScale)
{
	CharaNpc* p_instance = NULL;

	sBGScale = bgScale;

	int npc_num = sStageInfoCsv.GetRowNum();

	// CSVの数に応じてNPCクラスを生成する
	if (npc_num > 0) {
		p_instance = new CharaNpc[npc_num];
		APP_ASSERT_PRINT(p_instance != NULL,
						 "Failed new CharaNpc[%d]\n", npc_num);

		for (int iI = 0; iI < npc_num; iI ++) {
			p_instance[iI].SetupCsvInfo(iI, (VECTOR &)ZERO_VEC);
		}
	}
	
	return p_instance;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/CharaNpc*
CharaNpc::RecreateNpcInstance(int csvSubNo, CharaNpc* pNpcInstance, int& rNpcNum, int stageIndex, float bgScale)
{
	if (pNpcInstance != NULL) {
		for (int iI = 0; iI < rNpcNum; iI ++) {
			GraphicManager::GetInstance()->RemoveDrawModel(&pNpcInstance[iI]);	// 描画エントリー
			CollisionManager::GetInstance()->RemoveBodyCollision(&pNpcInstance[iI]);
			pNpcInstance[iI].DeleteModel();
		}
		delete[] pNpcInstance;
		pNpcInstance = NULL;
	}

	sCsvSubNo = csvSubNo;

	// CSVファイルの読み直し
	sStageInfoCsv.DeleteFile();
	SetupNpcResourceInfo(stageIndex);

	rNpcNum = sStageInfoCsv.GetRowNum();
	for (int iI = 0; iI < rNpcNum; iI ++) {
		ResourceManager::GetInstance()->LoadCharaResource(sStageInfoCsv.GetString(iI, eCSV_FILE_NAME));
	}
	return CharaNpc::CreateNpcInstance(bgScale);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
CharaNpc::SetCsvSubNo(BYTE subNo)
{
	sCsvSubNo = subNo;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/BYTE
CharaNpc::GetCsvSubNo(void)
{
	return sCsvSubNo;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaNpc::SetupCsvInfo(BYTE npcIndex, VECTOR& rDiffPos)
{
	mNpcIndex = npcIndex;

	char* resource_name = sStageInfoCsv.GetString(mNpcIndex, eCSV_FILE_NAME);

	this->LinkCharaResource(resource_name);
	
	mPosition = VScale(VGet(sStageInfoCsv.GetFloat(mNpcIndex, eCSV_POS_X),
							sStageInfoCsv.GetFloat(mNpcIndex, eCSV_POS_Y),
							sStageInfoCsv.GetFloat(mNpcIndex, eCSV_POS_Z) ), sBGScale);
	mPosition = VSub(mPosition, rDiffPos);
	mRotation	= VGet(0.0f,
					   Utility::DegToRad(sStageInfoCsv.GetFloat(mNpcIndex, eCSV_ROT_Y) ),
					   0.0f);
	mScale		= CHARA_DEF_SCALE;
			
	this->Setup();

	if (sStageInfoCsv.GetBool(mNpcIndex, eCSV_IS_TOON)) {
		this->EnableToonShader(0.02f, 0.02f);
	}
	else {
		this->DisableToonShader();
	}
	GraphicManager::GetInstance()->EntryDrawModel(this);	// 描画エントリー
	CollisionManager::GetInstance()->EntryBodyCollision(this);
			
	if (strcmp(resource_name, NAME_Seek) == 0) {
		SetDrawFunc((ModelBase::DRAW_FUNC)DrawSeekFunc);		// シーク専用の描画関数を設定
	}
	else if (strcmp(resource_name, NAME_Kitsune) == 0) {
		SetDrawFunc((ModelBase::DRAW_FUNC)DrawKitsuneFunc);
	}

	this->StartMotion(sStageInfoCsv.GetInt(mNpcIndex, eCSV_MOTION_IDEL), true, 0.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaNpc::LinkCharaResource(const char* charaName)
{
	// モデルのリソース関連付け
	ModelBase::LinkModel(charaName);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaNpc::EnableShadow(void)
{
	mShadow.EnableShadow();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaNpc::DisableShadow(void)
{
	mShadow.DisableShadow();
}

//---------------------------------------------------
/**
	他のキャラとのあたり判定
 */
//---------------------------------------------------
void
CharaNpc::UpdateAttackChara(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaNpc::SetShadowVector(VECTOR moveVec)
{
	mShadow.SetShadowVector(moveVec);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaNpc::ReloadInformation(int stageIndex, VECTOR& rDiffPos)
{
	// CSVファイルのリロード
	sStageInfoCsv.DeleteFile();
	SetupNpcResourceInfo(stageIndex);

	this->DeleteModel();
	this->SetupCsvInfo(mNpcIndex, rDiffPos);
}

/**** NPC特有の処理 ****/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaNpc::StartIdleMotion(void)
{
	this->StartMotion((int)sStageInfoCsv.GetFloat(mNpcIndex, eCSV_MOTION_IDEL), true, 0.5f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaNpc::StartTalkMotion(void)
{
	this->StartMotion((int)sStageInfoCsv.GetFloat(mNpcIndex, eCSV_MOTION_TALK), true, 0.0f, NULL, 0.05f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaNpc::RenderNpcMessage(void)
{
	char* msg[GraphicManager::kMESSAGE_LINE_NUM] = {
		sStageInfoCsv.GetString(mNpcIndex, eCSV_MSG_00),
		sStageInfoCsv.GetString(mNpcIndex, eCSV_MSG_01),
		sStageInfoCsv.GetString(mNpcIndex, eCSV_MSG_02),
		sStageInfoCsv.GetString(mNpcIndex, eCSV_MSG_03)
	};

	GraphicManager::GetInstance()->RenderMessage(sStageInfoCsv.GetString(mNpcIndex, eCSV_NAME), msg);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CharaNpc::StartAfterTalkEvent(void)
{
	int task_id = sStageInfoCsv.GetInt(mNpcIndex, eCSV_AFTER_EVT_NO);
	if (task_id != -1) {
		if (ActionTaskManager::GetInstance()->NotifyAction( (ActionTaskManager::tTaskInfo *)GetNpcTaskTable(task_id) ) == true) {
			return true;
		}
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
CharaNpc::GetOptionValue(void)
{
	return sStageInfoCsv.GetInt(mNpcIndex, eCSV_OPTION);
}

/*=====================================*
 * mPhaseState
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaNpc::phaseEnterStateIdle(void)
{
	this->SetMatrixType(ModelBase::eMATRIX_TYPE_TRS);
	StartMotionEx(&sMotionParam[eMT_IDLE]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CharaNpc::phaseStateIdle(void)
{
	return;
}


//---------------------------------------------------
/**
 * シーク専用の描画関数
 * 前髪の透過をなんとかする為
 */
//---------------------------------------------------
void DrawSeekFunc(int model_handle)
{
	int mesh_num = MV1GetMeshNum(model_handle);
	for (int iI = 0; iI < 13; iI ++) {
		MV1DrawMesh(model_handle, iI);
	}
	for (int iI = 13+1; iI < mesh_num; iI ++) {
		MV1DrawMesh(model_handle, iI);
	}
	MV1DrawMesh(model_handle, 13);
}

/**** end of file ****/


