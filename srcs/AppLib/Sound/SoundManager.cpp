/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * サウンド管理クラス
 * @author SPATZ.
 * @data   2012/02/05 17:19:19
 */
//---------------------------------------------------
#ifdef _DEBUG
#include "AppLib/Input/InputManager.h"
#endif // _DEBUG

#include "SoundManager.h"
#include "dxlib.h"

// ダミー
static SoundComponent	sDummyComponent;
static char				sNowPlayMusic[32] = {0};

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/ SoundManager* const
SoundManager::GetInstance(void)
{
	static SoundManager sInstance;
	return &sInstance;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/ SoundComponent* const
SoundManager::GetVcComponent(const char* cmpName)
{
	tComponentWork* p_work;

	for (unsigned int iI = 0; iI < SoundManager::GetInstance()->GetVcComponentList().size(); iI ++) {
		p_work = &SoundManager::GetInstance()->GetVcComponentList().at(iI);
		if (strcmp(cmpName, p_work->mName) == 0) {
			return p_work->mpComponent;
		}
	}
	return &sDummyComponent;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
/*static*/ SoundComponent* const
SoundManager::GetSndComponent(void)
{
	tComponentWork* p_work = &SoundManager::GetInstance()->GetVcComponentList().at(0);
	return p_work->mpComponent;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SoundManager::LoadSoundComponent(const int soundNum)
{
	LoadVoiceComponent((char *)"Sound", soundNum);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SoundManager::LoadVoiceComponent(char* cmpName, const int soundNum)
{
	// 多重読み込み禁止
	if (CheckLoadComponent(cmpName) == true) {
		return;
	}

	SoundComponent* p_cmp = NULL;
	p_cmp = new SoundComponent();

	// 名前用ワークを生成
	char* p_name = NULL;
	size_t size = strlen(cmpName);
	p_name = new char[(size + 1)];
	memset(p_name, 0x00, (size + 1));
	strcpy_s(p_name, (size + 1), cmpName);
	p_name[size] = '\0';
	
	p_cmp->LoadSound(p_name, soundNum);
	
	tComponentWork work;
	work.mName = p_name;
	work.mpComponent = p_cmp;

	mComponentList.push_back(work);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
SoundManager::CheckLoadComponent(char* cmpName)
{
	for (unsigned int iI = 0; iI < mComponentList.size(); iI ++) {
		if (strcmp(mComponentList.at(iI).mName, cmpName) == 0) {
			return true;
		}
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SoundManager::Update(void)
{
	if (mSoundFadeWork.mIsFade == TRUE) {
		float work = mVolume;
		work += mSoundFadeWork.mAddVolume;
		if (((mSoundFadeWork.mAddVolume > 0) && (work >= mSoundFadeWork.mTargetVolume)) ||
			((mSoundFadeWork.mAddVolume < 0) && (work <= mSoundFadeWork.mTargetVolume))) {
			work = (float)mSoundFadeWork.mTargetVolume;
			mSoundFadeWork.mIsFade = FALSE;
		}
		SetVolumeBGM((BYTE)work);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SoundManager::PlayBGM(const char* musicName)
{
#ifdef _DEBUG
	if (CHECK_PRESS_JOY(JOY_INPUT_UP)) {
		StopMusic();
		return;
	}
#endif // _DEBUG

	// 再生中のBGMと指定のBGMが異なる場合は
	if (strcmp(sNowPlayMusic, musicName) != 0) {
		const char* music_root_name = "data/ogg";
		char music_name[64] = {0};
		sprintf_s(music_name, sizeof(music_name), "%s/%s", music_root_name, musicName);

		//			PlayMusic("data/ogg/Zelda.ogg", DX_PLAYTYPE_LOOP);
		PlayMusic(music_name, DX_PLAYTYPE_LOOP);

		// 現在再生中のBGM名を保管
		strcpy_s(sNowPlayMusic, sizeof(sNowPlayMusic), musicName);

		SetVolumeBGM(kSOUND_VOLUME_DEFAULT);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SoundManager::StopBGM(void)
{
	StopMusic();
	memset(sNowPlayMusic, 0x00, sizeof(sNowPlayMusic));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SoundManager::SetVolumeBGM(BYTE setVolume)
{
	mVolume = setVolume;

	float volume = (float)mVolume * ((float)mMusicVolScale / 100.0f);
	SetVolumeMusic((int)volume);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
BYTE
SoundManager::GetVolumeBGM(void)
{
	return mVolume;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
SoundManager::StartFadeVolumeBGM(BYTE setVolume, float fadeSec)
{
	float frame = fadeSec * 60.0f;
	int diff = setVolume - mVolume;

	if (diff != 0) {
		mSoundFadeWork.mIsFade		= TRUE;
		mSoundFadeWork.mTargetVolume= setVolume;
		mSoundFadeWork.mAddVolume	= ((float)diff / frame);
		if ((int)mSoundFadeWork.mAddVolume == 0) {
			if (diff > 0) {
				mSoundFadeWork.mAddVolume = 1.0f;
			}
			else {
				mSoundFadeWork.mAddVolume = -1.0f;
			}				
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
SoundManager::SoundManager()
{
	mComponentList.clear();

	mVolume			= 255;

	mMusicVolScale = 100;
	mSoundVolScale = 100;
	mVoiceVolScale = 100;

	memset(&mSoundFadeWork, 0, sizeof(mSoundFadeWork));
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
SoundManager::~SoundManager()
{
	for (unsigned int iI = 0; iI < mComponentList.size(); iI ++) {

		delete[] mComponentList.at(iI).mName;
		mComponentList.at(iI).mName = NULL;

		delete mComponentList.at(iI).mpComponent;
		mComponentList.at(iI).mpComponent = NULL;
	}
}


 /**** end of file ****/
