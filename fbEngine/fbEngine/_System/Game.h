#pragma once

// グローバル変数:
extern HINSTANCE hInst;                                // 現在のインターフェイス

//ゲームスタート
extern int GameStart(_In_ HINSTANCE hInstance,
	_In_ int nCmdShow,
	unsigned int IDC);

//ウィンドウクラス作成
extern ATOM MyRegisterClass(HINSTANCE hInstance,
	UINT        style,
WNDPROC     lpfnWndProc,
int         cbClsExtra,
int         cbWndExtra,
HICON       hIcon,
HCURSOR     hCursor,
HBRUSH      hbrBackground,
LPCWSTR     lpszMenuName,
LPCWSTR     lpszClassName,
HICON       hIconSm);
//インスタンス初期化
extern BOOL InitInstance(HINSTANCE h_Instance,
	int cmd);