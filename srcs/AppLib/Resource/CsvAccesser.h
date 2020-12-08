/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * csvaccesser.h
 * CSVファイルアクセスクラス
 * @author SPATZ.
 * @data   2011/05/15 00:56:35
 */
//---------------------------------------------------
#ifndef __CSV_ACCESSER_H__
#define __CSV_ACCESSER_H__

#include <windows.h>
#include <vector>

/*=====================================*
 * define
 *=====================================*/

/*=====================================*
 * class
 *=====================================*/
class CsvAccesser {
public:
	/* 関数 */
	CsvAccesser();
	~CsvAccesser();

	void Create(void);

	bool		LoadFile(const char* fileName);
	void		DeleteFile(void);

	void		GetLastWriteTime(const char* fileName, FILETIME* fileTime);

	char*		GetString(DWORD row, DWORD column=0);
	float		GetFloat(DWORD row, DWORD column=0);
	int			GetInt(DWORD row, DWORD column=0);
	bool		GetBool(DWORD row, DWORD column=0);
	VECTOR		GetVector(DWORD row, DWORD column=0);

	DWORD		GetRowNum(void);
	DWORD		GetColumnNum(void);

private:
	bool loadResource(const char* fileName);
	static void deleteResource(void);
	void		setupRowList(void);
	void		changeDot2Null(void);

	/* 定数 */
	enum eLoadPhase {
		eLOAD_PHASE_NONE,
		eLOAD_PHASE_LOADING,
		eLOAD_PHASE_FINISH,
		
		eLOAD_PHASE_MAX,
	};

	/* 構造体 */
	// リソース読み込み関連
	int				mFileHandle;
	DWORD			mFileSize;
	void*			mpBuffer;

	struct tStringWork {
		int		mLength;
		char*	mpStr;
	};
	std::vector<std::vector<tStringWork>> mRowList;

	/* 変数 */
	eLoadPhase	mLoadPhase;

};


#endif // __MARCH_H__

/**** end of file ****/



