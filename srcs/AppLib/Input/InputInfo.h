/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 入力情報の定義ヘッダー
 * @author SPATZ.
 * @data   2011/11/21 19:47:51
 */
//---------------------------------------------------
#ifndef __INPUT_INFO_H__
#define __INPUT_INFO_H__

#include "DxLib.h"

//キー定義
enum {
	eKEY_BACK,				//バックスペースキー
	// eKEY_TAB,			//タブキー
	eKEY_RETURN,			//エンターキー

	eKEY_LSHIFT,			//左シフトキー
	// eKEY_RSHIFT,			//右シフトキー
	eKEY_LCONTROL,		//左コントロールキー
	eKEY_RCONTROL,		//右コントロールキー
	eKEY_ESCAPE,			//エスケープキー
	eKEY_SPACE,				//スペースキー
	eKEY_PGUP,			//ＰａｇｅＵＰキー
	eKEY_PGDN,			//ＰａｇｅＤｏｗｎキー
	eKEY_END,			//エンドキー
	eKEY_HOME,			//ホームキー
	eKEY_LEFT,				//左キー
	eKEY_UP,				//上キー
	eKEY_RIGHT,				//右キー
	eKEY_DOWN,				//下キー
	eKEY_INSERT,			//インサートキー
	eKEY_DELETE,			//デリートキー

	eKEY_MINUS,			//－キー
	// eKEY_YEN,			//￥キー
	// eKEY_PREVTRACK,		//＾キー
	// eKEY_PERIOD,			//．キー
	// eKEY_SLASH,			//／キー
	eKEY_LALT,			//左ＡＬＴキー
	// eKEY_RALT,			//右ＡＬＴキー
	// eKEY_SCROLL,			//ScrollLockキー
	// eKEY_SEMICOLON,		//；キー
	// eKEY_COLON,			//：キー
	// eKEY_LBRACKET,		//［キー
	// eKEY_RBRACKET,		//］キー
	// eKEY_AT,				//＠キー
	// eKEY_BACKSLASH,		//＼キー
	// eKEY_COMMA,			//，キー
	// eKEY_KANJI,			//漢字キー
	// eKEY_CONVERT,		//変換キー
	// eKEY_NOCONVERT,		//無変換キー
	// eKEY_KANA,			//カナキー
	// eKEY_APPS,			//アプリケーションメニューキー
	// eKEY_CAPSLOCK,		//CaspLockキー
	// eKEY_SYSRQ,			//PrintScreenキー
	// eKEY_PAUSE,			//PauseBreakキー
	// eKEY_LWIN,			//左Ｗｉｎキー
	// eKEY_RWIN,			//右Ｗｉｎキー

	// eKEY_NUMLOCK,		//テンキー０
#ifdef _DEBUG
	eKEY_NUMPAD0,		//テンキー０
	eKEY_NUMPAD1,		//テンキー１
	eKEY_NUMPAD2,		//テンキー２
	eKEY_NUMPAD3,		//テンキー３
	eKEY_NUMPAD4,		//テンキー４
	eKEY_NUMPAD5,		//テンキー５
	eKEY_NUMPAD6,		//テンキー６
	eKEY_NUMPAD7,		//テンキー７
	eKEY_NUMPAD8,		//テンキー８
	eKEY_NUMPAD9,		//テンキー９
#endif // _DEBUG
	// eKEY_MULTIPLY,		//テンキー＊キー
	// eKEY_ADD,			//テンキー＋キー
	// eKEY_SUBTRACT,		//テンキー－キー
	// eKEY_DECIMAL,		//テンキー．キー
	// eKEY_DIVIDE,			//テンキー／キー
	// eKEY_NUMPADENTER,	//テンキーのエンターキー

	eKEY_F1,				//Ｆ１キー
	// eKEY_F2,				//Ｆ２キー
	//	eKEY_F3,				//Ｆ３キー
	eKEY_F4,				//Ｆ４キー
	eKEY_F5,				//Ｆ５キー
	eKEY_F6,				//Ｆ６キー
	eKEY_F7,				//Ｆ７キー
	eKEY_F8,				//Ｆ８キー
	// eKEY_F9,				//Ｆ９キー
	//	eKEY_F10,			//Ｆ１０キー
	//	eKEY_F11,			//Ｆ１１キー
	//	eKEY_F12,			//Ｆ１２キー

