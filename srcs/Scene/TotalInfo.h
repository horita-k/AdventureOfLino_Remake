/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * TotalInfo.cpp
 * タスク(イベント管理)クラス
 * @author SPATZ.
 * @data   2012/04/04 14:48:17
 */
//---------------------------------------------------
#ifndef __TOTAL_INFO_H__
#define __TOTAL_INFO_H__

#include "dxlib.h"
#include "DataDefine/ItemDef.h"
#include "../Charactor/CharaInfo.h"
#include "EventFunctions.h"
#include "AppLib/Graphic/LayoutBase.h"

#include <time.h>

/*=====================================*
 * define
 *=====================================*/
#define GAME_TITLE		("AdventureOfLino Ver2.0.1")

/*=====================================*
 * 汎用定義値
 *=====================================*/
static const float	kBASIC_FRAME_COUNTER= 1.0f;
static const BYTE	kWEAPON_FLAG_BYTE	= 3;

#ifdef _DEBUG

#define DBG_KEY_DUMP_STATUS		(eKEY_NUMPAD1)
#define DBG_KEY_SKYAWAY			(eKEY_NUMPAD2)
#define DBG_KEY_WARP_LASTPOS	(eKEY_NUMPAD3)
#define DBG_KEY_EVENT_CLEAR		(eKEY_NUMPAD4)
#define DBG_KEY_CURE_PLAYER		(eKEY_NUMPAD5)
#define DBG_KEY_HEAL_ENEMY		(eKEY_NUMPAD6)
#define DBG_KEY_VISIBLE_EARTH	(eKEY_NUMPAD7)
#define DBG_KEY_VISIBLE_WALL	(eKEY_NUMPAD8)
#define DBG_KEY_START_EVENT		(eKEY_NUMPAD9)

#endif // _DEBUG

// サブウェポン数管理
enum eSubweaponNum {
	eSUBWEAPON_NUM_BOMB = 0,
	eSUBWEAPON_NUM_ARROW,
	eSUBWEAPON_NUM_HEART_PIECE,

	eSUBWEAPON_NUM_BIN_EMPTY,
	eSUBWEAPON_NUM_BIN_MAGENTA,
	eSUBWEAPON_NUM_BIN_CYAN,
	eSUBWEAPON_NUM_BIN_YELLOW,
	eSUBWEAPON_NUM_BIN_FAILY,

	eSUBWEAPON_NUM_MAX,
};

static const BYTE kEMPTY_BIN_NUM = 4;


// 時間関連
enum eTimeKind {
	eTIME_KIND_SEC = 0,
	eTIME_KIND_MIN,
	eTIME_KIND_HOUR,

	eTIME_KIND_MAX,
};

/*=====================================*
 * セーブ関連
 * 2015/05/08 MajorVersionを 1 => 2に更新 *
 *=====================================*/
/* 各スロットごとのセーブ用構造体 */
/* ※ ここに要素を追加する場合は SaveData, UpdateLoadDataFromBuffer, ClearSaveInfo, ConvSaveData_101_to_201 にも処理が必要 */
struct tSaveWork 
{
	unsigned char	mSavePlaceIndex;
	unsigned char	mSaveMaxHP;
	unsigned char	mSaveNowHP;
	unsigned short	mSaveMoney;
	unsigned long	mSaveEventFlag[eEVENT_MAX];
	unsigned long	mSaveObjectFlag[eEVENT_MAX];
	unsigned long	mSaveNaviTalkFlag[eEVENT_MAX];
	unsigned long	mSaveFieldInfoFlag[eFIELD_MAX];
	//	ULONGLONG		mSaveWeaponFlag;
	ULONG			mSaveWeaponFlag[kWEAPON_FLAG_BYTE];
	unsigned char	mSaveNowWeapon;
	UCHAR			mSaveSubWeaponNum[eSUBWEAPON_NUM_MAX];
	UCHAR			mSavePlayTime[eTIME_KIND_MAX];
	DWORD			mSaveCosplayBit;
	UCHAR			mSaveNowCosplayIndex;						// 現在のコスプレ
	UCHAR			mSaveColorRevision;							// 色彩補正値
	UCHAR			mSaveSubPlaceIndex;
	DWORD			mSaveKeyItemBit;
	UCHAR			mSaveNowMP;									// マジックポイント
	UCHAR			mSaveSongBit;								// 唄(オカリナ)ビット
};
#define kSAVE_DATA_HEADER				("AOLS")
static const char* saveFileName			= "Save.dat";
/* ゲームバージョン Ver 2.0.1 β版配布 */
/* ゲームバージョン Ver 2.0.2 β版不具合修正*/
static const char  kSAVE_DATA_MAJOR_VER			= 2;
static const char  kSAVE_DATA_MINOR_VER			= 1;

