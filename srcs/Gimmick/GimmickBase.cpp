/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ギミックベース
 * @author SPATZ.
 * @data   2015/01/09 00:15:30
 */
//---------------------------------------------------
#include "AppLib/Basic/Basic.h"
#include "GimmickBase.h"
#include "GimmickExtend.h"
#include "DataDefine/MotionDef.h"
#include "../Charactor/CharaPlayer.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Collision/CollisionManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "AppLib/Sound/SoundManager.h"
#include "Scene/ActionTaskManager.h"
#include "Utility/Utility.h"
#include "../Scene/ExternDefine.h"
#include "../Scene/CommandFunctions.h"
#include "DataDefine/VoiceDef.h"

/*=====================================*
 * static
 *=====================================*/
static std::vector<GimmickBase *>	sCollisionGimmick;	// ギミック情報格納コンテナ
static CsvAccesser					sStageInfoCsv;		// ステージ毎のギミック情報csv

static GimmickBase::tOnNodeInfo sOnNodeList[GimmickBase::kON_NODE_MODEL_NUM];

/*static*/const char* GimmickBase::strCsvRootPath = "data/CSV/gmk";
/*static*/VECTOR GimmickBase::sRevisePos	= ZERO_VEC;
/*static*/BOOL	 GimmickBase::sPauseAttach	= FALSE;

/* ギミックパラメータテーブル */
static const GimmickBase::tGimmickBaseParam sGimmickBaseParam[eGIMMICK_KIND_MAX] = {
	{RES_MDL_LIFT,				"LIF",			},	// eGIMMICK_KIND_LIFT
	{RES_MDL_SHIP,				"SHI",			},	// eGIMMICK_KIND_SHIP
	{RES_MDL_ROLLMOGU,			"ROL",			},	// eGIMMICK_KIND_ROLLMOGU
	{RES_MDL_BIGKEY,			"KEY",			},	// eGIMMICK_KIND_KEY
	{RES_MDL_TROLLEY,			"TRO",			},	// eGIMMICK_KIND_TROLLEY
	{RES_MDL_ELEVATOR,			"ELE",			},	// eGIMMICK_KIND_ELEVATOR
	{RES_MDL_TOWER,				"TOW",			},	// eGIMMICK_KIND_TOWER
	{RES_MDL_ELEVATOR_B,		"ELEB",			},	// eGIMMICK_KIND_ELEVATOR_B
	{RES_MDL_BABEL_SWITCH,		"BabelSwitch",	},	// eGIMMICK_KIND_BABEL_SWITCH
	{RES_MDL_HOLE,				"HOL",			},	// eGIMMICK_KIND_HOLE
	{RES_MDL_WHELL,				"WHE",			},	// eGIMMICK_KIND_WHEEL
	{RES_MDL_EYE,				"EYE",			},	// eGIMMICK_KIND_EYE
	{RES_MDL_CANNON,			"CAN",			},	// eGIMMICK_KIND_CANNON
	{RES_MDL_ELEVATOR_B,		"ELEC",			},	// eGIMMICK_KIND_ELEVATOR_B
	{RES_MDL_ELEVATOR_B,		"Lean",			},	// eGIMMICK_KIND_LEAN_FLOOR
	{RES_MDL_AIRSHIP,			"AIR",			},	// eGIMMICK_KIND_AIRSHIP
	{RES_MDL_ELEVATOR_B,		"LiftAuto",		},	// eGIMMICK_KIND_LIFT_AUTO
	{RES_MDL_CLOCK,				"Clock",		},	// eGIMMICK_KIND_CLOCK
	{RES_MDL_CANNON,			"Switch",		},	// eGIMMICK_KIND_SWITCH
	{RES_MDL_ROLLMOGU,			"EleMogu",		},	// eGIMMICK_KIND_ELEVATOR_MOGU
	{RES_MDL_ROLLMOGU,			"FootSwitch",	},	// eGIMMICK_KIND_FOOT_SWITCH
	{RES_MDL_TETO,				"Teto",			},	// eGIMMICK_KIND_TETO
	{RES_MDL_TROLLEY,			"TrolleyEx",	},	// eGIMMICK_KIND_TROLLEY_EX
	{RES_MDL_HOLE,				"PLight",		},	// eGIMMICK_KIND_PLIGHT
	{RES_MDL_BARRIER,			"Barrier",		},	// eGIMMICK_KIND_BARRIER
	{RES_MDL_ELEVATOR_B,		"FloatBlock",	},	// eGIMMICK_KIND_FLOAT_BLOCK
	{RES_MDL_MAGIC_CUBE,		"MagicCube",	},	// eGIMMICK_KIND_MAGIC_CUBE
	{RES_MDL_MAGIC_CUBE,		"PressWall",	},	// eGIMMICK_KIND_PRESS_WALL
	{RES_MDL_LEAN_PASSAGE,		"LeanPassage",	},	// eGIMMICK_KIND_LEAN_PASSAGE
	{RES_MDL_ROLLING_PIPE,		"RollingPipe",	},	// eGIMMICK_KIND_ROLLING_PIPE
	{RES_MDL_SCREW,				"Screw",		},	// eGIMMICK_KIND_SCREW
	{RES_MDL_LASER_BEAM,		"LaserBeam",	},	// eGIMMICK_KIND_LASER_BEAM
	{RES_MDL_MAGIC_CUBE,		"PressWallHard",},	// eGIMMICK_KIND_PRESS_WALL_HARD
	{RES_MDL_SHIP,				"Boat",			},	// eGIMMICK_KIND_BOAT
};

