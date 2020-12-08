/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * path.cpp
 * パス解決クラス
 * @author SPATZ.
 * @data   2011/04/02 16:42:45
 */
//---------------------------------------------------
/*=====================================*
 * include
 *=====================================*/
#include <windows.h>
#include <stdio.h>

#include "Basic.h"
#include "Path.h"

/*=====================================*
 * static
 *=====================================*/
/*static*/char Path::mRootDirName[256] = {0};

/*=====================================*
 * class
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
Path::Path()
{
	memset(mRootDirName, '\0', sizeof(mRootDirName));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/bool
Path::SetupRootDirectory(const char* pFindDirectory)
{
	char cur_dir[256] = {0};
	HANDLE handle = NULL;
	WIN32_FIND_DATA fd;
	BOOL result = FALSE;
	memset(&fd, 0, sizeof(fd));
	// カレントディレクトリから指定ディレクトリが見つかるまでさかのぼって検索する
	while (1) {
		handle = FindFirstFile(pFindDirectory, &fd);
		if (handle != INVALID_HANDLE_VALUE) {
			GetCurrentDirectory(sizeof(cur_dir), cur_dir);
			sprintf_s(mRootDirName, sizeof(mRootDirName), "%s/%s", cur_dir, pFindDirectory);
			break;
		}
		result = SetCurrentDirectory("../");
		char info[256] = {0};
		size_t length;
		GetCurrentDirectory(sizeof(info), info);
		// ドライブディスクまでさかのぼったかを判定
		length = strlen(info);
		if (info[length-1] == '\\') {
			result = FALSE;
		}
		if (result == FALSE) {
			MessageBox(NULL, "Not Found DataDirectory", "====HoliAssert====", MB_OK);
			return false;
		}
	}
	result = FindClose(handle);
	if (result == FALSE) {
		MessageBox(NULL, "Faild FindClose", "====HoliAssert====", MB_OK);
		return false;
	}

	// 指定のデータディレクトリの発見に成功
	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
Path::ConvertFullFilename(char* outName, size_t outNameSize, const char* fileName)
{
	APP_ASSERT((strlen(mRootDirName) != 0),
			   "mRootDirName is not Setup\n");

	// 内部で文字列を初期化する
	memset(outName, '\0', outNameSize); 

	// 元々フルパス指定の場合は変更しない
	if ((strlen(fileName) >= 3) &&
		(fileName[1] == ':') &&
		(fileName[2] == '\\')) {
		strcpy_s(outName, outNameSize, fileName);
	} else {
		char tmp[256] = {0};
		sprintf_s(tmp, sizeof(tmp), "%s/%s", mRootDirName, fileName);
		
		strcpy_s(outName, outNameSize, tmp);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
Path::ConvertLocalFilename(char* outName, size_t outNameSize, const char* fileName)
{
	APP_ASSERT((strlen(mRootDirName) != 0),
			   "mRootDirName is not Setup\n");

	int cmp_value =  memcmp(mRootDirName, fileName, strlen(mRootDirName));
	APP_ASSERT_PRINT(cmp_value == 0,
					 "ConvertLocalFilename is Invalid name : %s\n", fileName);

	DWORD address = (DWORD)fileName + (DWORD)strlen(mRootDirName) + 1;
	DWORD length = strlen(fileName) - strlen(mRootDirName);

	APP_ASSERT_PRINT(length < outNameSize,
					 "Select outNameSize %d is Smaller than getBufferSize %d\n", outNameSize, length);

	memcpy(outName, (void *)address, length);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
Path::GetDirectoryName(char* outName, size_t outNameSize, const char* fileName)
{
	int last;
	int tmpA = Basic::LastIndexOf(fileName, '/');
	int tmpB = Basic::LastIndexOf(fileName, '\\');
	last = (tmpA > tmpB) ? tmpA : tmpB;
	APP_ASSERT_PRINT((last != -1),
					 "Failed GetDirectoryName : %s\n", fileName);
	APP_ASSERT((DWORD)last < outNameSize,
			   "Failed GetDirectoryName, enough copy dest buffer\n");
	// ファイル名からパスだけをもぎ取る
	memcpy(outName, fileName, (sizeof(char) * last));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
Path::GetOnlyFileName(char* outName, size_t outNameSize, const char* fileName)
{
	int last;
	int tmpA = Basic::LastIndexOf(fileName, '/');
	int tmpB = Basic::LastIndexOf(fileName, '\\');
	last = (tmpA > tmpB) ? tmpA : tmpB;
	APP_ASSERT_PRINT((last != -1),
					 "Failed GetDirectoryName : %s\n", fileName);
	APP_ASSERT((DWORD)last < outNameSize,
			   "Failed GetDirectoryName, enough copy dest buffer\n");
	// ファイル名からパスだけをもぎ取る
	DWORD address = (DWORD)fileName + (DWORD)last + 1;
	DWORD length = strlen(fileName);

	memcpy(outName, (void *)address, length - last);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/int
Path::GetFileNum(const char* filePath)
{
	int file_cnt = 0;

	// 選択フォルダのファイル数を指定する
	WIN32_FIND_DATA findData;
	HANDLE hnd = NULL;
	hnd = FindFirstFile(filePath, &findData);
	APP_ASSERT( (hnd !=  INVALID_HANDLE_VALUE),
				"Failed FindFirstFile() \n");
	do {
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			file_cnt ++;
		}
	} while (FindNextFile(hnd, &findData));

	FindClose(hnd);	

	return file_cnt;
}

/**** end of file ****/


