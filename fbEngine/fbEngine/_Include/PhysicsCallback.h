#pragma once
#include "_Include\CollisionInclude.h"

namespace fbPhysicsCallback
{
	//ゴーストと重なっているペアを探すコールバック
	struct MyGhostPairCallback : public btGhostPairCallback
	{
	public:
		//重なったときに呼ばれる関数
		btBroadphasePair*	addOverlappingPair(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1)
		{
			btCollisionObject* colObj0 = (btCollisionObject*)proxy0->m_clientObject;
			btCollisionObject* colObj1 = (btCollisionObject*)proxy1->m_clientObject;
			//ゴーストオブジェクトにアップキャスト
			//(元がゴーストオブジェクト以外ならnullになる)
			btGhostObject* ghost0 = btGhostObject::upcast(colObj0);
			btGhostObject* ghost1 = btGhostObject::upcast(colObj1);
			
			if (ghost0)
			{
				//ペア追加
				ghost0->addOverlappingObjectInternal(proxy1, proxy0);
				Collision* coll0 = (Collision*)ghost0->getUserPointer();
				Collision* coll1 = (Collision*)ghost1->getUserPointer();
			}
			if (ghost1)
			{
				//ペア追加
				ghost1->addOverlappingObjectInternal(proxy0, proxy1);
			}
			return 0;
		}
		//重なりから抜けた時に呼ばれる関数
		void*	removeOverlappingPair(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1, btDispatcher* dispatcher)
		{
			btCollisionObject* colObj0 = (btCollisionObject*)proxy0->m_clientObject;
			btCollisionObject* colObj1 = (btCollisionObject*)proxy1->m_clientObject;
			//ゴーストオブジェクトにアップキャスト
			//(元がゴーストオブジェクト以外ならnullになる)
			btGhostObject* ghost0 = btGhostObject::upcast(colObj0);
			btGhostObject* ghost1 = btGhostObject::upcast(colObj1);
			if (ghost0)
			{
				ghost0->removeOverlappingObjectInternal(proxy1, dispatcher, proxy0);
			}
			if (ghost1)
			{
				ghost1->removeOverlappingObjectInternal(proxy0, dispatcher, proxy1);
			}
			return 0;
		}
	};

	//ヒットした中で最も近いものを返すコールバック
	struct ClosestContactResultCallback : public btCollisionWorld::ContactResultCallback
	{
	public:
		ClosestContactResultCallback()
		{

		}
		//衝突された時の関数
		virtual	btScalar	addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override
		{
			
			//ポジション取得
			const Vector3* vColl0Pos = (Vector3*)(&colObj0Wrap->getWorldTransform().getOrigin());
			const Vector3* vColl1Pos = (Vector3*)(&colObj1Wrap->getWorldTransform().getOrigin());
			Vector3 vDist;
			//自身と当たったオブジェクトとの距離を計算
			vDist.Subtract(*vColl0Pos, *vColl1Pos);
			//距離(2乗のままなのは計算を省くため)
			float distTmpSq = vDist.LengthSq();
			//一時的(temporary)にコリジョンを格納する
			Collision* hitObjectTmp;
			//近いなら
			if (distTmpSq < distSq) {
				// col0 と col1 のどちらかは自分なのでアドレスを比較して確かめる
				if (me->GetCollisonObj() == colObj0Wrap->getCollisionObject()) {
					hitObjectTmp = (Collision*)colObj1Wrap->getCollisionObject()->getUserPointer();
				}
				else {
					hitObjectTmp = (Collision*)colObj0Wrap->getCollisionObject()->getUserPointer();
				}
				//hitオブジェクトがある
				if (hitObjectTmp)
				{
					//属性が一致するか？マスクをとる
					if ((attribute & hitObjectTmp->GetCollisonObj()->getUserIndex()) != 0)
					{
						//距離を更新
						distSq = distTmpSq;
						//コリジョンを更新
						hitObject = hitObjectTmp;
					}
				}
			}

			return 0.0f;
		}
	public:
		int attribute;					//指定したコリジョン属性とのみ当たり判定をとる
		float distSq = FLT_MAX;			//距離を保持
		Collision* me = nullptr;		//自身のアドレス
		Collision* hitObject = nullptr;	//ヒットしたオブジェクト
	};

	//ヒットしたもの全てを返すコールバック
	struct AllHitsContactResultCallback : public btCollisionWorld::ContactResultCallback
	{
	public:
		AllHitsContactResultCallback()
		{

		}
		//衝突された時の関数
		virtual	btScalar	addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override
		{
			//一時的(temporary)にコリジョンを格納する
			Collision* hitObjectTmp = nullptr;

			// col0 と col1 のどちらかは自分なのでアドレスを比較して確かめる
			if (me->GetCollisonObj() == colObj0Wrap->getCollisionObject()) {
				hitObjectTmp = (Collision*)colObj1Wrap->getCollisionObject()->getUserPointer();
			}
			else {
				hitObjectTmp = (Collision*)colObj0Wrap->getCollisionObject()->getUserPointer();
			}

			//hitオブジェクトがある
			if (hitObjectTmp)
			{
				//属性が一致するか？マスクをとる
				if ((attribute & hitObjectTmp->GetCollisonObj()->getUserIndex()) != 0)
				{
					//重複チェック
					for each (Collision* coll in hitObjects)
					{
						//アドレス比較
						if(coll == hitObjectTmp)
						{
							return 0.0f;
						}
					}
					//コリジョンを追加
					hitObjects.push_back(hitObjectTmp);
				}
			}
			return 0.0f;
		}
	public:
		int attribute;					//指定したコリジョン属性とのみ当たり判定をとる
		Collision* me = nullptr;		//自身のアドレス
		vector<Collision*> hitObjects;	//ヒットしたオブジェクト
	};

