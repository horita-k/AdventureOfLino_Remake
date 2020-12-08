
//---------------------------------------------------
/**
 * 基本処理関数
 * @author SPATZ.
 * @data   2011/12/10 19:09:47	
 */
//---------------------------------------------------
#include <d3dx9.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "Basic.h"

#include "AppLib/Input/InputManager.h"
#include "AppLib/Graphic/GraphicManager.h"

#ifdef _DEBUG
#include "PIC.h"
#endif // _DEBUG

#ifdef _DEBUG
/*static*/ float Basic::sDbgToolValue[12] = {0};
#endif // _DEBUG

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void ConvertMtx(D3DXMATRIX* pOut, const MATRIX* pIn)
{
	pOut->_11 = pIn->m[0][0];
	pOut->_12 = pIn->m[0][1];
	pOut->_13 = pIn->m[0][2];
	pOut->_14 = pIn->m[0][3];

	pOut->_21 = pIn->m[1][0];
	pOut->_22 = pIn->m[1][1];
	pOut->_23 = pIn->m[1][2];
	pOut->_24 = pIn->m[1][3];

	pOut->_31 = pIn->m[2][0];
	pOut->_32 = pIn->m[2][1];
	pOut->_33 = pIn->m[2][2];
	pOut->_34 = pIn->m[2][3];

	pOut->_41 = pIn->m[3][0];
	pOut->_42 = pIn->m[3][1];
	pOut->_43 = pIn->m[3][2];
	pOut->_44 = pIn->m[3][3];
}

