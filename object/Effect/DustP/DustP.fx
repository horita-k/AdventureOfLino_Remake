//--------------------------------------------------------------//
// Dust
//--------------------------------------------------------------//
//--------------------------------------------------------------//
// ParticleSystem
//--------------------------------------------------------------//
//string Dust_ParticleSystem_Particles : ModelData = ".\\models\\DustP.x";

float4x4 WorldViewProjectionMatrix : WORLDVIEWPROJECTION;
float4x4 WorldViewTransposeMatrix : WORLDVIEWTRANSPOSE;
float time_0_X : Time;
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
> = float( 0.45 );
float particleSpread
<
   string UIName = "particleSpread";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 50.00;
> = float( 50.00 );
float particleSpeed
<
   string UIName = "particleSpeed";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 2.00;
> = float( 1.94 );
float particleSystemHeight;
float particleSize
<
   string UIName = "particleSize";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 20.00;
> = float( 1.60 );
/*
float3 scale
<
   string UIName = "scale";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float3( 1.15, 1.15, 1.15 );
*/

float scale = 0.04f;

float gravity = -0.198f;

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

// The model for the particle system consists of a hundred quads.
// These quads are simple (-1,-1) to (1,1) quads where each quad
// has a z ranging from 0 to 1. The z will be used to differenciate
// between different particles

struct VS_OUTPUT {
   float4 Pos: POSITION;
   float2 texCoord: TEXCOORD0;
   float color: TEXCOORD1;
};

VS_OUTPUT Dust_ParticleSystem_Vertex_Shader_main(float4 Pos: POSITION){
   VS_OUTPUT Out;

/*
	if (time_0_X > 10) {
		scale = 0.0f;
	}
*/

   // Loop particles
   float t = frac(Pos.z + particleSpeed * time_0_X);
//   float t = frac(Pos.z + particleSpeed);

   // Determine the shape of the system
   float s = pow(t, particleSystemShape);

   float3 pos;
   // Spread particles in a semi-random fashion
   pos.x = particleSpread * s * tan(62 * Pos.z);
   pos.z = particleSpread * s * tan(163 * Pos.z);


   // Particles goes up
//   pos.y = particleSystemHeight * t;
   pos.y = particleSpread * s * tan(100 * Pos.z);

   // Billboard the quads.
   // The view matrix gives us our right and up vectors.
   pos += particleSize * (Pos.x * WorldViewTransposeMatrix[0] + Pos.y * WorldViewTransposeMatrix[1]);
   // And put the system into place
   pos += particleSystemPosition;
   
   float V0 = 2;
	float time = (time_0_X * (Pos.z + 4.0f)) * 10.0f;
   pos.y += (V0 * time) + 0.5 * gravity * (time * time);
   if (pos.y < 0.0f) {
/*
   		pos.x = 0.0f;
   		pos.y = 0.0f;
   		pos.z = 0.0f;
   		*/
   }
   
//   pos *= scale;
	pos.x *= scale;
	pos.y *= scale;
	pos.z *= scale;

   float4x4 tmp = mul(WorldMtx, WorldViewProjectionMatrix);
//   float4x4 tmp = mul(WorldViewProjectionMatrix, WorldMtx);

//   Out.Pos = mul(tmp, float4(pos, 1));
   Out.Pos = mul(float4(pos, 1), tmp);
   Out.texCoord = Pos.xy;
   Out.color = (1 - t) * Alpha;

   return Out;
}


float particleShape
<
   string UIName = "particleShape";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 1.00;
> = float( 0.03 );
texture DustP_Tex0
<
   string ResourceName = "DustP0.tga";
>;
sampler Flame = sampler_state
{
   Texture = (DustP_Tex0);
   ADDRESSU = CLAMP;
   ADDRESSV = CLAMP;
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
};
float4 Dust_ParticleSystem_Pixel_Shader_main(float2 texCoord: TEXCOORD0, float color: TEXCOORD1) : COLOR {
   // Fade the particle to a circular shape
   float fade = pow(dot(texCoord, texCoord), particleShape);
   return (1 - fade) * tex2D(Flame, float2(color,0.0f));
}


//--------------------------------------------------------------//
// Technique Section for Dust
//--------------------------------------------------------------//
technique Dust
{
   pass ParticleSystem
   {
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      SRCBLEND = ONE;
      DESTBLEND = ONE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;

      VertexShader = compile vs_2_0 Dust_ParticleSystem_Vertex_Shader_main();
      PixelShader = compile ps_2_0 Dust_ParticleSystem_Pixel_Shader_main();
   }

}

