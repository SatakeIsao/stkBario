#include "stdafx.h"
#include "TitleMenu.h"
#include "core/ParameterManager.h"
#include "sound/SoundManager.h"
#include "ui/Layout.h"
#include "ui/UIAnimationFactory.h"
#include "ui/UIAnimation.h"

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

			if (isOpenJustNow_)
			{
				isOpenJustNow_ = false;
			}

			if (currentState_ == MenuState::enTitle)
			{
				/** Aボタン開いたらメニューが開く */
				if (g_pad[0]->IsTrigger(enButtonA))
				{
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
				textTitle->transform.localScale = Vector3(0.3f, 0.3f, 1.0f);
				textTitle->transform.localPosition = Vector3(0.0f, 360.0f, 0.0f);
			}
			if (textStart)
			{
				textStart->isDraw = true;
			}
			if (textHowToPlay)
			{
				textHowToPlay->isDraw = true;
			}
			if (textAward)
			{
				textAward->isDraw = true;
			}
			if (textExit)
			{
				textExit->isDraw = true;
			}
			if (cursol)
			{
				cursol->isDraw = true;
			}

			/** 表示 */
			if (buttonA)
			{
				buttonA->isDraw = false;
			}
			if (textPush)
			{
				textPush->isDraw = false;
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
				textTitle->transform.localScale = Vector3(1.0f, 1.0f, 1.0f);
				textTitle->transform.localPosition = Vector3(0.0f, 50.0f, 0.0f);
			}
			if (textStart)
			{
				textStart->isDraw = false;
			}
			if (textHowToPlay)
			{
				textHowToPlay->isDraw = false;
			}
			if (textAward)
			{
				textAward->isDraw = false;
			}
			if (textExit)
			{
				textExit->isDraw = false;
			}
			if (cursol)
			{
				cursol->isDraw = false;
			}

			/** 非表示 */
			if (buttonA)
			{
				buttonA->isDraw = true;
			}
			if (textPush)
			{
				textPush->isDraw = true;
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
			/** キャンバス（UI全体) */
			//{
			//	auto* canvas = GetCanvas();
			//	if (canvas)
			//	{
			//		canvas->transform.localScale = Vector3::One;
			//	}
			//}
		}
	}
}