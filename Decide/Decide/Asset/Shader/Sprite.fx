/*
 *2D描画shader
 */
//マトリックス
float4x4	wp;

//基点
float pivotx;
float pivoty;

//ブレンドカラー
float4 blendColor;
//透過色(仮)
float4 clipColor;

//テクスチャ。
texture g_texture;
sampler g_textureSampler =
sampler_state
{
	Texture = <g_texture>;
	MipFilter = NONE;
	MinFilter = NONE;
	MagFilter = NONE;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_INPUT
{
	float4 pos		: POSITION;
	float2 uv		: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 pos		: POSITION;
	float2 uv		: TEXCOORD0;
};

//頂点シェーダ
VS_OUTPUT vs_main(VS_INPUT In)
{
	//Outを0で初期化
	VS_OUTPUT Out = (VS_OUTPUT)0;
	//ピボット調整
	Out.pos = In.pos - float4(pivotx, pivoty, 0, 0);
	Out.pos = mul(Out.pos, wp);
	
	//UV調整
	Out.uv = In.uv;

	return Out;
}

/*!
* @brief	半透明合成用のピクセルシェーダー。
*/
float4 ps_main(VS_OUTPUT In) : COLOR0
{
	float4 color = tex2D(g_textureSampler, In.uv);
	
	//透明色と同じカラーなら透明に
	if (clipColor.r == color.r &&
		clipColor.g == color.g &&
		clipColor.b == color.b &&
		clipColor.a == color.a)
	{
		clip(-1);
	}

	return color * blendColor;
}

//sprite描画テクニック
technique SpriteTech
{
	//半透明合成
	pass p0
	{
		VertexShader = compile vs_2_0 vs_main();
		PixelShader = compile ps_2_0 ps_main();
	}
};