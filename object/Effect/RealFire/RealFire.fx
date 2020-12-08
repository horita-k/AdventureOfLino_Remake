//���A�����H���G�t�F�N�g
//������l�FLobelia
//�������FFireParticleSystemEx.fx

// �p�[�e�B�N�����̏���iX�t�@�C���ƘA�����Ă���̂ŁA�ύX�s�j
#define PARTICLE_MAX_COUNT  1000

// �p�[�e�B�N����
#define PARTICLE_COUNT 1000


// ���̕������Œ肷�邩�ۂ�(0 or 1)
#define FIX_FIRE_DIRECTION  0

// ���̕����@�iFIX_FIRE_DIRECTION�� 1 ���w�肵���ꍇ�̂ݗL���j
float3 fireDirection = float3( 0.0, 1.0, 0.0 );

// �ȉ��̂悤�Ɏw�肷��΁A�ʃI�u�W�F�N�g��Y�����ɂ���āA���̌����𐧌�ł���B
//float4x4 control_object : CONTROLOBJECT < string Name = "negi.x"; >;
//static float3 fireDirection  = control_object._21_22_23;

#if FIX_FIRE_DIRECTION
#define TEX_HEIGHT  PARTICLE_COUNT
#else
#define TEX_HEIGHT  (PARTICLE_COUNT*2)
#endif

float4x4 world_matrix : World;
float4x4 view_proj_matrix : ViewProjection;
float4x4 view_trans_matrix : ViewTranspose;
float4x4 inv_view_matrix : VIEWINVERSE;
float4x4 inv_world_matrix : WORLDINVERSE;
static float scaling = length(world_matrix[0]);

float time_0_X : Time;
float Alpha = 1.0f;

// �F���̊���
float ColorRate		= 0.50f;//1.0f;

//�������̊���
float FireRatio
<
   string UIName = "FireRatio";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 1.00;
> = float(0.25);
//> = float(0.45);

float particleSystemShape
<
   string UIName = "particleSystemShape";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 2.00;
> = float( 1.00 );
float particleSpread
<
   string UIName = "particleSpread";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 50.00;
> = float( 5 );
float particleSpeed
<
   string UIName = "particleSpeed";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 2.00;
> = float( 0.5 );
float particleSystemHeight
<
   string UIName = "particleSystemHeight";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 160.00;
> = float( 80 );
//> = float( 30 );
float particleSize
<
   string UIName = "particleSize";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 20.00;
> = float( 2 );
//��]���x
float RotateSpd
<
   string UIName = "RotateSpd";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = -20.00;
   float UIMax = 20.00;
> = float(1.0);
//�k�����x�i0�F�k�����Ȃ��j
float ScaleSpd
<
   string UIName = "ScaleSpd";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = -20.00;
   float UIMax = 20.00;
> = float(1);


// The model for the particle system consists of a hundred quads.
// These quads are simple (-1,-1) to (1,1) quads where each quad
// has a z ranging from 0 to 1. The z will be used to differenciate
// between different particles


texture ParticleBaseTex : RenderColorTarget
<
   int Width=1;
   int Height=TEX_HEIGHT;
   string Format="A32B32G32R32F";
>;
texture ParticleBaseTex2 : RenderColorTarget
<
   int Width=1;
   int Height=TEX_HEIGHT;
   string Format="A32B32G32R32F";
>;
texture DepthBuffer : RenderDepthStencilTarget <
   int Width=1;
   int Height=TEX_HEIGHT;
    string Format = "D24S8";
>;
sampler ParticleBase = sampler_state
{
   Texture = (ParticleBaseTex);
   ADDRESSU = CLAMP;
   ADDRESSV = CLAMP;
   MAGFILTER = NONE;
   MINFILTER = NONE;
   MIPFILTER = NONE;
};
sampler ParticleBase2 = sampler_state
{
   Texture = (ParticleBaseTex2);
   ADDRESSU = CLAMP;
   ADDRESSV = CLAMP;
   MAGFILTER = NONE;
   MINFILTER = NONE;
   MIPFILTER = NONE;
};
struct VS_OUTPUT {
   float4 Pos: POSITION;
   float2 texCoord: TEXCOORD0;
   float color: TEXCOORD1;
};

