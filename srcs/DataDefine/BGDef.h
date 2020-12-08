/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * BGファイルの定義	
 * @author SPATZ.
 * @data   2012/04/04 16:15:43
 */
//---------------------------------------------------
#ifndef __BG_DEF_H__
#define __BG_DEF_H__

enum {
	eBG_TYPE_EARTH,		// 0
	eBG_TYPE_WALL,
	eBG_TYPE_ROOF,
	eBG_TYPE_TASK,
	eBG_TYPE_PLACE,
	eBG_TYPE_VISIBLE,
	eBG_TYPE_ITEM,
	eBG_TYPE_WATER,			//	eBG_TYPE_RESERVE_00,	// 7
	eBG_TYPE_WATER_02,		// 9
	eBG_TYPE_BATTLE_WALL,	// eBG_TYPE_RESERVE_01,		// 8	
	//	eBG_TYPE_ALPHA,				// 10
	eBG_TYPE_ALPHA_EARTH,	// 10
	eBG_TYPE_ALPHA_WALL,	// 11
	eBG_TYPE_ALPHA_VISIBLE,	// 12

	eBG_TYPE_GIMMICK,		// 13


	eBG_TYPE_MAX,

	eBG_TYPE_EXT_00 = eBG_TYPE_ITEM,
};

/*=====================================*
 * gimmic
 *=====================================*/
// Stage 29
enum {
	eGIMMICK_29_BARRIER = eBG_TYPE_GIMMICK,
	eGIMMICK_29_TRUMP_JOKER,
	eGIMMICK_29_CARD_STAND,
	eGIMMICK_29_TRUMP_KING,
	eGIMMICK_29_TRUMP_QUEEN,
	eGIMMICK_29_TRUMP_JACK,
	eGIMMICK_29_ARMER_1,
	eGIMMICK_29_ARMER_2,
	eGIMMICK_29_ARMER_3,
	eGIMMICK_29_ARMER_4,
	eGIMMICK_29_BIG_KEY,
};

enum eFootstepKind {
	eFOOTSTEP_KIND_DEFAULT = 0,		// アスファルト
	eFOOTSTEP_KIND_WATER,			// 水
	eFOOTSTEP_KIND_GRASS,			// 草
	eFOOTSTEP_KIND_SAND,			// 砂
	eFOOTSTEP_KIND_SOIL,			// 土
	eFOOTSTEP_KIND_COMICAL,			// コミカル
	eFOOTSTEP_KIND_GUARDIAN,		// マジックアーマー

	eFOOTSTEP_KIND_MAX,
};

#define GET_FOOTSTEP_RELATE_NUM(info)	(sizeof(info) / sizeof(tFootstepRelateInfo))

struct tFootstepRelateInfo {
	BYTE			mMaterialIndex;
	eFootstepKind	mFootstepKind;
};

static const tFootstepRelateInfo sFootstepRelateInfoBG08_Table[] = {
	{1,		eFOOTSTEP_KIND_SOIL},
	{3,		eFOOTSTEP_KIND_SOIL},
	{5,		eFOOTSTEP_KIND_SOIL},
};

static const tFootstepRelateInfo sFootstepRelateInfoBG10_Table[] = {
	{1,		eFOOTSTEP_KIND_SOIL},
	{16,	eFOOTSTEP_KIND_SOIL},
	{26,	eFOOTSTEP_KIND_SOIL},
	{27,	eFOOTSTEP_KIND_SAND},
};
static const tFootstepRelateInfo sFootstepRelateInfoBG11_Table[] = {
	{1,		eFOOTSTEP_KIND_SOIL},
	{16,	eFOOTSTEP_KIND_SOIL},
	{18,	eFOOTSTEP_KIND_SAND},
	{26,	eFOOTSTEP_KIND_SOIL},
	{27,	eFOOTSTEP_KIND_SAND},
};

static const tFootstepRelateInfo sFootstepRelateInfoBG13_Table[] = {
	{1,		eFOOTSTEP_KIND_SOIL},
	{16,	eFOOTSTEP_KIND_SOIL},
	{71,	eFOOTSTEP_KIND_GRASS},
};

static const tFootstepRelateInfo sFootstepRelateInfoBG16_Table[] = {
	{0,		eFOOTSTEP_KIND_SOIL},
	{12,	eFOOTSTEP_KIND_GRASS},
	{59,	eFOOTSTEP_KIND_COMICAL},
};

