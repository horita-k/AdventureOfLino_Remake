/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * サウンドコンポーネントクラス
 * @author SPATZ.
 * @data   2012/02/05 17:15:58
 */
//---------------------------------------------------
#ifndef __SOUND_COMPONENT_H__
#define __SOUND_COMPONENT_H__

/*=====================================*
 * class
 *=====================================*/
class SoundComponent
{
 public:
	SoundComponent();
	~SoundComponent();

	void		LoadSound(char* componentName, const int soundNum);
	int			PlaySound(int soundIndex);
	void		StopSound(int soundIndex);
	void		StopAll(void);
	bool		IsPlay(int soundIndex);
	void		SetVolume(int soundIndex, int setVolume);
	void		SetVolumeScale(float volumeScale);
	void		SetFrequency(int soundIndex, int setFrequency);

private:
	int*		mpSoundIndex;
	int			mSoundMax;
	float		mVolumeScale;

};

#endif // __SOUND_COMPONENT_H__

 /**** end of file ****/
