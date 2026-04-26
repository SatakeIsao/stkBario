/**
 * BootScene.cpp
 * 起動画面のシーン
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
{
}


TitleScene::~TitleScene()
{
	DeleteGO(titleMenu_);
	DeleteGO(manualMenu_);
	DeleteGO(awardMenu_);
}


bool TitleScene::Start()
{
	// サウンド管理生成
	app::SoundManager::Initialize();
	app::SoundManager::Get().PlayBGM(static_cast<int>(app::SoundKind::Title));
	// タイトルメニューオブジェクト
	{
		titleMenu_ = NewGO<app::ui::TitleMenu>(static_cast<uint8_t>(ObjectPriority::Pause));
		state_ = TitleSceneState::TitleMenu;
	}
	
	return true;
}


void TitleScene::Update()
{
	// ① タイトルメニュー操作中の処理
	if (state_ == TitleSceneState::TitleMenu)
	{
		if (titleMenu_ && titleMenu_->IsReadyToSelect())
		{
			if (g_pad[0]->IsTrigger(enButtonA))
			{
				if (titleMenu_->GerCurrentIndex() == 0) // スタート
				{
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
					m_requestSceneId = BattleScene::ID();
				}
				else if (titleMenu_->GerCurrentIndex() == 1) // あそびかた
				{
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));

					// 状態をマニュアルメニューに切り替え
					state_ = TitleSceneState::ManualMenu;

					// TitleMenuのアニメーション等を閉じる (実装に合わせて調整)
					titleMenu_->OnClose();
					// 入力が被らないように非表示にするか、一旦削除する
					// ここでは動的生成・破棄の例
					DeleteGO(titleMenu_);
					titleMenu_ = nullptr;

					// マニュアルメニューを生成して開く
					manualMenu_ = NewGO<app::ui::ManualMenu>(static_cast<uint8_t>(ObjectPriority::Pause));
					manualMenu_->OnOpen();
				}
				else if (titleMenu_->GerCurrentIndex() == 2) // アワード
				{
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));

					// 状態をマニュアルメニューに切り替え
					state_ = TitleSceneState::AwardMenu;

					// TitleMenuのアニメーション等を閉じる (実装に合わせて調整)
					titleMenu_->OnClose();
					// 入力が被らないように非表示にするか、一旦削除する
					// ここでは動的生成・破棄の例
					DeleteGO(titleMenu_);
					titleMenu_ = nullptr;

					// マニュアルメニューを生成して開く
					awardMenu_ = NewGO<app::ui::AwardMenu>(static_cast<uint8_t>(ObjectPriority::Pause));
					awardMenu_->OnOpen();
				}
				else if (titleMenu_->GerCurrentIndex() == 3) // おわる
				{
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
					exit(0);
				}
			}
		}
	}
	// ② マニュアルメニュー操作中の処理
	else if (state_ == TitleSceneState::ManualMenu)
	{
		if (g_pad[0]->IsPress(enButtonB)) // Bボタンが押され続けている間
		{
			bButtonHoldTime_ += g_gameTime->GetFrameDeltaTime(); // 経過時間を加算
		}
		else
		{
			bButtonHoldTime_ = 0.0f; // 離したらリセット
		}
		// Bボタンでタイトルメニューに戻る
		if (bButtonHoldTime_ >= B_BUTTON_HOLD_THRESHOLD)
		{
			bButtonHoldTime_ = 0.0f;
			app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));

			// 状態を戻す
			state_ = TitleSceneState::TitleMenu;

			// ManualMenuを閉じる・破棄する
			manualMenu_->OnClose();
			DeleteGO(manualMenu_);
			manualMenu_ = nullptr;

			// TitleMenuを再度生成する
			titleMenu_ = NewGO<app::ui::TitleMenu>(static_cast<uint8_t>(ObjectPriority::Pause));
			// 再生成直後からメニュー操作可能な状態にするための関数を呼ぶとスムーズです
			// titleMenu_->OnOpen(); 
		}
	}
	//称号メニュー
	else if (state_ == TitleSceneState::AwardMenu)
	{
		// 例: Bボタンでタイトルメニューに戻る
		if (g_pad[0]->IsTrigger(enButtonB))
		{
			app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));

			// 状態を戻す
			state_ = TitleSceneState::TitleMenu;

			// ManualMenuを閉じる・破棄する
			manualMenu_->OnClose();
			DeleteGO(manualMenu_);
			manualMenu_ = nullptr;

			// TitleMenuを再度生成する
			titleMenu_ = NewGO<app::ui::TitleMenu>(static_cast<uint8_t>(ObjectPriority::Pause));
			// 再生成直後からメニュー操作可能な状態にするための関数を呼ぶとスムーズです
			// titleMenu_->OnOpen(); 
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