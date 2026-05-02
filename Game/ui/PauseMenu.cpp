#include "stdafx.h"
#include "PauseMenu.h"
#include "battle/BattleManager.h"
#include "core/ParameterManager.h"
#include "sound/SoundManager.h"
#include "ui/UIAnimationFactory.h"
#include "ui/UIAnimation.h"
#include "ui/AwardManager.h"

namespace
{
	static app::ui::UIAnimationSequence* seq = nullptr;
}

namespace app
{
	namespace ui
	{
		PauseMenu::PauseMenu()
		{
			// バイナリ経由で読み込み済みのためここでは何もしない
		}

		PauseMenu::~PauseMenu()
		{}

		void PauseMenu::Update()
		{
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::MasterPauseMenuParameter>();

			auto* canvas = GetCanvas();
			if (canvas)
			{
				//閉じる
				{
					auto* closeAnim = canvas->FindAnimation(Hash32("ScaleDown"));
					if (closeAnim && !closeAnim->IsPlay())
					{
						canvas->RemoveAnimation(Hash32("ScaleDown"));
						closeAnim = nullptr;
						isPause_ = false;
					}
				}
				//開く
				{
					auto* openAnim = canvas->FindAnimation(Hash32("ScaleUp"));
					if (openAnim && !openAnim->IsPlay())
					{
						canvas->RemoveAnimation(Hash32("ScaleUp"));
					}
				}
			}

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
			//auto* parameter = app::core::ParameterManager::Get().GetParameter<app::core::MasterPauseMenuParameter>();
			// 音を変える/タイトルにもどるテキストの場所
			{
				const float x = (cursolIndex_ == 0) ? p->cursolPositionXA : p->cursolPositionXB;
				auto cursol = GetUI<UIIcon>(Hash32("Cursol"));
				cursol->transform.localPosition.x = x;
			}
			{
				const float y = (cursolIndex_ == 0) ? p->cursolPositionYA : p->cursolPositionYB;
				auto cursol = GetUI<UIIcon>(Hash32("Cursol"));
				cursol->transform.localPosition.y = y;
			}

			PlaySelectedAnimation();
			MenuBase::Update();
		}

		void PauseMenu::OnOpen()
		{
			isPause_ = true;

			// キャンバス
			{
				auto* canvas = GetCanvas();
				if (canvas)
				{
					canvas->transform.localScale = Vector3::Zero;
					canvas->RemoveAnimation(Hash32("ScaleUp"));
					canvas->RemoveAnimation(Hash32("ScaleDown"));
					//アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(canvas, Hash32("ScaleUp"));
					auto* openAnim = canvas->FindAnimation(Hash32("ScaleUp"));
					if (openAnim) openAnim->Play();
				}

				if (app::ui::AwardManager::IsAvailable())
				{
					app::ui::AwardManager::Get().OnTimeStopper();
				}
			}
			//カーソル
			{
				auto cursol = GetUI<UIIcon>(Hash32("Cursol"));
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

		void PauseMenu::OnClose()
		{
			// キャンバス
			{
				auto* canvas = GetCanvas();
				if (canvas)
				{
					canvas->RemoveAnimation(Hash32("ScaleUp"));
					canvas->RemoveAnimation(Hash32("ScaleDown"));
					//アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(canvas, Hash32("ScaleDown"));
					auto* closeAnim = canvas->FindAnimation(Hash32("ScaleDown"));
					if (closeAnim) closeAnim->Play();
				}
			}
			//カーソル
			{
				auto cursol = GetUI<UIIcon>(Hash32("Cursol"));
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

		void PauseMenu::PlaySelectedAnimation()
		{
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::MasterPauseMenuParameter>();
			const Vector3 SELECTION_COLOR(p->selectionColorX, p->selectionColorY, p->selectionColorZ);
			const Vector3 DEFAULT_COLOR(p->defaultColorX, p->defaultColorY, p->defaultColorZ);
			const Vector3 SELECTION_SCALE(p->selectionScaleX, p->selectionScaleY, p->selectionScaleZ);
			const Vector3 DEFAULT_SCALE(p->defaultScaleX, p->defaultScaleY, p->defaultScaleZ);
			const int MAX_CURSOL_INDEX = p->maxCursolIndex;
			/** TODO: Updateで、処理が走っているので、無駄な処理を改善したい */
			auto* textSound = GetUI<app::ui::UIIcon>(Hash32("text_ChangeTheSound"));
			auto* textTitle = GetUI<app::ui::UIIcon>(Hash32("text_ReturnToTitle"));

			/** 各項目選択中に拡大アニメーションを再生 */
			if (cursolIndex_ == 0
				&& textSound)
			{
				/** リセット: 黄色から白 */
				textTitle->color.Set(DEFAULT_COLOR);
				/** リセット: 等倍に戻す */
				textTitle->transform.localScale = DEFAULT_SCALE;

				/** 黄色 */
				textSound->color.Set(SELECTION_COLOR);
				/** スケール拡大 */
				textSound->transform.localScale = SELECTION_SCALE;
			}
			else if (cursolIndex_ == p->maxCursolIndex
				&& textTitle)
			{
				/** リセット: 黄色から白 */
				textSound->color.Set(DEFAULT_COLOR);
				/** リセット: 等倍に戻す */
				textSound->transform.localScale = SELECTION_SCALE;

				/** 黄色 */
				textTitle->color.Set(SELECTION_COLOR);
				/** スケール拡大 */
				textTitle->transform.localScale = SELECTION_SCALE;
			}
		}

		void PauseMenu::InitializeLogic()
		{
			// サウンドバーの位置情報設定
			// アニメーションとかいれたり
			/** キャンバス（UI全体) */
			{
				auto* canvas = GetCanvas();
				if (canvas)
				{
					canvas->transform.localScale = Vector3::Zero;
				}
			}
			/** カーソル */
			{
				auto cursol = GetUI<UIIcon>(Hash32("Cursol"));
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