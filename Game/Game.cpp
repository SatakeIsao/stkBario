#include "stdafx.h"
#include "Game.h"

#include "core/Fade.h"
#include "core/ParameterManager.h"
#include "scene/SceneManager.h"
#include "sound/SoundManager.h"


Game::Game()
{
}

Game::~Game()
{
	DeleteGO(m_sceneManager);
	DeleteGO(m_fadeObject);
	app::SoundManager::Finalize();
	app::core::ParameterManager::Finalize();
}

bool Game::Start()
{
	// パラメーター管理生成
	app::core::ParameterManager::Initialize();
	// サウンド管理生成
	app::SoundManager::Initialize();
	// Fade処理生成
	m_fadeObject = NewGO<FadeObject>(static_cast<uint8_t>(ObjectPriority::Fade));
	// シーン管理生成
	m_sceneManager = NewGO<SceneManagerObject>(static_cast<uint8_t>(ObjectPriority::Default));

	return true;
}

void Game::Update()
{
	
}

void Game::Render(RenderContext& rc)
{

}


