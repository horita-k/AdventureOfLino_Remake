/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ボイスファイルの定義	
 * @author SPATZ.
 * @data   2012/02/05 16:47:50
 */
//---------------------------------------------------
#ifndef __VOICE_DEF_H__
#define __VOICE_DEF_H__

enum {
	eVC_ATTACK_1,
	eVC_JUMP,
	eVC_DAMAGE,
	eVC_DOWN,
	eVC_ATTACK_2,

	eVC_ATTACK_3,
	eVC_FALL_DOWN,
	eVC_ATTACK_4,

	eVC_MAX,
};                      

enum {
	eVC_NAVY_HEY = 0,
	eVC_NAVY_LISTN,

	eVC_NAVY_MAX,
};

enum {
	// 0
	eSE_LANDING,			// 着地音
	eSE_SWING_SWORD,		// 剣を振る音
	eSE_HIT_SWORD,			// 攻撃のヒット音
	eSE_ERASE_MODEL,		// 敵消滅
	eSE_HOOK_START,			// フックショット構え
	// 5
	eSE_HOOK_SHOTTING,		// フックショット発射
	eSE_REPELED_WALL,		// フックショットが壁に当たる音
	eSE_HOOK_END,			// フックショットが戻ってきた音
	eSE_FOOT_STEP_A,		// 足音A
	eSE_FOOT_STEP_B,		// 足音B
	// 10
	eSE_MESSAGE_IN,			// メッセージ開始
	eSE_MESSAGE_OUT,		// メッセージ終了
	eSE_TREASURE,			// お宝
	eSE_EXPLOSION,			// 爆発？
	eSE_ENCOUNT,			// エンカウント
	// 15
	eSE_CHARGE,				// 溜め
	eSE_ROLLING,			// 回転攻撃
	eSE_OPEN_WING,			// 翼を広げる
	eSE_SKY_AWAY,			// 空に飛んでいく
	eSE_BURST,				// 爆発
	// 20
	eSE_STAGE_CLEAR,		// ステージクリア
	eSE_AWAY,				// 回避音
	eSE_PUT_SWORD,			// 剣を収める音
	eSE_BOYOYON,			// ぼよよん
	eSE_PRESS_KEY,			// 何かキーを押したとき
	// 25
	eSE_SELECT_KEY,			// 何かキーを押したとき
	eSE_CHANGE_PAGE,		// ページをめくったとき
	eSE_CANCEL,				// キャンセル音
	eSE_GETITEM,			// ごまだれショートバージョン
	eSE_NG,
	// 30
	eSE_WARP,				// ワープ音
	eSE_COMICAL,			// コミカルな音（キツーネの足音とか)
	eSE_DECIDE_KEY,			// 決定のシステム音
	eSE_ITEM_APPEAR,		// アイテム出現
	eSE_FALLDOWN,			// 落下音
	// 35
	eSE_LANDING_WATER,		// 水の中での着地音
	eSE_FOOT_STEP_WATER_A,	// 水の中での足音A
	eSE_FOOT_STEP_WATER_B,	// 水の中での足音B
	eSE_ENEMY_APPEAR,		// 敵出現
	eSE_CLEAR_BATTLEBOX,	// バトルボックス削除
	// 40
	eSE_SOLVE_PUZZLE,		// 謎解き音
	eSE_BOOMERANG_RUN,		// ブーメラン風きり音
	eSE_MONSTER_VOICE,		// 怪物の雄たけび
	eSE_GET_MONEY,			// 金取得
	eSE_HEAVY_SWING,		// 重い風きり
	// 45
	eSE_DANGER,				// 危険
	eSE_MAGIC,				// 魔法
	eSE_THUNDER,			// 雷
	eSE_SHIELD,				// シールド
	eSE_DARK,				// 闇
	// 50
	eSE_SPREAD,				// 炸裂
	eSE_HORROR,				// ホラー
	eSE_FLAME_ARIA,			// 炎・詠唱
	eSE_FLAME_SHOOT,		// 炎・放出
	eSE_REFLECT,			// 魔法跳ね返し
	// 55
	eSE_BREAK_BARRIER,		// バリア破壊
	eSE_BREAK_JAR,			// 壺破壊
	eSE_HIT_HEAVY,			// 重い打撃音
	eSE_FOOT_STEP_GRASS_A,	// 草の上での足音A
	eSE_FOOT_STEP_GRASS_B,	// 草の上での足音B
	// 60
	eSE_LANDING_GRASS,		// 草の上での着地音
	eSE_FOOT_STEP_SAND_A,	// 砂の上での足音A
	eSE_FOOT_STEP_SAND_B,	// 砂の上での足音B
	eSE_LANDING_SAND,		// 砂の上での着地音
	eSE_FOOT_STEP_SOIL_A,	// 土の上での足音A
	// 65
	eSE_FOOT_STEP_SOIL_B,	// 土の上での足音B
	eSE_LANDING_SOIL,		// 土の上での着地音
	eSE_FALL_STONE,			// 石を落とした音
	eSE_FALL_FOREST_SND_A,	// 森の音Ａ
	eSE_FALL_FOREST_SND_B,	// 森の音Ｂ
	// 70
	eSE_HOLY,				// 聖なる感じの音
	eSE_GET_KEYITEM,		// キーアイテム取得時
	eSE_HIT_SWORD_2,		// 斬撃２
	eSE_PIYOPIYO,			// ぴよぴよ
	eSE_HIT_SWORD_3,		// 斬撃３
	// 75
	eSE_PULL_ARCHERY,		// 弓を引く
	eSE_SHOOT_ARCHERY,		// 弓を放つ
	eSE_HIT_ARROW,			// 弓がささる
	eSE_CAT,				// 猫
	eSE_TROLLEY,			// トロッコ
	// 80
	eSE_MOTOR_START,		// モーター・回転始め
	eSE_MOTOR_RUNNING,		// モーター・回転中
	eSE_WINGS,				// 羽ばたく
	eSE_TORNADE,			// 竜巻
	eSE_LASER,				// レーザー
	// 85
	eSE_CANNON_LASER_CHARGE,// 特大レーザー・溜め
	eSE_CANNON_LASER,		// 特大レーザー
	eSE_CANNON_LASER_DOING,	// 特大レーザー・放出中
	eSE_MACHINE_BOOTUP,		// 機械ブートアップ
	eSE_SNAPPING,			// ビンタ
	// 90
	eSE_BBA,				// うるさい BBA
	eSE_JET_AIRSHIP,		// 飛空船の音
	eSE_FOOT_STEP_ROBOT,	// ロボットの足音
	eSE_ELEVATOR,			// エレベーター音
	eSE_DRAGON_FLYING,		// 龍の飛行
	// 95
	eSE_BRAKE,				// ブレーキ音
	eSE_OCARINA_WILEDNESS,	// オカリナ音・草原の唄
	eSE_OCARINA_WING,		// オカリナ音・翼の唄
	eSE_OCARINA_HANDLING,	// オカリナ音・操りの唄
	eSE_OCARINA_SUN,		// オカリナ音・太陽の唄
	
