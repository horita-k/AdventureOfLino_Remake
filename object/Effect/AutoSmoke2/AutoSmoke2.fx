float4 SmokeColor = {10.0, 10.0, 10.0, 0.4};

//���[�t�擾
//float morph_Anm 		= 0.0030; // : CONTROLOBJECT < string name = "(self)"; string item = "�Đ����x"; >;
//float morph_Anm 		= 0.0020; // : CONTROLOBJECT < string name = "(self)"; string item = "�Đ����x"; >;
float morph_Anm 		= 0; // : CONTROLOBJECT < string name = "(self)"; string item = "�Đ����x"; >;
//float morph_Num 		= 0.1000; // : CONTROLOBJECT < string name = "(self)"; string item = "���q��"; >;
//float morph_Num 		= 0.0550; // : CONTROLOBJECT < string name = "(self)"; string item = "���q��"; >;
float morph_Num 		= 0.0250; // : CONTROLOBJECT < string name = "(self)"; string item = "���q��"; >;
float morph_MinWidth	= 0.0500; // : CONTROLOBJECT < string name = "(self)"; string item = "�ŏ���"; >;
float morph_MaxWidth 	= 0.0900; // : CONTROLOBJECT < string name = "(self)"; string item = "�ő啝"; >;
//float morph_Height 		= 0.0200; // : CONTROLOBJECT < string name = "(self)"; string item = "����"; >;
float morph_Height 		= 0.2000; // : CONTROLOBJECT < string name = "(self)"; string item = "����"; >;
float morph_H 			= 0.0000; // : CONTROLOBJECT < string name = "(self)"; string item = "�F��"; >;
float morph_S 			= 0.1000; // : CONTROLOBJECT < string name = "(self)"; string item = "�ʓx"; >;
float morph_B 			= 0.0000; // : CONTROLOBJECT < string name = "(self)"; string item = "���x"; >;
float morph_A 			= 0.5000; // : CONTROLOBJECT < string name = "(self)"; string item = "�����x"; >;
float morph_g_Sip 		= 0.0000; // : CONTROLOBJECT < string name = "(self)"; string item = "�X�P�[��+"; >;
float morph_g_Sim 		= 0.0000; // : CONTROLOBJECT < string name = "(self)"; string item = "�X�P�[��-"; >;
float morph_Si 			= 1.0000; // : CONTROLOBJECT < string name = "(self)"; string item = "���qSi"; >;
float morph_Si_p 		= 1.0000; // : CONTROLOBJECT < string name = "(self)"; string item = "���q�g��"; >;
//float morph_Rot_p 		= 0.0000; // : CONTROLOBJECT < string name = "(self)"; string item = "��]+"; >;
float morph_Rot_p 		= 0; // : CONTROLOBJECT < string name = "(self)"; string item = "��]+"; >;
//float morph_Rot_m 		= 0.0280; // : CONTROLOBJECT < string name = "(self)"; string item = "��]-"; >;
//float morph_Rot_m 		= 0.0050; // : CONTROLOBJECT < string name = "(self)"; string item = "��]-"; >;
float morph_Rot_m 		= 0; // : CONTROLOBJECT < string name = "(self)"; string item = "��]-"; >;
float morph_Daen 		= 0.0000; // : CONTROLOBJECT < string name = "(self)"; string item = "�`��"; >;

//#define _PLAY_MMD				// MMD�ł̎g�p

#ifdef _PLAY_MMD
float4 mtx0 = {20, 0, 0, 0};
float4 mtx1 = {0, 20, 0, 0};
float4 mtx2 = {0, 0, 20, 0};
float4 mtx3 = {0, 0, 0, 0};
#else 
float4 mtx0 = {2000, 0, 0, 0};
float4 mtx1 = {0, 2000, 0, 0};
float4 mtx2 = {0, 0, 2000, 0};
float4 mtx3 = {0, 0, 0, 0};
#endif // _PLAY_MMD


//�[�x�}�b�v�ۑ��e�N�X�`��
shared texture2D SPE_DepthTex : RENDERCOLORTARGET;
sampler2D SPE_DepthSamp = sampler_state {
    texture = <SPE_DepthTex>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;
    AddressU  = CLAMP;
    AddressV = CLAMP;
};
//�\�t�g�p�[�e�B�N���G���W���g�p�t���O
bool use_spe = false;


