#include "stdafx.h"
#include "Particle.h"
#include "Camera.h"
#include "Effect.h"
#include "EffectManager.h"
#include "VertexDefinition.h"
#include "Vertex.h"

Vertex* Particle::_Vertex = nullptr;

void Particle::Awake()
{
	//頂点バッファ作成
	if (_Vertex == nullptr)
	{
		_Vertex = new Vertex();
		//後で上下反転させるのであえて左回りにつくる。
		//ポジション定義
		VERTEX_POSITION position[] = {
			{ -0.5f, 0.5f, 0.0f, 1.0f },//左上
			{ 0.5f, 0.5f, 0.0f, 1.0f},//右上
			{ -0.5f, -0.5f, 0.0f, 1.0f },//左下
			{ 0.5f, -0.5f, 0.0f, 1.0f},//右下
		};
		//UV定義
		VERTEX_TEXCOORD texcoord[] = {
			{ 0.0f, 0.0f, },//左上
			{ 1.0f, 0.0f, },//右上
			{ 0.0f, 1.0f, },//左下
			{ 1.0f, 1.0f, },//右下
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
	applyForce = Vector3::zero;
	texture = nullptr;
	isDead = false;
}

void Particle::Update()
{
	float deltaTime = Time::DeltaTime();
	Vector3 addGrafity = gravity;
	addGrafity.Scale(deltaTime);
	velocity.Add(addGrafity);
	Vector3 force = applyForce;
	force.x += (((float)Random::RandDouble() - 0.5f) * 2.0f) * addVelocityRandomMargih.x;
	force.y += (((float)Random::RandDouble() - 0.5f) * 2.0f) * addVelocityRandomMargih.y;
	force.z += (((float)Random::RandDouble() - 0.5f) * 2.0f) * addVelocityRandomMargih.z;
	force.Scale(deltaTime);
	velocity.Add(force);
	Vector3 addPos = velocity;
	addPos.Scale(deltaTime);
	applyForce = Vector3::zero;

	transform->localPosition.Add(addPos);
	transform->Update();
	if (isBillboard) {
		//ビルボード処理を行う。
		//カメラの回転行列取得
		const D3DXMATRIX& mCameraRot = camera->transform->RotateMatrix();
		Quaternion qRot;
		qRot.SetRotation(Vector3(mCameraRot.m[2][0], mCameraRot.m[2][1], mCameraRot.m[2][2]), rotateZ);
		D3DXMATRIX rot;
		//クウォータニオンから行列作成
		D3DXMatrixRotationQuaternion(&rot,(D3DXQUATERNION*)&qRot);
		
		D3DXMATRIX world,trans;
		D3DXMatrixIdentity(&trans);
		//移動行列作成
		Vector3 pos = transform->position;
		trans.m[3][0] = pos.x;
		trans.m[3][1] = pos.y;
		trans.m[3][2] = pos.z;
		//回転行列？
		D3DXMatrixMultiply(
			&world,
			&mCameraRot,
			&rot);
		//移動行列をかける
		D3DXMatrixMultiply(
			&world,
			&world,
			&rot);

		//transform->WorldMatrix(world);
	}
	

	timer += deltaTime;
	switch (state) {
	case eStateRun:
		if (timer >= life) {
			if (isFade) {
				state = eStateFadeOut;
				timer = 0.0f;
			}
			else {
				state = eStateDead;
			}
		}
		break;
	case eStateFadeOut: {
		float t = timer / fadeTIme;
		timer += deltaTime;
		alpha = initAlpha + (-initAlpha)*t;
		if (alpha <= 0.0f) {
			alpha = 0.0f;
			state = eStateDead;	//死亡。
		}
	}break;
	case eStateDead:
		isDead = true;
		break;
	}
}

void Particle::Render()
{
	D3DXMATRIX wvp;
	
	wvp = transform->WorldMatrix() * camera->View() * camera->Projection();

	//シェーダー適用開始。
	//αブレンド許可
	(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	switch (alphaBlendMode)
	{
	case 0:
		//乗算
		(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		shaderEffect->SetTechnique("ColorTexPrimTrans");
		break;
	case 1:
		//加算合成
		(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		shaderEffect->SetTechnique("ColorTexPrimAdd");
		break;
	}

	shaderEffect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
	shaderEffect->BeginPass(0);
	//Zバッファ
	(*graphicsDevice()).SetRenderState(D3DRS_ZENABLE, TRUE);
	(*graphicsDevice()).SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	shaderEffect->SetMatrix("g_mWVP", &wvp);
	shaderEffect->SetTexture("g_texture", texture->pTexture);
	shaderEffect->SetFloat("g_alpha", alpha);
	shaderEffect->SetValue("g_mulColor", mulColor, sizeof(Vector4));
	shaderEffect->CommitChanges();

	_Vertex->DrawPrimitive();

	shaderEffect->EndPass();
	shaderEffect->End();

	//変更したステートを元に戻す
	(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	(*graphicsDevice()).SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	(*graphicsDevice()).SetRenderState(D3DRS_ZENABLE, TRUE);
}

void Particle::Init(const ParicleParameter & param,Vector3 & emitPosition)
{
	texture = LOADTEXTURE((char*)param.texturePath);
	shaderEffect = EffectManager::LoadEffect("Particle.fx");
	this->camera = GameObjectManager::mainCamera;
	transform->localScale = Vector3(param.size.x, param.size.y, 1.0f);
	life = param.life;
	velocity = param.initVelocity;
	//初速度に乱数を加える。
	velocity.x += (((float)Random::RandDouble() - 0.5f) * 2.0f) * param.initVelocityVelocityRandomMargin.x;
	velocity.y += (((float)Random::RandDouble() - 0.5f) * 2.0f) * param.initVelocityVelocityRandomMargin.y;
	velocity.z += (((float)Random::RandDouble() - 0.5f) * 2.0f) * param.initVelocityVelocityRandomMargin.z;
	transform->localPosition = emitPosition;
	transform->localPosition.x += (((float)Random::RandDouble() - 0.5f) * 2.0f) * param.initPositionRandomMargin.x;
	transform->localPosition.y += (((float)Random::RandDouble() - 0.5f) * 2.0f) * param.initPositionRandomMargin.y;
	transform->localPosition.z += (((float)Random::RandDouble() - 0.5f) * 2.0f) * param.initPositionRandomMargin.z;
	addVelocityRandomMargih = param.addVelocityRandomMargih;
	gravity = param.gravity;
	isFade = param.isFade;
	state = eStateRun;
	initAlpha = param.initAlpha;
	alpha = initAlpha;
	fadeTIme = param.fadeTime;
	isBillboard = param.isBillboard;
	brightness = param.brightness;
	alphaBlendMode = param.alphaBlendMode;
	mulColor = param.mulColor;
	rotateZ = 3.1415 * 2.0f * (float)Random::RandDouble();
}