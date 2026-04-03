#include "stdafx.h"
#include "TitleMenu.h"
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
		TitleMenu::TitleMenu()
		{
			/** ゲームオーバーレイアウト */
			{
				layout_ = std::make_unique<app::ui::Layout>();
				layout_->Initialize<app::ui::MenuBase>("Assets/ui/layout/titleMenu.json");
			}
			app::core::ParameterManager::Get().LoadParameter<app::core::TitleMenuParameter>("Assets/master/TitleMenuParameter.json", [](const nlohmann::json& j, app::core::TitleMenuParameter& p)
				{
					//TODO; X座標もやりたいなぁ
					p.cursolPositionX[0] = j["cursolPositionXA"];
					p.cursolPositionX[1] = j["cursolPositionXB"];
					p.cursolPositionX[2] = j["cursolPositionXC"];
					p.cursolPositionX[3] = j["cursolPositionXD"];

					p.cursolPositionY[0] = j["cursolPositionYA"];
					p.cursolPositionY[1] = j["cursolPositionYB"];
					p.cursolPositionY[2] = j["cursolPositionYC"];
					p.cursolPositionY[3] = j["cursolPositionYD"];
				});
		}

		TitleMenu::~TitleMenu()
		{}

		void TitleMenu::Update()
		{
			if (isOpenJustNow_)
			{
				isOpenJustNow_ = false;
			}

			if (currentState_ == MenuState::enTitle)
			{
				/** Aボタン開いたらメニューが開く */
				if (g_pad[0]->IsTrigger(enButtonA))
				{
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
					currentState_ = MenuState::enMenu;
					isOpenJustNow_ = true;
					/** 関数等でアニメーションを再生 */
					OnOpen();
				}
				else
				{
					OnClose();
				}
			}

			if (currentState_ == MenuState::enMenu)
			{
				/** Aボタン開いたらメニューが開く */
				if (g_pad[0]->IsTrigger(enButtonB))
				{
					currentState_ = MenuState::enTitle;
					isOpenJustNow_ = false;
					/** 関数等でアニメーションを再生 */
					OnClose();
				}

				if (g_pad[0]->IsTrigger(enButtonDown))
				{
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
					cursolIndex_++;
					if (cursolIndex_ >= 3)
					{
						cursolIndex_ = 3;
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
			}



			// 動的に数値をUIに設定
			auto* parameter = app::core::ParameterManager::Get().GetParameter<app::core::TitleMenuParameter>();
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

		void TitleMenu::Render(RenderContext& rc)
		{
			if (layout_) {
				layout_->Render(rc);
			}
		}

		void TitleMenu::OnOpen()
		{
			auto textTitle = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_title"));
			auto textStart = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_start"));
			auto textHowToPlay = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_howToPlay"));
			auto textAward = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_award"));
			auto textExit = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_exit"));
			auto cursol = layout_->GetMenu()->GetUI<UIIcon>(Hash32("Cursol"));
			auto buttonA = layout_->GetMenu()->GetUI<UIIcon>(Hash32("buttonA"));
			auto textPush = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_push"));

			if (textTitle)
			{
				textTitle->RemoveAnimation(Hash32("TitleMoveToCenter"));
				textTitle->RemoveAnimation(Hash32("TitleScaleToCenter"));

				app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(textTitle, Hash32("TitleMoveToMenu"));
				app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(textTitle, Hash32("TitleScaleToMenu"));

				// 再生
				auto* moveAnim = textTitle->FindAnimation(Hash32("TitleMoveToMenu"));
				if (moveAnim) moveAnim->Play();

				auto* scaleAnim = textTitle->FindAnimation(Hash32("TitleScaleToMenu"));
				if (scaleAnim) scaleAnim->Play();
			}
			if (textStart)
			{
				textStart->isDraw = true;
				textStart->RemoveAnimation(Hash32("SlideOut_Start"));
				app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(textStart, Hash32("SlideIn_Start"));
				auto* anim = textStart->FindAnimation(Hash32("SlideIn_Start"));
				if (anim) anim->Play();
			}
			if (textHowToPlay)
			{
				textHowToPlay->isDraw = true;
				textHowToPlay->RemoveAnimation(Hash32("SlideOut_HowToPlay"));
				app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(textHowToPlay, Hash32("SlideIn_HowToPlay"));
				auto* anim = textHowToPlay->FindAnimation(Hash32("SlideIn_HowToPlay"));
				if (anim) anim->Play();
			}
			if (textAward)
			{
				textAward->isDraw = true;
				textAward->RemoveAnimation(Hash32("SlideOut_Award"));
				app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(textAward, Hash32("SlideIn_Award"));
				auto* anim = textAward->FindAnimation(Hash32("SlideIn_Award"));
				if (anim) anim->Play();

			}
			if (textExit)
			{
				textExit->isDraw = true;
				textExit->RemoveAnimation(Hash32("SlideOut_Exit"));
				app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(textExit, Hash32("SlideIn_Exit"));
				auto* anim = textExit->FindAnimation(Hash32("SlideIn_Exit"));
				if (anim) anim->Play();
			}
			if (cursol)
			{
				cursol->isDraw = true; // 表示する

				// アニメーションをアタッチ (UIAlphaAnimationクラスが存在すると仮定)
				app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(cursol, Hash32("FadeIn"));

				// アニメーションを取得して再生
				auto* anim = cursol->FindAnimation(Hash32("FadeIn"));
				if (anim)
				{
					anim->Play();
				}
			}

			/** 表示 */
			if (buttonA)
			{
				buttonA->isDraw = false;
				auto* anim = buttonA->FindAnimation(Hash32("FadeIn"));
				if (anim)
				{
					anim->Stop();
				}
			}
			if (textPush)
			{
				textPush->isDraw = false;
				auto* anim = textPush->FindAnimation(Hash32("FadeIn"));
				if (anim)
				{
					anim->Stop();
				}
			}
		}

		void TitleMenu::OnClose()
		{
			auto textTitle = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_title"));
			auto textStart = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_start"));
			auto textHowToPlay = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_howToPlay"));
			auto textAward = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_award"));
			auto textExit = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_exit"));
			auto cursol = layout_->GetMenu()->GetUI<UIIcon>(Hash32("Cursol"));
			auto buttonA = layout_->GetMenu()->GetUI<UIIcon>(Hash32("buttonA"));
			auto textPush = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_push"));

			if (textTitle)
			{
				textTitle->RemoveAnimation(Hash32("TitleMoveToMenu"));
				textTitle->RemoveAnimation(Hash32("TitleScaleToMenu"));

				app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(textTitle, Hash32("TitleMoveToCenter"));
				app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(textTitle, Hash32("TitleScaleToCenter"));

				// 再生
				auto* moveAnim = textTitle->FindAnimation(Hash32("TitleMoveToCenter"));
				if (moveAnim) moveAnim->Play();

				auto* scaleAnim = textTitle->FindAnimation(Hash32("TitleScaleToCenter"));
				if (scaleAnim) scaleAnim->Play();
			}
			if (textStart)
			{
				textStart->RemoveAnimation(Hash32("SlideIn_Start"));
				app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(textStart, Hash32("SlideOut_Start"));
				auto* anim = textStart->FindAnimation(Hash32("SlideOut_Start"));
				if (anim) anim->Play();
			}
			if (textHowToPlay)
			{
				textHowToPlay->RemoveAnimation(Hash32("SlideIn_HowToPlay"));
				app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(textHowToPlay, Hash32("SlideOut_HowToPlay"));
				auto* anim = textHowToPlay->FindAnimation(Hash32("SlideOut_HowToPlay"));
				if (anim) anim->Play();
			}
			if (textAward)
			{
				textAward->RemoveAnimation(Hash32("SlideIn_Award"));
				app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(textAward, Hash32("SlideOut_Award"));
				auto* anim = textAward->FindAnimation(Hash32("SlideOut_Award"));
				if (anim) anim->Play();
			}
			if (textExit)
			{
				textExit->RemoveAnimation(Hash32("SlideIn_Exit"));
				app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(textExit, Hash32("SlideOut_Exit"));
				auto* anim = textExit->FindAnimation(Hash32("SlideOut_Exit"));
				if (anim) anim->Play();
			}
			if (cursol)
			{
				auto* anim = cursol->FindAnimation(Hash32("FadeIn"));
				if (anim)
				{
					anim->Stop();
				}
				cursol->isDraw = false;
			}

			/** 表示 */
			if (buttonA)
			{
				buttonA->isDraw = true;
				// アニメーションをアタッチ (UIAlphaAnimationクラスが存在すると仮定)
				app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(buttonA, Hash32("FadeIn"));

				// アニメーションを取得して再生
				auto* anim = buttonA->FindAnimation(Hash32("FadeIn"));
				if (anim)
				{
					anim->Play();
				}
			}
			if (textPush)
			{
				textPush->isDraw = true;
				
				app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(textPush, Hash32("FadeIn"));

				// アニメーションを取得して再生
				auto* anim = textPush->FindAnimation(Hash32("FadeIn"));
				if (anim)
				{
					anim->Play();
				}
			}
		}

		void TitleMenu::PlaySelectedAnimation()
		{
			/** TODO: Updateで、処理が走っているので、無駄な処理を改善したい */
			auto textStart = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_start"));
			auto textHowToPlay = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_howToPlay"));
			auto textAward = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_award"));
			auto textExit = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_exit"));

			/** 各項目選択中に拡大アニメーションを再生 */
			if (cursolIndex_ == 0
				&& textStart)
			{
				/** リセット: 黄色から白 */
				textHowToPlay->color.Set(Vector3(1.0f, 1.0f, 1.0f));
				/** リセット: 等倍に戻す */
				textHowToPlay->transform.localScale = Vector3(1.0f, 1.0f, 0.0f);

				/** 黄色 */
				textStart->color.Set(Vector3(1.0f, 1.0f, 0.0f));
				/** スケール拡大 */
				textStart->transform.localScale = Vector3(1.3f, 1.3f, 0.0f);
			}
			else if (cursolIndex_ == 1
				&& textHowToPlay)
			{
				/** リセット: 黄色から白 */
				textStart->color.Set(Vector3(1.0f, 1.0f, 1.0f));
				/** リセット: 等倍に戻す */
				textStart->transform.localScale = Vector3(1.0f, 1.0f, 0.0f);
				/** リセット: 黄色から白 */
				textAward->color.Set(Vector3(1.0f, 1.0f, 1.0f));
				/** リセット: 等倍に戻す */
				textAward->transform.localScale = Vector3(1.0f, 1.0f, 0.0f);

				/** 黄色 */
				textHowToPlay->color.Set(Vector3(1.0f, 1.0f, 0.0f));
				/** スケール拡大 */
				textHowToPlay->transform.localScale = Vector3(1.3f, 1.3f, 0.0f);
			}
			else if (cursolIndex_ == 2
				&& textAward)
			{
				/** リセット: 黄色から白 */
				textHowToPlay->color.Set(Vector3(1.0f, 1.0f, 1.0f));
				/** リセット: 等倍に戻す */
				textHowToPlay->transform.localScale = Vector3(1.0f, 1.0f, 0.0f);
				/** リセット: 黄色から白 */
				textExit->color.Set(Vector3(1.0f, 1.0f, 1.0f));
				/** リセット: 等倍に戻す */
				textExit->transform.localScale = Vector3(1.0f, 1.0f, 0.0f);

				/** 黄色 */
				textAward->color.Set(Vector3(1.0f, 1.0f, 0.0f));
				/** スケール拡大 */
				textAward->transform.localScale = Vector3(1.3f, 1.3f, 0.0f);
			}
			else if (cursolIndex_ == 3
				&& textExit)
			{
				/** リセット: 黄色から白 */
				textAward->color.Set(Vector3(1.0f, 1.0f, 1.0f));
				/** リセット: 等倍に戻す */
				textAward->transform.localScale = Vector3(1.0f, 1.0f, 0.0f);

				/** 黄色 */
				textExit->color.Set(Vector3(1.0f, 1.0f, 0.0f));
				/** スケール拡大 */
				textExit->transform.localScale = Vector3(1.3f, 1.3f, 0.0f);
			}
		}

		void TitleMenu::InitializeLogic()
		{
			// サウンドバーの位置情報設定
			// アニメーションとかいれたり
			/** カーソルUI */
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