/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * オブジェクトベースクラス
 * @author SPATZ.
 * @data   2012/04/14 19:35:52
 */
//---------------------------------------------------
#include "ObjectBase.h"

#include "Object/ObjectTresure.h"
#include "Object/ObjectSwitch.h"
#include "Object/ObjectItem.h"
#include "Object/ObjectHand.h"
#include "Object/ObjectSeesaw.h"

#include "AppLib/Basic/Basic.h"
#include "../Charactor/CharaPlayer.h"
#include "DataDefine/MotionDef.h"
#include "DataDefine/CharaDef.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Collision/CollisionManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "Scene/ActionTaskManager.h"
#include "Utility/Utility.h"

//#include "CharaInfo.h"

/*=====================================*
 * static
 *=====================================*/
static GET_ITEM_HANDLER sGetItemHandler = NULL;

static const float kOBJECT_TREASURE_HIDE_OFFSET_Y = 99999.9f;

static std::vector<ObjectBase *> sActiveObjectList;

/*=====================================*
 * リソース情報
 *=====================================*/
static const ObjectBase::tObjectBaseParam sObjectBaseParam[eOBJECT_KIND_MAX] = {
	{RES_MDL_TREASURE,			65.0f,		20.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_TRESURE
	{RES_MDL_MOGUREVER,			90.0f,		20.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_MOGU_SWITCH,
	{RES_MDL_FACESWITCH,		120.0f,		160.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_FACE_SWITCH,
	{RES_MDL_HEART,				20.0f,		20.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_HEART,
	{RES_MDL_MONEY_1,			20.0f,		20.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_MONEY_1,
	{RES_MDL_TREASURE,			30.0f,		8.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_SMALL_TRESURE
	{RES_MDL_HEART_CASE,		100.0f,		24.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_HEART_CASE
	{RES_MDL_JAR,				50.0f,		10.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_JAR_TYPE_A
	{RES_MDL_BOMB,				50.0f,		10.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_BOMB
	{RES_MDL_LOCK,				1000.0f,	140.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_LOCK
	{RES_MDL_STONE,				100.0f,		16.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_STONE
	{RES_MDL_SUNOHARA,			50.0f,		10.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_SUNOHARA
	{RES_MDL_SEESAW,			100.0f,		30.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_SEESAW
	{RES_MDL_USACHU,			50.0f,		10.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_USACHU
	{RES_MDL_CRYSTAL,			50.0f,		24.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_CRYSTAL
	{RES_MDL_MONEY_5,			20.0f,		20.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_MONEY_5,
	{RES_MDL_MONEY_10,			20.0f,		20.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_MONEY_10,
	{RES_MDL_MONEY_20,			20.0f,		20.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_MONEY_20,
	{RES_MDL_MONEY_50,			20.0f,		20.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_MONEY_50,
	{RES_MDL_ARROW,				100.0f,		10.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_ARROW
	{RES_MDL_MONEY_100,			20.0f,		20.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_MONEY_100,
	{RES_MDL_MONEY_1,			20.0f,		20.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_MONEY_200,
	{RES_MDL_LOCK_CENTER,		100.0f,		50.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_METEO
	{RES_MDL_LOCK_CENTER,		100.0f,		50.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_METEO_REFRECT
	{RES_MDL_MONEY_1,			20.0f,		20.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_MONEY_400,
	{RES_MDL_DUMMY_OBJ,			0.0f,		0.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_DUMMY_OBJ
	{"",						0.0f,		0.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_DUMMY_26
	{"",						0.0f,		0.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_DUMMY_27
	{RES_MDL_FACESWITCH,		450.0f,		800.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_FACE_SWITCH_BIG,
	{RES_MDL_CRYSTAL_SWITCH,	90.0f,		5.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_CRYSTAL_SWITCH
	{RES_MDL_LOCK_CENTER,		20.0f,		10.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_MISSILE
	{NAME_Usaida,				300.0f,		75.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_GETDOWN
	{RES_MDL_HEART,				20.0f,		0.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_CHECK_POINT
	{RES_MDL_PLATE_COMMON,		20.0f,		20.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_PLATE_ARROW
	{RES_MDL_PLATE_COMMON,		20.0f,		20.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_PLATE_BOMB
	{RES_MDL_CARD_JK,			20.0f,		20.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_CARD_JK
	{RES_MDL_CARD_COMMON,		8.0f,		20.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_CARD_K
	{RES_MDL_CARD_COMMON,		8.0f,		20.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_CARD_Q
	{RES_MDL_CARD_COMMON,		12.0f,		20.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_CARD_J
	{RES_MDL_LOCK,				142.0f,		20.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_SMALL_LOCK
	{RES_MDL_JAR,				50.0f,		10.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_JAR_TYPE_B
	{RES_MDL_JAR,				50.0f,		10.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_JAR_TYPE_C
	{RES_MDL_HEART_PIECE,		100.0f,		24.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_HEART_PIECE
	{RES_MDL_BOOK_CLOSE,		7.0f,		10.0f,	ObjectBase::eOBJECT_CHECK_ENABLE},	// eOBJECT_KIND_BOOK
	{RES_MDL_CRYSTAL_SWITCH,	360.0f,		28.0f,	ObjectBase::eOBJECT_CHECK_DISABLE},	// eOBJECT_KIND_CRYSTAL_SWITCH_BIG
};

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectBase::ObjectBase()
{
	mObjectIndex		= 0;
	mItemKind			= eITEM_KIND_NONE;
	mIsUsed				= false;

	mHideEventIndex = -1;
	mIsHide = false;
	mpEntryTargetModel = NULL;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ObjectBase::~ObjectBase()
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
ObjectBase::LoadAllObjectBase(void)
{
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
ObjectBase::SetGetItemHandler(GET_ITEM_HANDLER handler)
{
	sGetItemHandler = handler;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/GET_ITEM_HANDLER
ObjectBase::GetGetItemHandler(void)
{
	return sGetItemHandler;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/ObjectBase*
ObjectBase::CreateInstance(USHORT objectKind, USHORT stageIndex)
{
	ObjectBase* p_create_instance = NULL;

	// 各オブジェクトごとのインスタンスを生成 
	switch (objectKind) {
	case eOBJECT_KIND_TRESURE:
	case eOBJECT_KIND_SMALL_TRESURE:
	case eOBJECT_KIND_CARD_JK:
	case eOBJECT_KIND_CARD_K:
	case eOBJECT_KIND_CARD_Q:
	case eOBJECT_KIND_CARD_J:
	case eOBJECT_KIND_BOOK:
		p_create_instance = new ObjectTresure();
		break;

	case eOBJECT_KIND_CRYSTAL:
		p_create_instance = new ObjectTresureCrystal();
		break;

	case eOBJECT_KIND_MOGU_SWITCH:
	case eOBJECT_KIND_FACE_SWITCH:
	case eOBJECT_KIND_FACE_SWITCH_BIG:
	case eOBJECT_KIND_CRYSTAL_SWITCH:
	case eOBJECT_KIND_CRYSTAL_SWITCH_BIG:
	case eOBJECT_KIND_GETDOWN:
		p_create_instance = new ObjectSwitch(ConvertPlaceIndex_TO_EventDefine(stageIndex), (BYTE)objectKind);
		break;
	case eOBJECT_KIND_HEART:
	case eOBJECT_KIND_MONEY_1:
	case eOBJECT_KIND_MONEY_5:
	case eOBJECT_KIND_MONEY_10:
	case eOBJECT_KIND_MONEY_20:
	case eOBJECT_KIND_MONEY_50:
	case eOBJECT_KIND_MONEY_100:
	case eOBJECT_KIND_MONEY_200:
	case eOBJECT_KIND_MONEY_400:
	case eOBJECT_KIND_HEART_CASE:
	case eOBJECT_KIND_HEART_PIECE:
	case eOBJECT_KIND_PLATE_ARROW:
	case eOBJECT_KIND_PLATE_BOMB:
		p_create_instance = new ObjectItem(false);
		break;

	case eOBJECT_KIND_CHECK_POINT:
		p_create_instance = new ObjectItem_CheckPoint();
		break;

	case eOBJECT_KIND_JAR_TYPE_A:
	case eOBJECT_KIND_JAR_TYPE_B:
	case eOBJECT_KIND_JAR_TYPE_C:
		p_create_instance = new ObjectHand_Jar();
		break;
	case eOBJECT_KIND_BOMB:
		p_create_instance = new ObjectHand_Bomb();
		break;
	case eOBJECT_KIND_LOCK:
	case eOBJECT_KIND_SMALL_LOCK:
		p_create_instance = new ObjectHand_Lock();
		break;
	case eOBJECT_KIND_STONE:
		p_create_instance = new ObjectHand_Stone();
		break;

	case eOBJECT_KIND_SUNOHARA:
		p_create_instance = new ObjectHand_Sunohara();
		break;

	case eOBJECT_KIND_USACHU:
		p_create_instance = new ObjectHand_Usachu();
		break;

	case eOBJECT_KIND_SEESAW:
		p_create_instance = new ObjectSeesaw();
		break;
	default:
		APP_ASSERT_PRINT( (NULL),
						  "Not Support object kind : %d \n", objectKind);
		break;
	};

	return p_create_instance;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/ObjectBase::tObjectBaseParam* 
ObjectBase::GetObjectBaseParam(int eObjectKind)
{
	return (tObjectBaseParam *)&sObjectBaseParam[eObjectKind];
}

/*=====================================*
 * ActiveObjectList 関連
 *=====================================*/
/*static*/std::vector<ObjectBase *>&	
ObjectBase::GetActiveObjectListRef(void)
{
	return sActiveObjectList;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
ObjectBase::AddActiveObjectList(ObjectBase* pObject)
{
	std::vector<ObjectBase *>::iterator ite;
	for (ite = sActiveObjectList.begin(); ite != sActiveObjectList.end(); ite ++) {
		if (*ite == pObject) {
			// 重複チェック
			return;
		}
	}

	sActiveObjectList.push_back(pObject);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
ObjectBase::RemoveActiveObjectList(ObjectBase* pObject)
{
	std::vector<ObjectBase *>::iterator ite;
	for (ite = sActiveObjectList.begin(); ite != sActiveObjectList.end(); ite ++) {
		if (*ite == pObject) {
			sActiveObjectList.erase(ite);
			break;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectBase::Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg)
{
	APP_ASSERT_PRINT((objectIdx < eOBJECT_KIND_MAX),
					 "setted objectIdx %d is Invaid value \n", objectIdx);
	APP_ASSERT_PRINT((itemIdx < eITEM_KIND_MAX),
					 "setted itemIdx %d is Invaid value \n", itemIdx);

	mObjectIndex	= objectIdx;
	mItemKind		= itemIdx;
	mIsUsed			= false;
	mObjectType		= eOBJECT_TYPE_ERROR;
	const tObjectBaseParam* p_param = &sObjectBaseParam[mObjectIndex];

	this->LinkModel(p_param->mResourceName);
	this->Position()	= *pSetPos;
	this->Rotation().y	= Utility::DegToRad((float)rotDeg);
	this->Scale()		= VGet(p_param->mScale,
							   p_param->mScale,
							   p_param->mScale);
	this->mBodySize		= p_param->mObjectSize;
	this->mBodyHeight	= p_param->mObjectSize;

	ModelPmd::Setup();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectBase::Update(void)
{
	ModelPmd::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
ObjectBase::CheckDamageCollNormal(tCheckDamageWork* pParam)
{
	return ModelCollision::CheckDamageCollNormal(pParam);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectBase::ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType)
{
	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectBase::ForciblySetPositionByCentering(void)
{
	ModelBase::ForciblySetPositionByCentering();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectBase::GetBodyCapsule(ShapeCapsule* pCapsule)
{
	pCapsule->mPosA	= mPosition;
	pCapsule->mPosA.y += mBodySize * 0.5f;
	pCapsule->mPosB	= pCapsule->mPosA;
	pCapsule->mPosB.y += 0.01f;
	pCapsule->mRadius= mBodySize;

	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectBase::SetNowGravity(float gravity)
{
	APP_ASSERT( (NULL), "Not Support this function \n");
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectBase::HideTreasure(int hideEventIndex)
{
	if (mIsHide == true) {
		return;
	}

	mHideEventIndex = hideEventIndex;

	this->mPosition.y -= kOBJECT_TREASURE_HIDE_OFFSET_Y;

	mIsHide = true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectBase::DeHideTreasure(void)
{
	/*
	if (mHideEventIndex == -1) {
		return;
	}
	*/
	if (mIsHide == false) {
		return;
	}

//	mHideEventIndex = -1;

	this->mPosition.y += kOBJECT_TREASURE_HIDE_OFFSET_Y;

	mIsHide = false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
ObjectBase::InsertDrawModelSelf(void)
{
	GraphicManager::GetInstance()->InsertDrawModel(this, mpEntryTargetModel);
}


/**** end of file ****/

