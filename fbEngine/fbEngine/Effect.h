
#pragma once

class Effect
{
public:	
	ID3DXEffect* GetEffect()
	{
		return effect;
	}

	void SetTexture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture)
	{
		effect->SetTexture(hParameter, pTexture);
	}

	void SetEffect(ID3DXEffect* e)
	{
		effect = e;
	}

	void Begin(UINT *pPasses, DWORD Flags)
	{
		effect->Begin(pPasses, Flags);
	}

	void BeginPass(UINT Pass)
	{
		effect->BeginPass(Pass);
	}

	void SetTechnique(LPCSTR tecname)
	{
		effect->SetTechnique(tecname);
	}

	void SetMatrix(LPCSTR parameterName, D3DXMATRIX* mat)
	{
		effect->SetMatrix(parameterName, mat);
	}

	void SetMatrixArray(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count)
	{
		effect->SetMatrixArray(hParameter, pMatrix, Count);
	}

	void SetVector(D3DXHANDLE hParameter, D3DXVECTOR4* pVector)
	{
		effect->SetVector(hParameter, pVector);
	}

	void SetVectorArray(D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count)
	{
		effect->SetVectorArray(hParameter, pVector, Count);
	}

	void SetValue(D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes)
	{
		effect->SetValue(hParameter, pData, Bytes);
	}

	void GetValue(D3DXHANDLE hParameter, LPVOID pData, UINT Bytes)
	{
		effect->GetValue(hParameter, pData, Bytes);
	}

	void SetInt(D3DXHANDLE hParameter, INT n)
	{
		effect->SetInt(hParameter, n);
	}

	void SetBool(D3DXHANDLE hParameter, BOOL b)
	{
		effect->SetBool(hParameter, b);
	}

	void SetFloat(D3DXHANDLE hParameter, FLOAT f)
	{
		effect->SetFloat(hParameter,f);
	}

	void CommitChanges()
	{
		effect->CommitChanges();
	}

	void EndPass()
	{
		effect->EndPass();	
	}

	void End()
	{
		effect->End();
	}

	void Release()
	{
		if (effect != NULL) {
			effect->Release();
			//ポインタ解放
			SAFE_DELETE(effect);
		}
	}

	ID3DXEffect* GetD3DXEffect()
	{
		return effect;
	}

private:
	ID3DXEffect* effect;	//Effectへのポインタ

};