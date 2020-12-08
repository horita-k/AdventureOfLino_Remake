/* ======================================================================== *
 * セマンティック変数
 * ======================================================================== */
float4x4 	WorldMatrix					: WORLD;               // ワールド変換行列
float4x4 	WorldViewProjectionMatrix 	: WorldViewProjection;
float4x4	WorldViewTransposeMatrix 	: WorldViewTranspose;
float4x4	ViewTransposeMatrix 		: ViewTranspose;

float4x4	ViewProjectionMatrix		: ViewProjection;

//カメラ座標
float3   	CameraPosition    			: POSITION  < string Object = "Camera"; >;

float		time_0_X 					: Time;

//複製の本数
int CloneNum = 1;
//保存用変数
int index = 0;

/* ======================================================================== *
 * 変数
 * ======================================================================== */
string Hit_ParticleSystem_Particles : ModelData = "hit.x";

float4 particleSystemPosition
<
	string UIName = "particleSystemPosition";
	string UIWidget = "Direction";
	bool UIVisible =  true;
	float4 UIMin = float4( -100.00, -100.00, -100.00, -100.00 );
	float4 UIMax = float4( 100.00, 100.00, 100.00, 100.00 );
	bool Normalize =  false;
> = float4( 0.00, 0.00, 0.00, 1.00 );

float particleSystemShape
<
	string UIName = "particleSystemShape";
	string UIWidget = "Numeric";
	bool UIVisible =  true;
	float UIMin = 0.00;
	float UIMax = 2.00;
> = float( 0.25 );

float particleSpread
<
	string UIName = "particleSpread";
	string UIWidget = "Numeric";
	bool UIVisible =  true;
	float UIMin = 0.00;
	float UIMax = 50.00;
> = float( 8.00 );

float particleSpeed
<
	string UIName = "particleSpeed";
	string UIWidget = "Numeric";
	bool UIVisible =  true;
	float UIMin = 0.00;
	float UIMax = 2.00;
> = float( 2.94 );

float particleSystemHeight;

float particleSize
<
	string UIName = "particleSize";
	string UIWidget = "Numeric";
	bool UIVisible =  true;
	float UIMin = 0.00;
	float UIMax = 20.00;
> = float( 1.00 );

float3 scale
<
	string UIName = "scale";
	string UIWidget = "Numeric";
	bool UIVisible =  false;
	float UIMin = -1.00;
	float UIMax = 1.00;
> = float3( 0.05, 0.05, 0.05 );

float4x4 WorldMtx
<
	string UIName = "WorldMtx";
	string UIWidget = "Numeric";
	bool UIVisible =  false;
> = float4x4( 1.00, 0.00, 0.00, 0.00, 0.00, 1.00, 0.00, 0.00, 0.00, 0.00, 1.00, 0.00, 0.00, 0.00, 0.00, 1.00 );

float particleShape
<
	string UIName = "particleShape";
	string UIWidget = "Numeric";
	bool UIVisible =  true;
	float UIMin = 0.00;
	float UIMax = 1.00;
> = float( 0.03 );

/* ======================================================================== *
 * 外部設定変数
 * ======================================================================== */
float4 ExtSetPosA[12];
float4 ExtSetPosB[12];

float Alpha 		= 1.0f;

//背景画像テクスチャ
texture BackTex <
    string ResourceName = "test_tex.png";
>;
//背景画像参照用サンプラ
sampler BackSampler = sampler_state {
    texture 	= <BackTex>;
    MinFilter	= LINEAR;
    MagFilter	= LINEAR;
    MipFilter	= NONE;
    AddressU	= CLAMP;
    AddressV	= CLAMP;
};

// The model for the particle system consists of a hundred quads.
// These quads are simple (-1,-1) to (1,1) quads where each quad
// has a z ranging from 0 to 1. The z will be used to differenciate
// between different particles

/* ======================================================================== *
 * 構造体
 * ======================================================================== */
struct VS_OUTPUT {
	float4 Pos			: POSITION;
	float3 Normal		: TEXCOORD0;
	float3 Eye			: TEXCOORD1;
	float2 ScreenTex	: TEXCOORD2;
};

