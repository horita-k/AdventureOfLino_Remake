/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ActionTaskManager.cpp
 * タスク(イベント管理)クラス
 * @author SPATZ.
 * @data   2012/04/04 14:48:17
 */
//---------------------------------------------------
#ifndef __ACTION_TASK_MANAGER_H__
#define __ACTION_TASK_MANAGER_H__

#include "dxlib.h"

typedef bool (*NOTIFY_FUNC)(void *pParam);

/*=====================================*
 * class
 *=====================================*/
class ActionTaskManager {
	
 public:
	enum eTaskType {
		eTASK_NONE,		// 0
		eTASK_MOVE,		// 1
		eTASK_FALLDOWN,	// 2
		eTASK_EVENT,	// 3
		eTASK_ENEMY,	// 4
		eTASK_NAVI,		// 5
		eTASK_COLLISION_MAX,

		eTASK_SHOP=eTASK_COLLISION_MAX,	// 6
		eTASK_GETITEM,					// 7
		eTASK_ENEMY_ITEM,				// 8
		eTASK_SAVE,						// 9
		eTASK_NAVI_EX,					// 10
		eTASK_CHECK_POINT,				// 11
		eTASK_FLYING_MOVE,				// 12
		eTASK_PLAY_OCARINA,				// 13

		eTASK_MAX,
	};

	struct tTaskInfo {
		eTaskType		mTaskType;
		long long		mTaskValueA;
		long			mTaskValueB;
	};

	static ActionTaskManager* const GetInstance(void);

	/* 関数 */
	void		Setup(NOTIFY_FUNC func);
	void		Update(void);
	void		UpdateActionTask(VECTOR* pVecStart, VECTOR* pVecEnd);
	bool		NotifyAction(tTaskInfo* pTaskInfo);

 private:
	ActionTaskManager();
	~ActionTaskManager();

	/* 変数 */
	static NOTIFY_FUNC	mNotifyFunc;

};

#endif //__ACTION_TASK_MANAGER_H__

 /**** end of file ****/
