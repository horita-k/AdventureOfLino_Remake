/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * csvaccesser.cpp
 * CSVファイルアクセスクラス
 * @author SPATZ.
 * @data   2011/05/15 00:59:40
 */
//---------------------------------------------------
//#include <list>

#include "dxlib.h"

#include "AppLib/Basic/Basic.h"
#include "CsvAccesser.h"

/*=====================================*
 * define
 *=====================================*/
//---------------------------------------------------
/**
   コンストラクタ
*/
//---------------------------------------------------
CsvAccesser::CsvAccesser()
{
	// 初期化
	mFileHandle = NULL;
	mFileSize = 0;
	mpBuffer = NULL;

	mLoadPhase		= eLOAD_PHASE_NONE;
}

//---------------------------------------------------
/**
   デストラクタ
*/
//---------------------------------------------------
CsvAccesser::~CsvAccesser()
{
	DeleteFile();
}

//---------------------------------------------------
/**
   生成
*/
//---------------------------------------------------
void
CsvAccesser::Create(void)
{

}

/*=====================================*
 * リソース読み込み関連
 *=====================================*/
//---------------------------------------------------
/**
   同期でリソース読み込み
*/
//---------------------------------------------------
bool
CsvAccesser::LoadFile(const char* fileName)
{
	// 一度破棄を行う
	DeleteFile();

	// ファイル名のポインタを取得
	if (loadResource(fileName) == false) {
		return false;
	}

	setupRowList();

	changeDot2Null();

	mLoadPhase = eLOAD_PHASE_FINISH;

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CsvAccesser::DeleteFile(void)
{
	// ファイル読み込みバッファの破棄
	if (mpBuffer != NULL) {
		delete[] mpBuffer;
		mpBuffer = NULL;
	}

	// 読み込み完了したのでファイルを閉じる
	if (mFileHandle != -1) {
		FileRead_close(mFileHandle);
		mFileHandle = -1;
	}

	// 全要素を削除
	mRowList.clear();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CsvAccesser::GetLastWriteTime(const char* fileName, FILETIME* fileTime)
{
	WIN32_FIND_DATA find_data;
	HANDLE hFile;
	hFile = FindFirstFile(fileName, &find_data);
	APP_ASSERT_PRINT((hFile != INVALID_HANDLE_VALUE),
					 "Failed FindFirstFile(%s) \n", fileName);
	FindClose(hFile);
	FileTimeToLocalFileTime(&find_data.ftLastWriteTime, fileTime);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
char*
CsvAccesser::GetString(DWORD row, DWORD column)
{
	APP_ASSERT_PRINT(row < GetRowNum(),
					 "Select row is OverFlow[%d / %d]\n", row, GetRowNum());
	APP_ASSERT_PRINT(column < GetColumnNum(),
					 "Select column is OverFlow[%d / %d]\n", column, GetColumnNum());
	
	tStringWork str_work;
	// 指定された row, column からデータを抽出
	str_work = mRowList.at(row).at(column);

	return str_work.mpStr;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
float 
CsvAccesser::GetFloat(DWORD row, DWORD column)
{
	return (float)atof(GetString(row, column));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int 
CsvAccesser::GetInt(DWORD row, DWORD column)
{
	return atoi(GetString(row, column));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
CsvAccesser::GetBool(DWORD row, DWORD column)
{
	const char* tmp = GetString(row, column);
	if (strcmp(tmp, "true") == 0 || 
		strcmp(tmp, "TRUE") == 0 ||
		strcmp(tmp, "True") == 0) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
VECTOR
CsvAccesser::GetVector(DWORD row, DWORD column)
{
	VECTOR out = ZERO_VEC;

	char* str = GetString(row, column);

	const char* search_word = " ";
	
	char* next;
	char buf[16];
	DWORD size;

	next = strstr(str, search_word);
	size = (DWORD)next - (DWORD)str;
	memcpy(buf, str, size);
	buf[size] = '\0';
	out.x = (float)atof(buf);

	str = next + 1;

	next = strstr(str, search_word);
	size = (DWORD)next - (DWORD)str;
	memcpy(buf, str, size);
	buf[size] = '\0';
	out.y = (float)atof(buf);

	str = next + 1;

	next = strstr(str, search_word);
	if (next == NULL) {
		out.z = (float)atof(str);
	} else {
		size = (DWORD)next - (DWORD)str;
		memcpy(buf, str, size);
		buf[size] = '\0';
		out.z = (float)atof(buf);
	} 

	return out;
}

//---------------------------------------------------
/**
   
 */
//---------------------------------------------------
DWORD
CsvAccesser::GetRowNum(void)
{
	return mRowList.size();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
DWORD
CsvAccesser::GetColumnNum(void)
{
	return mRowList.at(0).size();
}
	
/*=====================================*
  private method
  *=====================================*/
//---------------------------------------------------
/**
   リソース読み込み
*/
//---------------------------------------------------
bool
CsvAccesser::loadResource(const char* fileName)
{
	BOOL result = FALSE;
	
	char fullname[256] = {0};
	// ファイルを開く

	mFileHandle = FileRead_open(fileName);

	// ファイルの読み込みに失敗した場合はエラーリターン
	if (mFileHandle == 0) {
		PRINT_CONSOLE("Failed FileRead_open %s\n", fullname);
		return false;
	}

	PRINT_CONSOLE("====CsvAccesser::loadResource() mFileHandle : %d \n====");

	// ファイルサイズを取得
	mFileSize = (DWORD)FileRead_size(fileName);
	APP_ASSERT_PRINT(mFileSize != -1,
					 "Failed FileRead_size %s\n", fullname);
	PRINT_CONSOLE("Open CsvAccesser file %s, [%d]byte\n", fileName, mFileSize);

	// ファイル読み込み用バッファのメモリを確保
	mpBuffer = new char[mFileSize];
	APP_ASSERT_PRINT(mpBuffer != NULL,
					 "Failed Alloc FileBuffer, size %d\n", mFileSize);
	
	DWORD readed_byte = 0;
	// ファイル同期読み込み
	FileRead_read((void *)mpBuffer, mFileSize, mFileHandle);
	// 読み込み完了したのでファイルを閉じる
	if (mFileHandle != -1) {
		FileRead_close(mFileHandle);
		mFileHandle = -1;
	}

	return true;
}
	
//---------------------------------------------------
/**
   リソース破棄
*/
//---------------------------------------------------
/*static*/void
CsvAccesser::deleteResource(void)
{
	return;
}

//---------------------------------------------------
/**
   
 */
//---------------------------------------------------
void
CsvAccesser::setupRowList(void)
{
	char* buf = (char *)mpBuffer;

	// 行と列の数を調べる
	DWORD last_index = 0;
	
	std::vector<tStringWork> tmp_list;
	tStringWork str_work;

	for (DWORD iI = 0; iI < mFileSize; iI ++) {

		if (buf[iI] == ',') {
			str_work.mLength	= iI - last_index;
			str_work.mpStr		= &buf[last_index];
			tmp_list.push_back(str_work);

			last_index = iI + 1;
		}

		if (buf[iI] == '\r' && buf[iI + 1] == '\n') {
			
			str_work.mLength	= iI - last_index;
			str_work.mpStr		= &buf[last_index];
			tmp_list.push_back(str_work);

			mRowList.push_back(tmp_list);
			tmp_list.clear();

			last_index = iI + 2;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
CsvAccesser::changeDot2Null(void)
{
	char* buf = (char *)mpBuffer;

	for (DWORD iI = 0; iI < mFileSize; iI ++) {
		if (buf[iI] == ',') {
			buf[iI] = 0;
		}
	}
}

/**** end of file ****/




