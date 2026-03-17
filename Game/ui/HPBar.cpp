#include "stdafx.h"
#include "HPBar.h"
#include "ui/Layout.h"
#include "ui/UIAnimationFactory.h"
#include "ui/UIAnimation.h"


namespace
{
	static const int MAX_HP = 8;
	static const float BOUNCE_DURACTION = 0.30f;
}

namespace app
{
	namespace ui
	{
		HPBarObject::HPBarObject()
		{
			layout_ = std::make_unique<app::ui::Layout>();
			layout_->Initialize <app::ui::MenuBase>("Assets/ui/layout/hpLayout.json");
		}

		HPBarObject::~HPBarObject()
		{
		}

		void HPBarObject::Update()
		{
			/** インデックス数に応じて表示/非表示切り替え */
			if (layout_) {
				auto* menu = layout_->GetMenu();
				if (menu) {
					/** 青色 */
					Vector3 targetColor = Vector3(0.0f, 0.569f, 1.0f);
					if (currentHP_ < 3) {
						/** 緑色 */
						targetColor = Vector3(1.0f, 0.0f, 0.0f);
					}
					else if (currentHP_ < 5) {
						/** 黄色 */
						targetColor = Vector3(1.0f, 1.0f, 0.0f);
					}
					else if (currentHP_ < 7) {
						/** 赤色 */
						targetColor = Vector3(0.0f, 1.0f, 0.0f);
					}

					for (int i = 1; i <= MAX_HP; i++) {
						/** 文字列を作成 */
						std::string name = "HPBar_" + std::to_string(i);

						/** Hash32でキー化してUIを取得 */
						uint32_t key = Hash32(name.c_str());
						/** UIIconとして取得 */
						auto* barIcon = menu->GetUI<app::ui::UIIcon>(key);

						if (barIcon) {

							/** 現在のHP以下なら表示、それより大きければ非表示 */
							bool isVisible = (i <= currentHP_);

							/** 色をイージングで変える場合は以下のように */
							float targetAlpha = isVisible ? 1.0f : 0.15f;
							float currentAlpha = barIcon->color.w;
							float newAlpha = currentAlpha + (targetAlpha - currentAlpha) * 0.1f;
							barIcon->color.w = newAlpha;

							barIcon->color.x = targetColor.x;
							barIcon->color.y = targetColor.y;
							barIcon->color.z = targetColor.z;
						}
					}
				}
				layout_->Update();
			}
		}

		void HPBarObject::Render(RenderContext& rc)
		{
			if (layout_) {
				layout_->Render(rc);
			}
		}




		/******************************************************/


		CoinUIObject::CoinUIObject()
		{
			layout_ = std::make_unique<app::ui::Layout>();
			layout_->Initialize<app::ui::MenuBase>("Assets/ui/layout/coinLayout.json");
		}

		CoinUIObject::~CoinUIObject()
		{
		}
	
		void CoinUIObject::Update()
		{
			auto* menu = layout_->GetMenu();
			if (menu)
			{
				auto coinDigit = menu->GetUI<app::ui::UIDigit>(Hash32("coinNumbers"));
				if (coinDigit)
				{
					coinDigit->SetNumber(currentCoin_);
					// バウンド中の処理
					if (bounceState_ != BounceState::enStop)
					{
						bounceTime_ += g_gameTime->GetFrameDeltaTime();

						if (bounceState_ == BounceState::enUp 
							&& bounceTime_ >= 0.15f)
						{
							bounceState_ = BounceState::enDown;

							// 上昇アニメーションを止める
							auto* animUp = coinDigit->FindAnimation(Hash32("CoinBounceUpY"));
							if (animUp)
							{
								animUp->Stop();
							}

							// 下降アニメーションをアタッチして再生
							auto* animDown = coinDigit->FindAnimation(Hash32("CoinBounceDownY"));
							if (!animDown)
							{
								coinDigit->RemoveAnimation(Hash32("CoinBounceUpY"));
								coinDigit->RemoveAnimation(Hash32("CoinBounceDownY"));
								app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(coinDigit, Hash32("CoinBounceDownY"));
								animDown = coinDigit->FindAnimation(Hash32("CoinBounceDownY"));
							}
							if (animDown)
							{
								animDown->Play();
							}
						}
						else if (bounceState_ == BounceState::enDown 
							&& bounceTime_ >= 0.30f)
						{
							bounceState_ = BounceState::enStop;

							// 下降アニメーションを止める
							auto* animDown = coinDigit->FindAnimation(Hash32("CoinBounceDownY"));
							if (animDown)
							{
								animDown->Stop();
							}
						}
					}
				}
			}
			layout_->Update();
		}

		void CoinUIObject::Render(RenderContext& rc)
		{
			if (layout_) {
				layout_->Render(rc);
			}
		}

		void CoinUIObject::GetPlayAnimation()
		{
			auto* menu = layout_->GetMenu();
			if (menu)
			{
				auto coinDigit = menu->GetUI<app::ui::UIDigit>(Hash32("coinNumbers"));
				if (coinDigit)
				{
					// 上昇用のアニメーションをアタッチして再生
					auto* animUp = coinDigit->FindAnimation(Hash32("CoinBounceUpY"));
					if (!animUp)
					{
						coinDigit->RemoveAnimation(Hash32("CoinBounceUpY"));
						coinDigit->RemoveAnimation(Hash32("CoinBounceDownY"));
						app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(coinDigit, Hash32("CoinBounceUpY"));
						animUp = coinDigit->FindAnimation(Hash32("CoinBounceUpY"));
					}

					if (animUp)
					{
						animUp->Play();
						bounceState_ = BounceState::enUp;
						bounceTime_ = 0.0f;
					}
				}
			}
		}
	}
}