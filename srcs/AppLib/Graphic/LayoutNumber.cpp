/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * 画像ベースクラス
 * @author SPATZ.
 * @data   2012/02/18 17:56:36
 */
//---------------------------------------------------
#include "AppLib/Basic/Basic.h"
#include "AppLib/Resource/ResourceLayout.h"
#include "AppLib/Graphic/GraphicManager.h"

#include "LayoutNumber.h"

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
LayoutNumber::LayoutNumber()
{
	memset(mLayoutNumberHandle, 0x00, sizeof(mLayoutNumberHandle));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
LayoutNumber::~LayoutNumber()
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutNumber::Create(void)
{
	LayoutBase::Create();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutNumber::Setup(void)
{
	LayoutBase::Setup();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutNumber::Update(void)
{
	LayoutBase::Update();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutNumber::Destroy(void)
{
	LayoutBase::Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutNumber::LinkLayout(int setNumber, const char* fileName)
{
	APP_ASSERT_PRINT((setNumber >= 0) && (setNumber < 10),
					 "setNumber = %d is Invalid Value \n", setNumber);
	mLayoutNumberHandle[setNumber] = ResourceLayout::GetInstance()->GetHandle(fileName);
	APP_ASSERT_PRINT(mLayoutNumberHandle[setNumber] != -1,
					 "Failed LinkLayout : %s \n", fileName);
	if (setNumber == 0) {
		SetNumber(0);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutNumber::UnlinkLayout(void)
{
	for (int iI = 0; iI < 10; iI ++) {
		mLayoutNumberHandle[iI] = -1;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
LayoutNumber::SetNumber(int number)
{
	APP_ASSERT_PRINT(number < 10,
					 "number is Invalid Value \n", number);
	mLayoutHandle = mLayoutNumberHandle[number];
}

 /**** end of file ****/
