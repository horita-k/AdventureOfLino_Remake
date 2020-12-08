/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ユーティリティクラス
 * @author SPATZ.
 * @data   2011/11/27 15:32:16	
 */
//---------------------------------------------------
#include <math.h>

#include "Utility.h"
#include "dxlib.h"
#include "AppLib/Basic/Basic.h"
#include "AppLib/Sound/SoundManager.h"
#include "AppLib/Graphic/GraphicManager.h"
#include "AppLib/Input/InputManager.h"
#include "Scene/SceneManager.h"

#include "DataDefine/VoiceDef.h"

static BYTE sSelect = 0;

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
Utility::CorrectAngleFrom000To360(float* pRadian)
{
	//	if (pRadian == NULL) return;
	APP_ASSERT( (pRadian != NULL),
				"pRadian is NULL \n");

	if (*pRadian >( PHI_F * 2.0f) ) {
		*pRadian -= (PHI_F * 2.0f);
	}
	if (*pRadian < 0.0f) {
		*pRadian += (PHI_F * 2.0f);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/float
Utility::DegToRad(float deg)
{
	return deg / 180.0f * PHI_F;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/float
Utility::RadToDeg(float rad)
{
	return rad / PHI_F * 180.0f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/float
Utility::GetFPS(void)
{
	static int count = 0;
	static int time = 0;
	static int f[60];
	static int ave = 0;

	f[count%60] = GetNowCount() - time;
	time = GetNowCount();
	if (count % 60 == 59) {
		ave = 0;
		for (int iI = 0; iI < 60; iI ++) {
			ave += f[iI];
		}
		ave /= 60;
	}
	count ++;
	if (ave == 0) {
		return 0;
	}
	return 1000.0f/(float)ave;
}

//---------------------------------------------------
/**
   座標・注視点・空方向を指定して行列を求める
 */
//---------------------------------------------------
/*static*/void
Utility::MTXLookAtMatrix(MATRIX* pOut, VECTOR* pPos, VECTOR* pLook, VECTOR* pUp)
{
	VECTOR x_axis, y_axis, z_axis, tmp;

	tmp = VNorm(*pUp);

	z_axis = VSub(*pLook, *pPos);
	z_axis = VNorm(z_axis);		// 単位化
	x_axis = VCross(tmp, z_axis);
	if (VSize(x_axis) == 0.0f) {
		x_axis = VGet(1.0f, 0.0f, 0.0f);
	} else {
		x_axis = VNorm(x_axis);	// 単位化
	}
	y_axis = VCross(z_axis, x_axis);
	y_axis = VNorm(y_axis);		// 正規化

	pOut->m[0][0] = x_axis.x;	pOut->m[0][1] = x_axis.y;	pOut->m[0][2] = x_axis.z;	pOut->m[0][3] = 0.0f;
	pOut->m[1][0] = y_axis.x;	pOut->m[1][1] = y_axis.y;	pOut->m[1][2] = y_axis.z;	pOut->m[1][3] = 0.0f;
	pOut->m[2][0] = z_axis.x;	pOut->m[2][1] = z_axis.y;	pOut->m[2][2] = z_axis.z;	pOut->m[2][3] = 0.0f;
	pOut->m[3][0] = 0.0f;		pOut->m[3][1] = 0.0f;		pOut->m[3][2] = 0.0f;		pOut->m[3][3] = 1.0f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/Utility::eQuestion
Utility::ExecuteQuestion(bool isSound/*=true*/)
{
	static const BYTE kSELECT_MAX = 2; // (Yes = 0, No = 1);

	/* ファイル選択 */
	if (CHECK_TRIG_JOY(JOY_INPUT_DOWN) || CHECK_TRIG_KEY(eKEY_DOWN)) {
		sSelect = (sSelect + 1 > kSELECT_MAX - 1) ? 0 : sSelect + 1;
		SoundManager::GetSndComponent()->PlaySound(eSE_SELECT_KEY);
	} 
	if (CHECK_TRIG_JOY(JOY_INPUT_UP) || CHECK_TRIG_KEY(eKEY_UP)) {
		sSelect = (sSelect - 1 < 0) ? kSELECT_MAX - 1 : sSelect - 1;
		SoundManager::GetSndComponent()->PlaySound(eSE_SELECT_KEY);
	}				

	if (sSelect == 0) {
		GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_2, 
													"　⇒大丈夫だ、問題ない", 
													GetColor(255, 0, 0));
		GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_3, 
													"　　一番いいのを頼む" , 
													GetColor(128, 128, 128));
	} else {
		GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_2, 
													"　　大丈夫だ、問題ない", 
													GetColor(128, 128, 128));
		GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_3, 
													"　⇒一番いいのを頼む" , 
													GetColor(255, 0, 0));
	}

	if (CHECK_TRIG_JOY(JOY_INPUT_JUMP)) {
		if (isSound) {
			SoundManager::GetSndComponent()->PlaySound(eSE_CANCEL);
		}
		sSelect = 0;
		return eQUESTION_NO;
	}

	if (CHECK_TRIG_ANY_JOY()) {
		if (sSelect == 0) {
			if (isSound) {
				SoundManager::GetSndComponent()->PlaySound(eSE_DECIDE_KEY);
			}
			sSelect = 0;
			
			Utility::UpdateLoadingImage();

			return eQUESTION_YES;
		} else {
			if (isSound) {
				SoundManager::GetSndComponent()->PlaySound(eSE_CANCEL);
			}
			sSelect = 0;
			return eQUESTION_NO;
		}		
	}
	return eQUESTION_NONE;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/BYTE
Utility::ExecuteSelection3(const char* strA, const char* strB, const char* strC)
{
	int color, color_r;
	color = GetColor(128, 128, 128);
	color_r = GetColor(255, 0, 0);

	if (CHECK_TRIG_JOY(JOY_INPUT_DOWN) || CHECK_TRIG_KEY(eKEY_DOWN)) {
		sSelect = SELECT_ROTATION_ADD(sSelect, 3);
		SoundManager::GetSndComponent()->PlaySound(eSE_SELECT_KEY);
	}
	if (CHECK_TRIG_JOY(JOY_INPUT_UP) || CHECK_TRIG_KEY(eKEY_UP)) {
		sSelect = SELECT_ROTATION_SUB(sSelect, 3);
		SoundManager::GetSndComponent()->PlaySound(eSE_SELECT_KEY);
	}

	switch (sSelect) {
	case 0:
		GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_1,	(char *)strA, color_r);
		GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_2,	(char *)strB, color);
		GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_3,	(char *)strC, color);
		break;
	case 1:
		GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_1,	(char *)strA, color);
		GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_2,	(char *)strB, color_r);
		GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_3,	(char *)strC, color);
		break;
	case 2:
		GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_1,	(char *)strA, color);
		GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_2,	(char *)strB, color);
		GraphicManager::GetInstance()->RenderString(GraphicManager::eSTRING_TYPE_MESSAGE_3,	(char *)strC, color_r);
		break;
	};

	if (CHECK_TRIG_JOY(JOY_INPUT_JUMP)) {
		sSelect = 2;
		return sSelect;
	}

	if (CHECK_TRIG_ANY_JOY()) {
		BYTE tmp = sSelect;
		sSelect = 0;
		return tmp;
	}
	return 0xFF;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/bool
