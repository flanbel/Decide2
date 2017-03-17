#include "stdafx.h"
#include "Vertex.h"

Vertex::~Vertex()
{
	//シェアードポインタなので参照がなくなったら解放されるはず！
	_VertexBuffer.clear();
	SAFE_DELETE(_Declaration);
}

void Vertex::Initialize(fbEngine::PRIMITIVETYPE type, int vertexnum)
{
	switch (type)
	{
	case fbEngine::PRIMITIVETYPE::TRIANGLELIST:
		_PolygonNum = vertexnum / 3;
		break;
	case fbEngine::PRIMITIVETYPE::TRIANGLESTRIP:
		_PolygonNum = vertexnum - 2;
		break;
	default:
		return;
		break;
	}
	_PrimitiveType = (D3DPRIMITIVETYPE)type;
}

void Vertex::CreateVertexBuffer(void * vertexinfo, int vertexnum, int vertexsize, D3DVERTEXELEMENT9 element)
{
	int Vnum;
	switch (_PrimitiveType)
	{
	case (D3DPRIMITIVETYPE)fbEngine::PRIMITIVETYPE::TRIANGLELIST:
		Vnum = _PolygonNum * 3;
		break;
	case (D3DPRIMITIVETYPE)fbEngine::PRIMITIVETYPE::TRIANGLESTRIP:
		Vnum = _PolygonNum + 2;
		break;
	}

	//FB_ASSERT(vertexnum <= Vnum, "頂点数足りてませんよ。");

	IDirect3DVertexBuffer9* buffer;
	int length = vertexsize * vertexnum;
	//ポジションの頂点バッファ作成
	(*graphicsDevice()).CreateVertexBuffer(
		length,						//頂点バッファのサイズ(サイズ*頂点数)
		D3DUSAGE_WRITEONLY,
		0,							//固定機能ではないので非ＦＶＦ頂点で作成する
		D3DPOOL_MANAGED,
		&buffer,					//頂点バッファ
		NULL
	);

	//頂点バッファの先頭アドレスへの参照を格納する。
	VOID* pVertices;
	//アドレス取得
	buffer->Lock(
		0,						//ロックする先頭
		length,					//ロックする量
		(void**)&pVertices,		//先頭アドレス
		D3DLOCK_DISCARD			//フラグ
	);
	//アドレスの参照へ頂点定義をコピー
	memcpy(pVertices, vertexinfo, length);
	//ロック解除
	buffer->Unlock();

	//頂点サイズ保持
	_VertexSize.push_back(vertexsize);
	//ストリーム番号
	element.Stream = _VertexBuffer.size();
	//エレメント保持
	_Element.push_back(element);
	//頂点バッファ保持
	_VertexBuffer.push_back(buffer);
}

void Vertex::CreateDeclaration()
{
	//エレメントの配列作成
	D3DVERTEXELEMENT9* elements = new D3DVERTEXELEMENT9[_Element.size() + 1];
	unsigned short i = 0;
	for (i = 0; i < _Element.size(); i++)
	{
		elements[i] = _Element[i];
	}
	//締め
	elements[i] = D3DDECL_END();
	//デコレーション作成
	(*graphicsDevice()).CreateVertexDeclaration(elements, &_Declaration);
	//もういらないから削除
	SAFE_DELETE_ARRAY(elements);
}

void Vertex::DrawPrimitive()
{
	//デコレーション設定
	(*graphicsDevice()).SetVertexDeclaration(_Declaration);
	//ストリームセット
	for (unsigned short i = 0; i < _VertexBuffer.size(); i++)
	{
		(*graphicsDevice()).SetStreamSource(i, _VertexBuffer[i], 0, _VertexSize[i]);
	}
	//描画
	(*graphicsDevice()).DrawPrimitive(_PrimitiveType, 0, _PolygonNum);
}
