/*
 *2D描画shader
 */

//フェードカラー
float4 g_FadeColor;

//消えるライン
float g_Alpha;

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
	VS_OUTPUT Out = (VS_OUTPUT)0;
	//正規化されているのでそのまま
	Out.pos = In.pos;
	//そのまま
	Out.uv = In.uv;

	return Out;
}

/*!
* @brief	半透明合成用のピクセルシェーダー。
*/
float4 ps_main(VS_OUTPUT In) : COLOR0
{
	float4 color = g_FadeColor;
	//アルファ変更
	color.a = g_Alpha;

	return color;
}

technique SceneTech
{
	pass p0
	{
		VertexShader = compile vs_2_0 vs_main();
		PixelShader = compile ps_2_0 ps_main();
	}
};