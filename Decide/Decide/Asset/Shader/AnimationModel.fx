/*!
 * @brief	スキンモデルシェーダー。(4ボーンスキニング)
 */
#include "../../../../fbEngine/fbEngine/System.h"
bool Texflg;							//テクスチャ
bool Spec;								//スペキュラ
bool ReceiveShadow;						//影を写す

float4 g_cameraPos;						//カメラの位置

//スキン行列。
#define MAX_MATRICES  50
float4x3    g_mWorldMatrixArray[MAX_MATRICES] : WORLDMATRIXARRAY;

float4x4 g_worldMatrix;					//ワールド行列。
float4x4 g_viewMatrix;					//ビュー行列。
float4x4 g_projectionMatrix;			//プロジェクション行列。
float4x4 g_rotationMatrix;				//回転行列。法線を回転させるために必要になる。ライティングするなら必須。

float	g_numBone;			//骨の数。

int g_LightNum;									//ライトの数
float4	g_diffuseLightDirection[MAX_LIGHTNUM];	//ディフューズライトの方向。
float4	g_diffuseLightColor[MAX_LIGHTNUM];		//ディフューズライトのカラー。
float4	g_ambientLight;								//環境光。

float4  g_diffuseMaterial : COLOR0;					//マテリアルカラー
float4  g_blendcolor;//混ぜる色

float4x4 g_LVP;					//ライトからみたビュープロジェクション行列

float4  g_Textureblendcolor;	//テクスチャに混ぜる色
texture g_diffuseTexture;		//ディフューズテクスチャ。
sampler g_diffuseTextureSampler = 
sampler_state
{
	Texture = <g_diffuseTexture>;
    MipFilter = NONE;
    MinFilter = NONE;
    MagFilter = NONE;
    AddressU = Wrap;
	AddressV = Wrap;
};

texture g_Shadow;				//深度テクスチャ
sampler g_ShadowSampler_0 =
sampler_state
{
	Texture = <g_Shadow>;
	MipFilter = NONE;
	MinFilter = NONE;
	MagFilter = NONE;
	AddressU = CLAMP;
	AddressV = CLAMP;
};


/*!
 * @brief	入力頂点
 */
struct VS_INPUT
{
    float4  Pos             : POSITION;
    float4  BlendWeights    : BLENDWEIGHT;
    float4  BlendIndices    : BLENDINDICES;
    float3  normal          : NORMAL;
	float4	color			: COLOR0;
    float2  uv            : TEXCOORD0;
};

/*!
 * @brief	出力頂点。
 */
struct VS_OUTPUT
{
	float4  Pos     : POSITION;
	float4	color	: COLOR0;
    float3  normal	: TEXCOORD0;
    float2  uv		: TEXCOORD1;
	float4  world	: TEXCOORD2;
	float4  lvp		: TEXCOORD3;
};

