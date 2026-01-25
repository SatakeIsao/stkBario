/**
 * DebugScene.cpp
 * デバッグ動作確認用シーン
 */

#include "stdafx.h"
#if defined(APP_DEBUG)

#include "DebugScene.h"
#include "battle/BattleManager.h"
#include "ui/Layout.h"


namespace
{
	// @todo for test
	static app::ui::Layout* testLayout;
}


DebugScene::DebugScene()
{
}


DebugScene::~DebugScene()
{
	app::battle::BattleManager::Finalize();
}


bool DebugScene::Start()
{
	app::battle::BattleManager::Initialize();

	app::battle::BattleManager::Get().Start();

	testLayout = new app::ui::Layout();
	testLayout->Initialize<app::ui::MenuBase>("Assets/ui/layout/testLayout.json");

	return true;
}


void DebugScene::Update()
{
	app::battle::BattleManager::Get().Update();

	testLayout->Update();
}


void DebugScene::Render(RenderContext& rc)
{
	testLayout->Render(rc);
}


bool DebugScene::RequestScene(uint32_t& id, float& waitTime)
{
	if (g_pad[0]->IsTrigger(enButtonA)) {
		//id = DebugScene::ID();
		//waitTime = 3.0f;
		//return true;
	}

	return false;
}

#endif // APP_DEBUG