#include "Sprite.h"
#include "Effect.h"
#include "VertexDefinition.h"
#include "Vertex.h"

Vertex* Sprite::_Vertex = nullptr;

Sprite::Sprite(GameObject * g, Transform * t) :
	Component(g, t, typeid(this).name()),
	_Texture(nullptr),
	_Pivot(0.5f, 0.5f),
	_UV(0.0f, 1.0f, 0.0f, 1.0f),
	_BlendColor(Color::white),
	_ClipColor(Color::zero),
	_FadeTime(10.0f),
	_FTimer(0.0f),
	_FadeLine(1.0f),
	_SpriteEffect(((DWORD)fbSprite::SpriteEffectE::NONE))
{
	
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

		//ポジション定義
		//VERTEX_POSITION position[] = {
		//	{ 0.0f, 0.0f, 0.0f, 1.0f },//左下
		//	{ 0.0f, 1.0f, 0.0f, 1.0f },//左上
		//	{ 1.0f, 0.0f, 0.0f, 1.0f },//右下
		//	{ 1.0f, 1.0f, 0.0f, 1.0f },//右上
		//};
		//UV定義
		VERTEX_TEXCOORD texcoord[] = {
			{ 0.0f, 0.0f },//左上
			{ 1.0f, 0.0f },//右上
			{ 0.0f, 1.0f },//左下
			{ 1.0f, 1.0f },//右下
		};

		//頂点宣言(頂点がどのように構成されているか)
		D3DVERTEXELEMENT9 elements[] = {
			{ 0, 0              , D3DDECLTYPE_FLOAT4  , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, // 頂点座標
			{ 1, 0              , D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD   , 0 }, // UV
			D3DDECL_END()
		};

		_Vertex->Initialize(fbEngine::PRIMITIVETYPE::TRIANGLESTRIP, 4);
		_Vertex->CreateVertexBuffer(position, 4, sizeof(VERTEX_POSITION), elements[0]);
		_Vertex->CreateVertexBuffer(texcoord, 4, sizeof(VERTEX_TEXCOORD), elements[1]);
		_Vertex->CreateDeclaration();
	}
}

void Sprite::Update()
{
	Fade();
}

