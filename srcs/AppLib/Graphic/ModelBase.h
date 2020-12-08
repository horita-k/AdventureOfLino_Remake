/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * モデルベースクラス
 * @author SPATZ.
 * @data   2011/11/20 22:46:50
 */
//---------------------------------------------------
#ifndef __MODEL_BASE_H__
#define __MODEL_BASE_H__

#include "DxLib.h"

/*=====================================*
 * class
 *=====================================*/
class ModelBase
{
 public:

	enum eMatrixType {
		eMATRIX_TYPE_TRS,
		eMATRIX_TYPE_MATRIX,

		eMATRIX_TYPE_MAX,
	};
	
	/* typedef */
	typedef void(*DRAW_FUNC)(int mdl_handle);

	ModelBase();
	virtual ~ModelBase();

	virtual void		Create(void);
	virtual void		Setup(void);

	virtual void		Calc(void);
	virtual void		Draw(void);
	virtual void		Update(void);

	virtual void		PreUpdate(void)		{Update();}
	virtual void		PreDraw(void)		{}

	virtual void		Destroy(void);

	virtual void		LoadModel(const char* fileName);
	void				LinkModel(const char* fileName);
	virtual void		DeleteModel(void);

	virtual void		ReviseModel(VECTOR* pMoveVec){ }

	void				ForciblySetPositionByCentering(bool isUpdateMatrix=false);

	VECTOR&				Position(void)			{ return mPosition; }
	VECTOR&				Rotation(void)			{ return mRotation; }
	VECTOR&				Scale(void)				{ return mScale; }	
	MATRIX&				Matrix(void)			{ return mMatrix; }

	int					GetModelHandle(void)	{ return mModelHandle; }

	void				SetMatrixType(eMatrixType type);
	eMatrixType			GetMatrixType(void);
	void				SetDiffuse(COLOR_F material);
	void				SetEmissive(COLOR_F material);
	void				SetAmbient(COLOR_F material);
	void				SetSpecular(COLOR_F material);
	virtual void		SetVisible(bool isVisible, int frameIndex=-1);
	void				SetMeshVisible(bool isVisible, int meshIndex);
	
	float				GetAlpha(void);
	void				SetAlpha(float rate);	/* 0.0 -> 1.0 */

	float				GetAlpha(int frameIndex);
	void				SetAlpha(int frameIndex, float rate);	/* 0.0 -> 1.0 */

	void				EnableToonShader(float dot=0.04f, float width=0.04f);
	void				DisableToonShader(void);

	void				SetEnableEmissive(bool isEnable);

	int					SearchMaterialName(int frameIndex, const char* strMaterialName);
	int					GetIncludeMaterialNameNum(int frameIndex, const char* strMaterialName);

	void				GetMeshMinPosition(VECTOR* pOutPos, int mesh_index);
	void				GetMeshMaxPosition(VECTOR* pOutPos, int mesh_index);
	void				GetMeshMdlPosition(VECTOR* pOutPos, int mesh_index);

	void				SetDrawFunc(DRAW_FUNC func)		{ mDrawFunc = func; }

	virtual void		SetPositionOnCollision(void);
	void				SetRotationToTarget(VECTOR* pTargetPos);
	
	void				ChangeTexture(BYTE texIndex, char* texName, BOOL isLoad=FALSE);
	void				ChangeSphereTexAll(int texIndex);
	void				RevertSphereTexAll(void);

	void				GetRotationVecFromMatrix(VECTOR* pRotVec);

	// TRSの情報を元にマトリックスの値を更新
	void				UpdateMatrixFromTRS(void);
	
	void				UpdateMatrixFromRotY_Normal(float setRotY, VECTOR& rNormal);


protected:
	int			mModelHandle;
	VECTOR		mPosition;
	VECTOR		mRotation;
	VECTOR		mScale;
	MATRIX		mMatrix;

	eMatrixType	mMatrixType;

	bool		mIsSetup;
	float		mAlpha;
	CHAR		mSphereTexIndex;		// スフィアマップのテクスチャ番号

	DRAW_FUNC	mDrawFunc;


};

#endif // __MODEL_BASE_H__

 /**** end of file ****/

