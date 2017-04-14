#include "PlatePrimitive.h"
#include "Effect.h"
#include "Vertex.h"
#include "VertexDefinition.h"
#include "Camera.h"
#include "Light.h"

Vertex* PlatePrimitive::_Vertex = nullptr;

PlatePrimitive::PlatePrimitive(char * name) :
	GameObject(name),
	_BlendColor(Color::white)
{
	//頂点バッファ作成
	if (_Vertex == nullptr)
	{
		_Vertex = new Vertex();
		//ポジション定義
		VERTEX_POSITION position[] = {
			{ 0.0f, 0.0f, 0.0f, 1.0f },//左上
			{ 1.0f, 0.0f, 0.0f, 1.0f },//右上
			{ 0.0f, -1.0f, 0.0f, 1.0f },//左下
			{ 1.0f, -1.0f, 0.0f, 1.0f },//右下
		};
		//UV定義
		VERTEX_TEXCOORD texcoord[] = {
			{ 0.0f, 0.0f },//左上
			{ 1.0f, 0.0f },//右上
			{ 0.0f, 1.0f },//左下
			{ 1.0f, 1.0f },//右下
		};
		//法線定義
		VERTEX_NORMAL normal[] = {
			{ 0.0f, 0.0f, -1.0f, 1.0f },//左上
			{ 0.0f, 0.0f, -1.0f, 1.0f },//右上
			{ 0.0f, 0.0f, -1.0f, 1.0f },//左下
			{ 0.0f, 0.0f, -1.0f, 1.0f },//右下
		};

		//頂点宣言(頂点がどのように構成されているか)
		D3DVERTEXELEMENT9 elements[] = {
			//ストリーム番号,オフセット値,データ型,?,セマンティクス,セマンティクス番号
			{ 0, 0, D3DDECLTYPE_FLOAT4,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION, 0 } , // 頂点座標
			{ 1, 0, D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD, 0 } , // UV
			{ 2, 0,	D3DDECLTYPE_FLOAT4,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_NORMAL,0 },//法線
		};

		_Vertex->Initialize(fbEngine::PRIMITIVETYPE::TRIANGLESTRIP, 4);
		_Vertex->CreateVertexBuffer(position, 4, sizeof(VERTEX_POSITION), elements[0]);
		_Vertex->CreateVertexBuffer(texcoord, 4, sizeof(VERTEX_TEXCOORD), elements[1]);
		_Vertex->CreateVertexBuffer(normal, 4, sizeof(VERTEX_NORMAL), elements[2]);
		_Vertex->CreateDeclaration();
	}
}

void PlatePrimitive::Awake()
{
	//エフェクト読み込み
	_Effect = EffectManager::LoadEffect("Primitive.fx");
	_Size = Vector2(1.0f, 1.0f);
	_Pivot = Vector2(0.5f, 0.5f);
	_Texture = nullptr;
}

void PlatePrimitive::Start()
{
	_Camera = GameObjectManager::mainCamera;
	_Light = GameObjectManager::mainLight;
}

void PlatePrimitive::Update()
{

}

void PlatePrimitive::Render()
{
	D3DXMATRIX  matWorld, matSize;

	D3DXMatrixIdentity(&matSize);
	D3DXMatrixIdentity(&matWorld);
	//サイズを設定
	D3DXMatrixScaling(&matSize, _Size.x, _Size.y, 1.0f);
	//ピボット分移動
	matWorld._41 = _Pivot.x;
	matWorld._42 = _Pivot.y;
	//_Pivot * サイズ　*　ワールド(スケール　*　回転　*　ポジション)
	matWorld = matWorld * matSize * transform->GetWorldMatrix();

	//Zバッファ
	(*graphicsDevice()).SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//テクニック設定
	_Effect->SetTechnique("NormalRender");

	_Effect->Begin(NULL, 0);
	_Effect->BeginPass(0);

	_Effect->SetMatrix("g_worldMatrix", &matWorld);
	_Effect->SetMatrix("g_rotationMatrix", transform->GetRotateMatrixAddress());
	_Effect->SetMatrix("g_viewMatrix", &(D3DXMATRIX)_Camera->GetViewMat());
	_Effect->SetMatrix("g_projectionMatrix", &(D3DXMATRIX)_Camera->GetProjectionMat());

	_Effect->SetValue("g_blendColor", _BlendColor, sizeof(Color));

	const int num = _Light->GetNum();
	Vector4 dir[System::MAX_LIGHTNUM];
	Color color[System::MAX_LIGHTNUM];
	const vector<DirectionalLight*>& vec = _Light->GetLight();
	FOR(idx,num)
	{
		dir[idx] = vec[idx]->Direction();
		color[idx] = vec[idx]->GetColor();
	}
	//ライトの向きを転送。
	_Effect->SetValue("g_diffuseLightDirection", &dir, sizeof(Vector4)*System::MAX_LIGHTNUM);
	//ライトのカラーを転送。
	_Effect->SetValue("g_diffuseLightColor", &color, sizeof(Color)*System::MAX_LIGHTNUM);
	//ライト数セット
	_Effect->SetInt("g_LightNum", num);
	//環境光
	_Effect->SetVector("g_ambientLight", &D3DXVECTOR4(0.5, 0.5, 0.5, 1));
	//カメラのポジションセット
	Vector3 campos = _Camera->transform->GetPosition();
	_Effect->SetValue("g_cameraPos", &D3DXVECTOR4(campos.x, campos.y, campos.z, 1.0f), sizeof(D3DXVECTOR4));

	//テクスチャが格納されていればセット
	if (_Texture != NULL)
	{
		_Effect->SetTexture("g_Texture", _Texture);
		_Effect->SetBool("Texflg", true);
	}
	//テクスチャがないならカラーセット
	else
	{
		//白色
		_Effect->SetValue("g_Color", &Color::white, sizeof(Color));
		_Effect->SetBool("Texflg", false);
	}
	//反射フラグ
	_Effect->SetBool("Spec", false);

	//この関数を呼び出すことで、データの転送が確定する。描画を行う前に一回だけ呼び出す。
	_Effect->CommitChanges();

	_Vertex->DrawPrimitive();

	_Effect->EndPass();
	_Effect->End();

	//変更したステートを元に戻す
	(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	(*graphicsDevice()).SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void PlatePrimitive::SetTexture(TEXTURE* t)
{
	_Size = t->Size;
	_Texture = t->pTexture;
}

void PlatePrimitive::SetPivot(Vector2 v2)
{
	_Pivot = v2;
}

void PlatePrimitive::SetPivot(float x, float y)
{
	_Pivot = Vector2(x, y);
}

void PlatePrimitive::SetBlendColor(Color c)
{
	_BlendColor = c;
}