/* ------------------------------------------------------------------------ */
/**** ゲーム配布 1.1版の時のセーブデータ ****/
enum {
	eVER101_EVENT_00 = 0,
 	eVER101_EVENT_01,
	eVER101_EVENT_05,
	eVER101_EVENT_10,
	eVER101_EVENT_13,
	eVER101_EVENT_21,
	eVER101_EVENT_22,
	eVER101_EVENT_24,			// 地下水路
	eVER101_EVENT_25,			// 地下牢
	eVER101_EVENT_26,			// 地底湖
	eVER101_EVENT_27,			// 宝箱ステージ

	eVER101_EVENT_MAX,
};
enum {
	eVER101_FIELD_HOME = 0,
	eVER101_FIELD_REBERNIAN,
	eVER101_FIELD_WILDERNESS,
	eVER101_FIELD_WATERWAY,
	eVER101_FIELD_PRISM,

	eVER101_FIELD_MAX,
};
struct tVer101_SaveWork 
{
	unsigned char	mSavePlaceIndex;
	unsigned char	mSaveMaxHP;
	unsigned char	mSaveNowHP;
	unsigned short	mSaveMoney;
	unsigned long	mSaveEventFlag[eVER101_EVENT_MAX];
	unsigned long	mSaveObjectFlag[eVER101_EVENT_MAX];
	unsigned long	mSaveNaviTalkFlag[eVER101_EVENT_MAX];
	unsigned long	mSaveFieldInfoFlag[eVER101_FIELD_MAX];
	unsigned long	mSaveWeaponFlag;
};
/**** ゲーム配布 1.1版の時のセーブデータ ****/
/* ------------------------------------------------------------------------ */


/*=====================================*
 * コンフィグ関連の情報
 *=====================================*/
static const char*			kCONFIG_HEADER_STRING	= "LOLS";
/* コンフィグバージョン Ver 2.1 β版配布		*/
/* コンフィグバージョン Ver 2.2 β版不具合修正	*/
static const unsigned short kCONFIG_MAJAR_VER	= 2;
static const unsigned short kCONFIG_MINOR_VER	= 2;

enum eConfigParam {
	/*
	eCONFIG_VIDEO_SIZE,                // 画面サイズ
	eCONFIG_GRAPH_QUALITY,		       // 画質クオリティ
	eCONFIG_IS_ANTIALIAS,              // アンチエリアス
	eCONFIG_EFF_QUALITY,				// エフェクトクオリティ
	eCONFIG_JOY_1,                     // joy 通常攻撃
	eCONFIG_JOY_2,                     // joy サブウェポン使用
	eCONFIG_JOY_3,                     // joy ジャンプ
	eCONFIG_JOY_4,                     // joy 回避
	eCONFIG_JOY_5,                     // joy ロックオン
	eCONFIG_JOY_6,                     // joy サブウェポン切替
	eCONFIG_JOY_7,                     // joy マップ表示
	eCONFIG_JOY_8,                     // joy ポーズ/スキップ
	eCONFIG_JOY_9,                     // joy カメラ切替

	eCONFIG_CAP_1,		// 正直不要データだが、整合性の為バッファに保存・・・
	eCONFIG_CAP_2,
	eCONFIG_CAP_3,
	eCONFIG_CAP_4,
	eCONFIG_CAP_5,
	eCONFIG_CAP_6,
	eCONFIG_CAP_7,
	eCONFIG_CAP_8,
	eCONFIG_CAP_9,
	eCONFIG_CAP_10,
	eCONFIG_CAP_11,
	eCONFIG_CAP_12,

	// Ver1.0.1追加分
	eCONFIG_JOY_10,						// joy カメラリセット
	eCONFIG_ANALOG_REVERSE,				// アナログキー反転
		
	eCONFIG_MAX,
	*/
	// Graphic
	eCONFIG_SCREEN_SIZE,       // 画面サイズ
	eCONFIG_GRAPHIC_QUALITY,   // グラフィック品質
	eCONFIG_ANTIALIAS,         // アンチエイリアス
	eCONFIG_BACKGROUND_RUN,    // バックグラウンド起動
	// Pad
	eCONFIG_JOY_1,                     //
	eCONFIG_JOY_2,                     //
	eCONFIG_JOY_3,                     //
	eCONFIG_JOY_4,                     //
	eCONFIG_JOY_5,                     //
	eCONFIG_JOY_6,                     //
	eCONFIG_JOY_7,                     //
	eCONFIG_JOY_8,                     //
	eCONFIG_JOY_9,                     //
	eCONFIG_JOY_10,
	eCONFIG_CAP_1,
	eCONFIG_CAP_2,
	eCONFIG_CAP_3,
	eCONFIG_CAP_4,
	eCONFIG_CAP_5,
	eCONFIG_CAP_6,
	eCONFIG_CAP_7,
	eCONFIG_CAP_8,
	eCONFIG_CAP_9,
	eCONFIG_CAP_10,
	eCONFIG_CAP_11,
	eCONFIG_CAP_12,
	// Analog
	eCONFIG_LEFT_ANALOG_HORIZON,
	eCONFIG_LEFT_ANALOG_VERTICAL,
	eCONFIG_LEFT_ANALOG_AXIS,
	eCONFIG_RIGHT_ANALOG_HORIZON,
	eCONFIG_RIGHT_ANALOG_VERTICAL,
	eCONFIG_RIGHT_ANALOG_AXIS,
	// KeCONFIG_yboard
	eCONFIG_KEY_UP,
	eCONFIG_KEY_DOWN,
	eCONFIG_KEY_RIGHT,
	eCONFIG_KEY_LEFT,

