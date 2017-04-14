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

		_Vertex->Initialize(fbEngine::PRIMITIVETYPE::TRIANGLESTRIP, 4);
		_Vertex->CreateVertexBuffer(position, 4, sizeof(VERTEX_POSITION), elements[0]);
		_Vertex->CreateVertexBuffer(texcoord, 4, sizeof(VERTEX_TEXCOORD), elements[1]);
		_Vertex->CreateDeclaration();
	}
	_ApplyForce = Vector3::zero;
	_Texture = nullptr;
	_IsDead = false;
}

void Particle::Update()
{
	float deltaTime = Time::DeltaTime();
	Vector3 addGrafity = _Gravity;
	addGrafity.Scale(deltaTime);
	_Velocity.Add(addGrafity);
	Vector3 force = _ApplyForce;
	force.x += (((float)Random::RandDouble() - 0.5f) * 2.0f) * _AddVelocityRandomMargih.x;
	force.y += (((float)Random::RandDouble() - 0.5f) * 2.0f) * _AddVelocityRandomMargih.y;
	force.z += (((float)Random::RandDouble() - 0.5f) * 2.0f) * _AddVelocityRandomMargih.z;
	force.Scale(deltaTime);
	_Velocity.Add(force);
	Vector3 addPos = _Velocity;
	addPos.Scale(deltaTime);
	_ApplyForce = Vector3::zero;

	transform->SetLocalPosition(transform->GetLocalPosition() + addPos);
	transform->Update();
	if (_IsBillboard) {
		//ビルボード処理を行う。
		//カメラの回転行列取得
		//const D3DXMATRIX& mCameraRot = _Camera->transform->GetRotateMatrix();
		//Quaternion qRot;
		//qRot.SetRotation(Vector3(mCameraRot.m[2][0], mCameraRot.m[2][1], mCameraRot.m[2][2]), _RotateZ);
		//D3DXMATRIX rot;
		////クォータニオンから行列作成
		//D3DXMatrixRotationQuaternion(&rot,(D3DXQUATERNION*)&qRot);
		//
		//D3DXMATRIX world,trans;
		//D3DXMatrixIdentity(&trans);
		////移動行列作成
		//Vector3 pos = transform->GetPosition();
		//trans.m[3][0] = pos.x;
		//trans.m[3][1] = pos.y;
		//trans.m[3][2] = pos.z;
		////回転行列？
		//D3DXMatrixMultiply(
		//	&world,
		//	&mCameraRot,
		//	&rot);
		////移動行列をかける
		//D3DXMatrixMultiply(
		//	&world,
		//	&world,
		//	&rot);

		//transform->SetWorldMatrix(world);
		transform->LockAt(_Camera->gameObject);
	}
	

	_Timer += deltaTime;
	switch (_State) {
	case eStateRun:
		if (_Timer >= _Life) {
			if (_IsFade) {
				_State = eStateFadeOut;
				_Timer = 0.0f;
			}
			else {
				_State = eStateDead;
			}
		}
		break;
	case eStateFadeOut: {
		float t = _Timer / _FadeTIme;
		_Timer += deltaTime;
		_Alpha = _InitAlpha + (-_InitAlpha)*t;
		if (_Alpha <= 0.0f) {
			_Alpha = 0.0f;
			_State = eStateDead;	//死亡。
		}
	}break;
	case eStateDead:
		_IsDead = true;
		break;
	}
}

void Particle::Render()
{
	D3DXMATRIX wvp;
	
	wvp = transform->GetWorldMatrix() * _Camera->GetViewMat() * _Camera->GetProjectionMat();

	//シェーダー適用開始。
	//αブレンド許可
	(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	switch (_AlphaBlendMode)
	{
	case 0:
		//乗算
		(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		_Effect->SetTechnique("ColorTexPrimTrans");
		break;
	case 1:
		//加算合成
		(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		_Effect->SetTechnique("ColorTexPrimAdd");
		break;
	}

	_Effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
	_Effect->BeginPass(0);
	//Zバッファ
	(*graphicsDevice()).SetRenderState(D3DRS_ZENABLE, TRUE);
	(*graphicsDevice()).SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	_Effect->SetMatrix("g_mWVP", &wvp);
	_Effect->SetTexture("g_texture", _Texture->pTexture);
	_Effect->SetFloat("g_alpha", _Alpha);
	_Effect->SetValue("g_mulColor", _MulColor, sizeof(Vector4));
	_Effect->CommitChanges();

	_Vertex->DrawPrimitive();

	_Effect->EndPass();
	_Effect->End();

	//変更したステートを元に戻す
	(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	(*graphicsDevice()).SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	(*graphicsDevice()).SetRenderState(D3DRS_ZENABLE, TRUE);
}

void Particle::Init(const ParicleParameter & param,const Vector3 & emitPosition)
{
	_Texture = LOADTEXTURE((char*)param.texturePath);
	_Effect = EffectManager::LoadEffect("Particle.fx");
	this->_Camera = GameObjectManager::mainCamera;
	transform->SetLocalScale(Vector3(param.size.x, param.size.y, 1.0f));
	_Life = param.life;
	_Velocity = param.initVelocity;
	//初速度に乱数を加える。
	_Velocity.x += (((float)Random::RandDouble() - 0.5f) * 2.0f) * param.initVelocityVelocityRandomMargin.x;
	_Velocity.y += (((float)Random::RandDouble() - 0.5f) * 2.0f) * param.initVelocityVelocityRandomMargin.y;
	_Velocity.z += (((float)Random::RandDouble() - 0.5f) * 2.0f) * param.initVelocityVelocityRandomMargin.z;
	Vector3 lpos = emitPosition;
	lpos.x += (((float)Random::RandDouble() - 0.5f) * 2.0f) * param.initPositionRandomMargin.x;
	lpos.y += (((float)Random::RandDouble() - 0.5f) * 2.0f) * param.initPositionRandomMargin.y;
	lpos.z += (((float)Random::RandDouble() - 0.5f) * 2.0f) * param.initPositionRandomMargin.z;
	transform->SetLocalPosition(lpos);
	_AddVelocityRandomMargih = param.addVelocityRandomMargih;
	_Gravity = param.gravity;
	_IsFade = param.isFade;
	_State = eStateRun;
	_InitAlpha = param.initAlpha;
	_Alpha = _InitAlpha;
	_FadeTIme = param.fadeTime;
	_IsBillboard = param.isBillboard;
	_Brightness = param.brightness;
	_AlphaBlendMode = param.alphaBlendMode;
	_MulColor = param.mulColor;
	_RotateZ = 3.1415 * 2.0f * (float)Random::RandDouble();
}