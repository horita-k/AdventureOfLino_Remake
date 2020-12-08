/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * EnemyDef.cpp
 * @author kenichi-horita.
 * @data   2014/01/14 22:47:05	
 */
//---------------------------------------------------
#include "EnemyDef.h"
#include "CharaDef.h"

#include "AppLib/Resource/ResourceModel.h"

// 敵情報なし
#define ENEMY_NONE		((eEnemyKind)-1)

/*=====================================*
 * 敵パラメータテーブル
 *=====================================*/
static const tEnemyWork sEnemyWorkTbl[eENEMY_KIND_MAX] = {
	{"Killer",           12.0f,               90.0f,                 200.0f,        kCHARA_WALK_SPEED*2.0f,      4,       eOBJECT_KIND_MONEY_1,    40,      "キラーマシーン",      },
	{NAME_Fighter,       12.0f,               45.0f,                 150.0f,        kCHARA_WALK_SPEED,           3,       eOBJECT_KIND_MONEY_1,    100,     "ファイター",          },
	{NAME_Fresbelk,      12.0f,               90.0f,                 200.0f,        kCHARA_WALK_SPEED*2.0f,      92,      eOBJECT_KIND_HEART,      40,      "フレスベルク",        },
	{"Giginebura",       14.0f,               520.0f,                360.0f,        kCHARA_WALK_SPEED*2.0f,      50,      eOBJECT_KIND_HEART,      40,      "ギギネブラ",          },
	{"Ray",              12.0f,               90.0f,                 260.0f,        kCHARA_WALK_SPEED*2.0f,      30,      eOBJECT_KIND_HEART,      40,      "レイ",                },
	{"01/Dragon",        DRAGON_SCALE,        400.0f,                180.0f,        kCHARA_WALK_SPEED*2.0f,      70,      eOBJECT_KIND_HEART,      100,     "トリケラ",            },
	{"01/Gargoyle",      12.0f,               100.0f,                260.0f,        kCHARA_WALK_SPEED*2.0f,      7,       eOBJECT_KIND_HEART,      40,      "ガーゴイル",          },
	{"Anhel",            12.0f,               300.0f,                660.0f,        kCHARA_WALK_SPEED*2.0f,      50,      eOBJECT_KIND_HEART,      40,      "アンヘル",            },
	{NAME_Hatophon,      14.0f,               110.0f,                360.0f,        kCHARA_WALK_SPEED*2.0f,      36,      eOBJECT_KIND_HEART_CASE, 100,     "ハトフォン",          },
	{NAME_Hatophon,      14.0f,               110.0f,                360.0f,        kCHARA_WALK_SPEED*2.0f,      36,      eOBJECT_KIND_HEART,      60,      "ハトフォン",          },
	{"01/Dewri",         12.0f,               90.0f,                 200.0f,        kCHARA_WALK_SPEED*2.0f,      4,       eOBJECT_KIND_MONEY_1,    40,      "とり",                },
	{"01/Goblin",        GOBLIN_SCALE,        90.0f,                 200.0f,        kCHARA_WALK_SPEED*1.0f,      8,       eOBJECT_KIND_HEART,      40,      "ゴブリン",            },
	{"01/Bat",           12.0f,               90.0f,                 200.0f,        kCHARA_WALK_SPEED*2.5f,      2,       eOBJECT_KIND_MONEY_1,    40,      "こうもりだいふく",    },
	{"01/Minotaur",      12.0f,               90.0f,                 200.0f,        kCHARA_WALK_SPEED*2.0f,      4,       eOBJECT_KIND_HEART,      40,      "ミノタウロス",        },
	{"01/Bear",          12.0f,               90.0f,                 200.0f,        kCHARA_WALK_SPEED*2.0f,      4,       eOBJECT_KIND_MONEY_1,    40,      "クマ松",              },
	{"01/Malon",         12.0f,               90.0f,                 260.0f,        kCHARA_WALK_SPEED*1.5f,      55,      eOBJECT_KIND_HEART,      100,     "マロン",              },
	{"01/Knight",        12.0f,               90.0f,                 260.0f,        kCHARA_WALK_SPEED*1.0f,      8,       eOBJECT_KIND_HEART,      40,      "騎士",                },
	{"01/Dragon",        DRAGON_SCALE*5.0f,   400.0f*5.0f,           180.0f*5.0f,   kCHARA_WALK_SPEED*10.0f,     400,     eOBJECT_KIND_HEART,      100,     "巨大トリケラ",        },
	{"01/Goblin",        GOBLIN_SCALE*5.0f,   90.0f*5.0f,            200.0f*5.0f,   kCHARA_WALK_SPEED*5.0f,      300,     eOBJECT_KIND_HEART,      100,     "巨大ゴブリン",        },
	{NAME_DarkWizard,    12.0f,               90.0f,                 260.0f,        kCHARA_WALK_SPEED,           38,      eOBJECT_KIND_HEART_CASE, 0,       "闇の司祭",            },
	{"01/Magician",      12.0f,               90.0f,                 320.0f,        kCHARA_WALK_SPEED*1.0f,      4,       eOBJECT_KIND_MONEY_5,    40,      "魔法使い",            },
	{"01/Armer",         10.0f,               90.0f,                 260.0f,        kCHARA_WALK_SPEED*1.0f,      5,       eOBJECT_KIND_MONEY_20,   40,      "動く鎧",              },
	{NAME_Oak,           12.0f,               90.0f,                 160.0f,        kCHARA_WALK_SPEED*1.0f,      5,       eOBJECT_KIND_MONEY_10,   40,      "オーク",              },
	{"01/Woodman",       12.0f,               90.0f,                 200.0f,        kCHARA_WALK_SPEED*2.0f,      4,       eOBJECT_KIND_MONEY_10,   40,      "ウッドマン",          },
	{"01/Woodman",       10.0f*5.0f,          90.0f*5.0f,            200.0f*5.0f,   kCHARA_WALK_SPEED*2.0f,      20,      eOBJECT_KIND_MONEY_10,   40,      "ビッグウッドマン",    },
	{"01/Slime",         10.0f,               90.0f,                 50.0f,         kCHARA_WALK_SPEED*1.0f,      6,       eOBJECT_KIND_HEART,      40,      "モチスライム",        },
	{"01/Rabbit",        12.0f,               90.0f,                 200.0f,        kCHARA_WALK_SPEED*1.0f,      4,       eOBJECT_KIND_HEART,      40,      "うさぎ",              },
	{"01/RedDrag",       12.0f,               800.0f,                200.0f*5.0f,   kCHARA_WALK_SPEED*3.0f,      65,      eOBJECT_KIND_HEART_CASE, 100,     "赤竜",                },
	{"01/Minotaurus",    12.0f,               90.0f,                 200.0f,        kCHARA_WALK_SPEED*2.0f,      8,       eOBJECT_KIND_HEART,      40,      "ミノタウロス",        },
	{"01/Demon",         475.0f,              1800.0f,               12500.0f,      kCHARA_WALK_SPEED*2.0f,      10,      eOBJECT_KIND_HEART_CASE, 100,     "羊頭のデーモン",      },
	{"01/Scorpion",      92.0f,               68.0f,                 52.0f,         kCHARA_WALK_SPEED*2.0f,      6,       eOBJECT_KIND_MONEY_10,   60,      "モブサソリ",          },
	{"01/RobotA",        12.0f,               80.0f,                 200.0f,        kCHARA_WALK_SPEED*1.4f,      8,       eOBJECT_KIND_MONEY_10,   40,      "N-YANロボ",           },
	{"01/RobotB",        12.0f,               80.0f,                 200.0f,        kCHARA_WALK_SPEED*1.0f,      4,       eOBJECT_KIND_HEART,      80,      "N-YANロボBLUE",       },
	{"01/RobotC",        12.0f,               80.0f,                 200.0f,        kCHARA_WALK_SPEED*1.4f,      8,       eOBJECT_KIND_HEART,      40,      "-",                   },
	{"01/Teeger",        TEEGER_SCALE,        TEEGER_BODY_SIZE,      570.0f,        kCHARA_WALK_SPEED*2.0f,      68,      eOBJECT_KIND_HEART,      40,      "てぃ～が～ちゃん",    },
	{"01/Helicopter",    12.0f,               90.0f,                 200.0f,        kCHARA_WALK_SPEED*2.0f,      2,       eOBJECT_KIND_HEART,      0,       "偵察ロボ",            },
	{NAME_MekaDragon,    12.0f,               90.0f,                 200.0f,        kCHARA_WALK_SPEED*3.0f,      56,      eOBJECT_KIND_HEART_CASE, 100,     "アルニャート",        },
	{NAME_SA_14,         12.0f,               180.0f,                400.0f,        kCHARA_WALK_SPEED*1.0f,      20,      eOBJECT_KIND_MONEY_50,   40,      "SA-14",               },
	{"01/Teeger",        TEEGER_SCALE*0.35f,  TEEGER_BODY_SIZE*0.35f,570.0f*0.35f,  kCHARA_WALK_SPEED*1.0f,      8,       eOBJECT_KIND_MONEY_20,   40,      "ミニてぃ～が～",      },
	{NAME_MinoArmer,     12.0f,               220.0f,                300.0f,        kCHARA_WALK_SPEED*1.0f,      28,      eOBJECT_KIND_MONEY_100,  100,     "ミノタウロス・キング",},
	{NAME_Spider,        12.0f,               220.0f,                300.0f,        kCHARA_WALK_SPEED*1.0f,      28,      eOBJECT_KIND_MONEY_100,  100,     "スパイダー",          },
	{NAME_HongKong,      12.0f,               90.0f,                 200.0f,        kCHARA_WALK_SPEED*1.5f,      95,      eOBJECT_KIND_HEART,      100,     "香港映画P",           },
	{NAME_Loana,         12.0f,               90.0f,                 200.0f,        kCHARA_WALK_SPEED*1.5f,      66,      eOBJECT_KIND_HEART,      100,     "ロアナ",              },
	{NAME_Oak,           12.0f,               90.0f,                 160.0f,        kCHARA_WALK_SPEED*1.0f,      8,       eOBJECT_KIND_MONEY_10,   40,      "ダイナマイト・オーク",},
	{NAME_Usae,          12.0f,               80.0f,                 200.0f,        kCHARA_WALK_SPEED*1.5f,      56,      eOBJECT_KIND_HEART,      100,     "うさえもん",          },
	{NAME_Usaida,        12.0f,               80.0f,                 200.0f,        kCHARA_WALK_SPEED*1.5f,      22,      eOBJECT_KIND_HEART,      100,     "ウサイダーマッ",      },
	{NAME_Gimuri,        12.0f,               60.0f,                 150.0f,        kCHARA_WALK_SPEED*1.5f,      88,      eOBJECT_KIND_HEART,      100,     "ギムリ",              },
	{NAME_Excivy,        12.0f,               120.0f,                500.0f,        kCHARA_WALK_SPEED*2.0f,      102,     eOBJECT_KIND_HEART,      40,      "エクシヴィ",          },

};