/*=====================================*
 * class GimmickBase
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickBase::GimmickBase()
{
	mState				= 0;
	mCounter			= 0;
	mCsvNo				= 0;
	mExtendBufferNum	= 0;
	mpExtendBuffer		= NULL;
	mWakeupEvent		= -1;
	mOnNodeInfoHandleBit= 0x00;
	mGimmickNodeType	= eGMK_NODE_TYPE_PROGRAM;		// デフォルトはプログラム指定
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickBase::~GimmickBase()
{
	if (mpExtendBuffer != NULL) {
		delete[] mpExtendBuffer;
		mpExtendBuffer = NULL;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/bool
GimmickBase::LoadAllGimmick(int stageIndex)
{
	// 一度vectorをクリアする
	sCollisionGimmick.clear();
	memset(sOnNodeList, 0, sizeof(sOnNodeList) );

	char csv_file[32] = {0};
	// data/CSV/gmk/g???.csv
	sprintf_s(csv_file, sizeof(csv_file), "%s/g%03d.csv", strCsvRootPath, stageIndex);
	
	// csvファイルが存在しない場合はエラーリターン
	if (sStageInfoCsv.LoadFile(csv_file) == false) {
		return false;
	}

	int gimmick_num = 0;
	char* str_keyword = NULL;
	GimmickBase* p_instance = NULL;
	int gimmick_kind = 0;
	BYTE extend_buf_num = 0;

	gimmick_num = sStageInfoCsv.GetRowNum();
	for (int iI = 0; iI < gimmick_num; iI ++) {
		str_keyword = sStageInfoCsv.GetString(iI, 0);

		// 該当のキーの検索
		for (gimmick_kind = 0; gimmick_kind < eGIMMICK_KIND_MAX; gimmick_kind ++) {
			if (EQUAL_STRING(str_keyword, sGimmickBaseParam[gimmick_kind].mCsvKeyword) ) {
				break;
			}
		}
		APP_ASSERT( (gimmick_kind < eGIMMICK_KIND_MAX),
					"Not Found gimmick_kind \n");

		// 該当のリソースを読み込む
		ResourceModel::GetInstance()->LoadGimmickResource(sGimmickBaseParam[gimmick_kind].mResourceName);
		
		p_instance = NULL;
		
		switch (gimmick_kind) {
		case eGIMMICK_KIND_LIFT:
		case eGIMMICK_KIND_ELEVATOR_B:
		case eGIMMICK_KIND_ELEVATOR_MOGU:
			p_instance = new GimmickExtend_Lift();
			extend_buf_num = GimmickExtend_Lift::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_ELEVATOR_C:
			p_instance = new GimmickExtend_LiftLoop();
			extend_buf_num = GimmickExtend_Lift::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_SHIP:
			p_instance = new GimmickExtend_Ship();
			extend_buf_num = 0;
			break;
		case eGIMMICK_KIND_ROLLMOGU:
		case eGIMMICK_KIND_KEY:
		case eGIMMICK_KIND_WHEEL:
		case eGIMMICK_KIND_EYE:
		case eGIMMICK_KIND_ROLLING_PIPE:
		case eGIMMICK_KIND_BOAT:
			p_instance = new GimmickExtend_Roll();
			extend_buf_num = GimmickExtend_Roll::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_SCREW:
		case eGIMMICK_KIND_LASER_BEAM:
			p_instance = new GimmickExtend_RollDamage();
			extend_buf_num = GimmickExtend_RollDamage::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_TROLLEY:
			p_instance = new GimmickExtend_Trolley();
			extend_buf_num = GimmickExtend_Trolley::eCSVLINE_MAX;
			break;
		case eGIMMICK_KIND_ELEVATOR:
			p_instance = new GimmickExtend_Motion();
			extend_buf_num = GimmickExtend_Motion::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_CLOCK:
			p_instance = new GimmickExtend_Clock();
			extend_buf_num = GimmickExtend_Clock::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_TOWER:
			p_instance = new GimmickExtend_Tower();
			extend_buf_num = GimmickExtend_Tower::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_HOLE:
			p_instance = new GimmickExtend_Hole();
			extend_buf_num = GimmickExtend_Hole::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_CANNON:
			p_instance = new GimmickExtend_Cannon();
			extend_buf_num = 0;
			break;

		case eGIMMICK_KIND_LEAN_FLOOR:
		case eGIMMICK_KIND_LEAN_PASSAGE:
			p_instance = new GimmickExtend_LeanFloor();
			extend_buf_num = GimmickExtend_LeanFloor::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_LIFT_AUTO:
			p_instance = new GimmickExtend_LiftAuto();
			extend_buf_num = GimmickExtend_LiftAuto::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_SWITCH:
			p_instance = new GimmickExtend_Switch();
			extend_buf_num = GimmickExtend_Switch::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_FOOT_SWITCH:
		case eGIMMICK_KIND_BABEL_SWITCH:
			p_instance = new GimmickExtend_FootSwitch();
			extend_buf_num = GimmickExtend_FootSwitch::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_TETO:
			p_instance = new GimmickExtend_Teto();
			extend_buf_num = GimmickExtend_Teto::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_TROLLEY_EX:
			p_instance = new GimmickExtend_TrolleyEx();
			extend_buf_num = GimmickExtend_TrolleyEx::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_PLIGHT:
			p_instance = new GimmickExtend_PLight();
			extend_buf_num = GimmickExtend_PLight::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_BARRIER:
			p_instance = new GimmickExtend_Barrier();
			extend_buf_num = GimmickExtend_Barrier::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_AIRSHIP:
			p_instance = new GimmickExtend_AirShip();
			extend_buf_num = GimmickExtend_AirShip::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_FLOAT_BLOCK:
			p_instance = new GimmickExtend_FloatBlock();
			extend_buf_num = GimmickExtend_FloatBlock::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_MAGIC_CUBE:
			p_instance = new GimmickExtend_MagicCube();
			extend_buf_num = GimmickExtend_MagicCube::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_PRESS_WALL:
			p_instance = new GimmickExtend_PressWall();
			extend_buf_num = GimmickExtend_PressWall::eCSVLINE_MAX;
			break;

		case eGIMMICK_KIND_PRESS_WALL_HARD:
			p_instance = new GimmickExtend_PressWallHard();
			extend_buf_num = GimmickExtend_PressWallHard::eCSVLINE_MAX;
			break;

		default:
			APP_ASSERT( (NULL),
						"Invalid gimmick kind \n");
			break;
		};
		
		APP_ASSERT( (p_instance != NULL),
					"Failed new GimmickBase() \n");
		p_instance->LinkModel(sGimmickBaseParam[gimmick_kind].mResourceName);
		p_instance->LoadParamCsv(stageIndex, iI, extend_buf_num, gimmick_kind);

		sCollisionGimmick.push_back(p_instance);
	}

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
GimmickBase::DeleteAllGimmick(void)
{
	std::vector<GimmickBase *>::iterator ite;
	for (ite = sCollisionGimmick.begin(); ite != sCollisionGimmick.end(); ite ++) {

		((GimmickBase *)(*ite))->DeleteModel();
		delete ( (GimmickBase *)(*ite) );
	}

	sCollisionGimmick.clear();
	memset(sOnNodeList, 0, sizeof(sOnNodeList) );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
GimmickBase::SetupAllGimmick(void)
{
	GimmickBase::sRevisePos = ZERO_VEC;

	std::vector<GimmickBase *>::iterator ite;
	for (ite = sCollisionGimmick.begin(); ite != sCollisionGimmick.end(); ite ++) {
		((GimmickBase *)(*ite))->Setup();
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickBase::GetBodyCapsule(ShapeCapsule* pCapsule)
{
	// コールされるのが想定外の関数のため
	//	APP_ASSERT( (NULL), 
	//				"Not Support GimmickBase::GetBodyCapsule() \n");

	//<! たてまえ上、絶対当たらない値に設定する
	pCapsule->mPosA	= ZERO_VEC;
	pCapsule->mPosB	= ZERO_VEC;
	pCapsule->mRadius= 0.0f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickBase::SetNowGravity(float gravity)
{
	APP_ASSERT( (NULL), "Not Support this function \n");
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
GimmickBase::GetGimmickBaseNum(void)
{
	return sCollisionGimmick.size();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickBase* 
GimmickBase::GetGimmickBase(int gimmickIndex)
{
	return sCollisionGimmick.at(gimmickIndex);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
GimmickBase* 
GimmickBase::GetGimmickBaseFromEarthCollIndex(INT8 onCollisionIndex)
{
	int mdl_handle = CollisionManager::GetInstance()->GetEarthModelHandle(onCollisionIndex);

	std::vector<GimmickBase *>::iterator ite;
	for (ite = sCollisionGimmick.begin(); ite != sCollisionGimmick.end(); ite ++) {
		// 登録されているギミックと同等の場合はギミックのポインタを返す
		if ((*ite)->GetModelHandle() == mdl_handle) {
			return (*ite);
		}
	}
	return NULL;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/GimmickBase*
GimmickBase::GetGimmickBaseFromWallCollIndex(INT8 onCollisionIndex)
{
	int mdl_handle = CollisionManager::GetInstance()->GetWallModelHandle(onCollisionIndex);
	
	std::vector<GimmickBase *>::iterator ite;
	for (ite = sCollisionGimmick.begin(); ite != sCollisionGimmick.end(); ite ++) {
		// 登録されているギミックと同等の場合はギミックのポインタを返す
		if ((*ite)->GetModelHandle() == mdl_handle) {
			return (*ite);
		}
	}
	return NULL;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
GimmickBase::ReloadInformationAll(int stageNo)
{
	
	sStageInfoCsv.DeleteFile();
	char csv_file[32] = {0};
	// data/CSV/gmk/g???.csv
	sprintf_s(csv_file, sizeof(csv_file), "%s/g%03d.csv", strCsvRootPath, stageNo);
	sStageInfoCsv.LoadFile(csv_file);


	std::vector<GimmickBase *>::iterator ite;
	for (ite = sCollisionGimmick.begin(); ite != sCollisionGimmick.end(); ite ++) {
		((GimmickBase *)(*ite))->ReloadInformation(stageNo);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
GimmickBase::ReloadInformationAllFallDown(int stageNo)
{
	
	sStageInfoCsv.DeleteFile();
	char csv_file[32] = {0};
	// data/CSV/gmk/g???.csv
	sprintf_s(csv_file, sizeof(csv_file), "%s/g%03d.csv", strCsvRootPath, stageNo);
	sStageInfoCsv.LoadFile(csv_file);


	std::vector<GimmickBase *>::iterator ite;
	for (ite = sCollisionGimmick.begin(); ite != sCollisionGimmick.end(); ite ++) {

		if (((GimmickBase *)(*ite))->GetKind() == eGIMMICK_KIND_PRESS_WALL_HARD) {
			// nop.
		}		
		else {
			((GimmickBase *)(*ite))->ReloadInformation(stageNo);
		}

	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
GimmickBase::ReviseAllGimmickOnPosition(VECTOR& rMoveVec)
{
	GimmickBase* p_gimmick = NULL;

	GimmickBase::sRevisePos = VAdd(GimmickBase::sRevisePos, rMoveVec);

	std::vector<GimmickBase *>::iterator ite;
	for (ite = sCollisionGimmick.begin(); ite != sCollisionGimmick.end(); ite ++) {
		p_gimmick = (*ite);

		p_gimmick->Position() = VSub(p_gimmick->Position(), rMoveVec);
		p_gimmick->ForciblySetPositionByCentering(true);

		p_gimmick->ReviseOnPosition(rMoveVec);
	}

	// トロッコのコリジョンも更新させる
	GimmickExtend_TrolleyEx::UpdateTrolleyExCollision();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/CHAR
GimmickBase::GetEmptyOnNodeListIndex(void)
{
	CHAR iI;
	for (iI = 0; iI < kON_NODE_MODEL_NUM; iI ++) {
		if (sOnNodeList[iI].mpOnModel == NULL) {
			return iI;
		}
	}
	return -1;
}

//---------------------------------------------------
/**

 */