static const char* strLogFileName = "DebugLog.txt";
static FILE* sFp = NULL;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
Basic::OpenDebugLog(void)
{
	fopen_s(&sFp, strLogFileName, "a");
	APP_ASSERT_PRINT(sFp != NULL,
					 "Failed fopen() : %s \n", strLogFileName);

   time_t timer;

   /* 現在時刻の取得 */
   time(&timer);

   /* 現在時刻を文字列に変換して表示 */
   Basic::WriteDebugLog("********************************\n");
//   Basic::WriteDebugLog(ctime(&timer));
   Basic::WriteDebugLog("********************************\n");
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
Basic::CloseDebugLog(void)
{
	if (sFp != NULL) {
		fclose(sFp);
		sFp = NULL;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
Basic::WriteDebugLog(const char* str)
{
	APP_ASSERT(sFp != NULL,
			   "sFp is NULL\n");
	APP_ASSERT(str != NULL,
			   "Setting str is NULL\n");
	fputs(str, sFp);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
Basic::FlushDebugLog(void)
{
	fflush(sFp);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
Basic::QuickSort(int* pArray, int beginIndex, int endIndex)
{
	int i, j;
	int pivot;   /* 枢軸 */
	int work;
		
	pivot = pArray[ ( beginIndex + endIndex ) / 2 ];  /* 範囲の中央にある要素を枢軸とする */
	i = beginIndex;
	j = endIndex;
	
	while( 1 )
	{
		while( pArray[i] < pivot ){ ++i; }  /* 枢軸以上の値が見つかるまで右方向へ進めていく */
		while( pArray[j] > pivot ){ --j; }  /* 枢軸以下の値が見つかるまで左方向へ進めていく */
		if( i >= j ){ break; }  /* 左右から進めてきたiとjがぶつかったらループを終える */
		
		/* 要素を交換する */
		work = pArray[i];
		pArray[i] = pArray[j];
		pArray[j] = work;
		
		/* 次回に備えて、注目点をずらす */
		i++;
		j--;
	}
	
	/* 枢軸の左側に要素が２つ以上あれば、その部分についてクイックソートを再帰させる */
	if( beginIndex < i - 1 ){ QuickSort( pArray, beginIndex, i - 1 ); }
	/* 枢軸の右側に要素が２つ以上あれば、その部分についてクイックソートを再帰させる */
	if( j + 1 < endIndex ){ QuickSort( pArray, j + 1, endIndex ); }
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
Basic::QuickSortEx(int* pArray, int* pArrayIndex, int beginIndex, int endIndex)
{
	int i, j;
	int pivot;   /* 枢軸 */
	int work;
	int work_index;
		
	pivot = pArray[ ( beginIndex + endIndex ) / 2 ];  /* 範囲の中央にある要素を枢軸とする */
	i = beginIndex;
	j = endIndex;
	
	while( 1 )
	{
		while( pArray[i] < pivot ){ ++i; }  /* 枢軸以上の値が見つかるまで右方向へ進めていく */
		while( pArray[j] > pivot ){ --j; }  /* 枢軸以下の値が見つかるまで左方向へ進めていく */
		if( i >= j ){ break; }  /* 左右から進めてきたiとjがぶつかったらループを終える */
		
		/* 要素を交換する */
		work = pArray[i];
		work_index = pArrayIndex[i];
		pArray[i] = pArray[j];
		pArray[j] = work;
		pArrayIndex[i] = pArrayIndex[j];
		pArrayIndex[j] = work_index;
		
		/*
		if ((pArray[i] == -33686019) || (pArray[j] == -33686019)) {
			int tmp;
			tmp = 0;
		}
		*/

		/* 次回に備えて、注目点をずらす */
		i++;
		j--;
	}
	
	/* 枢軸の左側に要素が２つ以上あれば、その部分についてクイックソートを再帰させる */
	if( beginIndex < i - 1 ){ QuickSortEx( pArray, pArrayIndex, beginIndex, i - 1 ); }
	/* 枢軸の右側に要素が２つ以上あれば、その部分についてクイックソートを再帰させる */
	if( j + 1 < endIndex ){ QuickSortEx( pArray, pArrayIndex, j + 1, endIndex ); }
}

//---------------------------------------------------
/**
   標準アサート
 */
//---------------------------------------------------
void
Basic::HoliAssert(char* method, char* str)
{
	int ret = 0;
	char mesg[256];
	memset(mesg,'\0',sizeof(mesg));
	sprintf_s(mesg, sizeof(mesg), "%s \n %s\n", method, str);

	OutputDebugString("====HoliAssert====");
	OutputDebugString("\n");
	OutputDebugString(mesg);

	ret = MessageBox(NULL, mesg, "====HoliAssert====", MB_ABORTRETRYIGNORE|MB_ICONEXCLAMATION);
	if (ret == IDABORT) {
#ifdef _DEBUG
			Basic::FlushDebugLog();
#endif // _DEBUG
		//		abort();
		//		exit(0);
	} else if (ret == IDRETRY) {
		// 現状は何も処理をいれないz
	} else if (ret == IDIGNORE) {

	}
}

//---------------------------------------------------
/**
   システムエラー
 */
//---------------------------------------------------
void
Basic::SystemError(char* method, char* str)
{
	int ret = 0;
	char mesg[256];
	memset(mesg,'\0',sizeof(mesg));
	sprintf_s(mesg, sizeof(mesg), "%s \n %s\n\n", method, str);

	OutputDebugString("==== System Error ====");
	OutputDebugString("\n");
	OutputDebugString(mesg);

	ret = MessageBox(NULL, mesg, "==== System Error ====", MB_OK);

	// 意図的に強制終了させる
	InputManager::GetInstance()->EnablePressKey(eKEY_ESCAPE);
	
	/* 強制終了 */
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
Basic::SystemMessage(char* header_msg, char* explain_msg)
{
	int ret = 0;

	ret = MessageBox(NULL, explain_msg, header_msg, MB_OK);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/bool
Basic::SystemMessageAnswer(char* header_msg, char* explain_msg)
{
	int ret = 0;
	ret = MessageBox(NULL, explain_msg, header_msg, MB_OKCANCEL);

	if (ret == IDOK) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
 * 同期でプロセスを起動させる
 */
//---------------------------------------------------
/*static*/BOOL
Basic:: CreateProcessSync(const char* szFileName)
{
    PROCESS_INFORMATION pi;
    STARTUPINFO si;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    int nRet = CreateProcess(
            NULL, (LPTSTR)szFileName, NULL, NULL, FALSE,
            NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi
            );

    if (nRet == 0) // 起動に失敗した場合
        return FALSE;
    
    // 起動したプロセスが終了するまで待機
    while (WaitForSingleObject(pi.hProcess, 0) != WAIT_OBJECT_0) {
        Sleep(100);
    }

    return FALSE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/**static*/int
Basic::IndexOf(const char* str, const char keyword)
{
	int length = strlen(str);
	for (int iI = 0; iI < length; iI ++) {
		if (str[iI] == keyword) {
			return iI;
		}
	}
	// 指定の文字が見つからなかった場合
	return -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/int
Basic::LastIndexOf(const char* str, const char keyword)
{
	int length = strlen(str);
	for (int iI = length - 1; iI >= 0; iI --) {
		if (str[iI] == keyword) {
			return iI;
		}
	}
	// 指定の文字が見つからなかった場合
	return -1;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
Basic::ShowDump(unsigned char* addr, unsigned int size)
{
	PRINT_CONSOLE(" ==== Start Dump ==== \n");
	
	const unsigned int showlen = 8;
	for (unsigned int iI = 0; iI < size; iI += showlen) {
		unsigned char buf[showlen] = {0};
		if (size - iI > showlen) {
			memcpy(&buf[0], &addr[iI], showlen);
		} else {
			memcpy(&buf[0], &addr[iI], (size - iI));
		}
		PRINT_CONSOLE("[0x%x] : %x %x %x %x   %x %x %x %x  \n", (unsigned int)addr + iI, 
					  buf[iI+0],buf[iI+1],buf[iI+2],buf[iI+3],buf[iI+4],buf[iI+5],buf[iI+6],buf[iI+7]);
	}
}

/*static*/void 
Basic::RenderMessage(const char* name, char* msg1, char* msg2, char* msg3, char* msg4, int color)
{
	char* tmpmassgearrayABC[4] = {};
	tmpmassgearrayABC[0] = msg1;
	tmpmassgearrayABC[1] = msg2;
	tmpmassgearrayABC[2] = msg3;
	tmpmassgearrayABC[3] = msg4;
	GraphicManager::GetInstance()->RenderMessage((char*)name, tmpmassgearrayABC, color);
}

 /**** end of file ****/
