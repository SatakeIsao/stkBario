/**
 * GameClearScene.cpp
 * ゲームクリア画面を表示
 */

#include "stdafx.h"
#include "GameClearScene.h"
#include "TitleScene.h"
#include "ui/ResultMenu.h"
#include "sound/SoundManager.h"
#include "core/ParameterManager.h"





GameClearScene::GameClearScene()
{}


GameClearScene:: ~GameClearScene()
{
	DeleteGO(resultMenu_);
}


bool GameClearScene::Start()
{
	//リザルトメニューマネージャーオブジェクト
	{
		resultMenu_ = NewGO<app::ui::ResultMenu>(static_cast<uint8_t>(ObjectPriority::Pause));
	}
	return true;
}


void GameClearScene::Update()
{
	/** リザルトUIが全部表示され、Aボタン押したら遷移 */
	if (resultMenu_ && resultMenu_->IsReadyToExit())
	{
		if (g_pad[0]->IsTrigger(enButtonA))
		{
			app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
			m_requestSceneId = TitleScene::ID();
		}
	}
}


void GameClearScene::Render(RenderContext& rc)
{}


bool GameClearScene::RequestScene(uint32_t& id, float& waitTime)
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