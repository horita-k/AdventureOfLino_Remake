/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * サウンドコンポーネントクラス
 * @author SPATZ.
 * @data   2012/02/05 17:15:58
 */
//---------------------------------------------------
#include <stdio.h>
#include "AppLib/Basic/Basic.h"
#include "dxlib.h"
#include "SoundComponent.h"

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
SoundComponent::SoundComponent()
{
	mpSoundIndex	= NULL;
	mSoundMax		= 0;
	mVolumeScale	= 1.0f;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
SoundComponent::~SoundComponent()
{
	if (mpSoundIndex != NULL) {
		for (int iI = 0; iI < mSoundMax; iI ++) {
			DeleteSoundMem(iI);
		}
		delete[] mpSoundIndex;
		mpSoundIndex = NULL;
		mSoundMax = 0;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SoundComponent::LoadSound(char* componentName, const int soundNum)
{
	mpSoundIndex = new int[soundNum];
	APP_ASSERT_PRINT(mpSoundIndex != NULL, 
					 "Failed new mpSoundIndex[%d]\n", soundNum);

	mSoundMax = soundNum;
	
	// ボイス読み込み
	char chara_root_dir[128] = {0};

	char sound_name[128] = {0};

	if (strcmp(componentName, "Sound") == 0) {
		for (int iI = 0; iI < soundNum; iI ++) {
			memset(sound_name, 0x00, sizeof(sound_name));
			sprintf_s(sound_name, sizeof(sound_name), "data/Sound/Sound%02d.ogg", iI);
			mpSoundIndex[iI] = LoadSoundMem(sound_name);
		}
	} else {

		sprintf_s(chara_root_dir, sizeof(chara_root_dir), "data/Chara/%s", componentName);

		// 参照ファイルを見つける
		sprintf_s(sound_name, sizeof(sound_name), "%s/Voice/Refer.txt", chara_root_dir);
		
		int fhnd;
		fhnd = FileRead_open(sound_name, FALSE);

		APP_ASSERT_PRINT((fhnd != 0),
						 "Failed open charactor voice refference text file : %s \n", sound_name);

		// 参照ファイルからデータを読み込む
		char reffer_buf[32] = {0};
		FileRead_gets(reffer_buf, sizeof(reffer_buf), fhnd);
			
		sprintf_s(chara_root_dir, sizeof(chara_root_dir), "data/VC/%s", reffer_buf);

		// 実際に存在するファイルから読み込む
		for (int iI = 0; iI < soundNum; iI ++) {
			memset(sound_name, 0x00, sizeof(sound_name));
			sprintf_s(sound_name, sizeof(sound_name), "%s/Voice%02d.wav", chara_root_dir, iI);
			mpSoundIndex[iI] = LoadSoundMem(sound_name);
		}

	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
SoundComponent::PlaySound(int soundIndex)
{
	APP_ASSERT_PRINT(soundIndex < mSoundMax,
					 "PlaySound index is Over mSoundMax : %d\n", soundIndex);
	StopSoundMem(mpSoundIndex[soundIndex]);

	return PlaySoundMem(mpSoundIndex[soundIndex], DX_PLAYTYPE_BACK, TRUE);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SoundComponent::StopSound(int soundIndex)
{
	APP_ASSERT_PRINT(soundIndex < mSoundMax,
					 "PlaySound index is Over mSoundMax : %d\n", soundIndex);
	StopSoundMem(mpSoundIndex[soundIndex]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SoundComponent::StopAll(void)
{
	for (int iI = 0; iI < mSoundMax; iI ++) {
		StopSound(iI);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
SoundComponent::IsPlay(int soundIndex)
{
	APP_ASSERT_PRINT(soundIndex < mSoundMax,
					 "PlaySound index is Over mSoundMax : %d\n", soundIndex);
	if (CheckSoundMem(mpSoundIndex[soundIndex]) == 1) {
		return true;
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SoundComponent::SetVolume(int soundIndex, int setVolume)
{
	APP_ASSERT_PRINT(soundIndex < mSoundMax,
					 "PlaySound index is Over mSoundMax : %d\n", soundIndex);
	ChangeVolumeSoundMem((int)((float)setVolume * mVolumeScale), mpSoundIndex[soundIndex]);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SoundComponent::SetVolumeScale(float volumeScale)
{
	int volume = 0;
	volume = (int)(volumeScale * 255.0f);

	mVolumeScale = volumeScale;
	for (int iI = 0; iI < mSoundMax; iI ++) {
		ChangeVolumeSoundMem(volume, mpSoundIndex[iI]);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SoundComponent::SetFrequency(int soundIndex, int setFrequency)
{
	APP_ASSERT_PRINT(soundIndex < mSoundMax,
					 "PlaySound index is Over mSoundMax : %d\n", soundIndex);
	SetFrequencySoundMem(setFrequency, mpSoundIndex[soundIndex]);
}

 /**** end of file ****/
