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

	void Init(const ParicleParameter& param,Vector3& emitPosition);
	/*!
	*@brief	パーティクルに力を加える。
	*@param[in]	applyForce		乱数生成に使用する乱数生成機。
	*/
	void ApplyForce(Vector3& applyForce)
	{
		this->applyForce = applyForce;
	}
	bool IsDead()
	{
		return isDead;
	}
private:		
	static Vertex* _Vertex;						//頂点


	TEXTURE*		texture;
	Effect*			shaderEffect;				//!<シェーダーエフェクト。
	Camera*	camera;						//!<カメラ。
	float			life;						//!<ライフ。
	float			timer;						//!<タイマー。
	Vector3			velocity;					//!<速度。
	Vector3			gravity;					//!<重力。
	float			rotateZ;					//!<Z軸周りの回転。
	Vector3			addVelocityRandomMargih;	//!<速度の積分のときのランダム幅。
	bool			isDead;						//!<死亡フラグ。
	bool			isFade;						//!<死ぬときにフェードアウトする？
	float			fadeTIme;					//!<フェードの時間。
	eState			state;						//!<状態。
	float			initAlpha;					//!<初期アルファ。
	float			alpha;						//!<アルファ。
	bool			isBillboard;				//!<ビルボード？
	Vector3			applyForce;					//!<外部から加わる力。
	float			brightness;					//!<輝度。ブルームが有効になっているとこれを強くすると光が溢れます。
	int				alphaBlendMode;				//!<0半透明合成、1加算合成。
	Color			mulColor;					//!<乗算カラー。
};