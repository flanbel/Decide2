/*!
* @brief	キャラクタのコリジョンコントロール。
*/

#include "CharacterController.h"
#include "CollisionAttr.h"
#include "Physics.h"

		//衝突したときに呼ばれる関数オブジェクト(地面用)
//struct SweepResultGround : public btCollisionWorld::ConvexResultCallback
//{
//	bool isHit = false;									//衝突フラグ。
//	Vector3 hitPos = Vector3(0.0f, -FLT_MAX, 0.0f);	//衝突点。
//	Vector3 startPos = Vector3::zero;					//レイの始点。
//	Vector3 hitNormal = Vector3::zero;				//衝突点の法線。
//	btCollisionObject* me = nullptr;					//自分自身。自分自身との衝突を除外するためのメンバ。
//	float dist = FLT_MAX;								//衝突点までの距離。一番近い衝突点を求めるため。FLT_MAXは単精度の浮動小数点が取りうる最大の値。
//
//														//衝突したときに呼ばれるコールバック関数。
//	virtual	btScalar	addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
//	{
//		if (convexResult.m_hitCollisionObject == me
//			|| convexResult.m_hitCollisionObject->getUserIndex() == enCollisionAttr_Character
//			) {
//			//自分に衝突した。or キャラクタ属性のコリジョンと衝突した。
//			return 0.0f;
//		}
//		//衝突点の法線を引っ張ってくる。
//		Vector3 hitNormalTmp = *(Vector3*)&convexResult.m_hitNormalLocal;
//		//上方向と法線のなす角度を求める。
//		float angle = hitNormalTmp.Dot(Vector3::up);
//		angle = fabsf(acosf(angle));
//		if (angle < 3.1415 * 0.3f		//地面の傾斜が54度より小さいので地面とみなす。
//			|| convexResult.m_hitCollisionObject->getUserIndex() == enCollisionAttr_Ground //もしくはコリジョン属性が地面と指定されている。
//			) {
//			//衝突している。
//			isHit = true;
//			Vector3 hitPosTmp = *(Vector3*)&convexResult.m_hitPointLocal;
//			//衝突点の距離を求める。。
//			Vector3 vDist;
//			vDist.Subtract(hitPosTmp, startPos);
//			float distTmp = vDist.Length();
//			if (dist > distTmp) {
//				//この衝突点の方が近いので、最近傍の衝突点を更新する。
//				hitPos = hitPosTmp;
//				hitNormal = *(Vector3*)&convexResult.m_hitNormalLocal;
//				dist = distTmp;
//			}
//		}
//		return 0.0f;
//	}
//};
//		//衝突したときに呼ばれる関数オブジェクト(壁用)
struct SweepResultWall : public btCollisionWorld::ConvexResultCallback
{
	bool isHit = false;						//衝突フラグ。
	Vector3 hitPos = Vector3::zero;		//衝突点。
	Vector3 startPos = Vector3::zero;		//レイの始点。
	float dist = FLT_MAX;					//衝突点までの距離。一番近い衝突点を求めるため。FLT_MAXは単精度の浮動小数点が取りうる最大の値。
	Vector3 hitNormal = Vector3::zero;	//衝突点の法線。
	btCollisionObject* me = nullptr;		//自分自身。自分自身との衝突を除外するためのメンバ。
											//衝突したときに呼ばれるコールバック関数。
	virtual	btScalar	addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
	{
		if (convexResult.m_hitCollisionObject == me) {
			//自分に衝突した。or 地面に衝突した。
			return 0.0f;
		}
		//衝突点の法線を引っ張ってくる。
		Vector3 hitNormalTmp;
		hitNormalTmp.Set(convexResult.m_hitNormalLocal.x(), convexResult.m_hitNormalLocal.y(), convexResult.m_hitNormalLocal.z());
		//上方向と衝突点の法線のなす角度を求める。
		float angle = fabsf(acosf(hitNormalTmp.Dot(Vector3::up)));
		if (angle >= 3.1415 * 0.3f		//地面の傾斜が54度以上なので壁とみなす。
			|| convexResult.m_hitCollisionObject->getUserIndex() == enCollisionAttr_Character	//もしくはコリジョン属性がキャラクタなので壁とみなす。
			) {
			isHit = true;
			Vector3 hitPosTmp;
			hitPosTmp.Set(convexResult.m_hitPointLocal.x(), convexResult.m_hitPointLocal.y(), convexResult.m_hitPointLocal.z());
			//交点との距離を調べる。
			Vector3 vDist;
			vDist.Subtract(hitPosTmp, startPos);
			vDist.y = 0.0f;
			float distTmp = vDist.Length();
			if (distTmp < dist) {
				//この衝突点の方が近いので、最近傍の衝突点を更新する。
				hitPos = hitPosTmp;
				dist = distTmp;
				hitNormal = hitNormalTmp;
			}
		}
		return 0.0f;
	}
};
	


