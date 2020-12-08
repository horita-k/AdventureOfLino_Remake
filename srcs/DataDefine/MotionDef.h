/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * モーションファイルの定義	
 * @author SPATZ.
 * @data   2011/11/27 11:43:56
 */
//---------------------------------------------------
#ifndef __MOTION_DEF_H__
#define __MOTION_DEF_H__

#include "../AppLib/Graphic/ModelType.h"

/*=====================================*
 * リノのモーション
 *=====================================*/
enum {
	eMT_IDLE = 0,						// 0	アイドル
	eMT_WALK,							// 1	歩き
	eMT_RUN,							// 2	走り
	eMT_JUMP_START,						// 3	ジャンプ開始
	eMT_JUMP_DOING,						// 4	ジャンプ中
	eMT_JUMP_END,						// 5	ジャンプ終了
	eMT_DAMAGE,							// 6	ダメージ
	eMT_DOWN_START,						// 7	倒れこみ開始
	eMT_DOWN_DOING,						// 8	倒れこみ中
	eMT_DOWN_END,						// 9	倒れこみ終了
	eMT_STANDARD_MAX,
	eMT_ATTACK_1 = eMT_STANDARD_MAX,	// 10	攻撃１
	eMT_ATTACK_2,						// 11	攻撃２
	eMT_ATTACK_3,						// 12	攻撃３
	eMT_ATTACK_ROLL,					// 13	回転攻撃
	eMT_ATTACK_THRUST,					// 14	突き攻撃
	eMT_HOOK_SHOT,						// 15	フックショット
	eMT_HOOK_MOVE,						// 16	フックショット移動中
	eMT_TAKE_TREASURE,					// 17	宝箱取得
	eMT_AWAY_RIGHT,						// 18	回避右
	eMT_AWAY_LEFT,						// 19	回避左
	eMT_AWAY_BACK,						// 20	回避後ろ
	eMT_RUN_FAST,						// 21	高速走り
	eMT_GREETING,						// 22	挨拶(手を振る)
	eMT_IDLE_NOWEAPON,					// 23	武器未装備時アイドル
	eMT_ATTACK_4,						// 24	攻撃４
	eMT_PEACE,							// 25	ピース
	eMT_PUT_SWORD,						// 26	剣をしまう
	eMT_SLEEP,							// 27	寝ている
	eMT_BATOKIN_EVENT,					// 28	バトーキンイベント
	eMT_KITCHEN_EVENT,					// 29	リノの家のイベント
	eMT_KICK_TREASURE,					// 30	宝箱空け
	eMT_021_00_EVENT,					// 31	イベントモーション
	eMT_BOOMERANG,						// 32	ブーメラン
	eMT_AWAY_FRONT,						// 33	回避前
	eMT_ATTACK_JUMP,					// 34	ジャンプ攻撃
	eMT_026_00_EVENT,					// 35	イベントモーション
	eMT_KICK_TREASURE_SHORT,			// 36	宝箱空け(ショート版)
	eMT_IDLE_SLEEP,						// 37	眠たい
	eMT_WEEKLY,							// 38	瀕死
	eMT_DUMMY_39,						// 39	ダミーモーション
	eMT_SMILE_00,						// 40	リノの笑顔０
	eMT_SMILE_01,						// 41	リノの笑顔１
	eMT_SMILE_02,						// 42	リノの笑顔２
	eMT_SMILE_03,						// 43	リノの笑顔３
	eMT_SMILE_04,						// 44	リノの笑顔４
	eMT_SMILE_05,						// 45	リノの笑顔５
	eMT_SMILE_06,						// 46	リノの笑顔６
	eMT_SMILE_07,						// 47	リノの笑顔７
	eMT_SMILE_08,						// 48	リノの笑顔８
	eMT_SMILE_09,						// 49	リノの笑顔９
	eMT_029_00_EVENT,					// 50	イベントモーション
	eMT_029_01_EVENT,					// 51	イベントモーション
	eMT_029_04_EVENT,					// 52	イベントモーション
	eMT_SHIELD_GUARD,					// 53	盾ガード
	eMT_SMILE_QUESTION,					// 54	リノの笑顔　疑問
	eMT_014_01_EVENT,					// 55	イベントモーション
	eMT_RAISE_IDLE,						// 56	物を持ち上げた時のアイドル
	eMT_RAISE_WALK,						// 57	物を持ち上げた時の歩き
	eMT_RAISE_THROW,					// 58	物を投げる
	eMT_RAISE_PUT,						// 59	物を置く
	eMT_RAISE_SWORD,					// 60	剣を掲げる
	eMT_TAKEBACK_ARCHERY,				// 61	弓を引く
	eMT_CHARGE_ARCHERY,					// 62	弓の溜め時
	eMT_SHOOT_ARCHERY,					// 63	弓を放つ
	eMT_WORRY,							// 64	頭を抱える
	eMT_SMILE_YEAH,						// 65	リノの笑顔　イエーイ！

