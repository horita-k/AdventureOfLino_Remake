/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * サウンド管理クラス
 * @author SPATZ.
 * @data   2012/02/05 17:19:19
 */
//---------------------------------------------------
#ifndef __SOUND_MANAGER_H__
#define __SOUND_MANAGER_H__

#include <vector>
#include "../Basic/Basic.h"
#include "SoundComponent.h"

struct tComponentWork {
	char*			mName;
	SoundComponent*	mpComponent;
};

// 音量のデフォルト値
#define kSOUND_VOLUME_DEFAULT	(255)

/*=====================================*
 * class
 *=====================================*/
class SoundManager
{
 public:
	static SoundManager* const			GetInstance(void);
	static SoundComponent* const		GetVcComponent(const char* cmpName);
	static SoundComponent* const		GetSndComponent(void);

	void	LoadVoiceComponent(char* cmpName, const int soundNum);
	void	LoadSoundComponent(const int soundNum);

	bool	CheckLoadComponent(char* cmpName);

	std::vector<tComponentWork>&	GetVcComponentList(void)		{ return mComponentList; }

	void	Update(void);

	void	PlayBGM(const char* musicName);
	void	StopBGM(void);
	void	SetVolumeBGM(BYTE setVolume);
	BYTE	GetVolumeBGM(void);

	void	StartFadeVolumeBGM(BYTE setVolume, float fadeSec);

private:
	SoundManager();
	~SoundManager();

	/* 変数 */
	std::vector<tComponentWork>	mComponentList;

	BYTE mVolume;

	BYTE mMusicVolScale;		/* 0 ～ 100 */
	BYTE mSoundVolScale;
	BYTE mVoiceVolScale;

	struct tSoundFadeWork {
		BOOL	mIsFade;
		BYTE	mTargetVolume;
		float	mAddVolume;
	} mSoundFadeWork;

};

#endif // __SOUND_MANAGER_H__

 /**** end of file ****/