VS_OUTPUT FireParticleSystem_Vertex_Shader_main(float4 Pos: POSITION){
   VS_OUTPUT Out;

   int idx = round(Pos.z*PARTICLE_MAX_COUNT);
   Pos.z = float(idx)/PARTICLE_COUNT;
   
   // Loop particles
   float t = frac(Pos.z + particleSpeed * time_0_X);
   // Determine the shape of the system
   float s = pow(t, particleSystemShape);

   float3 pos;
   // Spread particles in a semi-random fashion
   pos.x = 0 * s * cos(62 * Pos.z);
   pos.z = 0 * s * sin(163 * Pos.z);
   // Particles goes up
   pos.y = particleSpread * t;
   
   float rnd = cos(62 * Pos.z)*2*3.1415;
   float radx = sin(163 * Pos.z)*2*3.1415;
   float rady = cos(256 * Pos.z)*2*3.1415;
   float radz = cos(510 * Pos.z)*2*3.1415;

   pos.x = (particleSpread * cos(rnd) * s);
   pos.y = particleSpread * t;
   pos.z = (particleSpread * sin(rnd) * s);
   
   //����t�]
   //pos.z = particleSpread * sin(rnd) - (particleSpread * sin(rnd) * s);
   
   float4x4 matRot;
   
   //X����]
   matRot[0] = float4(1,0,0,0); 
   matRot[1] = float4(0,cos(radx),sin(radx),0); 
   matRot[2] = float4(0,-sin(radx),cos(radx),0); 
   matRot[3] = float4(0,0,0,1); 
   
   pos = mul(pos,matRot);
   
   //Y����] 
   matRot[0] = float4(cos(rady),0,-sin(rady),0); 
   matRot[1] = float4(0,1,0,0); 
   matRot[2] = float4(sin(rady),0,cos(rady),0); 
   matRot[3] = float4(0,0,0,1); 
 
   pos = mul(pos,matRot);
 
   //Z����]
   matRot[0] = float4(cos(radz),sin(radz),0,0); 
   matRot[1] = float4(-sin(radz),cos(radz),0,0); 
   matRot[2] = float4(0,0,1,0); 
   matRot[3] = float4(0,0,0,1); 
   
   pos = mul(pos,matRot);

   pos.y += pow(t,2)*particleSystemHeight;

	#if FIX_FIRE_DIRECTION
	   float3 dirY = fireDirection;
	#else
	   float2 dir_tex_coord = float2( 0.5, float(idx)/TEX_HEIGHT+ 0.5 + 0.5/TEX_HEIGHT);
	   float3 dirY = tex2Dlod(ParticleBase2, float4(dir_tex_coord,0,1)).xyz;
	#endif
	
   dirY = normalize(dirY);
   float3 dirX = normalize( float3(dirY.y, -dirY.x, 0) );
   float3 dirZ = cross(dirX, dirY);
   float3x3 rotMat = { dirX, dirY, dirZ };
   pos = mul(pos, rotMat);
   

   
   particleSize = max(0,particleSize+ScaleSpd*t);
   pos += particleSize * (Pos.x * view_trans_matrix[0] + Pos.y * view_trans_matrix[1]);
   pos *= scaling / 10;
   
   float2 base_tex_coord = float2( 0.5, float(idx)/TEX_HEIGHT + 0.5/TEX_HEIGHT);
   float4 base_pos = tex2Dlod(ParticleBase2, float4(base_tex_coord,0,1));
   pos += base_pos.xyz;
   
     
   Out.Pos = mul(float4(pos, 1), view_proj_matrix);
   
   Out.texCoord = (Pos.xy*0.5+0.5);
   //�c�����
   if(t <= 0.01)
   {
   		Out.color = 0;
   }else{
	   Out.color = (1 - t)*(cos(time_0_X+t*11)*0.1+0.9);
   }
   
   if ( idx >= PARTICLE_COUNT ) Out.Pos.z=-2;
//	Out.color *= ColorRate * Alpha;
	Out.color = ColorRate * Alpha;

	//	Out.color *= 1.0f * Alpha;
	
   return Out;
}

float particleShape
<
   string UIName = "particleShape";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0.00;
   float UIMax = 1.00;
> = float( 0.37 );
texture Particle_Tex
<
   string ResourceName = "RealFire0.png";
>;
sampler Particle = sampler_state
{
   Texture = (Particle_Tex);
   ADDRESSU = CLAMP;
   ADDRESSV = CLAMP;
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
};
float4 FireParticleSystem_Pixel_Shader_main(float2 texCoord: TEXCOORD0, float color: TEXCOORD1) : COLOR {
   float4 col = tex2D(Particle,texCoord);
   col.a *= pow(color,1/FireRatio);
   return col;
}
float4 FireParticleSystem_Pixel_Shader_Alpha(float2 texCoord: TEXCOORD0, float color: TEXCOORD1) : COLOR {
   float4 col = tex2D(Particle,texCoord);
   col.rgb = (col.r+col.g+col.b)/3;
   col.rgb *= color;
   col.a *= color;
	
   return col;
}

