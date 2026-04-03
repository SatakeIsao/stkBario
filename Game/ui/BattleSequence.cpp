#include "stdafx.h"
#include "BattleSequence.h"
#include "ui/UIAnimationFactory.h"
#include "ui/UIAnimation.h"
#include "ui/AwardManager.h"
#include "battle/BattleManager.h"
#include "sound/SoundManager.h"

namespace
{
	static app::ui::UIAnimationSequence* seq = nullptr;
}

namespace app
{
	namespace ui
	{
		BattleSequence::BattleSequence()
		{
			layout_ = std::make_unique<app::ui::Layout>();
			layout_->Initialize<app::ui::MenuBase>("Assets/ui/layout/BattleSequenceMenuLayout.json");
			currentDown_ = SequenceName::Wait;

			auto* menu = layout_->GetMenu();
			if (menu)
			{
				auto* readyIcon = menu->GetUI<UIIcon>(Hash32("Ready"));
				if (readyIcon)
				{
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(readyIcon, Hash32("ScaleUp_Ready"));
				}
			}
		}

		BattleSequence::~BattleSequence()
		{
		}

		void BattleSequence::Update()
		{
			if (seq_)   seq_->Update(g_gameTime->GetFrameDeltaTime());
			// 【重要】先にレイアウトを更新（ここでホットリロードによるUI再生成が行われるため）
			layout_->Update();
			auto* menu = layout_->GetMenu();

			// タイマーの更新
			if (currentDown_ == SequenceName::Wait)
			{
				delayTimer_ -= g_gameTime->GetFrameDeltaTime();
				if (delayTimer_ <= 0.0f)
				{
					currentDown_ = SequenceName::Ready;

					if (menu)
					{
						auto* readyIcon = menu->GetUI<UIIcon>(Hash32("Ready"));
						if (readyIcon)
						{
							//seq = new app::ui::UIAnimationSequence();
							//seq->Add(Hash32("ScaleUp_Ready"));
							//seq->Play(readyIcon);

							/** DEBUG_TEST: UIAnimシーケンスで再生したい */
							{
								app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(readyIcon, Hash32("ScaleUp_Ready"));
								auto* anim = readyIcon->FindAnimation(Hash32("ScaleUp_Ready"));
								if (anim) anim->Play();
								/** TODO: あとでSE変更 */
								app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Ready));
							}
							{
								app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(readyIcon, Hash32("FadeOut2"));
								auto* anim = readyIcon->FindAnimation(Hash32("FadeOut2"));
								if (anim) anim->Play();
							}
						}
					}
				}
			}
			else if (currentDown_ == SequenceName::Ready)
			{
				maxCountDownTimer_ -= g_gameTime->GetFrameDeltaTime();
				if (maxCountDownTimer_ <= 0.0f)
				{
					currentDown_ = SequenceName::GO;
					hasPlayedGoFadeOut_ = false;
					if (menu)
					{
						auto* goIcon = menu->GetUI<UIIcon>(Hash32("Go"));
						if (goIcon)
						{
							app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(goIcon, Hash32("ScaleUp_Go"));
							auto* anim = goIcon->FindAnimation(Hash32("ScaleUp_Go"));
							if (anim) anim->Play();
							//効果音
							app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Go));
						}
					}

				}
			}
			else if (currentDown_ == SequenceName::GO)
			{
				goTimer_ -= g_gameTime->GetFrameDeltaTime();
				if (goTimer_ <= 1.0f
					&& !hasPlayedGoFadeOut_)
				{
					hasPlayedGoFadeOut_ = true;

					if (menu)
					{
						auto* goIcon = menu->GetUI<UIIcon>(Hash32("Go"));
						if (goIcon)
						{
							goIcon->RemoveAnimation(Hash32("ScaleUp_Go"));
							{
								app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(goIcon, Hash32("FadeOut"));
								auto* animFade = goIcon->FindAnimation(Hash32("FadeOut"));
								if (animFade) animFade->Play();
							}
							{
								app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(goIcon, Hash32("ScaleUp_Go2"));
								auto* animFade2 = goIcon->FindAnimation(Hash32("ScaleUp_Go2"));
								if (animFade2)animFade2->Play();
							}
						}
					}
				}

				if (goTimer_ <= 0.0f)
				{
					currentDown_ = SequenceName::Finished;
				}
			}
			else if (currentDown_ == SequenceName::GameOver)
			{
				if (menu)
				{
					auto* gameOverIcon = menu->GetUI<UIIcon>(Hash32("GameOver"));

					// バウンド中の処理
					if (bounceState_ != BounceState::enStop)
					{
						gameOverTimer_ += g_gameTime->GetFrameDeltaTime();

						// 1. 最初の落下(FirstDown)完了 (JSONのduration: 0.30秒)
						if (bounceState_ == BounceState::enFirstDown && gameOverTimer_ >= 0.30f)
						{
							bounceState_ = BounceState::enFirstUp;
							gameOverTimer_ = 0.0f; // ★ここが超重要！タイマーをリセット

							gameOverIcon->RemoveAnimation(Hash32("GameOverBounceDownY"));
							app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(gameOverIcon, Hash32("GameOverBounceUpY"));
							auto* anim = gameOverIcon->FindAnimation(Hash32("GameOverBounceUpY"));
							/** TODO: あとでSE変更 */
							app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::GameOver));
							if (anim) anim->Play();
						}
						// 2. 1回目の上昇(FirstUp)完了 (JSONのduration: 0.20秒)
						else if (bounceState_ == BounceState::enFirstUp && gameOverTimer_ >= 0.20f)
						{
							bounceState_ = BounceState::enSecondDown;
							gameOverTimer_ = 0.0f; // ★リセット

							gameOverIcon->RemoveAnimation(Hash32("GameOverBounceUpY"));
							app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(gameOverIcon, Hash32("GameOverBounceSecondDownY"));
							auto* anim = gameOverIcon->FindAnimation(Hash32("GameOverBounceSecondDownY"));
							if (anim) anim->Play();
						}
						// 3. 2回目の落下(SecondDown)完了 (JSONのduration: 0.20秒)
						else if (bounceState_ == BounceState::enSecondDown && gameOverTimer_ >= 0.20f)
						{
							bounceState_ = BounceState::enSecondUp;
							gameOverTimer_ = 0.0f; // ★リセット

							gameOverIcon->RemoveAnimation(Hash32("GameOverBounceSecondDownY"));
							app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(gameOverIcon, Hash32("GameOverBounceSecondUpY"));
							auto* anim = gameOverIcon->FindAnimation(Hash32("GameOverBounceSecondUpY"));
							if (anim) anim->Play();
						}
						// 4. 2回目の上昇(SecondUp)完了 (JSONのduration: 0.15秒)
						else if (bounceState_ == BounceState::enSecondUp && gameOverTimer_ >= 0.15f)
						{
							bounceState_ = BounceState::enThirdDown;
							gameOverTimer_ = 0.0f; // ★リセット

							gameOverIcon->RemoveAnimation(Hash32("GameOverBounceSecondUpY"));
							app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(gameOverIcon, Hash32("GameOverBounceThirdDownY"));
							auto* anim = gameOverIcon->FindAnimation(Hash32("GameOverBounceThirdDownY"));
							if (anim) anim->Play();
						}
						// 5. 3回目の落下(ThirdDown)完了 (JSONのduration: 0.15秒)
						else if (bounceState_ == BounceState::enThirdDown && gameOverTimer_ >= 1.15f)
						{
							bounceState_ = BounceState::enStop;

							auto* anim = gameOverIcon->FindAnimation(Hash32("GameOverBounceThirdDownY"));
							if (anim) anim->Stop();
							app::battle::BattleManager::Get().SetGameOverAnimFinished(true);
							//isGameOverAnimFinished_ = true;
							//currentDown_ = SequenceName::Finished;
						}
					}
				}
			}
			else if (currentDown_ == SequenceName::GameClear)
			{
				if (menu)
				{
					auto* gameClearIcon = menu->GetUI<UIIcon>(Hash32("GameClear"));
					if (gameClearIcon)
					{
						// ゲームクリアUIのステート処理
						if (gameClearState_ != GameClearState::enStop)
						{
							gameClearTimer_ += g_gameTime->GetFrameDeltaTime();

							// 1. 拡大(PopUp)完了 (JSONのduration: 0.20秒)
							if (gameClearState_ == GameClearState::enPopUp && gameClearTimer_ >= 1.20f)
							{
								gameClearState_ = GameClearState::enShrinkBack;
								gameClearTimer_ = 0.0f; // ★タイマーリセット

								gameClearIcon->RemoveAnimation(Hash32("GameClearScalePopUp"));
								app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(gameClearIcon, Hash32("GameClearScaleShrink"));
								auto* anim = gameClearIcon->FindAnimation(Hash32("GameClearScaleShrink"));
								if (anim) anim->Play();
							}
							// 縮小完了
							else if (gameClearState_ == GameClearState::enShrinkBack && gameClearTimer_ >= 0.85f)
							{
								gameClearState_ = GameClearState::enWait;
								gameClearTimer_ = 0.0f; // タイマーリセット

								auto* anim = gameClearIcon->FindAnimation(Hash32("GameClearScaleShrink"));
								if (anim) anim->Stop();
							}
							// 待機完了
							else if (gameClearState_ == GameClearState::enWait && gameClearTimer_ >= 2.00f)
							{
								//isGameOverAnimFinished_ = true;
								gameClearState_ = GameClearState::enStop;
								gameClearTimer_ = 0.0f; // タイマーリセット
							
								// ここでリザルト画面への遷移や、フェードアウトアニメーションを再生する
								//currentDown_ = SequenceName::Finished;
								app::battle::BattleManager::Get().SetGameClearAnimFinished(true);

								if (app::ui::AwardManager::IsAvailable()) {
									auto& battleMgr = app::battle::BattleManager::Get();

									// クリアタイムを計算 (120秒制限の場合)
									float maxTime = 120.0f;
									float clearTime = maxTime - battleMgr.GetRemainTime();

									// ステージ内の最大数 (※仮の数字です。必要に応じて調整してください)
									const int maxSlimes = 6;
									const int maxCoins = 15;

									// マネージャーに最終報告！
									app::ui::AwardManager::Get().CheckResultAwards(
										battleMgr.GetPlayerHP(),  // 残りHP
										clearTime,                // かかった時間
										maxSlimes,                // ステージのスライム総数
										maxCoins,                 // ステージのコイン総数
										battleMgr.GetTotalCoin()  // 実際に集めたコイン数
									);
								}
							}
						}

						///// アニメーションをアタッチ
						///app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(gameClearIcon, Hash32("GameClearScalePopUp"));
						///app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(gameClearIcon, Hash32("GameClearScaleShrink"));
						///app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(gameClearIcon, Hash32("ScaleUp"));
						///
						///seq_ = std::make_unique<app::ui::UIAnimationSequence>();
						///seq_->Add(Hash32("GameClearScalePopUp"));
						///seq_->Add(Hash32("GameClearScaleShrink"));
						///
						///
						////** すべて表示されたら再生 */
						///// アニメーションを再生
						///seq_->Play(gameClearIcon);
						///
						/////gameClearIcon->RemoveAnimation(Hash32("ScaleUp_Go"));
						/////app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(gameClearIcon, Hash32("ScaleUp_Go"));
						/////auto* anim = gameClearIcon->FindAnimation(Hash32("ScaleUp_Go"));
						/////if (anim) anim->Play();
					}
				}

				//if (gameClearTimer_ <= 0.0f)
				//{
				//	currentDown_ = SequenceName::Wait;
				//}
			}
			else if (currentDown_ == SequenceName::TimeUp)
			{
				if (menu)
				{
					auto* timeUpIcon = menu->GetUI<UIIcon>(Hash32("TimeUp"));
					if (timeUpIcon)
					{
						if (timeUpState_ != TimeUpState::enStop)
						{
							timeUpTimer_ += g_gameTime->GetFrameDeltaTime();

							// 1. スライドイン（行き過ぎ）完了
							if (timeUpState_ == TimeUpState::enSlideIn && timeUpTimer_ >= 0.20f)
							{
								timeUpState_ = TimeUpState::enSlideBack;
								timeUpTimer_ = 0.0f;

								timeUpIcon->RemoveAnimation(Hash32("TimeUpSlideIn"));
								app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(timeUpIcon, Hash32("TimeUpSlideBack"));
								auto* anim = timeUpIcon->FindAnimation(Hash32("TimeUpSlideBack"));
								/** TODO: あとでSE変更 */
								app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::TimeUp));
								if (anim) anim->Play();
							}
							// 2. 戻り完了（その後一定時間待つ）
							else if (timeUpState_ == TimeUpState::enSlideBack && timeUpTimer_ >= 0.15f)
							{
								timeUpState_ = TimeUpState::enWait;
								timeUpTimer_ = 0.0f;

								auto* anim = timeUpIcon->FindAnimation(Hash32("TimeUpSlideBack"));
								if (anim) anim->Stop();
							}
							// 3. アニメーション完了後、2秒ほど待ってからシーン遷移フラグを立てる
							else if (timeUpState_ == TimeUpState::enWait && timeUpTimer_ >= 2.0f)
							{
								timeUpState_ = TimeUpState::enStop;
								app::battle::BattleManager::Get().SetTimeUpAnimFinished(true);
							}
						}
					}
				}
			}
			
			///** ゴールしたら */
			//if(g_pad[0]->IsTrigger(enButtonLB1))
			//{
			//	currentDown_ = SequenceName::GameClear;
			//	// ★追加：ステートとタイマーを初期化
			//	gameClearState_ = GameClearState::enPopUp;
			//	gameClearTimer_ = 0.0f;
			//
			//	// ★追加：最初のアニメーション（PopUp）をここでアタッチして再生開始！
			//	if (menu)
			//	{
			//		auto* gameClearIcon = menu->GetUI<UIIcon>(Hash32("GameClear"));
			//		if (gameClearIcon)
			//		{
			//			app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(gameClearIcon, Hash32("GameClearScalePopUp"));
			//			auto* anim = gameClearIcon->FindAnimation(Hash32("GameClearScalePopUp"));
			//			/** TODO: あとでSE変更 */
			//			app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
			//			if (anim) anim->Play();
			//		}
			//	}
			//}

			/** タイマーが0になったら */
			//if(g_pad[0]->IsTrigger(enButtonY)
			//	|| )
			//{
			//	currentDown_ = SequenceName::TimeUp;
			//}


			///** PlayerのHP0になったら */
			//if (g_pad[0]->IsTrigger(enButtonX)
			//	)
			//{
			//	GetPlayAnimation();
			//	currentDown_ = SequenceName::GameOver;
			//}


			// 毎フレーム安全にUIを取得し、表示状態を上書きする
			//auto* menu = layout_->GetMenu();
			if (menu)
			{
				// 事前計算したハッシュ値を使うので超高速
				auto* readyIcon = menu->GetUI<UIIcon>(Hash32("Ready"));
				auto* goIcon = menu->GetUI<UIIcon>(Hash32("Go"));
				auto* gameOverIcon = menu->GetUI<UIIcon>(Hash32("GameOver"));
				auto* gameClearIcon = menu->GetUI<UIIcon>(Hash32("GameClear"));
				auto* timeUpIcon = menu->GetUI<UIIcon>(Hash32("TimeUp"));

				// isDraw フラグを使って物理的に描画をON/OFFする
				if (currentDown_ == SequenceName::Wait || currentDown_ == SequenceName::Finished)
				{
					if (readyIcon) readyIcon->isDraw = false;
					if (goIcon) goIcon->isDraw = false;
					if (gameOverIcon) gameOverIcon->isDraw = false;
					if (gameClearIcon) gameClearIcon->isDraw = false;
					if (timeUpIcon) timeUpIcon->isDraw = false;
				}
				else if (currentDown_ == SequenceName::Ready)
				{
					if (readyIcon) readyIcon->isDraw = true;
					if (goIcon)    goIcon->isDraw = false;
					if (gameOverIcon) gameOverIcon->isDraw = false;
					if (gameClearIcon)    gameClearIcon->isDraw = false;
					if (timeUpIcon) timeUpIcon->isDraw = false;
				}
				else if (currentDown_ == SequenceName::GO)
				{
					if (readyIcon) readyIcon->isDraw = false;
					if (goIcon)    goIcon->isDraw = true;
					if (gameOverIcon) gameOverIcon->isDraw = false;
					if (gameClearIcon)    gameClearIcon->isDraw = false;
					if (timeUpIcon) timeUpIcon->isDraw = false;
				}
				else if (currentDown_ == SequenceName::GameOver)
				{
					if (readyIcon) readyIcon->isDraw = false;
					if (goIcon)    goIcon->isDraw = false;
					if (gameOverIcon) gameOverIcon->isDraw = true;
					if (gameClearIcon)    gameClearIcon->isDraw = false;
					if (timeUpIcon) timeUpIcon->isDraw = false;
				}
				else if (currentDown_ == SequenceName::GameClear)
				{
					if (readyIcon) readyIcon->isDraw = false;
					if (goIcon)    goIcon->isDraw = false;
					if (gameOverIcon) gameOverIcon->isDraw = false;
					if (gameClearIcon)    gameClearIcon->isDraw = true;
					if (timeUpIcon) timeUpIcon->isDraw = false;
				}
				else if (currentDown_ == SequenceName::TimeUp)
				{
					if (readyIcon) readyIcon->isDraw = false;
					if (goIcon)    goIcon->isDraw = false;
					if (gameOverIcon) gameOverIcon->isDraw = false;
					if (gameClearIcon)    gameClearIcon->isDraw = false;
					if (timeUpIcon) timeUpIcon->isDraw = true;
				}
			}
		}

		void BattleSequence::Render(RenderContext& rc)
		{
			if (layout_)
			{
				layout_->Render(rc);
			}
		}

		void BattleSequence::GetPlayAnimation()
		{
			auto* menu = layout_->GetMenu();
			if (menu)
			{
				auto gameOverIcon = menu->GetUI<app::ui::UIIcon>(Hash32("GameOver"));
				if (gameOverIcon)
				{
					// 上昇用のアニメーションをアタッチして再生
					auto* animUp = gameOverIcon->FindAnimation(Hash32("GameOverBounceUpY"));
					if (!animUp)
					{
						gameOverIcon->RemoveAnimation(Hash32("GameOverBounceUpY"));
						gameOverIcon->RemoveAnimation(Hash32("GameOverBounceDownY"));
						app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(gameOverIcon, Hash32("GameOverBounceDownY"));
						animUp = gameOverIcon->FindAnimation(Hash32("GameOverBounceDownY"));
					}

					if (animUp)
					{
						animUp->Play();
						bounceState_ = BounceState::enFirstDown;
						gameOverTimer_ = 0.0f;
					}
				}
			}
		}

		void BattleSequence::StartGameOver()
		{
			// 何度も呼ばれないように、現在の状態がGameOverでない時だけ実行する
			if (currentDown_ != SequenceName::GameOver)
			{
				currentDown_ = SequenceName::GameOver;
				GetPlayAnimation();
			}
		}

		void BattleSequence::StartTimeUp()
		{
			if (currentDown_ != SequenceName::TimeUp)
			{
				currentDown_ = SequenceName::TimeUp;

				// ステートとタイマーを初期化
				timeUpState_ = TimeUpState::enSlideIn;
				timeUpTimer_ = 0.0f;

				// 最初のアニメーション（スライドイン）をアタッチして再生
				auto* menu = layout_->GetMenu();
				if (menu) {
					auto* timeUpIcon = menu->GetUI<UIIcon>(Hash32("TimeUp"));
					if (timeUpIcon) {
						app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(timeUpIcon, Hash32("TimeUpSlideIn"));
						auto* anim = timeUpIcon->FindAnimation(Hash32("TimeUpSlideIn"));
						if (anim) anim->Play();
					}
				}
			}
		}
		void BattleSequence::StartGameClear()
		{
			if (currentDown_ != SequenceName::GameClear)
			{
				currentDown_ = SequenceName::GameClear;

				// ステートとタイマーを初期化
				gameClearState_ = GameClearState::enPopUp;
				gameClearTimer_ = 0.0f;

				// 最初のアニメーション（PopUp）をここでアタッチして再生開始！
				auto* menu = layout_->GetMenu();
				if (menu)
				{
					auto* gameClearIcon = menu->GetUI<UIIcon>(Hash32("GameClear"));
					if (gameClearIcon)
					{
						app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(gameClearIcon, Hash32("GameClearScalePopUp"));
						auto* anim = gameClearIcon->FindAnimation(Hash32("GameClearScalePopUp"));
						app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::GameClear));
						if (anim) anim->Play();
					}
				}
			}
		}
	}
}
