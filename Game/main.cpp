#include "stdafx.h"
#include "system/system.h"
#include "ModelRender.h"
#include "Game.h"


// K2EngineLowのグローバルアクセスポイント。
K2EngineLow* g_k2EngineLow = nullptr;

/// <summary>
/// メイン関数
/// </summary>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	// ゲームの初期化。
	InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Bario"));
	
	// k2EngineLowの初期化。
	g_k2EngineLow = new K2EngineLow();
	g_k2EngineLow->Init(g_hWnd, FRAME_BUFFER_W, FRAME_BUFFER_H);
	g_camera3D->SetPosition({ 0.0f, 100.0f, -200.0f });
	g_camera3D->SetTarget({ 0.0f, 50.0f, 0.0f });

	//追加したコード
	SceneLight m_sceneLight;
	m_sceneLight.Init();
	g_sceneLight = &m_sceneLight;
	//
	RenderingEngine m_renderingEngine;
	m_renderingEngine.Init();
	g_renderingEngine = &m_renderingEngine;

	CollisionObjectManager m_collisionObjectManager;
	g_collisionObjectManager = &m_collisionObjectManager;

	//Fade* fade = NewGO<Fade>(0);
	Game* game = NewGO<Game>(0);

	// ここからゲームループ。
	while (DispatchWindowMessage())
	{
		//追加したコード
		auto& renderContext = g_graphicsEngine->GetRenderContext();
		//
		
		// フレームの開始時に呼び出す必要がある処理を実行
		g_k2EngineLow->BeginFrame();

		// ゲームオブジェクトマネージャーの更新処理を呼び出す。
		g_k2EngineLow->ExecuteUpdate();

		// ゲームオブジェクトマネージャーの描画処理を呼び出す。
		g_k2EngineLow->ExecuteRender();

		//追加したコード

		//ライトの更新処理
		m_sceneLight.Update();

		//レンダリングエンジンの描画処理
		m_renderingEngine.Execute(renderContext);

		// デバッグ描画処理を実行する。
		g_k2EngineLow->DebubDrawWorld();

		// フレームの終了時に呼び出す必要がある処理を実行。
		g_k2EngineLow->EndFrame();
	}

	delete g_k2EngineLow;

	return 0;
}

