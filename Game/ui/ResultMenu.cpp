#include "stdafx.h"
#include "ResultMenu.h"
#include "core/ParameterManager.h"
#include "ui/Layout.h"
#include "ui/UIAnimationFactory.h"
#include "ui/UIAnimation.h"
#include "ui/InGameUI.h"
#include "scene/SceneManager.h"
#include "AwardManager.h"
#include "battle/BattleManager.h"
#include "sound/SoundManager.h"



namespace app
{
	namespace ui
	{
		ResultMenu::ResultMenu()
		{
			/** ゲームオーバーレイアウト */
			{
				layout_ = std::make_unique<app::ui::Layout>();
				layout_->Initialize<app::ui::MenuBase>("Assets/ui/layout/resultMenu.json");
			}
			InitializeLogic();
		}

		ResultMenu::~ResultMenu()
		{}

		void ResultMenu::Update()
		{
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::ResultMenuParameter>();
			const Vector3 p_selection_color(p->selectionColorX, p->selectionColorY, p->selectionColorZ);
			const Vector3 p_selection_scale(p->selectionScaleX, p->selectionScaleY, p->selectionScaleZ);
			const Vector3 p_default_scale(p->defaultScaleX, p->defaultScaleY, p->defaultScaleZ);
			const Vector3 p_default_color(p->defaultColorX, p->defaultColorY, p->defaultColorZ);
			const Vector4 p_bonus_text_color(p->bonusTextColorX, p->bonusTextColorY, p->bonusTextColorZ, p->bonusTextColorW);
			/** 処理負荷を考えてなんとかしたい */
			if (seq_)   seq_->Update(g_gameTime->GetFrameDeltaTime());
			if (seqTextNext_) seqTextNext_->Update(g_gameTime->GetFrameDeltaTime());
			if (seqAward_)   seqAward_->Update(g_gameTime->GetFrameDeltaTime());
			if (seqtimeMinutes_)  seqtimeMinutes_->Update(g_gameTime->GetFrameDeltaTime());
			if (seqtimeSeconds_)  seqtimeSeconds_->Update(g_gameTime->GetFrameDeltaTime());
			if (seqCoinNumbers_)  seqCoinNumbers_->Update(g_gameTime->GetFrameDeltaTime());
			if (seqSlimeNumbers_) seqSlimeNumbers_->Update(g_gameTime->GetFrameDeltaTime());
			if (seqScoreNumbers_) seqScoreNumbers_->Update(g_gameTime->GetFrameDeltaTime());

			// 順番にアニメーションを再生するステートマシン
			if (state_ != ResultState::Finished) {
				sequenceTimer_ += g_gameTime->GetFrameDeltaTime();

				// タイマーを強制的に進めてスキップ
				if (g_pad[0]->IsTrigger(enButtonA)) {
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
					sequenceTimer_ = 2.0f;
				}

				auto menu = layout_->GetMenu();
				if (menu) {
					// タイム
					if (state_ == ResultState::Init && sequenceTimer_ >= p->nextStepDelay) {
						state_ = ResultState::Time;
						sequenceTimer_ = 0.0f;
						auto timeNumbers = menu->GetUI<app::ui::UIDigit>(Hash32("timeNumbers"));
						timeNumbers->SetZeroPadding(true);
						if (timeNumbers && seqtimeMinutes_) seqtimeMinutes_->Play(timeNumbers);
						app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Pop));
					}
					// コイン
					else if (state_ == ResultState::Time && sequenceTimer_ >= p->nextStepDelay) {
						state_ = ResultState::Coin;
						sequenceTimer_ = 0.0f;
						auto coinNumbers = menu->GetUI<app::ui::UIDigit>(Hash32("coinNumbers"));
						if (coinNumbers && seqCoinNumbers_) seqCoinNumbers_->Play(coinNumbers);
						app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Pop));
					}
					// スライム
					else if (state_ == ResultState::Coin && sequenceTimer_ >= p->nextStepDelay) {
						state_ = ResultState::Slime;
						sequenceTimer_ = 0.0f;
						auto slimeNumbers = menu->GetUI<app::ui::UIDigit>(Hash32("slimeNumbers"));
						if (slimeNumbers && seqSlimeNumbers_) seqSlimeNumbers_->Play(slimeNumbers);
						app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Pop));
					}
					// スコア
					else if (state_ == ResultState::Slime && sequenceTimer_ >= p->nextStepDelay) {
						state_ = ResultState::Score;
						sequenceTimer_ = 0.0f;
						auto scoreNumbers = menu->GetUI<app::ui::UIDigit>(Hash32("scoreNumbers"));
						scoreNumbers->SetZeroPadding(true);
						if (scoreNumbers && seqScoreNumbers_) seqScoreNumbers_->Play(scoreNumbers);
						app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Pop));
					}
					// アワード
					else if (state_ == ResultState::Score && sequenceTimer_ >= p->awardStepDelay) {
						state_ = ResultState::Award;
						sequenceTimer_ = 0.0f;
						// 固定のアイコンではなく、記憶しておいたランダムなアイコンを再生する
						if (randomAwardIcon_ && seqAward_) seqAward_->Play(randomAwardIcon_);
						app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Pop));
					}
					// ボタンを表示して完了
					else if (state_ == ResultState::Award && sequenceTimer_ >= p->awardStepDelay) {
						state_ = ResultState::Finished;
						sequenceTimer_ = 0.0f;
						auto buttonA = menu->GetUI<app::ui::UIIcon>(Hash32("buttonA"));
						auto text_next = menu->GetUI<app::ui::UIIcon>(Hash32("text_next"));
						if (buttonA && seq_) seq_->Play(buttonA);
						if (text_next && seq_) seq_->Play(text_next);
					}
				}
			}
			// Finishedになった後もタイマーを進める（シーン遷移の誤爆バッファ用）
			else {
				sequenceTimer_ += g_gameTime->GetFrameDeltaTime();
			}
			layout_->Update();
		}

		void ResultMenu::Render(RenderContext& rc)
		{
			if (layout_) {
				layout_->Render(rc);
			}
		}

		void ResultMenu::OnOpen()
		{}

		void ResultMenu::OnClose()
		{}

		void ResultMenu::PlaySelectedAnimation()
		{
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::ResultMenuParameter>();
			const Vector3 p_selection_color(p->selectionColorX, p->selectionColorY, p->selectionColorZ);
			const Vector3 p_selection_scale(p->selectionScaleX, p->selectionScaleY, p->selectionScaleZ);
			const Vector3 p_default_scale(p->defaultScaleX, p->defaultScaleY, p->defaultScaleZ);
			const Vector3 p_default_color(p->defaultColorX, p->defaultColorY, p->defaultColorZ);
			/** TODO: Updateで、処理が走っているので、無駄な処理を改善したい */
			auto textRetry = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_retry"));
			auto textTitle = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_ReturnToTitle"));

			/** 各項目選択中に拡大アニメーションを再生 */
			if (cursolIndex_ == 0
				&& textRetry)
			{
				/** リセット: 黄色から白 */
				textTitle->color.Set(p_default_color);
				/** リセット: 等倍に戻す */
				textTitle->transform.localScale = p_default_scale;

				/** 黄色 */
				textRetry->color.Set(p_selection_color);
				/** スケール拡大 */
				textRetry->transform.localScale = p_selection_scale;
			}
			else if (cursolIndex_ == 1
				&& textTitle)
			{
				/** リセット: 黄色から白 */
				textRetry->color.Set(p_default_color);
				/** リセット: 等倍に戻す */
				textRetry->transform.localScale = p_default_scale;

				/** 黄色 */
				textTitle->color.Set(p_selection_color);
				/** スケール拡大 */
				textTitle->transform.localScale = p_selection_scale;
			}
		}

		void ResultMenu::ShowAwardUI(AwardType awardType)
		{
			auto* menu = layout_->GetMenu();
			if (!menu) return;

			// 1. 表示するUIの名前（文字列）を入れる箱を用意
			const char* uiName = nullptr;

			// 2. 称号の種類に合わせて、UIの名前だけを決定する
			switch (awardType)
			{
			case AwardType::enComplete:         uiName = "text_award_Complete"; break;
			case AwardType::enCoinMaster:       uiName = "text_award_CoinMaster"; break;
			case AwardType::enForgetful:        uiName = "text_award_Forgetful"; break;
			case AwardType::enSlimeKiller:      uiName = "text_award_SlimeKiller"; break;
			case AwardType::enGentleWorld:      uiName = "text_award_GentleWorld"; break;
			case AwardType::enLifeIsPrecious:   uiName = "text_award_LifeIsPrecious"; break;
			case AwardType::enJumpingFrog:      uiName = "text_award_JumpingFrog"; break;
			case AwardType::enBouncingRabbit:   uiName = "text_award_BouncingRabbit"; break;
			case AwardType::enRelaxedPerson:    uiName = "text_award_RelaxedPerson"; break;
			case AwardType::enSpeedStar:        uiName = "text_award_SpeedStar"; break;
			case AwardType::enSoundPlay:        uiName = "text_award_SoundPlay"; break;
			case AwardType::enChallengerHeart:  uiName = "text_award_ChallengerHeart"; break;
			case AwardType::enTimeStopper:      uiName = "text_award_TimeStopper"; break;
			case AwardType::enBeardedMan:       uiName = "text_award_BeardedMan"; break;
			case AwardType::enLifeMax:          uiName = "text_award_LifeMax"; break;
			default: break;
			}

			// 3. 最後に1回だけ、決定した名前のUIを探して表示をONにする！
			if (uiName != nullptr)
			{
				auto* awardUI = menu->GetUI<app::ui::UIIcon>(Hash32(uiName));
				if (awardUI) awardUI->isDraw = true;
			}
		}

		void ResultMenu::InitializeLogic()
		{
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::ResultMenuParameter>();
			const Vector4 p_bonus_text_color(p->bonusTextColorX, p->bonusTextColorY, p->bonusTextColorZ, p->bonusTextColorW);
			// サウンドバーの位置情報設定
			// アニメーションとかいれたり
			auto menu = layout_->GetMenu();
			if (!menu) return;

			// アニメーションで出現するまでUIを非表示(スケール0)にしておく
			auto buttonA = menu->GetUI<app::ui::UIIcon>(Hash32("buttonA"));
			auto text_next = menu->GetUI<app::ui::UIIcon>(Hash32("text_next"));
			auto timeNumbersBackground = menu->GetUI<app::ui::UIDigit>(Hash32("timeNumbersBackground"));
			auto timeNumbers = menu->GetUI<app::ui::UIDigit>(Hash32("timeNumbers"));
			auto coinNumbers = menu->GetUI<app::ui::UIDigit>(Hash32("coinNumbers"));
			auto slimeNumbers = menu->GetUI<app::ui::UIDigit>(Hash32("slimeNumbers"));
			auto scoreNumbers = menu->GetUI<app::ui::UIDigit>(Hash32("scoreNumbers"));

			if (buttonA) buttonA->color.w = 0.0f;
			if (text_next) text_next->color.w = 0.0f;
			if (timeNumbersBackground) timeNumbersBackground->transform.localScale = Vector3::Zero;
			if (timeNumbers) timeNumbers->transform.localScale = Vector3::Zero;
			if (coinNumbers) coinNumbers->transform.localScale = Vector3::Zero;
			if (slimeNumbers) slimeNumbers->transform.localScale = Vector3::Zero;
			if (scoreNumbers) scoreNumbers->transform.localScale = Vector3::Zero;

			// コイン数をここで1回だけセットする
			if (coinNumbers) {
				int coin = SceneManager::Get().GetTotalCoin();
				coinNumbers->SetNumber(coin);
			}

			/** 残りタイムを経過タイムに変換 */
			if (timeNumbers) {
				const int timer = SceneManager::Get().GetCurrentTimer();

				const int masterTimer = p->masterMaxTime - timer;
				timeNumbers->SetNumber(masterTimer);
			}

			/** スコアの計算 */
			if (scoreNumbers
				&& coinNumbers
				&& timeNumbers) {
				int coin = SceneManager::Get().GetTotalCoin();
				coinNumbers->SetNumber(coin);

				const int timer = SceneManager::Get().GetCurrentTimer();
				int masterTimer = p->masterMaxTime - timer;
				timeNumbers->SetNumber(masterTimer);

				int timerBonus = 0.0f;

				if (masterTimer <= p->thresholdTimeRankS)
				{
					timerBonus = p->timeBonusRankS;

					timeNumbers->color = p_bonus_text_color;
				}
				else if (masterTimer <= p->thresholdTimeRankA)
				{
					timerBonus = p->timeBonusRankA;
				}
				else if (masterTimer <= p->thresholdTimeRankB)
				{
					timerBonus = p->timeBonusRankB;
				}
				else if (masterTimer <= p->thresholdTimeRankC)
				{
					timerBonus = p->timeBonusRankC;
				}

				coin *= p->coinScoreWeight;

				const int finalScore = (coin + timerBonus);
				scoreNumbers->SetNumber(finalScore);
			}

			/** カーソルUI */
			{
				auto buttonA_Icon = layout_->GetMenu()->GetUI<app::ui::UIIcon>(Hash32("buttonA"));
				if (buttonA_Icon)
				{
					// アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(buttonA_Icon, Hash32("FadeIn"));

					seq_ = std::make_unique<app::ui::UIAnimationSequence>();
					seq_->Add(Hash32("FadeIn"));
				}
			}

			/** もどるUI */
			{
				auto text_next_Icon = layout_->GetMenu()->GetUI<app::ui::UIIcon>(Hash32("text_next"));
				if (text_next_Icon)
				{
					// アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(text_next_Icon, Hash32("FadeIn"));

					if (!seq_) {
						seq_ = std::make_unique<app::ui::UIAnimationSequence>();
						seq_->Add(Hash32("FadeIn"));
					}
				}
			}

			/** 経過タイム背景表示UI */
			{
				auto timeNumbersBackground_Icon = layout_->GetMenu()->GetUI<app::ui::UIDigit>(Hash32("timeNumbersBackground"));
				if (timeNumbersBackground_Icon)
				{
					timeNumbersBackground_Icon->SetZeroPadding(true);
					// アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(timeNumbersBackground_Icon, Hash32("ScaleUp_BackgroundDigits"));
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(timeNumbersBackground_Icon, Hash32("ScaleDown_BackgroundDigits"));

					seqtimeBackGroundSeconds_ = std::make_unique<app::ui::UIAnimationSequence>();
					seqtimeBackGroundSeconds_->Add(Hash32("ScaleUp_BackgroundDigits"));
					seqtimeBackGroundSeconds_->Add(Hash32("ScaleDown_BackgroundDigits"));
				}
			}
			/** 経過タイム表示UI */
			{
				auto timeNumbers_Icon = layout_->GetMenu()->GetUI<app::ui::UIDigit>(Hash32("timeNumbers"));
				if (timeNumbers_Icon)
				{
					timeNumbers_Icon->SetZeroPadding(true);
					// アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(timeNumbers_Icon, Hash32("ScaleUp_digits"));
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(timeNumbers_Icon, Hash32("ScaleDown_digits"));

					seqtimeMinutes_ = std::make_unique<app::ui::UIAnimationSequence>();
					seqtimeMinutes_->Add(Hash32("ScaleUp_digits"));
					seqtimeMinutes_->Add(Hash32("ScaleDown_digits"));
				}
			}
			/** コイン数字 */
			{
				auto coinNumbers_Icon = layout_->GetMenu()->GetUI<app::ui::UIDigit>(Hash32("coinNumbers"));
				if (coinNumbers_Icon)
				{
					// アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(coinNumbers_Icon, Hash32("ScaleUp_digits"));
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(coinNumbers_Icon, Hash32("ScaleDown_digits"));

					seqCoinNumbers_ = std::make_unique<app::ui::UIAnimationSequence>();
					seqCoinNumbers_->Add(Hash32("ScaleUp_digits"));
					seqCoinNumbers_->Add(Hash32("ScaleDown_digits"));
				}
			}
			/** スライム数字 */
			{
				auto slimeNumbers_Icon = layout_->GetMenu()->GetUI<app::ui::UIDigit>(Hash32("slimeNumbers"));
				if (slimeNumbers_Icon)
				{
					// アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(slimeNumbers_Icon, Hash32("ScaleUp_digits"));
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(slimeNumbers_Icon, Hash32("ScaleDown_digits"));

					seqSlimeNumbers_ = std::make_unique<app::ui::UIAnimationSequence>();
					seqSlimeNumbers_->Add(Hash32("ScaleUp_digits"));
					seqSlimeNumbers_->Add(Hash32("ScaleDown_digits"));
				}
			}
			/** スコア数字 */
			{
				auto scoreNumbers_Icon = layout_->GetMenu()->GetUI<app::ui::UIDigit>(Hash32("scoreNumbers"));
				if (scoreNumbers_Icon)
				{
					scoreNumbers_Icon->SetZeroPadding(true);
					// アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(scoreNumbers_Icon, Hash32("ScaleUp_digits"));
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(scoreNumbers_Icon, Hash32("ScaleDown_digits"));

					seqScoreNumbers_ = std::make_unique<app::ui::UIAnimationSequence>();
					seqScoreNumbers_->Add(Hash32("ScaleUp_digits"));
					seqScoreNumbers_->Add(Hash32("ScaleDown_digits"));
				}
			}

			/** アワード (ランダム選出＆アニメーション設定) */
			if (app::ui::AwardManager::IsAvailable())
			{
				AwardType randomAward;

				// 解禁済みの称号をランダムに取得する（※1つも無い場合は if の中に入らない）
				if (app::ui::AwardManager::Get().GetRandomUnlockedAward(randomAward))
				{
					// 該当するUIを取得して変数に保持
					randomAwardIcon_ = GetRandomAwardUI(randomAward);

					if (randomAwardIcon_)
					{
						// 最初は見えないようにスケール0にする
						randomAwardIcon_->transform.localScale = Vector3::Zero;

						// 選ばれたアイコンにアニメーションをアタッチ！
						app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(randomAwardIcon_, Hash32("ScaleUp_digits"));
						app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(randomAwardIcon_, Hash32("ScaleDown_digits"));

						seqAward_ = std::make_unique<app::ui::UIAnimationSequence>();
						seqAward_->Add(Hash32("ScaleUp_digits"));
						seqAward_->Add(Hash32("ScaleDown_digits"));
					}
				}
			}
		}

		app::ui::UIIcon* ResultMenu::GetRandomAwardUI(AwardType awardType)
		{
			auto* menu = layout_->GetMenu();
			if (!menu) return nullptr;

			const char* uiName = nullptr;
			switch (awardType)
			{
			case AwardType::enComplete:         uiName = "text_award_complete"; break;
			case AwardType::enCoinMaster:       uiName = "text_award_coinMaster"; break;
			case AwardType::enForgetful:        uiName = "text_award_forgetful"; break;
			case AwardType::enSlimeKiller:      uiName = "text_award_slimeKiller"; break;
			case AwardType::enGentleWorld:      uiName = "text_award_gentleWorld"; break;
			case AwardType::enLifeIsPrecious:   uiName = "text_award_life"; break;
			case AwardType::enJumpingFrog:      uiName = "text_award_jumpingFrog"; break;
			case AwardType::enBouncingRabbit:   uiName = "text_award_jumpingRabbit"; break;
			case AwardType::enRelaxedPerson:    uiName = "text_award_laidBack"; break;
			case AwardType::enSpeedStar:        uiName = "text_award_speedStar"; break;
			case AwardType::enSoundPlay:        uiName = "text_award_soundPlay"; break;
			case AwardType::enChallengerHeart:  uiName = "text_award_challengeHeart"; break;
			case AwardType::enTimeStopper:      uiName = "text_award_stopTime"; break;
			case AwardType::enBeardedMan:       uiName = "text_award_beardedMan"; break;
			case AwardType::enLifeMax:          uiName = "text_award_fullOfEnergy"; break;
			default: return nullptr;
			}
			return menu->GetUI<app::ui::UIIcon>(Hash32(uiName));
		}
	}
}