//**************************************************************//
//  Effect File exported by RenderMonkey 1.6
//
//  - Although many improvements were made to RenderMonkey FX  
//    file export, there are still situations that may cause   
//    compilation problems once the file is exported, such as  
//    occasional naming conflicts for methods, since FX format 
//    does not support any notions of name spaces. You need to 
//    try to create workspaces in such a way as to minimize    
//    potential naming conflicts on export.                    
//    
//  - Note that to minimize resulting name collisions in the FX 
//    file, RenderMonkey will mangle names for passes, shaders  
//    and function names as necessary to reduce name conflicts. 
//**************************************************************//

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
> = float( 2.0 );
float particleSpread
<
   string UIName = "particleSpread";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 50.00;
//> = float( 1.00 );
> = float( 1.20 );
float particleSpeed
<
   string UIName = "particleSpeed";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 2.00;
> = float( 0.8 );
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
> = float( 1.00 );
// The model for the particle system consists of a hundred quads.
// These quads are simple (-1,-1) to (1,1) quads where each quad
// has a z ranging from 0 to 1. The z will be used to differenciate
// between different particles

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
   // Spread particles in a semi-random fashion
   pos.x = particleSpread * s * sin(62 * (Pos.z * 4.0f));
   pos.z = particleSpread * s * cos(163 * (Pos.z * 4.0f));
   // Particles goes up
//   pos.y = particleSystemHeight * t;

	/*
	pos.y = sin(particleSpread * s * tan(163 * (Pos.z * 4.0f))) * 0.8f;
	pos.y -= (s * s * 0.0002f);
	*/

	float v0 = 0.01f;
	float g = -0.4f;
	pos.y = (v0 * s) + (0.5f * g * (s * s)) * (Pos.z * 4.0f);
	pos.y = 0.8f * particleSpread * s * tan(163 * (Pos.z * 4.0f));

	/*
	float max_length = 1.5f;
	if (abs(pos.x) > max_length) {
		pos.x = 0;	
	}
	if (abs(pos.y) > max_length) {
		pos.y = 0;	
	}
	if (abs(pos.z) > max_length) {
		pos.z = 0;	
	}
	*/

	float max_length = 1.3f;
	float len = length(pos);
	if (len > max_length) {
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
	}
	
   // Billboard the quads.
   // The view matrix gives us our right and up vectors.
   //pos += particleSize * (Pos.x * WorldViewTransposeMatrix[0] + Pos.y * WorldViewTransposeMatrix[1]);
   pos += particleSize * (Pos.x * billboard_vec_x + Pos.y * billboard_vec_y);
//   pos += 1.2f;
	
   Out.Pos = mul(float4(pos, 1), WorldViewProjectionMatrix);
   Out.texCoord = Pos.xy;
	Out.color = 1 - t;
//	Out.color = ((frac(1 - s) * 0.2f) + 0.4f) * Alpha;
	Out.color = ((frac(1 - s) * 0.2f) + 0.4f);
	
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
   // Fade the particle to a circular shape
   float fade = pow(dot(texCoord, texCoord), particleShape);
   return (1 - fade) * tex2D(Navi, float2(color,0.5f)) * Alpha;
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

