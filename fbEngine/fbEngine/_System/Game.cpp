#include "Game.h"

CGraphicsDevice g_graphicsDevice;
HINSTANCE hInst;                                // 現在のインターフェイス
HWND g_MainWindow;
Vector2 g_WindowSize;

//コールバック関数
//LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
//INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void Initialize();
void Update();
void Draw();

//アプリケーションの始まり
int GameStart(_In_ HINSTANCE hInstance,
	_In_ int nCmdShow,
	unsigned int IDC)
{
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC));

    MSG msg;

	Initialize();

	// メイン メッセージ ループ:
	do
	{
		//メッセージが存在するか検証
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);		//仮想キーメッセージを文字メッセージに変換
			DispatchMessage(&msg);		//メッセージをウィンドウプロシージャに送出
		}
		else
		{
			Update();
			Draw();
		}

	} while (msg.message != WM_QUIT);

    return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance,
	UINT        style,
	WNDPROC     lpfnWndProc,
	int         cbClsExtra,
	int         cbWndExtra,
	HICON       hIcon,
	HCURSOR     hCursor,
	HBRUSH      hbrBackground,
	LPCWSTR     lpszMenuName,
	LPCWSTR     lpszClassName,
	HICON       hIconSm)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = style;//CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = lpfnWndProc;
	wcex.cbClsExtra = cbClsExtra;
	wcex.cbWndExtra = cbWndExtra;
	wcex.hInstance = hInstance;
	wcex.hIcon = hIcon;//LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FBENGIN));
	wcex.hCursor = hCursor;//LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = hbrBackground;//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = lpszMenuName;//MAKEINTRESOURCEW(IDC_FBENGIN);
	wcex.lpszClassName = lpszClassName;// szWindowClass;
	wcex.hIconSm = hIconSm;//LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

   if (!g_MainWindow)
   {
      return FALSE;
   }
   //デバイス初期化
   graphicsDevice().InitD3d(g_MainWindow);
   //ウィンドウ表示
   ShowWindow(g_MainWindow, nCmdShow);
   //ウィンドウ更新
   UpdateWindow(g_MainWindow);

   return TRUE;
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
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    switch (message)
//    {
//    case WM_COMMAND:
//        {
//            int wmId = LOWORD(wParam);
//            // 選択されたメニューの解析:
//           /* switch (wmId)
//            {
//            case IDM_ABOUT:
//                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
//                break;
//            case IDM_EXIT:
//                DestroyWindow(hWnd);
//                break;
//            default:
//                return DefWindowProc(hWnd, message, wParam, lParam);
//            }*/
//        }
//        break;
//    case WM_PAINT:
//        {
//            PAINTSTRUCT ps;
//            HDC hdc = BeginPaint(hWnd, &ps);
//            // TODO: HDC を使用する描画コードをここに追加してください...
//            EndPaint(hWnd, &ps);
//        }
//        break;
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        break;
//    default:
//        return DefWindowProc(hWnd, message, wParam, lParam);
//    }
//    return 0;
//}
//
//// バージョン情報ボックスのメッセージ ハンドラーです。
//INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    UNREFERENCED_PARAMETER(lParam);
//    switch (message)
//    {
//    case WM_INITDIALOG:
//        return (INT_PTR)TRUE;
//
//    case WM_COMMAND:
//        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
//        {
//            EndDialog(hDlg, LOWORD(wParam));
//            return (INT_PTR)TRUE;
//        }
//        break;
//    }
//    return (INT_PTR)FALSE;
//}

void Initialize()
{
	INSTANCE(InputManager)->Initialize();
	INSTANCE(PhysicsWorld)->Start();
	Random::Seed(timeGetTime());
}

void Update() 
{
	Time::Update();
	INSTANCE(SoundEngine)->Update();
	INSTANCE(InputManager)->Update();
	INSTANCE(SceneManager)->UpdateScene();
}

void Draw() 
{
	//画面を白色でクリア
	(*graphicsDevice()).Clear(0,
		NULL,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
		D3DCOLOR_XRGB(255, 255, 255),
		1.0f,
		0);

	//シーン開始
	if (SUCCEEDED((*graphicsDevice()).BeginScene()))
	{
		INSTANCE(SceneManager)->DrawScene();
		//シーン終了
		(*graphicsDevice()).EndScene();
	}
	(*graphicsDevice()).Present(NULL, NULL, NULL, NULL);
}