	eMT_ATTACK_SMASH_START,				// 66	跳び斬り開始
	eMT_ATTACK_SMASH_DOING,				// 67	跳び斬り中
	eMT_ATTACK_SMASH_END,				// 68	跳び斬り終了
	
	eMT_LOOK_ARROUND,					// 69	周りを見回す
	eMT_DOWN_HIP,						// 70	尻もちをつく
	eMT_CLING_LEFT_HAND,				// 71	左手でつかむ
	eMT_ATTACK_SLAPPING,				// 72	ビンタ
	eMT_DAMAGE_SLAPPING,				// 73	ビンタ
	eMT_SMILE_SHY,						// 74	リノの笑顔・照れ
	eMT_ATTACK_REAR,					// 75	背面斬り
	eMT_ATTACK_ROLLING_KICK,			// 76	回し蹴り
	eMT_RIDE_VEHICLE,					// 77	乗り物に乗ってる
	eMT_RIDE_VEHICLE_RUNNING,			// 78	乗り物に乗ってる(移動中)
	
	eMT_SHIELD_PIYOPIYO,				// 79	ピヨピヨ
	eMT_SHIELD_PIYOPIYO_2ND,			// 80	ピヨピヨ
	eMT_GUTS_POSE,						// 81	ガッツポーズ

	eMT_MAGIC_ARIA_START,				// 82	魔法詠唱開始
	eMT_MAGIC_ARIA_DOING,				// 83	魔法詠唱中
	eMT_OCARINA_STANCE,					// 84	オカリナ・構える
	eMT_SHIELD_GUARD_DOING,				// 85	盾ガードはじき
	eMT_COPY_ROD,						// 86   コピーロッド
	eMT_OCARINA_PLAY_START,				// 87   オカリナ・演奏開始
	eMT_OCARINA_PLAY_DOING,				// 88   オカリナ・演奏中
	eMT_SLEEP_ENDING,					// 89	寝ている、エンディング用

	eMT_MAX,							// 90
};

enum {
	eMT_NAVI_STAGE22_START = 3,
	eMT_NAVI_026_00_EVENT,
	eMT_NAVI_024_00_WELCOM_WATERWAY,
	eMT_NAVI_029_00_WELCOM_CASTLE,

	eMT_NAVI_MAX,
};

enum {
	eMT_WIZARD_029_00_WELCOM_CASTLE = 13,
	eMT_WIZARD_029_01_APPEAR_WIZARD = 14,
	eMT_WIZARD_029_04_COME_FRES = 15,
};

enum {
	eMT_CLOCK_029_00_WELCOM_CASTLE = 2,
};

// フレスベルク・戦闘用
enum {
	// 10
	eMT_FRESBELK_ATTACK_1 = eMT_STANDARD_MAX,
	eMT_FRESBELK_ATTACK_2,
	eMT_FRESBELK_ATTACK_3,
	eMT_FRESBELK_ATTACK_4,
	eMT_FRESBELK_ATTACK_5,

