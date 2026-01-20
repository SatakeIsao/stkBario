#include "stdafx.h"
#include "SceneManager.h"

#include "StartupScene.h"

#if defined(APP_DEBUG)
#include "BootScene.h"
#include "DebugScene.h"
#endif // APP_DEBUG

#include "core/Fade.h"


SceneManager* SceneManager::m_instance = nullptr;	// 初期化


SceneManager::SceneManager()
{
	AddSceneMap<StartupScene>();

#if defined(APP_DEBUG)
	AddSceneMap<BootScene>();
	AddSceneMap<DebugScene>();
#endif // APP_DEBUG
}


SceneManager::~SceneManager()
{
}


void SceneManager::Update()
{
	if (m_currentScene) {
		m_currentScene->Update();
		if (m_currentScene->RequestScene(m_nextSceneId, m_waitTime)) {
			delete m_currentScene;
			m_currentScene = nullptr;

			Fade::Get().Enable();
		}
	}

	if (m_nextSceneId != INVALID_SCENE_ID) {
		m_elapsedTime += g_gameTime->GetFrameDeltaTime();
		if (m_elapsedTime >= m_waitTime) {
			CreateScene(m_nextSceneId);
			m_waitTime = 0.0f;
			m_elapsedTime = 0.0f;
			m_nextSceneId = INVALID_SCENE_ID;

			Fade::Get().Disable();
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
#if defined(APP_DEBUG)
	SceneManager::Get().CreateScene(DebugScene::ID());
#else
	SceneManager::Get().CreateScene(StartupScene::ID());
#endif // APP_DEBUG
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