	//レイを飛ばしてヒットした中で最も近いものを返す。
	struct	ClosestRayResultCallback : public btCollisionWorld::RayResultCallback
	{
	public:
		ClosestRayResultCallback()
		{
		}

		virtual	btScalar	addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
		{
			if ((attribute & rayResult.m_collisionObject->getUserIndex()) == 0)
			{
				return 0.0f;
			}

			//始点から終点を0～1にした時のヒットした位置の割合。
			m_closestHitFraction = rayResult.m_hitFraction;
			//ヒットしたコリジョン
			m_collisionObject = rayResult.m_collisionObject;
			if (normalInWorldSpace)
			{
				m_hitNormalWorld = rayResult.m_hitNormalLocal;
			}
			else
			{
				///need to Transform normal into worldspace
				m_hitNormalWorld = m_collisionObject->getWorldTransform().getBasis()*rayResult.m_hitNormalLocal;
			}
			//ヒットした位置を取得
			m_hitPointWorld.setInterpolate3(fromPos, toPos, rayResult.m_hitFraction);
			return rayResult.m_hitFraction;
		}
	public:
		int attribute;						//指定したコリジョン属性とのみ当たり判定をとる
		btVector3	fromPos;				//レイの始点
		btVector3	toPos;					//レイの終点

		btVector3	m_hitNormalWorld;		//衝突点の法線
		btVector3	m_hitPointWorld;		//レイがヒットした位置
		Collision* hitObject = nullptr;						//ヒットしたオブジェクト
	};

	//コリジョンを飛ばして最初にヒットしたコリジョンを返すコールバック
	struct ClosestConvexResultCallback : public btCollisionWorld::ConvexResultCallback
	{
	public:
		//衝突したときに呼ばれる関数。
		virtual	btScalar	addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
		{
			//自身と衝突したか？ &&
			//属性が合わなかった
			if (convexResult.m_hitCollisionObject == me &&
				(attribute & convexResult.m_hitCollisionObject->getUserIndex()) == 0)
			{
				//ヒットしなかった。
				return 0.0f;
			}

			//ヒットした位置
			Vector3 hitPosTmp = *(Vector3*)&convexResult.m_hitPointLocal;
			//衝突点の距離を求める
			Vector3 vDist;
			vDist.Subtract(hitPosTmp, startPos);
			float distTmp = vDist.Length();
			//距離比較
			if (dist > distTmp) {
				//更新
				hitPos = hitPosTmp;
				dist = distTmp;
				hitObject = (Collision*)convexResult.m_hitCollisionObject->getUserPointer();
			}
			return 0.0f;
		}
	public:
		int attribute;										//指定したコリジョン属性とのみ当たり判定をとる。
		float dist = FLT_MAX;								//衝突点までの距離。一番近い衝突点を求めるため。FLT_MAXは単精度の浮動小数点が取りうる最大の値。
		Vector3 startPos = Vector3::zero;					//レイの始点。
		Vector3 hitPos = Vector3(0.0f, -FLT_MAX, 0.0f);		//衝突点。
		btCollisionObject* me = nullptr;					//自分自身。自分自身との衝突を除外するためのメンバ。
		Collision* hitObject = nullptr;						//ヒットしたオブジェクト
	};

	//衝突したときに呼ばれる関数オブジェクト(地面用)
	struct SweepResultGround : public btCollisionWorld::ConvexResultCallback
	{
	public:
		//衝突したときに呼ばれる関数。
		virtual	btScalar	addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
		{
			if (convexResult.m_hitCollisionObject == me
				|| (convexResult.m_hitCollisionObject->getUserIndex() & (short)fbCollisionAttributeE::CHARACTER) != 0
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
				|| (convexResult.m_hitCollisionObject->getUserIndex() & (short)fbCollisionAttributeE::GROUND) != 0 //もしくはコリジョン属性が地面と指定されている。
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
	public:
		bool isHit = false;									//衝突フラグ。
		Vector3 hitPos = Vector3(0.0f, -FLT_MAX, 0.0f);	//衝突点。
		Vector3 startPos = Vector3::zero;					//レイの始点。
		Vector3 hitNormal = Vector3::zero;				//衝突点の法線。
		btCollisionObject* me = nullptr;					//自分自身。自分自身との衝突を除外するためのメンバ。
		float dist = FLT_MAX;								//衝突点までの距離。一番近い衝突点を求めるため。FLT_MAXは単精度の浮動小数点が取りうる最大の値。
	};
}