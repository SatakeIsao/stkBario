#include "stdafx.h"
#include "BattleScene.h"
#include "GameOverScene.h"
#include "GameClearScene.h"
#include "TitleScene.h"
#include "battle/BattleManager.h"
#include "ui/SoundOptionMenu.h"
#include "ui/BattleSequence.h"
#include "core/PauseManager.h"
#include "SceneManager.h"

namespace
{
}



BattleScene::BattleScene()
{
}


BattleScene::~BattleScene()
{
	app::battle::BattleManager::Finalize();
	app::core::PauseManager::Finalize();
}


bool BattleScene::Start()
{
	app::battle::BattleManager::Initialize();
	app::battle::BattleManager::Get().Start();

	return true;
}


void BattleScene::Update()
{
	app::battle::BattleManager::Get().Update();
}


void BattleScene::Render(RenderContext& rc)
{
}


bool BattleScene::RequestScene(uint32_t& id, float& waitTime)
{
	/** ゲームオーバーアイコンのアニメーション終了したら */
	if (app::battle::BattleManager::Get().IsGameOverAnimFinished()
		|| app::battle::BattleManager::Get().IsTimeUpAnimFinished())
	{
		id = GameOverScene::ID();
		waitTime = 3.0f;
		return true;
	}
	/** ゲームクリアアイコンのアニメーション終了したら */
	if (app::battle::BattleManager::Get().IsGameClearAnimFinished())
	{
		// 遷移前に現在のコイン数を取得し、SceneManagerに預ける
		int currentCoin = app::battle::BattleManager::Get().GetTotalCoin();
		SceneManager::Get().SetTotalCoin(currentCoin);
	
		int currentTimer = app::battle::BattleManager::Get().GetRemainTime();
		SceneManager::Get().SetCurrentTimer(currentTimer);
	
		id = GameClearScene::ID();
		waitTime = 3.0f;
		return true;
	}

	if (app::core::PauseManager::Get().IsReturnToTitleRequested())
	{
		id = TitleScene::ID();
		waitTime = 3.0f;
		return true;
	}

	if (requestSceneId_ != INVALID_SCENE_ID)
	{
		id = requestSceneId_;
		waitTime = 1.0f;
		return true;
	}
	
	return false;
}


void BattleScene::Change()
{
}


bool BattleScene::CanChange() const
{
	return true;
}