static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_NONE[][kENEMY_ONCE_MAX] = {
	{ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE,}, 
};
static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_000[][kENEMY_ONCE_MAX] = {
	{ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE,}, 
};
static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_001[][kENEMY_ONCE_MAX] = {
	{ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE,}, 
};
static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_002[][kENEMY_ONCE_MAX] = {
	{ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE,}, 
};
static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_003[][kENEMY_ONCE_MAX] = {
	{ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE,}, 
};
static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_004[][kENEMY_ONCE_MAX] = {
	{ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE,}, 
};
static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_005[][kENEMY_ONCE_MAX] = {
	{ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE,}, 
};
static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_010[][kENEMY_ONCE_MAX] = {
	// 0
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_FIGHTER,eENEMY_KIND_BAT,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_GOBULIN,ENEMY_NONE,				ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_GOBULIN,eENEMY_KIND_BAT,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_FIGHTER,	eENEMY_KIND_FIGHTER,eENEMY_KIND_FIGHTER,	ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_DRAGON,	eENEMY_KIND_DRAGON,	eENEMY_KIND_DRAGON,		ENEMY_NONE, ENEMY_NONE,},
	// 5
	{eENEMY_KIND_FIGHTER,	eENEMY_KIND_FIGHTER,eENEMY_KIND_FIGHTER,	ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_DRAGON,	eENEMY_KIND_DRAGON,	eENEMY_KIND_DRAGON,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_FIGHTER,eENEMY_KIND_BAT,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_GOBULIN,ENEMY_NONE,			ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_GOBULIN,eENEMY_KIND_BAT,		ENEMY_NONE, ENEMY_NONE,},
	// 10
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_GOBULIN,eENEMY_KIND_BAT,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_FIGHTER,	eENEMY_KIND_FIGHTER,eENEMY_KIND_FIGHTER,	ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_DRAGON,	eENEMY_KIND_DRAGON,	eENEMY_KIND_DRAGON,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_FIGHTER,eENEMY_KIND_BAT,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_GOBULIN,ENEMY_NONE,				ENEMY_NONE, ENEMY_NONE,},
	// 15
	{eENEMY_KIND_FIGHTER,	eENEMY_KIND_FIGHTER,eENEMY_KIND_FIGHTER,	ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_DRAGON,	eENEMY_KIND_DRAGON,	eENEMY_KIND_DRAGON,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_FIGHTER,eENEMY_KIND_BAT,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_GOBULIN,ENEMY_NONE,			ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_GOBULIN,eENEMY_KIND_BAT,		ENEMY_NONE, ENEMY_NONE,},
	// 20
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_GOBULIN,eENEMY_KIND_BAT,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_FIGHTER,	eENEMY_KIND_FIGHTER,eENEMY_KIND_FIGHTER,	ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_DRAGON,	eENEMY_KIND_DRAGON,	eENEMY_KIND_DRAGON,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_FIGHTER,eENEMY_KIND_BAT,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_GOBULIN,ENEMY_NONE,			ENEMY_NONE, ENEMY_NONE,},
	// 25
	{eENEMY_KIND_FIGHTER,	eENEMY_KIND_FIGHTER,eENEMY_KIND_FIGHTER,	ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_DRAGON,	eENEMY_KIND_DRAGON,	eENEMY_KIND_DRAGON,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_FIGHTER,eENEMY_KIND_BAT,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_GOBULIN,ENEMY_NONE,			ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_GOBULIN,eENEMY_KIND_BAT,		ENEMY_NONE, ENEMY_NONE,},
	// 30
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_GOBULIN,eENEMY_KIND_BAT,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_FIGHTER,	eENEMY_KIND_FIGHTER,eENEMY_KIND_FIGHTER,	ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_DRAGON,	eENEMY_KIND_DRAGON,	eENEMY_KIND_DRAGON,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_FIGHTER,eENEMY_KIND_BAT,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_GOBULIN,ENEMY_NONE,			ENEMY_NONE, ENEMY_NONE,},
	// 35
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_GOBULIN,eENEMY_KIND_BAT,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_DRAGON_B,	ENEMY_NONE, ENEMY_NONE,				ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN_B,	ENEMY_NONE, ENEMY_NONE,				ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN_B,	eENEMY_KIND_GOBULIN, eENEMY_KIND_GOBULIN,	ENEMY_NONE, ENEMY_NONE,},
};
static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_KARUN_A[][kENEMY_ONCE_MAX] = {
	{ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE,}, 
};
static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_KARUN_BOSS_A[][kENEMY_ONCE_MAX] = {
	{ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE,}, 
};