static const tFootstepRelateInfo sFootstepRelateInfoBG21_Table[] = {
	{13,	eFOOTSTEP_KIND_SOIL},
	{16,	eFOOTSTEP_KIND_SOIL},
	{19,	eFOOTSTEP_KIND_SOIL},
	{65,	eFOOTSTEP_KIND_GRASS},
	{66,	eFOOTSTEP_KIND_SOIL},
};

static const tFootstepRelateInfo sFootstepRelateInfoBG22_Table[] = {
	{23,	eFOOTSTEP_KIND_SOIL},
	{27,	eFOOTSTEP_KIND_COMICAL},
};

static const tFootstepRelateInfo sFootstepRelateInfoBG29_Table[] = {
	{14,	eFOOTSTEP_KIND_GRASS},
	{106,	eFOOTSTEP_KIND_SOIL},
	{122,	eFOOTSTEP_KIND_GRASS},
};

static const tFootstepRelateInfo sFootstepRelateInfoBG34_Table[] = {
	{0,		eFOOTSTEP_KIND_GRASS},
};

static const tFootstepRelateInfo sFootstepRelateInfoBG35_Table[] = {
	{0,		eFOOTSTEP_KIND_SOIL},
	{1,		eFOOTSTEP_KIND_SOIL},
	{4,		eFOOTSTEP_KIND_SOIL},
	{11,	eFOOTSTEP_KIND_SOIL},
};

static const tFootstepRelateInfo sFootstepRelateInfoBG40_Table[] = {
	{0,		eFOOTSTEP_KIND_SOIL},
};

static const tFootstepRelateInfo sFootstepRelateInfoBG48_Table[] = {
	{0,		eFOOTSTEP_KIND_SOIL},
};

/*=====================================*
  
 *=====================================*/
struct tFootstepRelateTableInfo {
	BYTE						mPlaceIndex;
	const tFootstepRelateInfo*	mpRelateInfo;
	BYTE						mRelateNum;
};

/*=====================================*

 *=====================================*/
static const tFootstepRelateTableInfo sFootstepRelateManageTable[] = {
	{ 8,	sFootstepRelateInfoBG08_Table, GET_FOOTSTEP_RELATE_NUM(sFootstepRelateInfoBG08_Table)},
	{10,	sFootstepRelateInfoBG10_Table, GET_FOOTSTEP_RELATE_NUM(sFootstepRelateInfoBG10_Table)},
	{11,	sFootstepRelateInfoBG11_Table, GET_FOOTSTEP_RELATE_NUM(sFootstepRelateInfoBG11_Table)},
	{13,	sFootstepRelateInfoBG13_Table, GET_FOOTSTEP_RELATE_NUM(sFootstepRelateInfoBG13_Table)},
	{16,	sFootstepRelateInfoBG16_Table, GET_FOOTSTEP_RELATE_NUM(sFootstepRelateInfoBG16_Table)},
	{21,	sFootstepRelateInfoBG21_Table, GET_FOOTSTEP_RELATE_NUM(sFootstepRelateInfoBG21_Table)},
	{22,	sFootstepRelateInfoBG22_Table, GET_FOOTSTEP_RELATE_NUM(sFootstepRelateInfoBG22_Table)},
	{29,	sFootstepRelateInfoBG29_Table, GET_FOOTSTEP_RELATE_NUM(sFootstepRelateInfoBG29_Table)},
	{34,	sFootstepRelateInfoBG34_Table, GET_FOOTSTEP_RELATE_NUM(sFootstepRelateInfoBG34_Table)},
	{35,	sFootstepRelateInfoBG35_Table, GET_FOOTSTEP_RELATE_NUM(sFootstepRelateInfoBG35_Table)},
	{40,	sFootstepRelateInfoBG40_Table, GET_FOOTSTEP_RELATE_NUM(sFootstepRelateInfoBG40_Table)},
	{48,	sFootstepRelateInfoBG48_Table, GET_FOOTSTEP_RELATE_NUM(sFootstepRelateInfoBG48_Table)},
	{0xFF,	NULL, 0},	// ダミーテーブル
};
static const BYTE	sFootstepRelateMangerTableNum = (sizeof(sFootstepRelateManageTable) / sizeof(tFootstepRelateTableInfo) - 1); // ダミーの分引く

#endif // __BG_DEF_H__

 /**** end of file ****/
