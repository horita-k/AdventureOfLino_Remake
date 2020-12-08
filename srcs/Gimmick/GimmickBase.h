/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * ギミックベース
 * @author SPATZ.
 * @data   2015/01/09 00:15:30
 */
//---------------------------------------------------
#ifndef __GIMMICK_BASE_H__
#define __GIMMICK_BASE_H__

#include "AppLib/Graphic/ModelCollision.h"
#include "GimmickBase.h"
//#include "AppLib/Resource/CsvAccesser.h"

/*=====================================*
 * class
 *=====================================*/

/* ギミック種別定義 */
enum eGimmickKind {
	eGIMMICK_KIND_LIFT = 0,
	eGIMMICK_KIND_SHIP,
	eGIMMICK_KIND_ROLLMOGU,
	eGIMMICK_KIND_KEY,
	eGIMMICK_KIND_TROLLEY,
	eGIMMICK_KIND_ELEVATOR,
	eGIMMICK_KIND_TOWER,
	eGIMMICK_KIND_ELEVATOR_B,
	eGIMMICK_KIND_BABEL_SWITCH,				// バベルの塔のスイッチ
	eGIMMICK_KIND_HOLE,						// 爆弾穴
	eGIMMICK_KIND_WHEEL,					// 歯車
	eGIMMICK_KIND_EYE,						// 巨大目
	eGIMMICK_KIND_CANNON,					// 大砲
	eGIMMICK_KIND_ELEVATOR_C,
	eGIMMICK_KIND_LEAN_FLOOR,				// 傾く床
	eGIMMICK_KIND_AIRSHIP,					// 飛行船
	eGIMMICK_KIND_LIFT_AUTO,				// 自動でループ移動するリフト
	eGIMMICK_KIND_CLOCK,					// 振り子時計
	eGIMMICK_KIND_SWITCH,					// 汎用スイッチ
	eGIMMICK_KIND_ELEVATOR_MOGU,			// モグー式エレベータ
	eGIMMICK_KIND_FOOT_SWITCH,				// 踏むスイッチ
	eGIMMICK_KIND_TETO,						// 大動物
	eGIMMICK_KIND_TROLLEY_EX,				// トロッコ改良
	eGIMMICK_KIND_PLIGHT,					// ポイントライト
	eGIMMICK_KIND_BARRIER,					// バリア
	eGIMMICK_KIND_FLOAT_BLOCK,				// 浮くブロック
	eGIMMICK_KIND_MAGIC_CUBE,				// 魔法のキューブ
	eGIMMICK_KIND_PRESS_WALL,				// 押しつぶす壁
	eGIMMICK_KIND_LEAN_PASSAGE,				// 傾く通路
	eGIMMICK_KIND_ROLLING_PIPE,				// 回転する筒
	eGIMMICK_KIND_SCREW,					// プロペラ
	eGIMMICK_KIND_LASER_BEAM,				// レーザービーム
	eGIMMICK_KIND_PRESS_WALL_HARD,			// 押しつぶす壁(壊れないタイプ)
	eGIMMICK_KIND_BOAT,

	eGIMMICK_KIND_MAX,
};

class GimmickBase : public ModelCollision {
 public:
	struct tOnNodeInfo {
		ModelCollision*		mpOnModel;
		VECTOR				mAddVec;
		VECTOR				mPrevRot;
	};

	static const BYTE	kON_NODE_MODEL_NUM = 8;
	static const char*	strCsvRootPath;
	static VECTOR		sRevisePos;
	static BOOL			sPauseAttach;

	enum eCsvLine {
		eCSVLINE_NONE = 0,

		eCSVLINE_ORG_POS_X,
		eCSVLINE_ORG_POS_Y,
		eCSVLINE_ORG_POS_Z,
		eCSVLINE_ORG_ROT_X,
		eCSVLINE_ORG_ROT_Y,
		eCSVLINE_ORG_ROT_Z,
		eCSVLINE_ORG_SCA_X,
		eCSVLINE_ORG_SCA_Y,
		eCSVLINE_ORG_SCA_Z,
		eCSVLINE_RESERVE_9,

		eCSVLINE_MAX,
	};