VS_OUTPUT VSMain( VS_INPUT In )
{
	VS_OUTPUT o = (VS_OUTPUT)0;
	
	//ブレンドするボーンのインデックス。
	int4 IndexVector = D3DCOLORtoUBYTE4(In.BlendIndices);
	
	//ブレンドレート。
	float BlendWeightsArray[4] = (float[4])In.BlendWeights;
    int   IndexArray[4]        = (int[4])IndexVector;
    float LastWeight = 0.0f;
    float3 Pos = 0.0f;
    float3 normal = 0.0f;
	//ボーン数ループ?
    for (int iBone = 0; iBone < g_numBone-1; iBone++)
    {
        LastWeight = LastWeight + BlendWeightsArray[iBone];
        
        Pos += mul(In.Pos, g_mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
        normal += mul(In.normal, g_mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
    }
    LastWeight = 1.0f - LastWeight; 

    //？？
	Pos += (mul(In.Pos, g_mWorldMatrixArray[IndexArray[g_numBone-1]]) * LastWeight);
    normal += (mul(In.normal, g_mWorldMatrixArray[IndexArray[g_numBone-1]]) * LastWeight);

	o.world = float4(Pos.xyz, 1.0f);
	o.lvp = mul(o.world, g_LVP);				//ライトの目線によるワールドビュー射影変換をする

	//ワールド行列
	//Pos = mul(float4(Pos.xyz, 1.0f), g_worldMatrix);
	//ビュー行列
	Pos = mul(float4(Pos.xyz, 1.0f), g_viewMatrix);
	//プロジェクション行列
	o.Pos = mul(float4(Pos.xyz, 1.0f), g_projectionMatrix);

	//法線
	//o.normal = mul(normal, g_rotationMatrix);	//法線を回す。
	o.normal = normal;

    o.uv = In.uv;
	o.color = In.color;

	return o;
}

/*!
 * @brief	ピクセルシェーダー。
 */
float4 PSMain( VS_OUTPUT In ):COLOR0
{
	float4 color = (float4)0;	//最終的に出力するカラー
	float4 diff = (float4)0;	//メッシュのマテリアル
	float3 normal = normalize(In.normal);
	//カラー
	if (Texflg)
	{
		diff = tex2D(g_diffuseTextureSampler, In.uv) * g_Textureblendcolor;
	}
	else
	{
		diff = g_diffuseMaterial;
	}
	color = diff;

	float4 light = 0.0f;

	//デフューズライトを計算。
	for (int i = 0; i < g_LightNum; i++)
	{
		//0.0f未満なら0.0fを返す
		light.xyz += max(0.0f, -dot(normal, g_diffuseLightDirection[i].xyz))* g_diffuseLightColor[i].xyz;
		light.a = 1.0f;
	}

	//スペキュラーライト
	if(true)
	{
		float3 spec = 0.0f;
		float3 toEyeDir = normalize(g_cameraPos.xyz - In.world);
		float3 R = -toEyeDir + 2.0f * dot(normal, toEyeDir) * normal;
		for (int i = 0; i < g_LightNum; i++)
		{
			//スペキュラ成分を計算する。
			//反射ベクトルを計算。
			float3 L = -g_diffuseLightDirection[i].xyz;
			spec += g_diffuseLightColor[i] * pow(max(0.0f, dot(L, R)), 2) * g_diffuseLightColor[i].w;	//スペキュラ強度。
		}

		light.xyz += spec.xyz;
	}

	if (ReceiveShadow)
	{
		sampler texSampler[3];
		texSampler[0] = g_ShadowSampler_0;
		texSampler[1] = g_ShadowSampler_0;
		texSampler[2] = g_ShadowSampler_0;

		for (int i = 0; i < 1; i++) {
			float4 posInLVP = In.lvp;
			posInLVP.xyz /= posInLVP.w;
			//uv座標に変換。
			float2 shadowMapUV = float2(0.5f, -0.5f) * posInLVP.xy + float2(0.5f, 0.5f);
			float2 shadow_val = 1.0f;

			if (shadowMapUV.x < 0.99f && shadowMapUV.y < 0.99f && shadowMapUV.x > 0.01f && shadowMapUV.y > 0.01f) {
				shadow_val = tex2D(texSampler[i], shadowMapUV).rg;
				float depth = min(posInLVP.z, 1.0f);
				//バリアンスシャドウマップのフラグ
				if (false) {
					if (depth > shadow_val.r) {
						// σ^2
						float depth_sq = shadow_val.r * shadow_val.r;
						float variance = max(shadow_val.g - depth_sq, 0.0006f);
						float md = depth - shadow_val.r;
						float P = variance / (variance + md * md);
						light.rgb *= pow(P, 5.0f);
					}
				}
				else {
					if (depth > shadow_val.r + 0.006f) {
						//色半減
						light.rgb *= 0.5f;
					}
				}
				break;
			}
		}
	}
	//影になっていない。
	//カラーにライトを掛ける
	color.xyz *= light.xyz;
	//元の色×アンビエントライトを足す
	color.xyz += diff.xyz * g_ambientLight.xyz;
	return color;
}

//本描画
technique NormalRender
{
	pass p0
	{
		VertexShader = compile vs_3_0 VSMain();
		PixelShader = compile ps_3_0 PSMain();
	}
}


//////////////////////////////////////////////////////////////
//影描画用

struct VS_ShadowIN {
	float4  pos             : POSITION;
	float4  BlendWeights    : BLENDWEIGHT;
	float4  BlendIndices    : BLENDINDICES;
};

struct VS_ShadowOUT {
	float4	pos		: POSITION;
	float4	shadow		: TEXCOORD0;
};

VS_ShadowOUT VSShadow(VS_ShadowIN In)
{
	VS_ShadowOUT Out = (VS_ShadowOUT)0;

	//ブレンドするボーンのインデックス。
	int4 IndexVector = D3DCOLORtoUBYTE4(In.BlendIndices);

	//ブレンドレート。
	float BlendWeightsArray[4] = (float[4])In.BlendWeights;
	int   IndexArray[4] = (int[4])IndexVector;
	float LastWeight = 0.0f;
	float3 pos = 0.0f;
	//ボーン数ループ?
	for (int iBone = 0; iBone < g_numBone - 1; iBone++)
	{
		LastWeight = LastWeight + BlendWeightsArray[iBone];

		pos += mul(In.pos, g_mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
	}
	LastWeight = 1.0f - LastWeight;

	//各行列をかける
	pos += (mul(In.pos, g_mWorldMatrixArray[IndexArray[g_numBone - 1]]) * LastWeight);	//最後のやつ？
	pos = mul(float4(pos.xyz, 1.0f), g_viewMatrix);
	float4 P = mul(float4(pos.xyz, 1.0f), g_projectionMatrix);
	Out.shadow = Out.pos = P;

	return Out;
}

float4 PSShadow(VS_ShadowOUT In) : COLOR0	//レンダーターゲット0に出力
{
	//深度
	float4 depth = (float4)0;

	//深度は射影変換済みの頂点の Z / W で算出できる
	depth = In.shadow.z / In.shadow.w;

	return float4(depth.xyz, 1.0f);
	//return float4(1, 0, 0, 1);
}

technique Shadow
{
	pass p0
	{
		VertexShader = compile vs_2_0 VSShadow();
		PixelShader = compile ps_2_0 PSShadow();
	}
}