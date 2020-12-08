//--------------------------------------------------------------//
// FireParticleSystem
//--------------------------------------------------------------//
//--------------------------------------------------------------//
// ParticleSystem
//--------------------------------------------------------------//

float4x4 WorldViewProjectionMatrix : WORLDVIEWPROJECTION;
float4x4 WorldViewTransposeMatrix : WORLDVIEWTRANSPOSE;
static float3 billboard_vec_x = normalize(WorldViewTransposeMatrix[0].xyz);
static float3 billboard_vec_y = normalize(WorldViewTransposeMatrix[1].xyz);

float time_0_X : Time;
float particleSystemShape
<
   string UIName = "particleSystemShape";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 2.00;
> = float( 0.10 );
float particleSpread
<
   string UIName = "particleSpread";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 50.00;
//> = float( 10.00 );
> = float( 7.00 );
float particleSpeed
<
   string UIName = "particleSpeed";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 2.00;
//> = float( 0.48 );
> = float( 0.08 );

float particleSystemHeight
<
   string UIName = "particleSystemHeight";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 160.00;
//> = float( 40.00 );
> = float( 11.00 );

float particleSize
<
   string UIName = "particleSize";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 20.00;
//> = float( 3.80 );
> = float( 1.20 );

float Alpha
<
   string UIName = "Alpha";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = 0.00;
   float UIMax = 1.00;
> = float( 1.00 );

struct VS_OUTPUT {
   float4 Pos: POSITION;
   float2 texCoord: TEXCOORD0;
   float color: TEXCOORD1;
};

VS_OUTPUT FireParticleSystem_Vertex_Shader_main(float4 Pos: POSITION){
   VS_OUTPUT Out;

   // Loop particles
   float t = frac(Pos.z + particleSpeed * time_0_X);
   // Determine the shape of the system
   float s = pow(t, particleSystemShape);

   float3 pos;
   // Spread particles in a semi-random fashion
   pos.x = particleSpread * s * cos(62 * Pos.z);
   pos.z = particleSpread * s * sin(163 * Pos.z);
   // Particles goes up
   pos.y = particleSystemHeight * t;

   // Billboard the quads.
   // The view matrix gives us our right and up vectors.
   //pos += particleSize * (Pos.x * WorldViewTransposeMatrix[0] + Pos.y * WorldViewTransposeMatrix[1]);
   pos += particleSize * (Pos.x * billboard_vec_x + Pos.y * billboard_vec_y);
   pos /= 10;
   
   Out.Pos = mul(float4(pos, 1), WorldViewProjectionMatrix);
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
> = float( 0.06 );
texture Flame_Tex0
<
   string ResourceName = "Forest0.png";
>;
sampler Flame = sampler_state
{
   Texture = (Flame_Tex0);
   ADDRESSU = CLAMP;
   ADDRESSV = CLAMP;
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
};
float4 FireParticleSystem_Pixel_Shader_main(float2 texCoord: TEXCOORD0, float color: TEXCOORD1) : COLOR {
   // Fade the particle to a circular shape
   float fade = pow(dot(texCoord, texCoord), particleShape);
   return (1 - fade) * tex2D(Flame, float2(color,0.5f));
}


//--------------------------------------------------------------//
// Technique Section for Effect Workspace.Particle Effects.FireParticleSystem
//--------------------------------------------------------------//
technique FireParticleSystem
{
   pass ParticleSystem
   {
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = ONE;
      DESTBLEND = ONE;

      VertexShader = compile vs_1_1 FireParticleSystem_Vertex_Shader_main();
      PixelShader = compile ps_2_0 FireParticleSystem_Pixel_Shader_main();
   }

}