	eKEY_A,					//Ａキー
	eKEY_B,					//Ｂキー
	eKEY_C,					//Ｃキー
	eKEY_D,					//Ｄキー
	eKEY_E,					//Ｅキー
	eKEY_F,					//Ｆキー
	eKEY_G,					//Ｇキー
	eKEY_H,					//Ｈキー
	eKEY_I,					//Ｉキー
	eKEY_J,					//Ｊキー
	eKEY_K,					//Ｋキー
	eKEY_L,					//Ｌキー
	eKEY_M,					//Ｍキー
	eKEY_N,					//Ｎキー
	eKEY_O,					//Ｏキー
	eKEY_P,					//Ｐキー
	eKEY_Q,					//Ｑキー
	eKEY_R,					//Ｒキー
	eKEY_S,					//Ｓキー
	eKEY_T,					//Ｔキー
	eKEY_U,					//Ｕキー
	eKEY_V,					//Ｖキー
	eKEY_W,					//Ｗキー
	eKEY_X,					//Ｘキー
	eKEY_Y,					//Ｙキー
	eKEY_Z,					//Ｚキー

	eKEY_0,					//０キー
	eKEY_1,					//１キー
	eKEY_2,					//２キー
	eKEY_3,					//３キー
	eKEY_4,					//４キー
	eKEY_5,					//５キー
	eKEY_6,					//６キー
	eKEY_7,					//７キー
	eKEY_8,					//８キー
	eKEY_9,					//９キー

	eKEY_MAX,

	/*
	// マウス用定義
	eMOUSE_LCLICK,
	eMOUSE_RCLICK,
	eMOUSE_MIDDLE,
	*/
};

