// Decide.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "Decide.h"
#include "fbEngine/Game.h"
#include "GameSystem.h"

#define MAX_LOADSTRING 100

// グローバル変数:
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void InitializeGame();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化しています。
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DECIDE, szWindowClass, MAX_LOADSTRING);

    MyRegisterClass(hInstance,
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DECIDE)),
		LoadCursor(nullptr, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		MAKEINTRESOURCEW(IDC_DECIDE),
		szWindowClass,
		LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL))
	);

	g_MainWindow = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, WindowW, WindowH, nullptr, nullptr, hInstance, nullptr);

	D3DXVECTOR2 WindowSize, ClientSize, Diff;
	RECT window, client;

	//ウィンドウエリアサイズ
	GetWindowRect(g_MainWindow, &window);
	WindowSize.x = (float)(window.right - window.left);
	WindowSize.y = (float)(window.bottom - window.top);
	//クライアントエリアサイズ
	GetClientRect(g_MainWindow, &client);
	ClientSize.x = (float)(client.right - client.left);
	ClientSize.y = (float)(client.bottom - client.top);
	//差を求める
	Diff.x = WindowSize.x - ClientSize.x;
	Diff.y = WindowSize.y - ClientSize.y;
	//ゲームウィンドウの大きさ調整

	g_WindowSize = Vector2(WindowW,WindowH);

	SetWindowPos(g_MainWindow, HWND_TOP, 0, 0, WindowW + (int)Diff.x, WindowH + (int)Diff.y, (SWP_NOZORDER));

    // アプリケーションの初期化を実行します:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	InitializeGame();

    return GameStart(hInstance, nCmdShow, IDC_DECIDE);
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウの描画
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

#include "TitleScene.h"
#include "BattleScene.h"
#include "CharaSelectScene.h"
#include "ResultScene.h"
void InitializeGame()
{
	INSTANCE(SoundEngine)->Init();

	INSTANCE(SceneManager)->Add<TitleScene>();
	INSTANCE(SceneManager)->Add<CharaSelectScene>();
	INSTANCE(SceneManager)->Add<BattleScene>();
	INSTANCE(SceneManager)->Add<ResultScene>();
	INSTANCE(SceneManager)->ChangeScene("TitleScene");
}