struct VS_OUTPUT2 {
   float4 Pos: POSITION;
   float2 texCoord: TEXCOORD0;
};

VS_OUTPUT2 ParticleBase_Vertex_Shader_main(float4 Pos: POSITION, float2 Tex: TEXCOORD) {
   VS_OUTPUT2 Out;
  
   Out.Pos = Pos;
   Out.texCoord = Tex ;
   return Out;
}

float4 ParticleBase_Pixel_Shader_main(float2 texCoord: TEXCOORD0) : COLOR {
   int idx = round(texCoord.y*TEX_HEIGHT);
   if ( idx >= PARTICLE_COUNT ) idx -= PARTICLE_COUNT;
   
   float t = frac(float(idx)/PARTICLE_COUNT + particleSpeed * time_0_X);
   texCoord += float2(0.5, 0.5/TEX_HEIGHT);
   
   float4 old_color = tex2D(ParticleBase2, texCoord);
   if ( old_color.a <= t ) {
      old_color.a = t;
      return old_color;
   } else {
		#if !FIX_FIRE_DIRECTION
		      if ( texCoord.y < 0.5 ) {
		         return float4(world_matrix._41_42_43, t);
		      } else {
		         return float4(world_matrix._21_22_23, t);
		      }
		#else
		      return float4(world_matrix._41_42_43, t);
		#endif
   }
}

VS_OUTPUT2 ParticleBase2_Vertex_Shader_main(float4 Pos: POSITION, float2 Tex: TEXCOORD) {
   VS_OUTPUT2 Out;
  
   Out.Pos = Pos;
   Out.texCoord = Tex + float2(0.5, 0.5/TEX_HEIGHT);
	
   return Out;
}

float4 ParticleBase2_Pixel_Shader_main(float2 texCoord: TEXCOORD0) : COLOR {
   return tex2D(ParticleBase, texCoord);
}

float4 ClearColor = {0,0,0,1};
float ClearDepth  = 1.0;

//--------------------------------------------------------------//
// Technique Section for Effect Workspace.Particle Effects.FireParticleSystem
//--------------------------------------------------------------//
technique FireParticleSystem <
    string Script = 
        "RenderColorTarget0=ParticleBaseTex;"
	    "RenderDepthStencilTarget=DepthBuffer;"
		"ClearSetColor=ClearColor;"
		"ClearSetDepth=ClearDepth;"
	    "Pass=ParticleBase;"
	    
        "RenderColorTarget0=ParticleBaseTex2;"
		"ClearSetColor=ClearColor;"
		"ClearSetDepth=ClearDepth;"
		"Clear=Color;"
		"Clear=Depth;"
	    "Pass=ParticleBase2;"
	    
	    
        "RenderColorTarget0=;"
	    "RenderDepthStencilTarget=;"

		//"LoopByCount=count;"
        //"LoopGetIndex=index;"
		    "Pass=ParticleSystem_Smoke;"
        //"LoopEnd=;"

		//"LoopByCount=count;"
        //"LoopGetIndex=index;"
		    "Pass=ParticleSystem;"
        //"LoopEnd=;"
    ;
> {
  pass ParticleBase < string Script = "Draw=Buffer;";>
  {
      ALPHABLENDENABLE = FALSE;
      ALPHATESTENABLE=FALSE;
      VertexShader = compile vs_1_1 ParticleBase_Vertex_Shader_main();
      PixelShader = compile ps_2_0 ParticleBase_Pixel_Shader_main();
   }
  pass ParticleBase2 < string Script = "Draw=Buffer;";>
  {
      ALPHABLENDENABLE = FALSE;
      ALPHATESTENABLE=FALSE;
      VertexShader = compile vs_1_1 ParticleBase2_Vertex_Shader_main();
      PixelShader = compile ps_2_0 ParticleBase2_Pixel_Shader_main();
   }
   pass ParticleSystem
   {
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = ONE;
      VertexShader = compile vs_3_0 FireParticleSystem_Vertex_Shader_main();
      PixelShader = compile ps_3_0 FireParticleSystem_Pixel_Shader_main();
   }
   
   pass ParticleSystem_Smoke
   {
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = INVSRCALPHA;
      VertexShader = compile vs_3_0 FireParticleSystem_Vertex_Shader_main();
      PixelShader = compile ps_3_0 FireParticleSystem_Pixel_Shader_Alpha();
   }
}

