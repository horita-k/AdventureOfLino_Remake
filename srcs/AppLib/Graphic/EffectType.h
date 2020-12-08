/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * エフェクトファイルの定義	
 * @author SPATZ.
 * @data   2012/05/03 01:59:45
 */
//---------------------------------------------------
#ifndef __EFFECT_TYPE_H__
#define __EFFECT_TYPE_H__


#include <d3dx9.h>

/*=====================================*
 * 構造体
 *=====================================*/
struct MY_VERTEX
{
	// 変換済み頂点
	FLOAT X, Y, Z, RHW;
	// ディフューズ色
	D3DCOLOR Diffuse;
	// テクスチャ座標
	FLOAT U0, V0;
	FLOAT U1, V1;
};
// FVF
#define D3DFVF_VERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX2|D3DFVF_TEXCOORDSIZE2(0)|D3DFVF_TEXCOORDSIZE2(1))

/*=====================================*
 * typedef
 *=====================================*/
typedef	D3DXHANDLE		EFFHND_TECH;	// テクニック用定義
typedef	D3DXHANDLE		EFFHND_PASS;	// パス用定義
typedef	D3DXHANDLE		EFFHND_PARM;	// パラメータ用定義
typedef	D3DXHANDLE		EFFHND_ANNO;	// アノテーション用定義

/*=====================================*
 * 参照ファイル
 *
 * E:\project\DXLibTest\設計用\エフェクト学習.xls
 * セマンティック定義 sheet 
 *
 * 定義を追加する場合は上のシートを用いて追加すること！！
 *=====================================*/
/*=====================================*
 * セマンティック定義
 *=====================================*/
/**** セマンティック識別定義 ****/	
enum eSmtKind {
    eSMT_TIME,						    // 0 : 0フレーム目からの再生時間を表す
    eSMT_WORLD,                         // 1 : ワールド変換行列
    eSMT_VIEW,                          // 2 : ビュー変換行列
    eSMT_VIEWINVERSE,                   // 3 : ビュー変換行列　逆行列
    eSMT_VIEWPROJECTION,                // 4 : ビュー変換行列×プロジェクション変換行列
    eSMT_VIEWPROJECTIONINVERSE,         // 5 : ビュー変換行列×プロジェクション変換行列　逆行列
    eSMT_PROJECTION,                    // 6 : プロジェクション変換行列
    eSMT_PROJECTIONINVERSE,             // 7 : プロジェクション変換行列　ぎゃk行列
    eSMT_WORLDVIEWPROJECTION,           // 8 : ワールド変換行列×ビュー変換行列×プロジェクション変換行列
    eSMT_WORLDVIEWPROJECTIONINVERSE,    // 9 : ワールド変換行列×ビュー変換行列×プロジェクション変換行列　逆行列
    eSMT_WORLDINVERSETRANSPOSE,         // 10 : ワールド変換行列　逆倒置行列
    eSMT_WORLDVIEW,                     // 11 : ワールド変換行列×ビュー変換行列
    eSMT_VIEWINVERSETRANSPOSE,          // 12 : ビュー変換行列　逆倒置行列
    eSMT_WORLDINVERSE,                  // 13 : ワールド変換行列　逆行列
    eSMT_WORLDVIEWTRANSPOSE,            // 14 : ワールド変換行列×ビュー変換行列　倒置行列
    eSMT_WORLDVIEWINVERSE,              // 15 : ワールド変換行列×ビュー変換行列　逆行列
    eSMT_VIEWTRANSPOSE,                 // 16 : ビュー変換行列　倒置行列
    eSMT_POSITION,                      // 17 : 座標ベクトル
    eSMT_DIRECTION,                     // 18 : 方向ベクトル
    eSMT_MODELDATA,                     // 19 : モデルデータ指定
    eSMT_DIFFUSE,                       // 20 : ディフューズ色（拡散光）
    eSMT_AMBIENT,                       // 21 : アンビエント色（環境光）
    eSMT_EMISSIVE,                      // 22 : エミッション色（放射光）
    eSMT_SPECULAR,                      // 23 : スペキュラ色（鏡面光）
    eSMT_SPECULARPOWER,                 // 24 : スペキュラの強度
    eSMT_TOONCOLOR,                     // 25 : トゥーン色
    eSMT_EDGECOLOR,                     // 26 : 輪郭色
    eSMT_GROUNDSHADOWCOLOR,             // 27 : 地面影色
    eSMT_ELAPSEDTIME,                   // 28 : 前回の描画からの経過時間を表す
    eSMT_CONTROLOBJECT,                 // 29 : 指定したオブジェクトの、座標やワールド変換行列を取得する。
    eSMT_RENDERCOLORTARGET,             // 30 : レンダリングターゲットに指定可能なサーフェイスを生成する。
    eSMT_RENDERDEPTHSTENCILTARGET,      // 31 : 深度ステンシルサーフェイス（いわゆるZバッファ）を生成する。
    eSMT_TEXTUREVALUE,                  // 32 : 指定したテクスチャの、テクセル情報を取得して配列に格納する。
    eSMT_STANDARDSGLOBAL,               // 33 : SAS（Standard Annotations and Semantics）のバージョンを指定する。
    eSMT_OFFSCREENRENDERTARGET,         // 34 : オフスクリーンレンダーターゲットを生成します。
    eSMT_VIEWPORTPIXELSIZE,				// 35 : ビューポートピクセルサイズ
    eSMT_DUMMY,                         // 36 : ダミー
    eSMT_MAX,                           // 37 : 要素数
};


