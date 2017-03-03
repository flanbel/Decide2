#include "Sprite.h"
#include "Effect.h"
#include "VertexDefinition.h"
#include "Vertex.h"

Vertex* Sprite::_Vertex = nullptr;

Sprite::Sprite(GameObject * g, Transform * t) :
	Component(g, t),
	_Texture(nullptr),
	_Pivot(0.5f, 0.5f),
	_BlendColor(Color::white),
	_ClipColor(Color::zero),
	_SpriteEffect(SpriteEffectE::NONE)
{
	name = (char*)typeid(*this).name();
}

void Sprite::Awake()
{
	
}

void Sprite::Start()
{
	//頂点バッファ作成
	if(_Vertex == nullptr)
	{
		_Vertex = new Vertex();
		//後で上下反転させるのであえて左回りにつくる。
		//ポジション定義
		VERTEX_POSITION position[] = {
			{ 0.0f, 0.0f, 0.0f, 1.0f },//左下
			{ 1.0f, 0.0f, 0.0f, 1.0f },//右下
			{ 0.0f, 1.0f, 0.0f, 1.0f },//左上
			{ 1.0f, 1.0f, 0.0f, 1.0f },//右上
		};
		//UV定義
		VERTEX_TEXCOORD texcoord[] = {
			{ 0.0f, 0.0f },//左下
			{ 1.0f, 0.0f },//右下
			{ 0.0f, 1.0f },//左上
			{ 1.0f, 1.0f },//右上
		};

		//頂点宣言(頂点がどのように構成されているか)
		D3DVERTEXELEMENT9 elements[] = {
			{ 0, 0              , D3DDECLTYPE_FLOAT4  , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, // 頂点座標
			{ 1, 0              , D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD   , 0 }, // UV
			D3DDECL_END()
		};

		_Vertex->Initialize(PRIMITIVETYPE::TRIANGLESTRIP, 4);
		_Vertex->CreateVertexBuffer(position, 4, sizeof(VERTEX_POSITION), elements[0]);
		_Vertex->CreateVertexBuffer(texcoord, 4, sizeof(VERTEX_TEXCOORD), elements[1]);
		_Vertex->CreateDeclaration();
	}
}

void Sprite::PreRender()
{
	
}

void Sprite::ImageRender()
{
	if (_Texture == nullptr)
		return;

	if (_SpriteEffect & SpriteEffectE::SHADOW)
		_CreateShadow();

	//エフェクト読み込み
	_Effect = EffectManager::LoadEffect("Sprite.fx");

	D3DXMATRIX  matWorld, matSize, matScale, matRot, matTrans;

	D3DXMatrixIdentity(&matWorld);
	//画像のサイズを設定
	D3DXMatrixScaling(&matSize, _Texture->size.x, _Texture->size.y, 1.0f);
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
	_Effect->SetTechnique("SpriteTech");

	_Effect->Begin(NULL, 0);
	_Effect->BeginPass(0);

	//行列
	_Effect->SetMatrix("wp", &wp);

	//テクスチャ
	_Effect->SetTexture("g_texture", _Texture->pTexture);
	//pivot設定(スケーリングや回転の基点)
	_Effect->SetFloat("pivotx", _Pivot.x);
	_Effect->SetFloat("pivoty", _Pivot.y);

	//色設定
	_Effect->SetValue("blendColor",_BlendColor,sizeof(Color));
	//透明色設定
	_Effect->SetValue("clipColor", _ClipColor, sizeof(Color));

	//この関数を呼び出すことで、データの転送が確定する。描画を行う前に一回だけ呼び出す。
	_Effect->CommitChanges();
	//描画
	_Vertex->DrawPrimitive();

	_Effect->EndPass();
	_Effect->End();

	//変更したステートを元に戻す
	(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
}

void Sprite::_CreateShadow()
{
	//影生成
	float offset = 7.0f;
	Color save = _BlendColor;
	//色を黒に
	_BlendColor = Color::black;
	transform->position.x += offset * transform->scale.x;
	transform->position.y += offset * transform->scale.y;
	_SpriteEffect = (SpriteEffectE)(_SpriteEffect - SpriteEffectE::SHADOW);

	ImageRender();

	//戻す
	_BlendColor = save;
	transform->position.x -= offset * transform->scale.x;
	transform->position.y -= offset * transform->scale.y;
	_SpriteEffect = (SpriteEffectE)(_SpriteEffect + SpriteEffectE::SHADOW);
}