	eSE_MAX,
};

/*=====================================*
 * BGM
 *=====================================*/
/**** キャラのテーマ ****/
// タイトル
#define BGM_TITLE			("WindOfBorghese.mp3")				// ボルゲーゼの風

// リノのテーマ
#define BGM_LINO_THEMA		("HillOfSolarClock.ogg")			// 日時計の丘
// エルダのテーマ
#define BGM_ELDA_THEMA		("TraditionalHill.ogg")				// 伝承の丘

/**** 街の曲 ****/
#define BGM_LIBELUNIA		("SmallTown.ogg")					// 小さな町
#define BGM_CASTLE			("Strategy_loop.ogg")				// PerituneMaterial_Strategy_loop
#define BGM_CASTLE_2		("Kishidan.ogg")					// 騎士団
#define BGM_COLLEGE			("DoorOfMagicIsOpenedNight.ogg")	// 魔法の扉は夜開く
#define BGM_HARBOR			("Resort.ogg")						// PerituneMaterial_Resort
#define BGM_VILLAGE			("SimpleVillage.ogg")				// 素朴な村

/**** フィールド ****/
#define BGM_FIELD			("OverWorld5.ogg")					// PerituneMaterial_OverWorld5_loop
#define BGM_RIDE_VEHICLE	("RideVehicle.ogg")					// 風に揺られて
#define BGM_DEPARTURE		("Departure.ogg")					// 出発（こおろぎ様）
#define BGM_JAPAN			("ohka_japan.ogg")					// ohka_japan.ogg

/**** ダンジョン ****/
#define BGM_DUNGEON_A		("02_Dungeon/Hishou.ogg")			// 覇道
#define BGM_DUNGEON_C		("RaceOfSand.ogg")					// 砂の民族
#define BGM_DUNGEON_3		("02_Dungeon/EdgeAndCorridor.ogg")	// 
#define BGM_DUNGEON_D		("GoesToWarFire.ogg")				// 戦火へ赴く
#define BGM_DUNGEON_E		("Deep_Woods_loop.ogg")				// PerituneMaterial_Deep_Woods_loop
#define BGM_DUNGEON_F		("02_Dungeon/FreezingWorld.ogg")	// 氷結の世界
#define BGM_DUNGEON_F_2		("02_Dungeon/loop_122.ogg")			// loop_122
#define BGM_DUNGEON_G		("The_Night_Industrial.ogg")		// The_Night_Industrial
#define BGM_DUNGEON_Z		("EternalSamsara.ogg")				// 永久の輪廻

/**** ボス戦 ****/
#define BGM_BOSS_A			("Hodou.ogg")						// 覇道
#define BGM_BOSS_B			("A-pe-kamuy.ogg")					// a-pe-kamuy_mixv2
#define BGM_BOSS_C			("DigitalWorld.ogg")				// DigitalWorld
#define BGM_BOSS_W			("FlameOfJudge.ogg")				// 裁きの炎
#define BGM_BOSS_D			("refrectable.mp3")					// refectable

/**** ストーリー ****/
#define BGM_FORCE_PLACE		("02_Dungeon/TownOfThe Mirage.ogg")	// 蜃気楼の街
#define BGM_CRYSTAL_PLACE	("02_Dungeon/YugenNoMai.ogg")		// 幽玄の舞
#define BGM_SKYSHIP			("Amakakeru.ogg")					// 天駆ける
#define BGM_SERIUS			("Keisou1.ogg")						// 喧騒Ⅰ
#define BGM_DUNGEON_B		("02_Dungeon/Sougenwoyuku.ogg")		// 草原をゆく
#define BGM_ENDING			("StaffRoll.ogg")					// 内包する大地

/**** その他 ****/
#define BGM_SKYLAND			("GreenWind.ogg")
#define BGM_SHIP			("UnderLake.ogg")
#define BGM_ZELDA			("Zelda.ogg")
#define BGM_TOWN			("City24.ogg")
#define BGM_DUMMY			("")

#endif // __VOICE_DEF_H__

 /**** end of file ****/
