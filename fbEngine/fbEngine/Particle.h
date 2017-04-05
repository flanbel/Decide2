#pragma once
#include "GameObject.h"
#include "ParticleEmitter.h"

class Vertex;

//パーティクルクラス
class Particle :public GameObject
{
public:
	enum eState {
		eStateRun,
		eStateFadeOut,
		eStateDead,
	};
	Particle(char* name) :GameObject(name) {}
	void Awake()override;
	void Update()override;
	void Render() override;

	void Init(const ParicleParameter& param,const Vector3& emitPosition);
	/*!
	*@brief	パーティクルに力を加える。
	*@param[in]	_ApplyForce		乱数生成に使用する乱数生成機。
	*/
	void ApplyForce(Vector3& _ApplyForce)
	{
		this->_ApplyForce = _ApplyForce;
	}
	bool IsDead()
	{
		return _IsDead;
	}
private:		
	static Vertex* _Vertex;						//頂点


	TEXTURE*		_Texture;					//画像
	Effect*			_Effect;					//シェーダーエフェクト。
	Camera*			_Camera;					//カメラ。
	float			_Life;						//ライフ。
	float			_Timer;						//タイマー。
	Vector3			_Velocity;					//速度。
	Vector3			_Gravity;					//重力。
	float			_RotateZ;					//Z軸周りの回転。
	Vector3			_AddVelocityRandomMargih;	//速度の積分のときのランダム幅。
	bool			_IsDead;					//死亡フラグ。
	bool			_IsFade;					//死ぬときにフェードアウトする？
	float			_FadeTIme;					//フェードの時間。
	eState			_State;						//状態。
	float			_InitAlpha;					//初期アルファ。
	float			_Alpha;						//アルファ。
	bool			_IsBillboard;				//ビルボード？
	Vector3			_ApplyForce;				//外部から加わる力。
	float			_Brightness;				//輝度。ブルームが有効になっているとこれを強くすると光が溢れます。
	int				_AlphaBlendMode;			//0半透明合成、1加算合成。
	Color			_MulColor;					//乗算カラー。
};