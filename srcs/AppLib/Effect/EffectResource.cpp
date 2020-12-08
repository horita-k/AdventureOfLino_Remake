/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * エフェクト管理クラス
 * @author SPATZ.
 * @data   2014/01/28 23:12:38
 */
//---------------------------------------------------

#define _CRT_SECURE_NO_WARNINGS

/*=====================================*
 * include
 *=====================================*/
#include "AppLib/Basic/Basic.h"
#include "AppLib/Graphic/CameraManager.h"

#include "EffectResource.h"
#include "AppLib/Graphic/EffectType.h"

#ifdef _DEBUG
#include "Charactor/CharaBase.h"
#include "AppLib/Graphic/GraphicManager.h"
#endif // _DEBUG

#ifdef _DEBUG
#define PRINT_CONSOLE_EFF(str, ...)		PRINT_CONSOLE(str, __VA_ARGS__);
#else 
#define PRINT_CONSOLE_EFF(str, ...)
#endif // _DEBUG


/*=====================================*
 * static 
 *=====================================*/
static const BYTE kSTENCIL_INFO_NUM = 8;		//<! ここの値は mStencilHandleUseBit(8bit) と連動している為、
												//<! 9以上にする場合は mStencilHandleUseBit の型を WORD / DWORD に変更すること！

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
EffectResource::EffectResource()
{
	// メンバ変数初期化
	initParameters();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
EffectResource::~EffectResource()
{
	Destroy();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectResource::Create(const char* effectName, const char* modelName)
{
	bool result = false;
	D3DXEFFECT_DESC		effect_desc;
	D3DXPARAMETER_DESC	param_desc;
	D3DXTECHNIQUE_DESC	technique_desc;

	LPSTR				tex_name = {0};

	char res_name[128] = {0};
	char root_dir[128] = {0};
	// モデルの読み込み

	// ルートディレクトリの設定
	//<! ここでの res_name は root_dir 生成の為のただのバッファとして使用してるので注意！
	int path_len = strlen(effectName);
	for (int iI = (path_len - 1); iI >= 0; iI -- ) {
		if ((effectName[iI] == '/') || (effectName[iI] == '\\')) {
			memcpy(res_name, effectName, iI);
			break;
		}
	}
	sprintf_s(root_dir, sizeof(root_dir), "object/Effect/%s", res_name);

	// エフェクトファイルの読み込み
	memset(res_name, 0, sizeof(res_name));
	sprintf_s(res_name, sizeof(res_name), "object/Effect/%s", effectName);
	result = loadEff(res_name);
	SYSTEM_ERROR((result != false),
				 "\n %sのエフェクトファイル読み込みに失敗しました。\nゲームを終了します。", res_name);
	
	// モデルファイルの読み込み
	if (modelName != NULL) {
		memset(res_name, 0, sizeof(res_name));
		sprintf_s(res_name, sizeof(res_name), "object/Effect/%s", modelName);
	} else {
		int effname_len = strlen(res_name);
		res_name[effname_len -2] = 'x';
		res_name[effname_len -1] = NULL;
	}
	result = loadMdl(res_name);
	SYSTEM_ERROR((result != false),
				 "\n %sのモデルファイル読み込みに失敗しました。\nゲームを終了します。", res_name);

	// エフェクトの情報を取得
	EffectResource::getEffectInfo(&effect_desc);

	// メンバ変数にエフェクト情報を展開
	mParameterNum	= effect_desc.Parameters;
	mTechniqueNum	= effect_desc.Techniques;
	mFunctionNum	= effect_desc.Functions;
	mpPassNum = new BYTE[mTechniqueNum];
	APP_ASSERT( (mpPassNum != NULL), "Failed new mpPassNum\n");
	// パス数の取得
	for (int iI = 0; iI < mTechniqueNum; iI ++) {
		EffectResource::getTechInfo(&technique_desc, iI);
		mpPassNum[iI] = technique_desc.Passes;
	}

#ifdef _DEBUG
	LogoutEffectInfo();
#endif // _DEBUG

	// パラメータ情報の取得
	D3DXHANDLE parameter_handle= NULL;
	for (BYTE param_cnt = 0; param_cnt < mParameterNum; param_cnt ++) {
		parameter_handle = EffectResource::getParamInfo(&param_desc, param_cnt);
		
		// パラメータがセマンティック情報の場合
		if (param_desc.Semantic != NULL) {
			addSemanticInstance(param_cnt, &param_desc);
		// パラメータがテクスチャ情報の場合かつ、セマンティックが指定されていないパラメータのみをテクスチャファイルと指定する
		} else if ((param_desc.Type == D3DXPT_TEXTURE)     ||
				   (param_desc.Type == D3DXPT_TEXTURE1D)   || 
				   (param_desc.Type == D3DXPT_TEXTURE2D)   ||
				   (param_desc.Type == D3DXPT_TEXTURE3D)) {
			if (param_desc.Semantic == NULL) {
				// アノテーション情報を取得
				tAnnotationWork annowork[1] = {"ResourceName", D3DXPT_STRING, (void *)&tex_name,};
				getAnnotationParam(param_cnt, 1, annowork, (sizeof(annowork) / sizeof(tAnnotationWork)));
				
				memset(res_name, 0x00, sizeof(res_name));
				sprintf_s(res_name, sizeof(res_name), "%s/%s", root_dir, tex_name);
				result = loadTex(res_name, parameter_handle);
				SYSTEM_ERROR((result != false),
							 "\n %sのテクスチャファイル読み込みに失敗しました。\nゲームを終了します。", res_name);
			}
		}
	}
	
	int tech_index = 0;
	EffectResource::getTechInfo(&technique_desc, tech_index);
	// テクニックのスクリプト情報の生成
	if (technique_desc.Annotations != 0) {
		setupTechniqueScript(tech_index);
	}

	// パスの描画情報の生成
	setupDrawTypeByPass(tech_index);

}

//---------------------------------------------------
/**
 * Technique のスクリプト情報を取得する
 */
//---------------------------------------------------
void
EffectResource::setupTechniqueScript(int techIndex)
{
	LPSTR tech_script = NULL;
	char* p_script_buf = NULL;

	char* tokptr = NULL;
	int script_kind = 0;
	char* tok_tmp = NULL;

	D3DXPARAMETER_DESC anno_desc = {0};

	D3DXHANDLE hAnno;
	hAnno = EffectResource::getTechAnnotationInfo(&anno_desc, techIndex);
	mpEffect->GetString(hAnno, (LPCSTR *)&tech_script);

	int script_len = 0;
	script_len = strlen(tech_script);
	p_script_buf = new char[(script_len + 1)];
	memset(p_script_buf, 0, (script_len + 1));

	strcpy_s(p_script_buf, (script_len + 1), (char *)tech_script);

	tScriptWork* p_script = NULL;
	char* access_ptr;

	access_ptr = p_script_buf;

	for (;;) {

		/**** コマンド名の取得 ****/
		tokptr = strtok(access_ptr, "=");
		if (tokptr == NULL) {
			break;
		}

		// ワークの確保
		p_script = new tScriptWork;

		for (script_kind = 0; script_kind < eSCRIPT_MAX; script_kind ++) {
			if (strcmp(tokptr, strScriptName[script_kind]) == 0) {
				break;
			}
		}
		p_script->mScriptKind		= (eScriptKind)script_kind;

		access_ptr = NULL;

		/**** パラメータの取得 ****/
		char check_word = tokptr[strlen(tokptr)+1];
		// Scriptの文字列に = と ; がくっついてる場合の考慮
		memset(p_script->mParamString, 0, sizeof(p_script->mParamString));
		if (check_word != ';') {
			tokptr = strtok(access_ptr,  ";");
			strcpy_s(p_script->mParamString, sizeof(p_script->mParamString), tokptr);
		} else {
			access_ptr = tokptr + (strlen(tokptr) + 1 + 1);
			strcpy_s(p_script->mParamString, sizeof(p_script->mParamString), "");
		}

		/**** パラメータ書き換え処理 ****/
		if (p_script->mScriptKind == eSCRIPT_Pass) {
			D3DXHANDLE hPass = NULL;
			D3DXPASS_DESC pass_desc;
			// パスの場合は、名前に紐づく Pass名に変更する
			for (int iI = 0; iI < mpPassNum[techIndex]; iI ++) {
				EffectResource::getPassInfo(&pass_desc, techIndex, iI);

				if (strcmp(pass_desc.Name, p_script->mParamString) == 0) {
					memset(p_script->mParamString, 0, sizeof(p_script->mParamString));
					sprintf_s(p_script->mParamString, sizeof(p_script->mParamString), "%d", iI);
					break;
				}
			}
		}
		mScriptList.push_back(p_script);
	}

	if (p_script_buf != NULL) {
		delete[] p_script_buf;
		p_script_buf = NULL;
	}

}

//---------------------------------------------------
/**
 * Passの描画タイプをGeometoryかBufferかの情報をセットアップする
 */
//---------------------------------------------------
void
EffectResource::setupDrawTypeByPass(int techIndex)
{
	D3DXPASS_DESC		pass_desc;
	D3DXPARAMETER_DESC	anno_desc;
	LPSTR draw_type_string = NULL;
	
	ePassDrawType pass_draw_type;

	// パスの場合は、名前に紐づく Pass名に変更する
	for (int iI = 0; iI < mpPassNum[techIndex]; iI ++) {

		// デフォルトでは Geometory を設定しておく
		pass_draw_type = ePASS_DRAW_TYPE_GEOMETORY;

		// パス情報取得
		EffectResource::getPassInfo(&pass_desc, techIndex, iI);

		if (pass_desc.Annotations > 0) {
			
			D3DXHANDLE hAnno;
			hAnno = EffectResource::getPassAnnotationInfo(&anno_desc, iI, 0);
			if (strcmp(anno_desc.Name, "Script") == 0) {
				if (anno_desc.Type == D3DXPT_STRING) {
					mpEffect->GetString(hAnno, (LPCSTR *)&draw_type_string);
					if (strcmp(draw_type_string, "Draw=Buffer;") == 0) {
						pass_draw_type = ePASS_DRAW_TYPE_BUFFER;
					}
				}
			}
		}

		mPassDrawTypeList.push_back(pass_draw_type);
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectResource::Destroy(void)
{
	SAFE_RELEASE(mpEffect);
	SAFE_RELEASE(mpBuffer);

	// ステンシル情報の破棄
	if (mpStencilInfo != NULL) {
		for (int iI = 0; iI < kSTENCIL_INFO_NUM; iI ++) {
			for (int iJ = 0; iJ < eRENDER_COLOR_TARGET_MAX; iJ ++) {
				SAFE_RELEASE(mpStencilInfo[iI].mpRenderColorTarget[iJ]);
			}
			SAFE_RELEASE(mpStencilInfo[iI].mpRenderDepthStencilTarget);
		}
		delete[] mpStencilInfo;
		mpStencilInfo = NULL;
	}

	{
		std::vector<IDirect3DTexture9 *>::iterator ite;
		for (ite = mTextureList.begin(); ite != mTextureList.end(); ite ++) {
			SAFE_RELEASE(*ite);
		}
		mTextureList.clear();
	}
	{
		std::vector<tSemanticInfo *>::iterator ite;
		for (ite = mSemanticList.begin(); ite != mSemanticList.end(); ite ++) {
			delete (*ite);
		}
		mSemanticList.clear();
	}
	{
		std::vector<tScriptWork *>::iterator ite;
		for (ite = mScriptList.begin(); ite != mScriptList.end(); ite ++) {
			tScriptWork* p_script = (*ite);
			if (p_script != NULL) {
				delete p_script;
			}
		}
		mScriptList.clear();
	}

	mPassDrawTypeList.clear();

	if (mModelWork.mpMesh != NULL) {
		mModelWork.mpMesh->Release();
		mModelWork.mpMesh = NULL;
	}

	if (mModelWork.mpAdjucency != NULL) {
		mModelWork.mpAdjucency->Release();
		mModelWork.mpAdjucency = NULL;
	}
	if (mModelWork.mpMaterialBuffer != NULL) {
		mModelWork.mpMaterialBuffer->Release();
		mModelWork.mpMaterialBuffer = NULL;
	}
	if (mModelWork.mpEffectInstance != NULL) {
		mModelWork.mpEffectInstance->Release();
		mModelWork.mpEffectInstance = NULL;
	}
	mModelWork.mMaterialsNum = 0;

	if (mpPassNum != NULL) {
		delete[] mpPassNum;
		mpPassNum = NULL;
	}

	// メンバ変数初期化
	initParameters();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
EffectResource::GetSemanticNum(void)
{
	return mSemanticList.size();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
EffectResource::GetSemanticNum(eSmtKind semanticKind)
{
	int num = 0;

	std::vector<tSemanticInfo *>::iterator ite;
	for (ite = mSemanticList.begin(); ite != mSemanticList.end(); ite ++) {
		if ( (*ite)->mSemKind == semanticKind) {
			num ++;
		}
	}
	return num;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
eSmtKind
EffectResource::GetSemanticType(int index)
{
	return mSemanticList.at(index)->mSemKind;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
D3DXHANDLE
EffectResource::GetSemanticName(eSmtKind semanticKind, int index)
{
	int num = 0;
	D3DXEFFECT_DESC		effect_desc		= {0};
	D3DXHANDLE			hParam			= NULL;
	D3DXPARAMETER_DESC	param_desc		= {0};

	std::vector<tSemanticInfo *>::iterator ite;
	for (ite = mSemanticList.begin(); ite != mSemanticList.end(); ite ++) {
		if ( (*ite)->mSemKind == semanticKind) {

			if (index == num) {

				// エフェクトの情報を取得
				mpEffect->GetDesc(&effect_desc);
				
				// パラメータ情報の取得
				hParam = mpEffect->GetParameter(NULL, (*ite)->mSemIndex);
				mpEffect->GetParameterDesc(hParam, &param_desc);
				return param_desc.Name;
			}
			num ++;
		}
	}

	return NULL;

}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
int
EffectResource::GetScriptNum(void)
{
	return mScriptList.size();
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
EffectResource::tScriptWork*
EffectResource::GetScriptWork(int index)
{
	return mScriptList.at(index);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
DWORD
EffectResource::GetMaterialNum(void)
{
	return mModelWork.mMaterialsNum;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectResource::DrawModel(int materialIndex)
{
	mModelWork.mpMesh->DrawSubset(materialIndex);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectResource::GetCloneEffect(ID3DXEffect** ppEffect)
{
	HRESULT hr;

	// リソースのポインタの取得
	LPDIRECT3DDEVICE9 p_device = NULL;

	hr = mpEffect->GetDevice(&p_device);
	APP_ASSERT(hr != D3DERR_INVALIDCALL,
			   "Failed ID3DXEffect::GetDevice()\n");
	hr = mpEffect->CloneEffect(p_device, ppEffect);
	APP_ASSERT(hr != D3DERR_INVALIDCALL,
			   "Failed ID3DXEffect::CloneEffect() : Invalid Call\n");
	APP_ASSERT(hr != D3DXERR_INVALIDDATA,
			   "Failed ID3DXEffect::CloneEffect() : Invalid Data\n");
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
D3DXHANDLE
EffectResource::GetSemanticHandle(ID3DXEffect* pEffect, int index)
{
	return pEffect->GetParameter(NULL, mSemanticList.at(index)->mSemIndex);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
EffectResource::ePassDrawType
EffectResource::GetPassDrawType(int index)
{
	return mPassDrawTypeList.at(index);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
EffectResource::IsExistStencilSurface(void)
{
	return (mpStencilInfo == NULL) ? false : true;
}

//---------------------------------------------------
/**
 * 未使用のステンシルサーフェイスのハンドルを検索してその値を返す
 * ※全て使用されている場合は0を返す。(仕様とする)
 */
//---------------------------------------------------
void
EffectResource::HoldStencilSurfaceHandle(BYTE& rGetHandle)
{
	BYTE check_bit = 0;

	for (int iI = 0; iI < kSTENCIL_INFO_NUM; iI ++) {
		check_bit = (BYTE)(1 << iI);
		if (!(mStencilHandleUseBit & check_bit ) ) {
			rGetHandle = iI;
			mStencilHandleUseBit |= check_bit;
			break;
		}
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectResource::ReleaseStencilSurfaceHandle(BYTE& rGetHandle)
{
	BYTE check_bit = 0;

	check_bit = (BYTE)(1 << rGetHandle);
	mStencilHandleUseBit &= ~check_bit;

	rGetHandle = 0xFF;	//<! 関数内部でハンドルを初期化する
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
IDirect3DTexture9*
EffectResource::GetRenderColorTargetTexture(BYTE StencilSurfaceHandle, int index)
{
	return mpStencilInfo[StencilSurfaceHandle].mpRenderColorTarget[index];
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
IDirect3DSurface9*
EffectResource::GetRenderDepthStencilSurface(BYTE StencilSurfaceHandle)
{
	return mpStencilInfo[StencilSurfaceHandle].mpRenderDepthStencilTarget;
}

#ifdef _DEBUG
//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectResource::LogoutEffectInfo(void)
{
	/* nop. */
}
#endif // _DEBUG

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectResource::initParameters(void)
{
	mpEffect	= NULL;
	mpBuffer	= NULL;
	memset(&mModelWork, 0, sizeof(mModelWork));

	mParameterNum	= 0;
	mTechniqueNum	= 0;
	mFunctionNum	= 0;
	mpPassNum		= NULL;

	mTextureList.clear();
	mSemanticList.clear();
	mScriptList.clear();
	mPassDrawTypeList.clear();
	mStencilHandleUseBit = 0x00;

	mpStencilInfo = NULL;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
EffectResource::loadEff(const char* fileName)
{
	HRESULT hr;

	// ファイルからエフェクトをロード
	hr = D3DXCreateEffectFromFile(GET_D3DDEV(),
								  fileName,
								  NULL,
								  NULL,
								  //								  0,
								  D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY,
								  NULL,
								  &mpEffect,
								  &mpBuffer);
	
    if ( FAILED(hr) ) {
		// ファイルが存在しない
		if (mpBuffer == NULL) {
			PRINT_CONSOLE("%s(L:%d) : mpBuffer is NULL \n", __FILE__, __LINE__);
			return false;
		}
        //エラーメッセージ表示(文字コードがANSI)
        ::MessageBoxA(NULL,(LPCSTR)(mpBuffer->GetBufferPointer()),"",MB_OK);
        mpBuffer->Release();
		PRINT_CONSOLE("%s(L:%d) : FAILED(hr) \n", __FILE__, __LINE__);
		return false;
	}
	if (hr != D3D_OK) {
		PRINT_CONSOLE("%s(L:%d) : hr != D3D_OK \n", __FILE__, __LINE__);
		return false;
	}

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
EffectResource::loadMdl(const char* fileName)
{
	HRESULT hr;
	tModelWork* p_work = &mModelWork;

	hr = D3DXLoadMeshFromX(fileName,					// pFilename
//						   D3DXMESH_MANAGED,			// Options
							0,
						   GET_D3DDEV(),				// pDevice
						   &p_work->mpAdjucency,		// ppAdjacency
						   &p_work->mpMaterialBuffer,	// ppMaterials
						   &p_work->mpEffectInstance,	// ppEffectInstances
						   &p_work->mMaterialsNum,		// pNumMaterials
						   &p_work->mpMesh);			// ppMesh

	if (hr != D3D_OK) {
		if (hr == D3DERR_INVALIDCALL) {
			PRINT_CONSOLE("%s(L:%d) : hr == D3DERR_INVALIDCALL \n", __FILE__, __LINE__);
			return false;
		}
		if (hr == E_OUTOFMEMORY) {
			PRINT_CONSOLE("%s(L:%d) : hr == E_OUTOFMEMORY \n", __FILE__, __LINE__);
			return false;
		}
		PRINT_CONSOLE("%s(L:%d) : hr != D3D_OK \n", __FILE__, __LINE__);
		return false;
	}

	if (p_work->mpMaterialBuffer == NULL) {
		PRINT_CONSOLE("%s(L:%d) : p_work->mpMaterialBuffer is NULL \n", __FILE__, __LINE__);
		return false;
	}

	// マテリアル情報とテクスチャ情報を設定
	D3DXMATERIAL* p_material = (D3DXMATERIAL *)p_work->mpMaterialBuffer->GetBufferPointer();
	if (p_material == NULL) {
		PRINT_CONSOLE("%s(L:%d) : p_material is NULL \n", __FILE__, __LINE__);
		return false;
	}

	//<! 法線ベクトルが設定されていない場合、
	//<! 法線ベクトルを設定し、新たに LPD3DXMESH を複製する
	if ((p_work->mpMesh->GetFVF() & D3DFVF_NORMAL) == false) {
		ID3DXMesh* p_tmp_mesh = NULL;
		p_work->mpMesh->CloneMeshFVF(p_work->mpMesh->GetOptions(),
									 p_work->mpMesh->GetFVF()|D3DFVF_NORMAL,
									 GET_D3DDEV(),
									 &p_tmp_mesh);
		p_work->mpMesh->Release();
		p_work->mpMesh = NULL;
		p_work->mpMesh = p_tmp_mesh;
	}

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
EffectResource::loadTex(const char* fileName, D3DXHANDLE hParam)
{
	IDirect3DTexture9* p_texture = NULL;

	HRESULT hr;
	DWORD width = 256;
	DWORD height = 256;
	DWORD miplevel = 0;
	// テクスチャの性質や確保するメモリの場所を指定するオプション
	DWORD usage = D3DUSAGE_AUTOGENMIPMAP;
	DWORD filter_flag = ( D3DX_FILTER_POINT, 
						  D3DX_FILTER_LINEAR, 
						  (D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER));

	/* ①ファイルを名前で直接読み込む場合 */
	hr = D3DXCreateTextureFromFile(GET_D3DDEV(),
								   fileName,
								   &p_texture);
	if (hr != D3D_OK) {
		PRINT_CONSOLE("%s(L:%d) : hr = 0x%x \n", __FILE__, __LINE__, hr);
		return false;
	}

	// テクスチャとエフェクトの関連付け
	hr = mpEffect->SetTexture(hParam, p_texture);

	if (hr != D3D_OK) {
		PRINT_CONSOLE("%s(L:%d) : hr = 0x%x \n", __FILE__, __LINE__, hr);
	}

	mTextureList.push_back(p_texture);

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectResource::getAnnotationParam(int paramIndex, int annoNum, tAnnotationWork* pAnnoWork, int annoWorkNum)
{
	HRESULT hr;
	D3DXHANDLE hAnno;
	D3DXPARAMETER_DESC	anno_desc = {0};
	tAnnotationWork*	p_annowork = NULL;

	for (int iI = 0; iI < annoNum; iI ++) {
		hAnno = EffectResource::getParamAnnotationInfo(&anno_desc, paramIndex, iI);

		for (int iJ = 0; iJ < annoWorkNum; iJ ++) {
			p_annowork = &pAnnoWork[iJ];
			if (strcmp(anno_desc.Name, p_annowork->mAnnoName) == 0) {
				
				switch (p_annowork->mParamType) {
				case D3DXPT_INT:
					hr = mpEffect->GetInt(hAnno, (INT *)p_annowork->mpParam);
					break;
				case D3DXPT_STRING:
					hr = mpEffect->GetString(hAnno, (LPCSTR *)p_annowork->mpParam);
					break;

					//<! 現状はこれだけ、　他に使いたい場合は拡張してね！

				default:
					APP_ASSERT_PRINT(NULL, 
									 "Not Support annotation param type %d \n", p_annowork->mParamType);
					break;
				};
			}
			APP_ASSERT(hr == D3D_OK, "Failed getAnnotationParam() \n");

		}
	}	
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
DWORD
EffectResource::getD3DFORMAT_FromString(char* format)
{
	if (strcmp(format, "A8R8G8B8") == 0) {
		return D3DFMT_A8R8G8B8;
	} else if (strcmp(format, "A32B32G32R32F") == 0) {
		return D3DFMT_A32B32G32R32F;
	} else if (strcmp(format, "D24S8") == 0) {
		return D3DFMT_D24S8;
	} else if (strcmp(format, "D3DFMT_A8R8G8B8") == 0){
		return D3DFMT_A8R8G8B8;
	}else{
		APP_ASSERT_PRINT(NULL,
						 "Failed getD3DFORMAT_FromString() Unsupport format %d \n", format);
	}

	return 0xFFFFFFFF;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectResource::getEffectInfo(D3DXEFFECT_DESC* pDesc)
{
	mpEffect->GetDesc(pDesc);
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
D3DXHANDLE
EffectResource::getTechInfo(D3DXTECHNIQUE_DESC* pDesc, int techIndex)
{
	D3DXHANDLE hTechnique = NULL;
	hTechnique = mpEffect->GetTechnique(techIndex);
	mpEffect->GetTechniqueDesc(hTechnique, pDesc);

	return hTechnique;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
D3DXHANDLE
EffectResource::getTechAnnotationInfo(D3DXPARAMETER_DESC* pDesc, int techIndex)
{
	D3DXHANDLE hTechnique;
	hTechnique = mpEffect->GetTechnique(techIndex);
	D3DXHANDLE hAnno;
	hAnno = mpEffect->GetAnnotation(hTechnique, 0);

	mpEffect->GetParameterDesc(hAnno, pDesc);

	return hAnno;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
D3DXHANDLE
EffectResource::getPassInfo(D3DXPASS_DESC* pDesc, int techIndex, int passIndex)
{
	D3DXHANDLE hTechnique, hPass;
	hTechnique	= mpEffect->GetTechnique(techIndex);
	hPass		= mpEffect->GetPass(hTechnique, passIndex);
	mpEffect->GetPassDesc(hPass, pDesc);

	return hPass;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
D3DXHANDLE
EffectResource::getPassAnnotationInfo(D3DXPARAMETER_DESC* pDesc, int techIndex, int passIndex)
{
	D3DXHANDLE hTechnique, hPass, hAnno;
	hTechnique	= mpEffect->GetTechnique(techIndex);
	hPass		= mpEffect->GetPass(hTechnique, passIndex);
	hAnno		= mpEffect->GetAnnotation(hPass, 0);

	mpEffect->GetParameterDesc(hAnno, pDesc);

	return hAnno;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
D3DXHANDLE
EffectResource::getParamInfo(D3DXPARAMETER_DESC* pDesc, int paramIndex)
{
	D3DXHANDLE handle = NULL;
	handle = mpEffect->GetParameter(NULL, paramIndex);
	mpEffect->GetParameterDesc(handle, pDesc);

	return handle;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
D3DXHANDLE
EffectResource::getParamAnnotationInfo(D3DXPARAMETER_DESC* pAnnoDesc, int paramIndex, int annoIndex)
{
	D3DXHANDLE hParam, hAnno;
	hParam = mpEffect->GetParameter(NULL, paramIndex);
	hAnno  = mpEffect->GetAnnotation(hParam, annoIndex);
	mpEffect->GetParameterDesc(hAnno, pAnnoDesc);

	return hAnno;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
bool
EffectResource::addSemanticInstance(int paramIndex, D3DXPARAMETER_DESC* pParamDesc)
{
	char sementic_name[32] = {0};

	// セマンティック名を大文字にする
	strcpy_s(sementic_name, sizeof(sementic_name), pParamDesc->Semantic);
	for (unsigned int iI = 0; iI < strlen(sementic_name); iI ++) {
		sementic_name[iI] = toupper( sementic_name[iI] );
	}

	int sem_kind = 0;
	for (sem_kind = 0; sem_kind < eSMT_MAX; sem_kind ++) {
		if (strcmp(sementic_name, strSmtName[sem_kind]) == 0) {
			break;
		}
	}
	APP_ASSERT_PRINT(sem_kind != eSMT_MAX,
					 "semantic %s is Unsupport name \n", pParamDesc->Semantic);

#ifdef _DEBUG
	if (sem_kind == eSMT_MAX) {
		sem_kind = eSMT_DUMMY;
	}
#endif // _DEBUG

	tSemanticInfo* p_info = new tSemanticInfo();
	p_info->mSemKind	= (eSmtKind)sem_kind;
	p_info->mSemIndex	= paramIndex;

	/**** セマンティック固有のリソースを確保する ****/
	switch (sem_kind) {
	case eSMT_RENDERCOLORTARGET:
		{
			// ステンシル情報を生成されていない場合、生成する
			if (mpStencilInfo == NULL) {
				mpStencilInfo = new tStencilInfo[kSTENCIL_INFO_NUM];
				APP_ASSERT( (mpStencilInfo != NULL),
							"Failed new tStencilInfo[] \n");
				memset(mpStencilInfo, 0, (sizeof(tStencilInfo) * kSTENCIL_INFO_NUM) );
			}

			// Texture 情報が設定されていない RENDERCOLORTARGET の番号を探す
			int target_index;
			for (target_index = 0; target_index < eRENDER_COLOR_TARGET_MAX; target_index++) {
				if (mpStencilInfo[0].mpRenderColorTarget[target_index] == NULL) {
					break;
				}
			}
			APP_ASSERT( (target_index < eRENDER_COLOR_TARGET_MAX), "no empty RenderColorTarget Texture \n");

			// 指定の RENDERCOLORTARGET? に対して、内部で kSTENCIL_INFO_NUM の要素分テクスチャの生成を行う
			resourceSMT_RENDERCOLORTARGET(target_index, paramIndex, pParamDesc, p_info);
		}
		break;

	case eSMT_RENDERDEPTHSTENCILTARGET:
		{
			
			// 指定の RENDERDEPTHSTENCILTARGET に対して、内部で kSTENCIL_INFO_NUM の要素分サーフェイスの生成を行う
			resourceSMT_RENDERDEPTHSTENCILTARGET(paramIndex, pParamDesc, p_info);
		}
		break;

	default:
		break;
	};

	// セマンティックリストに追加する
	mSemanticList.push_back(p_info);

	return true;
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
// サーフェイスの幅高取得関数
bool GetSurfaceWH( IDirect3DSurface9 *pSurf, UINT &uiWidth, UINT &uiHeight )
{
	if( !pSurf ) return false;

	D3DSURFACE_DESC SufDesc;
	pSurf->GetDesc( &SufDesc );
	uiWidth  = SufDesc.Width;
	uiHeight = SufDesc.Height;

	return true;
}

//---------------------------------------------------
/**
 * セマンティックに RENDERCOLORTARGET が指定の場合に
 * 必要なリソース(テクスチャ)を用意する
 * RENDERCOLORTARGET 記載のアノテーションのパラメータは以下を参照
 *		https://sites.google.com/site/mikumikumoving/manual/effect-1
 *
 ・ string ResourceType
　 テクスチャの種類を指定します。型と矛盾した値は指定できません。
　 "2D" "3D" "CUBE"
　 型が"texture"かつ"2D"以外のテクスチャを生成する場合には、必ずこのアノテーションを指定してください。
　 通常の2Dテクスチャでは省略可能です。

・ string ResourceName
　 テクスチャ画像ファイルを指定します。
　 サポートしているファイルフォーマットは bmp, dds, dib, jpg, png, tga です。
　 相対パスでファイルを指定した場合、エフェクトファイルが入っているフォルダが基準になります。

・ int Width
・ int Height
・ int Depth
　 テクスチャの幅、高さ、深さをピクセル単位で指定します。
　 Dimensions, ViewportRatio とは同時に指定できません。
　 デフォルト値は64です。ResourceNameが指定されている場合は、画像ファイルから自動的に取得されます。

・ int2 or int3 Dimensions
　 テクスチャの幅、高さ、深さをピクセル単位で指定します。
　 ViewportRatio, Width, Height, Depth と同時には指定できません。

・ float2 ViewportRatio
　 テクスチャの幅と高さを、スクリーンサイズの比で指定します。
　 スクリーンと同じサイズのテクスチャを生成するには、"float2 ViewportRatio = {1.0, 1.0};" と指定します。
　 Dimensions, Width, Height, Depthとは同時に指定できません。

・ string Format
　 テクスチャのフォーマットを指定します。
　 省略した場合、"A8R8G8B8"が使用されます。
　 指定可能なフォーマットは、D3DFORMAT（http://msdn.microsoft.com/ja-jp/library/bb172558(v=VS.85).aspx)を参照してください。
　 "A8R8G8B8"、"FMT_A8R8G8B8"、"D3DFMT_A8R8G8B8"のいずれの書式でも指定できます。

・ int Miplevels
　 ミップマップを指定したレベルで生成します。
　 省略するか0を指定した場合、完全なミップマップチェーンが作成されます。
　 1を指定すると、ミップマップは生成されません。

・ int Levels
　 Miplevelsの別名です。
 */
//---------------------------------------------------
void
EffectResource::resourceSMT_RENDERCOLORTARGET(int targetIndex, int paramIndex, D3DXPARAMETER_DESC* pParamDesc, tSemanticInfo* pSmtInfo)
{
	IDirect3DTexture9* p_texture = NULL;
	HRESULT	hr = NULL;

	// 生成テクスチャのパラメータのデフォルト値
	float	ViewportRatio[2]= {1.0f, 1.0f};
	INT		Width			= 0;
	INT		Height			= 0;
	char*	Format			= (char*)"A8R8G8B8";

	GetWindowSize((int *)&Width, (int *)&Height);
	tAnnotationWork anno_work[3] = {
		{"Width",		D3DXPT_INT,		(void *)&Width},
		{"Height",		D3DXPT_INT,		(void *)&Height},
		{"Format",		D3DXPT_STRING,	(void *)&Format},
	};


	getAnnotationParam(paramIndex, pParamDesc->Annotations, anno_work, (sizeof(anno_work))/ sizeof(tAnnotationWork)) ;

	D3DFORMAT format_value = (D3DFORMAT)0;
	format_value = (D3DFORMAT)getD3DFORMAT_FromString(Format);

	for (int stencil_index = 0; stencil_index < kSTENCIL_INFO_NUM; stencil_index ++) {

		hr = D3DXCreateTexture (GET_D3DDEV(),			// 
								Width,					// UINT Width,
								Height,					// UINT Height,
								1,						// UINT Levels,
								D3DUSAGE_RENDERTARGET,	// DWORD Usage,
								format_value,			// D3DFORMAT Format,
								D3DPOOL_DEFAULT,		// D3DPOOL Pool,
								&p_texture);			// IDirect3DTexture9** pp_texture,
		APP_ASSERT((hr == D3D_OK), "Failed D3DXCreateTexture()\n");
		
		mpStencilInfo[stencil_index].mpRenderColorTarget[targetIndex] = p_texture;
	}
}

//---------------------------------------------------
/**
	
 */
//---------------------------------------------------
void
EffectResource::resourceSMT_RENDERDEPTHSTENCILTARGET(int paramIndex, D3DXPARAMETER_DESC* pParamDesc, tSemanticInfo* pSmtInfo)
{
	HRESULT	hr = NULL;

	INT Width = 0;
	INT Height = 0;
	char* Format = NULL;
	D3DFORMAT format_value = (D3DFORMAT)0;

	tAnnotationWork anno_work[3] = {
		{"Width",		D3DXPT_INT,		(void *)&Width},
		{"Height",		D3DXPT_INT,		(void *)&Height},
		{"Format",		D3DXPT_STRING,	(void *)&Format},
	};

	getAnnotationParam(paramIndex, pParamDesc->Annotations, anno_work, (sizeof(anno_work) / sizeof(tAnnotationWork)));
	format_value = (D3DFORMAT)getD3DFORMAT_FromString(Format);

	IDirect3DTexture9* p_texture = NULL;
	// Z値テクスチャの幅高を保持
	UINT uiZTexWidth = 0;
	UINT uiZTexHeight = 0;
	// Z値テクスチャサーフェイスを保持
	IDirect3DSurface9* pZTexSurf = NULL;

	for (int stencil_index = 0; stencil_index < kSTENCIL_INFO_NUM; stencil_index ++) {

		// RENDERCOLORTARGET0 のテクスチャ情報を取得
		p_texture = mpStencilInfo[stencil_index].mpRenderColorTarget[0];
		p_texture->GetSurfaceLevel( 0, &pZTexSurf );
		GetSurfaceWH( pZTexSurf, uiZTexWidth, uiZTexHeight );

		// 深度バッファサーフェイスの作成
		hr = GET_D3DDEV()->CreateDepthStencilSurface(uiZTexWidth,
													 uiZTexHeight, 
													 format_value, // D3DFMT_D24S8
													 D3DMULTISAMPLE_NONE, 
													 0, 
													 FALSE, 
													 &mpStencilInfo[stencil_index].mpRenderDepthStencilTarget,
													 NULL);
		
		APP_ASSERT((hr == D3D_OK), "Failed CreateDepthStencilSurface() \n");
	}

}

/**** end of file ****/
