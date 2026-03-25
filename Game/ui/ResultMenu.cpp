#include "stdafx.h"
#include "ResultMenu.h"
#include "core/ParameterManager.h"
#include "ui/Layout.h"
#include "ui/UIAnimationFactory.h"
#include "ui/UIAnimation.h"
#include "ui/HPBar.h"
#include "scene/SceneManager.h"

namespace 
{
	constexpr const int MAX_TIME = 120.0f;
}


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
			
			/** 使う所パッと思いつかないので一旦コメントアウト */
			//app::core::ParameterManager::Get().LoadParameter<app::core::GameOverMenuParameter>("Assets/master/GameOverMenuParameter.json", [](const nlohmann::json& j, app::core::GameOverMenuParameter& p)
			//	{
			//		//TODO; X座標もやりたいなぁ
			//		p.cursolPositionX[0] = j["cursolPositionXA"];
			//		p.cursolPositionX[1] = j["cursolPositionXB"];
			//
			//		p.cursolPositionY[0] = j["cursolPositionYA"];
			//		p.cursolPositionY[1] = j["cursolPositionYB"];
			//	});
		}

		ResultMenu::~ResultMenu()
		{}

		void ResultMenu::Update()
		{
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
						sequenceTimer_ = 2.0f;
					}

				auto menu = layout_->GetMenu();
				if (menu) {
					// タイム
					if (state_ == ResultState::Init && sequenceTimer_ >= 0.5f) {
						state_ = ResultState::Time;
						sequenceTimer_ = 0.0f;
						auto timeNumbers = menu->GetUI<app::ui::UIDigit>(Hash32("timeNumbers"));
						//auto time2Numbers = menu->GetUI<app::ui::UIDigit>(Hash32("time2Numbers"));
						timeNumbers->SetZeroPadding(true);
						if (timeNumbers && seqtimeMinutes_) seqtimeMinutes_->Play(timeNumbers);
						//if (time2Numbers && seqtimeSeconds_) seqtimeSeconds_->Play(time2Numbers);
					}
					// コイン
					else if (state_ == ResultState::Time && sequenceTimer_ >= 0.5f) {
						state_ = ResultState::Coin;
						sequenceTimer_ = 0.0f;
						auto coinNumbers = menu->GetUI<app::ui::UIDigit>(Hash32("coinNumbers"));
						if (coinNumbers && seqCoinNumbers_) seqCoinNumbers_->Play(coinNumbers);
					}
					// スライム
					else if (state_ == ResultState::Coin && sequenceTimer_ >= 0.5f) {
						state_ = ResultState::Slime;
						sequenceTimer_ = 0.0f;
						auto slimeNumbers = menu->GetUI<app::ui::UIDigit>(Hash32("slimeNumbers"));
						if (slimeNumbers && seqSlimeNumbers_) seqSlimeNumbers_->Play(slimeNumbers);
					}
					// スコア
					else if (state_ == ResultState::Slime && sequenceTimer_ >= 0.5f) {
						state_ = ResultState::Score;
						sequenceTimer_ = 0.0f;
						auto scoreNumbers = menu->GetUI<app::ui::UIDigit>(Hash32("scoreNumbers"));
						scoreNumbers->SetZeroPadding(true);
						if (scoreNumbers && seqScoreNumbers_) seqScoreNumbers_->Play(scoreNumbers);
					}
					// アワード
					else if (state_ == ResultState::Score && sequenceTimer_ >= 1.0f) {
						state_ = ResultState::Award;
						sequenceTimer_ = 0.0f;
						auto text_award = menu->GetUI<app::ui::UIIcon>(Hash32("text_award_coinMaster"));
						if (text_award && seqAward_) seqAward_->Play(text_award);
					}
					// ボタンを表示して完了
					else if (state_ == ResultState::Award && sequenceTimer_ >=1.0f) {
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

		void ResultMenu::InitializeLogic()
		{
			// サウンドバーの位置情報設定
			// アニメーションとかいれたり
			auto menu = layout_->GetMenu();
			if (!menu) return;

			// アニメーションで出現するまでUIを非表示(スケール0)にしておく
			auto buttonA = menu->GetUI<app::ui::UIIcon>(Hash32("buttonA"));
			auto text_award = menu->GetUI<app::ui::UIIcon>(Hash32("text_award_coinMaster"));
			auto text_next = menu->GetUI<app::ui::UIIcon>(Hash32("text_next"));
			auto timeNumbers = menu->GetUI<app::ui::UIDigit>(Hash32("timeNumbers"));
			auto coinNumbers = menu->GetUI<app::ui::UIDigit>(Hash32("coinNumbers"));
			auto slimeNumbers = menu->GetUI<app::ui::UIDigit>(Hash32("slimeNumbers"));
			auto scoreNumbers = menu->GetUI<app::ui::UIDigit>(Hash32("scoreNumbers"));

			if (buttonA) buttonA->color.w = 0.0f;
			if (text_award) text_award->transform.localScale = Vector3::Zero;
			if (text_next) text_next->color.w = 0.0f;
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

				const int masterTimer = MAX_TIME - timer;
				timeNumbers->SetNumber(masterTimer);
			}

			/** スコアの計算 */
			if (scoreNumbers
				&& coinNumbers
				&& timeNumbers) {
				int coin = SceneManager::Get().GetTotalCoin();
				coinNumbers->SetNumber(coin);

				const int timer = SceneManager::Get().GetCurrentTimer();
				int masterTimer = MAX_TIME - timer;
				timeNumbers->SetNumber(masterTimer);

				/** しきい値 */
				const int test = 100.0f;
				int test2 = 0.0f;

				if (masterTimer <= 30.0f)
				{
					test2 = 3000.0f;
					timeNumbers->color = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
				}
				else if (masterTimer <= 60.0f)
				{
					test2 = 2500.0f;
				}
				else if (masterTimer <= 90.0f)
				{
					test2 = 2000.0f;
				}
				else if (masterTimer <= 120.0f)
				{
					test2 = 1000.0f;
				}

				coin *= test;

				const int memory = (coin + test2);
				scoreNumbers->SetNumber(memory);
			}

			/** カーソルUI */
			{
				auto buttonA = layout_->GetMenu()->GetUI<app::ui::UIIcon>(Hash32("buttonA"));
				if (buttonA)
				{
					// アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(buttonA, Hash32("FadeIn"));

					seq_ = std::make_unique<app::ui::UIAnimationSequence>();
					seq_->Add(Hash32("FadeIn"));
				}
			}

			/** アワード */
			{
				auto text_award_coinMaster = layout_->GetMenu()->GetUI<app::ui::UIIcon>(Hash32("text_award_coinMaster"));
				if (text_award_coinMaster)
				{
					// アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(text_award_coinMaster, Hash32("ScaleUp_digits"));
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(text_award_coinMaster, Hash32("ScaleDown_digits"));
					
					seqAward_ = std::make_unique<app::ui::UIAnimationSequence>();
					seqAward_->Add(Hash32("ScaleUp_digits"));
					seqAward_->Add(Hash32("ScaleDown_digits"));
				}
			}
			/** もどるUI */
			{
				auto text_next = layout_->GetMenu()->GetUI<app::ui::UIIcon>(Hash32("text_next"));
				if (text_next)
				{
					// アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(text_next, Hash32("FadeIn"));

					seq_ = std::make_unique<app::ui::UIAnimationSequence>();
					seq_->Add(Hash32("FadeIn"));
				}
			}
			/** 経過タイム表示UI */
			{
				auto timeNumbers = layout_->GetMenu()->GetUI<app::ui::UIDigit>(Hash32("timeNumbers"));
				if (timeNumbers)
				{
					timeNumbers->SetZeroPadding(true);
					// アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(timeNumbers, Hash32("ScaleUp_digits"));
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(timeNumbers, Hash32("ScaleDown_digits"));

					seqtimeMinutes_ = std::make_unique<app::ui::UIAnimationSequence>();
					seqtimeMinutes_->Add(Hash32("ScaleUp_digits"));
					seqtimeMinutes_->Add(Hash32("ScaleDown_digits"));
				}
			}
			/** コイン数字 */
			{
				auto coinNumbers = layout_->GetMenu()->GetUI<app::ui::UIDigit>(Hash32("coinNumbers"));
				if (coinNumbers)
				{
					// アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(coinNumbers, Hash32("ScaleUp_digits"));
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(coinNumbers, Hash32("ScaleDown_digits"));

					seqCoinNumbers_ = std::make_unique<app::ui::UIAnimationSequence>();
					seqCoinNumbers_->Add(Hash32("ScaleUp_digits"));
					seqCoinNumbers_->Add(Hash32("ScaleDown_digits"));
				}
			}
			/** スライム数字 */
			{
				auto slimeNumbers = layout_->GetMenu()->GetUI<app::ui::UIDigit>(Hash32("slimeNumbers"));
				if (slimeNumbers)
				{
					// アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(slimeNumbers, Hash32("ScaleUp_digits"));
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(slimeNumbers, Hash32("ScaleDown_digits"));

					seqSlimeNumbers_ = std::make_unique<app::ui::UIAnimationSequence>();
					seqSlimeNumbers_->Add(Hash32("ScaleUp_digits"));
					seqSlimeNumbers_->Add(Hash32("ScaleDown_digits"));
				}
			}
			/** スコア数字 */
			{
				auto scoreNumbers = layout_->GetMenu()->GetUI<app::ui::UIDigit>(Hash32("scoreNumbers"));
				if (scoreNumbers)
				{
					scoreNumbers->SetZeroPadding(true);
					// アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(scoreNumbers, Hash32("ScaleUp_digits"));
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(scoreNumbers, Hash32("ScaleDown_digits"));

					seqScoreNumbers_ = std::make_unique<app::ui::UIAnimationSequence>();
					seqScoreNumbers_->Add(Hash32("ScaleUp_digits"));
					seqScoreNumbers_->Add(Hash32("ScaleDown_digits"));
				}
			}
		}
	}
}