static const int sEnemyArrayENEMY_TBL_IDX_016[][kENEMY_ONCE_MAX] = {
	// 0
	{eENEMY_KIND_GOBULIN,		-1,			-1,		-1, -1,},
	{eENEMY_KIND_WOODMAN,		-1,			-1,		-1, -1,},
	{eENEMY_KIND_MINOTAURUS,	-1,			-1,		-1, -1,},
	{eENEMY_KIND_SLIME,			eENEMY_KIND_SLIME,		-1,		-1, -1,},
	{eENEMY_KIND_GOBULIN,		-1,			-1,		-1, -1,},
	// 5
	{eENEMY_KIND_SLIME,			-1,	-1,	-1, -1,},
	{eENEMY_KIND_WOODMAN,		eENEMY_KIND_WOODMAN, -1, -1, -1,},
	{eENEMY_KIND_BAT,			eENEMY_KIND_BAT,		eENEMY_KIND_FIGHTER, -1, -1,},
	{eENEMY_KIND_WOODMAN,		eENEMY_KIND_WOODMAN, eENEMY_KIND_WOODMAN, eENEMY_KIND_WOODMAN, -1,},
	{eENEMY_KIND_GOBULIN,		eENEMY_KIND_WOODMAN, -1, -1, -1,},
	// 10
	{eENEMY_KIND_GOBULIN,		-1,	-1,	-1, -1,},
	{eENEMY_KIND_SLIME,			eENEMY_KIND_WOODMAN, -1, -1, -1,},
	{eENEMY_KIND_FIGHTER,		eENEMY_KIND_BAT, -1, -1, -1,},
	{eENEMY_KIND_GOBULIN,		eENEMY_KIND_SLIME, -1, -1, -1,},
};

