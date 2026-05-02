#include "stdafx.h"
#include "InGameUI.h"
#include "ui/Layout.h"
#include "ui/UIAnimationFactory.h"
#include "ui/UIAnimation.h"
#include "battle/BattleManager.h"
#include "core/ParameterManager.h"



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
		{}

		void HPBarObject::Update()
		{
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::InGameUiParameter>();
			const Vector3 p_color_normal(p->colorHpMaxX, p->colorHpMaxY, p->colorHpMaxZ);
			const Vector3 p_color_caution(p->colorCautionX, p->colorCautionY, p->colorCautionZ);
			const Vector3 p_color_danger(p->colorDangerX, p->colorDangerY, p->colorDangerZ);
			const Vector3 p_color_safe(p->colorSafeX, p->colorSafeY, p->colorSafeZ);
			const Vector4 p_color_highscore(p->colorHighscoreX, p->colorHighscoreY, p->colorHighscoreZ, p->colorHighscoreW);
			const Vector4 p_color_default(p->colorDefaultX, p->colorDefaultY, p->colorDefaultZ, p->colorDefaultW);
			const Vector3 p_scale_default(p->scaleDefaultX, p->scaleDefaultY, p->scaleDefaultZ);

			/** インデックス数に応じて表示/非表示切り替え */
			if (layout_) {
				auto* menu = layout_->GetMenu();
				if (menu) {
					/** 青色 */
					Vector3 targetColor = p_color_normal;
					if (currentHP_ < p->thresholdLow) {
						/** 緑色 */
						targetColor = p_color_danger;
					}
					else if (currentHP_ < p->thresholdMid) {
						/** 黄色 */
						targetColor = p_color_caution;
					}
					else if (currentHP_ < p->thresholdHigh) {
						/** 赤色 */
						targetColor = p_color_safe;
					}

					for (int i = 1; i <= p->maxHp; i++) {
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
							float targetAlpha = isVisible ? p->alphaActive : p->alphaInactive;
							float currentAlpha = barIcon->color.w;
							float newAlpha = currentAlpha + (targetAlpha - currentAlpha) * p->lerpSpeed;
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
		{}

		void CoinUIObject::Update()
		{
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::InGameUiParameter>();
			const Vector3 p_color_normal(p->colorHpMaxX, p->colorHpMaxY, p->colorHpMaxZ);
			const Vector3 p_color_caution(p->colorCautionX, p->colorCautionY, p->colorCautionZ);
			const Vector3 p_color_danger(p->colorDangerX, p->colorDangerY, p->colorDangerZ);
			const Vector3 p_color_safe(p->colorSafeX, p->colorSafeY, p->colorSafeZ);
			const Vector4 p_color_highscore(p->colorHighscoreX, p->colorHighscoreY, p->colorHighscoreZ, p->colorHighscoreW);
			const Vector4 p_color_default(p->colorDefaultX, p->colorDefaultY, p->colorDefaultZ, p->colorDefaultW);
			const Vector3 p_scale_default(p->scaleDefaultX, p->scaleDefaultY, p->scaleDefaultZ);

			auto* menu = layout_->GetMenu();
			if (menu)
			{
				auto coinDigit = menu->GetUI<app::ui::UIDigit>(Hash32("coinNumbers"));
				if (coinDigit)
				{
					coinDigit->SetZeroPadding(true);
					coinDigit->SetNumber(currentCoin_);

					if (currentCoin_ >= p->colorChangeCount)
					{
						/** ハイスコアで黄色を増す */
						coinDigit->color = p_color_highscore;
					}

					// バウンド中の処理
					if (bounceState_ != BounceState::enStop)
					{
						bounceTime_ += g_gameTime->GetFrameDeltaTime();

						if (bounceState_ == BounceState::enUp
							&& bounceTime_ >= p->bounceUpLimit)
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
							&& bounceTime_ >= p->bounceTotalDuration)
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




		/******************************************************/


		TimerUIObject::TimerUIObject()
		{
			layout_ = std::make_unique<app::ui::Layout>();
			layout_->Initialize<app::ui::MenuBase>("Assets/ui/layout/timerLayout.json");

			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::InGameUiParameter>();
			timer_ = p->maxTime;
		}

		TimerUIObject::~TimerUIObject()
		{}

		void TimerUIObject::Update()
		{
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::InGameUiParameter>();
			const Vector3 p_color_normal(p->colorHpMaxX, p->colorHpMaxY, p->colorHpMaxZ);
			const Vector3 p_color_caution(p->colorCautionX, p->colorCautionY, p->colorCautionZ);
			const Vector3 p_color_danger(p->colorDangerX, p->colorDangerY, p->colorDangerZ);
			const Vector3 p_color_safe(p->colorSafeX, p->colorSafeY, p->colorSafeZ);
			const Vector4 p_color_highscore(p->colorHighscoreX, p->colorHighscoreY, p->colorHighscoreZ, p->colorHighscoreW);
			const Vector4 p_color_default(p->colorDefaultX, p->colorDefaultY, p->colorDefaultZ, p->colorDefaultW);
			const Vector3 p_scale_default(p->scaleDefaultX, p->scaleDefaultY, p->scaleDefaultZ);

			// 0未満にならないように制限
			if (timer_ <= 0.0f) {
				timer_ = 0.0f;
			}

			// タイマーが巻き戻った時（リトライなど）にフラグをリセット
			if (timer_ > p->triggerPulse100) hasPulsed100_ = false;
			if (timer_ > p->triggerPulse50)  hasPulsed50_ = false;
			if (timer_ > p->triggerPulse30)  hasPulsed30_ = false;

			// 100秒になった瞬間
			if (timer_ <= p->triggerPulse100 && !hasPulsed100_) {
				hasPulsed100_ = true;
				bounceState_ = BounceState::enUp;
				bounceTime_ = 0.0f;
			}
			// 50秒になった瞬間
			else if (timer_ <= p->triggerPulse50 && !hasPulsed50_) {
				hasPulsed50_ = true;
				bounceState_ = BounceState::enUp;
				bounceTime_ = 0.0f;
			}
			// 30秒になった瞬間
			else if (timer_ <= p->triggerPulse30 && !hasPulsed30_) {
				hasPulsed30_ = true;
				bounceState_ = BounceState::enUp;
				bounceTime_ = 0.0f;
			}

			// パルスの状態更新（0.2秒で拡大、さらに0.2秒で縮小して停止）
			if (bounceState_ != BounceState::enStop)
			{
				bounceTime_ += g_gameTime->GetFrameDeltaTime();

				if (bounceState_ == BounceState::enUp && bounceTime_ >= p->pulseUpDuration) {
					bounceState_ = BounceState::enDown;
				}
				else if (bounceState_ == BounceState::enDown && bounceTime_ >= p->pulseTotalDuration) {
					// 停止
					bounceState_ = BounceState::enStop;
				}
			}

			/** UIの描画とアニメーション制御 */
			auto* menu = layout_->GetMenu();
			if (menu)
			{
				/** 数字 (timerDigit) の処理 */
				{
					auto timerDigit = menu->GetUI<app::ui::UIDigit>(Hash32("timerNumbers"));
					if (timerDigit)
					{
						// 数値のセット
						timerDigit->SetZeroPadding(true);
						timerDigit->SetNumber(static_cast<int>(std::ceil(timer_)));

						// スケールアニメーション
						if (bounceState_ == BounceState::enUp)
						{
							auto* animDown = timerDigit->FindAnimation(Hash32("timerPulseScaleDown"));
							if (animDown && animDown->IsPlay()) animDown->Stop();

							auto* animUp = timerDigit->FindAnimation(Hash32("timerPulseScaleUp"));
							if (!animUp) {
								app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(timerDigit, Hash32("timerPulseScaleUp"));
								animUp = timerDigit->FindAnimation(Hash32("timerPulseScaleUp"));
							}
							if (animUp && !animUp->IsPlay()) animUp->Play();
						}
						else if (bounceState_ == BounceState::enDown)
						{
							auto* animUp = timerDigit->FindAnimation(Hash32("timerPulseScaleUp"));
							if (animUp && animUp->IsPlay()) animUp->Stop();

							auto* animDown = timerDigit->FindAnimation(Hash32("timerPulseScaleDown"));
							if (!animDown) {
								app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(timerDigit, Hash32("timerPulseScaleDown"));
								animDown = timerDigit->FindAnimation(Hash32("timerPulseScaleDown"));
							}
							if (animDown && !animDown->IsPlay()) animDown->Play();
						}
						else // BounceState::enStop
						{
							auto* animUp = timerDigit->FindAnimation(Hash32("timerPulseScaleUp"));
							if (animUp && animUp->IsPlay()) animUp->Stop();
							auto* animDown = timerDigit->FindAnimation(Hash32("timerPulseScaleDown"));
							if (animDown && animDown->IsPlay()) animDown->Stop();

							timerDigit->transform.scale = Vector3::One;
						}


						/** 点滅アニメーション */
						auto* animRed = timerDigit->FindAnimation(Hash32("timerRedFlash"));
						auto* animFast = timerDigit->FindAnimation(Hash32("timerFastBlink"));

						// 拡大・縮小の最中 (ドクン！とやっている間)
						if (bounceState_ != BounceState::enStop)
						{
							if (animRed && animRed->IsPlay()) animRed->Stop(); // 通常点滅は止める

							// 高速チカチカアニメーションを再生
							if (!animFast) {
								app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(timerDigit, Hash32("timerFastBlink"));
								animFast = timerDigit->FindAnimation(Hash32("timerFastBlink"));
							}
							if (animFast && !animFast->IsPlay()) animFast->Play();
						}
						// 拡大・縮小が終わった後 (30秒以下で、enStopになった後)
						else if (app::battle::BattleManager::Get().IsBlinking())
						{
							if (animFast && animFast->IsPlay()) animFast->Stop(); // 高速チカチカを止める

							// 通常の赤点滅を再生
							if (!animRed) {
								app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(timerDigit, Hash32("timerRedFlash"));
								animRed = timerDigit->FindAnimation(Hash32("timerRedFlash"));
							}
							if (animRed && !animRed->IsPlay()) animRed->Play();
						}
						// 通常時 (31秒以上、または0秒)
						else
						{
							if (animRed && animRed->IsPlay()) animRed->Stop();
							if (animFast && animFast->IsPlay()) animFast->Stop();

							if (timer_ <= 0.0f) {
								timerDigit->color = p_color_danger;	// 0秒は赤固定
							}
							else {
								timerDigit->color = p_color_default;	// 通常は白固定
							}
						}
					}
				}


				/** アイコン (timerUI) の処理 */
				{
					auto timerUI = menu->GetUI<app::ui::UIIcon>(Hash32("timerUI"));
					if (timerUI)
					{
						// スケール(拡大/縮小)アニメーション
						if (bounceState_ == BounceState::enUp)
						{
							auto* animDown = timerUI->FindAnimation(Hash32("timerPulseScaleDown"));
							if (animDown && animDown->IsPlay()) animDown->Stop();

							auto* animUp = timerUI->FindAnimation(Hash32("timerPulseScaleUp"));
							if (!animUp) {
								app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(timerUI, Hash32("timerPulseScaleUp"));
								animUp = timerUI->FindAnimation(Hash32("timerPulseScaleUp"));
							}
							if (animUp && !animUp->IsPlay()) animUp->Play();
						}
						else if (bounceState_ == BounceState::enDown)
						{
							auto* animUp = timerUI->FindAnimation(Hash32("timerPulseScaleUp"));
							if (animUp && animUp->IsPlay()) animUp->Stop();

							auto* animDown = timerUI->FindAnimation(Hash32("timerPulseScaleDown"));
							if (!animDown) {
								app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(timerUI, Hash32("timerPulseScaleDown"));
								animDown = timerUI->FindAnimation(Hash32("timerPulseScaleDown"));
							}
							if (animDown && !animDown->IsPlay()) animDown->Play();
						}
						else // BounceState::enStop
						{
							auto* animUp = timerUI->FindAnimation(Hash32("timerPulseScaleUp"));
							if (animUp && animUp->IsPlay()) animUp->Stop();
							auto* animDown = timerUI->FindAnimation(Hash32("timerPulseScaleDown"));
							if (animDown && animDown->IsPlay()) animDown->Stop();

							timerUI->transform.scale = p_scale_default; // スケールを1.0に戻す
						}


						// 色 (点滅) アニメーション
						auto* animRed = timerUI->FindAnimation(Hash32("timerRedFlash"));
						auto* animFast = timerUI->FindAnimation(Hash32("timerFastBlink"));

						// 拡大・縮小の最中 (ドクン！とやっている間)
						if (bounceState_ != BounceState::enStop)
						{
							if (animRed && animRed->IsPlay()) animRed->Stop();

							if (!animFast) {
								app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(timerUI, Hash32("timerFastBlink"));
								animFast = timerUI->FindAnimation(Hash32("timerFastBlink"));
							}
							if (animFast && !animFast->IsPlay()) animFast->Play();
						}
						// 拡大・縮小が終わった後 (30秒以下で、enStopになった後)
						else if (app::battle::BattleManager::Get().IsBlinking())
						{
							if (animFast && animFast->IsPlay()) animFast->Stop();

							if (!animRed) {
								app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(timerUI, Hash32("timerRedFlash"));
								animRed = timerUI->FindAnimation(Hash32("timerRedFlash"));
							}
							if (animRed && !animRed->IsPlay()) animRed->Play();
						}
						// 通常時 (31秒以上、または0秒)
						else
						{
							if (animRed && animRed->IsPlay()) animRed->Stop();
							if (animFast && animFast->IsPlay()) animFast->Stop();

							if (timer_ <= 0.0f) {
								timerUI->color = p_color_danger;	// 0秒は赤固定
							}
							else {
								timerUI->color = p_color_default; // 通常は白固定
							}
						}
					}
				}

			}
			layout_->Update();
		}

		void TimerUIObject::Render(RenderContext& rc)
		{
			if (layout_) {
				layout_->Render(rc);
			}
		}
	}
}