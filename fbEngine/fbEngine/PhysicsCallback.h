#pragma once
#include "Collision.h"

struct MyContactResultCallback : public btCollisionWorld::ContactResultCallback
{
public:
	MyContactResultCallback()
	{

	}
	//衝突された時のコールバック関数
	virtual	btScalar	addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override
	{
		const Vector3* vColl0Pos = (Vector3*)(&colObj0Wrap->getWorldTransform().getOrigin());
		const Vector3* vColl1Pos = (Vector3*)(&colObj1Wrap->getWorldTransform().getOrigin());
		Vector3 vDist;
		vDist.Subtract(*vColl0Pos, *vColl1Pos);
		float distTmpSq = vDist.LengthSq();
		Collision* hitObjectTmp;
		if (distTmpSq < distSq) {
			//こちらの方が近い。
			if (queryCollisionObject == colObj0Wrap->getCollisionObject()) {
				hitObjectTmp = (Collision*)colObj1Wrap->getCollisionObject()->getUserPointer();
			}
			else {
				hitObjectTmp = (Collision*)colObj0Wrap->getCollisionObject()->getUserPointer();
			}
			//リジッドボディ同士のあたり判定はまだ実装してない
			if (hitObjectTmp && id == hitObjectTmp->GetCollisonObj()->getUserIndex()) {
				distSq = distTmpSq;
				hitObject = hitObjectTmp;
				hitObject->SetHit(true);
			}
		}

		return 0.0f;
	}
public:
	btCollisionObject* queryCollisionObject = nullptr;
	Collision* hitObject = nullptr;
	float distSq = FLT_MAX;
	int id = 0;
};

//衝突したときに呼ばれる関数オブジェクト(地面用)
struct SweepResultGround : public btCollisionWorld::ConvexResultCallback
{
	bool isHit = false;									//衝突フラグ。
	Vector3 hitPos = Vector3(0.0f, -FLT_MAX, 0.0f);	//衝突点。
	Vector3 startPos = Vector3::zero;					//レイの始点。
	Vector3 hitNormal = Vector3::zero;				//衝突点の法線。
	btCollisionObject* me = nullptr;					//自分自身。自分自身との衝突を除外するためのメンバ。
	float dist = FLT_MAX;								//衝突点までの距離。一番近い衝突点を求めるため。FLT_MAXは単精度の浮動小数点が取りうる最大の値。

														//衝突したときに呼ばれるコールバック関数。
	virtual	btScalar	addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
	{
		if (convexResult.m_hitCollisionObject == me
			|| convexResult.m_hitCollisionObject->getUserIndex() == 5
			) {
			//自分に衝突した。or キャラクタ属性のコリジョンと衝突した。
			return 0.0f;
		}
		//衝突点の法線を引っ張ってくる。
		Vector3 hitNormalTmp = *(Vector3*)&convexResult.m_hitNormalLocal;
		//上方向と法線のなす角度を求める。
		float angle = hitNormalTmp.Dot(Vector3::up);
		angle = fabsf(acosf(angle));
		if (angle < D3DX_PI * 0.2f		//地面の傾斜が54度より小さいので地面とみなす。
			|| convexResult.m_hitCollisionObject->getUserIndex() == 999 //もしくはコリジョン属性が地面と指定されている。
			) {
			//衝突している。
			isHit = true;

			Vector3 hitPosTmp = *(Vector3*)&convexResult.m_hitPointLocal;
			//衝突点の距離を求める。。
			Vector3 vDist;
			vDist.Subtract(hitPosTmp, startPos);
			float distTmp = vDist.Length();
			if (dist > distTmp) {
				//この衝突点の方が近いので、最近傍の衝突点を更新する。
				hitPos = hitPosTmp;
				hitNormal = *(Vector3*)&convexResult.m_hitNormalLocal;
				dist = distTmp;
			}
		}
		return 0.0f;
	}
};