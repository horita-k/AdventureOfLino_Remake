/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * イベント関連集約
 * @author SPATZ
 * @data   2013/08/03 10:40:29
 */
//---------------------------------------------------
/*=====================================*
 * include
 *=====================================*/
#include <windows.h>
#include "dxlib.h"

#include "AppLib/Basic/Basic.h"

#include "AppLib/Input/InputManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/RenderBloom.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Graphic/EffekseerManager.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Collision/CollisionManager.h"
#include "AppLib/Effect/EffectManager.h"

#include "AppLib/Resource/ResourceManager.h"
#include "AppLib/Resource/ResourceModel.h"
#include "AppLib/Resource/ResourceLayout.h"

#include "SceneManager.h"
#include "Scene/ActionTaskManager.h"
#include "Scene/EventManager.h"
#include "Scene/SceneMainGame.h"
#include "SubScenePlayOcarina.h"

#include "DataDefine/VoiceDef.h"
#include "DataDefine/MessageDef.h"
#include "DataDefine/MotionDef.h"
#include "DataDefine/BGDef.h"
#include "DataDefine/EffectDef.h"
#include "DataDefine/EnemyDef.h"
#include "DataDefine/CharaDef.h"

#include "Charactor/PersonalChara/CharaEnemyGiginebura.h"
#include "Charactor/PersonalChara/CharaEnemyStrategist.h"
#include "Charactor/PersonalChara/CharaEnemyDragon.h"
#include "Charactor/PersonalChara/CharaEnemyHatophon.h"
#include "Charactor/PersonalChara/CharaEnemyGoburin.h"
#include "Charactor/PersonalChara/CharaEnemyMonk.h"
#include "Charactor/PersonalChara/CharaEnemyKnight.h"
#include "Charactor/PersonalChara/CharaEnemyWizard.h"
#include "Charactor/PersonalChara/CharaEnemyFresbelk.h"
#include "Charactor/PersonalChara/CharaEnemyArmer.h"
#include "Charactor/PersonalChara/CharaEnemyMagician.h"
#include "Charactor/PersonalChara/CharaEnemyOak.h"

#include "Object/ObjectBase.h"
#include "Gimmick/GimmickBase.h"
#include "Gimmick/GimmickExtend.h"

#include "ExternDefine.h"		//<! Extern 宣言は全てここに集約する

#include "EventFunctions.h"
#include "CommandFunctions.h"
#include "StageFunc/StageFuncDef.h"

#include "Utility/Utility.h"

struct tEventConvertTable {
	BYTE	mPlaceIndex;		// ステージ番号(数値)
	BYTE	mTopEventNo;		// 先頭のイベント番号
};

enum {
    eEVENT_00_TOP  = 0,
    eEVENT_01_TOP  = (eEVENT_00_TOP + eEVENT_00_MAX),    // 
    eEVENT_08_TOP  = (eEVENT_01_TOP + eEVENT_01_MAX),    // 鹿の国
    eEVENT_10_TOP  = (eEVENT_08_TOP + eEVENT_08_MAX),    // リベルニア荒野
    eEVENT_11_TOP  = (eEVENT_10_TOP + eEVENT_10_MAX),    // リベルニア港
    eEVENT_13_TOP  = (eEVENT_11_TOP + eEVENT_11_MAX),    // 貴族街
    eEVENT_14_TOP  = (eEVENT_13_TOP + eEVENT_13_MAX),    // 魔術大学
    eEVENT_16_TOP  = (eEVENT_14_TOP + eEVENT_14_MAX),    // 忘却の森
    eEVENT_21_TOP  = (eEVENT_16_TOP + eEVENT_16_MAX),    // リベルニア商業区
    eEVENT_22_TOP  = (eEVENT_21_TOP + eEVENT_21_MAX),    // リノの家
    eEVENT_24_TOP  = (eEVENT_22_TOP + eEVENT_22_MAX),    // 地下水路
    eEVENT_25_TOP  = (eEVENT_24_TOP + eEVENT_24_MAX),    // 地下牢
    eEVENT_26_TOP  = (eEVENT_25_TOP + eEVENT_25_MAX),    // 地底湖
    eEVENT_28_TOP  = (eEVENT_26_TOP + eEVENT_26_MAX),    // 森の洞窟
    eEVENT_29_TOP  = (eEVENT_28_TOP + eEVENT_28_MAX),    // リベルニア王城・下
    eEVENT_34_TOP  = (eEVENT_29_TOP + eEVENT_29_MAX),    // 迷いの森
    eEVENT_35_TOP  = (eEVENT_34_TOP + eEVENT_34_MAX),    // ジモト村
    eEVENT_36_TOP  = (eEVENT_35_TOP + eEVENT_35_MAX),    // スルト鉱山
    eEVENT_38_TOP  = (eEVENT_36_TOP + eEVENT_36_MAX),    // クリスタルルーム
    eEVENT_39_TOP  = (eEVENT_38_TOP + eEVENT_38_MAX),    // スルト鉱山・トロッコ場所
    eEVENT_40_TOP  = (eEVENT_39_TOP + eEVENT_39_MAX),    // ハラッパ
    eEVENT_41_TOP  = (eEVENT_40_TOP + eEVENT_40_MAX),    // バベルの塔
    eEVENT_43_TOP  = (eEVENT_41_TOP + eEVENT_41_MAX),    // バベルの塔・ボス
    eEVENT_44_TOP  = (eEVENT_43_TOP + eEVENT_43_MAX),    // バベルの塔・エレベータ
    eEVENT_46_TOP  = (eEVENT_44_TOP + eEVENT_44_MAX),    // バベルの塔・牢獄
    eEVENT_47_TOP  = (eEVENT_46_TOP + eEVENT_46_MAX),    // 飛空戦ダリス
    eEVENT_48_TOP  = (eEVENT_47_TOP + eEVENT_47_MAX),    // バベルの塔・外観
    eEVENT_49_TOP  = (eEVENT_48_TOP + eEVENT_48_MAX),    // ハラッパ・トンネル
    eEVENT_52_TOP  = (eEVENT_49_TOP + eEVENT_49_MAX),    // バベルの塔2
    eEVENT_MAX_TOP = (eEVENT_52_TOP + eEVENT_52_MAX),    // 

};

static const tEventConvertTable sEvtCnvTable[] = {
    {0,   eEVENT_00_TOP     }, // eEVENT_00
    {1,   eEVENT_01_TOP     }, // eEVENT_01
    {8,   eEVENT_08_TOP     }, // eEVENT_08
    {10,  eEVENT_10_TOP     }, // eEVENT_10
    {11,  eEVENT_11_TOP     }, // eEVENT_11
    {13,  eEVENT_13_TOP     }, // eEVENT_13
    {14,  eEVENT_14_TOP     }, // eEVENT_14
    {16,  eEVENT_16_TOP     }, // eEVENT_16
    {21,  eEVENT_21_TOP     }, // eEVENT_21
    {22,  eEVENT_22_TOP     }, // eEVENT_22
    {24,  eEVENT_24_TOP     }, // eEVENT_24
    {25,  eEVENT_25_TOP     }, // eEVENT_25
    {26,  eEVENT_26_TOP     }, // eEVENT_26
    {28,  eEVENT_28_TOP     }, // eEVENT_28
    {29,  eEVENT_29_TOP     }, // eEVENT_29
    {34,  eEVENT_34_TOP     }, // eEVENT_34
    {35,  eEVENT_35_TOP     }, // eEVENT_35
    {36,  eEVENT_36_TOP     }, // eEVENT_36
    {38,  eEVENT_38_TOP     }, // eEVENT_38
    {39,  eEVENT_39_TOP     }, // eEVENT_39
    {40,  eEVENT_40_TOP     }, // eEVENT_40
    {41,  eEVENT_41_TOP     }, // eEVENT_41
    {43,  eEVENT_43_TOP     }, // eEVENT_43
    {44,  eEVENT_44_TOP     }, // eEVENT_44
    {46,  eEVENT_46_TOP     }, // eEVENT_46
    {47,  eEVENT_47_TOP     }, // eEVENT_47
    {48,  eEVENT_48_TOP     }, // eEVENT_48
    {49,  eEVENT_49_TOP     }, // eEVENT_49
    {52,  eEVENT_52_TOP     }, // eEVENT_52


};

/* Static Assert */
APP_STATIC_ASSERT( eEVENT_MAX == (sizeof(sEvtCnvTable) / sizeof(tEventConvertTable)) ,
				   "APP_ASTATIC_ASSERT : sEvtCnvTable num != eEVENT_MAX \n");

/*=====================================*
 * イベントリスト
 *=====================================*/
struct tEventListInfo {
	EventFunc	mEventFunc;		// イベント関数
	BOOL		mIsLongEvent;	// ロングイベントフラグ
};

// イベントリストテーブル
static tEventListInfo sEventListInfo[] = {
	{&event00_Opening,                     FALSE },          // eEVENT_00_OPENING 
	{&event01_GetMasterSword,              FALSE },          // eEVENT_01_GET_MASTER_SWORD 
	{&event01_EndBattleFres,               FALSE },          // eEVENT_01_END_BATTLE_FRES
	{&event08_BattleQuest2,                TRUE  },          // eEVENT_08_BATTLE_QUEST_2
	{&event08_BattleQuest2_End,            TRUE  },          // eEVENT_08_BATTLE_QUEST_2_END
	{&event08_StartRoomBoss,               FALSE },          // eEVENT_08_START_ROOM_BOSS
	{&event08_EndRoomBoss,                 FALSE },          // eEVENT_08_END_ROOM_BOSS
	{&event08_CannonJump,                  FALSE },          // eEVENT_08_CANNON_JUMP
	{&event10_CurePlayer,                  FALSE },          // eEVENT_10_CURE_PLAYER
	{&event10_RideVehicle,                 FALSE },          // eEVENT_10_RIDE_VEHICLE
	{&event10_MoveForest,                  FALSE },          // eEVENT_10_MOVE_FOREST
	{&event10_MoveMine,                    FALSE },          // eEVENT_10_MOVE_MINE
	{&event10_GetMagicTornade,             FALSE },          // eEVENT_10_GET_MAGIC_TORNADE
	{&event10_RideVehicleLong,             FALSE },          // eEVENT_10_RIDE_VEHICLE_LONG
	{&event10_GetFaily,                    FALSE },          // eEVENT_10_GET_FAILY
	{&event11_BattleQuest2,                TRUE  },          // eEVENT_11_BATTLE_QUEST_2
	{&event11_BattleQuest2_End,            TRUE  },          // eEVENT_11_BATTLE_QUEST_2_END
	{&event11_HintStone,                   FALSE },          // eEVENT_11_HINT_STONE
	{&event11_GoToOverSea,                 FALSE },          // eEVENT_11_GOTO_OVERSEA
	{&event11_BackToOverSea,               FALSE },          // eEVENT_11_BACKTO_OVERSEA
	{&event11_WakeupTakoruka,              FALSE },          // eEVENT_11_WAKEUP_TAKORUKA
	{&event13_Takoruka,                    FALSE },          // eEVENT_13_TAKORUKA
	{&event13_GoToPrison,                  FALSE },          // eEVENT_13_GOTO_PRISON
	{&event14_WelcomeColleage,             TRUE  },          // eEVENT_14_WELCOM_COLLEAGE
	{&event14_WillTalk,                    TRUE  },          // eEVENT_14_WILL_TALK
	{&event14_AboutSultMine,               TRUE  },          // eEVENT_14_ABOUT_SULT_MINE
	{&event14_WiseMan,                     FALSE },          // eEVENT_14_WISEMAN
	{&event16_WelcomeForest,               TRUE  },          // eEVENT_16_WELCOM_FOREST
	{&event16_UsaemonCome1,                TRUE  },          // eEVENT_16_USAEMON_COME_1
	{&event16_UsaemonCome2,                TRUE  },          // eEVENT_16_USAEMON_COME_2
	{&event16_RescueUsachu,                TRUE  },          // eEVENT_16_RESCUE_USACHU
	{&event16_SetStone1,                   FALSE },          // eEVENT_16_SET_STONE_1
	{&event16_SetStone2,                   FALSE },          // eEVENT_16_SET_STONE_2
	{&event16_RiverIsLower,                FALSE },          // eEVENT_16_RIVER_IS_LOWER
	{&event16_RiverIsUpper,                FALSE },          // eEVENT_16_RIVER_IS_UPPER
	{&event16_WarpCrystalHall,             TRUE  },          // eEVENT_16_WARP_CRYSTAL_HALL
	{&event16_AppearSeasow,                FALSE },          // eEVENT_16_APPEAR_SEASOW
	{&event16_OpenWireA,                   FALSE },          // eEVENT_16_OPEN_WIRE_A
	{&event16_CloseWireA,                  FALSE },          // eEVENT_16_CLOSE_WIRE_A
	{&event16_OpenWireB,                   FALSE },          // eEVENT_16_OPEN_WIRE_B
	{&event16_CloseWireB,                  FALSE },          // eEVENT_16_CLOSE_WIRE_B
	{&event16_SwordTreasure,               FALSE },          // eEVENT_16_SWORD_TREASURE
	{&event16_UsaemonCome1_B,              TRUE  },          // eEVENT_16_USAEMON_COME_1_B
	{&event16_RescueUsachuAfter,           TRUE  },          // eEVENT_16_RESCUE_USACHU_AFTER
	{&event16_RevertUsachu,                FALSE },          // eEVENT_16_REVERT_USACHU
	{&event16_HelpUsachu,                  TRUE  },          // eEVENT_16_HELP_USACHU
	{&event16_SetStone3,                   FALSE },          // eEVENT_16_SET_STONE_3
	{&event16_UsaemonCome3,                TRUE  },          // eEVENT_16_USAEMON_COME_3
	{&event21_NaviExplain,                 TRUE  },          // eEVENT_21_NAVI_EXPLAIN
	{&event21_BattleQuest1,                TRUE  },          // eEVENT_21_BATTLE_QUEST_1
	{&event21_BattleQuest1_End,            TRUE  },          // eEVENT_21_BATTLE_QUEST_1_END
	{&event21_MoveToEarth,                 FALSE },          // eEVENT_21_MOVE_TO_EARTH
	{&event21_MoveToColleage,              FALSE },          // eEVENT_21_MOVE_TO_COLLEAGE
	{&event21_BinShop,                     TRUE  },          // eEVENT_21_BIN_SHOP
	{&event22_Wakeup,                      TRUE  },          // eEVENT_22_WAKEUP
	{&event22_GeneralOcarina,              FALSE },          // eEVENT_22_GENERAL_OCARINA
	{&event22_MoveToHomeOut,               FALSE },          // eEVENT_22_MOVE_TO_HOME_OUT
	{&event22_MoveToStairUp,               FALSE },          // eEVENT_22_MOVE_TO_STAIR_UP
	{&event24_WelcomeWaterway,             TRUE  },          // eEVENT_24_WELCOME_WATERWAY 
	{&event24_Dummy01,                     FALSE },          // eEVENT_24_DUMMY1
	{&event24_WaterIsLower,                FALSE },          // eEVENT_24_WATER_IS_LOWER
	{&event24_WaterIsUpper,                FALSE },          // eEVENT_24_WATER_IS_UPPER
	{&event24_Dummy02,                     FALSE },          // eEVENT_24_DUMMY2
	{&event24_ClearBarrier,                FALSE },          // eEVENT_24_CLEAR_BARRIER
	{&event24_WarpEntrance,                FALSE },          // eEVENT_24_WARP_ENTRANCE
	{&event24_BlockToCastle,               FALSE },          // eEVENT_24_BLOCK_TO_CASTLE
	{&event25_LeaTalk,                     TRUE  },          // eEVENT_25_LEA_TALK 
	{&event26_BossAppear,                  TRUE  },          // eEVENT_26_BOSS_APPEAR 
	{&event26_ClearBarrier,                FALSE },          // eEVENT_26_CLEAR_BARRIER
	{&event26_EndBossBattle,               FALSE },          // eEVENT_26_END_BOSS_BATTLE
	{&event28_BossAppear,                  TRUE  },          // eEVENT_28_BOSS_APPEAR 
	{&event28_EndBossBattle,               FALSE },          // eEVENT_28_END_BOSS_BATTLE
	{&event28_BtlTreasureA,                FALSE },          // eEVENT_28_BTL_TREASURE_A
	{&event29_WelcomeCastle,               TRUE  },          // eEVENT_29_WELCOME_CASTLE 
	{&event29_LionTreasure,                FALSE },          // eEVENT_29_LION_TREASURE
	{&event29_BossStart,                   TRUE  },          // eEVENT_29_BOSS_START
	{&event29_BossEnd,                     FALSE },          // eEVENT_29_BOSS_END
	{&event29_BtlTreasureD,                FALSE },          // eEVENT_29_BTL_TREASURE_D
	{&event29_GateSecurity,                FALSE },          // eEVENT_29_GATE_SECURITY
	{&event29_BtlTreasureA,                FALSE },          // eEVENT_29_BTL_TREASURE_A
	{&event29_BtlTreasureB,                FALSE },          // eEVENT_29_BTL_TREASURE_B
	{&event29_BtlTreasureC,                FALSE },          // eEVENT_29_BTL_TREASURE_C
	{&event29_BattleArmer,                 FALSE },          // eEVENT_29_BATTLE_ARMER
	{&event29_BigKey,                      FALSE },          // eEVENT_29_BIG_KEY
	{&event29_AboutMaruiforce,             TRUE  },          // eEVENT_29_ABOUT_MARUIFORCE
	{&event29_MinoGuardianOrMove,          TRUE  },          // eEVENT_29_MINO_GUARDIAN_OR_MOVE
	{&event29_AppearWizard,                TRUE  },          // eEVENT_29_APPEAR_WIZARD
	{&event29_ComeFres,                    TRUE  },          // eEVENT_29_COME_FRES
	{&event29_AppearTrump,                 FALSE },          // eEVENT_29_APPEAR_TRUMP
	{&event29_BattleQuest3,                TRUE  },          // eEVENT_29_BATTLE_QUEST_3
	{&event29_BattleQuest3_End,            TRUE  },          // eEVENT_29_BATTLE_QUEST_3_END
	{&event29_MoveRichCity,                TRUE  },          // eEVENT_29_MOVE_RICHCITY
	{&event29_GetMagicFlame,               TRUE  },          // eEVENT_29_GET_MAGIC_FLAME
	{&event29_MoveUpperCastle,             TRUE  },          // eEVENT_29_MOVE_UPPER_CASTLE
	{&event29_MoveLowerCastle,             TRUE  },          // eEVENT_29_MOVE_LOWER_CASTLE
	{&event29_RevivalPoppo,                FALSE },          // eEVENT_29_REVIVAL_POPPO
	{&event29_GoToFressRoom,               FALSE },          // eEVENT_29_GO_TO_FRESS_ROOM
	{&event34_MoveToAnyWhere,              FALSE },          // eEVENT_34_MOVE_TO_ANY_WHERE
	{&event34_PlayedSongOfSun,             FALSE },          // eEVENT_34_PLAYED_SONG_OF_SUN
	{&event35_WelcomeVillage,              TRUE  },          // eEVENT_35_WELCOME_VILLAGE
	{&event35_EndBetaGame,                 TRUE  },          // eEVENT_35_END_BETA_GAME
	{&event35_GetSongWildness,             TRUE  },          // eEVENT_35_GET_SONG_WILDNESS
	{&event35_GetSongWing,                 TRUE  },          // eEVENT_35_GET_SONG_WING
	{&event35_GetSongSun,                  TRUE  },          // eEVENT_35_GET_SONG_SUN
	{&event35_AfterGetGreenCrystal,        FALSE },          // eEVENT_35_AFTER_GET_GREEN_CRYSTAL
	{&event36_ClearBarrier00,              FALSE },          // eEVENT_36_CLEAR_BARRIER_0 
	{&event36_ClearBarrier01,              FALSE },          // eEVENT_36_CLEAR_BARRIER_1
	{&event36_ClearBarrier02,              FALSE },          // eEVENT_36_CLEAR_BARRIER_2
	{&event36_MoveReverse00,               FALSE },          // eEVENT_36_MOVE_REVERSE_0
	{&event36_DirectionLightOn,            TRUE  },          // eEVENT_36_DIRECTION_LIGHT_ON
	{&event36_DirectionLightOff,           TRUE  },          // eEVENT_36_DIRECTION_LIGHT_OFF
	{&event36_BossStart,                   TRUE  },          // eEVENT_36_BOSS_START
	{&event36_AppearHeartCase,             FALSE },          // eEVENT_36_APPEAR_HEARTCASE
	{&event36_WarpTreasureRoom,            FALSE },          // eEVENT_36_WARP_TREASUREROOM
	{&event36_WarpMine,                    FALSE },          // eEVENT_36_WARP_MINE
	{&event36_WelcomeMine,                 TRUE  },          // eEVENT_36_WELCOME_MINE
	{&event36_BossEnd,                     FALSE },          // eEVENT_36_BOSS_END
	{&event36_DirectionLightOn1st,         TRUE  },          // eEVENT_36_DIRECTION_LIGHT_ON_1ST
	{&event36_ClearBarrier03,              FALSE },          // eEVENT_36_CLEAR_BARRIER_3
	{&event36_MoveReverse01,               FALSE },          // eEVENT_36_MOVE_REVERSE_1
	{&event36_FallOnlyBoss,                FALSE },          // eEVENT_36_FALL_ONLY_BOSS
	{&event36_AdviceTrolley,               FALSE },          // eEVENT_36_ADVICE_TROLLEY
	{&event36_AdviceMine,                  FALSE },          // eEVENT_36_ADVICE_MINE
	{&event38_CrystalForest,               TRUE  },          // eEVENT_38_CRYSTAL_FOREST 
	{&event38_TalkSento,                   TRUE  },          // eEVENT_38_TALK_SENTO
	{&event38_CrystalEarth,                TRUE  },          // eEVENT_38_CRYSTAL_EARTH
	{&event38_TalkYodel,                   TRUE  },          // eEVENT_38_TALK_YODEL
	{&event38_WarpOutWorld,                TRUE  },          // eEVENT_38_WARP_OUT_WORLD
	{&event39_GoToOutside,                 FALSE },          // eEVENT_39_GO_TO_OUTSIDE 
	{&event39_GoToInside,                  FALSE },          // eEVENT_39_GO_TO_INSIDE
	{&event40_WelcomWilderness,            TRUE  },          // eEVENT_40_WELCOME_WILDERNESS 
	{&event40_TornadeEvent,                TRUE  },          // eEVENT_40_TORNADE_EVENT
	{&event40_BattleTeeder,                FALSE },          // eEVENT_40_BATTLE_TEEDER
	{&event40_BattleUsiderman,             TRUE  },          // eEVENT_40_BATTLE_USIDERMAN
	{&event40_FallOnlyBoss,                FALSE },          // eEVENT_40_FALL_ONLY_BOSS
	{&event40_FailedUsiderman,             FALSE },          // eEVENT_40_FAILED_USIDERMAN
	{&event40_DisapperTornade,             FALSE },          // eEVENT_40_DISAPPER_TORNADE
	{&event40_WakeupBabel,                 TRUE  },          // eEVENT_40_WAKEUP_BABEL
	{&event40_MoveBabel,                   FALSE },          // eEVENT_40_MOVE_BABEL
	{&event40_BtlTreasureA,                FALSE },          // eEVENT_40_BTL_TREASURE_A
	{&event40_BlockTornade,                FALSE },          // eEVENT_40_BLOCK_TORNADE
	{&event40_BlockToSult,                 FALSE },          // eEVENT_40_BLOCK_TO_SULT
	{&event40_GetMagicThunder,             FALSE },          // eEVENT_40_GET_MAGIC_THUNDER
	{&event41_Dummy00,                     FALSE },          // eEVENT_41_DUMMY_00
	{&event41_Dummy01,                     FALSE },          // eEVENT_41_DUMMY_01
	{&event41_WelcomTower,                 TRUE  },          // eEVENT_41_WELCOM_TOWER
	{&event41_PowerOn,                     TRUE  },          // eEVENT_41_POWER_ON
	{&event41_RevertGuardian,              FALSE },          // eEVENT_41_REVERT_GUARDIAN
	{&event41_BtlTreasureA,                FALSE },          // eEVENT_41_BTL_TREASURE_A
	{&event41_BtlTreasureB,                FALSE },          // eEVENT_41_BTL_TREASURE_B
	{&event41_BtlTreasureC,                FALSE },          // eEVENT_41_BTL_TREASURE_C
	{&event41_BigKey,                      FALSE },          // eEVENT_41_BIG_KEY
	{&event43_BossStart,                   TRUE  },          // eEVENT_43_BOSS_APPEAR 
	{&event43_EndBossBattle,               FALSE },          // eEVENT_43_END_BOSS_BATTLE
	{&event43_BtlTreasureA,                FALSE },          // eEVENT_43_BTL_TREASURE_A
	{&event44_ElevatorUp,                  FALSE },          // eEVENT_44_ELEVATOR_UP
	{&event46_HelpSeek,                    TRUE  },          // eEVENT_46_HELP_SEEK 
	{&event46_EndEnemyBattle,              FALSE },          // eEVENT_46_END_ENEMY_BATTLE
	{&event46_WaterIsLower,                FALSE },          // eEVENT_46_WATER_IS_LOWER
	{&event46_WaterIsUpper,                FALSE },          // eEVENT_46_WATER_IS_UPPER
	{&event47_WelcomeAirship,              TRUE  },          // eEVENT_47_WELCOME_AIRSHIP 
	{&event47_TalkSeek,                    TRUE  },          // eEVENT_47_TALK_SEEK
	{&event47_StaffRoll,                   TRUE  },          // eEVENT_47_STAFF_ROLL
	{&event47_FallAirship,                 FALSE },          // eEVENT_47_FALL_AIRSHIP
	{&event47_ExitAirship,                 FALSE },          // eEVENT_47_EXIT_AIRSHIP
	{&event47_CrashAirship,                FALSE },          // eEVENT_47_CRASH_AIRSHIP
	{&event48_AppearAirship,               TRUE  },          // eEVENT_48_APPEAR_SKYSHIP
	{&event48_MiddlePlace,                 TRUE  },          // eEVENT_48_MIDDLE_PLACE
	{&event48_AppearTower,                 TRUE  },          // eEVENT_48_APPEAR_TOWER
	{&event48_GetSongHandling,             TRUE  },          // eEVENT_48_GET_SONG_HANDLING
	{&event49_MoveBabel,                   FALSE },          // eEVENT_49_MOVE_BABEL
	{&event52_ClearBarrier01,              FALSE },          // eEVENT_52_CLEAR_BARRIER_1
	{&event52_TrapBattle01,                FALSE },          // eEVENT_52_TRAP_BATTLE_1
	{&event52_WakeupLift01,                FALSE },          // eEVENT_52_WAKEUP_LIFT_1
	{&event52_ClearBarrier02A,             FALSE },          // eEVENT_52_CLEAR_BARRIER_2A
	{&event52_ClearBarrier02B,             FALSE },          // eEVENT_52_CLEAR_BARRIER_2B
	{&event52_ClearBarrier03A,             FALSE },          // eEVENT_52_CLEAR_BARRIER_3A
	{&event52_ClearBarrier03B,             FALSE },          // eEVENT_52_CLEAR_BARRIER_3B
	{&event52_ClearLaser_3F,               FALSE },          // eEVENT_52_CLEAR_LASER_3F
	{&event52_ClearBarrier03C1,            FALSE },          // eEVENT_52_CLEAR_BARRIER_3C1
	{&event52_ClearBarrier03C2,            FALSE },          // eEVENT_52_CLEAR_BARRIER_3C2
	{&event52_TrapBattle03,                FALSE },          // eEVENT_52_TRAP_BATTLE_3
	{&event52_TrapBattle_4F_A,             FALSE },          // eEVENT_52_TRAP_BATTLE_4F_A
	{&event52_ClearBarrier4F_A,            FALSE },          // eEVENT_52_CLEAR_BARRIER_4F_A
	{&event52_WakeupLift_4F_A,             FALSE },          // eEVENT_52_WAKEUP_LIFT_4F_A
	{&event52_BootupMagicArmer,            FALSE },          // eEVENT_52_BOOTUP_MAGIC_ARMER
	{&event52_ClearBarrier4F_B,            FALSE },          // eEVENT_52_CLEAR_BARRIER_4F_B
	{&event52_UpCubeBlock_4F_A,            FALSE },          // eEVENT_52_UP_CUBE_BLOCK_4F_A
	{&event52_DnCubeBlock_4F_A,            FALSE },          // eEVENT_52_DN_CUBE_BLOCK_4F_A
	{&event52_UpCubeBlock_4F_B,            FALSE },          // eEVENT_52_UP_CUBE_BLOCK_4F_B
	{&event52_DnCubeBlock_4F_B,            FALSE },          // eEVENT_52_DN_CUBE_BLOCK_4F_B
	{&event52_UpCubeBlock_4F_C,            FALSE },          // eEVENT_52_UP_CUBE_BLOCK_4F_C
	{&event52_DnCubeBlock_4F_C,            FALSE },          // eEVENT_52_DN_CUBE_BLOCK_4F_C
	{&event52_UpCubeBlock_4F_D,            FALSE },          // eEVENT_52_UP_CUBE_BLOCK_4F_D
	{&event52_DnCubeBlock_4F_D,            FALSE },          // eEVENT_52_DN_CUBE_BLOCK_4F_D
	{&event52_UpCubeBlock_4F_E,            FALSE },          // eEVENT_52_UP_CUBE_BLOCK_4F_E
	{&event52_DnCubeBlock_4F_E,            FALSE },          // eEVENT_52_DN_CUBE_BLOCK_4F_E
	{&event52_ClearBarrier4F_C,            FALSE },          // eEVENT_52_CLEAR_BARRIER_4F_C

};

/* Static Assert */
APP_STATIC_ASSERT( eEVENT_MAX_TOP == (sizeof(sEventListInfo) / sizeof(tEventListInfo)) ,
				   "APP_ASTATIC_ASSERT : sEventListInfo num != eEVENT_MAX_TOP \n");

/*=====================================*
 * ナビォトーク
 *=====================================*/
static const char* sStrNaviTalkMessage[][kEVENT_FLAG_NUM] = {
	{	// eEVENT_00, /* batokin */
		"","","","","","","","","","","","","","","","",
	},
	{	// eEVENT_01, /* LakeOfCave */
		"","","","","","","","","","","","","","","","",
	},
	{	// eEVENT_08, /* Kingdom */
		"","","","","","","","","","","","","","","","",
	},
	{	// eEVENT_10_REV_FIELD, /* Revelnear, Field */
		"","","","","","","","","","","","","","","","",
	},
	{	// eEVENT_11
		"","","","","","","","","","","","","","","","",
	},
	{	// eEVENT_13
		"","","","","","","","","","","","","","","","",
	},
	{	// eEVENT_14
		"","","","","","","","","","","","","","","","",
	},
	{	// eEVENT_16
		"","","","","","","","","","","","","","","","",
	},
	{	// eEVENT_21
		"","","","","","","","","","","","","","","","",
	},
	{	// eEVENT_22
		"","","","","","","","","","","","","","","","",
	},
	{	// eEVENT_24
		"この別れ道はたしか北だったと思うよ",
		"行き止まりだ・・・引き返そう",
		"宝箱とかは見落とさない様にね",
		"リノ！貯水池だよ。溺れないようにね",
		"上にスイッチが・・・何か飛び道具があれば・・・",
		"あとちょっとで地下牢だよ！",
		"ここからは攻撃ボタンで攻撃が可能になるよ！",
		"ごめん体験版ではお城へは行けないんだ",
		"","","","","","","",
	},
	//	eEVENT_25,			// 地下牢
	{"","","","","","","","","","","","","","","","",},
	//	eEVENT_26,			// 地底湖
	{"","","","","","","","","","","","","","","","",},
	//	eEVENT_28,			// 森の洞窟
	{"","","","","","","","","","","","","","","","",},
	//	eEVENT_29,			// リベルニア王城・下
	{"","","","","","","","","","","","","","","","",},
	//	eEVENT_34,
	{"","","","","","","","","","","","","","","","",},
	//	eEVENT_35,
	{"","","","","","","","","","","","","","","","",},
	//	eEVENT_36,			// スルト鉱山
	{
		"リノ！あそこに巨大なモンスターの頭の骨があるよ！",
		"","","","","","","","","","","","","","","",
	},
	//	eEVENT_38,			// クリスタルルーム
	{"","","","","","","","","","","","","","","","",},
	//	eEVENT_39,			// スノレト鉱山・トロッコ場所
	{"","","","","","","","","","","","","","","","",},
	//	eEVENT_40,			// ハラッパ
	{"","","","","","","","","","","","","","","","",},
	//	eEVENT_41,			// バベルの塔
	{"この空間いったい・・・？　これが古代文明の建物・・・",
	 "上に部屋がある・・・　この箱から登れそうだよ！",
	 "この扉を開けるには大きな鍵が必要だよ！",
	 "","","","","","","","","","","","","",},
	//	eEVENT_43,			// バベルの塔・ボス
	{"","","","","","","","","","","","","","","","",},
	//	eEVENT_44,			// バベルの塔・エレベータ
	{"","","","","","","","","","","","","","","","",},
	//	eEVENT_46,			// バベルの塔・牢獄
	{"リノ！この先は閉じ込められて進めそうにないよ！",
	 "ここは牢屋っぽいね、もしかするとシークがいるかも…","","","","","","","","","","","","","","",},
	//	eEVENT_47
	{"","","","","","","","","","","","","","","","",},
	//	eEVENT_48
	{"きっとどこかに入口があるはずだよ！探してみよう！",
	 "",
	 "","","","","","","","","","","","","","",},
	//	eEVENT_49
	{"","","","","","","","","","","","","","","","",},
	//	eEVENT_52
	{"","","","","","","","","","","","","","","","",},
};

/* Static Assert */
APP_STATIC_ASSERT( eEVENT_MAX == (sizeof(sStrNaviTalkMessage) / (sizeof(char *) * kEVENT_FLAG_NUM)),
				   "APP_ASTATIC_ASSERT : sStrNaviTalkMessage != eEVENT_MAX \n");


enum {
	eBATTLE_QUEST_POS_A = 0,
	eBATTLE_QUEST_POS_B,
	eBATTLE_QUEST_POS_C,
	eBATTLE_QUEST_POS_D,
	eBATTLE_QUEST_POS_E,

	eBATTLE_QUEST_POS_MAX,
};

struct tBattleQuestEnemySet {
	int			mBattleQuestPos;
	eEnemyKind	mEnemyKind;
};

struct tBattleQuestInfo {
	const tBattleQuestEnemySet*	mpEnemySet;
	const BYTE					mSetArrayNum;
	const eItemKind				mGiftItemKind;
};

/*=====================================*
 * 住居地区予選
 *=====================================*/
static const tBattleQuestEnemySet sBattleQuestEnemySet_A1[] = {
	{eBATTLE_QUEST_POS_A,	eENEMY_KIND_GOBULIN,},
	{eBATTLE_QUEST_POS_B,	eENEMY_KIND_GOBULIN,},
	{eBATTLE_QUEST_POS_C,	eENEMY_KIND_GOBULIN,},
};
static const tBattleQuestEnemySet sBattleQuestEnemySet_A2[] = {
	{eBATTLE_QUEST_POS_A,	eENEMY_KIND_MALON,},
};
static const tBattleQuestEnemySet sBattleQuestEnemySet_A3[] = {
	{eBATTLE_QUEST_POS_B,	eENEMY_KIND_MAGICIAN,},
	{eBATTLE_QUEST_POS_C,	eENEMY_KIND_KNIGHT,},
	{eBATTLE_QUEST_POS_D,	eENEMY_KIND_MAGICIAN,},
	{eBATTLE_QUEST_POS_E,	eENEMY_KIND_KNIGHT,},
};
static const tBattleQuestEnemySet sBattleQuestEnemySet_A4[] = {
	{eBATTLE_QUEST_POS_B,	eENEMY_KIND_ARMER,},
	{eBATTLE_QUEST_POS_C,	eENEMY_KIND_ARMER,},
	{eBATTLE_QUEST_POS_D,	eENEMY_KIND_ARMER,},
	{eBATTLE_QUEST_POS_E,	eENEMY_KIND_ARMER,},
};
static const tBattleQuestEnemySet sBattleQuestEnemySet_A5[] = {
	{eBATTLE_QUEST_POS_A,	eENEMY_KIND_GOBULIN,},
	{eBATTLE_QUEST_POS_B,	eENEMY_KIND_HATOPHON_Z,},
	{eBATTLE_QUEST_POS_C,	eENEMY_KIND_HATOPHON_Z,},
};
static const tBattleQuestEnemySet sBattleQuestEnemySet_A6[] = {
	{eBATTLE_QUEST_POS_A,	eENEMY_KIND_LOANA,},
};

/*=====================================*
 * 港区本戦
 *=====================================*/
static const tBattleQuestEnemySet sBattleQuestEnemySet_B1[] = {
	{eBATTLE_QUEST_POS_A,	eENEMY_KIND_MINOTAURUS,},
	{eBATTLE_QUEST_POS_B,	eENEMY_KIND_MINOTAURUS,},
	{eBATTLE_QUEST_POS_C,	eENEMY_KIND_MINOTAURUS,},
};
static const tBattleQuestEnemySet sBattleQuestEnemySet_B2[] = {
	{eBATTLE_QUEST_POS_A,	eENEMY_KIND_OAK_BOMB,},
	{eBATTLE_QUEST_POS_B,	eENEMY_KIND_OAK_BOMB,},
	{eBATTLE_QUEST_POS_C,	eENEMY_KIND_OAK_BOMB,},
};
static const tBattleQuestEnemySet sBattleQuestEnemySet_B3[] = {
	{eBATTLE_QUEST_POS_B,	eENEMY_KIND_MINO_ARMER,},
	{eBATTLE_QUEST_POS_C,	eENEMY_KIND_MINO_ARMER,},
};
static const tBattleQuestEnemySet sBattleQuestEnemySet_B4[] = {
	{eBATTLE_QUEST_POS_A,	eENEMY_KIND_WOODMAN,},
	{eBATTLE_QUEST_POS_B,	eENEMY_KIND_WOODMAN,},
	{eBATTLE_QUEST_POS_C,	eENEMY_KIND_WOODMAN,},
	{eBATTLE_QUEST_POS_D,	eENEMY_KIND_WOODMAN,},
	{eBATTLE_QUEST_POS_E,	eENEMY_KIND_WOODMAN,},
};
static const tBattleQuestEnemySet sBattleQuestEnemySet_B5[] = {
	{eBATTLE_QUEST_POS_A,	eENEMY_KIND_FRESBELK,},
};
static const tBattleQuestEnemySet sBattleQuestEnemySet_B6[] = {
	{eBATTLE_QUEST_POS_A,	eENEMY_KIND_HONGKONG,},
};

/*=====================================*
 * 全国大会
 *=====================================*/
static const tBattleQuestEnemySet sBattleQuestEnemySet_C1[] = {
	{eBATTLE_QUEST_POS_A,	eENEMY_KIND_MALON,},
	{eBATTLE_QUEST_POS_B,	eENEMY_KIND_MALON,},
	{eBATTLE_QUEST_POS_C,	eENEMY_KIND_MALON,},
};
static const tBattleQuestEnemySet sBattleQuestEnemySet_C2[] = {
	{eBATTLE_QUEST_POS_A,	eENEMY_KIND_GIMURI,},
};
static const tBattleQuestEnemySet sBattleQuestEnemySet_C3[] = {
	{eBATTLE_QUEST_POS_A,	eENEMY_KIND_SA_14,},
	{eBATTLE_QUEST_POS_B,	eENEMY_KIND_ROBOT_B,},
	{eBATTLE_QUEST_POS_C,	eENEMY_KIND_ROBOT_B,},
	{eBATTLE_QUEST_POS_D,	eENEMY_KIND_HELICOPTER,},
	{eBATTLE_QUEST_POS_E,	eENEMY_KIND_HELICOPTER,},
};
static const tBattleQuestEnemySet sBattleQuestEnemySet_C4[] = {
	{eBATTLE_QUEST_POS_B,	eENEMY_KIND_USAEMON,},
	{eBATTLE_QUEST_POS_C,	eENEMY_KIND_USIDERMAN,},
};
static const tBattleQuestEnemySet sBattleQuestEnemySet_C5[] = {
	{eBATTLE_QUEST_POS_A,	eENEMY_KIND_TEEGER_M,},
	{eBATTLE_QUEST_POS_B,	eENEMY_KIND_TEEGER,},
};
static const tBattleQuestEnemySet sBattleQuestEnemySet_C6[] = {
	{eBATTLE_QUEST_POS_A,	eENEMY_KIND_EXCIVY,},
};


#define BT_ENEMY_NUM(array)	(sizeof(array) / sizeof(tBattleQuestEnemySet))

static const tBattleQuestInfo sBattleQuestInfo[eBATTLE_QUEST_RANK_MAX][6] = {
	{
		{sBattleQuestEnemySet_A1, BT_ENEMY_NUM(sBattleQuestEnemySet_A1), eITEM_KIND_MONEY_50},
		{sBattleQuestEnemySet_A2, BT_ENEMY_NUM(sBattleQuestEnemySet_A2), eITEM_KIND_HEART},
		{sBattleQuestEnemySet_A3, BT_ENEMY_NUM(sBattleQuestEnemySet_A3), eITEM_KIND_PORTION},
		{sBattleQuestEnemySet_A4, BT_ENEMY_NUM(sBattleQuestEnemySet_A4), eITEM_KIND_MONEY_200},
		{sBattleQuestEnemySet_A5, BT_ENEMY_NUM(sBattleQuestEnemySet_A5), eITEM_KIND_HEART},
		{sBattleQuestEnemySet_A6, BT_ENEMY_NUM(sBattleQuestEnemySet_A6), eITEM_KIND_COSPLAY_LOANA},
	},{
		{sBattleQuestEnemySet_B1, BT_ENEMY_NUM(sBattleQuestEnemySet_B1), eITEM_KIND_MONEY_100},
		{sBattleQuestEnemySet_B2, BT_ENEMY_NUM(sBattleQuestEnemySet_B2), eITEM_KIND_HEART},
		{sBattleQuestEnemySet_B3, BT_ENEMY_NUM(sBattleQuestEnemySet_B3), eITEM_KIND_MONEY_200},
		{sBattleQuestEnemySet_B4, BT_ENEMY_NUM(sBattleQuestEnemySet_B4), eITEM_KIND_MONEY_200},
		{sBattleQuestEnemySet_B5, BT_ENEMY_NUM(sBattleQuestEnemySet_B5), eITEM_KIND_PORTION},
		{sBattleQuestEnemySet_B6, BT_ENEMY_NUM(sBattleQuestEnemySet_B6), eITEM_KIND_COSPLAY_HONGKONG},
	},{
		{sBattleQuestEnemySet_C1, BT_ENEMY_NUM(sBattleQuestEnemySet_C1), eITEM_KIND_MONEY_200},
		{sBattleQuestEnemySet_C2, BT_ENEMY_NUM(sBattleQuestEnemySet_C2), eITEM_KIND_MONEY_200},
		{sBattleQuestEnemySet_C3, BT_ENEMY_NUM(sBattleQuestEnemySet_C3), eITEM_KIND_MONEY_400},
		{sBattleQuestEnemySet_C4, BT_ENEMY_NUM(sBattleQuestEnemySet_C4), eITEM_KIND_PORTION},
		{sBattleQuestEnemySet_C5, BT_ENEMY_NUM(sBattleQuestEnemySet_C5), eITEM_KIND_MONEY_400},
		{sBattleQuestEnemySet_C6, BT_ENEMY_NUM(sBattleQuestEnemySet_C6), eITEM_KIND_TROPHY},
	},
};

struct tBattleQuestStageInfo {
	int		mEventBattleFlag;
	BYTE	mPlaceIndexPlayer;	// プレイヤーの位置
	BYTE	mPlaceIndexEnemy;	// 敵の位置
	BYTE	mPlaceIndexNpc;		// NPCの位置
	BYTE	mPlaceIndexEnd;		// 武道会の終了位置
	const char*	mStrRankName;		// 大会名
	const char*	mStrClearedMessage;	// クリア後のメッセージ
};

static const tBattleQuestStageInfo sBattleQuestStageInfo[eBATTLE_QUEST_RANK_MAX] = {
	{eEVENT_21_BATTLE_QUEST_1_END, 14, 13, 15, 12, "地区予選", "次の公国本戦でもがんばってくれたまえ"},
	{eEVENT_11_BATTLE_QUEST_2_END, 1,   2,  3,  4, "公国本戦", "次の全国大会でもがんばってくれたまえ"},
	{eEVENT_29_BATTLE_QUEST_3_END, 36, 37, 38, 39, "全国大会", "がんばったなリノ、おまえがナンバー１だ!!"},
};

#define	sGaltSoldierName ("ガルト兵")

// スタッフロールのCSVファイル数
static const BYTE kSTAFFROLL_CSV_NUM = 3;

// スタッフロールのでテキスト１行の高さ
static const BYTE kSTAFFROLL_TEXT_LINE_HEIGHT = 32;

/*=====================================*
 * イベント用汎用エフェクトハンドル
 *=====================================*/
static int sEventFuncEffectIndex = -1;

//---------------------------------------------------
/**
 * イベント関数の実行
 */
//---------------------------------------------------
bool
ExecuteEventFunc(int eventKind, int eventFlag)
{
	BYTE event_index = (sEvtCnvTable[eventKind].mTopEventNo + eventFlag);
	return sEventListInfo[event_index].mEventFunc();
}

//---------------------------------------------------
/**
 * ロングイベント取得関数
 */
//---------------------------------------------------
BOOL
IsLongEventFlag(int eventKind, int eventFlag)
{
	BYTE event_index = (sEvtCnvTable[eventKind].mTopEventNo + eventFlag);
	return sEventListInfo[event_index].mIsLongEvent;
}

//---------------------------------------------------
/**
 * ステージインデックスを eEVENT_??? に変換する関数
 */
//---------------------------------------------------
int ConvertPlaceIndex_TO_EventDefine(int placeIndex)
{
	for (int iI = 0; iI < eEVENT_MAX; iI ++) {
		if (sEvtCnvTable[iI].mPlaceIndex == placeIndex) {
			return iI;
		}
	}
	return eEVENT_ERROR;
}

//---------------------------------------------------
/**
 * ステージインデックスを eFIELD_??? に変換する関数 
 */
//---------------------------------------------------
int ConvertPlaceIndex_TO_FieldInfoDefine(int stageIndex)
{
	for (int iI = 0; iI < eFIELD_MAX; iI ++) {
		for (int iJ = 0; iJ < sFieldWorkTable[iI].mArrayNum; iJ ++) {
			if (sFieldWorkTable[iI].mpArray[iJ] == stageIndex) {
				return iI;
			}
		}
	}
	return eFIELD_ERROR;
}

//---------------------------------------------------
/**
   ナビォトーク
 */
//---------------------------------------------------
const char* GetStrNaviTalkMessage(int eventDef, int taskValue)
{
	return sStrNaviTalkMessage[eventDef][taskValue];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
eventGeneral_BlockCharactor(int checkStartEventPhase, int placeIndex, const char* pChName, char* pTalk[4])
{
	int check_event_phase = (sEventPhase - checkStartEventPhase);
	switch(check_event_phase) {
	case 0:
		if (strcmp(pChName, "") == 0) {
			MSG_TYPE_TELOP();	
		} else {
			MSG_TYPE_MESSAGE();
		}
		MSG_FADE_IN(0.3f);
		sEventPhase ++;
		break;
	case 1:
		if (MSG_IS_FADE() == false) {
			sEventPhase ++;
		}
		break;
	case 2:
		RENDER_MESSAGE(pChName, pTalk[0], pTalk[1], pTalk[2], pTalk[3]);
		if (CHECK_TRIG_ANY_JOY()) {
			MSG_FADE_OUT(0.4f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			sEventPhase ++;
		}	
		break;
	case 3:
		if (MSG_IS_FADE() == false) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
	case 4:
		if (IS_FADE() == false) {
			FADE_IN(0.5f);
			{
				VECTOR pos; float rot;
				getPlayerPosRotFromBGModel(&pos, &rot, placeIndex);
				sPlayer.Position() = pos;
				sPlayer.Rotation().y = rot;
			}
			sEventPhase ++;
		}
		break;
	case 5:
		return true;
	default:
		break;
	};
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
eventGeneral_BlockCharactor(int checkStartEventPhase, VECTOR* pOfsPos, float chRot, const char* pChName, char* pTalk[4])
{
	int check_event_phase = (sEventPhase - checkStartEventPhase);
	switch(check_event_phase) {
	case 0:
		if (strcmp(pChName, "") == 0) {
			MSG_TYPE_TELOP();	
		} else {
			MSG_TYPE_MESSAGE();
		}
		MSG_FADE_IN(0.3f);
		sEventPhase ++;
		break;
	case 1:
		if (MSG_IS_FADE() == false) {
			sEventPhase ++;
		}
		break;
	case 2:
		RENDER_MESSAGE(pChName, pTalk[0], pTalk[1], pTalk[2], pTalk[3]);
		if (CHECK_TRIG_ANY_JOY()) {
			MSG_FADE_OUT(0.4f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			sEventPhase ++;
		}	
		break;
	case 3:
		if (MSG_IS_FADE() == false) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
	case 4:
		if (IS_FADE() == false) {
			FADE_IN(0.5f);
			sPlayer.Position() = VAdd(sPlayer.Position(), *pOfsPos);
			sPlayer.Rotation().y = Utility::DegToRad(chRot);
			sEventPhase ++;
		}
		break;
	case 5:
		return true;
	default:
		break;
	};
	return false;
}

//---------------------------------------------------
/**
   汎用ステージ移動関数
 */
//---------------------------------------------------
bool
eventGeneral_MovePlace(int checkStartEventPhase, int stagePlaceIndex)
{
	int check_event_phase = (sEventPhase - checkStartEventPhase);
	switch (check_event_phase) {
	case 0:
		sPlayer.PermitAcceptInput(false);

		// ロックオン解除
		sPlayer.DisableLockonChara();
		destroyAllEnemy(); // 敵を全滅させる

		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == false) {

			sStagePlaceIndex = stagePlaceIndex;

			// 移動中のフェードアウト時に呼ばれるハンドラ
			spStageFunc->NotifyMovePlaceWhileFadeout();

			VECTOR pos;
			float rot;
			getPlayerPosRotFromBGModel(&pos, &rot, stagePlaceIndex);
			sPlayer.Position() = pos;
			sPlayer.Rotation().y = rot;
			FADE_IN(0.5f);

			CameraManager::GetInstance()->SetPatternGameBegine();

			sPlayer.PermitAcceptInput(true);

			return true;
		}
		break;
	default:break;
	};
	return false;
}

//---------------------------------------------------
/**
 * イベント内で汎用的に使える関数
 */
//---------------------------------------------------
bool
eventGeneral_ClearBarrier(int frameIndex, int checkStartEventPhase, SHORT camDeg/*=-1*/)
{
	VECTOR pos;
	int mesh_index;
	float alpha;

	int check_event_phase = (sEventPhase - checkStartEventPhase);

	// カメラを切り替える
	mesh_index = MV1GetFrameMesh(sModelMap.GetModelHandle(), 
								 frameIndex, 
								 0);
	sModelMap.GetMeshMdlPosition(&pos, mesh_index);
	CameraManager::GetInstance()->Target() = pos;

	// カメラ角度指摘がある場合、カメラを対象に近づけて指定角度の位置に移動させる
	if (camDeg != -1) {
		float rad = Utility::DegToRad((float)camDeg);

		VECTOR vec;
		vec.x = sin(rad + DX_PI);
		vec.y = 0.1f;
		vec.z = cos(rad + DX_PI);
		vec = VScale(vec, 2500.0f);
		CameraManager::GetInstance()->Position() = VAdd(pos, vec);
	}
	
	switch (check_event_phase) {
	case 0:
		SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);
		sPlayer.UnvisibleWeaponArrow();
		sEventPhase ++;
		break;
	case 1:

		sEventPhase ++;
		break;
	case 2:
		alpha = sModelMap.GetAlpha(frameIndex);
		alpha += -0.01f;
		if (alpha < 0.0f) alpha = 0.0f;
		sModelMap.SetAlpha(frameIndex, alpha);
		if (alpha <= 0.0f) {
			sEventPhase ++;
		}
		break;

	case 3:
		if (sEventCount > 60.0f) {
			CollisionManager::GetInstance()->RemoveWallCollision(sModelMap.GetModelHandle(), frameIndex);
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_RESET);
			return true;
		}
		break;

	default:
		break;
	};
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
eventXX_BattleQuest(eBattleQuestRank rank, int clearMatchNum)
{
	bool result = false;
	const tBattleQuestStageInfo* p_stage_info = &sBattleQuestStageInfo[rank];

	switch (sEventPhase) {
	case 0:
		if (MSG_IS_FADE() == false) {
			MSG_TYPE_TELOP();
			MSG_FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;
	case 1:
		if (MSG_IS_FADE() == false) sEventPhase ++;
		break;

	case 2:
		{
			char tmp[128];
			sprintf_s(tmp, sizeof(tmp), "リベルニア一武道会・%sに参加しますか？", p_stage_info->mStrRankName);
			RENDER_MESSAGE("", tmp, "",  "",  "");

			Utility::eQuestion qs;
			qs = Utility::ExecuteQuestion();
			if (qs == Utility::eQUESTION_YES)	sEventPhase ++;
			if (qs == Utility::eQUESTION_NO)	return true;	// 終了
		}
		break;
	case 3:
		MSG_TYPE_MESSAGE();
		CameraManager::GetInstance()->SetPatternGameBegine();
		CameraManager::GetInstance()->Target() = spNpc[0].Position();
		CameraManager::GetInstance()->Target().y += spNpc[0].BodyHeight()*0.5f;
		CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_LOW, true);

		if (clearMatchNum == 0) {
			// 初参加の場合
			RENDER_MESSAGE(sGaltSoldierName, "その意気やよし！", "それではルールを説明しよう！",  "",  "");
			if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		}
		else if (clearMatchNum >= kFIELD_INFO_XX_ARENA_BATTLE_NUM) {

			char tmp[128];
			sprintf_s(tmp, sizeof(tmp), "リノ、君は%sではもう優勝したのだよ", p_stage_info->mStrRankName);
			char tmp2[128];
			sprintf_s(tmp2, sizeof(tmp2), "%s", p_stage_info->mStrClearedMessage);
			RENDER_MESSAGE(sGaltSoldierName, tmp, tmp2,  "",  "");

			if (CHECK_TRIG_ANY_JOY()) return true;

		}
		else {
			char tmp[128];
			char tmp_message[16] = {0};
			if ((clearMatchNum+1) == (kFIELD_INFO_XX_ARENA_BATTLE_NUM)) {
				sprintf_s(tmp_message, sizeof(tmp_message), "決勝戦");
			}
			else if ((clearMatchNum+1) == (kFIELD_INFO_XX_ARENA_BATTLE_NUM-1)) {
				sprintf_s(tmp_message, sizeof(tmp_message), "準決勝戦");
			}
			else {
				sprintf_s(tmp_message, sizeof(tmp_message), "第%d回戦", clearMatchNum+1); 
			}
			sprintf_s(tmp, sizeof(tmp), "リノ、君は%sの続きからとなる", tmp_message);
			RENDER_MESSAGE(sGaltSoldierName, "その意気やよし！", tmp, "", "");
			if (CHECK_TRIG_ANY_JOY()) sEventPhase = 6;
		}
		break;

	case 4:
		RENDER_MESSAGE(sGaltSoldierName, "形式は１対１での真剣勝負！", "時間は無制限", "相手が倒れるまで戦ってもらうぞ、リノ！",  "その際にアイテムは何を使用しても構わない");
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;
	case 5:
		RENDER_MESSAGE(sGaltSoldierName, "１勝する毎に何か賞品がもらえる", "６回戦まで勝てたら優勝となる！", "", "");
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;

	case 6:
		RENDER_MESSAGE(sGaltSoldierName, "それでは、武運を祈る！", "", "", "");
		if (CHECK_TRIG_ANY_JOY()) {
			MSG_FADE_OUT(0.5f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			sEventPhase = 100;

			// このタイミングでフラグをたてる
			//			SET_EVENT_END_FLAG();
		}
		break;
	default:
		result = eventGeneral_Common_BattleQuestUpdate(rank, clearMatchNum);
		break;
	}

	return result;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
eventXX_BattleQuest_End(eBattleQuestRank rank, int clearMatchNum)
{
	bool result = false;

	const tBattleQuestStageInfo* p_stage_info = &sBattleQuestStageInfo[rank];

	switch (sEventPhase) {
	case 0:
		if (sEventCount > 30.0f) {
			MSG_TYPE_MESSAGE();
			MSG_FADE_IN(0.3f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);

			//<! １勝分ビットを有効にする
			unsigned long enable_bit = 0;
			clearMatchNum ++;
			// 一度武道回のビットを落とす
			TotalInfo::GetInstance()->SetDisableFieldInfoFlag(sStageIndex, kFIELD_INFO_XX_ARENA_MASK);
			enable_bit = ((unsigned long)clearMatchNum & kFIELD_INFO_XX_ARENA_MASK);
			TotalInfo::GetInstance()->SetEnableFieldInfoFlag(sStageIndex, enable_bit);

			sEventPhase ++;
		}
		break;
	case 1:
		if (MSG_IS_FADE() == false) sEventPhase ++;
		break;
	case 2:
		RENDER_MESSAGE(sGaltSoldierName, "そこまで！", "お互い正々堂々といい戦いだった！", "今回の賞品を渡そう", "");
		if (CHECK_TRIG_ANY_JOY()) {
			MSG_FADE_OUT(0.5f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			sEventPhase ++;
		}
		break;

	case 3:
		if (MSG_IS_FADE() == false) {
			MSG_TYPE_TELOP();
			MSG_FADE_IN(0.3f);
			SoundManager::GetSndComponent()->PlaySound(eSE_GETITEM);
			sEventPhase ++;
		}
		break;
	case 4:
		if (MSG_IS_FADE() == false) {
			char tmp[256];
			char *msg2, *msg3, *msg4;
			eItemKind item_kind = (eItemKind)0;

			const tBattleQuestInfo* p_info = &sBattleQuestInfo[rank][clearMatchNum - 1];
			item_kind = p_info->mGiftItemKind;
			{
				const tItemDetailInfo * p_item_detail = GetItemDetailInfo(item_kind);
				sprintf_s(tmp, sizeof(tmp), "%sをてにいれた！", p_item_detail->mDispName);
				msg2 = (char*)p_item_detail->mExplain0;
				msg3 = (char*)p_item_detail->mExplain1;
				msg4 = (char*)p_item_detail->mExplain2;
			}
			RENDER_MESSAGE("", tmp, msg2, msg3, msg4);

			if (CHECK_TRIG_ANY_JOY()) {
				if (item_kind == eITEM_KIND_HEART_CASE) {
					sPlayer.AddMaxHitpoint(4);
					sPlayer.AddHitPoint(9999); // 全回復
					SoundManager::GetSndComponent()->PlaySound(eSE_COMICAL);
				} else {
					updateGetItem(item_kind);
				}
				MSG_FADE_OUT(0.5f);
				SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
				sEventPhase ++;
			}
		}
		break;

	case 5:
		if (MSG_IS_FADE() == false) {
			MSG_TYPE_MESSAGE();
			MSG_FADE_IN(0.3f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
			if (clearMatchNum >= kFIELD_INFO_XX_ARENA_BATTLE_NUM) {
				sEventPhase = 11;		// 全てクリア時へ
			} else {
				sEventPhase ++;
			}
		}
		break;
	case 6:
		if (MSG_IS_FADE() == false) {
			RENDER_MESSAGE(sGaltSoldierName, "続けて次の試合を行うか？", "", "", "");
			Utility::eQuestion qs;
			qs = Utility::ExecuteQuestion();
			if (qs == Utility::eQUESTION_YES)	sEventPhase ++;
			if (qs == Utility::eQUESTION_NO) {
				sEventPhase = 10;		// キャンセル時へ
			}
		}
		break;
	case 7:
		RENDER_MESSAGE(sGaltSoldierName, "その意気やよし！", "",  "",  "");
		if (CHECK_TRIG_ANY_JOY()) {
			MSG_FADE_OUT(0.5f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			sEventPhase = 100;
		}
		break;

	case 10:
		RENDER_MESSAGE(sGaltSoldierName, "分かった", "続きを行いたい場合は",  "また私に話しかけるとよい",  "");
		if (CHECK_TRIG_ANY_JOY()) {
			MSG_FADE_OUT(0.5f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			sEventPhase = 20;
		}
		break;

	case 11:
		{
			char tmp[128];
			sprintf_s(tmp, sizeof(tmp), "%s", p_stage_info->mStrClearedMessage);
			RENDER_MESSAGE(sGaltSoldierName, "おめでとう", "君がこの武道会の優勝者だ！", tmp,  "");
			if (CHECK_TRIG_ANY_JOY()) {
				MSG_FADE_OUT(0.5f);
				SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
				sEventPhase = 20;
			}
		}
		break;

	case 20:
		if (MSG_IS_FADE() == false) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
	case 21:
		if (IS_FADE() == false) {
			// 真っ暗な中、色々処理を行う
			getPlayerPosRotFromBGModel(&spNpc[0].Position(), &spNpc[0].Rotation().y, p_stage_info->mPlaceIndexEnd);
			spNpc[0].SetPositionOnCollision();

			sPlayer.Position() = spNpc[0].Position();
			sPlayer.Position().x += sin(spNpc[0].Rotation().y) * -200.0f;
			sPlayer.Position().z += cos(spNpc[0].Rotation().y) * -200.0f;
			sPlayer.SetRotationToTarget(&spNpc[0].Position());
			sPlayer.SetPositionOnCollision();

			sPlayer.SetActionLimit(eACTION_NG);

			CameraManager::GetInstance()->SetPatternGameBegine();
			SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);

			sModelMap.SetVisible(true, eBG_TYPE_GIMMICK + 0);
			sModelMap.SetVisible(true, eBG_TYPE_GIMMICK + 1);
			CollisionManager::GetInstance()->EntryEarthCollision(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 0));
			CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 1));

			for (int iI = 1; iI < sNpcNum; iI ++) {
				spNpc[iI].ReloadInformation(sStageIndex, (VECTOR &)ZERO_VEC);
			}
			FADE_IN(0.5f);
			return true;
		}
		break;

	default:
		result = eventGeneral_Common_BattleQuestUpdate(rank, clearMatchNum);
		break;

	}
	return result;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
eventGeneral_Common_BattleQuestUpdate(eBattleQuestRank rank, int clearMatchNum)
{
	static const float sSetDetailPos[eBATTLE_QUEST_POS_MAX][2] = {
		{0.0f,		 0.0f,},			// eBATTLE_QUEST_POS_A,
		{-300.0f,	 200.0f,},			// eBATTLE_QUEST_POS_B,
		{ 300.0f,	 200.0f,},			// eBATTLE_QUEST_POS_C,
		{-300.0f,	-100.0f,},			// eBATTLE_QUEST_POS_D,
		{ 300.0f,	-100.0f,},			// eBATTLE_QUEST_POS_E,
	};

	const tBattleQuestStageInfo* p_stage_info = &sBattleQuestStageInfo[rank];

	VECTOR set_pos;
	float set_rot;

	switch (sEventPhase) {
	case 100:
		if (MSG_IS_FADE() == false) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;

	case 101:
		if (IS_FADE() == false) {
			// 真っ暗な中、色々処理を行う
			getPlayerPosRotFromBGModel(&sPlayer.Position(), &sPlayer.Rotation().y, p_stage_info->mPlaceIndexPlayer);
			sPlayer.SetPositionOnCollision();

			sPlayer.SetActionLimit(eACTION_OK);

			getPlayerPosRotFromBGModel(&spNpc[0].Position(), &spNpc[0].Rotation().y, p_stage_info->mPlaceIndexNpc);

			CameraManager::GetInstance()->SetPatternGameBegine();

			sModelMap.SetVisible(false, eBG_TYPE_GIMMICK + 0);
			sModelMap.SetVisible(false, eBG_TYPE_GIMMICK + 1);
			CollisionManager::GetInstance()->RemoveEarthCollision(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 0));
			CollisionManager::GetInstance()->RemoveWallCollision(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 1));

			SoundManager::GetInstance()->PlayBGM(BGM_BOSS_B);

			// 戦闘が始まる直前でロードする
			eEnemyKind ene_kind = (eEnemyKind)-1;
			const tBattleQuestInfo* p_info = &sBattleQuestInfo[rank][clearMatchNum];
			for (int iI = 0; iI < p_info->mSetArrayNum; iI ++) {
				if (ene_kind != p_info->mpEnemySet[iI].mEnemyKind) {
					ene_kind = p_info->mpEnemySet[iI].mEnemyKind;
					ResourceManager::GetInstance()->LoadCharaResource((char *)GetEnemyWorkTable(ene_kind)->mResourceName);
					LoadEnemyAttachmentModel(ene_kind);
				}
			}

			for (int iI = 1; iI < sNpcNum; iI ++) {
				spNpc[iI].Position().y += sDissappearLength;
				GraphicManager::GetInstance()->RemoveDrawModel(&spNpc[iI]);
				spNpc[iI].DisableShadow();
			}

			FADE_IN(0.5f);
			sEventPhase ++;
		}
		break;

	case 102:
		if (IS_FADE() == false) {
			CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_BATTLE_WALL);
			sModelMap.VisibleBattleBox();

			sEventPhase ++;
		}
		break;

	case 103:
		if (sEventCount > 60.0f) {
			MSG_TYPE_MESSAGE();
			MSG_FADE_IN(0.3f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
			sEventPhase ++;
		}
		break;

	case 104:
		if (MSG_IS_FADE() == false) {
			char tmp_message[16] = {0};
			if ((clearMatchNum+1) == (kFIELD_INFO_XX_ARENA_BATTLE_NUM)) {
				sprintf_s(tmp_message, sizeof(tmp_message), "決勝戦");
			}
			else if ((clearMatchNum+1) == (kFIELD_INFO_XX_ARENA_BATTLE_NUM-1)) {
				sprintf_s(tmp_message, sizeof(tmp_message), "準決勝戦");
			}
			else {
				sprintf_s(tmp_message, sizeof(tmp_message), "第%d回戦", clearMatchNum+1); 
			}
			RENDER_MESSAGE(sGaltSoldierName, tmp_message, "はじめ！！！", "", "");
			if (sEventCount > 120.0f) {
				MSG_FADE_OUT(0.5f);
				SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);

				getPlayerPosRotFromBGModel(&set_pos, &set_rot, p_stage_info->mPlaceIndexEnemy);

				const tBattleQuestInfo* p_info = &sBattleQuestInfo[rank][clearMatchNum];
				for (int iI = 0; iI < p_info->mSetArrayNum; iI ++) {


					MATRIX mtx = MGetRotY(set_rot + DX_PI);
					VECTOR diff = VGet(sSetDetailPos[p_info->mpEnemySet[iI].mBattleQuestPos][0], 
									   0.0f,
									   sSetDetailPos[p_info->mpEnemySet[iI].mBattleQuestPos][1]);
					diff = VTransform(diff, mtx);

					createEnemyWork2(iI, p_info->mpEnemySet[iI].mEnemyKind, &VAdd(set_pos, diff));
				}

				int event_index = ConvertPlaceIndex_TO_EventDefine(sStageIndex);
				APP_ASSERT_PRINT((event_index != eEVENT_ERROR),
								 "Select event_index %d is invalid value\n", event_index);
				// バトルフラグを立てる
				sEventBattleFlag = TASKVALUE_TO_BATTLEFLAG(event_index, p_stage_info->mEventBattleFlag);

				return true;
			}
		}
		break;
	}
	return false;
}

/*=====================================*
 * ナビォルミナンス
 *=====================================*/
static int sNaviEffHandle = -1;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
eventGeneral_NaviLuminusStart(ModelPmd* pModel)
{
	// ナビォエフェクト再生
	if (sNaviEffHandle != -1) {
		EffectManager::GetInstance()->Stop(sNaviEffHandle, 0.5f);
		sNaviEffHandle = -1;
	}
	VECTOR tmp;
	pModel->GetFramePosFromName(&tmp, "首");
	sNaviEffHandle = EffectManager::GetInstance()->Play(eEF_NAVI,
														&tmp,
														&VScale(pModel->Scale(), 1.5f),
														0.0f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
eventGeneral_NaviLuminusUpdate(ModelPmd* pModel)
{
	VECTOR setpos;
	pModel->GetFramePosFromName(&setpos, "首");
	VECTOR vec;
	vec = VSub(CameraManager::GetInstance()->Position(), setpos);
	vec = VScale(VNorm(vec), 50.0f);
	EffectManager::GetInstance()->Position(sNaviEffHandle) = VAdd(setpos, vec);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
eventGeneral_NaviLuminusBigLight(void)
{
	float scale = (sNavi.Scale().x * 1.5f + 200.0f);
	EffectManager::GetInstance()->Scale(sNaviEffHandle) = VGet(scale, scale, scale);
	EffectManager::GetInstance()->SetAlpha(sNaviEffHandle, 0.2f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
eventGeneral_NaviLuminusEnd(void)
{
	if (sNaviEffHandle != -1) {
		EffectManager::GetInstance()->Stop(sNaviEffHandle, 0.5f);
		sNaviEffHandle = -1;
	}
}

//---------------------------------------------------
/**
 * 戦闘後の宝箱出現汎用イベント　
 */
//---------------------------------------------------
bool
eventGeneral_BattleTreasure(int checkStartEventPhase/*=0*/)
{
	bool is_found = false;

	int check_event_phase = (sEventPhase - checkStartEventPhase);
	switch(check_event_phase) {
	case 0:
		for (int iI = 0; iI < sObjectBaseNum; iI ++) {
			if ((sppObjectBase[iI]->GetObjectType() == ObjectBase::eOBJECT_TYPE_TREASURE) ||
				(sppObjectBase[iI]->GetObjectType() == ObjectBase::eOBJECT_TYPE_SWITCH) ||
				(sppObjectBase[iI]->GetObjectType() == ObjectBase::eOBJECT_TYPE_HAND)) {
			
				ObjectBase* p_treasure = (ObjectTresure *)sppObjectBase[iI];

				if (p_treasure->GetHideEventIndex() == sTaskInfo.mTaskValueB) {
					p_treasure->DeHideTreasure();
					p_treasure->SetPositionOnCollision();
					SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);

					EFK_HANDLE handle;
					VECTOR scale;
					scale = VScale(p_treasure->Scale(), 2.0f);
					handle = EffekseerManager::GetInstance()->Play(eEFK_HEAL, 
																   &p_treasure->Position(), 
																   &scale);

					p_treasure->Scale() = ZERO_VEC;

					is_found = true;
					break;
				}
			}
		}
		if (is_found) {
			sEventPhase ++;
		}
		APP_ASSERT((is_found == true),
				   "Not found event treasure\n");
		break;

	case 1:
		for (int iI = 0; iI < sObjectBaseNum; iI ++) {
			if ((sppObjectBase[iI]->GetObjectType() == ObjectBase::eOBJECT_TYPE_TREASURE) ||
				(sppObjectBase[iI]->GetObjectType() == ObjectBase::eOBJECT_TYPE_SWITCH) ||
				(sppObjectBase[iI]->GetObjectType() == ObjectBase::eOBJECT_TYPE_HAND)) {

				ObjectBase* p_treasure = (ObjectTresure *)sppObjectBase[iI];

				if (p_treasure->GetHideEventIndex() == sTaskInfo.mTaskValueB) {
					float scale;
					ObjectBase::tObjectBaseParam* p_param;
					p_param = ObjectBase::GetObjectBaseParam(p_treasure->GetObjectKind());

					scale = p_treasure->Scale().x;
					float diff;
					diff = (p_param->mScale - scale) * 0.1f;
					scale += diff;
					if (scale >= (p_param->mScale * 0.95f)) {
						scale = p_param->mScale;

						sEventPhase ++;
					}
					p_treasure->Scale() = VGet(scale, scale, scale);

					CameraManager::GetInstance()->Target() = p_treasure->Position();
					CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_DUMMY);
				}
			}
		}
		break;

	case 2:
		if (sEventCount > 60.0f) {
			SET_EVENT_END_FLAG();
			return true;
		}	
		break;
	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
eventGeneral_WakeupGimmick(SHORT camDeg/*=-1*/)
{
	GimmickBase* p_gimmick = NULL;

	switch(sEventPhase) {
	case 0:
		// ギミックの中から現在の指定のイベントを契機に動作するギミックを検査する
		for (int iI = 0; iI < GimmickBase::GetGimmickBaseNum(); iI ++) {
			p_gimmick = (GimmickBase *)GimmickBase::GetGimmickBase(iI);
			if (p_gimmick->GetWakeupEventValue() == sTaskInfo.mTaskValueB) {

				// この時点でフラグを立てる
				SET_EVENT_END_FLAG();
				SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);
				sEventPhase++;
				break;
			}
		}
		APP_ASSERT_PRINT((sEventPhase == 1), 
						 "Not Found Wakeup Event Value : %d \n", sTaskInfo.mTaskValueB);

		break;
	case 1:
		// ギミックの中から現在の指定のイベントを契機に動作するギミックを検査する
		for (int iI = 0; iI < GimmickBase::GetGimmickBaseNum(); iI ++) {
			p_gimmick = (GimmickBase *)GimmickBase::GetGimmickBase(iI);
			if (p_gimmick->GetWakeupEventValue() == sTaskInfo.mTaskValueB) {

				CameraManager::GetInstance()->Target() = p_gimmick->Position();
				// カメラ角度指摘がある場合、カメラを対象に近づけて指定角度の位置に移動させる
				if (camDeg != -1) {
					float rad = Utility::DegToRad((float)camDeg);

					VECTOR vec;
					vec.x = sin(rad + DX_PI);
					vec.y = 0.1f;
					vec.z = cos(rad + DX_PI);
					vec = VScale(vec, 2500.0f);
					CameraManager::GetInstance()->Position() = VAdd(p_gimmick->Position(), vec);
				}

				if (sEventCount > 180.0f) {
					SET_EVENT_END_FLAG();
					return true;
				}	
				break;
			}
		}
		break;
	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
eventGeneral_AnythingMove(void)
{
	switch (sEventPhase) {
	case 0:
		if (sEventCount > 10) {
			sEventPhase ++;
		}
		break;

	case 1:
		SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
		CameraManager::GetInstance()->StartShakeCamera(0.5f, 10.0f, 90.0f);
		sEventPhase ++;
		break;

	case 2:
		if (sEventCount > 90) {
			MSG_TYPE_TELOP();
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
			MSG_FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;

	case 3:
		RENDER_MESSAGE("", "どこかでものが動いた音がした", "",  "",  "");
		if (CHECK_TRIG_ANY_JOY()) {
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			MSG_FADE_OUT(0.3f);
			VISIBLE_BATTLE_ICON();
			SET_EVENT_END_FLAG();
			return true;
		}
		break;

	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
 * ほむらの書を持って居ないケース
 */
//---------------------------------------------------
bool
eventGeneral_NotHaveBook(void)
{
	switch (sEventPhase) {
	case 0:
		if (MSG_IS_FADE() == false) {
			sEventPhase ++;
		}
		break;

	case 1:
		MSG_TYPE_TELOP();
		MSG_FADE_IN(0.3f);
		SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
		sEventPhase ++;
		break;

	case 2:
		if (MSG_IS_FADE() == false) {

			RENDER_MESSAGE("", "古代の民の言語で書かれてあって", "読む事ができない！", "", "");
			if (CHECK_TRIG_ANY_JOY()) {
				MSG_FADE_OUT(0.4f);
				SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
				return true;
			}
		}
		break;

	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
 * 魔法取得のイベント	
 */
//---------------------------------------------------
bool
eventGeneral_GetMagic(eItemKind itemKind)
{
	const tItemDetailInfo * p_item_detail = GetItemDetailInfo(itemKind);

	switch (sEventPhase) {
	case 0:
		if (MSG_IS_FADE() == false) {
			sEventPhase ++;
		}
		break;

	case 1:
		MSG_TYPE_TELOP();
		MSG_FADE_IN(0.3f);
		SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
		sEventPhase ++;
		break;

	case 2:
		if (MSG_IS_FADE() == false) {

			// 既に持っているなら終了
			if (TotalInfo::GetInstance()->IsWeaponFlag(itemKind)) {
				MSG_TYPE_MESSAGE();
				RENDER_MESSAGE("ナビォ", "ここの魔法は既に取得してるよ！", "", "", "");
			}
			else {
				RENDER_MESSAGE("", "退魔の剣を持つ勇者が現れた時", "石碑に託された魔法を授けよう！", "", "");
			}
			if (CHECK_TRIG_ANY_JOY()) {
				MSG_FADE_OUT(0.4f);
				SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
				if (TotalInfo::GetInstance()->IsWeaponFlag(itemKind)) {
					return true; // 終了
				}
				else {
					sEventPhase ++;
				}
			}
		}
		break;

	case 3:
		if (MSG_IS_FADE() == false) {
			SoundManager::GetSndComponent()->PlaySound(eSE_GETITEM);
			MSG_FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;

	case 4:
		{
			char tmp[256] = {0};
			const char *msg2, *msg3, *msg4;
			sprintf_s(tmp, sizeof(tmp), "%sをてにいれた！", p_item_detail->mDispName);
			msg2			= p_item_detail->mExplain0;
			msg3			= p_item_detail->mExplain1;
			msg4			= p_item_detail->mExplain2;
			RENDER_MESSAGE_COL("", tmp, msg2, msg3, msg4, GetColor(0, 0, 0));

			if (CHECK_TRIG_ANY_JOY()) {
				updateGetItem(itemKind);
				MSG_FADE_OUT(0.4f);
				SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
				return true;
			}
		}
		break;

	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
 * 唄取得のイベント
 */
//---------------------------------------------------
bool
eventGeneral_GetSong(BYTE songType)
{
	CharaNpc* p_npc = NULL;
	for (int iI = 0; iI < sNpcNum; iI ++) {
		if (strcmp(CharaNpc::GetNpcResourceName(iI), NAME_Alpu) == 0) {
			p_npc = &spNpc[iI];
		}
	}			
	APP_ASSERT((p_npc != NULL), "Not Found Npc \n");

	ModelPmd* p_subweapon = (ModelPmd *)sPlayer.GetAttachAddr(CharaPlayer::eATTACH_MODEL_SUB_WEAPON)->mpAttachedModel;

	switch (sEventPhase) {
	case 0:
		if (MSG_IS_FADE() == false) {
			sEventPhase ++;
		}
		break;

	case 1:
		MSG_TYPE_MESSAGE();
		MSG_FADE_IN(0.3f);
		SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
		sEventPhase ++;
		break;

	case 2:
		if (MSG_IS_FADE() == false) {

			// ティタニス中なら
			if (sPlayer.CosplayIndex() == eWINMENU_COSPLAY_TITANIS) {
				RENDER_MESSAGE("楽師アルプ", 
							   "あらあら、",
							   "いいティタニスに乗ってますね、リノさん・・・",
							   "", "");
				if (CHECK_TRIG_ANY_JOY()) {
					MSG_FADE_OUT_PATTERN();
					return true;
				}
			}
			// 既に持っているなら終了
			else if (TotalInfo::GetInstance()->IsSongType(songType)) {
				sEventPhase = 9;
			}
			// オカリナを持っていない場合
			else if (TotalInfo::GetInstance()->IsWeaponFlag(eITEM_KIND_OCARINA) == false) {
				RENDER_MESSAGE("楽師アルプ", 
							   "あたなが演奏できる楽器を持っていたら", 
							   "いつか一緒に演奏してみたいですね",
							   "","");
				if (CHECK_TRIG_ANY_JOY()) {
					MSG_FADE_OUT_PATTERN();
					return true;
				}
			}
			else {
				RENDER_MESSAGE("楽師アルプ", 
							   "そうだ、私の新しい曲が出来たんです。", 
							   "よかったらそのオカリナで一緒に弾いてみませんか？", "", "");
				if (CHECK_TRIG_ANY_JOY()) {

					// コスプレ中なら
					if (sPlayer.CosplayIndex() != 0) {
						sPlayer.RevertCosplay();
					}
					// キャラ周辺のカメラの設定
					CameraManager::GetInstance()->SetPatternArroundPlayer(-30,		// camDeg
																		  -310,		// camLen
																		  160,		// camPosY
																		  60);		// camTarY
					
					changeSubWeapon(eITEM_KIND_OCARINA);
					// アタッチパターンを変更
					sPlayer.ChangeAttachPattern(CharaPlayer::eATTACH_PATTERN_OCARINA);
					sPlayer.StartMotion(eMT_OCARINA_STANCE, true, 0.5f, NULL, 1.0f);
					MSG_FADE_IN(0.1f);

					sEventPhase ++;
				}
			}
		}
		break;

	case 3:
		{
			sPlayer.AttachOcarina(p_subweapon);

			char song_name[64];
			sprintf_s(song_name, sizeof(song_name), 
				"曲名は「%s」です。", GetSongName(songType));
			RENDER_MESSAGE("楽師アルプ", 
						   "え？弾くのが恥ずかしいって？",
						   "そんな事を言わず一緒に弾きましょうよ。",
						   song_name, 
						   "さぁいきましょう");
			if (CHECK_TRIG_ANY_JOY()) {
				MSG_FADE_OUT_PATTERN();
				SoundManager::GetInstance()->SetVolumeBGM(0);
				
				sEventPhase ++;
			}
		}
		break;

	case 4:
		sPlayer.AttachOcarina(p_subweapon);

		if (MSG_IS_FADE() == false) {
			// キャラ周辺のカメラの設定
			CameraManager::GetInstance()->SetPatternArroundPlayer(-25,		// camDeg
																  -280,		// camLen
																  140,		// camPosY
																  110);		// camTarY

			sPlayer.StartMotion(eMT_OCARINA_PLAY_START, false, 0.6f);
			p_npc->StartMotion(1, false, 0.6f);

			sEventPhase ++;
		}
		break;

	case 5:
		{
			sPlayer.AttachOcarina(p_subweapon);

			SubScenePlayOcarina::tSongInfo* p_info = SubScenePlayOcarina::GetSongInfoTable(songType);

			if (sPlayer.GetMotionIndex() == eMT_OCARINA_PLAY_START) {
				if (sPlayer.GetIsEndMotion()) {
					sPlayer.StartMotion(eMT_OCARINA_PLAY_DOING, true, p_info->mMotionSpeed);
					p_npc->StartMotion(2, true, p_info->mMotionSpeed * 0.5f);
				}
			}
			else {
				if (sPlayer.EqualPhaseFrame(23.0f)) {
					SoundManager::GetSndComponent()->PlaySound(p_info->mSongSoundIndex);
				}
				else if (sPlayer.BetweenPhaseFrame(23.0f, 9999.0f)) {
					// オカリナの音色がおわったら？
					if (SoundManager::GetSndComponent()->IsPlay(p_info->mSongSoundIndex) == false) {
						sPlayer.StartMotion(eMT_OCARINA_STANCE, true, 0.5f, NULL, 0.05f);
						p_npc->StartMotion(0, true, 0.5f);
						sEventPhase ++;
					}
				}
			}
		}
		break;

	case 6:
		sPlayer.AttachOcarina(p_subweapon);

		if (sPlayer.GetIsMotionBlend() == false) {

			MSG_TYPE_TELOP();
			SoundManager::GetSndComponent()->PlaySound(eSE_GETITEM);
			MSG_FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;

	case 7:
		{
			sPlayer.AttachOcarina(p_subweapon);

			char tmp[256] = {0};
			sprintf_s(tmp, sizeof(tmp), "%sを覚えた！", GetSongName(songType));
			RENDER_MESSAGE_COL("", tmp, 
							   "アイテムからオカリナを選択して演奏できるぞ", "", "", GetColor(0, 0, 0));
			if (CHECK_TRIG_ANY_JOY()) {
				TotalInfo::GetInstance()->AddSongType(songType);
				MSG_FADE_OUT_PATTERN();

				sEventPhase ++;
			}
		}
		break;

	case 8:
		sPlayer.AttachOcarina(p_subweapon);

		if (MSG_IS_FADE() == false) {
			MSG_TYPE_MESSAGE();
			MSG_FADE_IN_PATTERN();
			SoundManager::GetInstance()->SetVolumeBGM(kSOUND_VOLUME_DEFAULT);

			// キャラ周辺のカメラの設定
			CameraManager::GetInstance()->SetPatternArroundPlayer(235,		// camDeg
																  -200,		// camLen
																  210,		// camPosY
																  110);		// camTarY

			sEventPhase ++;
		}
		break;

	case 9:
		//		sPlayer.AttachOcarina(p_subweapon);

		if (MSG_IS_FADE() == false) {
			RENDER_MESSAGE("楽師アルプ", 
						   "私が教えた曲", "良かったら演奏してみて下さいね", "", "");
			if (CHECK_TRIG_ANY_JOY()) {
				MSG_FADE_OUT_PATTERN();

				sPlayer.ChangeAttachPattern(CharaPlayer::eATTACH_PATTERN_NORMAL);

				return true;
			}
		}
		break;

	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
eventGeneral_SetObjectStone(int stagePlaceIndex)
{
	VECTOR item_pos; float rot;
	getPlayerPosRotFromBGModel(&item_pos, &rot, stagePlaceIndex);

	EFK_HANDLE handle;
	VECTOR scale;

	ObjectBase::eObjectType object_type = (ObjectBase::eObjectType)0;
	UCHAR hand_state = 0;

	switch (sEventPhase) {
	case 0:
		CameraManager::GetInstance()->Target() = item_pos;
		sEventPhase ++;
		break;

	case 1:
		SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);
		scale = VGet(20.0f, 20.0f, 20.0f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_HEAL, 
													   &item_pos, 
													   &scale);
		sEventPhase ++;
		break;

	case 2:
		for (int iI = 0; iI < sObjectBaseNum; iI ++) {
			object_type = sppObjectBase[iI]->GetObjectType();
			if (object_type == ObjectBase::eOBJECT_TYPE_HAND) {
				if (((ObjectHand *)sppObjectBase[iI])->GetHandType() == ObjectHand::eHAND_TYPE_STONE) {
					sppObjectBase[iI]->Setup(eOBJECT_KIND_STONE, 
											 (eItemKind)0, 
											 &item_pos, 
											 0);

					ObjectBase::AddActiveObjectList(sppObjectBase[iI]);

					sppObjectBase[iI]->SetPositionOnCollision();
					sEventPhase ++;
					break;
				}
			}
		}
		break;

	case 3:
		if (sEventCount > 60.0f) {

			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);

			return true;
		}
		break;

	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
eventGeneral_MoveGimmick(int stagePlaceIndex, eGimmickKind gmkKind, VECTOR& rMoveVecRate)
{
	// 対象のブロック番号を取得
	// イレギュラーではあるが、指定座標と近いブロックかどうかで判定を行う。
	VECTOR pos;	float rot;
	getPlayerPosRotFromBGModel(&pos, &rot, stagePlaceIndex);

	float nearest_dist = 99999.9f;
	int nearest_index = -1;
	GimmickBase* p_gimmick = NULL;
	for (int iI = 0; iI < GimmickBase::GetGimmickBaseNum(); iI ++) {
		p_gimmick = GimmickBase::GetGimmickBase(iI);
		if (p_gimmick->GetKind() == gmkKind) {

			VECTOR org_pos;
			p_gimmick->getOriginalPosition(&org_pos);

				//			float dist = VSize(VSub(pos, p_gimmick->Position()));
			float dist = VSize(VSub(pos, org_pos));
			if (nearest_dist > dist) {
				nearest_dist = dist;
				nearest_index = iI;
			}
		}	
	}

	APP_ASSERT_PRINT((nearest_index != -1),
					 "Not Found Nearest index %d \n", nearest_index);
	p_gimmick = GimmickBase::GetGimmickBase(nearest_index);

	// カメラを切り替える
	switch (sEventPhase) {
	case 0:
		SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);
		SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
		CameraManager::GetInstance()->Target() = p_gimmick->Position();
		CameraManager::GetInstance()->Position() = VAdd(p_gimmick->Position(), VGet(1500.0f, 1500.0f, 1500.0f) );
		sEventPhase ++;
		break;

	case 1:
		p_gimmick->Position() = VAdd(p_gimmick->Position(), VScale(rMoveVecRate, (float)sEventCount) );

		if (sEventCount > 90) {
			SoundManager::GetSndComponent()->PlaySound(eSE_FALL_STONE);
			sEventPhase ++;
		}
		break;

	case 2:
		return true;

	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event00_Opening(void)
{
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event01_GetMasterSword(void)
{
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event01_EndBattleFres(void)
{
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event08_BattleQuest2(void)
{
	if (!(IS_END_EVT(eEVENT_35, eEVENT_35_AFTER_GET_GREEN_CRYSTAL))) {
		switch (sEventPhase) {
		case 0:	if (MSG_IS_FADE() == false) {MSG_FADE_IN_PATTERN();	sEventPhase ++;	}	break;
		case 1:	RENDER_MESSAGE(sGaltSoldierName, "すまんがまだ準備中だ", "また後できてくれ！",  "",  "");
			if (CHECK_TRIG_ANY_JOY()) {	MSG_FADE_OUT_PATTERN();	return true;}
			break;
		default: break;
		};
	}
	else if (!(TotalInfo::GetInstance()->GetFieldInfoFlagBit(21) & kFIELD_INFO_21_ARENA_5CLEAR_BIT)) {
		switch (sEventPhase) {
		case 0:	if (MSG_IS_FADE() == false) {MSG_FADE_IN_PATTERN();	sEventPhase ++;	}	break;
		case 1:	RENDER_MESSAGE(sGaltSoldierName, "むっ！", "君はまだ地区予選を突破していないな",  "そっちをクリアしてからまた来てくれ！",  "");
			if (CHECK_TRIG_ANY_JOY()) {	MSG_FADE_OUT_PATTERN();	return true;}
			break;
		default: break;
		};
	}
	else {
		// 現在何回戦かを調べる
		int clear_match_num = (TotalInfo::GetInstance()->GetFieldInfoFlagBit(sStageIndex) & kFIELD_INFO_XX_ARENA_MASK);
		return eventXX_BattleQuest(eBATTLE_QUEST_RANK_B, clear_match_num);
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event08_BattleQuest2_End(void)
{
	// 現在何回戦かを調べる
	int clear_match_num = (TotalInfo::GetInstance()->GetFieldInfoFlagBit(sStageIndex) & kFIELD_INFO_XX_ARENA_MASK);

	return eventXX_BattleQuest_End(eBATTLE_QUEST_RANK_B, clear_match_num);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event08_StartRoomBoss(void)
{
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event08_EndRoomBoss(void)
{
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event08_CannonJump(void)
{
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event10_CurePlayer(void)
{
	sPlayer.AddHitPoint(9999); // 全回復
	sPlayer.AddNowMagicPoint(9999);
	sPlayer.addPiyoPiyoGauge(9999);

	SoundManager::GetSndComponent()->PlaySound(eSE_HOLY);

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event10_RideVehicle(void)
{
	sPlayer.ProcessRideVehicle();

	// ティタニスを非表示にする
	for (int iI = 0; iI < sNpcNum; iI ++) {
		if (strcmp(CharaNpc::GetNpcResourceName(iI), NAME_Titanis) == 0) {
			spNpc[iI].Scale() = ZERO_VEC;
			spNpc[iI].Position().y += sDissappearLength * 1.1f;
			spNpc[iI].SetShadowVector(spNpc[iI].Position());

			spNpc[iI].DisableShadow();
			break;
		}
	}

	SoundManager::GetInstance()->PlayBGM(BGM_RIDE_VEHICLE);

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event10_MoveForest(void)
{
	if (IS_END_EVT(eEVENT_14, eEVENT_14_WILL_TALK)) {
		ActionTaskManager::tTaskInfo param = {ActionTaskManager::eTASK_MOVE, 16, 0};
		ActionTaskManager::GetInstance()->NotifyAction(&param);
		
	}
	else {
		const char* strTalk[4] = {"リノ！", "この先は深い森だよ！",  "むやみに進むと危険だ！",  ""};
		return eventGeneral_BlockCharactor(0, 13, "ナビォ", (char **)strTalk);
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event10_MoveMine(void)
{
	if (IS_END_EVT(eEVENT_14, eEVENT_14_ABOUT_SULT_MINE)) {
		ActionTaskManager::tTaskInfo param = {ActionTaskManager::eTASK_MOVE, 36, 0};
		ActionTaskManager::GetInstance()->NotifyAction(&param);
		
	}
	else {
		const char* strTalk[4] = {"リノ！", "この先は暗すぎて進めそうにないよ！",  "",  ""};
		return eventGeneral_BlockCharactor(0, &VGet(1350.0f, 0.0f, 0.0f), 90.0f, 
										   "ナビォ", (char **)strTalk);
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event10_GetMagicTornade(void)
{
	if (TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_BOOK) == FALSE) {
		return eventGeneral_NotHaveBook();
	}
	else {
		return eventGeneral_GetMagic(eITEM_KIND_MAGIC_TORNADE);
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event10_RideVehicleLong(void)
{
	switch (sEventPhase) {
	case 0:
		if (sEventCount > 30) {
			MSG_TYPE_MESSAGE();
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
			MSG_FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;

	case 1:
		if (MSG_IS_FADE() == false) {

			BOOL is_titanis = FALSE;
			for (int iI = 0; iI < sNpcNum; iI ++) {
				if (strcmp(CharaNpc::GetNpcResourceName(iI), NAME_Titanis) == 0) {
					is_titanis = TRUE;
					break;
				}
			}

			if (is_titanis == TRUE) {
				RENDER_MESSAGE("ナビォ", "ティタニスがこっちに来たよ！", "", "", "");
				if (CHECK_TRIG_ANY_JOY()) {
					SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
					MSG_FADE_OUT(0.3f);

					sEventPhase ++;
				}
			}
			else {
				RENDER_MESSAGE("ナビォ", "このあたりにティタニスがいないよ", "", "", "");
				if (CHECK_TRIG_ANY_JOY()) {
					SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
					MSG_FADE_OUT(0.3f);

					SoundManager::GetInstance()->SetVolumeBGM(kSOUND_VOLUME_DEFAULT);

					return true;
				}
			}

		}
		break;
	case 2:
		if (MSG_IS_FADE() == false) {
			FADE_OUT(0.3f);
			sEventPhase ++;
		}
		break;

	case 3:
		if (IS_FADE() == false) {

			sPlayer.ProcessRideVehicle();

			for (int iI = 0; iI < sNpcNum; iI ++) {
				if (strcmp(CharaNpc::GetNpcResourceName(iI), NAME_Titanis) == 0) {
					spNpc[iI].Scale() = ZERO_VEC;
					spNpc[iI].Position().y += sDissappearLength * 1.1f;
					spNpc[iI].SetShadowVector(spNpc[iI].Position());

					spNpc[iI].DisableShadow();
					break;
				}
			}

			FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;

	case 4:
		if (MSG_IS_FADE() == false) {

			SoundManager::GetInstance()->PlayBGM(BGM_RIDE_VEHICLE);
			return true;
		}
		break;

	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event10_GetFaily(void)
{
	switch (sEventPhase) {
	case 0:
		if (MSG_IS_FADE() == false) {
			MSG_TYPE_TELOP();
			MSG_FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;
	case 1:
		if (MSG_IS_FADE() == false) sEventPhase ++;
		break;

	case 2:
		{
			RENDER_MESSAGE("", "妖精を空きビンに捕まえますか？", "",  "",  "");

			Utility::eQuestion qs;
			qs = Utility::ExecuteQuestion(false);
			if (qs == Utility::eQUESTION_YES) {

				// 空きビンがない場合
				if (isEmptyBinContents() == FALSE) {
					MSG_TYPE_MESSAGE();
					SoundManager::GetSndComponent()->PlaySound(eSE_NG);
					sEventPhase = 3;
				}
				// 捕獲ケース
				else {
					MSG_TYPE_TELOP();
					SoundManager::GetSndComponent()->PlaySound(eSE_GETITEM);
					sEventPhase = 4;
				}
			}
			if (qs == Utility::eQUESTION_NO) {
				SoundManager::GetSndComponent()->PlaySound(eSE_CANCEL);
				return true;	// 終了
			}
		}
		break;
	case 3:
		RENDER_MESSAGE("ナビォ", "妖精を入れる空きビンがないよ！", "",  "",  "");
		if (CHECK_TRIG_ANY_JOY()) return true; // 終了

		break;

	case 4:
		{
			char msgA[64];
			const tItemDetailInfo* p_info = GetItemDetailInfo(eITEM_KIND_BIN_FAIRY);

			sprintf_s(msgA, sizeof(msgA), "%sを手に入れた！", p_info->mDispName);
			RENDER_MESSAGE("", msgA, 
						   p_info->mExplain1,
						   p_info->mExplain2,
						   "");
			if (CHECK_TRIG_ANY_JOY()) {

				addMyWeapon(eITEM_KIND_BIN_FAIRY);
				addBinContents(eITEM_KIND_BIN_FAIRY);
				
				// 妖精を非表示にする
				for (int iI = 0; iI < sNpcNum; iI ++) {
					if (strcmp(CharaNpc::GetNpcResourceName(iI), NAME_Faily) == 0) {
						spNpc[iI].Scale() = ZERO_VEC;
						spNpc[iI].Position().y += sDissappearLength * 1.1f;
						spNpc[iI].SetShadowVector(spNpc[iI].Position());
						
						spNpc[iI].DisableShadow();
					}
				}



				return true; // 終了
			}
		}

		break;

	default:
		break;
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event11_BattleQuest2(void)
{
	//	if (!(IS_END_EVT(eEVENT_35, eEVENT_35_AFTER_GET_GREEN_CRYSTAL))) {
	if (false) {
		switch (sEventPhase) {
		case 0:	if (MSG_IS_FADE() == false) {MSG_FADE_IN_PATTERN();	sEventPhase ++;	}	break;
		case 1:	RENDER_MESSAGE(sGaltSoldierName, "すまんがまだ準備中だ", "また後できてくれ！",  "",  "");
			if (CHECK_TRIG_ANY_JOY()) {	MSG_FADE_OUT_PATTERN();	return true;}
			break;
		default: break;
		};
	}
	//	else if (!(TotalInfo::GetInstance()->GetFieldInfoFlagBit(21) & kFIELD_INFO_21_ARENA_5CLEAR_BIT)) {
	else if (false) {
		switch (sEventPhase) {
		case 0:	if (MSG_IS_FADE() == false) {MSG_FADE_IN_PATTERN();	sEventPhase ++;	}	break;
		case 1:	RENDER_MESSAGE(sGaltSoldierName, "むっ！", "君はまだ地区予選を突破していないな",  "そっちをクリアしてからまた来てくれ！",  "");
			if (CHECK_TRIG_ANY_JOY()) {	MSG_FADE_OUT_PATTERN();	return true;}
			break;
		default: break;
		};
	}
	else {
		// 現在何回戦かを調べる
		int clear_match_num = (TotalInfo::GetInstance()->GetFieldInfoFlagBit(sStageIndex) & kFIELD_INFO_XX_ARENA_MASK);
		return eventXX_BattleQuest(eBATTLE_QUEST_RANK_B, clear_match_num);
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event11_BattleQuest2_End(void)
{
	// 現在何回戦かを調べる
	int clear_match_num = (TotalInfo::GetInstance()->GetFieldInfoFlagBit(sStageIndex) & kFIELD_INFO_XX_ARENA_MASK);

	return eventXX_BattleQuest_End(eBATTLE_QUEST_RANK_B, clear_match_num);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event11_HintStone(void)
{
	if (TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_BOOK) == FALSE) {
		return eventGeneral_NotHaveBook();
	}
	else {
		switch (sEventPhase) {
		case 0:
			if (MSG_IS_FADE() == false) sEventPhase ++;
			break;

		case 1:
			MSG_TYPE_TELOP();
			MSG_FADE_IN(0.3f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
			sEventPhase ++;
			break;

		case 2:
			if (MSG_IS_FADE() == false) {
				RENDER_MESSAGE("", "リベルニア荒野の北の果てにある大きな岩場", "そこの石碑に竜巻の魔法が眠っている・・・", "", "");
				if (CHECK_TRIG_ANY_JOY()) {
					MSG_FADE_OUT(0.4f);
					SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
					return true;
				}
			}
			break;

		default:
			break;
		};
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event11_GoToOverSea(void)
{
	switch (sEventPhase) {
	case 0:
		if (MSG_IS_FADE() == false) {
			MSG_TYPE_TELOP();
			MSG_FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;
	case 1:
		if (MSG_IS_FADE() == false) sEventPhase ++;
		break;

	case 2:
		{
			RENDER_MESSAGE("", "船に乗って「鹿の国」に行きますか？", "",  "",  "");

			Utility::eQuestion qs;
			qs = Utility::ExecuteQuestion();
			if (qs == Utility::eQUESTION_YES) {
				sEventPhase ++;
			}
			if (qs == Utility::eQUESTION_NO) {
				return true;	// 終了
			}
		}
		break;
	case 3:
		RENDER_MESSAGE("船乗り", "よし！それでは鹿の国に出向だー！", "",  "",  "");
		MSG_TYPE_MESSAGE();
		if (CHECK_TRIG_ANY_JOY()) {
			ActionTaskManager::tTaskInfo param = {ActionTaskManager::eTASK_MOVE, 8, 0};
			ActionTaskManager::GetInstance()->NotifyAction(&param);
			sEventPhase ++;
		}
		break;

	default:
		break;
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event11_BackToOverSea(void)
{
	switch (sEventPhase) {
	case 0:
		if (MSG_IS_FADE() == false) {
			MSG_TYPE_TELOP();
			MSG_FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;
	case 1:
		if (MSG_IS_FADE() == false) sEventPhase ++;
		break;

	case 2:
		{
			RENDER_MESSAGE("", "船に乗って「リベルニア」に行きますか？", "",  "",  "");

			Utility::eQuestion qs;
			qs = Utility::ExecuteQuestion();
			if (qs == Utility::eQUESTION_YES) {
				sEventPhase ++;
			}
			if (qs == Utility::eQUESTION_NO) {
				return true;	// 終了
			}
		}
		break;
	case 3:
		RENDER_MESSAGE("船乗り", "よし！それではリベルニアに出向だー！", "",  "",  "");
		MSG_TYPE_MESSAGE();
		if (CHECK_TRIG_ANY_JOY()) {
			ActionTaskManager::tTaskInfo param = {ActionTaskManager::eTASK_MOVE, 11, 5};
			ActionTaskManager::GetInstance()->NotifyAction(&param);
			sEventPhase ++;
		}
		break;

	default:
		break;
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event11_WakeupTakoruka(void)
{
	CharaNpc* p_tako = NULL;
	for (int iI = 0; iI < sNpcNum; iI ++) {
		if (strcmp(CharaNpc::GetNpcResourceName(iI), NAME_Takoruka) == 0) {
			p_tako = &spNpc[iI];
			break;
		}
	}
	APP_ASSERT((p_tako != NULL),
			   "Not Found PoppoStone Npc Model \n");


	switch (sEventPhase) {
	case 0:
		SoundManager::GetInstance()->SetVolumeBGM(kSOUND_VOLUME_DEFAULT);
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_DUMMY);

		// カメラ位置を更新
		VECTOR cam_pos, cam_tar;
		cam_tar = p_tako->Position();
		cam_tar.y += p_tako->BodyHeight() * 0.5f;
		cam_pos.x = cam_tar.x + (sin(p_tako->Rotation().y + DX_PI) * 1300.0f);
		cam_pos.y = cam_tar.y + 50.0f;
		cam_pos.z = cam_tar.z + (cos(p_tako->Rotation().y + DX_PI) * 1300.0f);

		CameraManager::GetInstance()->Position() = cam_pos;
		CameraManager::GetInstance()->Target() = cam_tar;

		sEventPhase ++;
		break;

	case 1:
		if (sEventCount == 120) {
			SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);
			p_tako->StartMotion(2, false, 0.7f);

			sEventPhase ++;
			sEventCount = 0;
		}
		break;

	case 2:
		if (sEventCount == 120) {
			MSG_TYPE_TELOP();
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;

	case 3:
		if (IS_FADE() == false) {

			int color = GetColor(255, 255, 255);
			RENDER_MESSAGE_COL("", "昼寝から目覚めたたこは",
							   "用事を思い出し、海の彼方へ行ってしまった・・・", 
							   "", "", color);
			if (CHECK_TRIG_ANY_JOY()) {
				MSG_FADE_OUT_PATTERN();
				sEventPhase ++;
			}
		}
		break;

	case 4:
		if (MSG_IS_FADE() == false) {
			// オカリナが宙に浮いてしまう不具合回避
			sPlayer.ChangeAttachPattern(CharaPlayer::eATTACH_PATTERN_HAND);

			/*===================================== *
			 * NPC の割り当てを変更させる
			 *===================================== */
			spNpc = CharaNpc::RecreateNpcInstance(1, spNpc, sNpcNum, sStageIndex, spBGInfo->mScale);

			FADE_IN(1.0f);

			SET_EVENT_END_FLAG();
			return true;
		}
		break;

	default:
		break;

	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event13_Takoruka(void)
{
	if (IS_END_EVT(eEVENT_29, eEVENT_29_COME_FRES)) {
		if (sEventPhase == 0) {
			ActionTaskManager::tTaskInfo param = {ActionTaskManager::eTASK_MOVE, 29, 29};
			ActionTaskManager::GetInstance()->NotifyAction(&param);
			sEventPhase ++;
		}
	}
	else {
		const char* strTalk[4] = {"なぜかタコの形をしたものが", "道をふさいでいる",  "",  ""};
		return eventGeneral_BlockCharactor(0, &VGet((450.0f * ALL_SCALE), 0.0f, 0.0f), 90.0f, 
										   "", (char **)strTalk);
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event13_GoToPrison(void)
{
	if ((sPlayer.CosplayIndex() == eWINMENU_COSPLAY_MASK) ||
		(sPlayer.CosplayIndex() == eWINMENU_COSPLAY_HONGKONG) ) {
		switch (sEventPhase) {
		case 0:
			if(sEventCount > 60) {
				MSG_TYPE_MESSAGE();
				MSG_FADE_IN_PATTERN();
				sEventPhase ++;
			}
			break;

		case 1:
			RENDER_MESSAGE("ガルト兵", "む！怪しい奴め！", "牢屋へぶち込んでくれる！", "", "");
			if (CHECK_TRIG_ANY_JOY()) {
				MSG_FADE_OUT_PATTERN();
				sEventPhase ++;
			}
			break;

		case 2:
			if (MSG_IS_FADE() == false) {
				ActionTaskManager::tTaskInfo param = {ActionTaskManager::eTASK_MOVE, 25, 2};
				ActionTaskManager::GetInstance()->NotifyAction(&param);
				sEventPhase ++;
			}
			break;
			
		default:
			break;
		};
	}
	else {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event13_GoInRoom(void)
{
	return eventGeneral_MovePlace(0, 8);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event13_GoOutRoom(void)
{
	return eventGeneral_MovePlace(0, 7);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event14_WelcomeColleage(void)
{
	static const VECTOR offset = VScale(ZERO_VEC, 10.0f);

	static const int model_num = 2;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		ResourceModel::GetInstance()->LoadGimmickResource(RES_MDL_TETO);

		EventManager::GetInstance()->LoadCameraFile("data/Camera/014_00_Camera.vmd", 10);
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);
		EventManager::GetInstance()->SetModelParameter(1, RES_MDL_TETO,	1,						NULL,		true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		p_model->EnableToonShader();

		sEventCsv.LoadFile("data/CSV/event14_00.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv, &offset);

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction(&sEventCsv);
		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;
		{
			VECTOR pos;	float rot;
			getPlayerPosRotFromBGModel(&pos, &rot, 0);
			sPlayer.Position() = pos;
			sPlayer.Rotation().y = Utility::DegToRad(180.0f);
		}
		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();
		SET_EVENT_END_FLAG();
		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event14_WillTalk(void)
{
	static const VECTOR offset = VScale(ZERO_VEC, 10.0f);

	static const int model_num = 6;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		ResourceLayout::GetInstance()->LoadLayoutResource("Evt014_01_A.jpg");
		ResourceLayout::GetInstance()->LoadLayoutResource("Evt014_01_B.jpg");
		ResourceLayout::GetInstance()->LoadLayoutResource("Evt014_01_C.jpg");
		ResourceLayout::GetInstance()->LoadLayoutResource("Evt014_01_D.jpg");
		ResourceLayout::GetInstance()->LoadLayoutResource("Evt014_01_E.jpg");

		ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_MONEY);

		ResourceModel::GetInstance()->LoadGimmickResource(RES_MDL_TETO);

		ResourceManager::GetInstance()->LoadCharaResource(NAME_Will);
		ResourceManager::GetInstance()->LoadCharaResource("03/Pan");
		ResourceManager::GetInstance()->LoadCharaResource(NAME_Celciana);

		EventManager::GetInstance()->LoadCameraFile("data/Camera/014_01_Camera.vmd", 10);
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_014_01_EVENT,	&sModelMap, false);
		EventManager::GetInstance()->SetModelParameter(1, RES_MDL_TETO,	1,					NULL,		false);
		EventManager::GetInstance()->SetModelParameter(2, NAME_Navi,	0,					NULL,		true);
		EventManager::GetInstance()->SetModelParameter(3, NAME_Will,	0,					NULL,		true);
		EventManager::GetInstance()->SetModelParameter(4, "03/Pan",		0,					NULL,		false);
		EventManager::GetInstance()->SetModelParameter(5, NAME_Celciana,	eMT_CELCIANA_014_000_EVENT, 	NULL,		false);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		p_model->EnableToonShader();

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2); // Navi
		eventGeneral_NaviLuminusStart(p_model);

		sEventCsv.LoadFile("data/CSV/event14_01.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv, &offset);

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
		eventGeneral_NaviLuminusUpdate(p_model);

		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction(&sEventCsv);
		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();
		SoundManager::GetInstance()->PlayBGM(BGM_COLLEGE);

		/*===================================== *
		 * NPC の割り当てを変更させる
		 *===================================== */
		spNpc = CharaNpc::RecreateNpcInstance(1, spNpc, sNpcNum, sStageIndex, spBGInfo->mScale);
		
		// 非表示にしている可能性のモデルを表示させる
		sModelMap.SetVisible(true);
		for (unsigned int iI = 0; iI < sSubStageInfoList.size(); iI ++) {
			spSubModelMap[iI].SetVisible(true);
		}
		sModelXfile[eMD_SKYDOME].SetVisible(true);

		{
			VECTOR pos;	float rot;
			getPlayerPosRotFromBGModel(&pos, &rot, 3);
			sPlayer.Position() = pos;
			sPlayer.Rotation().y = Utility::DegToRad(180.0f);
		}
		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();
		SET_EVENT_END_FLAG();
		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event14_AboutSultMine(void)
{
	static const VECTOR offset = VScale(ZERO_VEC, 10.0f);

	static const int model_num = 4;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		EventManager::GetInstance()->LoadCameraFile("data/Camera/014_02_Camera.vmd", 10);
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	0,					&sModelMap, false);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Navi,	0,					NULL,		true);
		EventManager::GetInstance()->SetModelParameter(2, NAME_Will,	0,					NULL,		true);
		EventManager::GetInstance()->SetModelParameter(3, NAME_Celciana,0,				 	NULL,		false);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusStart(p_model);

		sEventCsv.LoadFile("data/CSV/event14_02.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv, &offset);

		startCsvAction2nd();

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusUpdate(p_model);

		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);
		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();

		TotalInfo::GetInstance()->AddKeyItem(eKEYITEM_NAVI_LIGHT);

		/*===================================== *
		 * NPC の割り当てを変更させる
		 *===================================== */
		spNpc = CharaNpc::RecreateNpcInstance(2, spNpc, sNpcNum, sStageIndex, spBGInfo->mScale);

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();
		SET_EVENT_END_FLAG();
		return true;
		
	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event14_WiseMan(void)
{
	static USHORT sTotalPercent = 0;

	switch (sEventPhase) {
	case 0:
		if (MSG_IS_FADE() == false) {
			MSG_TYPE_MESSAGE();
			MSG_FADE_IN_PATTERN();
			sEventPhase ++;
		}
		break;

	case 1:
		RENDER_MESSAGE("物知りばあちゃん", "アナタのやりこみ具合を", "当ててあげるわ・・・フンッ！", "", "");
		if (CHECK_TRIG_ANY_JOY()) {
			sEventPhase ++;
		}
		break;

	case 2:
		RENDER_MESSAGE("物知りばあちゃん", "・・・", "", "", "");
		if (CHECK_TRIG_ANY_JOY()) {
			sEventPhase ++;
		}
		break;

	case 3:
		RENDER_MESSAGE("物知りばあちゃん", "・・・でたわ、アナタのやりこみ度は・・・", "", "", "");
		if (CHECK_TRIG_ANY_JOY()) {
			sEventPhase ++;
		}
		break;

	case 4:
		{
			UCHAR obj_num;
			UCHAR obj_max;

			sTotalPercent = 0;//クリア

			// アイテム数
			obj_num = SceneMainGame::GetMyWeaponNum();

			if (TotalInfo::GetInstance()->IsWeaponFlag(eITEM_KIND_BIN_EMPTY) ) {
				obj_num += (TotalInfo::GetInstance()->GetItemNum(eSUBWEAPON_NUM_BIN_EMPTY) - 1);
			}
			if (TotalInfo::GetInstance()->IsWeaponFlag(eITEM_KIND_BIN_MAGENTA) ) {
				obj_num += (TotalInfo::GetInstance()->GetItemNum(eSUBWEAPON_NUM_BIN_MAGENTA) - 1);
			}
			if (TotalInfo::GetInstance()->IsWeaponFlag(eITEM_KIND_BIN_CYAN) ) {
				obj_num += (TotalInfo::GetInstance()->GetItemNum(eSUBWEAPON_NUM_BIN_CYAN) - 1);
			}
			if (TotalInfo::GetInstance()->IsWeaponFlag(eITEM_KIND_BIN_YELLOW) ) {
				obj_num += (TotalInfo::GetInstance()->GetItemNum(eSUBWEAPON_NUM_BIN_YELLOW) - 1);
			}
			if (TotalInfo::GetInstance()->IsWeaponFlag(eITEM_KIND_BIN_FAIRY) ) {
				obj_num += (TotalInfo::GetInstance()->GetItemNum(eSUBWEAPON_NUM_BIN_FAILY) - 1);
			}
			obj_num --; // No Item はカウントせず

			obj_max = 14; // 14のはず
			sTotalPercent += (obj_num, obj_max, ((obj_num * 100) / obj_max));

			char item_str[64];
			sprintf_s(item_str, sizeof(item_str),   "集めたアイテム数　　　：%02d / %02d (%03d %%)", 
					  obj_num, obj_max, ((obj_num * 100) / obj_max));

			// 貴重なアイテム
			obj_num = 0;
			obj_max = eKEYITEM_MAX;
			for (int iI = 0; iI < obj_max; iI ++) {
				if (TotalInfo::GetInstance()->IsKeyItem((USHORT)iI)) {
					obj_num ++;
				}
			}
			sTotalPercent += (obj_num, obj_max, ((obj_num * 100) / obj_max));

			char key_str[64];
			sprintf_s(key_str, sizeof(key_str),     "集めた大事な物数　　　：%02d / %02d (%03d %%)", 
					  obj_num, obj_max, ((obj_num * 100) / obj_max));

			// ハートのカケラ数
			obj_num = TotalInfo::GetInstance()->GetItemNum(eSUBWEAPON_NUM_HEART_PIECE);
			obj_max = 20;
			sTotalPercent += (obj_num, obj_max, ((obj_num * 100) / obj_max));
			char heart_str[64];
			sprintf_s(heart_str, sizeof(heart_str), "集めたハートのパーツ数：%02d / %02d (%03d %%)", 
					  obj_num, obj_max, ((obj_num * 100) / obj_max));

			// コスプレ数
			obj_num = TotalInfo::GetInstance()->GetCosplayNum();
			obj_max = (eWINMENU_COSPLAY_MAX - 2);
			sTotalPercent += (obj_num, obj_max, ((obj_num * 100) / obj_max));
			char cos_str[64];
			sprintf_s(cos_str, sizeof(cos_str),     "集めたコスプレ数　　　：%02d / %02d (%03d %%)", 
					  obj_num, obj_max, ((obj_num * 100) / obj_max));

			RENDER_MESSAGE("物知りばあちゃん", 
						   item_str, key_str, heart_str, cos_str);
			if (CHECK_TRIG_ANY_JOY()) {
				sEventPhase ++;
			}
		}
		break;

	case 5:
		{
			char total_str[64];
			const char* str_lank = NULL;
			
			if (sTotalPercent == 400) {
				str_lank = "極めたのね、すごいわ！";
			}
			else if (sTotalPercent > 350) {
				str_lank = "あとちょっとで極めれるわ！";
			}
			else if (sTotalPercent > 250) {
				str_lank = "かなり頑張ってるわね！";
			}
			else if (sTotalPercent > 100) {
				str_lank = "ちょっとずつきわめてきてるわね";
			}
			else {
				str_lank = "まだやりこみが少ないようね！";
			}

			sprintf_s(total_str, sizeof(total_str),
					  "総合やりこみパーセントは %d %%よ!", (sTotalPercent / 4));

			RENDER_MESSAGE("物知りばあちゃん", 
						   total_str,
						   str_lank, "", "");
			if (CHECK_TRIG_ANY_JOY()) {
				MSG_FADE_OUT_PATTERN();
				return true;
			}
		}
		break;
			
	default:
		break;
	};
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_WelcomeForest(void)
{
	//	static const VECTOR offset = VScale(VGet(0.0f, 163.0f, -1600.0f), 10.0f);
	static const VECTOR offset = VScale(ZERO_VEC, 10.0f);

	static const int model_num = 2;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	switch (sEventPhase) {
	case 0:

		spStageFunc->SetPatternLight(1);

		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		EventManager::GetInstance()->LoadCameraFile("data/Camera/016_00_Camera.vmd", 10.0f);
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON, &sModelMap, true);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Navi, 0, NULL, true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1); // Navi
		eventGeneral_NaviLuminusStart(p_model);

		sEventCsv.LoadFile("data/CSV/event16_00.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv, &offset);

		/* アイテムを全て表示にさせる(StartEvent にて非表示にされる為) */
		visibleObjectAll(true);

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction(&sEventCsv);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusUpdate(p_model);

		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();

		SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);
		
		spStageFunc->SetPatternLight(0);

		{
			VECTOR pos;	float rot;
			getPlayerPosRotFromBGModel(&pos, &rot, 0);

			sPlayer.Position() = pos;
			sPlayer.Rotation().y = Utility::DegToRad(180.0f);
		}
		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();
		SET_EVENT_END_FLAG();
		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_UsaemonCome1(void)
{
	ModelPmd* p_model = NULL;
	EFK_HANDLE handle;
	VECTOR scale;

	if (sEventPhase >= 2) {
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
	}

	switch (sEventPhase) {
	case 0:
		SoundManager::GetSndComponent()->PlaySound(eSE_LANDING_WATER);
		FADE_OUT(0.5f);
		HIDE_BATTLE_ICON();
		sEventPhase ++;

		break;
	case 1:
		if (IS_FADE() == true) return false;

		ResourceManager::GetInstance()->LoadCharaResource(NAME_Usae);

		EventManager::GetInstance()->LoadCameraFile("data/Camera/dummy.vmd");
		EventManager::GetInstance()->EntryModelNum(2);
		EventManager::GetInstance()->SetModelParameter(0, NAME_Usae, 0, &sModelMap, false);
		EventManager::GetInstance()->SetModelParameter(1, RES_MDL_LOCK, 0, NULL, false);
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->EnableToonShader(0.02f, 0.02f);

		{
			VECTOR vec, tmpA, tmpB;
			float dist;
			vec = VNorm(VSub(ZERO_VEC, sPlayer.Position()));
			p_model->Position() = VAdd(sPlayer.Position(), VScale(vec, 600.0f));
			p_model->SetRotationToTarget(&sPlayer.Position());
			p_model->SetVisible(false);

			tmpA = sPlayer.Position();
			tmpA.y += (sPlayer.BodyHeight() * 0.25f);
			tmpB = p_model->Position();
			tmpB.y += (sPlayer.BodyHeight() * 0.65f);

			vec = VSub(tmpB, tmpA);
			dist = VSize(vec);
			tmpA = VAdd(tmpA, VScale(VNorm(vec), (dist * 0.475f)));
			
			CameraManager::GetInstance()->Position() = tmpA;
			CameraManager::GetInstance()->Target() = tmpB;
		}

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		p_model->Position() = VGet(9566.5313f, 2580.6724f, 31244.570f);
		p_model->Scale() = VGet(140.0f, 140.0f, 140.0f);

		sEventCsv.LoadFile("data/CSV/event16_01.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		/* アイテムを全て表示にさせる(StartEvent にて非表示にされる為) */
		visibleObjectAll(true);

		FADE_IN(0.5f);

		sEventPhase ++;
		break;
	case 2:
		if (IS_FADE() == true) return false;

		SoundManager::GetSndComponent()->PlaySound(eSE_HOLY);
		scale = VScale(p_model->Scale(), 3.5f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_HEAL, 
													   &p_model->Position(), 
													   &scale);
		scale = VScale(p_model->Scale(), 1.0f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_DISAPPER, 
													   &p_model->Position(), 
													   &scale);
		sEventPhase ++;
		break;

	case 3:
		if (sEventCount > 70.0f) {
			p_model->SetVisible(true);
			sEventPhase ++;
		}
		break;

	case 4:
		if (sEventCount > 120.0f) {
			sEventPhase ++;
		}
		break;
		
	case 5:
		MSG_TYPE_MESSAGE();
		SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
		MSG_FADE_IN(0.3f);
		sEventPhase ++;
		break;

	case 6:
		if (MSG_IS_FADE() == false) {
			RENDER_MESSAGE("森の精霊", "私はこの森の精霊･･･ いやこの森の神だ･･･", "", "", "");
			if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		}
		break;

	case 7:
		RENDER_MESSAGE("森の神", "なに？森のクリスタルを探してる？", "あのクリスタルの場所は", "勇者の素質を持った者しか入ることができない",  "そこへの道は私だけが知っている");
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;

	case 8:
		RENDER_MESSAGE("森の神", "その場所を案内してあげたいのだが･･･", "しかし今、大変なことが起こっていてな･･･", "", "");
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;

	case 9:
		RENDER_MESSAGE("森の神", "・・・私の頼みごとを聞いてくれるか？", "", "", "");
		Utility::eQuestion qs;
		qs = Utility::ExecuteQuestion();
		if (qs == Utility::eQUESTION_YES)	sEventPhase ++;
		if (qs == Utility::eQUESTION_NO)	sEventPhase = 20;	// 終了
		break;

	case 10:
		RENDER_MESSAGE("森の神", "すまない", "実は私の『嫁』が古城に行ったきり帰ってこなくてな", "私の代わりに探してきて欲しいのだよ", "");
		if (CHECK_TRIG_ANY_JOY()) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;

	case 11:
		if (IS_FADE() == false) {
			CameraManager::GetInstance()->Position() = VGet(7413.2676f, 2344.2703f, 28580.256f);
			CameraManager::GetInstance()->Target()   = VGet(8003.6318f, 2497.3958f, 29255.625f);
			FADE_IN(0.5f);
			sEventPhase ++;
		}
		break;

	case 12:
		if (IS_FADE() == false) {
			RENDER_MESSAGE("森の神", "この森の奥に古城があるのだが", "誰かが古城に続く道を大岩で塞いだらしく", "帰ってこれなくなったようなのだ", "");
			if (CHECK_TRIG_ANY_JOY()) {
				sEventPhase ++;
			}
		}
		break;


	case 13:
		RENDER_MESSAGE("森の神", "確かこの森のどこかに", "大岩を砕く力が隠されていたはずだ", "湖の西側にある『石碑』にそのヒントが書かれている", "");
		if (CHECK_TRIG_ANY_JOY()) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;

	case 14:
		if (IS_FADE() == false) {
			{
				VECTOR vec, tmpA, tmpB;
				float dist;
				tmpA = sPlayer.Position();
				tmpA.y += (sPlayer.BodyHeight() * 0.25f);
				tmpB = p_model->Position();
				tmpB.y += (sPlayer.BodyHeight() * 0.65f);

				vec = VSub(tmpB, tmpA);
				dist = VSize(vec);
				tmpA = VAdd(tmpA, VScale(VNorm(vec), (dist * 0.475f)));
			
				CameraManager::GetInstance()->Position() = tmpA;
				CameraManager::GetInstance()->Target() = tmpB;
			}
			FADE_IN(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 15:
		if (IS_FADE() == false) {
			RENDER_MESSAGE("森の神", "嫁をここまで連れてきてくれたら", "クリスタルの場所を案内しよう", "それでは･･･嫁を頼む！", "");
			if (CHECK_TRIG_ANY_JOY()) {
				SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
				MSG_FADE_OUT(0.3f);
				SET_EVENT_END_FLAG();
				sEventPhase ++;
			}
		}
		break;

	case 16:
		SoundManager::GetSndComponent()->PlaySound(eSE_HOLY);
		scale = VScale(p_model->Scale(), 3.5f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_HEAL, 
													   &p_model->Position(), 
													   &scale);
		scale = VScale(p_model->Scale(), 1.0f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_DISAPPER, 
													   &p_model->Position(), 
													   &scale);
		sEventPhase ++;
		break;

	case 17:
		if (sEventCount > 70.0f) {
			p_model->SetVisible(false);
			sEventPhase = 99;
		}
		break;

		/* 断りのケース */
	case 20:
		RENDER_MESSAGE("森の神", "ばかっ!", "", "", "");
		if (CHECK_TRIG_ANY_JOY()) {
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			MSG_FADE_OUT(0.3f);
			sEventPhase = 16;
		}
		break;

	case 99:
		VISIBLE_BATTLE_ICON();
		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		return true;

	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_UsaemonCome2(void)
{
	static const float sEvt24_WaterCheckHeight	= -40.0f;
	static const float sEvt24_WaterStopHeight	= -100.0f;

	ModelPmd* p_model = NULL;
	EFK_HANDLE handle;
	VECTOR scale;

	if (sEventPhase >= 2) {
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
	}

	switch (sEventPhase) {
	case 0:
		SoundManager::GetSndComponent()->PlaySound(eSE_LANDING_WATER);
		FADE_OUT(0.5f);
		HIDE_BATTLE_ICON();
		sEventPhase ++;

		break;
	case 1:
		if (IS_FADE() == true) return false;

		ResourceManager::GetInstance()->LoadCharaResource(NAME_Usae);

		EventManager::GetInstance()->LoadCameraFile("data/Camera/dummy.vmd");
		EventManager::GetInstance()->EntryModelNum(1);
		EventManager::GetInstance()->SetModelParameter(0, NAME_Usae, 0, &sModelMap, false);
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->EnableToonShader(0.02f, 0.02f);

		{
			VECTOR vec, tmpA, tmpB;
			float dist;
			vec = VNorm(VSub(ZERO_VEC, sPlayer.Position()));
			p_model->Position() = VAdd(sPlayer.Position(), VScale(vec, 600.0f));
			p_model->SetRotationToTarget(&sPlayer.Position());
			p_model->SetVisible(false);

			tmpA = sPlayer.Position();
			tmpA.y += (sPlayer.BodyHeight() * 0.25f);
			tmpB = p_model->Position();
			tmpB.y += (sPlayer.BodyHeight() * 0.65f);

			vec = VSub(tmpB, tmpA);
			dist = VSize(vec);
			//			tmpA = VAdd(tmpA, VScale(VNorm(vec), (dist * 0.35f)));
			tmpA = VAdd(tmpA, VScale(VNorm(vec), (dist * 0.475f)));
			
			CameraManager::GetInstance()->Position() = tmpA;
			CameraManager::GetInstance()->Target() = tmpB;
		}

		sEventCsv.LoadFile("data/CSV/event16_01.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		/* アイテムを全て表示にさせる(StartEvent にて非表示にされる為) */
		visibleObjectAll(true);

		FADE_IN(0.5f);

		sEventPhase ++;
		break;
	case 2:
		if (IS_FADE() == true) return false;

		SoundManager::GetSndComponent()->PlaySound(eSE_HOLY);
		scale = VScale(p_model->Scale(), 3.5f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_HEAL, 
													   &p_model->Position(), 
													   &scale);
		scale = VScale(p_model->Scale(), 1.0f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_DISAPPER, 
													   &p_model->Position(), 
													   &scale);
		sEventPhase ++;
		break;

	case 3:
		if (sEventCount > 70.0f) {
			p_model->SetVisible(true);
			sEventPhase ++;
		}
		break;

	case 4:
		if (sEventCount > 120.0f) {
			sEventPhase ++;
		}
		break;
		
	case 5:
		MSG_TYPE_MESSAGE();
		SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
		MSG_FADE_IN(0.3f);
		sEventPhase ++;
		break;

	case 6:
		if (MSG_IS_FADE() == false) {
			RENDER_MESSAGE("森の神", "ウサミ！無事だったか！", "心配かけたな･･･", "もう君を二度とはなしたりしない･･･", "");
			if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		}
		break;

	case 7:
		RENDER_MESSAGE("森の神", "緑の小さな少女よ、嫁を連れ戻してくれてありがとう", "君には勇者の素質がある", "約束通りクリスタルの間に案内しよう！", "");
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;

	case 8:
		RENDER_MESSAGE("森の神", "ただしクリスタルの間には試練が待ち構えている", "このさきは きけんじゃぞ", "きをつけていきなされ。", "");
		if (CHECK_TRIG_ANY_JOY()) {
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			MSG_FADE_OUT(0.3f);

			sEventPhase ++;
		}
		break;

	case 9:
		SoundManager::GetSndComponent()->PlaySound(eSE_HOLY);
		scale = VScale(p_model->Scale(), 3.5f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_HEAL, 
													   &p_model->Position(), 
													   &scale);
		scale = VScale(p_model->Scale(), 1.0f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_DISAPPER, 
													   &p_model->Position(), 
													   &scale);
		sEventPhase ++;
		break;

	case 10:
		if (sEventCount > 70.0f) {
			p_model->SetVisible(false);
			SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);

			VECTOR vec, tmpA, tmpB;
			float dist;
			tmpA = sPlayer.Position();
			tmpA.y += (sPlayer.BodyHeight() * 1.0f);
			tmpB = p_model->Position();
			//			tmpB.y += (sPlayer.BodyHeight() * 0.0f);

			vec = VSub(tmpA, tmpB);
			dist = VSize(vec);
			tmpA = VAdd(tmpA, VScale(VNorm(vec), (dist * 3.0f)));
			
			CameraManager::GetInstance()->Position() = tmpA;
			CameraManager::GetInstance()->Target() = tmpB;

			SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
			//			CameraManager::GetInstance()->StartVibeCamera(2.5f, 7.0f, 10.0f);
			CameraManager::GetInstance()->StartShakeCamera(5.5f, 30.0f, 1600.0f);

			sEventPhase ++;
		}
		break;

	case 11:
		//		sModelMap.WaterOffsetPos() += -0.04f;
		sModelMap.WaterOffsetPos() += -0.12f;
		if (sModelMap.WaterOffsetPos() < sEvt24_WaterCheckHeight) {
			sModelMap.WaterOffsetPos() = sEvt24_WaterStopHeight;
			sEventPhase = 20;
		}

		break;

	case 20:

		VECTOR eff_pos;	float rot;
		getPlayerPosRotFromBGModel(&eff_pos, &rot, 9);
		sEventFuncEffectIndex = EffectManager::GetInstance()->Play(eEF_AURA,&eff_pos,
															&VGet(300.0f, 100.0f, 300.0f));
		EffectManager::GetInstance()->Rotation(sEventFuncEffectIndex) = ZERO_VEC;

		SET_EVENT_END_FLAG();

		VISIBLE_BATTLE_ICON();
		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		return true;

	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_RescueUsachu(void)
{
	switch (sEventPhase) {

	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;

	case 1:
		if (IS_FADE() == false) {
			ObjectBase* p_object = search1stFindObjectKind(eOBJECT_KIND_USACHU);
			CameraManager::GetInstance()->Target() = p_object->Position();
			//			p_object->SetRotationToTarget(&sPlayer.Position());
			
			VECTOR vec = VNorm(VSub(CameraManager::GetInstance()->Position(),
									CameraManager::GetInstance()->Target()));
			vec = VScale(vec, 300.0f);
			CameraManager::GetInstance()->Position() = VAdd(vec, CameraManager::GetInstance()->Target());

			SoundManager::GetInstance()->PlayBGM(BGM_BOSS_A);

			FADE_IN(0.5f);

			sEventPhase ++;
		}
		break;

	case 2:
		if (IS_FADE() == false) {
			MSG_TYPE_MESSAGE();
			MSG_FADE_IN(0.3f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
			
			sEventPhase ++;
		}
		break;

	case 3:
		if (MSG_IS_FADE() == false) sEventPhase ++;
		break;

	case 4:
		RENDER_MESSAGE("うさえもんの嫁", "ちゅーっ！ちゅーっ！！", "助けてちゅーっ！",  "",  "");
		if (CHECK_TRIG_ANY_JOY()) {
			// きつねの描画方式を変更する
			for (int iI = 0; iI < sNpcNum; iI ++) {
				if (strcmp(CharaNpc::GetNpcResourceName(iI), NAME_MinoArmer) == 0) {
					VECTOR pos;
					spNpc[iI].GetFramePosFromName(&pos, "頭");
					CameraManager::GetInstance()->Target() = pos;
					CameraManager::GetInstance()->Position() = VAdd(pos, VGet(200.0f, -150.0f, 200.0f));
					//					spNpc[iI].SetRotationToTarget(&sPlayer.Position());
					break;
				}
			}
			sEventPhase ++;
		}
		break;
		
	case 5:
		RENDER_MESSAGE("司祭の手下", "なんだ貴様ッ！？ うさえもんの嫁を返せだと？",  "",  "","");
		if (CHECK_TRIG_ANY_JOY()) {
			VECTOR pos = CameraManager::GetInstance()->Target();
			CameraManager::GetInstance()->Position() = (VAdd(pos, VScale(VGet(200.0f, -150.0f, 200.0f), 0.75f)));
			sEventPhase ++;
		}
		break;

	case 6:
		RENDER_MESSAGE("司祭の手下", "・・・違うッ！！これは俺の嫁になるべき女だ！", "",  "",  "");
		if (CHECK_TRIG_ANY_JOY()) {
			VECTOR pos = CameraManager::GetInstance()->Target();
			CameraManager::GetInstance()->Position() = (VAdd(pos, VScale(VGet(200.0f, -150.0f, 200.0f), 0.50f)));
			sEventPhase ++;
		}
		break;

	case 7:
		RENDER_MESSAGE("司祭の手下", "返して欲しければ俺と戦え！",  "", "",  "");
		if (CHECK_TRIG_ANY_JOY()) {

			MSG_FADE_OUT(0.3f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);

			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 8:
		if (IS_FADE() == false) {

			VECTOR pos;	float rot;
			getPlayerPosRotFromBGModel(&pos, &rot, 11);
			sPlayer.Position() = pos;
			sPlayer.Rotation().y = rot;

			getPlayerPosRotFromBGModel(&pos, &rot, 12);
			createEnemyWork2(0, eENEMY_KIND_MINO_ARMER, &pos);

			sPlayer.SetRotationToTarget(&sppEnemy[0]->Position());
			sppEnemy[0]->SetRotationToTarget(&sPlayer.Position());

			CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_BATTLE_WALL);
			sModelMap.VisibleBattleBox();

			// バトルフラグを立てる
			sEventBattleFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_16, eEVENT_16_RESCUE_USACHU_AFTER);
		
			// リカバリー、またはゲームオーバー時にクリアされるフラグを設定
			sEventRecoveryFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_16, eEVENT_16_RESCUE_USACHU);

			SET_EVENT_END_FLAG();
			return true;
		}
		break;

	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_SetStone1(void)
{
	return eventGeneral_SetObjectStone(15);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_SetStone2(void)
{
	return eventGeneral_SetObjectStone(16);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_RiverIsLower(void)
{
	SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);

	int model_handle, mesh_index;
	model_handle = sModelMap.GetModelHandle();

	mesh_index = MV1GetFrameMesh(model_handle, eBG_TYPE_WATER, 1);
	MV1SetMeshVisible(model_handle, mesh_index, FALSE);

	mesh_index = MV1GetFrameMesh(model_handle, eBG_TYPE_WATER_02, 1);
	MV1SetMeshVisible(model_handle, mesh_index, FALSE);

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_RiverIsUpper(void)
{
	SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);

	int model_handle, mesh_index;
	model_handle = sModelMap.GetModelHandle();

	mesh_index = MV1GetFrameMesh(model_handle, eBG_TYPE_WATER, 1);
	MV1SetMeshVisible(model_handle, mesh_index, TRUE);

	mesh_index = MV1GetFrameMesh(model_handle, eBG_TYPE_WATER_02, 1);
	MV1SetMeshVisible(model_handle, mesh_index, TRUE);

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_WarpCrystalHall(void)
{
	EFK_HANDLE handle;
	VECTOR scale;

	ObjectBase::eObjectType object_type = (ObjectBase::eObjectType)0;
	UCHAR hand_state = 0;

	switch (sEventPhase) {
	case 0:
		SoundManager::GetSndComponent()->PlaySound(eSE_WARP);
		scale = VGet(20.0f, 20.0f, 20.0f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_DISAPPER,
													   &sPlayer.Position(), 
													   &scale);
		sEventPhase ++;
		break;

	case 1:
		//		sPlayer.Scale() = VScale(sPlayer.Scale(), 0.95f);
		if (sEventCount > 60) {
			sPlayer.Scale() = VScale(sPlayer.Scale(), 0.0f);
			FADE_COL_BLACK();
			FADE_OUT(0.75f);
			sEventPhase ++;
		}
		break;

	case 2:
		if (IS_FADE() == false) {
			ActionTaskManager::tTaskInfo param;
			param.mTaskType = ActionTaskManager::eTASK_MOVE;
			param.mTaskValueA = 28;
			param.mTaskValueB = 0;
			ActionTaskManager::GetInstance()->NotifyAction(&param);
			//			return true;
			sEventPhase ++;
		}
		break;

	case 3:
		// dammy
		break;

	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
//bool
//event16_BtlTreasureA(void)
bool
event16_AppearSeasow(void)
{
	bool is_end = false;

	is_end = eventGeneral_AnythingMove();
	if (is_end == true) {

		for (int iI = 0; iI < sObjectBaseNum; iI ++) {
			if ((sppObjectBase[iI]->GetObjectType() == ObjectBase::eOBJECT_TYPE_TREASURE) ||
				(sppObjectBase[iI]->GetObjectType() == ObjectBase::eOBJECT_TYPE_SWITCH)) {
				ObjectBase* p_treasure = (ObjectTresure *)sppObjectBase[iI];
				if (p_treasure->GetHideEventIndex() == sTaskInfo.mTaskValueB) {
					float scale;
					ObjectBase::tObjectBaseParam* p_param;
					p_param = ObjectBase::GetObjectBaseParam(p_treasure->GetObjectKind());
					scale = p_treasure->Scale().x;
					float diff;
					diff = (p_param->mScale - scale) * 0.1f;
					scale += diff;
					if (scale >= (p_param->mScale * 0.95f)) {
						scale = p_param->mScale;
						//						sEventPhase ++;
					}
					p_treasure->Scale() = VGet(scale, scale, scale);
					//					CameraManager::GetInstance()->Target() = p_treasure->Position();
				}
			}
		}




		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_OpenWireA(void)
{
	// カメラを切り替える
	VECTOR framepos;
	int mesh_index = MV1GetFrameMesh(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 1), 0);
	sModelMap.GetMeshMdlPosition(&framepos, mesh_index);

	switch (sEventPhase) {
	case 0:
		SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);
		SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
		CameraManager::GetInstance()->Target() = framepos;
		CameraManager::GetInstance()->Position() = VAdd(framepos, VGet(0.0f, 0.0f, 1500.0f) );
		//		CameraManager::GetInstance()->Position().y = sPlayer.Position().y;
		sEventPhase ++;
		break;

	case 1:
		MV1SetFrameUserLocalMatrix(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 1),
								   MGetTranslate(VGet(0.0f, (sEventCount)/3.0f, 0.0f)));
		if (sEventCount > 90) {
			SoundManager::GetSndComponent()->PlaySound(eSE_FALL_STONE);
			sEventPhase ++;
		}
		break;

	case 2:
		return true;

	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_CloseWireA(void)
{
	// カメラを切り替える
	VECTOR framepos;
	int mesh_index = MV1GetFrameMesh(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 1), 0);
	sModelMap.GetMeshMdlPosition(&framepos, mesh_index);

	switch (sEventPhase) {
	case 0:
		//		SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);
		SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
		CameraManager::GetInstance()->Target() = framepos;
		CameraManager::GetInstance()->Position() = VAdd(framepos, VGet(0.0f, 0.0f, 1500.0f) );
		//		CameraManager::GetInstance()->Position().y = sPlayer.Position().y;
		sEventPhase ++;
		break;

	case 1:
		MV1SetFrameUserLocalMatrix(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 1),
								   MGetTranslate(VGet(0.0f, (90.0f - sEventCount)/3.0f, 0.0f)));
		if (sEventCount > 90) {
			SoundManager::GetSndComponent()->PlaySound(eSE_FALL_STONE);
			sEventPhase ++;
		}
		break;

	case 2:
		return true;

	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_OpenWireB(void)
{
	// カメラを切り替える
	VECTOR framepos;
	int mesh_index = MV1GetFrameMesh(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 2), 0);
	sModelMap.GetMeshMdlPosition(&framepos, mesh_index);

	switch (sEventPhase) {
	case 0:
		SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);
		SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
		CameraManager::GetInstance()->Target() = framepos;
		CameraManager::GetInstance()->Position() = VAdd(framepos, VGet(2000.0f, -500.0f, 0.0f) );
		sEventPhase ++;
		break;

	case 1:
		MV1SetFrameUserLocalMatrix(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 2),
								   MGetTranslate(VGet(0.0f, sEventCount/2.0f, 0.0f)));
		if (sEventCount > 90) {
			sEventPhase ++;
		}
		break;

	case 2:
		{
			StageFunc016* p_stage = (StageFunc016 *)spStageFunc;
			p_stage->StartGateTimer(200);
		}

		return true;

	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_CloseWireB(void)
{
	// カメラを切り替える
	VECTOR framepos;
	int mesh_index = MV1GetFrameMesh(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 2), 0);
	sModelMap.GetMeshMdlPosition(&framepos, mesh_index);

	switch (sEventPhase) {
	case 0:
		//		SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);
		SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
		CameraManager::GetInstance()->Target() = framepos;
		CameraManager::GetInstance()->Position() = VAdd(framepos, VGet(2000.0f, -500.0f, 0.0f) );
		//		CameraManager::GetInstance()->Position().y = sPlayer.Position().y;
		sEventPhase ++;
		break;

	case 1:
		MV1SetFrameUserLocalMatrix(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 2),
								   MGetTranslate(VGet(0.0f, (90.0f - sEventCount)/2.0f, 0.0f)));
		if (sEventCount > 90) {
			SoundManager::GetSndComponent()->PlaySound(eSE_FALL_STONE);
			sEventPhase ++;
		}
		break;

	case 2:
		{
			StageFunc016* p_stage = (StageFunc016 *)spStageFunc;
			p_stage->StartGateTimer(0);
		}

		return true;

	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_SwordTreasure(void)
{
	switch (sEventPhase) {
	case 0:
		if (MSG_IS_FADE() == false) {
			MSG_TYPE_TELOP();
			MSG_FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;
	case 1:
		if (MSG_IS_FADE() == false) sEventPhase ++;
		break;
	case 2:
		RENDER_MESSAGE("", "剣の裏にスイッチがあります", "スイッチを押しますか？",  "",  "");
		Utility::eQuestion qs;
		qs = Utility::ExecuteQuestion();
		if (qs == Utility::eQUESTION_YES)	sEventPhase ++;
		if (qs == Utility::eQUESTION_NO)	return true;	// 終了
		break;

	case 3:
		MSG_FADE_OUT(0.5f);
		sEventPhase ++;
		break;

	case 4:
		if (MSG_IS_FADE() == false) {
			SoundManager::GetSndComponent()->PlaySound(eSE_HOOK_START);
			sEventPhase ++;
		}		
		break;

	case 5:
		if (sEventCount > 60) sEventPhase ++;
		break;

	default:
		return eventGeneral_BattleTreasure(6);
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_UsaemonCome1_B(void)
{
	ModelPmd* p_model = NULL;
	EFK_HANDLE handle;
	VECTOR scale;

	if (sEventPhase >= 2) {
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
	}

	switch (sEventPhase) {
	case 0:
		SoundManager::GetSndComponent()->PlaySound(eSE_LANDING_WATER);
		FADE_OUT(0.5f);
		HIDE_BATTLE_ICON();
		sEventPhase ++;

		break;
	case 1:
		if (IS_FADE() == true) return false;

		ResourceManager::GetInstance()->LoadCharaResource(NAME_Usae);

		EventManager::GetInstance()->LoadCameraFile("data/Camera/dummy.vmd");
		EventManager::GetInstance()->EntryModelNum(1);
		EventManager::GetInstance()->SetModelParameter(0, NAME_Usae, 0, &sModelMap, false);
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->EnableToonShader(0.02f, 0.02f);

		{
			VECTOR vec, tmpA, tmpB;
			float dist;
			vec = VNorm(VSub(ZERO_VEC, sPlayer.Position()));
			p_model->Position() = VAdd(sPlayer.Position(), VScale(vec, 600.0f));
			p_model->SetRotationToTarget(&sPlayer.Position());
			p_model->SetVisible(false);

			tmpA = sPlayer.Position();
			tmpA.y += (sPlayer.BodyHeight() * 0.25f);
			tmpB = p_model->Position();
			tmpB.y += (sPlayer.BodyHeight() * 0.65f);

			vec = VSub(tmpB, tmpA);
			dist = VSize(vec);
			tmpA = VAdd(tmpA, VScale(VNorm(vec), (dist * 0.475f)));
			
			CameraManager::GetInstance()->Position() = tmpA;
			CameraManager::GetInstance()->Target() = tmpB;
		}

		sEventCsv.LoadFile("data/CSV/event16_01.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		/* アイテムを全て表示にさせる(StartEvent にて非表示にされる為) */
		visibleObjectAll(true);

		FADE_IN(0.5f);

		sEventPhase ++;
		break;
	case 2:
		if (IS_FADE() == true) return false;

		SoundManager::GetSndComponent()->PlaySound(eSE_HOLY);
		scale = VScale(p_model->Scale(), 3.5f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_HEAL, 
													   &p_model->Position(), 
													   &scale);
		scale = VScale(p_model->Scale(), 1.0f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_DISAPPER, 
													   &p_model->Position(), 
													   &scale);
		sEventPhase ++;
		break;

	case 3:
		if (sEventCount > 70.0f) {
			p_model->SetVisible(true);
			sEventPhase ++;
		}
		break;

	case 4:
		if (sEventCount > 120.0f) {
			sEventPhase ++;
		}
		break;
		
	case 5:
		MSG_TYPE_MESSAGE();
		SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
		MSG_FADE_IN(0.3f);
		sEventPhase ++;
		break;

	case 6:
		RENDER_MESSAGE("森の神", "確かこの森のどこかに", "大岩を砕く力が隠されていたはずだ", "湖の西側にある『石碑』にそのヒントが書かれている", "");
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;
		
	case 7:
		if (IS_FADE() == false) {
			RENDER_MESSAGE("森の神", "古城の奥にいる嫁をここまで連れてきてくれたら", "クリスタルの場所を案内しよう", "それでは･･･嫁を頼む！", "");
			if (CHECK_TRIG_ANY_JOY()) {
				SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
				MSG_FADE_OUT(0.3f);
				sEventPhase = 16;
			}
		}
		break;

	case 16:
		SoundManager::GetSndComponent()->PlaySound(eSE_HOLY);
		scale = VScale(p_model->Scale(), 3.5f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_HEAL, 
													   &p_model->Position(), 
													   &scale);
		scale = VScale(p_model->Scale(), 1.0f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_DISAPPER, 
													   &p_model->Position(), 
													   &scale);
		sEventPhase ++;
		break;

	case 17:
		if (sEventCount > 70.0f) {
			p_model->SetVisible(false);
			sEventPhase = 99;
		}
		break;

	case 99:
		VISIBLE_BATTLE_ICON();
		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();

		// フラグをたてない
		// SET_EVENT_END_FLAG();

		return true;

	};
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_RescueUsachuAfter(void)
{
	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		SoundManager::GetInstance()->StopBGM();

		/*===================================== *
		 * NPC の割り当てを変更させる
		 *===================================== */
		spNpc = CharaNpc::RecreateNpcInstance(1, spNpc, sNpcNum, sStageIndex, spBGInfo->mScale);

		sEventPhase ++;
		break;

	case 1:
		if (IS_FADE() == false) {
			VECTOR pos;	float rot;
			getPlayerPosRotFromBGModel(&pos, &rot, 13);
			ObjectBase* p_object = search1stFindObjectKind(eOBJECT_KIND_USACHU);
			p_object->Position() = pos;

			getPlayerPosRotFromBGModel(&pos, &rot, 14);
			sPlayer.Position() = pos;

			sPlayer.SetRotationToTarget(&p_object->Position());
			p_object->SetRotationToTarget(&sPlayer.Position());

			CameraManager::GetInstance()->SetPatternCharaBase((ModelCollision *)p_object);
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_MESSAGE);

			FADE_IN(0.5f);

			sEventPhase ++;
		}
		break;

	case 2:
		if (IS_FADE() == false) {
			SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);

			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
			MSG_FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;

	case 3:
		if (MSG_IS_FADE() == false) sEventPhase ++;
		break;

	case 4:
		RENDER_MESSAGE("うさえもんの嫁", "助けてくれてありがとう！", "家に帰りたいちゅーっ！",  "",  "");
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;

	case 5:
		RENDER_MESSAGE("うさえもんの嫁", "私を持ち上げて、湖の家まで",  "連れてって欲しいでちゅーっ！", "", "");
		if (CHECK_TRIG_ANY_JOY()) {
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
			MSG_FADE_IN(0.3f);

			FADE_OUT(0.3f);
			sEventPhase ++;
		}
		break;

	case 6:
		if (IS_FADE() == false) {

			VISIBLE_BATTLE_ICON();
			SET_EVENT_END_FLAG();

			return true;
		}

	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_RevertUsachu(void)
{
	switch (sEventPhase) {
	case 0:
		if (MSG_IS_FADE() == false) {
			MSG_TYPE_MESSAGE();
			MSG_FADE_IN(0.3f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
			sEventPhase ++;
		}
		break;

	case 1:
		if (MSG_IS_FADE() == false) {
			sEventPhase ++;
		}
		break;
		
	case 2:
		// 既に運ぶ済の場合
		if (IS_END_EVT(eEVENT_16, eEVENT_16_USAEMON_COME_2)) {
			RENDER_MESSAGE("きつーね", "あ、もううさちゅうを湖まで運んでくれたんだね！", "ありがとう！",  "",  "");
			if (CHECK_TRIG_ANY_JOY()) {
				SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
				MSG_FADE_OUT(0.3f);
				return true;
			}
		}
		// それ以外
		else {
			RENDER_MESSAGE("きつーね", "うさちゅうをこの場所に戻す？", "",  "",  "");
			Utility::eQuestion qs;
			qs = Utility::ExecuteQuestion();
			if (qs == Utility::eQUESTION_YES) {
				FADE_OUT(0.5f);
				sEventPhase ++;
			}
			if (qs == Utility::eQUESTION_NO)	return true;	// 終了
		}
		break;

	case 3:
		if (IS_FADE() == false) {
			SoundManager::GetSndComponent()->PlaySound(eSE_WARP);

			VECTOR pos;	float rot;
			getPlayerPosRotFromBGModel(&pos, &rot, 13);
			ObjectBase* p_object = search1stFindObjectKind(eOBJECT_KIND_USACHU);
			p_object->Position() = pos;
			EffekseerManager::GetInstance()->Play(eEFK_HEAL, &pos, 20.0f);

			return true;
		}
		break;
		
	default:
		break;
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_HelpUsachu(void)
{
	switch (sEventPhase) {
	case 0:
		MSG_TYPE_MESSAGE();

		//		SoundManager::GetInstance()->StopBGM();
		SoundManager::GetInstance()->SetVolumeBGM(20);
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;

	case 1:
		if (IS_FADE() == false) {
			VECTOR pos;	float rot;
			getPlayerPosRotFromBGModel(&pos, &rot, 13);
			ObjectBase* p_object = search1stFindObjectKind(eOBJECT_KIND_USACHU);

			CameraManager::GetInstance()->Target() = p_object->Position();
			CameraManager::GetInstance()->Position() = VAdd(p_object->Position(), VGet(4000.0f, -3000.0f, 4000.0f));
			FADE_IN(0.5f);

			sEventPhase ++;
		}
		break;

	case 2:
		if (IS_FADE() == false) {
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
			MSG_FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;

	case 3:
		if (MSG_IS_FADE() == false) sEventPhase ++;
		break;

	case 4:
		RENDER_MESSAGE("？？？", "きゃーっ！！", "誰か助けてーッ！",  "",  "");
		if (CHECK_TRIG_ANY_JOY()) {
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			MSG_FADE_OUT(0.3f);
			sEventPhase ++;
		}
		break;

	case 5:
		if (sEventCount > 90) {
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
			MSG_FADE_IN(0.3f);
			CameraManager::GetInstance()->SetPatternGameBegine();
			sEventPhase ++;
		}
		break;

	case 6:
		if (MSG_IS_FADE() == false) {
			sEventPhase ++;
		}
		break;

	case 7:
		RENDER_MESSAGE("ナビォ", "リノ！女の子の声だ！",  "城壁の上から聞こえたよ！", "", "");
		if (CHECK_TRIG_ANY_JOY()) {
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			MSG_FADE_OUT(0.3f);
			sEventPhase ++;
		}
		break;

	case 8:
		if (MSG_IS_FADE() == false) {
			SoundManager::GetInstance()->SetVolumeBGM(kSOUND_VOLUME_DEFAULT);

			VISIBLE_BATTLE_ICON();
			SET_EVENT_END_FLAG();
			return true;
		}
		break;

	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_SetStone3(void)
{
	return eventGeneral_SetObjectStone(17);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event16_UsaemonCome3(void)
{
	ModelPmd* p_model = NULL;
	EFK_HANDLE handle;
	VECTOR scale;

	if (sEventPhase >= 2) {
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
	}

	switch (sEventPhase) {
	case 0:
		SoundManager::GetSndComponent()->PlaySound(eSE_LANDING_WATER);
		FADE_OUT(0.5f);
		HIDE_BATTLE_ICON();
		sEventPhase ++;

		break;
	case 1:
		if (IS_FADE() == true) return false;

		ResourceManager::GetInstance()->LoadCharaResource(NAME_Usae);

		EventManager::GetInstance()->LoadCameraFile("data/Camera/dummy.vmd");
		EventManager::GetInstance()->EntryModelNum(1);
		EventManager::GetInstance()->SetModelParameter(0, NAME_Usae, 0, &sModelMap, false);
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->EnableToonShader(0.02f, 0.02f);

		{
			VECTOR vec, tmpA, tmpB;
			float dist;
			vec = VNorm(VSub(ZERO_VEC, sPlayer.Position()));
			p_model->Position() = VAdd(sPlayer.Position(), VScale(vec, 600.0f));
			p_model->SetRotationToTarget(&sPlayer.Position());
			p_model->SetVisible(false);

			tmpA = sPlayer.Position();
			tmpA.y += (sPlayer.BodyHeight() * 0.25f);
			tmpB = p_model->Position();
			tmpB.y += (sPlayer.BodyHeight() * 0.65f);

			vec = VSub(tmpB, tmpA);
			dist = VSize(vec);
			tmpA = VAdd(tmpA, VScale(VNorm(vec), (dist * 0.475f)));
			
			CameraManager::GetInstance()->Position() = tmpA;
			CameraManager::GetInstance()->Target() = tmpB;
		}

		sEventCsv.LoadFile("data/CSV/event16_01.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		/* アイテムを全て表示にさせる(StartEvent にて非表示にされる為) */
		visibleObjectAll(true);

		FADE_IN(0.5f);

		sEventPhase ++;
		break;
	case 2:
		if (IS_FADE() == true) return false;

		SoundManager::GetSndComponent()->PlaySound(eSE_HOLY);
		scale = VScale(p_model->Scale(), 3.5f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_HEAL, 
													   &p_model->Position(), 
													   &scale);
		scale = VScale(p_model->Scale(), 1.0f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_DISAPPER, 
													   &p_model->Position(), 
													   &scale);
		sEventPhase ++;
		break;

	case 3:
		if (sEventCount > 70.0f) {
			p_model->SetVisible(true);
			sEventPhase ++;
		}
		break;

	case 4:
		if (sEventCount > 120.0f) {
			sEventPhase ++;
		}
		break;
		
	case 5:
		MSG_TYPE_MESSAGE();
		SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
		MSG_FADE_IN(0.3f);
		sEventPhase ++;
		break;

	case 6:
		RENDER_MESSAGE("森の神", "私は今うさちゅうとニャンニャンしているところだ", "", "", "");
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;
		
	case 7:
		if (IS_FADE() == false) {
			RENDER_MESSAGE("森の神", "邪魔しないで頂きたい！", "", "", "");
			if (CHECK_TRIG_ANY_JOY()) {
				SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
				MSG_FADE_OUT(0.3f);
				sEventPhase = 16;
			}
		}
		break;

	case 16:
		SoundManager::GetSndComponent()->PlaySound(eSE_HOLY);
		scale = VScale(p_model->Scale(), 3.5f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_HEAL, 
													   &p_model->Position(), 
													   &scale);
		scale = VScale(p_model->Scale(), 1.0f);
		handle = EffekseerManager::GetInstance()->Play(eEFK_DISAPPER, 
													   &p_model->Position(), 
													   &scale);
		sEventPhase ++;
		break;

	case 17:
		if (sEventCount > 70.0f) {
			p_model->SetVisible(false);
			sEventPhase = 99;
		}
		break;

	case 99:
		VISIBLE_BATTLE_ICON();
		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();

		// フラグをたてない
		// SET_EVENT_END_FLAG();

		return true;

	};
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event21_NaviExplain(void)
{
	ModelPmd* p_model = NULL;
	bool is_end = false;
	VECTOR pos;
	static const float sDiff = 50.0f;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;

	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();

		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		ResourceManager::GetInstance()->LoadCharaResource(NAME_Kitsune);

		EventManager::GetInstance()->LoadCameraFile("data/Camera/021_00_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(3);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino, eMT_021_00_EVENT);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Kitsune, 1);
		EventManager::GetInstance()->SetModelParameter(2, NAME_Navi, eMT_NAVI_STAGE22_START);
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->DisableToonShader();
		EventManager::GetInstance()->GetModelPmd(0)->DisableShadow();

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawKitsuneFunc);
		p_model->DisableToonShader();
		EventManager::GetInstance()->GetModelPmd(1)->DisableShadow();
		//		p_model->EnableToonShader(0.001f, 0.002f);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
		eventGeneral_NaviLuminusStart(p_model);

		CameraManager::GetInstance()->SetFarClip(DEFAULT_FAR_CLIP * 1.2f);
		sModelXfile[eMD_SKYDOME].Scale()	= VGet(610.0f, 610.0f, 610.0f);

		sEventCsv.LoadFile("data/CSV/event21.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */

		if (EventManager::GetInstance()->EqualPhaseFrame(360.0f)) {
			sPlayer.Scale() = ZERO_VEC;
			sPlayer.StartMotionEx(&sMotionParam[eMT_IDLE_NOWEAPON]);
		} else if (EventManager::GetInstance()->BetweenPhaseFrame(0.0f, 360.0f)) {
			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
			p_model->GetFramePosFromName(&pos, "頭");
			pos.y += (sDiff * ALL_SCALE);
			sPlayer.Position() = pos;
			sPlayer.Rotation().y = Utility::DegToRad(90.0f);
			sPlayer.StartMotion(eMT_SLEEP, false, 1.0f, NULL);
		}

		is_end = updateCsvAction(&sEventCsv);

		eventGeneral_NaviLuminusUpdate( (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2) );

		if (is_end) {
			FADE_COL_BLACK();
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		/* スタートを押すと外れてしまうため、例外処理 */
		if (sPlayer.Scale().x > 0.0f) {
			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
			p_model->GetFramePosFromName(&pos, "頭");
			pos.y += (sDiff * ALL_SCALE);
			sPlayer.Position() = pos;
			sPlayer.Rotation().y = Utility::DegToRad(90.0f);
		}

		if (IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();

		SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);
		
		CameraManager::GetInstance()->SetDefaultNearFarClip();
		sModelXfile[eMD_SKYDOME].Scale()	= VGet(600.0f, 600.0f, 600.0f);

		sStagePlaceIndex = 8;
		//		sStagePlaceIndex = 0;
		VECTOR pos;
		float rot;
		getPlayerPosRotFromBGModel(&pos, &rot, sStagePlaceIndex);
		sPlayer.Position() = pos;
		sPlayer.Rotation().y = rot;
		sPlayer.SetPositionOnCollision();
		sPlayer.Position().y = -994.5f; // もう直値でいいや

		//		sPlayer.StartMotionEx(&sMotionParam[eMT_IDLE_NOWEAPON]);
		sPlayer.StartMotion(eMT_IDLE_NOWEAPON, true, 0.5f, NULL, 0.9f);

		VISIBLE_BATTLE_ICON();
		SET_EVENT_END_FLAG();
		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event21_BattleQuest1(void)
{
	// 現在何回戦かを調べる
	int clear_match_num = (TotalInfo::GetInstance()->GetFieldInfoFlagBit(sStageIndex) & kFIELD_INFO_XX_ARENA_MASK);

	return eventXX_BattleQuest(eBATTLE_QUEST_RANK_A, clear_match_num);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event21_BattleQuest1_End(void)
{
	// 現在何回戦かを調べる
	int clear_match_num = (TotalInfo::GetInstance()->GetFieldInfoFlagBit(sStageIndex) & kFIELD_INFO_XX_ARENA_MASK);

	return eventXX_BattleQuest_End(eBATTLE_QUEST_RANK_A, clear_match_num);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event21_MoveToEarth(void)
{
	if (IS_END_EVT(eEVENT_14, eEVENT_14_WILL_TALK)) {
		ActionTaskManager::tTaskInfo param = {ActionTaskManager::eTASK_MOVE, 10, 999};
		ActionTaskManager::GetInstance()->NotifyAction(&param);
		
	}
	else {
		const char* strTalk[4] = {"リノ！", "リベルニアでやらないといけない事があるよ！",  "まだ外にへ行けないよ！",  ""};
		return eventGeneral_BlockCharactor(0, 8,
										   "ナビォ", (char **)strTalk);
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event21_MoveToColleage(void)
{
	if (IS_END_EVT(eEVENT_29, eEVENT_29_ABOUT_MARUIFORCE)) {
		ActionTaskManager::tTaskInfo param = {ActionTaskManager::eTASK_MOVE, 14, 0};
		ActionTaskManager::GetInstance()->NotifyAction(&param);
		
	}
	else {
		const char* strTalk[4] = {"リノ！", "やらないといけない事があるだろ？",  "寄り道してる場合じゃないよ！",  ""};
		return eventGeneral_BlockCharactor(0, &VGet(0.0f, 0.0f, -450.0f), 0.0f, 
										   "ナビォ", (char **)strTalk);
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event21_BinShop(void)
{
	switch (sEventPhase) {
	case 0:
		if (MSG_IS_FADE() == false) {
			MSG_TYPE_TELOP();
			MSG_FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;
	case 1:
		if (MSG_IS_FADE() == false) sEventPhase ++;
		break;

	case 2:
		{
			RENDER_MESSAGE("", "空きビンを100ルピーで買いますか？", "",  "",  "");

			Utility::eQuestion qs;
			qs = Utility::ExecuteQuestion(false);
			if (qs == Utility::eQUESTION_YES) {
				//				sEventPhase ++;

				// 既に持っている場合
				if (TotalInfo::GetInstance()->GetFieldInfoFlagBit(sStageIndex) & kFIELD_INFO_21_BUY_BIN_BIT) {
					MSG_TYPE_MESSAGE();
					SoundManager::GetSndComponent()->PlaySound(eSE_NG);
					sEventPhase = 3;
				}
				// お金が足りない場合
				else if (TotalInfo::GetInstance()->GetNowMoney() < 100) {
					MSG_TYPE_MESSAGE();
					SoundManager::GetSndComponent()->PlaySound(eSE_NG);
					sEventPhase = 4;
				}
				// 購入ケース
				else {
					MSG_TYPE_TELOP();
					SoundManager::GetSndComponent()->PlaySound(eSE_GETITEM);
					TotalInfo::GetInstance()->AddMoney(-100);
					sEventPhase = 5;
				}
			}
			if (qs == Utility::eQUESTION_NO) {
				SoundManager::GetSndComponent()->PlaySound(eSE_CANCEL);
				return true;	// 終了
			}
		}
		break;
	case 3:
		RENDER_MESSAGE("お店のおばちゃん", "おや...うちのビンをもう持ってるじゃない", "うちはお一人様１つしか渡せないんだわ",  "すまないねぇ",  "");
		if (CHECK_TRIG_ANY_JOY()) return true; // 終了

		break;

	case 4:
		RENDER_MESSAGE("お店のおばちゃん", "おや...お金が足りないよ", "",  "",  "");
		if (CHECK_TRIG_ANY_JOY()) return true; // 終了

		break;

	case 5:
		{
			char msgA[64];
			const tItemDetailInfo* p_info = GetItemDetailInfo(eITEM_KIND_BIN_EMPTY);

			sprintf_s(msgA, sizeof(msgA), "%sを手に入れた！", p_info->mDispName);
			RENDER_MESSAGE("", msgA, 
						   p_info->mExplain1,
						   p_info->mExplain2,
						   "");
			if (CHECK_TRIG_ANY_JOY()) {
				addMyWeapon(eITEM_KIND_BIN_EMPTY);
				TotalInfo::GetInstance()->AddItemNum(eSUBWEAPON_NUM_BIN_EMPTY, 1, true);

				TotalInfo::GetInstance()->SetEnableFieldInfoFlag(sStageIndex, kFIELD_INFO_21_BUY_BIN_BIT);

				return true; // 終了
			}
		}

		break;

	default:
		break;
	}

	return false;

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event22_Wakeup(void)
{
	ModelPmd* p_model = NULL;
	bool is_end = false;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;

	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();

		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		spNpc[0].Scale() = ZERO_VEC;
		spNpc[0].DisableShadow();

		sModelMap.Scale() = VGet(1.0f, 1.0f, 1.0f);

		sModelMap.SetVisible(false, eBG_TYPE_GIMMICK + 0);

		EventManager::GetInstance()->LoadCameraFile("data/Camera/022_00_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(2);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino, eMT_KITCHEN_EVENT);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Navi, 1);
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */

		p_model->SetMeshVisible(false,  kBAG_FRAME_INDEX);
		p_model->SetMeshVisible(false,  kKNIFE_FRAME_INDEX);
		p_model->SetMeshVisible(false,  kHAT_FRAME_INDEX);
		p_model->SetMeshVisible(false,	kSHEATH_FRAME_INDEX);
		p_model->SetMeshVisible(false,	kHILT_FRAME_INDEX);
		p_model->EnableToonShader();
		EventManager::GetInstance()->GetModelPmd(1)->DisableShadow();	// 影非表示

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusStart(p_model);

		sEventCsv.LoadFile("data/CSV/event22.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction(&sEventCsv);

		eventGeneral_NaviLuminusUpdate( (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1) );

		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();

		sPlayer.Scale() = CHARA_DEF_SCALE;
		spNpc[0].Scale() = CHARA_DEF_SCALE;

		sModelMap.Scale() = VGet(spBGInfo->mScale, spBGInfo->mScale, spBGInfo->mScale);

		sPlayer.EnableShadow();
		spNpc[0].BodySize() *= 0.2f;
		spNpc[0].EnableShadow();
		VECTOR tmp;
		spNpc[0].GetFramePosFromName(&tmp, "首");
		tmp.y = 192.0f;
		EffectManager::GetInstance()->Play(eEF_NAVI,
										   &tmp,
										   &VScale(spNpc[0].Scale(), 1.5f),
										   0.0f);


		sModelMap.SetVisible(true, eBG_TYPE_GIMMICK + 0);
		
		VISIBLE_BATTLE_ICON();

		//		CameraManager::GetInstance()->SetDefaultNearFarClip();

		SET_EVENT_END_FLAG();
		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event22_GeneralOcarina(void)
{
	switch (sEventPhase) {
	case 0:


		break;

	case 1:
		break;

	default:
		break;
	};

	return false;
}


//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool event22_MoveToHomeOut(void)
{
	if (IS_END_EVT(eEVENT_21, eEVENT_21_NAVI_EXPLAIN)) {
		return eventGeneral_MovePlace(0, 3);
	}
	else {
		const char* strTalk[4] = {"出かける前にナビォに話しかけてみよう！", "",  "",  ""};
		return eventGeneral_BlockCharactor(0, &VGet(0.0f, 0.0f, -100.0f), 180.0f, 
										   "", (char **)strTalk);
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool event22_MoveToStairUp(void)
{
	if (IS_END_EVT(eEVENT_21, eEVENT_21_NAVI_EXPLAIN)) {
		return eventGeneral_MovePlace(0, 5);
	}
	else {
		const char* strTalk[4] = {"２階には武器になりそうな物はない", "",  "",  ""};
		return eventGeneral_BlockCharactor(0, &VGet(0.0f, 0.0f, 100.0f), 270.0f, 
										   "", (char **)strTalk);
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event24_WelcomeWaterway(void)
{
	ModelPmd* p_model = NULL;
	bool is_end = false;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		EventManager::GetInstance()->LoadCameraFile("data/Camera/024_00_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(2);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino, eMT_IDLE, &sModelMap);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Navi, eMT_NAVI_024_00_WELCOM_WATERWAY, NULL);
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();
		p_model->Position() = sPlayer.Position();
		p_model->Rotation() = sPlayer.Rotation();

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusStart(p_model);

		sEventCsv.LoadFile("data/CSV/event24.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		startCsvAction2nd();
		
		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);

		eventGeneral_NaviLuminusUpdate( (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1) );

		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();
		SET_EVENT_END_FLAG();
		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool	event24_Dummy01(void) {return false;}
bool	event24_Dummy02(void) {return false;}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event24_WaterIsLower(void)
{
	switch (sEventPhase) {
	case 0:
		SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);
		sEventPhase ++;
		break;
	case 1:
		sModelMap.WaterOffsetPos() += -0.04f;
		if (sModelMap.WaterOffsetPos() < sEvt24_WaterCheckHeight) {
			sModelMap.WaterOffsetPos() = sEvt24_WaterStopHeight;

			sModelMap.SetFramePosFromIndex(eBG_TYPE_GIMMICK, &VGet(0.0f, sEvt24_WaterStopHeight, 0.0f));
			return true;
		}
		break;
	default:
		break;
	};
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event24_WaterIsUpper(void)
{
	switch (sEventPhase) {
	case 0:
		SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);
		sModelMap.WaterOffsetPos() = sEvt24_WaterCheckHeight;
		sEventPhase ++;
		break;
	case 1:
		sModelMap.WaterOffsetPos() += 0.04f;
		if (sModelMap.WaterOffsetPos() >= 0.0f) {
			sModelMap.WaterOffsetPos() = 0.0f;

			sModelMap.SetFramePosFromIndex(eBG_TYPE_GIMMICK, (VECTOR *)&ZERO_VEC);

			return true;
		}
		break;
	default:
		break;
	};
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event24_ClearBarrier(void)
{
	return eventGeneral_ClearBarrier(eBG_TYPE_GIMMICK + 1, 0);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event24_WarpEntrance(void)
{
	switch (sEventPhase) {
	case 0:
		if (MSG_IS_FADE() == false) {
			MSG_TYPE_TELOP();
			MSG_FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;
	case 1:
		if (MSG_IS_FADE() == false) sEventPhase ++;
		break;
	case 2:
		RENDER_MESSAGE("", "地下水路の入り口へ戻りますか？", "",  "",  "");
		Utility::eQuestion qs;
		qs = Utility::ExecuteQuestion();
		if (qs == Utility::eQUESTION_YES) {
			MSG_FADE_OUT(0.3f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			sEventPhase ++;
		} else if (qs == Utility::eQUESTION_NO)	{
			return true;	// 終了
		}
		break;
	case 3:
		if (MSG_IS_FADE() == false) {
			SoundManager::GetSndComponent()->PlaySound(eSE_WARP);
			FADE_OUT(0.75f);
			sEventPhase ++;
		}
		break;
	case 4:
		if (IS_FADE() == false) {
			getPlayerPosRotFromBGModel(&sPlayer.Position(), &sPlayer.Rotation().y, 0);
			FADE_IN(0.75f);
			return true; // 終了
		}

		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event24_BlockToCastle(void)
{
	if (IS_END_EVT(eEVENT_25, eEVENT_25_LEA_TALK)) {
		ActionTaskManager::tTaskInfo param = {ActionTaskManager::eTASK_MOVE, 29, 28};
		ActionTaskManager::GetInstance()->NotifyAction(&param);

	}
	else {
		const char* strTalk[4] = {"リノ！", "まずはエルダ様を助けないと！",  "",  ""};

		return eventGeneral_BlockCharactor(0, &VGet(450.0f, 0.0f, 0.0f), 90.0f, 
										   "ナビォ", (char **)strTalk);
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event25_LeaTalk(void)
{
	CharaNpc* p_model = NULL;
	bool is_end = false;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();
		
		spNpc[0].Scale() = ZERO_VEC;
		spNpc[0].DisableShadow();

		EventManager::GetInstance()->LoadCameraFile("data/Camera/025_00_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(3);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino, eMT_IDLE, &sModelMap);
		//		EventManager::GetInstance()->SetModelParameter(1, NAME_Elda, eMT_LEA_PRISON_TALK, &sModelMap);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Elda, eMT_LEA_PRISON_TALK, &sModelMap);
		p_model = EventManager::GetInstance()->GetModelPmd(1);
		p_model->EnableToonShader(0.02f, 0.01f);
		p_model->DisableShadow();

		p_model = EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();
		p_model->DisableShadow();
		
		sEventCsv.LoadFile("data/CSV/event25.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		startCsvAction2nd();

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		if (updateCsvAction2nd(&sEventCsv) ) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;
		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();

		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();

		spNpc[0].Scale() = CHARA_DEF_SCALE;
		spNpc[0].EnableShadow();

		VISIBLE_BATTLE_ICON();
		SET_EVENT_END_FLAG();
		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event26_BossAppear(void)
{
	ModelPmd* p_model = NULL;
	bool is_end = false;
	VECTOR set_pos;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		EventManager::GetInstance()->LoadCameraFile("data/Camera/026_00_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(3);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino, 		eMT_026_00_EVENT, &sModelMap);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Hatophon, 	eMT_HATOPHON_026_000_EVENT, &sModelMap);
		EventManager::GetInstance()->SetModelParameter(2, NAME_Navi, 		eMT_NAVI_026_00_EVENT, NULL);
		p_model = EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
		eventGeneral_NaviLuminusStart(p_model);

		sEventCsv.LoadFile("data/CSV/event26.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction(&sEventCsv);

		eventGeneral_NaviLuminusUpdate( (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2) );

		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();

		p_model = EventManager::GetInstance()->GetModelPmd(0);
		p_model->GetFramePosFromName(&set_pos, "センター");
		sPlayer.Position() = set_pos;
		p_model = EventManager::GetInstance()->GetModelPmd(1);
		p_model->GetFramePosFromName(&set_pos, "センター");
		set_pos.y = sPlayer.Position().y;	// 高さをあわせる

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();

		VISIBLE_BATTLE_ICON();
		createEnemyWork2(0, eENEMY_KIND_HATOPHON, &set_pos);
		CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_BATTLE_WALL);
		sModelMap.VisibleBattleBox();

		SoundManager::GetInstance()->PlayBGM(BGM_BOSS_A);

		// バトルフラグを立てる
		sEventBattleFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_26, eEVENT_26_END_BOSS_BATTLE);
		
		// リカバリー、またはゲームオーバー時にクリアされるフラグを設定
		sEventRecoveryFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_26, eEVENT_26_BOSS_APPEAR);

		SET_EVENT_END_FLAG();

		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event26_ClearBarrier(void)
{
	bool is_end = eventGeneral_ClearBarrier(eBG_TYPE_GIMMICK + 0, 0);
	if (is_end == true) {
		/****  再生のBGM ****/
		SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);

		SET_EVENT_END_FLAG();

		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event26_EndBossBattle(void)
{
	//	mPhase = ePHASE_STAGECLEAR;
	sDungeonIndex = 0;
	spScenBase->SetPhase(SceneMainGame::ePHASE_STAGECLEAR);

	// バトルフラグを立てる
	sEventBattleFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_26, eEVENT_26_CLEAR_BARRIER);

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event28_BossAppear(void)
{
	static const VECTOR offset = VScale(ZERO_VEC, 10.0f);
	static const int model_num = 3;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	float now_frame = EventManager::GetInstance()->GetPhaseFrame();

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		ResourceManager::GetInstance()->LoadCharaResource(NAME_RedDrag);

		EventManager::GetInstance()->LoadCameraFile("data/Camera/028_00_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON, &sModelMap, true);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Navi, 0, NULL, true);
		EventManager::GetInstance()->SetModelParameter(2, NAME_RedDrag, 0, NULL, true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1); // Navi
		eventGeneral_NaviLuminusStart(p_model);

		sEventCsv.LoadFile("data/CSV/event28_00.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv, &offset);

		/* アイテムを全て表示にさせる(StartEvent にて非表示にされる為) */
		visibleObjectAll(true);

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction(&sEventCsv);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusUpdate(p_model);

		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();

		{
			GraphicManager::GetInstance()->EnableFogPattern(255, 214, 7, 100.0f, 24000.0f);
			GraphicManager::GetInstance()->BackupFogParam();
		}

		VISIBLE_BATTLE_ICON();
		{
			VECTOR pos;	float rot;
			getPlayerPosRotFromBGModel(&pos, &rot, 1);
			createEnemyWork2(0, eENEMY_KIND_RED_DRAGON, &pos);

			getPlayerPosRotFromBGModel(&pos, &rot, 2);
			sPlayer.Position() = pos;
			sPlayer.Position().x += 100.0f;
		}
		sPlayer.SetRotationToTarget(&sppEnemy[0]->Position());
		sppEnemy[0]->SetRotationToTarget(&sPlayer.Position());

		CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_BATTLE_WALL);
		sModelMap.VisibleBattleBox();

		SoundManager::GetInstance()->PlayBGM(BGM_BOSS_A);

		// バトルフラグを立てる
		sEventBattleFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_28, eEVENT_28_END_BOSS_BATTLE);
		
		// リカバリー、またはゲームオーバー時にクリアされるフラグを設定
		sEventRecoveryFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_28, eEVENT_28_BOSS_APPEAR);

		SET_EVENT_END_FLAG();

		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event28_EndBossBattle(void)
{
	//	mPhase = ePHASE_STAGECLEAR;
	sDungeonIndex = 2;
	spScenBase->SetPhase(SceneMainGame::ePHASE_STAGECLEAR);

	// バトルフラグを立てる
	sEventBattleFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_28, eEVENT_28_BTL_TREASURE_A);

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event28_BtlTreasureA(void)
{
	bool is_end;

	StageFunc028* p_stage = (StageFunc028 *)spStageFunc;

	if (sEventPhase == 0) {
		ObjectBase* p_object = search1stFindObjectKind(eOBJECT_KIND_CRYSTAL);
		APP_ASSERT( (p_object != NULL), "Not Found eOBJECT_KIND_CRYSTAL \n");
		processAppearCrystal(2,			// stagePlaceIndex
							 p_object,	// pCrystalObject
							 0,			// crystalColor : 0 : Green
							 TRUE,
							 &p_stage->mEffHndAura,
							 &p_stage->mEffHndCrystal);
	}

	is_end = eventGeneral_BattleTreasure();
	if (is_end == true) {

		GraphicManager::tFogParam fogParam;
		fogParam.mIsEnable = TRUE;
		fogParam.mIsActive = TRUE;
		fogParam.mColR = 0;
		fogParam.mColG = 0;
		fogParam.mColB = 0;
		fogParam.mNear = 100.0f;
		fogParam.mFar = 8000.0f;
		GraphicManager::GetInstance()->ChangeFogParam(1.0f, &fogParam);
		GraphicManager::GetInstance()->BackupFogParam();

		 /****  再生のBGM ****/
		SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);
		
		SET_EVENT_END_FLAG();
		
		return true;
	}
	return false;
}

 //---------------------------------------------------
 /**

  */
 //---------------------------------------------------
 bool
 event29_WelcomeCastle(void)
 {
	 ModelPmd* p_model = NULL;
	 bool is_end = false;

	 switch (sEventPhase) {
	 case 0:
		 FADE_OUT(0.5f);
		 sEventPhase ++;
		 break;
	 case 1:
		 if (IS_FADE() == true) return false;

		 HIDE_BATTLE_ICON();
		 sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		 sPlayer.Scale() = ZERO_VEC;
		 sPlayer.DisableShadow();

		 ResourceManager::GetInstance()->LoadCharaResource(NAME_DarkWizard);
		 ResourceManager::GetInstance()->LoadCharaResource(NAME_MobClockW);

		 EventManager::GetInstance()->LoadCameraFile("data/Camera/029_00_Camera.vmd");
		 EventManager::GetInstance()->EntryModelNum(4);
		 EventManager::GetInstance()->SetModelParameter(0, Name_Lino, eMT_029_00_EVENT, &sModelMap);
		 EventManager::GetInstance()->SetModelParameter(1, NAME_Navi, eMT_NAVI_029_00_WELCOM_CASTLE, NULL);
		 EventManager::GetInstance()->SetModelParameter(2, NAME_DarkWizard, eMT_WIZARD_029_00_WELCOM_CASTLE, NULL);
		 EventManager::GetInstance()->SetModelParameter(3, NAME_MobClockW, eMT_CLOCK_029_00_WELCOM_CASTLE, NULL);
		 p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		 p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		 p_model->EnableToonShader();

		 sEventCsv.LoadFile("data/CSV/event29_00.csv");
		 EventManager::GetInstance()->StartEvent(&sEventCsv);

		 p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		 eventGeneral_NaviLuminusStart(p_model);

		 FADE_IN(0.5f);
		 sEventPhase ++;
		 break;

	 case 2:
		 /* CSVファイルのアクションを更新 */
		 is_end = updateCsvAction(&sEventCsv);

		 p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		 eventGeneral_NaviLuminusUpdate(p_model);

		 if (is_end) {
			 FADE_OUT(0.5f);
			 sEventPhase ++;
		 }
		 break;

	 case 3:
		 if (IS_FADE() == true) return false;

		 eventGeneral_NaviLuminusEnd();
		 SoundManager::GetInstance()->PlayBGM(BGM_CASTLE);

		 sStagePlaceIndex = 30;
		 getPlayerPosRotFromBGModel(&sPlayer.Position(), &sPlayer.Rotation().y, sStagePlaceIndex);

		 EventManager::GetInstance()->EndEvent();
		 sEventCsv.DeleteFile();
		 sPlayer.Scale() = CHARA_DEF_SCALE;
		 sPlayer.EnableShadow();
		 VISIBLE_BATTLE_ICON();
		 SET_EVENT_END_FLAG();

		 return true;

	 default:
		 break;
	 }
	 return false;
 }

 //---------------------------------------------------
 /**

  */
 //---------------------------------------------------
 bool
 event29_AppearWizard(void)
 {
	 ModelPmd* p_model = NULL;
	 bool is_end = false;
	 //	static const VECTOR offset = VScale(VGet(-200.0f, -102.8f, 980.0f), 10.0f);
	 static const VECTOR offset = VScale(VGet(200.0f, 102.8f, -980.0f), 10.0f);

	 switch (sEventPhase) {
	 case 0:
		 FADE_OUT(0.5f);
		 sEventPhase ++;
		 break;
	 case 1:
		 if (IS_FADE() == true) return false;

		 HIDE_BATTLE_ICON();
		 sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		 sPlayer.SetAlpha(0.0f);
		 sPlayer.Scale() = ZERO_VEC;
		 sPlayer.DisableShadow();

		 EventManager::GetInstance()->LoadCameraFile("data/Camera/029_01_Camera.vmd");
		 EventManager::GetInstance()->EntryModelNum(4);
		 EventManager::GetInstance()->SetModelParameter(0, Name_Lino, eMT_029_01_EVENT, &sModelMap);
		 EventManager::GetInstance()->SetModelParameter(1, "01//DarkWizard", eMT_WIZARD_029_01_APPEAR_WIZARD, NULL);
		 EventManager::GetInstance()->SetModelParameter(2, RES_MDL_CARD_JK, 0, NULL);
		 EventManager::GetInstance()->SetModelParameter(3, NAME_Navi, eMT_IDLE, NULL);

		 p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		 p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		 p_model->EnableToonShader();

		 sEventCsv.LoadFile("data/CSV/event29_01.csv");
		 EventManager::GetInstance()->StartEvent(&sEventCsv, &offset);

		 p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		 p_model->Position() = VAdd(p_model->Position(), offset);
		 p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		 p_model->Position() = VAdd(p_model->Position(), offset);

		 p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
		 p_model->Position() = VAdd(p_model->Position(), offset);
		 p_model->Position() = VAdd(p_model->Position(), VScale(VGet(15.0f, 0.5f, 0.0f), 10.0f));
		 ((CharaNpc *)p_model)->DisableShadow();

		 p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(3);
		 p_model->Position() = VAdd(p_model->Position(), offset);
		 p_model->Position() = VAdd(p_model->Position(), VScale(VGet(13.55f, 0.5f, 21.5f), 10.0f));
		 eventGeneral_NaviLuminusStart(p_model);

		 // トランプ非表示
		 sModelMap.SetAlpha((eBG_TYPE_GIMMICK + 1), 0.0f);

		 FADE_IN(0.5f);
		 sEventPhase ++;
		 break;

	 case 2:
		 /* CSVファイルのアクションを更新 */
		 is_end = updateCsvAction(&sEventCsv);

		 p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(3);
		 eventGeneral_NaviLuminusUpdate(p_model);

		 if (is_end) {
			 FADE_OUT(0.5f);
			 sEventPhase ++;
		 }
		 break;

	 case 3:
		 if (IS_FADE() == true) return false;

		 eventGeneral_NaviLuminusEnd();

		 EventManager::GetInstance()->EndEvent();
		 sEventCsv.DeleteFile();
		 sPlayer.SetAlpha(1.0f);
		 sPlayer.Scale() = CHARA_DEF_SCALE;
		 sPlayer.EnableShadow();
		 VISIBLE_BATTLE_ICON();
		 SET_EVENT_END_FLAG();
		 return true;

	 default:
		 break;
	 }
	 return false;
 }

 //---------------------------------------------------
 /**

  */
 //---------------------------------------------------
 bool
 event29_BossStart(void)
 {
	 /* 指定のイベントを終了してない場合は普通に移動させる */
	 if (! ((IS_END_EVT(eEVENT_29, eEVENT_29_APPEAR_WIZARD) == true) && (IS_END_EVT(eEVENT_29, eEVENT_29_COME_FRES) == false) )) {
		 return eventGeneral_MovePlace(0, 5);
	 }

	 bool is_end = false;
	 VECTOR pos;
	 float rot;

	 switch (sEventPhase) {
	 case 0:

		 // ロックオン解除
		 sPlayer.DisableLockonChara();
		 destroyAllEnemy(); // 敵を全滅させる

		 FADE_OUT(0.5f);
		 sEventPhase ++;
		 break;
	 case 1:
		 if (IS_FADE() == true) return false;

		 HIDE_BATTLE_ICON();
		 sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		 getPlayerPosRotFromBGModel(&pos, &rot, 24);
		 sPlayer.Position() = pos;

		 getPlayerPosRotFromBGModel(&pos, &rot, 23);
		 createEnemyWork2(0, eENEMY_KIND_WIZARD, &pos);
		 sppEnemy[0]->PermitAcceptAI(false);
		 sppEnemy[0]->SetAlpha(1.0f);

		 sPlayer.SetRotationToTarget(&sppEnemy[0]->Position());
		 sppEnemy[0]->SetRotationToTarget(&sPlayer.Position());

		 CameraManager::GetInstance()->SetPatternGameBegine();
		 CameraManager::GetInstance()->Position().y += 100.0f;
		 CameraManager::GetInstance()->Target() = sppEnemy[0]->Position();


		 SoundManager::GetInstance()->PlayBGM(BGM_BOSS_W);

		 FADE_IN(1.0f);

		 sEventPhase ++;

		 break;

	 case 2:
		 if (IS_FADE() == false) {
			 MSG_TYPE_MESSAGE();
			 MSG_FADE_IN(0.3f);
			 SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
			 sEventPhase ++;
		 }
		 break;

	 case 3:
		 if (MSG_IS_FADE() == false) sEventPhase ++;
		 break;

	 case 4:
		 RENDER_MESSAGE("闇の司祭", "我々の邪魔はさせん", "ここで消えてもらおう",  "",  "");
		 if (sEventCount > 120.0f)  {
			 SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			 MSG_TYPE_MESSAGE();
			 MSG_FADE_OUT(0.4f);
			 sEventPhase ++;
		 }
		 break;

	 case 5:
		 		if (MSG_IS_FADE() == false) sEventPhase ++;
		break;

	case 6:
		sppEnemy[0]->PermitAcceptAI(true);
		sppEnemy[0]->ChangeState(CharaEnemy::eCHARA_STATE_APPEAR);

		CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_BATTLE_WALL);
		sModelMap.VisibleBattleBox();

		CameraManager::GetInstance()->StartDeleyCamera(CameraManager::eDELAY_SPEED_LOW, true);

		// 鎧との戦闘フラグを立てる
		TotalInfo::GetInstance()->SetEventFlag(eEVENT_29, eEVENT_29_BATTLE_ARMER);

		VISIBLE_BATTLE_ICON();

		// バトルフラグを立てる
		sEventBattleFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_29, eEVENT_29_BOSS_END);
		
		// リカバリー、またはゲームオーバー時にクリアされるフラグを設定
		sEventRecoveryFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_29, eEVENT_29_BOSS_START);

		return true;

	default:
		break;
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_BossEnd(void)
{
	//	mPhase = ePHASE_STAGECLEAR;
	sDungeonIndex = 1;
	spScenBase->SetPhase(SceneMainGame::ePHASE_STAGECLEAR);

	// バトルフラグを立てる
	sEventBattleFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_29, eEVENT_29_COME_FRES);

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_BtlTreasureD(void)
{
	return eventGeneral_BattleTreasure();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_GateSecurity(void)
{
	unsigned long field_bit = TotalInfo::GetInstance()->GetFieldInfoFlagBit(sStageIndex);

	VECTOR target_pos;
	float alpha;

	switch (sEventPhase) {
	case 0:
		// カメラを切り替える
		sModelMap.GetMeshMdlPosition(&target_pos, 
									 MV1GetFrameMesh(sModelMap.GetModelHandle(), 
													 (eBG_TYPE_GIMMICK + 2), 
													 0));
		CameraManager::GetInstance()->Target() = target_pos;

		if (MSG_IS_FADE() == false) {
			MSG_TYPE_TELOP();
			MSG_FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;
	case 1:
		if (MSG_IS_FADE() == false) sEventPhase ++;
		break;
		
	case 2:
		RENDER_MESSAGE("", "石版に文字が書かれている・・・", "",  "",  "");
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;

	case 3:
		RENDER_MESSAGE("", "『中庭に隠されている３つの札を台座におさめよ", "さすれば道は開かれる・・・』",  "",  "");
		if (CHECK_TRIG_ANY_JOY()) {
			if (((!(field_bit & kFIELD_INFO_29_TRUMP_PUT_K)) && TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_CARD_K)) ||
				((!(field_bit & kFIELD_INFO_29_TRUMP_PUT_Q)) && TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_CARD_Q)) ||
				((!(field_bit & kFIELD_INFO_29_TRUMP_PUT_J)) && TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_CARD_J))) {
				sEventPhase ++;
			} else {
				sEventPhase = 90;	// 終了
			}
		}
		break;
	case 4:
		RENDER_MESSAGE("", "現在持っているトランプを台座にはめますか？", "",  "",  "");
		Utility::eQuestion qs;
		qs = Utility::ExecuteQuestion();
		if (qs == Utility::eQUESTION_YES){
			MSG_FADE_OUT(0.5f);
			sEventPhase ++;
		}
		if (qs == Utility::eQUESTION_NO){
			MSG_FADE_OUT(0.5f);
			sEventPhase = 90;	// 終了
		}
		break;

	case 5:
		if (MSG_IS_FADE() == false) sEventPhase ++;
		break;

	// King
	case 6:
		if ((!(field_bit & kFIELD_INFO_29_TRUMP_PUT_K)) &&
			(TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_CARD_K))) {
			sModelMap.SetVisible(true, eBG_TYPE_GIMMICK + 3);
			SoundManager::GetSndComponent()->PlaySound(eSE_HOOK_START);
			TotalInfo::GetInstance()->SetEnableFieldInfoFlag(sStageIndex, kFIELD_INFO_29_TRUMP_PUT_K);
			sEventPhase ++;
		} else {
			sEventPhase = 7;
		}
		break;

	case 7:
		if (sEventCount > 60) {
			sEventPhase ++;
		}
		break;

	// Queen
	case 8:
		if ((!(field_bit & kFIELD_INFO_29_TRUMP_PUT_Q)) &&
			(TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_CARD_Q))) {
			sModelMap.SetVisible(true, eBG_TYPE_GIMMICK + 4);
			SoundManager::GetSndComponent()->PlaySound(eSE_HOOK_START);
			TotalInfo::GetInstance()->SetEnableFieldInfoFlag(sStageIndex, kFIELD_INFO_29_TRUMP_PUT_Q);
			sEventPhase ++;
		} else {
			sEventPhase = 9;
		}
		break;

	case 9:
		if (sEventCount > 60) {
			sEventPhase ++;
		}
		break;

	// Jack
	case 10:
		if ((!(field_bit & kFIELD_INFO_29_TRUMP_PUT_J)) &&
			(TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_CARD_J))) {
			sModelMap.SetVisible(true, eBG_TYPE_GIMMICK + 5);
			SoundManager::GetSndComponent()->PlaySound(eSE_HOOK_START);
			TotalInfo::GetInstance()->SetEnableFieldInfoFlag(sStageIndex, kFIELD_INFO_29_TRUMP_PUT_J);
			sEventPhase ++;
		} else {
			sEventPhase = 11;
		}
		break;

	case 11:
		if (sEventCount > 60) {
			sEventPhase = 12;
		}
		break;
		
		// 全部ＯＫかをチェック
	case 12:
		if ((field_bit & kFIELD_INFO_29_TRUMP_PUT_K) && 
			(field_bit & kFIELD_INFO_29_TRUMP_PUT_Q) && 
			(field_bit & kFIELD_INFO_29_TRUMP_PUT_J)) {

			SoundManager::GetSndComponent()->PlaySound(eSE_CLEAR_BATTLEBOX);
			SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);

			sEventPhase ++;
		} else {
			sEventPhase = 90;
		}
		break;

	case 13:
		alpha = sModelMap.GetAlpha(eBG_TYPE_GIMMICK + 2);
		alpha += -0.05f;
		if (alpha < 0.0f) {

			sModelMap.SetVisible(false, eBG_TYPE_GIMMICK + 2);
			sModelMap.SetVisible(false, eBG_TYPE_GIMMICK + 3);
			sModelMap.SetVisible(false, eBG_TYPE_GIMMICK + 4);
			sModelMap.SetVisible(false, eBG_TYPE_GIMMICK + 5);

			// 台座のコリジョン破棄
			CollisionManager::GetInstance()->RemoveEarthCollision(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 2));
			
			SET_EVENT_END_FLAG();

			sEventPhase ++;
		} else {
			sModelMap.SetAlpha((eBG_TYPE_GIMMICK + 2), alpha);
			sModelMap.SetAlpha((eBG_TYPE_GIMMICK + 3), alpha);
			sModelMap.SetAlpha((eBG_TYPE_GIMMICK + 4), alpha);
			sModelMap.SetAlpha((eBG_TYPE_GIMMICK + 5), alpha);
		}
		break;

	default:
		return eventGeneral_ClearBarrier(eBG_TYPE_GIMMICK + 0, 13);

	case 90:
		if (MSG_IS_FADE() == false) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
	case 91:
		if (IS_FADE() == false) {
			FADE_IN(0.5f);
			sPlayer.Position() = VAdd(sPlayer.Position(), VGet((450.0f * ALL_SCALE), 0.0f, 0.0f));
			sPlayer.Rotation().y = Utility::DegToRad(90.0f);
			return true;
		}
		break;
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_BtlTreasureA(void)
{
	return eventGeneral_BattleTreasure();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_BtlTreasureB(void)
{
	return eventGeneral_BattleTreasure();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_BtlTreasureC(void)
{
	return eventGeneral_BattleTreasure();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_BattleArmer(void)
{
	if (sEventCount == 0) {
		CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_BATTLE_WALL);
		sModelMap.VisibleBattleBox();
		destroyAllEnemy(); // 敵を全滅させる
	}

	if (sEventCount == 10) {
		sModelMap.SetVisible(false, eGIMMICK_29_ARMER_1);
		sModelMap.SetVisible(false, eGIMMICK_29_ARMER_2);
		sModelMap.SetVisible(false, eGIMMICK_29_ARMER_3);
		sModelMap.SetVisible(false, eGIMMICK_29_ARMER_4);

		VECTOR pos;	float rot;
		for (int iI = 0; iI < 4; iI ++) {
			getPlayerPosRotFromBGModel(&pos, &rot, 32 + iI);
			createEnemyWork2(iI, eENEMY_KIND_ARMER, &pos);
		}
	}

	if (sEventCount >= 20) {

		SET_EVENT_END_FLAG();

		// リカバリー、またはゲームオーバー時にクリアされるフラグを設定
		sEventRecoveryFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_29, eEVENT_29_BATTLE_ARMER);

		return true;
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_BigKey(void)
{
	if (TotalInfo::GetInstance()->GetHasFieldKey(sStageIndex)) {
		
		bool is_end = eventGeneral_ClearBarrier(eGIMMICK_29_BIG_KEY, 0);
		if (is_end == true) {
			SET_EVENT_END_FLAG();
			return true;
		}
	} else {
		const char* strTalk[4] = {"この扉を開けるには", "大きな鍵が必要だ",  "",  ""};
		
		return eventGeneral_BlockCharactor(0, &VGet((-450.0f * ALL_SCALE), 0.0f, 0.0f), 270.0f, 
										   "", (char **)strTalk);
	}	
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_AboutMaruiforce(void)
{
	static const VECTOR offset = VScale(VGet(-9.0f, -41.5f, -475.0f), 10.0f);
	static const int model_num = 11;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;
	VECTOR eff_pos;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		ResourceManager::GetInstance()->LoadCharaResource(NAME_MaruiForce);
		ResourceManager::GetInstance()->LoadCharaResource(NAME_LibeSkydome);
		ResourceManager::GetInstance()->LoadCharaResource(NAME_Elda);
		ResourceManager::GetInstance()->LoadCharaResource("03/CrysR");
		ResourceManager::GetInstance()->LoadCharaResource("03/CrysG");
		ResourceManager::GetInstance()->LoadCharaResource("03/CrysB");
		ResourceManager::GetInstance()->LoadCharaResource("03/CrysY");
		
		ResourceLayout::GetInstance()->LoadLayoutResource("Evt029_11_A.jpg");

		EventManager::GetInstance()->LoadCameraFile("data/Camera/029_11_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Navi,		0,						NULL,		true);
		EventManager::GetInstance()->SetModelParameter(2, NAME_Fresbelk,	eMT_FRES_IDLE_NOWEAPON,	NULL,		true);
		EventManager::GetInstance()->SetModelParameter(3, "01//DarkWizard", 16, NULL, true);
		EventManager::GetInstance()->SetModelParameter(4, NAME_MaruiForce,	0,						(ModelBase *)EVENT_MANAGER_AFTER_EFFECT_DRAW, true);
		EventManager::GetInstance()->SetModelParameter(5, NAME_LibeSkydome,0,						NULL,		true);
		EventManager::GetInstance()->SetModelParameter(6, "03/CrysR",0,						NULL,		true);
		EventManager::GetInstance()->SetModelParameter(7, "03/CrysG",0,						NULL,		true);
		EventManager::GetInstance()->SetModelParameter(8, "03/CrysB",0,						NULL,		true);
		EventManager::GetInstance()->SetModelParameter(9, "03/CrysY",0,						NULL,		true);
		EventManager::GetInstance()->SetModelParameter(10, NAME_Elda,		eMT_ELDA_CHAIRING,		NULL,	true);


		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1); // Navi
		eventGeneral_NaviLuminusStart(p_model);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(10);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawEldaFunc);		/* エルダ専用の描画関数を設定 */

		sEventCsv.LoadFile("data/CSV/event29_11.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv, &offset);

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction(&sEventCsv);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusUpdate(p_model);

		/* 周囲をまとう炎をのエフェクトの管理 */
		if (EventManager::GetInstance()->EqualPhaseFrame(2330.0f)) {
			size = 300.0f;
			if (sEventFuncEffectIndex != -1) {
				EffectManager::GetInstance()->Stop(sEventFuncEffectIndex, 0);
				sEventFuncEffectIndex = -1;
			}
			eff_pos = VScale(VGet(0.0f, -200.5f, 26.3f), spBGInfo->mScale);
			eff_pos = VAdd(eff_pos, offset);
			sEventFuncEffectIndex = EffectManager::GetInstance()->Play(eEF_FLAME,
																&eff_pos,
																&VGet(size, size, size),
																0.0f);
		}


		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		if (sEventFuncEffectIndex != -1) {
			EffectManager::GetInstance()->Stop(sEventFuncEffectIndex, 0);
			sEventFuncEffectIndex = -1;
		}

		eventGeneral_NaviLuminusEnd();
		SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);

		// 非表示にしている可能性のモデルを表示させる
		sModelMap.SetVisible(true);
		for (unsigned int iI = 0; iI < sSubStageInfoList.size(); iI ++) {
			spSubModelMap[iI].SetVisible(true);
		}
		sModelXfile[eMD_SKYDOME].SetVisible(true);

		{
			VECTOR pos;	float rot;
			getPlayerPosRotFromBGModel(&pos, &rot, 22);
			sPlayer.Position() = pos;
			sPlayer.Rotation().y = Utility::DegToRad(90.0f);
		}

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();
		SET_EVENT_END_FLAG();
		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
 * 番兵のミノタウロス、もしくは移動イベント
 */
//---------------------------------------------------
bool
event29_MinoGuardianOrMove(void)
{
	if (IS_END_EVT(eEVENT_29, eEVENT_29_COME_FRES)) {
		return eventGeneral_MovePlace(0, 11);
	}
	else {
		if (sPlayer.CosplayIndex() == eWINMENU_COSPLAY_LAPAN) {

			switch (sEventPhase) {
			case 0:
				if(sEventCount > 60) {
					MSG_TYPE_MESSAGE();
					SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
					MSG_FADE_IN(0.3f);
					sEventPhase ++;
				}
				break;

			case 1:
				if (MSG_IS_FADE() == false) {
					RENDER_MESSAGE("ミノタウロス", "あで？", "おめーはうさぎの商人じゃねーか", "さっき通したと思ったんだけどなー？", "");
					if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
				}
				break;

			case 2:
				RENDER_MESSAGE("ミノタウロス", "まあいいや", "ほらっ！さっさと通んなッ！", "",  "");
				if (CHECK_TRIG_ANY_JOY()) {
					SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
					MSG_FADE_OUT(0.3f);
					sEventPhase ++;
				}
				break;

			case 3:
				if (MSG_IS_FADE() == false) {
					sEventPhase ++;
				}
				break;
			
			default:
				return eventGeneral_MovePlace(4, 11);
				break;
			};
		}
		else {
			const char* strTalk[4] = {"番兵のミノタウロスが", "道をふさいでいる",  "",  ""};
			return eventGeneral_BlockCharactor(0, &VGet(0.0f, 0.0f, -500.0f), 180.0f, 
											   "", (char **)strTalk);
		}
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_LionTreasure(void)
{
	return eventGeneral_BattleTreasure();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_ComeFres(void)
{
	static const VECTOR offset = VScale(VGet(-9.0f, -41.5f, -475.0f), 10.0f);
	static const int model_num = 5;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;
	VECTOR eff_pos;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		ResourceManager::GetInstance()->LoadCharaResource(NAME_Fresbelk);

		EventManager::GetInstance()->LoadCameraFile("data/Camera/029_04_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino, eMT_029_04_EVENT, &sModelMap);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Fresbelk, eMT_FRES_029_04_COME_FRES, NULL);
		EventManager::GetInstance()->SetModelParameter(2, NAME_DarkWizard, eMT_WIZARD_029_04_COME_FRES, NULL);

		EventManager::GetInstance()->SetModelParameter(3, RES_MDL_LANCE, 0, NULL);
		EventManager::GetInstance()->SetModelParameter(4, RES_MDL_FRES_SHIELD, 0, NULL);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		sEventCsv.LoadFile("data/CSV/event29_04.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv, &offset);

		for (int iI = 0; iI < model_num; iI ++) {
			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(iI);
			p_model->Position() = VAdd(p_model->Position(), offset);
		}
		GraphicManager::GetInstance()->DisableFogPattern();		// フォグを無効にする

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);

		/* 周囲をまとう炎をのエフェクトの管理 */
		if (EventManager::GetInstance()->EqualPhaseFrame(180.0f)) {
			size = 1000.0f;
			if (sEventFuncEffectIndex != -1) {
				EffectManager::GetInstance()->Stop(sEventFuncEffectIndex, 0);
				sEventFuncEffectIndex = -1;
			}
			eff_pos = VGet(910.0f, -5415.0f, -4750.0f);
			sEventFuncEffectIndex = EffectManager::GetInstance()->Play(eEF_FLAME,&eff_pos,&VGet(size, size, size),0.0f);
			//			GraphicManager::GetInstance()->EnableFogPattern(10,10,10,10.0f,10000.0f);
			GraphicManager::GetInstance()->EnableFogPattern(10,10,10,10.0f,5000.0f);
		}

		/* エフェクトの発生などはプログラム手動で行う */
		if (EventManager::GetInstance()->EqualPhaseFrame(120.0f)) {
			size = 100.0f;
			if (sNaviEffHandle != -1) {
				EffectManager::GetInstance()->Stop(sNaviEffHandle, 0);
				sNaviEffHandle = -1;
			}
			p_model->GetFramePosFromName(&eff_pos, "操作中心");
			sNaviEffHandle = EffectManager::GetInstance()->Play(eEF_FLAME,&eff_pos,&VGet(size, size, size),0.0f);
			EffectManager::GetInstance()->Rotation(sNaviEffHandle).z = 0.0f;

		} else {
			float now_frame = EventManager::GetInstance()->GetPhaseFrame();
			if (EventManager::GetInstance()->BetweenPhaseFrame(120.0f, 180.0f)) {
				size = ((now_frame - 120.0f) * 3.0f) + 10.0f;
				EffectManager::GetInstance()->Scale(sNaviEffHandle) = VGet(size, size, size);

			} else if (EventManager::GetInstance()->BetweenPhaseFrame(180.0f, 260.0f)) {
				size = ((now_frame - 180.0f) * 2.5f) + 100.0f;
				EffectManager::GetInstance()->Scale(sNaviEffHandle) = VGet(size, size, size);

			} else if (EventManager::GetInstance()->BetweenPhaseFrame(260.0f, 270.0f)) {
				EffectManager::GetInstance()->Rotation(sNaviEffHandle).z = Utility::DegToRad(-90.0f);
				p_model->GetFramePosFromName(&eff_pos, "右中指３");
				EffectManager::GetInstance()->Position(sNaviEffHandle) = eff_pos;
				size = 0.0f;
				EffectManager::GetInstance()->Scale(sNaviEffHandle) = VGet(size, size, size);

			} else if (EventManager::GetInstance()->BetweenPhaseFrame(270.0f, 330.0f)) {
				p_model->GetFramePosFromName(&eff_pos, "右中指３");
				EffectManager::GetInstance()->Position(sNaviEffHandle) = eff_pos;
				size = ((now_frame - 270.0f) * 7.5f) + 100.0f;
				EffectManager::GetInstance()->Scale(sNaviEffHandle) = VGet(size, size, size);

			} else if (EventManager::GetInstance()->BetweenPhaseFrame(330.0f, 550.0f)) {
				p_model->GetFramePosFromName(&eff_pos, "右中指３");
				EffectManager::GetInstance()->Position(sNaviEffHandle) = eff_pos;
				size = 500.0f;
				EffectManager::GetInstance()->Scale(sNaviEffHandle) = VGet(size, size, size);

			} else if (EventManager::GetInstance()->BetweenPhaseFrame(550.0f, 600.0f)) {
				p_model->GetFramePosFromName(&eff_pos, "右中指３");
				EffectManager::GetInstance()->Position(sNaviEffHandle) = eff_pos;
				size = ((now_frame - 550.0f) * -9.4f) + 500.0f;
				EffectManager::GetInstance()->Scale(sNaviEffHandle) = VGet(size, size, size);

			} else if (EventManager::GetInstance()->BetweenPhaseFrame(600.0f, 820.0f)) {
				p_model->GetFramePosFromName(&eff_pos, "右中指３");
				EffectManager::GetInstance()->Position(sNaviEffHandle) = eff_pos;
				size = 0.0f;
				EffectManager::GetInstance()->Scale(sNaviEffHandle) = VGet(size, size, size);

			}
		}

		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction(&sEventCsv);

		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		GraphicManager::GetInstance()->DisableFogPattern();		// フォグを無効にする
		if (sNaviEffHandle != -1) {
			EffectManager::GetInstance()->Stop(sNaviEffHandle, 0);
			sNaviEffHandle = -1;
		}
		if (sEventFuncEffectIndex != -1) {
			EffectManager::GetInstance()->Stop(sEventFuncEffectIndex, 0);
			sEventFuncEffectIndex = -1;
		}

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		{
			VECTOR pos;	float rot;
			getPlayerPosRotFromBGModel(&pos, &rot, 23);
			sPlayer.Position() = pos;
			sPlayer.Rotation().y = Utility::DegToRad(90.0f);
		}

		spBGInfo->mMusicName = BGM_CASTLE_2;
		SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);

		sPlayer.AddHitPoint(999);

		// 鎧との戦闘フラグを立てる
		TotalInfo::GetInstance()->SetEventFlag(eEVENT_29, eEVENT_29_BATTLE_ARMER);

		/*===================================== *
		 * NPC の割り当てを変更させる
		 *===================================== */
		spNpc = CharaNpc::RecreateNpcInstance(1, spNpc, sNpcNum, sStageIndex, spBGInfo->mScale);

		/* モブクロックを操作し、存在しない場所にもっていく */
		for (int iI = 0; iI < sNpcNum; iI ++) {
			spNpc[iI].SetPositionOnCollision();
		}

		// バリアの追加
		CollisionManager::GetInstance()->RemoveWallCollision(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 11));
		sModelMap.SetVisible(false, (eBG_TYPE_GIMMICK + 11));
		
		// Actionlimit を変更させる
		spBGInfo->mActionLimit = eACTION_NG;

		VISIBLE_BATTLE_ICON();
		SET_EVENT_END_FLAG();
		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_AppearTrump(void)
{
	sPlayer.StartMotion(eMT_SHIELD_GUARD, true, 0.25f, NULL, 1.0f);
	
	switch (sEventPhase) {
	case 0:
		if (sEventCount > (90.0f)) {
			sEventPhase ++;
		}
		break;

	default:
		return eventGeneral_BattleTreasure(1);

	};
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_BattleQuest3(void)
{
	if (!(IS_END_EVT(eEVENT_35, eEVENT_35_END_BETA_GAME))) {
		switch (sEventPhase) {
		case 0:	if (MSG_IS_FADE() == false) {MSG_FADE_IN_PATTERN();	sEventPhase ++;	}	break;
		case 1:	RENDER_MESSAGE(sGaltSoldierName, "すまんがまだ準備中だ", "また後できてくれ！",  "",  "");
			if (CHECK_TRIG_ANY_JOY()) {	MSG_FADE_OUT_PATTERN();	return true;}
			break;
		default: break;
		};
	}
	else if (!(TotalInfo::GetInstance()->GetFieldInfoFlagBit(8) & kFIELD_INFO_08_ARENA_5CLEAR_BIT)) {
		switch (sEventPhase) {
		case 0:	if (MSG_IS_FADE() == false) {MSG_FADE_IN_PATTERN();	sEventPhase ++;	}	break;
		case 1:	RENDER_MESSAGE(sGaltSoldierName, "むっ！", "君はまだ公国本戦を突破していないな",  "そっちをクリアしてからまた来てくれ！",  "");
			if (CHECK_TRIG_ANY_JOY()) {	MSG_FADE_OUT_PATTERN();	return true;}
			break;
		default: break;
		};
	}
	else {
		// 現在何回戦かを調べる
		int clear_match_num = (TotalInfo::GetInstance()->GetFieldInfoFlagBit(sStageIndex) & kFIELD_INFO_XX_ARENA_MASK);
		return eventXX_BattleQuest(eBATTLE_QUEST_RANK_C, clear_match_num);
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_BattleQuest3_End(void)
{
	// 現在何回戦かを調べる
	int clear_match_num = (TotalInfo::GetInstance()->GetFieldInfoFlagBit(sStageIndex) & kFIELD_INFO_XX_ARENA_MASK);

	return eventXX_BattleQuest_End(eBATTLE_QUEST_RANK_C, clear_match_num);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_MoveRichCity(void)
{
	if (IS_END_EVT(eEVENT_29, eEVENT_29_COME_FRES)) {
		if (sEventPhase == 0) {
			ActionTaskManager::tTaskInfo param = {ActionTaskManager::eTASK_MOVE, 13, 15};
			ActionTaskManager::GetInstance()->NotifyAction(&param);
			sEventPhase ++;
		}
	}
	else {
		const char* strTalk[4] = {"なぜかタコの形をしたものが", "道をふさいでいる",  "",  ""};
		return eventGeneral_BlockCharactor(0, &VGet(-450.0f, 0.0f, 0.0f), 270.0f, 
										   "ナビォ", (char **)strTalk);
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_GetMagicFlame(void)
{
	if (TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_BOOK) == FALSE) {
		return eventGeneral_NotHaveBook();
	}
	else {
		return eventGeneral_GetMagic(eITEM_KIND_MAGIC_FLAME);
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_MoveUpperCastle(void)
{
	/*
	if ((sEventPhase == 1) && (IS_FADE() == false)) {
		NotifyMovePlaceWhileFadeout();
	}
	*/
	return eventGeneral_MovePlace(0, 41);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_MoveLowerCastle(void)
{
	/*
	if ((sEventPhase == 1) && (IS_FADE() == false)) {
		NotifyMovePlaceWhileFadeout();
	}
	*/
	return eventGeneral_MovePlace(0, 40);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_RevivalPoppo(void)
{
	CharaNpc* p_poppo = NULL;
	for (int iI = 0; iI < sNpcNum; iI ++) {
		if (strcmp(CharaNpc::GetNpcResourceName(iI), NAME_PoppoStone) == 0) {
			p_poppo = &spNpc[iI];
			break;
		}
	}
	APP_ASSERT((p_poppo != NULL),
			   "Not Found PoppoStone Npc Model \n");


	switch (sEventPhase) {
	case 0:
		CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_DUMMY);

		// カメラ位置を更新
		VECTOR cam_pos, cam_tar;
		cam_tar = p_poppo->Position();
		cam_tar.y += p_poppo->BodyHeight() * 0.7f;
		cam_pos.x = cam_tar.x + (sin(p_poppo->Rotation().y + DX_PI) * 300.0f);
		cam_pos.y = cam_tar.y + 50.0f;
		cam_pos.z = cam_tar.z + (cos(p_poppo->Rotation().y + DX_PI) * 300.0f);

		CameraManager::GetInstance()->Position() = cam_pos;
		CameraManager::GetInstance()->Target() = cam_tar;

		SoundManager::GetInstance()->PlayBGM(BGM_DEPARTURE);

		sEventPhase ++;
		break;

	case 1:
		if (sEventCount == 120) {
			SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
			CameraManager::GetInstance()->StartShakeCamera(2.5f, 30.0f, 120.0f);
			sEventPhase ++;
			sEventCount = 0;
		}
		break;

	case 2:
		if (sEventCount == 60) {
			
			VECTOR eff_pos = p_poppo->Position();
			eff_pos.y += (p_poppo->BodyHeight() * 0.5f);
			EffekseerManager::GetInstance()->Play(eEFK_GETKEYITEM, &eff_pos, 8.0f);
			//			EffekseerManager::GetInstance()->Play(eEFK_BREAK_BARRIER, &eff_pos, 8.0f);

			SoundManager::GetSndComponent()->PlaySound(eSE_REFLECT);

			FADE_COL_WHITE();
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;

	case 3:
		if (IS_FADE() == FALSE) {
			// ポッポモデルをロード
			ResourceManager::GetInstance()->LoadCharaResource(NAME_Poppo);

			// モデルファイル読み込みの処理
			p_poppo->DeleteModel();
			p_poppo->LinkModel(NAME_Poppo);
			p_poppo->Setup(true);
	
			sEventPhase ++;
			sEventCount = 0;
		}
		break;

	case 4:
		if (sEventCount == 60) {
			sEventCount = 0;
			sEventPhase ++;
			FADE_IN(1.0f);
		}
		break;

	case 5:
		if (sEventCount == 120) {

			// キャラの位置を移動
			VECTOR ch_pos = p_poppo->Position();
			ch_pos.x += (sin(p_poppo->Rotation().y + DX_PI) * 100.0f);
			ch_pos.z += (cos(p_poppo->Rotation().y + DX_PI) * 100.0f);
			sPlayer.Position() = ch_pos;
			sPlayer.SetRotationToTarget(&p_poppo->Position());
			
			// オカリナが宙に浮いてしまう不具合回避
			sPlayer.ChangeAttachPattern(CharaPlayer::eATTACH_PATTERN_HAND);

			// キャラ周辺のカメラの設定
			CameraManager::GetInstance()->SetPatternArroundPlayer(300,		// camDeg
																  -500,		// camLen
																  260,		// camPosY
																  160);		// camTarY
			MSG_TYPE_MESSAGE();
			MSG_FADE_IN_PATTERN();
			sEventPhase ++;
		}
		break;

	case 6:
		if (MSG_IS_FADE() == false) {
			RENDER_MESSAGE("ぽっぽ伯爵", "リノ！",
						   "オカリナの音色を聞かせてくれてありがとう！", 
						   "君が奏でるその曲のおかげで復活できたよ！", "");
			if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		}
		break;

	case 7:
		if (MSG_IS_FADE() == false) {
			RENDER_MESSAGE("ぽっぽ伯爵", 
						   "君の事はよく知っている・・・", 
						   "君が生まれた時からよく知っているよ", "", "");
			if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		}
		break;

	case 8:
		if (MSG_IS_FADE() == false) {
			RENDER_MESSAGE("ぽっぽ伯爵", 
						   "・・・そうだ",
						   "これから「翼の唄」を奏でてくれたら", 
						   "空から君を迎えに行くよ", "");
			if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		}
		break;

	case 9:
		if (MSG_IS_FADE() == false) {
			RENDER_MESSAGE("ぽっぽ伯爵", 
						   "『一度行ったことのある』ところなら、", 
						   "わたしがひとっ飛びで送ってあげるよ！", "", "");
			if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		}
		break;

	case 10:
		if (MSG_IS_FADE() == false) {
			RENDER_MESSAGE("ぽっぽ伯爵", 
						   "じゃあね、リノ、また会おう！", 
						   "", "", "");
			if (CHECK_TRIG_ANY_JOY()) {
				MSG_FADE_OUT_PATTERN();
				sEventPhase ++;
			}
		}
		break;

	case 11:
		if (MSG_IS_FADE() == false) {
			/*===================================== *
			 * NPC の割り当てを変更させる
			 *===================================== */
			spNpc = CharaNpc::RecreateNpcInstance(2, spNpc, sNpcNum, sStageIndex, spBGInfo->mScale);

			// オカリナが宙に浮いてしまう不具合回避
			sPlayer.ChangeAttachPattern(CharaPlayer::eATTACH_PATTERN_NORMAL);

			SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);
			SET_EVENT_END_FLAG();
			return true;
		}
		break;

	default:
		break;

	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event29_GoToFressRoom(void)
{
	if (sPlayer.CosplayIndex() == eWINMENU_COSPLAY_FRES) {
		switch (sEventPhase) {
		case 0:
			if(sEventCount > 60) {
				MSG_TYPE_MESSAGE();
				MSG_FADE_IN_PATTERN();
				sEventPhase ++;
			}
			break;

		case 1:
			RENDER_MESSAGE("鳥番兵", "あれ？", "これは公爵様ではありませんか", "失礼しました。お通り下さい", "");
			if (CHECK_TRIG_ANY_JOY()) {
				MSG_FADE_OUT_PATTERN();
				sEventPhase ++;
			}
			break;

		case 2:
			if (MSG_IS_FADE() == false) {
				ActionTaskManager::tTaskInfo param = {ActionTaskManager::eTASK_MOVE, 12, 0};
				ActionTaskManager::GetInstance()->NotifyAction(&param);
				sEventPhase ++;
			}
			break;
			
		default:
			break;
		};
	}
	else {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event34_MoveToAnyWhere(void)
{
	if ((GraphicManager::GetInstance()->GetEnableDirectionLight() == true) &&
		(sPlayer.Rotation().y > Utility::DegToRad(120.0f)) &&
		(sPlayer.Rotation().y < Utility::DegToRad(150.0f))) {

		switch (sEventPhase) {
		case 0:
			MSG_FADE_OUT(0.3f);

			SoundManager::GetSndComponent()->PlaySound(eSE_WARP);
			FADE_COL_WHITE();
			FADE_OUT(1.5f);
			sEventPhase ++;
			break;

		case 1:
			if (IS_FADE() == false) {
				FADE_COL_BLACK();
				ActionTaskManager::tTaskInfo param = {ActionTaskManager::eTASK_MOVE, 16, 0};
				ActionTaskManager::GetInstance()->NotifyAction(&param);
				sEventPhase ++;
			}
			break;

		default:
			break;

		};
	
	}
	else {

		switch (sEventPhase) {
		case 0:
			MSG_TYPE_TELOP();
			MSG_FADE_IN_PATTERN();
			sEventPhase ++;
			break;

		case 1:
			if (MSG_IS_FADE() == false) {
				RENDER_MESSAGE("", "森の不思議な力によって", "忘却の森の奥に行くことができない！", "", "");
				if (CHECK_TRIG_ANY_JOY()) {
					MSG_FADE_OUT(0.3f);

					SoundManager::GetSndComponent()->PlaySound(eSE_WARP);
					FADE_OUT(0.5f);
					sEventPhase ++;
				}
			}
			break;

		case 2:
			if (IS_FADE() == false) {
				ActionTaskManager::tTaskInfo param = {ActionTaskManager::eTASK_MOVE, 35, 2};
				ActionTaskManager::GetInstance()->NotifyAction(&param);
				sEventPhase ++;
			}
			break;

		default:
			break;

		};


	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event34_PlayedSongOfSun(void)
{
	switch (sEventPhase) {
	case 0:
		if (GraphicManager::GetInstance()->GetEnableDirectionLight() == true) {
			sEventPhase = 2;
		}
		else {
			SoundManager::GetSndComponent()->PlaySound(eSE_HOLY);
			FADE_COL_WHITE();
			FADE_OUT(2.00f);
			sEventPhase ++;
		}
		break;

	case 1:
		if (IS_FADE() == false) {
			
			// ディレクショナルライトを有効にする
			GraphicManager::GetInstance()->SetEnableDirectionLight(true);

			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
			CameraManager::GetInstance()->ResetCameraPosY();

			FADE_IN(0.75f);
			sEventPhase ++;
		}
		break;

	case 2:
		if (IS_FADE() == false) {
			
			MSG_TYPE_MESSAGE();
			MSG_FADE_IN_PATTERN();
			sEventPhase ++;
		}
		break;

	case 3:
		if (MSG_IS_FADE() == false) {
			RENDER_MESSAGE("？？？", "この森は眠りから目覚めた！", "忘却の森の奥へ進むには", "「北西」を目指して進むのだ！", "");
			if (CHECK_TRIG_ANY_JOY()) {
				sPlayer.ChangeAttachPattern(CharaPlayer::eATTACH_PATTERN_NORMAL);
				MSG_FADE_OUT_PATTERN();
				return true;
			}
		}
		break;


	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event35_WelcomeVillage(void)
{
	static const VECTOR offset = VScale(ZERO_VEC, 10.0f);

	static const int model_num = 3;

	ModelPmd* p_model = NULL;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		// 続きのイベントを指定
		sNextEventNo = eEVENT_35_END_BETA_GAME;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		// 必要なリソースを読み込む
		ResourceManager::GetInstance()->LoadCharaResource(NAME_Seek);

		EventManager::GetInstance()->LoadCameraFile("data/Camera/035_00_Camera.vmd", 10);
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	0,					&sModelMap, false);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Navi,	0,					NULL,		true);
		EventManager::GetInstance()->SetModelParameter(2, NAME_Seek,	0,					NULL,		true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusStart(p_model);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawSeekFunc);		/* シーク専用の描画関数を設定 */
		p_model->EnableToonShader(0.02f, 0.02f);

		sEventCsv.LoadFile("data/CSV/event35_00.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv, &offset);

		startCsvAction2nd();

		FADE_IN(0.5f);

		sEventPhase ++;
		break;
		
	case 2:
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusUpdate(p_model);

		/* CSVファイルのアクションを更新 */
		if (updateCsvAction2nd(&sEventCsv) ) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();
		SET_EVENT_END_FLAG();

		SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);

		return true;
		
	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event35_EndBetaGame(void)
{
	int color = GetColor(255, 255, 255);

	switch (sEventPhase) {
	case 0:

		if (sEventCount > 180) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;

	case 1:
		if (IS_FADE() == false) {
			MSG_TYPE_TELOP();
			MSG_FADE_IN(0.3f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
			sEventPhase ++;
		}
		break;

	case 2:
		RENDER_MESSAGE_COL("", "β版で遊べるストーリーは以上です。", 
						   "ここからは引き続き街の探索などをお楽しみ下さい",  "",  "", color);
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;
		
	case 3:
		RENDER_MESSAGE_COL("", "またリベルニア一武道会の全国大会に参加可能になりました",  
						   "地区予選、本戦がまだの方も是非挑戦してみて下さい",  "", "", color);
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;

	case 4:
		RENDER_MESSAGE_COL("", "最後に、このゲームをプレイして頂き、",
						   "本当にありがとうございました！",  "", "", color);
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;
		
	case 5:
		MSG_TYPE_TELOP();
		MSG_FADE_OUT(0.3f);
		SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
		sEventPhase ++;
	break;
	
	case 6:
		if (sEventCount > 180) {
			SET_EVENT_END_FLAG();
			return true;
		}

	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool event35_GetSongWildness(void)
{
	return eventGeneral_GetSong(eSONG_TYPE_WILDNESS);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool event35_GetSongWing(void)
{
	return eventGeneral_GetSong(eSONG_TYPE_WING);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool event35_GetSongSun(void)
{
	return eventGeneral_GetSong(eSONG_TYPE_SUN);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event35_AfterGetGreenCrystal(void)
{
	switch (sEventPhase) {
	case 0:
		SoundManager::GetInstance()->StopBGM();

		FADE_OUT(0.5f);
		HIDE_BATTLE_ICON();
		sEventPhase ++;

		break;
	case 1:
		if (IS_FADE() == true) return false;

		EventManager::GetInstance()->LoadCameraFile("data/Camera/dummy.vmd");
		EventManager::GetInstance()->EntryModelNum(0);

		// キャラ周辺のカメラの設定
		CameraManager::GetInstance()->SetPatternArroundPlayer(30.0f,		// camDeg
															  -400.0f,		// camLen
															  220.0f,		// camPosY
															  60.0f);		// camTarY

		sEventCsv.LoadFile("data/CSV/event16_01.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		/* アイテムを全て表示にさせる(StartEvent にて非表示にされる為) */
		visibleObjectAll(true);

		FADE_IN(0.5f);

		sEventPhase ++;
		break;
		
	case 2:
		MSG_TYPE_MESSAGE();
		SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
		MSG_FADE_IN(0.3f);
		sEventPhase ++;
		break;

	case 3:
		if (MSG_IS_FADE() == false) {
			RENDER_MESSAGE("ウィル", "リノ、リノよ･･･聞こえるか？", "わしじゃ、ウィルじゃ", "直接お前さんの心に話しかけておる", "");
			if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		}
		break;
	case 4:
		RENDER_MESSAGE("ウィル", "どうやら森のクリスタルを入手できたようじゃな", "よくやったわい！", "",  "");
		if (CHECK_TRIG_ANY_JOY()) {
			RENDER_MESSAGE("", "", "", "", "");
			sEventPhase ++;
		}
		break;

	case 5:
		if (sEventCount > 30.0f) {
			RENDER_MESSAGE("セルシアナ", "リノちゃん！おつかれ～！ヽ(*´∀｀)ノ", "次のクリスタルの場所が分かったんだけど", "ちょっと面倒な場所にあるのよね･･･(´･ω･`; )",  "");
			if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		}
		break;
	case 6:
		RENDER_MESSAGE("セルシアナ", "また説明したいから魔術大学に戻ってきて！", "道中のでかい敵にやられないでね(´；ω；｀)", "じゃあね～",  "");
		if (CHECK_TRIG_ANY_JOY()) {
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			MSG_FADE_OUT(0.3f);
			sEventPhase ++;
		}
		break;
	case 7:
		if (MSG_IS_FADE() == false) {
			VISIBLE_BATTLE_ICON();
			EventManager::GetInstance()->EndEvent();
			sEventCsv.DeleteFile();

			SET_EVENT_END_FLAG();

			SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);
			return true;
		}
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event36_ClearBarrier00(void)
{
	bool is_end = eventGeneral_ClearBarrier(eBG_TYPE_GIMMICK + 0, 0, 30);
	if (is_end == true) {
		SET_EVENT_END_FLAG();
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event36_ClearBarrier01(void)
{
	bool is_end = eventGeneral_ClearBarrier(eBG_TYPE_GIMMICK + 1, 0);
	if (is_end == true) {
		SET_EVENT_END_FLAG();
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event36_ClearBarrier02(void)
{
	bool is_end = eventGeneral_ClearBarrier(eBG_TYPE_GIMMICK + 2, 0);
	if (is_end == true) {
		SET_EVENT_END_FLAG();
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*
bool
event36_Dummy00(void)
{
	return false;
}
*/

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event36_MoveReverse00(void)
{
	if (IS_END_EVT(eEVENT_36, eEVENT_36_DIRECTION_LIGHT_ON)) {
		if (sEventPhase == 0) {
			ActionTaskManager::tTaskInfo param;
			param.mTaskType = ActionTaskManager::eTASK_MOVE;
			param.mTaskValueA = 39;
			param.mTaskValueB = 20;
			ActionTaskManager::GetInstance()->NotifyAction(&param);
			sEventPhase ++;
		}
	}
	else {
		const char* strTalk[4] = {"リノ！", "この先は暗すぎて進めそうにないよ！！",  "",  ""};
		return eventGeneral_BlockCharactor(0, &VGet(0.0f, 0.0f, 450.0f), 0.0f, 
										   "ナビォ", (char **)strTalk);
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event36_DirectionLightOn(void)
{
	GraphicManager * const p_graphic = GraphicManager::GetInstance();
	int model_handle = -1;

	VECTOR flame_pos = VGet(2.5f, 1400.0f, 2668.0f);

	switch (sEventPhase) {
	case 0:
		{
			// 初回イベントが未実施の場合は、初回イベントに強制的に変更させる
			if (IS_END_EVT(eEVENT_36, eEVENT_36_DIRECTION_LIGHT_ON_1ST) == false) {
				sTaskInfo.mTaskValueB = eEVENT_36_DIRECTION_LIGHT_ON_1ST;
				return false;
			}
			
			// 操作不能に
			sPlayer.PermitAcceptInput(false);
			// 白フェードを行う
			FADE_COL_WHITE();
			FADE_OUT(0.75f);

			sEventPhase ++;
		}
		break;

	case 1:
		if (IS_FADE() == true) return false;
		{
			/* ライトの切り替え */
			StageFunc036* p_stage = (StageFunc036 *)spStageFunc;
			p_stage->SetPatternLight(StageFunc036::eLIGHT_PATTEN_FLAME);

			SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);
			SoundManager::GetInstance()->PlayBGM(BGM_DUNGEON_F_2);
	
			// フラグの切り替えを行う
			TotalInfo::GetInstance()->DisableEventFlag(eEVENT_36, eEVENT_36_DIRECTION_LIGHT_OFF);
			SET_EVENT_END_FLAG();

			FADE_COL_WHITE();
			FADE_IN(1.5f);
			sEventPhase ++;
		}
		break;

	case 2:
		if (IS_FADE() == false) {
			
			FADE_COL_BLACK();
			sPlayer.PermitAcceptInput(true);
			return true;
		}
		break;
	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event36_DirectionLightOff(void)
{
	GraphicManager * const p_graphic = GraphicManager::GetInstance();

	switch (sEventPhase) {
	case 0:
		{
			// 操作不能に
			sPlayer.PermitAcceptInput(false);

			// 黒フェードを行う
			FADE_COL_BLACK();
			FADE_OUT(0.75f);

			// 炎エフェクトを消す
			StageFunc036* p_stage = (StageFunc036 *)spStageFunc;

			int eff_index = p_stage->GetEffectFlameIndex();
			if (eff_index != -1) {
				EffectManager::GetInstance()->Stop(eff_index);
				p_stage->SetEffectFlameIndex(-1);
			}
			//			STOP_EFFECT(eff_index);

			sEventPhase ++;
		}
		break;
	case 1:
		// フェード待ち
		if (IS_FADE() == true) return false;
		{
			// 隠し通路などを表示する
			MV1SetFrameVisible(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 3), FALSE);

			SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);
		
			// イベント36のライトオフ関数
			StageFunc036* p_stage = (StageFunc036 *)spStageFunc;
			p_stage->SetPatternLight(StageFunc036::eLIGHT_PATTEN_NAVI_LIGHT);
			SoundManager::GetInstance()->PlayBGM(BGM_DUNGEON_F);

			// フラグ更新・このタイミングでおげとく
			TotalInfo::GetInstance()->DisableEventFlag(eEVENT_36, eEVENT_36_DIRECTION_LIGHT_ON);
			SET_EVENT_END_FLAG();

			FADE_COL_BLACK();
			FADE_IN(1.5f);
			sEventPhase++;
		}
		break;
			
	case 2:
		if (IS_FADE() == false) {
			sPlayer.PermitAcceptInput(true);
			return true;
		}
		break;
	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event36_BossStart(void)
{
	const VECTOR offset = VScale(VGet(0.0f, -3.4f, 0.0f), 10.0f);
	const int model_num = 4;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	switch (sEventPhase) {
	case 0:
		SoundManager::GetInstance()->PlayBGM("");
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		ResourceManager::GetInstance()->LoadCharaResource((char *)NAME_MobClockW);
		ResourceManager::GetInstance()->LoadCharaResource((char *)NAME_Demon);

		EventManager::GetInstance()->LoadCameraFile("data/Camera/036_06_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,	&sModelMap, true);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Navi,		0,				NULL,		true);
		EventManager::GetInstance()->SetModelParameter(2, NAME_MobClockW,	0,				NULL,		true);
		EventManager::GetInstance()->SetModelParameter(3, NAME_Demon,		0,				NULL,		true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1); // Navi
		eventGeneral_NaviLuminusStart(p_model);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(3);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawDemonFunc);		/* デーモン専用の描画関数を設定 */

		sEventCsv.LoadFile("data/CSV/event36_06.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv, &offset);
		startCsvAction2nd();

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusUpdate(p_model);
		
		if (EventManager::GetInstance()->BetweenPhaseFrame(121.0f, 9999.0f)) {
			StageFunc036* p_stage = (StageFunc036 *)spStageFunc;
			//			int eff_index = getEventFunctionEffectHandle();
			int eff_index = p_stage->GetEffectFlameIndex();
			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(3);
			VECTOR pos;
			p_model->GetFramePosFromName(&pos, "頭");
			EffectManager::GetInstance()->Position(eff_index) = pos;
			EffectManager::GetInstance()->Scale(eff_index) = VGet(1800.0f, 1800.0f, 1800.0f);
		}

		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();

		{
			VECTOR set_pos;
			float set_rot;

			SoundManager::GetInstance()->PlayBGM(BGM_BOSS_A);

			getPlayerPosRotFromBGModel(&set_pos, &set_rot, 8);
			sPlayer.Position() = set_pos;
			sPlayer.Rotation().y = set_rot;
			sPlayer.SetPositionOnCollision();

			getPlayerPosRotFromBGModel(&set_pos, &set_rot, 1);
			createEnemyWork2(0, eENEMY_KIND_DEMON, &set_pos);

			/* オブジェクト表示の切り替え */
			StageFunc036* p_stage = (StageFunc036 *)spStageFunc;
			spStageFunc->SetPatternObject(StageFunc036::eOBJECT_PATTERN_BOSS);
			
			CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_BATTLE_WALL);
			sModelMap.VisibleBattleBox();

			// バトルフラグを立てる
			sEventBattleFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_36, eEVENT_36_BOSS_END);
	
			// リカバリー、またはゲームオーバー時にクリアされるフラグを設定
			sEventRecoveryFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_36, eEVENT_36_BOSS_START);

		}

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();
		SET_EVENT_END_FLAG();
		return true;
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event36_BossEnd(void)
{
	sDungeonIndex = 3;
	spScenBase->SetPhase(SceneMainGame::ePHASE_STAGECLEAR);

	// バトルフラグを立てる
	sEventBattleFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_36, eEVENT_36_APPEAR_HEARTCASE);

	/* モブクロックを操作し、存在しない場所にもっていく */
	for (int iI = 0; iI < sNpcNum; iI ++) {
		if (strcmp(CharaNpc::GetNpcResourceName(iI), NAME_MobClockW) == 0) {
			spNpc[iI].Position().y += 10000.0f;
		}
	}

	// Actionlimit を変更させる
	spBGInfo->mActionLimit = eACTION_NG;

	SET_EVENT_END_FLAG();

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event36_DirectionLightOn1st(void)
{
	GraphicManager * const p_graphic = GraphicManager::GetInstance();

	//	VECTOR flame_pos = VGet(2.5f, 1400.0f, 2668.0f);

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.3f);
		sPlayer.DisableLockonChara();
		TotalInfo::GetInstance()->DisableLockonLayout();

		sPlayer.PermitAcceptInput(false);

		//<! カメラ制御の為、このフラグをこの時点でたてる
		TotalInfo::GetInstance()->SetEventFlag(eEVENT_36, eEVENT_36_DIRECTION_LIGHT_ON);

		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;
		{
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_DUMMY);
			CameraManager::GetInstance()->Position() = VGet(2229.0488f, 423.76584f, -1996.1423f);
			CameraManager::GetInstance()->Target() = VGet(2104.8845f, 492.52908f, -1802.6097f);
			HIDE_BATTLE_ICON();
			SoundManager::GetSndComponent()->PlaySound(eSE_FLAME_SHOOT);

			/* ライトの切り替え */
			StageFunc036* p_stage = (StageFunc036 *)spStageFunc;
			p_stage->SetPatternLight(StageFunc036::eLIGHT_PATTEN_FLAME_1ST);
			
			FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;

	case 2:
		if (IS_FADE() == true) return false;
		sEventPhase ++;
		break;

	case 3:
		if (sEventCount > (90.0f)) {
			FADE_COL_WHITE();
			FADE_OUT(2.0f);
			sEventPhase ++;
		}
		break;

	case 4:
		{
			if (IS_FADE() == true) return false;
			FADE_COL_BLACK();

			SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);
			SoundManager::GetInstance()->PlayBGM(BGM_DUNGEON_F_2);

			TotalInfo::GetInstance()->DisableEventFlag(eEVENT_36, eEVENT_36_DIRECTION_LIGHT_OFF);
			SET_EVENT_END_FLAG();

			/* ライトの切り替え (初回とは別にもう一度コールする)*/
			StageFunc036* p_stage = (StageFunc036 *)spStageFunc;
			p_stage->SetPatternLight(StageFunc036::eLIGHT_PATTEN_FLAME);

			VISIBLE_BATTLE_ICON();
			sPlayer.PermitAcceptInput(true);
		}
		return true;

	default:
		break;
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*
bool
event36_WakeupLift(void)
{
	return eventGeneral_WakeupGimmick();
}
*/

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event36_ClearBarrier03(void)
{
	bool is_end = eventGeneral_ClearBarrier(eBG_TYPE_GIMMICK + 6, 0);
	if (is_end == true) {
		SET_EVENT_END_FLAG();
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool 
event36_MoveReverse01(void)
{
	if (IS_END_EVT(eEVENT_36, eEVENT_36_DIRECTION_LIGHT_ON)) {
		if (sEventPhase == 0) {
			ActionTaskManager::tTaskInfo param;
			param.mTaskType = ActionTaskManager::eTASK_MOVE;
			param.mTaskValueA = 39;
			param.mTaskValueB = 21;
			ActionTaskManager::GetInstance()->NotifyAction(&param);
			sEventPhase ++;
		}
	}
	else {
		const char* strTalk[4] = {"リノ！", "この先は暗すぎて進めそうにないよ！！",  "",  ""};
		return eventGeneral_BlockCharactor(0, &VGet(0.0f, 0.0f, 450.0f), 0.0f, 
										   "ナビォ", (char **)strTalk);
	}

	return false;

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event36_FallOnlyBoss(void)
{
	/* ※ この関数では処理をせず、SceneMainGame::phaseEventBefore() で処理をおこなっている */
	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event36_AdviceTrolley(void)
{
	switch (sEventPhase) {
	case 0:
		if (MSG_IS_FADE() == false) {
			MSG_TYPE_TELOP();
			MSG_FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;
	case 1:
		if (MSG_IS_FADE() == false) sEventPhase ++;
		break;
	case 2:
		RENDER_MESSAGE("", "トロッコの乗り方について説明を受けますか？", "",  "",  "");
		Utility::eQuestion qs;
		qs = Utility::ExecuteQuestion();
		if (qs == Utility::eQUESTION_YES)	sEventPhase ++;
		if (qs == Utility::eQUESTION_NO)	return true;	// 終了
		break;

	case 3:
		MSG_TYPE_MESSAGE();
		RENDER_MESSAGE("モグー", 
					   "トロッコは攻撃を当てると動き出すよ！", 
					   "ジャンプしてトロッコの中に入ってから攻撃してみよう",
					   "",  
					   "");
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;

	case 4:
		RENDER_MESSAGE("モグー", 
					   "トロッコに乗っている時では、", 
					   "『ジャンプ』ボタンを押すとトロッコごとジャンプするよ！",
					   "レールの上に岩がある時に使うといいね",  
					   "");
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;

	case 5:
		RENDER_MESSAGE("モグー", 
					   "ただし、あんまりジャンプしすぎると",
					   "レールからはみ出るから気をつけてね、",
					   "途中で落ちた場合はチェックポイントまで戻るからね",
					   "");
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;

	case 6:
		RENDER_MESSAGE("モグー", 
					   "あと、トロッコに乗れずおいていかれた場合、",
					   "しばらくすると元の場所にトロッコが復活するから",
					   "大人しく待ってた方がいいよ！",
					   "");
		if (CHECK_TRIG_ANY_JOY()) return true;
		break;

	default:
		break;
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event36_AdviceMine(void)
{
	switch (sEventPhase) {
	case 0:
		if (MSG_IS_FADE() == false) {
			MSG_TYPE_TELOP();
			MSG_FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;
	case 1:
		if (MSG_IS_FADE() == false) sEventPhase ++;
		break;
	case 2:
		RENDER_MESSAGE("", "鉱山について説明を受けますか？", "",  "",  "");
		Utility::eQuestion qs;
		qs = Utility::ExecuteQuestion();
		if (qs == Utility::eQUESTION_YES)	sEventPhase ++;
		if (qs == Utility::eQUESTION_NO)	return true;	// 終了
		break;

	case 3:
		MSG_TYPE_MESSAGE();
		RENDER_MESSAGE("モグー族長", 
					   "この鉱山は５階まであって", 
					   "その最上階に古代都市に続く道がある", 
					   "",  
					   "");
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;

	case 4:
		RENDER_MESSAGE("モグー族長", 
					   "まずここから北へ行き２階に続く通路を探しなさい",
					   "２階へ行くと灯りを照らすスイッチが所々にある",
					   "少しずつ灯りを増やしつつ最上階を目指すといいじゃろ",
					   "");
		if (CHECK_TRIG_ANY_JOY()) return true;
		break;

	default:
		break;
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event36_WarpTreasureRoom(void)
{
	VECTOR pos_a, pos_b;
	float rot;
	getPlayerPosRotFromBGModel(&pos_a, &rot, 6);
	pos_b = sPlayer.Position();

	pos_a.y = 0.0f;
	pos_b.y = 0.0f;

	VECTOR vec = VSub(pos_b, pos_a);
	float dist = VSize(vec);
		
	VECTOR damage_vec;
	damage_vec = VScale(VNorm(sPlayer.MoveVec()), -1.0f);

	if (dist < 710.0f) {
		sPlayer.ProcessDamage(1, false, &sPlayer.Position(), &damage_vec, true, eATTACK_TYPE_ENEMY_NORMAL);
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event36_WarpMine(void)
{
	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;

	case 1:
		if (IS_FADE() == false) {
			ActionTaskManager::tTaskInfo param;
			param.mTaskType = ActionTaskManager::eTASK_MOVE;
			param.mTaskValueA = 36;
			param.mTaskValueB = 6;
			ActionTaskManager::GetInstance()->NotifyAction(&param);

			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_NORMAL);
			sNavi.EnableNaviCursor();

			// ディレクションライト
			GraphicManager::GetInstance()->SetEnableDirectionLight(false);
			// ポイントライト有効
			GraphicManager::GetInstance()->SetEnablePLight(true);
			// オリジナルシェーダー有効
			GraphicManager::GetInstance()->SetEnableOriginalShader(true);
			// 自己発光を無効する
			sModelMap.SetEnableEmissive(false);

			//			return true;
			sEventPhase ++;
		}
		break;

	default:
		break;

	};
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event36_WelcomeMine(void)
{
	const int model_num = 2;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	switch (sEventPhase) {
	case 0:
		SoundManager::GetInstance()->StopBGM();
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		EventManager::GetInstance()->LoadCameraFile("data/Camera/036_10_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Navi,		0,						NULL,		true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1); // Navi
		eventGeneral_NaviLuminusStart(p_model);

		// ライトを無効にする
		GraphicManager::GetInstance()->SetEnablePLight(false, ePLIGHT_NO_00);
		GraphicManager::GetInstance()->SetEnablePLight(false, ePLIGHT_NO_01);

		// オリジナルシェーダー無効
		GraphicManager::GetInstance()->SetEnableOriginalShader(false);
		// ディレクションライト有効
		GraphicManager::GetInstance()->SetEnableDirectionLight(true);
		SetLightDifColor(GetColorF(COL_BYTE_2_FLOAT(45), 
								   COL_BYTE_2_FLOAT(45), 
								   COL_BYTE_2_FLOAT(45), 0.0f));
		SetLightSpcColor(GetColorF(COL_BYTE_2_FLOAT(0), 
								   COL_BYTE_2_FLOAT(0), 
								   COL_BYTE_2_FLOAT(0), 0.0f));
		SetLightAmbColor(GetColorF(COL_BYTE_2_FLOAT(10), 
								   COL_BYTE_2_FLOAT(10), 
								   COL_BYTE_2_FLOAT(10), 0.0f));

		sEventCsv.LoadFile("data/CSV/event36_10.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);
		startCsvAction2nd();

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusUpdate(p_model);
		
		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		{
			if (IS_FADE() == true) return false;

			GraphicManager::GetInstance()->SetDefaultDirectionLight();

			StageFunc036* p_stage = (StageFunc036 *)spStageFunc;
			p_stage->SetPatternLight(StageFunc036::eLIGHT_PATTEN_NAVI_LIGHT);
			SoundManager::GetInstance()->PlayBGM(BGM_DUNGEON_F);

			eventGeneral_NaviLuminusEnd();

			EventManager::GetInstance()->EndEvent();
			sEventCsv.DeleteFile();
			sPlayer.Scale() = CHARA_DEF_SCALE;
			sPlayer.EnableShadow();
			VISIBLE_BATTLE_ICON();
			SET_EVENT_END_FLAG();
		}
		return true;
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event36_AppearHeartCase(void)
{
	if (eventGeneral_MovePlace(0, 9)) {
		SoundManager::GetInstance()->PlayBGM(BGM_DUNGEON_F_2);
		sPlayer.Rotation().y = Utility::DegToRad(90.0f);

		/* オブジェクト表示の切り替え */
		StageFunc036* p_stage = (StageFunc036 *)spStageFunc;
		spStageFunc->SetPatternObject(StageFunc036::eOBJECT_PATTERN_NORMAL);

		return true;
	}
	return false;
}

/*=====================================*

 *=====================================*/

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event38_CrystalForest(void)
{
	const int model_num = 1;

	ModelPmd* p_model = NULL;
	bool is_end = false;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		EventManager::GetInstance()->LoadCameraFile("data/Camera/038_00_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		sEventCsv.LoadFile("data/CSV/event38_00.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		 //<! StartEvent()でアイテムを全て非表示にされるので、表示に戻す
		 visibleObjectAll(true);
		 
		 startCsvAction2nd();

		 FADE_IN(0.5f);
		 sEventPhase ++;
		 break;

	 case 2:
		 /* CSVファイルのアクションを更新 */
		 is_end = updateCsvAction2nd(&sEventCsv);
		 if (is_end) {
			 FADE_OUT(0.5f);
			 sEventPhase ++;
		 }
		 break;
		 
	case 3:
		if (IS_FADE() == true) return false;
		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();

		// クリスタル表示前にスキップされた場合の処理
		if (sppObjectBase[0]->Scale().y < 1.0f) {
			processAppearCrystal(3,					// stagePlaceIndex
								 sppObjectBase[0],	// pCrystalObject
								 0,					// crystalColor : 0 : Green, 1 : Yellow
								 FALSE);			// isEfk
		}
		SET_EVENT_END_FLAG();
		return true;

	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event38_TalkSento(void)
{
	const int model_num = 2;

	ModelPmd* p_model = NULL;
	bool is_end = false;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		// 必要なファイルをロードしておく
		ResourceManager::GetInstance()->LoadCharaResource(NAME_FaceSento);

		EventManager::GetInstance()->LoadCameraFile("data/Camera/038_01_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,		eMT_IDLE_NOWEAPON,		&sModelMap, true);
		EventManager::GetInstance()->SetModelParameter(1, NAME_FaceSento,	0,						&sModelMap, true);
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		// せんとくんは仕方なしのここで座標指定する
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		p_model->Position() = VScale(VGet(0.0f, 359.76f, -233.31f), 10.0f);						//<! 全てのモデル(キャラ込)が10倍するので、10倍させる
		p_model->Rotation().x = Utility::DegToRad(-22.3f);
		p_model->Rotation().y = Utility::DegToRad(180.0f);
		p_model->Scale() = VGet(100.0f, 100.0f, 100.0f);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawSentoFunc);		/* せんとさん専用の描画関数を設定 */

		sEventCsv.LoadFile("data/CSV/event38_01.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		 //<! StartEvent()でアイテムを全て非表示にされるので、表示に戻す
		 visibleObjectAll(true);
		 
		 startCsvAction2nd();

		 FADE_IN(0.5f);
		 sEventPhase ++;
		 break;

	 case 2:
		 /* CSVファイルのアクションを更新 */
		 is_end = updateCsvAction2nd(&sEventCsv);
		 if (is_end) {
			 FADE_OUT(0.5f);
			 sEventPhase ++;
		 }
		 break;
		 
	case 3:
		if (IS_FADE() == true) return false;
		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();

		// 帰り道用のエフェクトを再生
		{
			VECTOR eff_pos; float rot;
			getPlayerPosRotFromBGModel(&eff_pos, &rot, 0);
			EffectManager::GetInstance()->Play(eEF_AURA, &eff_pos, &VGet(200.0f, 100.0f, 200.0f) );
		}

		SET_EVENT_END_FLAG();
		return true;
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event38_CrystalEarth(void)
{
	const int model_num = 1;

	ModelPmd* p_model = NULL;
	bool is_end = false;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		EventManager::GetInstance()->LoadCameraFile("data/Camera/038_02_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		sEventCsv.LoadFile("data/CSV/event38_02.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		 //<! StartEvent()でアイテムを全て非表示にされるので、表示に戻す
		 visibleObjectAll(true);
		 
		 startCsvAction2nd();

		 FADE_IN(0.5f);
		 sEventPhase ++;
		 break;

	 case 2:
		 /* CSVファイルのアクションを更新 */
		 is_end = updateCsvAction2nd(&sEventCsv);
		 if (is_end) {
			 FADE_OUT(0.5f);
			 sEventPhase ++;
		 }
		 break;
		 
	case 3:
		if (IS_FADE() == true) return false;
		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();

		// クリスタル表示前にスキップされた場合の処理
		if (sppObjectBase[1]->Scale().y < 1.0f) {
			processAppearCrystal(4,					// stagePlaceIndex
								 sppObjectBase[1],	// pCrystalObject
								 1,					// crystalColor : 0 : Green, 1 : Yellow
								 FALSE);			// isEfk
		}

		SET_EVENT_END_FLAG();
		return true;
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event38_TalkYodel(void)
{
	const int model_num = 2;

	ModelPmd* p_model = NULL;
	bool is_end = false;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		// 必要なファイルをロードしておく
		ResourceManager::GetInstance()->LoadCharaResource(NAME_FaceYodel);

		EventManager::GetInstance()->LoadCameraFile("data/Camera/038_03_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,		eMT_IDLE_NOWEAPON,		&sModelMap, true);
		EventManager::GetInstance()->SetModelParameter(1, NAME_FaceYodel,	0,						&sModelMap, true);
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		// せんとくんは仕方なしのここで座標指定する
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		//		p_model->Position() = VScale(VGet(-233.31f, 359.76f, 0.0f), 10.0f);						//<! 全てのモデル(キャラ込)が10倍するので、10倍させる
		p_model->Position() = VScale(VGet(-233.31f, 320.00f, 0.0f), 10.0f);						//<! 全てのモデル(キャラ込)が10倍するので、10倍させる
		p_model->Rotation().z = Utility::DegToRad(-22.3f);
		p_model->Rotation().y = Utility::DegToRad(270.0f);
		p_model->Scale() = VGet(100.0f, 100.0f, 100.0f);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawSentoFunc);		/* せんとさん専用の描画関数を設定 */

		sEventCsv.LoadFile("data/CSV/event38_03.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		 //<! StartEvent()でアイテムを全て非表示にされるので、表示に戻す
		 visibleObjectAll(true);
		 
		 startCsvAction2nd();

		 FADE_IN(0.5f);
		 sEventPhase ++;
		 break;

	 case 2:
		 /* CSVファイルのアクションを更新 */
		 is_end = updateCsvAction2nd(&sEventCsv);
		 if (is_end) {
			 FADE_OUT(0.5f);
			 sEventPhase ++;
		 }
		 break;
		 
	case 3:
		if (IS_FADE() == true) return false;
		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();

		// 帰り道用のエフェクトを再生
		{
			VECTOR eff_pos; float rot;
			getPlayerPosRotFromBGModel(&eff_pos, &rot, 0);
			EffectManager::GetInstance()->Play(eEF_AURA, &eff_pos, &VGet(200.0f, 100.0f, 200.0f) );
		}

		SET_EVENT_END_FLAG();
		return true;
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event38_WarpOutWorld(void)
{
	switch (sEventPhase) {
	case 0:
		{
			SoundManager::GetSndComponent()->PlaySound(eSE_WARP);
			EffekseerManager::GetInstance()->Play(eEFK_DISAPPER,
												  &sPlayer.Position(), 
												  16.0f);
			sEventPhase ++;
		}
		break;
	case 1:
		if (sEventCount > 60) {
			sPlayer.Scale() = ZERO_VEC;
			sPlayer.Position().y -= 100.0f;
			FADE_COL_BLACK();
			FADE_OUT(0.75f);
			sEventPhase ++;
		}
		break;
	case 2:
		if (IS_FADE() == false) {
			ActionTaskManager::tTaskInfo param;
			param.mTaskType = ActionTaskManager::eTASK_MOVE;

			if (IS_END_EVT(eEVENT_38, eEVENT_38_TALK_YODEL)) {
				param.mTaskValueA = 48;
				param.mTaskValueB = 10;
			}
			else {
				param.mTaskValueA = 35;
				param.mTaskValueB = 2;
			}
			ActionTaskManager::GetInstance()->NotifyAction(&param);
			sEventPhase ++;
		}
		break;
	case 3:
		// dammy
		break;
	default:
		break;
	};
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event39_GoToOutside(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	GimmickBase* p_gimmick = NULL;
	p_gimmick = (GimmickBase *)GimmickBase::GetGimmickBaseFromEarthCollIndex(p_player->GetOnCollisionIndex());
	if (p_gimmick != NULL) {

		switch (sEventPhase) {
		case 0:
			FADE_COL_WHITE();
			FADE_OUT(0.4f);

			sEventPhase ++;
			break;

		case 1:
			if (IS_FADE() == false) {
				// ライトの設定を行う
				spStageFunc->SetPatternLight(1);

				FADE_COL_WHITE();
				FADE_IN(1.0f);

				sEventPhase ++;
			}
			break;

		case 2:
			if (IS_FADE() == false) {
				return true;
			}
			break;

		default:
			break;
		};


	}
	else {
		const char* strTalk[4] = {"リノ！", "この先は崖だ！危ないよ！",  "",  ""};
		
		return eventGeneral_BlockCharactor(0, &VGet(0.0f, 0.0f, 450.0f), 0.0f, 
										   "ナビォ", (char **)strTalk);
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event39_GoToInside(void)
{
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	switch (sEventPhase) {
	case 0:
		FADE_COL_WHITE();
		FADE_OUT(0.1f);

		sEventPhase ++;
		break;

	case 1:
		if (IS_FADE() == false) {
			// ライトの設定を行う
			spStageFunc->SetPatternLight(0);

			FADE_COL_WHITE();
			FADE_IN(1.0f);

			sEventPhase ++;
		}
		break;

	case 2:
		if (IS_FADE() == false) {
			return true;
		}
		break;

	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event40_WelcomWilderness(void)
{
	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	switch (sEventPhase) {
	case 0:
		SoundManager::GetInstance()->PlayBGM("");
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		{
			const int model_num = 2;

			if (IS_FADE() == true) return false;

			HIDE_BATTLE_ICON();
			sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

			sPlayer.Scale() = ZERO_VEC;
			sPlayer.DisableShadow();

			ResourceManager::GetInstance()->LoadCharaResource(NAME_Mask);

			EventManager::GetInstance()->LoadCameraFile("data/Camera/040_00_Camera.vmd");
			EventManager::GetInstance()->EntryModelNum(model_num);
			EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);
			EventManager::GetInstance()->SetModelParameter(1, NAME_Mask,	0,						NULL,		true);

			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
			p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
			p_model->EnableToonShader();

			sEventCsv.LoadFile("data/CSV/event40_00.csv");
			EventManager::GetInstance()->StartEvent(&sEventCsv, NULL, true, true);

			startCsvAction2nd();

			FADE_IN(0.5f);
			sEventPhase ++;
		}
		 break;

	 case 2:
		 /* CSVファイルのアクションを更新 */
		 is_end = updateCsvAction2nd(&sEventCsv);

		 //<! ギミックモデル0をトロッコと決めつけて処理をおこなう
		 //<! トロッコの位置をリノの位置に設定する
		 p_model = GimmickBase::GetGimmickBase(0);
		 p_model->Position() = EventManager::GetInstance()->GetModelPmd(0)->Position();
		 p_model->Rotation().y = EventManager::GetInstance()->GetModelPmd(0)->Rotation().y;
		 p_model->Scale() = VGet(24.0f, 24.0f, 24.0f);

		 // ついでに春原さんも後ろに乗せる
		 p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		 p_model->Position() = EventManager::GetInstance()->GetModelPmd(0)->Position();
		 p_model->Position().x += 64.0f;

		 // トロッコの車輪の音
		 if (EventManager::GetInstance()->BetweenPhaseFrame(0.0f, 310.0f)) {
			 if ((sEventCount % 8) == 0) {
				 SoundManager::GetSndComponent()->PlaySound(eSE_TROLLEY);
			 }
		 }
		 
		 if (is_end) {
			 FADE_OUT(0.5f);

			 if (CHECK_TRIG_JOY(JOY_INPUT_START)) {
				 sEventPhase = 5;
			 }
			 else {
				 sEventPhase ++;
			 }
		 }
		 break;
		 
	case 3:
		{
			const int model_num = 3;

			if (IS_FADE() == true) return false;

			GraphicManager::GetInstance()->EntryDrawFontFunc(NULL);
			sDrawFreeFontValue = eFREE_FONT_NONE;

			EventManager::GetInstance()->EndEvent();
			sEventCsv.DeleteFile();

			// 必要なリソースを読み込む
			ResourceManager::GetInstance()->LoadCharaResource(NAME_Mask);

			EventManager::GetInstance()->LoadCameraFile("data/Camera/040_01_Camera.vmd");
			EventManager::GetInstance()->EntryModelNum(model_num);
			EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);
			EventManager::GetInstance()->SetModelParameter(1, NAME_Navi,	0,						NULL,		true);
			EventManager::GetInstance()->SetModelParameter(2, NAME_Mask,	0,						NULL,		true);

			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
			p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
			p_model->EnableToonShader();

			sEventCsv.LoadFile("data/CSV/event40_01.csv");
			EventManager::GetInstance()->StartEvent(&sEventCsv, NULL, true, true);

			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
			eventGeneral_NaviLuminusStart(p_model);

			startCsvAction2nd();

			FADE_IN(0.5f);
			sEventPhase ++;
		}
		break;

	case 4:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusUpdate(p_model);
		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;

	case 5:
		{
			if (IS_FADE() == true) return false;

			eventGeneral_NaviLuminusEnd();

			SoundManager::GetInstance()->PlayBGM(BGM_DUNGEON_C);

			EventManager::GetInstance()->EndEvent();
			sEventCsv.DeleteFile();
			sPlayer.Scale() = CHARA_DEF_SCALE;
			sPlayer.EnableShadow();

			ObjectBase* p_enemy = search1stFindObjectKind(eOBJECT_KIND_GETDOWN);
			APP_ASSERT( (p_enemy != NULL),
						"Not Founc search1stFindObjectKind(eOBJECT_KIND_GETDOWN) \n");
			p_enemy->SetVisible(false);

			// オリジナル座標に戻す
			VECTOR org_pos;
			GimmickBase::GetGimmickBase(0)->getOriginalPosition(&org_pos);
			GimmickBase::GetGimmickBase(0)->Position() = org_pos;

			VISIBLE_BATTLE_ICON();
			SET_EVENT_END_FLAG();
		}
		return true;

	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event40_DisapperTornade(void)
{
	VECTOR pos;
	float rot;
	float sTornadeCamLen;
	VECTOR vec;
	CameraManager* const p_cammgr = CameraManager::GetInstance();
	StageFunc040* p_stage = (StageFunc040 *)spStageFunc;
	CharaBase* p_player = (CharaBase *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();

	ObjectBase* p_enemy = search1stFindObjectKind(eOBJECT_KIND_GETDOWN);
	APP_ASSERT( (p_enemy != NULL),
				"Not Founc search1stFindObjectKind(eOBJECT_KIND_GETDOWN) \n");

	switch (sEventPhase) {
	case 0:
		{
			//<! この時点でフラグを立てる！
			SET_EVENT_END_FLAG();
			// フォントハンドラを初期化
			GraphicManager::GetInstance()->EntryDrawFontFunc(NULL);

			SoundManager::GetSndComponent()->PlaySound(eSE_HIT_SWORD);

			ObjectBase* p_enemy = search1stFindObjectKind(eOBJECT_KIND_GETDOWN);
			APP_ASSERT( (p_enemy != NULL),
						"Not Founc search1stFindObjectKind(eOBJECT_KIND_GETDOWN) \n");
			p_enemy->EnableToonShader(0.01f, 0.01f);
			
			VECTOR eff_pos;
			eff_pos = p_enemy->Position();
			eff_pos.y += 800.0f;
			int eff_handle = EffectManager::GetInstance()->Play(eEF_HIT, &eff_pos, 140.0f, 0.0f);
			EffectManager::GetInstance()->Stop(eff_handle, 0.75f);
			sEventPhase ++;
		}
		break;

	case 1:
		//<! 数秒待つ
		if (sEventCount > 30) {
			// カメラを近めに変える
			getPlayerPosRotFromBGModel(&pos, &rot, 3);
			pos.y += 600.0f;
			sTornadeCamLen = 3000.0f;
			p_cammgr->Target() = pos;
			vec = VScale(VNorm(VSub(p_player->Position(), p_cammgr->Target() ) ), sTornadeCamLen);
			p_cammgr->Position() = VAdd(p_cammgr->Target(), vec);
			
			p_enemy->StartMotion(10, true, 0.7f);
			p_enemy->SetMotionNowTime(428.0f);
			p_enemy->SetAlpha(0.8f);

		EffectManager::GetInstance()->SetAlpha(p_stage->mEffHndTornade, 0.2f);

			MSG_TYPE_MESSAGE();
			MSG_FADE_IN(0.3f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);

			SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
			CameraManager::GetInstance()->StartVibeCamera(3.0f, 6.0f, 8.0f);
			
			sEventPhase ++;
		}
		break;
	case 2:
		//<! メッセージ開始
		RENDER_MESSAGE("砂の神", "ぎゃーッ！！", "", "", "");
		if (sEventCount > 80) {

			MSG_FADE_OUT(0.4f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);

			p_enemy->SetMotionSpeed(0.0f);

			sEventPhase ++;
		}	
		break;

	case 3:
		// カメラを遠景に変える
		getPlayerPosRotFromBGModel(&pos, &rot, 2);
		sTornadeCamLen = 20000.0f;
		p_cammgr->Target() = pos;
		vec = VScale(VNorm(VSub(p_player->Position(), p_cammgr->Target() ) ), sTornadeCamLen);
		p_cammgr->Position() = VAdd(p_cammgr->Target(), vec);

		SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);
		EffectManager::GetInstance()->SetAlpha(p_stage->mEffHndTornade, 1.0f);
		sEventPhase ++;

		break;
	case 4:
		{
			if (p_stage->mEffHndTornade != -1) {
				EffectManager::GetInstance()->Stop(p_stage->mEffHndTornade, 1.6f);
				p_stage->mEffHndTornade = -1;
			}
			sEventPhase ++;
		}
		break;
	case 5:
		if (sEventCount > 180.0f) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;

	case 6:
		// フェード中にカメラを近づける
		if (IS_FADE() == true) return false;

		SoundManager::GetInstance()->PlayBGM("RaceOfSand.ogg");

		p_enemy->Position().x = sPlayer.Position().x + 0.0f;
		p_enemy->Position().y = sPlayer.Position().y + -100.0f;
		p_enemy->Position().z = sPlayer.Position().z + 2000.0f;
		p_enemy->StartMotion(0, true, 0.4f);

		CameraManager::GetInstance()->SetPatternGameBegine();
		CameraManager::GetInstance()->Target() = p_enemy->Position();
		CameraManager::GetInstance()->Target().y += 700.0f;

		FADE_IN(0.5f);
		sEventPhase ++;
		break;

	case 7:
		if (IS_FADE() == true) return false;
		MSG_TYPE_MESSAGE();
		MSG_FADE_IN(0.3f);
		SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
		sEventPhase ++;
		break;

	case 8:
		//<! メッセージ開始
		RENDER_MESSAGE("砂の神", "オマエ、いい腕してるな！", "オレ様の動きを見切ったのは", "オマエが初めてだ！", "");
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;
		
	case 9:
		//<! メッセージ開始
		RENDER_MESSAGE("砂の神", "さては、かなりの風使いと見た！", "", "", "");
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;

	case 10:
		//<! メッセージ開始
		RENDER_MESSAGE("砂の神", "さてと、オレ様はうさえもん家に遊びにでも行くか！", "じゃあな、あばよ！", "", "");
		if (CHECK_TRIG_ANY_JOY()) {
			MSG_FADE_OUT(0.5f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			sEventPhase ++;
		}
		break;

	case 11:
		if (MSG_IS_FADE() == false) {
			SoundManager::GetSndComponent()->PlaySound(eSE_FALLDOWN);
			sEventPhase ++;
		}
		break;

	case 12:
		p_enemy->Position().y += 400.0f;
		p_enemy->Position().z += 600.0f;
		CameraManager::GetInstance()->Target() = p_enemy->Position();
		CameraManager::GetInstance()->Target().y += 700.0f;
		
		if (VSize(VSub(p_enemy->Position(), sPlayer.Position())) > 200000.0f) {
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_RESET);
			p_enemy->SetVisible(false);

			/*===================================== *
			 * NPC の割り当てを変更させる
			 *===================================== */
			spNpc = CharaNpc::RecreateNpcInstance(0, spNpc, sNpcNum, sStageIndex, spBGInfo->mScale);

			// 変則システムのバトルのため、イベントリカバリーフラグをここで落とす
			sEventRecoveryFlag = -1;

			//<! フラグはこのPhaseの最初で立てている為、明示的にコメントアウト
			// SET_EVENT_END_FLAG();

			return true;
		}
		break;

	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool 
event40_Dummy00(void)
{
	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event40_WakeupBabel(void)
{
	const int model_num = 4;
	const BYTE end_phase_no = 10;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	StageFunc040* p_stage = (StageFunc040 *)spStageFunc;
	
	ObjectBase* p_enemy = search1stFindObjectKind(eOBJECT_KIND_GETDOWN);
	APP_ASSERT( (p_enemy != NULL),
				"Not Founc search1stFindObjectKind(eOBJECT_KIND_GETDOWN) \n");
	p_enemy->SetVisible(false);

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		{
			HIDE_BATTLE_ICON();
			sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

			sPlayer.Scale() = ZERO_VEC;
			sPlayer.DisableShadow();

			// ライトの設定を行う
			spStageFunc->SetPatternLight(1);

			// 必要なモデルのロード
			ResourceManager::GetInstance()->LoadCharaResource(NAME_Seek);
			ResourceModel::GetInstance()->LoadItemResource(RES_MDL_PIKOHUM);

			// イベントリソースの設定
			EventManager::GetInstance()->LoadCameraFile("data/Camera/040_04_Camera.vmd");
			EventManager::GetInstance()->EntryModelNum(model_num);
			EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);
			EventManager::GetInstance()->SetModelParameter(1, NAME_Navi,	0,						NULL,		true);
			EventManager::GetInstance()->SetModelParameter(2, NAME_Seek,	0,						NULL,		true);
			EventManager::GetInstance()->SetModelParameter(3, RES_MDL_PIKOHUM,	0,					NULL,		true);

			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
			p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
			p_model->EnableToonShader(0.02f, 0.02f);

			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
			p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawSeekFunc);		/* シーク専用の描画関数を設定 */
			p_model->EnableToonShader(0.02f, 0.02f);
			/* ピコハンをアタッチ */
			p_model->AttachModel(EventManager::GetInstance()->GetModelPmd(3),
								 ModelPmd::eATTACHED_MODEL_NORMAL,
								 p_model->GetFrameIndexFromName("右小指２") );
		
			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(3);
			p_model->SetVisible(false);

			// マップステージの入れ替え
			reloadModelMap(&spSubModelMap[1], "data/model/BG/Harappa/MainStage.mv1", true);
			spSubModelMap[1].SetVisible(false);

			// ロードが長いのでハングしている様に見せない為、BGMはここで止める
			SoundManager::GetInstance()->PlayBGM("");

			sEventCsv.LoadFile("data/CSV/event40_04.csv");
			EventManager::GetInstance()->StartEvent(&sEventCsv, 
													NULL,					// シフト座標設定(NULLの場合はオフセットなし)
													false,					// NPC表示フラグ
													true);					// アイテム表示フラグ

			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
			eventGeneral_NaviLuminusStart(p_model);

			startCsvAction2nd();

			// Harappa のサブマップを沈ませるテスト (リセット)
			{
				spSubModelMap[0].Position().y = 0.0f;
				spSubModelMap[0].Rotation().x = 0.0f;
			}

			FADE_IN(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 2:
		// シークのモーションがピコハンを掲げてる場合のみ、ピコハンを表示させる
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(3);
		if ( ( (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2))->GetMotionIndex() == 13/* ピコハンを構える */) {
			p_model->SetVisible(true);
		} else {
			p_model->SetVisible(false);
		}

		/* nop */
		break;

	case 3:
		// 傾ける
		{
			CameraManager::GetInstance()->StartVibeCamera(0.5f, 30.0f, 90.0f);
			SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
			SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);
		
			spSubModelMap[0].Rotation().x = -0.002f;

			ObjectBase* p_crystl = search1stFindObjectKind(eOBJECT_KIND_CRYSTAL);
			APP_ASSERT( (p_crystl != NULL), 
						"Not Found eOBJECT_KIND_CRYSTAL \n");
			p_crystl->Position().y += 55.0f;
		
			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
			p_model->Position().y += 55.0f;
		
			//<! 追加
			//			EffectManager::GetInstance()->StopAll();
			if (p_stage->mEffHndCrystal != -1) {
				EffectManager::GetInstance()->Stop(p_stage->mEffHndCrystal, 0.0f);
				p_stage->mEffHndCrystal = -1;
			}
			if (p_stage->mEffHndAura != -1) {
				EffectManager::GetInstance()->Stop(p_stage->mEffHndAura, 0.0f);
				p_stage->mEffHndAura = -1;
			}

			sEventPhase ++;
		}
		break;

	case 4:		
		/* nop */
		break;

	case 5:
		// Harappa のサブマップを沈ませる
		{
			float down_move_speed = 12.0f;
			sModelMap.SetVisible(false, eBG_TYPE_VISIBLE);
			spSubModelMap[0].Position().y -= down_move_speed;

			ObjectBase* p_crystl = search1stFindObjectKind(eOBJECT_KIND_CRYSTAL);
			APP_ASSERT( (p_crystl != NULL), 
						"Not Found eOBJECT_KIND_CRYSTAL \n");
			p_crystl->Position().y -= down_move_speed;

			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
			p_model->Position().y -= down_move_speed;
		}

		if (sEventCount > 30) {
			CameraManager::GetInstance()->StartShakeCamera(0.5f, 30.0f, 90.0f);

			SoundManager::GetSndComponent()->SetVolume(eSE_BURST, 150);
			SoundManager::GetSndComponent()->SetVolume(eSE_EXPLOSION, 150);

			SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
			SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);

			sEventCount = 0;
		}

		break;

	case 6:
		// バベル のサブマップの位置を表示・設定
		spSubModelMap[1].SetVisible(true);
		spSubModelMap[1].Position().y = -34000.0f;
		sEventPhase ++;
		break;

	case 7:
		{
			// バベル のサブマップを浮上
			float up_move_speed = 55;
			spSubModelMap[1].Position().y += up_move_speed;
			if (spSubModelMap[1].Position().y >= 0.0f) {
				spSubModelMap[1].Position().y = 0.0f;
			}

			//			if (sEventCount > 30) {
			if (sEventCount > 30) {
				CameraManager::GetInstance()->StartVibeCamera(0.5f, 30.0f, 90.0f);
				SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
				SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);

				//  * 埃エフェクトを意図的に出力させる(スクリプトと同じ処理をおこなう為)
				evtCmdFunc_04(4);

				sEventCount = 0;
			}
		}
		break;

	case 8:
		{
			// バベル のサブマップを浮上
			float up_move_speed = 25;
			spSubModelMap[1].Position().y += up_move_speed;
			if (spSubModelMap[1].Position().y >= 0.0f) {
				spSubModelMap[1].Position().y = 0.0f;
			} else { 
				//			}
				if (sEventCount > 30) {
					VECTOR pos; float rot;
					getPlayerPosRotFromBGModel(&pos, &rot, 2);
					CameraManager::GetInstance()->StartVibeCamera(0.5f, 300.0f, 900.0f);
					SoundManager::GetSndComponent()->PlaySound(eSE_BURST);
					SoundManager::GetSndComponent()->PlaySound(eSE_EXPLOSION);

					//  * 埃エフェクトを意図的に出力させる(スクリプトと同じ処理をおこなう為)
					evtCmdFunc_04(2);

					sEventCount = 0;
				}
			}

		}
		break;

	case 9:
		// 遠景の砂漠を表示する
		sModelMap.SetVisible(true, eBG_TYPE_VISIBLE);
		break;

	case end_phase_no:
		{
			if (IS_FADE() == true) return false;
			eventGeneral_NaviLuminusEnd();
			EventManager::GetInstance()->EndEvent();
			sEventCsv.DeleteFile();

			VECTOR pos;	float rot;
			getPlayerPosRotFromBGModel(&pos, &rot, 1);
			sPlayer.Position() = pos;
			sPlayer.Rotation().y = rot;
			sPlayer.Scale() = CHARA_DEF_SCALE;
			sPlayer.EnableShadow();

			// 遠景の砂漠を表示する
			sModelMap.SetVisible(true, eBG_TYPE_VISIBLE);

			// バベルの塔を表示する
			spSubModelMap[1].SetVisible(true);

			// クリスタルの間をダミーデータに入れ替え
			reloadModelMap(&spSubModelMap[0], "data/model/BG/dummy.mv1", false);
			spSubModelMap[0].SetVisible(false);

			// 変更しておいたサウンドのサイズを元に戻す
			SoundManager::GetSndComponent()->SetVolume(eSE_BURST, 255);
			SoundManager::GetSndComponent()->SetVolume(eSE_EXPLOSION, 255);

			VISIBLE_BATTLE_ICON();
			SET_EVENT_END_FLAG();


			ActionTaskManager::tTaskInfo param;
			param.mTaskType = ActionTaskManager::eTASK_MOVE;
			param.mTaskValueA = 48;
			param.mTaskValueB = 0;
			ActionTaskManager::GetInstance()->NotifyAction(&param);
			
			//		return true;
			sEventPhase ++;
		}
		//		return true;
		return false;

	default:
		return false;
	}

	if (sEventPhase >= 2) {
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);
		eventGeneral_NaviLuminusUpdate( (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1) );
		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase = end_phase_no;
		}
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event40_TornadeEvent(void)
{
	const int model_num = 3;
	const BYTE end_phase_no = 10;


	ModelPmd* p_model = NULL;
	bool is_end = false;
	StageFunc040* p_stage = (StageFunc040 *)spStageFunc;

	ObjectBase* p_enemy = search1stFindObjectKind(eOBJECT_KIND_GETDOWN);
	APP_ASSERT( (p_enemy != NULL),
				"Not Founc search1stFindObjectKind(eOBJECT_KIND_GETDOWN) \n");

	switch (sEventPhase) {
	case 0:
		SoundManager::GetInstance()->StopBGM();
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		// 竜巻のモーフ数を変更する
		p_stage->SetTornadeMorphNumEvent();

		// 必要なモデルのロード
		ResourceManager::GetInstance()->LoadCharaResource(NAME_Seek);

		EventManager::GetInstance()->LoadCameraFile("data/Camera/040_05_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Navi,	0,						NULL,		true);
		EventManager::GetInstance()->SetModelParameter(2, NAME_Seek,	0,						NULL,		true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawSeekFunc);		/* シーク専用の描画関数を設定 */
		p_model->EnableToonShader(0.02f, 0.02f);

		sEventCsv.LoadFile("data/CSV/event40_05.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv, NULL, false ,true);

		// 敵のモーションをリセットさせて止めておく
		p_enemy->SetVisible(true);
		p_enemy->StartMotion(0, true, 0.5f);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusStart(p_model);

		startCsvAction2nd();

		FADE_IN(0.5f);
		sEventPhase ++;

		break;
		
	case 2:
		{
			float alpha = (sEventCount * 0.003f);
			if (alpha > 1.0f) {
				alpha = 1.0f;
			}
			EffectManager::GetInstance()->SetAlpha(p_stage->mEffHndTornade, (1.0f - alpha) );
			p_enemy->SetAlpha(alpha);

			p_enemy->SetRotationToTarget(&CameraManager::GetInstance()->Position());
			p_enemy->Position().y = CameraManager::GetInstance()->Target().y;
			p_enemy->Position().y -= 1000.0f;

			/* CSVファイルのアクションを更新 */
			is_end = updateCsvAction2nd(&sEventCsv);
			if (is_end)	{		// 終了判定
				FADE_OUT(0.5f);	
				sEventPhase = end_phase_no;
			}			
		}
		break;
		
	case 3:
		p_enemy->StartMotion(10, true, 0.5f);
		sEventPhase ++;
		break;

	case 4:
	case 7:
	case 9:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);
		eventGeneral_NaviLuminusUpdate( (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1) );
		if (is_end)	{		// 終了判定
			FADE_OUT(0.5f);	
			sEventPhase = end_phase_no;
		}			
		break;

	case 5:
		{
			float alpha = (sEventCount * 0.006f);
			if (alpha > 1.0f) {
				alpha = 1.0f;
			}
			EffectManager::GetInstance()->SetAlpha(p_stage->mEffHndTornade, alpha);
		
			/* CSVファイルのアクションを更新 */
			is_end = updateCsvAction2nd(&sEventCsv);
			if (is_end)	{		// 終了判定
				FADE_OUT(0.5f);	
				sEventPhase = end_phase_no;
			}			
		}
		break;

	case 6:
		// 竜巻の形状を変化させる
		p_stage->mIsChangeShapeTornade = true;
		
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);
		
		sEventPhase ++;
		break;

	case 8:
		// 竜巻の形状を変化させる
		p_stage->mIsChangeShapeTornade = false;

		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);
		eventGeneral_NaviLuminusUpdate( (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1) );

		sEventPhase ++;
		break;

	case end_phase_no:
		if (IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();

		SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);
		
		/*===================================== *
		 * NPC の割り当てを変更させる
		 *===================================== */
		spNpc = CharaNpc::RecreateNpcInstance(1, spNpc, sNpcNum, sStageIndex, spBGInfo->mScale);

		// 竜巻のモーフ数を変更する
		p_stage->SetTornadeMorphNumDefault();

		VECTOR pos;	float rot;
		getPlayerPosRotFromBGModel(&pos, &rot, 3);
		p_enemy->Position() = pos;
		p_enemy->StartMotion(10, true, 0.5f);
		p_enemy->SetVisible(true);
		p_enemy->SetAlpha(1.0f);
		p_enemy->DisableToonShader();

		getPlayerPosRotFromBGModel(&pos, &rot, 1);
		sPlayer.Position() = pos;
		sPlayer.Rotation().y = rot;
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();
		SET_EVENT_END_FLAG();

		// 竜巻の形状を元に戻す
		p_stage->mIsChangeShapeTornade = false;
		EffectManager::GetInstance()->SetAlpha(p_stage->mEffHndTornade, 1.0f);

		SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);
		SoundManager::GetInstance()->SetVolumeBGM(kSOUND_VOLUME_DEFAULT);

		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event40_BattleUsiderman(void)
{
	const int model_num = 2;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;
	StageFunc040* p_stage = (StageFunc040 *)spStageFunc;

	ObjectBase* p_enemy = search1stFindObjectKind(eOBJECT_KIND_GETDOWN);
	APP_ASSERT( (p_enemy != NULL),
				"Not Founc search1stFindObjectKind(eOBJECT_KIND_GETDOWN) \n");

	switch (sEventPhase) {
	case 0:
		SoundManager::GetInstance()->PlayBGM("Hodou.ogg");
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		p_enemy->SetVisible(true);

		// 必要なリソースを読み込む
		ResourceManager::GetInstance()->LoadCharaResource(NAME_Seek);

		EventManager::GetInstance()->LoadCameraFile("data/Camera/040_06_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Seek,	0,						NULL,		true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawSeekFunc);		/* シーク専用の描画関数を設定 */
		p_model->EnableToonShader();

		sEventCsv.LoadFile("data/CSV/event40_06.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		startCsvAction2nd();

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);

		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();

		sPlayer.Rotation().y = Utility::DegToRad(180.0f);

		VISIBLE_BATTLE_ICON();
		SET_EVENT_END_FLAG();

		// リカバリー、またはゲームオーバー時にクリアされるフラグを設定
		sEventRecoveryFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_40, eEVENT_40_BATTLE_USIDERMAN);

		// うさいだーまん戦のカウンタをクリアする.
		p_stage->mTornadeBattleCount = (21 * 60);
			
		SoundManager::GetSndComponent()->PlaySound(eSE_TORNADE);
		p_stage->mIsChangeShapeTornade = true;

		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event40_BtlTreasureA(void)
{
	bool is_end = false;
	is_end = eventGeneral_BattleTreasure();
	if (is_end) {
		SoundManager::GetInstance()->PlayBGM(BGM_DUNGEON_C);
	}
	return is_end;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event40_BattleTeeder(void)
{
	VECTOR pos; float rot;
	SoundManager::GetInstance()->PlayBGM(BGM_BOSS_C);

	CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_BATTLE_WALL);
	sModelMap.VisibleBattleBox();
	destroyAllEnemy(); // 敵を全滅させる
	
	getPlayerPosRotFromBGModel(&pos, &rot, 7);
	createEnemyWork2(0, eENEMY_KIND_TEEGER, &pos);

	SET_EVENT_END_FLAG();

	// リカバリー、またはゲームオーバー時にクリアされるフラグを設定
	sEventRecoveryFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_40, eEVENT_40_BATTLE_TEEDER);

	// バトルフラグを立てる
	sEventBattleFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_40, eEVENT_40_BTL_TREASURE_A);

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event40_BlockToSult(void)
{
	ActionTaskManager::tTaskInfo param = {ActionTaskManager::eTASK_MOVE, 36, 52};
	ActionTaskManager::GetInstance()->NotifyAction(&param);

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event40_FallOnlyBoss(void)
{
	/* ※ この関数では処理をせず、SceneMainGame::phaseEventBefore() で処理をおこなっている */
	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event40_BlockTornade(void)
{
	const char* strTalk[4] = {"リノ！", "竜巻でこれ以上近づけないよ！",  "",  ""};
	
	return eventGeneral_BlockCharactor(0, &VGet(0.0f, 0.0f, -1000.0f), 180.0f, 
									   "ナビォ", (char **)strTalk);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event40_MoveBabel(void)
{
	ActionTaskManager::tTaskInfo param = {ActionTaskManager::eTASK_MOVE, 48, 0};
	ActionTaskManager::GetInstance()->NotifyAction(&param);

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event40_FailedUsiderman(void)
{
	switch (sEventPhase) {
	case 0:
		if (MSG_IS_FADE() == false) {

			ObjectBase* p_enemy = search1stFindObjectKind(eOBJECT_KIND_GETDOWN);
			APP_ASSERT( (p_enemy != NULL),
						"Not Founc search1stFindObjectKind(eOBJECT_KIND_GETDOWN) \n");
			p_enemy->StartMotion(0, true, 0.5f);
			p_enemy->SetRotationToTarget(&sPlayer.Position());
			
			MSG_TYPE_MESSAGE();
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
			MSG_FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;
	case 1:
		if (MSG_IS_FADE() == false) {
			SoundManager::GetInstance()->StopBGM();
			sEventPhase ++;
		}
		break;

	case 2:
		MSG_TYPE_MESSAGE();
		RENDER_MESSAGE("砂の神", 
					   "なんだねキミは！？", 
					   "そんな高い所からミーを狙っていたのかね？",
					   "こらーッ！！！",  
					   "");
		if (CHECK_TRIG_ANY_JOY()) {
			SoundManager::GetSndComponent()->PlaySound(eSE_TORNADE);
			StageFunc040* p_stage = (StageFunc040 *)spStageFunc;
			p_stage->SetTornadeMorphNumDefault();

			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			MSG_FADE_OUT(0.3f);

			sEventPhase ++;
		}
		break;

	case 3:
		if (sEventCount > 90) {
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
			MSG_FADE_IN(0.3f);
			sEventPhase ++;
		}
		break;

	case 4:
		RENDER_MESSAGE("ナビォ", 
					   "リノーッ！！ダメだ", 
					   "時間切れだ！奴にばれちまった！",
					   "もうこの作戦は通用しないッ！",  
					   "");
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;

	case 5:
		RENDER_MESSAGE("ナビォ",
					   "もうだめだぁ・・・おしまいだぁ！",
					   "",
					   "",
					   "");
		if (CHECK_TRIG_ANY_JOY()) {
			MSG_FADE_IN(0.3f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			sEventPhase ++;
		}
		break;

	case 6:
		FADE_BOARD()->StartFade(200, 3.0f);
		spScenBase->SetPhase(SceneMainGame::ePHASE_GAMEOVER_BEFORE);

		return true;

	default:
		break;
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event40_GetMagicThunder(void)
{
	if (TotalInfo::GetInstance()->IsKeyItem(eKEYITEM_BOOK) == FALSE) {
		return eventGeneral_NotHaveBook();
	}
	else {
		return eventGeneral_GetMagic(eITEM_KIND_MAGIC_THUNDER);
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event41_Dummy00(void)
{
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event41_Dummy01(void)
{
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event41_WelcomTower(void)
{
	const int model_num = 1;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		// 必要なリソースを読み込む
		EventManager::GetInstance()->LoadCameraFile("data/Camera/041_02_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		sEventCsv.LoadFile("data/CSV/event41_02.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		startCsvAction2nd();

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);

		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();
		SET_EVENT_END_FLAG();

		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event41_PowerOn(void)
{
	const int model_num = 0;

	bool is_end = false;
	float size = 0;

	switch (sEventPhase) {
	case 0:
		SET_EVENT_END_FLAG();

		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		// 必要なリソースを読み込む
		EventManager::GetInstance()->LoadCameraFile("data/Camera/041_03_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(model_num);

		sEventCsv.LoadFile("data/CSV/event41_03.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		startCsvAction2nd();

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);

		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();

		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event41_RevertGuardian(void)
{
	switch (sEventPhase) {
	case 0:
		if (MSG_IS_FADE() == false) {
			MSG_TYPE_MESSAGE();
			MSG_FADE_IN(0.3f);
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
			sEventPhase ++;
		}
		break;

	case 1:
		if (MSG_IS_FADE() == false) {
			sEventPhase ++;
		}
		break;
		
	case 2:
		// 既に運ぶ済の場合
		if (false) {
			//		if (IS_END_EVT(eEVENT_16, eEVENT_16_USAEMON_COME_2)) {
			RENDER_MESSAGE("きつーね", "あ、もううさちゅうを湖まで運んでくれたんだね！", "ありがとう！",  "",  "");
			if (CHECK_TRIG_ANY_JOY()) {
				SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
				MSG_FADE_OUT(0.3f);
				return true;
			}
		}
		// それ以外
		else {
			RENDER_MESSAGE("ナビォ", "ロボットを元の場所に戻す？", "",  "",  "");
			Utility::eQuestion qs;
			qs = Utility::ExecuteQuestion();
			if (qs == Utility::eQUESTION_YES) {
				FADE_OUT(0.5f);
				sEventPhase ++;
			}
			if (qs == Utility::eQUESTION_NO)	return true;	// 終了
		}
		break;

	case 3:
		if (IS_FADE() == false) {
			SoundManager::GetSndComponent()->PlaySound(eSE_WARP);

			StageFunc041* p_stage = (StageFunc041 *)spStageFunc;

			p_stage->RevertGuardian();

			/*
			VECTOR pos;	float rot;
			getPlayerPosRotFromBGModel(&pos, &rot, 13);
			ObjectBase* p_object = search1stFindObjectKind(eOBJECT_KIND_USACHU);
			p_object->Position() = pos;
			EffekseerManager::GetInstance()->Play(eEFK_HEAL, &pos, 20.0f);
			*/
			

			return true;
		}
		break;
		
	default:
		break;
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event41_BtlTreasureA(void)
{
	return eventGeneral_BattleTreasure();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event41_BtlTreasureB(void)
{
	return eventGeneral_BattleTreasure();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event41_BtlTreasureC(void)
{
	return eventGeneral_BattleTreasure();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event41_BigKey(void)
{
	if (TotalInfo::GetInstance()->GetHasFieldKey(sStageIndex)) {
		
		bool is_end = eventGeneral_ClearBarrier(StageFunc041::eBG_TYPE_GIMMICK_BIG_KEY, 0);
		if (is_end == true) {
			SET_EVENT_END_FLAG();
			return true;
		}
	} else {
		const char* strTalk[4] = {"この扉を開けるには", "大きな鍵が必要だ",  "",  ""};
		return eventGeneral_BlockCharactor(0, &VGet(0.0f, 0.0f, -450.0f), 180.0f, 
										   "", (char **)strTalk);
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event43_BossStart(void)
{
	const int model_num = 3;

	StageFunc043* p_stage = (StageFunc043 *)spStageFunc;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		// 必要なリソースを読み込む
		ResourceManager::GetInstance()->LoadCharaResource(NAME_MekaDragon);
//		ResourceLayout::GetInstance()->LoadLayoutResource((char *)GetEnemyWorkTable(eENEMY_KIND_MEKADRAGON)->mLayoutName);

		EventManager::GetInstance()->LoadCameraFile("data/Camera/043_00_Camera.vmd");

		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,		eMT_IDLE_NOWEAPON,	&sModelMap, true);
		EventManager::GetInstance()->SetModelParameter(1, NAME_MekaDragon,	0,					NULL,		true);
		EventManager::GetInstance()->SetModelParameter(2, NAME_Navi,		0,					NULL,		true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		{
			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
			p_model->Rotation().x = Utility::DegToRad(-14.9f);
			float scale = 36.0f;
			p_model->Scale() = VGet(scale, scale, scale);
		}
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
		eventGeneral_NaviLuminusStart(p_model);

		sEventCsv.LoadFile("data/CSV/event43_00.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		startCsvAction2nd();

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);

		eventGeneral_NaviLuminusUpdate( (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2) );

		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();

		VECTOR pos;	float rot;
		getPlayerPosRotFromBGModel(&pos, &rot, 1);
		sPlayer.Position() = pos;

		getPlayerPosRotFromBGModel(&pos, &rot, 4);
		createEnemyWork2(0, eENEMY_KIND_MEKADRAGON, &pos);
		CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_BATTLE_WALL);
		sModelMap.VisibleBattleBox();

		sPlayer.SetRotationToTarget(&pos);

		// バトルフラグを立てる
		sEventBattleFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_43, eEVENT_43_END_BOSS_BATTLE);
		
		// リカバリー、またはゲームオーバー時にクリアされるフラグを設定
		sEventRecoveryFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_43, eEVENT_43_BOSS_APPEAR);

		//		RenderBloom::SetActiveBloom(FALSE);


		SoundManager::GetInstance()->PlayBGM(BGM_BOSS_D);

		p_stage->SetPatternObject(1);
		p_stage->SetPatternLight(1);
		
		{
			StageFunc043* p_stage = (StageFunc043 *)SceneMainGame::GetStageFunc();
			p_stage->StartFadeOutWall();
		}

		//		sModelMap.SetVisible(false, eBG_TYPE_VISIBLE);

		SET_EVENT_END_FLAG();

		return true;

	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event43_EndBossBattle(void)
{
	sDungeonIndex = 4;
	spScenBase->SetPhase(SceneMainGame::ePHASE_STAGECLEAR);

	// バトルフラグを立てる
	sEventBattleFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_43, eEVENT_43_BTL_TREASURE_A);

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event43_BtlTreasureA(void)
{
	bool is_end;

	StageFunc043* p_stage = (StageFunc043 *)spStageFunc;

	if (sEventPhase == 0) {
		ObjectBase* p_object = search1stFindObjectKind(eOBJECT_KIND_CRYSTAL);
		APP_ASSERT( (p_object != NULL), "Not Found eOBJECT_KIND_CRYSTAL \n");
		processAppearCrystal(1,			// stagePlaceIndex
							 p_object,	// pCrystalObject
							 1,			// crystalColor : 1 : Yellow
							 TRUE,
							 &p_stage->mEffHndAura,
							 &p_stage->mEffHndCrystal);
	}
		
	is_end = eventGeneral_BattleTreasure();
	if (is_end == true) {
		GraphicManager::tFogParam fogParam;
		fogParam.mIsEnable = TRUE;
		fogParam.mIsActive = TRUE;
		fogParam.mColR = 0;
		fogParam.mColG = 0;
		fogParam.mColB = 0;
		fogParam.mNear = 100.0f;
		fogParam.mFar = 18000.0f;
		GraphicManager::GetInstance()->ChangeFogParam(1.0f, &fogParam);
		GraphicManager::GetInstance()->BackupFogParam();
		
		/****  再生のBGM ****/
		SoundManager::GetInstance()->PlayBGM(BGM_FORCE_PLACE);
		
		SET_EVENT_END_FLAG();
		return true;
	 }
	 return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event44_ElevatorUp(void)
{
	const int model_num = 1;
	
	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		// 必要なリソースを読み込む
		if (sStagePlaceIndex == 0) {
			EventManager::GetInstance()->LoadCameraFile("data/Camera/044_00_Camera.vmd");
		} else if (sStagePlaceIndex == 1) {
			EventManager::GetInstance()->LoadCameraFile("data/Camera/044_01_Camera.vmd");
		}

		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		sEventCsv.LoadFile("data/CSV/event44_00.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		startCsvAction2nd();

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);

		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();

		ActionTaskManager::tTaskInfo param;
		param.mTaskType = ActionTaskManager::eTASK_MOVE;
		if (sStagePlaceIndex == 0) {
			param.mTaskValueA = 45;
			param.mTaskValueB = 0;
		} else if (sStagePlaceIndex == 1) {
			param.mTaskValueA = 41;
			param.mTaskValueB = 1;
		}
		ActionTaskManager::GetInstance()->NotifyAction(&param);
		//			return true;
		sEventPhase ++;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event46_HelpSeek(void)
{
	const int model_num = 2;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		// ※ ここのイベントはこのスケール値で作っている為、捻じ曲げて対応
		sModelMap.Scale() = VGet(12.0f, 12.0f, 12.0f);

		// 必要なリソースを読み込む
		ResourceManager::GetInstance()->LoadCharaResource(NAME_Seek);

		// 必要なリソースを読み込む
		EventManager::GetInstance()->LoadCameraFile("data/Camera/046_00_Camera.vmd");

		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Seek,	0,						NULL,		true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader(0.02f, 0.02f);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawSeekFunc);		/* シーク専用の描画関数を設定 */
		p_model->EnableToonShader(0.008f, 0.008f);
		p_model->StartMotion(16, false, 0.5f, NULL, 1.0f);

		sEventCsv.LoadFile("data/CSV/event46_00.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		startCsvAction2nd();

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);

		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();

		VISIBLE_BATTLE_ICON();

		// ※ ここのイベントはこのスケール値で作っている為、捻じ曲げて対応
		sModelMap.Scale() = VGet(spBGInfo->mScale, spBGInfo->mScale, spBGInfo->mScale);

		// 必要なリソースを読み込む
		ResourceManager::GetInstance()->LoadCharaResource(NAME_SA_14);
//		ResourceLayout::GetInstance()->LoadLayoutResource((char *)GetEnemyWorkTable(eENEMY_KIND_SA_14)->mLayoutName);

		VECTOR pos;	float rot;
		getPlayerPosRotFromBGModel(&pos, &rot, 4);
		sPlayer.Position() = pos;
		sPlayer.Rotation().y = rot;

		getPlayerPosRotFromBGModel(&pos, &rot, 2);
		createEnemyWork2(0, eENEMY_KIND_SA_14, &pos);
		CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_BATTLE_WALL);
		sModelMap.VisibleBattleBox();

		SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);

		// バトルフラグを立てる
		sEventBattleFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_46, eEVENT_46_END_ENEMY_BATTLE);
		
		// リカバリー、またはゲームオーバー時にクリアされるフラグを設定
		sEventRecoveryFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_46, eEVENT_46_HELP_SEEK);

		SET_EVENT_END_FLAG();

		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event46_EndEnemyBattle(void)
{
	const int model_num = 3;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		// ※ ここのイベントはこのスケール値で作っている為、捻じ曲げて対応
		sModelMap.Scale() = VGet(12.0f, 12.0f, 12.0f);

		// 扉を非表示にする
		sModelMap.SetVisible(false, eBG_TYPE_GIMMICK + 0);
		CollisionManager::GetInstance()->RemoveWallCollision(sModelMap.GetModelHandle(), (eBG_TYPE_GIMMICK + 0));

		// 必要なリソースを読み込む
		ResourceManager::GetInstance()->LoadCharaResource(NAME_Seek);

		// 必要なリソースを読み込む
		EventManager::GetInstance()->LoadCameraFile("data/Camera/046_01_Camera.vmd");

		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Seek,	0,						NULL,		true);
		EventManager::GetInstance()->SetModelParameter(2, NAME_Navi,	0,					NULL,		true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader(0.02f, 0.02f);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawSeekFunc);		/* シーク専用の描画関数を設定 */
		p_model->EnableToonShader(0.01f, 0.01f);
		p_model->StartMotion(16, false, 0.5f, NULL, 1.0f);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
		eventGeneral_NaviLuminusStart(p_model);

		sEventCsv.LoadFile("data/CSV/event46_01.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		startCsvAction2nd();

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);

		// 落下防止
		sPlayer.SetNowGravity(0.0f);

		eventGeneral_NaviLuminusUpdate( (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2) );

		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();

		VISIBLE_BATTLE_ICON();

		// ※ ここのイベントはこのスケール値で作っている為、捻じ曲げて対応
		sModelMap.Scale() = VGet(spBGInfo->mScale, spBGInfo->mScale, spBGInfo->mScale);

		SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);

		{
			VECTOR pos;	float rot;
			getPlayerPosRotFromBGModel(&pos, &rot, 5);
			sPlayer.Position() = pos;
			sPlayer.Rotation().y = rot;
		}

		SET_EVENT_END_FLAG();

		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event46_WaterIsLower(void)
{
	SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);

	StageFunc046* p_stage = (StageFunc046 *)spStageFunc;
	p_stage->SetWaterType(StageFunc046::eWATER_TYPE_DOWN_GOING);

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event46_WaterIsUpper(void)
{
	SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);

	StageFunc046* p_stage = (StageFunc046 *)spStageFunc;
	p_stage->SetWaterType(StageFunc046::eWATER_TYPE_UP_GOING);

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event47_WelcomeAirship(void)
{
	const int model_num = 2;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		// 必要なリソースを読み込む
		EventManager::GetInstance()->LoadCameraFile("data/Camera/047_00_Camera.vmd");

		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,	&sModelMap, true);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Navi,	0,					NULL,		true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader(0.02f, 0.02f);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusStart(p_model);

		sEventCsv.LoadFile("data/CSV/event47_00.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		startCsvAction2nd();

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);

		eventGeneral_NaviLuminusUpdate( (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1) );

		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();

		VISIBLE_BATTLE_ICON();

		SET_EVENT_END_FLAG();

		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event47_TalkSeek(void)
{
	const int model_num = 3;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		// 続きのイベントを指定
		sNextEventNo = eEVENT_47_STAFF_ROLL;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		// 必要なリソースを読み込む
		ResourceManager::GetInstance()->LoadCharaResource(NAME_Seek);

		// 必要なリソースを読み込む
		EventManager::GetInstance()->LoadCameraFile("data/Camera/047_01_Camera.vmd");

		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Navi,	0,						NULL,		true);
		EventManager::GetInstance()->SetModelParameter(2, NAME_Seek,	0,						NULL,		true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader(0.02f, 0.02f);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusStart(p_model);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawSeekFunc);		/* シーク専用の描画関数を設定 */
		p_model->EnableToonShader(0.015f, 0.015f);

		sEventCsv.LoadFile("data/CSV/event47_01.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		startCsvAction2nd();

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);

		eventGeneral_NaviLuminusUpdate( (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1) );

		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();

		VISIBLE_BATTLE_ICON();

		VECTOR pos; float rot;
		getPlayerPosRotFromBGModel(&pos, &rot, 4);
		sPlayer.Position() = pos;
		sPlayer.Rotation().y = rot;

		SET_EVENT_END_FLAG();

		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event47_StaffRoll(void)
{
	const int model_num = 10;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	StageFunc047* p_stage = (StageFunc047 *)spStageFunc;
	StageFunc047::tStaffRollWork* p_staff = &((StageFunc047 *)spStageFunc)->mStaffroll;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		// 続きのイベントを指定
		sNextEventNo = eEVENT_47_CRASH_AIRSHIP;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		// 必要なリソースを読み込む
		ResourceManager::GetInstance()->LoadCharaResource(NAME_Seek);
		ResourceManager::GetInstance()->LoadCharaResource(NAME_Bird);

		ResourceLayout::GetInstance()->LoadLayoutResource("Ending_Main.png");
		ResourceLayout::GetInstance()->LoadLayoutResource("Ending_Sub.png");

		ResourceLayout::GetInstance()->LoadLayoutResource("TitleLogo.jpg");

		// 必要なリソースを読み込む
		EventManager::GetInstance()->LoadCameraFile("data/Camera/047_02_Camera.vmd");

		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Navi,	0,						NULL,		true);
		EventManager::GetInstance()->SetModelParameter(2, NAME_Seek,	0,						NULL,		true);

		EventManager::GetInstance()->SetModelParameter(3, NAME_Bird,	0,						NULL,		true);
		EventManager::GetInstance()->SetModelParameter(4, NAME_Bird,	0,						NULL,		true);
		EventManager::GetInstance()->SetModelParameter(5, NAME_Bird,	0,						NULL,		true);
		EventManager::GetInstance()->SetModelParameter(6, NAME_Bird,	0,						NULL,		true);
		EventManager::GetInstance()->SetModelParameter(7, NAME_Bird,	0,						NULL,		true);
		EventManager::GetInstance()->SetModelParameter(8, NAME_Bird,	0,						NULL,		true);
		EventManager::GetInstance()->SetModelParameter(9, NAME_Bird,	0,						NULL,		true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader(0.02f, 0.02f);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusStart(p_model);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawSeekFunc);		/* シーク専用の描画関数を設定 */
		p_model->EnableToonShader(0.015f, 0.015f);

		sModelXfile[eMD_SKYDOME].Position().y = -10000.0f;
		sModelXfile[eMD_SKYDOME].Rotation().y = 1.9f;
		
		sEventCsv.LoadFile("data/CSV/event47_02.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		startCsvAction2nd();

		// 疑似フェード板
		{
			LayoutBase* p_fade_board = EventManager::GetInstance()->GetLayoutBase(0);
			float aspect_rate = GraphicManager::GetInstance()->GetAspectRate();
			p_fade_board->LinkLayout("fade_board.png");
			p_fade_board->SetPositionByCentering(320, 240);
			p_fade_board->SetSize( (int)(642.0f / aspect_rate), 480 );
			p_fade_board->SetColor(0, 0, 0);
			p_fade_board->SetAlpha(255);

			// 正式フェード板は非表示にする
			FADE_IN(0.1f);

			// タイトルロゴ
			static const tLayoutSetting sLayoutSettingLog = {
				"TitleLogo.jpg", 320, -9999, 128, 96, 255,
			};
			const tLayoutSetting* p_setting;
			LayoutBase* p_layout = NULL;
			p_setting = &sLayoutSettingLog;

			if (p_stage->mpTitleLog == NULL) {
				p_stage->mpTitleLog = new LayoutBase();
			}
			APP_ASSERT((p_stage->mpTitleLog != NULL),
					   "Failed new LayoutBase() \n");
			p_layout = p_stage->mpTitleLog;
			p_layout->LinkLayout(p_setting->mStrLayoutName);
			p_layout->SetPositionByCentering(p_setting->mPosX, p_setting->mPosY);
			p_layout->SetSize(p_setting->mSizeX, p_setting->mSizeY);
			p_layout->SetAlpha(p_setting->mAlpha);
			GraphicManager::GetInstance()->EntryDrawLayout(p_layout);
		}

		// エンディング用のフォントを作成　＆　設定する
		GraphicManager::GetInstance()->CreateEndingFontData();
		GraphicManager::GetInstance()->SetFontType(GraphicManager::eFONT_TYPE_ENDING);

		// スタッフロールの演出開始
		p_staff->mCurCsvFileIndex = 0;
		startStaffRollEvent(p_staff->mCurCsvFileIndex);
		GraphicManager::GetInstance()->EntryDrawFontFunc(DrawFontFunc_StaffRollString);

		//		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		{
			/* CSVファイルのアクションを更新 */
			is_end = updateCsvAction2nd(&sEventCsv);
			eventGeneral_NaviLuminusUpdate( (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1) );

			/* スタッフロールの開始 */
			if (EventManager::GetInstance()->BetweenPhaseFrame(290, 99999)) {
				// 現在読み込み中のCSVファイルが終了しているか？
				if (isEndStaffRollEvent() ) {
					if ((p_staff->mCurCsvFileIndex + 1) >= kSTAFFROLL_CSV_NUM) {
						// 全てのCSVファイルの読み込みが終了
						GraphicManager::GetInstance()->EntryDrawFontFunc(NULL);
						SoundManager::GetInstance()->StartFadeVolumeBGM(0, 0.5f);
						sEventPhase ++;
						sEventCount = 0;
					}
					else {
						// 新しいCSVファイルのロードを行う
						p_staff->mCurCsvFileIndex ++;
						startStaffRollEvent(p_staff->mCurCsvFileIndex);
					}
				}
				else {
					// スタッフローンイベント更新
					updateStaffRollEvent();
				}
			}

			if (is_end) {
				FADE_OUT(0.5f);

				GraphicManager::GetInstance()->EntryDrawFontFunc(NULL);
				SoundManager::GetInstance()->StartFadeVolumeBGM(0, 0.5f);

				sEventPhase = 4;
				sEventCount = 0;
			}

			const float fade_start_frame	= 4300.0f;
			const float fade_end_frame		= 5300.0f;
			const float fade_start_color	= 0.09f;
			const float fade_end_color		= 0.87f;

			const float fade_start_diff		= 0.86f;
			const float fade_end_diff		= 0.92f;

			// どんどん夕方に変えていく 
			if (EventManager::GetInstance()->BetweenPhaseFrame(fade_start_frame, fade_end_frame) ) {
				
				float color_red;
				float now_frame = EventManager::GetInstance()->GetPhaseFrame();
				float fade_rate = (now_frame - fade_start_frame) / (fade_end_frame - fade_start_frame);
				
				color_red = fade_rate * (fade_end_diff - fade_start_diff);
				color_red += fade_start_diff;
				SetLightDifColor(GetColorF(color_red, 0.811760f, 0.600000f, 0.000000f) );

				color_red = fade_rate * (fade_end_color - fade_start_color);
				color_red += fade_start_color;
				SetLightAmbColor(GetColorF(color_red, 0.105880f, 0.278430f, 0.000000f) );
			}

		}
		break;
		
	case 3:
		if (sEventCount > 240) {
			sEventPhase ++;
		}
		break;

	case 4:
		{
			/*
			LayoutBase* p_layout = NULL;
			p_layout = EventManager::GetInstance()->GetLayoutBase(1);
			p_layout->StartFade(0, 0.5f); 
			p_layout = EventManager::GetInstance()->GetLayoutBase(2);
			p_layout->StartFade(0, 0.5f); 
			*/

			// エンディング用のフォントを戻す　＆　破棄する
			GraphicManager::GetInstance()->SetFontType(GraphicManager::eFONT_TYPE_DEFAULT);
			GraphicManager::GetInstance()->DeleteEndingFontData();

			GraphicManager::GetInstance()->EntryDrawFontFunc(NULL);

			sEventPhase ++;
		}
		break;

	case 5:
		if (IS_FADE() == true) return false;
		//		if (MSG_IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();

		// 疑似フェード板
		{
			LayoutBase* p_fade_board = EventManager::GetInstance()->GetLayoutBase(0);
			APP_ASSERT((p_fade_board != NULL),
					   "p_fade_board is NULL \n");
			p_fade_board->SetAlpha(0);
			p_fade_board->SetColor(255, 255, 255);

			StageFunc047* p_stage = (StageFunc047 *)spStageFunc;;
			if (p_stage->mpTitleLog != NULL) {
				GraphicManager::GetInstance()->RemoveDrawLayout(p_stage->mpTitleLog);
				delete p_stage->mpTitleLog;
				p_stage->mpTitleLog = NULL;
			}
		}

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();

		GraphicManager::GetInstance()->SetEnableDirectionLight(TRUE);
		GraphicManager::GetInstance()->SetDefaultDirectionLight();

		VISIBLE_BATTLE_ICON();

		SoundManager::GetInstance()->PlayBGM("");

		SET_EVENT_END_FLAG();

		return true;
		
	default:
		APP_ASSERT(NULL, "Invalid case \n");
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event47_FallAirship(void)
{
	StageFunc047::tStaffRollWork* p_staff = &((StageFunc047 *)spStageFunc)->mStaffroll;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;

	case 1:
		if (IS_FADE() == true) return false;
		
		// エンディング用のフォントを作成　＆　設定する
		GraphicManager::GetInstance()->CreateEndingFontData();
		GraphicManager::GetInstance()->SetFontType(GraphicManager::eFONT_TYPE_ENDING);

		// スタッフロールの演出開始
		p_staff->mCurCsvFileIndex = 0;
		startStaffRollEvent(p_staff->mCurCsvFileIndex);
		GraphicManager::GetInstance()->EntryDrawFontFunc(DrawFontFunc_StaffRollString);

		sEventPhase ++;
		break;

	case 2:

		// 現在読み込み中のCSVファイルが終了しているか？
		if (isEndStaffRollEvent() ) {
			if ((p_staff->mCurCsvFileIndex + 1) >= kSTAFFROLL_CSV_NUM) {
				// 全てのCSVファイルの読み込みが終了
				GraphicManager::GetInstance()->EntryDrawFontFunc(NULL);
				sEventPhase ++;
			}
			else {
				// 新しいCSVファイルのロードを行う
				p_staff->mCurCsvFileIndex ++;
				startStaffRollEvent(p_staff->mCurCsvFileIndex);
			}
		}
		else {
			// スタッフローンイベント更新
			updateStaffRollEvent();
		}

		break;

	case 3:
		// エンディング用のフォントを戻す　＆　破棄する
		GraphicManager::GetInstance()->SetFontType(GraphicManager::eFONT_TYPE_DEFAULT);
		GraphicManager::GetInstance()->DeleteEndingFontData();

		GraphicManager::GetInstance()->SetEnableDirectionLight(TRUE);
		GraphicManager::GetInstance()->SetDefaultDirectionLight();

		VISIBLE_BATTLE_ICON();

		SoundManager::GetInstance()->PlayBGM("");

		SET_EVENT_END_FLAG();

		return true;

	default:
		APP_ASSERT(NULL, "Invalide case \n");
		break;
	};
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event47_ExitAirship(void)
{
	if (IS_END_EVT(eEVENT_48, eEVENT_48_APPEAR_SKYSHIP) == false) {
		ActionTaskManager::tTaskInfo param;
		param.mTaskType = ActionTaskManager::eTASK_MOVE;
		param.mTaskValueA = 41;
		param.mTaskValueB = 9;
		ActionTaskManager::GetInstance()->NotifyAction(&param);
	}
	else {
		const char* strTalk[4] = {"リノ！", "飛行中は外に出れないよ！",  "",  ""};
		return eventGeneral_BlockCharactor(0, &VGet(0.0f, 0.0f, 450.0f), 180.0f,
										   "ナビォ", (char **)strTalk);
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event47_CrashAirship(void)
{
	static const VECTOR offset = VScale(ZERO_VEC, 10.0f);

	static const int model_num = 4;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		// 必要なリソースを読み込む
		ResourceManager::GetInstance()->LoadCharaResource(NAME_Seek);
		ResourceManager::GetInstance()->LoadCharaResource(NAME_Ortiz);

		EventManager::GetInstance()->LoadCameraFile("data/Camera/047_05_Camera.vmd", 10);
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	0,					&sModelMap, false);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Navi,	0,					NULL,		true);
		EventManager::GetInstance()->SetModelParameter(2, NAME_Seek,	0,					NULL,		true);
		EventManager::GetInstance()->SetModelParameter(3, NAME_Ortiz,	0,					NULL,		true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();
		p_model->SetMeshVisible(false,	kSHEATH_FRAME_INDEX);
		p_model->SetMeshVisible(false,	kHILT_FRAME_INDEX);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusStart(p_model);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawSeekFunc);		/* シーク専用の描画関数を設定 */
		p_model->EnableToonShader(0.02f, 0.02f);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(3);
		p_model->EnableToonShader(0.01f, 0.01f);

		sEventCsv.LoadFile("data/CSV/event47_05.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv, &offset);

		startCsvAction2nd();

		///		FADE_IN(0.5f); //<! あえてフェードインさせない
		{
			// Skyエフェクトを消す
			StageFunc047* p_stage = (StageFunc047 *)spStageFunc;
			STOP_EFFECT(p_stage->mEffHndSky);

			// 飛空艇非表示
			GimmickBase* p_gimmick = GimmickBase::Search1stFindGimmickKind(eGIMMICK_KIND_AIRSHIP);
			APP_ASSERT((p_gimmick != NULL), "Not found gimmick \n");
			p_gimmick->SetVisible(false);
		}

		sEventPhase ++;
		break;
		
	case 2:
		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusUpdate(p_model);

		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);
		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();
		SET_EVENT_END_FLAG();


		sModelMap.SetVisible(true);
		for (unsigned int iI = 0; iI < sSubStageInfoList.size(); iI ++) {
			spSubModelMap[iI].SetVisible(true);
		}
		sModelXfile[eMD_SKYDOME].SetVisible(true);

		{
			ActionTaskManager::tTaskInfo param = {ActionTaskManager::eTASK_MOVE, 35, 0};
			ActionTaskManager::GetInstance()->NotifyAction(&param);
		}
		
		//		return true;
		
	default:
		break;
	};

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event48_AppearAirship(void)
{
	const int model_num = 5;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	StageFunc048* p_stage = (StageFunc048 *)spStageFunc;

	switch (sEventPhase) {
	case 0:
		SoundManager::GetInstance()->PlayBGM(BGM_DUNGEON_C);
		FADE_OUT(0.5f);
		sEventPhase ++;

		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		// 必要なリソースを読み込む
		ResourceManager::GetInstance()->LoadCharaResource((char *)NAME_Rafia);
		ResourceManager::GetInstance()->LoadCharaResource((char *)NAME_MobClockW);
		ResourceModel::GetInstance()->LoadGimmickResource((char *)RES_MDL_AIRSHIP);

		// 必要なリソースを読み込む
		EventManager::GetInstance()->LoadCameraFile("data/Camera/040_09_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Navi,		0,				NULL,		true);
		EventManager::GetInstance()->SetModelParameter(2, NAME_Rafia,		0,				NULL,		true);
		EventManager::GetInstance()->SetModelParameter(3, RES_MDL_AIRSHIP,	0,				NULL,		true);
		EventManager::GetInstance()->SetModelParameter(4, NAME_MobClockW,	0,				NULL,		true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader(0.02f, 0.02f);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1); // Navi
		eventGeneral_NaviLuminusStart(p_model);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawRafiaFunc);		/* ラフィア専用の描画関数を設定 */

		sEventCsv.LoadFile("data/CSV/event40_09.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		{
			ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_SKYDOME_BLUE_A);
			int graph_handle = ResourceLayout::GetInstance()->GetHandle(RES_LYT_SKYDOME_BLUE_A);
			MV1SetTextureGraphHandle(sModelXfile[eMD_SKYDOME].GetModelHandle(), 0,graph_handle, FALSE);
		}
		
		startCsvAction2nd();

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusUpdate(p_model);

		if (is_end) {
			EventManager::GetInstance()->PauseEvent();

			GraphicManager::GetInstance()->EntryDrawFontFunc((FUNC_DRAW_FONT)DrawFreeFontFunc);
			sDrawFreeFontValue = eFREE_FONT_APPEAR_SKYSHIP_EVT_2;

			p_stage->mSlapCntPlayer = 0;
			p_stage->mSlapCntEnemy = 0;
			p_stage->mRafiaTalkIndex = -1;
			p_stage->mIsSlapBattleWin = FALSE;

			/* 各キャラを配置させる */
			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
			p_model->Position() = VGet(-5063.10f, 24972.40f, 33402.00f);
			p_model->StartMotion(eMT_IDLE, true, 0.5f, NULL);

			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
			p_model->Position() = VGet(-4912.20f, 24972.32f, 33419.40f);

			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
			p_model->Position() = VGet(-5119.75f, 24969.96f, 33484.68f);
			p_model->SetVisible(true);
			p_model->SetAlpha(1.0f);
			((CharaNpc *)p_model)->EnableShadow();
			p_model->StartMotion(eMT_DAMAGE_SLAPPING, true, 0.25f, NULL, 1.0f);

			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(4);
			p_model->SetVisible(false);
			p_model->SetAlpha(0.0f);
			((CharaNpc *)p_model)->DisableShadow();

			CameraManager::GetInstance()->Position() = VGet(-5156.45f, 25158.98f, 34343.66f);
			CameraManager::GetInstance()->Target() = VGet(-5044.00f, 25037.90f, 33489.50f);

			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
			p_model->SetRotationToTarget(&EventManager::GetInstance()->GetModelPmd(2)->Position());

			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
			p_model->SetRotationToTarget(&EventManager::GetInstance()->GetModelPmd(0)->Position());

			SoundManager::GetInstance()->StopBGM();

			sEventPhase ++;
		}
		break;
		
	case 3:		// ビンタ合戦
		is_end = false;

		{
			static const BYTE slap_max = 6;
			bool is_hit = false;

			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
			eventGeneral_NaviLuminusUpdate(p_model);

			ModelPmd* p_player = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
			ModelPmd* p_enemy  = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
			
			// 自キャラの処理
			if (p_player->GetMotionIndex() == eMT_ATTACK_SLAPPING) {
				if (p_player->EqualPhaseFrame(22.0f)) {
					p_enemy->StartMotion(eMT_RAFIA_DAMAGE_SLAPPING, false, 0.5f, &sMotionParam[eMT_IDLE]);
					is_hit = true;
					p_stage->mSlapCntEnemy ++;
				}
			}
			if (p_player->GetMotionIndex() == eMT_IDLE) {
				if (p_stage->mSlapCntEnemy < slap_max) {
					if (CHECK_TRIG_ANY_JOY() ) {
						p_player->StartMotion(eMT_ATTACK_SLAPPING, false, 0.5f, &sMotionParam[eMT_IDLE]);
					}
				}
			}
			if (p_stage->mSlapCntEnemy == slap_max) {
				p_enemy->StartMotion(eMT_RAFIA_HIP_DOWN, false, 0.5f, NULL);
				if (p_enemy->GetIsEndMotion()) {
					p_stage->mIsSlapBattleWin = TRUE;
					is_end = true;
				}
			}

			// 敵キャラの処理
			if (p_enemy->GetMotionIndex() == eMT_RAFIA_ATTACK_SLAPPING) {
				if (p_enemy->EqualPhaseFrame(22.0f)) {
					p_player->StartMotion(eMT_DAMAGE_SLAPPING, false, 0.5f, &sMotionParam[eMT_IDLE]);
					is_hit = true;
					p_stage->mSlapCntPlayer ++;
				}
			}
			if (p_enemy->GetMotionIndex() == eMT_IDLE) {
				if ((p_stage->mSlapCntPlayer < slap_max) && (p_stage->mSlapCntEnemy != 0)) {
					if (GetRand(10) == 0) {
						p_enemy->StartMotion(eMT_RAFIA_ATTACK_SLAPPING, false, 0.5f, &sMotionParam[eMT_IDLE]);

						//						SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
						MSG_TYPE_MESSAGE();
						MSG_FADE_IN(0.2f);
						p_stage->mRafiaTalkIndex = GetRand(5);
					}
				}
			}
			if (p_stage->mSlapCntPlayer == slap_max) {
				p_player->StartMotion(eMT_DOWN_HIP, false, 0.5f, NULL);
				if (p_player->GetIsEndMotion()) {
					p_stage->mIsSlapBattleWin = FALSE;
					is_end = true;
				}
			}

			// メッセージ処理
			if (p_stage->mRafiaTalkIndex != -1) {
				if (MSG_IS_FADE() == false) {
					static const char* strRafiaTalk[] = {
						"やったわね！",
						"お返し！",
						"こいつ！",
						"やめなさい！",
						"いたいわね！",
						"いい加減にしなさい！",
					};

					APP_ASSERT_PRINT((p_stage->mRafiaTalkIndex < sizeof(strRafiaTalk) / sizeof(const char*)),
									 "Invalid access strRafiaTalk[] \n");
					RENDER_MESSAGE("ラフィア", (char *)strRafiaTalk[p_stage->mRafiaTalkIndex], "", "", "");
				}
			}
			if (is_hit) {
				SoundManager::GetSndComponent()->PlaySound(eSE_SNAPPING);
				if (p_stage->mRafiaTalkIndex != -1) {
					MSG_FADE_OUT(0.2f);
					p_stage->mRafiaTalkIndex = -1;
				}
			}
		}

		if (is_end) {
			GraphicManager::GetInstance()->EntryDrawFontFunc(NULL);
			sDrawFreeFontValue = eFREE_FONT_NONE;

			FADE_OUT(0.5f);
			sEventPhase ++;
		}

		break;

	case 4:
		if (IS_FADE() == true) return false;

			GraphicManager::GetInstance()->EntryDrawFontFunc(NULL);
			sDrawFreeFontValue = eFREE_FONT_NONE;

			EventManager::GetInstance()->EndEvent();
			sEventCsv.DeleteFile();

			EventManager::GetInstance()->LoadCameraFile("data/Camera/040_09_02_Camera.vmd");
			EventManager::GetInstance()->EntryModelNum(model_num-1);
			EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);
			EventManager::GetInstance()->SetModelParameter(1, NAME_Navi,		0,				NULL,		true);
			EventManager::GetInstance()->SetModelParameter(2, NAME_Rafia,		0,				NULL,		true);
			EventManager::GetInstance()->SetModelParameter(3, RES_MDL_AIRSHIP,	0,				NULL,		true);

			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
			p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
			p_model->EnableToonShader(0.02f, 0.02f);
			
			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(2);
			p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawRafiaFunc);		/* ラフィア専用の描画関数を設定 */
			

			// ビンタ戦の状況に応じてCSVの読み込み先を変える
			if (p_stage->mIsSlapBattleWin == TRUE) {
				sEventCsv.LoadFile("data/CSV/event40_09_02.csv");
			}
			else {
				sEventCsv.LoadFile("data/CSV/event40_09_03.csv");
			}

			EventManager::GetInstance()->StartEvent(&sEventCsv);
			startCsvAction2nd();

			// ブルーム設定
			{
				RenderBloom::SetBloomType(RenderBloom::eBLOOM_TYPE_BRIGHT_LESS);
				RenderBloom::SetBloomParam(22, 15, 1000);
				RenderBloom::tBloomBrightParam param = {0};
				param.mClipParam = 210;
				RenderBloom::BloomBrightParam() = param;

				// フォグ設定
				GraphicManager::GetInstance()->DisableFogPattern();

				// エミッシブ
				sModelMap.SetEnableEmissive(true);
			}

			FADE_IN(0.5f);

			sEventPhase ++;
		break;


	case 5:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusUpdate(p_model);

		/*
		if (EventManager::GetInstance()->EqualPhaseFrame(1090.0f)) {
			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
			EventManager::GetInstance()->GetModelPmd(3)->AttachModel(p_model, ModelPmd::eATTACHED_MODEL_MOTION, 2);
		}
		*/
		
		if (EventManager::GetInstance()->EqualPhaseFrame(680.0f)) {
			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(3);
			p_model->Position() = VGet(-351.83f, 2318.50f, 2798.61f);
		}
		else if (EventManager::GetInstance()->BetweenPhaseFrame(680.0f, 730.0f)) {
			VECTOR target_pos = VScale(VGet(-381.19f, 2612.40f, 2793.24f), 10.0f);
			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(3);
			p_model->Position() = VAdd(VScale(VSub(target_pos, p_model->Position()), 0.1f), p_model->Position());
		} 
		else if (EventManager::GetInstance()->BetweenPhaseFrame(730.0f, 762.0f)) {
			VECTOR target_pos = VScale(VGet(-385.62f, 2573.20f, 2792.44f), 10.0f);
			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(3);
			p_model->Position() = VAdd(VScale(VSub(target_pos, p_model->Position()), 0.1f), p_model->Position());
		}
		else if (EventManager::GetInstance()->BetweenPhaseFrame(762.0f, 792.0f)) {
			VECTOR target_pos = VScale(VGet(-390.93f, 2635.80f, 2791.47f), 10.0f);
			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(3);
			p_model->Position() = VAdd(VScale(VSub(target_pos, p_model->Position()), 0.1f), p_model->Position());
		}

		if (EventManager::GetInstance()->BetweenPhaseFrame(680.0f, 10000.0f)) {
		//		if (EventManager::GetInstance()->BetweenPhaseFrame(762.0f, 10000.0f)) {
			const float rate = 0.08f;
			const float scale = 2.6f;
			float diff = sin(rate * EventManager::GetInstance()->GetPhaseFrame()) * scale;
			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(3);
			p_model->Position().y += diff;
		}

		if (EventManager::GetInstance()->BetweenPhaseFrame(1090.0f, 10000.0f)) {
			// リノモデルを飛空船にアタッチ
			VECTOR pos; float rot;
			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(3);
			p_model->GetFramePosFromIndex(&pos, 2);
			rot = p_model->Rotation().y;
			
			p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
			p_model->Position() = pos;
			p_model->Rotation().y = rot + Utility::DegToRad(-90.0f);
		}

		// 空エフェクトを消す
		if (EventManager::GetInstance()->EqualPhaseFrame(1330.0f)) {
			if (p_stage->mEffHndSky != -1) {
				EffectManager::GetInstance()->Stop(p_stage->mEffHndSky, 0.5f);
				p_stage->mEffHndSky = -1;
			}
		}

		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;

	case 6:
		if (IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();

		VISIBLE_BATTLE_ICON();

		SET_EVENT_END_FLAG();


		ActionTaskManager::tTaskInfo param;
		param.mTaskType = ActionTaskManager::eTASK_MOVE;
		param.mTaskValueA = 47;
		param.mTaskValueB = 0;
		ActionTaskManager::GetInstance()->NotifyAction(&param);

		//		return true;
		sEventPhase ++;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool event48_MiddlePlace(void)
{
	const int model_num = 2;

	ModelPmd* p_model = NULL;
	bool is_end = false;
	float size = 0;

	switch (sEventPhase) {
	case 0:
		FADE_OUT(0.5f);
		sEventPhase ++;
		break;
	case 1:
		if (IS_FADE() == true) return false;

		HIDE_BATTLE_ICON();
		sModelXfile[eMD_LOCKON].Scale() = ZERO_VEC;

		sPlayer.Scale() = ZERO_VEC;
		sPlayer.DisableShadow();

		// 必要なリソースを読み込む
		EventManager::GetInstance()->LoadCameraFile("data/Camera/040_10_Camera.vmd");
		EventManager::GetInstance()->EntryModelNum(model_num);
		EventManager::GetInstance()->SetModelParameter(0, Name_Lino,	eMT_IDLE_NOWEAPON,		&sModelMap, true);
		EventManager::GetInstance()->SetModelParameter(1, NAME_Navi,	0,						NULL,		true);

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(0);
		p_model->SetDrawFunc((ModelBase::DRAW_FUNC)DrawLinoFunc);		/* リノ専用の描画関数を設定 */
		p_model->EnableToonShader();

		p_model = (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1);
		eventGeneral_NaviLuminusStart(p_model);

		sEventCsv.LoadFile("data/CSV/event40_10.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		startCsvAction2nd();

		FADE_IN(0.5f);
		sEventPhase ++;
		break;
		
	case 2:
		/* CSVファイルのアクションを更新 */
		is_end = updateCsvAction2nd(&sEventCsv);

		eventGeneral_NaviLuminusUpdate( (ModelPmd *)EventManager::GetInstance()->GetModelPmd(1) );

		if (is_end) {
			FADE_OUT(0.5f);
			sEventPhase ++;
		}
		break;
		
	case 3:
		if (IS_FADE() == true) return false;

		eventGeneral_NaviLuminusEnd();

		EventManager::GetInstance()->EndEvent();
		sEventCsv.DeleteFile();
		sPlayer.Scale() = CHARA_DEF_SCALE;
		sPlayer.EnableShadow();
		VISIBLE_BATTLE_ICON();
		SET_EVENT_END_FLAG();

		return true;
		
	default:
		break;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event48_AppearTower(void)
{
	switch (sEventPhase) {
	case 0:
		SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);

		FADE_OUT(0.5f);
		HIDE_BATTLE_ICON();
		sEventPhase ++;

		break;
	case 1:
		if (IS_FADE() == true) return false;

		EventManager::GetInstance()->LoadCameraFile("data/Camera/dummy.vmd");
		EventManager::GetInstance()->EntryModelNum(0);

		// キャラ周辺のカメラの設定
		CameraManager::GetInstance()->SetPatternArroundPlayer(180.0f,		// camDeg
															  -400.0f,		// camLen
															  90.0f,		// camPosY
															  60.0f);		// camTarY

		sEventCsv.LoadFile("data/CSV/event16_01.csv");
		EventManager::GetInstance()->StartEvent(&sEventCsv);

		/* アイテムを全て表示にさせる(StartEvent にて非表示にされる為) */
		visibleObjectAll(true);

		FADE_IN(0.5f);

		sEventPhase ++;
		break;
		
	case 2:
		MSG_TYPE_MESSAGE();
		SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_IN);
		MSG_FADE_IN(0.3f);
		sEventPhase ++;
		break;

	case 3:
		if (MSG_IS_FADE() == false) {
			RENDER_MESSAGE("ウィル", "リノ、わしじゃウィルじゃ。", "お前さんの心に直接話しかけておる。", "そのバベノレの塔はとても危険じゃ！", "");
			if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		}
		break;
	case 4:
		RENDER_MESSAGE("セルシアナ", "リノちゃん聞いて！", "その塔の周りには強い『重力』が働いているわ！", "",  "");
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;

	case 5:
		RENDER_MESSAGE("セルシアナ", "リノちゃんの自慢のジャンプも効果半減よ。", "塔の外から頂上を目指すのは諦めて、", "塔の内部から頂上を目指して！",  "");
		if (CHECK_TRIG_ANY_JOY()) sEventPhase ++;
		break;

	case 6:
		RENDER_MESSAGE("ウィル", "まずは塔に近づいて入口を探すのじゃ！", "用心せい！", "",  "");
		if (CHECK_TRIG_ANY_JOY()) {
			SoundManager::GetSndComponent()->PlaySound(eSE_MESSAGE_OUT);
			MSG_FADE_OUT(0.3f);
			sEventPhase ++;
		}
		break;
	case 7:
		if (MSG_IS_FADE() == false) {
			VISIBLE_BATTLE_ICON();
			EventManager::GetInstance()->EndEvent();
			sEventCsv.DeleteFile();

			SET_EVENT_END_FLAG();

			SoundManager::GetInstance()->PlayBGM(spBGInfo->mMusicName);
			return true;
		}
	};
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event48_GetSongHandling(void)
{
	return eventGeneral_GetSong(eSONG_TYPE_HANDLING);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event49_MoveBabel(void)
{
	ActionTaskManager::tTaskInfo param;
	param.mTaskType = ActionTaskManager::eTASK_MOVE;

	if (IS_END_EVT(eEVENT_40, eEVENT_40_WAKEUP_BABEL) == false) {
		param.mTaskValueA = 40;
		param.mTaskValueB = 9;
	}
	else {
		param.mTaskValueA = 48;
		param.mTaskValueB = 0;
	}
	ActionTaskManager::GetInstance()->NotifyAction(&param);

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event52_ClearBarrier01(void)
{
	if (eventGeneral_ClearBarrier(StageFunc052::eBG_TYPE_GIMMICK_BARRIER_1, 0, 30) == true) {
		SET_EVENT_END_FLAG();
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event52_TrapBattle01(void)
{
	if (sEventCount == 0) {
		CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_BATTLE_WALL);
		sModelMap.VisibleBattleBox();
		destroyAllEnemy(); // 敵を全滅させる
	}

	if (sEventCount == 10) {

		VECTOR pos;	float rot;
		getPlayerPosRotFromBGModel(&pos, &rot, 11);
		for (int iI = 0; iI < 5; iI ++) {
			float setrot = ((DX_PI * 2.0f * (float)iI) / 5.0f);
			MATRIX mtx = MGetRotY(setrot);
			VECTOR diff = VGet(1000.0f, 0.0f, 0.0f);
			diff = VTransform(diff, mtx);
			createEnemyWork2(iI, eENEMY_KIND_HELICOPTER, &VAdd(pos, diff));
		}
	}

	if (sEventCount >= 20) {
		// リカバリー、またはゲームオーバー時にクリアされるフラグを設定
		sEventRecoveryFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_52, eEVENT_52_TRAP_BATTLE_1);
		SET_EVENT_END_FLAG();
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event52_WakeupLift01(void)
{
	if (eventGeneral_WakeupGimmick() ) {
		SET_EVENT_END_FLAG();
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool 
event52_ClearBarrier02A(void)
{
	if (IS_END_EVT(eEVENT_52, eEVENT_52_CLEAR_BARRIER_2B) ) {
		if (eventGeneral_ClearBarrier(StageFunc052::eBG_TYPE_GIMMICK_BARRIER_2, 0, 150) == true) {
			SET_EVENT_END_FLAG();
			return true;
		}
	}
	else {
		SET_EVENT_END_FLAG();
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event52_ClearBarrier02B(void)
{
	if (IS_END_EVT(eEVENT_52, eEVENT_52_CLEAR_BARRIER_2A) ) {
		if (eventGeneral_ClearBarrier(StageFunc052::eBG_TYPE_GIMMICK_BARRIER_2, 0, 150) == true) {
			SET_EVENT_END_FLAG();
			return true;
		}
	}
	else {
		SET_EVENT_END_FLAG();
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event52_ClearBarrier03A(void)
{
	if (eventGeneral_ClearBarrier(StageFunc052::eBG_TYPE_GIMMICK_BARRIER_3A, 0, 230) == true) {
		SET_EVENT_END_FLAG();
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event52_ClearBarrier03B(void)
{
	if (eventGeneral_ClearBarrier(StageFunc052::eBG_TYPE_GIMMICK_BARRIER_3B, 0, 30) == true) {
		SET_EVENT_END_FLAG();
		return true;
	}
	return false;
}


//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event52_ClearLaser_3F(void)
{
	VECTOR pos;
	int mesh_index;
	float alpha;
	
	int frameIndex = StageFunc052::eBG_TYPE_GIMMICK_LASER_3F;

	int check_event_phase = (sEventPhase);

	// カメラを切り替える
	mesh_index = MV1GetFrameMesh(sModelMap.GetModelHandle(), 
								 frameIndex, 
								 0);
	sModelMap.GetMeshMdlPosition(&pos, mesh_index);
	CameraManager::GetInstance()->Target() = pos;

	switch (check_event_phase) {
	case 0:
		SoundManager::GetSndComponent()->PlaySound(eSE_SOLVE_PUZZLE);
		sEventPhase ++;
		break;
	case 1:

		sEventPhase ++;
		break;
	case 2:
		alpha = sModelMap.GetAlpha(frameIndex);
		alpha += -0.01f;
		if (alpha < 0.0f) alpha = 0.0f;
		sModelMap.SetAlpha(frameIndex, alpha);
		if (alpha <= 0.0f) {
			sEventPhase ++;
		}
		break;

	case 3:
		if (sEventCount > 60.0f) {
			CollisionManager::GetInstance()->RemoveWallCollision(sModelMap.GetModelHandle(), frameIndex);
			CameraManager::GetInstance()->ChangeCameraState(eCAM_STATE_RESET);

			SET_EVENT_END_FLAG();

			return true;
		}
		break;

	default:
		break;
	};
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event52_ClearBarrier03C1(void)
{
	if (sEventPhase == 0) {
		// 該当のスイッチが全て押されているかのチェック
		GimmickBase* p_gimmick = NULL;
		for (int iI = 0; iI < GimmickBase::GetGimmickBaseNum(); iI ++) {
			p_gimmick = GimmickBase::GetGimmickBase(iI);
			if (p_gimmick->GetKind() == eGIMMICK_KIND_BABEL_SWITCH) {
				int event_value = (int)p_gimmick->GetGimmickExtParamValue(GimmickExtend_FootSwitch::eCSVLINE_TASK_VALUE);
				if (event_value == eEVENT_52_CLEAR_BARRIER_3C2) {
					if (p_gimmick->GetState() == GimmickExtend_FootSwitch::eSTATE_ON_MODEL_SWITCH) {
						
						sEventPhase ++;
					}
				}
			}	
		}

		if (sEventPhase == 0) {
			return true;
		}
	}
	else {
		if (eventGeneral_ClearBarrier(StageFunc052::eBG_TYPE_GIMMICK_BARRIER_3C, 1, 30) == true) {
			// SET_EVENT_END_FLAG();
			TotalInfo::GetInstance()->SetEventFlag(eEVENT_52, eEVENT_52_CLEAR_BARRIER_3C1);
			TotalInfo::GetInstance()->SetEventFlag(eEVENT_52, eEVENT_52_CLEAR_BARRIER_3C2);
			return true;
		}
	}

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event52_ClearBarrier03C2(void)
{
	if (sEventPhase == 0) {
		// 該当のスイッチが全て押されているかのチェック
		GimmickBase* p_gimmick = NULL;
		for (int iI = 0; iI < GimmickBase::GetGimmickBaseNum(); iI ++) {
			p_gimmick = GimmickBase::GetGimmickBase(iI);
			if (p_gimmick->GetKind() == eGIMMICK_KIND_BABEL_SWITCH) {
				int event_value = (int)p_gimmick->GetGimmickExtParamValue(GimmickExtend_FootSwitch::eCSVLINE_TASK_VALUE);
				if (event_value == eEVENT_52_CLEAR_BARRIER_3C1) {
					if (p_gimmick->GetState() == GimmickExtend_FootSwitch::eSTATE_ON_MODEL_SWITCH) {
						
						sEventPhase ++;
					}
				}
			}	
		}

		if (sEventPhase == 0) {
			return true;
		}
	}
	else {
		if (eventGeneral_ClearBarrier(StageFunc052::eBG_TYPE_GIMMICK_BARRIER_3C, 1, 30) == true) {

			TotalInfo::GetInstance()->SetEventFlag(eEVENT_52, eEVENT_52_CLEAR_BARRIER_3C1);
			TotalInfo::GetInstance()->SetEventFlag(eEVENT_52, eEVENT_52_CLEAR_BARRIER_3C2);
//			SET_EVENT_END_FLAG();
			return true;
		}
	}

	return false;
}


//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event52_TrapBattle03(void)
{
	if (sEventCount == 0) {
		CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_BATTLE_WALL);
		sModelMap.VisibleBattleBox();
		destroyAllEnemy(); // 敵を全滅させる
	}

	if (sEventCount == 10) {

		VECTOR pos;	float rot;
		getPlayerPosRotFromBGModel(&pos, &rot, 32);
		for (int iI = 0; iI < 5; iI ++) {
			float setrot = ((DX_PI * 2.0f * (float)iI) / 5.0f);
			MATRIX mtx = MGetRotY(setrot);
			VECTOR diff = VGet(1000.0f, 0.0f, 0.0f);
			diff = VTransform(diff, mtx);
			createEnemyWork2(iI, eENEMY_KIND_ROBOT_B, &VAdd(pos, diff));
		}
	}

	if (sEventCount >= 20) {

		// バトルフラグを立てる
		sEventBattleFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_52, eEVENT_52_CLEAR_BARRIER_3A);

		// リカバリー、またはゲームオーバー時にクリアされるフラグを設定
		sEventRecoveryFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_52, eEVENT_52_TRAP_BATTLE_3);
		SET_EVENT_END_FLAG();
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event52_TrapBattle_4F_A(void)
{
	if (sEventCount == 0) {
		CollisionManager::GetInstance()->EntryWallCollision(sModelMap.GetModelHandle(), eBG_TYPE_BATTLE_WALL);
		sModelMap.VisibleBattleBox();
		destroyAllEnemy(); // 敵を全滅させる
	}

	if (sEventCount == 10) {
		VECTOR pos;	float rot;
		getPlayerPosRotFromBGModel(&pos, &rot, 42);
		for (int iI = 0; iI < 5; iI ++) {
			float setrot = ((DX_PI * 2.0f * (float)iI) / 5.0f);
			MATRIX mtx = MGetRotY(setrot);
			VECTOR diff = VGet(750.0f, 0.0f, 0.0f);
			diff = VTransform(diff, mtx);
			createEnemyWork2(iI, eENEMY_KIND_ROBOT_A, &VAdd(pos, diff));
		}
	}

	if (sEventCount >= 20) {

		// バトルフラグを立てる
		sEventBattleFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_52, eEVENT_52_CLEAR_BARRIER_4F_A);
		
		// リカバリー、またはゲームオーバー時にクリアされるフラグを設定
		sEventRecoveryFlag = TASKVALUE_TO_BATTLEFLAG(eEVENT_52, eEVENT_52_TRAP_BATTLE_4F_A);
		SET_EVENT_END_FLAG();
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event52_ClearBarrier4F_A(void)
{
	if (eventGeneral_ClearBarrier(StageFunc052::eBG_TYPE_GIMMICK_BARRIER_4A, 0, 210) == true) {
		SET_EVENT_END_FLAG();
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event52_WakeupLift_4F_A(void)
{
	if (eventGeneral_WakeupGimmick(225) ) {
		SET_EVENT_END_FLAG();
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event52_BootupMagicArmer(void)
{
	CharaGuardian* p_guardin = spStageFunc->GetGuardianPtr();

	// キャラ周辺のカメラの設定(ギミック乗っている時に追従するため、ずっと設定)
	CameraManager::GetInstance()->SetPatternArroundPlayer(0.0f,			// camDeg
														  -700.0f,		// camLen
														  300.0f,		// camPosY
														  270.0f);		// camTarY

	switch (sEventPhase) {
	case 0:
		{
			INT8 col_index = p_guardin->GetOnCollisionIndex();

			p_guardin->ChangeBody2Player();
		
			// ガーディアンがギミックに乗っている場合、
			// リノと座標変換した後、ギミック位置が戻されてしまうため、内部情報をリセットする
			GimmickBase* p_gimmick = NULL;
			p_gimmick = (GimmickBase *)GimmickBase::GetGimmickBaseFromEarthCollIndex(col_index);
			if (p_gimmick != NULL) {
				p_gimmick->ResetOnNodeInfo(p_guardin);
			}
	
			sEventPhase ++;
		}
		break;

	case 1:
		if (sEventCount > 30) {
			sPlayer.StartMotion(CharaGuardian::eMT_BOOTUP, false, 0.5f);
			sEventPhase ++;
		}
		break;

	case 2:
		if (sEventCount == 10) {
			VECTOR eff_pos = sPlayer.Position();
			eff_pos.y += (sPlayer.BodyHeight() * 0.5f);
			EffekseerManager::GetInstance()->Play(eEFK_GETKEYITEM, &eff_pos, 8.0f);
		}
		else if (sEventCount == 30) {
			int model_handle = sPlayer.GetModelHandle();
			MV1SetMaterialSphereMapBlendTypeAll(model_handle,
												DX_MATERIAL_BLENDTYPE_MODULATE); // 加算
			//			for (int iI = 0; iI < (sizeof(sAddingTable) / sizeof(BYTE)); iI ++) {
			for (int iI = 0; iI < (sizeof(kGUARDIAN_SPHERE_ADD_MATERIAL_TABLE) / sizeof(BYTE)); iI ++) {
				MV1SetMaterialSphereMapBlendType(model_handle,
												 //												 sAddingTable[iI],
												 kGUARDIAN_SPHERE_ADD_MATERIAL_TABLE[iI],
												 DX_MATERIAL_BLENDTYPE_ADDITIVE); // 乗算
			}
			SoundManager::GetSndComponent()->PlaySound(eSE_REFLECT);
		}
		else if (sEventCount == 70) {
			SoundManager::GetInstance()->SetVolumeBGM(kSOUND_VOLUME_DEFAULT);
			return true;
		}

		break;

	default:
		break;

	};

	return false;
}


//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool event52_ClearBarrier4F_B(void)
{
	if (eventGeneral_ClearBarrier(StageFunc052::eBG_TYPE_GIMMICK_BARRIER_4B, 0, 210) == true) {
		SET_EVENT_END_FLAG();
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event52_UpCubeBlock_4F_A(void)
{
	return eventGeneral_MoveGimmick(43, eGIMMICK_KIND_PRESS_WALL_HARD, (VECTOR&)VGet(0.0f, 0.5f, 0.0f) );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
event52_DnCubeBlock_4F_A(void)
{
	return eventGeneral_MoveGimmick(43, eGIMMICK_KIND_PRESS_WALL_HARD, (VECTOR&)VGet(0.0f, -0.5f, 0.0f) );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool event52_UpCubeBlock_4F_B(void)
{
	return eventGeneral_MoveGimmick(44, eGIMMICK_KIND_PRESS_WALL_HARD, (VECTOR&)VGet(0.0f, 0.45f, 0.0f) );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool event52_DnCubeBlock_4F_B(void)
{
	return eventGeneral_MoveGimmick(44, eGIMMICK_KIND_PRESS_WALL_HARD, (VECTOR&)VGet(0.0f, -0.45f, 0.0f) );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool event52_UpCubeBlock_4F_C(void)
{
	//	return eventGeneral_MoveGimmick(45, eGIMMICK_KIND_PRESS_WALL_HARD, VGet(0.0f, -0.45f, 0.0f) );
	return eventGeneral_MoveGimmick(45, eGIMMICK_KIND_PRESS_WALL_HARD, (VECTOR&)VGet(0.0f, 0.0f, -0.45f) );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool event52_DnCubeBlock_4F_C(void)
{
	//	return eventGeneral_MoveGimmick(45, eGIMMICK_KIND_PRESS_WALL_HARD, VGet(0.0f, 0.45f, 0.0f) );
	return eventGeneral_MoveGimmick(45, eGIMMICK_KIND_PRESS_WALL_HARD, (VECTOR&)VGet(0.0f, 0.0f, 0.45f) );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool event52_UpCubeBlock_4F_D(void)
{
	return eventGeneral_MoveGimmick(46, eGIMMICK_KIND_PRESS_WALL_HARD, (VECTOR&)VGet(0.925f, 0.45f, 0.0f) );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool event52_DnCubeBlock_4F_D(void)
{
	return eventGeneral_MoveGimmick(46, eGIMMICK_KIND_PRESS_WALL_HARD, (VECTOR&)VGet(-0.925f, -0.45f, 0.0f) );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool event52_UpCubeBlock_4F_E(void)
{
	//	return eventGeneral_MoveGimmick(47, eGIMMICK_KIND_PRESS_WALL_HARD, VGet(0.925f, 0.45f, 0.0f) );
	return eventGeneral_MoveGimmick(47, eGIMMICK_KIND_PRESS_WALL_HARD, (VECTOR&)VGet(0.925f, 0.0f, 0.0f) );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool event52_DnCubeBlock_4F_E(void)
{
	//	return eventGeneral_MoveGimmick(47, eGIMMICK_KIND_PRESS_WALL_HARD, VGet(-0.925f, -0.45f, 0.0f) );
	return eventGeneral_MoveGimmick(47, eGIMMICK_KIND_PRESS_WALL_HARD, (VECTOR&)VGet(-0.925f, -0.0f, 0.0f) );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool event52_ClearBarrier4F_C(void)
{
	if (eventGeneral_ClearBarrier(StageFunc052::eBG_TYPE_GIMMICK_BARRIER_4C, 0, 300) == true) {
		SET_EVENT_END_FLAG();
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int	getEventFunctionEffectHandle(void)
{
	return sEventFuncEffectIndex;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void DrawFontFunc_BattleUsiderManCounter(void)
{
	StageFunc040* p_stage = (StageFunc040 *)spStageFunc;
	char str_draw_font[24];
	int color = GetColor(0, 0, 0);

	sprintf_s(str_draw_font, sizeof(str_draw_font), "のこり時間 : %02d秒", (p_stage->mTornadeBattleCount / 60) );
	GraphicManager::GetInstance()->DrawFontString(560, 120, str_draw_font, color, 30);
}

/*=====================================*
 * スタッフロール関連
 *=====================================*/
//---------------------------------------------------
/**
 * 既にCSVファイルのインスタンス生成している場合は破棄し、
 * CSVファイルのインスタンス生成及びファイルロードを行う
 */
//---------------------------------------------------
void
startStaffRollEvent(BYTE csvFileIndex)
{
	// スタッフロール用のCSVファイル名
	static const char* sStrStaffrollCsvName[kSTAFFROLL_CSV_NUM] = {
		"staff_roll_A.csv",
		"staff_roll_B.csv",
		"staff_roll_C.csv",
	};

	StageFunc047::tStaffRollWork* p_staff = &((StageFunc047 *)spStageFunc)->mStaffroll;

	if (p_staff->mpCsv != NULL) {
		p_staff->mpCsv->DeleteFile();
		delete p_staff->mpCsv;
		p_staff->mpCsv = NULL;
	}

	p_staff->mpCsv = new CsvAccesser();
	APP_ASSERT((p_staff->mpCsv != NULL),
			   "Failed new CsvAccesser() \n");
			   
	char filepath[64];
	sprintf_s(filepath, sizeof(filepath), "data/CSV/%s", sStrStaffrollCsvName[csvFileIndex]);
	p_staff->mpCsv->LoadFile(filepath);

	p_staff->mCurCsvRowIndex	= 0;
	p_staff->mShiftTextPosY		= 480;	//<! 画面下から表示する
	p_staff->mIsEndReadCsv		= FALSE;

	if (csvFileIndex == 1) {
		p_staff->mTextPosX = 480;
		p_staff->mTextColor = GetColor(0, 0, 0);

		//		FADE_IN(0.75f);
		LayoutBase* p_fade_board = EventManager::GetInstance()->GetLayoutBase(0);
		p_fade_board->StartFade(0, 0.75f);
	}
	else {
		p_staff->mTextPosX = 320;
		p_staff->mTextColor = GetColor(255, 255, 255);

		FADE_OUT(0.75f);		// 本物のフェード板も一緒にフェードアウト
		LayoutBase* p_fade_board = EventManager::GetInstance()->GetLayoutBase(0);
		p_fade_board->StartFade(255, 0.75f);

	}
}

//---------------------------------------------------
/**
 * CSVファイルからのスタッフロール表示処理の終了チェック
 */
//---------------------------------------------------
BOOL
isEndStaffRollEvent(void)
{
	StageFunc047::tStaffRollWork* p_staff = &((StageFunc047 *)spStageFunc)->mStaffroll;

	if (p_staff->mIsEndReadCsv) {
		return TRUE;
	}
	return FALSE;
}

//---------------------------------------------------
/**
 * CSVファイルからのスタッフロール更新処理
 */
//---------------------------------------------------
void
updateStaffRollEvent(void)
{
	StageFunc047::tStaffRollWork* p_staff = &((StageFunc047 *)spStageFunc)->mStaffroll;
	
	// 行数の補正などを行う処理をする
	p_staff->mShiftTextPosY -= 1;
	if (p_staff->mShiftTextPosY <= -kSTAFFROLL_TEXT_LINE_HEIGHT) {
		p_staff->mShiftTextPosY = 0;

		p_staff->mCurCsvRowIndex ++;
	}
}

//---------------------------------------------------
/**
 * DrawFontFunc で呼ばれるスタッフロールのテキスト表示ハンドラ
 */
//---------------------------------------------------
void DrawFontFunc_StaffRollString(void)
{
	// 1度に表示するスタッフロールの行数
	static const BYTE kSTAFFROLL_DISP_LINE_NUM = 16;

	StageFunc047::tStaffRollWork* p_staff = &((StageFunc047 *)spStageFunc)->mStaffroll;

	APP_ASSERT((p_staff->mpCsv != NULL),
			   "p_staff->mpCsv is NULL \n");

	GraphicManager* const p_graphic = GraphicManager::GetInstance();

	// CSVの最大行数
	int csv_row_max = p_staff->mpCsv->GetRowNum();
	
	// テキスト描画情報
	int   text_pos_x = p_staff->mTextPosX;
	int   text_pos_y = 0;
	char* text_str	 = NULL;
	int   text_color = p_staff->mTextColor;
	
	int read_csv_row_index = 0;

	// 画面に表示できる行数分のスタッフロールのテキストを描画する
	for (int iI = 0; iI < kSTAFFROLL_DISP_LINE_NUM; iI ++) {
		// CSVから読み込む行数を取得
		read_csv_row_index = p_staff->mCurCsvRowIndex + iI;
		
		// CSVの最大行数を超えてる場合、フラグを立てて処理を終了
		if (csv_row_max <= read_csv_row_index) {
			p_staff->mIsEndReadCsv = TRUE;
			break;
		}
		// read_csv_row_index行のテキストを指定の位置に描画する
		else {
			text_str = p_staff->mpCsv->GetString(read_csv_row_index, 0);
			
			text_pos_y = iI * kSTAFFROLL_TEXT_LINE_HEIGHT + p_staff->mShiftTextPosY;
			p_graphic->DrawFontStringCenter(text_pos_x,
											text_pos_y,
											text_str,
											text_color,
											30);
		}
	};
}

 /**** end of file ****/
