/**
 * BootScene.cpp
 * 起動画面のシーン
 */
#include "stdafx.h"
#include "TitleScene.h"
#include "BattleScene.h"
#include "sound/SoundManager.h"
#include "ui/TitleMenu.h"

#if defined(APP_DEBUG)
#include "DebugScene.h"
#endif // APP_DEBUG

TitleScene::TitleScene()
{
}


TitleScene::~TitleScene()
{
	DeleteGO(titleMenu_);
}


bool TitleScene::Start()
{
	//リザルトメニューマネージャーオブジェクト
	{
		titleMenu_ = NewGO<app::ui::TitleMenu>(static_cast<uint8_t>(ObjectPriority::Pause));
	}
	return true;
}


void TitleScene::Update()
{
	if (titleMenu_ && titleMenu_->IsReadyToSelect())
	{
		if (g_pad[0]->IsTrigger(enButtonA)) {
#if defined(APP_DEBUG)

			if (titleMenu_->GerCurrentIndex() == 0)
			{
				/** バトルシーン遷移 */
				app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
				m_requestSceneId = BattleScene::ID();
			}
			if (titleMenu_->GerCurrentIndex() == 1)
			{
				/**
				 * あそびかた開く
				 * これはメニュー側でやる
				 */
			}
			if (titleMenu_->GerCurrentIndex() == 2)
			{
				/**
				 * 称号メニュー開く
				 * これはメニュー側でやる
				 */
			}
			if (titleMenu_->GerCurrentIndex() == 3)
			{
				/** ゲーム終了 */
				app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
				exit(0);
			}
#endif
		}
	}
}


void TitleScene::Render(RenderContext& rc)
{
}


bool TitleScene::RequestScene(uint32_t& id, float& waitTime)
{
	if (m_requestSceneId != INVALID_SCENE_ID) {
		id = m_requestSceneId;
		waitTime = 3.0f;
		return true;
	}
	return false;
}