static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_024[][kENEMY_ONCE_MAX] = {
	{eENEMY_KIND_FIGHTER,	eENEMY_KIND_FIGHTER,ENEMY_NONE,			ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_FIGHTER,	eENEMY_KIND_BAT,	eENEMY_KIND_BAT,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_FIGHTER,eENEMY_KIND_BAT,		ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	ENEMY_NONE,		ENEMY_NONE,			ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_GOBULIN,	eENEMY_KIND_GOBULIN,eENEMY_KIND_BAT,		ENEMY_NONE, ENEMY_NONE,},
};
static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_ARENA[][kENEMY_ONCE_MAX] = {
	{eENEMY_KIND_KNIGHT, eENEMY_KIND_GOBULIN, eENEMY_KIND_MALON, eENEMY_KIND_HATOPHON_Z, eENEMY_KIND_DRAGON, },
	{eENEMY_KIND_FRESBELK, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE},
};

static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_026[][kENEMY_ONCE_MAX] = {
	{eENEMY_KIND_HATOPHON,	ENEMY_NONE,		ENEMY_NONE,			ENEMY_NONE, ENEMY_NONE,}, 
};
static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_028[][kENEMY_ONCE_MAX] = {
	{eENEMY_KIND_RED_DRAGON, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE, ENEMY_NONE,}, 
};

