#pragma once
#include "Collider.h"

class SkinModel;

/*!
 * @brief	メッシュコライダー。
 */
class MeshCollider : public Collider
{
public:
	/*!
	 * @brief	コンストラクタ。
	 */
	MeshCollider(GameObject* g, Transform* t);
	/*!
	 * @brief	デストラクタ。
	 */
	~MeshCollider();
	/*!
	 * @brief	CSkinModelDataからメッシュコライダーを生成。
	 *@param[in]	model		スキンモデル。
	 */
	void Create(SkinModel* model);
	btCollisionShape* GetBody() override
	{
		return meshShape;
	}
private:
	typedef std::vector<Vector3>		VertexBuffer;		//頂点バッファ。
	typedef std::vector<unsigned int>	IndexBuffer;		//インデックスバッファ。
	std::vector<VertexBuffer*>		vertexBufferArray;		//頂点バッファの配列。
	std::vector<IndexBuffer*>		indexBufferArray;		//インデックスバッファの配列。
	btBvhTriangleMeshShape*			meshShape;				//メッシュ形状。
	btTriangleIndexVertexArray*		stridingMeshInterface;
};

