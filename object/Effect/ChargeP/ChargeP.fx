string Charge_ParticleSystem_Particles : ModelData = "Charge.x";

/* ======================================================================== *
 * セマンティック変数
 * ======================================================================== */
float4x4 	WorldViewProjectionMatrix 	: WORLDVIEWPROJECTION;
float4x4	WorldViewTransposeMatrix 	: WORLDVIEWTRANSPOSE;
float		time_0_X 					: Time;

/* ======================================================================== *
 * 変数
 * ======================================================================== */
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
> = float( 0.12 );

float particleSpread
<
	string UIName = "particleSpread";
	string UIWidget = "Numeric";
	bool UIVisible =  true;
	float UIMin = 0.00;
	float UIMax = 50.00;
> = float( 172.100 );

float particleSpeed
<
	string UIName = "particleSpeed";
	string UIWidget = "Numeric";
	bool UIVisible =  true;
	float UIMin = 0.00;
	float UIMax = 2.00;
//> = float( -1.84 );
> = float( -2.5 );

float particleSystemHeight;

float particleSize
<
	string UIName = "particleSize";
	string UIWidget = "Numeric";
	bool UIVisible =  true;
	float UIMin = 0.00;
	float UIMax = 20.00;
> = float( 8.00 );

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

float Alpha
<
	string UIName = "Alpha";
	string UIWidget = "Numeric";
	bool UIVisible =  false;
	float UIMin = 0.00;
	float UIMax = 1.00;
> = float( 1.00 );

float particleShape
<
	string UIName = "particleShape";
	string UIWidget = "Numeric";
	bool UIVisible =  true;
	float UIMin = 0.00;
	float UIMax = 1.00;
> = float( 0.08 );

texture ChargeP_Tex0
< string ResourceName = "ChargeP0.tga"; >;

// The model for the particle system consists of a hundred quads.
// These quads are simple (-1,-1) to (1,1) quads where each quad
// has a z ranging from 0 to 1. The z will be used to differenciate
// between different particles

/* ======================================================================== *
 * 構造体
 * ======================================================================== */
struct VS_OUTPUT {
	float4 Pos: POSITION;
	float2 texCoord: TEXCOORD0;
	float color: TEXCOORD1;
};

/* ======================================================================== *
 * サンプラ
 * ======================================================================== */
sampler Flame = sampler_state
{
	Texture = (ChargeP_Tex0);
	ADDRESSU = CLAMP;
	ADDRESSV = CLAMP;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
};

/* ======================================================================== *
 * 頂点シェーダ
 * ======================================================================== */
VS_OUTPUT Charge_ParticleSystem_Vertex_Shader_main(float4 Pos: POSITION){
	VS_OUTPUT Out;

	// Loop particles
	float t = frac(Pos.z + particleSpeed * time_0_X);

	// Determine the shape of the system
	float s = pow(t, particleSystemShape);

	float3 pos;
	pos.x = particleSpread * s * tan(102 * Pos.z);
	pos.z = particleSpread * s * tan(63 * Pos.z);

	// Particles goes up
//	pos.y = particleSpread * s * tan(10 * Pos.z);
	pos.y = particleSpread * s * tan(10 * Pos.z);

	// リミットをつける
	float max_length = 400.3f;
	float len = length(pos);
/*
	if (len > max_length) {
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
	}
*/
	float add_alpha = 1.0f;
/*
	if (len < 10.0f) {
		add_alpha = 0.0f;
	}
*/	
	// ビルボードの４つご
	// 表示マトリックスは、正しくて上に向かうベクトルを与えます
	pos += particleSize * (Pos.x * WorldViewTransposeMatrix[0] + Pos.y * WorldViewTransposeMatrix[1]);
	// And put the system into place
	pos += particleSystemPosition;
	
	pos *= scale;

	float4x4 tmp = mul(WorldMtx, WorldViewProjectionMatrix);

	Out.Pos = mul(float4(pos, 1), tmp);
	Out.texCoord = Pos.xy;
	Out.color = (1 - t) * Alpha * add_alpha;

	return Out;
}

/* ======================================================================== *
 * ピクセルシェーダ
 * ======================================================================== */
float4 Charge_ParticleSystem_Pixel_Shader_main(float2 texCoord: TEXCOORD0, float color: TEXCOORD1) : COLOR {
	// Fade the particle to a circular shape
	float fade = pow(dot(texCoord, texCoord), particleShape);
	return (1 - fade) * tex2D(Flame, float2(color,0.5f));
}

//--------------------------------------------------------------//
// テクニック
//--------------------------------------------------------------//
technique Charge
{
	pass ParticleSystem
	{
		ZENABLE = TRUE;
		ZWRITEENABLE = FALSE;
		SRCBLEND = ONE;
		DESTBLEND = ONE;
		CULLMODE = NONE;
		ALPHABLENDENABLE = TRUE;

		VertexShader = compile vs_2_0 Charge_ParticleSystem_Vertex_Shader_main();
		PixelShader = compile ps_2_0 Charge_ParticleSystem_Pixel_Shader_main();
	}
}