//������
float SmokeHeight = 10.0;

//�����i�ŏ��j
float SmokeWidth_Min = 10.0;

//�����i�ő�j
float SmokeWidth_Max = 10.0;

//�p�[�e�B�N���傫��
float ParticleSize = 0.1;

//�p�[�e�B�N���\�����i�ő�15000)
#define PARTICLE_MAX 10000

//�A�j���X�s�[�h
float AnmSpd = 5;
float Alpha = 1.0;


float3   LightDirection    : DIRECTION < string Object = "Light"; >;
float3   LightColor      : SPECULAR   < string Object = "Light"; >;
float3   CameraPosition    : POSITION  < string Object = "Camera"; >;
float4x4 world_view_proj_matrix : WorldViewProjection;
float4x4 world_view_trans_matrix : WorldViewTranspose;
float4x4 inv_view_matrix : WORLDVIEWINVERSE;
//float4x4 world_matrix : CONTROLOBJECT < string name = "(self)";string item = "�Z���^�[";>;
//float4x4 world_matrix = float4{ {0,0,0,0}, {1,1,1,1}, {2,2,2,2}, {3,3,3,3} };
//float4x4 tmp : CONTROLOBJECT < string name = "(self)";string item = "�Z���^�[";>;
//float4x4 aworldMatrix = float4x4( mtx0, mtx0, mtx0, mtx0  );
//float4x4 aworldMatrix = ( {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}  );

static float3 billboard_vec_x = normalize(world_view_trans_matrix[0].xyz);
static float3 billboard_vec_y = normalize(world_view_trans_matrix[1].xyz);

float time_0_X : Time;
float particleSystemShape = float( 1.00 );
float particleSpread = float( 20.00 );
float particleSpeed = float( 0.48 );
float particleSystemHeight = float( 80.00 );
float particleSize = float( 5 );
//��]���x
float RotateSpd = float(0.15);
//�k�����x�i0�F�k�����Ȃ��j
float ScaleSpd = float(0);


// The model for the particle system consists of a hundred quads.
// These quads are simple (-1,-1) to (1,1) quads where each quad
// has a z ranging from 0 to 1. The z will be used to differenciate
// between different particles

//���Ԃ�
int ParticleNum = 15000;

//HSB�ϊ��p�F�e�N�X�`��
texture2D ColorPallet <
    string ResourceName = "ColorPallet.png";
>;
sampler PalletSamp = sampler_state {
    texture = <ColorPallet>;
	ADDRESSU = CLAMP;
	ADDRESSV = CLAMP;
};

texture2D rndtex <
    string ResourceName = "random256x256.bmp";
>;
sampler rnd = sampler_state {
    texture = <rndtex>;
};

texture NormalBase_Tex
<
   string ResourceName = "NormalBase.png";
>;
sampler NormalBase = sampler_state
{
   Texture = (NormalBase_Tex);
   ADDRESSU = CLAMP;
   ADDRESSV = CLAMP;
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = NONE;
};

//�����e�N�X�`���T�C�Y
#define RNDTEX_WIDTH  256
#define RNDTEX_HEIGHT 256

//�����擾
float4 getRandom(float rindex)
{
    float2 tpos = float2(rindex % RNDTEX_WIDTH, trunc(rindex / RNDTEX_WIDTH));
    tpos += float2(0.5,0.5);
    tpos /= float2(RNDTEX_WIDTH, RNDTEX_HEIGHT);
    return tex2Dlod(rnd, float4(tpos,0,1));
}

float4   MaterialDiffuse   : DIFFUSE  < string Object = "Geometry"; >;

struct VS_OUTPUT {
   float4 Pos: POSITION;
   float2 Tex: TEXCOORD0;
   float color: TEXCOORD1;
   float t:TEXCOORD2;
   float2 NormalTex  : TEXCOORD3;
   float3 Eye		  : TEXCOORD4;
   float3 WPos: TEXCOORD5;
   float4 LastPos: TEXCOORD6;
};
////////////////////////////////////////////////////////////////////////////////////////////////
// ���W��2D��]
float2 Rotation2D(float2 pos, float rot)
{
    float x = pos.x * cos(rot) - pos.y * sin(rot);
    float y = pos.x * sin(rot) + pos.y * cos(rot);

    return float2(x,y);
}