void Sprite::ImageRender()
{
	//テクスチャがないなら描画しない。
	if (_Texture == nullptr)
		return;
	//各エフェクト
	{
		if (_SpriteEffect & (DWORD)fbSprite::SpriteEffectE::OUTLINE)
			_CreateOutLine();

		if (_SpriteEffect & (DWORD)fbSprite::SpriteEffectE::SHADOW)
			_CreateShadow();
	}

	//シェーダファイル読み込み
	_Effect = EffectManager::LoadEffect("Sprite.fx");

	D3DXMATRIX  matWorld, matSize, matScale, matRot, matTrans;
	//初期化
	//D3DXMatrixIdentity(&matWorld);
	//画像のサイズを設定
	D3DXMatrixScaling(&matSize, _Texture->Size.x, _Texture->Size.y, 1.0f);
	//設定されたスケールを設定
	D3DXMatrixScaling(&matScale, transform->GetScale().x, transform->GetScale().y, transform->GetScale().z);
	//回転
	D3DXMatrixRotationZ(&matRot, D3DXToRadian(transform->GetAngle().z));
	/*static float r = 0.0f;
	if (KeyBoardInput->isPressed(DIK_UP))
		r += 0.01f;
	if (KeyBoardInput->isPressed(DIK_DOWN))
		r -= 0.01f;
	if (KeyBoardInput->isPressed(DIK_RIGHT))
		r = 0.0f;
	D3DXMatrixRotationX(&matRot, D3DXToRadian(r));*/
	//移動
	D3DXMatrixTranslation(&matTrans, transform->GetPosition().x, transform->GetPosition().y, transform->GetPosition().z);

	//画像サイズ　*　スケール　*　回転　*　ポジション
	matWorld = matSize * matScale *matRot * matTrans;

	RECT rect;
	//ウィンドウハンドルからクライアントエリアのサイズを取得
	GetClientRect(g_MainWindow, &rect);
	float w = (float)rect.right;
	float h = (float)rect.bottom;

	//射影変換行列
	//スクリーンのサイズに収める。
	D3DXMATRIX proj(
		2.0f / w, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0 / h, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	//調整用、整理のため上と分けたかった
	D3DXMATRIX adjustment
	(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,	//マイナスは上下を反転させるため
		0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f		//画面の中心が基準になっているのでXに-1.0f,Yに1.0fだけオフセットする。
	);

	D3DXMATRIX wp = matWorld * (proj*adjustment);

	(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//テクニック設定
	_Effect->SetTechnique("SpriteTech");

	_Effect->Begin(NULL, 0);
	_Effect->BeginPass(0);

	//行列
	_Effect->SetMatrix("g_WP", &wp);

	//テクスチャ
	_Effect->SetTexture("g_texture", _Texture->pTexture);
	//始点と長さ
	Vector2 uvstart(_UV.x, _UV.z), uvlen(_UV.y - _UV.x, _UV.w - _UV.z);
	//UV設定
	_Effect->SetValue("g_UVStart", &uvstart, sizeof(Vector2));
	_Effect->SetValue("g_UVLength", &uvlen, sizeof(Vector2));
	//pivot設定(スケーリングや回転の基点)
	_Effect->SetFloat("g_Pivotx", _Pivot.x);
	_Effect->SetFloat("g_Pivoty", _Pivot.y);

	//色設定
	_Effect->SetValue("g_BlendColor",_BlendColor,sizeof(Color));
	//透明色設定
	_Effect->SetValue("g_ClipColor", _ClipColor, sizeof(Color));
	Vector4 flg = { (float)((int)_SpriteEffect & 2),0,0,0 };
	//フラグ送信
	_Effect->SetValue("g_EffectFlg", &flg,sizeof(Vector4));
	//消えるアレボーダーライン的なアレ
	_Effect->SetFloat("g_FadeLine", _FadeLine);

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

void Sprite::SetEffectFlg(const DWORD& e)
{
	_SpriteEffect = e;
}

bool Sprite::SetEffectFlg(const DWORD& e, bool f)
{
	//既に有効かどうか？
	if ((_SpriteEffect & e) > 0 && f == false)
	{
		//無効に
		_SpriteEffect = _SpriteEffect - e;
		return true;
	}
	else if ((_SpriteEffect & e) == 0 && f == true)
	{
		//有効に
		_SpriteEffect = _SpriteEffect + e;
		return true;
	}
	return false;
}

void Sprite::Fade()
{
	//Fadeかどうか？
	if((_SpriteEffect & (DWORD)fbSprite::SpriteEffectE::FADE) > 0)
	{
		//タイマー増加
		_FTimer += Time::DeltaTime();
		//フェードインかアウトか？
		int fadein = 1;
		//一けた目のビットで判定
		if ((_SpriteEffect & fadein) > 0)
		{
			//フェードイン(0.0->1.0)
			_FadeLine = min(1.0f, (_FTimer / _FadeTime));
		}
		else
		{
			//フェードアウト(1.0->0.0)
			_FadeLine = max(0.0f, 1.0f - (_FTimer / _FadeTime));
		}
	}
}

void Sprite::_CreateShadow()
{
	//変更前に値を保存
	Color colorbuf = _BlendColor;
	DWORD effectbuf = _SpriteEffect;
	Vector3 posbuf, pos;
	pos = posbuf = transform->GetPosition();
	//色を黒に
	_BlendColor = Color::black;
	//フラグを消す(通常描画)
	_SpriteEffect = (DWORD)fbSprite::SpriteEffectE::NONE;
	//ずらす量
	Vector2 offset = Vector2(_Texture->Size.x * 0.05f, _Texture->Size.y * 0.05f);
	
	pos.x += offset.x * transform->GetScale().x;
	pos.y += offset.y * transform->GetScale().y;
	transform->SetPosition(pos);

	ImageRender();

	//戻す
	_BlendColor = colorbuf;
	_SpriteEffect = effectbuf;
	transform->SetPosition(posbuf);
}

void Sprite::_CreateOutLine()
{
	//変更前に値を保存
	Color colorbuf = _BlendColor;
	DWORD effectbuf = _SpriteEffect;
	Vector3 posbuf, pos;
	pos = posbuf = transform->GetPosition();
	//色を黒に
	_BlendColor = Color::black;
	//フラグを消す(通常描画)
	_SpriteEffect = (DWORD)fbSprite::SpriteEffectE::NONE;
	//移動量
	float offset = 1.0f;
	//上下左右に移動させて描画
	FOR(i,4)
	{
		switch (i)
		{
		case 0:
			pos.x += offset;
			break;
		case 1:
			pos.x -= offset;
			break;
		case 2:
			pos.y += offset;
			break;
		case 3:
			pos.y -= offset;
			break;
		default:
			break;
		}
		transform->SetPosition(pos);
		ImageRender();
		transform->SetPosition(posbuf);
	}

	//戻す
	_BlendColor = colorbuf;
	_SpriteEffect = effectbuf;
	transform->SetPosition(posbuf);
}
