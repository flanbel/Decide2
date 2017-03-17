/*!
 *@brief	3Dモデルの描画
 */
#define MAX_LIGHTNUM 4
bool Texflg;							//テクスチャ
bool Spec;								//スペキュラ
bool ReceiveShadow;						//影を写す

bool SkyBox;

float4 g_cameraPos;						//カメラの位置

float4x4 g_rotationMatrix;				//回転行列。法線を回転させるために必要になる。ライティングするなら必須。
float4x4 g_worldMatrix;					//ワールド行列。
float4x4 g_viewMatrix;					//ビュー行列。
float4x4 g_projectionMatrix;			//プロジェクション行列。

int g_LightNum;									//ライトの数
float4	g_diffuseLightDirection[MAX_LIGHTNUM];	//ディフューズライトの方向。
float4	g_diffuseLightColor[MAX_LIGHTNUM];		//ディフューズライトのカラー。
float4	g_ambientLight;							//環境光。

float4  g_diffuseMaterial;		//マテリアルカラー
float4  g_blendcolor;			//全体に混ぜる色

float4x4 g_LVP;					//ライトからみたビュープロジェクション行列

float2 g_TexelSize;				//テクセルサイズ(深度バッファの)

float4  g_Textureblendcolor;	//テクスチャに混ぜる色
texture g_Texture;				//テクスチャ。
sampler g_TextureSampler = 
sampler_state
{
	Texture = <g_Texture>;
    MipFilter = NONE;
    MinFilter = NONE;
    MagFilter = NONE;
	AddressU = CLAMP;
	AddressV = CLAMP;
};
			
samplerCUBE g_cubeSampler =
sampler_state
{
	Texture = <g_Texture>;
	MipFilter = NONE;
	MinFilter = NONE;
	MagFilter = NONE;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

texture g_Shadow;				//深度テクスチャ
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

//頂点情報構造体

struct VS_INPUT{
	float4	_Pos		: POSITION;
	float4	_Color	: COLOR0;
	float3	_Normal	: NORMAL;
	float2	_UV		: TEXCOORD0;
};

struct VS_OUTPUT{
	float4	_Pos	: POSITION;
	float4	_Color	: COLOR0;
	float3	_Normal	: NORMAL;
	float2	_UV		: TEXCOORD0;
	float4  _World	: TEXCOORD1;
	float4	_LVP	: TEXCOORD2;
};


/*!
 *@brief	頂点シェーダー。
 */
VS_OUTPUT VSMain(VS_INPUT In)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	float4 pos;
	pos = mul( In._Pos, g_worldMatrix );		//モデルのローカル空間からワールド空間に変換。
	
	Out._World = pos;						//ワールド行列を保持
	Out._LVP = mul(pos, g_LVP);				//ライトの目線によるワールドビュー射影変換をする
	//スカイボックスはビュー行列をかけない。
	if (!SkyBox)
	{
		pos = mul(pos, g_viewMatrix);			//ワールド空間からビュー空間に変換。
	}
	pos = mul( pos, g_projectionMatrix );	//ビュー空間から射影空間に変換。

	Out._Pos = pos;
	Out._Color = In._Color;
	Out._UV = In._UV;
	Out._Normal = mul(In._Normal, g_rotationMatrix);	//法線を回す。
	
	return Out;
}

/*!
 *@brief	ピクセルシェーダー。
 */
float4 PSMain(VS_OUTPUT In):COLOR0
{
	float4 color = (float4)0;	//最終的に出力するカラー
	float4 diff = (float4)0;	//メッシュのマテリアル
	float3 normal = normalize(In._Normal.xyz);
	//カラー
	if (Texflg)
	{
		if(SkyBox)
		{
			
			//反転しているので-1をかけて法線をもどす
			diff = texCUBE(g_cubeSampler, In._Normal * -1.0f);
			return diff;
		}
		else
		{
			diff = tex2D(g_TextureSampler, In._UV) * g_Textureblendcolor;
		}
	}
	else
	{
		diff = g_diffuseMaterial;
	}
	diff *= g_blendcolor;
	color = diff;

	float4 light = (float4)0;

	//デフューズライトを計算。
	for (int i = 0; i < g_LightNum; i++)
	{
		//0.0f未満なら0.0fを返す
		light.rgb += max(0.0f, -dot(normal, g_diffuseLightDirection[i].xyz)) * g_diffuseLightColor[i].rgb;
	}

	//スペキュラーライト
	if (true)
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

		light.rgb += spec.xyz;
	}

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

	color.rgb *= light.rgb + g_ambientLight.rgb;
	
	return color;
}

//普通に描画する用
technique NormalRender
{
	pass p0
	{
		VertexShader = compile vs_3_0 VSMain();
		PixelShader = compile ps_3_0 PSMain();
	}
}

//////////////////////////////////////////////////////////////
//事前描画

struct PS_PreOUTPUT {
	float4	_Color	: COLOR0;		//色
	float4	_Depth	: COLOR1;		//深度
	float4	_Luminance	: COLOR2;	//輝度
};

PS_PreOUTPUT PSPre(VS_OUTPUT In)
{
	PS_PreOUTPUT o = (PS_PreOUTPUT)0;	//最終的に出力するカラー
	float4 diff = (float4)0;	//メッシュのマテリアル
	float3 normal = normalize(In._Normal.xyz);
	//カラー
	if (Texflg)
	{
		if (SkyBox)
		{

			//反転しているので-1をかけて法線をもどす
			o._Color = texCUBE(g_cubeSampler, In._Normal * -1.0f);
			return o;
		}
		else
		{
			diff = tex2D(g_TextureSampler, In._UV) * g_Textureblendcolor;
		}
	}
	else
	{
		//マテリアルの色そのまま
		diff = g_diffuseMaterial;
	}
	diff *= g_blendcolor;
	o._Color = diff;

	float4 light = (float4)0;

	//デフューズライトを計算。
	for (int i = 0; i < g_LightNum; i++)
	{
		//0.0f未満なら0.0fを返す
		light.rgb += max(0.0f, -dot(normal, g_diffuseLightDirection[i].xyz)) * g_diffuseLightColor[i].rgb;
	}

	//スペキュラーライト
	if (true)
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

		light.rgb += spec.xyz;
	}

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

	o._Color.rgb *= light.rgb + g_ambientLight.rgb;

	return o;
}

technique PreRender
{
	pass p0
	{
		VertexShader = compile vs_3_0 VSMain();
		PixelShader = compile ps_3_0 PSPre();
	}
}

//////////////////////////////////////////////////////////////
//影描画用

struct VS_ShadowOUT {
	float4	_Pos		: POSITION;
	float4	_Shadow	: TEXCOORD;
};

VS_ShadowOUT VSShadow(float4 Pos : POSITION)
{
	VS_ShadowOUT Out = (VS_ShadowOUT)0;

	float4 pos;
	pos = mul(Pos, g_worldMatrix);		//モデルのローカル空間からワールド空間に変換。
	pos = mul(pos, g_viewMatrix);		//ワールド空間からビュー空間に変換。
	pos = mul(pos, g_projectionMatrix);	//ビュー空間から射影空間に変換。

	Out._Shadow = Out._Pos = pos;

	return Out;
}

float4 PSShadow(VS_ShadowOUT In) : COLOR0	//レンダーターゲット0に出力
{
	//深度
	float4 depth = (float4)0;

	//深度は射影変換済みの頂点の Z / W で算出できる
	depth = In._Shadow.z / In._Shadow.w;
	
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