// リベルニア城
static const int sEnemyArrayENEMY_TBL_IDX_029[][kENEMY_ONCE_MAX] = {
	// 0
	{eENEMY_KIND_KNIGHT,	-1,	-1,		-1, -1,},
	{eENEMY_KIND_MAGICIAN,	eENEMY_KIND_MAGICIAN,	-1,		-1, -1,},
	{eENEMY_KIND_KNIGHT,	eENEMY_KIND_KNIGHT,		-1,		-1, -1,},
	{eENEMY_KIND_ARMER,		eENEMY_KIND_ARMER,		eENEMY_KIND_ARMER,	eENEMY_KIND_ARMER, -1,},
	{eENEMY_KIND_MAGICIAN,	eENEMY_KIND_MAGICIAN,	eENEMY_KIND_KNIGHT, eENEMY_KIND_KNIGHT, -1,},
	// 5
	{eENEMY_KIND_OAK,		-1, -1, -1, -1,},
	{eENEMY_KIND_ARMER,		eENEMY_KIND_ARMER,		eENEMY_KIND_ARMER,	-1, -1,},
	{eENEMY_KIND_KNIGHT,	eENEMY_KIND_OAK,		-1, -1, -1,},
	{eENEMY_KIND_KNIGHT,	eENEMY_KIND_KNIGHT,		-1,-1, -1,},
	{eENEMY_KIND_WIZARD,	-1,	-1,	-1, -1,},
	// 10
	{eENEMY_KIND_ARMER,		eENEMY_KIND_ARMER,		eENEMY_KIND_KNIGHT, -1, -1,},
};

static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_036[][kENEMY_ONCE_MAX] = {
	{eENEMY_KIND_MINOTAURUS, eENEMY_KIND_MINOTAURUS, ENEMY_NONE,		ENEMY_NONE,			ENEMY_NONE,}, 
	{eENEMY_KIND_WOODMAN,	eENEMY_KIND_SLIME,		eENEMY_KIND_SLIME,	ENEMY_NONE,			ENEMY_NONE,}, 
	{eENEMY_KIND_BAT,		eENEMY_KIND_BAT,		eENEMY_KIND_GOBULIN,	ENEMY_NONE,			ENEMY_NONE,}, 
	{eENEMY_KIND_KNIGHT,	eENEMY_KIND_MAGICIAN,	eENEMY_KIND_MAGICIAN,	ENEMY_NONE,			ENEMY_NONE,}, 
};

