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
			// バイナリ経由で読み込み済みのためここでは何もしない
		}

		GameOverMenu::~GameOverMenu()
		{}

		void GameOverMenu::Update()
		{
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::GameOverMenuParameter>();
			if (g_pad[0]->IsTrigger(enButtonDown))
			{
				app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
				cursolIndex_++;
				if (cursolIndex_ >= p->maxCursolIndex)
				{
					cursolIndex_ = p->maxCursolIndex;
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
			//auto* parameter = app::core::ParameterManager::Get().GetParameter<app::core::GameOverMenuParameter>();
			// 音を変える/タイトルにもどるテキストの場所
			{
				const float x = (cursolIndex_ == 0) ? p->cursolPositionXA : p->cursolPositionXB;
				auto cursol = layout_->GetMenu()->GetUI<UIIcon>(Hash32("Cursol"));
				cursol->transform.localPosition.x = x;
			}
			{
				const float y = (cursolIndex_ == 0) ? p->cursolPositionYA : p->cursolPositionYB;
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
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::GameOverMenuParameter>();
			const Vector3 SELECTION_COLOR(p->selectionColorX, p->selectionColorY, p->selectionColorZ);
			const Vector3 DEFAULT_COLOR(p->defaultColorX, p->defaultColorY, p->defaultColorZ);
			const Vector3 SELECTION_SCALE(p->selectionScaleX, p->selectionScaleY, p->selectionScaleZ);
			const Vector3 DEFAULT_SCALE(p->defaultScaleX, p->defaultScaleY, p->defaultScaleZ);
			/** TODO: Updateで、処理が走っているので、無駄な処理を改善したい */
			auto textRetry = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_retry"));
			auto textTitle = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_ReturnToTitle"));

			/** 各項目選択中に拡大アニメーションを再生 */
			if (cursolIndex_ == 0
				&& textRetry)
			{
				/** リセット: 黄色から白 */
				textTitle->color.Set(DEFAULT_COLOR);
				/** リセット: 等倍に戻す */
				textTitle->transform.localScale = DEFAULT_SCALE;

				/** 黄色 */
				textRetry->color.Set(SELECTION_COLOR);
				/** スケール拡大 */
				textRetry->transform.localScale = SELECTION_SCALE;
			}
			else if (cursolIndex_ == p->maxCursolIndex
				&& textTitle)
			{
				/** リセット: 黄色から白 */
				textRetry->color.Set(DEFAULT_COLOR);
				/** リセット: 等倍に戻す */
				textRetry->transform.localScale = DEFAULT_SCALE;

				/** 黄色 */
				textTitle->color.Set(SELECTION_COLOR);
				/** スケール拡大 */
				textTitle->transform.localScale = SELECTION_SCALE;
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