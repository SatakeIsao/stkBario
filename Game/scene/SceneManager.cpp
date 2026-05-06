#include "stdafx.h"
#include "SceneManager.h"

#include "StartupScene.h"
#include "TitleScene.h"
#include "BootScene.h"
#include "BattleScene.h"
#include "GameOverScene.h"
#include "GameClearScene.h"
#include "core/Fade.h"
#include "sound/SoundManager.h"




SceneManager* SceneManager::m_instance = nullptr;	// 初期化


SceneManager::SceneManager()
{
	AddSceneMap<StartupScene>();
	AddSceneMap<TitleScene>();
	//AddSceneMap<BootScene>();
	AddSceneMap<BattleScene>();
	AddSceneMap<GameOverScene>();
	AddSceneMap<GameClearScene>();
}


SceneManager::~SceneManager()
{
}


void SceneManager::Update()
{
	// 通常のシーン更新（遷移リクエストが来ていない時だけ更新する）
	if (m_currentScene && nextSceneId_ == INVALID_SCENE_ID) {

		m_currentScene->Update();

		if (m_currentScene->RequestScene(nextSceneId_, m_waitTime)) {
			// シーン遷移リクエストが来た！

			// 次に遷移するシーンがゲームオーバーかどうかで演出を変える
			if (nextSceneId_ == GameOverScene::ID()) {
				// ゲームオーバーならスライム演出
				Fade::Get().Enable(FadeState::SlimeAnim);
				app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::SlimeAnim));
			}
			else {
				// それ以外はBロゴ演出
				Fade::Get().Enable(FadeState::BIconAnim);
				app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::BIconAmim));
			}
		}
	}

	// シーン遷移処理中
	if (nextSceneId_ != INVALID_SCENE_ID) {

		// スライム演出が完了し、画面が真っ黒になったかチェック
		if (Fade::Get().IsFadedOut()) {

			// 画面が黒くなったので、ここで初めて古いシーンを削除する
			if (m_currentScene != nullptr) {
				delete m_currentScene;
				m_currentScene = nullptr;
			}

			// Bロゴが回転している間のロード時間をカウント
			m_elapsedTime += g_gameTime->GetFrameDeltaTime();
			if (m_elapsedTime >= m_waitTime) {

				// 時間が来たら新しいシーンを生成
				CreateScene(nextSceneId_);

				if (nextSceneId_ == GameOverScene::ID()) {
					Fade::Get().StartSlimeFadeIn();
				}
				else {
					Fade::Get().StartFadeIn();
				}

				m_waitTime = 0.0f;
				m_elapsedTime = 0.0f;
				nextSceneId_ = INVALID_SCENE_ID;
			}
		}
	}
}


void SceneManager::Render(RenderContext& rc)
{
	if (m_currentScene) {
		m_currentScene->Render(rc);
	}
}


void SceneManager::CreateScene(const uint32_t id)
{
	auto it = m_sceneMap.find(id);
	if (it == m_sceneMap.end()) {
		K2_ASSERT(false, "新規シーンが追加されていません。\n");
	}
	auto& createSceneFunc = it->second;
	m_currentScene = createSceneFunc();
	m_currentScene->Start();
}




/*****************************************************/


SceneManagerObject::SceneManagerObject()
{
	SceneManager::Initialize();
}


SceneManagerObject::~SceneManagerObject()
{
	SceneManager::Finalize();
}


bool SceneManagerObject::Start()
{
	// 最初のシーンを設定
	/** デバックテスト */
	SceneManager::Get().CreateScene(TitleScene::ID());
	// SceneManager::Get().CreateScene(BattleScene::ID());
	// SceneManager::Get().CreateScene(GameClearScene::ID());
	return true;
}


void SceneManagerObject::Update()
{
	SceneManager::Get().Update();
}


void SceneManagerObject::Render(RenderContext& rc)
{
	SceneManager::Get().Render(rc);
}