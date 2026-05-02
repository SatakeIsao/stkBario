#include "stdafx.h"
#include "ReturnToTitleMenu.h"
#include "core/ParameterManager.h"
#include "sound/SoundManager.h"
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
		ReturnToTitleMenu::ReturnToTitleMenu()
		{
			// バイナリ経由で読み込み済みのためここでは何もしない
		}

		ReturnToTitleMenu:: ~ReturnToTitleMenu()
		{}

		void ReturnToTitleMenu::Update()
		{
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::ReturnToTitleMenuParameter>();
			const Vector3 SELECTION_COLOR(p->selectionColorX, p->selectionColorY, p->selectionColorZ);
			const Vector3 DEFAULT_COLOR(p->defaultColorX, p->defaultColorY, p->defaultColorZ);
			const Vector3 SELECTION_SCALE(p->selectionScaleX, p->selectionScaleY, p->selectionScaleZ);
			const Vector3 DEFAULT_SCALE(p->defaultScaleX, p->defaultScaleY, p->defaultScaleZ);
			const int MAX_CURSOL_INDEX = p->maxCursolIndex;

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
				if (cursolIndex_ >= MAX_CURSOL_INDEX)
				{
					cursolIndex_ = MAX_CURSOL_INDEX;
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
			/** はいをえらぶ */
			if (cursolIndex_ == 0
				&& g_pad[0]->IsTrigger(enButtonA))
			{
				app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
				isDecidedYes_ = true;
			}
			/** いいえをえらぶ */
			if (cursolIndex_ == MAX_CURSOL_INDEX
				&& g_pad[0]->IsTrigger(enButtonA))
			{
				app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
				isDecidedNo_ = true;
			}
			else
			{
				isDecidedNo_ = false;
			}

			// 動的に数値をUIに設定
			auto* parameter = app::core::ParameterManager::Get().GetParameter<app::core::ReturnToTitleMenuParameter>();
			// はい/いいえテキストの場所
			{
				const float x = (cursolIndex_ == 0) ? parameter->cursolPositionXA : parameter->cursolPositionXB;
				auto cursol = GetUI<UIIcon>(Hash32("Cursol"));
				cursol->transform.localPosition.x = x;
			}
			{
				const float y = (cursolIndex_ == 0) ? parameter->cursolPositionYA : parameter->cursolPositionYB;
				auto cursol = GetUI<UIIcon>(Hash32("Cursol"));
				cursol->transform.localPosition.y = y;
			}

			PlaySelectedAnimation();
			MenuBase::Update();
		}

		void ReturnToTitleMenu::OnOpen()
		{
			isPause_ = true;

			// キャンバス
			{
				auto* canvas = GetCanvas();
				if (canvas)
				{
					canvas->RemoveAnimation(Hash32("ScaleUp"));
					canvas->RemoveAnimation(Hash32("ScaleDown"));
					//アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(canvas, Hash32("ScaleUp"));
					auto* openAnim = canvas->FindAnimation(Hash32("ScaleUp"));
					if (openAnim) openAnim->Play();
				}
			}

			// カーソル
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

		void ReturnToTitleMenu::OnClose()
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
			// カーソル
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

		void ReturnToTitleMenu::PlaySelectedAnimation()
		{
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::ReturnToTitleMenuParameter>();
			const Vector3 SELECTION_COLOR(p->selectionColorX, p->selectionColorY, p->selectionColorZ);
			const Vector3 DEFAULT_COLOR(p->defaultColorX, p->defaultColorY, p->defaultColorZ);
			const Vector3 SELECTION_SCALE(p->selectionScaleX, p->selectionScaleY, p->selectionScaleZ);
			const Vector3 DEFAULT_SCALE(p->defaultScaleX, p->defaultScaleY, p->defaultScaleZ);
			const int MAX_CURSOL_INDEX = p->maxCursolIndex;

			/** TODO: Updateで、処理が走っているので、無駄な処理を改善したい */
			auto* textYes = GetUI<app::ui::UIIcon>(Hash32("text_Yes"));
			auto* textNo = GetUI<app::ui::UIIcon>(Hash32("text_No"));

			/** 各項目選択中に拡大アニメーションを再生 */
			if (cursolIndex_ == 0
				&& textYes)
			{
				/** リセット: 黄色から白 */
				textNo->color.Set(DEFAULT_COLOR);
				/** リセット: 等倍に戻す */
				textNo->transform.localScale = DEFAULT_SCALE;

				/** 黄色 */
				textYes->color.Set(SELECTION_COLOR);
				/** スケール拡大 */
				textYes->transform.localScale = SELECTION_SCALE;
			}
			else if (cursolIndex_ == MAX_CURSOL_INDEX
				&& textNo)
			{
				/** リセット: 黄色から白 */
				textYes->color.Set(DEFAULT_COLOR);
				/** リセット: 等倍に戻す */
				textYes->transform.localScale = DEFAULT_SCALE;

				/** 黄色 */
				textNo->color.Set(SELECTION_COLOR);
				/** スケール拡大 */
				textNo->transform.localScale = SELECTION_SCALE;
			}
		}

		void ReturnToTitleMenu::InitializeLogic()
		{
			// サウンドバーの位置情報設定
			// アニメーションとかいれたり

			/** キャンバス（UI全体) */
			auto* canvas = GetCanvas();
			if (canvas)
			{
				canvas->transform.localScale = Vector3::Zero;
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