	eCONFIG_KEY_CAM_UP,
	eCONFIG_KEY_CAM_DOWN,
	eCONFIG_KEY_CAM_RIGHT,
	eCONFIG_KEY_CAM_LEFT,
	eCONFIG_KEY_RESET_CAMERA,
	eCONFIG_KEY_CHANGE_CAMERA,

	eCONFIG_KEY_ATTACK,
	eCONFIG_KEY_USE_ITEM,
	eCONFIG_KEY_JUMP,
	eCONFIG_KEY_AWAY,
	eCONFIG_KEY_LOCKON,
	eCONFIG_KEY_SELECT_ITEM,
	eCONFIG_KEY_SHOW_MAP,
	eCONFIG_KEY_SKIP_EVENT,

	eCONFIG_MOUSE_SCROLL_VAL,

	eCONFIG_MAX,
};

/*=====================================*
 * ウィンドウズメニュー一覧定義
 *=====================================*/
enum {
	// ファイル
	eWINMENU_MENU_ROOT = 0,
	eWINMENU_MENU_TO_TITLE,
	eWINMENU_MENU_EXIT,

	// 表示
	eWINMENU_VISIBLE_ROOT,
	eWINMENU_VISIBLE_FPS,

	// キーコンフィグ
	eWINMENU_KEYCONFIG_ROOT,
	eWINMENU_KEYCONFIG_KEYCONFIG,

	// 影の表示
	eWINMENU_SHADOW_ROOT,
	eWINMENU_SHADOW_CIRCLE,
	eWINMENU_SHADOW_SHADOWMAP_MODEL,
	
	// 色彩補正
	eWINMENU_COLOR_REVISION_ROOT,
	eWINMENU_COLOR_REVISION_DISABLE,
	eWINMENU_COLOR_REVISION_ENABLE,

	eWINMENU_MAX,

	// 拡張コスプレ情報
	eWINMENU_COSPLAY_ROOT = eWINMENU_MAX,
};
/*=====================================*
 * コスプレ情報
 *=====================================*/
enum {
	// コスプレ
	eWINMENU_COSPLAY_NONE = 0,					// リノ(コスプレなし)
	eWINMENU_COSPLAY_FRES,						// フレスベルク
	eWINMENU_COSPLAY_DABAH,						// だばぁ
	eWINMENU_COSPLAY_SUNOPHONE,					// スノフォン
	eWINMENU_COSPLAY_MASK,						// マスク
	eWINMENU_COSPLAY_BAT,						// こうもり
	eWINMENU_COSPLAY_TDA_MIKU,					// ミク
	eWINMENU_COSPLAY_PRONAMA,					// プロ生ちゃん
	eWINMENU_COSPLAY_LAPAN,						// ラパン
	eWINMENU_COSPLAY_CHILNO,					// チルノ
	eWINMENU_COSPLAY_HONGKONG,					// 香港映画P
	eWINMENU_COSPLAY_LOANA,						// ロアナ
	eWINMENU_COSPLAY_YUKARI,					// 結月ゆかり
	eWINMENU_COSPLAY_NAKANO,					// 鷺宮カノ