/**** セマンティック文字列 ****/	
static const char* strSmtName[eSMT_MAX] = {
    "TIME",								// 0 : 0フレーム目からの再生時間を表す
    "WORLD",                            // 1 : ワールド変換行列
    "VIEW",                             // 2 : ビュー変換行列
    "VIEWINVERSE",                      // 3 : ビュー変換行列　逆行列
    "VIEWPROJECTION",                   // 4 : ビュー変換行列×プロジェクション変換行列
    "VIEWPROJECTIONINVERSE",            // 5 : ビュー変換行列×プロジェクション変換行列　逆行列
    "PROJECTION",                       // 6 : プロジェクション変換行列
    "PROJECTIONINVERSE",                // 7 : プロジェクション変換行列　ぎゃk行列
    "WORLDVIEWPROJECTION",              // 8 : ワールド変換行列×ビュー変換行列×プロジェクション変換行列
    "WORLDVIEWPROJECTIONINVERSE",       // 9 : ワールド変換行列×ビュー変換行列×プロジェクション変換行列　逆行列
    "WORLDINVERSETRANSPOSE",            // 10 : ワールド変換行列　逆倒置行列
    "WORLDVIEW",                        // 11 : ワールド変換行列×ビュー変換行列
    "VIEWINVERSETRANSPOSE",             // 12 : ビュー変換行列　逆倒置行列
    "WORLDINVERSE",                     // 13 : ワールド変換行列　逆行列
    "WORLDVIEWTRANSPOSE",               // 14 : ワールド変換行列×ビュー変換行列　倒置行列
    "WORLDVIEWINVERSE",                 // 15 : ワールド変換行列×ビュー変換行列　逆行列
    "VIEWTRANSPOSE",                    // 16 : ビュー変換行列　倒置行列
    "POSITION",							// 17 : 座標ベクトル
    "DIRECTION",						// 18 : 方向ベクトル
    "MODELDATA",						// 19 : モデルデータ指定
    "DIFFUSE",							// 20 : ディフューズ色（拡散光）
    "AMBIENT",							// 21 : アンビエント色（環境光）
    "EMISSIVE",							// 22 : エミッション色（放射光）
    "SPECULAR",							// 23 : スペキュラ色（鏡面光）
    "SPECULARPOWER",					// 24 : スペキュラの強度
    "TOONCOLOR",						// 25 : トゥーン色
    "EDGECOLOR",						// 26 : 輪郭色
    "GROUNDSHADOWCOLOR",				// 27 : 地面影色
    "ELAPSEDTIME",						// 28 : 前回の描画からの経過時間を表す
    "CONTROLOBJECT",					// 29 : 指定したオブジェクトの、座標やワールド変換行列を取得する。
    "RENDERCOLORTARGET",				// 30 : レンダリングターゲットに指定可能なサーフェイスを生成する。
    "RENDERDEPTHSTENCILTARGET",			// 31 : 深度ステンシルサーフェイス（いわゆるZバッファ）を生成する。
    "TEXTUREVALUE",						// 32 : 指定したテクスチャの、テクセル情報を取得して配列に格納する。
    "STANDARDSGLOBAL",					// 33 : SAS（Standard Annotations and Semantics）のバージョンを指定する。
    "OFFSCREENRENDERTARGET",			// 34 : オフスクリーンレンダーターゲットを生成します。
    "VIEWPORTPIXELSIZE",				// 35 : ビューポートピクセルサイズ
    "DUMMY",							// 36 : ダミー
};	