static const unsigned char sKeyConnectDefineTable[eKEY_MAX] = {
	KEY_INPUT_BACK,				//	eKEY_BACK,			//バックスペースキー
	// KEY_INPUT_TAB,			//	eKEY_TAB,			//タブキー
	KEY_INPUT_RETURN,			//	eKEY_RETURN,		//エンターキー	

	KEY_INPUT_LSHIFT,		//	eKEY_LSHIFT,		//左シフトキー	
	// KEY_INPUT_RSHIFT,		//	eKEY_RSHIFT,		//右シフトキー	
	KEY_INPUT_LCONTROL,		//	eKEY_LCONTROL,		//左コントロールキー	
	KEY_INPUT_RCONTROL,		//	eKEY_RCONTROL,		//右コントロールキー	
	KEY_INPUT_ESCAPE,			//	eKEY_ESCAPE,		//エスケープキー	
	KEY_INPUT_SPACE,			//	eKEY_SPACE,			//スペースキー
	KEY_INPUT_PGUP,				//	eKEY_PGUP,			//ＰａｇｅＵＰキー
	KEY_INPUT_PGDN,				//	eKEY_PGDN,			//ＰａｇｅＤｏｗｎキー
	KEY_INPUT_END,				//	eKEY_END,			//エンドキー
	KEY_INPUT_HOME,				//	eKEY_HOME,			//ホームキー
	KEY_INPUT_LEFT,				//	eKEY_LEFT,			//左キー
	KEY_INPUT_UP,				//	eKEY_UP,			//上キー
	KEY_INPUT_RIGHT,			//	eKEY_RIGHT,			//右キー
	KEY_INPUT_DOWN,				//	eKEY_DOWN,			//下キー
	KEY_INPUT_INSERT,			//	eKEY_INSERT,		//インサートキー	
	KEY_INPUT_DELETE,			//	eKEY_DELETE,		//デリートキー	

	KEY_INPUT_MINUS,			//	eKEY_MINUS,			//－キー
	// KEY_INPUT_YEN,			//	eKEY_YEN,			//￥キー
	// KEY_INPUT_PREVTRACK,		//	eKEY_PREVTRACK,		//＾キー	
	// KEY_INPUT_PERIOD,		//	eKEY_PERIOD,		//．キー	
	// KEY_INPUT_SLASH,			//	eKEY_SLASH,			//／キー
	KEY_INPUT_LALT,			//	eKEY_LALT,			//左ＡＬＴキー
	// KEY_INPUT_RALT,			//	eKEY_RALT,			//右ＡＬＴキー
	// KEY_INPUT_SCROLL,		//	eKEY_SCROLL,		//ScrollLockキー	
	// KEY_INPUT_SEMICOLON,		//	eKEY_SEMICOLON,		//；キー	
	// KEY_INPUT_COLON,			//	eKEY_COLON,			//：キー
	// KEY_INPUT_LBRACKET,		//	eKEY_LBRACKET,		//［キー	
	// KEY_INPUT_RBRACKET,		//	eKEY_RBRACKET,		//］キー	
	// KEY_INPUT_AT,			//	eKEY_AT,			//＠キー
	// KEY_INPUT_BACKSLASH,		//	eKEY_BACKSLASH,		//＼キー	
	// KEY_INPUT_COMMA,			//	eKEY_COMMA,			//，キー
	// KEY_INPUT_KANJI,			//	eKEY_KANJI,			//漢字キー
	// KEY_INPUT_CONVERT,		//	eKEY_CONVERT,		//変換キー	
	// KEY_INPUT_NOCONVERT,		//	eKEY_NOCONVERT,		//無変換キー	
	// KEY_INPUT_KANA,			//	eKEY_KANA,			//カナキー
	// KEY_INPUT_APPS,			//	eKEY_APPS,			//アプリケーションメニューキー
	// KEY_INPUT_CAPSLOCK,		//	eKEY_CAPSLOCK,		//CaspLockキー	
	// KEY_INPUT_SYSRQ,			//	eKEY_SYSRQ,			//PrintScreenキー
	// KEY_INPUT_PAUSE,			//	eKEY_PAUSE,			//PauseBreakキー
	// KEY_INPUT_LWIN,			//	eKEY_LWIN,			//左Ｗｉｎキー
	// KEY_INPUT_RWIN,			//	eKEY_RWIN,			//右Ｗｉｎキー

	// KEY_INPUT_NUMLOCK,		//	eKEY_NUMLOCK,		//テンキー０	
#ifdef _DEBUG
	KEY_INPUT_NUMPAD0,		//	eKEY_NUMPAD0,		//テンキー０	
	KEY_INPUT_NUMPAD1,		//	eKEY_NUMPAD1,		//テンキー１	
	KEY_INPUT_NUMPAD2,		//	eKEY_NUMPAD2,		//テンキー２	
	KEY_INPUT_NUMPAD3,		//	eKEY_NUMPAD3,		//テンキー３	
	KEY_INPUT_NUMPAD4,		//	eKEY_NUMPAD4,		//テンキー４	
	KEY_INPUT_NUMPAD5,		//	eKEY_NUMPAD5,		//テンキー５	
	KEY_INPUT_NUMPAD6,		//	eKEY_NUMPAD6,		//テンキー６	
	KEY_INPUT_NUMPAD7,		//	eKEY_NUMPAD7,		//テンキー７	
	KEY_INPUT_NUMPAD8,		//	eKEY_NUMPAD8,		//テンキー８	
	KEY_INPUT_NUMPAD9,		//	eKEY_NUMPAD9,		//テンキー９	
#endif // _DEBUG
	// KEY_INPUT_MULTIPLY,		//	eKEY_MULTIPLY,		//テンキー＊キー	
	// KEY_INPUT_ADD,			//	eKEY_ADD,			//テンキー＋キー
	// KEY_INPUT_SUBTRACT,		//	eKEY_SUBTRACT,		//テンキー－キー	
	// KEY_INPUT_DECIMAL,		//	eKEY_DECIMAL,		//テンキー．キー	
	// KEY_INPUT_DIVIDE,		//	eKEY_DIVIDE,		//テンキー／キー	
	// KEY_INPUT_NUMPADENTER,	//	eKEY_NUMPADENTER,	//テンキーのエンターキー		

	KEY_INPUT_F1,			//	eKEY_F1,			//Ｆ１キー
	// KEY_INPUT_F2,			//	eKEY_F2,			//Ｆ２キー
	//	KEY_INPUT_F3,			//	eKEY_F3,			//Ｆ３キー
	KEY_INPUT_F4,			//	eKEY_F4,			//Ｆ４キー
	KEY_INPUT_F5,			//	eKEY_F5,			//Ｆ５キー
	KEY_INPUT_F6,			//	eKEY_F6,			//Ｆ６キー
	KEY_INPUT_F7,			//	eKEY_F7,			//Ｆ７キー
	KEY_INPUT_F8,			//	eKEY_F8,			//Ｆ８キー
	// KEY_INPUT_F9,			//	eKEY_F9,			//Ｆ９キー
	//	KEY_INPUT_F10,			//	eKEY_F10,			//Ｆ１０キー
	//	KEY_INPUT_F11,			//	eKEY_F11,			//Ｆ１１キー	
	// KEY_INPUT_F12,			//	eKEY_F12,			//Ｆ１２キー	

	KEY_INPUT_A,				//	eKEY_A,				//Ａキー
	KEY_INPUT_B,				//	eKEY_B,				//Ｂキー
	KEY_INPUT_C,				//	eKEY_C,				//Ｃキー
	KEY_INPUT_D,				//	eKEY_D,				//Ｄキー
	KEY_INPUT_E,				//	eKEY_E,				//Ｅキー
	KEY_INPUT_F,				//	eKEY_F,				//Ｆキー
	KEY_INPUT_G,				//	eKEY_G,				//Ｇキー
	KEY_INPUT_H,				//	eKEY_H,				//Ｈキー
	KEY_INPUT_I,				//	eKEY_I,				//Ｉキー
	KEY_INPUT_J,				//	eKEY_J,				//Ｊキー
	KEY_INPUT_K,				//	eKEY_K,				//Ｋキー
	KEY_INPUT_L,				//	eKEY_L,				//Ｌキー
	KEY_INPUT_M,				//	eKEY_M,				//Ｍキー
	KEY_INPUT_N,				//	eKEY_N,				//Ｎキー
	KEY_INPUT_O,				//	eKEY_O,				//Ｏキー
	KEY_INPUT_P,				//	eKEY_P,				//Ｐキー
	KEY_INPUT_Q,				//	eKEY_Q,				//Ｑキー
	KEY_INPUT_R,				//	eKEY_R,				//Ｒキー
	KEY_INPUT_S,				//	eKEY_S,				//Ｓキー
	KEY_INPUT_T,				//	eKEY_T,				//Ｔキー
	KEY_INPUT_U,				//	eKEY_U,				//Ｕキー
	KEY_INPUT_V,				//	eKEY_V,				//Ｖキー
	KEY_INPUT_W,				//	eKEY_W,				//Ｗキー
	KEY_INPUT_X,				//	eKEY_X,				//Ｘキー
	KEY_INPUT_Y,				//	eKEY_Y,				//Ｙキー
	KEY_INPUT_Z,				//	eKEY_Z,				//Ｚキー

	KEY_INPUT_0,				//	eKEY_0,				//０キー
	KEY_INPUT_1,				//	eKEY_1,				//１キー
	KEY_INPUT_2,				//	eKEY_2,				//２キー
	KEY_INPUT_3,				//	eKEY_3,				//３キー
	KEY_INPUT_4,				//	eKEY_4,				//４キー
	KEY_INPUT_5,				//	eKEY_5,				//５キー
	KEY_INPUT_6,				//	eKEY_6,				//６キー
	KEY_INPUT_7,				//	eKEY_7,				//７キー
	KEY_INPUT_8,				//	eKEY_8,				//８キー
	KEY_INPUT_9,				//	eKEY_9,				//９キー
};

