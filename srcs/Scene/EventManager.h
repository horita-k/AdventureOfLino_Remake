/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * EventManager.cpp
 * タスク(イベント管理)クラス
 * @author SPATZ.
 * @data   2012/04/04 14:48:17
 */
//---------------------------------------------------
#ifndef __EVENT_MANAGER_H__
#define __EVENT_MANAGER_H__

#include "dxlib.h"

//#include "AppLib/Graphic/CharaBase.h"
#include "../Charactor/CharaNPC.h"
#include "AppLib/Resource/CsvAccesser.h"

/*=====================================*
 * define
 *=====================================*/
#define kEVTMGR_ANIM_TYPE_FADE_IN		(0x01)	// フェードインのアニメショーン
#define kEVTMGR_ANIM_TYPE_FADE_OUT		(0x02)	// フェードアウトのアニメショーン
#define kEVTMGR_ANIM_TYPE_ROTATION		(0x04)	// 回転のアニメショーン
#define kEVTMGR_ANIM_TYPE_SCALE			(0x08)	// スケールのアニメショーン

#define EVENT_MANAGER_AFTER_EFFECT_DRAW	(0xFFFFFFFF)

#define kEVTMGR_LAYOUT_NUM				(4)		// EventManager で使えるレイアウトの数

/*=====================================*
 * struct
 *=====================================*/

/**** ヘッダ ****/
struct Header
{
	char header[30];	// "Vocaloid Motion Data 0002\0" 30byte // (MMDver2以前のvmdは"Vocaloid Motion Data file\0")
	char modelName[20]; // モデル名 20byte(MMDver2以前のvmdは10byte) // 終端 0x00,パディング 0xFD, 文字コード：シフトJIS（ボーン名･表情名も同様）// 内容がカメラ，照明の場合は"カメラ・照明\0on Data"となる
};

/**** カメラキーフレーム要素データ(61Bytes/要素) ****/
struct CameraFrame
{
	DWORD frame;		// フレーム番号
	float distance;		// 目標点とカメラの距離(目標点がカメラ前面でマイナス)
	float x;			// 目標点のX軸位置
	float y;			// 目標点のY軸位置
	float z;			// 目標点のZ軸位置
	float rx;			// カメラのx軸回転(rad)(MMD数値入力のマイナス値)
	float ry;			// カメラのy軸回転(rad)
	float rz;			// カメラのz軸回転(rad)
	char bezier[24];	// 補間パラメータ
	DWORD viewAngle;	// 視野角(deg)
	char parth;			// パースペクティブ, 0:ON, 1:OFF
};

/*=====================================*
 * class
 *=====================================*/
class EventManager 
{
 public:

	/*=====================================*
	 * struct
	 *=====================================*/
	/**** モデルアニメーション構造体 ****/
	struct tModelAnimInfo 
	{
		BYTE	mAnimState;
		float	mStartFrame;
		float	mEndFrame;
		float	mRotationSpeed;
		float	mDefaultScale;
	};

	/**** レイアウトアニメーションの拡縮情報 ****/
	struct tLayoutAnimScaleInfo
	{
		int		mBaseWidth;
		int		mBaseHeight;
		int		mTargetWidth;
		int		mTargetHeight;
	};

	/**** レイアウトアニメーション構造体 ****/
	struct tLayoutAnimInfo
	{
		// 共通情報
		BYTE					mAnimState;
		float					mStartFrame;
		float					mEndFrame;
		// 各アニメーション情報
		tLayoutAnimScaleInfo	mLytAnmScaleInfo;
	};

	static EventManager* const GetInstance(void);

	/* 関数 */
	void		Setup(void);
	void		LoadCameraFile(const char* fileName, float scaleCamera=10.0f);
	void		StartEvent(CsvAccesser* pCsvAccesser=NULL, 
						   const VECTOR* pShiftPos=NULL,		// シフト座標設定(NULLの場合はオフセットなし)
						   bool isVisibleNpc=false,				// NPC表示フラグ
						   bool isVisibleObject=false);			// アイテム表示フラグ
						   
	void		SeekEvent(float seekFrame);
	void		Update(void);
	void		EndEvent(void);
	bool		IsEndCamera(void)		{ return mIsEndCamera; }
	bool		IsStartEvent(void)		{ return mIsStartEvent; }
	void		PauseEvent(void)		{ mIsPauseEvent = TRUE; }

	void				EntryModelNum(int num);
	int					GetModelNum(void)			{ return mModelNum; }
	void				SetModelParameter(int modelIdx, const char* strName, int motionIdx, ModelBase* pInsertBeforeModel=NULL, bool isMotionLoop=false);
	CharaNpc* const		GetModelPmd(int modelIdx); // あえてキャストして渡してます

	LayoutBase*			GetLayoutBase(int layoutIdx);
	LayoutBase*			GetSystemLayoutBase(int layoutIdx);

	void				EnableOffsetCamera(float setPosX, float offsetPosY, float setPosZ, float setRot);
	
	// 時間取得関数
	bool				EqualPhaseFrame(float frame);
	bool				BetweenPhaseFrame(float secMin, float secMax);
	
	float				GetPhaseFrame(void)						{ return mFrameCounter; }
	void				SetPhaseFrame(float frame)				{ mFrameCounter = frame; }
	void				GetShiftCameraPosition(VECTOR* pShiftPos)	{ *pShiftPos = mShifCamPos; }

	DWORD				GetNextKey(void)						{ return mNextKey; }
	void				SetNextKey(DWORD nextKey)				{ mNextKey = nextKey; }

	// この関数は他の関数から呼ばれる事を期待している関数
	void				UpdateEventAnimation(void);
	void				SetModelAnimInfoWork(int modelIdx, tModelAnimInfo* pAnimation);
	void				SetLayoutAnimInfoWork(int layoutlIdx, tLayoutAnimInfo* pAnimation);

 private:
	EventManager();
	~EventManager();

	void		getFrameCamPosTar(CameraFrame* pFrame, VECTOR* pPos, VECTOR* pTar);

	/* 変数 */
	float		mFrameCounter;
	bool		mIsStartEvent;
	BOOL		mIsPauseEvent;

	char*		mpCameraFrame;
	DWORD		mCameraKeyNum;
	DWORD		mNextKey;
	int			mFileHandle;
	bool		mIsEndCamera;

	bool		mIsOffsetCamera;
	float		mSetCamPosX;
	float		mOffsetCamPosY;
	float		mSetCamPosZ;
	float		mOffsetCamRot;
	float		mCameraScale;

	/* モデル関連 */
	int			mModelNum;
	CharaNpc*	mpCharaBase;

	/* レイアウト関連 */
	LayoutBase*	mpAssignLayout;
	LayoutBase* mpAssignSystemLayout;

	tModelAnimInfo*		mpModelAnimWork;
	tLayoutAnimInfo*	mpLayoutAnimWork;

	int*		mpIndex;

	// シフトカメラポジション (Offsetとは違う)
	VECTOR		mShifCamPos;
	CsvAccesser* mpCsvAccesser;

};

#endif //__EVENT_MANAGER_H__

 /**** end of file ****/
