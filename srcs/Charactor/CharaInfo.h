//---------------------------------------------------
/**
 * キャラに関する情報
 * @author SPATZ.
 * @data   2011/12/13 00:33:01
 */
//---------------------------------------------------
#ifndef __CHARA_INFO_H__
#define __CHARA_INFO_H__

#include "dxlib.h"
#include "AppLib/Input/InputManager.h"

/*=====================================*
 * define
 *=====================================*/
/* キャラが方向転換する時の角速度 */
static const float kTURN_MOVE_ROT		= 20.0f * DX_PI / 180.0f;
static const float kRUN_FOWARD_SPEED	= 0.2f * 1.5f ;
static const float kJUMP_FOWARD_SPEED	= 0.30f;
static const float kSTEP_BACK_SPEED		= 0.06f;
static const float kSTEP_SIDE_SPEED		= 0.08f;
/* ジャンプ力 */
static const float kJUMP_GRAVITY		= 32.0f;
extern		 float kGRAVITY_SPEED;

/* キャラの歩くスピード */
static const float kCHARA_WALK_SPEED	= 8.0f ;

/* キャラの走るスピード */
static const float kCHARA_RUN_SPEED		= 42.0f ;

/* キャラのダッシュのスピード */
static const float kCHARA_DASH_SPEED	= 60.0f;

/* キャラの走る加速度 */
static const float kCHARA_ADD_MOVE_SPEED= 0.8f ;
static const float kCHARA_AWAY_SPEED	= 0.4f ;
static const int   kWEAPON_TOP_NODE_INDEX	= 1;
static		 VECTOR sUpVec = VGet(0.0f, 1.0f, 0.0f);
static		int	  kSYSTEM_DAMAGE_VALUE	= -9998;

#endif // __CHARA_INFO_H__

 /**** end of file ****/

