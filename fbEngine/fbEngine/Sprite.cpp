#include "Sprite.h"
#include "Effect.h"
#include "VertexDefinition.h"

//ポジションバッファー
LPDIRECT3DVERTEXBUFFER9 Sprite::postionBuffer = nullptr;
//UVバッファー
LPDIRECT3DVERTEXBUFFER9 Sprite::texcoordBuffer = nullptr;
//デコレーション
IDirect3DVertexDeclaration9 *Sprite::pvertexDec = nullptr;

void Sprite::Awake()
{
	blendColor = Color::white;
	clipColor = Color(0, 0, 0, 0);
	mask = false;
	Shadow = false;
}

void Sprite::Start()
{
	//頂点バッファ作成

	if (postionBuffer == nullptr)
	{
		//後で上下反転させるのであえて左回りにつくる。
		//ポジション定義
		VERTEX_POSITION position[] = {
			{ 0.0f, 0.0f, 0.0f, 1.0f },//左下
			{ 1.0f, 0.0f, 0.0f, 1.0f },//右下
			{ 0.0f, 1.0f, 0.0f, 1.0f },//左上
			{ 1.0f, 1.0f, 0.0f, 1.0f },//右上
		};
		//右回り
		//VERTEX_POSITION position[] = {
		//	{ 0.0f, 1.0f, 0.0f, 1.0f },//左上
		//	{ 1.0f, 1.0f, 0.0f, 1.0f },//右上
		//	{ 0.0f, 0.0f, 0.0f, 1.0f },//左下
		//	{ 1.0f, 0.0f, 0.0f, 1.0f },//右下
		//};
		//UV定義
		VERTEX_TEXCOORD texcoord[] = {
			{ 0.0f, 0.0f },//左下
			{ 1.0f, 0.0f },//右下
			{ 0.0f, 1.0f },//左上
			{ 1.0f, 1.0f },//右上
		};

		//ポジションの頂点バッファ作成
		(*graphicsDevice()).CreateVertexBuffer(
			sizeof(VERTEX_POSITION) * 4,	//頂点バッファのサイズ(VERTEX_POSITION*頂点数)
			D3DUSAGE_WRITEONLY,
			D3DFVF_XYZW,
			D3DPOOL_MANAGED,
			&postionBuffer,
			NULL
		);

		//UVの頂点バッファ生成
		(*graphicsDevice()).CreateVertexBuffer(
			sizeof(VERTEX_TEXCOORD) * 4,	//頂点バッファのサイズ(VERTEX_TEXCOORD*頂点数)
			D3DUSAGE_WRITEONLY,
			D3DFVF_TEX0,
			D3DPOOL_MANAGED,
			&texcoordBuffer,
			NULL
		);

		//頂点バッファの先頭アドレスへの参照を格納する。
		VOID* pVertices;
		//アドレス取得
		postionBuffer->Lock(
			0,						//ロックする先頭
			sizeof(VERTEX_POSITION) * 4,	//ロックする量
			(void**)&pVertices,		//先頭アドレス
			D3DLOCK_DISCARD			//フラグ
		);
		//アドレスの参照へ頂点定義をコピー
		memcpy(pVertices, position, sizeof(VERTEX_POSITION) * 4);
		//ロック解除
		postionBuffer->Unlock();


		//アドレス取得
		texcoordBuffer->Lock(
			0,						//ロックする先頭
			sizeof(VERTEX_TEXCOORD) * 4,	//ロックする量
			(void**)&pVertices,		//先頭アドレス
			D3DLOCK_DISCARD			//フラグ
		);
		//アドレスの参照へ頂点定義をコピー
		memcpy(pVertices, texcoord, sizeof(VERTEX_TEXCOORD) * 4);
		//ロック解除
		texcoordBuffer->Unlock();

		//頂点宣言(頂点がどのように構成されているか)
		D3DVERTEXELEMENT9 vertexElem[] = {
			{ 1, 0              , D3DDECLTYPE_FLOAT4  , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, // 頂点座標
			{ 0, 0              , D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD   , 0 }, // UV
			D3DDECL_END()
		};
		//デコレーション作成
		(*graphicsDevice()).CreateVertexDeclaration(vertexElem, &pvertexDec);
	}
}

void Sprite::PreRender()
{
	
}

void Sprite::ImageRender()
{
	if (texture == nullptr)
		return;

	if (Shadow)
		CreateShadow();

	//エフェクト読み込み
	effect = EffectManager::LoadEffect("Sprite.fx");

	D3DXMATRIX  matWorld, matSize, matScale, matRot, matTrans;

	D3DXMatrixIdentity(&matWorld);
	//画像のサイズを設定
	D3DXMatrixScaling(&matSize, texture->size.x, texture->size.y, 1.0f);
	//設定されたスケールを設定
	D3DXMatrixScaling(&matScale, transform->scale.x, transform->scale.y, transform->scale.z);
	//回転
	D3DXMatrixRotationZ(&matRot, D3DXToRadian(transform->angle.z));
	//移動
	D3DXMatrixTranslation(&matTrans, transform->position.x, transform->position.y, transform->position.z);

	//画像サイズ　*　スケール　*　回転　*　ポジション
	matWorld = matSize * matScale * matRot * matTrans;

	RECT rect;
	//ウィンドウハンドルからクライアントエリアのサイズを取得
	GetClientRect(g_MainWindow, &rect);
	float w = (float)(rect.right - rect.left);
	float h = (float)(rect.bottom - rect.top);

	// 射影変換行列？
	//縦横を画面のサイズでスケーリング
	//画面の中心が基準になっているのでXに-1.0f,Yに1.0fだけオフセットする。
	D3DXMATRIX Proj(
		2.0f / w, 0.0f, 0.0f, 0.0f,
		0.0f, -2.0 / h, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f
	);

	D3DXMATRIX wp = matWorld * Proj;

	(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//テクニック設定
	effect->SetTechnique("SpriteTech");

	effect->Begin(NULL, 0);
	effect->BeginPass(0);

	//行列
	effect->SetMatrix("wp", &wp);

	//テクスチャ
	effect->SetTexture("g_texture", texture->pTexture);
	//pivot設定(スケーリングや回転の基点)
	effect->SetFloat("pivotx", texture->pivot.x);
	effect->SetFloat("pivoty", texture->pivot.y);

	//色設定
	effect->SetValue("blendColor",blendColor,sizeof(Color));
	//透明色設定
	effect->SetValue("clipColor", clipColor, sizeof(Color));

	//この関数を呼び出すことで、データの転送が確定する。描画を行う前に一回だけ呼び出す。
	effect->CommitChanges();

	//FVFのかわり
	(*graphicsDevice()).SetVertexDeclaration(pvertexDec);     // 頂点宣言を通知
	//ストリームセット
	(*graphicsDevice()).SetStreamSource(1, postionBuffer, 0, sizeof(VERTEX_POSITION));
	(*graphicsDevice()).SetStreamSource(0, texcoordBuffer, 0, sizeof(VERTEX_TEXCOORD));


	//D3DPT_TRIANGLESTRIPは連続した頂点で三角形を形成
	(*graphicsDevice()).DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	effect->EndPass();
	effect->End();

	//変更したステートを元に戻す
	(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
}

void Sprite::CreateShadow()
{
	//影生成
	float offset = 5.0f;
	Color hoji = blendColor;
	blendColor = Color::black;
	transform->position.x += offset * transform->scale.x;
	transform->position.y += offset * transform->scale.y;
	Shadow = false;

	ImageRender();

	//戻す
	blendColor = hoji;
	transform->position.x -= offset * transform->scale.x;
	transform->position.y -= offset * transform->scale.y;
	Shadow = true;
}