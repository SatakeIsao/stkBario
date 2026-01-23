/**
 * Application.cpp
 * アプリケーション制御用クラス群
 * 更新処理やレンダー処理の管理
 */
#include "stdafx.h"
#include "Application.h"
#include "Game.h"


namespace app
{
	Application::Application()
	{
	}


	Application::~Application()
	{
	}


	void Application::Initialize(HWND hwnd)
	{
		// k2EngineLowの初期化。
		k2EngineLow_ = new K2EngineLow();
		k2EngineLow_->Init(hwnd, FRAME_BUFFER_W, FRAME_BUFFER_H);
		g_camera3D->SetPosition({ 0.0f, 100.0f, -200.0f });
		g_camera3D->SetTarget({ 0.0f, 50.0f, 0.0f });

		PhysicsWorld::Initialize();

		//シーンライト初期化
		sceneLight_.Init();
		g_sceneLight = &sceneLight_;
		// レンダリングエンジン
		renderingEngine_.Init();
		g_renderingEngine = &renderingEngine_;

		// コリジョンオブジェクトマネージャー
		// TODO: あとで消す
		CollisionObjectManager m_collisionObjectManager;
		g_collisionObjectManager = &m_collisionObjectManager;

		// ここから下でゲーム固有の初期化処理を行う。
		game_ = NewGO<Game>(static_cast<uint8_t>(ObjectPriority::Default));
	}


	void Application::BeginFrame()
	{
		k2EngineLow_->BeginFrame();
	}


	void Application::Update()
	{
		for (auto* pad : g_pad) {
			pad->Update();
		}
		g_soundEngine->Update();
		
		GameObjectManager::GetInstance()->ExecuteUpdate();
		
		//物理エンジンのアップデートを呼び出す。
		PhysicsWorld::Get().Update(1.0f / 60.0f);	// 固定値で更新

		// エフェクトエンジンの更新。
		EffectEngine::GetInstance()->Update(g_gameTime->GetFrameDeltaTime());
	}


	void Application::Render(RenderContext& rc)
	{
		// ゲームオブジェクトマネージャーの描画処理を実行。
		GameObjectManager::GetInstance()->ExecuteRender(rc);


		//ライトの更新処理(この間でする)
		g_sceneLight->Update();

		
		//レンダリングエンジンの描画処理
		g_renderingEngine->Execute(rc);
	}


	void Application::RenderDebug(RenderContext& rc)
	{
#if defined(K2_DEBUG)
		//当たり判定描画処理を実行。
		PhysicsWorld::Get().DebubDrawWorld(rc);
#endif
	}


	void Application::EndFrame()
	{
		k2EngineLow_->EndFrame();
	}


	void Application::Finalize()
	{
		DeleteGO(game_);

		PhysicsWorld::Finalize();

		delete k2EngineLow_;
	}
}