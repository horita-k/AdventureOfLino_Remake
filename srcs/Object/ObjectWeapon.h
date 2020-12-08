/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * オブジェクトベースクラス
 * @author SPATZ.
 * @data   2012/10/01 23:27:16
 */
//---------------------------------------------------
#ifndef __OBJECT_WEAPON_H__
#define __OBJECT_WEAPON_H__

#include "ObjectBase.h"
#include "BladeEffect.h"
#include "AppLib/Graphic/EffekseerManager.h"

/*=====================================*
 * class
 *=====================================*/
class ObjectWeapon : public ObjectBase {
 public:

	enum {
		eWEAPON_TYPE_ARROW = 0,
		eWEAPON_TYPE_METEO,
		eWEAPON_TYPE_MISSILE,
		eWEAPON_TYPE_SONIC_WAVE,
		eWEAPON_TYPE_COPY_ROD,
		
		eWEAPON_TYPE_MAX,
		eWEAPON_TYPE_ERROR,
	};

	/* 関数 */
	ObjectWeapon();
	virtual ~ObjectWeapon();

	virtual void 	Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg);
	virtual void 	Update(void);
	virtual bool	GetIsRetire(void);
	virtual void	SetUsed(void);

	UCHAR&			State(void);
	VECTOR&			MoveVec(void)			{ return mMoveVec; }
	BYTE			GetHandType(void)		{ return mWeaponType; }
	bool			CheckEnableAttack(void)	{ return true; }

 private:

protected:
	/*変数*/
	UCHAR			mState;
	VECTOR			mMoveVec;
	float			mGravity;
	BYTE			mWeaponType;
};

/*=====================================*
 * ObjectWeapon_Arrow
 *=====================================*/
class ObjectWeapon_Arrow : public ObjectWeapon {
public:
	
	enum  {
		eWEAPON_ARROW_STATE_INBOW = 0,
		eWEAPON_ARROW_STATE_SHOOTING,
		eWEAPON_ARROW_STATE_STICK,

		eWEAPON_ARROW_STATE_MAX,
	};

	ObjectWeapon_Arrow();

	void Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg, bool isRaise=false);
	void Update(void);
	bool GetIsRetire(void);

	void ProcessSetArchery(void);
	void ProcessShoot(VECTOR* pBottomPos, VECTOR* pTopPos);

	void AddArrowMoveVec(VECTOR& addVec);

	/* 関数 */
private:
	void	phaseArrowStateInArchery(void);
	void	phaseArrowStateShooting(void);
	void	phaseArrowStateStick(void);

	/* 変数 */
	typedef void (ObjectWeapon_Arrow::*PHASE_WEAPON_ARROW_STATE)(void);
	static PHASE_WEAPON_ARROW_STATE mPhaseState[eWEAPON_ARROW_STATE_MAX];

	VECTOR			mTopPos;
	VECTOR			mEndPos;

	BladeEffect		mSubweaponBladeEffect;
	int				mEffIndex;
};

/*=====================================*
 * ObjectWeapon_Meteo
 *=====================================*/
class ObjectWeapon_Meteo : public ObjectWeapon {
public:
	
	enum  {
		eWEAPON_METEO_STATE_NONE = 0,
		eWEAPON_METEO_STATE_FLOAT_UP,
		eWEAPON_METEO_STATE_FLOATING,
		eWEAPON_METEO_STATE_CHARGE,
		eWEAPON_METEO_STATE_SHOOTING,
		eWEAPON_METEO_STATE_DISAPPEAR,
		eWEAPON_METEO_STATE_BREAKED,

		eWEAPON_METEO_STATE_MAX,
	};

	ObjectWeapon_Meteo();
	~ObjectWeapon_Meteo();

	virtual void Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg, bool isRaise=false);
	void Update(void);
	
	void ProcessFloat(VECTOR& rStartPos);
	void ProcessCharge(void);
	void ProcessShoot(VECTOR& rMoveVec);
	bool GetIsRetire(void);

	virtual void ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);
	bool CheckDamageCollArrow(tCheckDamageWork* pParam);
	

	/* 関数 */
private:
	void	phaseMeteoStateNone(void);
	void	phaseMeteoStateFloatUp(void);
	void	phaseMeteoStateFloating(void);
	void	phaseMeteoStateCharge(void);
	void	phaseMeteoStateShooting(void);
	void	phaseMeteoStateDisappear(void);
	void	phaseMeteoStateBreaked(void);

	/* 変数 */
	typedef void (ObjectWeapon_Meteo::*PHASE_WEAPON_METEO_STATE)(void);
	static PHASE_WEAPON_METEO_STATE mPhaseState[eWEAPON_METEO_STATE_MAX];

protected:
	int				mEffIndex;
	bool			mIsRefrect;
};

/*=====================================*
 * ObjectWeapon_MeteoRefrect
 *=====================================*/
class ObjectWeapon_MeteoRefrect : public ObjectWeapon_Meteo
{
public:
	ObjectWeapon_MeteoRefrect();

	void Update(void);
	void Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg, bool isRaise=false);
	void ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);
	void ProcessStarEffect(void);
};

