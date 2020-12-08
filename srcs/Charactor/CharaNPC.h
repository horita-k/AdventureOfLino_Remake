/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * NPC管理クラス
 * @author SPATZ.
 * @data   2012/04/14 19:33:22	
 */
//---------------------------------------------------
#ifndef __CHARA_NPC_H__
#define __CHARA_NPC_H__

#include "AppLib/Graphic/ModelCollision.h"
#include "AppLib/Graphic/Shadow.h"
#include "AppLib/Resource/CsvAccesser.h"

/*=====================================*
 * class
 *=====================================*/
class CharaNpc : public ModelCollision {
 public:

	/* 定義 */

	enum {
		eCSV_FILE_NAME = 0,

		eCSV_MOTION_IDEL,
		eCSV_MOTION_TALK,

		eCSV_POS_X,
		eCSV_POS_Y,
		eCSV_POS_Z,
		eCSV_ROT_Y,

		eCSV_AFTER_EVT_NO,
		eCSV_IS_TOON,

		eCSV_NAME,
		eCSV_MSG_00,
		eCSV_MSG_01,
		eCSV_MSG_02,
		eCSV_MSG_03,

		eCSV_OPTION,

		eCSV_MAX,
	};

	enum {
		eNPC_OPTION_NONE = 0,
		eNPC_OPTION_TURN,
		eNPC_OPTION_NOT_TURN,

		eNPC_OPTION_MAX,
	};

	static const char*	strCsvRootPath;
	static CsvAccesser	sStageInfoCsv;
	static float		sBGScale;
	static BYTE			sCsvSubNo;

	/* 関数 */
	CharaNpc();
	~CharaNpc();

	virtual void Setup(bool isShadow=false);
	virtual void Update(void);

	static void			SetupNpcResourceInfo(int stageIndex);

	static int			GetNpcResourceNum(void);
	static char*		GetNpcResourceName(int npcIndex);

	static CharaNpc*	CreateNpcInstance(float bgScale);
	static CharaNpc*	RecreateNpcInstance(int csvSubNo, CharaNpc* pNpcInstance, int& rNpcNum, int stageIndex, float bgScale);

	static void			SetCsvSubNo(BYTE subNo);
	static BYTE			GetCsvSubNo(void);

	void		SetupCsvInfo(BYTE npcIndex, VECTOR& rDiffPos);

	void		LinkCharaResource(const char* charaName);

	void		EnableShadow(void);
	void		DisableShadow(void);

	void		ChangeState(int setState);
	void		UpdateAttackChara(void);
	void		SetupOnceDrawShadow(void)				{ mShadow.SetupOnceDrawShadow(); }

	bool		GetIsRetire(void)						{ return false; }
	void		SetShadowVector(VECTOR moveVec);
	void		GetBodyCapsule(ShapeCapsule* pCapsule)	{}
	void		SetNowGravity(float gravity)			{}

	void		ReloadInformation(int stageIndex, VECTOR& rDiffPos);

	// NPC特有の処理
	void		StartIdleMotion(void);
	void		StartTalkMotion(void);
	void		RenderNpcMessage(void);
	bool		StartAfterTalkEvent(void);
	int			GetOptionValue(void);

 private:
	/*=====================================*
	 * phaseState
	 *=====================================*/
	void		phaseEnterStateIdle(void);
	void		phaseStateIdle(void);

	/*変数*/
	Shadow		mShadow;
	CHAR		mNpcIndex;

};

void DrawSeekFunc(int model_handle);

#endif // __CHARA_NPC_H__

 /**** end of file ****/

