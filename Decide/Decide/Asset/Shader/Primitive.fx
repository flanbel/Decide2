/*!
 *@brief	3Dモデルの描画
 */
#define MAX_LIGHTNUM 4
bool Texflg;							//テクスチャ
bool Spec;	//スペキュラフラグ

float4x4 g_rotationMatrix;				//回転行列。法線を回転させるために必要になる。ライティングするなら必須。
float4x4 g_worldMatrix;					//ワールド行列。
float4x4 g_viewMatrix;					//ビュー行列。
float4x4 g_projectionMatrix;			//プロジェクション行列。

int g_LightNum;									//ライトの数
float4	g_diffuseLightDirection[MAX_LIGHTNUM];	//ディフューズライトの方向。
float4	g_diffuseLightColor[MAX_LIGHTNUM];		//ディフューズライトのカラー。
float4	g_ambientLight;								//環境光。

float4 g_cameraPos;						//カメラの位置

float4  g_Color;		//カラー
float4  g_blendColor;

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

//頂点情報構造体

struct VS_INPUT{
	float4	pos		: POSITION;
	float2	uv		: TEXCOORD0;
	float3	normal	: NORMAL;
};

struct VS_OUTPUT{
	float4	pos		: POSITION;
	float2	uv		: TEXCOORD0;
	float3	normal	: TEXCOORD1;
	float4  world	: TEXCOORD2;
	float4	wvp		: TEXCOORD3;
};


/*!
 *@brief	頂点シェーダー。
 */
VS_OUTPUT VSMain(VS_INPUT In)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	float4 pos;
	pos = mul( In.pos, g_worldMatrix );		//モデルのローカル空間からワールド空間に変換。
	
	Out.world = pos;
	
	pos = mul( pos, g_viewMatrix );			//ワールド空間からビュー空間に変換。
	pos = mul( pos, g_projectionMatrix );	//ビュー空間から射影空間に変換。

	Out.pos = pos;
	Out.wvp = Out.pos;
	Out.uv = In.uv;
	Out.normal = mul(normalize(In.normal), g_rotationMatrix);	//法線を回す。
	
	return Out;
}

struct PS_OUTPUT {
	//色
	float4 color	: COLOR0;
	//深度
	float4 depth	: COLOR1;
	//輝度
	float4 brightness	: COLOR2;
};

/*!
 *@brief	ピクセルシェーダー。
 */
PS_OUTPUT PSMain(VS_OUTPUT In)
{
	PS_OUTPUT Out = (PS_OUTPUT)0;
	//テクスチャの有無
	if (Texflg){
		//uvに張り付け
		Out.color = tex2D(g_TextureSampler, In.uv);
	}
	else
	{
		//マテリアルのカラーにする。
		Out.color = g_Color;
	}

	Out.color *= g_blendColor;

	//デフューズライトを計算。
	{
		float4 dif = 0.0f;
		for (int i = 0; i < g_LightNum; i++)
		{
			//0.0f未満なら0.0fを返す
			dif.xyz += max(0.0f, dot(In.normal.xyz, -g_diffuseLightDirection[i].xyz))
				* g_diffuseLightColor[i].xyz;
		}

		dif += g_ambientLight;

		Out.color.xyz *= dif.xyz;
	}

	//スペキュラーライト
	if(Spec)
	{
		float3 spec = 0.0f;
		float3 toEyeDir = normalize(g_cameraPos.xyz - In.world);
		float3 R = -toEyeDir + 2.0f * dot(In.normal, toEyeDir) * In.normal;

		for (int i = 0; i < g_LightNum; i++)
		{
			//スペキュラ成分を計算する。
			//反射ベクトルを計算。
			float3 L = -g_diffuseLightDirection[i].xyz;
			spec += g_diffuseLightColor[i] * pow(max(0.0f, dot(L, R)), 10) * g_diffuseLightColor[i].w * 100;	//スペキュラ強度。
		}
		Out.color.xyz += spec.xyz;
	}
	//深度計算
	//深度は射影変換済みの頂点の Z / W で算出できる
	Out.depth = In.wvp.z / In.wvp.w;
	Out.brightness = float4(1, 1, 1, 1);

	return Out;
}

////////////////////////////
//テクニック

//普通に描画する用
technique NormalRender
{
	pass p0
	{
		VertexShader = compile vs_3_0 VSMain();
		PixelShader = compile ps_3_0 PSMain();
	}
}

technique PreRender
{
	pass p0
	{
		VertexShader = compile vs_3_0 VSMain();
		PixelShader = compile ps_3_0 PSMain();
	}
}