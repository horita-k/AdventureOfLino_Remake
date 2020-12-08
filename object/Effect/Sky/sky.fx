//�X�N���[�����x
float2 ScrollSpd
<
   string UIName = "ScrollSpd";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float2 UIMin = -1;
   float2 UIMax = 1;
> = float2( 0.0, 0.2 );
//�_�S�̗̂ʁi���݁H�j 0�`1
float CloudVolume
<
   string UIName = "CloudVolume";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0;
   float UIMax = 1;
> = float( 0.6 );
//�_�̐F
float3 CloudColor
<
   string UIName = "CloudColor";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float3 UIMin = 0;
   float3 UIMax = 1;
> = float4( 1.00, 1.00, 1.00, 1.0 );
//�����T�C�Y
float Scale
<
   string UIName = "Scale";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0;
   float UIMax = 100000;
> = float( 80000 );

//�e�N�X�`�����[�v��
int TexLoop
<
   string UIName = "TexLoop";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   int UIMin = 0;
   int UIMax = 50;
> = int( 30 );

//�����X�N���[���l
float FirstScroll
<
   string UIName = "FirstScroll";
   string UIWidget = "Numeric";
   bool UIVisible =  true;
   float UIMin = 0;
   float UIMax = 1;
> = float( 0 );

#define BLENDMODE_SRC SRCALPHA
#define BLENDMODE_DEST INVSRCALPHA

//--�悭�킩��Ȃ��l�͂������牺�͂�������Ⴞ��--//

float time_0_X : Time;
float4x4 wvpmat : WorldViewProjection;

float4   MaterialDiffuse   : DIFFUSE  < string Object = "Geometry"; >;
float3   LightDiffuse      : DIFFUSE   < string Object = "Light"; >;
static float4 DiffuseColor  = MaterialDiffuse  * float4(LightDiffuse, 1.0f);

texture SkyTex_s
<
   string ResourceName = "CloudMain.png";
>;
sampler SkyTex_s_Sampler = sampler_state
{
   Texture = (SkyTex_s);
   ADDRESSU = WRAP;
   ADDRESSV = WRAP;
   Filter = LINEAR;
};
texture SkyTex_m
<
   string ResourceName = "CloudSub1.png";
>;
sampler SkyTex_m_Sampler = sampler_state
{
   Texture = (SkyTex_m);
   ADDRESSU = WRAP;
   ADDRESSV = WRAP;
   Filter = LINEAR;
};
texture SkyTex_l
<
   string ResourceName = "CloudSub2.png";
>;
sampler SkyTex_l_Sampler = sampler_state
{
   Texture = (SkyTex_l);
   ADDRESSU = WRAP;
   ADDRESSV = WRAP;
   Filter = LINEAR;
};

struct VS_OUTPUT {
   float4 Pos: POSITION;
   float2 texCoord: TEXCOORD0;
};

VS_OUTPUT SkyDome_Vertex_Shader_main(float4 Pos: POSITION){
   VS_OUTPUT Out;

   Out.texCoord = (Pos.xz*0.5 + 0.5) * (TexLoop+1);

   //�ϊ��ς݃t���O���I�t
   Pos.w = 0;
   //�����T�C�Y�Ɋg��
   Pos *= Scale;
   //�ϊ��ς݃t���O���I��
   Pos.w = 1;
   Out.Pos = mul(Pos, wvpmat);

   return Out;
}
//�s�N�Z���V�F�[�_
float4 SkyDome_Pixel_Shader_main(float2 texCoord: TEXCOORD0,float color: TEXCOORD1) : COLOR {
   
   float2 add = ScrollSpd * (time_0_X + FirstScroll);
   
   float4 col_s = tex2D(SkyTex_s_Sampler,texCoord + add);
   float4 col_m = tex2D(SkyTex_m_Sampler,texCoord + add*0.75);
   float4 col_l = tex2D(SkyTex_l_Sampler,texCoord + add*0.5);
   
   float4 col = col_s + col_m + col_l;
   float a = col.r*(1-CloudVolume);
   col.a = min(1,((1-a)));
   if(CloudVolume < 0.1)
   {
   		col.a *= (CloudVolume * 10);
   }
   col.rgb = CloudColor;
   
   
   return col;
}

//�e�N�j�b�N�̒�`
technique lineSystem <
    string Script = 
		//�`��Ώۂ����C����ʂ�
        "RenderColorTarget0=;"
	    "RenderDepthStencilTarget=;"
	    //�p�X�̑I��
	    "Pass=lineSystem;"
    ;
> {
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
      VertexShader = compile vs_2_0 SkyDome_Vertex_Shader_main();
      PixelShader = compile ps_2_0 SkyDome_Pixel_Shader_main();
	/*
   	*/
   }
}