	// 乗り物ティタニスを番兵にさせる
	eWINMENU_COSPLAY_TITANIS,					// ティタニス
	eWINMENU_COSPLAY_GUARDIAN,					// ガーディアン
	
	eWINMENU_COSPLAY_MAX,
};

/* 不要・・・？
enum eConfigLoadState {
	eCONFIG_LOAD_OK = 0,
	eCONFIG_LOAD_ERR_HEADER,	// ヘッダーの相違エラー
	eCONFIG_LOAD_ERR_VERSION,	// バージョン相違エラー
};
*/

struct tDisplaySetting {
	int mResolusionX;
	int mResolusionY;
	int mColorBit;
	int mWindowSizeX;
	int mWindowSizeY;
};
static const tDisplaySetting sDisplaySetting[] = {
	/*
	  {1680,	1050,	32,		-1,		-1, },	// フルスクリーン
	  {1024,	768,	32,		640,	480,},	// 動画キャプチャ用画面
	  {800,	600,	32,		800,	600,},	// 
	  {1024,	768,	32,		1024,	768,},	// 高画質ゲーム用画面
	  {960,	720,	32,		640,	480,},	// 
	  {640,	480,	32,		640,	480,},	// 
	  {512,	384,	32,		512,	384,},	// 
	  {2048,	1536,	32,		1024,	768,},	// 
	*/
	{320, 	240,	32,		320, 	240, },
	{640, 	360,    32,    	640, 	360, },
	{640, 	480,    32,    	640, 	480, },
	{800, 	600,    32,    	800, 	600, },
	{1024,	768,    32,    	1024,	768, },
	{1280,	720,    32,    	1280,	720, },
	{1280,	960,    32,    	1280,	960, },
	{1600,	900,    32,    	1600,	900, },
	{1600,	1200,   32,    	1600,	1200,},
	{1920,	1080,   32,    	1920,	1080,},
	{2048,	1536,   32,    	2048,	1536,},
	//<! デバッグ用に急所ついか
	{1024,	768,	32,		640,	480,},	// 動画キャプチャ用画面
};

/*=====================================*
 * class
 *=====================================*/
class TotalInfo {
	
 public:
	static TotalInfo* const GetInstance(void);

	// ロード状況
	enum eLoadStatus {
		eLOAD_FAILED = 0,
		eLOAD_SUCEESS,
		eLOAD_REQUEST_RELOAD,		// 再ロード依頼
	};

	// レイアウト情報
	enum {
		eLY_MONEY = 0,
		eLY_COMPASS,
		eLY_LOCKON,
		eLY_HEART_00,
		eLY_HEART_01,
		eLY_HEART_NOW,
		eLY_PIYOPIYO_GAUGE_FRAME,
		eLY_PIYOPIYO_GAUGE_LINE,
		eLY_PIYOPIYO_GAUGE_ICON,
		eLY_MAGIC_GAUGE_FRAME,
		eLY_MAGIC_GAUGE_LINE,
		eLY_MAGIC_GAUGE_ICON,
		eLY_LINO_FACE,
		eLY_RING_FLAME,
		eLY_SUB_WEAPON,
		eLY_GAUGE_LINE,
		eLY_GAUGE_FRAME,
		eLY_MAP_ARROW,
		eLY_MAP_POSITION,
		eLY_BALLOON,
		// 以下のレイアウトが最初は表示されていないレイアウト
		eLY_HAS_MAP,							// マップを持っているか？
		eLY_HAS_KEY,							// 鍵を持っているか？

		eLY_MAX,
	};

	//	static int sDefaultMotionModelHandle;
	struct tLoadWork {
		unsigned char		mPlaceIndex;
		unsigned char		mMaxHP;
		unsigned char		mNowHP;
		unsigned char		mNowWeapon;
		UCHAR				mNowCosplayIndex;
		UCHAR				mSubPlaceIndex;
		UCHAR				mNowMP;
	};

	struct tWinMenuCosplayInfo {
		const char* mMessage;
		const char* mModelName;
		const char* mLayoutName;
	};

