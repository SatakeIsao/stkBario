#include "stdafx.h"
#include "GameOverMenu.h"
#include "core/ParameterManager.h"
#include "sound/SoundManager.h"
#include "ui/Layout.h"
#include "ui/UIAnimationFactory.h"
#include "ui/UIAnimation.h"

namespace
{
	static app::ui::UIAnimationSequence* seq = nullptr;
}

namespace app
{
	namespace ui
	{
		GameOverMenu::GameOverMenu()
		{
			/** ゲームオーバーレイアウト */
			{
				layout_ = std::make_unique<app::ui::Layout>();
				layout_->Initialize<app::ui::MenuBase>("Assets/ui/layout/gameOverMenu.json");
			}
			app::core::ParameterManager::Get().LoadParameter<app::core::GameOverMenuParameter>("Assets/master/GameOverMenuParameter.json", [](const nlohmann::json& j, app::core::GameOverMenuParameter& p)
				{
					//TODO; X座標もやりたいなぁ
					p.cursolPositionX[0] = j["cursolPositionXA"];
					p.cursolPositionX[1] = j["cursolPositionXB"];

					p.cursolPositionY[0] = j["cursolPositionYA"];
					p.cursolPositionY[1] = j["cursolPositionYB"];
				});
		}

		GameOverMenu::~GameOverMenu()
		{
			app::core::ParameterManager::Get().UnloadParameter<app::core::GameOverMenuParameter>();
		}
		
		void GameOverMenu::Update()
		{
			//auto* canvas = GetCanvas();
			//if (canvas)
			//{
			//	//閉じる
			//	{
			//		auto* closeAnim = canvas->FindAnimation(Hash32("ScaleDown"));
			//		if (closeAnim && !closeAnim->IsPlay())
			//		{
			//			canvas->RemoveAnimation(Hash32("ScaleDown"));
			//			closeAnim = nullptr;
			//			isPause_ = false;
			//		}
			//	}
			//	//開く
			//	{
			//		auto* openAnim = canvas->FindAnimation(Hash32("ScaleUp"));
			//		if (openAnim && !openAnim->IsPlay())
			//		{
			//			canvas->RemoveAnimation(Hash32("ScaleUp"));
			//		}
			//	}
			//}

			if (g_pad[0]->IsTrigger(enButtonDown))
			{
				app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
				cursolIndex_++;
				if (cursolIndex_ >= 1)
				{
					cursolIndex_ = 1;
				}
			}
			if (g_pad[0]->IsTrigger(enButtonUp))
			{
				app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
				cursolIndex_--;
				if (cursolIndex_ < 0)
				{
					cursolIndex_ = 0;
				}
			}

			

			// 動的に数値をUIに設定
			auto* parameter = app::core::ParameterManager::Get().GetParameter<app::core::GameOverMenuParameter>();
			// 音を変える/タイトルにもどるテキストの場所
			{
				const float x = parameter->cursolPositionX[cursolIndex_];
				auto cursol = layout_->GetMenu()->GetUI<UIIcon>(Hash32("Cursol"));
				cursol->transform.localPosition.x = x;
			}
			{
				const float y = parameter->cursolPositionY[cursolIndex_];
				auto cursol = layout_->GetMenu()->GetUI<UIIcon>(Hash32("Cursol"));
				cursol->transform.localPosition.y = y;
			}

			PlaySelectedAnimation();
			layout_->Update();
		}

		void GameOverMenu::Render(RenderContext& rc)
		{
			if (layout_) {
				layout_->Render(rc);
			}
		}

		void GameOverMenu::OnOpen()
		{
			// カーソル
			{
				auto cursol = layout_->GetMenu()->GetUI<UIIcon>(Hash32("Cursol"));
				if (cursol)
				{
					cursol->isDraw = true;
					// アニメーションをアタッチ (UIAlphaAnimationクラスが存在すると仮定)
					app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(cursol, Hash32("FadeIn"));

					// アニメーションを取得して再生
					auto anim = cursol->FindAnimation(Hash32("FadeIn"));
					if (anim)
					{
						anim->Play();
					}
				}
			}
		}

		void GameOverMenu::OnClose()
		{
			// カーソル
			{
				auto cursol = layout_->GetMenu()->GetUI<UIIcon>(Hash32("Cursol"));
				if (cursol)
				{
					cursol->isDraw = true;
					// アニメーションをアタッチ (UIAlphaAnimationクラスが存在すると仮定)
					app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(cursol, Hash32("FadeIn"));

					// アニメーションを取得して再生
					auto anim = cursol->FindAnimation(Hash32("FadeIn"));
					if (anim)
					{
						anim->Stop();
					}
					cursol->isDraw = false;
				}
			}
		}

		void GameOverMenu::PlaySelectedAnimation()
		{
			/** TODO: Updateで、処理が走っているので、無駄な処理を改善したい */
			auto textRetry = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_retry"));
			auto textTitle = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_ReturnToTitle"));

			/** 各項目選択中に拡大アニメーションを再生 */
			if (cursolIndex_ == 0
				&& textRetry)
			{
				/** リセット: 黄色から白 */
				textTitle->color.Set(Vector3(1.0f, 1.0f, 1.0f));
				/** リセット: 等倍に戻す */
				textTitle->transform.localScale = Vector3(1.0f, 1.0f, 0.0f);

				/** 黄色 */
				textRetry->color.Set(Vector3(1.0f, 1.0f, 0.0f));
				/** スケール拡大 */
				textRetry->transform.localScale = Vector3(1.3f, 1.3f, 0.0f);
			}
			else if (cursolIndex_ == 1
				&& textTitle)
			{
				/** リセット: 黄色から白 */
				textRetry->color.Set(Vector3(1.0f, 1.0f, 1.0f));
				/** リセット: 等倍に戻す */
				textRetry->transform.localScale = Vector3(1.0f, 1.0f, 0.0f);

				/** 黄色 */
				textTitle->color.Set(Vector3(1.0f, 1.0f, 0.0f));
				/** スケール拡大 */
				textTitle->transform.localScale = Vector3(1.3f, 1.3f, 0.0f);
			}
		}
	
		void GameOverMenu::InitializeLogic()
		{
			// サウンドバーの位置情報設定
			// アニメーションとかいれたり
			/** カーソル */
			{
				auto cursol = layout_->GetMenu()->GetUI<UIIcon>(Hash32("Cursol"));
				if (cursol)
				{
					// アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(cursol, Hash32("FadeIn"));
					seq = new app::ui::UIAnimationSequence();
					seq->Add(Hash32("FadeIn"));
					// アニメーションを再生
					seq->Play(cursol);
				}
			}
		}
	}
}