static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_039[][kENEMY_ONCE_MAX] = {
	{eENEMY_KIND_MINOTAURUS, eENEMY_KIND_GOBULIN,	ENEMY_NONE,		ENEMY_NONE,			ENEMY_NONE,}, 
	{eENEMY_KIND_WOODMAN,	eENEMY_KIND_WOODMAN,	eENEMY_KIND_WOODMAN,	ENEMY_NONE,			ENEMY_NONE,}, 
	{eENEMY_KIND_MAGICIAN,	eENEMY_KIND_MAGICIAN,	eENEMY_KIND_SLIME,	ENEMY_NONE,			ENEMY_NONE,}, 
};

static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_040[][kENEMY_ONCE_MAX] = {
	{eENEMY_KIND_TEEGER,	ENEMY_NONE,				ENEMY_NONE,			ENEMY_NONE,			ENEMY_NONE,}, 
	{eENEMY_KIND_ROBOT_B,	ENEMY_NONE,				ENEMY_NONE,			ENEMY_NONE,			ENEMY_NONE,}, 
	{eENEMY_KIND_ROBOT_A,	ENEMY_NONE,				ENEMY_NONE,			ENEMY_NONE,			ENEMY_NONE,}, 
	{eENEMY_KIND_SCORPION,	ENEMY_NONE,				ENEMY_NONE,			ENEMY_NONE,			ENEMY_NONE,}, 
	// 塔の上
	{eENEMY_KIND_SA_14,		ENEMY_NONE,				ENEMY_NONE,			ENEMY_NONE,			ENEMY_NONE,}, 
	{eENEMY_KIND_ROBOT_A,	eENEMY_KIND_ROBOT_A,	ENEMY_NONE,			ENEMY_NONE,			ENEMY_NONE,}, 
	{eENEMY_KIND_HELICOPTER,eENEMY_KIND_ROBOT_A,	eENEMY_KIND_HELICOPTER, 	ENEMY_NONE,			ENEMY_NONE,}, 
	{eENEMY_KIND_HELICOPTER,eENEMY_KIND_HELICOPTER,	ENEMY_NONE,			ENEMY_NONE,			ENEMY_NONE,}, 
};

static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_041[][kENEMY_ONCE_MAX] = {
	{eENEMY_KIND_HELICOPTER,	eENEMY_KIND_HELICOPTER,	eENEMY_KIND_HELICOPTER,	eENEMY_KIND_HELICOPTER,			ENEMY_NONE,}, 
	{eENEMY_KIND_SA_14,			eENEMY_KIND_ROBOT_A,	eENEMY_KIND_ROBOT_A,	ENEMY_NONE,			ENEMY_NONE,}, 
	{eENEMY_KIND_TEEGER_M,	eENEMY_KIND_TEEGER_M,	ENEMY_NONE,				ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_SA_14,		eENEMY_KIND_TEEGER_M,		eENEMY_KIND_ROBOT_B,				ENEMY_NONE,			ENEMY_NONE,}, 
	{eENEMY_KIND_SA_14,		eENEMY_KIND_SA_14,			eENEMY_KIND_SA_14,		eENEMY_KIND_SA_14,	eENEMY_KIND_SA_14,},
	

	{eENEMY_KIND_ROBOT_A,	eENEMY_KIND_ROBOT_B,	eENEMY_KIND_ROBOT_B,	ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_SA_14,		eENEMY_KIND_SA_14,		ENEMY_NONE,				ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_TEEGER_M,	eENEMY_KIND_TEEGER_M,	ENEMY_NONE,				ENEMY_NONE, ENEMY_NONE,},
};

static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_042[][kENEMY_ONCE_MAX] = {
	{eENEMY_KIND_ROBOT_A,	eENEMY_KIND_ROBOT_B,	eENEMY_KIND_ROBOT_B,	ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_SA_14,		eENEMY_KIND_SA_14,		ENEMY_NONE,				ENEMY_NONE, ENEMY_NONE,},
	{eENEMY_KIND_TEEGER_M,	eENEMY_KIND_TEEGER_M,	ENEMY_NONE,				ENEMY_NONE, ENEMY_NONE,},
};

