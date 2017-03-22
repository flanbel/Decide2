/*!
 * @brief	ブルーム
 */

texture g_Scene;	//シーンテクスチャ。

sampler g_SceneSampler = 
sampler_state
{
    Texture = <g_Scene>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

struct VS_INPUT{
	float4	pos : POSITION;
};

struct VS_OUTPUT{
	float4 pos : POSITION;
	float2  tex : TEXCOORD;
};


VS_OUTPUT VSMain( VS_INPUT In )
{
	VS_OUTPUT Out;
	Out.pos = In.pos;		//トランスフォーム済み頂点なのでそのまま//-1.0f~1.0fの間ってことっぽい
	Out.tex = (In.pos * 0.5f) + 0.5f;
	Out.tex.y = 1.0f - Out.tex.y;
	return Out;
}
float4 PSSamplingLuminance( VS_OUTPUT In ) : COLOR
{
	//テクスチャから1以上の数値が取れるのか？
	float4 color = tex2D(g_SceneSampler, In.tex);
	float t;
	//t = dot(color.xyz, float3(0.2125f, 0.7154f, 0.0721f));
	t = dot(color.xyz, float3(0.2f, 0.2f, 0.2f));
	clip(t - 1.001f);			//輝度が1.0以下ならピクセルキル
	color.xyz *= (t - 1.0f);
	return color;
}

struct VS_BlurOutput{
	float4 pos	: POSITION;
	float2 tex0	: TEXCOORD0;
	float2 tex1 : TEXCOORD1;
	float2 tex2 : TEXCOORD2;
	float2 tex3 : TEXCOORD3;
	float2 tex4 : TEXCOORD4;
	float2 tex5 : TEXCOORD5;
	float2 tex6 : TEXCOORD6;
	float2 tex7 : TEXCOORD7;
};
texture g_Blur;	//ブラーテクスチャ

sampler g_blurSampler = 
sampler_state
{
    Texture = <g_Blur>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};




float2 g_LuminanceTexSize;		//輝度テクスチャのサイズ。
float2 g_Offset;				//オフセット
float  g_Weight[8];				//ガウスフィルタの重み。
/*!
 * @brief	Xブラー頂点シェーダー。
 */
VS_BlurOutput VSXBlur(VS_INPUT In)
{
	VS_BlurOutput Out;
	Out.pos = In.pos;
	float2 tex = (In.pos * 0.5f) + 0.5f;;
	tex.y = 1.0f - tex.y;
	tex += float2( 0.5/g_LuminanceTexSize.x, 0.5/g_LuminanceTexSize.y);
	Out.tex0 = tex + float2( - 1.0f/g_LuminanceTexSize.x, 0.0f );
    Out.tex1 = tex + float2( - 3.0f/g_LuminanceTexSize.x, 0.0f );
    Out.tex2 = tex + float2( - 5.0f/g_LuminanceTexSize.x, 0.0f );
    Out.tex3 = tex + float2( - 7.0f/g_LuminanceTexSize.x, 0.0f );
    Out.tex4 = tex + float2( - 9.0f/g_LuminanceTexSize.x, 0.0f );
    Out.tex5 = tex + float2( -11.0f/g_LuminanceTexSize.x, 0.0f );
    Out.tex6 = tex + float2( -13.0f/g_LuminanceTexSize.x, 0.0f );
    Out.tex7 = tex + float2( -15.0f/g_LuminanceTexSize.x, 0.0f );
    return Out;
}
/*!
 * @brief	Xブラーピクセルシェーダー。
 */
float4 PSXBlur( VS_BlurOutput In ) : COLOR
{
	float4 Color;
	Color  = g_Weight[0] * (tex2D( g_blurSampler, In.tex0 )
	                 + tex2D( g_blurSampler, In.tex7 + g_Offset ));
	Color += g_Weight[1] * (tex2D( g_blurSampler, In.tex1 )
	                 + tex2D( g_blurSampler, In.tex6 + g_Offset ));
	Color += g_Weight[2] * (tex2D( g_blurSampler, In.tex2 )
	            + tex2D( g_blurSampler, In.tex5 + g_Offset ));
	Color += g_Weight[3] * (tex2D( g_blurSampler, In.tex3 )
	                 + tex2D( g_blurSampler, In.tex4 + g_Offset ));
	Color += g_Weight[4] * (tex2D( g_blurSampler, In.tex4 )
	                 + tex2D( g_blurSampler, In.tex3 + g_Offset ));
	Color += g_Weight[5] * (tex2D( g_blurSampler, In.tex5 )
	                 + tex2D( g_blurSampler, In.tex2 + g_Offset ));
	Color += g_Weight[6] * (tex2D( g_blurSampler, In.tex6 )
	                 + tex2D( g_blurSampler, In.tex1 + g_Offset ));
	Color += g_Weight[7] * (tex2D( g_blurSampler, In.tex7 )
	                 + tex2D( g_blurSampler, In.tex0 + g_Offset ));
	return Color;
}
/*!
 * @brief	Yブラー頂点シェーダー。
 */
VS_BlurOutput VSYBlur(VS_INPUT In)
{
	VS_BlurOutput Out;
	Out.pos = In.pos;
	float2 tex = (In.pos * 0.5f) + 0.5f;
	tex.y = 1.0f - tex.y;
	tex += float2( 0.5/g_LuminanceTexSize.x, 0.5/g_LuminanceTexSize.y);
	Out.tex0 = tex + float2( 0.0f,- 1.0f/g_LuminanceTexSize.y  );
    Out.tex1 = tex + float2( 0.0f,- 3.0f/g_LuminanceTexSize.y  );
    Out.tex2 = tex + float2( 0.0f,- 5.0f/g_LuminanceTexSize.y  );
    Out.tex3 = tex + float2( 0.0f,- 7.0f/g_LuminanceTexSize.y  );
    Out.tex4 = tex + float2( 0.0f,- 9.0f/g_LuminanceTexSize.y  );
    Out.tex5 = tex + float2( 0.0f,-11.0f/g_LuminanceTexSize.y  );
    Out.tex6 = tex + float2( 0.0f,-13.0f/g_LuminanceTexSize.y  );
    Out.tex7 = tex + float2( 0.0f,-15.0f/g_LuminanceTexSize.y  );
    return Out;
}
/*!
 * @brief	Yブラーピクセルシェーダー。
 */
float4 PSYBlur( VS_BlurOutput In ) : COLOR
{
	float4 Color;
	Color  = g_Weight[0] * (tex2D( g_blurSampler, In.tex0 )
	                 + tex2D( g_blurSampler, In.tex7 + g_Offset ));
	Color += g_Weight[1] * (tex2D( g_blurSampler, In.tex1 )
	                 + tex2D( g_blurSampler, In.tex6 + g_Offset ));
	Color += g_Weight[2] * (tex2D( g_blurSampler, In.tex2 )
	            + tex2D( g_blurSampler, In.tex5 + g_Offset ));
	Color += g_Weight[3] * (tex2D( g_blurSampler, In.tex3 )
	                 + tex2D( g_blurSampler, In.tex4 + g_Offset ));
	Color += g_Weight[4] * (tex2D( g_blurSampler, In.tex4 )
	                 + tex2D( g_blurSampler, In.tex3 + g_Offset ));
	Color += g_Weight[5] * (tex2D( g_blurSampler, In.tex5 )
	                 + tex2D( g_blurSampler, In.tex2 + g_Offset ));
	Color += g_Weight[6] * (tex2D( g_blurSampler, In.tex6 )
	                 + tex2D( g_blurSampler, In.tex1 + g_Offset ));
	Color += g_Weight[7] * (tex2D( g_blurSampler, In.tex7 )
	                 + tex2D( g_blurSampler, In.tex0 + g_Offset ));

	return Color;
}

//合成テクスチャ。
texture g_CombineTex00;
sampler g_combineSampler00 = 
sampler_state
{
    Texture = <g_CombineTex00>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};
texture g_CombineTex01;
sampler g_combineSampler01 = 
sampler_state
{
    Texture = <g_CombineTex01>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};
texture g_CombineTex02;
sampler g_combineSampler02 = 
sampler_state
{
    Texture = <g_CombineTex02>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};
texture g_CombineTex03;
sampler g_combineSampler03 = 
sampler_state
{
    Texture = <g_CombineTex03>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

texture g_CombineTex04;
sampler g_combineSampler04 = 
sampler_state
{
    Texture = <g_CombineTex04>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

float4 PSCombine( VS_OUTPUT In ) : COLOR
{
	float2 uv = In.tex;
	uv += g_Offset;
	float4 combineColor = tex2D(g_combineSampler00, uv);
	combineColor += tex2D(g_combineSampler01, uv);
	combineColor += tex2D(g_combineSampler02, uv);
	combineColor += tex2D(g_combineSampler03, uv);
	combineColor += tex2D(g_combineSampler04, uv);
	combineColor.xyz /= 5.0f;
	return combineColor;
}
/*!
 * @brief	ファイナル。
 */
VS_OUTPUT VSFinal( VS_INPUT In )
{
	VS_OUTPUT Out;
	Out.pos = In.pos;		//トランスフォーム済み頂点なのでそのまま
	Out.tex = (In.pos.xy * 0.5f) + 0.5f;
	Out.tex.y = 1.0f - Out.tex.y;
	Out.tex += g_Offset;
	return Out;
}
float4 PSFinal( VS_OUTPUT In ) : COLOR
{
	float2 uv = In.tex;
	return clamp(tex2D(g_blurSampler, uv ), 0.0f, 1.0f);
}
/*!
 * @brief	輝度抽出テクニック。
 */
technique SamplingLuminance
{
	pass p0
	{
		VertexShader = compile vs_3_0 VSMain();
		PixelShader = compile ps_3_0 PSSamplingLuminance();
	}
}

/*!
 * @brief	横ブラー
 */
technique XBlur
{
	pass p0
	{
		VertexShader = compile vs_3_0 VSXBlur();
		PixelShader = compile ps_3_0 PSXBlur();
	}
}

/*!
 * @brief	縦ブラー
 */
technique YBlur
{
	pass p0
	{
		VertexShader = compile vs_3_0 VSYBlur();
		PixelShader = compile ps_3_0 PSYBlur();
	}
}
/*!
 * @brief	合成。
 */
technique Combine
{
	pass p0
	{
		VertexShader = compile vs_3_0 VSMain();
		PixelShader = compile ps_3_0 PSCombine();
	}
}
technique Final
{
	pass p0
	{
		VertexShader = compile vs_3_0 VSFinal();
		PixelShader = compile ps_3_0 PSFinal();
	}
}