/**
 * GameOverScene.cpp
 * ゲームオーバー画面を起動
 */

#include "stdafx.h"
#include "GameOverScene.h"
#include "TitleScene.h"
#include "BattleScene.h"
#include "ui/GameOverMenu.h"
#include "sound/SoundManager.h"
#include "core/ParameterManager.h"





GameOverScene::GameOverScene()
{}


GameOverScene:: ~GameOverScene()
{
	DeleteGO(gameOverMenu_);
}


bool GameOverScene::Start()
{
	//ゲームオーバーマネージャーオブジェクト
	{
		gameOverMenu_ = NewGO<app::ui::GameOverMenu>(static_cast<uint8_t>(ObjectPriority::Pause));
		gameOverMenu_->InitializeLogic();
	}
	return true;
}


void GameOverScene::Update()
{
	if (gameOverMenu_->GerCurrentIndex() == 0
		&& g_pad[0]->IsTrigger(enButtonA))
	{
		app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
		m_requestSceneId = BattleScene::ID();
	}
	if (gameOverMenu_->GerCurrentIndex() == 1
		&& g_pad[0]->IsTrigger(enButtonA))
	{
		app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
		m_requestSceneId = TitleScene::ID();
	}
}


void GameOverScene::Render(RenderContext& rc)
{}


bool GameOverScene::RequestScene(uint32_t& id, float& waitTime)
{
	auto* sceneParam = app::core::ParameterManager::Get().GetParameter<app::core::MasterSceneParameter>();
	if (m_requestSceneId != INVALID_SCENE_ID)
	{
		id = m_requestSceneId;
		waitTime = sceneParam->sceneTransitionWaitTime;
		return true;
	}
	return false;
}


void GameOverScene::Change()
{}


bool GameOverScene::CanChange() const
{
	if (g_pad[0]->IsTrigger(enButtonRight))
	{
		return true;
	}
	return false;
}