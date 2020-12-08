/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * メインソース
 * @author SPATZ.
 * @data   2015/01/09 00:15:30
 */
//---------------------------------------------------
#ifdef _DEBUG
// メモリリーク検出関連
/*
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
*/
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <sys/types.h>
#include <sys/stat.h>

#endif // _DEBUG
#include "dxlib.h"
#include "Scene/SceneManager.h"
#include "AppLib/Basic/Basic.h"
#include "AppLib/Effect/EffectManager.h"
#include "Scene/TotalInfo.h"
#include "../resource.h"
#include <math.h>

#define __USE_ARCHIVE_SYSTEM	/* dxlib のアーカイブシステムを使用するか？ */

//---------------------------------------------------
/**
 * メイン関数 
 */
//---------------------------------------------------
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
#ifdef _DEBUG
	//<! メモリリーク検出ツール
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|
				   _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG
	
	// コンフィグデータの読み込み
	if (TotalInfo::GetInstance()->LoadConfigData() == false) {
		DxLib_End();
		return 0;
	}

	int reso_type = TotalInfo::GetInstance()->GetConfigVideoSize();
	const tDisplaySetting* p_setting = NULL;
	if (reso_type > (sizeof(sDisplaySetting) / sizeof(tDisplaySetting))) {
		reso_type = 1; // 適当に
	}
	p_setting = &sDisplaySetting[reso_type];

	/**** タイトル名とアイコンを変更 ****/
	SetMainWindowText(GAME_TITLE);
	//	SetWindowIconID(IDI_ICON1);
	SetWindowIconID( IDI_ICON2 ); 

#ifdef LIB_VER_3_12e
	/**** サウンドのボリューム変更に伴い、旧バージョンを使用するフラグ ****/
	SetUseOldVolumeCalcFlag(TRUE);
#endif

	/**** 画面モードの変更 ****/
	if ((p_setting->mWindowSizeX == -1) || (p_setting->mWindowSizeY == -1)) {
		ChangeWindowMode(FALSE);		// フルスクリーン
	}
	else {
		SetWindowSize(p_setting->mWindowSizeX, p_setting->mWindowSizeY);
		ChangeWindowMode(TRUE);			// ウィンドウモード
		SetWindowSizeChangeEnableFlag(TRUE);	// ウインドウのサイズを自由に変更出来るように設定する
	}
	int res_x, res_y;
	res_x = p_setting->mResolusionX;
	res_y = p_setting->mResolusionY;
	float reso_rate = TotalInfo::GetInstance()->GetConfigResolutionRateFromGraphQuality();
	res_x = (int)((float)res_x * reso_rate);
	res_y = (int)((float)res_y * reso_rate);
	SetGraphMode(res_x, res_y, p_setting->mColorBit);
	SetDrawArea(0, 0, res_x, res_y);

	/**** 乱数初期化 ****/
	SRand(65536);
	
	SetUseBackCulling(TRUE);

#ifndef _DEBUG /* only release */
	SetOutApplicationLogValidFlag(FALSE);		// ログメッセージを無効にする	
#else // _DEBUG
	SetOutApplicationLogValidFlag(FALSE);		// ログメッセージを無効にする	
#endif // _DEBUG

	// フルシーンアンチエイリアスを設定する
	if (TotalInfo::GetInstance()->GetConfigIsAntiAlias() == true) {
		SetFullSceneAntiAliasingMode( 4, 2 ) ;
	}

	// バックグラウンドでも動作させる
	if (TotalInfo::GetInstance()->GetConfigIsBackgroundRun()) {
		SetAlwaysRunFlag(true);
	}

	SetUseSoftwareMixingSoundFlag( TRUE ) ;

	
#ifdef LIB_VER_3_12e
	SetUseDirect3DVersion( DX_DIRECT3D_9EX ) ;
#endif // LIB_VER_3_12e

	/**** ＤＸライブラリの初期化 ****/
	if( DxLib_Init() < 0 )
	{
		// エラーが発生したら直ちに終了
		return -1 ;
	}

	// ウィンドウのメニューリストのセットアップ
	TotalInfo::GetInstance()->SetupWinMenu();

	/**** SceneManager の生成 ****/
	SceneManager *const p_sceneman = SceneManager::GetInstance();
	p_sceneman->Create();

#ifdef __USE_ARCHIVE_SYSTEM
	/**** アーカイブシステムを使用する ****/
	char pasuwaado[64] = {0};
	// passwordは直接打たず、関節的に入力 : 2DbCsG5x byfhuICZ 8PXlUZE (2DbCsG5xbyfhuICZ8PXlUZE)
	sprintf_s(&pasuwaado[0], sizeof(pasuwaado), "%sbyfhuICZ%s", "2DbCsG5x", "8PXlUZE");

	SetUseDXArchiveFlag(TRUE);
	SetDXArchiveExtension("spatz");
	SetDXArchiveKeyString(pasuwaado);
	const char* filename = "data.spatz";

#endif // __USE_ARCHIVE_SYSTEM

	// シーンマネージャのセットアップ
	p_sceneman->Setup();

	/**** キーボードのコンフィグ設定 ****/
	TotalInfo::GetInstance()->SetupAssignKeyboard();

	/**** ジョイパッドのコンフィグ設定 ****/
	TotalInfo::GetInstance()->SetupAssignJoyInput();

	/**** アナログの上下・左右反転ビットの設定 ****/
	TotalInfo::GetInstance()->SetupAnalogReverse();

	/**** マウス感度の設定 ****/
	TotalInfo::GetInstance()->SetupMouseSensitive();

	/* メインループ */
	while (p_sceneman->CheckMainLoop()) {
		/* シーンメインループ */
		p_sceneman->Update();
	}

	// SceneManagerからメインタスクを破棄
	p_sceneman->RemoveDebugTask();
	// ＤＸライブラリの後始末
	DxLib_End() ;

#ifdef _DEBUG
//	_CrtDumpMemoryLeaks();
#endif // _DEBUG

	// ソフトの終了
	return 0 ;
}

 /**** end of file ****/
