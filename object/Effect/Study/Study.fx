/* ======================================================================== *
 * �Z�}���e�B�b�N�ϐ�
 * ======================================================================== */
float4x4 	WorldMatrix					: WORLD;               // ���[���h�ϊ��s��
float4x4 	WorldViewProjectionMatrix 	: WorldViewProjection;
float4x4	WorldViewTransposeMatrix 	: WorldViewTranspose;
float4x4	ViewTransposeMatrix 		: ViewTranspose;

float4x4	ViewProjectionMatrix		: ViewProjection;

//�J�������W
float3   	CameraPosition    			: POSITION  < string Object = "Camera"; >;

float		time_0_X 					: Time;

//�����̖{��
int CloneNum = 1;
//�ۑ��p�ϐ�
int index = 0;

/* ======================================================================== *
 * �ϐ�
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
 * �O���ݒ�ϐ�
 * ======================================================================== */
float4 ExtSetPosA[12];
float4 ExtSetPosB[12];

float Alpha 		= 1.0f;

//�w�i�摜�e�N�X�`��
texture BackTex <
    string ResourceName = "test_tex.png";
>;
//�w�i�摜�Q�Ɨp�T���v��
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
 * �\����
 * ======================================================================== */
struct VS_OUTPUT {
	float4 Pos			: POSITION;
	float3 Normal		: TEXCOORD0;
	float3 Eye			: TEXCOORD1;
	float2 ScreenTex	: TEXCOORD2;
};

/* ======================================================================== *
 * ���_�V�F�[�_
 * ======================================================================== */
VS_OUTPUT Hit_ParticleSystem_Vertex_Shader_main(
	float4 Pos		: POSITION, 		/* ���[�J�����W�̒��_	*/
	float3 Normal 	: NORMAL			/* �@��					*/
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
	
	// �E�[�̒��_���珇�Ԃ�...
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
		
	/* �Ȃ����Q�{���Ă�����Ƃ��܂�����... */
	set_localpos *= 2.0f;

	// �r���{�[�h�̂S��
	// �\���}�g���b�N�X�́A�������ď�Ɍ������x�N�g����^���܂�
	set_localpos += particleSize * (Pos.x * WorldViewTransposeMatrix[0] + Pos.y * WorldViewTransposeMatrix[1]);
	// And put the system into place
	set_localpos += particleSystemPosition;
	
	set_localpos *= scale;

	// World -> Local�Ŏ���
	float4x4 tmp = WorldViewProjectionMatrix;
	
	Out.Pos 	= mul(float4(set_localpos, 1), tmp);
	
	// �@�������[���h�s��ŉ�]
	Out.Normal	= mul( Normal, WorldMatrix);
	
	// �J�����Ƒ��Έʒu�v�Z
	Out.Eye		= CameraPosition - mul( Pos, WorldMatrix);
	
	// �X�N���[�����W���v�Z
	Out.ScreenTex.x = (Out.Pos.x / Out.Pos.w);
	Out.ScreenTex.y = -(Out.Pos.y / Out.Pos.w);

	Out.ScreenTex.x = 0.5 * (abs(Out.Pos.x) / Out.Pos.w);
	Out.ScreenTex.y = 1 * (abs(Out.Pos.y) / Out.Pos.w);

	
	return Out;
}

/* ======================================================================== *
 * �s�N�Z���V�F�[�_
 * ======================================================================== */
float4 Hit_ParticleSystem_Pixel_Shader_main(VS_OUTPUT IN) : COLOR
{
	return float4(IN.ScreenTex,1,1);
}

//--------------------------------------------------------------//
// �e�N�j�b�N
//--------------------------------------------------------------//
technique lineSystem
<string Script = 
					//�`��Ώۂ����C����ʂ�
        			"RenderColorTarget0=;"
				    "RenderDepthStencilTarget=;"
				    //�p�X�̑I��
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