//---------------------------------------------------
/*static*/GimmickBase::tOnNodeInfo*	
GimmickBase::GetOnNodeInfo(CHAR nodeIndex)
{
	return &sOnNodeList[nodeIndex];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/GimmickBase*
GimmickBase::Search1stFindGimmickKind(eGimmickKind kind)
{
	std::vector<GimmickBase *>::iterator ite;
	for (ite = sCollisionGimmick.begin(); ite != sCollisionGimmick.end(); ite ++) {
		if ((*ite)->GetKind() == kind) {
			return (*ite);
		}
	}

	return NULL;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
GimmickBase::SetGimmickPauseAttach(BOOL isAttach)
{
	sPauseAttach = isAttach;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
GimmickBase::SetVisibleAll(bool isVisible)
{
	std::vector<GimmickBase *>::iterator ite;
	for (ite = sCollisionGimmick.begin(); ite != sCollisionGimmick.end(); ite ++) {
		(*ite)->SetVisible(isVisible);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickBase::DeleteModel(void)
{
	ModelPmd::DeleteModel();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickBase::LoadParamCsv(int stageIndex, BYTE csvNo, BYTE extendBufNum, BYTE gimmickKind)
{
	mCsvNo			= csvNo;
	mGimmickKind	= gimmickKind;

	mExtendBufferNum = extendBufNum;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickBase::ReloadInformation(int stageIndex)
{
	// セットアップしなおす
	commonSetup();
	extendSetup();		/* 拡張 Setup() */
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
float
GimmickBase::GetGimmickExtParamValue(BYTE extParamIndex)
{
	APP_ASSERT_PRINT( (extParamIndex < mExtendBufferNum),
					  "Select paramIndex : %d is Invalid Value \n", extParamIndex);
	return mpExtendBuffer[extParamIndex];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickBase::SetGimmickExtParamValue(BYTE extParamIndex, float paramValue)
{
	APP_ASSERT_PRINT( (extParamIndex < mExtendBufferNum),
					  "Select paramIndex : %d is Invalid Value \n", extParamIndex);
	mpExtendBuffer[extParamIndex] = paramValue;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickBase::Setup(void)
{
	this->Position()	= ZERO_VEC;
	this->Rotation()	= ZERO_VEC;
	this->Scale()		= ZERO_VEC;
	this->mBodySize		= 0.0f;
	this->mBodyHeight	= 0.0f;

	ModelCollision::Setup();

	GraphicManager::GetInstance()->EntryDrawModel(this);
	CollisionManager::GetInstance()->EntryEarthCollision(mModelHandle, -1);
	CollisionManager::GetInstance()->EntryWallCollision(mModelHandle, -1);

	GraphicManager::GetInstance()->EntryDrawPreUpdateModel(this);

	mState = 0;

	
	strcpy_s(mOnNodeName, sizeof(mOnNodeName), "OnPoint00");

	MV1SetUseVertDifColor( mModelHandle, TRUE);
	MV1SetUseVertSpcColor( mModelHandle, TRUE);

	commonSetup();
	extendSetup();		/* 拡張 Setup() */
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickBase::PreDraw(void)
{
	// αテスト判定の値は100が理想 (実験済み)
#ifdef LIB_VER_3_12e
	MV1SetMaterialDrawAlphaTestAll(mModelHandle, TRUE, DX_CMP_GREATER, 128);
#else
	SetDrawAlphaTest(DX_CMP_GREATER, 100);
#endif

	ModelPmd::Update();

#ifdef LIB_VER_3_12e
	MV1SetMaterialDrawAlphaTestAll(mModelHandle, FALSE, DX_CMP_GREATER, 0);
#else
	SetDrawAlphaTest( -1, 0 ) ;
#endif
}

//---------------------------------------------------
/**
 * プレイヤーを動かす前のアップデート (１回目)
 */
//---------------------------------------------------
void
GimmickBase::PreUpdate(void)
{
	// αテスト判定の値は100が理想 (実験済み)
	SetDrawAlphaTest(DX_CMP_GREATER, 100);
	ModelPmd::Update();
	SetDrawAlphaTest( -1, 0 ) ;

	//<! 拡張 Update()
	extendUpdate();

	CHAR node_index = 0;
	tOnNodeInfo* p_info = NULL;

	// 自モデル(ギミック)に乗っている全てのモデルに対して while() で制御する
	while (mOnNodeInfoHandleBit != 0x00) {
		if (mOnNodeInfoHandleBit & 0x01) {
			p_info = GimmickBase::GetOnNodeInfo(node_index);
			APP_ASSERT( (p_info != NULL),
						"tOnNodeInfo is NULL \n");
			APP_ASSERT( (p_info->mpOnModel != NULL),
						"tOnNodeInfo::mpOnCollModel is NULL \n");

			if (mGimmickNodeType == eGMK_NODE_TYPE_PROGRAM) {
				int frameIndex = 0;

				// 乗っているフレームのマトリックスを取得
				MATRIX node_mtx;
				this->GetFrameMatrixFromIndex(&node_mtx, frameIndex);

				// マトリックス座標の設定
				VECTOR mtx_pos;
				Utility::ExtractMatrixPosition(&mtx_pos, &node_mtx);
		
				VECTOR move_rot = VSub(mRotation, p_info->mPrevRot);
				MATRIX rot_mtx;
				rot_mtx = MGetIdent();
				MATRIX tmp;
				tmp = MGetIdent();
				tmp = MGetRotX(move_rot.x);
				rot_mtx = MMult(rot_mtx, tmp);

				tmp = MGetIdent();
				tmp = MGetRotY(move_rot.y);
				rot_mtx = MMult(rot_mtx, tmp);

				tmp = MGetIdent();
				tmp = MGetRotZ(move_rot.z);
				rot_mtx = MMult(rot_mtx, tmp);
		
				// マトリックス座標からの差分
				VECTOR add_vec = p_info->mAddVec;

				// マトリックス座標と回転マトリックスから、指定座標を求める
				VECTOR target_vec;
				target_vec = VTransform(add_vec, rot_mtx);
				target_vec = VAdd(target_vec, mtx_pos);
				PRINT_SCREEN(GetColor(255, 0, 0), "gmk_vec = {%.2f, %.2f, %.2f}", target_vec.x, target_vec.y, target_vec.z);
				p_info->mpOnModel->Position() = target_vec;

			}
			else if (mGimmickNodeType == eGMK_NODE_TYPE_DATA) {
				VECTOR frame_pos;
				bool result = GetFramePosFromName(&frame_pos, (char *)mOnNodeName);
				APP_ASSERT(result, "Failed ModelPmd::GetFramePosFromName\n");
				VECTOR diff = VSub(frame_pos, mOnPosition);
				p_info->mpOnModel->Position() = VAdd(p_info->mpOnModel->Position(), diff);

				p_info->mpOnModel->MoveVec() = diff;

			}

			p_info->mpOnModel->ForciblySetPositionByCentering();
			p_info->mpOnModel->SetNowGravity(0.0f);
			p_info->mpOnModel = NULL;		// 使いまわしをしない為破棄
		}

		// ビットシフトして他に乗ってるモデル情報が無いかを判別する
		mOnNodeInfoHandleBit =( mOnNodeInfoHandleBit >> 1);
		node_index ++;
	}

	// コリジョンの更新を個別に行う
	CollisionManager::GetInstance()->RefreshModel(mModelHandle, // modelHandle
												  -1);			// frameIndex
}

//---------------------------------------------------
/**
 * プレイヤーを動かした後のアップデート (２回目)
 */
//---------------------------------------------------
void
GimmickBase::Update(void)
{
	if (mGimmickNodeType == eGMK_NODE_TYPE_DATA) {
		GetFramePosFromName(&mOnPosition, (char *)mOnNodeName);
	}

}

//---------------------------------------------------
/**
 * プレイヤーこのギミックモデルに乗っている場合のマトリックス更新
 */
//---------------------------------------------------
void
GimmickBase::SetCharaPositionToFrameMatrix(ModelCollision* pModelCollision, int frameIndex)
{
	if (sPauseAttach == TRUE) {
		return;
	}

	APP_ASSERT( (pModelCollision != NULL),
				"pModelCollision is NULL");
	
	// 空きの情報を取得する
	CHAR get_index = GimmickBase::GetEmptyOnNodeListIndex();
	APP_ASSERT( (get_index != -1),
				"Not Found EmptyOnNodeList \n");
	tOnNodeInfo* p_info = GimmickBase::GetOnNodeInfo(get_index);
	APP_ASSERT( (p_info != NULL),
				"tOnNodeInfo is NULL \n");
	p_info->mpOnModel = pModelCollision;

	if (mGimmickNodeType == eGMK_NODE_TYPE_PROGRAM) {
		// 乗っているフレームのマトリックスを取得
		MATRIX node_mtx;
		this->GetFrameMatrixFromIndex(&node_mtx, frameIndex);

		// マトリックス座標の設定
		VECTOR mtx_pos;
		Utility::ExtractMatrixPosition(&mtx_pos, &node_mtx);

		// マトリックス座標からの差分
		VECTOR add_vec = VSub(p_info->mpOnModel->Position(), mtx_pos);
		p_info->mAddVec = add_vec;

		// １フレ前の回転情報を保持する
		p_info->mPrevRot = mRotation;
	}
	else if (mGimmickNodeType == eGMK_NODE_TYPE_DATA) {
		SetFrameUserMatrix((char *)mOnNodeName, 
						   &p_info->mpOnModel->Position(),
						   (VECTOR *)&ZERO_VEC,
						   &VGet(0.0f, 1.0f, 0.0f),
						   &mScale);
	}

	// 有効なノード情報をビットにして格納する
	mOnNodeInfoHandleBit |= ENUM_2_BIT(get_index);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickBase::ReviseOnPosition(VECTOR& rMoveVec)
{
	mOnPosition = VSub(mOnPosition, rMoveVec);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
GimmickBase::GetWakeupEventValue(void)
{
	return (int)mWakeupEvent;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickBase::GetGimmickMoveVec(VECTOR& rMoveVec)
{
	if (mGimmickNodeType == eGMK_NODE_TYPE_DATA) {
		VECTOR frame_pos;
		bool result = GetFramePosFromName(&frame_pos, (char *)mOnNodeName);
		APP_ASSERT(result, "Failed ModelPmd::GetFramePosFromName\n");
		rMoveVec = VAdd(VScale(VSub(frame_pos, mOnPosition), 0.75f), rMoveVec);
	}
}

//---------------------------------------------------
/**
   指定モデルのOnNode情報をリセットする。
 */
//---------------------------------------------------
void
GimmickBase::ResetOnNodeInfo(ModelCollision* pOnModel)
{
	APP_ASSERT((pOnModel != NULL),
			   "pOnModel is NULL \n");
	
	for (UCHAR iI = 0; iI < kON_NODE_MODEL_NUM; iI ++) {
		GimmickBase::tOnNodeInfo* node_info = GimmickBase::GetOnNodeInfo(iI);
		if (node_info->mpOnModel == pOnModel) {
			node_info->mpOnModel = NULL;

			DWORD bit = (1 << iI);
			mOnNodeInfoHandleBit ^= bit;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickBase::getOriginalPosition(VECTOR* pPos)
{
	pPos->x = sStageInfoCsv.GetFloat(mCsvNo, GimmickBase::eCSVLINE_ORG_POS_X);
	pPos->y = sStageInfoCsv.GetFloat(mCsvNo, GimmickBase::eCSVLINE_ORG_POS_Y);
	pPos->z = sStageInfoCsv.GetFloat(mCsvNo, GimmickBase::eCSVLINE_ORG_POS_Z);

	// ReviseAllModel の差分の座標を加味する
	*pPos = VSub(*pPos, GimmickBase::sRevisePos);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickBase::getOriginalRotation(VECTOR* pRot)
{
	pRot->x = Utility::DegToRad(sStageInfoCsv.GetFloat(mCsvNo, GimmickBase::eCSVLINE_ORG_ROT_X) );
	pRot->y = Utility::DegToRad(sStageInfoCsv.GetFloat(mCsvNo, GimmickBase::eCSVLINE_ORG_ROT_Y) );
	pRot->z = Utility::DegToRad(sStageInfoCsv.GetFloat(mCsvNo, GimmickBase::eCSVLINE_ORG_ROT_Z) );

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickBase::getOriginalScale(VECTOR* pScale)
{
	pScale->x = sStageInfoCsv.GetFloat(mCsvNo, GimmickBase::eCSVLINE_ORG_SCA_X);
	pScale->y = sStageInfoCsv.GetFloat(mCsvNo, GimmickBase::eCSVLINE_ORG_SCA_Y);
	pScale->z = sStageInfoCsv.GetFloat(mCsvNo, GimmickBase::eCSVLINE_ORG_SCA_Z);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickBase::searchNearesetNode(VECTOR* pPosition, int frameIndex)
{
	static const int array_num = 9;
	BYTE indexArray[array_num] = {7, 6, 11, 14, 19, 42, 43, 48, 24};

	int iI = 0;
	for (iI = 0; iI < array_num; iI ++) {
		if (indexArray[iI] == frameIndex) {
			break;
		}		
	}
	if (iI == array_num) {
		iI = 0;
	}

	sprintf_s(mOnNodeName, sizeof(mOnNodeName), "OnPoint%02d", iI);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
GimmickBase::commonSetup(void)
{
	getOriginalPosition(&mPosition);
	getOriginalRotation(&mRotation);
	getOriginalScale(&mScale);

	// 拡張バッファが存在する場合は、拡張バッファ数の分だけ生成バッファに拡張情報を取得させる
	if (mExtendBufferNum != 0) {
		if (mpExtendBuffer != NULL) {
			delete[] mpExtendBuffer;
			mpExtendBuffer = NULL;
		}

		mpExtendBuffer = new float[mExtendBufferNum];
		APP_ASSERT_PRINT( (mpExtendBuffer != NULL),
						  "Failed new float[%d] \n", mExtendBufferNum);

		for (int iI = 0; iI < mExtendBufferNum; iI ++) {
			mpExtendBuffer[iI] = sStageInfoCsv.GetFloat(mCsvNo, (GimmickBase::eCSVLINE_MAX + iI) );
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
GimmickBase::isOnNodeModel(void)
{
	return (mOnNodeInfoHandleBit == 0x00) ? false : true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
GimmickBase::getSoundVolume(float maxHearingDist)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	float dist_to_player = VSize(VSub(mPosition, p_player->Position() ) );
	int volume = (255 - (int)(  (dist_to_player / maxHearingDist ) * 255.0f ) );
	if (volume < 0)		volume = 0;
	if (volume > 255)	volume = 255;

	return volume;
}

/**** end of file ****/