	/* 関数 */
	void		Setup(void);
	void		Update(void);

	/*=====================================*
	 * コンフィグデータ関連
	 *=====================================*/
	bool		LoadConfigData(void);
	bool		SetupConfigData(char* pBuf, int size);

	void		SetupAssignKeyboard(void);
	void		SetupAssignJoyInput(void);
	void		SetupAnalogReverse(void);
	void		SetupMouseSensitive(void);

	int			GetConfigVideoSize(void);
	float		GetConfigResolutionRateFromGraphQuality(void);
	bool		GetConfigIsAntiAlias(void);
	bool		GetConfigIsBackgroundRun(void);
	int			GetConfigEffectQuality(void);

	float		GetConfigMusicVolumeScale(void)	{ return mMusicVolumeScale; }
	float		GetConfigSoundVolumeScale(void)	{ return mSoundVolumeScale; }
	float		GetConfigVoiceVolumeScale(void)	{ return mVoiceVolumeScale; }

	/*=====================================*
	 * セーブ関連
	 *=====================================*/
	bool		SaveData(int slotID, int stageIndex, int maxHP, int nowHP, int nowWeapon, BYTE cosplayIndex, BYTE subPlaceIndex, UCHAR nowMP);

	bool		saveDataWork(int slotID, tSaveWork& rSaveWork, bool isConv=false);

	//	eLoadStatus	LoadData(int slotID);
	eLoadStatus	LoadDataToBuffer(tSaveWork* p3LoadWork);
	void		UpdateLoadDataFromBuffer(tSaveWork& rLoadWork);
	void		SetPlayintSaveSlot(int slotIndex)						{ mPlayingSaveSlot = slotIndex; }

	void		GetLoadWork(TotalInfo::tLoadWork* pLoadWork)			{ *pLoadWork = mLoadWork; }
	void		ClearSaveInfo(tSaveWork& rSaveWork);
	void		ConvSaveData_101_to_201(tSaveWork& rSaveWork);
	//	void		GetTotalPlayTime(UCHAR* pPlayTimeArray);

	/*=====================================*
	 * Event flags 関連
	 *=====================================*/
	void		SetEventFlag(unsigned long kind, unsigned long flag);
	bool		GetEventFlagEnable(unsigned long kind, unsigned long flag);
	ULONG		GetEventFlagValue(unsigned long kind);
	void		SetEventFlagValue(unsigned long kind, unsigned long value);
	void		DisableEventFlag(unsigned long kind, unsigned long flag);
	void		ResetAllEventFlag(void);

	/*=====================================*
	 * ナビォトーク 関連
	 *=====================================*/
	void		SetNaviTalkFlag(unsigned long kind, unsigned long flag);
	bool		GetNaviTalkFlagEnable(unsigned long kind, unsigned long flag);

	/*=====================================*
	 * Object Flags 関連
	 *=====================================*/
	void			SetObjectFlagBit(unsigned long kind, unsigned long bit);
	unsigned long	GetObjectFlagBit(unsigned long kind);

	/*=====================================*
	 * サブウェポンフラグ
	 *=====================================*/
	void		SetWeaponFlag(eItemKind item);
	void		DisableWeaponFlag(eItemKind item);
	bool		IsWeaponFlag(eItemKind item);

	/*=====================================*
	 * Item flags 関連
	 *=====================================*/
	void		EnableItemFlag(eItemKind kind);
	bool		IsItemFlag(eItemKind kind);

	/*=====================================*
	 * フィールドごとのflags 関連
	 *=====================================*/
	void		SetEnableFieldInfoFlag(int stageIndex, unsigned long flag);
	void		SetDisableFieldInfoFlag(int stageIndex, unsigned long flag);
	bool		GetHasFieldMap(int stageIndex);
	bool		GetHasFieldKey(int stageIndex);

	bool		GetIsFieldStartNoConvert(int fieldIndex);
	unsigned long GetFieldInfoFlagBit(int stageIndex);
	void		ResetFieldInfoFlag(void);

	/*=====================================*
	 * レイアウト関連
	 *=====================================*/
	void		LoadTotalInfoLayout(void);

	void		VisibleBattleIcon(void);
	void		HideBattleIcon(void);
	void		UpdateCompass(void);
	void		CrazyCompass(void);
	void		SetCommapssForShowMap(void);

