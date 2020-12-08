/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * エフェクト管理クラス
 * @author SPATZ.
 * @data   2014/01/28 23:12:38
 */
//---------------------------------------------------
/*=====================================*
 * include
 *=====================================*/
#include "AppLib/Basic/Basic.h"
#include "AppLib/Graphic/CameraManager.h"

#include "EffectNode.h"
#include "EffectManager.h"
#include "AppLib/Graphic/GraphicManager.h"

#ifdef _DEBUG
#include "Charactor/CharaBase.h"
#endif // _DEBUG


/*=====================================*
 * static 
 *=====================================*/
static const float kTARGET_FPS = 60.0f;

static const char* STR_SMT_TIME		= "time_0_X";
static const char* STR_SMT_ALPHA	= "Alpha";

/*=====================================*
 * SemanticFunc
 *=====================================*/
/*static*/EffectNode::SMT_FUNC EffectNode::mSmtFunc[eSMT_MAX] = {
    &EffectNode::funcSmt_TIME,
    &EffectNode::funcSmt_WORLD,
    &EffectNode::funcSmt_VIEW,
    &EffectNode::funcSmt_VIEWINVERSE,
    &EffectNode::funcSmt_VIEWPROJECTION,
    &EffectNode::funcSmt_VIEWPROJECTIONINVERSE,
    &EffectNode::funcSmt_PROJECTION,
    &EffectNode::funcSmt_PROJECTIONINVERSE,
    &EffectNode::funcSmt_WORLDVIEWPROJECTION,
    &EffectNode::funcSmt_WORLDVIEWPROJECTIONINVERSE,
    &EffectNode::funcSmt_WORLDINVERSETRANSPOSE,
    &EffectNode::funcSmt_WORLDVIEW,
    &EffectNode::funcSmt_VIEWINVERSETRANSPOSE,
    &EffectNode::funcSmt_WORLDINVERSE,
    &EffectNode::funcSmt_WORLDVIEWTRANSPOSE,
    &EffectNode::funcSmt_WORLDVIEWINVERSE,
    &EffectNode::funcSmt_VIEWTRANSPOSE,
    &EffectNode::funcSmt_POSITION,
    &EffectNode::funcSmt_DIRECTION,
    &EffectNode::funcSmt_MODELDATA,
    &EffectNode::funcSmt_DIFFUSE,
    &EffectNode::funcSmt_AMBIENT,
    &EffectNode::funcSmt_EMISSIVE,
    &EffectNode::funcSmt_SPECULAR,
    &EffectNode::funcSmt_SPECULARPOWER,
    &EffectNode::funcSmt_TOONCOLOR,
    &EffectNode::funcSmt_EDGECOLOR,
    &EffectNode::funcSmt_GROUNDSHADOWCOLOR,
    &EffectNode::funcSmt_ELAPSEDTIME,
    &EffectNode::funcSmt_CONTROLOBJECT,
    &EffectNode::funcSmt_RENDERCOLORTARGET,
    &EffectNode::funcSmt_RENDERDEPTHSTENCILTARGET,
    &EffectNode::funcSmt_TEXTUREVALUE,
    &EffectNode::funcSmt_STANDARDSGLOBAL,
    &EffectNode::funcSmt_OFFSCREENRENDERTARGET,
    &EffectNode::funcSmt_VIEWPORTPIXELSIZE,
    &EffectNode::funcSmt_DUMMY,
};

