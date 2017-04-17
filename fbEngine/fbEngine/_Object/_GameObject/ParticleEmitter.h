#pragma once
#include "GameObject.h"
class Particle;

//パーティクル生成パラメータ
struct ParicleParameter {
	//初期化。
	void Init()
	{
		memset(this, 0, sizeof(ParicleParameter));
		size = Vector2(1, 1);
		initAlpha = 1.0f;
		brightness = 1.0f;
		isBillboard = true;
		mulColor = Color::white;
	}
	const char* texturePath;						//!<テクスチャのファイルパス。
	Vector3		initVelocity;						//!<初速度。
	Vector2		size;								//パーティクルサイズ
	float		life;								//!<寿命。単位は秒。
	float		intervalTime;						//!<発生時間。単位は秒。
	Vector3		initPositionRandomMargin;			//!<初期位置のランダム幅。
	Vector3		initVelocityVelocityRandomMargin;	//!<初速度のランダム幅。
	Vector3		addVelocityRandomMargih;			//!<速度の積分のときのランダム幅。
	Vector3		gravity;							//!<重力。
	bool		isFade;								//!<死ぬときにフェードアウトする？
	float		fadeTime;							//!<フェードする時間。
	float		initAlpha;							//!<初期アルファ値。
	bool		isBillboard;						//!<ビルボード？
	float		brightness;							//!<輝度。ブルームが有効になっているとこれを強くすると光が溢れます。
	int			alphaBlendMode;						//!<0半透明合成、1加算合成。
	Color		mulColor;							//!<乗算カラー。
};
	
//パーティクルの発生機	
class ParticleEmitter : public GameObject {
public:
	ParticleEmitter(char* name) :GameObject(name) {};
	~ParticleEmitter() {};
	/*!
	 *@brief	初期化。
	 *@_Param[in]	random		乱数生成に使用する乱数生成機。
	 *@_Param[in]	camera		パーティクルの描画で使用するカメラ。
	 *@_Param[in]	_Param		パーティクル生成用のパラメータ。
	 *@_Param[in]	emitPosition	エミッターの座標。
	 */
	void Init(const ParicleParameter& _Param);
	void Start() override;
	void Update() override;
	void Render() override;
	/*!
	*@brief	パーティクルに力を加える。
	*@_Param[in]	applyForce		乱数生成に使用する乱数生成機。
	*/
	void ApplyForce(Vector3& applyForce);
	void SetEmitFlg(bool b);
private:
	//パーティクル生成
	void Emit();
private:
	bool	emit;//生成フラグ
	float					_Timer;			//!<タイマー
	ParicleParameter		_Param;			//!<パラメータ。
	std::list<Particle*>	_ParticleList;	//!<パーティクルのリスト。
};