	void		EnableLockonLayout(VECTOR* pWorldPos, BOOL isHighlight=TRUE);
	void		DisableLockonLayout(void);
	void		SetActiveLockonEnemyName(BOOL isActive);

	void		UpdateHeartLayout(int maxHP, int nowHP);
	void		ShowHeartLayout(int maxHP, int nowHP, BOOL isTitle=FALSE);

	LayoutBase*	GetSystemLayout(int index);

	const char*	GetLinkLayoutName(int numIndex);
	void		UpdatePiyoPiyoGaugeByRate(float rate, float magicRate);

	void		FadeinLockonEnemyLayout(int gaugeSize);
	void		FadeoutLockonEnemyLayout(void);
	void		ChangePlayerFaceLayout(bool isEncount);

	void		MsgTypeTelop(void);
	void		MsgTypeMessage(void);
	void		MsgFadeIn(float sec=0.3f);
	void		MsgFadeOut(float sec=0.3f);
	bool		MsgIsFade(void);

	void		setupLayoutSettingInfo(int lytIndex);
	void		setLayoutPosPadRight(int lytIndex);

	/*=====================================*
	 * 金関連
	 *=====================================*/
	int			GetNowMoney(void);
	void		AddMoney(int value);

	void		AddItemNum(eSubweaponNum subWeaponNum, int value, bool isUpdateDraw=true);
	UCHAR		GetItemNum(eSubweaponNum subWeaponNum);

	/*=====================================*
	 * ウィンドウメニュー関連
	 *=====================================*/
	void		SetupWinMenu(void);
	void		SetEnableWinMenu(BOOL isEnable);
	BYTE&		IsVisibleFPS(void)						{ return mIsVisibleFPS; }

	// 色彩補正有効・無効フラグ (ただし、メニュー設定が優先)
	void		SetEnableColorRevision(BOOL isEnable);

	/*=====================================*
	 * 時間関連
	 *=====================================*/
	void		StartPlayingTime(void);
	void		UpdatePlayingTime(void);

	/*=====================================*
	 * コスプレ関係
	 *=====================================*/
	BYTE		GetCosplayNum(void);
	DWORD		GetCosplayBit(void)						{ return mCosplayBit; }
	void		AddCosplay(int cosplayIndex);
	tWinMenuCosplayInfo* GetCosplayInfo(int cosplayKind);
	
	/*=====================================*
	 * キーアイテム関連
	 *=====================================*/
	DWORD		GetKeyItemBit(void);
	void		AddKeyItem(USHORT keyItemIndex);
	BOOL		IsKeyItem(USHORT keyItemIndex);

	/*=====================================*
	 * 唄(オカリナ)関連
	 *=====================================*/
	void		AddSongType(UCHAR songType);
	BOOL		IsSongType(UCHAR songType);

	// 
	//	static unsigned long sInputAttack[2];
	//	unsigned long&	InputAttack(int index)		{ return mInputAttack[index]; }

 private:
	TotalInfo();
	~TotalInfo();

	/* 変数 */
	tLoadWork			mLoadWork;

	ULONG				mEventFlag[eEVENT_MAX];
	ULONG				mObjectFlag[eEVENT_MAX];
	ULONG				mNaviTalkFlag[eEVENT_MAX];
	ULONG				mFieldInfoFlag[eFIELD_MAX];
	//	ULONGLONG			mWeaponFlag;
	ULONG				mWeaponFlag[kWEAPON_FLAG_BYTE];

	ULONG				mItemFlag;

	//<! 以下はセーブ情報に移動する予定
	float				mMusicVolumeScale;
	float				mSoundVolumeScale;
	float				mVoiceVolumeScale;

	UCHAR				mSubWeaponNum[eSUBWEAPON_NUM_MAX];
	UCHAR				mTotalPlayTime[eTIME_KIND_MAX];

	time_t				mPlayStartTime;
	DWORD				mCosplayBit;
	DWORD				mKeyItemBit;
	UCHAR				mSongBit;

	/*=====================================*
	 * 金関連
	 *=====================================*/
	int					mNowMoney;

	/*=====================================*
	 * メニューで変更できる項目
	 *=====================================*/
	BYTE				mIsVisibleFPS;
	CHAR				mPlayingSaveSlot;

	//	ULONG		mInputAttack[2];

};

// ウィンドウメニュー更新通知コールバック関数
void CallbackMenuSelect(const TCHAR *pItemName, int nItemID);

#endif //__TOTAL_INFO_H__

 /**** end of file ****/