#define COLOR_FLOAT_TO_INT(fvalue)		(int)( (int)255.0f * (fvalue / 1.0f) )

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
EffectNode::EffectNode()
{
	mPosition = ZERO_VEC;
	mRotation = ZERO_VEC;
	mScale = VGet(1.0f, 1.0f, 1.0f);
	
	mMatrixType = eMATRIX_TYPE_TRS;

	mIsPlay = false;

	memset(&mLifetime, 0x00, sizeof(mLifetime));
	memset(&mAlphaWork, 0x00, sizeof(mAlphaWork));
	memset(&mFade, 0x00, sizeof(mFade));

	mpEffect		= NULL;
	mpResourceRef	= NULL;

	mSemCount		= 0;
	memset(mSemHandle, 0, sizeof(mSemHandle));

	mStencilHandle = 0xFF;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
EffectNode::~EffectNode()
{
	Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::Create(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::Setup(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::Update(void)
{
	if (mIsPlay) {
		
		/**** calc ****/
		calc();

		/**** exec ****/
		if (exec() == false) {

			/**** draw ****/
			(this->*mDrawFunc)();
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::Destroy(void)
{

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::Play(EffectResource* pResource, float fadeSec/*=0.5f*/)
{
	HRESULT hr;

	pResource->GetCloneEffect(&mpEffect);
	APP_ASSERT(mpEffect != NULL,
			   "Failed CloneEffect()\n");

	// リソース情報をコピー
	mpResourceRef = pResource;
	
	memset(mSemHandle, 0, sizeof(mSemHandle));
	mSemCount = 0;

	// リソースクラスからハンドルの情報を取得
	for (int iI = 0; iI < mpResourceRef->GetSemanticNum(); iI ++) {
		mSemHandle[mSemCount] = mpResourceRef->GetSemanticHandle(mpEffect, iI);
		mSemCount ++;
	}

	// ステンシルサーフェイス情報が存在するリソースの場合、ステンシルサーフェイス番号を取得する
	if (mpResourceRef->IsExistStencilSurface() == true) {
		mpResourceRef->HoldStencilSurfaceHandle(mStencilHandle);

		// セマンティックの RenderColorTarget を検索し、texture に複製された mpEffect に設定する
		LPDIRECT3DTEXTURE9 p_texture = NULL;
		D3DXHANDLE hTexName = NULL;
		int render_color_target_num = mpResourceRef->GetSemanticNum(eSMT_RENDERCOLORTARGET);
		for (int iI = 0; iI < render_color_target_num; iI ++) {
			p_texture = mpResourceRef->GetRenderColorTargetTexture(mStencilHandle, iI);
			hTexName = (D3DXHANDLE)mpResourceRef->GetSemanticName(eSMT_RENDERCOLORTARGET, iI);
			
			hr = mpEffect->SetTexture(hTexName, p_texture);
		}

	}

	// リソースにスクリプト指定がある場合は描画方式をスクリプトに設定する
	if (mpResourceRef->GetScriptNum() == 0) {
		mDrawFunc = &EffectNode::drawFuncNormal;		// 通常描画関数
	} else {
		mDrawFunc = &EffectNode::drawFuncScript;		// スクリプト描画関数
	}
	
	this->SetLonevity(STR_SMT_TIME);

	if (fadeSec <= 0.0f) {
		SetAlpha(1.0f);
		mFade.mIsActive = FALSE;
		mFade.mSpeed = 0.0f;
	} else {
		float diff = (float)(1.0f - mAlphaWork.mAlpha);
		mFade.mSpeed = diff / (fadeSec * kTARGET_FPS);
		if (mFade.mSpeed > 0.0f) {
			mFade.mIsActive = TRUE;
		}
	}

	mIsPlay = true;
	mLifetime.mCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::Stop(float fadeSec/*=0.5f*/)
{
	if (mIsPlay == false) return;

	if (fadeSec <= 0.0f) {
		// ０フレで削除
		SetAlpha(0);
		mFade.mIsActive = FALSE;
		mFade.mSpeed = 0.0f;
		deleteEffect();
		mIsPlay = false;

	} else {
		float diff = (float)(0.0f - mAlphaWork.mAlpha);
		mFade.mSpeed = diff / (fadeSec * kTARGET_FPS);
		if (mFade.mSpeed < 0.0f) {
			mFade.mIsActive = TRUE;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::SetLonevity(const char* pName)
{
	mLifetime.mIsActive = TRUE;
	strcpy_s(mLifetime.mName, sizeof(mLifetime.mName), pName);
	mLifetime.mCount = 0;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::SetAlpha(float fAlpha)
{
	if (mpEffect == NULL) return;

	HRESULT hr;
	mAlphaWork.mAlpha = fAlpha;
	hr = mpEffect->SetFloat(STR_SMT_ALPHA, mAlphaWork.mAlpha);

	// アサートではなくログ警告レベル
	PRINT_ASSERT(hr != D3DERR_INVALIDCALL,
				 "Failed Effect::SetAlpha\n");
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
float
EffectNode::GetAlpha(void)
{
	return mAlphaWork.mAlpha;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::SetVec3(const char* pName, const VECTOR* pVec3)
{
	FLOAT4 vector;
	vector.x = pVec3->x;
	vector.y = pVec3->y;
	vector.z = pVec3->z;
	vector.w = 1.0f;
	this->SetVec4(pName, &vector);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::SetVec4(const char* pName, const FLOAT4* pVec4)
{
	if (mpEffect == NULL) return;

	HRESULT hr;
	
	D3DXVECTOR4 buf;
	buf.x = pVec4->x;
	buf.y = pVec4->y;
	buf.z = pVec4->z;
	buf.w = pVec4->w;
	hr = mpEffect->SetVector(pName, &buf);
	PRINT_ASSERT(hr != D3DERR_INVALIDCALL,
				 "Failed ID3DXEffect::SetMatrix, %s\n", pName);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::SetFloat(const char* pName, const float fValue)
{
	if (mpEffect == NULL) return;

	HRESULT hr;
	hr = mpEffect->SetFloat(pName, fValue);
	APP_ASSERT_PRINT(hr != D3DERR_INVALIDCALL,
					 "Failed ID3DXEffect::SetFloat, %s\n", pName);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::SetFloat2(const char* pName, const float* pValue)
{
	if (mpEffect == NULL) return;

	HRESULT hr;
	const FLOAT float_array[2] = {pValue[0], pValue[1]};
	hr = mpEffect->SetFloatArray(pName, float_array, 2);
	APP_ASSERT_PRINT(hr != D3DERR_INVALIDCALL,
					 "Failed ID3DXEffect::SetFloat2, %s\n", pValue);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::SetMatrix(const char* pName, const MATRIX* pMatrix)
{
	if (mpEffect == NULL) return;

	HRESULT hr;

	D3DXMATRIX buf;
	ConvertMtx(&buf, pMatrix);
	hr = mpEffect->SetMatrix(pName, &buf);
	PRINT_ASSERT(hr != D3DERR_INVALIDCALL,
				 "Failed ID3DXEffect::SetMatrix, %s\n", pName);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::GetFloat(const char* pName, float* pValue)
{
	if (mpEffect == NULL) return;

	HRESULT hr;
	hr = mpEffect->GetFloat(pName, pValue);
	APP_ASSERT_PRINT(hr != D3DERR_INVALIDCALL,
					 "Failed ID3DXEffect::SetFloat, %s\n", pName);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::GetVec4(const char* pName, FLOAT4* pVec4)
{
	if (mpEffect == NULL) return;

	HRESULT hr;
	
	D3DXVECTOR4 buf;
	hr = mpEffect->GetVector(pName, &buf);
	PRINT_ASSERT(hr != D3DERR_INVALIDCALL,
				 "Failed ID3DXEffect::SetMatrix, %s\n", pName);
	pVec4->x = buf.x;
	pVec4->y = buf.y;
	pVec4->z = buf.z;
	pVec4->w = buf.w;
}

/*=====================================*
 * private:
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
EffectNode::exec(void)
{
	// 設定するセマンティク情報
	tSemanticSetInfo set_info;
	ConvertMtx(&set_info.world_mtx, &mCalcedMtx);
	ConvertMtx(&set_info.view_mtx,	&GetCameraViewMatrix());
	ConvertMtx(&set_info.proj_mtx,	&GetCameraProjectionMatrix());

	for (int iI = 0; iI < (int)mSemCount; iI ++) {
		eSmtKind sem_kind = mpResourceRef->GetSemanticType(iI);
		(this->*mSmtFunc[sem_kind])(iI, &set_info);
	}

	// 寿命が設定されている場合の処理
	if (mLifetime.mIsActive == TRUE) {
		this->SetFloat(mLifetime.mName, (float)mLifetime.mCount/kTARGET_FPS);
		mLifetime.mCount = mLifetime.mCount + EffectManager::sEffectUpdateSpeed;
	}

	// フェードが設定されている場合の処理
	if (mFade.mIsActive == TRUE) {
		float speed = mFade.mSpeed;
		float new_alpha = this->GetAlpha() + speed;
		if (new_alpha >= 1.0f) {
			new_alpha = 1.0f;
			mFade.mIsActive = FALSE;
			mFade.mSpeed = 0.0f;
		}

		if (new_alpha <= 0.0f) {
			new_alpha = 0.0f;
			mFade.mIsActive = FALSE;
			mFade.mSpeed = 0.0f;
			mIsPlay = false;
		}

		// アルファの更新
		this->SetAlpha(new_alpha);

		// 処理が終わってからエフェクトを削除
		if (mIsPlay == false) {
			deleteEffect();
			return true; // 破棄が完了したので、処理を終了する
		}
	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
EffectNode::calc(void)
{
	if (mMatrixType == eMATRIX_TYPE_MATRIX) return false;

	//<! ＴＲＳの行列を作成
	MATRIX tmp;
	mCalcedMtx = MGetIdent();
	
	tmp = MGetIdent();
	tmp = MGetScale(mScale);
	mCalcedMtx = MMult(mCalcedMtx, tmp);

	tmp = MGetIdent();
	tmp = MGetRotX(mRotation.x);
	mCalcedMtx = MMult(mCalcedMtx, tmp);

	tmp = MGetIdent();
	tmp = MGetRotY(mRotation.y);
	mCalcedMtx = MMult(mCalcedMtx, tmp);

	tmp = MGetIdent();
	tmp = MGetRotZ(mRotation.z);
	mCalcedMtx = MMult(mCalcedMtx, tmp);

	tmp = MGetIdent();
	tmp = MGetTranslate(mPosition);
	mCalcedMtx = MMult(mCalcedMtx, tmp);

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
EffectNode::drawFuncNormal(void)
{
	HRESULT hr;
	UINT pass_cnt, pass_num;

	hr = mpEffect->Begin(&pass_num,	0);
	for (pass_cnt = 0; pass_cnt < pass_num; pass_cnt ++) {
		// パスに応じたモデル描画
		renderPassModel(pass_cnt);
	}
	hr = mpEffect->End();

	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
EffectNode::drawFuncScript(void)
{
	HRESULT hr;
	int script_num = mpResourceRef->GetScriptNum();
	EffectResource::tScriptWork* p_work;

	int script_index = 0;
	int loopByCount	= 0;
	int loopGetIndex= 0;
	int loop_start_script_index = 0;

	float clear_color[4] = {0};
	float clear_depth = 0.0f;

	while (true) {
		p_work = mpResourceRef->GetScriptWork(script_index);
		char* param_string = p_work->mParamString;
		
		switch (p_work->mScriptKind) {
		case eSCRIPT_RenderColorTarget:			 // 0 : レンダリングターゲットを設定します。
		case eSCRIPT_RenderColorTarget0:		 // 1 : レンダリングターゲットを設定します。
			{
				LPDIRECT3DTEXTURE9 p_texture = NULL;
				LPDIRECT3DSURFACE9 render_target = NULL;
				EFFHND_PARM hParam;

				hParam = mpEffect->GetParameterByName(NULL, param_string);
				if (strcmp(param_string, "") == 0) {
					render_target = EffectManager::GetInstance()->GetOrgRenderTarget();
					GET_D3DDEV()->SetRenderTarget(0, render_target);
				} else {
					hr = mpEffect->GetTexture(hParam, (LPDIRECT3DBASETEXTURE9 *)&p_texture);
					APP_ASSERT( (p_texture != NULL),
						"Failed GetTexture() \n");

					hr = p_texture->GetSurfaceLevel(0, &render_target);
					if (SUCCEEDED(hr) ) {
						GET_D3DDEV()->SetRenderTarget(0, render_target);
						render_target->Release();

						D3DSURFACE_DESC surface_desc;
						p_texture->GetLevelDesc(0, &surface_desc);
						mBufferRectWidth	= (USHORT)surface_desc.Width;
						mBufferRectHeight	= (USHORT)surface_desc.Height;
					}
				}
			}

			break;

		case eSCRIPT_RenderColorTarget1:         // 2 : レンダリングターゲットを設定します。
		case eSCRIPT_RenderColorTarget2:         // 3 : レンダリングターゲットを設定します。
		case eSCRIPT_RenderColorTarget3:         // 4 : レンダリングターゲットを設定します。
			APP_ASSERT_PRINT(NULL, "Unsupport this label : %d \n", param_string);
			break;

		case eSCRIPT_RenderDepthStencilTarget:	 // 5 : 深度ステンシルサーフェイスを設定します。
			{
				LPDIRECT3DSURFACE9 p_stencil_surface = NULL;
				if (strcmp(param_string, "") == 0) {
					p_stencil_surface = EffectManager::GetInstance()->GetOrgDepthStencil();
				} else {
					p_stencil_surface = mpResourceRef->GetRenderDepthStencilSurface(mStencilHandle);
				}
				hr = GET_D3DDEV()->SetDepthStencilSurface(p_stencil_surface);
			}
			break;
			
		case eSCRIPT_ClearSetColor:              // 6 : レンダリングターゲットをクリアする色を設定する。
			mpEffect->GetFloatArray(param_string, (FLOAT *)&clear_color, 4);
			break;

		case eSCRIPT_ClearSetDepth:              // 7 : 深度ステンシルサーフェイスをクリアするZ値を設定する。
			mpEffect->GetFloat(param_string, (FLOAT *)&clear_depth);
			break;

		case eSCRIPT_Clear:                      // 8 : (=Color):レンダリングターゲットをクリアします。, (=Depth):深度ステンシルサーフェイスをクリアします。
			if (strcmp(param_string, "Color") == 0) {
				int r, g, b;
				r = COLOR_FLOAT_TO_INT(clear_color[0]);
				g = COLOR_FLOAT_TO_INT(clear_color[1]);
				b = COLOR_FLOAT_TO_INT(clear_color[2]);
				D3DCOLOR color = (D3DCOLOR_ARGB(255, 0, 0, 0));
				hr = GET_D3DDEV()->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, color, clear_depth, 0);
			}
			break;

		case eSCRIPT_ScriptExternal:             // 9 : 他のオブジェクトを描画します。
			{
				LPDIRECT3DSURFACE9 p_stencil_surface = NULL;
				p_stencil_surface = mpResourceRef->GetRenderDepthStencilSurface(mStencilHandle);
				
				mpEffect->Begin( NULL, 0 );
				mpEffect->BeginPass(0);
				DrawRect(0.0f, 
						 0.0f, 
						 (float)mBufferRectWidth, 
						 (float)mBufferRectHeight, 
						 0.0f, 0.0f, 1.0f, 1.0f, 
						 D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
				mpEffect->EndPass();
				mpEffect->End();
			}
			break;

		case eSCRIPT_Pass:
			// パスを通してエフェクトを描画
			funcScript_Pass(param_string);
			break;

		case eSCRIPT_LoopByCount:
			hr = mpEffect->GetInt(param_string, (INT *)&loopByCount);
			loopGetIndex = 0;
			loop_start_script_index = script_index;
			break;

		case eSCRIPT_LoopEnd:
			if (loopGetIndex < loopByCount) {
				loopGetIndex ++;
				script_index = (loop_start_script_index + 1);
				continue;
			}
			break;

		case eSCRIPT_LoopGetIndex:
			hr = mpEffect->SetInt(param_string, (INT)loopGetIndex);
			break;

		default:
			break;
		};

		script_index ++;
		if (script_index == script_num) break;

	}
	return false;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::deleteEffect(void)
{
	// ステンシルサーフェイス情報が存在するリソースの場合、ステンシルサーフェイス番号を解放する
	// ※ハンドルは関数内部で初期化される)
	if (mpResourceRef->IsExistStencilSurface() == true) {
		mpResourceRef->ReleaseStencilSurfaceHandle(mStencilHandle);
	}

	// 複製したエフェクトクラスの破棄
	SAFE_RELEASE(mpEffect);
	mpEffect = NULL;
}

//---------------------------------------------------
/**
 * 四角形の描画
 */
//---------------------------------------------------
void DrawQuad(
			  float x0, float y0, float x1, float y1, 
			  float x2, float y2, float x3, float y3, 
			  float u0, float v0, float u1, float v1, 
			  D3DCOLOR diffuse)
{
	HRESULT hr;

	// 頂点バッファをデバイスに設定
	LPDIRECT3DVERTEXBUFFER9 vb = EffectManager::GetInstance()->GetVertexBuffer();
	if (vb == NULL) return;

	hr = GET_D3DDEV()->SetStreamSource(0, vb, 0, sizeof(MY_VERTEX));
	hr = GET_D3DDEV()->SetFVF(D3DFVF_VERTEX);
	// 頂点バッファのロック
	MY_VERTEX* v;
	if (SUCCEEDED( vb->Lock(0, 4*sizeof(MY_VERTEX), (void**)&v, 0) )) {
		// 頂点座標の設定
		v[0].X=x0; v[0].Y=y0;
		v[1].X=x1; v[1].Y=y1;
		v[2].X=x2; v[2].Y=y2;
		v[3].X=x3; v[3].Y=y3;

		// テクスチャ座標の設定
		v[0].U0=v[0].U1=u0; v[0].V0=v[0].V1=v0;
		v[1].U0=v[1].U1=u1; v[1].V0=v[1].V1=v0;
		v[2].U0=v[2].U1=u0; v[2].V0=v[2].V1=v1;
		v[3].U0=v[3].U1=u1; v[3].V0=v[3].V1=v1;

		// Z, RHW, 頂点色の設定
		for (int i=0; i<4; i++) {
			v[i].Z=0;
			v[i].RHW=1; 
			v[i].Diffuse=diffuse;
		}
		// 頂点バッファの解放
		vb->Unlock();
		// レンダリング条件の設定
		hr = GET_D3DDEV()->SetRenderState(D3DRS_ZENABLE, FALSE);
		hr = GET_D3DDEV()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		// プリミティブの描画
		hr = GET_D3DDEV()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		// レンダリング条件の復帰
		hr = GET_D3DDEV()->SetRenderState(D3DRS_ZENABLE, TRUE);
		hr = GET_D3DDEV()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void DrawRect(float x0, float y0, float x1, float y1, 
			  float u0, float v0, float u1, float v1, D3DCOLOR diffuse)
{
	DrawQuad(x0, y0, x1, y0, x0, y1, 
			 x1, y1, u0, v0, u1, v1, diffuse);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::renderPassModel(int passCnt)
{
	HRESULT hr;
	EffectResource::ePassDrawType pass_draw_type = mpResourceRef->GetPassDrawType(passCnt);
	switch (pass_draw_type) {
	case EffectResource::ePASS_DRAW_TYPE_GEOMETORY:

		for (DWORD iI = 0; iI < mpResourceRef->GetMaterialNum(); iI ++) {
			// エフェクトの開 始
			hr = mpEffect->BeginPass(passCnt);
			mpResourceRef->DrawModel(iI);
			// エフェクトの終了
			hr = mpEffect->EndPass();
		}
		break;

	case EffectResource::ePASS_DRAW_TYPE_BUFFER:
		{
			hr = mpEffect->BeginPass(passCnt);
			DrawRect(0.0f, 
					 0.0f, 
					 (float)mBufferRectWidth, 
					 (float)mBufferRectHeight, 
					 0.0f, 0.0f, 1.0f, 1.0f, 
					 D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
			hr = mpEffect->EndPass();
		}
		break;

	default:
		break;
	};
}

/*=====================================*
 * function
 *=====================================*/
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void EffectNode::funcSmt_TIME(int semIndex, tSemanticSetInfo* pInfo) 
{
	return; // 0;
} 
 
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_WORLD(int semIndex, tSemanticSetInfo* pInfo)
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	mpEffect->SetMatrix(sem_handle, &pInfo->world_mtx);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_VIEW(int semIndex, tSemanticSetInfo* pInfo)
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	mpEffect->SetMatrix(sem_handle, &pInfo->view_mtx);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_VIEWINVERSE(int semIndex, tSemanticSetInfo* pInfo)
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXMATRIXA16 mat_view_inv;
	D3DXMatrixInverse( &mat_view_inv, 0x0, &pInfo->view_mtx );
	mpEffect->SetMatrix(sem_handle, &mat_view_inv );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_VIEWPROJECTION(int semIndex, tSemanticSetInfo* pInfo)
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXMATRIXA16 mat_view_proj;
	D3DXMatrixMultiply( &mat_view_proj, &pInfo->view_mtx, &pInfo->proj_mtx);
	mpEffect->SetMatrix(sem_handle, &mat_view_proj);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_VIEWPROJECTIONINVERSE(int semIndex, tSemanticSetInfo* pInfo)
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXMATRIX mat_view_proj;
	D3DXMatrixMultiply( &mat_view_proj, &pInfo->view_mtx, &pInfo->proj_mtx );
		
	D3DXMATRIXA16 mat_view_proj_inv;
	D3DXMatrixInverse( &mat_view_proj_inv, 0x0, &mat_view_proj );
	mpEffect->SetMatrix(sem_handle, &mat_view_proj_inv);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_PROJECTION(int semIndex, tSemanticSetInfo* pInfo)
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	mpEffect->SetMatrix(sem_handle, &pInfo->proj_mtx);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_PROJECTIONINVERSE(int semIndex, tSemanticSetInfo* pInfo)
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXMATRIX mat_view_proj;
	D3DXMatrixMultiply( &mat_view_proj, &pInfo->view_mtx, &pInfo->proj_mtx );
		
	D3DXMATRIXA16 mat_view_proj_inv;
	D3DXMatrixInverse( &mat_view_proj_inv, 0x0, &mat_view_proj );
	mpEffect->SetMatrix(sem_handle, &mat_view_proj_inv);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_WORLDVIEWPROJECTION(int semIndex, tSemanticSetInfo* pInfo)
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXMATRIXA16 mat_view_proj;
	D3DXMATRIXA16 mat_world_view_proj;
	D3DXMatrixMultiply( &mat_view_proj, &pInfo->view_mtx, &pInfo->proj_mtx );
	D3DXMatrixMultiply( &mat_world_view_proj, &pInfo->world_mtx, &mat_view_proj );
		
	mpEffect->SetMatrix(sem_handle, &mat_world_view_proj );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_WORLDVIEWPROJECTIONINVERSE(int semIndex, tSemanticSetInfo* pInfo)
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXMATRIXA16 mat_view_proj;
	D3DXMATRIXA16 mat_world_view_proj;
	D3DXMATRIXA16 mat_world_view_proj_inv;
	D3DXMatrixMultiply( &mat_view_proj, &pInfo->view_mtx, &pInfo->proj_mtx );
	D3DXMatrixMultiply( &mat_world_view_proj, &pInfo->world_mtx, &mat_view_proj );
	D3DXMatrixInverse( &mat_world_view_proj_inv, 0x0, &mat_world_view_proj );
		
	mpEffect->SetMatrix(sem_handle, &mat_world_view_proj_inv );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_WORLDINVERSETRANSPOSE(int semIndex, tSemanticSetInfo* pInfo)
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXMATRIXA16 mat_world_it;
	D3DXMatrixInverse(&mat_world_it,0x0, &pInfo->world_mtx);
	D3DXMatrixTranspose(&mat_world_it,&mat_world_it);
		
	mpEffect->SetMatrix(sem_handle, &mat_world_it );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_WORLDVIEW(int semIndex, tSemanticSetInfo* pInfo)
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXMATRIXA16 mat_world_view;
	D3DXMatrixMultiply( &mat_world_view, &pInfo->world_mtx, &pInfo->view_mtx );
	mpEffect->SetMatrix(sem_handle, &mat_world_view );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_VIEWINVERSETRANSPOSE(int semIndex, tSemanticSetInfo* pInfo)
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXMATRIXA16 mat_view_it;
	D3DXMatrixInverse(&mat_view_it, 0x0, &pInfo->view_mtx);
	D3DXMatrixTranspose(&mat_view_it,&mat_view_it);
		
	mpEffect->SetMatrix(sem_handle, &mat_view_it );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_WORLDINVERSE(int semIndex, tSemanticSetInfo* pInfo)
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXMATRIXA16 mat_world_inv;
	D3DXMatrixInverse( &mat_world_inv, 0x0, &pInfo->world_mtx );
		
	mpEffect->SetMatrix(sem_handle, &mat_world_inv );
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_WORLDVIEWTRANSPOSE(int semIndex, tSemanticSetInfo* pInfo)
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXMATRIXA16 mat_view_proj;
	D3DXMATRIXA16 mat_world_view_proj;
	D3DXMATRIXA16 mat_world_view_proj_trans;
	D3DXMatrixMultiply( &mat_view_proj, &pInfo->view_mtx, &pInfo->proj_mtx );
	D3DXMatrixMultiply( &mat_world_view_proj, &pInfo->world_mtx, &mat_view_proj );
	D3DXMatrixTranspose(&mat_world_view_proj_trans ,&mat_world_view_proj);

	mpEffect->SetMatrix(sem_handle,
						&mat_world_view_proj_trans);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_WORLDVIEWINVERSE(int semIndex, tSemanticSetInfo* pInfo) 
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXMATRIXA16 mat_view_proj;
	D3DXMATRIXA16 mat_world_view_proj;
	D3DXMATRIXA16 mat_world_view_proj_inv;
	D3DXMatrixMultiply( &mat_view_proj, &pInfo->view_mtx, &pInfo->proj_mtx );
	D3DXMatrixMultiply( &mat_world_view_proj, &pInfo->world_mtx, &mat_view_proj );
	D3DXMatrixInverse(&mat_world_view_proj_inv , 0x00, &mat_world_view_proj);

	mpEffect->SetMatrix(sem_handle,
						&mat_world_view_proj_inv);
}
	
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_VIEWTRANSPOSE(int semIndex, tSemanticSetInfo* pInfo)
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXMATRIXA16 mat_view_t;
	D3DXMatrixTranspose(&mat_view_t, &pInfo->view_mtx);
		
	mpEffect->SetMatrix(sem_handle, &mat_view_t );
}
	
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_POSITION (int semIndex, tSemanticSetInfo* pInfo) 
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	VECTOR cam_pos = CameraManager::GetInstance()->Position();

	D3DXVECTOR4 position;
	position.x  = cam_pos.x;
	position.y  = cam_pos.y;
	position.z  = cam_pos.z;
	position.w  = 0.0f;
	mpEffect->SetVector(sem_handle, &position);
} 

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_DIRECTION (int semIndex, tSemanticSetInfo* pInfo) 
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	VECTOR light_dir = GraphicManager::GetInstance()->LightDirection();

	D3DXVECTOR4 direction;
	direction.x  = light_dir.x;
	direction.y  = light_dir.y;
	direction.z  = light_dir.z;
	direction.w  = 0.0f;
	mpEffect->SetVector(sem_handle, &direction);
} 

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void 
EffectNode::funcSmt_MODELDATA (int semIndex, tSemanticSetInfo* pInfo) 
{
	return; // nop.
} 
	
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void 
EffectNode::funcSmt_DIFFUSE (int semIndex, tSemanticSetInfo* pInfo)
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXVECTOR4 diffuse;
	diffuse.x  = 0.9f;
	diffuse.y  = 0.9f;
	diffuse.z  = 0.9f;
	diffuse.w  = 0.9f;
	mpEffect->SetVector(sem_handle, &diffuse);
} 

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_AMBIENT (int semIndex, tSemanticSetInfo* pInfo) 
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXVECTOR4 diffuse;
	diffuse.x  = 0.9f;
	diffuse.y  = 0.9f;
	diffuse.z  = 0.9f;
	diffuse.w  = 0.9f;
	mpEffect->SetVector(sem_handle, &diffuse);
} 

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_EMISSIVE (int semIndex, tSemanticSetInfo* pInfo) 
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXVECTOR4 diffuse;
	diffuse.x  = 0.9f;
	diffuse.y  = 0.9f;
	diffuse.z  = 0.9f;
	diffuse.w  = 0.9f;
	mpEffect->SetVector(sem_handle, &diffuse);
} 

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_SPECULAR (int semIndex, tSemanticSetInfo* pInfo) 
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXVECTOR4 diffuse;
	diffuse.x  = 0.9f;
	diffuse.y  = 0.9f;
	diffuse.z  = 0.9f;
	diffuse.w  = 0.9f;
	mpEffect->SetVector(sem_handle, &diffuse);
} 

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_SPECULARPOWER (int semIndex, tSemanticSetInfo* pInfo) 
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	float power = 1.0f;
	mpEffect->SetFloat(sem_handle, power);
} 

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_TOONCOLOR (int semIndex, tSemanticSetInfo* pInfo) 
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXVECTOR4 diffuse;
	diffuse.x  = 0.9f;
	diffuse.y  = 0.9f;
	diffuse.z  = 0.9f;
	diffuse.w  = 0.9f;
	mpEffect->SetVector(sem_handle, &diffuse);
}
 
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_EDGECOLOR (int semIndex, tSemanticSetInfo* pInfo) 
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	D3DXVECTOR4 diffuse;
	diffuse.x  = 0.9f;
	diffuse.y  = 0.9f;
	diffuse.z  = 0.9f;
	diffuse.w  = 0.9f;
	mpEffect->SetVector(sem_handle, &diffuse);
} 

void EffectNode::funcSmt_GROUNDSHADOWCOLOR (int semIndex, tSemanticSetInfo* pInfo) { PRINT_CONSOLE("semantic : GROUNDSHADOWCOLOR is Unsupport \n"); } 
void EffectNode::funcSmt_ELAPSEDTIME (int semIndex, tSemanticSetInfo* pInfo) { PRINT_CONSOLE("semantic : ELAPSEDTIME is Unsupport \n"); } 

void EffectNode::funcSmt_CONTROLOBJECT (int semIndex, tSemanticSetInfo* pInfo) 
{ 
	//	PRINT_CONSOLE("semantic : CONTROLOBJECT is Unsupport \n"); 
}
 

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_RENDERCOLORTARGET (int semIndex, tSemanticSetInfo* pInfo) 
{
	return; // nop.
} 

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_RENDERDEPTHSTENCILTARGET (int semIndex, tSemanticSetInfo* pInfo) 
{
	return; // nop.
} 

void EffectNode::funcSmt_TEXTUREVALUE (int semIndex, tSemanticSetInfo* pInfo) { PRINT_CONSOLE("semantic : TEXTUREVALUE is Unsupport \n"); } 

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void EffectNode::funcSmt_STANDARDSGLOBAL (int semIndex, tSemanticSetInfo* pInfo)
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];
	mpEffect->SetFloat(sem_handle, 0.8f);
} 

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcSmt_OFFSCREENRENDERTARGET (int semIndex, tSemanticSetInfo* pInfo) 
{ 
	// 現状、暫定でアサート回避
	//	APP_ASSERT(NULL, "semantic : OFFSCREENRENDERTARGET is Unsupport \n"); 
} 

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void EffectNode::funcSmt_VIEWPORTPIXELSIZE (int semIndex, tSemanticSetInfo* pInfo) 
{
	D3DXHANDLE sem_handle = mSemHandle[semIndex];

	int x, y, depth;
	GetScreenState(&x, &y, &depth);

	const FLOAT float_array[2] = {(float)x, (float)y};
	mpEffect->SetFloatArray(sem_handle, float_array, 2);
} 

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void EffectNode::funcSmt_DUMMY (int semIndex, tSemanticSetInfo* pInfo) 
{ 
	return; // nop.
} 

/*=====================================*
 * スクリプト関数処理
 *=====================================*/
void EffectNode::funcScript_RenderColorTarget (char* pParamString) { PRINT_CONSOLE("script : RenderColorTarget is Unsupport \n"); } 

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcScript_RenderColorTarget0 (char* pParamString) 
{ 
	// 現状処理はいれないが、暫定的にアサート解除
	//	APP_ASSERT(NULL, "script : RenderColorTarget0 is Unsupport \n"); 
} 

void EffectNode::funcScript_RenderColorTarget1 (char* pParamString) { PRINT_CONSOLE("script : RenderColorTarget1 is Unsupport \n"); } 
void EffectNode::funcScript_RenderColorTarget2 (char* pParamString) { PRINT_CONSOLE("script : RenderColorTarget2 is Unsupport \n"); } 
void EffectNode::funcScript_RenderColorTarget3 (char* pParamString) { PRINT_CONSOLE("script : RenderColorTarget3 is Unsupport \n"); } 

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcScript_RenderDepthStencilTarget (char* pParamString) 
{
	// 現状処理はいれないが、暫定的にアサート解除
	//	APP_ASSERT(NULL, "script : RenderDepthStencilTarget is Unsupport \n"); 
} 

void EffectNode::funcScript_ClearSetColor (char* pParamString) { PRINT_CONSOLE("script : ClearSetColor is Unsupport \n"); } 
void EffectNode::funcScript_ClearSetDepth (char* pParamString) { PRINT_CONSOLE("script : ClearSetDepth is Unsupport \n"); } 
void EffectNode::funcScript_Clear (char* pParamString) { PRINT_CONSOLE("script : Clear is Unsupport \n"); } 
void EffectNode::funcScript_ScriptExternal (char* pParamString) { PRINT_CONSOLE("script : ScriptExternal is Unsupport \n"); } 

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcScript_Pass(char* pParamString) 
{
	HRESULT hr;
	UINT pass_num;
	int pass_index = atoi(pParamString);

	hr = mpEffect->Begin(&pass_num, 0);
	// パスに応じたモデル描画
	renderPassModel(pass_index);
	hr = mpEffect->End();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcScript_LoopByCount (char* pParamString)
{
} 

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcScript_LoopEnd (char* pParamString) 
{
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectNode::funcScript_LoopGetIndex (char* pParamString) 
{
}

void EffectNode::funcScript_Draw (char* pParamString) { PRINT_CONSOLE("script : Draw is Unsupport \n"); } 


/**** end of file ****/