VS_OUTPUT ParticleFunc(int num,float4 Pos,float deflen,float maxlen,float maxheight,float spd,float2 texCoord)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	float4x4 world_matrix = float4x4( mtx0, mtx1, mtx2, mtx3 );

	Pos.xy *= 10.0;
	Out.Tex = texCoord;

	float fi = Pos.z;
	fi = fi/(PARTICLE_MAX*morph_Num);
	float r = getRandom(Pos.z);
	float t = 1 - frac(fi * particleSpeed * 123);
	float s = pow(t, particleSystemShape); 

	AnmSpd *= morph_Anm;
	float local_t = frac(time_0_X * AnmSpd + fi);
	Out.t = t;
	float3 pos;
	float len = 1 * ((1-(local_t)));
	len = 1-len * len;
	len *= t*maxlen;

	SmokeWidth_Min *= morph_MinWidth;
	SmokeWidth_Max *= morph_MaxWidth;

	r += time_0_X*(morph_Rot_p-morph_Rot_m)*t;

	float SmokeWidth = max(0,SmokeWidth_Max - SmokeWidth_Min);
	pos.x = particleSpread * cos(r*8) * (len * SmokeWidth + SmokeWidth_Min)*(1-morph_Daen);
	pos.z = particleSpread * sin(r*8) * (len * SmokeWidth + SmokeWidth_Min);

	float m = max(0,(1-local_t) - spd);

	float4x4 rot = world_matrix;
	rot[3].xyz = 0;
	float3 DownVec = normalize(mul(float3(0,1,0),rot));

	pos.y = 1+abs(1-(m*m*m)) * 10*fi*maxheight;
	pos.y += pow(len,2)*SmokeHeight*64*morph_Height;
	//float work = 1 * (((local_t)));
	//pos -= DownVec * (work*work) * 20;
	pos = mul(pos,world_matrix);

	float3 w = (particleSize * float3(Pos.xy - float2(0,0),0)) * max(0,(1-t * ScaleSpd));
	//�ʏ��]
	//��]�s��̍쐬
	float rad = t * RotateSpd + cos(62 * fi);
	float4x4 matRot;
	matRot[0] = float4(cos(rad),sin(rad),0,0); 
	matRot[1] = float4(-sin(rad),cos(rad),0,0); 
	matRot[2] = float4(0,0,1,0); 
	matRot[3] = float4(0,0,0,1); 
	w = mul(w,matRot);

	Out.NormalTex =  Rotation2D(texCoord*2-1,-rad);
	Out.NormalTex = Out.NormalTex*0.5+0.5;


	//�r���{�[�h��]
	w = mul(w,inv_view_matrix);
	//�g��
	w = mul(w,length(world_matrix[0])*1)*ParticleSize*morph_Si*(1+pow(t,3)*morph_Si_p*16);


	// Billboard the quads.
	// The view matrix gives us our right and up vectors.
	//pos += (Pos.x * view_trans_matrix[0] + Pos.y * view_trans_matrix[1]);
	pos += w;
	pos.y -= 1.0;

	pos.xyz *= max(0,1+(morph_g_Sip-morph_g_Sim)*64);

	pos.xyz += world_matrix[3].xyz;

	Out.Eye =  pos.xyz - CameraPosition;
	Out.Pos = mul(float4(pos, 1), world_view_proj_matrix);
	Out.LastPos = Out.Pos;
	Out.WPos = pos.xyz;
	Out.color = (1-(local_t)) - t*0.25;
	Out.color = lerp(0,Out.color,min(1,(1-(1-(local_t)))*4));

	if(Pos.z > PARTICLE_MAX*morph_Num)
	{
		Out.Pos.z = -2;
		return Out;
	}
	return Out;
}