/*=====================================*
 * スクリプト定義
 *=====================================*/
/**** スクリプト識別定義 ****/	
enum eScriptKind {
	eSCRIPT_RenderColorTarget,			// 0 : レンダリングターゲットを設定します。
	eSCRIPT_RenderColorTarget0,         // 1 : レンダリングターゲットを設定します。
	eSCRIPT_RenderColorTarget1,         // 2 : レンダリングターゲットを設定します。
	eSCRIPT_RenderColorTarget2,         // 3 : レンダリングターゲットを設定します。
	eSCRIPT_RenderColorTarget3,         // 4 : レンダリングターゲットを設定します。
	eSCRIPT_RenderDepthStencilTarget,   // 5 : 深度ステンシルサーフェイスを設定します。
	eSCRIPT_ClearSetColor,              // 6 : レンダリングターゲットをクリアする色を設定する。
	eSCRIPT_ClearSetDepth,              // 7 : 深度ステンシルサーフェイスをクリアするZ値を設定する。
	eSCRIPT_Clear,                      // 8 : (=Color):レンダリングターゲットをクリアします。, (=Depth):深度ステンシルサーフェイスをクリアします。
	eSCRIPT_ScriptExternal,             // 9 : 他のオブジェクトを描画します。
	eSCRIPT_Pass,                       // 10 : 指定したパスを実行します
	eSCRIPT_LoopByCount,                // 11 : 指定した回数だけ、スクリプトの一部をループします。
	eSCRIPT_LoopEnd,                    // 12 : 指定した回数だけ、スクリプトの一部をループします。
	eSCRIPT_LoopGetIndex,               // 13 : ループ中のループカウンタの値を、指定したパラメータに設定します。
	eSCRIPT_Draw,                       // 14 : (=Geometory):オブジェクトを描画します。 (=Buffer):レンダリングターゲットのスクリーンと一致するサイズの、長方形のポリゴンを描画します。
	eSCRIPT_MAX,                        // 15 : 要素数
};

/**** スクリプト文字列 ****/	
static const char* strScriptName[eSCRIPT_MAX] = {
	"RenderColorTarget",				// 0 : レンダリングターゲットを設定します。
	"RenderColorTarget0",           	// 1 : レンダリングターゲットを設定します。
	"RenderColorTarget1",           	// 2 : レンダリングターゲットを設定します。
	"RenderColorTarget2",           	// 3 : レンダリングターゲットを設定します。
	"RenderColorTarget3",           	// 4 : レンダリングターゲットを設定します。
	"RenderDepthStencilTarget",     	// 5 : 深度ステンシルサーフェイスを設定します。
	"ClearSetColor",                	// 6 : レンダリングターゲットをクリアする色を設定する。
	"ClearSetDepth",                	// 7 : 深度ステンシルサーフェイスをクリアするZ値を設定する。
	"Clear",                        	// 8 : (=Color):レンダリングターゲットをクリアします。, (=Depth):深度ステンシルサーフェイスをクリアします。
	"ScriptExternal",               	// 9 : 他のオブジェクトを描画します。
	"Pass",                         	// 10 : 指定したパスを実行します
	"LoopByCount",                  	// 11 : 指定した回数だけ、スクリプトの一部をループします。
	"LoopEnd",                      	// 12 : 指定した回数だけ、スクリプトの一部をループします。
	"LoopGetIndex",                 	// 13 : ループ中のループカウンタの値を、指定したパラメータに設定します。
	"Draw",                         	// 14 : (=Geometory):オブジェクトを描画します。 (=Buffer):レンダリングターゲットのスクリーンと一致するサイズの、長方形のポリゴンを描画します。
};

#endif // __EFFECT_TYPE_H__

 /**** end of file ****/

