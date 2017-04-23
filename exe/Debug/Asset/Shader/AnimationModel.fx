/*!
 * @brief	スキンモデルシェーダー。(4ボーンスキニング)
 */
#define MAX_LIGHTNUM 4
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

float2 g_TexelSize;				//テクセルサイズ(深度バッファの)

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

texture g_Depth;				//メインカメラから見た深度テクスチャ
sampler g_DepthSampler =
sampler_state
{
	Texture = <g_Depth>;
	MipFilter = NONE;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

texture g_Shadow;				//影テクスチャ
sampler g_ShadowSampler_0 =
sampler_state
{
	Texture = <g_Shadow>;
	MipFilter = NONE;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};


/*!
 * @brief	入力頂点
 */
struct VS_INPUT
{
    float4  _Pos             : POSITION;
    float4  _BlendWeights    : BLENDWEIGHT;
    float4  _BlendIndices    : BLENDINDICES;
    float3  _Normal          : NORMAL;
	float4	_Color			: COLOR0;
    float2  _UV            : TEXCOORD0;
};

/*!
 * @brief	出力頂点。
 */
struct VS_OUTPUT
{
	float4  _Pos    : POSITION;
	float4	_Color	: COLOR0;
    float3  _Normal	: TEXCOORD0;
    float2  _UV		: TEXCOORD1;
	float4  _World	: TEXCOORD2;	//ワールド座標
	float4  _LVP	: TEXCOORD3;	//ライト(影用カメラ)からみた行列
	float4  _WVP	: TEXCOORD4;	//カメラから見た行列
};

VS_OUTPUT VSMain( VS_INPUT In )
{
	VS_OUTPUT o = (VS_OUTPUT)0;
	
	//ブレンドするボーンのインデックス。
	int4 IndexVector = D3DCOLORtoUBYTE4(In._BlendIndices);
	
	//ブレンドレート。
	float BlendWeightsArray[4] = (float[4])In._BlendWeights;
    int   IndexArray[4]        = (int[4])IndexVector;
    float LastWeight = 0.0f;
    float3 pos = 0.0f;
    float3 normal = 0.0f;
	//ボーン数ループ?
    for (int iBone = 0; iBone < g_numBone-1; iBone++)
    {
        LastWeight = LastWeight + BlendWeightsArray[iBone];
        
        pos += mul(In._Pos, g_mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
        normal += mul(In._Normal, g_mWorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
    }
    LastWeight = 1.0f - LastWeight; 

    //？？
	pos += (mul(In._Pos, g_mWorldMatrixArray[IndexArray[g_numBone-1]]) * LastWeight);
    normal += (mul(In._Normal, g_mWorldMatrixArray[IndexArray[g_numBone-1]]) * LastWeight);

	o._World = float4(pos.xyz, 1.0f);
	o._LVP = mul(o._World, g_LVP);				//ライトの目線によるワールドビュー射影変換をする

	//ワールド行列
	//_Pos = mul(float4(_Pos.xyz, 1.0f), g_worldMatrix);
	//ビュー行列
	pos = mul(float4(pos.xyz, 1.0f), g_viewMatrix);
	//プロジェクション行列
	o._WVP = o._Pos = mul(float4(pos.xyz, 1.0f), g_projectionMatrix);

	//法線
	//o._Normal = mul(normal, g_rotationMatrix);	//法線を回す。
	o._Normal = normal;

    o._UV = In._UV;
	o._Color = In._Color;

	return o;
}

/*!
 * @brief	ピクセルシェーダー。
 */
float4 PSMain( VS_OUTPUT In ):COLOR0
{
	float4 color = (float4)0;	//最終的に出力するカラー
	float4 diff = (float4)0;	//メッシュのマテリアル
	float3 normal = normalize(In._Normal);
	//カラー
	if (Texflg)
	{
		diff = tex2D(g_diffuseTextureSampler, In._UV) * g_Textureblendcolor;
	}
	else
	{
		diff = g_diffuseMaterial;
	}
	diff *= g_blendcolor;
	color = diff;

	float4 light = 0.0f;

	//エッジ抽出
	if(false)
	{
		float4 WVP = In._WVP;
		WVP.xyz /= WVP.w;		//深度計算
		float2 shadowMapUV = float2(0.5f, -0.5f) * WVP.xy + float2(0.5f, 0.5f);
		//近傍4ピクセルの色
		float4 depth,depth1, depth2, depth3, depth4;
		depth = tex2D(g_ShadowSampler_0, shadowMapUV);	//元
		depth1 = tex2D(g_ShadowSampler_0, float2(shadowMapUV.x + g_TexelSize.x, shadowMapUV.y));	//右
		depth2 = tex2D(g_ShadowSampler_0, float2(shadowMapUV.x - g_TexelSize.x, shadowMapUV.y));	//左
		depth3 = tex2D(g_ShadowSampler_0, float2(shadowMapUV.x, shadowMapUV.y + g_TexelSize.y));	//下
		depth4 = tex2D(g_ShadowSampler_0, float2(shadowMapUV.x, shadowMapUV.y - g_TexelSize.y));	//上

		//差異がしきい値を超えたなら
		if (abs(depth.x - depth1.x) > 0.15f ||
			abs(depth.x - depth2.x) > 0.15f ||
			abs(depth.x - depth3.x) > 0.15f ||
			abs(depth.x - depth4.x) > 0.15f)
		{
			return 1;
		}
	}

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
		float3 toEyeDir = normalize(g_cameraPos.xyz - In._World);
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

	//影
	if (ReceiveShadow)
	{
		sampler texSampler[3];
		texSampler[0] = g_ShadowSampler_0;
		texSampler[1] = g_ShadowSampler_0;
		texSampler[2] = g_ShadowSampler_0;

		for (int i = 0; i < 1; i++) {
			float4 posInLVP = In._LVP;
			posInLVP.xyz /= posInLVP.w;
			//uv座標に変換。
			float2 shadowMapUV = float2(0.5f, -0.5f) * posInLVP.xy + float2(0.5f, 0.5f);
			float2 shadow_val = 1.0f;

			if (shadowMapUV.x < 0.99f && shadowMapUV.y < 0.99f && shadowMapUV.x > 0.01f && shadowMapUV.y > 0.01f) {
				shadow_val = tex2D(texSampler[i], shadowMapUV).rg;
				float depth = min(posInLVP.z, 1.0f);
				//バリアンスシャドウマップのフラグ
				if (true) {
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
					//影なら
					if (depth > shadow_val.r + 0.006f) {
						//色半減
						light.rgb *= 0.5f;
					}
				}
				break;
			}
		}
	}
	//
	color.rgb *= light.rgb + g_ambientLight.rgb;
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
	float4  _BlendWeights    : BLENDWEIGHT;
	float4  _BlendIndices    : BLENDINDICES;
};

struct VS_ShadowOUT {
	float4	pos		: POSITION;
	float4	shadow		: TEXCOORD0;
};

VS_ShadowOUT VSShadow(VS_ShadowIN In)
{
	VS_ShadowOUT Out = (VS_ShadowOUT)0;

	//ブレンドするボーンのインデックス。
	int4 IndexVector = D3DCOLORtoUBYTE4(In._BlendIndices);

	//ブレンドレート。
	float BlendWeightsArray[4] = (float[4])In._BlendWeights;
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
	Out.pos = P;
	Out.shadow = P;

	return Out;
}

float4 PSShadow(VS_ShadowOUT In) : COLOR0	//レンダーターゲット0に出力
{
	//深度
	float4 depth = (float4)0;

	//深度は射影変換済みの頂点の Z / W で算出できる
	depth = In.shadow.z / In.shadow.w;

	return float4(depth.xyz, 1.0f);
}

technique Shadow
{
	pass p0
	{
		VertexShader = compile vs_2_0 VSShadow();
		PixelShader = compile ps_2_0 PSShadow();
	}
}