VS_OUTPUT ParticleVS(float4 Pos: POSITION,float2 Tex : TEXCOORD0){
	VS_OUTPUT Out = (VS_OUTPUT)0;

	if((1-((time_0_X * AnmSpd) % 1.0)) != 0 || (1-((time_0_X * AnmSpd) % 1.0)) != 1)
	{
		Out = ParticleFunc(ParticleNum,Pos,0.5,0.75,0,0.2,Tex);
	}
	Out.Tex *= 0.25;
	
	int Index0 = Pos.z*0.1;
	
	Index0 %= 16;
	int tw = Index0%4;
	int th = Index0/4;

	Out.Tex.x += tw*0.25;
	Out.Tex.y += th*0.25;
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
   string ResourceName = "particle.png";
>;
sampler Particle = sampler_state
{
   Texture = (Particle_Tex);
   ADDRESSU = CLAMP;
   ADDRESSV = CLAMP;
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = NONE;
};
float3x3 compute_tangent_frame(float3 Normal, float3 View, float2 UV)
{
  float3 dp1 = ddx(View); 
  float3 dp2 = ddy(View);
  float2 duv1 = ddx(UV);
  float2 duv2 = ddy(UV);

  float3x3 M = float3x3(dp1, dp2, cross(dp1, dp2));
  float2x3 inverseM = float2x3(cross(M[1], M[2]), cross(M[2], M[0]));
  float3 Tangent = mul(float2(duv1.x, duv2.x), inverseM);
  float3 Binormal = mul(float2(duv1.y, duv2.y), inverseM);

  return float3x3(normalize(Tangent), normalize(Binormal), Normal);
}

float4 AllPS(VS_OUTPUT IN) : COLOR {
	float4 col = tex2D(Particle,IN.Tex);

	col.rgb = col.rgb * 2.0 - 1.0;
	col.b = 0;
	float4 normal = tex2D(NormalBase,IN.NormalTex);
	//return normal;
	normal.rgb  = normal.rgb * 2 - 1;
	normal.rgb += col.rgb*0.5;
	normal.a *= col.a;
	//Eye.y = -IN.Eye.y;
	float3x3 tangentFrame = compute_tangent_frame(normalize(IN.Eye), normalize(IN.Eye), IN.NormalTex);
	normal.xyz = normalize(mul(normal.xyz, tangentFrame));
	float d = pow(saturate(dot(-LightDirection,-normal.xyz)*0.5+0.5),1);

	//return float4(d,0,0,normal.a);

	SmokeColor.w *= Alpha;
	
	//return normal;
	col = float4(d,d,d,normal.a);
	col *= SmokeColor;

	col.a *= IN.color * (1-morph_A);

	float r = morph_B*64;

	float3 AddColor = tex2D(PalletSamp,float2(morph_H,morph_S)).rgb*r;

	col.rgb *= LightColor+0.3;
	col.rgb += AddColor * pow(1-IN.t,4);
	
	if(use_spe)
	{
		float2 ScTex = IN.LastPos.xyz/IN.LastPos.w;
		ScTex.y *= -1;
		ScTex.xy += 1;
		ScTex.xy *= 0.5;
		
	    // �[�x
	    float dep = length(CameraPosition - IN.WPos);
	    float scrdep = tex2D(SPE_DepthSamp,ScTex).r;
	    
	    float adddep = 1-saturate(length(abs(frac(IN.Tex*4)-0.5)));
	    dep = length(dep-scrdep);
	    dep = smoothstep(0,10,dep);
	    //return float4(dep,0,0,1);
	    col.a *= dep;
    }
	
	
	
	return col;
}

//�������@�̐ݒ�
//
//�����������F
//BLENDMODE_SRC SRCALPHA
//BLENDMODE_DEST INVSRCALPHA
//
//���Z�����F
//
//BLENDMODE_SRC SRCALPHA
//BLENDMODE_DEST ONE

#define BLENDMODE_SRC SRCALPHA
#define BLENDMODE_DEST INVSRCALPHA
//--------------------------------------------------------------//
// Technique Section for Effect Workspace.Particle Effects.FireParticleSystem
//--------------------------------------------------------------//
technique FireParticleSystem
{
   pass ParticlePass
   {
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = INVSRCALPHA;

      VertexShader = compile vs_3_0 ParticleVS();
      PixelShader = compile ps_3_0 AllPS();
   }
}
/*
technique FireParticleSystem_SS < string MMDPass = "object_ss"; > {
   pass ParticlePass
   {
      ZENABLE = TRUE;
      ZWRITEENABLE = FALSE;
      CULLMODE = NONE;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = INVSRCALPHA;

      VertexShader = compile vs_3_0 ParticleVS();
      PixelShader = compile ps_3_0 AllPS();
   }
}
*/