/*=====================================*
 * ObjectWeapon_Missile
 *=====================================*/
class ObjectWeapon_Missile : public ObjectWeapon {
public:
	
	enum  {
		eWEAPON_MISSILE_STATE_NONE = 0,
		eWEAPON_MISSILE_STATE_SHOOTING,
		eWEAPON_MISSILE_STATE_DISAPPEAR,
		eWEAPON_MISSILE_STATE_BREAKED,

		eWEAPON_MISSILE_STATE_MAX,
	};

	ObjectWeapon_Missile();
	~ObjectWeapon_Missile();
	char*			GetModelName(void)		{ return (char*)"ミサイル"; }

	virtual void Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg, ModelBase* pMaster, bool isRaise=false);
	void Update(void);
	
	void ProcessShoot(VECTOR& rStartPos, VECTOR& rMoveVec);
	bool GetIsRetire(void);

	virtual void ProcessDamage(int attackPower, bool isSetMode, VECTOR* pEffPos, VECTOR* pDamageDownVec, bool isDamageDown, eAttackType attackType);
	bool CheckDamageColl(tCheckDamageWork* pParam);

	/* 関数 */
private:
	void	phaseMissileStateNone(void);
	void	phaseMissileStateShooting(void);
	void	phaseMissileStateDisappear(void);
	void	phaseMissileStateBreaked(void);

	/* 変数 */
	typedef void (ObjectWeapon_Missile::*PHASE_WEAPON_MISSILE_STATE)(void);
	static PHASE_WEAPON_MISSILE_STATE mPhaseState[eWEAPON_MISSILE_STATE_MAX];

protected:
	int				mEffHndSmoke;
	ModelBase*		mpMaster;

};

/*=====================================*
 * ObjectWeapon_SonicWave
 *=====================================*/
class ObjectWeapon_SonicWave : public ObjectWeapon {
public:
	
	enum  {
		eWEAPON_SONIC_WAVE_STATE_ATTACH = 0,
		eWEAPON_SONIC_WAVE_STATE_SHOOTING,
		eWEAPON_SONIC_WAVE_STATE_DISAPPEAR,

		eWEAPON_SONIC_WAVE_STATE_MAX,
	};

	ObjectWeapon_SonicWave();
	~ObjectWeapon_SonicWave();

	void Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg, bool isRaise=false);
	void Update(void);

	void ProcessCreateSonick(MATRIX* pMatrix);
	void SetSonicWaveMatrix(MATRIX* pMatrix);
	void ProcessShoot(VECTOR* pMoveVec, float setScale, float hitSize);
	void ProcessDisappear(void);

	/* 関数 */
private:
	void	phaseSonicWaveStateAttach(void);
	void	phaseSonicWaveStateShooting(void);
	void	phaseSonicWaveStateDisappear(void);

	/* 変数 */
	typedef void (ObjectWeapon_SonicWave::*PHASE_WEAPON_SONIC_WAVE_STATE)(void);
	static PHASE_WEAPON_SONIC_WAVE_STATE mPhaseState[eWEAPON_SONIC_WAVE_STATE_MAX];

	EFK_HANDLE		mEfkHandle;
	//	VECTOR			mMoveVec;

};


/*=====================================*
 * ObjectWeapon_CopyRod
 *=====================================*/
class ObjectWeapon_CopyRod : public ObjectWeapon {
public:
	
	enum  {
		eWEAPON_COPY_ROD_STATE_ATTACH = 0,
		eWEAPON_COPY_ROD_STATE_SHOOTING,
		eWEAPON_COPY_ROD_STATE_HITTING_OBJ,
		eWEAPON_COPY_ROD_STATE_DISAPPEAR,

		eWEAPON_COPY_ROD_STATE_MAX,
	};

	ObjectWeapon_CopyRod();
	~ObjectWeapon_CopyRod();

	void Setup(int objectIdx, eItemKind itemIdx, VECTOR* pSetPos, int rotDeg, bool isRaise=false);
	void Update(void);

	void ProcessCreate(MATRIX* pMatrix);
	void SetCopyRodMatrix(MATRIX* pMatrix);
	void ProcessShoot(VECTOR* pMoveVec, float setScale, float hitSize, ModelCollision* pLockonModel=NULL);
	void ProcessHittingObj(float effScale);
	void ProcessDisappear(void);

	/* 関数 */
private:
	void	phaseCopyRodStateAttach(void);
	void	phaseCopyRodStateShooting(void);
	void	phaseCopyRodStateHittingObj(void);
	void	phaseCopyRodStateDisappear(void);

	void	revertCameraState(void);

	
	/* 変数 */
	typedef void (ObjectWeapon_CopyRod::*PHASE_WEAPON_COPY_ROD_STATE)(void);
	static PHASE_WEAPON_COPY_ROD_STATE mPhaseState[eWEAPON_COPY_ROD_STATE_MAX];

	EFK_HANDLE		mEfkHandle;
	ModelCollision*	mpLockonModel;

};





#endif // __OBJECT_WEAPON_H__

 /**** end of file ****/