static const eEnemyKind sEnemyArrayENEMY_TBL_IDX_046[][kENEMY_ONCE_MAX] = {
	{eENEMY_KIND_TEEGER_M,	eENEMY_KIND_TEEGER_M,	ENEMY_NONE,				ENEMY_NONE, ENEMY_NONE,},
};

static const int sEnemyArrayENEMY_TBL_IDX_TEST[][kENEMY_ONCE_MAX] = {
	{eENEMY_KIND_FRESBELK, -1,-1,-1,-1,},
	{eENEMY_KIND_USAEMON, eENEMY_KIND_USIDERMAN,-1,-1,-1,},
	{eENEMY_KIND_EXCIVY, -1,-1,-1,-1,},
	{eENEMY_KIND_HONGKONG, -1,-1,-1,-1,},
	{eENEMY_KIND_HONGKONG, -1,-1,-1,-1,},
	{eENEMY_KIND_LOANA, -1,-1,-1,-1,},
	{eENEMY_KIND_FRESBELK, -1,-1,-1,-1,},
	{eENEMY_KIND_GIMURI, -1,-1,-1,-1,},
	{eENEMY_KIND_USAEMON, eENEMY_KIND_USIDERMAN,-1,-1,-1,},
	{-1, -1,-1,-1,-1,},
	{-1, -1,-1,-1,-1,},
};

static const tEnemyTableInfo sEnemyArrayTable[eENE_TBL_IDX_MAX] = {
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_NONE,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_NONE),},			// 	eENE_TBL_IDX_NONE
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_000,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_000),},			// 	eENE_TBL_IDX_000
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_001,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_001),},			// 	eENE_TBL_IDX_001
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_002,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_002),},			// 	eENE_TBL_IDX_002
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_003,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_003),},			// 	eENE_TBL_IDX_003
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_004,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_004),},			// 	eENE_TBL_IDX_004
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_005,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_005),},			// 	eENE_TBL_IDX_005
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_010,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_010),},			// 	eENE_TBL_IDX_010
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_KARUN_A,			ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_KARUN_A),},		// 	eENE_TBL_KARUN_A
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_KARUN_BOSS_A,		ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_KARUN_BOSS_A),},	// 	eENE_TBL_KARUN_BOSS_A
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_016,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_016),},			// 	eENE_TBL_IDX_016
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_024,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_024),},			// 	eENE_TBL_IDX_024
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_026,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_026),},			// 	eENE_TBL_IDX_026
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_ARENA,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_ARENA),},			// 	eENE_TBL_IDX_ARENA
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_028,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_028),},			// 	eENE_TBL_IDX_028
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_029,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_029),},			// 	eENE_TBL_IDX_029
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_036,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_036),},			// 	eENE_TBL_IDX_036
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_039,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_039),},			// 	eENE_TBL_IDX_039
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_040,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_040),},			// 	eENE_TBL_IDX_040
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_041,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_041),},			// 	eENE_TBL_IDX_041
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_042,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_042),},			// 	eENE_TBL_IDX_042
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_046,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_046),},			// 	eENE_TBL_IDX_046
	{(eEnemyKind *)sEnemyArrayENEMY_TBL_IDX_TEST,				ENEMY_ARRAY_NUM(sEnemyArrayENEMY_TBL_IDX_TEST),},			// 	eENE_TBLIDX_TEST
};

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
const tEnemyWork* GetEnemyWorkTable(int enemyKind)
{
	return &sEnemyWorkTbl[enemyKind]; 
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
const tEnemyTableInfo* GetEnemyTableInfo(int tableIndex)
{
	return &sEnemyArrayTable[tableIndex];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void LoadEnemyAttachmentModel(eEnemyKind kind)
{
	ResourceModel* p_res_mdl = ResourceModel::GetInstance();

	switch (kind) {
	case eENEMY_KIND_KNIGHT:
		p_res_mdl->LoadItemResource(RES_MDL_SWORD);
		break;

	case eENEMY_KIND_MINOTAURUS:
		p_res_mdl->LoadItemResource(RES_MDL_AX);
		break;

	case eENEMY_KIND_MAGICIAN:
		p_res_mdl->LoadItemResource(RES_MDL_STAFF);
		break;

	case eENEMY_KIND_OAK:
		p_res_mdl->LoadItemResource(RES_MDL_SHIELD);
		break;

	default:
		break;
	};
}

 /**** end of file ****/
