/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * Effekseer機能管理クラス
 * @author SPATZ.
 * @data   2013/06/15 17:11:30
 */
//---------------------------------------------------
#ifndef __EFFEKSEER_APP_MANAGER_H__
#define __EFFEKSEER_APP_MANAGER_H__

#include <Effekseer.h>
#include <EffekseerRendererDX9.h>

#include "DxLib.h"

#include "EffectType.h"

typedef		::Effekseer::Handle		EFK_HANDLE;

/*=====================================*
 * define
 *=====================================*/
#define STOP_EFFEKSEER(hnd, fade)							\
	if (hnd != -1) {										\
		EffekseerManager::GetInstance()->Stop(hnd, fade);	\
		hnd = -1;											\
	}

/*=====================================*
 * class
 *=====================================*/
class EffekseerManager
{
 public:

	static EffekseerManager* const GetInstance(void);

	virtual void		Create(void);
	virtual void		Setup(void);
	virtual void		Destroy(void);

	BOOL				SetupResourceBufferNum(BYTE bufNum);

	bool				LoadResource(int eEffID, char* effectName);
	virtual void		DeleteEffect(int eEffID);

	virtual void		Update(VECTOR& rCamPos, VECTOR& rCamTar, VECTOR& rCamUp);
	void				Draw(VECTOR& rCamPos, VECTOR& rCamTar, VECTOR& rCamUp);

	EFK_HANDLE			Play(int eResHandle, VECTOR* pPos, VECTOR* pScale, float fadeSec=0.5f);
	EFK_HANDLE			Play(int eResHandle, VECTOR* pPos, float allScale, float fadeSec=0.5f);
	EFK_HANDLE			PlayMtx(int eResHandle, MATRIX* pMtx);
	EFK_HANDLE			PlayMtx(int eResHandle, VECTOR* pTrans, VECTOR* pScale);

	void				Stop(EFK_HANDLE eEffectHandle, float fadeSec=0.5f);
	bool				IsPlay(EFK_HANDLE eEffectHandle);
	void				StopAll(void);
	
	void				SetPosition(EFK_HANDLE eEffectHandle, VECTOR* pSetPos);
	void				SetRotationAxisX(EFK_HANDLE eEffectHandle, float rotateX);
	void				SetRotationAxisY(EFK_HANDLE eEffectHandle, float rotateY);

	void				SetRotationAxis(EFK_HANDLE eEffectHandle, VECTOR* pAxisVec, float angle);
	
	void				SetScale(EFK_HANDLE eEffectHandle, VECTOR* pSetSca);

	void				SetMatrix(EFK_HANDLE eEffectHandle, MATRIX* pMtx);

	void				SetSpeed(EFK_HANDLE eEffectHandle, float speed);
	int					GetUpdateTime(EFK_HANDLE eEffectHandle);

 private:
	EffekseerManager();
	~EffekseerManager();

	/**** 変数 ****/
	float				mScreenRate;
	int*				mpEfkResHandleTable;
	BYTE				mEfkResBufferNum;

};

#endif // __EFFEKSEER_APP_MANAGER_H__

 /**** end of file ****/

