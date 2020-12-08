//���G�t�F�N�g
//�������ЂƁF���x���A�i�r�[���}��P�j


//�����̖{��
int CloneNum = 128;

//�e�N�X�`����
texture WindFire_Tex0
<
   string ResourceName = "WindFire0.png";
>;

//�S�̂̍Đ����x
float AnmSpd = 1;

//�S�̂̍���
float Height = 35;

//�z�u���̒�������̂���̗�����
float SetPosRand = 0.1;

//���ЂƂЂƂ̍����ő�l
//float LocalHeight = 0.5;
float LocalHeight = 2.5;

//���̍L���苭��
float WindSizeSpd = 1;

//�L����̗�����
float WindSizeRnd = 1;

//�e�N�X�`���J��Ԃ���
float ScrollNum = 1;

//�F�ݒ�
float3 Color = float3( 0.5, 0.5, 0.5 );

//���邳
float Brightness = 4;

//�c�ݗ�
float DifPow = 10.0;

//�S�̂̉�]���x
float RotateSpd = 0.5;

//�ʉ�]�W���i�X���̂΂���j
float RotateRatio = 0.5;

//�ŏ����a
float MinSize = 3;

//���ˎ��ԃI�t�Z�b�g�i�����_���l�j
float ShotRandOffset = 0;

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


//--�悭�킩��Ȃ��l�͂������牺�͂�������Ⴞ��--//


//�v�Z�p�e�N�X�`���T�C�Y
#define TEX_SIZE 256

#define TEX_WIDTH TEX_SIZE
#define TEX_HEIGHT TEX_SIZE

texture DistortionRT: OFFSCREENRENDERTARGET <
    string Description = "OffScreen RenderTarget for DistortionField.fx";
    int Width = TEX_SIZE;
    int Height = TEX_SIZE;
    float4 ClearColor = { 0, 0, 0, 1 };
    float ClearDepth = 1.0;
    bool AntiAlias = false;
    string DefaultEffect = 
        "self = hide;";
>;

sampler DistortionView = sampler_state {
    texture = <DistortionRT>;
    AddressU  = CLAMP;
    AddressV = CLAMP;
};



texture2D WindFire_Tex1 <
    string ResourceName = "WindFire1.png";
>;
sampler rnd = sampler_state {
    texture = <WindFire_Tex1>;
};


float time_0_X : Time;
//�΂̒l
#define PI 3.1415
//�p�x�����W�A���l�ɕϊ�
#define RAD(x) ((x * PI) / 180.0)

float4x4 WorldViewProjectionMatrix : WORLDVIEWPROJECTION;
float Alpha
<
   string UIName = "Alpha";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = 0.00;
   float UIMax = 1.00;
> = float( 1.00 );

float4   MaterialDiffuse   : DIFFUSE  < string Object = "Geometry"; >;
float3   LightDiffuse      : DIFFUSE   < string Object = "Light"; >;
static float4 DiffuseColor  = MaterialDiffuse  * float4(LightDiffuse, 1.0f);

static float3	sGetRand;

struct VS_OUTPUT {
   float4 Pos: POSITION;
   float2 texCoord: TEXCOORD0;
   float color: TEXCOORD1;
   float alpha: TEXCOORD2;
   float2 CenterTex: TEXCOORD3;
   float2 DistTex: TEXCOORD4;
};

//�ۑ��p�ϐ�
int index = 0;
//���ˎ���
float ThunderWaitTime = 60;

