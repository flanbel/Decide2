#pragma once

//プリミティブタイプ.
namespace fbEngine
{
	enum class PRIMITIVETYPE {
		TRIANGLELIST = D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST,	//トライアングルリスト。
		TRIANGLESTRIP = D3DPRIMITIVETYPE::D3DPT_TRIANGLESTRIP,  //トライアングルストリップ。
	};
}

//頂点を扱う
class Vertex:Noncopyable
{
public:
	~Vertex();
	//初期化
	// 描画方法
	// 頂点数
	void Initialize(fbEngine::PRIMITIVETYPE type,int vertexnum);
	//頂点バッファの作成
	// vertexinfo 頂点情報の配列
	// vertexnum　頂点数
	// vertexsize 頂点サイズ
	void CreateVertexBuffer(void* vertexinfo,int vertexnum,int vertexsize, D3DVERTEXELEMENT9 element);
	//頂点バッファとエレメントからデコレーション作成
	void CreateDeclaration();
	//描画
	void DrawPrimitive();
private:
	//描画方法
	D3DPRIMITIVETYPE _PrimitiveType;
	//ポリゴン数
	int _PolygonNum;
	//頂点バイト数
	vector<int> _VertexSize;
	//頂点バッファ
	vector<IDirect3DVertexBuffer9*> _VertexBuffer;
	//頂点宣言
	vector<D3DVERTEXELEMENT9> _Element;
	//デコレーション
	IDirect3DVertexDeclaration9 *_Declaration;
};