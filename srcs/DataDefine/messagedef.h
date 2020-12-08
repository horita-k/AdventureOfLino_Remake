/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * MESSAGEファイルの定義	
 * @author SPATZ.
 * @data   2012/04/04 16:15:43
 */
//---------------------------------------------------
#ifndef __MESSAGE_DEF_H__
#define __MESSAGE_DEF_H__

#include "dxlib.h"
#include "AppLib/Basic/Basic.h"
#include "Scene/EventFunctions.h"
#include "Scene/ActionTaskManager.h"
#include "CharaDef.h"
#include "ItemDef.h"


/*=====================================*
 * ショップ情報
 *=====================================*/
struct tShopInfo {
	const eItemKind*	mpItemList;
	BYTE				mItemNum;
};

enum {
	eSHOP_21_GIMURI = 0,
	eSHOP_13_ACCSE,
	eSHOP_011,					// 29	// 港町・店
	eSHOP_035,					// 30	// ジモト村・店
	eSHOP_0XX,					// 31	// ジパング・店
	eSHOP_LAPIN_024,			// 32	// ラパン地下水路・店
	eSHOP_LAPIN_025,			// 33	// ラパン地下牢・店
	eSHOP_LAPIN_016,			// 34	// ラパン森・店
	eSHOP_LAPIN_036,			// 35	// ラパン鉱山・店
	eSHOP_LAPIN_040,			// 36	// ラパンハラッパ・店
	eSHOP_COSPLAY_021,			// 37	// コスプレ・店
	eSHOP_014,					// 41	// 魔術大学・店

	eSHOP_MAX,
};

/*=====================================*
 * メッセージ後の開始タスク
 *=====================================*/
// ※ソースの方にも追加する事！
enum {
	eMSG_TASK_MOVE_005_000 = 0,			// 0
	eMSG_TASK_MOVE_000_B,				// 1
	eMSG_TASK_EVENT_010_000,			// 2
	eMSG_TASK_MOVE_021_000,				// 3
	eMSG_TASK_SHOP_021,					// 4	// リベルニア・店
	eMSG_TASK_MOVE_024_000,				// 5
	eMSG_TASK_SAVE,						// 6
	eMSG_TASK_BATTLE_QUEST_1,			// 7	// 武道大会・地区予選
	eMSG_TASK_SHOP_ACS_013,				// 8	// 貴族街・店
	eMSG_TASK_EVENT_029_012,			// 9	// ミノタウロス番兵 or 移動イベント
	eMSG_TASK_EVENT_021_BIN_SHOP,		// 10	// おばあちゃんのビン売り
	eMSG_TASK_RIDE_VEHICLE,				// 11
	eMSG_TASK_EVENT_016_014,			// 12	// 剣スイッチイベント
	eMSG_TASK_EVENT_016_017,			// 13	// うさちゅう戻しイベント
	eMSG_TASK_EVENT_036_016,			// 14	// トロッコ操作説明イベント
	eMSG_TASK_EVENT_036_017,			// 15	// 鉱山説明イベント
	eMSG_TASK_BATTLE_QUEST_2,			// 16	// 武道大会・本戦
	eMSG_TASK_BATTLE_QUEST_3,			// 17	// 武道大会・全国大会
	eMSG_TASK_GET_MAGIC_FLAME,			// 18	// 魔法取得・炎
	eMSG_TASK_GET_MAGIC_TORNADE,		// 19	// 魔法取得・竜巻
	eMSG_TASK_GET_MAGIC_THUNDER,		// 20	// 魔法取得・雷
	eMSG_TASK_HINT_STONE,				// 21	// 石碑の魔法ヒント
	eMSG_TASK_GET_SONG_WILDNESS,		// 22	// 唄取得・草原の唄
	eMSG_TASK_GET_SONG_WING,			// 23	// 唄取得・翼の唄
	eMSG_TASK_GET_SONG_HANDLING,		// 24	// 唄取得・操りの唄
	eMSG_TASK_MOVE_UPPER_CASTLE,		// 25	// 城上部へ
	eMSG_TASK_MOVE_LOWER_CASTLE,		// 26	// 城下部へ
	eMSG_TASK_GET_SONG_SUN,				// 27	// 唄取得・太陽の唄
	eMSG_TASK_GET_FAILY,				// 28	// 妖精取得イベント
	eMSG_TASK_SHOP_011,					// 29	// 港町・店
	eMSG_TASK_SHOP_035,					// 30	// ジモト村・店
	eMSG_TASK_SHOP_0XX,					// 31	// ジパング・店
	eMSG_TASK_SHOP_LAPIN_024,			// 32	// ラパン地下水路・店
	eMSG_TASK_SHOP_LAPIN_025,			// 33	// ラパン地下牢・店
	eMSG_TASK_SHOP_LAPIN_016,			// 34	// ラパン森・店
	eMSG_TASK_SHOP_LAPIN_036,			// 35	// ラパン鉱山・店
	eMSG_TASK_SHOP_LAPIN_040,			// 36	// ラパンハラッパ・店
	eMSG_TASK_SHOP_COSPLAY_021,			// 37	// コスプレ・店
	
	eMSG_TASK_MOVE_PRISON,				// 38	// 牢獄イベント
	eMSG_TASK_MOVE_FRESS_ROOM,			// 39	// フレスの部屋
	eMSG_TASK_MOVE_PRISON_OUT,			// 40	// 牢獄イベント・牢屋から出る
	
	eMSG_TASK_SHOP_014,					// 41	// 魔術大学・店
	eMSG_TASK_WISE_MAN,					// 42	// 物知りばあちゃん

	eMSG_TASK_GOTO_OVERSEA,				// 43	// 港→和の国
	eMSG_TASK_BACKTO_OVERSEA,			// 44	// 和の国→港


	eMSG_TASK_MAX,
};

enum eUseToon {
	eUSE_TOON_DISABLE,	// トゥーン未使用
	eUSE_TOON_ENABLE,	// トゥーン使用

	eUSE_TOON_MAX,
};

// NPC情報取得関数
const ActionTaskManager::tTaskInfo* GetNpcTaskTable(int task_id);
const tShopInfo* GetShopInfoList(int shopIndex);


#endif // __MESSAGE_DEF_H__

 /**** end of file ****/
