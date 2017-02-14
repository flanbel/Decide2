#include "ParticleEmitter.h"
#include "Particle.h"

/*!
 * @brief	パーティクル生成機
 */
void ParticleEmitter::Init(const ParicleParameter& param )
{
		this->param = param;
		timer = param.intervalTime;
}
void ParticleEmitter::Start()
{
}
void ParticleEmitter::Update()
{
	if (emit)
	{
		timer += Time::DeltaTime();
		if (timer >= param.intervalTime) {
			//パーティクルを生成。
			Particle* p = new Particle("particle");
			p->Awake();
			p->Init(param, transform->position);
			timer = 0.0f;
			particleList.push_back(p);
		}
	}
	list<Particle*>::iterator p = particleList.begin();
	while (p != particleList.end()) {
		//死
		if ((*p)->IsDead()) {
			SAFE_DELETE(*p);
			p = particleList.erase(p);
		}
		//生
		else {
			(*p)->Update();
			p++;
		}
	}
}
	/*!
	*@brief	パーティクルに力を加える。
	*@param[in]	applyForce		乱数生成に使用する乱数生成機。
	*/
void ParticleEmitter::ApplyForce(Vector3& applyForce)
{
	for (auto p : particleList) {
		p->ApplyForce(applyForce);
	}
}
	void ParticleEmitter::Emit(bool b)
	{
		emit = b;
	}
void ParticleEmitter::Render()
{
	for (auto p : particleList) {
		p->Render();
	}
}
