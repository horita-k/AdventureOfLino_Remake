/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ライブラリの色々融合したテスト
 * @author SPATZ.
 * @data   2011/11/21 00:55:18
 */
//---------------------------------------------------
#ifndef __SCENE_MAIN_GAME_H__
#define __SCENE_MAIN_GAME_H__

#include "AppLib/Graphic/ModelBase.h"
#include "SceneBase.h"
#include "SceneInfo.h"
#include "DataDefine/EnemyDef.h"
#include "Object/ObjectBase.h"
#include "AppLib/Resource/CsvAccesser.h"
#include "StageFunc/StageFuncBase.h"

//class StageFuncBase;

/*=====================================*
  class
 *=====================================*/
class SceneMainGame : public SceneBase {
public:
	/* 定数 */
	enum {
		ePHASE_LOAD_BEFORE,
		ePHASE_LOADING,
		ePHASE_SETUP,
		ePHASE_GAME_BEFORE,
		ePHASE_GAME,
		ePHASE_SHOW_MAP,
		ePHASE_CHANGE_STAGE,
		ePHASE_FALL_DOWN,
		ePHASE_RECOVERY,
		ePHASE_MESSAGE_BEFORE,
		ePHASE_MESSAGE,
		ePHASE_EVENT_BEFORE,
		ePHASE_EVENT,
		ePHASE_GET_ITEM_BEFORE,
		ePHASE_GET_ITEM,
		ePHASE_GAMEOVER_BEFORE,
		ePHASE_GAMEOVER,
		ePHASE_STAGECLEAR,
		ePHASE_SHOP,
		ePHASE_SELECT_WEAPON,
		ePHASE_SELECT_WEAPON_END,
		ePHASE_START_BATTLE,
		ePHASE_NAVI_TALK,
		ePHASE_NAVI_TALK_END,
		ePHASE_SAVE,
		ePHASE_MOVE_PLACE,
		ePHASE_FLYING_MOVE,
		ePHASE_PLAY_OCARINA,
		ePHASE_TEST_CONNECT,

		ePHASE_MAX,
	};

	/* 関数 */
	SceneMainGame();
	~SceneMainGame();

	/*=====================================*
	 * useful method
	 *=====================================*/
	void		Create(int debugArg);
	void		Update(void);
	void		Destroy(void);
  
	static const char*	GetStageName(int index);
	static int			GetStageIndex(void);
	static int			GetStageActionLimit(void);
	static bool			GetIsBattle(void);

	static bool			RequestChangeState(void* pParam);
	int					GetLoadingPercent(void);
	void				ReviseAllModel(void);
	void				PauseReviseAllModel(bool isPause);
	static StageFuncBase*		GetStageFunc(void);
	static USHORT				GetMyWeaponNum(void);


	// 指定のキャラにプレイヤーを変更する
	static void			ChangePlayerModel(const char* strModelName, BOOL isEffect=TRUE);
	static void			StartNaviTalk(const char* strTalk);
	static void			BeforeCreateShadowFunc(void* args);
	static void			AfterCreateShadowFunc(void* args);

	/*=====================================*
	 * ステージ毎の情報
	 *=====================================*/
	// ステージ毎のバッファ

private:

	void		destroyEnemyWork(int enemyIndex);
	int			getItemNumFromMapData(void);
	void		setupItemParameterFromMapData(void);

	void		updateNpcEvent(void);
	void		updateObjectInputCheck(void);

	static void updateObjectUsedFromObjectFlag(void);
	static void	setObjectFlagFromObjectUsed(void);

	void		attachSkydomeTexture(const char* texName);

	/*=====================================*
	 * phase
	 *=====================================*/
	typedef void (SceneMainGame::*PHASE_FUNC)(void);
	static PHASE_FUNC mBasePhaseFunc[ePHASE_MAX];

	void phaseLoadBefore(void);
	void phaseLoading(void);
	void phaseSetup(void);
	void phaseGameBefore(void);
	void phaseGame(void);
	void phaseShowMap(void);
	void phaseChangeStage(void);
	void phaseFallDown(void);
	void phaseRecovery(void);
	void phaseMessageBefore(void);
	void phaseMessage(void);
	void phaseEventBefore(void);
	void phaseEvent(void);
	void phaseGetItemBefore(void);
	void phaseGetItem(void);
	void phaseGameOverBefore(void);
	void phaseGameOver(void);
	void phaseStageclear(void);
	void phaseShop(void);
	void phaseSelectWeapon(void);
	void phaseSelectWeaponEnd(void);
	void phaseStartBattle(void);
	void phaseNaviTalk(void);
	void phaseNaviTalkEnd(void);
	void phaseSave(void);
	void phaseMovePlace(void);
	void phaseFlyingMove(void);
	void phasePlayOcarina(void);
	void phaseTestConnect(void);

#ifdef _DEBUG
	void	debugPramInput(void);

#endif _DEBUG

	/* 変数 */
};



#endif // __SCENE_MAIN_GAME_H__

/**** end of file ****/


