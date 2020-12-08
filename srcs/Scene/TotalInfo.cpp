/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * TotalInfo.cpp
 * タスク(イベント管理)クラス
 * @author SPATZ.
 * @data   2012/04/04 14:48:17
 */
//---------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS

#include "AppLib/Basic/Basic.h"
#include "AppLib/Resource/ResourceLayout.h"
#include "AppLib/Graphic/LayoutNumber.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Graphic/Shadow.h"
#include "AppLib/Graphic/ShadowMap.h"
#include "AppLib/Graphic/RenderBloom.h"
#include "AppLib/Graphic/CameraManager.h"
#include "AppLib/Collision/CollisionManager.h"
#include "AppLib/Resource/ResourceManager.h"
#include "AppLib/Resource/ResourceModel.h" 
#include "AppLib/Sound/SoundManager.h"
#include "Charactor/CharaPlayer.h"
#include "SceneManager.h"
#include "SceneMainGame.h"

#include "TotalInfo.h"
#include "DataDefine/VoiceDef.h"

#include "Utility/Utility.h"

#include <time.h>

//#include "CommandFunctions.h"


//#ifdef _DEBUG
#include "AppLib/Input/InputManager.h"
//#endif // _DEBUG

static LayoutBase		sTILayout[TotalInfo::eLY_MAX];
static LayoutNumber		sTILayoutNumber[4];
static LayoutNumber		sTiLytItemNum[2];

// コンフィグ情報
static byte sConfigParam[eCONFIG_MAX] = {0};
static BYTE	sIsMenuColorRevision = TRUE;		// 色彩補正

static const tLayoutSetting sLytSettingList[TotalInfo::eLY_MAX] = {
	// Name,							Pos,		Size,		Alpha
	{RES_LYT_MONEY,						516, 440,	32,  32,	200,			},//	eLY_MONEY = 0,
	{RES_LYT_COMPASS,					56,	 162,	64,  64,	200,			},//	eLY_COMPASS,
	{RES_LYT_LOCKON,					320, 240,	32,  32,	0,				},//	eLY_LOCKON,
	{RES_LYT_HEART_FULL,				120, 32,	24,  24,	200,			},//	eLY_HEART_00,
	{RES_LYT_HEART_EMPTY,				0,   0,		24,  24,	128,			},//	eLY_HEART_01,
	{RES_LYT_HEART_1DIV4,				0,   0,		24,  24,	200,			},//	eLY_HEART_NOW,
	{RES_LYT_ICON_GAUGE_FRAME,			190, 56,	120, 8,		176,			},//	eLY_PIYOPIYO_GAUGE_FRAME,
	{RES_LYT_ICON_GAUGE_LINE,			190, 56,	120, 8,		176,			},//	eLY_PIYOPIYO_GAUGE_LINE,
	//	{RES_LYT_PIYOPIYO,					88,  54,	20,  20,	230,			},//	eLY_PIYOPIYO_GAUGE_ICON
	{RES_LYT_PIYOPIYO,					118,  54,	20,  20,	230,			},//	eLY_PIYOPIYO_GAUGE_ICON
	{RES_LYT_ICON_GAUGE_FRAME,			190, 78,	120, 8,		176,			},//	eLY_MAGIC_GAUGE_FRAME,
	{RES_LYT_ICON_GAUGE_LINE,			190, 78,	120, 8,		176,			},//	eLY_MAGIC_GAUGE_LINE,
	//	{RES_LYT_MAGIC_GAUGE,				88,  74,	20,  20,	255,			},//	eLY_MAGIC_GAUGE_ICON
	{RES_LYT_MAGIC_GAUGE,				118,  74,	20,  20,	255,			},//	eLY_MAGIC_GAUGE_ICON
	{RES_LYT_LINO_FACE_A,				62,  60,	98,  98,	255,			},	//	eLY_LINO_FACE,
	{RES_LYT_RING_FRAME,				568, 64,	86,  86,	176,			},	//	eLY_RING_FLAME,
	{"",								568, 64,	80,	 80,	255,			},	//	eLY_SUB_WEAPON,
	{RES_LYT_ICON_GAUGE_LINE,			146, 440,	256, 16,	0,				},	//	eLY_GAUGE_LINE,
	{RES_LYT_ICON_GAUGE_FRAME,			146, 440,	256, 16,	0,				},	//	eLY_GAUGE_FRAME,
	{RES_LYT_MAPARROW,					0,   0,		48,	 64,	0,				},	//	eLY_MAP_ARROW,
	{RES_LYT_ICON_MAP_LINO_FACE,		0,   0,		32,	 32,	0,				},	//	eLY_MAP_POSITION,
	{RES_LYT_BALLOON_MSG,				320, 384,	0,	 0,		0,				},	//	eLY_BALLOON,
	{RES_LYT_ITEM_RING_MAP,				120, 102,	32,	 32,	0,				},	//	eLY_HAS_MAP,							// マップを持っているか？
	{RES_LYT_ITEM_RING_BOSS_KEY,		158, 102,	32,	 32,	0,				},	//	eLY_HAS_KEY,							// 鍵を持っているか？

};

/*=====================================*
 * ハートのレイアウト名
 *=====================================*/
static const char* sHalfHeartLayoutName[4] = {
	RES_LYT_HEART_FULL,
	RES_LYT_HEART_1DIV4,
	RES_LYT_HEART_2DIV4,
	RES_LYT_HEART_3DIV4,
};

/*=====================================*
 * メニュー関連
 *=====================================*/
struct tWinMenuInfo {
	const CHAR	mRootMenuIndex;
	const char* mMenuName;
};

const tWinMenuInfo sWinMenuInfo[eWINMENU_MAX] = {
	// メニュー
	{-1,						"メニュー",},				// eWINMENU_MENU_ROOT = 0,
	{eWINMENU_MENU_ROOT,		"タイトルへ",},				// eWINMENU_MENU_EXIT,
	{eWINMENU_MENU_ROOT,		"ゲーム終了",},				// eWINMENU_MENU_TO_TITLE,
	// 表示
	{-1,						"表示",},					// eWINMENU_VISIBLE_ROOT
	{eWINMENU_VISIBLE_ROOT,		"FPS表示",},				// eWINMENU_VISIBLE_FPS

	// キーコンフィグ
	{-1,						"キーコンフィグ",},			// eWINMENU_KEYCONFIG_ROOT
	{eWINMENU_KEYCONFIG_ROOT,	"キーコンフィグ",},			// eWINMENU_KEYCONFIG_ROOT

	// 影の表示
	{-1,						"影の表示",},				// eWINMENU_SHADOW_ROOT
	{eWINMENU_SHADOW_ROOT,		"丸影",},					// eWINMENU_SHADOW_CIRCLE
	{eWINMENU_SHADOW_ROOT,		"シャドウマップ(キャラのみ)",},		// eWINMENU_SHADOW_SHADOWMAP_MODEL

	// 色彩補正
	{-1,						"描画方法",}	,			// eWINMENU_COLOR_REVISION_ROOT
	{eWINMENU_COLOR_REVISION_ROOT,		"ノーマル",},			// eWINMENU_COLOR_REVISION_DISABLE
	{eWINMENU_COLOR_REVISION_ROOT,		"色鮮やか",},				// eWINMENU_COLOR_REVISION_ENABLE

};

/*=====================================*
 * コスプレ定義
 *=====================================*/
