/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ライブラリの色々融合したテスト
 * @author SPATZ.
 * @data   2011/11/21 00:58:44
 */
//---------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS

#include <math.h>
#include <vector>

#include <d3dx9math.h>

#include "SceneManager.h"
#include "SceneMainGame.h"

#include "AppLib/Basic/Basic.h"
#include "AppLib/Basic/Counter.h"
#include "AppLib/Basic/Path.h"
#include "AppLib/Input/InputManager.h"

#include "Scene/ActionTaskManager.h"
#include "Scene/TotalInfo.h"
#include "Scene/EventManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/ShadowMap.h"
#include "AppLib/Graphic/RenderBloom.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/ModelPmd.h"
#include "AppLib/Graphic/ModelMap.h"
#include "AppLib/Effect/EffectManager.h"
#include "AppLib/Graphic/EffekseerManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Collision/CollisionManager.h"
#include "DataDefine/VoiceDef.h"
#include "DataDefine/MessageDef.h"
#include "DataDefine/MotionDef.h"
#include "DataDefine/BGDef.h"
#include "DataDefine/EffectDef.h"
#include "DataDefine/CharaDef.h"
#include "Charactor/CharaInfo.h"
#include "Object/ObjectBase.h"
#include "Object/ObjectTresure.h"
#include "Object/ObjectSwitch.h"
#include "Object/ObjectItem.h"
#include "Object/ObjectHand.h"
#include "Object/ObjectSeesaw.h"
#include "Gimmick/GimmickBase.h"
#include "Gimmick/GimmickExtend.h"
#include "Charactor/CharaPlayer.h"
#include "Charactor/CharaEnemy.h"
#include "Charactor/CharaNPC.h"
#include "Charactor/CharaNavi.h"
#include "AppLib/Resource/ResourceManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "AppLib/Resource/ResourceLayout.h"
#include "dxlib.h"
#include "Utility/Utility.h"
#include "AppLib/Graphic/LayoutNumber.h"

#include "EventFunctions.h"
#include "CommandFunctions.h"

/* SubScene */
#include "SubSceneItemMenu.h"
#include "SubSceneFlyingMove.h"
#include "SubSceneGetItem.h"
#include "SubScenePlayOcarina.h"

// eBG_TYPE_ITEM のアイテム型の文字列長
#define	BG_TYPE_ITEM_MATERIAL_NAME_LEN		(16)
#define BG_TYPE_ITEM_KEYWORD				("Tresu")
#define LABEL_TYPE_NAME_LEN					(5)
#define LABEL_OBJECT_NAME_LEN				(3)
#define LABEL_ITEM_NAME_LEN					(3)
#define LABEL_ROT_NAME_LEN					(3)


static const USHORT	 kCHECK_USING_MEMORY_ATTENTION_SIZE	= 1300;					// 使用メモリ警告サイズ

static VECTOR	sReviseShadowPos	= ZERO_VEC;
static USHORT	sCheckObjectIndex	= 0;
static USHORT	sCheckNpcIndex		= 0;
static BYTE		sItemMenuDefault	= (BYTE)SubSceneItemMenu::eMENU_KIND_ITEM;

/*=====================================*
 * フェーズ関数
 *=====================================*/
/* static */SceneMainGame::PHASE_FUNC
SceneMainGame::mBasePhaseFunc[SceneMainGame::ePHASE_MAX] = {
	&SceneMainGame::phaseLoadBefore,
	&SceneMainGame::phaseLoading,
	&SceneMainGame::phaseSetup,
	&SceneMainGame::phaseGameBefore,
	&SceneMainGame::phaseGame,
	&SceneMainGame::phaseShowMap,
	&SceneMainGame::phaseChangeStage,
	&SceneMainGame::phaseFallDown,
	&SceneMainGame::phaseRecovery,
	&SceneMainGame::phaseMessageBefore,
	&SceneMainGame::phaseMessage,
	&SceneMainGame::phaseEventBefore,
	&SceneMainGame::phaseEvent,
	&SceneMainGame::phaseGetItemBefore,
	&SceneMainGame::phaseGetItem,
	&SceneMainGame::phaseGameOverBefore,
	&SceneMainGame::phaseGameOver,
	&SceneMainGame::phaseStageclear,
	&SceneMainGame::phaseShop,
	&SceneMainGame::phaseSelectWeapon,
	&SceneMainGame::phaseSelectWeaponEnd,
	&SceneMainGame::phaseStartBattle,
	&SceneMainGame::phaseNaviTalk,
	&SceneMainGame::phaseNaviTalkEnd,
	&SceneMainGame::phaseSave,
	&SceneMainGame::phaseMovePlace,
	&SceneMainGame::phaseFlyingMove,
	&SceneMainGame::phasePlayOcarina,
	&SceneMainGame::phaseTestConnect,
};