// パッド入力定義
#define kJOY_DOWN								(0x00000001)	// ↓チェックマスク
#define kJOY_LEFT								(0x00000002)	// ←チェックマスク
#define kJOY_RIGHT								(0x00000004)	// →チェックマスク
#define kJOY_UP									(0x00000008)	// ↑チェックマスク
#define kJOY_A									(0x00000010)	// Ａボタンチェックマスク
#define kJOY_B									(0x00000020)	// Ｂボタンチェックマスク
#define kJOY_C									(0x00000040)	// Ｃボタンチェックマスク
#define kJOY_X									(0x00000080)	// Ｘボタンチェックマスク
#define kJOY_Y									(0x00000100)	// Ｙボタンチェックマスク
#define kJOY_Z									(0x00000200)	// Ｚボタンチェックマスク
#define kJOY_L									(0x00000400)	// Ｌボタンチェックマスク
#define kJOY_R									(0x00000800)	// Ｒボタンチェックマスク
#define kJOY_START								(0x00001000)	// ＳＴＡＲＴボタンチェックマスク
#define kJOY_M									(0x00002000)	// Ｍボタンチェックマスク

#define kJOY_NUM								(14)

#define kJOY_D									(0x00004000)
#define kJOY_F									(0x00008000)
#define kJOY_G									(0x00010000)
#define kJOY_H									(0x00020000)
#define kJOY_I									(0x00040000)
#define kJOY_J									(0x00080000)
#define kJOY_K									(0x00100000)
#define kJOY_LL									(0x00200000)
#define kJOY_N									(0x00400000)
#define kJOY_O									(0x00800000)
#define kJOY_P									(0x01000000)
#define kJOY_RR									(0x02000000)
#define kJOY_S									(0x04000000)
#define kJOY_T									(0x08000000)
#define kJOY_U									(0x10000000)
#define kJOY_V									(0x20000000)
#define kJOY_W									(0x40000000)
#define kJOY_XX									(0x80000000)

#endif // __INPUT_INFO_H__

/**** end of file ****/
