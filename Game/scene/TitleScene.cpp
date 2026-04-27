/**
 * TitleScene.cpp
 * タイトル画面のシーン
 */
#include "stdafx.h"
#include "TitleScene.h"
#include "BattleScene.h"
#include "sound/SoundManager.h"
#include "ui/TitleMenu.h"
#include "ui/ManualMenu.h"
#include "ui/AwardMenu.h"

#if defined(APP_DEBUG)
#include "DebugScene.h"
#endif // APP_DEBUG

namespace
{
	constexpr float B_BUTTON_HOLD_THRESHOLD = 1.0f;
}

TitleScene::TitleScene()
{}


TitleScene::~TitleScene()
{
	DeleteGO(titleMenu_);
	DeleteGO(manualMenu_);
	DeleteGO(awardMenu_);
}


bool TitleScene::Start()
{
	app::SoundManager::Get().PlayBGM(static_cast<int>(app::SoundKind::Title));

	// タイトルメニューオブジェクト生成
	{
		titleMenu_ = NewGO<app::ui::TitleMenu>(static_cast<uint8_t>(ObjectPriority::Pause));
		state_ = TitleSceneState::TitleMenu;
	}

	return true;
}


void TitleScene::Update()
{
	// タイトルメニュー操作中の処理
	if (state_ == TitleSceneState::TitleMenu)
	{
		if (titleMenu_ && titleMenu_->IsReadyToSelect())
		{
			if (g_pad[0]->IsTrigger(enButtonA))
			{
				savedCursolIndex_ = titleMenu_->GerCurrentIndex();
				ExecuteTitleMenuSelection(savedCursolIndex_);
			}
		}
	}
	// マニュアル（あそびかた）メニュー操作中の処理
	else if (state_ == TitleSceneState::ManualMenu)
	{
		// 開いた直後のフレームは入力を無視する
		if (isSubMenuJustOpened_)
		{
			isSubMenuJustOpened_ = false;
		}
		else
		{
			if (g_pad[0]->IsPress(enButtonB)) // Bボタンが押され続けている
			{
				bButtonHoldTime_ += g_gameTime->GetFrameDeltaTime(); // 経過時間を加算
			}
			else
			{
				bButtonHoldTime_ = 0.0f; // 離されたらリセット
			}

			// Bボタン長押しでタイトルメニューに戻る
			if (bButtonHoldTime_ >= B_BUTTON_HOLD_THRESHOLD)
			{
				bButtonHoldTime_ = 0.0f;
				app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));

				// ManualMenuを閉じて削除
				manualMenu_->OnClose();
				DeleteGO(manualMenu_);
				manualMenu_ = nullptr;

				state_ = TitleSceneState::TitleMenu;
				titleMenu_ = NewGO<app::ui::TitleMenu>(static_cast<uint8_t>(ObjectPriority::Pause));
				titleMenu_->OpenImmediate();
			}
		}
	}
	// アワード（実績）メニュー操作中
	else if (state_ == TitleSceneState::AwardMenu)
	{
		// Bボタンでタイトルメニューに戻る
		if (g_pad[0]->IsTrigger(enButtonB))
		{
			app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));

			// 状態を戻す
			state_ = TitleSceneState::TitleMenu;

			// AwardMenuを閉じて削除
			if (awardMenu_) {
				awardMenu_->OnClose();
				DeleteGO(awardMenu_);
				awardMenu_ = nullptr;
			}

			// TitleMenuを再度生成
			state_ = TitleSceneState::TitleMenu;
			titleMenu_ = NewGO<app::ui::TitleMenu>(static_cast<uint8_t>(ObjectPriority::Pause));
			titleMenu_->OpenImmediate();
		}
	}
}


void TitleScene::ExecuteTitleMenuSelection(int index)
{
	if (index == 0) // スタート
	{
		app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
		m_requestSceneId = BattleScene::ID();
	}
	else if (index == 1) // あそびかた
	{
		app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));

		state_ = TitleSceneState::ManualMenu;
		isSubMenuJustOpened_ = true;
		bButtonHoldTime_ = 0.0f;   

		if (titleMenu_)
		{
			titleMenu_->OnClose();
			DeleteGO(titleMenu_);
			titleMenu_ = nullptr;
		}

		manualMenu_ = NewGO<app::ui::ManualMenu>(static_cast<uint8_t>(ObjectPriority::Pause));
		manualMenu_->OnOpen();
	}
	else if (index == 2) // アワード
	{
		app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));

		state_ = TitleSceneState::AwardMenu;

		if (titleMenu_)
		{
			titleMenu_->OnClose();
			DeleteGO(titleMenu_);
			titleMenu_ = nullptr;
		}

		awardMenu_ = NewGO<app::ui::AwardMenu>(static_cast<uint8_t>(ObjectPriority::Pause));
		awardMenu_->OnOpen();
	}
	else if (index == 3) // おわり
	{
		app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
		exit(0);
	}
}


void TitleScene::Render(RenderContext& rc)
{}


bool TitleScene::RequestScene(uint32_t& id, float& waitTime)
{
	if (m_requestSceneId != INVALID_SCENE_ID) {
		id = m_requestSceneId;
		waitTime = 3.0f;
		return true;
	}
	return false;
}