// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。
// Windows ヘッダー ファイル:
#include <windows.h>
// C ランタイム ヘッダー ファイル
#include <stdlib.h>
#include <malloc.h>
#include <tchar.h>
#include <memory>


// TODO: プログラムに必要な追加ヘッダーをここで参照してください
//ダイレクトＸ
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include <d3d9.h>
#include <d3dx9.h>
//XAudio
#pragma comment(lib, "X3daudio.lib")
#include <X3DAudio.h>
#include <XAudio2.h>
#include <XAudio2fx.h>
//DirectShow
#pragma comment(lib, "Strmiids.lib")
#pragma comment(lib, "Quartz.lib")
#include <dshow.h>
#include <strmif.h>
#include <objbase.h>
#include <vmr9.h>
//コンパイラを通すために必要
#define _WIN32_DCOM
//XInput
#pragma comment(lib, "Xinput.lib")
#include <XInput.h>
//DirectInput
// DirectInputバージョンの宣言
#define	DIRECTINPUT_VERSION	0x0800
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <dinput.h>

using namespace std;

#include <vector>
#include <map>
#include <list>
#include <typeinfo>

#define SAFE_RELEASE(p)			{ if(p) { (p)->Release(); (p) = NULL; } }	//インスタンスリリース
#define SAFE_DELETE_ARRAY(p)	{ if(p!=nullptr) { delete[] (p);   (p) = nullptr; } }	//インスタンス配列削除
#define SAFE_DELETE(p)			{ if(p!=nullptr) { delete (p);     (p) = nullptr; } }	//インスタンス削除

//p回繰り返す
#define FOR(p) for(short i = 0;i < (int)p;i++)
//シングルトン取得
#define INSTANCE(p) p::Instance()

#define FontSize 300.0f

#include "System.h"
#include "Noncopyable.h"
#include "Time.h"
#include "Random.h"
#include "Vector.h"
#include "Color.h"
#include "Support.h"
#include "Transform.h"
#include "GameObject.h"

#include "SoundEngine.h"
#include "GraphicsDevice.h"		//Direct3D管理ヘッダインクルード
#include "GameObjectManager.h"
#include "SceneManager.h"
#include "EffectManager.h"
#include "TextureManager.h"
#include "SkinModelManager.h"
#include "RenderTargetManager.h"
#include "InputManager.h"
//物理系
//このなかでほかのやつらをインクルードしている。
#include "../bulletPhysics/src/btBulletDynamicsCommon.h"
#include "../bulletPhysics/src/BulletCollision/CollisionDispatch/btGhostObject.h"
#include "Physics.h"

extern HWND g_MainWindow;	//ウィンドウハンドル
extern Vector2 g_WindowSize;

//キー定義
#define VK_A 0x41
#define VK_B 0x42
#define VK_C 0x43
#define VK_D 0x44
#define VK_E 0x45
#define VK_F 0x46
#define VK_G 0x47
#define VK_H 0x48
#define VK_I 0x49
#define VK_J 0x4A
#define VK_K 0x4B
#define VK_L 0x4C
#define VK_M 0x4D
#define VK_N 0x4E
#define VK_O 0x4F
#define VK_P 0x50
#define VK_Q 0x51
#define VK_R 0x52
#define VK_S 0x53
#define VK_T 0x54
#define VK_U 0x55
#define VK_V 0x56
#define VK_W 0x57
#define VK_X 0x58
#define VK_Y 0x59
#define VK_Z 0x5A