const TotalInfo::tWinMenuCosplayInfo sWinMenuCosplayInfoName[eWINMENU_COSPLAY_MAX] = {
	// コスプレ
	{"リノ(コスプレなし)",		Name_Lino,		RES_LYT_COSPLAY_LINO},			// eWINMENU_COSPLAY_NONE,
	{"フレスベルク",			NAME_Fresbelk,	RES_LYT_COSPLAY_FRES},			// eWINMENU_COSPLAY_DABAH,
	{"だばぁ",					NAME_Dava,		RES_LYT_COSPLAY_DABAH},			// eWINMENU_COSPLAY_DABAH,
	{"ハトフォン",				NAME_Hatophon,	RES_LYT_COSPLAY_HATOPHONE},		// eWINMENU_COSPLAY_SUNOPHONE,
	{"マスク・ザ・春原",		NAME_Mask,		RES_LYT_COSPLAY_MASK},			// eWINMENU_COSPLAY_MASK,
	{"こうもり",				NAME_Bat,		RES_LYT_COSPLAY_BAT},			// eWINMENU_COSPLAY_BAT,
	{"初音ミク",				NAME_TdaMiku,	RES_LYT_COSPLAY_TDA_MIKU},		// eWINMENU_COSPLAY_TDA_MIKU
	{"プロ生ちゃん",			NAME_Pronama,	RES_LYT_COSPLAY_PRONAMA},		// eWINMENU_COSPLAY_PRONAMA
	{"らぱん",					NAME_Lapan,		RES_LYT_COSPLAY_LAPAN},			// eWINMENU_COSPLAY_PRONAMA
	{"チルノ",					NAME_Chilno,	RES_LYT_COSPLAY_CHILNO},		// eWINMENU_COSPLAY_CHILNO
	{"香港映画P",				NAME_HongKong,	RES_LYT_COSPLAY_HONG_KONG},		// eWINMENU_COSPLAY_HONGKONG
	{"ロアナ",					NAME_Loana,		RES_LYT_COSPLAY_LOANA},			// eWINMENU_COSPLAY_LOANA
	{"結月ゆかり",				NAME_Yukari,	RES_LYT_COSPLAY_YUKARI},		// eWINMENU_COSPLAY_YUKARI
	{"鷺宮カノ",				NAME_Nakano,	RES_LYT_COSPLAY_NAKANO},		// eWINMENU_COSPLAY_NAKANO
	
	// 乗り物ティタニスを番兵にさせる
	{"",						"",				""},							// eWINMENU_COSPLAY_TITANIS,
	{"",						"",				""},							// eWINMENU_COSPLAY_GUARDIAN,
};

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/TotalInfo* const 
TotalInfo::GetInstance(void)
{
	static TotalInfo sInstance;
	return &sInstance;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::Setup(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::Update(void)
{
	return;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
TotalInfo::LoadConfigData(void)
{
	/**** Config.dat を読み込み ****/
	static const char* dat_file = "Setting.dat";
	int file_handle = -1;
	int file_size = -1;
	int result = 0;
	char* file_buffer = NULL;
	file_handle =FileRead_open(dat_file, FALSE);
	file_size = (int)FileRead_size(dat_file);
	if ((file_handle == 0) || (file_size == -1)) {
		SYSTEM_MESSAGE("ファイル読み込みエラー", "Setting.datの読み込みに失敗しました。\nSetting.exeを実行して下さい。");
		return false;
	}
	file_buffer = new char[file_size];
	APP_ASSERT_PRINT((file_buffer != NULL),
					 "Failed new file_buffer[%d] \n", file_size);
	result = FileRead_read(file_buffer, file_size, file_handle);
	APP_ASSERT_PRINT(result != -1,
					 "Failed FileRead_read() \n");
	
	/**** dataファイルの中身をセッティング ****/
	if (SetupConfigData(file_buffer, file_size) == false) {
		delete[] file_buffer;
		FileRead_close(file_handle);
		file_handle = -1;

		return false;
	}
	delete[] file_buffer;
	FileRead_close(file_handle);
	file_handle = -1;

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
TotalInfo::SetupConfigData(char* pBuf, int size)
{
	// ヘッダーのチェック
	char header_str[5] = {0};
	memcpy(&header_str[0], &pBuf[0], 4);
	PRINT_CONSOLE("Setting.dat : header = {%c %c %c %c}\n", header_str[0], header_str[1], header_str[2], header_str[3]);
	if (strcmp(header_str, kCONFIG_HEADER_STRING) != 0) {

		SYSTEM_MESSAGE("ファイル不整合エラー", "Setting.datの中身が異常です。\n再度Setting.exeを実行して下さい。");

		return false;
	}

	// バージョンのチェック
	unsigned short majar_ver, minor_ver;
	memcpy(&minor_ver, &pBuf[4], 2);
	memcpy(&majar_ver, &pBuf[6], 2);
	PRINT_CONSOLE("Setting.dat : version = %d.%d \n", majar_ver, minor_ver);
	if ((majar_ver != kCONFIG_MAJAR_VER) || (minor_ver != kCONFIG_MINOR_VER)) {

		char msg[256] = {0};
		sprintf_s(msg, sizeof(msg), "Setting.datのバージョンが不正です。\n 正：%d.%d \n 現：%d.%d \n再度Setting.exeを実行して下さい。", 
				  kCONFIG_MAJAR_VER, kCONFIG_MINOR_VER, majar_ver, minor_ver);
		SYSTEM_MESSAGE("バージョン相違エラー", msg);

		return false;
	}

	// 各情報を読み込み
	memcpy(&sConfigParam[0], &pBuf[8], sizeof(sConfigParam));

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
unsigned int convConfigKeyboardStr_TO_KeyIndex(byte str)
{
	const byte sVALUE_ENTER = 0x01;
	const byte sVALUE_SPACE = 0x02;
    const byte sVALUE_ALT = 0x03;

	switch (str) {
	case sVALUE_ENTER:
		return eKEY_RETURN;

	case sVALUE_SPACE:
		return eKEY_SPACE;

	case sVALUE_ALT:
		return eKEY_LALT;

	default:
		/* default のみ以下の処理をおこなう */
		break;
	}

	return ((str - 'a')  + eKEY_A);
}

#define ASSIGN_MACRO_KEY(keyindex)		(convConfigKeyboardStr_TO_KeyIndex(sConfigParam[keyindex]) )
#define ASSIGN_MACRO(joyindex)	(1 << (sConfigParam[joyindex]+4))
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::SetupAssignKeyboard(void)
{
	InputManager* const p_mgr = InputManager::GetInstance();
	
	p_mgr->AssignKeyboard(JOY_INPUT_UP, 			ASSIGN_MACRO_KEY(eCONFIG_KEY_UP) );
	p_mgr->AssignKeyboard(JOY_INPUT_DOWN, 			ASSIGN_MACRO_KEY(eCONFIG_KEY_DOWN) );
	p_mgr->AssignKeyboard(JOY_INPUT_RIGHT,			ASSIGN_MACRO_KEY(eCONFIG_KEY_RIGHT) );
	p_mgr->AssignKeyboard(JOY_INPUT_LEFT, 			ASSIGN_MACRO_KEY(eCONFIG_KEY_LEFT) );
	p_mgr->AssignKeyboard(JOY_INPUT_JUMP, 			ASSIGN_MACRO_KEY(eCONFIG_KEY_JUMP) );
	p_mgr->AssignKeyboard(JOY_INPUT_AWAY, 			ASSIGN_MACRO_KEY(eCONFIG_KEY_AWAY) );
	p_mgr->AssignKeyboard(JOY_INPUT_LOCKON, 		ASSIGN_MACRO_KEY(eCONFIG_KEY_LOCKON) );
	p_mgr->AssignKeyboard(JOY_INPUT_SELECT_SUB, 	ASSIGN_MACRO_KEY(eCONFIG_KEY_SELECT_ITEM) );
	p_mgr->AssignKeyboard(JOY_INPUT_SHOW_MAP, 		ASSIGN_MACRO_KEY(eCONFIG_KEY_SHOW_MAP) );
	p_mgr->AssignKeyboard(JOY_INPUT_START, 			ASSIGN_MACRO_KEY(eCONFIG_KEY_SKIP_EVENT) );
	p_mgr->AssignKeyboard(JOY_INPUT_CAM_RESET, 		ASSIGN_MACRO_KEY(eCONFIG_KEY_RESET_CAMERA) );
	p_mgr->AssignKeyboard(JOY_INPUT_ATTACK, 		ASSIGN_MACRO_KEY(eCONFIG_KEY_ATTACK) );
	p_mgr->AssignKeyboard(JOY_INPUT_SUB_WEAPON, 	ASSIGN_MACRO_KEY(eCONFIG_KEY_USE_ITEM) );
	p_mgr->AssignKeyboard(JOY_INPUT_CAM_SWITCH, 	ASSIGN_MACRO_KEY(eCONFIG_KEY_CHANGE_CAMERA) );

	p_mgr->AssignKeyboard(JOY_INPUT_CAM_UP,			ASSIGN_MACRO_KEY(eCONFIG_KEY_CAM_UP) );
	p_mgr->AssignKeyboard(JOY_INPUT_CAM_DOWN,		ASSIGN_MACRO_KEY(eCONFIG_KEY_CAM_DOWN) );
	p_mgr->AssignKeyboard(JOY_INPUT_CAM_RIGHT,		ASSIGN_MACRO_KEY(eCONFIG_KEY_CAM_RIGHT) );
	p_mgr->AssignKeyboard(JOY_INPUT_CAM_LEFT,		ASSIGN_MACRO_KEY(eCONFIG_KEY_CAM_LEFT) );

	//	p_mgr->AssignKeyboard(JOY_INPUT_ROLL_CAMERA, ASSIGN_MACRO_KEY(eCONFIG_KEY_ROLL_CAMERA) );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::SetupAssignJoyInput(void)
{
	InputManager* const p_mgr = InputManager::GetInstance();

	p_mgr->AssignJoyPad(JOY_INPUT_ATTACK,			ASSIGN_MACRO(eCONFIG_JOY_1));
	p_mgr->AssignJoyPad(JOY_INPUT_JUMP,				ASSIGN_MACRO(eCONFIG_JOY_2));
	p_mgr->AssignJoyPad(JOY_INPUT_SUB_WEAPON,		ASSIGN_MACRO(eCONFIG_JOY_3));
	p_mgr->AssignJoyPad(JOY_INPUT_AWAY,				ASSIGN_MACRO(eCONFIG_JOY_4));
	p_mgr->AssignJoyPad(JOY_INPUT_LOCKON,			ASSIGN_MACRO(eCONFIG_JOY_5));
	p_mgr->AssignJoyPad(JOY_INPUT_SELECT_SUB,		ASSIGN_MACRO(eCONFIG_JOY_6));
	p_mgr->AssignJoyPad(JOY_INPUT_SHOW_MAP,			ASSIGN_MACRO(eCONFIG_JOY_7));
	p_mgr->AssignJoyPad(JOY_INPUT_START,			ASSIGN_MACRO(eCONFIG_JOY_8));
	p_mgr->AssignJoyPad(JOY_INPUT_CAM_SWITCH,		ASSIGN_MACRO(eCONFIG_JOY_9));
	p_mgr->AssignJoyPad(JOY_INPUT_CAM_RESET,		ASSIGN_MACRO(eCONFIG_JOY_10));
	//	p_mgr->AssignJoyPad(JOY_INPUT_RUN,			(1<<sConfigParam[eCONFIG_JOY_4]));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::SetupAnalogReverse(void)
{
	BOOL rLHori = FALSE;
	BOOL rLVert = FALSE;
	BOOL rLAxis = FALSE;
	BOOL rRHori = FALSE;
	BOOL rRVert = FALSE;
	BOOL rRAxis = FALSE;

	rLHori = (sConfigParam[eCONFIG_LEFT_ANALOG_HORIZON] == 0) ?			FALSE : TRUE;
	rLVert = (sConfigParam[eCONFIG_LEFT_ANALOG_VERTICAL] == 0) ?		FALSE : TRUE;
	rLAxis = (sConfigParam[eCONFIG_LEFT_ANALOG_AXIS] == 0) ?			FALSE : TRUE;

	rRHori = (sConfigParam[eCONFIG_RIGHT_ANALOG_HORIZON] == 0) ?		FALSE : TRUE;
	rRVert = (sConfigParam[eCONFIG_RIGHT_ANALOG_VERTICAL] == 0) ?		FALSE : TRUE;
	rRAxis = (sConfigParam[eCONFIG_RIGHT_ANALOG_AXIS] == 0) ?			FALSE : TRUE;

	InputManager::GetInstance()->AssignAnalogReverse(rLHori, rLVert, rLAxis, rRHori, rRVert, rRAxis);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::SetupMouseSensitive(void)
{
	InputManager::GetInstance()->SetMouseSensitive(sConfigParam[eCONFIG_MOUSE_SCROLL_VAL]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
TotalInfo::GetConfigVideoSize(void)
{
	return (int)sConfigParam[eCONFIG_SCREEN_SIZE];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
float
TotalInfo::GetConfigResolutionRateFromGraphQuality(void)
{
	static const float sResolutionRateTable[3] = {
		0.75f,
		1.00f,
		1.25f,
	};
	return sResolutionRateTable[(int)sConfigParam[eCONFIG_GRAPHIC_QUALITY]];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
TotalInfo::GetConfigIsAntiAlias(void)
{
	if (sConfigParam[eCONFIG_ANTIALIAS] == 0) {
		return false;
	}
	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
TotalInfo::GetConfigIsBackgroundRun(void)
{
	if (sConfigParam[eCONFIG_BACKGROUND_RUN] == 0) {
		return false;
	}
	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
TotalInfo::GetConfigEffectQuality(void)
{
	return sConfigParam[eCONFIG_GRAPHIC_QUALITY];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
TotalInfo::SaveData(int slotID, int stageIndex, int maxHP, int nowHP, int nowWeapon, BYTE cosplayIndex, BYTE subPlaceIndex, UCHAR nowMP)
{
	APP_ASSERT_PRINT((slotID < 3),
					 "Select slotID %d is Invaid", slotID);
	
	// 時間計測の終了・更新
	TotalInfo::GetInstance()->UpdatePlayingTime();
	TotalInfo::GetInstance()->StartPlayingTime();

	tSaveWork save_work = {0};					// 各スロット毎のセーブ情報
	save_work.mSavePlaceIndex	= stageIndex;
	save_work.mSaveMaxHP		= maxHP;
	save_work.mSaveNowHP		= nowHP;

	save_work.mSaveMoney		= mNowMoney;
	memcpy(save_work.mSaveEventFlag,	mEventFlag,		sizeof(save_work.mSaveEventFlag));
	memcpy(save_work.mSaveObjectFlag,	mObjectFlag,	sizeof(save_work.mSaveObjectFlag));
	memcpy(save_work.mSaveNaviTalkFlag, mNaviTalkFlag,	sizeof(save_work.mSaveNaviTalkFlag));
	memcpy(save_work.mSaveFieldInfoFlag,mFieldInfoFlag, sizeof(save_work.mSaveFieldInfoFlag));
	memcpy(save_work.mSaveWeaponFlag,	mWeaponFlag,	sizeof(save_work.mSaveWeaponFlag) );

	save_work.mSaveNowWeapon	= nowWeapon;

	memcpy(save_work.mSaveSubWeaponNum,		mSubWeaponNum,		sizeof(save_work.mSaveSubWeaponNum) );
	memcpy(save_work.mSavePlayTime,			mTotalPlayTime,		sizeof(save_work.mSavePlayTime));
	save_work.mSaveCosplayBit = mCosplayBit;

	save_work.mSaveNowCosplayIndex	= cosplayIndex;
	save_work.mSaveColorRevision	= sIsMenuColorRevision;
	save_work.mSaveSubPlaceIndex	= subPlaceIndex;
	save_work.mSaveKeyItemBit		= mKeyItemBit;

	save_work.mSaveNowMP			= nowMP;
	save_work.mSaveSongBit			= mSongBit;

	return saveDataWork(slotID, save_work);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
TotalInfo::saveDataWork(int slotID, tSaveWork& rSaveWork, bool isConv/*=false*/)
{
	if (slotID == -1) {
		if (mPlayingSaveSlot == -1) {
			slotID = 0;
		}
		else {
			slotID = mPlayingSaveSlot;
		}
	}

	char header_name[5] = kSAVE_DATA_HEADER;	// ヘッダー
	char time_work[6];							// 日付
	char version[2];							// バージョン
	long top_offset[3];							// 各スロットの先頭オフセット
	int ret = 0;

	WORD slot_size = ( (sizeof(tSaveWork) / 32) + 1) * 32;
	top_offset[0] = 0x20;		// 適当
	top_offset[1] = top_offset[0] + slot_size;
	top_offset[2] = top_offset[1] + slot_size;
#ifdef _DEBUG
	PRINT_CONSOLE("top_offset[0]:0x%x, [1]:0x%x, [2]:0x%x. \n", top_offset[0], top_offset[1], top_offset[2]);
#endif // _DEBUG

	FILE* fp = NULL;

	tSaveWork other_save_a, other_save_b;
	BYTE set_slot_a, set_slot_b;
	int getsize = 0;

	// セーブファイルを一度ロードする
	fopen_s(&fp, saveFileName, "rb");

	/* セーブファイルがない場合 */
	if (fp == NULL) {
		set_slot_a = 0;
		if (set_slot_a == slotID) set_slot_a ++;
		set_slot_b = set_slot_a + 1;
		if (set_slot_b == slotID) set_slot_b ++;

		ClearSaveInfo(other_save_a);
		ClearSaveInfo(other_save_b);
	} 
	/* 旧セーブデータからの変換の場合は個別のセーブデータの指定をする */
	else if (isConv == true) {
		slotID = 0;
		set_slot_a = 1;
		set_slot_b = 2;
		ClearSaveInfo(other_save_a);
		ClearSaveInfo(other_save_b);

		// ファイルクローズ
		SAFE_FILE_CLOSE(fp);
	}
	/* 正規セーブデータが存在する場合はRead & Modifyでセーブファイルを更新する */
	else {
		set_slot_a = 0;

		// その他データ1.
		if (set_slot_a == slotID) set_slot_a ++;
		ret = fseek(fp, top_offset[set_slot_a], SEEK_SET); 
		getsize = fread((void *)&other_save_a, sizeof(tSaveWork), 1, fp);
		if (slot_size < getsize) {
			SYSTEM_MESSAGE("システムエラー",
						   "\n セーブデータのフォーマットが異常です。\nゲームを終了します。");
			return false;
		}
	
		set_slot_b = set_slot_a + 1;
	
		// その他データ2.
		if (set_slot_b == slotID) set_slot_b ++;
		ret = fseek(fp, top_offset[set_slot_b], SEEK_SET); 
		getsize = fread((void *)&other_save_b, sizeof(tSaveWork), 1, fp);
		if (slot_size < getsize) {
			SYSTEM_MESSAGE("システムエラー",
						   "\n セーブデータのフォーマットが異常です。\nゲームを終了します。");
			return false;
		}

		// ファイルクローズ
		SAFE_FILE_CLOSE(fp);

	} 

	/**** 書き込むバッファを保存する ****/
	fopen_s(&fp, saveFileName, "wb");
	if (fp == NULL) {
		return false;	// ファイル読み込み失敗
	}

	time_t timer;
	struct tm *t_st;
	time(&timer);/* 現在時刻の取得 */
	t_st = localtime(&timer);   /* 現在時刻を構造体に変換 */
	time_work[0] = (unsigned char)t_st->tm_year;
	time_work[1] = (unsigned char)t_st->tm_mon+1;
	time_work[2] = (unsigned char)t_st->tm_mday;
	time_work[3] = (unsigned char)t_st->tm_hour;
	time_work[4] = (unsigned char)t_st->tm_min;
	time_work[5] = (unsigned char)t_st->tm_sec;

	version[0]	= kSAVE_DATA_MAJOR_VER;
	version[1]	= kSAVE_DATA_MINOR_VER;

	// 実際にファイルに書き込み
	fwrite((void *)header_name, strlen(header_name),	1, fp);
	fwrite((void *)time_work,	sizeof(time_work),		1, fp);
	fwrite((void *)version,		sizeof(version),		1, fp);
	fwrite((void *)top_offset,	sizeof(top_offset),		1, fp);

	// slot
	ret = fseek(fp, top_offset[slotID], SEEK_SET); 
	fwrite((void *)&rSaveWork,	sizeof(rSaveWork),		1, fp);

	// slot
	ret = fseek(fp, top_offset[set_slot_a], SEEK_SET); 
	fwrite((void *)&other_save_a,	sizeof(rSaveWork),		1, fp);

	// slot
	ret = fseek(fp, top_offset[set_slot_b], SEEK_SET); 
	fwrite((void *)&other_save_b,	sizeof(rSaveWork),		1, fp);

	// ファイルクローズ
	SAFE_FILE_CLOSE(fp);

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
TotalInfo::eLoadStatus
TotalInfo::LoadDataToBuffer(tSaveWork* p3LoadWork)
{
	char header_name[5] = {0};					// ヘッダー
	char time_work[6];							// 日付
	char version[2];							// バージョン
	long top_offset[3];							// 各スロットの先頭オフセット
	//	tSaveWork save_work = {0};					// 各スロット毎のセーブ情報

	FILE* fp = NULL;
	fopen_s(&fp, saveFileName, "rb");
	if (fp == NULL) {
		return eLOAD_FAILED;	// ファイル読み込み失敗
	}

	// ファイルの長さを取得
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	int getsize;

	getsize = fread((void *)header_name,	strlen(kSAVE_DATA_HEADER),	1, fp);
	getsize = fread((void *)time_work,		sizeof(time_work),	1, fp);
	getsize = fread((void *)version,		sizeof(version),1, fp);
	getsize = fread((void *)top_offset,		sizeof(top_offset),	1, fp);

	int ret = fseek(fp, top_offset[0], SEEK_SET); 

	// ヘッダー不一致
	if (strcmp(header_name, kSAVE_DATA_HEADER) != 0) {
		SYSTEM_MESSAGE("エラー", "セーブデータが正規の物ではありません");

		// ファイルクローズ
		SAFE_FILE_CLOSE(fp);

		return eLOAD_FAILED;
	}

	// ロード情報のポインタがNULLの場合、セーブデータが存在するかのチェックを行う用途での呼び出しの為、
	// 移行のロード処理をおこなわず eLOAD_SUCEESS で処理を抜ける
	
	if (p3LoadWork == NULL) {
		// ファイルクローズ
		SAFE_FILE_CLOSE(fp);

		return eLOAD_SUCEESS;
	}

	getsize = fread((void *)&p3LoadWork[0],	sizeof(tSaveWork),		1, fp);

	// バージョン不一致の場合！
	if (!( (version[0] == kSAVE_DATA_MAJOR_VER) && (version[1] == kSAVE_DATA_MINOR_VER) ) ) {
		bool is_conv = false;
		SoundManager::GetSndComponent()->PlaySound(eSE_CANCEL);
		is_conv = SYSTEM_MESSAGE_ANS("古いセーブデータが存在します！", 
									 "古いセーブデータを本バージョンのデータに変換しますか？\n変換すると以前の状態から遊ぶ事ができます。\n(変換後のデータで旧バージョンをプレイする事はできません)");
		if (is_conv == true) {

			// ファイルクローズ
			SAFE_FILE_CLOSE(fp);

			// セーブデータを旧バージョンから現バージョンに変換
			ConvSaveData_101_to_201(p3LoadWork[0]);

			// 再ロード要求
			return eLOAD_REQUEST_RELOAD;
		}

		return eLOAD_FAILED;
	}

	ret = fseek(fp, top_offset[1], SEEK_SET); 
	getsize = fread((void *)&p3LoadWork[1],	sizeof(tSaveWork), 1, fp);

	ret = fseek(fp, top_offset[2], SEEK_SET); 
	getsize = fread((void *)&p3LoadWork[2],	sizeof(tSaveWork), 1, fp);

	// ファイルクローズ
	SAFE_FILE_CLOSE(fp);

	return eLOAD_SUCEESS;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::UpdateLoadDataFromBuffer(tSaveWork& rLoadWork)
{
	mLoadWork.mPlaceIndex		= rLoadWork.mSavePlaceIndex;
	mLoadWork.mMaxHP			= rLoadWork.mSaveMaxHP;
	mLoadWork.mNowHP			= rLoadWork.mSaveNowHP;

	mNowMoney					= rLoadWork.mSaveMoney;

	memcpy(mEventFlag,			rLoadWork.mSaveEventFlag,		sizeof(mEventFlag));
	memcpy(mObjectFlag,			rLoadWork.mSaveObjectFlag,		sizeof(mObjectFlag));
	memcpy(mNaviTalkFlag,		rLoadWork.mSaveNaviTalkFlag,	sizeof(mNaviTalkFlag));
	memcpy(mFieldInfoFlag,		rLoadWork.mSaveFieldInfoFlag,	sizeof(mFieldInfoFlag));

	//	mWeaponFlag					= rLoadWork.mSaveWeaponFlag;
	memcpy(mWeaponFlag,			rLoadWork.mSaveWeaponFlag,		sizeof(mWeaponFlag) );

	mLoadWork.mNowWeapon		= rLoadWork.mSaveNowWeapon;

	memcpy(mSubWeaponNum,		rLoadWork.mSaveSubWeaponNum,	sizeof(mSubWeaponNum) );
	memcpy(mTotalPlayTime,		rLoadWork.mSavePlayTime,		sizeof(mTotalPlayTime) );
	mCosplayBit					= rLoadWork.mSaveCosplayBit;

	mLoadWork.mNowCosplayIndex	= rLoadWork.mSaveNowCosplayIndex;
	sIsMenuColorRevision		= rLoadWork.mSaveColorRevision;	// 色彩補正有効無効フラグ
	mLoadWork.mSubPlaceIndex	= rLoadWork.mSaveSubPlaceIndex;
	mKeyItemBit					= rLoadWork.mSaveKeyItemBit;

	mLoadWork.mNowMP			= rLoadWork.mSaveNowMP;
	mSongBit					= rLoadWork.mSaveSongBit;

	// 金の表示を更新
	this->AddMoney(0);

	// 時間計測の開始
	TotalInfo::GetInstance()->StartPlayingTime();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::ClearSaveInfo(tSaveWork& rSaveWork)
{
	rSaveWork.mSavePlaceIndex	= 22;
	rSaveWork.mSaveMaxHP		= 12;
	rSaveWork.mSaveNowHP		= 12;

	rSaveWork.mSaveMoney		= 100;

	memset(rSaveWork.mSaveEventFlag,	0, sizeof(rSaveWork.mSaveEventFlag));
	memset(rSaveWork.mSaveObjectFlag,	0, sizeof(rSaveWork.mSaveObjectFlag));
	memset(rSaveWork.mSaveNaviTalkFlag,	0, sizeof(rSaveWork.mSaveNaviTalkFlag));
	memset(rSaveWork.mSaveFieldInfoFlag,0, sizeof(rSaveWork.mSaveFieldInfoFlag));
	//	rSaveWork.mSaveWeaponFlag	= 0;
	memset(rSaveWork.mSaveWeaponFlag,	0, sizeof(rSaveWork.mSaveWeaponFlag));

	rSaveWork.mSaveNowWeapon	= eITEM_KIND_NOWEAPON;

	memset(rSaveWork.mSaveSubWeaponNum,		0, sizeof(rSaveWork.mSaveSubWeaponNum));
	memset(rSaveWork.mSavePlayTime,			0, sizeof(rSaveWork.mSavePlayTime));
	rSaveWork.mSaveCosplayBit	= (1 << eWINMENU_COSPLAY_NONE);

	rSaveWork.mSaveNowCosplayIndex	= eWINMENU_COSPLAY_NONE;
	rSaveWork.mSaveColorRevision	= TRUE;
	rSaveWork.mSaveSubPlaceIndex	= 0;
	rSaveWork.mSaveKeyItemBit		= (1 << eKEYITEM_SEAL_KEY) | (1 << eKEYITEM_WALLET_A);
	
	rSaveWork.mSaveNowMP		= 100;
	rSaveWork.mSaveSongBit		= 0;
}

//---------------------------------------------------
/**
 * セーブデータを1.0.1から2.0.1のデータへ変換する
 */
//---------------------------------------------------
void
TotalInfo::ConvSaveData_101_to_201(tSaveWork& rSaveWork)
{
	tVer101_SaveWork old_save = {0};

	APP_ASSERT((sizeof(&old_save) < sizeof(rSaveWork)),
			   "Invaid SaveData size \n");

	// この状態では rSaveWork には旧データが入っているはずなので、ローカルにコピー
	memcpy(&old_save, &rSaveWork, sizeof(old_save));
	
	// 新セーブデータをクリア
	memset(&rSaveWork, 0, sizeof(rSaveWork));

	rSaveWork.mSavePlaceIndex	= old_save.mSavePlaceIndex;
	rSaveWork.mSaveMaxHP		= old_save.mSaveMaxHP;
	rSaveWork.mSaveNowHP		= old_save.mSaveNowHP;
	rSaveWork.mSaveMoney		= old_save.mSaveMoney;

	unsigned long* p_now = NULL;
	unsigned long* p_old = NULL;

	p_now = rSaveWork.mSaveEventFlag;
	p_old = old_save.mSaveEventFlag;
	p_now[eEVENT_00] = p_old[eVER101_EVENT_00];
	p_now[eEVENT_01] = p_old[eVER101_EVENT_01];
	p_now[eEVENT_08] = p_old[eVER101_EVENT_05];
	p_now[eEVENT_10] = p_old[eVER101_EVENT_10];
	p_now[eEVENT_13] = p_old[eVER101_EVENT_13];
	p_now[eEVENT_21] = p_old[eVER101_EVENT_21];
	p_now[eEVENT_22] = p_old[eVER101_EVENT_22];
	p_now[eEVENT_24] = p_old[eVER101_EVENT_24];
	p_now[eEVENT_25] = p_old[eVER101_EVENT_25];
	p_now[eEVENT_26] = p_old[eVER101_EVENT_26];
	/*p_now[eEVENT_27] = p_old[eVER101_EVENT_27];*/
	
	p_now = rSaveWork.mSaveObjectFlag;
	p_old = old_save.mSaveObjectFlag;
	p_now[eEVENT_00] = p_old[eVER101_EVENT_00];
	p_now[eEVENT_01] = p_old[eVER101_EVENT_01];
	p_now[eEVENT_08] = p_old[eVER101_EVENT_05];
	p_now[eEVENT_10] = p_old[eVER101_EVENT_10];
	p_now[eEVENT_13] = p_old[eVER101_EVENT_13];
	p_now[eEVENT_21] = p_old[eVER101_EVENT_21];
	p_now[eEVENT_22] = p_old[eVER101_EVENT_22];
	p_now[eEVENT_24] = p_old[eVER101_EVENT_24];
	p_now[eEVENT_25] = p_old[eVER101_EVENT_25];
	p_now[eEVENT_26] = p_old[eVER101_EVENT_26];
	/*p_now[eEVENT_27] = p_old[eVER101_EVENT_27];*/

	p_now = rSaveWork.mSaveNaviTalkFlag;
	p_old = old_save.mSaveNaviTalkFlag;
	p_now[eEVENT_00] = p_old[eVER101_EVENT_00];
	p_now[eEVENT_01] = p_old[eVER101_EVENT_01];
	p_now[eEVENT_08] = p_old[eVER101_EVENT_05];
	p_now[eEVENT_10] = p_old[eVER101_EVENT_10];
	p_now[eEVENT_13] = p_old[eVER101_EVENT_13];
	p_now[eEVENT_21] = p_old[eVER101_EVENT_21];
	p_now[eEVENT_22] = p_old[eVER101_EVENT_22];
	p_now[eEVENT_24] = p_old[eVER101_EVENT_24];
	p_now[eEVENT_25] = p_old[eVER101_EVENT_25];
	p_now[eEVENT_26] = p_old[eVER101_EVENT_26];
	/*p_now[eEVENT_27] = p_old[eVER101_EVENT_27];*/

	p_now = rSaveWork.mSaveFieldInfoFlag;
	p_old = old_save.mSaveFieldInfoFlag;
	p_now[eFIELD_HOME]		= p_old[eVER101_FIELD_HOME];
	p_now[eFIELD_REBERNIAN]	= p_old[eVER101_FIELD_REBERNIAN];
	p_now[eFIELD_WILDERNESS]= p_old[eVER101_FIELD_WILDERNESS];
	p_now[eFIELD_WATERWAY]	= p_old[eVER101_FIELD_WATERWAY];
	p_now[eFIELD_PRISM]		= p_old[eVER101_FIELD_PRISM];

	//	rSaveWork.mSaveWeaponFlag	= old_save.mSaveWeaponFlag;
	memcpy(&rSaveWork.mSaveWeaponFlag[0], &old_save.mSaveWeaponFlag, sizeof(old_save.mSaveWeaponFlag) );
	p_now[eFIELD_REBERNIAN]	&= ~kFIELD_INFO_XX_ARENA_MASK;


	rSaveWork.mSaveNowWeapon	= eITEM_KIND_NOWEAPON;
	memset(rSaveWork.mSaveSubWeaponNum,		sizeof(rSaveWork.mSaveSubWeaponNum), 0);
	memset(rSaveWork.mSavePlayTime,			sizeof(rSaveWork.mSavePlayTime), 0);
	rSaveWork.mSaveCosplayBit	= (1 << eWINMENU_COSPLAY_NONE);

	rSaveWork.mSaveNowCosplayIndex	= eWINMENU_COSPLAY_NONE;
	rSaveWork.mSaveColorRevision	= TRUE;		// 変換後は有効
	rSaveWork.mSaveSubPlaceIndex	= 0;

	rSaveWork.mSaveKeyItemBit		= (1 << eKEYITEM_SEAL_KEY) | (1 << eKEYITEM_WALLET_A);

	// リノの笑顔を変換
	for (int iI = 0; iI < 7; iI ++) {
		if (rSaveWork.mSaveFieldInfoFlag[eFIELD_HOME] & (1 << iI)) {
			//			TotalInfo::GetInstance()->AddKeyItem(eKEYITEM_SMILE_0 + iI);
			rSaveWork.mSaveKeyItemBit |= (1 << (eKEYITEM_SMILE_0 + iI) );
		}
	}

	rSaveWork.mSaveNowMP		= 100;
	rSaveWork.mSaveSongBit		= 0;

	// 一度ファイルを上書きする
	saveDataWork(0, rSaveWork, true);
}

/*=====================================*
 * Event flags 関連
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::SetEventFlag(unsigned long kind, unsigned long flag)
{
	APP_ASSERT_PRINT((kind < eEVENT_MAX),
					 "select event kind is invalid\n");

	mEventFlag[kind] |= EVENT_FLAG(flag);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
TotalInfo::GetEventFlagEnable(unsigned long kind, unsigned long flag)
{
	APP_ASSERT_PRINT((kind < eEVENT_MAX),
					 "select event kind is invalid\n");

	if (mEventFlag[kind] & EVENT_FLAG(flag)) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
ULONG
TotalInfo::GetEventFlagValue(unsigned long kind)
{
	APP_ASSERT_PRINT((kind < eEVENT_MAX),
					 "select event kind is invalid\n");

	return mEventFlag[kind];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::SetEventFlagValue(unsigned long kind, unsigned long value)
{
	APP_ASSERT_PRINT((kind < eEVENT_MAX),
					 "select event kind is invalid\n");

	mEventFlag[kind] = value;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::DisableEventFlag(unsigned long kind, unsigned long flag)
{
	APP_ASSERT_PRINT((kind < eEVENT_MAX),
					 "select event kind is invalid\n");

	mEventFlag[kind] &= ~EVENT_FLAG(flag);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::ResetAllEventFlag(void)
{
	memset(mEventFlag, 0x00, sizeof(mEventFlag));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::SetNaviTalkFlag(unsigned long kind, unsigned long flag)
{
	APP_ASSERT_PRINT((kind < eEVENT_MAX),
					 "select event kind is invalid\n");

	mNaviTalkFlag[kind] |= EVENT_FLAG(flag);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
TotalInfo::GetNaviTalkFlagEnable(unsigned long kind, unsigned long flag)
{
	APP_ASSERT_PRINT((kind < eEVENT_MAX),
					 "select event kind is invalid\n");

	if (mNaviTalkFlag[kind] & EVENT_FLAG(flag)) {
		return true;
	}
	return false;
}

/*=====================================*
 * Object Flags 関連
 *=====================================*/
//---------------------------------------------------
/**
   
 */
//---------------------------------------------------
void
TotalInfo::SetObjectFlagBit(unsigned long kind, unsigned long bit)
{
	APP_ASSERT_PRINT((kind < eEVENT_MAX),
					 "select event kind is invalid\n");

	mObjectFlag[kind] = bit;
	PRINT_CONSOLE("%s : mObjectFlag[%d] = 0x%08x \n", __FUNCTION__, kind, mObjectFlag[kind]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
unsigned long
TotalInfo::GetObjectFlagBit(unsigned long kind)
{
	APP_ASSERT_PRINT((kind < eEVENT_MAX),
					 "select event kind is invalid\n");

	PRINT_CONSOLE("%s : mObjectFlag[%d] = 0x%08x \n", __FUNCTION__, kind, mObjectFlag[kind]);
	return mObjectFlag[kind];
}

/*=====================================*
 * サブウェアポン　関連
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::SetWeaponFlag(eItemKind item)
{
	//	APP_ASSERT_PRINT((item < sizeof(mWeaponFlag) * 8),
	//					 "item index %d is over flow\n", item);
	APP_ASSERT_PRINT((item < (32 * kWEAPON_FLAG_BYTE) ),
					 "item index %d is over flow\n", item);

	//	mWeaponFlag |= (1i64 << item);
	mWeaponFlag[(item/32)] |= (1 << (item%32) );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::DisableWeaponFlag(eItemKind item)
{
	//	APP_ASSERT_PRINT((item < sizeof(mWeaponFlag) * 8),
	//					 "item index %d is over flow\n", item);
	APP_ASSERT_PRINT((item < (32 * kWEAPON_FLAG_BYTE) ),
					 "item index %d is over flow\n", item);

	//	mWeaponFlag &= ~(1i64 << item);
	mWeaponFlag[(item/32)] &= ~(1 << (item%32) );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
TotalInfo::IsWeaponFlag(eItemKind item)
{
	//	APP_ASSERT_PRINT((item < sizeof(mWeaponFlag) * 8),
	//					 "item index %d is over flow\n", item);
	APP_ASSERT_PRINT((item < (32 * kWEAPON_FLAG_BYTE) ),
					 "item index %d is over flow\n", item);

	//	if (mWeaponFlag & (1i64 << item)) {
	if (mWeaponFlag[(item/32)] & (1 << (item%32) ) ) {
		return true;
	}
	return false;
}

/*=====================================*
 * Item flags 関連
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::EnableItemFlag(eItemKind kind)
{
	mItemFlag |= ENUM_2_BIT(kind);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
TotalInfo::IsItemFlag(eItemKind kind)
{
	if (mItemFlag & ENUM_2_BIT(kind)) {
		return true;
	}
	return false;
}

/*=====================================*
 * フィールドごとのflags 関連
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::SetEnableFieldInfoFlag(int stageIndex, unsigned long flag)
{
	int field_index = ConvertPlaceIndex_TO_FieldInfoDefine(stageIndex);
	APP_ASSERT_PRINT(((field_index > eFIELD_ERROR) && (field_index < eFIELD_MAX)),
					 "Select stageIndex %d is Invalid \n", stageIndex);
	
	mFieldInfoFlag[field_index] |= flag;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::SetDisableFieldInfoFlag(int stageIndex, unsigned long flag)
{
	int field_index = ConvertPlaceIndex_TO_FieldInfoDefine(stageIndex);
	APP_ASSERT_PRINT(((field_index > eFIELD_ERROR) && (field_index < eFIELD_MAX)),
					 "Select stageIndex %d is Invalid \n", stageIndex);
	
	mFieldInfoFlag[field_index] &= ~flag;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
TotalInfo::GetHasFieldMap(int stageIndex)
{
	int field_index = ConvertPlaceIndex_TO_FieldInfoDefine(stageIndex);

	if (field_index != eFIELD_ERROR) {
		if (mFieldInfoFlag[field_index] & kFIELD_INFO_MAP_BIT) {
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
TotalInfo::GetHasFieldKey(int stageIndex)
{
	int field_index = ConvertPlaceIndex_TO_FieldInfoDefine(stageIndex);

	if (field_index != eFIELD_ERROR) {
		if (mFieldInfoFlag[field_index] & kFIELD_INFO_KEY_BIT) {
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
TotalInfo::GetIsFieldStartNoConvert(int fieldIndex)
{
	if (fieldIndex != eFIELD_ERROR) {
		if (mFieldInfoFlag[fieldIndex] & kFIELD_INFO_START_BIT) {
			return true;
		}
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
unsigned long
TotalInfo::GetFieldInfoFlagBit(int stageIndex)
{
	int field_index = ConvertPlaceIndex_TO_FieldInfoDefine(stageIndex);
	APP_ASSERT_PRINT(((field_index > eFIELD_ERROR) && (field_index < eFIELD_MAX)),
					 "Select stageIndex %d is Invalid \n", stageIndex);

	return mFieldInfoFlag[field_index];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::ResetFieldInfoFlag(void)
{
	memset(&mFieldInfoFlag, 0x00, sizeof(mFieldInfoFlag));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::LoadTotalInfoLayout(void)
{
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_COMPASS);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_MONEY);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_NUMBER_0);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_NUMBER_1);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_NUMBER_2);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_NUMBER_3);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_NUMBER_4);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_NUMBER_5);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_NUMBER_6);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_NUMBER_7);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_NUMBER_8);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_NUMBER_9);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_LOCKON);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_HEART_FULL);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_HEART_3DIV4);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_HEART_2DIV4);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_HEART_1DIV4);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_HEART_EMPTY);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_ICON_GAUGE_FRAME);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_ICON_GAUGE_LINE);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_PIYOPIYO);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_MAGIC_GAUGE);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_RING_FRAME);

	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_LINO_FACE_A);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_LINO_FACE_B);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_LINO_FACE_C);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_LINO_FACE_D);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_MAPARROW);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_ICON_MAP_LINO_FACE);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_BALLOON);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_BALLOON_MSG);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_ITEM_RING_MAP);
	ResourceLayout::GetInstance()->LoadLayoutResource(RES_LYT_ITEM_RING_BOSS_KEY);

	LayoutBase* p_layout = NULL;
	LayoutNumber* p_number = NULL;

	for (int iI = 0; iI < 4; iI ++) {
		p_number = &sTILayoutNumber[iI];
		for (int iJ = 0; iJ < 10; iJ ++) {
			p_number->LinkLayout(iJ, GetLinkLayoutName(iJ));
		}
		p_number->SetPositionByCentering(600 - (iI*20), 440);
		p_number->SetSize(32, 32);
		p_number->SetAlpha(0);

		p_number->PadRightPos();

		if (GraphicManager::GetInstance()->CheckEntryDrawLayout(p_number) == false) {
			GraphicManager::GetInstance()->EntryDrawLayout(p_number);
		}
	}

	// アイテム数の表示を行うレイアウト
	for (int iI = 0; iI < 2; iI ++) {
		p_number = &sTiLytItemNum[iI];
		for (int iJ = 0; iJ < 10; iJ ++) {
			p_number->LinkLayout(iJ, GetLinkLayoutName(iJ));
		}
		p_number->SetPositionByCentering(600 - (iI*16), 88);
		p_number->SetSize(20, 20);
		p_number->SetAlpha(0);

		p_number->PadRightPos();

		if (GraphicManager::GetInstance()->CheckEntrySystemLayout(p_number) == false) {
			GraphicManager::GetInstance()->EntrySystemLayout(p_number);
		}
	}

	for (int iI = 0; iI < eLY_MAX; iI ++) {
		setupLayoutSettingInfo(iI);
	}

	int lyt_index = -1;

	lyt_index = eLY_MONEY;
	//	setLayoutPosPadRight(lyt_index);
	sTILayout[lyt_index].PadRightPos();

	lyt_index = eLY_COMPASS;
	sTILayout[lyt_index].SetPosition(sLytSettingList[lyt_index].mPosX, sLytSettingList[lyt_index].mPosY);

	/* ピヨピヨゲージの枠と線 */
	lyt_index = eLY_PIYOPIYO_GAUGE_LINE;
	sTILayout[lyt_index].SetPosition(sLytSettingList[lyt_index].mPosX, sLytSettingList[lyt_index].mPosY);
	sTILayout[lyt_index].SetColor(255, 255, 0);

	lyt_index = eLY_PIYOPIYO_GAUGE_FRAME;
	sTILayout[lyt_index].SetPosition(sLytSettingList[lyt_index].mPosX, sLytSettingList[lyt_index].mPosY);

	lyt_index = eLY_PIYOPIYO_GAUGE_ICON;
	sTILayout[lyt_index].SetPosition(sLytSettingList[lyt_index].mPosX, sLytSettingList[lyt_index].mPosY);

	/* 魔法ゲージの枠と線 */
	lyt_index = eLY_MAGIC_GAUGE_LINE;
	sTILayout[lyt_index].SetPosition(sLytSettingList[lyt_index].mPosX, sLytSettingList[lyt_index].mPosY);
	sTILayout[lyt_index].SetColor(0, 255, 0);

	lyt_index = eLY_MAGIC_GAUGE_FRAME;
	sTILayout[lyt_index].SetPosition(sLytSettingList[lyt_index].mPosX, sLytSettingList[lyt_index].mPosY);

	lyt_index = eLY_MAGIC_GAUGE_ICON;
	sTILayout[lyt_index].SetPosition(sLytSettingList[lyt_index].mPosX, sLytSettingList[lyt_index].mPosY);

	lyt_index = eLY_LINO_FACE;
	sTILayout[lyt_index].SetPosition(sLytSettingList[lyt_index].mPosX, sLytSettingList[lyt_index].mPosY);

	lyt_index = eLY_SUB_WEAPON;
	sTILayout[lyt_index].PadRightPos();

	lyt_index = eLY_RING_FLAME;
	sTILayout[lyt_index].PadRightPos();

	lyt_index = eLY_GAUGE_LINE;
	sTILayout[lyt_index].SetPosition(sLytSettingList[lyt_index].mPosX, sLytSettingList[lyt_index].mPosY);

	lyt_index = eLY_GAUGE_FRAME;
	sTILayout[lyt_index].SetPosition(sLytSettingList[lyt_index].mPosX, sLytSettingList[lyt_index].mPosY);
	
	lyt_index = eLY_HAS_MAP;
	sTILayout[lyt_index].SetPosition(sLytSettingList[lyt_index].mPosX, sLytSettingList[lyt_index].mPosY);

	lyt_index = eLY_HAS_KEY;
	sTILayout[lyt_index].SetPosition(sLytSettingList[lyt_index].mPosX, sLytSettingList[lyt_index].mPosY);

	float aspect_rate = GraphicManager::GetInstance()->GetAspectRate();
	sTILayout[eLY_BALLOON].SetSize( (int)(630.0f / aspect_rate), 192);

	/* 金の表示を更新 */
	this->AddMoney(0);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::VisibleBattleIcon(void)
{
	sTILayoutNumber[0].SetAlpha(200);
	sTILayoutNumber[1].SetAlpha(200);
	sTILayoutNumber[2].SetAlpha(200);
	sTILayoutNumber[3].SetAlpha(200);
	sTILayout[eLY_COMPASS].SetPositionByCentering(sLytSettingList[eLY_COMPASS].mPosX,
												  sLytSettingList[eLY_COMPASS].mPosY); // 特別設定

	for (int iI = 0; iI < eLY_MAX; iI ++) {
		sTILayout[iI].SetAlpha(sLytSettingList[iI].mAlpha);
	}

	eItemKind item_kind = eITEM_KIND_NONE;
	CharaPlayer* p_player = NULL;
	p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();


	//	APP_ASSERT((p_player != NULL), "Failed get GetAttackCollisionPlayerAddress()\n");
	// サブウェポンの指定武器情報を取得
	if (p_player != NULL) {
		item_kind = p_player->ItemWork().mItemSubWeapon;
	}
	if ( (item_kind == eITEM_KIND_BOMB)		||
		 (item_kind == eITEM_KIND_ARCHERY) ) {
		sTiLytItemNum[0].SetAlpha(200);
		sTiLytItemNum[1].SetAlpha(200);
	} else {
		sTiLytItemNum[0].SetAlpha(0);
		sTiLytItemNum[1].SetAlpha(0);
	}

	int stage_index = SceneMainGame::GetStageIndex();

	// 現在のフィールドのマップを持っている場合はマップを表示
	if (TotalInfo::GetInstance()->GetHasFieldMap(stage_index)) {
		sTILayout[eLY_HAS_MAP].SetAlpha(220);
	}
	// 現在のフィールドの大きなを持っている場合は大きな鍵を表示
	if (TotalInfo::GetInstance()->GetHasFieldKey(stage_index)) {
		sTILayout[eLY_HAS_KEY].SetAlpha(220);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::HideBattleIcon(void)
{
	for (int iI = 0; iI < eLY_MAX; iI ++) {
		sTILayout[iI].SetAlpha(0);
	}

	sTILayoutNumber[0].SetAlpha(0);
	sTILayoutNumber[1].SetAlpha(0);
	sTILayoutNumber[2].SetAlpha(0);
	sTILayoutNumber[3].SetAlpha(0);

	sTiLytItemNum[0].SetAlpha(0);
	sTiLytItemNum[1].SetAlpha(0);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::UpdateCompass(void)
{
	VECTOR vec;
	vec = VSub(CameraManager::GetInstance()->Target(), CameraManager::GetInstance()->Position());
	float cam_angle = atan2(vec.x, vec.z);
	sTILayout[eLY_COMPASS].Rotation() = -cam_angle;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::CrazyCompass(void)
{
	sTILayout[eLY_COMPASS].Rotation() += 0.5f;
	Utility::CorrectAngleFrom000To360(&sTILayout[eLY_COMPASS].Rotation());
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::SetCommapssForShowMap(void)
{
	sTILayout[eLY_COMPASS].SetPositionByCentering(64, 64);
	sTILayout[eLY_COMPASS].Rotation() = 0.0f;
	sTILayout[eLY_COMPASS].SetAlpha(255);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::EnableLockonLayout(VECTOR* pWorldPos, BOOL isHighlight/*=TRUE*/)
{
	float aspect_rate = GraphicManager::GetInstance()->GetAspectRate();
	VECTOR screen_pos;
	LayoutBase* p_layout = &sTILayout[eLY_LOCKON];
	screen_pos = ConvWorldPosToScreenPos(*pWorldPos);
	screen_pos.x /= GraphicManager::GetInstance()->GetLayoutScaleRateX();
	screen_pos.y /= GraphicManager::GetInstance()->GetLayoutScaleRateY();

	p_layout->SetPositionByCentering((int)(screen_pos.x * aspect_rate), (int)screen_pos.y);
	p_layout->SetAlpha(128);
	if (isHighlight) {
		p_layout->SetColor(255, 255, 0);
	}
	else {
		p_layout->SetColor(0, 255, 255);
	}

	if (GraphicManager::GetInstance()->CheckEntryDrawLayout(p_layout) == false) {
		GraphicManager::GetInstance()->EntryDrawLayout(p_layout);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::DisableLockonLayout(void)
{
	LayoutBase* p_layout = &sTILayout[eLY_LOCKON];
	if (GraphicManager::GetInstance()->CheckEntryDrawLayout(p_layout) == true) {
		GraphicManager::GetInstance()->RemoveDrawLayout(p_layout);
	}

//	FADE_BOARD()->StartFade(0, 0.2f);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void DrawEnemyNameFontFunc(void)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	if (p_player == NULL) return;

	ModelCollision* p_lockon = p_player->GetLockonChara();
	if (p_lockon != NULL) {
		char* set_str = p_lockon->GetModelName();

		ChangeFont("HG丸ｺﾞｼｯｸM-PRO");
		GraphicManager::GetInstance()->DrawFontString(36, 408, set_str, GetColor(0, 0, 0), 30);
		GraphicManager::GetInstance()->DrawFontString(34, 406, set_str, GetColor(255, 255, 255), 30);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::SetActiveLockonEnemyName(BOOL isActive)
{
	if (isActive) {
		GraphicManager::GetInstance()->EntryLockonFontFunc(DrawEnemyNameFontFunc);
	}
	else {
		GraphicManager::GetInstance()->EntryLockonFontFunc(NULL);
	}
}

static const int sHeartInterval = 22;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::UpdateHeartLayout(int maxHP, int nowHP)
{
	int full_heart_num = ((nowHP - 1)/ 4);
	int half_heart_value = nowHP % 4;
	int empty_heart_num = (((maxHP - nowHP - 1) / 4) + 1);

	int sTmpPosX = 118;

	sTILayout[eLY_HEART_00].SetPosition(sTmpPosX, 32);
	sTILayout[eLY_HEART_00].SettingContinuDraw(full_heart_num , sHeartInterval, 0);
	sTILayout[eLY_HEART_01].SettingContinuDraw(empty_heart_num, sHeartInterval, 0);
	sTILayout[eLY_HEART_01].SetPosition(sTmpPosX + (sHeartInterval * ( (maxHP/4) - empty_heart_num )), 32);
	sTILayout[eLY_HEART_NOW].LinkLayout(sHalfHeartLayoutName[half_heart_value]);
	sTILayout[eLY_HEART_NOW].SetPosition(sTmpPosX + (sHeartInterval * full_heart_num ), 32);
	sTILayout[eLY_HEART_NOW].SettingContinuDraw((nowHP == 0) ? 0 : 1 , sHeartInterval, 0); 
	static float sHeartCount = 0;
	sHeartCount += 0.1f;
	float size = (sin(sHeartCount) * 8.0f) + 32.0f;
	sTILayout[eLY_HEART_NOW].SetSize((int)size, (int)size);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::ShowHeartLayout(int maxHP, int nowHP, BOOL isTitle/*=FALSE*/)
{
	int sTmpPosX;
	if (isTitle == TRUE) {
		if (GraphicManager::GetInstance()->IsWideScreen()) {
			sTmpPosX = 177;
		}
		else {
			sTmpPosX = 72;
		}
	}
	else {
		sTmpPosX = 118;
	}

	int sTmpPosY = 156;

	// ※ SceneSelectFile::fadeInLeftPageLayout() が参照
	sTILayout[eLY_HEART_00].SetAlpha(200);
	sTILayout[eLY_HEART_01].SetAlpha(128);
	sTILayout[eLY_HEART_NOW].SetAlpha(200);

	int full_heart_num = ((nowHP - 1)/ 4);
	int half_heart_value = nowHP % 4;
	int empty_heart_num = (((maxHP - nowHP - 1) / 4) + 1);
	sTILayout[eLY_HEART_00].SetPosition(sTmpPosX, sTmpPosY);
	sTILayout[eLY_HEART_00].SettingContinuDraw(full_heart_num , sHeartInterval, 0);
	sTILayout[eLY_HEART_01].SettingContinuDraw(empty_heart_num, sHeartInterval, 0);
	sTILayout[eLY_HEART_01].SetPosition(sTmpPosX + (sHeartInterval * ( (maxHP/4) - empty_heart_num )), sTmpPosY);
	sTILayout[eLY_HEART_NOW].LinkLayout(sHalfHeartLayoutName[half_heart_value]);
	sTILayout[eLY_HEART_NOW].SetPosition(sTmpPosX + (sHeartInterval * full_heart_num ), sTmpPosY);
	sTILayout[eLY_HEART_NOW].SettingContinuDraw((nowHP == 0) ? 0 : 1 , sHeartInterval, 0); 
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
LayoutBase*
TotalInfo::GetSystemLayout(int index)
{
	APP_ASSERT_PRINT((index < eLY_MAX),
					 "Select index %d is Invalid value \n");
	
	return &sTILayout[index];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
const char*
TotalInfo::GetLinkLayoutName(int numIndex)
{
	APP_ASSERT_PRINT((numIndex < 10),
					 "Select numIndex : %d \n", numIndex);

	static const char* strLinkLayoutTable[10] = {
		RES_LYT_NUMBER_0,
		RES_LYT_NUMBER_1,
		RES_LYT_NUMBER_2,
		RES_LYT_NUMBER_3,
		RES_LYT_NUMBER_4,
		RES_LYT_NUMBER_5,
		RES_LYT_NUMBER_6,
		RES_LYT_NUMBER_7,
		RES_LYT_NUMBER_8,
		RES_LYT_NUMBER_9,
	};

	return strLinkLayoutTable[numIndex];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::UpdatePiyoPiyoGaugeByRate(float rate, float magicRate)
{
	float aspect_rate = GraphicManager::GetInstance()->GetAspectRate();

	{
		const tLayoutSetting* p_setting = &sLytSettingList[eLY_PIYOPIYO_GAUGE_LINE];

		int lyt_size_x, lyt_pos_x;
	
		lyt_size_x = (int)((float)(p_setting->mSizeX * rate));
		sTILayout[eLY_PIYOPIYO_GAUGE_LINE].SetSize(lyt_size_x, p_setting->mSizeY);
	
		lyt_pos_x = p_setting->mPosX - (int)(float)((p_setting->mSizeX - lyt_size_x) / 2) ;
		sTILayout[eLY_PIYOPIYO_GAUGE_LINE].SetPosition(lyt_pos_x, p_setting->mPosY);
	}

	{
		const tLayoutSetting* p_setting = &sLytSettingList[eLY_MAGIC_GAUGE_LINE];

		int lyt_size_x, lyt_pos_x;
	
		lyt_size_x = (int)((float)(p_setting->mSizeX * magicRate));
		sTILayout[eLY_MAGIC_GAUGE_LINE].SetSize(lyt_size_x, p_setting->mSizeY);
	
		lyt_pos_x = p_setting->mPosX - (int)((float)((p_setting->mSizeX - lyt_size_x) / 2) );
		sTILayout[eLY_MAGIC_GAUGE_LINE].SetPosition(lyt_pos_x, p_setting->mPosY);
		
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::FadeinLockonEnemyLayout(int gaugeSize)
{
	float aspect_rate = GraphicManager::GetInstance()->GetAspectRate();

	const tLayoutSetting* p_setting = &sLytSettingList[eLY_GAUGE_LINE];

	LayoutBase* p_layout;
	//	p_layout = &sTILayout[eLY_ENEMY_FACE];
	//	p_layout->LinkLayout(pStrLayoutName);
	//	p_layout->StartFade(255, 0.0f);

	p_layout = &sTILayout[eLY_GAUGE_FRAME];
	p_layout->StartFade(255, 0.0f);

	p_layout = &sTILayout[eLY_GAUGE_LINE];
	p_layout->StartFade(255, 0.0f);
	p_layout->SetSize(gaugeSize, p_setting->mSizeY);
	p_layout->SetPosition(p_setting->mPosX - (int)(float)((p_setting->mSizeX - gaugeSize) / 2),
						  p_setting->mPosY);

//	GraphicManager::GetInstance()->EntryLockonFontFunc(DrawEnemyNameFontFunc);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::FadeoutLockonEnemyLayout(void)
{
	sTILayout[eLY_GAUGE_FRAME].StartFade(0, 0.1f);
	sTILayout[eLY_GAUGE_LINE].StartFade(0, 0.1f);
	//	sTILayout[eLY_ENEMY_FACE].StartFade(0, 0.1f);

//	GraphicManager::GetInstance()->EntryLockonFontFunc(NULL);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::ChangePlayerFaceLayout(bool isEncount)
{
	CharaPlayer* p_player = (CharaPlayer *)CollisionManager::GetInstance()->GetAttackCollisionPlayerAddress();
	
	float hp_rate = (float)p_player->GetHitpoint() / (float)p_player->GetMaxHitpoint();

	if (p_player->IsDamage()) {
		sTILayout[eLY_LINO_FACE].LinkLayout("LinoFaceC.png");
	}
	else if (hp_rate < 0.35f) {
		static int sDangerCnt = 0;
		sDangerCnt ++;
		if (sDangerCnt > 50) {
			SoundManager::GetSndComponent()->PlaySound(eSE_DANGER);
			sDangerCnt = 0;
		}
		sTILayout[eLY_LINO_FACE].LinkLayout("LinoFaceD.png");
	}
	else if (isEncount) {
		sTILayout[eLY_LINO_FACE].LinkLayout("LinoFaceB.png");
	}
	else {
		sTILayout[eLY_LINO_FACE].LinkLayout("LinoFaceA.png");
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::MsgTypeTelop(void)
{
	sTILayout[eLY_BALLOON].LinkLayout("Balloon.png");
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::MsgTypeMessage(void)
{
	sTILayout[eLY_BALLOON].LinkLayout("BalloonMsg.png");
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::MsgFadeIn(float sec/*=0.3f*/)
{
	sTILayout[eLY_BALLOON].StartFade(186, sec);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::MsgFadeOut(float sec/*=0.3f*/)
{
	sTILayout[eLY_BALLOON].StartFade(0, sec);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
TotalInfo::MsgIsFade(void)
{
	return sTILayout[eLY_BALLOON].IsFade();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::setupLayoutSettingInfo(int lytIndex)
{
	LayoutBase* p_layout = NULL;

	const tLayoutSetting* p_setting = &sLytSettingList[lytIndex];
	
	p_layout = &sTILayout[lytIndex];

	if (strcmp(p_setting->mStrLayoutName, "") != 0) {
		p_layout->LinkLayout(p_setting->mStrLayoutName);
	}
	p_layout->SetPositionByCentering(p_setting->mPosX, p_setting->mPosY);
	p_layout->SetSize(p_setting->mSizeX, p_setting->mSizeY);
	p_layout->SetAlpha(p_setting->mAlpha);
	if (GraphicManager::GetInstance()->CheckEntryDrawLayout(p_layout) == false) {
		GraphicManager::GetInstance()->EntryDrawLayout(p_layout);
	}
}

//---------------------------------------------------
/**
 * レイアウトをサイズを加味して、右詰めにする
 */
//---------------------------------------------------
void
TotalInfo::setLayoutPosPadRight(int lytIndex)
{
	int wnd_right_pos;
	wnd_right_pos = (int)(640.0f / GraphicManager::GetInstance()->GetAspectRate());

	int diff_pos = 640 - (sLytSettingList[lytIndex].mPosX + (sLytSettingList[lytIndex].mSizeX / 2) );
	sTILayout[lytIndex].SetPosition(wnd_right_pos - diff_pos - (sLytSettingList[lytIndex].mSizeX / 2),
									sLytSettingList[lytIndex].mPosY);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
TotalInfo::GetNowMoney(void)
{
	return mNowMoney;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::AddMoney(int value)
{
	mNowMoney += value;
	if (mNowMoney < 0) mNowMoney = 0;

	USHORT wallet_max = 100;
	if (mKeyItemBit & (1 << eKEYITEM_WALLET_C)) {
		wallet_max = 9999;
	}
	else if (mKeyItemBit & (1 << eKEYITEM_WALLET_B)) {
		wallet_max = 1100;
	}
	else if (mKeyItemBit & (1 << eKEYITEM_WALLET_A)) {
		wallet_max = 600;
	}

	if (mNowMoney > wallet_max) mNowMoney = wallet_max;

	sTILayoutNumber[0].SetNumber((mNowMoney / 1)	% 10);
	sTILayoutNumber[1].SetNumber((mNowMoney / 10)	% 10);
	sTILayoutNumber[2].SetNumber((mNowMoney / 100)	% 10);
	sTILayoutNumber[3].SetNumber((mNowMoney / 1000) % 10);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::AddItemNum(eSubweaponNum subWeaponNum, int value, bool isUpdateDraw/*=true*/)
{
	UCHAR* p_item_num = NULL;

	APP_ASSERT_PRINT( (subWeaponNum < eSUBWEAPON_NUM_MAX),
					  "Select subWeaponNum %d is Invalid value \n", subWeaponNum);
	p_item_num = &mSubWeaponNum[subWeaponNum];

	*p_item_num += value;
	// 下限
	if (*p_item_num <= 0)  *p_item_num = 0;

	switch (subWeaponNum) {
	case eSUBWEAPON_NUM_ARROW:
		if (*p_item_num >= 50) *p_item_num = 50;
		break;
	case eSUBWEAPON_NUM_BOMB:
		if (*p_item_num >= 10) *p_item_num = 10;
		break;
	case eSUBWEAPON_NUM_HEART_PIECE:
		//		if (*p_item_num >= 4) *p_item_num = 4;
		// 特に制限を設けない
		break;

	case eSUBWEAPON_NUM_BIN_EMPTY:
	case eSUBWEAPON_NUM_BIN_MAGENTA:
	case eSUBWEAPON_NUM_BIN_CYAN:
	case eSUBWEAPON_NUM_BIN_YELLOW:
	case eSUBWEAPON_NUM_BIN_FAILY:
		if (*p_item_num >= 4) *p_item_num = 4;
		break;

	default:
		APP_ASSERT_PRINT((subWeaponNum < eSUBWEAPON_NUM_MAX),
						 "Invalid subWeaponNum index %d \n", subWeaponNum);
		break;
	}
	
	if (isUpdateDraw) {
		sTiLytItemNum[0].SetNumber((*p_item_num / 1) % 10);
		sTiLytItemNum[1].SetNumber((*p_item_num / 10) % 10);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
UCHAR
TotalInfo::GetItemNum(eSubweaponNum subWeaponNum)
{
	return mSubWeaponNum[subWeaponNum];
}

/*=====================================*
 * ウィンドウメニュー関連
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::SetupWinMenu(void)
{
	SetUseKeyAccelFlag(FALSE);

	// メニューが選択された場合のコールバック関数を登録
	SetMenuItemSelectCallBackFunction(CallbackMenuSelect);

	tWinMenuInfo* p_info = NULL;
	char* p_rootname = NULL;
	for (BYTE iI = 0; iI < eWINMENU_MAX; iI ++) {
		p_info = (tWinMenuInfo *)&sWinMenuInfo[iI];

		if (p_info->mRootMenuIndex == -1) {
			p_rootname = NULL;
		} else {
			p_rootname = (char *)sWinMenuInfo[p_info->mRootMenuIndex].mMenuName;
		}
		AddMenuItem_Name(p_rootname, p_info->mMenuName);
	}

	//<! ロード状況により、シャドウマップの初期値を設定する(予定！)
	SetMenuItemMark_ID(eWINMENU_SHADOW_CIRCLE,				MENUITEM_MARK_RADIO);
	SetMenuItemMark_ID(eWINMENU_SHADOW_SHADOWMAP_MODEL,		MENUITEM_MARK_NONE);

	// シャドウマップ未使用設定
	ShadowMap::SetShadowMapType(ShadowMap::eSHADOWMAP_TYPE_NONE);
	Shadow::EnableShadowMap(FALSE);

	SetUseMenuFlag(TRUE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::SetEnableWinMenu(BOOL isEnable)
{
	tWinMenuInfo* p_info = NULL;
	char* p_rootname = NULL;
	for (BYTE iI = 0; iI < eWINMENU_MAX; iI ++) {
		p_info = (tWinMenuInfo *)&sWinMenuInfo[iI];

		if (p_info->mRootMenuIndex == -1) {
			p_rootname = NULL;
		} else {
			p_rootname = (char *)sWinMenuInfo[p_info->mRootMenuIndex].mMenuName;
		}
		SetMenuItemEnable_Name(p_rootname, isEnable);
	}
}

//---------------------------------------------------
/**
 * 色彩補正有効・無効フラグ (ただし、メニュー設定が優先)
 */
//---------------------------------------------------
void
TotalInfo::SetEnableColorRevision(BOOL isEnable)
{
	// 引数設定が無効、もしくはメニュー設定が無効の場合
	if ((isEnable == FALSE) || (sIsMenuColorRevision == FALSE)) {

		// 色彩補正無効
		//		GraphicManager::GetInstance()->SetDefaultDirectionLight();

		RenderBloom::SetActiveBloom(FALSE);

		SetMenuItemMark_ID(eWINMENU_COLOR_REVISION_DISABLE,		MENUITEM_MARK_RADIO);
		SetMenuItemMark_ID(eWINMENU_COLOR_REVISION_ENABLE,		MENUITEM_MARK_NONE);
	}
	else {
		RenderBloom::SetActiveBloom(TRUE);

		SetMenuItemMark_ID(eWINMENU_COLOR_REVISION_DISABLE,		MENUITEM_MARK_NONE);
		SetMenuItemMark_ID(eWINMENU_COLOR_REVISION_ENABLE,		MENUITEM_MARK_RADIO);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::StartPlayingTime(void)
{
	time(&mPlayStartTime);	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::UpdatePlayingTime(void)
{
#define CONV_HMS_TO_SECOND(hour, min, sec)	((hour * 3600) + (min * 60) + sec)

	// 終了時間の取得
	time_t end_time;
	time(&end_time);

	// どちらも一度秒に変換する
	ULONG start_sec, end_sec, play_sec;
	struct tm* p_time_object;
	int day_start, day_end;

	PRINT_CONSOLE("%s: UpdatePlayingTime() * \n", __FUNCTION__);

	p_time_object= localtime(&mPlayStartTime);
	day_start = p_time_object->tm_mday;
	start_sec	= CONV_HMS_TO_SECOND(p_time_object->tm_hour, p_time_object->tm_min, p_time_object->tm_sec);
	PRINT_CONSOLE("%s: start sec : %d, (%d:%d:%d)\n", __FUNCTION__, start_sec, p_time_object->tm_hour, p_time_object->tm_min, p_time_object->tm_sec);

	p_time_object	= localtime(&end_time);
	day_end = p_time_object->tm_mday;
	if (day_end != day_start) p_time_object->tm_hour += 24;
	end_sec		= CONV_HMS_TO_SECOND(p_time_object->tm_hour, p_time_object->tm_min, p_time_object->tm_sec);
	PRINT_CONSOLE("%s: end sec : %d, (%d:%d:%d)\n", __FUNCTION__, end_sec, p_time_object->tm_hour, p_time_object->tm_min, p_time_object->tm_sec);
	
	play_sec = (end_sec - start_sec);
	PRINT_CONSOLE("%s: play_sec %d = (%d - %d) \n", __FUNCTION__, play_sec, end_sec, start_sec);
	
	ULONG total_sec;
	total_sec	= CONV_HMS_TO_SECOND(mTotalPlayTime[eTIME_KIND_HOUR],
									 mTotalPlayTime[eTIME_KIND_MIN],
									 mTotalPlayTime[eTIME_KIND_SEC]);
	PRINT_CONSOLE("%s: total sec : %d, (%d:%d:%d) : before \n", __FUNCTION__, total_sec, mTotalPlayTime[eTIME_KIND_HOUR], mTotalPlayTime[eTIME_KIND_MIN], mTotalPlayTime[eTIME_KIND_SEC]);
	
	total_sec += play_sec;

	mTotalPlayTime[eTIME_KIND_SEC]	= (total_sec / 1)   % 60;
	mTotalPlayTime[eTIME_KIND_MIN]	= (total_sec / 60)	% 60;
	mTotalPlayTime[eTIME_KIND_HOUR] = (UCHAR)(total_sec / 3600);
	PRINT_CONSOLE("%s: total sec : %d, (%d:%d:%d) : after_time \n", __FUNCTION__, total_sec, mTotalPlayTime[eTIME_KIND_HOUR], mTotalPlayTime[eTIME_KIND_MIN], mTotalPlayTime[eTIME_KIND_SEC]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
BYTE
TotalInfo::GetCosplayNum(void)
{
	BYTE cosplay_cnt = 0;

	for (int iI = 0; iI < eWINMENU_COSPLAY_MAX; iI ++) {
		if (mCosplayBit & (1 << iI) ) {
			cosplay_cnt ++;
		}
	}
	return cosplay_cnt;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::AddCosplay(int cosplayIndex)
{
	mCosplayBit |= (1 << cosplayIndex);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
TotalInfo::tWinMenuCosplayInfo*
TotalInfo::GetCosplayInfo(int cosplayKind)
{
	APP_ASSERT_PRINT((cosplayKind < eWINMENU_COSPLAY_MAX),
					 "Ivalid cosplayKind : %d \n", cosplayKind);
	return (TotalInfo::tWinMenuCosplayInfo *)&sWinMenuCosplayInfoName[cosplayKind];
}

/*=====================================*
 * キーアイテム関連
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
DWORD
TotalInfo::GetKeyItemBit(void)
{
	return mKeyItemBit;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::AddKeyItem(USHORT keyItemIndex)
{
#ifdef _DEBUG
	APP_ASSERT_PRINT( ((sizeof(mKeyItemBit) * 8) > keyItemIndex),
					  "setting bit mKeyItemBit is over flow.\n");
#endif // _DEBUG
	mKeyItemBit |= (1 << keyItemIndex);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
BOOL
TotalInfo::IsKeyItem(USHORT keyItemIndex)
{
#ifdef _DEBUG
	APP_ASSERT_PRINT( ((sizeof(mKeyItemBit) * 8) > keyItemIndex),
					  "setting bit mKeyItemBit is over flow.\n");
#endif // _DEBUG
	return (mKeyItemBit & (1 << keyItemIndex) ) ? TRUE : FALSE;
}

/*=====================================*
 * 唄(オカリナ)関連
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
TotalInfo::AddSongType(UCHAR songType)
{
#ifdef _DEBUG
	APP_ASSERT( ((sizeof(mSongBit) * 8) > songType),
				"setting bit mSongBit is over flow.\n");
#endif // _DEBUG
	mSongBit |= (1 << songType);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
BOOL
TotalInfo::IsSongType(UCHAR songType)
{
#ifdef _DEBUG
	APP_ASSERT( ((sizeof(mSongBit) * 8) > songType),
				"setting bit mSongBit is over flow.\n");
#endif // _DEBUG
	return (mSongBit & (1 << songType) ) ? TRUE : FALSE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
TotalInfo::TotalInfo()
{
	memset(&mLoadWork, 0x00, sizeof(mLoadWork));

	memset(mEventFlag,	0x00, sizeof(mEventFlag));
	memset(mObjectFlag, 0x00, sizeof(mObjectFlag));
	memset(mNaviTalkFlag, 0x00, sizeof(mNaviTalkFlag));
	memset(&mItemFlag,	0x00, sizeof(mItemFlag));
	memset(&mFieldInfoFlag, 0x00, sizeof(mFieldInfoFlag));
	//	memset(&mWeaponFlag, 0x00, sizeof(mWeaponFlag));
	memset(mWeaponFlag, 0x00, sizeof(mWeaponFlag));

	// サブウェポン数の指定
	mSubWeaponNum[eSUBWEAPON_NUM_BOMB]	= 10;
	mSubWeaponNum[eSUBWEAPON_NUM_ARROW] = 50;
	mSubWeaponNum[eSUBWEAPON_NUM_HEART_PIECE] = 0;

	mSubWeaponNum[eSUBWEAPON_NUM_BIN_EMPTY]		= 0;
	mSubWeaponNum[eSUBWEAPON_NUM_BIN_MAGENTA]	= 0;
	mSubWeaponNum[eSUBWEAPON_NUM_BIN_CYAN]		= 0;
	mSubWeaponNum[eSUBWEAPON_NUM_BIN_YELLOW]	= 0;
	mSubWeaponNum[eSUBWEAPON_NUM_BIN_FAILY]		= 0;

	memset(&mTotalPlayTime, 0, sizeof(mTotalPlayTime));
	memset(&mPlayStartTime, 0, sizeof(mPlayStartTime));
	mCosplayBit	= (1 << eWINMENU_COSPLAY_NONE);
	mKeyItemBit = 0;

	mNowMoney = 100;

	mMusicVolumeScale = 1.0f;
	mSoundVolumeScale = 1.0f;
	mVoiceVolumeScale = 1.0f;

	mIsVisibleFPS = FALSE;

	mPlayingSaveSlot = -1;

#ifdef _DEBUG
	sIsMenuColorRevision	= FALSE;		// 色彩補正無効
#else
	sIsMenuColorRevision	= TRUE;		// 色彩補正無効
#endif // _DEBUG
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
TotalInfo::~TotalInfo()
{
	for (int iI = 0; iI < eLY_MAX; iI ++) {
		GraphicManager::GetInstance()->RemoveSystemLayout(&sTILayout[iI]);
	}
}

//---------------------------------------------------
/**
   ウィンドウメニュー更新通知コールバック関数
 */
//---------------------------------------------------
void CallbackMenuSelect(const TCHAR *pItemName, int nItemID)
{
	PRINT_CONSOLE("nItemID : %d \n", nItemID);

	switch (nItemID) {

	case eWINMENU_KEYCONFIG_KEYCONFIG:
		// Setting.exeを実行させる(同期処理)
		SoundManager::GetSndComponent()->PlaySound(eSE_ITEM_APPEAR);
		Basic::CreateProcessSync("Setting.exe -p");

		// Setting.exe から復帰
		InputManager::GetInstance()->Refresh();
		SoundManager::GetSndComponent()->PlaySound(eSE_CANCEL);

		// Config情報の再設定
		TotalInfo::GetInstance()->LoadConfigData();
		TotalInfo::GetInstance()->SetupAssignKeyboard();
		TotalInfo::GetInstance()->SetupAssignJoyInput();
		TotalInfo::GetInstance()->SetupAnalogReverse();
		TotalInfo::GetInstance()->SetupMouseSensitive();

		ShowCursor(FALSE);
		break;

	case eWINMENU_SHADOW_CIRCLE:
		SetMenuItemMark_ID(eWINMENU_SHADOW_CIRCLE,				MENUITEM_MARK_RADIO);
		SetMenuItemMark_ID(eWINMENU_SHADOW_SHADOWMAP_MODEL,		MENUITEM_MARK_NONE);

		// シャドウマップ未使用設定
		ShadowMap::SetShadowMapType(ShadowMap::eSHADOWMAP_TYPE_NONE);
		Shadow::EnableShadowMap(FALSE);
		break;

	case eWINMENU_SHADOW_SHADOWMAP_MODEL:
		SetMenuItemMark_ID(eWINMENU_SHADOW_CIRCLE,				MENUITEM_MARK_NONE);
		SetMenuItemMark_ID(eWINMENU_SHADOW_SHADOWMAP_MODEL,		MENUITEM_MARK_RADIO);

		// シャドウマップモデルのみ適用
		ShadowMap::SetShadowMapType(ShadowMap::eSHADOWMAP_TYPE_CHARA_ONLY);
		Shadow::EnableShadowMap(TRUE);
		break;

	case eWINMENU_COLOR_REVISION_DISABLE:
		SetMenuItemMark_ID(eWINMENU_COLOR_REVISION_DISABLE,		MENUITEM_MARK_RADIO);
		SetMenuItemMark_ID(eWINMENU_COLOR_REVISION_ENABLE,		MENUITEM_MARK_NONE);

		// ブルームの表示としてはオフにする
		sIsMenuColorRevision = FALSE;
		TotalInfo::GetInstance()->SetEnableColorRevision(TRUE);	// メニュー設定はゲーム時のみ有効なのでTRUEに
		break;

	case eWINMENU_COLOR_REVISION_ENABLE:
		SetMenuItemMark_ID(eWINMENU_COLOR_REVISION_DISABLE,		MENUITEM_MARK_NONE);
		SetMenuItemMark_ID(eWINMENU_COLOR_REVISION_ENABLE,		MENUITEM_MARK_RADIO);

		// デフォルトのライト
		sIsMenuColorRevision = TRUE;
		TotalInfo::GetInstance()->SetEnableColorRevision(TRUE);	// メニュー設定はゲーム時のみ有効なのでTRUEに
		break;

	case eWINMENU_MENU_TO_TITLE:
		ShowCursor(TRUE);
		if (SYSTEM_MESSAGE_ANS("タイトル画面に戻ります", "本当にタイトル画面に戻りますか？")) {
			SceneBase::EndScene();
		}
		ShowCursor(FALSE);

		break;

	case eWINMENU_MENU_EXIT:
		ShowCursor(TRUE);
		if (SYSTEM_MESSAGE_ANS("ゲームを終了します", "本当にゲームを終了しますか？")) {
			InputManager::GetInstance()->EnablePressKey(eKEY_ESCAPE);
		}
		ShowCursor(FALSE);

		break;

	case eWINMENU_VISIBLE_FPS:
		TotalInfo::GetInstance()->IsVisibleFPS() ^= TRUE;
		if (TotalInfo::GetInstance()->IsVisibleFPS() == FALSE) {
			SetMainWindowText(GAME_TITLE);
			SetMenuItemMark_ID(eWINMENU_VISIBLE_FPS, MENUITEM_MARK_NONE);
		} else {
			SetMenuItemMark_ID(eWINMENU_VISIBLE_FPS, MENUITEM_MARK_CHECK);
		}

		break;

	default:
		break;

	};
}

 /**** end of file ****/