void CCharacterController::Init(float radius, float height)
{
	//コリジョン作成。
	m_radius = radius;
	m_height = height;
	m_collider->Create(radius, height);

	//剛体を初期化。
	//RigidBodyInfo rbInfo;
	//rbInfo.collider = m_collider;
	//rbInfo.mass = 0.0f;
	m_rigidBody->Create(0.0f,m_collider,5);
	btTransform& trans = m_rigidBody->GetCollisonObj()->getWorldTransform();
	//剛体の位置を更新。
	trans.setOrigin(btVector3(transform->position.x, transform->position.y, transform->position.z));
	//@todo 未対応。trans.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z));
	m_rigidBody->GetCollisonObj()->setUserIndex(enCollisionAttr_Character);
	m_rigidBody->GetCollisonObj()->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
	PhysicsWorld::Instance()->AddRigidBody(m_rigidBody);
}
void CCharacterController::Execute()
{
	//速度に重力加速度を加える。
	m_moveSpeed.y += m_gravity * 1.0f / 60.0f;
	//次の移動先となる座標を計算する。
	Vector3 nextPosition = transform->position;
	//速度からこのフレームでの移動量を求める。オイラー積分。
	Vector3 addPos = m_moveSpeed;
	addPos.Scale(1.0f/60.0f);
	nextPosition.Add(addPos);

	//XZ平面での衝突検出と衝突解決を行う。
	{
		int loopCount = 0;
		while (true) {
			//現在の座標から次の移動先へ向かうベクトルを求める。
			Vector3 addPos;
			addPos.Subtract(nextPosition, transform->position);
			Vector3 addPosXZ = addPos;
			addPosXZ.y = 0.0f;
			if (addPosXZ.Length() < FLT_EPSILON) {
				//XZ平面で動きがないので調べる必要なし。
				//FLT_EPSILONは1より大きい、最小の値との差分を表す定数。
				//とても小さい値のことです。
				break;
			}
			//カプセルコライダーの中心座標 + 0.2の座標をposTmpに求める。
			Vector3 posTmp = transform->position;
			posTmp.y += m_height * 0.5f + m_radius + 0.2f;
			//レイを作成。
			btTransform start, end;
			start.setIdentity();
			end.setIdentity();
			//始点はカプセルコライダーの中心座標 + 0.2の座標をposTmpに求める。
			start.setOrigin(btVector3(posTmp.x, posTmp.y, posTmp.z));
			//終点は次の移動先。XZ平面での衝突を調べるので、yはposTmp.yを設定する。
			end.setOrigin(btVector3(nextPosition.x, posTmp.y, nextPosition.z));

			SweepResultWall callback;
			callback.me = m_rigidBody->GetCollisonObj();
			callback.startPos = posTmp;
			//衝突検出。
			PhysicsWorld::Instance()->ConvexSweepTest((const btConvexShape*)m_collider->GetBody(), start, end, callback);

			if (callback.isHit) {
				//当たった。
				//壁。
				Vector3 vT0, vT1;
				//XZ平面上での移動後の座標をvT0に、交点の座標をvT1に設定する。
				vT0.Set(nextPosition.x, 0.0f, nextPosition.z);
				vT1.Set(callback.hitPos.x, 0.0f, callback.hitPos.z);
				//めり込みが発生している移動ベクトルを求める。
				Vector3 vMerikomi;
				vMerikomi.Subtract(vT0, vT1);
				//XZ平面での衝突した壁の法線を求める。。
				Vector3 hitNormalXZ = callback.hitNormal;
				hitNormalXZ.y = 0.0f;
				hitNormalXZ.Normalize();
				//めり込みベクトルを壁の法線に射影する。
				float fT0 = hitNormalXZ.Dot(vMerikomi);
				//押し戻し返すベクトルを求める。
				//押し返すベクトルは壁の法線に射影されためり込みベクトル+半径。
				Vector3 vOffset;
				vOffset = hitNormalXZ;
				vOffset.Scale(-fT0 + m_radius);
				nextPosition.Add(vOffset);
			}
			else {
				//どことも当たらないので終わり。
				break;
			}
			loopCount++;
			if (loopCount == 5) {
				break;
			}
		}
	}
	//XZの移動は確定。
	transform->position.x = nextPosition.x;
	transform->position.z = nextPosition.z;
	//下方向を調べる。
	{
		Vector3 addPos;
		addPos.Subtract(nextPosition, transform->position);

		transform->position = nextPosition;	//移動の仮確定。
									//レイを作成する。
		btTransform start, end;
		start.setIdentity();
		end.setIdentity();
		//始点はカプセルコライダーの中心。
		start.setOrigin(btVector3(transform->position.x, transform->position.y + m_height * 0.5f + m_radius, transform->position.z));
		//終点は地面上にいない場合は1m下を見る。
		//地面上にいなくてジャンプで上昇中の場合は上昇量の0.01倍下を見る。
		//地面上にいなくて降下中の場合はそのまま落下先を調べる。
		Vector3 endPos;
		endPos.Set(start.getOrigin().x(), start.getOrigin().y(), start.getOrigin().z());
		if (m_isOnGround == false) {
			if (addPos.y > 0.0f) {
				//ジャンプ中とかで上昇中。
				//上昇中でもXZに移動した結果めり込んでいる可能性があるので下を調べる。
				endPos.y -= addPos.y * 0.01f;
			}
			else {
				//落下している場合はそのまま下を調べる。
				endPos.y += addPos.y;
			}
		}
		else {
			//地面上にいない場合は1m下を見る。
			endPos.y -= 1.0f;
		}
		end.setOrigin(btVector3(endPos.x, endPos.y, endPos.z));
		SweepResultGround callback;
		callback.me = m_rigidBody->GetCollisonObj();
		callback.startPos.Set(start.getOrigin().x(), start.getOrigin().y(), start.getOrigin().z());
		//衝突検出。
		PhysicsWorld::Instance()->ConvexSweepTest((const btConvexShape*)m_collider->GetBody(), start, end, callback);
		if (callback.isHit) {
			//当たった。
			Vector3 Circle;
			float x = 0.0f;
			float offset = 0.0f;	//押し戻す量。
			Circle = Vector3::zero;

			Circle = transform->position;
			Circle.y = callback.hitPos.y;//円の中心
			Vector3 v;
			v.Subtract(Circle, callback.hitPos);
			x = v.Length();//物体の角とプレイヤーの間の横幅の距離が求まる。

			offset = sqrt(max(0.0f, m_radius*m_radius - x*x));//yの平方根を求める。

			m_moveSpeed.y = 0.0f;
			m_isJump = false;
			m_isOnGround = true;
			nextPosition.y = callback.hitPos.y + offset - m_radius;
		}
		else {
			//地面上にいない。
			m_isOnGround = false;
		}
	}
	//移動確定。
	transform->position = nextPosition;
	btRigidBody* btBody = (btRigidBody*)m_rigidBody->GetCollisonObj();
	//剛体を動かす。
	btBody->setActivationState(DISABLE_DEACTIVATION);
	btTransform& trans = btBody->getWorldTransform();
	//剛体の位置を更新。
	trans.setOrigin(btVector3(transform->position.x, transform->position.y, transform->position.z));
	//@todo 未対応。 trans.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z));
}
	/*!
	* @brief	死亡したことを通知。
	*/
void CCharacterController::RemoveRigidBoby()
{
	PhysicsWorld::Instance()->RemoveRigidBody(m_rigidBody);
}