VS_OUTPUT lineSystem_Vertex_Shader_main(float4 Pos: POSITION){
   VS_OUTPUT Out;
    float findex = index;
	float offset = findex*(ThunderWaitTime / (CloneNum*ThunderWaitTime)) + sin(findex)*ShotRandOffset;
	float time_buf = time_0_X*AnmSpd + offset;
    float t = time_buf*60;
 
 
 //�����_���l
  float3 rand = tex2Dlod(rnd, float4(findex/(float)CloneNum,0,0,1));
  /*
  float3 rand ;
  static int sTmp = 0;
  sTmp = sTmp + 1;
	rand.x = sTmp;
	rand.y = 1;
	rand.z = sTmp;
	*/
	
	sGetRand = rand;

   t %= ThunderWaitTime;

 
   Out.texCoord.y = (Pos.x + 1)/2 - 0.001;
   Out.texCoord.x = Pos.z * ScrollNum + rand.x;
   if(t > ThunderWaitTime/2 * (60/ThunderWaitTime))
   {
   		t = 0;
   }
   Out.alpha = t / (ThunderWaitTime/2 * (60/ThunderWaitTime));
   
   MinSize += t*0.05*WindSizeSpd*WindSizeRnd*rand.z;
   float h = saturate(t*0.05)*LocalHeight;
   //Z�l�i0�`�P�j����p�x���v�Z���A���W�A���l�ɕϊ�����
   float rad = RAD(Pos.z * 360);
   
   
   //--xz���W��ɔz�u����
   
   //x���}�C�i�X=�O��
   if(Pos.x < 0)
   {
   		Out.Pos.x = cos(rad) * MinSize;	
   		Out.Pos.z = sin(rad) * MinSize;
   		//y�l�͍����p�����[�^���̂܂�
   		Out.Pos.y = h/2;
   }else{
	   //����
   		Out.Pos.x = cos(rad) * MinSize;		   
   		Out.Pos.z = sin(rad) * MinSize;
   		Out.Pos.y = -h/2;
   } 
   float4 Center = Out.Pos;
   Center.y = 0;
   Center.w = 1;
   Out.Pos.w = 1;
   
	float radx = (-0.5 + rand.x)*2*2*3.1415;
	float radz = (-0.5 + rand.z)*2*2*3.1415;
	float rady = time_0_X*AnmSpd * RotateSpd;
	
	radx *= RotateRatio;
	radz *= RotateRatio;

  float4x4 matRot;
   
   //Y����] 
   matRot[0] = float4(cos(rady),0,-sin(rady),0); 
   matRot[1] = float4(0,1,0,0); 
   matRot[2] = float4(sin(rady),0,cos(rady),0); 
   matRot[3] = float4(0,0,0,1); 
 
   Out.Pos = mul(Out.Pos,matRot);
   Center = mul(Center,matRot);
   
   
   //X����]
   matRot[0] = float4(1,0,0,0); 
   matRot[1] = float4(0,cos(radx),sin(radx),0); 
   matRot[2] = float4(0,-sin(radx),cos(radx),0); 
   matRot[3] = float4(0,0,0,1); 
   
   Out.Pos = mul(Out.Pos,matRot);
   Center = mul(Center,matRot);
 
   //Z����]
   matRot[0] = float4(cos(radz),sin(radz),0,0); 
   matRot[1] = float4(-sin(radz),cos(radz),0,0); 
   matRot[2] = float4(0,0,1,0); 
   matRot[3] = float4(0,0,0,1); 
   
   Out.Pos = mul(Out.Pos,matRot);
   Center = mul(Center,matRot);
   
   Out.Pos.x += rand.x*SetPosRand-SetPosRand/2;
   Out.Pos.z += rand.z*SetPosRand-SetPosRand/2;
   
   Out.Pos.y += rand.y*Height;
   Center.y  += rand.y*Height;
   Out.Pos = mul(Out.Pos, WorldViewProjectionMatrix);
   Center = mul(Center, WorldViewProjectionMatrix);
   Out.color = t * Alpha;


	//WVP�ϊ��ςݍ��W����X�N���[�����W�ɕϊ�
	float3 TgtPos = Out.Pos.xyz/Out.Pos.w;
	TgtPos.y *= -1;
	TgtPos.xy += 1;
	TgtPos.xy *= 0.5;
	
	Out.DistTex = TgtPos.xy;
	
	float4 wc = mul( TgtPos, WorldViewProjectionMatrix );

	TgtPos = Center.xyz/Center.w;
	TgtPos.y *= -1;
	TgtPos.xy += 1;
	TgtPos.xy *= 0.5;
	
	Out.CenterTex = TgtPos.xy;   
   return Out;
}

//�e�N�X�`���̐ݒ�
sampler AuraTex1Sampler = sampler_state
{
   //�g�p����e�N�X�`��
   Texture = (WindFire_Tex0);
   //�e�N�X�`���͈�0.0�`1.0���I�[�o�[�����ۂ̏���
   //WRAP:���[�v
   ADDRESSU = WRAP;
   ADDRESSV = CLAMP;
   //�e�N�X�`���t�B���^�[
   //LINEAR:���`�t�B���^
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
   MIPFILTER = LINEAR;
};

//�s�N�Z���V�F�[�_

//�ʓx�v�Z�p
const float4 calcY = float4( 0.2989f, 0.5866f, 0.1145f, 0.00f );

float4 lineSystem_Pixel_Shader_main(float2 texCoord: TEXCOORD0,float color: TEXCOORD1,float alpha: TEXCOORD2,float2 CenterTex: TEXCOORD3,float2 DistTex: TEXCOORD4) : COLOR {
   //���͂��ꂽ�e�N�X�`�����W�ɏ]���ĐF��I������
   
   float4 col = float4(tex2D(AuraTex1Sampler,texCoord+float2(color*0.01,0)));   

   float dif = col.r*DifPow;  
   if(alpha > 0.5) alpha = 1-alpha;

   color = saturate(color*0.05);
   
   col.a = col.r * Brightness * DiffuseColor.a * color * alpha;
 
   col.rgb *= Brightness;
   col.rgb *= Color.rgb;
   
   //�e�N�X�`���̃x�N�g��
   float2 tex_vec = normalize(DistTex - CenterTex);
   col.rgb += tex2D(DistortionView,DistTex+tex_vec*0.025*dif).rgb;
   col.rgb *= col.a;
   
   return col;
}

//�e�N�j�b�N�̒�`
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
   //���C���p�X
   pass lineSystem
   {
      //Z�l�̍l���F����
      ZENABLE = TRUE;
      //Z�l�̕`��F���Ȃ�
      ZWRITEENABLE = FALSE;
      //�J�����O�I�t�i���ʕ`��
      CULLMODE = NONE;
      //���u�����h���g�p����
      ALPHABLENDENABLE = TRUE;
      //���u�����h�̐ݒ�i�ڂ����͍ŏ��̒萔���Q�Ɓj
      SRCBLEND=BLENDMODE_SRC;
      DESTBLEND=BLENDMODE_DEST;
      //�g�p����V�F�[�_��ݒ�
      VertexShader = compile vs_3_0 lineSystem_Vertex_Shader_main();
      PixelShader = compile ps_3_0 lineSystem_Pixel_Shader_main();
   }
}

