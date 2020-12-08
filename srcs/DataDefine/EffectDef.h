/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * エフェクトファイルの定義
 * @author SPATZ.
 * @data   2012/04/29 20:13:58
 */
//---------------------------------------------------
#ifndef __EFFECT_DEF_H__
#define __EFFECT_DEF_H__

//<!!!! ここはイベントでも使われる可能性があるので原則入れ替えないこと !!!!!
enum {
	// 0
	eEF_FLAME = 0,
	eEF_HIT,
	eEF_THUNDER,
	eEF_AURA,
	eEF_WIND,
	// 5
	eEF_DUST,
	eEF_MY_TAIL,
	eEF_BLACKDUST,
	eEF_WATERDUST,
	eEF_LIGHTNING_EX,
	// 10
	eEF_LIGHTNING,
	eEF_NAVI,
	eEF_ENERGY_BALL,
	eEF_PROTECT,
	eEF_WINDFIRE,
	// 15
	eEF_ENERGY_BALL_R,
	eEF_ENERGY_BALL_G,
	eEF_ENERGY_BALL_B,
	eEF_MARUI,
	eEF_FLAME_BOMB,
	// 20
	eEF_CHARGE,
	eEF_FOREST,
	eEF_ARROW,
	eEF_GREEN,
	//	eEF_BLUR,
	// 25
	eEF_REALFIRE,
	eEF_TORNADE,
	eEF_WHITE_TAIL,	
	eEF_SKY,

	// この２つは最後
	eEF_DRAG_DROP,
	//	eEF_BLADE,

	eEF_MAX,
};

enum {
	// 0
	eEFK_MAGIC_SQUARE = 0,
	eEFK_SHIELD,
	eEFK_MAGIC_BALL,
	eEFK_SPREAD,
	eEFK_DISAPPER,
	// 5
	eEFK_PROTECT_START,
	eEFK_HEAL,
	eEFK_BREAK_BARRIER,
	eEFK_EXPLOSION_BOMB,
	eEFK_BREAK_JAR,
	// 10
	eEFK_BOSS_DEATH,
	eEFK_SHOCK,			// 大きな衝撃
	eEFK_PIYOPIYO,		// ぴよぴよ
	eEFK_SHOCK_THIN,	// 薄い衝撃
	eEFK_BREAK_LOCK,	// 岩の破壊
	// 15
	eEFK_TELEKINESIS,	// 念動力
	eEFK_GETKEYITEM,	// キーアイテム入手
	eEFK_LASER,			// レーザー
	eEFK_CANNON_LASER_SHOOT,	// 特大レーザー
	eEFK_SONICK_WAVE,	// ソニックウェーブ
	// 20
	eEFK_WIND,			// 風切り
	eEFK_THRUST,		// 突き
	eEFK_PORTION,		// ポーション
	eEFK_CANNON_LASER_CHARGE,		// チャージ
	eEFK_AURA_FLAME,	// 炎
	// 25
	eEFK_HELL_FLAME,	// 炎
	eEFK_REAL_FLAME,	// 炎
	eEFK_STAGE_CLEAR,	// ステージクリア
	eEFK_LIGHTNING_STRIKE,		// 雷
	eEFK_SENPU_AREA,	// 竜巻
	// 30
	eEFK_MAGMA,			// マグマエフェクト
	eEFK_HIT_LIGHTNING,	// 雷ヒット
	eEFK_LIGHT_PILLAR,	// 光の柱

#ifdef _DEBUG
	eEFK_DRAG_DROP_SAMPLE, // ドラッグ＆ドラップ用テスト
#endif // _DEBUG

	eEFK_MAX,
};


#endif // __EFFECT_DEF_H__

 /**** end of file ****/