	eMT_FRESBELK_MAX,
};
// フレスベルク・イベント用
enum {
	// 10
	eMT_FRES_ATTACK_1 = eMT_STANDARD_MAX,
	/*
	eMT_FRES_ATTACK_2,
	eMT_FRES_ATTACK_3,
	eMT_FRES_ATTACK_ROLL,
	eMT_FRES_ATTACK_JUMP,
	*/
	// 15
	eMT_FRES_EVENT00,
	eMT_FRES_TALK,
	eMT_FRES_029_04_COME_FRES = 15,
	eMT_FRES_IDLE_NOWEAPON,
	eMT_FRES_CROSS_ARM,

	eMT_FRES_MAX,
};

enum {
	// 10
	eMT_GIGINEBURA_ATTACK_1 = eMT_STANDARD_MAX,
	eMT_GIGINEBURA_ATTACK_2,
	eMT_GIGINEBURA_ATTACK_3,
	eMT_GIGINEBURA_ATTACK_4,

	eMT_GIGINEBURA_MAX,
};

enum {
	// 10
	eMT_GOBURIN_ATTACK_1 = eMT_STANDARD_MAX,
	eMT_GOBURIN_ATTACK_2,
	eMT_GOBURIN_ATTACK_3,
	eMT_GOBURIN_ATTACK_4,

	eMT_GOBURIN_MAX,
};

enum {
	// 10
	eMT_STRATEGIST_ATTACK_1 = eMT_STANDARD_MAX,
	eMT_STRATEGIST_ATTACK_2,
	eMT_STRATEGIST_ATTACK_3,
	eMT_STRATEGIST_ATTACK_4,

	eMT_STRATEGIST_MAX,
};

enum {
	// 10
	eMT_KNIGHT_ATTACK_1 = eMT_STANDARD_MAX,
	eMT_KNIGHT_ATTACK_2,
	eMT_KNIGHT_ATTACK_3,
	eMT_KNIGHT_ATTACK_4,

	eMT_KNIGHT_MAX,
};

enum {
	// 10
	eMT_MONK_ATTACK_1 = eMT_STANDARD_MAX,
	eMT_MONK_ATTACK_2,
	eMT_MONK_ATTACK_3,
	eMT_MONK_AWAY,

	eMT_MONK_MAX,
};

enum {
	// 10
	eMT_DRAGON_ATTACK_1 = eMT_STANDARD_MAX,
	eMT_DRAGON_ATTACK_2,
	eMT_DRAGON_ATTACK_3,
	eMT_DRAGON_ATTACK_4,

	eMT_DRAGON_MAX,
};

enum {
	// 10
	eMT_ANHEL_ATTACK_1 = eMT_STANDARD_MAX,
	eMT_ANHEL_ATTACK_2,
	eMT_ANHEL_ATTACK_3,
	eMT_ANHEL_ATTACK_4,

	eMT_ANHEL_MAX,
};

enum {
	// 10
	eMT_HATOPHON_ATTACK_1 = eMT_STANDARD_MAX,
	eMT_HATOPHON_ATTACK_2,
	eMT_HATOPHON_ATTACK_3,
	eMT_HATOPHON_ATTACK_4,
	eMT_HATOPHON_026_000_EVENT,

	eMT_HATOPHON_MAX,
};

enum {
	eMT_TRESURE_IDLE = 0,
	eMT_TRESURE_EVENT,
	eMT_TRESURE_EVENT_SHORT,

	eMT_TRESURE_MAX, 
};

enum {
	eMT_LEA_IDLE = 0,
	eMT_LEA_XXX, 
	eMT_LEA_PRISON_TALK,

	eMT_LEA_MAX,
};

enum {
	eMT_ELDA_IDLE = 0,
	eMT_ELDA_TALK,
	eMT_025_00_EVENT_OLD,
	eMT_ELDA_CHAIRING,
};

enum {
	eMT_CELCIANA_IDLE = 0,
	eMT_CELCIANA_014_000_EVENT,

