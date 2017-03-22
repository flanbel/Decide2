/*
 *2D描画shader
 */
//マトリックス
float4x4 g_WP;

//UVの始点
float2 g_UVStart;
//UVの長さ
float2 g_UVLength;

//基点
float g_Pivotx;
float g_Pivoty;

//ブレンドカラー
float4 g_BlendColor;
//透過色(仮)
float4 g_ClipColor;

//エフェクトフラグ	xはフェード
float4 g_EffectFlg;
//消えるライン
float g_FadeLine;

//テクスチャ。
texture g_texture;
sampler g_textureSampler =
sampler_state
{
	Texture = <g_texture>;
	MipFilter = NONE;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
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
	//スクリーン変換をかける前にピボット調整
	Out.pos = In.pos - float4(g_Pivotx, g_Pivoty, 0, 0);
	//スクリーン画面に変換
	Out.pos = mul(Out.pos, g_WP);
	
	//UV調整
	Out.uv = g_UVStart + (In.uv * g_UVLength);

	return Out;
}

/*!
* @brief	半透明合成用のピクセルシェーダー。
*/
float4 ps_main(VS_OUTPUT In) : COLOR0
{
	float4 color = tex2D(g_textureSampler, In.uv);
	
	//消す色と同じ色ならピクセルキル
	if (g_ClipColor.r == color.r &&
		g_ClipColor.g == color.g &&
		g_ClipColor.b == color.b &&
		g_ClipColor.a == color.a)
	{
		clip(-1);
	}

	//フェードフラグ確認
	if (g_EffectFlg.x)
	{
		color.a = g_FadeLine;
	}

	return color * g_BlendColor;
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