	// ギミックの上に乗っているかの判別の指定
	enum eGimmickNodeType {
		eGMK_NODE_TYPE_PROGRAM = 0,
		eGMK_NODE_TYPE_DATA,

		eGMK_NODE_TYPE_MAX,
	};

	/* ギミックパラメータ構造体 */
	struct tGimmickBaseParam {
		const char*			mResourceName;
		const char*			mCsvKeyword;
	};

	/* 関数 */
	GimmickBase();
	~GimmickBase();

	// ギミック関連
	static bool			LoadAllGimmick(int stageIndex);
	static void			DeleteAllGimmick(void);
	static void			SetupAllGimmick(void);

	static int			GetGimmickBaseNum(void);
	static GimmickBase*	GetGimmickBase(int gimmickIndex);
	static GimmickBase*	GetGimmickBaseFromEarthCollIndex(INT8 onCollisionIndex);
	static GimmickBase*	GetGimmickBaseFromWallCollIndex(INT8 onCollisionIndex);
	static void			ReloadInformationAll(int stageNo);
	static void			ReloadInformationAllFallDown(int stageNo);

	static void			ReviseAllGimmickOnPosition(VECTOR& rMoveVec);

	static CHAR			GetEmptyOnNodeListIndex(void);
	static tOnNodeInfo*	GetOnNodeInfo(CHAR nodeIndex);
	static GimmickBase*	Search1stFindGimmickKind(eGimmickKind kind);
	static void			SetGimmickPauseAttach(BOOL isAttach);
	static void			SetVisibleAll(bool isVisible);
	
	void				DeleteModel(void);
	void				LoadParamCsv(int stageIndex, BYTE csvNo, BYTE extendBufNum, BYTE gimmickKind);
	void				ReloadInformation(int stageIndex);

	BYTE				GetExtendBufferNum(void)						{ return mExtendBufferNum; }
	float				GetGimmickExtParamValue(BYTE extParamIndex);
	void				SetGimmickExtParamValue(BYTE extParamIndex, float paramValue);
	eGimmickKind		GetKind(void)									{ return (eGimmickKind)mGimmickKind; }
	BYTE				GetState(void)									{ return mState; }
	void				SetState(BYTE state)							{ mState = state; }
	void				SetNodeType(eGimmickNodeType nodeType)			{ mGimmickNodeType = nodeType; }
	bool				CheckEnableAttack(void)							{ return true; }

	void				Setup(void);
	void				GetBodyCapsule(ShapeCapsule* pCapsule);
	void				SetNowGravity(float gravity);

	virtual void		PreDraw(void);
	virtual void		PreUpdate(void);
	virtual void		Update(void);

	void				SetCharaPositionToFrameMatrix(ModelCollision* pModelCollision, int frameIndex);

	virtual void		ReviseOnPosition(VECTOR& rMoveVec);
	virtual int			GetWakeupEventValue(void);
	void				GetGimmickMoveVec(VECTOR& rMoveVec);
	void				ResetOnNodeInfo(ModelCollision* pOnModel);

	void				getOriginalPosition(VECTOR* pPos);
	void				getOriginalRotation(VECTOR* pRot);
	void				getOriginalScale(VECTOR* pScale);

protected:
	void				searchNearesetNode(VECTOR* pPosition, int frameIndex);
	void				commonSetup(void);
	virtual	void		extendSetup(void)=0;
	virtual	void		extendUpdate(void)=0;
	bool				isOnNodeModel(void);

	int					getSoundVolume(float maxHearingDist);

	/*変数*/
protected:

	BYTE				mGimmickKind;			// ギミック種別 (eGimmickKind にキャストして使用)

	VECTOR				mOnPosition;
	char				mOnNodeName[10];

	// 有効なノード情報を格納するビット
	BYTE				mOnNodeInfoHandleBit;

	BYTE				mGimmickNodeType;
	BYTE				mState;
	WORD				mCounter;
	BYTE				mCsvNo;
	CHAR				mWakeupEvent;			// ギミック起動イベント
	BYTE				mExtendBufferNum;
	float*				mpExtendBuffer;			// 拡張バッファ


};


#endif // __GIMMICK_BASE_H__

 /**** end of file ****/