/* ======================================================================== *
 * 頂点シェーダ
 * ======================================================================== */
VS_OUTPUT Hit_ParticleSystem_Vertex_Shader_main(
	float4 Pos		: POSITION, 		/* ローカル座標の頂点	*/
	float3 Normal 	: NORMAL			/* 法線					*/
)
{
	VS_OUTPUT Out;

	// Loop particles
	float t = frac(Pos.z + particleSpeed * time_0_X);

	float3 world_pos = float3(0, 0, 0);
	world_pos.x = WorldMatrix._m30;
	world_pos.y = WorldMatrix._m31;
	world_pos.z = WorldMatrix._m32;
	
	float3 set_localpos;
	set_localpos.x = Pos.x;
	set_localpos.y = Pos.y;
	set_localpos.z = Pos.z;

	float3 set_pos = float3(0,0,0);
	
	set_localpos = Pos;
	
	const float kCMP_DIFF_VALUE = 0.001f;
	const float kSTART_CHECK_X	= 0.05f;
	const float kSHIFT_X		= -0.02f;
	
	// 右端の頂点から順番に...
	for (int iI = 0; iI < 12; iI ++) {
		if (Pos.x > (kSTART_CHECK_X + (kSHIFT_X * (float)iI)) - kCMP_DIFF_VALUE) {
			if (Pos.y > 0)	{
				set_localpos = ExtSetPosA[iI] - world_pos;
			} 
			else {
				set_localpos = ExtSetPosB[iI] - world_pos;
			}
			break;
		}
	}

	set_localpos -= Pos;
		
	/* なぜか２倍してあげるとうまくいく... */
	set_localpos *= 2.0f;

	// ビルボードの４つご
	// 表示マトリックスは、正しくて上に向かうベクトルを与えます
	set_localpos += particleSize * (Pos.x * WorldViewTransposeMatrix[0] + Pos.y * WorldViewTransposeMatrix[1]);
	// And put the system into place
	set_localpos += particleSystemPosition;
	
	set_localpos *= scale;

	// World -> Localで実験
	float4x4 tmp = WorldViewProjectionMatrix;
	
	Out.Pos 	= mul(float4(set_localpos, 1), tmp);
	
	// 法線をワールド行列で回転
	Out.Normal	= mul( Normal, WorldMatrix);
	
	// カメラと相対位置計算
	Out.Eye		= CameraPosition - mul( Pos, WorldMatrix);
	
	// スクリーン座標を計算
	Out.ScreenTex.x = (Out.Pos.x / Out.Pos.w);
	Out.ScreenTex.y = -(Out.Pos.y / Out.Pos.w);

	Out.ScreenTex.x = 0.5 * (abs(Out.Pos.x) / Out.Pos.w);
	Out.ScreenTex.y = 1 * (abs(Out.Pos.y) / Out.Pos.w);

	
	return Out;
}

/* ======================================================================== *
 * ピクセルシェーダ
 * ======================================================================== */
float4 Hit_ParticleSystem_Pixel_Shader_main(VS_OUTPUT IN) : COLOR
{
	return float4(IN.ScreenTex,1,1);
}

//--------------------------------------------------------------//
// テクニック
//--------------------------------------------------------------//
technique lineSystem
<string Script = 
					//描画対象をメイン画面に
        			"RenderColorTarget0=;"
				    "RenderDepthStencilTarget=;"
				    //パスの選択
					"LoopByCount=CloneNum;"
			        "LoopGetIndex=index;"
				    "Pass=lineSystem;"
			        "LoopEnd=;" ;> 

{
	pass lineSystem
	{
		ZENABLE = TRUE;
		ZWRITEENABLE = FALSE;
		SRCBLEND = ONE;
		DESTBLEND = ONE;
		CULLMODE = NONE;
		ALPHABLENDENABLE = TRUE;

		VertexShader = compile vs_2_0 Hit_ParticleSystem_Vertex_Shader_main();
		PixelShader = compile ps_2_0 Hit_ParticleSystem_Pixel_Shader_main();
	}
}

