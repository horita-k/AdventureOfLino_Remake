/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ユーティリティクラス
 * @author SPATZ.
 * @data   2011/11/27 15:30:43
 */
//---------------------------------------------------
#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "dxlib.h"

/*=====================================*
 * class
 *=====================================*/
class Utility
{
 public:
	enum eQuestion {
		eQUESTION_NONE = 0,
		eQUESTION_YES,
		eQUESTION_NO,

		eQUESTION_MAX,
	};

	static void	CorrectAngleFrom000To360(float* pRadian);

	static float DegToRad(float deg);
	static float RadToDeg(float rad);

	static float GetFPS(void);

	static void	MTXLookAtMatrix(MATRIX* pOut, VECTOR* pPos, VECTOR* pLook, VECTOR* pUp);

	static eQuestion	ExecuteQuestion(bool isSound=true);
	static BYTE			ExecuteSelection3(const char* strA, const char* strB, const char* strC);

	static bool GetFormulaOfTheSolution(float* pOutX, float a, float b, float c);

	static void UpdateLoadingImage(void);

	static void		ExtractMatrixPosition(VECTOR* pOutMtxPos, MATRIX* pInMtx);
	static void		ExtractRotationMatrix(MATRIX* pOutRotMtx, MATRIX* pInMtx, float scale);

	static void RoundOff(float& value);

	static void DumpUsingMemory(void);
	static DWORD GetUsingMemory(void);



};

#endif // __UTILITY_H__

 /**** end of file ****/

