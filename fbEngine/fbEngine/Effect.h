
#pragma once

class Effect
{
public:
	Effect(char* name)
	{
		_FileName = new char[strlen(name) + 1];
		//ファイル名コピー
		strcpy(_FileName, name);
	}

	void SetEffect(ID3DXEffect* e)
	{
		_Effect = e;
	}

	ID3DXEffect* GetEffect()
	{
		return _Effect;
	}

	void Begin(UINT *pPasses, DWORD Flags)
	{
		_Effect->Begin(pPasses, Flags);
	}

	void BeginPass(UINT Pass)
	{
		_Effect->BeginPass(Pass);
	}

	void SetTechnique(LPCSTR tecname)
	{
		_Effect->SetTechnique(tecname);
	}

	void SetTexture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture)
	{
		if (FAILED(_Effect->SetTexture(hParameter, pTexture)))
		{
			ErrorMessage(hParameter);
			abort();
		}
	}

	void SetMatrix(LPCSTR hParameter, D3DXMATRIX* mat)
	{
		if (FAILED(_Effect->SetMatrix(hParameter, mat)))
		{
			ErrorMessage(hParameter);
			abort();
		}
	}

	void SetMatrixArray(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count)
	{
		if (FAILED(_Effect->SetMatrixArray(hParameter, pMatrix, Count)))
		{
			ErrorMessage(hParameter);
			abort();
		}
	}

	void SetVector(D3DXHANDLE hParameter, D3DXVECTOR4* pVector)
	{
		if (FAILED(_Effect->SetVector(hParameter, pVector)))
		{
			ErrorMessage(hParameter);
			abort();
		}
	}

	void SetVectorArray(D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count)
	{
		if (FAILED(_Effect->SetVectorArray(hParameter, pVector, Count)))
		{
			ErrorMessage(hParameter);
			abort();
		}
	}

	void SetValue(D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes)
	{
		if (FAILED(_Effect->SetValue(hParameter, pData, Bytes)))
		{
			ErrorMessage(hParameter);
			abort();
		}
	}

	void GetValue(D3DXHANDLE hParameter, LPVOID pData, UINT Bytes)
	{
		if (FAILED(_Effect->GetValue(hParameter, pData, Bytes)))
		{
			ErrorMessage(hParameter);
			abort();
		}
	}

	void SetInt(D3DXHANDLE hParameter, INT n)
	{
		if (FAILED(_Effect->SetInt(hParameter, n)))
		{
			ErrorMessage(hParameter);
			abort();
		}
	}

	void SetBool(D3DXHANDLE hParameter, BOOL b)
	{
		if (FAILED(_Effect->SetBool(hParameter, b)))
		{
			ErrorMessage(hParameter);
			abort();
		}
	}

	void SetFloat(D3DXHANDLE hParameter, FLOAT f)
	{
		if (FAILED(_Effect->SetFloat(hParameter, f)))
		{
			ErrorMessage(hParameter);
			abort();
		}
	}

	void CommitChanges()
	{
		_Effect->CommitChanges();
	}

	void EndPass()
	{
		_Effect->EndPass();	
	}

	void End()
	{
		_Effect->End();
	}

	void Release()
	{
		if (_Effect != NULL) {
			_Effect->Release();
			//ポインタ解放
			SAFE_DELETE(_Effect);
		}
	}
private:
	//エラーを出す処理
	void ErrorMessage(D3DXHANDLE hParameter)
	{
		//メモリを多く確保しておく
		char mess[256];

		strcpy(mess, "シェーダファイル名：\"");
		strcat(mess, _FileName);
		strcat(mess, "\"\n");
		strcat(mess, "パラメータ名：\"");
		strcat(mess, hParameter);
		strcat(mess, "\"\nは存在しないまたは無効なパラメータです。\n名前ちゃんと確認してみ？");
		//失敗したら
		MessageBoxA(NULL, mess, "EffectError", MB_OK);
	}
private:
	ID3DXEffect* _Effect;	//Effectへのポインタ
	char* _FileName;		//シェーダファイル名
};