Utility::GetFormulaOfTheSolution(float* pOutX, float a, float b, float c)
{
	float in_root, x1, x2;
	in_root = (b * b) - (4 * a * c);
	if (in_root < 0) {
		// 解が見当たらない false
		return false;
	}
	x1 = (-b + sqrt(in_root)) / (2 * a);
	x2 = (-b - sqrt(in_root)) / (2 * a);
	if (x1 > x2) {
		*pOutX = x1;
	} else {
		*pOutX = x2;
	}
	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
Utility::UpdateLoadingImage(void)
{
	//<! 何かキーボード入力されていたらロード画面をキーボード表示に切り替える
	SceneManager::GetInstance()->ChangeLoadingImage(SceneManager::eLOAD_IMAGE_JOY);
	for (int iI = 0; iI < eKEY_MAX; iI ++) {
		if (CHECK_PRESS_KEY(iI)) {
			SceneManager::GetInstance()->ChangeLoadingImage(SceneManager::eLOAD_IMAGE_KEY);
			break;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
Utility::ExtractMatrixPosition(VECTOR* pOutMtxPos, MATRIX* pInMtx)
{
	pOutMtxPos->x = pInMtx->m[3][0];
	pOutMtxPos->y = pInMtx->m[3][1];
	pOutMtxPos->z = pInMtx->m[3][2];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
Utility::ExtractRotationMatrix(MATRIX* pOutRotMtx, MATRIX* pInMtx, float scale)
{
	*pOutRotMtx = *pInMtx;
	pOutRotMtx->m[3][0] = 0.0f;
	pOutRotMtx->m[3][1] = 0.0f;
	pOutRotMtx->m[3][2] = 0.0f;
	*pOutRotMtx = MScale(*pOutRotMtx, scale);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
Utility::RoundOff(float& value)
{
	int int_value = (int)value;
	float tmp = value - (float)int_value;
	
	if (tmp >= 0.5f) {
		value = (float)int_value + 1.0f;
	}
	else {
		value = (float)int_value;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/void
Utility::DumpUsingMemory(void)
{
	MEMORYSTATUSEX m = {sizeof m};
	GlobalMemoryStatusEx(&m);
	DWORD TotalVirtual = (DWORD)(m.ullTotalVirtual / (1024 * 1024) );
	DWORD AvailVirtual = (DWORD)(m.ullAvailVirtual / (1024 * 1024) );
	DWORD UsingMemory  = (unsigned) (((512 * 1024) + (m.ullTotalVirtual - m.ullAvailVirtual)) / (1024 * 1024));

	PRINT_CONSOLE(" * using mem : %u/%u MB(%.1f %%):Remain %u MB(%.1f %%)) \n", 
				  UsingMemory, 
				  TotalVirtual, 
				  (100.0f * (float)UsingMemory / (float)TotalVirtual), 
				  AvailVirtual, 
				  (100.0f * (float)AvailVirtual / (float)TotalVirtual) );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/DWORD
Utility::GetUsingMemory(void)
{
	MEMORYSTATUSEX m = {sizeof m};
	GlobalMemoryStatusEx(&m);
	return (unsigned) (((512 * 1024) + (m.ullTotalVirtual - m.ullAvailVirtual)) / (1024 * 1024));
}

 /**** end of file ****/