static tBGInfo sBGInfoTable[] = {
	{TRUE,  eRES_STG_SKYLAND,                      15.0f,   BGM_SKYLAND,           eENE_TBL_IDX_NONE,       eACTION_OK,}, // 0 空中庭園
	{TRUE,  eRES_STG_AIR_RUINS,                    15.0f,   BGM_DUNGEON_B,         eENE_TBL_IDX_NONE,       eACTION_OK,}, // 1 空フォース
	{TRUE,  eRES_STG_ROVININ_VILLAGE,              1.5f,    BGM_SKYLAND,           eENE_TBL_IDX_NONE,       eACTION_OK,}, // 2 
	{FALSE, eRES_STG_NONE,                         14.0f,   BGM_ZELDA,             eENE_TBL_IDX_001,        eACTION_OK,}, // 3 (未使用)
	{FALSE, eRES_STG_NONE,                         14.0f,   BGM_ZELDA,             eENE_TBL_IDX_001,        eACTION_OK,}, // 4 (未使用)
	{FALSE, eRES_STG_NONE,                         14.0f,   BGM_ZELDA,             eENE_TBL_IDX_003,        eACTION_OK,}, // 5 (未使用)
	{FALSE, eRES_STG_FORTRESS_CITY,                10.0f,   BGM_TOWN,              eENE_TBL_IDX_001,        eACTION_OK,}, // 6 
	{FALSE, eRES_STG_FORTRESS_FIELD,               10.0f,   BGM_ZELDA,             eENE_TBL_IDX_002,        eACTION_OK,}, // 7 
	{FALSE, eRES_STG_JAPAN_1,                      1.65f,   BGM_JAPAN,             eENE_TBL_IDX_NONE,       eACTION_NG,}, // 8 鹿の国1
	{TRUE,  eRES_STG_JAPAN_2,                      1.65f,   BGM_JAPAN,             eENE_TBL_IDX_NONE,       eACTION_NG,}, // 9 鹿の国2
	{FALSE, eRES_STG_REVELNEAR2ND_EARTH,           10.0f,   BGM_FIELD,             eENE_TBL_IDX_010,        eACTION_OK,}, // 10 リベ・大地
	{FALSE, eRES_STG_REVELNEAR2ND_HORBOR,          10.0f,   BGM_HARBOR,            eENE_TBL_IDX_NONE,       eACTION_NG,}, // 11 リベ・港
	{FALSE, eRES_STG_FRESS_ROOM,                   10.0f,   BGM_CASTLE_2,          eENE_TBL_IDX_NONE,       eACTION_NG,}, // 12 フレスの部屋
	{FALSE, eRES_STG_REVELNEAR2ND_RICHCITY,        10.0f,   BGM_LIBELUNIA,         eENE_TBL_IDX_NONE,       eACTION_NG,}, // 13 リベ・貴族街
	{FALSE, eRES_STG_VUAL_LIBRARY,                 1.0f,    BGM_COLLEGE,           eENE_TBL_IDX_NONE,       eACTION_NG,}, // 14 ダミー
	{TRUE,  eRES_STG_MARISA_ROOM,                  1.5f,    BGM_LIBELUNIA,         eENE_TBL_IDX_NONE,       eACTION_NG,}, // 15 ダミー
	{FALSE, eRES_STG_FOREST,                       24.0f,   BGM_DUNGEON_E,         eENE_TBL_IDX_016,        eACTION_OK,}, // 16 忘却の森
	{TRUE,  eRES_STG_OLD_PALACE,                   30.0f,   BGM_FORCE_PLACE,       eENE_TBL_IDX_TEST,       eACTION_OK,}, // 17 古い宮殿
	{TRUE,  eRES_STG_DEPTH_SEA,                    10.0f,   BGM_FORCE_PLACE,       eENE_TBL_IDX_TEST,       eACTION_OK,}, // 18 ダミー
	{FALSE, eRES_STG_FOREST_CAVE,                  10.0f,   BGM_DUNGEON_E,         eENE_TBL_IDX_NONE,       eACTION_OK,}, // 19 森の宝場所
	{TRUE,  eRES_STG_GRAVEYARD,                    2.0f,    BGM_DUMMY,             eENE_TBL_IDX_NONE,       eACTION_OK,}, // 20 墓地
	{FALSE, eRES_STG_REVELNEAR2ND_MERGE,           10.0f,   BGM_LIBELUNIA,         eENE_TBL_IDX_NONE,       eACTION_NG,}, // 21 リベルニア街
	{FALSE, eRES_STG_KITCHEN,                      1.2f,    BGM_LINO_THEMA,        eENE_TBL_IDX_NONE,       eACTION_NG,}, // 22 リノの家
	{TRUE,  eRES_STG_LIBRARY,                      10.0f,   BGM_LIBELUNIA,         eENE_TBL_IDX_NONE,       eACTION_NG,}, // 23 図書館
	{FALSE, eRES_STG_WATERWAY,                     14.0f,   BGM_DUNGEON_A,         eENE_TBL_IDX_024,        eACTION_OK,}, // 24 地下水路
	{FALSE, eRES_STG_PRISON,                       2.0f,    BGM_ELDA_THEMA,        eENE_TBL_IDX_NONE,       eACTION_NG,}, // 25 地下牢
	{FALSE, eRES_STG_UNDERGROUND_LAKE,             50.0f,   BGM_FORCE_PLACE,       eENE_TBL_IDX_026,        eACTION_OK,}, // 26 地底湖
	{FALSE, eRES_STG_TREASURE_ROOM,                10.0f,   BGM_DUNGEON_A,         eENE_TBL_IDX_NONE,       eACTION_OK,}, // 27 宝部屋
	{FALSE, eRES_STG_CRYSTAL_CAVE,                 50.0f,   BGM_FORCE_PLACE,       eENE_TBL_IDX_028,        eACTION_OK,}, // 28 クリスタル部屋
	{FALSE, eRES_STG_CASTLE_KINGDOM,               10.0f,   BGM_CASTLE,            eENE_TBL_IDX_029,        eACTION_OK,}, // 29 リベルニア王城
	{TRUE,  eRES_STG_SAILLING_SHIP,                16.0f,   BGM_SHIP,              eENE_TBL_IDX_NONE,       eACTION_OK,}, // 30 帆船
	{TRUE,  eRES_STG_WATER_CAVE,                   20.0f,   BGM_DUNGEON_B,         eENE_TBL_IDX_NONE,       eACTION_OK,}, // 31 海フォース
	{TRUE,  eRES_STG_BIG_SHIP,                     3.0f,    BGM_DUNGEON_A,         eENE_TBL_IDX_NONE,       eACTION_OK,}, // 32 ダミー
	{FALSE, eRES_STG_LINO_HOUSE,                   2.0f,    BGM_LINO_THEMA,        eENE_TBL_IDX_NONE,       eACTION_OK,}, // 33 リノの家
	{FALSE,	eRES_STG_LOST_FOREST,                  2.0f,	BGM_DUMMY,             eENE_TBL_IDX_NONE,       eACTION_OK,}, // 34 迷いの森
	{FALSE, eRES_STG_VILLAGE,                      1.3f,    BGM_VILLAGE,           eENE_TBL_IDX_NONE,       eACTION_NG,}, // 35 ジモト村
	{FALSE, eRES_STG_SULT_MANE,                    4.5f,    BGM_DUNGEON_F,         eENE_TBL_IDX_036,        eACTION_OK,}, // 36 スルト鉱山
	{FALSE, eRES_STG_SULT_MANE_TO_HARAPPA,         4.5f,    BGM_DUMMY,             eENE_TBL_IDX_036,        eACTION_OK,}, // 37 スルト鉱山・(廃止)
	{FALSE, eRES_STG_CRYSTAL_ROOM,                 7.0f,    BGM_CRYSTAL_PLACE,     eENE_TBL_IDX_NONE,       eACTION_NG,}, // 38 クリスタルルーム
	{FALSE, eRES_STG_SULT_MANE_TROLLEY,            4.5f,    BGM_DUNGEON_F_2,       eENE_TBL_IDX_039,        eACTION_OK,}, // 39 スルト鉱山・トロッコ
	{FALSE, eRES_STG_HARAPPA_SUB,                  7.5f,    BGM_DUNGEON_C,         eENE_TBL_IDX_040,        eACTION_OK,}, // 40 ハラッパ
	{FALSE, eRES_STG_PARANOIA,                     4.5f,    BGM_DUNGEON_G,         eENE_TBL_IDX_041,        eACTION_OK,}, // 41 バベル・
	{FALSE, eRES_STG_PARANOIA_CORE,                2.9f,    BGM_DUNGEON_G,         eENE_TBL_IDX_042,        eACTION_OK,}, // 42 バベル・コア
	{FALSE, eRES_STG_ELEVATOR,                     10.0f,   BGM_FORCE_PLACE,       eENE_TBL_IDX_NONE,       eACTION_OK,}, // 43 バベル・ボス戦
	{FALSE, eRES_STG_PARANOIA_SPIRAL,              16.0f,   BGM_DUNGEON_G,         eENE_TBL_IDX_NONE,       eACTION_OK,}, // 44 バベル・階段
	{FALSE, eRES_STG_AIRSHIP,                      1.5f,    BGM_SKYSHIP,           eENE_TBL_IDX_NONE,       eACTION_OK,}, // 45 飛空船ダリス(重複?)
	{FALSE, eRES_STG_BABEL_PRISON,                 18.0f,   BGM_DUNGEON_D,         eENE_TBL_IDX_046,		eACTION_OK,}, // 46 バベル・牢屋
	{FALSE, eRES_STG_AIRSHIP,                      1.5f,    BGM_SKYSHIP,           eENE_TBL_IDX_NONE,       eACTION_OK,}, // 47 飛空船ダリス
	{FALSE, eRES_STG_HARAPPA_MAIN,                 7.5f,    BGM_DUNGEON_D,         eENE_TBL_IDX_040,        eACTION_OK,}, // 48 ハラッパ・メイン
	{FALSE, eRES_STG_HARAPPA_CAVE,                 1.5f,    BGM_DUNGEON_C,         eENE_TBL_IDX_NONE,       eACTION_OK,}, // 49 ダミー
	{FALSE, eRES_STG_NONE,                         0.0f,    BGM_DUMMY,             eENE_TBL_IDX_NONE,       eACTION_NG,}, // 50 ダミー
	{FALSE, eRES_STG_NONE,                         0.0f,    BGM_DUMMY,             eENE_TBL_IDX_NONE,       eACTION_NG,}, // 51 ダミー
	{FALSE, eRES_STG_PARANOIA_2F,                  10.0f,   BGM_DUNGEON_D,         eENE_TBL_IDX_041,		eACTION_OK,}, // 52 バベルの塔2F
	{FALSE, eRES_STG_NONE,                         0.0f,    BGM_DUMMY,             eENE_TBL_IDX_NONE,       eACTION_NG,}, // 53 ダミー
	{FALSE, eRES_STG_NONE,                         0.0f,    BGM_DUMMY,             eENE_TBL_IDX_NONE,       eACTION_NG,}, // 54 ダミー
	{FALSE, eRES_STG_NONE,                         0.0f,    BGM_DUMMY,             eENE_TBL_IDX_NONE,       eACTION_NG,}, // 55 ダミー
	{FALSE, eRES_STG_NONE,                         0.0f,    BGM_DUMMY,             eENE_TBL_IDX_NONE,       eACTION_NG,}, // 56 ダミー
	{FALSE, eRES_STG_NONE,                         0.0f,    BGM_DUMMY,             eENE_TBL_IDX_NONE,       eACTION_NG,}, // 57 ダミー
	{FALSE, eRES_STG_NONE,                         0.0f,    BGM_DUMMY,             eENE_TBL_IDX_NONE,       eACTION_NG,}, // 58 ダミー
	{FALSE, eRES_STG_NONE,                         0.0f,    BGM_DUMMY,             eENE_TBL_IDX_NONE,       eACTION_NG,}, // 59 ダミー
	{TRUE,  eRES_STG_KARUN,                        20.0f,   BGM_DUNGEON_Z,         eENE_TBL_KARUN_A,        eACTION_OK,}, // 60 
	{TRUE,  eRES_STG_KARUN_THRONE,                 20.0f,   BGM_DUNGEON_Z,         eENE_TBL_KARUN_BOSS_A,   eACTION_OK,}, // 61 
	{TRUE,  eRES_STG_KARUN_BARADGULARAN,           200.0f,  BGM_DUNGEON_Z,         eENE_TBL_KARUN_BOSS_A,   eACTION_OK,}, // 62 
	{TRUE,  eRES_STG_KARUN_CASTLE,                 20.0f,   BGM_DUNGEON_Z,         eENE_TBL_IDX_NONE,       eACTION_OK,}, // 63 
	{TRUE,  eRES_STG_KARUN_GATE,                   20.0f,   BGM_DUNGEON_Z,         eENE_TBL_KARUN_A,        eACTION_OK,}, // 64 
	{TRUE,  eRES_STG_KARUN_TOWN,                   20.0f,   BGM_DUNGEON_Z,         eENE_TBL_IDX_NONE,       eACTION_OK,}, // 65 
	{TRUE,  eRES_STG_KARUN_FORGE,                  20.0f,   BGM_DUNGEON_Z,         eENE_TBL_KARUN_A,        eACTION_OK,}, // 66 
	{FALSE, eRES_STG_NONE,                         0.0f,    BGM_DUMMY,             eENE_TBL_IDX_NONE,       eACTION_NG,}, // 67 ダミー
	{FALSE, eRES_STG_NONE,                         0.0f,    BGM_DUMMY,             eENE_TBL_IDX_NONE,       eACTION_NG,}, // 68 ダミー
	{FALSE, eRES_STG_NONE,                         0.0f,    BGM_DUMMY,             eENE_TBL_IDX_NONE,       eACTION_NG,}, // 69 ダミー
#ifdef _DEBUG
	/**** ここより上位は移動禁止 ****/
	// 70
	{TRUE , eRES_STG_PITAGORA,						15.0f, 	BGM_BOSS_W, 	 			eENE_TBL_IDX_NONE, 		eACTION_OK, },//爆撃城
	{TRUE , eRES_STG_BAKUGEKI_CASTLE,				15.0f, 	BGM_DUNGEON_B, 	 			eENE_TBL_IDX_NONE, 		eACTION_OK, },//爆撃城
	{TRUE , eRES_STG_NEO_CITY,						15.0f, 	BGM_DUNGEON_B, 	 			eENE_TBL_IDX_NONE, 		eACTION_OK, },//Neo-City
	{TRUE , eRES_STG_SHINGEKI_NO_KABE,				50.0f, 	BGM_DUNGEON_B, 	 			eENE_TBL_IDX_NONE, 		eACTION_OK, },//爆撃城
	{TRUE , eRES_STG_BLUE_DEVIL,					200.0f, BGM_DUNGEON_B, 	 			eENE_TBL_IDX_NONE, 		eACTION_OK, },//青鬼
	{TRUE , eRES_STG_VALLEY,						5000.0f,BGM_DUNGEON_B, 	 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_ASH_STAGE,						50.0f,	BGM_DUNGEON_B, 	 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_DEVIL_CASTLE,					15.0f, 	BGM_DUNGEON_B, 	 			eENE_TBL_IDX_NONE, 		eACTION_OK, },//悪魔っぽい城
	{FALSE, eRES_STG_KINGDOM_GARDEN,				10.0f, 	BGM_DUNGEON_B,			 	eENE_TBL_IDX_NONE, 		eACTION_OK, },//旧リベルニア王城
	{FALSE, eRES_STG_HAKUGYOKURO,					2.0f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },//白玉楼
	// 80   
	{TRUE,  eRES_STG_MY_FOREST,						10.0f, 	BGM_DUNGEON_3,	 			eENE_TBL_IDX_TEST, 		eACTION_OK, },
	{TRUE,  eRES_STG_BLACK_BOX,						10.0f, 	BGM_DUMMY, 					eENE_TBL_IDX_TEST, 		eACTION_OK, },
	//	{TRUE,  eRES_STG_BLACK_BOX,						4.5f, 	BGM_DUMMY, 					eENE_TBL_IDX_TEST, 		eACTION_OK, },
	
	{TRUE,  eRES_STG_INN,							16.0f, 	BGM_TOWN,		 			eENE_TBL_IDX_NONE, 		eACTION_NG, },

	{FALSE, eRES_STG_CHIREIDEN_HONDEN,				20.0f, 	BGM_TOWN, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },//地霊殿
	{FALSE, eRES_STG_CHIREIDEN_ROBY,				20.0f, 	BGM_TOWN, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE,  eRES_STG_SIN_REI_CHO,					3.0f, 	BGM_TOWN,		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE,  eRES_STG_SHIRA_TAMA_ROW,				3.0f, 	BGM_TOWN,		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },

	{FALSE, eRES_STG_ARENA,							10.0f, 	BGM_BOSS_B, 	 			eENE_TBL_IDX_ARENA,		eACTION_OK, },
	{TRUE,  eRES_STG_BIG_WOOD,						10.0f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE,  eRES_STG_MIKAGETSU,						10.0f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_OUTSET_ISLAND,					10.0f, 	BGM_ZELDA, 					eENE_TBL_IDX_NONE, 		eACTION_OK, },

	{TRUE ,eRES_STG_MOB_HARBOR,						22.0f, 	BGM_DUNGEON_G, 	 			eENE_TBL_IDX_NONE, 		eACTION_OK, },//
	{TRUE ,eRES_STG_STEEMPANK_FACTORY,				22.0f, 	BGM_DUNGEON_G, 	 			eENE_TBL_IDX_NONE, 		eACTION_OK, },//

	{TRUE ,eRES_STG_TEMPLE_OF_FOREST,				22.0f, 	BGM_DUNGEON_G, 	 			eENE_TBL_IDX_NONE, 		eACTION_OK, },//
	{TRUE ,eRES_STG_FIRST_PLACE,					22.0f, 	BGM_DUNGEON_G, 	 			eENE_TBL_IDX_NONE, 		eACTION_OK, },//

	{TRUE , eRES_STG_NIGHTS_BAR,					12.0f, 	BGM_TOWN, 		 			eENE_TBL_IDX_NONE, 		eACTION_NG, },
	// 90   
	{FALSE, eRES_STG_KAIPELINA,						6.0f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_NEO_VENICE,					1.4f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{FALSE ,eRES_STG_HARRAPPA_TEST,					7.5f, 	BGM_TOWN,					eENE_TBL_IDX_NONE, 		eACTION_OK, },//ハラッパ
	{TRUE , eRES_STG_COOL_COOL_MOUNTAIN,			20.0f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_KAGITABA_FIELD_01,				1.4f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_KAGITABA_FIELD_02,				1.4f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_JOSAITOSHI,					24.0f, 	BGM_TOWN, 		 			eENE_TBL_IDX_NONE, 		eACTION_NG, },
	{TRUE , eRES_STG_TOY_BOX_STAGE,					3.0f, BGM_TOWN, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_STAR_CHART_STAGE,				1.0f, BGM_DUNGEON_3, 		 		eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_SKY_COLOR_CITY,				140.0f, BGM_TOWN, 		 			eENE_TBL_IDX_NONE, 		eACTION_NG, },
	// 100
	{TRUE , eRES_STG_GARAGE,						2.0f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },

	{TRUE , eRES_STG_CAVE,							30.0f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_CAVE_LOAD,						150.0f,	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },

	{FALSE ,eRES_STG_PARANOIA_PIPE,					4.5f, 	BGM_DUNGEON_G, 	 			eENE_TBL_IDX_041, 		eACTION_OK, },//バベル・

	{TRUE , eRES_STG_UTOPIA,						4.0f, 	BGM_TOWN, 		 			eENE_TBL_IDX_NONE, 		eACTION_NG, },
	{TRUE , eRES_STG_SAN_MARUKO_PLAZA,				1.4f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_NG, },
	{TRUE , eRES_STG_US_CONSTITUTION,				16.0f, 	BGM_TOWN, 		 			eENE_TBL_IDX_NONE, 		eACTION_NG, },
	{TRUE , eRES_STG_VILLAGE_077,					1.5f, 	BGM_TOWN, 		 			eENE_TBL_IDX_NONE, 		eACTION_NG, },
	{TRUE , eRES_STG_HARBOR,						1.5f, 	BGM_TOWN, 		 			eENE_TBL_IDX_NONE, 		eACTION_NG, },
	{TRUE , eRES_STG_TANI,							1.0f, 	BGM_TOWN, 		 			eENE_TBL_IDX_NONE, 		eACTION_NG, },
	{TRUE , eRES_STG_MEIRO,							2.0f, 	BGM_TOWN, 		 			eENE_TBL_IDX_NONE, 		eACTION_NG, },
	{TRUE , eRES_STG_DRMITORY,						100.0f, BGM_TOWN, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_SAIDAN_MQO,					20.0f, 	BGM_ZELDA, 		 			eENE_TBL_KARUN_A, 		eACTION_OK, },
	{TRUE , eRES_STG_SAIDAN_X,						200.0f, BGM_ZELDA, 		 			eENE_TBL_KARUN_A, 		eACTION_OK, },
	{TRUE , eRES_STG_ACTION,						10.0f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_BALL_COASTER,					200.0f, BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_PLANETS_CORE,					80.0f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_002, 		eACTION_OK, },
	{TRUE , eRES_STG_CAMOME_CITY,					35.0f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_NG, },
	{TRUE , eRES_STG_BOMB_BATTLE_FIELD,				20.0f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_REVELNEAR,						20.0f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_TMP_STAGE,						10.0f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_AREA_C,						500.0f, BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_BOSS_STAGE,					100.0f, BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_WARM_ISLAND,					10.0f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },
	{TRUE , eRES_STG_HALLOWEEN_TOWN,				20.0f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_004, 		eACTION_OK, },
	{TRUE , eRES_STG_GAREKI_CITY,					14.0f, 	BGM_ZELDA, 		 			eENE_TBL_IDX_NONE, 		eACTION_OK, },

#endif // _DEBUG
};
static const unsigned short kBG_INFO_NUM = sizeof(sBGInfoTable) / sizeof(tBGInfo);

/*=====================================*
 * エフェクト関連
 *=====================================*/
// エフェクトリソース情報
struct tEffectResourceInfo {
	const char* mShaderName;	// fx名
	const char* mModelName;		// モデル名
};
// 使用エフェクトリスト
static const tEffectResourceInfo sEffectResourceTable[eEF_MAX] = {
	{"Flame/Flame.fx",				"FlameMdlCmn.x"},			//	eEF_FLAME
	{"HitP/HitP.fx",				"DustMdlCmn.x"},			//	eEF_HIT
	{"Thunder/Thunder.fx",			"LightMdlCmn.x"},			//	eEF_THUNDER
	{"Aura/Aura.fx",				"WindMdlCmn.x"},			//	eEF_AURA
	{"Wind/Wind.fx",				"WindMdlCmn.x"},			//	eEF_WIND
	{"DustP/DustP.fx",				"DustMdlCmn.x"},			//	eEF_DUST
	{"Study/Study.fx",				NULL},						//	eEF_MY_TAIL
	{"BlackDustP/BlackDustP.fx",	"DustMdlCmn.x"},			//	eEF_BLACKDUST
	{"WaterDustP/WaterDustP.fx",	"DustMdlCmn.x"},			//	eEF_WATERDUST
	{"LightningEx/LightningEx.fx",	"LightMdlCmn.x"},			//	eEF_LIGHTNING_EX
	{"Lightning/Lightning.fx",		"LightMdlCmn.x"},			//	eEF_LIGHTNING
	{"Navi/Navi.fx",				"ParticleMdlCmn.x"},		//	eEF_NAVI
	{"EnergyBall/EnergyBall.fx",	"ParticleMdlCmn.x"},		//	eEF_ENERGY_BALL
	{"Navi/Navi.fx",				"ParticleMdlCmn.x"},		//	eEF_PROTECT
	{"WindFire/WindFire.fx",		"WindMdlCmn.x"},			//	eEF_WINDFIRE
	{"EnergyBall/EnergyBall.fx",	"ParticleMdlCmn.x"},		//	eEF_ENERGY_BALL_R
	{"EnergyBall/EnergyBall.fx",	"ParticleMdlCmn.x"},		//	eEF_ENERGY_BALL_G
	{"EnergyBall/EnergyBall.fx",	"ParticleMdlCmn.x"},		//	eEF_ENERGY_BALL_B
	{"Marui/Marui.fx",				"ParticleMdlCmn.x"},		//	eEF_MARUI
	{"FlameBomb/FlameBomb.fx",		"SmokeMdlCmn.x"},			//	eEF_FLAME_BOMB
	{"ChargeP/ChargeP.fx",			"DustMdlCmn.x"},			//	eEF_CHARGE
	{"Forest/Forest.fx",			"FlameMdlCmn.x"},			//	eEF_FOREST
	{"Arrow/Arrow.fx",				"FlameMdlCmn.x"},			//	eEF_ARROW
	{"Green/Navi.fx",				"ParticleMdlCmn.x"},		//	eEF_GREEN
	{"RealFire/RealFire.fx",		"TailMdlCmn.x"},			//	eEF_REALFIRE
	{"AutoSmoke2/AutoSmoke2.fx",	"SmokeMdlCmn.x"},			//	eEF_TORNADE
	{"RealFire/WhiteTail.fx",		"TailMdlCmn.x"},			//	eEF_WHITE_TAIL
	{"Sky/sky.fx",					NULL},						//	eEF_SKY
};

// Effekseerのエフェクト一覧
static const char* sEffekseerResourceName[eEFK_MAX] = {
	"MagicSquare.efk",				  // eEFK_MAGIC_SQUARE
	"MagicShield.efk",                // eEFK_SHIELD
	"CorrectItem3.efk",               // eEFK_MAGIC_BALL
	"Simple_Sprite_BillBoard.efk",    // eEFK_SPREAD
	"Simple_Sprite_FixedYAxis.efk",   // eEFK_DISAPPER

	"MagicHeal2.efk",                 // eEFK_PROTECT_START
	"MagicHeal1.efk",                 // eEFK_HEAL
	"Aya_Attack_grass.efk",           // eEFK_BREAK_BARRIER
	"Explosion.efk",                  // eEFK_EXPLOSION_BOMB
	"Aya_Attack_jar.efk",             // eEFK_BREAK_JAR

	"Boss_Death.efk",                 // eEFK_BOSS_DEATH
	"Shock.efk",                      // eEFK_SHOCK
	"Piyopiyo.efk",                   // eEFK_PIYOPIYO
	"ShockThin.efk",                  // eEFK_SHOCK_THIN
	"BreakLock.efk",                  // eEFK_BREAK_LOCK

	"Telekinesis.efk",                // eEFK_TELEKINESIS
	"GetKeyItem2.efk",                // eEFK_GETKEYITEM
	"Laser.efk",                      // eEFK_LASER
	"MekaDragonLaserShoot.efk",       // eEFK_CANNON_LASER_SHOOT
	"sonickwave.efk",                 // eEFK_SONICK_WAVE

	"wind.efk",                       // eEFK_WIND
	"remilia_heart_bullet.efk",       // eEFK_THRUST
	"HealPotion2.efk",                // eEFK_PORTION
	"MekaDragonLaserCharge.efk",      // eEFK_CANNON_LASER_CHARGE
	"utsuho_sun_area.efk",            // eEFK_AURA_FLAME

	"patch_stElmo_area.efk",          // eEFK_HELL_FLAME
	"RealFlame.efk",                  // eEFK_REAL_FLAME
	"TitleEffect.efk",                // eEFK_STAGE_CLEAR
	"LightningStrike.efk",            // eEFK_LIGHTNING_STRIKE
	"aya_senpu_area.efk",             // eEFK_SENPU_AREA

	"magma_effect.efk",               // eEFK_MAGMA
	"Thunder2.efk",                   // eEFK_HIT_LIGHTNING
	"MAKING.efk",					// eEFK_HIT_LIGHTNING

#ifdef _DEBUG
	"",								  // eEFK_DRAG_DROP_SAMPLE,
#endif // _DEBUG
};


/*=====================================*
 * マップ構造体
 *=====================================*/
struct tMapIconWork {
	LayoutBase		mpLayout[8];
	int				mLayoutNum;
};
struct tShowMapWork {
	BOOL			mIsZoom;
	float			mCamDist;
	VECTOR			mCamCenterPos;
};
static tShowMapWork	sShowMapWork;
static tMapIconWork	sMapIconMove;

/*=====================================*
 * static values
 *=====================================*/
SceneBase*		spScenBase;		// 自分自身のポインタ
CharaPlayer		sPlayer;
CharaEnemy**	sppEnemy				= NULL;
CharaNpc*		spNpc					= NULL;
ObjectBase**	sppObjectBase			= NULL;
int				sObjectBaseNum			= 0;
CharaEnemy*		spDisappearEnemy		= NULL;
ObjectItem		sDropItem[kENEMY_ITEM_NUM];
ModelMap		sModelMap;
ModelMap*		spSubModelMap			= NULL;
ModelBase		sModelXfile[eMD_MAX];
ModelPmd		sGimmick;

CharaNavi		sNavi;
LayoutBase*		spShopItemLayout		= NULL;

CsvAccesser		sEventCsv;
int				sStageIndex;
int				sStagePlaceIndex;
int				sIsMessageRange			= -1;
tBGInfo*		spBGInfo				= NULL;
ActionTaskManager::tTaskInfo sTaskInfo;
int				sEventBattleFlag		= -1;
CHAR			sNextEventNo			= -1;	// 次のイベント番号
int				sEventRecoveryFlag		= -1;	// 取り消すかもしれないイベントフラグ
bool			sIsBattle				= false;
char*			sStrNaviTalkPtr			= NULL;
int				sLoadStatus				= 0;
int				sDungeonIndex			= 0;
BYTE			sDrawFreeFontValue		= eFREE_FONT_NONE;
int				sEventPhase				= 0;
int				sEventCount				= 0;
int				sBattleIndex			= -1;
VECTOR			sReviseDiffPos			= ZERO_VEC;
std::vector<eItemKind>	sMyWeaponList;			// 装備リスト
std::vector<tStageInfo> sSubStageInfoList;		// ステージ名リスト
LayoutBase*		spStageClearLayoutList	= NULL;	// ステージクリア表示時のレイアウト
//static SubSceneItemMenu* spSubSceneItemMenu	= NULL;
static SubSceneBase* spSubSceneInst		= NULL;
StageFuncBase*  spStageFunc				= NULL;
int				sNpcNum;
float			sDissappearLength		= 12000.0f;
float			kGRAVITY_SPEED			= -0.98f;
BYTE			sOcarinaSongSelect		= 0;	// オカリナ選曲インデックス


/* ロード項目 */
enum {
	eLOAD_PLAYER = 0,
	eLOAD_ENEMY,
	eLOAD_NPC,
	eLOAD_SKYDOME,
	eLOAD_MAP,
	eLOAD_SUB_MAP,
	eLOAD_SHOW_A_MAP,
	eLOAD_OBJECT,
	eLOAD_TEXTURE,
	eLOAD_SOUND,
	eLOAD_EFFECT,

	eLOAD_MAX,
};

#ifdef _DEBUG
static const char* strLoadStateName[] = {
	"eLOAD_PLAYER ",	// eLOAD_PLAYER "
	"eLOAD_ENEMY",		// eLOAD_ENEMY"
	"eLOAD_NPC",		// eLOAD_NPC"
	"eLOAD_SKYDOME",	// eLOAD_SKYDOME"
	"eLOAD_MAP",		// eLOAD_MAP"
	"eLOAD_SUB_MAP",	// eLOAD_SUB_MAP"
	"eLOAD_SHOW_A_MAP",	// eLOAD_SHOW_A_MAP"
	"eLOAD_OBJECT",		// eLOAD_OBJECT"
	"eLOAD_TEXTURE",	// eLOAD_TEXTURE"
	"eLOAD_SOUND",		// eLOAD_SOUND"
	"eLOAD_EFFECT",		// eLOAD_EFFECT"
};
#endif // _DEBUG


//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
SceneMainGame::SceneMainGame()
{
	sStageIndex			= -1;
	sStagePlaceIndex	= -1;

#ifdef _DEBUG
	// クラスのサイズを調べる為のフラグ
	static BYTE sClassDumpFlag = FALSE;

	if (sClassDumpFlag == FALSE) {

		PRINT_CONSOLE("  * ActionTaskManager \t%08d \n", sizeof(ActionTaskManager ));
		PRINT_CONSOLE("  * Basic \t%08d \n", sizeof(Basic ));
		PRINT_CONSOLE("  * BillboardBase \t%08d \n", sizeof(BillboardBase ));
		PRINT_CONSOLE("  * BladeEffect\t%08d \n", sizeof(BladeEffect));
//		PRINT_CONSOLE("  * BookSystem \t%08d \n", sizeof(BookSystem ));
		PRINT_CONSOLE("  * CameraManager\t%08d \n", sizeof(CameraManager));
		PRINT_CONSOLE("  * CharaBase\t%08d \n", sizeof(CharaBase));
		PRINT_CONSOLE("  * CharaEnemy\t%08d \n", sizeof(CharaEnemy));
		PRINT_CONSOLE("  * CharaNavi\t%08d \n", sizeof(CharaNavi));
		PRINT_CONSOLE("  * CharaNpc\t%08d \n", sizeof(CharaNpc));
		PRINT_CONSOLE("  * CharaPlayer\t%08d \n", sizeof(CharaPlayer));
		PRINT_CONSOLE("  * CollisionManager\t%08d \n", sizeof(CollisionManager));
		PRINT_CONSOLE("  * Counter\t%08d \n", sizeof(Counter));
		PRINT_CONSOLE("  * CsvAccesser \t%08d \n", sizeof(CsvAccesser ));
		PRINT_CONSOLE("  * EffectManager \t%08d \n", sizeof(EffectManager ));
		PRINT_CONSOLE("  * EffectNode \t%08d \n", sizeof(EffectNode ));
		PRINT_CONSOLE("  * EffectResource \t%08d \n", sizeof(EffectResource ));
//		PRINT_CONSOLE("  * EffectTest\t%08d \n", sizeof(EffectTest));
		PRINT_CONSOLE("  * EffekseerManager\t%08d \n", sizeof(EffekseerManager));
		PRINT_CONSOLE("  * EventManager \t%08d \n", sizeof(EventManager ));
		PRINT_CONSOLE("  * GimmickBase\t%08d \n", sizeof(GimmickBase));
		PRINT_CONSOLE("  * GimmickExtend_Hole\t%08d \n", sizeof(GimmickExtend_Hole));
		PRINT_CONSOLE("  * GimmickExtend_Lift\t%08d \n", sizeof(GimmickExtend_Lift));
		PRINT_CONSOLE("  * GimmickExtend_Motion\t%08d \n", sizeof(GimmickExtend_Motion));
		PRINT_CONSOLE("  * GimmickExtend_Roll\t%08d \n", sizeof(GimmickExtend_Roll));
		PRINT_CONSOLE("  * GimmickExtend_Ship\t%08d \n", sizeof(GimmickExtend_Ship));
		PRINT_CONSOLE("  * GimmickExtend_Tower\t%08d \n", sizeof(GimmickExtend_Tower));
		PRINT_CONSOLE("  * GimmickExtend_Trolley\t%08d \n", sizeof(GimmickExtend_Trolley));
		PRINT_CONSOLE("  * GimmickExtend_Wheel\t%08d \n", sizeof(GimmickExtend_Wheel));
		PRINT_CONSOLE("  * GraphicManager\t%08d \n", sizeof(GraphicManager));
		PRINT_CONSOLE("  * InputManager\t%08d \n", sizeof(InputManager));
		PRINT_CONSOLE("  * LayoutBase\t%08d \n", sizeof(LayoutBase));
//		PRINT_CONSOLE("  * LayoutNumber\t%08d \n", sizeof(LayoutNumber));
		PRINT_CONSOLE("  * ModelBase\t%08d \n", sizeof(ModelBase));
		PRINT_CONSOLE("  * ModelCollision\t%08d \n", sizeof(ModelCollision));
		PRINT_CONSOLE("  * ModelMap\t%08d \n", sizeof(ModelMap));
		PRINT_CONSOLE("  * ModelPmd\t%08d \n", sizeof(ModelPmd));
		PRINT_CONSOLE("  * ObjectBase\t%08d \n", sizeof(ObjectBase));
		PRINT_CONSOLE("  * ObjectHand\t%08d \n", sizeof(ObjectHand));
		PRINT_CONSOLE("  * ObjectHand_Bomb\t%08d \n", sizeof(ObjectHand_Bomb));
		PRINT_CONSOLE("  * ObjectHand_Jar\t%08d \n", sizeof(ObjectHand_Jar));
		PRINT_CONSOLE("  * ObjectHand_Lock\t%08d \n", sizeof(ObjectHand_Lock));
		PRINT_CONSOLE("  * ObjectHand_Stone\t%08d \n", sizeof(ObjectHand_Stone));
		PRINT_CONSOLE("  * ObjectHand_Usachu\t%08d \n", sizeof(ObjectHand_Usachu));
		PRINT_CONSOLE("  * ObjectItem\t%08d \n", sizeof(ObjectItem));
		PRINT_CONSOLE("  * ObjectSeesaw\t%08d \n", sizeof(ObjectSeesaw));
		PRINT_CONSOLE("  * ObjectSwitch\t%08d \n", sizeof(ObjectSwitch));
		PRINT_CONSOLE("  * ObjectTresure\t%08d \n", sizeof(ObjectTresure));
		PRINT_CONSOLE("  * ObjectWeapon\t%08d \n", sizeof(ObjectWeapon));
		PRINT_CONSOLE("  * ObjectWeapon_Arrow\t%08d \n", sizeof(ObjectWeapon_Arrow));
		PRINT_CONSOLE("  * ObjectWeapon_Meteo\t%08d \n", sizeof(ObjectWeapon_Meteo));
		PRINT_CONSOLE("  * ObjectWeapon_MeteoRefrect\t%08d \n", sizeof(ObjectWeapon_MeteoRefrect));
		PRINT_CONSOLE("  * ObjectWeapon_Missile\t%08d \n", sizeof(ObjectWeapon_Missile));
		PRINT_CONSOLE("  * Path\t%08d \n", sizeof(Path));
		PRINT_CONSOLE("  * ResourceLayout\t%08d \n", sizeof(ResourceLayout));
		PRINT_CONSOLE("  * ResourceManager\t%08d \n", sizeof(ResourceManager));
		PRINT_CONSOLE("  * ResourceModel\t%08d \n", sizeof(ResourceModel));
		PRINT_CONSOLE("  * SceneBase \t%08d \n", sizeof(SceneBase ));
//		PRINT_CONSOLE("  * SceneEvent\t%08d \n", sizeof(SceneEvent));
		PRINT_CONSOLE("  * SceneManager\t%08d \n", sizeof(SceneManager));
		PRINT_CONSOLE("  * SceneMainGame\t%08d \n", sizeof(SceneMainGame));
//		PRINT_CONSOLE("  * SceneSelectFile\t%08d \n", sizeof(SceneSelectFile));
//		PRINT_CONSOLE("  * SceneTitle\t%08d \n", sizeof(SceneTitle));
		PRINT_CONSOLE("  * Shadow \t%08d \n", sizeof(Shadow ));
		PRINT_CONSOLE("  * ShapeBase\t%08d \n", sizeof(ShapeBase));
		PRINT_CONSOLE("  * ShapeBoard\t%08d \n", sizeof(ShapeBoard));
		PRINT_CONSOLE("  * ShapeBox\t%08d \n", sizeof(ShapeBox));
		PRINT_CONSOLE("  * ShapeCapsule\t%08d \n", sizeof(ShapeCapsule));
		PRINT_CONSOLE("  * ShapeLine\t%08d \n", sizeof(ShapeLine));
		PRINT_CONSOLE("  * SoundComponent\t%08d \n", sizeof(SoundComponent));
		PRINT_CONSOLE("  * SoundManager\t%08d \n", sizeof(SoundManager));
		PRINT_CONSOLE("  * TotalInfo \t%08d \n", sizeof(TotalInfo ));
		PRINT_CONSOLE("  * Utility\t%08d \n", sizeof(Utility));
//		PRINT_CONSOLE("  * VectorNode\t%08d \n", sizeof(VectorNode));

		sClassDumpFlag = TRUE;

	}

#endif // _DEBUG
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
SceneMainGame::~SceneMainGame()
{
	TotalInfo::GetInstance()->SetEnableWinMenu(FALSE);

	Destroy();

	EventManager::GetInstance()->EndEvent();

	sPlayer.Destroy();


	// 音楽をとめる
	SoundManager::GetInstance()->StopBGM();
}

//---------------------------------------------------
/**
   作成
 */
//---------------------------------------------------
void
SceneMainGame::Create(int debugArg)
{
	SceneBase::Create(debugArg);

	spScenBase			= this;	// 自分自身のポインタを覚えさせておく
	sStageIndex			= mDebugArg;
	sStagePlaceIndex	= 0;

	// データをロードする
	TotalInfo::tLoadWork load_work;
	TotalInfo::GetInstance()->GetLoadWork(&load_work);
	if (load_work.mPlaceIndex != 0) {	// ロード完了であれば
		sStageIndex							= load_work.mPlaceIndex;
		sPlayer.SetLoadParameter(load_work.mMaxHP, load_work.mNowHP, load_work.mNowMP);
		sPlayer.ItemWork().mItemSubWeapon	= (eItemKind)load_work.mNowWeapon;
		sPlayer.CosplayIndex()				= load_work.mNowCosplayIndex;
		sStagePlaceIndex					= load_work.mSubPlaceIndex;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::Update(void)
{
	int phase = mPhase;

	(this->*mBasePhaseFunc[mPhase])();

	if (mPhase > ePHASE_SETUP) {
		// HPの応じたハートの描画
		int hitpoint = sPlayer.GetHitpoint();
		if (hitpoint < 0) hitpoint = 0;
		int max_hitpoint = sPlayer.GetMaxHitpoint();

		// ハートのレイアウトの更新
		TotalInfo::GetInstance()->UpdateHeartLayout(max_hitpoint, hitpoint);
	}

	mPhaseCounter += kADD_PHASE_COUNTER_VALUE;
	// フェーズが切り替わったのなら
	if (phase != mPhase) {

		// 特別条件以外はナビォトークの設定をクリアする
		if (!(( phase == ePHASE_SELECT_WEAPON_END || phase == ePHASE_PLAY_OCARINA) &&
			(mPhase == ePHASE_GAME))) {
			mPhaseCounter = 0.0f;
			// ナビォのトークインデックスを変更
			sStrNaviTalkPtr = NULL;
			// DrawFontFuncをクリア
			GraphicManager::GetInstance()->EntryDrawFontFunc(NULL);
		}
	}

	/* ドラッグ＆ドロップ時の処理を記入 */
	SetDragFileValidFlag(TRUE);
	int drag_num = GetDragFileNum();
	if (drag_num > 0) {
		char buf[256] = {0};
		GetDragFilePath(buf);

		// 拡張子を取得する
		char extention[8] = {0};
		for (int iI = 0; iI < (signed)strlen(buf); iI ++) {
			if (buf[iI] == '.') {
				strcpy_s(extention, sizeof(extention), &buf[iI+1]);
			}
		}

		// 拡張子からファイル種別を判別する
		if ((strcmp(extention, "ogg") == 0) || (strcmp(extention, "mp3") == 0) || (strcmp(extention, "wav") == 0)) {
			// 音声ファイル読み込みの処理
			PlayMusic(buf, DX_PLAYTYPE_LOOP);
			PRINT_CONSOLE("drag file name : %s \n", buf);
		} else if ((strcmp(extention, "mv1") == 0)) {
			// モデルファイル読み込みの処理
			ResourceModel::GetInstance()->LoadDragMv1Resource(buf);
			sPlayer.DeleteModel();
			sPlayer.LinkModel(buf);
			sPlayer.Setup(false, sStageIndex);
			sPlayer.StartMotion(0, true, 20.0f);
		} else if ( (strcmp(extention, "jpg") == 0) ){
			// スカイドームテクスチャファイル読み込みの処理
			int graph_handle = -1;
			char file_name[64] = {0};
			char file_path[32] = {0};
			Path::GetOnlyFileName(file_name, sizeof(file_name), buf);
			sprintf_s(file_path, sizeof(file_path), "Skydome/%s", file_name);
			ResourceLayout::GetInstance()->LoadLayoutResource(file_path);
			graph_handle = ResourceLayout::GetInstance()->GetHandle(file_path);
			MV1SetTextureGraphHandle(sModelXfile[eMD_SKYDOME].GetModelHandle(),
									 0,											// TexIndex は0固定
									 graph_handle,
									 FALSE);
#ifdef _DEBUG
		}
		else if ( (strcmp(extention, "efk") == 0) ) {

			EffekseerManager::GetInstance()->DeleteEffect(eEFK_DRAG_DROP_SAMPLE);

			char file_name[64] = {0};
			Path::GetOnlyFileName(file_name, sizeof(file_name), buf);
			PRINT_CONSOLE("drag file name : %s \n", file_name);

			EffekseerManager::GetInstance()->LoadResource(eEFK_DRAG_DROP_SAMPLE, file_name);
			
			VECTOR eff_pos = sPlayer.Position();
			eff_pos.y += sPlayer.BodyHeight();
			float eff_scale = 60.0f;

			EffekseerManager::GetInstance()->Play(eEFK_DRAG_DROP_SAMPLE,
												  &eff_pos,
												  eff_scale);
		}
		else if ( (strcmp(extention, "fx") == 0) ) {

			static int sEffHandle = -1;

			// エフェクトファイルの読み込み
			char file_name[64] = {0};
			char dir_name[32]  = {0};
			char file_path[32] = {0};

			if (sEffHandle != -1) {
				EffectManager::GetInstance()->Stop(sEffHandle);
				sEffHandle =-1;
				return;
			}
			EffectManager::GetInstance()->UnloadResource(eEF_DRAG_DROP);

			Path::GetOnlyFileName(file_name, sizeof(file_name), buf);
			memcpy(dir_name, file_name, (strlen(file_name) - 3) );
			sprintf_s(file_path, sizeof(file_path), "%s/%s", dir_name, file_name);
			PRINT_CONSOLE("drag file name : %s \n", file_path);
			EffectManager::GetInstance()->LoadResource(eEF_DRAG_DROP, file_path, NULL);

			//			eff_handle = EffectManager::GetInstance()->Play(eEF_DRAG_DROP, &sPlayer.Position(), 50.0f, 0.0f);
			sEffHandle = EffectManager::GetInstance()->Play(eEF_DRAG_DROP, 
															&sPlayer.Position(), 
															10.0f);
			
#endif // _DEBUG
		}
	}

#ifdef _DEBUG
	if (CHECK_TRIG_KEY(DBG_KEY_DUMP_STATUS)) {

		PRINT_CONSOLE(" * ========================================================= * \n");
		PRINT_CONSOLE(" * Dump Start ============================================== * \n");
		PRINT_CONSOLE(" * ========================================================= * \n");
		// イベントフラグを表示させる
		int place_index = ConvertPlaceIndex_TO_EventDefine(sStageIndex);
		if (place_index != eEVENT_ERROR) {
			ULONG flag = TotalInfo::GetInstance()->GetEventFlagValue(place_index);
			PRINT_CONSOLE(" * event flag[%d] = 0x%08x \n", place_index, flag);
		}

		/* CameraManager のDump */
		CameraManager::GetInstance()->DumpStatus();

		PRINT_CONSOLE(" * ========================================================= * \n");
		PRINT_CONSOLE(" * Dump End ================================================ * \n");
		PRINT_CONSOLE(" * ========================================================= * \n");
	}
#endif // _DEBUG

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
SceneMainGame::GetLoadingPercent(void)
{
	int load_num = GetASyncLoadNum();
	float rate = (float)load_num / (float)eLOAD_MAX;
	return (int)(rate * 100.0f);

	//	return (int)(((float)(eLOAD_MAX - GetASyncLoadNum()) / (float)eLOAD_MAX) * 100.0f);
	//	return (int)( ( (float)GetASyncLoadNum() / (float)eLOAD_MAX) * 100.0f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::ReviseAllModel(void)
{
#ifdef _DEBUG
	SoundManager::GetSndComponent()->PlaySound(eSE_DECIDE_KEY);
#endif // _DEBUG

	VECTOR move_vec = sPlayer.Position();
	ModelBase* p_model = NULL;

	// ReviseAllModel の座標を記憶させる
	sReviseDiffPos = VAdd(sReviseDiffPos, move_vec);
	PRINT_CONSOLE("revise all model, sReviseDiffPos = {%.2f, %.2f, %.2f}, dist = %.2f \n", 
				  sReviseDiffPos.x, sReviseDiffPos.y, sReviseDiffPos.z, VSize(sReviseDiffPos) );

	sPlayer.ReviseModel(&move_vec);
  
	for (int iI = 0; iI < kENEMY_ONCE_MAX; iI ++) {
		p_model = sppEnemy[iI];
		p_model->Position() = VSub(p_model->Position(), move_vec);
		p_model->ForciblySetPositionByCentering(true);
	}

	for (int iI = 0; iI < sNpcNum; iI ++) {
		p_model = &spNpc[iI];
		p_model->Position() = VSub(p_model->Position(), move_vec);
		p_model->ForciblySetPositionByCentering(true);
		spNpc[iI].SetShadowVector(move_vec);
	}

	for (int iI = 0; iI < sObjectBaseNum; iI ++) {
		p_model = sppObjectBase[iI];
		p_model->Position() = VSub(p_model->Position(), move_vec);
		p_model->ForciblySetPositionByCentering(true);
	}


	p_model = &sModelMap;
	p_model->Position() = VSub(p_model->Position(), move_vec);
	p_model->ForciblySetPositionByCentering(true);
	
	for (unsigned int iI = 0; iI < sSubStageInfoList.size(); iI ++) {
		p_model = &spSubModelMap[ iI];
		p_model->Position() = VSub(p_model->Position(), move_vec);
		p_model->ForciblySetPositionByCentering(true);
	}

	for (int iI = 0; iI < eMD_MAX; iI ++) {
		p_model = &sModelXfile[iI];
		p_model->Position() = VSub(p_model->Position(), move_vec);
		p_model->ForciblySetPositionByCentering(true);
	}

	EffectManager* const p_effmgr = EffectManager::GetInstance();
	for (int iI = 0; iI < EffectManager::kEFFECT_NODE_NUM; iI ++) {
		p_effmgr->Position(iI) = VSub(p_effmgr->Position(iI), move_vec);
	}

	// ギミッククラスの mOnPosition の値を更新する
	GimmickBase::ReviseAllGimmickOnPosition(move_vec);

	CollisionManager::GetInstance()->Refresh();

	// ポイントライトの更新
	{
		VECTOR pos;
		GraphicManager::GetInstance()->GetPLightPosition(&pos);
		pos = VSub(pos, move_vec);
		GraphicManager::GetInstance()->SetPLightPosition(&pos);
	}

	CameraManager::GetInstance()->ReviseCamera(move_vec);

	spStageFunc->ReviseAllModel(move_vec);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::PauseReviseAllModel(bool isPause)
{
#ifdef _DEBUG
	//	SoundManager::GetSndComponent()->PlaySound(eSE_DECIDE_KEY);
#endif // _DEBUG

	//	VECTOR move_vec = sPlayer.Position();
	VECTOR move_vec = VScale(sReviseDiffPos, (isPause == true) ? -1.0f : 1.0f);
	ModelBase* p_model = NULL;

	// ReviseAllModel の座標を記憶させる
	sPlayer.ReviseModel(&move_vec);
  
	for (int iI = 0; iI < kENEMY_ONCE_MAX; iI ++) {
		p_model = sppEnemy[iI];
		p_model->Position() = VSub(p_model->Position(), move_vec);
		p_model->ForciblySetPositionByCentering(true);
	}

	for (int iI = 0; iI < sNpcNum; iI ++) {
		p_model = &spNpc[iI];
		p_model->Position() = VSub(p_model->Position(), move_vec);
		p_model->ForciblySetPositionByCentering(true);
		spNpc[iI].SetShadowVector(move_vec);
	}

	for (int iI = 0; iI < sObjectBaseNum; iI ++) {
		p_model = sppObjectBase[iI];
		p_model->Position() = VSub(p_model->Position(), move_vec);
		p_model->ForciblySetPositionByCentering(true);
	}


	p_model = &sModelMap;
	p_model->Position() = VSub(p_model->Position(), move_vec);
	p_model->ForciblySetPositionByCentering(true);
	
	for (unsigned int iI = 0; iI < sSubStageInfoList.size(); iI ++) {
		p_model = &spSubModelMap[ iI];
		p_model->Position() = VSub(p_model->Position(), move_vec);
		p_model->ForciblySetPositionByCentering(true);
	}

	for (int iI = 0; iI < eMD_MAX; iI ++) {
		p_model = &sModelXfile[iI];
		p_model->Position() = VSub(p_model->Position(), move_vec);
		p_model->ForciblySetPositionByCentering(true);
	}

	EffectManager* const p_effmgr = EffectManager::GetInstance();
	for (int iI = 0; iI < EffectManager::kEFFECT_NODE_NUM; iI ++) {
		p_effmgr->Position(iI) = VSub(p_effmgr->Position(iI), move_vec);
	}

	// ギミッククラスの mOnPosition の値を更新する
	GimmickBase::ReviseAllGimmickOnPosition(move_vec);

	CollisionManager::GetInstance()->Refresh();

	// ポイントライトの更新
	{
		VECTOR pos;
		GraphicManager::GetInstance()->GetPLightPosition(&pos);
		pos = VSub(pos, move_vec);
		GraphicManager::GetInstance()->SetPLightPosition(&pos);
	}

	CameraManager::GetInstance()->ReviseCamera(move_vec);

	spStageFunc->ReviseAllModel(move_vec);

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
//StageFuncBase*  spStageFunc				= NULL;
/*static*/StageFuncBase*
SceneMainGame::GetStageFunc(void)
{
	return spStageFunc;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/USHORT
SceneMainGame::GetMyWeaponNum(void)
{
	return (USHORT)sMyWeaponList.size();
}

//---------------------------------------------------
/**
   指定のキャラにプレイヤーを変更する
 */
//---------------------------------------------------
/*static*/void 
SceneMainGame::ChangePlayerModel(const char* strModelName, BOOL isEffect/*=TRUE*/)
{
	sPlayer.DisableLockonChara();

	// モデルファイル読み込みの処理
	ResourceModel::GetInstance()->LoadCharaResource( (char *)strModelName);
	sPlayer.DeleteModel();
	sPlayer.LinkModel(strModelName);
	sPlayer.Setup(false, sStageIndex);
	sPlayer.StartMotion(0, true, 0.5f);
	sPlayer.SetPositionOnCollision();
	
	if (strcmp( (char *)strModelName, Name_Lino) == 0) {
		sPlayer.ResumeAttach(CharaPlayer::eATTACH_MODEL_SWORD);
		sPlayer.SetActionLimit(spBGInfo->mActionLimit,
							   sPlayer.ItemWork().mItemSubWeapon);
	}
	else {
		sPlayer.PauseAttach(CharaPlayer::eATTACH_MODEL_SWORD);
		sPlayer.SetActionLimit(eACTION_NG, sPlayer.ItemWork().mItemSubWeapon);
	}
	
	if (isEffect) {
		EffekseerManager::GetInstance()->Play(eEFK_GETKEYITEM, &sPlayer.Position(), 10.0f);
		// 『えいっ！』
		SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_ATTACK_2);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/const char*
SceneMainGame::GetStageName(int index)
{
	//	return sBGInfoTable[index % kBG_INFO_NUM].mFileName;
	return sResourceStageNameTable[sBGInfoTable[index % kBG_INFO_NUM].mFileIndex];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/int
SceneMainGame::GetStageIndex(void)
{
	return sStageIndex;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/int
SceneMainGame::GetStageActionLimit(void)
{
	return spBGInfo->mActionLimit;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/bool
SceneMainGame::GetIsBattle(void)
{
	return sIsBattle;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/bool
SceneMainGame::RequestChangeState(void* pParam)
{
	ActionTaskManager::tTaskInfo* p_param;
	p_param = (ActionTaskManager::tTaskInfo *)pParam;

	// ePHASE_GAME 以外は弾く
	if (!(
		  (mPhase == SceneMainGame::ePHASE_GAME) || 
		  (mPhase == SceneMainGame::ePHASE_FLYING_MOVE) || 
		  (mPhase == SceneMainGame::ePHASE_PLAY_OCARINA) || 
		  ((mPhase == SceneMainGame::ePHASE_GET_ITEM) && (p_param->mTaskType == ActionTaskManager::eTASK_MOVE)) ||
		  ((mPhase == SceneMainGame::ePHASE_EVENT) && (p_param->mTaskType == ActionTaskManager::eTASK_MOVE))
	)) {

#ifdef _DEBUG
		PRINT_CONSOLE("Cannot Execute RequestChangeState() because, not permitted phase \n");
#endif // _DEBUG
		return false;
	}

	sTaskInfo.mTaskType		= p_param->mTaskType;
	sTaskInfo.mTaskValueA	= p_param->mTaskValueA;
	sTaskInfo.mTaskValueB	= p_param->mTaskValueB;

	switch (sTaskInfo.mTaskType) {
	case ActionTaskManager::eTASK_MOVE:

		PRINT_CONSOLE("Accept Task : [MOVE] , %d , %d\n", sTaskInfo.mTaskValueA, sTaskInfo.mTaskValueB);

		sPlayer.PermitAcceptInput(false);

		// オブジェクト使用状態からオブジェクトフラグを更新
		setObjectFlagFromObjectUsed();

		if (sStageIndex == sTaskInfo.mTaskValueA) {

			// バトルインデックスフラグは元に戻す
			sBattleIndex = -1;
			// イベントバトルフラグも元に戻す
			sEventBattleFlag = -1;
			
			// イベントフラグのキャンセル予約設定
			if (sEventRecoveryFlag != -1) {
				TotalInfo::GetInstance()->DisableEventFlag(BATTLEFLAG_TO_TASKVALUE_A(sEventRecoveryFlag),
														   BATTLEFLAG_TO_TASKVALUE_B(sEventRecoveryFlag));
			}

			// ロックオン解除
			sPlayer.DisableLockonChara();

			destroyAllEnemy(); // 敵を全滅させる
			mPhase = SceneMainGame::ePHASE_MOVE_PLACE;
		}
		else {
			//			GimmickBase::DeleteAllGimmick();

			// ギミックの中から現在の指定のイベントを契機に動作するギミックを検査する
			GimmickBase* p_gimmick = NULL;
			for (int iI = 0; iI < GimmickBase::GetGimmickBaseNum(); iI ++) {
				p_gimmick = (GimmickBase *)GimmickBase::GetGimmickBase(iI);
				p_gimmick->SetState(0);
			}

			mPhase = SceneMainGame::ePHASE_CHANGE_STAGE;
		}
		sStageIndex			= sTaskInfo.mTaskValueA;
		sStagePlaceIndex	= sTaskInfo.mTaskValueB;

		// フェードアウト
		FADE_BOARD()->StartFade(255, 0.5f);

		break;

	case ActionTaskManager::eTASK_FALLDOWN:

		// 悲鳴
		SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_FALL_DOWN);

		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_DUMMY);

		GimmickBase::SetGimmickPauseAttach(TRUE);

		mPhase = SceneMainGame::ePHASE_FALL_DOWN;
		FADE_OUT(0.75f);

		break;

	case ActionTaskManager::eTASK_EVENT:
		unsigned long event_kind;
		unsigned long event_flag;
		event_kind = sTaskInfo.mTaskValueA;
		APP_ASSERT_PRINT(event_kind < eEVENT_MAX,
						 "event_kind : %d is Invalid Value\n", event_kind);
		event_flag = sTaskInfo.mTaskValueB;
		APP_ASSERT_PRINT(event_flag < kEVENT_FLAG_NUM,
						 "event_flag : 0x%d is Invalid Value\n", event_flag);
		if (TotalInfo::GetInstance()->GetEventFlagEnable(event_kind, event_flag) == false) {
			mPhase = SceneMainGame::ePHASE_EVENT_BEFORE;
		} 
		else {
			// イベント実施後の場合は禁則解除
			//			sPlayer.PermitAcceptInput(true);
			return false;
		}

		break;

	case ActionTaskManager::eTASK_ENEMY:

		// アクション制限OKの場合のみ、敵を出現させる
		if (spBGInfo->mActionLimit == eACTION_OK) {
			if (sBattleIndex != sTaskInfo.mTaskValueA) {
				// ロックオン解除
				sPlayer.DisableLockonChara();

				/* イベント開始時は敵を全滅させる */
				destroyAllEnemy();

				sBattleIndex = sTaskInfo.mTaskValueA;

				mPhase = SceneMainGame::ePHASE_START_BATTLE;
			}
		}
		break;

	case ActionTaskManager::eTASK_NAVI:
		if (sStrNaviTalkPtr == NULL) {
			int event_def =	ConvertPlaceIndex_TO_EventDefine(sStageIndex);
			APP_ASSERT_PRINT((event_def != eEVENT_ERROR),
							"Select event_def %d is invalid value \n", event_def);
			//			const char* talk_msg = sStrNaviTalkMessage[event_def][sTaskInfo.mTaskValueB];
			const char* talk_msg = GetStrNaviTalkMessage(event_def, sTaskInfo.mTaskValueB);

			if (TotalInfo::GetInstance()->GetNaviTalkFlagEnable(event_def, sTaskInfo.mTaskValueB) == false) {
				SceneMainGame::StartNaviTalk(talk_msg);
				TotalInfo::GetInstance()->SetNaviTalkFlag(event_def, sTaskInfo.mTaskValueB);
			}
		}
		break;

	case ActionTaskManager::eTASK_SHOP:
		sPlayer.PermitAcceptInput(false);
		mPhase = SceneMainGame::ePHASE_SHOP;
		break;

	case ActionTaskManager::eTASK_GETITEM:
		sPlayer.PermitAcceptInput(false);
		mPhase = SceneMainGame::ePHASE_GET_ITEM_BEFORE;
		break;

	case ActionTaskManager::eTASK_ENEMY_ITEM:
		/* ここでアイテムを増やすと mDrawModelVector を破壊してしまうので、
		   別のタイミングで処理 */
		spDisappearEnemy = (CharaEnemy *)sTaskInfo.mTaskValueA;
		break;

	case ActionTaskManager::eTASK_SAVE:
		sPlayer.PermitAcceptInput(false);
		mPhase = SceneMainGame::ePHASE_SAVE;

		MSG_TYPE_TELOP();
		MSG_FADE_IN(0.2f);
		break;

	case ActionTaskManager::eTASK_NAVI_EX:
		if (sStrNaviTalkPtr == NULL) {
			const char* talk_msg = (const char *)sTaskInfo.mTaskValueA;
			SceneMainGame::StartNaviTalk(talk_msg);
		}
		break;

	case ActionTaskManager::eTASK_CHECK_POINT:

		SoundManager::GetSndComponent()->PlaySound(eSE_PRESS_KEY);
		SceneMainGame::StartNaviTalk(strNaviTalk_CheckPoint);
		sPlayer.SetCheckPointPtr((ObjectBase *)sTaskInfo.mTaskValueA);
		break;

	case ActionTaskManager::eTASK_FLYING_MOVE:
		sPlayer.PermitAcceptInput(false);
		mPhase = SceneMainGame::ePHASE_FLYING_MOVE;
		//		mPhaseCounter = 0.0f;

		// ※後で eTASK_MOVE が実行されるが、そのタイミングではオブジェクトフラグが更新されないのでこのタイミングで呼ぶ必要あり
		// オブジェクト使用状態からオブジェクトフラグを更新
		setObjectFlagFromObjectUsed();

		spSubSceneInst = new SubSceneFlyingMove();
		APP_ASSERT((spSubSceneInst != NULL),
				   "Failed new SubSceneFlyingMove class \n");
		spSubSceneInst->Create();

		break;


	case ActionTaskManager::eTASK_PLAY_OCARINA:

		sPlayer.PermitAcceptInput(false);	// 入力禁止
		mPhase = SceneMainGame::ePHASE_PLAY_OCARINA;

		spSubSceneInst = new SubScenePlayOcarina();
		APP_ASSERT((spSubSceneInst != NULL),
					"Failed new SubScenePlayOcarina class \n");
		spSubSceneInst->Create();
		((SubScenePlayOcarina *)spSubSceneInst)->SetOcarinaSongSelect(sOcarinaSongSelect);
		break;

	default:
		break;

	};

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::destroyEnemyWork(int enemyIndex)
{
	sppEnemy[enemyIndex]->ProcessDamage(kSYSTEM_DAMAGE_VALUE, true, (VECTOR *)&ZERO_VEC, (VECTOR *)&ZERO_VEC, false, eATTACK_TYPE_NORMAL); // 擬似的に敵を倒す
}
		
//---------------------------------------------------
/**
   BGモデルからプレイヤーの座標・角度を取得する
 */
//---------------------------------------------------
int
SceneMainGame::getItemNumFromMapData(void)
{
	int			bRet			= DXLIB_ERROR;
	int			map_mdl_hnd		= 0;
	USHORT		stage_item_num	= 0;
	USHORT		mat_idx			= 0;
	char*		mat_name		= NULL;
	USHORT		item_cnt		= 0;
	MV1_REF_POLYGONLIST ref_polylist;

	// マップのモデルハンドル取得
	map_mdl_hnd = sModelMap.GetModelHandle();

	// 参照用メッシュのセットアップ
	bRet = MV1SetupReferenceMesh(map_mdl_hnd, eBG_TYPE_ITEM, TRUE);
	APP_ASSERT((bRet == DXLIB_OK),
			   "Failed MV1SetupReferenceMesh() \n");

	// 参照用メッシュを取得する
	ref_polylist = MV1GetReferenceMesh(map_mdl_hnd, eBG_TYPE_ITEM, TRUE);

	// eBG_TYPE_ITEM のマテリアル数を取得する
	// eBG_TYPE_ITEM 内は全て正方形のポリゴンを想定しているので
	// 総ポリゴン数を２で割ったモノがマテリアル数とする
	stage_item_num = (ref_polylist.PolygonNum / 2);
	PRINT_CONSOLE("stage item num = %d \n", stage_item_num);

	for (int iI = 0; iI < stage_item_num; iI ++) {
		// マテリアル番号を取得し、
		// さらにマテリアル名を求めだす
		mat_idx = ref_polylist.Polygons[(iI * 2)].MaterialIndex;
		mat_name = (char *)MV1GetMaterialName(map_mdl_hnd, mat_idx);

		// マテリアル名がアイテム用のマテリアルかをチェックする
		if (strlen(mat_name) < BG_TYPE_ITEM_MATERIAL_NAME_LEN) {
			continue;
		}

		// マテリアル名から必要なラベルの文字列を取り出す
		// 命名規則として、 『TresuOOO_III_RRR』 とする 
		// OOO はオブジェクト種別(宝箱、壷...
		// III はアイテム種別(マスターソード、ハート...)
		// RRR は角度 (0 - 360)
		CHAR type_label	 [LABEL_TYPE_NAME_LEN + 1];

		memcpy(type_label, &mat_name[0], LABEL_TYPE_NAME_LEN);
		type_label[LABEL_TYPE_NAME_LEN] = '\0';

		// 指定のキーワードかを確認する
		if ((strcmp(type_label, BG_TYPE_ITEM_KEYWORD) == 0) ||
			//			(strcmp(type_label, "TRE") == 0)) {
			(memcmp(mat_name, "TRE", strlen("TRE")) == 0)) {
			
			// 生成用のカウンタをインクリメント
			item_cnt ++;
		}
	}
	return item_cnt ++;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::setupItemParameterFromMapData(void)
{
	int			bRet			= DXLIB_ERROR;
	int			map_mdl_hnd		= 0;
	USHORT		stage_item_num	= 0;
	USHORT		mat_idx			= 0;
	char*		mat_name		= NULL;
	USHORT		item_cnt		= 0;
	MV1_REF_POLYGONLIST ref_polylist;

	// マップのモデルハンドル取得
	map_mdl_hnd = sModelMap.GetModelHandle();

	// 参照用メッシュのセットアップ
	bRet = MV1SetupReferenceMesh(map_mdl_hnd, eBG_TYPE_ITEM, TRUE);
	APP_ASSERT((bRet == DXLIB_OK),
			   "Failed MV1SetupReferenceMesh() \n");

	// 参照用メッシュを取得する
	ref_polylist = MV1GetReferenceMesh(map_mdl_hnd, eBG_TYPE_ITEM, TRUE);

	// eBG_TYPE_ITEM のマテリアル数を取得する
	// eBG_TYPE_ITEM 内は全て正方形のポリゴンを想定しているので
	// 総ポリゴン数を２で割ったモノがマテリアル数とする
	stage_item_num = (ref_polylist.PolygonNum / 2);
	PRINT_CONSOLE("stage item num = %d \n", stage_item_num);

	for (int iI = 0; iI < stage_item_num; iI ++) {
		// マテリアル番号を取得し、
		// さらにマテリアル名を求めだす
		mat_idx = ref_polylist.Polygons[(iI * 2)].MaterialIndex;
		mat_name = (char *)MV1GetMaterialName(map_mdl_hnd, mat_idx);

		// マテリアル名がアイテム用のマテリアルかをチェックする
		if (strlen(mat_name) < BG_TYPE_ITEM_MATERIAL_NAME_LEN) {
			continue;
		}

		// 各ラベル名からアイテム情報を取得する
		int object_idx = 0;
		int hideevent_idx = 0;
		int item_idx = 0;
		int rot_value = 0;

		// 旧バージョンの指定
		if (memcmp(mat_name, BG_TYPE_ITEM_KEYWORD, strlen(BG_TYPE_ITEM_KEYWORD)) == 0) {
			int object_index= 0;
			sscanf(mat_name, "Tresu%d_%d_%d", 
				   &object_index, &item_idx, &rot_value);
			object_idx		= (object_index % 100);
			hideevent_idx	= (object_index / 100);
		}
		// 新バージョンの指定
		else {
			sscanf(mat_name, "TRE%d_%d_%d_%d", 
				   &hideevent_idx, &object_idx, &item_idx, &rot_value);
		}

		// オブジェクトを設置する場所を求めだす
		VECTOR center_pos = ZERO_VEC;
		int vtx_idx = 0;
		for (int iPolyCnt = 0; iPolyCnt < 2; iPolyCnt ++) {
			for (int iVtxCnt = 0; iVtxCnt < 3; iVtxCnt ++) {
				vtx_idx = ref_polylist.Polygons[(iI * 2) + iPolyCnt].VIndex[iVtxCnt];
				center_pos = VAdd(center_pos, ref_polylist.Vertexs[vtx_idx].Position);
//				PRINT_CONSOLE("poly[%d].vtx[%d] = %d : {%.2f, %.2f, %.2f} \n", iPolyCnt, iVtxCnt, vtx_idx, center_pos.x, center_pos.y, center_pos.z);
			}
		}
		center_pos = VScale(center_pos, (1.0f / 6.0f));
		center_pos = VScale(center_pos, spBGInfo->mScale);

		// 各オブジェクトに応じたインスタンスを生成 
		sppObjectBase[item_cnt] = ObjectBase::CreateInstance(object_idx, sStageIndex);

		APP_ASSERT(sppObjectBase[item_cnt] != NULL,
				   "Failed new ObjectBase \n");

		sppObjectBase[item_cnt]->Setup(object_idx, (eItemKind)item_idx, &center_pos, rot_value);
		
		//<! イベント後に発生させる宝箱のエントリー
		switch (object_idx) {
		case eOBJECT_KIND_TRESURE:
		case eOBJECT_KIND_SMALL_TRESURE:
		case eOBJECT_KIND_CRYSTAL:
		case eOBJECT_KIND_SEESAW:
		case eOBJECT_KIND_CARD_JK:
		case eOBJECT_KIND_STONE:
			if (hideevent_idx != 0) {
				int event_index = ConvertPlaceIndex_TO_EventDefine(sStageIndex);
				if (event_index != eEVENT_ERROR) {
					if (TotalInfo::GetInstance()->GetEventFlagEnable(event_index, hideevent_idx) == false) {
						((ObjectTresure *)sppObjectBase[item_cnt])->HideTreasure(hideevent_idx);
					}
				}
			}
			break;
		default:
			APP_ASSERT_PRINT((hideevent_idx == 0),
							 "Not Support HideEvent object index : %d \n", object_idx);
			break;
		};

		// 生成用のカウンタをインクリメント
		item_cnt ++;
	}

	//　参照用メッシュのセットアップ
	bRet = MV1TerminateReferenceMesh(map_mdl_hnd, eBG_TYPE_ITEM, TRUE);
	APP_ASSERT((bRet == DXLIB_OK),
			   "Failed MV1TerminateReferenceMesh() \n");
	return;
}

//---------------------------------------------------
/**
 * NPC とのイベント処理・だいたいアイテムと同じ
 */
//---------------------------------------------------
void
SceneMainGame::updateNpcEvent(void)
{
	CharaNpc* p_npc = NULL;

	VECTOR vecA, vecB;
	float len;
	vecA = sPlayer.Position();
	//	vecA.y = 0.0f;

	bool is_talk_enable = false;
	float player_body_size = sPlayer.BodySize();
	if ((sPlayer.GetState() != CharaBase::eCHARA_STATE_JUMP_UP) &&
		(sPlayer.GetState() != CharaBase::eCHARA_STATE_JUMP_DOWN)) {
		// 会話の距離の処理
		if (CHECK_TRIG_ANY_JOY()) {
			is_talk_enable = true;
		}
	}

	if (is_talk_enable) {
		for (int iI = 0; iI < sNpcNum; iI ++) {
			p_npc = &spNpc[iI];

			// メッセージの処理
			vecB = p_npc->Position();
			//			vecB.y = 0.0f;
			len = VSize(VSub(vecA, vecB) );
			APP_ASSERT_PRINT(len > 0.000f,
							 "Failed updateNpcEvent, player & npc pos is SamePosition\n");
			if (len < ((p_npc->BodySize() + player_body_size) * 1.2f) ) {
				sPlayer.PermitAcceptInput(false);
				CameraManager::GetInstance()->SaveTemporaryCamWork();

				// 全ての敵の動きを止める
				CharaEnemy::SetStopAllEnemyFlag(TRUE);

				mPhase = SceneMainGame::ePHASE_MESSAGE_BEFORE;
				sIsMessageRange = iI;
				break;
			}
		}
	}

	if (sNpcNum > 0) {

		p_npc = &spNpc[sCheckNpcIndex];
		// メッセージの処理
		vecB = p_npc->Position();
		//		vecB.y = 0.0f;
		len = VSize(VSub(vecA, vecB) );

		// 関係ない表示は非表示にさせる
		if (len > sDissappearLength) {
			GraphicManager::GetInstance()->RemoveDrawModel(p_npc);
			p_npc->DisableShadow();

		} else {
			GraphicManager::GetInstance()->EntryDrawModel(p_npc);
			p_npc->EnableShadow();
		}
		sCheckNpcIndex = ( (sCheckNpcIndex + 1) % sNpcNum);

	}	
}

//---------------------------------------------------
/**
 * オブジェクトと入力時の処理
 */
//---------------------------------------------------
void
SceneMainGame::updateObjectInputCheck(void)
{
	BOOL is_check_key = FALSE;
	BOOL has_key = TotalInfo::GetInstance()->GetHasFieldKey(sStageIndex);

	// キー入力のチェック
	if ((CHECK_TRIG_JOY(JOY_INPUT_AWAY)) ||
		(CHECK_TRIG_JOY(JOY_INPUT_ATTACK)) ) {
		int player_state = sPlayer.GetState();
		if ( (player_state != CharaBase::eCHARA_STATE_JUMP_UP) &&
			 (player_state != CharaBase::eCHARA_STATE_JUMP_DOWN) &&
			 (player_state != CharaPlayer::eCHARA_STATE_ATTACK_JUMP) &&
			 (sPlayer.CosplayIndex() != eWINMENU_COSPLAY_TITANIS) ) {
			is_check_key = true;
		}
	}

	VECTOR ch_pos = sPlayer.Position();
	VECTOR vecA = VGet(ch_pos.x, 0.0f, ch_pos.z);
	VECTOR vecB;
	float dist;

	if (is_check_key == TRUE) {

		float ch_size = sPlayer.BodySize();
		float ch_height = sPlayer.BodyHeight();

		std::vector<ObjectBase *>::iterator ite;
		for (ite = ObjectBase::GetActiveObjectListRef().begin(); ite != ObjectBase::GetActiveObjectListRef().end(); ite++) {
			vecB = (*ite)->Position();
			vecB.y = 0.0f;

			dist = VSize(VSub(vecA, vecB));
			APP_ASSERT_PRINT( (dist > 0.0f),
							  "Failed updateObjectBaseEvent, player & item pos is SamePosition\n");
			
			float total_size = ((ch_size + (*ite)->BodySize()) * 1.6f);
			float object_center = (*ite)->Position().y + ((*ite)->BodySize() * 0.5f);

			if ((dist < total_size) &&
				((ch_pos.y - ch_height) < object_center) && ((ch_pos.y + ch_height) > object_center) ) {

				if ( ((*ite)->GetObjectKind() == eOBJECT_KIND_TRESURE) && 
					 (has_key == FALSE) && (sStageIndex != 22) ) {
					// 鍵がないコメント
					StartNaviTalk(strNaviTalkNoKey);
				}
				else {
					// アイテム取得イベント発動
					(*ite)->StartCheckEvent();
				}
				break; // １つの宝のそばに他の宝は事実上ないので
			}
		}
	}
	


	// 遠方の関係ない表示を非表示にさせる
	int obj_num = ObjectBase::GetActiveObjectListRef().size();
	if (obj_num > 0) {
		ObjectBase* p_object = ObjectBase::GetActiveObjectListRef().at(sCheckObjectIndex % obj_num);
		ObjectBase::eObjectType obj_type = p_object->GetObjectType();
		vecB = p_object->Position();
		vecB.y = 0.0f;
	
		dist = VSize(VSub(vecA, vecB));
	
		if (dist > sDissappearLength) {
			if (obj_type == ObjectHand::eOBJECT_TYPE_HAND) {
				if ( ( (ObjectHand *)p_object)->State() == ObjectHand::eHAND_STATE_ON_EARTH) {
					GraphicManager::GetInstance()->RemoveDrawModel(p_object);
				} 
				else {
					// eOBJECT_TYPE_HAND でかつ eHAND_STATE_ON_EARTH 以外の場合は、遠方のモデルを非表示にしない
					// nop.
				}
			}
			else {
				GraphicManager::GetInstance()->RemoveDrawModel(p_object);
			}
		}
		else {
			p_object->InsertDrawModelSelf();
		}
		sCheckObjectIndex = ( (sCheckObjectIndex + 1) % obj_num);
	}

}

/*=====================================*

 *=====================================*/
//---------------------------------------------------
/**
 * オブジェクトフラグからオブジェクト使用状態を更新
 */
//---------------------------------------------------
/*static*/void
SceneMainGame::updateObjectUsedFromObjectFlag(void)
{
	// フィールド情報を基に使用済みアイテムの処理を行う
	unsigned long event_def = ConvertPlaceIndex_TO_EventDefine(sStageIndex);
	if (event_def != eEVENT_ERROR) {

		// 宝、スイッチ、アイテムのオブジェクトタイプのみのリストを作成する
		std::vector<ObjectBase*> save_obj_list;
		ObjectBase::eObjectType obj_type;
		for (int iI = 0; iI < sObjectBaseNum; iI ++) {
			obj_type = sppObjectBase[iI]->GetObjectType();
			if ((obj_type == ObjectBase::eOBJECT_TYPE_TREASURE)		||
				(obj_type == ObjectBase::eOBJECT_TYPE_SWITCH)		||
				(obj_type == ObjectBase::eOBJECT_TYPE_ITEM)) {
					
				save_obj_list.push_back(sppObjectBase[iI]);
			}
		}
		APP_ASSERT_PRINT((save_obj_list.size() < (sizeof(DWORD) * 8)),
						 "save_obj_list is over %d / 32 \n", save_obj_list.size());

		// 作成したリストとフラグから、使用済オブジェクトの設定をする
		unsigned long getflag = TotalInfo::GetInstance()->GetObjectFlagBit(event_def);
		for (unsigned int iI = 0; iI < save_obj_list.size(); iI ++) {
			if (getflag & (1 << iI)) {
				save_obj_list.at(iI)->SetUsed();	// 使用済みに設定する
			}
		}
			
		PRINT_CONSOLE("=>update object used From bit (obj_num : %d, getflag : 0x%08x) \n",
					  save_obj_list.size(), getflag);
	}
}

//---------------------------------------------------
/**
 * オブジェクト使用状態からオブジェクトフラグを更新
 */
//---------------------------------------------------
/*static*/void
SceneMainGame::setObjectFlagFromObjectUsed(void)
{
	DWORD event_index = ConvertPlaceIndex_TO_EventDefine(sStageIndex);
	if (event_index != eEVENT_ERROR) {

		// 宝、スイッチ、アイテムのオブジェクトタイプのみのリストを作成する
		std::vector<ObjectBase*> save_obj_list;
		ObjectBase::eObjectType obj_type;
		for (int iI = 0; iI < sObjectBaseNum; iI ++) {
			obj_type = sppObjectBase[iI]->GetObjectType();
			if ((obj_type == ObjectBase::eOBJECT_TYPE_TREASURE)		||
				(obj_type == ObjectBase::eOBJECT_TYPE_SWITCH)		||
				(obj_type == ObjectBase::eOBJECT_TYPE_ITEM)) {
					
				save_obj_list.push_back(sppObjectBase[iI]);
			}
		}
		APP_ASSERT_PRINT((save_obj_list.size() < (sizeof(DWORD) * 8)),
						 "save_obj_list is over %d / 32 \n", save_obj_list.size());

		// 作成したリストの関数からフラグの値を求める
		unsigned long setflag = 0;
		for (unsigned int iI = 0; iI < save_obj_list.size(); iI ++) {
			if (save_obj_list.at(iI)->IsUsed()) {
				setflag |= (1 << iI);
			}
		}
		TotalInfo::GetInstance()->SetObjectFlagBit(event_index, setflag);

		PRINT_CONSOLE("=>update bit From object used (obj_num : %d, setflag : 0x%08x) \n",
					  save_obj_list.size(), setflag);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::StartNaviTalk(const char* strTalk)
{
	SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
	MSG_TYPE_MESSAGE();
	MSG_FADE_IN(0.2f);
				
	sStrNaviTalkPtr = (char *)strTalk;
}

//---------------------------------------------------
/**
 * シャドウマップ描画前にコールされるハンドラ
 */
//---------------------------------------------------
/*static*/void
SceneMainGame::BeforeCreateShadowFunc(void* args)
{
	// プレイヤーのスフィアマップを解除
	sPlayer.ChangeSphereTexAll(-1);
	sGimmick.ChangeSphereTexAll(-1);

	EventManager* const p_evtmgr =  EventManager::GetInstance();
	for (int iI = 0; iI < p_evtmgr->GetModelNum(); iI ++) {
		p_evtmgr->GetModelPmd(iI)->ChangeSphereTexAll(-1);
	}
	for (int iI = 0; iI < sNpcNum; iI ++) {
		spNpc[iI].ChangeSphereTexAll(-1);
	}	
}

//---------------------------------------------------
/**
 * シャドウマップ描画後にコールされるハンドラ
 */
//---------------------------------------------------
/*static*/void
SceneMainGame::AfterCreateShadowFunc(void* args)
{
	// プレイヤーのスフィアマップを元に戻す
	sPlayer.RevertSphereTexAll();
	sGimmick.RevertSphereTexAll();

	EventManager* const p_evtmgr =  EventManager::GetInstance();
	for (int iI = 0; iI < p_evtmgr->GetModelNum(); iI ++) {
		p_evtmgr->GetModelPmd(iI)->RevertSphereTexAll();
	}
	for (int iI = 0; iI < sNpcNum; iI ++) {
		spNpc[iI].RevertSphereTexAll();
	}	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::attachSkydomeTexture(const char* texName)
{
	return;

	int lyt_handle = -1;

	ResourceLayout::GetInstance()->LoadLayoutResource((char *)texName);
	lyt_handle = ResourceLayout::GetInstance()->GetHandle(texName);
	MV1SetTextureGraphHandle(sModelXfile[eMD_SKYDOME].GetModelHandle(), 0, lyt_handle, FALSE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::Destroy(void)
{
	PRINT_CONSOLE("start SceneMainGame::destroy \n");

#ifdef _DEBUG
	/* 使用メモリーをダンプ */
	Utility::DumpUsingMemory();
#endif // _DEBUG

	CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_DUMMY);

	if (spSubSceneInst != NULL) {
		spSubSceneInst->Destroy();
		delete spSubSceneInst;
		spSubSceneInst = NULL;
	}

	// コリジョンの破棄
	PRINT_CONSOLE(" CollisionManager::Destroy() \n");
	CollisionManager::GetInstance()->Destroy();

	// モデルを描画から外す
	PRINT_CONSOLE(" GraphicManager::RemoveDrawModelAll() \n");
	GraphicManager::GetInstance()->RemoveDrawModelAll();
	
	// モデル(マップ描画前)を描画から外す
	PRINT_CONSOLE(" GraphicManager::RemoveDrawPreUpdateModelAll() \n");
	GraphicManager::GetInstance()->RemoveDrawPreUpdateModelAll();

	PRINT_CONSOLE(" GraphicManager::RemoveDrawMapModelAll() \n");
	GraphicManager::GetInstance()->RemoveDrawMapModelAll();

	// ナビォカーソルを無効にする
	sNavi.DisableNaviCursor();

	/*
	// レイアウトを描画から外す
	PRINT_CONSOLE(" GraphicManager::RemoveDrawLayout() \n");
	for (int iI = 0; iI < eLY_MAX; iI ++) {
		GraphicManager::GetInstance()->RemoveDrawLayout(&sLayout[iI]);
	}
	*/

	// モデルを当たり対象から外す
	PRINT_CONSOLE(" CollisionManager::RemoveBodyCollisionAll() \n");
	CollisionManager::GetInstance()->RemoveBodyCollisionAll();

	PRINT_CONSOLE(" CollisionManager::RemoveAttackCollisionAll() \n");
	CollisionManager::GetInstance()->RemoveAttackCollisionAll();

	// シャドウを描画から外す
	PRINT_CONSOLE(" GraphicManager::RemoveDrawShadowAll() \n");
	GraphicManager::GetInstance()->RemoveDrawShadowAll();

	// エフェクトの破棄
	//	PRINT_CONSOLE(" EffectManager::Destroy() \n");
	//	EffectManager::GetInstance()->Destroy();

	// エフェクトの全停止
	EffectManager::GetInstance()->StopAllSystem();


	PRINT_CONSOLE(" EffekseerManager::StopAll() \n");
	EffekseerManager::GetInstance()->StopAll();

	PRINT_CONSOLE(" CharaPlayer::DeleteModel() \n");
	sPlayer.DeleteModel();

	PRINT_CONSOLE(" CharaEnemy::DeleteModel() \n");
	if (sppEnemy != NULL) {
		for (int iI = 0; iI < kENEMY_ONCE_MAX; iI ++) {
			if (sppEnemy[iI] != NULL) {
				sppEnemy[iI]->DeleteModel();
				delete sppEnemy[iI];
			}
		}
		delete[] sppEnemy;
		sppEnemy = NULL;
	}
	
	PRINT_CONSOLE(" CharaNpc::DeleteModel() \n");
	if (spNpc != NULL) {
		for (int iI = 0; iI < sNpcNum; iI ++) {
			spNpc[iI].DeleteModel();
		}
		delete[] spNpc;
		spNpc = NULL;
	}
	//<! 明示的にコメントアウト
	// mNpcNum は テーブルの値を直接参照している為、
	// ０にするとテーブル自体を上書きしてしまうので０には設定しない
	//	sNpcNum = 0;

	PRINT_CONSOLE(" ObjectBase::DeleteModel() \n");
	if (sppObjectBase != NULL) {
		for (int iI = 0; iI < sObjectBaseNum; iI ++) {
			if (sppObjectBase[iI] != NULL) {
				sppObjectBase[iI]->DeleteModel();
				delete sppObjectBase[iI];
			}
		}
		delete[] sppObjectBase;
		sppObjectBase = NULL;
	}
	sObjectBaseNum = 0;

	PRINT_CONSOLE(" enemy item DeleteModel() \n");
	for (int iI = 0; iI < kENEMY_ITEM_NUM; iI ++) {
		sDropItem[iI].DeleteModel();
	}

	if (spShopItemLayout != NULL) {
		delete[] spShopItemLayout;
		spShopItemLayout = NULL;
	}

	if (spStageClearLayoutList != NULL) {
		delete[] spStageClearLayoutList;
		spStageClearLayoutList = NULL;
	}

	sEventCsv.DeleteFile();
	//	sItemCsv.DeleteFile();

	// モデルをアンロードする
	for (int iI = 0; iI < eMD_MAX; iI ++) {
		sModelXfile[iI].DeleteModel();
	}


	// ステージごとの情報を持っている場合は破棄する
	if (sTaskInfo.mTaskType != ActionTaskManager::eTASK_MOVE) {
		if (spStageFunc != NULL) {
			spStageFunc->DestroyBuf();
		}
	}

	// ステージ毎の情報を破棄
	if (spStageFunc != NULL) {
		delete spStageFunc;
		spStageFunc = NULL;
	}

	// ステージ情報の破棄
	sModelMap.DeleteModel();
	//<!	ResourceModel::GetInstance()->UnloadResource((char *)sResourceStageNameTable[spBGInfo->mFileIndex]);

	if (spSubModelMap != NULL) {
		for (unsigned int iI = 0; iI < sSubStageInfoList.size(); iI ++) {
			spSubModelMap[iI].DeleteModel();
			//<!			ResourceModel::GetInstance()->UnloadResource((char *)sResourceStageNameTable[sSubStageInfoList.at(iI).mFileIndex]);
		}
		delete[] spSubModelMap;
		spSubModelMap = NULL;
	}
	sSubStageInfoList.clear();

	sGimmick.DeleteModel();

	sNavi.DeleteModel();

	// ※メモリ容量、もしくは該当のステージに移動する前にリソースのメモリを解放する
	DWORD using_mem = Utility::GetUsingMemory();
	PRINT_CONSOLE("using_mem : %d MB \n", using_mem);
	if ((sStageIndex == 16) ||								// ステージが森の場合
		(using_mem > kCHECK_USING_MEMORY_ATTENTION_SIZE)) {	// 使用メモリ警告サイズ
		PRINT_CONSOLE("!!!! relase resource memory before change stage \n");
		ResourceModel::GetInstance()->Destroy();
	}

	for (int iI = 0; iI < sMapIconMove.mLayoutNum; iI ++) {
		GraphicManager::GetInstance()->RemoveDrawLayout(&sMapIconMove.mpLayout[iI]);
	}


	// ギミック情報を破棄する
	GimmickBase::DeleteAllGimmick();

	// フェードを有効にする
	FADE_BOARD()->SetAlpha(255);

	GraphicManager::GetInstance()->EntryDrawFontFunc(NULL);

	// シャドウマップ描画設定ハンドラ解除
	ShadowMap::SetBeforeCreateCallback(NULL);
	ShadowMap::SetAfterCreateCallback(NULL);

	PRINT_CONSOLE("end SceneMainGame::destroy \n");

	sStrNaviTalkPtr = NULL;

#ifdef _DEBUG
	/* 使用メモリーをダンプ */
	Utility::DumpUsingMemory();
#endif // _DEBUG
}

/*=====================================*
 * phase
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseLoadBefore(void)
{
	// 非同期読み込みを開始する
	//	SetUseASyncLoadFlag(TRUE);

	if (sStageIndex == -1) {
		sStageIndex = mDebugArg;
		sStagePlaceIndex = 0; // とりあえず０を設定
	}
	spBGInfo = (tBGInfo *)&sBGInfoTable[sStageIndex % kBG_INFO_NUM];

	// ステージ名リスト
	sSubStageInfoList.clear();

	// フォグを無効にする
	//	GraphicManager::GetInstance()->DisableFogPattern();

	// ナビォライトを無効にする
	sNavi.EnableNaviLight(FALSE);

	// NPCの設定はデフォルトの０に戻す
	CharaNpc::SetCsvSubNo(0);

	/* ステージ毎の処理をおこなう */
	if (spStageFunc != NULL) {
		delete spStageFunc;
		spStageFunc = NULL;
	}
	// Stagefunc のインスタンスを生成
	spStageFunc = StageFuncBase::CreateInstance(sStageIndex);
	APP_ASSERT_PRINT((spStageFunc != NULL),
					 "spStageFunc is NULL, %d \n", sStageIndex);

	// 各イベント毎の設定を行う
	spStageFunc->SetupBeforeLoad();

	sLoadStatus = 0;


	/**** 装備を整える ****/
	sMyWeaponList.clear();
	addMyWeapon(eITEM_KIND_NOWEAPON);
	for (int iI = 0; iI < eITEM_KIND_MAX; iI ++) {
		if (TotalInfo::GetInstance()->IsWeaponFlag((eItemKind)iI)) {
			addMyWeapon((eItemKind)iI);
		}
	}

	// 翼の唄は最初から覚えている。
	TotalInfo::GetInstance()->AddSongType(eSONG_TYPE_WING);

#ifdef _DEBUG
	/* ----------------------------------------------------------- */
	//<! デバッグ用
	addMyWeapon(eITEM_KIND_LONGHOOK);
	addMyWeapon(eITEM_KIND_BOOMERANG);
	addMyWeapon(eITEM_KIND_PORTION);
	addMyWeapon(eITEM_KIND_BOMB);
	addMyWeapon(eITEM_KIND_ARCHERY);

	addMyWeapon(eITEM_KIND_MAGIC_THUNDER);
	addMyWeapon(eITEM_KIND_MAGIC_FLAME);
	addMyWeapon(eITEM_KIND_MAGIC_TORNADE);
	//	addMyWeapon(eITEM_KIND_COPY_ROD);
	//	addMyWeapon(eITEM_KIND_PIKOHUM);
	addMyWeapon(eITEM_KIND_OCARINA);

	TotalInfo::GetInstance()->AddKeyItem(eKEYITEM_MASTERSWORD);
	TotalInfo::GetInstance()->AddKeyItem(eKEYITEM_LIB_SHIELD);
	TotalInfo::GetInstance()->AddKeyItem(eKEYITEM_ANGEL_WING);
	TotalInfo::GetInstance()->AddKeyItem(eKEYITEM_WALLET_A);
	TotalInfo::GetInstance()->AddKeyItem(eKEYITEM_BOOK);

	TotalInfo::GetInstance()->AddCosplay(eWINMENU_COSPLAY_FRES);
	TotalInfo::GetInstance()->AddCosplay(eWINMENU_COSPLAY_MASK);
	TotalInfo::GetInstance()->AddCosplay(eWINMENU_COSPLAY_LAPAN);
	TotalInfo::GetInstance()->AddCosplay(eWINMENU_COSPLAY_DABAH);

	TotalInfo::GetInstance()->AddSongType(eSONG_TYPE_SUN);
	TotalInfo::GetInstance()->AddSongType(eSONG_TYPE_WILDNESS);
	TotalInfo::GetInstance()->AddSongType(eSONG_TYPE_WING);
	TotalInfo::GetInstance()->AddSongType(eSONG_TYPE_HANDLING);

#endif // _DEBUG


#ifdef _DEBUG
	PRINT_CONSOLE("**** phaseLoadBefore : **** \n");

	PRINT_CONSOLE("loading log:\tstage name : %s \n", GetStageName(sStageIndex) );

	DWORD using_mem = Utility::GetUsingMemory();
	PRINT_CONSOLE("loading log:\t%d\t%s\t%.1f\t%d\t\t[load sec] [using mem] \n", 
				  -1, "start", 0.0f, using_mem );
	Counter::StartCounter(Counter::eCOUNT_TYPE_LOAD_ALL);

#endif // _DEBUG

	mPhase = SceneMainGame::ePHASE_LOADING;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseLoading(void)
{
	ResourceManager*	p_res_mgr	= ResourceManager::GetInstance();
	ResourceModel*		p_res_mdl	= ResourceModel::GetInstance();
	ResourceLayout*		p_res_lyt	= ResourceLayout::GetInstance();

	/* ロード中のファイルがあればリターン */
	if (GetASyncLoadNum() != 0) {
		return;
	}

#ifdef _DEBUG
	//	PRINT_CONSOLE("**** phaseLoading[%d] : start %s **** \n", sLoadStatus, strLoadStateName[sLoadStatus]);
	Counter::StartCounter(Counter::eCOUNT_TYPE_LOAD_EACH);
#endif 

	switch (sLoadStatus) {
	case eLOAD_PLAYER:
		p_res_mgr->LoadCharaResource(Name_Lino);
		SoundManager::GetVcComponent(Name_Lino)->SetVolumeScale(TotalInfo::GetInstance()->GetConfigVoiceVolumeScale());
		break;
		
	case eLOAD_ENEMY:
		for (int iI = 0; iI < GET_ENEMY_PATTERN_NUM(); iI ++) {
			for (int iJ = 0; iJ < kENEMY_ONCE_MAX; iJ ++) {
				eEnemyKind kind = GET_ENEMY_KIND(iI, iJ);
				if (kind == (eEnemyKind)-1) continue;
				p_res_mgr->LoadCharaResource((char *)GetEnemyWorkTable(kind)->mResourceName);

				LoadEnemyAttachmentModel(kind);
			}
		}
		break;

	case eLOAD_NPC:
		CharaNpc::SetupNpcResourceInfo(sStageIndex);
		sNpcNum = CharaNpc::GetNpcResourceNum();
		for (int iI = 0; iI < sNpcNum; iI ++) {
			p_res_mgr->LoadCharaResource(CharaNpc::GetNpcResourceName(iI));
		}
		break;

	case eLOAD_SKYDOME:
		// スカイドームとして割り当てるテクスチャ
		p_res_mdl->LoadModelResource("BG/Skydome/sky_day.mv1");
		sModelXfile[eMD_SKYDOME	].LinkModel("BG/Skydome/sky_day.mv1");
		{
			// マップローディングチェック
			char* map_name = p_res_mdl->GetResourceName("BG", 0);
			if (map_name != NULL) {
				PRINT_CONSOLE("Find BG Data = %s \n", map_name);
			}
		}
		break;

	case eLOAD_MAP:
		// ステージの読み込み
		ResourceModel::GetInstance()->LoadStageResource((char *)sResourceStageNameTable[spBGInfo->mFileIndex],
														spBGInfo->mIsExtData);
		break;

	case eLOAD_SUB_MAP:
		spSubModelMap = NULL;
		if (sSubStageInfoList.size() != 0) {
			spSubModelMap = new ModelMap[sSubStageInfoList.size()];
			APP_ASSERT_PRINT((spSubModelMap != NULL),
							 "Failed new ModelMap[%d] \n", sSubStageInfoList.size()) ;
			for (unsigned int iI = 0; iI < sSubStageInfoList.size(); iI ++) {
				ResourceModel::GetInstance()->LoadStageResource((char *)sResourceStageNameTable[sSubStageInfoList.at(iI).mFileIndex], 
																false);
			}
		}
		break;

	case eLOAD_SHOW_A_MAP:
		break;

	case eLOAD_OBJECT:
		// 常駐モデルのロード
		for (int iI = 0; iI < eRES_MDL_MAX; iI ++) {
			p_res_mdl->LoadItemResource(sResourceModelNameTable[iI]);
		}

		// 非常駐モデルのロード
		for (int iI = 0; iI < eRES_MDL_NON_RESI; iI ++) {
			// 該当のステージ番号か、対象アイテムを既に持っている場合、モデルをロードする
			if ((sStageIndex == sResourceModelTableNonResi[iI].mStageIndex) ||
				((sResourceModelTableNonResi[iI].mItemKind != -1) &&	(TotalInfo::GetInstance()->IsWeaponFlag(sResourceModelTableNonResi[iI].mItemKind))) ){
				p_res_mdl->LoadItemResource(sResourceModelTableNonResi[iI].mModelName);
			}
		}
		p_res_mgr->LoadCharaResource(NAME_Navi);
		p_res_mgr->LoadCharaResource(NAME_Mogu);

		sModelXfile[eMD_SWORD].LinkModel(RES_MDL_MASTERSWORD);
		sModelXfile[eMD_LOCKON].LinkModel(RES_MDL_LOCKON);
		sModelXfile[eMD_SHIELD].LinkModel(RES_MDL_LIB_SHIELD);

		// 全てのアイテムをあらかじめロード
		ObjectBase::LoadAllObjectBase();
		ObjectBase::SetGetItemHandler((GET_ITEM_HANDLER)updateGetItem);

		//ギミック関連をロードする
		GimmickBase::LoadAllGimmick(sStageIndex);
		break;

	case eLOAD_TEXTURE:
		// テクスチャの読み込み
		for (int iI = 0; iI < eRES_LYT_MAX; iI ++) {
			p_res_lyt->LoadLayoutResource(sResourceLayoutNameTable[iI]);
		}
		break;

	case eLOAD_SOUND:
		// サウンドを読み込み
		break;

	case eLOAD_EFFECT:
		{
			// エフェクト読み込み
			BOOL is_load_effect = EffectManager::GetInstance()->Create(eEF_MAX);
			if (is_load_effect == FALSE) {
				char model_name_buf[32] = {0};
				char* p_name;
				// 全てのエフェクトをロード
				for (int iI = 0; iI < eEF_MAX; iI ++) {
					if (iI == eEF_DRAG_DROP) continue;
					if (sEffectResourceTable[iI].mModelName == NULL) {
						p_name = NULL;
					}
					else {
						sprintf_s(model_name_buf, sizeof(model_name_buf), "ModelCommon/%s", sEffectResourceTable[iI].mModelName);
						p_name = model_name_buf;
					}
					EffectManager::GetInstance()->LoadResource(iI, sEffectResourceTable[iI].mShaderName, p_name);
				}
			}
		}

		{
			/* Effekseerのエフェクト一覧をロード */
			BOOL is_load_effkseer = EffekseerManager::GetInstance()->SetupResourceBufferNum(eEFK_MAX);
			if (is_load_effkseer == FALSE) {
				for (int iI = 0; iI < eEFK_MAX; iI ++) {
#ifdef _DEBUG
					if (iI == eEFK_DRAG_DROP_SAMPLE) continue;
#endif // _DEBUG
					EffekseerManager::GetInstance()->LoadResource(iI, (char *)sEffekseerResourceName[iI]);
				}
			}
		}
		break;

	default:
		APP_ASSERT_PRINT((NULL), "Invalid Load Status :%d \n", sLoadStatus);
		break;
		
	};
	
#ifdef _DEBUG
	Counter::StopCounter(Counter::eCOUNT_TYPE_LOAD_EACH);
	//	PRINT_CONSOLE(" * load time : %.1f sec\n", Counter::GetCountSec(Counter::eCOUNT_TYPE_LOAD_EACH));
	//	Utility::DumpUsingMemory();
	PRINT_CONSOLE("loading log:\t%d\t%s\t%.1f\t%u\t\t[load sec] [using mem] \n", 
				  sLoadStatus, 
				  strLoadStateName[sLoadStatus],
				  Counter::GetCountSec(Counter::eCOUNT_TYPE_LOAD_EACH), 
				  Utility::GetUsingMemory() );
#endif // _DEBUG

	sLoadStatus ++;

	if (sLoadStatus == eLOAD_MAX) {
		SetUseASyncLoadFlag(FALSE);

		/**** ロード終了 ****/
#ifdef _DEBUG
		Counter::StopCounter(Counter::eCOUNT_TYPE_LOAD_ALL);
		//		PRINT_CONSOLE("**** Finish Load All : %lld nsecd\n", Counter::GetCountSec(Counter::eCOUNT_TYPE_LOAD_ALL));
		PRINT_CONSOLE("loading log:\t**** Finish Load All : %f sec\n", Counter::GetCountSec(Counter::eCOUNT_TYPE_LOAD_ALL));
#endif 
		mPhase = SceneMainGame::ePHASE_SETUP;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseSetup(void)
{
	int before_time, after_time;
	LayoutBase* p_layout;

#ifdef _DEBUG
	GraphicManager::GetInstance()->DbgSetRenderString(false);
#endif // _DEBUG

	ActionTaskManager::GetInstance()->Setup(SceneMainGame::RequestChangeState);

	EventManager::GetInstance()->Setup();

	// グローバル変数初期化
	sIsBattle = false;
	sBattleIndex = -1;
	sCheckObjectIndex = 0;
	sCheckNpcIndex = 0;

	//	spCheckPointPtr = NULL;
	sPlayer.SetCheckPointPtr(NULL);		// チェックポイント情報をクリア

	sNextEventNo = -1;


	TotalInfo::GetInstance()->SetEnableWinMenu(TRUE);

	// for debug.
	// コスプレシステム
	TotalInfo::GetInstance()->AddCosplay(eWINMENU_COSPLAY_NONE);
	/* ----------------------------------------------------------- */

	sModelXfile[eMD_SKYDOME].Setup();
	sModelXfile[eMD_SKYDOME].Position()= ZERO_VEC;
	//	sModelXfile[eMD_SKYDOME].Scale()	= VGet(600.0f, 600.0f, 600.0f); // 以前までのスカイドーム値
	sModelXfile[eMD_SKYDOME].Scale()	= VGet(kSKYDOME_DEF_SCALE, kSKYDOME_DEF_SCALE, kSKYDOME_DEF_SCALE);

	// モデルマップのセットアップ
	sModelMap.LinkModel( sResourceStageNameTable[spBGInfo->mFileIndex] );
	sModelMap.Setup(spBGInfo->mScale);
	sModelMap.Position()= ZERO_VEC;
	sModelMap.Scale()	= VGet(spBGInfo->mScale, spBGInfo->mScale, spBGInfo->mScale);

	for (unsigned int iI = 0; iI < sSubStageInfoList.size(); iI ++) {
		spSubModelMap[iI].LinkModel( sResourceStageNameTable[sSubStageInfoList.at(iI).mFileIndex] );
		spSubModelMap[iI].Setup(spBGInfo->mScale);
		spSubModelMap[iI].Position()= ZERO_VEC;
		spSubModelMap[iI].Scale()	= VGet(spBGInfo->mScale, spBGInfo->mScale, spBGInfo->mScale);
	}

	sModelXfile[eMD_SWORD].Setup();
	//	MV1SetFrameVisible(sModelXfile[eMD_SWORD].GetModelHandle(), 2, FALSE); // 鞘を非表示にする
	sModelXfile[eMD_SWORD].SetVisible(false, 2); // 鞘を非表示にする
	sModelXfile[eMD_GETTED_ITEM].Setup();
	sModelXfile[eMD_LOCKON].Setup();
	sModelXfile[eMD_SHIELD].Setup();
	// 光のメッシュを非表示にする
	MV1SetMeshVisible(sModelXfile[eMD_SHIELD].GetModelHandle(), LIBSHIELD_LIGHT_MESH_IDX, FALSE);

	/**** コリジョンの設定 ****/
	CollisionManager::GetInstance()->Setup(&sModelMap, &spSubModelMap[0]);

	CollisionManager::GetInstance()->EntryEarthCollision(sModelMap.GetModelHandle(), eBG_TYPE_EARTH);
	CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_WALL);

	CollisionManager::GetInstance()->EntryEarthCollision(sModelMap.GetModelHandle(), eBG_TYPE_ALPHA_EARTH);
	CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_ALPHA_WALL);

	for (unsigned int iI = 0; iI < sSubStageInfoList.size(); iI ++) {
		if (sSubStageInfoList.at(iI).mIsCollision == true) {
			CollisionManager::GetInstance()->EntryEarthCollision(spSubModelMap[iI].GetModelHandle(), eBG_TYPE_EARTH);
			CollisionManager::GetInstance()->EntryWallCollision(spSubModelMap[iI].GetModelHandle(), eBG_TYPE_WALL);

			CollisionManager::GetInstance()->EntryEarthCollision(spSubModelMap[iI].GetModelHandle(), eBG_TYPE_ALPHA_EARTH);
			CollisionManager::GetInstance()->EntryWallCollision(spSubModelMap[iI].GetModelHandle(), eBG_TYPE_ALPHA_WALL);
		}
	}

	sNavi.LinkModel(NAME_Navi);
	sNavi.Setup();
	sNavi.Scale()		= VGet(7.0f, 7.0f, 7.0f);
	sNavi.StartMotion(0, true, 20.0f);
	sNavi.DisableShadow();

	VECTOR ch_pos;
	float ch_rot;
	bool ret;

	// 読み込んだモデルをリンクさせる
	before_time = GetNowCount();
	{
		sPlayer.LinkCharaResource(Name_Lino);
		sPlayer.Scale()		= CHARA_DEF_SCALE;

		// モデルデータからキャラの座標を求めだす
		if ((sStagePlaceIndex == 99) ||
			(sStagePlaceIndex == 999)) {
			// 移動させない！ (RevisePos は除く)
			sPlayer.Position() = VAdd(sPlayer.Position(), sReviseDiffPos);
		} 
		else {
			ret = getPlayerPosRotFromBGModel(&ch_pos, &ch_rot, sStagePlaceIndex);
			if (ret == false) {
				// 適当に配置
				sPlayer.Position() = VGet(500.0f, 20200.0f, 200.0f);
				sPlayer.Rotation().y = 0.0f;

			} 
			else {
				sPlayer.Position() = VGet(ch_pos.x + 10.0f, ch_pos.y, ch_pos.z + 10.0f);
				sPlayer.Rotation().y = ch_rot;

			}
		}
		sPlayer.Setup(false, sStageIndex);

		sPlayer.SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		//		sPlayer.SetDrawFunc((ModelBase::DRAW_FUNC)DrawUsamiFunc);		/* リノ専用の描画関数を設定 */
		GraphicManager::GetInstance()->EntryDrawModel(&sPlayer);	// 描画エントリー
		CollisionManager::GetInstance()->EntryBodyCollision(&sPlayer);		// 当たり判定エントリー
		CollisionManager::GetInstance()->EntryAttackCollision(&sPlayer);	// 当たり判定エントリー

		// 一度全てアタッチをはずす
		sPlayer.DeattachModelAll();

		/* マスターソードをアタッチ */
		sPlayer.AttachModel(&sModelXfile[eMD_SWORD], 
							ModelPmd::eATTACHED_MODEL_NORMAL,
							sPlayer.GetFrameIndexFromName("左中指１"));

		/* サブウェポンをアタッチ */
		sPlayer.AttachModel(&sGimmick, 
							ModelPmd::eATTACHED_MODEL_MOTION, 
							sPlayer.GetFrameIndexFromName("右中指１"));

		// 天使の翼をアタッチさせる
		sPlayer.SetupAttachAngelWing();

		/* 盾をアタッチ */
		sPlayer.AttachModel(&sModelXfile[eMD_SHIELD], 
							ModelPmd::eATTACHED_MODEL_NORMAL,
							sPlayer.GetFrameIndexFromName("右中指１"));

		// 現在装備しているサブウェポンを設定させる
		changeSubWeapon(sPlayer.ItemWork().mItemSubWeapon);

		/*===================================== *
		 * NPCの生成							*
		 *===================================== */
		if (spNpc == NULL) {
			spNpc = CharaNpc::CreateNpcInstance(spBGInfo->mScale);
		}

		/*=====================================*
		 * アイテムの生成
		 *=====================================*/
		sObjectBaseNum = getItemNumFromMapData(); /* アイテム数取得 */

		if (sObjectBaseNum != 0) {
			sppObjectBase = new ObjectBase*[sObjectBaseNum];
			APP_ASSERT(sppObjectBase != NULL, "Failed new ObjectBase\n");
			for (int iI = 0; iI < sObjectBaseNum; iI ++) {
				sppObjectBase[iI] = NULL;
			}
		}
		// 一括でパラメータ設定
		setupItemParameterFromMapData();

		/*=====================================*
		 * オブジェクトフラグからオブジェクト使用状態を更新
		 *=====================================*/
		updateObjectUsedFromObjectFlag();

		// チェック可能なオブジェクトの設定を行う
		// オブジェクトリストに以下の条件のポインタのみ追加する
		// ・CHECK_ENABLE の項目
		// ・未使用済みのオブジェクト
		ObjectBase::GetActiveObjectListRef().clear();
		
		int obj_kind = 0;
		for (int iI = 0; iI < sObjectBaseNum; iI ++) {
			obj_kind = sppObjectBase[iI]->GetObjectKind();
			if ((ObjectBase::GetObjectBaseParam(obj_kind)->mObjectCheck == ObjectBase::eOBJECT_CHECK_ENABLE) &&
				(sppObjectBase[iI]->IsUsed() == false) ) {
				ObjectBase::GetActiveObjectListRef().push_back(sppObjectBase[iI]);
			}
		}


		/*=====================================*
		 * ギミックの設定
		 *=====================================*/
		GimmickBase::SetupAllGimmick();

		/*=====================================*
		 * 敵キャラの生成						
		 *=====================================*/
		sppEnemy = new CharaEnemy*[kENEMY_ONCE_MAX];
		APP_ASSERT(sppEnemy != NULL, "Failed new CharaEnemy[]\n");
		for (int iI = 0; iI < kENEMY_ONCE_MAX; iI ++) {
			// 最初はダミーでとりあえず作っとく
			sppEnemy[iI] = new CharaEnemy;
			APP_ASSERT( (sppEnemy[iI] != NULL), 
						"Failed new CharaEnemy() \n");
		}

		// 全ての敵の動きを有効にする
		CharaEnemy::SetStopAllEnemyFlag(FALSE);
	}

	after_time = GetNowCount();
	PRINT_CONSOLE("PMD Link : %d mm second\n", after_time - before_time);

	/**** 描画エントリー ****/
	for (int iI = 0; iI < eMD_MAX; iI ++) {
		if (iI == eMD_GETTED_ITEM) continue;
		GraphicManager::GetInstance()->EntryDrawModel(&sModelXfile[iI]);
	}

	for (unsigned int iI = 0; iI < sSubStageInfoList.size(); iI ++) {
		GraphicManager::GetInstance()->EntryDrawMapModel(&spSubModelMap[iI]);
	}
	GraphicManager::GetInstance()->EntryDrawMapModel(&sModelMap);
	
	/**** マップのセットアップ ****/
	int mesh_idx = sModelMap.SearchMaterialName(eBG_TYPE_TASK, "FAL_000_000");
	if (mesh_idx != -1) {
		// FAL_000_000 のマテリアルのMin/Maxが見つかった場合
		VECTOR max_pos, min_pos;
		float view_angle = Utility::DegToRad(60.0f);
		VECTOR center_pos;
		float length;
		sModelMap.GetMeshMinPosition(&min_pos, mesh_idx);
		sModelMap.GetMeshMaxPosition(&max_pos, mesh_idx);
		max_pos.y = min_pos.y;

		center_pos = VScale(VAdd(max_pos, min_pos), 0.5f);
		length = abs(VSize(VSub(max_pos, min_pos)) * 0.5f);
		sShowMapWork.mIsZoom			= TRUE;
		sShowMapWork.mCamDist			= tan(view_angle) * length;

		float far_dist = (CameraManager::GetInstance()->GetFarClip());
		if (sShowMapWork.mCamDist > far_dist) {
			// でかすぎるマップは小さく写す
			//			sShowMapWork.mCamDist = far_dist;
			sShowMapWork.mCamDist *= 0.3f;
		}	

		sShowMapWork.mCamCenterPos		= center_pos;
	}
	else {
		sShowMapWork.mIsZoom			= TRUE;
		sShowMapWork.mCamDist			= 75000.0f;
		sShowMapWork.mCamCenterPos		= sPlayer.Position();
	}

	// 移動用のレイアウトの準備
	sMapIconMove.mLayoutNum = MV1GetFrameMeshNum(sModelMap.GetModelHandle(), eBG_TYPE_ROOF);

	APP_ASSERT_PRINT(sMapIconMove.mLayoutNum < (sizeof(sMapIconMove.mpLayout) / sizeof(sMapIconMove.mpLayout[0])),
					 "Get Layout Num is Over Capacity \n" );
	for (int iI = 0; iI < sMapIconMove.mLayoutNum; iI ++) {
		p_layout = &sMapIconMove.mpLayout[iI];
		p_layout->LinkLayout(RES_LYT_ICON_MAP_MOVE);
		p_layout->SetPositionByCentering(40, 440);
		p_layout->SetSize(16, 16);
		p_layout->SetAlpha(0);
	}
	for (int iI = 0; iI < sMapIconMove.mLayoutNum; iI ++) {
		p_layout = &sMapIconMove.mpLayout[iI];
		GraphicManager::GetInstance()->EntryDrawLayout(p_layout);
	}

#if 0
	/*=====================================*
	 * レイアウトの設定						*
	 *=====================================*/
	p_layout = &sLayout[eLY_LINO_FACE];
	p_layout->LinkLayout(RES_LYT_LINO_FACE_A);
	p_layout->SetPositionByCentering(80, 60);
	p_layout->SetSize(80, 80);
	p_layout->SetAlpha(255);

	/* サブウェポンのアイコン設定 */
	p_layout = &sLayout[eLY_SUB_WEAPON];
	/* LinkLayoutの処理は changeSubWeapon() 内で既に実行済み */
	p_layout->SetPositionByCentering(568, 64);
	p_layout->SetSize(80, 80);
	p_layout->SetAlpha(255);

	// メッセージ板
	float aspect_rate = GraphicManager::GetInstance()->GetAspectRate();
	p_layout = &sLayout[eLY_BALLOON];
	p_layout->LinkLayout(RES_LYT_BALLOON_MSG);
	p_layout->SetPositionByCentering(320, 384);
	p_layout->SetSize( (int)(630.0f / aspect_rate), 192);
	p_layout->SetAlpha(0);

	// Info
	p_layout = &sLayout[TotalInfo::eLY_INFO];
	p_layout->SetPositionByCentering(320, 240);
	p_layout->SetSize(512, 128);
	p_layout->SetAlpha(0);

	/* ＨＰゲージの枠 */
	p_layout = &sLayout[eLY_GAUGE_FRAME];
	p_layout->LinkLayout(RES_LYT_ICON_GAUGE_FRAME);
	p_layout->SetPositionByCentering(212, 440);
	p_layout->SetSize(256, 16);
	p_layout->SetAlpha(0);

	/* ＨＰゲージの線 */
	p_layout = &sLayout[eLY_GAUGE_LINE];
	p_layout->LinkLayout(RES_LYT_ICON_GAUGE_LINE);
	p_layout->SetPositionByCentering(212, 440);
	p_layout->SetSize(256, 16);
	p_layout->SetAlpha(0);

	/* ＨＰゲージの線 */
	p_layout = &sLayout[eLY_ENEMY_FACE];
	p_layout->LinkLayout(RES_LYT_FACE);
	p_layout->SetPositionByCentering(40, 440);
	p_layout->SetSize(64, 64);
	p_layout->SetAlpha(0);

	// マップを持っているか
	p_layout = &sLayout[eLY_HAS_MAP];
	p_layout->LinkLayout(RES_LYT_ITEM_RING_MAP);
	p_layout->SetPositionByCentering(590, 280);
	p_layout->SetSize(48, 48);
	p_layout->SetAlpha(220);

	// 鍵を持っているか
	p_layout = &sLayout[eLY_HAS_KEY];
	p_layout->LinkLayout(RES_LYT_ITEM_RING_BOSS_KEY);
	p_layout->SetPositionByCentering(590, 390);
	p_layout->SetSize(48, 48);
	p_layout->SetAlpha(220);

	/* マップ用の矢印 */
	p_layout = &sLayout[eLY_MAP_ARROW];
	p_layout->LinkLayout(RES_LYT_MAPARROW);
	p_layout->SetPositionByCentering(40, 440);
	p_layout->SetSize(48, 64);
	p_layout->SetAlpha(0);

	/* マップの表示アイコン */
	p_layout = &sLayout[eLY_MAP_POSITION];
	p_layout->LinkLayout(RES_LYT_ICON_MAP_LINO_FACE);
	p_layout->SetPositionByCentering(40, 440);
	p_layout->SetSize(32, 32);
	p_layout->SetAlpha(0);

	//	for (int iI = 0; iI < eLY_MAX; iI ++) {
	for (int iI = 0; iI < eLY_DEFAULT_DRAW_MAX; iI ++) {
		if (iI == TotalInfo::eLY_INFO) continue;
		GraphicManager::GetInstance()->EntryDrawLayout(&sLayout[iI]);
	}

	if (GraphicManager::GetInstance()->CheckEntrySystemLayout(&sLayout[TotalInfo::eLY_INFO]) == false) {
		GraphicManager::GetInstance()->EntrySystemLayout(&sLayout[TotalInfo::eLY_INFO]);
	}
#endif // 0

	TotalInfo::GetInstance()->SetActiveLockonEnemyName(TRUE);

	CameraManager::GetInstance()->SetPatternGameBegine();

	// sReviseDiffPosをクリア
	sReviseDiffPos = ZERO_VEC;

	// 入力禁止
	sPlayer.PermitAcceptInput(false);

	// 現在いるフィールドのスタートフラグを立てる
	TotalInfo::GetInstance()->SetEnableFieldInfoFlag(sStageIndex, kFIELD_INFO_START_BIT);

	// シャドウマップ描画設定ハンドラ登録
	ShadowMap::SetBeforeCreateCallback(BeforeCreateShadowFunc);
	ShadowMap::SetAfterCreateCallback(AfterCreateShadowFunc);

	mPhase = SceneMainGame::ePHASE_GAME_BEFORE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseGameBefore(void)
{
	// ブルームはシステムとして使用可
	RenderBloom::SetEnableSystemBloom(TRUE);

	// 色彩補正ともに有効に（ただし、メニュー設定が優先）
	TotalInfo::GetInstance()->SetEnableColorRevision(TRUE);

	sPlayer.SetPositionOnCollision();
	for (int iI = 0; iI < sNpcNum; iI ++) {
		spNpc[iI].SetPositionOnCollision();
		spNpc[iI].SetupOnceDrawShadow();
	}
	for (int iI = 0; iI < sObjectBaseNum; iI ++) {
		sppObjectBase[iI]->SetPositionOnCollision();
	}

	// ゲーム開始前に Far/Nearクリップを戻す
	CameraManager::GetInstance()->SetDefaultNearFarClip();

	// 画角を戻す
	CameraManager::GetInstance()->SetPerspectiveCamera(kDEFAULT_PERSPECTIVE_FOV);

	// コスプレ中なら変更させる
	if ((sPlayer.CosplayIndex() == eWINMENU_COSPLAY_TITANIS) ||
	    (sPlayer.CosplayIndex() == eWINMENU_COSPLAY_GUARDIAN)) {
		sPlayer.RevertCosplay();
	}
	if (sPlayer.CosplayIndex() != eWINMENU_COSPLAY_NONE) {
		TotalInfo::tWinMenuCosplayInfo* p_cosplay_info = TotalInfo::GetInstance()->GetCosplayInfo(sPlayer.CosplayIndex());
		SceneMainGame::ChangePlayerModel(p_cosplay_info->mModelName, FALSE);
	}

	// ロード後のマップ毎のセットアップ
	spStageFunc->SetupAfterLoad((int&)mPhase);
	StageFuncBase::UpdateLightInfo();

	// フォグのバックアップを取る
	GraphicManager::GetInstance()->BackupFogParam();

	if (mPhase == SceneMainGame::ePHASE_EVENT_BEFORE) {
		return;
	}

	// ナビォカーソルを有効にする
	sNavi.EnableNaviCursor();

	/****  再生のBGM ****/
	SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);

	VISIBLE_BATTLE_ICON();	// バトル用アイコン表示

	// フェードイン
	FADE_BOARD()->StartFade(0, 0.5f);

	sPlayer.PermitAcceptInput(true);
	mPhase = SceneMainGame::ePHASE_GAME;

	sModelXfile[eMD_SKYDOME].Rotation().y += 0.00025f;
	sModelXfile[eMD_SKYDOME].Position().x = sPlayer.Position().x;
	sModelXfile[eMD_SKYDOME].Position().z = sPlayer.Position().z;

	CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseGame(void)
{
#ifdef _DEBUG
	static int sEffIndex = -1;
	// イベントフラグの全クリア
	if (CHECK_TRIG_KEY(DBG_KEY_EVENT_CLEAR)) {
		TotalInfo::GetInstance()->ResetAllEventFlag();

		char message[32] = {0};
		VECTOR player_pos;
		player_pos = sPlayer.Position();
		sprintf_s(message, sizeof(message), "%.1f,%.1f,%.1f", player_pos.x, player_pos.y, player_pos.z);
		PRINT_CONSOLE("PlayerPos = %s\n", message);
	}

	if (CHECK_TRIG_KEY(eKEY_NUMPAD0)) {
		PRINT_CONSOLE("ReloadInformation() \n");
		GimmickBase::ReloadInformationAll(sStageIndex);
	}

	if (CHECK_TRIG_KEY(DBG_KEY_START_EVENT)) {

		double x, y;
		GetWindowSizeExtendRate(&x, &y);
		PRINT_CONSOLE("GetWindowSizeExtendRate(%lf, %lf \n", x, y);
		SetWindowSizeExtendRate(1.1, 1.1);
		GetWindowSizeExtendRate(&x, &y);
		PRINT_CONSOLE("GetWindowSizeExtendRate(%lf, %lf \n", x, y);
	}
#endif // _DEBUG

	/*=====================================*
	 * キャラ補正の処理
	 *=====================================*/
	float dist = VSize(sPlayer.Position());
#ifdef _DEBUG
	PRINT_SCREEN(GetColor(255, 255, 0), "dist : %.2f", dist);
#endif _DEBUG
	if (dist > ALL_MODEL_REVISE_DIST) {
		this->ReviseAllModel();
	}

	/*=====================================*S
	 * スカイドームの処理
	 *=====================================*/
	sModelXfile[eMD_SKYDOME].Rotation().y += 0.00025f;
	
	sModelXfile[eMD_SKYDOME].Position().x = sPlayer.Position().x;
	sModelXfile[eMD_SKYDOME].Position().z = sPlayer.Position().z;

	/*=====================================*
	 * カメラワーク
	 *=====================================*/
	ModelCollision* p_lockon = sPlayer.GetLockonChara();

	/*=====================================*
	 * ロックオン処理
	 *=====================================*/
	if (p_lockon != NULL) {
		float size = 1.5f;
		float hp_rate = (float)p_lockon->GetHitpoint() / (float)p_lockon->GetMaxHitpoint();

		int gauge_size = (int)((float)(256.0f * hp_rate));

		sModelXfile[eMD_LOCKON].Scale() = VGet(size, size, size);

		VECTOR lockonpos;
		p_lockon->GetLockonPosition(&lockonpos);
		sModelXfile[eMD_LOCKON].Position() = lockonpos;

		TotalInfo::GetInstance()->FadeinLockonEnemyLayout(gauge_size);
	}
	else {
		float size = 0.0f;
		sModelXfile[eMD_LOCKON].Scale() = VGet(size, size, size);

		TotalInfo::GetInstance()->FadeoutLockonEnemyLayout();
	}

	/*=====================================*
	 * 表情を切り替え
	 *=====================================*/
	bool is_encount = false;
	for (int iI = 0; iI < kENEMY_ONCE_MAX; iI ++) {
		if (sppEnemy[iI]->GetIsActive() == true) {
			is_encount = true;
			break;
		}
	}
	TotalInfo::GetInstance()->ChangePlayerFaceLayout(is_encount);

	/*=====================================*
	 * ステージ毎の処理
	 *=====================================*/
	spStageFunc->Update((int)mPhaseCounter);

	/*=====================================*
	 * NPCとの処理
	 *=====================================*/

	updateNpcEvent();

	/*=====================================*
	 * オブジェクトと入力時の処理
	 *=====================================*/
	updateObjectInputCheck();

	/*=====================================*
	 * アイテム処理
	 *=====================================*/
	if (spDisappearEnemy != NULL) {
		int rand_value = GetRand(100-1);
		int object_index;
		int percent;
		spDisappearEnemy->GetItemInfo(&object_index, &percent);
		if (rand_value < percent) {
			for (int iI = 0; iI < kENEMY_ITEM_NUM; iI ++) {
				if (sDropItem[iI].GetIsActive() == false) {
					if (spDisappearEnemy->GetIsActive() == true) {
						ObjectItem* p_item = &sDropItem[iI];
						VECTOR setpos = spDisappearEnemy->Position();
						p_item->Setup(object_index, (eItemKind)0, &spDisappearEnemy->Position(), 0);
						p_item->SetPositionOnCollision();
						p_item->SetRotationToTarget(&sPlayer.Position());
						break;
					}
				}
			}
		}
		spDisappearEnemy = NULL;
	}

	/*=====================================*
	 * イベントバトルの処理
	 *=====================================*/
	if (sIsBattle == true) {
		int active_enmy_num = 0;
		for (int iI = 0; iI < kENEMY_ONCE_MAX; iI ++) {
			if (sppEnemy[iI]->GetIsActive()) {
				active_enmy_num ++;
			}
		}
		if (active_enmy_num == 0) {
			// バトル用の壁、非表示
			CollisionManager::GetInstance()->RemoveWallCollision(sModelMap.GetModelHandle(),
																 eBG_TYPE_BATTLE_WALL);
			sModelMap.UnvisibleBattleBox();
			sPlayer.UnvisibleWeaponArrow();
			SoundManager::GetSndComponent()->PlaySound(eSE_CLEAR_BATTLEBOX);

			if (sEventBattleFlag != -1) {
				ActionTaskManager::tTaskInfo param;
				param.mTaskType = ActionTaskManager::eTASK_EVENT;
				param.mTaskValueA = BATTLEFLAG_TO_TASKVALUE_A(sEventBattleFlag);
				param.mTaskValueB = BATTLEFLAG_TO_TASKVALUE_B(sEventBattleFlag);
				RequestChangeState(&param);
				sEventBattleFlag = -1;

				sEventRecoveryFlag = -1;
			}
			sIsBattle = false;
		}
	}

	/*=====================================*
	 * マップ表示切替・サブウェポン切替
	 *=====================================*/
	int player_state = sPlayer.GetState();
	if ((player_state == CharaBase::eCHARA_STATE_IDLE) ||
		(player_state == CharaPlayer::eCHARA_STATE_SLEEP)) {
		if (CHECK_TRIG_JOY(JOY_INPUT_SHOW_MAP)) {

			if (TotalInfo::GetInstance()->GetHasFieldMap(sStageIndex)) {
				mPhase = SceneMainGame::ePHASE_SHOW_MAP;
			} else {
				StartNaviTalk(strNaviTalkNoMap);
			}
		}
		else if (CHECK_TRIG_JOY(JOY_INPUT_SELECT_SUB)) {
			if ((sPlayer.CosplayIndex() != eWINMENU_COSPLAY_TITANIS) &&
				(sPlayer.CosplayIndex() != eWINMENU_COSPLAY_GUARDIAN)) {
				if (sPlayer.IsSubWeaponUsing() == false) {

					spSubSceneInst = new SubSceneItemMenu();
					APP_ASSERT((spSubSceneInst != NULL),
							   "Failed new spSubSceneInst\n");
				
					((SubSceneItemMenu *)spSubSceneInst)->Create(sItemMenuDefault, sMyWeaponList.size());

					TotalInfo::GetInstance()->SetActiveLockonEnemyName(FALSE);

					mPhase = SceneMainGame::ePHASE_SELECT_WEAPON;
					CHECK_TRIG_JOY(JOY_INPUT_SELECT_SUB);
				}
			}
			else {
				SoundManager::GetSndComponent()->PlaySound(eSE_NG);
			}

		}
	}

	/*=====================================*
	 * ナビォのトーク処理
	 *=====================================*/
	if (sStrNaviTalkPtr != NULL) {
		phaseNaviTalk();
	}

	/*=====================================*
	 * F4でのリカバリー処理
	 *=====================================*/
	if (CHECK_TRIG_KEY(eKEY_F4)) {
		if (false) { //<! 体験版では０でもできることに
			// お金が足りない場合
			SoundManager::GetSndComponent()->PlaySound(eSE_NG);
		} else {

			if ((sPlayer.CosplayIndex() != eWINMENU_COSPLAY_TITANIS) &&
				(sPlayer.CosplayIndex() != eWINMENU_COSPLAY_GUARDIAN)) {

				sPlayer.PermitAcceptInput(false);
				// モデルのUpdateをとめる
				GraphicManager::GetInstance()->SetStopModeUpdate(true);

				SetVolumeMusic(128);
				SoundManager::GetSndComponent()->PlaySound(eSE_WARP);

				FADE_COL_WHITE();
				FADE_OUT(1.0f);
				mPhase = SceneMainGame::ePHASE_RECOVERY;
			} 
			else {
				SoundManager::GetSndComponent()->PlaySound(eSE_NG);
			}

		}
	}

	/*=====================================*
	 * ゲームオーバーの判定
	 *=====================================*/
	if (sPlayer.GetState() == CharaBase::eCHARA_STATE_RETIRE_END) {
		FADE_BOARD()->StartFade(200, 3.0f);
		SoundManager::GetInstance()->StopBGM();

		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_DUMMY);
		mPhase = SceneMainGame::ePHASE_GAMEOVER_BEFORE;
	}
#ifdef _DEBUG
	debugPramInput();
#endif // _DEBUG

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseShowMap(void)
{
	/* phaseShowMap の初期化 */
	if (EQUAL_PHASE_SECOND(0.0f)) {

		// ステージ毎の処理が必要な場合
		spStageFunc->ProcessShowMapBefore();

		sPlayer.PermitAcceptInput(false);

		/* レイアウト・モデルの表示/非表示 */
		HIDE_BATTLE_ICON();
		TotalInfo::GetInstance()->SetCommapssForShowMap();
		sModelXfile[eMD_SKYDOME	].SetVisible(false);

		for (int iI = 0; iI < sMapIconMove.mLayoutNum; iI ++) {
			sMapIconMove.mpLayout[iI].SetAlpha(255);
		}

		GraphicManager::GetInstance()->SetStopModeUpdate(true);

		/* カメラの値をセーブ */
		CameraManager::GetInstance()->SaveTemporaryCamWork();

		// カメラ位置はリノの真上に
		CameraManager::GetInstance()->Target() = sPlayer.Position();

		/* フォグの値を一時的に無効にする */
		GraphicManager::GetInstance()->PauseFog();

		/* マップの垂直の視線の場合は一時的にシャドウマップを解除する */
		ShadowMap::PauseShadowMap();

		/* システムとしてブルームを無効にする (※何故か表示がずれてしまうバグが発生する為) */
		RenderBloom::SetEnableSystemBloom(FALSE);

		sShowMapWork.mIsZoom = TRUE;

		TotalInfo::GetInstance()->SetActiveLockonEnemyName(FALSE);

		/* 上から見上げるカメラを設定する */
		return; // 意図的にリターン

	} // if (EQUAL_PHASE_SECOND(0.0f)) {


	/* 各十字キーに応じてカメラを移動させる */
	if (sShowMapWork.mIsZoom == TRUE) {
		static const float sMapCursorSpeed = 160.0f;
		if (CHECK_PRESS_JOY(JOY_INPUT_UP)) {
			CameraManager::GetInstance()->Target().z += sMapCursorSpeed;
		}
		else if (CHECK_PRESS_JOY(JOY_INPUT_DOWN)) {
			CameraManager::GetInstance()->Target().z -= sMapCursorSpeed;
		}
		if (CHECK_PRESS_JOY(JOY_INPUT_RIGHT)) {
			CameraManager::GetInstance()->Target().x += sMapCursorSpeed;
		}
		else if (CHECK_PRESS_JOY(JOY_INPUT_LEFT)) {
			CameraManager::GetInstance()->Target().x -= sMapCursorSpeed;
		}
	}

	// ファークリップを設定
	CameraManager::GetInstance()->SetFarClip(sShowMapWork.mCamDist*1.2f);

	if (sShowMapWork.mIsZoom) {
		CameraManager::GetInstance()->SetPatternDist(sShowMapWork.mCamDist*0.3f);
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_SHOW_MAP);

	} else {
		CameraManager::GetInstance()->SetPatternDist(
			sShowMapWork.mCamDist);
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_SHOW_MAP);

	}
	// 正射影カメラに設定
	CameraManager::GetInstance()->SetOrthoCamera( CameraManager::GetInstance()->GetCameraDist() );

	VECTOR screen_pos, world_pos;
	float aspect_rate = GraphicManager::GetInstance()->GetAspectRate();
	/* キャラの場所を２Dで表示する */
	world_pos = sPlayer.Position();
	screen_pos = ConvWorldPosToScreenPos(world_pos);
	screen_pos.x /= GraphicManager::GetInstance()->GetLayoutScaleRateX();
	screen_pos.y /= GraphicManager::GetInstance()->GetLayoutScaleRateY();

	LayoutBase* p_lyt_pos	= TotalInfo::GetInstance()->GetSystemLayout(TotalInfo::eLY_MAP_POSITION);
	LayoutBase* p_lyt_arrow = TotalInfo::GetInstance()->GetSystemLayout(TotalInfo::eLY_MAP_ARROW);

	p_lyt_pos->SetPositionByCentering((int)(screen_pos.x * aspect_rate), (int)screen_pos.y);
	p_lyt_arrow->SetPositionByCentering((int)(screen_pos.x * aspect_rate), (int)screen_pos.y);

	// 森の場合のみ、コンパスをおかしくする
	if (sStageIndex == 16) {
		p_lyt_arrow->Rotation() += 0.2f;
		if (p_lyt_arrow->Rotation() > (PHI_F * 2.0f)) {
			p_lyt_arrow->Rotation() -= (PHI_F * 2.0f);
		}
	}
	else {
		p_lyt_arrow->Rotation() = (sPlayer.Rotation().y + PHI_F);
	}

	// レイアウトを点滅させる
	if (((int)mPhaseCounter % 80) > 40) {
		p_lyt_pos->SetAlpha(0);
		p_lyt_arrow->SetAlpha(0);
	} else {
		p_lyt_pos->SetAlpha(255);
		p_lyt_arrow->SetAlpha(255);
	}

	/* イベントの場所にアイコンを表示 */
	const char* mat_name;
	int mesh_idx, mat_idx, iI;
	int model_handle = sModelMap.GetModelHandle();
	int mesh_num = MV1GetFrameMeshNum(model_handle, eBG_TYPE_ROOF);
	VECTOR max_pos, min_pos;
	LayoutBase* p_layout = NULL;
	int cnt_event = 0;
	int cnt_move = 0;

	for (iI = 0; iI < mesh_num; iI ++) {
		mesh_idx = MV1GetFrameMesh(model_handle, eBG_TYPE_ROOF, iI);
		mat_idx = MV1GetMeshMaterial(model_handle, mesh_idx) ;
		mat_name = MV1GetMaterialName(model_handle, mat_idx);
		p_layout = NULL; /* need */
		p_layout = &sMapIconMove.mpLayout[cnt_move];
		cnt_move ++;

		if (p_layout != NULL) {
			min_pos = MV1GetMeshMinPosition(model_handle, mesh_idx);
			min_pos = VScale(min_pos, sModelMap.Scale().x);
			max_pos = MV1GetMeshMaxPosition(model_handle, mesh_idx);
			max_pos = VScale(max_pos, sModelMap.Scale().x);
			world_pos = VScale(VAdd(max_pos, min_pos), 0.5f);
			world_pos = VAdd(world_pos, MV1GetFramePosition(model_handle, eBG_TYPE_ROOF));

			screen_pos = ConvWorldPosToScreenPos(world_pos);
			screen_pos.x /= GraphicManager::GetInstance()->GetLayoutScaleRateX();
			screen_pos.y /= GraphicManager::GetInstance()->GetLayoutScaleRateY();
			p_layout->SetPositionByCentering((int)(screen_pos.x * aspect_rate), (int)screen_pos.y);
			p_layout->SetAlpha(255);
		}
	}

	/* マップ表示終了判定 */
	if (CHECK_TRIG_JOY(JOY_INPUT_ATTACK) ||
		CHECK_TRIG_JOY(JOY_INPUT_AWAY) ||
		CHECK_TRIG_JOY(JOY_INPUT_JUMP) ||
		CHECK_TRIG_JOY(JOY_INPUT_SUB_WEAPON) ||
		CHECK_TRIG_JOY(JOY_INPUT_START) ||
		CHECK_TRIG_JOY(JOY_INPUT_SHOW_MAP)) {
		
		if (sShowMapWork.mIsZoom == TRUE) {
			// カメラ位置はリノの真上に
			CameraManager::GetInstance()->Target() = sPlayer.Position();
			sShowMapWork.mIsZoom = FALSE;
		}
		else {

			// 正射影カメラに設定
			CameraManager::GetInstance()->SetPerspectiveCamera(kDEFAULT_PERSPECTIVE_FOV);
			CameraManager::GetInstance()->LoadTemporaryCamWork();
			CameraManager::GetInstance()->Up() = VGet(0.0f, 1.0f, 0.0f);
			CameraManager::GetInstance()->SetDefaultNearFarClip();

			// ステージ毎の処理が必要な場合
			spStageFunc->ProcessShowMapAfter();

			sPlayer.PermitAcceptInput(true);

			VISIBLE_BATTLE_ICON();

			p_lyt_pos->SetAlpha(0);
			p_lyt_arrow->SetAlpha(0);

			for (int iI = 0; iI < sMapIconMove.mLayoutNum; iI ++) {
				sMapIconMove.mpLayout[iI].SetAlpha(0);
			}
			sModelXfile[eMD_SKYDOME	].SetVisible(true);

			GraphicManager::GetInstance()->SetStopModeUpdate(false);

			// カメラを戻す
			if (sPlayer.GetLockonChara() == NULL) {
				CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
			}
			else {
				CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_LOCKON);
			}
			/* フォグの値を一時的に有効にする */
			GraphicManager::GetInstance()->ResumeFog();

			/* 一時的にシャドウマップを解除を元のステータスに戻す */
			ShadowMap::ResumeShadowMap();

			/* システムとしてブルームを有効に戻す */
			RenderBloom::SetEnableSystemBloom(TRUE);

			TotalInfo::GetInstance()->SetActiveLockonEnemyName(TRUE);

			mPhase = SceneMainGame::ePHASE_GAME;
		}
	}

#ifdef _DEBUG
	debugPramInput();
#endif // _DEBUG
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseChangeStage(void)
{
	CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);

	if (FADE_BOARD()->IsFade() == false) {

		Destroy();
		sPlayer.PermitAcceptInput(true);
		mPhase = SceneMainGame::ePHASE_LOAD_BEFORE;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseFallDown(void)
{
	sPlayer.PermitAcceptInput(false);

	if (FADE_BOARD()->IsFade() == false) {
	
		VECTOR water_pos = ZERO_VEC;
		sModelMap.GetFramePosFromIndex(&water_pos, eBG_TYPE_WATER);
		float water_height;
		water_height = water_pos.y + sModelMap.WaterOffsetPos();

		// ギミックを全てロードしなおし
		GimmickBase::ReloadInformationAllFallDown(sStageIndex);

		//  リノモデルのギミックのOnNode情報をリセットする。
		INT8 col_index = sPlayer.GetOnCollisionIndex();
		GimmickBase* p_gimmick = NULL;
		p_gimmick = (GimmickBase *)GimmickBase::GetGimmickBaseFromEarthCollIndex(col_index);
		if (p_gimmick != NULL) {
			p_gimmick->ResetOnNodeInfo(&sPlayer);
		}

		if (sStageIndex == 39) {
			spStageFunc->SetPatternLight(0);
		}

		sPlayer.ProcessFallDown(water_height);

		// フェードイン
		FADE_BOARD()->StartFade(0, 0.75f);
		sPlayer.ChangeState(CharaPlayer::eCHARA_STATE_IDLE);
		sPlayer.PermitAcceptInput(true);
		sPlayer.SetActionLimit(spBGInfo->mActionLimit,
							   sPlayer.ItemWork().mItemSubWeapon);

		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);

		GimmickBase::SetGimmickPauseAttach(FALSE);

		// フェーズカウンターをリセットする
		mPhaseCounter = 0;

		mPhase = SceneMainGame::ePHASE_GAME;
	} 
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseRecovery(void)
{
	if (FADE_BOARD()->IsFade() == false) {
	
		// バトルボックスを解除する
		CollisionManager::GetInstance()->RemoveWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_BATTLE_WALL);
		sModelMap.UnvisibleBattleBox();
		sPlayer.UnvisibleWeaponArrow();
		/* 敵を全滅させる */
		for (int iI = 0; iI < kENEMY_ONCE_MAX; iI ++) {
			if (sppEnemy[iI]->GetIsActive() == true) {
				sppEnemy[iI]->ProcessDamage(kSYSTEM_DAMAGE_VALUE, true, &VGet(0.0f, 0.0f, 0.0f), (VECTOR *)&ZERO_VEC, false, eATTACK_TYPE_NORMAL); // 擬似的に敵を倒す
			}
		}

		// バトルインデックスフラグは元に戻す
		sBattleIndex = -1;
		// イベントバトルフラグも元に戻す
		sEventBattleFlag = -1;
	
		// 敵キャラ停止フラグ
		CharaEnemy::SetStopAllEnemyFlag(FALSE);

		// イベントフラグのキャンセル予約設定
		if (sEventRecoveryFlag != -1) {
			TotalInfo::GetInstance()->DisableEventFlag(BATTLEFLAG_TO_TASKVALUE_A(sEventRecoveryFlag),
													   BATTLEFLAG_TO_TASKVALUE_B(sEventRecoveryFlag));
		}

		// オブジェクト使用状態からオブジェクトフラグを更新
		setObjectFlagFromObjectUsed();

		//		// 移動中のフェードアウト時に呼ばれるハンドラ
		//		spStageFunc->NotifyMovePlaceWhileFadeout();

		GimmickBase::ReloadInformationAll(sStageIndex);

		//  リノモデルのギミックのOnNode情報をリセットする。
		INT8 col_index = sPlayer.GetOnCollisionIndex();
		GimmickBase* p_gimmick = NULL;
		p_gimmick = (GimmickBase *)GimmickBase::GetGimmickBaseFromEarthCollIndex(col_index);
		if (p_gimmick != NULL) {
			p_gimmick->ResetOnNodeInfo(&sPlayer);
		}

		// チェックポイントを通過していればその地点に戻る
		ObjectBase* p_check_point = sPlayer.GetCheckPointPtr();
		if (p_check_point != NULL) {
			sPlayer.Position() = p_check_point->Position();
			sPlayer.Position().x += 1.0f;		// 差分
			sPlayer.Rotation() = p_check_point->Rotation();
		}
		else {
			getPlayerPosRotFromBGModel(&sPlayer.Position(), &sPlayer.Rotation().y, sStagePlaceIndex);
		} 
		sPlayer.SetPositionOnCollision();

		sPlayer.ProcessRecovery();

		//<! リカバリー時にもこの関数を呼ぶので注意！
		spStageFunc->SetupAfterLoad((int&)mPhase);
		StageFuncBase::UpdateLightInfo();

		SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);

		SetVolumeMusic(255);

		// フェードイン
		FADE_COL_BLACK();
		FADE_IN(0.75f);
		//		FADE_BOARD()->StartFade(0, 0.75f);

		// モデルのUpdateを再開する
		GraphicManager::GetInstance()->SetStopModeUpdate(false);

		sPlayer.PermitAcceptInput(true);
		sPlayer.SetActionLimit(spBGInfo->mActionLimit,
							   sPlayer.ItemWork().mItemSubWeapon);

		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
		CameraManager::GetInstance()->SetPatternGameBegine();

		mPhase = SceneMainGame::ePHASE_GAME;
	} 
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseMessageBefore(void)
{
	APP_ASSERT(sIsMessageRange != -1, "sIsMessageRange is -1\n");

	CharaNpc* p_npc = &spNpc[sIsMessageRange];

	CameraManager::GetInstance()->SetPatternCharaBase((ModelCollision *)p_npc);
	CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_MESSAGE);

	sPlayer.SetRotationToTarget(&p_npc->Position());

	bool is_next = false;

	if (p_npc->GetOptionValue() == CharaNpc::eNPC_OPTION_NOT_TURN) {
		is_next = true;
	}
	else {
		VECTOR vec, tmp;
		vec = VSub(sPlayer.Position(), p_npc->Position());
		vec.y = 0.0f;
		float len = VSize(vec);
		tmp.x = sin(p_npc->Rotation().y) * -len;
		tmp.y = 0.0f;
		tmp.z = cos(p_npc->Rotation().y) * -len;
		float dot;
		dot = VDot(VNorm(vec), VNorm(tmp));
		tmp = VAdd(VScale(tmp, 6.0f), sPlayer.Position());
		p_npc->SetRotationToTarget(&tmp);

		if (dot > 0.999f) {
			is_next = true;
		}
	}

	if (is_next) {
		SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
		MSG_TYPE_MESSAGE();
		MSG_FADE_IN(0.3f);
		spNpc[sIsMessageRange].StartTalkMotion();
		mPhase = SceneMainGame::ePHASE_MESSAGE;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseMessage(void)
{
	APP_ASSERT(sIsMessageRange != -1, 
			   "sIsMessageRange is -1\n");
	CameraManager::GetInstance()->SetPatternCharaBase((ModelCollision *)&spNpc[sIsMessageRange]);
	CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_MESSAGE);

	if (MSG_IS_FADE() == true) {
		return;
	}

	spNpc[sIsMessageRange].RenderNpcMessage();

	if (CHECK_TRIG_ANY_JOY()) {
		SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
		MSG_FADE_OUT(0.4f);

		spNpc[sIsMessageRange].StartIdleMotion();

		CameraManager::GetInstance()->LoadTemporaryCamWork();
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
		mPhase = SceneMainGame::ePHASE_GAME;

		if (spNpc[sIsMessageRange].StartAfterTalkEvent() == false) {
			// 会話後イベントがない場合はキャラの禁則を解除する
			sPlayer.PermitAcceptInput(true);

			// 全ての敵の動きを有効にする
			CharaEnemy::SetStopAllEnemyFlag(FALSE);
		}
	}
}

/*=====================================*
 * EventFunc
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseEventBefore(void)
{
	/* イベント無視の判定処理 */
	bool is_ignore_event = false;
	bool is_change_camera = true;

	unsigned long event_kind = sTaskInfo.mTaskValueA;
	unsigned long event_flag = sTaskInfo.mTaskValueB;
	switch (event_kind) {

	case eEVENT_14:
		if (event_flag == eEVENT_14_ABOUT_SULT_MINE) {
			if (IS_END_EVT(eEVENT_35, eEVENT_35_AFTER_GET_GREEN_CRYSTAL) == false) {
				is_ignore_event = true;
			}
		}
		break;

	case eEVENT_29:
		if (event_flag == eEVENT_29_ABOUT_MARUIFORCE) {
			if (IS_END_EVT(eEVENT_29, eEVENT_29_COME_FRES) == false) {
				is_ignore_event = true;
			}
		}
		break;

	case eEVENT_36:
		if (event_flag == eEVENT_36_FALL_ONLY_BOSS) {
			if ((IS_END_EVT(eEVENT_36, eEVENT_36_BOSS_START) == true) &&
				(IS_END_EVT(eEVENT_36, eEVENT_36_BOSS_END) == false)) {
				//				is_ignore_event = true;
				CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_DUMMY);
				mPhase = SceneMainGame::ePHASE_FALL_DOWN;
				FADE_OUT(0.75f);
				SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_FALL_DOWN);
				return;
			}
			else {
				is_ignore_event = true;
			}
		}
		else if (event_flag == eEVENT_36_WARP_TREASUREROOM) {

			is_ignore_event = true;

			if ((sPlayer.CosplayIndex() != eWINMENU_COSPLAY_MASK) &&
				(sPlayer.IsDamage() == false)) {
				VECTOR damage_vec;
				damage_vec = VScale(VNorm(sPlayer.MoveVec()), -1.0f);
				sPlayer.ProcessDamage(2, false, &sPlayer.Position(), &damage_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
			}

		}
		break;

	case eEVENT_38:
		if (event_flag == eEVENT_38_WARP_OUT_WORLD) {
			// 土のクリスタルの場合
			if (IS_END_EVT(eEVENT_38, eEVENT_38_CRYSTAL_EARTH) == true) {
				if ((IS_END_EVT(eEVENT_38, eEVENT_38_CRYSTAL_EARTH) == false) ||
					(IS_END_EVT(eEVENT_38, eEVENT_38_TALK_YODEL) == false)) {
					is_ignore_event = true;
				}
			}
			// 森のクリスタルの場合
			else {
				if ((IS_END_EVT(eEVENT_38, eEVENT_38_CRYSTAL_FOREST) == false) ||
					(IS_END_EVT(eEVENT_38, eEVENT_38_TALK_SENTO) == false)) {
					is_ignore_event = true;
				}
			}
		}
		break;


	case eEVENT_39:
		if ((event_flag == eEVENT_39_GO_TO_OUTSIDE) ||
			(event_flag == eEVENT_39_GO_TO_INSIDE)) {			
			is_change_camera = false;
		}
		break;

	case eEVENT_40:
		if (event_flag == eEVENT_40_BATTLE_USIDERMAN) {
			if (IS_END_EVT(eEVENT_40, eEVENT_40_TORNADE_EVENT) == false) {
				is_ignore_event = true;
			}
		}
		else if (event_flag == eEVENT_40_MOVE_BABEL) {
			if (IS_END_EVT(eEVENT_40, eEVENT_40_WAKEUP_BABEL) == false) {
				is_ignore_event = true;
			}
		}
		else if (event_flag == eEVENT_40_BLOCK_TORNADE) {
			if ((IS_END_EVT(eEVENT_40, eEVENT_40_TORNADE_EVENT) == false) ||
				(IS_END_EVT(eEVENT_40, eEVENT_40_DISAPPER_TORNADE) == true)) {
				is_ignore_event = true;
			}
		}
		else if (event_flag == eEVENT_40_FALL_ONLY_BOSS) {
			if ((IS_END_EVT(eEVENT_40, eEVENT_40_BATTLE_USIDERMAN) == true) &&
				(IS_END_EVT(eEVENT_40, eEVENT_40_DISAPPER_TORNADE) == false)) {
				CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_DUMMY);
				mPhase = SceneMainGame::ePHASE_FALL_DOWN;
				FADE_OUT(0.75f);
				SoundManager::GetVcComponent(Name_Lino)->PlaySound(eVC_FALL_DOWN);
				return;
			}
			else {
				is_ignore_event = true;
			}
		}

		break;

	default:
		break;
	};

	if (is_ignore_event) {
		PRINT_CONSOLE("%s : ignore event \n", __FUNCTION__);
		mPhase = SceneMainGame::ePHASE_GAME;
		return;
	}

	sPlayer.PermitAcceptInput(false);

	// 一度座標を元に戻す
	PauseReviseAllModel(true);
	CameraManager::GetInstance()->Update();
	
	if (is_change_camera) {
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_DUMMY);
	}

	sEventPhase = 0;
	sEventCount = 0;

	sPlayer.DisableLockonChara();
	sPlayer.processCancelArchery();
	TotalInfo::GetInstance()->DisableLockonLayout();

	// ナビォカーソルを無効にする
	sNavi.DisableNaviCursor();
	
	if (IsLongEventFlag(event_kind, event_flag) == TRUE) {
		// イベント開始時は敵を全滅させる
		for (int iI = 0; iI < kENEMY_ONCE_MAX; iI ++) {
			if (sppEnemy[iI]->GetIsActive() == true) {
				sppEnemy[iI]->ProcessDamage(kSYSTEM_DAMAGE_VALUE, true, (VECTOR *)&ZERO_VEC, (VECTOR *)&ZERO_VEC, false, eATTACK_TYPE_NORMAL); // 擬似的に敵を倒す
			}
		}
	}
	else {
		// 全ての敵の動きを止める
		CharaEnemy::SetStopAllEnemyFlag(TRUE);
	}

	//	sStrNaviTalkPtr = NULL;

	mPhase = SceneMainGame::ePHASE_EVENT;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseEvent(void)
{
	unsigned long event_kind;
	unsigned long event_flag;
	event_kind = sTaskInfo.mTaskValueA;
	event_flag = sTaskInfo.mTaskValueB;

	int phase = sEventPhase;
	bool is_end_evnt = false;

	ULONG event_flag_value = TotalInfo::GetInstance()->GetEventFlagValue(event_kind);

	//	is_end_evnt = (this->*mEventFunc[event_kind][event_flag])();
	is_end_evnt = ExecuteEventFunc(event_kind, event_flag);

	EventManager::GetInstance()->Update();

	sEventCount ++;
	if (phase != sEventPhase) {
		sEventCount = 0;
	}

	if (is_end_evnt) {

		// 次のイベントが存在する場合は続けて遷移する
		if (sNextEventNo != -1) {
			
			sTaskInfo.mTaskValueB = sNextEventNo;
			sNextEventNo = -1;

			mPhase = SceneMainGame::ePHASE_EVENT_BEFORE;
			return;
		}

		FADE_COL_BLACK();
		FADE_IN(0.3f);
		MSG_FADE_OUT(0.3f);

		// 一度座標を元に戻す
		PauseReviseAllModel(false);

		// 画角を戻す
		CameraManager::GetInstance()->SetPerspectiveCamera(kDEFAULT_PERSPECTIVE_FOV);

		// イベント終了時に表示される宝箱があれば、表示される
		for (int iI = 0; iI < sObjectBaseNum; iI ++) {
			if ((sppObjectBase[iI]->GetObjectType() == ObjectBase::eOBJECT_TYPE_TREASURE) ||
				(sppObjectBase[iI]->GetObjectType() == ObjectBase::eOBJECT_TYPE_SWITCH)) {
				ObjectTresure* p_treasure = (ObjectTresure *)sppObjectBase[iI];
				if (p_treasure->GetHideEventIndex() == sTaskInfo.mTaskValueB) {
					p_treasure->DeHideTreasure();
					p_treasure->SetPositionOnCollision();
				}
			}
		}

		/* NPCは全て表示にさせる */
		for (int iI = 0; iI < sNpcNum; iI ++) {
			spNpc[iI].SetVisible(true);
		}

		/* ステージクリア以外のフェーズの場合 */
		if ((mPhase != SceneMainGame::ePHASE_STAGECLEAR) &&
			(mPhase != SceneMainGame::ePHASE_GAMEOVER_BEFORE)) {
			
			if (sStageIndex != 22) {	//<! Kitchen 以外はナビォカーソルを有効にする
				// ナビォカーソルを有効にする
				sNavi.EnableNaviCursor();
			}

			CameraManager::GetInstance()->SetPatternGameBegine();

			// 主人公の入力禁止を解除
			sPlayer.PermitAcceptInput(true);

			// 全ての敵の動きを有効にする
			CharaEnemy::SetStopAllEnemyFlag(FALSE);

			if (event_flag_value != TotalInfo::GetInstance()->GetEventFlagValue(event_kind) ) {
				CameraManager::GetInstance()->SetPatternGameBegine();
			}

			GimmickBase* p_gimmick = NULL;
			p_gimmick = (GimmickBase *)GimmickBase::GetGimmickBaseFromEarthCollIndex(sPlayer.GetOnCollisionIndex());
			if ( (p_gimmick != NULL) && (p_gimmick->GetKind() == eGIMMICK_KIND_TROLLEY_EX) ) {
				// トロッコに乗っている場合のみ、例外処理でカメラの挙動を戻さない。
				// ※トロッコ移動中のイベント用
				/* nothing to do. */
			}
			else {
				CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
			}

			mPhase = SceneMainGame::ePHASE_GAME;

		}
		// イベントフラグを立てる ⇒ それぞれの関数で立てるように変更 * @data   2012/08/11 15:20:36
		//		TotalInfo::GetInstance()->SetEventFlag(event_kind, event_flag);
	}

	/*=====================================*
	 * キャラ補正の処理
	 *=====================================*/
	/*
	float dist = VSize(sPlayer.Position());
#ifdef _DEBUG
	PRINT_SCREEN(GetColor(255, 255, 0), "dist : %.2f", dist);
#endif _DEBUG
	if (dist > ALL_MODEL_REVISE_DIST) {
		this->ReviseAllModel();
	}
	*/

	/*=====================================*
	 * スカイドームの処理
	 *=====================================*/
	sModelXfile[eMD_SKYDOME].Rotation().y += 0.00025f;
	sModelXfile[eMD_SKYDOME].Position().x = CameraManager::GetInstance()->Position().x;
	sModelXfile[eMD_SKYDOME].Position().z = CameraManager::GetInstance()->Position().z;

	/*=====================================*
	 * ステージ毎の処理
	 *=====================================*/
	spStageFunc->Update((int)mPhaseCounter);


#ifdef _DEBUG
	debugPramInput();
#endif // _DEBUG

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseGetItemBefore(void)
{
	CameraManager::GetInstance()->SaveTemporaryCamWork();
	CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_DUMMY);

	//	HIDE_BATTLE_ICON();
	//	TotalInfo::GetInstance()->HideBattleIcon();

	// 全ての敵の動きを止める
	CharaEnemy::SetStopAllEnemyFlag(TRUE);

	// アイテム取得のサブシーンクラスの作成
	spSubSceneInst = new SubSceneGetItem();
	APP_ASSERT((spSubSceneInst != NULL),
			   "Failed new SubSceneGetItem class \n");
	spSubSceneInst->Create();

	mPhase = SceneMainGame::ePHASE_GET_ITEM;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseGetItem(void)
{
	if (spSubSceneInst->Update() == true) {
		spSubSceneInst->Destroy();
		delete spSubSceneInst;
		spSubSceneInst = NULL;

		mPhase = SceneMainGame::ePHASE_GAME;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseGameOverBefore(void)
{
	//	LayoutBase* p_layout = SceneManager::GetInstance()->GetInfoLayout();
	LayoutBase* p_layout = SceneManager::GetInstance()->GetInfoLayout();

	if (FADE_BOARD()->IsFade() == false) {

		//		GraphicManager::GetInstance()->EntryDrawLayout(&sLayout[eLY_GAMEOVER]);
		p_layout->LinkLayout("Info_GameOver.png");
		p_layout->SetPositionByCentering(320, 240);
		p_layout->SetSize(512, 128);
		p_layout->SetAlpha(0);
		p_layout->StartFade(255, 1.0f);
		
		mPhase = SceneMainGame::ePHASE_GAMEOVER;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseGameOver(void)
{
	//	LayoutBase* p_layout = &sLayout[TotalInfo::eLY_INFO];
	LayoutBase* p_layout = SceneManager::GetInstance()->GetInfoLayout();

	if (p_layout->IsFade() == false) {

		//		if (CHECK_TRIG_ANY_JOY()) {

		// コンティニューかの質問
		GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_1, 
			(char*)"Continure？",
			GetColor(255, 255, 255));
		Utility::eQuestion qs;
		qs = Utility::ExecuteQuestion();
		if (qs != Utility::eQUESTION_NONE) {

			p_layout->SetAlpha(0);
			// バトルインデックスフラグは元に戻す
			sBattleIndex = -1;
			// イベントバトルフラグも元に戻す
			sEventBattleFlag = -1;
			// イベントフラグのキャンセル予約設定
			if (sEventRecoveryFlag != -1) {
				TotalInfo::GetInstance()->DisableEventFlag(BATTLEFLAG_TO_TASKVALUE_A(sEventRecoveryFlag),
														   BATTLEFLAG_TO_TASKVALUE_B(sEventRecoveryFlag));
			}

			// オブジェクト使用状態からオブジェクトフラグを更新
			setObjectFlagFromObjectUsed();
		}
		if (qs == Utility::eQUESTION_YES) {
			// リトライ

			sPlayer.SetHitpoint(sPlayer.GetMaxHitpoint());
			sPlayer.PermitAcceptInput(true);

			Destroy();
			mPhase = SceneMainGame::ePHASE_LOAD_BEFORE;

		} else if (qs == Utility::eQUESTION_NO) {

			TotalInfo::GetInstance()->HideBattleIcon();

			Destroy();

			SceneManager::GetInstance()->RequestChangeTask(eMENU_TITLE);
			this->EndScene();
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseStageclear(void)
{
	static const BYTE kLYT_STAGE_CLEAR_NUM = 5;

	static tLayoutSetting sStageClearLayoutTable[kLYT_STAGE_CLEAR_NUM] = {
		{RES_LYT_INFO_STAGE,		85, 360-320,	144, 72,		255,},
		{RES_LYT_INFO_1,			0xFF,0xFF,		72,	 72,		255,},
		{RES_LYT_INFO_MAP_1,		0xFF,0xFF,		288, 72,		255,},
		{RES_LYT_INFO_MAP_SUB_1,	510, 96,		384, 48,		255,},		// 表示内容によってX軸を下側で変更している
		//		{RES_LYT_INFO_FRAME,		320, 232,		640, 144,		255,},
		{RES_LYT_INFO_FRAME,		320, 232,		640, 1,		255,},
	};

	PRINT_SCREEN(GetColor(255, 255, 0), "mPhaseCounter = %.2f (%.2f) \n", mPhaseCounter, (mPhaseCounter/60.0f));

	LayoutBase* p_layout = NULL;

	if (EQUAL_PHASE_SECOND( 1.0f )) {

		// 常にアイコン非表示設定
		HIDE_BATTLE_ICON();

		// レイアウト情報作成
		if (spStageClearLayoutList == NULL) {
			spStageClearLayoutList = new LayoutBase[kLYT_STAGE_CLEAR_NUM];
		}
		APP_ASSERT((spStageClearLayoutList != NULL),
				   "Failed new spStageClearLayoutList \n");

		SoundManager::GetInstance()->StopBGM();

		sPlayer.PermitAcceptInput(true);		// 主人公の入力禁止を解除
		sPlayer.ChangeState(CharaPlayer::eCHARA_STATE_PUT_SWORD);

	}
	else if (EQUAL_PHASE_SECOND( 1.7f )) {

		VECTOR end_pos;
		end_pos = sPlayer.Position();
		end_pos.x += sin(sPlayer.Rotation().y) * -180.0f;
		end_pos.y += 100.0f;
		end_pos.z += cos(sPlayer.Rotation().y) * -180.0f;

		CameraManager::GetInstance()->StartMoveCamera(0.4f, 
													  &end_pos,
													  &CameraManager::GetInstance()->Target());


	}
	else if (EQUAL_PHASE_SECOND( 3.2f ) ) {

		VECTOR world, vec, cam_pos, cam_tar;
		const float mRate = 0.9f;

		cam_pos = CameraManager::GetInstance()->Position();
		cam_tar = CameraManager::GetInstance()->Target();
		vec = VSub(cam_tar, cam_pos);
		world = VAdd(cam_pos, VScale(vec, mRate) );
	
		EFK_HANDLE handle;
		handle = EffekseerManager::GetInstance()->Play(eEFK_STAGE_CLEAR,
													   &world,
													   50.0f);
		EffekseerManager::GetInstance()->SetRotationAxisY(handle, (atan2(vec.x, vec.z) ));
	}
	else if (EQUAL_PHASE_SECOND( 2.7f )) {

		//		FADE_BOARD()->StartFade(128, 0.6f);
		FADE_BOARD()->StartFade(64, 0.6f);
		SoundManager::GetSndComponent()->PlaySound(eSE_STAGE_CLEAR);

		//		p_layout = &sLayout[TotalInfo::eLY_INFO];
		p_layout = SceneManager::GetInstance()->GetInfoLayout();
		p_layout->LinkLayout(RES_LYT_INFO_STAGE_CLEAR);
		p_layout->SetPositionByCentering(320, 240);
		p_layout->SetSize(512, 1);
		p_layout->SetAlpha(0);
		p_layout->StartFade(255, 0.4f);

		float aspect_rate = GraphicManager::GetInstance()->GetAspectRate();
		const tLayoutSetting* p_setting = NULL;
		for (int iI = 0; iI < kLYT_STAGE_CLEAR_NUM; iI ++) {
			p_layout	= &spStageClearLayoutList[iI];
			p_setting	= &sStageClearLayoutTable[iI];

			p_layout->LinkLayout(p_setting->mStrLayoutName);
			p_layout->SetPositionByCentering(p_setting->mPosX, p_setting->mPosY);
			p_layout->SetSize((int)((float)p_setting->mSizeX / aspect_rate),
							  (int)((float)p_setting->mSizeY / aspect_rate) );
			p_layout->SetAlpha(p_setting->mAlpha);
			//			p_layout->StartFade(255, 0.4f);
			GraphicManager::GetInstance()->EntrySystemLayout(p_layout);
		}
		// ダンジョンに応じてレイアウト名を変更する
		spStageClearLayoutList[1].LinkLayout(sResourceLayoutNameTable[eRES_LYT_INFO_1			+ sDungeonIndex]);
		spStageClearLayoutList[2].LinkLayout(sResourceLayoutNameTable[eRES_LYT_INFO_MAP_1		+ sDungeonIndex]);
		spStageClearLayoutList[3].LinkLayout(sResourceLayoutNameTable[eRES_LYT_INFO_MAP_SUB_1	+ sDungeonIndex]);
		
		// 文字列が長い物は位置をずらす
		if ((sDungeonIndex == 1) ||	(sDungeonIndex == 3)) {
			spStageClearLayoutList[3].SetPositionByCentering(430, 96);
		}

		// 残り２つは各レイアウトのサイズに合わせてX軸を計算して配置する
		int pos_x1, pos_y1, size_x1;
		int pos_x2, pos_y2, size_x2;
		int size_y_tmp;
		spStageClearLayoutList[0].GetPositionByCentering(&pos_x1, &pos_y1);
		spStageClearLayoutList[0].GetSize(&size_x1, &size_y_tmp);
		size_x1 = (int)((float)size_x1 * aspect_rate);
		spStageClearLayoutList[1].GetSize(&size_x2, &size_y_tmp);
		size_x2 = (int)((float)size_x2 * aspect_rate);
		
		pos_x2 = pos_x1 + (size_x1 / 2) + 8;
		pos_y2 = pos_y1;
		spStageClearLayoutList[1].SetPositionByCentering(pos_x2, pos_y2);

		spStageClearLayoutList[2].GetSize(&size_x1, &size_y_tmp);
		size_x1 = (int)((float)size_x1 * aspect_rate);
		
		pos_x1 = pos_x2 + (size_x1 / 2) + 8;
		pos_y1 = pos_y2;
		spStageClearLayoutList[2].SetPositionByCentering(pos_x1, pos_y1);

	}
	else if (EQUAL_PHASE_SECOND(9.5f)) {
		FADE_IN(0.5f);
		//		p_layout = &sLayout[TotalInfo::eLY_INFO];
		p_layout = SceneManager::GetInstance()->GetInfoLayout();
		p_layout->StartFade(0, 0.5f);

		for (int iI = 0; iI < kLYT_STAGE_CLEAR_NUM; iI ++) {
			p_layout	= &spStageClearLayoutList[iI];
			p_layout->StartFade(0, 0.5f);
		}
	}
	else if (EQUAL_PHASE_SECOND(10.0f)) {
		
		for (int iI = 0; iI < kLYT_STAGE_CLEAR_NUM; iI ++) {
			p_layout	= &spStageClearLayoutList[iI];
			GraphicManager::GetInstance()->RemoveSystemLayout(p_layout);
		}

		if (spStageClearLayoutList != NULL) {
			delete[] spStageClearLayoutList;
			spStageClearLayoutList = NULL;
		}

		sPlayer.PermitAcceptInput(true);		// 主人公の入力禁止を解除
		sPlayer.ChangeState(CharaBase::eCHARA_STATE_IDLE);

		CameraManager::GetInstance()->SetPatternGameBegine();
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
		mPhase = SceneMainGame::ePHASE_GAME;

		VISIBLE_BATTLE_ICON();

		if (sEventBattleFlag != -1) {
			ActionTaskManager::tTaskInfo param;
			param.mTaskType = ActionTaskManager::eTASK_EVENT;
			param.mTaskValueA = BATTLEFLAG_TO_TASKVALUE_A(sEventBattleFlag);
			param.mTaskValueB = BATTLEFLAG_TO_TASKVALUE_B(sEventBattleFlag);
			RequestChangeState(&param);
			sEventBattleFlag = -1;

		}
		else {
			mPhase = SceneMainGame::ePHASE_GAME;

			//<! for debug
			//			SceneBase::EndScene(); // シーンを終了させる
		}
	}

	//	if (BETWEEN_PHASE_SECOND(2.7f, 9.5f)) {
	if (BETWEEN_PHASE_SECOND(3.7f, 9.5f)) {
		spStageClearLayoutList[4].AddSize(0, 4, 640, 144);
		
		p_layout = SceneManager::GetInstance()->GetInfoLayout();
		p_layout->AddSize(0, 4, 512, 128);
	}

#ifdef _DEBUG
	debugPramInput();
#endif // _DEBUG

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseShop(void)
{
	static Vec2 center = Vec2(320, 180);
	static const int kROT_COUNT = 30;

	LayoutBase* p_layout = NULL;
	float theta;
	char msg1[48] = {0};
	char msg2[48] = {0};

	static int sNowItemIndex = 0;
	static int sNextItemIndex = 0;
	static int sRotCnt = 0;
	static bool sIsRight = true;
	static float sIconDist = 0;


	int shop_index = sTaskInfo.mTaskValueA;

	const tShopInfo* p_info = NULL;
	p_info = GetShopInfoList(shop_index);
	APP_ASSERT((p_info != NULL),
			   "Failed GetShopInfoList()\n");

	const eItemKind*	p_shop		= NULL;
	int					layout_num	= 0;
	p_shop		= p_info->mpItemList;
	layout_num	= p_info->mItemNum;
	
	const tItemDetailInfo * p_item_detail = GetItemDetailInfo(p_shop[sNowItemIndex]);
	int item_money = p_item_detail->mPrice;

	TotalInfo* const p_total_info = TotalInfo::GetInstance();

	/**** 初期化処理 ****/
	if (EQUAL_PHASE_SECOND(0.0f)) {

		if (spShopItemLayout == NULL) {
			spShopItemLayout = new LayoutBase[layout_num];
		}
		for (int iI = 0; iI < layout_num; iI ++) {
			const tItemDetailInfo * p_item_detail = GetItemDetailInfo(p_shop[iI]);
			p_layout = &spShopItemLayout[iI];

			ResourceLayout::GetInstance()->LoadLayoutResource(p_item_detail->mLayoutName);
			p_layout->LinkLayout(p_item_detail->mLayoutName);
			p_layout->SetSize(80, 80);
			p_layout->SetAlpha(255);
			GraphicManager::GetInstance()->EntryDrawLayout(p_layout);
		}

		MSG_TYPE_TELOP();
		MSG_FADE_IN(0.3f);
		sNextItemIndex = 0;
		sNowItemIndex = layout_num / 2;
		sRotCnt = 0;
		sIconDist = 300.0f;
		mPhaseCounter = SEC2FRAME(21.0f);
		SoundManager::GetSndComponent()->PlaySound(eSE_ITEM_APPEAR);

		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_DUMMY);

		center.x = (int)(320.0f / GraphicManager::GetInstance()->GetAspectRate());
		center.y = 180;

		return;
	}

#define SHOP_STATE_BUY_CHECK		(1.0f)
#define SHOP_STATE_BUY_FINISH		(3.0f)
#define SHOP_STATE_NO_MONEY			(7.0f)
#define SHOP_STATE_EXIST_ITEM		(10.0f)
#define SHOP_STATE_END				(13.0f)
#define SHOP_STATE_NO_EMPTY_BIN		(16.0f)
#define SHOP_STATE_SELECT_ITEM		(21.0f)

	/*=====================================*
	 * 購入確認画面
	 *=====================================*/
	if (BETWEEN_PHASE_SECOND(SHOP_STATE_BUY_CHECK, (SHOP_STATE_BUY_CHECK + 1))) {
		// 購入確認画面
		mPhaseCounter = SEC2FRAME(SHOP_STATE_BUY_CHECK);

		sprintf_s(msg1, sizeof(msg1), "%sを買いますか？", p_item_detail->mDispName);
		sprintf_s(msg2, sizeof(msg2), "⇒ %dノレピー", item_money);
		RENDER_MESSAGE_COL("", msg1, msg2, "", "", GetColor(0, 0, 0));

		eItemKind select_item = p_shop[sNowItemIndex];

		Utility::eQuestion qs;
		qs = Utility::ExecuteQuestion(false);
		if (qs == Utility::eQUESTION_YES) {
			if (( p_total_info->IsWeaponFlag( select_item ) ) &&
				(!(select_item == eITEM_KIND_BIN_MAGENTA)) &&
				(!(select_item == eITEM_KIND_BIN_CYAN)) &&
				(!(select_item == eITEM_KIND_BIN_YELLOW)) &&
				(!(select_item == eITEM_KIND_BIN_FAIRY))) {
				
				//<! 既に購入アイテムを持っている場合
				SoundManager::GetSndComponent()->PlaySound(eSE_NG);
				mPhaseCounter = SEC2FRAME(SHOP_STATE_EXIST_ITEM);
			}
			else if ( 
					 ((select_item == eITEM_KIND_BIN_MAGENTA)	||
					  (select_item == eITEM_KIND_BIN_CYAN)		||
					  (select_item == eITEM_KIND_BIN_YELLOW)	||
					  (select_item == eITEM_KIND_BIN_FAIRY) ) &&
					 (isEmptyBinContents() == FALSE)) {
				//<! 薬を購入したが、空き瓶が存在しない場合
				SoundManager::GetSndComponent()->PlaySound(eSE_NG);
				mPhaseCounter = SEC2FRAME(SHOP_STATE_NO_EMPTY_BIN);
			}
			else {
				if (p_total_info->GetNowMoney() >= item_money) {
					p_total_info->AddMoney(-item_money);
					//<! まいどのステータスへ
					updateGetItem( select_item );
					SoundManager::GetSndComponent()->PlaySound(eSE_DECIDE_KEY);
					mPhaseCounter = SEC2FRAME(SHOP_STATE_BUY_FINISH);
				} else {
					//<! お金が足りないステータスへ
					SoundManager::GetSndComponent()->PlaySound(eSE_NG);
					mPhaseCounter = SEC2FRAME(SHOP_STATE_NO_MONEY);
				}
			}

		}
		else if (qs == Utility::eQUESTION_NO) {
			//<! キャンセル選択
			SoundManager::GetSndComponent()->PlaySound(eSE_CANCEL);
			mPhaseCounter = SEC2FRAME(SHOP_STATE_SELECT_ITEM);
		}
		return;
	}

	/*=====================================*
	 * まいどのステータス
	 *=====================================*/
	if (BETWEEN_PHASE_SECOND(SHOP_STATE_BUY_FINISH, (SHOP_STATE_BUY_FINISH + 2.0f))) {
		/**** 買い物成功の音 ****/
		sprintf_s(msg1, sizeof(msg1), "%sをてにいれた！", p_item_detail->mDispName);
		RENDER_MESSAGE_COL("", msg1, "", "", "", GetColor(0, 0, 0));
		return;
	}
	if (BETWEEN_PHASE_SECOND((SHOP_STATE_BUY_FINISH + 2.0f), (SHOP_STATE_BUY_FINISH + 3.0f))) {
		// 無理くりアップデートにシーケンスを戻す
		mPhaseCounter = SEC2FRAME(SHOP_STATE_SELECT_ITEM);
		return;
	}

	/*=====================================*
	 * お金がたりない場合のシーケンス
	 *=====================================*/
	if (BETWEEN_PHASE_SECOND((SHOP_STATE_NO_MONEY), (SHOP_STATE_NO_MONEY + 1.0f))) {
		/**** 買い物失敗の音 ****/
		RENDER_MESSAGE_COL("", "お金が足りない・・・だと・・・！？", "", "", "", GetColor(0, 0, 0));
		return;
	}
	if (BETWEEN_PHASE_SECOND((SHOP_STATE_NO_MONEY + 1.0f), (SHOP_STATE_NO_MONEY + 2.0f))) {
		// 無理くりアップデートにシーケンスを戻す
		mPhaseCounter = SEC2FRAME(SHOP_STATE_SELECT_ITEM);
	}

	/*=====================================*
	 * 既に持ち物があるばあい
	 *=====================================*/
	if (BETWEEN_PHASE_SECOND((SHOP_STATE_EXIST_ITEM), (SHOP_STATE_EXIST_ITEM + 1.0f))) {
		/**** 買い物失敗の音 ****/
		RENDER_MESSAGE_COL("", "このアイテムはもう持てないよ", "", "", "", GetColor(0, 0, 0));
		return;
	}
	if (BETWEEN_PHASE_SECOND((SHOP_STATE_EXIST_ITEM + 1.0f), (SHOP_STATE_EXIST_ITEM + 2.0f))) {
		// 無理くりアップデートにシーケンスを戻す
		mPhaseCounter = SEC2FRAME(SHOP_STATE_SELECT_ITEM);
	}

	/*=====================================*
	 * 空きビンが無い場合
	 *=====================================*/
	if (BETWEEN_PHASE_SECOND((SHOP_STATE_NO_EMPTY_BIN), (SHOP_STATE_NO_EMPTY_BIN + 1.0f))) {
		/**** 買い物失敗の音 ****/
		RENDER_MESSAGE_COL("", "空きビンがないとこれは持てないよ", "", "", "", GetColor(0, 0, 0));
		return;
	}
	if (BETWEEN_PHASE_SECOND((SHOP_STATE_NO_EMPTY_BIN + 1.0f), (SHOP_STATE_NO_EMPTY_BIN + 2.0f))) {
		// 無理くりアップデートにシーケンスを戻す
		mPhaseCounter = SEC2FRAME(SHOP_STATE_SELECT_ITEM);
	}
	
	/*=====================================*
	 * アイテム屋終了のシーケンス
	 *=====================================*/
	if (BETWEEN_PHASE_SECOND((SHOP_STATE_END), (SHOP_STATE_END + 1.0f))) {
		for (int iI = 0; iI < layout_num; iI ++) {
			GraphicManager::GetInstance()->RemoveDrawLayout(&spShopItemLayout[iI]);
		}
		if (spShopItemLayout != NULL) {
			delete[] spShopItemLayout;
			spShopItemLayout = NULL;
		}
		MSG_FADE_OUT(0.3f);

		// 主人公の入力禁止を解除
		sPlayer.PermitAcceptInput(true);

		// 全ての敵の動きを有効にする
		CharaEnemy::SetStopAllEnemyFlag(FALSE);

		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
		mPhase = SceneMainGame::ePHASE_GAME;
		return;
	}


	/**** アップデート処理 ****/
	/**** ここからは選択時間の処理 ****/
	sIconDist -= 20.0f;
	if (sIconDist < 100.0f) {
		sIconDist = 100.0f;
	}

	if (sRotCnt == 0) {
		if (CHECK_TRIG_JOY(JOY_INPUT_LEFT)) {
			sIsRight = false;
			sNextItemIndex = SELECT_ROTATION_ADD(sNextItemIndex, layout_num);
			SoundManager::GetSndComponent()->PlaySound(eSE_SELECT_KEY);
		}
		if (CHECK_TRIG_JOY(JOY_INPUT_RIGHT)) {
			sIsRight = true;
			sNextItemIndex = SELECT_ROTATION_SUB(sNextItemIndex, layout_num);
			SoundManager::GetSndComponent()->PlaySound(eSE_SELECT_KEY);
		}
	}

	float div_rad = Utility::DegToRad(360.0f / (float)layout_num);
	if (sNowItemIndex == sNextItemIndex) {
		for (int iI = 0; iI < layout_num; iI ++) {
			p_layout = &spShopItemLayout[iI];
			theta = (iI - sNowItemIndex) * div_rad;
			p_layout->SetPosition(center.x - (int)(sin(theta) * sIconDist * 1.5f),
								  center.y - (int)(cos(theta) * sIconDist));
		}
		sRotCnt = 0;
	} else {
		for (int iI = 0; iI < layout_num; iI ++) {
			p_layout = &spShopItemLayout[iI];

			float now_theta, next_theta;
			now_theta = (iI - sNowItemIndex) * div_rad;
			next_theta = (iI - sNextItemIndex) * div_rad;
			float offset_theta = next_theta - now_theta;
			if ((sIsRight == false) && (offset_theta > 0)) {
				offset_theta += -Utility::DegToRad(360.0f);
			}
			if ((sIsRight == true) && (offset_theta < 0)) {
				offset_theta += Utility::DegToRad(360.0f);
			}
			theta = now_theta + (offset_theta / kROT_COUNT) * sRotCnt;
			p_layout->SetPosition(center.x - (int)(sin(theta) * sIconDist * 1.5f),
								  center.y - (int)(cos(theta) * sIconDist));
			sRotCnt ++;
			if (sRotCnt >= kROT_COUNT) {
				sNowItemIndex = sNextItemIndex;
			}
		}
	}

	sprintf_s(msg1, sizeof(msg1), "%s ： %dノレピー", p_item_detail->mDispName, item_money);
	RENDER_MESSAGE_COL("", msg1, 
					   p_item_detail->mExplain0,
					   p_item_detail->mExplain1,
					   p_item_detail->mExplain2,
					   GetColor(0, 0, 0));

	if ((CHECK_TRIG_JOY(JOY_INPUT_ATTACK)) ||
		(CHECK_TRIG_JOY(JOY_INPUT_AWAY))) {
			
		// 購入確認画面へ
		SoundManager::GetSndComponent()->PlaySound(eSE_PRESS_KEY);
		mPhaseCounter = SEC2FRAME(SHOP_STATE_BUY_CHECK);
	}

	/**** 終了処理 ****/
	if (CHECK_TRIG_JOY(JOY_INPUT_JUMP)) {
		mPhaseCounter = SEC2FRAME(SHOP_STATE_END);
		SoundManager::GetSndComponent()->PlaySound(eSE_CANCEL);
		
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseSelectWeapon(void)
{
	if (spSubSceneInst->Update() == true) {
		((SubSceneItemMenu *)spSubSceneInst)->SetDefMenuKind(sItemMenuDefault);
		spSubSceneInst->Destroy();
		// <! タイミングの都合上、ここでは delete しない！

		mPhase = SceneMainGame::ePHASE_SELECT_WEAPON_END;
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseSelectWeaponEnd(void)
{
	if (MSG_IS_FADE() == false) {
		sPlayer.PermitAcceptInput(true);		// 主人公の入力禁止を解除

		TotalInfo::GetInstance()->SetActiveLockonEnemyName(TRUE);

		eItemKind selected_weapon = ((SubSceneItemMenu *)spSubSceneInst)->GetSelectWeapon();
		switch (selected_weapon) {
		case eITEM_KIND_PORTION:
			if (sPlayer.CosplayIndex() == 0) {
				sPlayer.ChangeState(CharaPlayer::eCHARA_STATE_GUTS_POSE);
				sPlayer.SetUsingRecoverItemKind(selected_weapon);
				deleteMyWeapon(selected_weapon);
			}else {	SoundManager::GetSndComponent()->PlaySound(eSE_NG);	}
			break;
			
		case eITEM_KIND_BIN_MAGENTA:
			if (sPlayer.CosplayIndex() == 0) {
				sPlayer.ChangeState(CharaPlayer::eCHARA_STATE_GUTS_POSE);
				sPlayer.SetUsingRecoverItemKind(selected_weapon);
				useBinContents(eITEM_KIND_BIN_MAGENTA);
			}else {	SoundManager::GetSndComponent()->PlaySound(eSE_NG);	}
			break;

		case eITEM_KIND_BIN_CYAN:
			if (sPlayer.CosplayIndex() == 0) {
				sPlayer.ChangeState(CharaPlayer::eCHARA_STATE_GUTS_POSE);
				sPlayer.SetUsingRecoverItemKind(selected_weapon);
				useBinContents(eITEM_KIND_BIN_CYAN);
			}else {	SoundManager::GetSndComponent()->PlaySound(eSE_NG);	}
			break;

		case eITEM_KIND_BIN_YELLOW:
			if (sPlayer.CosplayIndex() == 0) {
				sPlayer.ChangeState(CharaPlayer::eCHARA_STATE_GUTS_POSE);
				sPlayer.SetUsingRecoverItemKind(selected_weapon);
				useBinContents(eITEM_KIND_BIN_YELLOW);
			}else {	SoundManager::GetSndComponent()->PlaySound(eSE_NG);	}
			break;


		case eITEM_KIND_MAGIC_THUNDER:
			if (sPlayer.CosplayIndex() != 0) {
				SoundManager::GetSndComponent()->PlaySound(eSE_NG);
			}
			else if (sPlayer.CheckEnableMagic(eITEM_KIND_MAGIC_THUNDER) == FALSE) {
				SoundManager::GetSndComponent()->PlaySound(eSE_NG);
				SceneMainGame::StartNaviTalk(strNaviTalk_NoMagicPoint);
			}
			else {
				sPlayer.ChangeState(CharaPlayer::eCHARA_STATE_MAGIC_THUNDER);
			}
			break;

		case eITEM_KIND_MAGIC_FLAME:
			if (sPlayer.CosplayIndex() != 0) {
				SoundManager::GetSndComponent()->PlaySound(eSE_NG);
			}
			else if (sPlayer.CheckEnableMagic(eITEM_KIND_MAGIC_FLAME) == FALSE) {
				SoundManager::GetSndComponent()->PlaySound(eSE_NG);
				SceneMainGame::StartNaviTalk(strNaviTalk_NoMagicPoint);
			}
			else {
				sPlayer.ChangeState(CharaPlayer::eCHARA_STATE_MAGIC_FLAME);
			}
			break;

		case eITEM_KIND_MAGIC_TORNADE:
			if (sPlayer.CosplayIndex() != 0) {
				SoundManager::GetSndComponent()->PlaySound(eSE_NG);
			}
			else if (sPlayer.CheckEnableMagic(eITEM_KIND_MAGIC_TORNADE) == FALSE) {
				SoundManager::GetSndComponent()->PlaySound(eSE_NG);
				SceneMainGame::StartNaviTalk(strNaviTalk_NoMagicPoint);
			}
			else {
				sPlayer.ChangeState(CharaPlayer::eCHARA_STATE_MAGIC_TORNADE);
			}
			break;
			
		default:
			break;
		};

		delete spSubSceneInst;
		spSubSceneInst = NULL;

		MSG_FADE_OUT(0.2f);

		// カメラを戻す
		if (sPlayer.GetLockonChara() == NULL) {
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
		}
		else {
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_LOCKON);
		}

		mPhase = SceneMainGame::ePHASE_GAME;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseStartBattle(void)
{
	int model_handle = sModelMap.GetModelHandle();
	int mesh_num = MV1GetFrameMeshNum(model_handle, eBG_TYPE_TASK);

	int material_idx;
	int mesh_index = 0;
	for (int iI = 0; iI < mesh_num; iI ++) {
		mesh_index = MV1GetFrameMesh(model_handle, eBG_TYPE_TASK, iI);
		material_idx = MV1GetMeshMaterial(model_handle, mesh_index);

		// マテリアル名を取得する
		const char* mat_name = MV1GetMaterialName(model_handle, material_idx);
		// 『ENM_XXX_YYY』
		// 命名規則として、 『PlaceXXX_YYY』 とする (XXX は場所インデックス YYY は角度)
		// 名前の長さは 12 未満ははじく
		if (strlen(mat_name) < strlen("ENM_XXX_YYY")) {
			continue;
		}
		char cmd_name[3+1];
		memcpy(&cmd_name[0], &mat_name[0], sizeof(cmd_name));
		cmd_name[3] = '\0';
		if (strcmp(cmd_name, "ENM") != 0) {
			continue;
		}
			
		char place_label[3+1];
		int place_index;
		memcpy(&place_label[0], &mat_name[4], sizeof(place_label));
		place_label[3] = '\0';
		place_index = atoi(place_label);

		if (place_index == sTaskInfo.mTaskValueA) {

			VECTOR min_pos, max_pos;
			VECTOR vec;
			VECTOR world = MV1GetFramePosition(model_handle, eBG_TYPE_TASK);
			min_pos = MV1GetMeshMinPosition(model_handle, mesh_index);
			max_pos = MV1GetMeshMaxPosition(model_handle, mesh_index);
			min_pos = VScale(min_pos, spBGInfo->mScale);
			max_pos = VScale(max_pos, spBGInfo->mScale);

			vec = VSub(max_pos, min_pos);
			vec = VScale(vec, 0.1f);

			eEnemyKind enemy_kind;

			for (int iI = 0; iI < kENEMY_ONCE_MAX; iI ++) {
				const tEnemyTableInfo* p_work;
				p_work = GetEnemyTableInfo(spBGInfo->mEnemyTblIdx);
				enemy_kind = *(p_work->mpEnemyKind + (place_index * kENEMY_ONCE_MAX) + iI);

				if (enemy_kind == (eEnemyKind)-1) {
					continue;
				}

				static const float sAppearSignLength = 12000.0f;
	
				for (int retryCnt = 0; retryCnt < 5000; retryCnt ++) {
				//				for (;;) {
					int rndx, rndz;
					rndx = GetRand(10-2) + 1;
					rndz = GetRand(10-2) + 1;

					VECTOR pos;
					pos.x = min_pos.x + (vec.x * rndx);
					//					pos.y = (min_pos.y + max_pos.y) / 2.0f;
					pos.y = max_pos.y;
					pos.z = min_pos.z + (vec.z * rndz);

					pos = VAdd(world, pos);
					pos = VAdd(pos, VGet((float)iI, (float)iI, (float)iI));
					float dist = VSize(VSub(sPlayer.Position(), pos));
#ifdef _DEBUG
					//					PRINT_CONSOLE("dist : %.2f \n", dist);
#endif // _DEBUG
					if (dist < sAppearSignLength) {
						createEnemyWork2(iI, enemy_kind, &pos);
						break;
					}

				}

			}

			break;
		}
	}

	// バトル用の壁あり
	//	CollisionManager::GetInstance()->EnableBattleBox();
	int is_battle_box = (sTaskInfo.mTaskValueB % 10);
	if (is_battle_box == 1) { // 1 = バトルボックス壁あり
		CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_BATTLE_WALL);
		sModelMap.VisibleBattleBox();
	}

	int after_event_index = (sTaskInfo.mTaskValueB / 10);
	if (after_event_index != 0) {
		int event_index = ConvertPlaceIndex_TO_EventDefine(sStageIndex);
		APP_ASSERT_PRINT((event_index != eEVENT_ERROR),
						"Select event_index %d is invalid value\n", event_index);
		sEventBattleFlag = TASKVALUE_TO_BATTLEFLAG(event_index, after_event_index);
	}

	MSG_FADE_OUT(0.2f);

	//	sPlayer.PermitAcceptInput(true);		// 主人公の入力禁止を解除
	mPhase = SceneMainGame::ePHASE_GAME;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseNaviTalk(void)
{
	

	static int sTalkCount = 0;
	if (MSG_IS_FADE() == false) {
		RENDER_MESSAGE("ナビォ", sStrNaviTalkPtr, "", "", "");
		sTalkCount ++;
		if (sTalkCount > 180) {
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);

			TotalInfo::GetInstance()->SetActiveLockonEnemyName(TRUE);

			MSG_FADE_OUT(0.2f);
			// キャラクターのアップデートを再開する
			sStrNaviTalkPtr = NULL;
		}
	}
	else {
		TotalInfo::GetInstance()->SetActiveLockonEnemyName(FALSE);

		sTalkCount = 0;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseNaviTalkEnd(void)
{
	if (MSG_IS_FADE() == false) {
		sPlayer.PermitAcceptInput(true);		// 主人公の入力禁止を解除
		mPhase = SceneMainGame::ePHASE_GAME;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseSave(void)
{
	if (EQUAL_PHASE_SECOND(0.0f)) {
		MSG_FADE_IN(0.2f);
		mPhaseCounter = SEC2FRAME(10.0f);
	}

	if (BETWEEN_PHASE_SECOND(5.0f, 7.0f)) {
		RENDER_MESSAGE_COL("", "セーブが完了しました", "", "", "", GetColor(0, 0, 0));
	}
	else if (BETWEEN_PHASE_SECOND(7.0f, 8.0f)) {

		// 主人公の入力禁止を解除
		sPlayer.PermitAcceptInput(true);

		// 全ての敵の動きを有効にする
		CharaEnemy::SetStopAllEnemyFlag(FALSE);

		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
		mPhase = SceneMainGame::ePHASE_GAME;
		
		MSG_FADE_OUT(0.2f);
		
	}
	else {

		mPhaseCounter = SEC2FRAME(10.0f);	// 状態を遷移させないように
		GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_0, 
													"セーブしますか？", 
													GetColor(0, 0, 0));
		Utility::eQuestion qs;
		qs = Utility::ExecuteQuestion();

		if (qs == Utility::eQUESTION_YES) {
			// オブジェクト使用状態からオブジェクトフラグを更新
			setObjectFlagFromObjectUsed();

			BYTE sub_place_index = 0;
			// 一番近い座標を取得する
			{
				float nearest = 99999.0f;
				VECTOR pos; float rot;
				VECTOR ch_pos = sPlayer.Position();
				float dist = 0.0f;
				int cnt = 0;
				for (;;) {
					if (getPlayerPosRotFromBGModel(&pos, &rot, cnt)) {
						dist = VSize(VSub(ch_pos, pos));
						if (nearest > dist) {
							nearest = dist;
							sub_place_index = cnt;
						}
						cnt ++;
					}
					else {
						break;
					}
				}
			}

			// セーブ
			TotalInfo::GetInstance()->SaveData(-1,
											   sStageIndex, 
											   sPlayer.GetMaxHitpoint(), 
											   sPlayer.GetHitpoint(),
											   sPlayer.ItemWork().mItemSubWeapon,
											   sPlayer.CosplayIndex(),
											   sub_place_index,
											   //											   nearest_npc_index,
											   sPlayer.NowMP() );
			
			mPhaseCounter = SEC2FRAME(5.0f);

		} else if (qs == Utility::eQUESTION_NO) {
			sPlayer.PermitAcceptInput(true);		// 主人公の入力禁止を解除

			// 全ての敵の動きを有効にする
			CharaEnemy::SetStopAllEnemyFlag(FALSE);

			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
			mPhase = SceneMainGame::ePHASE_GAME;
			MSG_FADE_OUT(0.2f);
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseMovePlace(void)
{
	if (IS_FADE() == false) {

		// 移動中のフェードアウト時に呼ばれるハンドラ
		spStageFunc->NotifyMovePlaceWhileFadeout();

		VECTOR pos;
		float rot;
		getPlayerPosRotFromBGModel(&pos, &rot, sStagePlaceIndex);
		sPlayer.Position() = pos;
		sPlayer.Rotation().y = rot;
		FADE_IN(0.5f);

		CameraManager::GetInstance()->SetPatternGameBegine();

		sPlayer.PermitAcceptInput(true);

		mPhase = SceneMainGame::ePHASE_GAME;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseFlyingMove(void)
{
	if (spSubSceneInst->Update() == true) {
		spSubSceneInst->Destroy();
		delete spSubSceneInst;
		spSubSceneInst = NULL;

		mPhase = SceneMainGame::ePHASE_GAME;
	}
	
#ifdef _DEBUG
	debugPramInput();
#endif // _DEBUG
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phasePlayOcarina(void)
{
	if (spSubSceneInst->Update() == true) {

		UCHAR next_state = ((SubScenePlayOcarina *)spSubSceneInst)->GetNextState();
		sOcarinaSongSelect = ((SubScenePlayOcarina *)spSubSceneInst)->GetOcarinaSongSelect();

		spSubSceneInst->Destroy();
		delete spSubSceneInst;
		spSubSceneInst = NULL;

		switch (next_state) {

		case SubScenePlayOcarina::eNEXT_STATE_GAME:
			mPhase = SceneMainGame::ePHASE_GAME;
			break;

		case SubScenePlayOcarina::eNEXT_STATE_WAKEUP_EVENT:
			{
				BOOL is_event = FALSE;

				// 迷いの森にて
				if (sStageIndex == 34) {
					ActionTaskManager::tTaskInfo info = {
						ActionTaskManager::eTASK_EVENT, eEVENT_34, eEVENT_34_PLAYED_SONG_OF_SUN};
					ActionTaskManager::GetInstance()->NotifyAction(&info);
					is_event = TRUE;
				}
				// 港街にて
				else if ((sStageIndex == 11) &&
						 (IS_END_EVT(eEVENT_11, eEVENT_11_WAKEUP_TAKORUKA) == FALSE)) {
					CharaNpc* p_tako = NULL;
					for (int iI = 0; iI < sNpcNum; iI ++) {
						if (strcmp(CharaNpc::GetNpcResourceName(iI), NAME_Takoruka) == 0) {
							p_tako = &spNpc[iI];
							break;
						}
					}
					APP_ASSERT((p_tako != NULL),
							   "Not Found PoppoStone Npc Model \n");
					
					// タコるかがいるステージでかつ、距離が近い場合
					if ((p_tako != NULL) &&
						(VSize(VSub(sPlayer.Position(), p_tako->Position())) < 10000.0f)) {
						ActionTaskManager::tTaskInfo info = {
							ActionTaskManager::eTASK_EVENT, eEVENT_11, eEVENT_11_WAKEUP_TAKORUKA};
						ActionTaskManager::GetInstance()->NotifyAction(&info);
						is_event = TRUE;
					}
				}

				if (is_event == FALSE) {
					// 何もおこらないケース
					StartNaviTalk(strNaviTalk_NothingToDoSongSun);
					SoundManager::GetInstance()->SetVolumeBGM(kSOUND_VOLUME_DEFAULT);
					mPhase = SceneMainGame::ePHASE_GAME;
				}
			}
			break;

		case SubScenePlayOcarina::eNEXT_STATE_APPEAR_TITANIS:
			{
				sPlayer.StartMotionEx(&sMotionParam[eMT_IDLE_NOWEAPON]);
				sPlayer.ChangeAttachPattern(CharaPlayer::eATTACH_PATTERN_NORMAL);
				ActionTaskManager::tTaskInfo info = {
					ActionTaskManager::eTASK_EVENT, eEVENT_10, eEVENT_10_RIDE_VEHICLE_LONG};
				ActionTaskManager::GetInstance()->NotifyAction(&info);
			}
			break;

		case SubScenePlayOcarina::eNEXT_STATE_FLYING_MOVE:
			{
				// ぽっぽ復活イベントを終えている場合
				if (IS_END_EVT(eEVENT_29, eEVENT_29_REVIVAL_POPPO) == TRUE) {
					// ぽっぽ伯爵が召還できる場合
					if (SubSceneFlyingMove::CheckFlyingMoveEnable(sStageIndex)) {
						ActionTaskManager::tTaskInfo info = {
							ActionTaskManager::eTASK_FLYING_MOVE, 0, 0};
						ActionTaskManager::GetInstance()->NotifyAction(&info);
					}
					// ぽっぽ伯爵が召還できない場合
					else {
						StartNaviTalk(strNaviTalk_CannotFlyingMove);
						SoundManager::GetInstance()->SetVolumeBGM(kSOUND_VOLUME_DEFAULT);
						mPhase = SceneMainGame::ePHASE_GAME;
					}
				}
				// ぽっぽ復活イベントを終えていない場合
				else {
					CharaNpc* p_poppo = NULL;

					// リベルニア城にて
					if (sStageIndex == 29) {
						for (int iI = 0; iI < sNpcNum; iI ++) {
							if (strcmp(CharaNpc::GetNpcResourceName(iI), NAME_PoppoStone) == 0) {
								p_poppo = &spNpc[iI];
								break;
							}
						}
						APP_ASSERT((p_poppo != NULL),
								   "Not Found PoppoStone Npc Model \n");
					}

					// ぽっぽ伯爵がいるステージでかつ、距離が近い場合
					if ((p_poppo != NULL) &&
						(VSize(VSub(sPlayer.Position(), p_poppo->Position())) < 5000.0f)) {
						ActionTaskManager::tTaskInfo info = {
							ActionTaskManager::eTASK_EVENT, eEVENT_29, eEVENT_29_REVIVAL_POPPO};
						ActionTaskManager::GetInstance()->NotifyAction(&info);
					}
					else {
						// 何もおこらないケース
						StartNaviTalk(strNaviTalk_NothingToDoSongSun);
						SoundManager::GetInstance()->SetVolumeBGM(kSOUND_VOLUME_DEFAULT);
						mPhase = SceneMainGame::ePHASE_GAME;
					}
				}


			}
			break;

		case SubScenePlayOcarina::eNEXT_STATE_HANDLING_MAGICARMER:
			{
				CharaGuardian* p_guardin = spStageFunc->GetGuardianPtr();
				// マジックアーマーがいる場合
				if (p_guardin != NULL) {
					ActionTaskManager::tTaskInfo info = {
						ActionTaskManager::eTASK_EVENT, eEVENT_52, eEVENT_52_BOOTUP_MAGIC_ARMER };
					ActionTaskManager::GetInstance()->NotifyAction(&info);
				}
				// マジックアーマーがいない場合
				else {
					StartNaviTalk(strNaviTalk_NotFoundMagicArmer);
					SoundManager::GetInstance()->SetVolumeBGM(kSOUND_VOLUME_DEFAULT);
					mPhase = SceneMainGame::ePHASE_GAME;
				}
			}
			break;

		default:
			APP_ASSERT_PRINT((NULL),
							 "Invalid next_state \n");
			break;
		};

		if (mPhase == SceneMainGame::ePHASE_GAME) {
			
			sPlayer.PermitAcceptInput(true);
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
			sPlayer.ChangeAttachPattern(CharaPlayer::eATTACH_PATTERN_NORMAL);
			sPlayer.ChangeState(CharaPlayer::eCHARA_STATE_AFTER_EVENT);

		}

	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SceneMainGame::phaseTestConnect(void)
{
#ifdef _DEBUG
	// GameTestTool との連携を行う関数
	updateGameTestTool();
#endif // _DEBUG
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
#ifdef _DEBUG
void
SceneMainGame::debugPramInput(void)
{
	int set_phase = -1;

	static unsigned char sWaitCnt = 0;

	/* 60秒間に１回CSVファイルが更新されているかを確認し、
	   更新されていればCSV処理を実施する */
	if ((sWaitCnt % 6) == 0) {
	//	if (true) {

		// GameTestTool との連携を行う関数
		updateGameTestTool();

	}
	sWaitCnt ++;

}
#endif // _DEBUG


/**** end of file ****/


