/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 入力統括マネージャクラス
 * @author SPATZ.
 * @data   2011/11/21 19:40:23
 */
//---------------------------------------------------
#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

#include <windows.h>
#include "InputInfo.h"

#define CHECK_PRESS_KEY(val)	InputManager::GetInstance()->CheckPressKey(val)
#define CHECK_TRIG_KEY(val)		InputManager::GetInstance()->CheckTrigKey(val)

#define CHECK_PRESS_JOY(val)	InputManager::GetInstance()->CheckPressJoy(val)
#define CHECK_TRIG_JOY(val)		InputManager::GetInstance()->CheckTrigJoy(val)
#define CHECK_TRIG_ANY_JOY()	InputManager::GetInstance()->CheckTrigAnyJoy()

#define CHECK_PRESS_MOUSE(val)	InputManager::GetInstance()->CheckPressMouse(val)
#define CHECK_TRIG_MOUSE(val)	InputManager::GetInstance()->CheckTrigMouse(val)


/* キーパッド関連 */
enum eJoyInput {
	JOY_INPUT_UP = 0,
	JOY_INPUT_DOWN,
	JOY_INPUT_RIGHT,
	JOY_INPUT_LEFT,
	JOY_INPUT_ATTACK,
	JOY_INPUT_JUMP,
	JOY_INPUT_SUB_WEAPON,
	JOY_INPUT_AWAY,
	JOY_INPUT_LOCKON,
	JOY_INPUT_SELECT_SUB,
	JOY_INPUT_SHOW_MAP,
	JOY_INPUT_START,
	JOY_INPUT_CAM_SWITCH,
	JOY_INPUT_CAM_RESET,

	JOY_INPUT_CAM_UP,
	JOY_INPUT_CAM_DOWN,
	JOY_INPUT_CAM_RIGHT,
	JOY_INPUT_CAM_LEFT,

	JOY_INPUT_RUN,

	JOY_INPUT_MAX,
};

/*=====================================*
 * class
 *=====================================*/
class InputManager
{
 public:

	// アナログスティック反転ビット
	enum {
		eANALOG_REVERSE_HORI_LEFT = 0x01,
		eANALOG_REVERSE_VERT_LEFT = 0x02,
		eANALOG_REVERSE_AXIS_LEFT = 0x04,
		eANALOG_REVERSE_AXIS_RIGHT = 0x08,
		// 4bit なのでこれ以上追加は無理
	};

	static InputManager* const GetInstance(void);

	void		Create(void);
	void		Setup(void);
	void		Update(void);
	void		Destroy(void);

	void		Refresh(void);

	void		AssignKeyboard(eJoyInput joyInput, unsigned int keyIndex);
	void		AssignJoyPad(eJoyInput joyInput, unsigned int joyValue);
	void		AssignAnalogReverse(BOOL& rLHori, BOOL& rLVert, BOOL& rLAxis, BOOL& rRHori, BOOL& rRVert, BOOL& rRAxis);
	void		SetMouseSensitive(CHAR val);

	/*=====================================*
	 * 取得
	 *=====================================*/
	// キーボードのキーをチェック
	bool CheckPressKey(DWORD key);
	bool CheckTrigKey(DWORD key);
	
	void EnablePressKey(DWORD key);

	// マウスをチェック
	bool CheckPressMouse(int btn);
	bool CheckTrigMouse(int btn);
	void GetMouseNowPoint(int* pPointX, int* pPointY);
	void GetMouseMoveValue(int* pValueX, int* pValueY);

	// ジョイパッドのキーをチェック
	bool CheckPressJoy(eJoyInput joy);
	bool CheckTrigJoy(eJoyInput joy);
	bool CheckTrigAnyJoy(void);
	void StartVibe(void);
	void StopVibe(void);

	void GetJoyAnalogRight(int* pValueX, int* pValueY);


	void ResetNowJoyState(void);
	void ResetNowTrigJoy(void);

private:
	InputManager();
	~InputManager();

	unsigned int	mJoyInput[JOY_INPUT_MAX];
	DWORD			mKeyInput[JOY_INPUT_MAX];

	BYTE			mAnalogConfigSetting;

	CHAR			mAnalogRightHoriRate;
	CHAR			mAnalogRightVertRate;

	CHAR			mMoveMouseSensitive;		// 0 - 100

};

#endif // __MODEL_PMD_H__

 /**** end of file ****/
