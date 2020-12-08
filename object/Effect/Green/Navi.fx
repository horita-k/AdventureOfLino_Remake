// FireParticleSystem
// ParticleSystem
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
> = float( 0.5 );
float particleSpread
<
   string UIName = "particleSpread";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 50.00;
//> = float( 1.00 );
> = float( 0.80 );
float particleSpeed
<
   string UIName = "particleSpeed";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 2.00;
> = float( 1.0 );
float particleSystemHeight
<
   string UIName = "particleSystemHeight";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 160.00;
> = float( 1.00 );
float particleSize
<
   string UIName = "particleSize";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 20.00;
> = float( 0.40 );

float Alpha
<
   string UIName = "Alpha";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = 0.00;
   float UIMax = 1.00;
> = float( 1.0 );

struct VS_OUTPUT {
   float4 Pos: POSITION;
   float2 texCoord: TEXCOORD0;
   float color: TEXCOORD1;
};

VS_OUTPUT FireParticleSystem_Vertex_Shader_main(float4 Pos: POSITION){
   VS_OUTPUT Out;

   // Loop particles
   float t = frac((Pos.z * 4.0f) + particleSpeed * time_0_X);
   // Determine the shape of the system
   float s = pow(t, particleSystemShape);

   float3 pos;
   pos.x = particleSpread * s * sin(62 * (Pos.z * 4.0f));
   pos.z = particleSpread * s * cos(163 * (Pos.z * 4.0f));
   // Particles goes up
	pos.x = particleSpread * s * cos(62 * Pos.z);
   	pos.z = particleSpread * s * sin(163 * Pos.z);

	pos.x = particleSpread * s * cos(162 * Pos.z);
   	pos.z = particleSpread * s * sin(163 * Pos.z);

	float v0 = 0.01f;
	float g = -0.4f;
	pos.y = (v0 * s) + (0.5f * g * (s * s)) * (Pos.z * 4.0f);
	pos.y = 0.8f * particleSpread * s * tan(163 * (Pos.z * 4.0f));
	
	float max_length = 1.0f;
	float len = length(pos);
	if (len > max_length) {
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
	}
	
   // Billboard the quads.
   pos += particleSize * (Pos.x * billboard_vec_x + Pos.y * billboard_vec_y);
	
   Out.Pos = mul(float4(pos, 1), WorldViewProjectionMatrix);
   Out.texCoord = Pos.xy;
	Out.color = 1 - t;
	Out.color = ((frac(1 - s) * 0.2f) + 0.4f) * sqrt(Alpha);

   return Out;
}
float particleShape
<
   string UIName = "particleShape";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 1.00;
> = float( 0.05 );
texture Navi_Tex0
<
   string ResourceName = "Navi0.tga";
>;
sampler Navi = sampler_state
{
   Texture = (Navi_Tex0);
   ADDRESSU = CLAMP;
   ADDRESSV = CLAMP;
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
};
float4 FireParticleSystem_Pixel_Shader_main(float2 texCoord: TEXCOORD0, float color: TEXCOORD1) : COLOR {
   float fade = pow(dot(texCoord, texCoord), particleShape);
   return (1 - fade) * tex2D(Navi, float2(color,0.5f)) * sqrt(Alpha);
}

// Technique Section for Effect Workspace.Particle Effects.FireParticleSystem
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
