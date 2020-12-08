/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ステージ固有の処理
 * @author SPATZ.
 * @data   2015/08/26 22:06:24
 */
//---------------------------------------------------
#ifndef __STAGE_FUNC_047_H__
#define __STAGE_FUNC_047_H__

#include "StageFuncBase.h"
#include "AppLib\Resource\CsvAccesser.h"

/*=====================================*
  class
 *=====================================*/
class StageFunc047 : public StageFuncBase {
public:
	/* 定数 */

	/* 関数 */
	StageFunc047();
	~StageFunc047();

	void		SetupBeforeLoad(void);
	void		SetupAfterLoad(int& rPhase);
	void		Update(int counter);
	void		SetPatternLight(int pattern){}
	void		SetPatternObject(int pattern){}
	void		DestroyBuf(void);

private:

	void		destroy(void);

	/* 変数 */
public:
	/*
	USHORT	mGroupCount;
	SHORT	mScrollX;
	SHORT	mScrollY;
	BYTE	mIsReverse;
	int		mRollStringColor;
	*/
	LayoutBase*		mpTitleLog;
	/*
	BOOL	mIsEndScroll;
	*/

	/* スタッフロール関連 */
	struct tStaffRollWork {
		CsvAccesser*	mpCsv;					// CSVファイル
		BYTE			mCurCsvFileIndex;		// 現在読み込んでいるcsvファイルインデックス
		BOOL			mIsEndReadCsv;			// CSVファイル読み込み完了フラグ

		short			mCurCsvRowIndex;		// 現在読み込み中の行数
		short			mTextPosX;				// テキスト位置X
		short			mShiftTextPosY;			// シフトのテキスト位置Y
		int				mTextColor;				// テキスト色

	} mStaffroll;

	int		mEffHndSky;
};

#endif // __STAGE_FUNC_047_H__

/**** end of file ****/