	eMT_CELCIANA_MAX,
};

// ティーガー専用攻撃モーション
enum {
	// 10
	eMT_TEEDER_ATTACK_1 = eMT_STANDARD_MAX,
	eMT_TEEDER_ATTACK_2,
	eMT_TEEDER_ATTACK_3,
	eMT_TEEDER_ATTACK_4,

	eMT_TEEDER_MAX,
};

enum {
	// 10
	eMT_REDDRAGON_ATTACK_1 = eMT_STANDARD_MAX,
	eMT_REDDRAGON_ATTACK_2,
	eMT_REDDRAGON_ATTACK_3,
	eMT_REDDRAGON_ATTACK_4,
	eMT_REDDRAGON_ATTACK_5,		// 崩れ落ちるモーション
	eMT_REDDRAGON_ATTACK_6,	
	eMT_REDDRAGON_DOWN_START,	// ダウンモーション

	eMT_REDDRAGON_MAX,
};

enum {
	// 10
	eMT_DEMON_ATTACK_1 = eMT_STANDARD_MAX,
	eMT_DEMON_ATTACK_2,
	eMT_DEMON_ATTACK_3,
	eMT_DEMON_ATTACK_4,
	eMT_DEMON_ATTACK_5,
	// 15
	eMT_DEMON_UP_ARM,
	eMT_DEMON_INDICATE_FINGER,

	eMT_DEMON_MAX,
};

enum {
	eMT_RAFIA_ATTACK_1 = eMT_STANDARD_MAX,
	eMT_RAFIA_HIP_DOWN,
	eMT_RAFIA_ATTACK_SLAPPING,
	eMT_RAFIA_DAMAGE_SLAPPING,

	eMT_RAFIA_MAX,
};

