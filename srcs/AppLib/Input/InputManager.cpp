/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 入力統括マネージャクラス
 * @author SPATZ.
 * @data   2011/11/21 19:41:12
 */
//---------------------------------------------------
#include "InputInfo.h"
#include "InputManager.h"

#include "DxLib.h"

#include "AppLib/Basic/Basic.h"

static unsigned char sPrevPressKey[eKEY_MAX] = {0};
static unsigned char sTrigKey[eKEY_MAX] = {0};
static unsigned char sPressKey[eKEY_MAX] = {0};

static unsigned int sPrevPressJoy = 0;
static unsigned int sNowPressJoy = 0;

static ULONG sNowTrigJoy = 0;

static int sPrevPressMouse = 0;
static int sNowPressMouse = 0;

static int sNowMousePointX = 0;
static int sNowMousePointY = 0;

static int sPrevMousePointX = 0;
static int sPrevMousePointY = 0;

static int sMoveMousePointX = 0;
static int sMoveMousePointY = 0;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/InputManager * const
InputManager::GetInstance(void)
{
	static InputManager sInstance;
	return &sInstance;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
InputManager::InputManager()
{
	Refresh();

	memset(&mJoyInput[0], 0, sizeof(mJoyInput));
	memset(&mKeyInput[0], 0, sizeof(mKeyInput));

	mAnalogConfigSetting = 0x00;
	mAnalogRightHoriRate = 1;
	mAnalogRightVertRate = 1;

	mMoveMouseSensitive	= 50;	// 平均値
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
InputManager::~InputManager()
{
	//	this->StopVibe();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
InputManager::Create(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
InputManager::Setup(void)
{
	// ALTキーの停止を無効に設定
	SetSysCommandOffFlag(TRUE);

	// 何故か不思議だが、一度カーソルを非表示にする
	ShowCursor(FALSE);

	// パッド入力
	mJoyInput[JOY_INPUT_UP]				=	kJOY_UP;
	mJoyInput[JOY_INPUT_DOWN]			=	kJOY_DOWN;
	mJoyInput[JOY_INPUT_RIGHT]			=	kJOY_RIGHT;
	mJoyInput[JOY_INPUT_LEFT]			=	kJOY_LEFT;
	mJoyInput[JOY_INPUT_ATTACK]			=	kJOY_X;
	mJoyInput[JOY_INPUT_SUB_WEAPON]		=	kJOY_A;
	mJoyInput[JOY_INPUT_JUMP]			=	kJOY_C;
	mJoyInput[JOY_INPUT_AWAY]			=	kJOY_B;
	mJoyInput[JOY_INPUT_LOCKON]			=	kJOY_Y;
	mJoyInput[JOY_INPUT_SELECT_SUB]		=	kJOY_Z;
	mJoyInput[JOY_INPUT_SHOW_MAP]		=	kJOY_M;
	mJoyInput[JOY_INPUT_START]			=	kJOY_START;
	mJoyInput[JOY_INPUT_CAM_SWITCH]		=	kJOY_F;
	mJoyInput[JOY_INPUT_CAM_RESET]		=	kJOY_L;

	mJoyInput[JOY_INPUT_CAM_UP]			=	kJOY_XX;	// カメラ移動はあえてありえなさそうな値に設定
	mJoyInput[JOY_INPUT_CAM_DOWN]		=	kJOY_XX;	// カメラ移動はあえてありえなさそうな値に設定
	mJoyInput[JOY_INPUT_CAM_RIGHT]		=	kJOY_XX;	// カメラ移動はあえてありえなさそうな値に設定
	mJoyInput[JOY_INPUT_CAM_LEFT]		=	kJOY_XX;	// カメラ移動はあえてありえなさそうな値に設定

	mJoyInput[JOY_INPUT_RUN]			=	kJOY_XX;	// RUNはあえてありえなさそうな値に設定

	// キー入力
	mKeyInput[JOY_INPUT_UP]				=	eKEY_W;
	mKeyInput[JOY_INPUT_DOWN]			=	eKEY_S;
	mKeyInput[JOY_INPUT_RIGHT]			=	eKEY_D;
	mKeyInput[JOY_INPUT_LEFT]			=	eKEY_A;

	mKeyInput[JOY_INPUT_CAM_UP]			=	eKEY_I;
	mKeyInput[JOY_INPUT_CAM_DOWN]		=	eKEY_K;
	mKeyInput[JOY_INPUT_CAM_RIGHT]		=	eKEY_L;
	mKeyInput[JOY_INPUT_CAM_LEFT]		=	eKEY_J;

	mKeyInput[JOY_INPUT_CAM_RESET]		=	eKEY_F;
	mKeyInput[JOY_INPUT_CAM_SWITCH]		=	eKEY_P;
	mKeyInput[JOY_INPUT_ATTACK]			=	eKEY_U;
	mKeyInput[JOY_INPUT_SUB_WEAPON]		=	eKEY_O;
	mKeyInput[JOY_INPUT_JUMP]			=	eKEY_SPACE;
	mKeyInput[JOY_INPUT_AWAY]			=	eKEY_LALT;
	mKeyInput[JOY_INPUT_LOCKON]			=	eKEY_Q;
	mKeyInput[JOY_INPUT_SELECT_SUB]		=	eKEY_E;
	mKeyInput[JOY_INPUT_SHOW_MAP]		=	eKEY_M;
	mKeyInput[JOY_INPUT_START]			=	eKEY_RETURN;

	mKeyInput[JOY_INPUT_RUN]			=	eKEY_LSHIFT;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
InputManager::Update(void)
{
	if (GetWindowActiveFlag() == FALSE) {
		return;
	}

	// キーボードのステータス更新
	for (int iKey = 0; iKey < eKEY_MAX; iKey ++) {
		sPressKey[iKey] = CheckHitKey(sKeyConnectDefineTable[iKey]);
		sTrigKey[iKey] = FALSE;

		if (sPressKey[iKey] == TRUE) {
			if (sPrevPressKey[iKey] == FALSE) {
				sTrigKey[iKey] = TRUE;
			}
		}
		sPrevPressKey[iKey] = sPressKey[iKey];
	}	

	// マウスのステータス更新
	sPrevPressMouse = sNowPressMouse;
	sNowPressMouse = GetMouseInput();

	// マウスのポインタの更新
	//	int now_x, now_y;
	GetMousePoint(&sNowMousePointX, &sNowMousePointY);
	float mouse_sensitive = (float)(mMoveMouseSensitive + 1) / 51.0f;
	sMoveMousePointX = (int)((float)(sNowMousePointX - sPrevMousePointX) * mouse_sensitive);
	sMoveMousePointY = (int)((float)(sNowMousePointY - sPrevMousePointY) * mouse_sensitive);
	sPrevMousePointX = sNowMousePointX;
	sPrevMousePointY = sNowMousePointY;
	
	// ジョイパッドのステータス更新
	sPrevPressJoy	= sNowPressJoy;
	sNowPressJoy	= GetJoypadInputState(DX_INPUT_PAD1);


	int pov;
	pov = GetJoypadPOVState(DX_INPUT_PAD1, 0);
	switch (pov) {
	case 0:		
		sNowPressJoy |= (mJoyInput[JOY_INPUT_UP] | mJoyInput[JOY_INPUT_RUN]);
		break;
	case 4500:
		sNowPressJoy |= (mJoyInput[JOY_INPUT_UP] | mJoyInput[JOY_INPUT_RIGHT] | mJoyInput[JOY_INPUT_RUN]);
		break;
	case 9000:	
		sNowPressJoy |= (mJoyInput[JOY_INPUT_RIGHT] | mJoyInput[JOY_INPUT_RUN]);
		break;
	case 13500:
		sNowPressJoy |= (mJoyInput[JOY_INPUT_DOWN] | mJoyInput[JOY_INPUT_RIGHT] | mJoyInput[JOY_INPUT_RUN]);
		break;
	case 18000:	
		sNowPressJoy |= (mJoyInput[JOY_INPUT_DOWN] | mJoyInput[JOY_INPUT_RUN]);
		break;
	case 22500:
		sNowPressJoy |= (mJoyInput[JOY_INPUT_DOWN] | mJoyInput[JOY_INPUT_LEFT] | mJoyInput[JOY_INPUT_RUN]);
		break;
	case 27000:	
		sNowPressJoy |= (mJoyInput[JOY_INPUT_LEFT] | mJoyInput[JOY_INPUT_RUN]);
		break;
	case 31500:
		sNowPressJoy |= (mJoyInput[JOY_INPUT_UP] | mJoyInput[JOY_INPUT_LEFT] | mJoyInput[JOY_INPUT_RUN]);
		break;
	default: break;
	};


	// 左アナログの各ビット反転処理
	static const DWORD kMOVE_INPUT_MASK = 0x0F;
	DWORD set_joy = 0;
	if (mAnalogConfigSetting & eANALOG_REVERSE_HORI_LEFT) {
		set_joy |= (sNowPressJoy & mJoyInput[JOY_INPUT_LEFT]) << 1;
		set_joy |= (sNowPressJoy & mJoyInput[JOY_INPUT_RIGHT]) >> 1;
	}
	else {
		set_joy |= (sNowPressJoy & mJoyInput[JOY_INPUT_LEFT]);
		set_joy |= (sNowPressJoy & mJoyInput[JOY_INPUT_RIGHT]);
	}

	if (mAnalogConfigSetting & eANALOG_REVERSE_VERT_LEFT) {
		set_joy |= (sNowPressJoy & mJoyInput[JOY_INPUT_DOWN]) << 3;
		set_joy |= (sNowPressJoy & mJoyInput[JOY_INPUT_UP]) >> 3;
	}
	else {
		set_joy |= (sNowPressJoy & mJoyInput[JOY_INPUT_DOWN]);
		set_joy |= (sNowPressJoy & mJoyInput[JOY_INPUT_UP]);
	}
	
	if (mAnalogConfigSetting & eANALOG_REVERSE_AXIS_LEFT) {
		DWORD tmp = set_joy;
		set_joy = 0;
		set_joy |= (tmp & mJoyInput[JOY_INPUT_UP]) >> 2;
		set_joy |= (tmp & mJoyInput[JOY_INPUT_DOWN]) << 2;
		set_joy |= (tmp & mJoyInput[JOY_INPUT_LEFT]) << 2;
		set_joy |= (tmp & mJoyInput[JOY_INPUT_RIGHT]) >> 2;
	}
	sNowPressJoy = sNowPressJoy & ~kMOVE_INPUT_MASK;
	sNowPressJoy = sNowPressJoy | (set_joy & kMOVE_INPUT_MASK);

	// -------------------------------------------------------------
	// 以下、拡張
	// -------------------------------------------------------------
	// ジョイパッドのステータスにキーボード、マウス分も更新
	for (int iI = 0; iI < JOY_INPUT_MAX; iI ++) {
		if (sPressKey[ mKeyInput[iI] ] == TRUE) {
			sNowPressJoy |= mJoyInput[iI];
		}
	}

	// マウスのボタンも入力できるように対応
	if (sNowPressMouse & MOUSE_INPUT_LEFT) {
		sNowPressJoy |= mJoyInput[JOY_INPUT_ATTACK];
	}
	if (sNowPressMouse & MOUSE_INPUT_MIDDLE) {
		sNowPressJoy |= mJoyInput[JOY_INPUT_SUB_WEAPON];
	}

	// 入力済みのキーを取得する
	sNowTrigJoy = 0;
	for (int iI = 0; iI < JOY_INPUT_MAX; iI ++) {
		if ((sNowPressJoy & mJoyInput[iI]) != FALSE) {
			if ((sPrevPressJoy & mJoyInput[iI]) == FALSE) {
				sNowTrigJoy |= (1<<iI);
			}
		}
	}

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
InputManager::Destroy(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
InputManager::Refresh(void)
{
	memset(&sPrevPressKey, 0x00, sizeof(sPrevPressKey));
	memset(&sTrigKey, 0x00, sizeof(sTrigKey));
	memset(&sPressKey, 0x00, sizeof(sPressKey));
	sPrevPressJoy		= 0x00000000;
	sNowPressJoy		= 0x00000000;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
InputManager::AssignKeyboard(eJoyInput joyInput, unsigned int keyIndex)
{
	mKeyInput[joyInput] = keyIndex;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
InputManager::AssignJoyPad(eJoyInput joyInput, unsigned int joyValue)
{
	// 十字キーの分だけ4bitシフトさせる
	mJoyInput[joyInput]	= joyValue;
}

//---------------------------------------------------
/**
 * アナログ上下・左右反転ビット
 */
//---------------------------------------------------
void
InputManager::AssignAnalogReverse(BOOL& rLHori, BOOL& rLVert, BOOL& rLAxis, BOOL& rRHori, BOOL& rRVert, BOOL& rRAxis)
{
	mAnalogConfigSetting = 0;

	if (rLHori == TRUE) mAnalogConfigSetting |= eANALOG_REVERSE_HORI_LEFT;
	if (rLVert == TRUE) mAnalogConfigSetting |= eANALOG_REVERSE_VERT_LEFT;
	if (rLAxis == TRUE) mAnalogConfigSetting |= eANALOG_REVERSE_AXIS_LEFT;

	mAnalogRightHoriRate = (rRHori == TRUE) ? -1 : 1;
	mAnalogRightVertRate = (rRVert == TRUE) ? -1 : 1;
	if (rRAxis == TRUE) mAnalogConfigSetting |= eANALOG_REVERSE_AXIS_RIGHT;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
InputManager::SetMouseSensitive(CHAR val)
{
	mMoveMouseSensitive = val;
}

/*=====================================*
 * 取得
 *=====================================*/
//---------------------------------------------------
/**
// キーボードのキーをチェック
 */
//---------------------------------------------------
bool
InputManager::CheckPressKey(DWORD key)
{
	return (sPressKey[key] == TRUE) ? true : false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
InputManager::CheckTrigKey(DWORD key)
{
	return (sTrigKey[key] == TRUE) ? true : false;
}
	
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
InputManager::EnablePressKey(DWORD key)
{
	sPressKey[key] = TRUE;
}

//---------------------------------------------------
/**
 * マウスをチェック
 */
//---------------------------------------------------
bool
InputManager::CheckPressMouse(int btn)
{
	return ((sNowPressMouse & btn) == 0) ? false : true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
InputManager::CheckTrigMouse(int btn)
{
	if ((sNowPressMouse & btn) != 0) {
		if ((sPrevPressMouse & btn) == 0) {
			return true;
		}
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
InputManager::GetMouseNowPoint(int* pPointX, int* pPointY)
{
	*pPointX = sNowMousePointX;
	*pPointY = sNowMousePointY;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
InputManager::GetMouseMoveValue(int* pValueX, int* pValueY)
{
	*pValueX = sMoveMousePointX;
	*pValueY = sMoveMousePointY;
}

//---------------------------------------------------
/**
 // ジョイパッドのキーをチェック
 */
//---------------------------------------------------
bool
InputManager::CheckPressJoy(eJoyInput joy)
{
	return ((sNowPressJoy & mJoyInput[joy]) == FALSE) ? false : true;
}

//---------------------------------------------------
/**
 // ジョイパッドのキーをチェック
 */
//---------------------------------------------------
bool
InputManager::CheckTrigJoy(eJoyInput joy)
{
	return (sNowTrigJoy & (1<<joy)) ? true : false;
}
	
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
InputManager::CheckTrigAnyJoy(void)
{
	if ((CHECK_TRIG_JOY(JOY_INPUT_SUB_WEAPON)) ||
		(CHECK_TRIG_JOY(JOY_INPUT_AWAY)) ||
		(CHECK_TRIG_JOY(JOY_INPUT_ATTACK)) ||
		(CHECK_TRIG_JOY(JOY_INPUT_START)) ||
		(CHECK_TRIG_KEY(eKEY_RETURN))) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
InputManager::StartVibe(void)
{
	int ret = StartJoypadVibration(DX_INPUT_PAD1, 800, 240);
	APP_ASSERT( (ret == 0), "Failed StartVibe() \n");
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
InputManager::StopVibe(void)
{
	StopJoypadVibration(DX_INPUT_PAD1);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
InputManager::GetJoyAnalogRight(int* pValueX, int* pValueY)
{
	if (mAnalogConfigSetting & eANALOG_REVERSE_AXIS_RIGHT) {
		// X,Y軸反転
		GetJoypadAnalogInputRight(pValueY, pValueX, DX_INPUT_PAD1);
	} 
	else {
		// 通常
		GetJoypadAnalogInputRight(pValueX, pValueY, DX_INPUT_PAD1);
	}
	
	*pValueX *= mAnalogRightHoriRate;
	*pValueY *= mAnalogRightVertRate;


	// キーボードでのカメラ処理
	{
		const USHORT kKEYBOARD_CAM_VALUE		= 750;
		const USHORT kKEYBOARD_CAM_VALUE_SQRT	= 530; 

		BYTE is_key = FALSE;
		if (sPressKey[ mKeyInput[JOY_INPUT_CAM_UP] ] == TRUE) {
			*pValueY = -kKEYBOARD_CAM_VALUE;
			is_key = TRUE;
		}
		else if (sPressKey[ mKeyInput[JOY_INPUT_CAM_DOWN] ] == TRUE) {
			*pValueY = kKEYBOARD_CAM_VALUE;
			is_key = TRUE;
		}

		if (sPressKey[ mKeyInput[JOY_INPUT_CAM_RIGHT] ] == TRUE) {
			if (is_key) {
				*pValueY *= (int)((float)kKEYBOARD_CAM_VALUE_SQRT / (float)kKEYBOARD_CAM_VALUE);
				*pValueX = kKEYBOARD_CAM_VALUE_SQRT;
			}
			else {
				*pValueX = kKEYBOARD_CAM_VALUE;
			}
		}
		else if (sPressKey[ mKeyInput[JOY_INPUT_CAM_LEFT] ] == TRUE) {
			if (is_key) {
				*pValueY *= (int)((float)kKEYBOARD_CAM_VALUE_SQRT / (float)kKEYBOARD_CAM_VALUE);
				*pValueX = -kKEYBOARD_CAM_VALUE_SQRT;
			}
			else {
				*pValueX = -kKEYBOARD_CAM_VALUE;
			}
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
InputManager::ResetNowJoyState(void)
{
	sNowPressJoy = 0;
}

//---------------------------------------------------
/**
 * Trig情報をリセットする
 */
//---------------------------------------------------
void
InputManager::ResetNowTrigJoy(void)
{
	sNowTrigJoy = 0;
}

/**** end of file ****/
