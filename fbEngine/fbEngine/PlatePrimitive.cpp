#include "PlatePrimitive.h"
#include "Effect.h"
#include "Vertex.h"
#include "VertexDefinition.h"
#include "Camera.h"
#include "Light.h"

Vertex* PlatePrimitive::_Vertex = nullptr;

PlatePrimitive::PlatePrimitive(char * name) :
	GameObject(name),
	blendColor(Color::white)
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

		_Vertex->Initialize(PRIMITIVETYPE::TRIANGLESTRIP, 4);
		_Vertex->CreateVertexBuffer(position, 4, sizeof(VERTEX_POSITION), elements[0]);
		_Vertex->CreateVertexBuffer(texcoord, 4, sizeof(VERTEX_TEXCOORD), elements[1]);
		_Vertex->CreateVertexBuffer(normal, 4, sizeof(VERTEX_NORMAL), elements[2]);
		_Vertex->CreateDeclaration();
	}
}

void PlatePrimitive::Awake()
{
	//エフェクト読み込み
	effect = EffectManager::LoadEffect("Primitive.fx");
	size = Vector2(1.0f, 1.0f);
	pivot = Vector2(0.5f, 0.5f);
	texture = nullptr;
}

void PlatePrimitive::Start()
{
	camera = GameObjectManager::mainCamera;
	light = GameObjectManager::mainLight;
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
	D3DXMatrixScaling(&matSize, size.x, size.y, 1.0f);
	//ピボット分移動
	matWorld._41 = pivot.x;
	matWorld._42 = pivot.y;
	//pivot * サイズ　*　ワールド(スケール　*　回転　*　ポジション)
	matWorld = matWorld * matSize * transform->WorldMatrix();

	//Zバッファ
	(*graphicsDevice()).SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//テクニック設定
	effect->SetTechnique("NormalRender");

	effect->Begin(NULL, 0);
	effect->BeginPass(0);

	effect->SetMatrix("g_worldMatrix", &matWorld);
	effect->SetMatrix("g_rotationMatrix", &transform->RotateMatrix());
	effect->SetMatrix("g_viewMatrix", &camera->View());
	effect->SetMatrix("g_projectionMatrix", &camera->Projection());

	effect->SetValue("g_blendColor", blendColor, sizeof(Color));

	int num = light->GetNum();
	Vector4 dir[MAX_LIGHTNUM];
	Color color[MAX_LIGHTNUM];
	vector<DirectionalLight*> vec = light->GetLight();
	FOR(num)
	{
		dir[i] = vec[i]->Direction();
		color[i] = vec[i]->color;
	}
	//ライトの向きを転送。
	effect->SetValue("g_diffuseLightDirection", &dir, sizeof(Vector4)*MAX_LIGHTNUM);
	//ライトのカラーを転送。
	effect->SetValue("g_diffuseLightColor", &color, sizeof(Color)*MAX_LIGHTNUM);
	//ライト数セット
	effect->SetInt("g_LightNum", num);
	//環境光
	effect->SetVector("g_ambientLight", &D3DXVECTOR4(0.5, 0.5, 0.5, 1));
	//カメラのポジションセット
	Vector3 campos = camera->transform->position;
	effect->SetValue("g_cameraPos", &D3DXVECTOR4(campos.x, campos.y, campos.z, 1.0f), sizeof(D3DXVECTOR4));

	//テクスチャが格納されていればセット
	if (texture != NULL)
	{
		effect->SetTexture("g_Texture", texture);
		effect->SetBool("Texflg", true);
	}
	//テクスチャがないならカラーセット
	else
	{
		//白色
		effect->SetValue("g_Color", &Color::white, sizeof(Color));
		effect->SetBool("Texflg", false);
	}
	//反射フラグ
	effect->SetBool("Spec", false);

	//この関数を呼び出すことで、データの転送が確定する。描画を行う前に一回だけ呼び出す。
	effect->CommitChanges();

	_Vertex->DrawPrimitive();

	effect->EndPass();
	effect->End();

	//変更したステートを元に戻す
	(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	(*graphicsDevice()).SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void PlatePrimitive::SetTexture(TEXTURE* t)
{
	size = t->size;
	texture = t->pTexture;
}

void PlatePrimitive::SetPivot(Vector2 v2)
{
	pivot = v2;
}

void PlatePrimitive::SetPivot(float x, float y)
{
	pivot = Vector2(x, y);
}

void PlatePrimitive::SetBlendColor(Color c)
{
	blendColor = c;
}