static const tMotionParam sMotionParam[eMT_MAX] = {
	{eMT_IDLE,				true,	0.5f,	NULL,},
	{eMT_WALK,				true,	0.6f,	NULL,},						
	{eMT_RUN,				true,	1.0f,	NULL,},						
	{eMT_JUMP_START,		false,	0.5f,	&sMotionParam[eMT_JUMP_DOING],},
	{eMT_JUMP_DOING,		true,	0.5f,	NULL,},						
	{eMT_JUMP_END,			false,	0.75f,	NULL,},
	{eMT_DAMAGE,			false,	0.75f,	&sMotionParam[eMT_IDLE],},
	{eMT_DOWN_START,		false,	0.75f,	NULL,},
	{eMT_DOWN_DOING,		false,	0.75f,	NULL,},
	{eMT_DOWN_END,			false,	0.75f,	NULL,},
	{eMT_ATTACK_1,			false,	0.6f,	NULL,},
	{eMT_ATTACK_2,			false,	0.5f,	NULL,},
	{eMT_ATTACK_3,			false,	0.65f,	NULL,},
	{eMT_ATTACK_ROLL,		false,	0.75f,	NULL,},
	{eMT_ATTACK_THRUST,		false,	0.75f,	NULL,},
	{eMT_HOOK_SHOT,			true,	0.75f,	NULL,},
	{eMT_HOOK_MOVE,			false,	1.00f,	NULL,},
	{eMT_TAKE_TREASURE,		false,	1.00f,	NULL,},
	{eMT_AWAY_RIGHT,		false,	0.75f,	NULL,},
	{eMT_AWAY_LEFT,			false,	0.75f,	NULL,},
	{eMT_AWAY_BACK,			false,	0.75f,	NULL,},
	{eMT_RUN_FAST,			true,	0.75f,	NULL,},
	{eMT_GREETING,			false,	0.75f,	&sMotionParam[eMT_IDLE],},
	{eMT_IDLE_NOWEAPON,		true,	0.5f,	NULL,},
	{eMT_ATTACK_4,			false,	0.55f,	NULL,},
	{eMT_PEACE,				true,	0.25f,	NULL,},
	{eMT_PUT_SWORD,			false,	0.375f,	&sMotionParam[eMT_PEACE],},
	{eMT_SLEEP,				false,	0.50f,	NULL,},
	{eMT_BATOKIN_EVENT,		false,	0.50f,	NULL,},
	{eMT_KITCHEN_EVENT,		false,	0.50f,	NULL,},
	{eMT_KICK_TREASURE,		false,	0.50f,	NULL,},
	{eMT_021_00_EVENT,		false,	0.50f,	NULL,},
	{eMT_BOOMERANG,			false,	0.50f,	NULL,},
	{eMT_AWAY_FRONT,		false,	3.00f,	NULL,},
	{eMT_ATTACK_JUMP,		false,	0.75f,	NULL,},

	// 35
	{eMT_026_00_EVENT,		false,	0.00f,	NULL,},	//	eMT_026_00_EVENT,
	{eMT_KICK_TREASURE,		false,	0.00f,	NULL,},	//	eMT_KICK_TREASURE_SHORT,
	{eMT_IDLE_SLEEP,		false,	0.00f,	NULL,},	//	eMT_IDLE_SLEEP,
	{eMT_WEEKLY,			true,	0.2f,	NULL,}, //	eMT_WEEKLY
	{eMT_DUMMY_39,			false,	0.00f,	NULL,},	//	eMT_DUMMY_39,
	// 40
	{eMT_SMILE_00,			false,	0.50f,	NULL,},	//	eMT_SMILE_00,
	{eMT_SMILE_01,			false,	0.10f,	NULL,},	//	eMT_SMILE_01,
	{eMT_SMILE_02,			false,	0.10f,	NULL,},	//	eMT_SMILE_02,
	{eMT_SMILE_03,			false,	0.10f,	NULL,},	//	eMT_SMILE_03,
	{eMT_SMILE_04,			false,	0.10f,	NULL,},	//	eMT_SMILE_04,
	{eMT_SMILE_05,			false,	0.10f,	NULL,},	//	eMT_SMILE_05,
	{eMT_SMILE_06,			false,	0.50f,	NULL,},	//	eMT_SMILE_06,
	{eMT_SMILE_07,			false,	0.50f,	NULL,},	//	eMT_SMILE_07,
	{eMT_SMILE_08,			false,	0.50f,	NULL,},	//	eMT_SMILE_08,
	{eMT_SMILE_09,			false,	0.50f,	NULL,},	//	eMT_SMILE_09,

	{eMT_029_00_EVENT,		false,	0.50f,	NULL,},	//	eMT_029_00_EVENT,					// 50	イベントモーション
	{eMT_029_01_EVENT,		false,	0.50f,	NULL,},	//	eMT_029_01_EVENT,					// 51	イベントモーション
	{eMT_029_04_EVENT,		false,	0.50f,	NULL,},	//	eMT_029_04_EVENT,					// 52	イベントモーション
	{eMT_SHIELD_GUARD,		false,	0.50f,	NULL,},	//	eMT_SHIELD_GUARD,					// 53	盾ガード
	{eMT_SMILE_QUESTION,	false,	0.50f,	NULL,},	//	eMT_SMILE_QUESTION,					// 54	リノの笑顔　疑問
	{eMT_014_01_EVENT,		false,	0.50f,	NULL,},	//	eMT_014_01_EVENT,					// 55	イベントモーション
	{eMT_RAISE_IDLE,		true,	0.50f,	NULL,},	//	eMT_RAISE_IDLE,						// 56	物を持ち上げた時のアイドル
	{eMT_RAISE_WALK,		true,	0.50f,	NULL,},	//	eMT_RAISE_WALK,						// 57	物を持ち上げた時の歩き
	{eMT_RAISE_THROW,		false,	1.00f,	NULL,},	//	eMT_RAISE_THROW,					// 58	物を投げる
	{eMT_RAISE_PUT,			true,	0.50f,	NULL,},	//	eMT_RAISE_PUT,						// 59	物を置く

};

#endif // __MOTION_DEF_H__

 /**** end of file ****/
