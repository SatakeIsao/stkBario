#include "stdafx.h"
#include "BattleSequence.h"
#include "ui/UIAnimationFactory.h"
#include "ui/UIAnimation.h"
#include "ui/AwardManager.h"
#include "battle/BattleManager.h"
#include "sound/SoundManager.h"
#include "core/ParameterManager.h"


namespace app
{
	namespace ui
	{
		BattleSequence::BattleSequence()
		{
			// タイマー初期値をパラメーターから設定
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::BattleSequenceParameter>();
			delayTimer_ = p->delayWaitTime;
			maxCountDownTimer_ = p->readyDisplayTime;
			goTimer_ = p->goDisplayTime;
			gameOverTimer_ = 0.0f;
			gameClearTimer_ = 0.0f;
			timeUpTimer_ = 0.0f;

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
		{}

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
				if (goTimer_ <= app::core::ParameterManager::Get().GetParameter<app::core::BattleSequenceParameter>()->goFadeStartTime
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
				auto* p_seq = app::core::ParameterManager::Get().GetParameter<app::core::BattleSequenceParameter>();
				if (menu)
				{
					auto* gameOverIcon = menu->GetUI<UIIcon>(Hash32("GameOver"));

					// バウンド中の処理
					if (bounceState_ != BounceState::enStop)
					{
						gameOverTimer_ += g_gameTime->GetFrameDeltaTime();

						// 最初の落下(FirstDown)完了 (JSONのduration: 0.30秒)
						if (bounceState_ == BounceState::enFirstDown && gameOverTimer_ >= p_seq->gameoverFirstDown)
						{
							bounceState_ = BounceState::enFirstUp;
							/** リセット */
							gameOverTimer_ = 0.0f;

							gameOverIcon->RemoveAnimation(Hash32("GameOverBounceDownY"));
							app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(gameOverIcon, Hash32("GameOverBounceUpY"));
							auto* anim = gameOverIcon->FindAnimation(Hash32("GameOverBounceUpY"));
							/** 効果音再生 */
							app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::GameOver));
							if (anim) anim->Play();
						}
						// 1回目の上昇(FirstUp)完了 (JSONのduration: 0.20秒)
						else if (bounceState_ == BounceState::enFirstUp && gameOverTimer_ >= p_seq->gameoverFirstUp)
						{
							bounceState_ = BounceState::enSecondDown;
							/** リセット */
							gameOverTimer_ = 0.0f;

							gameOverIcon->RemoveAnimation(Hash32("GameOverBounceUpY"));
							app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(gameOverIcon, Hash32("GameOverBounceSecondDownY"));
							auto* anim = gameOverIcon->FindAnimation(Hash32("GameOverBounceSecondDownY"));
							if (anim) anim->Play();
						}
						// 2回目の落下(SecondDown)完了 (JSONのduration: 0.20秒)
						else if (bounceState_ == BounceState::enSecondDown && gameOverTimer_ >= p_seq->gameoverSecondDown)
						{
							bounceState_ = BounceState::enSecondUp;
							/** リセット */
							gameOverTimer_ = 0.0f;

							gameOverIcon->RemoveAnimation(Hash32("GameOverBounceSecondDownY"));
							app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(gameOverIcon, Hash32("GameOverBounceSecondUpY"));
							auto* anim = gameOverIcon->FindAnimation(Hash32("GameOverBounceSecondUpY"));
							if (anim) anim->Play();
						}
						// 2回目の上昇(SecondUp)完了 (JSONのduration: 0.15秒)
						else if (bounceState_ == BounceState::enSecondUp && gameOverTimer_ >= p_seq->gameoverSecondUp)
						{
							bounceState_ = BounceState::enThirdDown;
							/** リセット */
							gameOverTimer_ = 0.0f;

							gameOverIcon->RemoveAnimation(Hash32("GameOverBounceSecondUpY"));
							app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(gameOverIcon, Hash32("GameOverBounceThirdDownY"));
							auto* anim = gameOverIcon->FindAnimation(Hash32("GameOverBounceThirdDownY"));
							if (anim) anim->Play();
						}
						// 3回目の落下(ThirdDown)完了 (JSONのduration: 0.15秒)
						else if (bounceState_ == BounceState::enThirdDown && gameOverTimer_ >= p_seq->gameoverThirdDown)
						{
							bounceState_ = BounceState::enStop;

							auto* anim = gameOverIcon->FindAnimation(Hash32("GameOverBounceThirdDownY"));
							if (anim) anim->Stop();
							app::battle::BattleManager::Get().SetGameOverAnimFinished(true);
						}
					}
				}
			}
			else if (currentDown_ == SequenceName::GameClear)
			{
				auto* p_seq = app::core::ParameterManager::Get().GetParameter<app::core::BattleSequenceParameter>();
				if (menu)
				{
					auto* gameClearIcon = menu->GetUI<UIIcon>(Hash32("GameClear"));
					if (gameClearIcon)
					{
						// ゲームクリアUIのステート処理
						if (gameClearState_ != GameClearState::enStop)
						{
							gameClearTimer_ += g_gameTime->GetFrameDeltaTime();

							// 拡大(PopUp)完了 (JSONのduration: 0.20秒)
							if (gameClearState_ == GameClearState::enPopUp && gameClearTimer_ >= p_seq->gameclearPopUpTime)
							{
								gameClearState_ = GameClearState::enShrinkBack;
								/** リセット */
								gameClearTimer_ = 0.0f;

								gameClearIcon->RemoveAnimation(Hash32("GameClearScalePopUp"));
								app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(gameClearIcon, Hash32("GameClearScaleShrink"));
								auto* anim = gameClearIcon->FindAnimation(Hash32("GameClearScaleShrink"));
								if (anim) anim->Play();
							}
							// 縮小完了
							else if (gameClearState_ == GameClearState::enShrinkBack && gameClearTimer_ >= p_seq->gameclearShrinkTime)
							{
								gameClearState_ = GameClearState::enWait;
								/** リセット */
								gameClearTimer_ = 0.0f;

								auto* anim = gameClearIcon->FindAnimation(Hash32("GameClearScaleShrink"));
								if (anim) anim->Stop();
							}
							// 待機完了
							else if (gameClearState_ == GameClearState::enWait && gameClearTimer_ >= p_seq->gameclearWaitTime)
							{
								gameClearState_ = GameClearState::enStop;
								/** リセット */
								gameClearTimer_ = 0.0f;

								// ここでリザルト画面への遷移や、フェードアウトアニメーションを再生する
								app::battle::BattleManager::Get().SetGameClearAnimFinished(true);

								if (app::ui::AwardManager::IsAvailable()) {
									auto& battleMgr = app::battle::BattleManager::Get();

									// クリアタイムを計算 (120秒制限の場合)
									float maxTime = 120.0f;
									float clearTime = maxTime - battleMgr.GetRemainTime();

									// マネージャーに最終報告
									app::ui::AwardManager::Get().CheckResultAwards(
										battleMgr.GetPlayerHP(),  // 残りHP
										clearTime,                // かかった時間
										p_seq->stageMaxSlimes,         // ステージのスライム総数
										p_seq->stageMaxCoins,          // ステージのコイン総数
										battleMgr.GetTotalCoin()  // 実際に集めたコイン数
									);
								}
							}
						}
					}
				}
			}
			else if (currentDown_ == SequenceName::TimeUp)
			{
				auto* p_seq = app::core::ParameterManager::Get().GetParameter<app::core::BattleSequenceParameter>();
				if (menu)
				{
					auto* timeUpIcon = menu->GetUI<UIIcon>(Hash32("TimeUp"));
					if (timeUpIcon)
					{
						if (timeUpState_ != TimeUpState::enStop)
						{
							timeUpTimer_ += g_gameTime->GetFrameDeltaTime();

							// スライドイン（行き過ぎ）完了
							if (timeUpState_ == TimeUpState::enSlideIn && timeUpTimer_ >= p_seq->timeupSlideInTime)
							{
								timeUpState_ = TimeUpState::enSlideBack;
								timeUpTimer_ = 0.0f;

								timeUpIcon->RemoveAnimation(Hash32("TimeUpSlideIn"));
								app::ui::UIAnimationFactory::Attach<app::ui::UITranslateAniamtion>(timeUpIcon, Hash32("TimeUpSlideBack"));
								auto* anim = timeUpIcon->FindAnimation(Hash32("TimeUpSlideBack"));
								/** 効果音再生 */
								app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::TimeUp));
								if (anim) anim->Play();
							}
							// 戻り完了（その後一定時間待つ）
							else if (timeUpState_ == TimeUpState::enSlideBack && timeUpTimer_ >= p_seq->timeupSlideBackTime)
							{
								timeUpState_ = TimeUpState::enWait;
								timeUpTimer_ = 0.0f;

								auto* anim = timeUpIcon->FindAnimation(Hash32("TimeUpSlideBack"));
								if (anim) anim->Stop();
							}
							// アニメーション完了後、2秒ほど待ってからシーン遷移フラグを立てる
							else if (timeUpState_ == TimeUpState::enWait && timeUpTimer_ >= p_seq->timeupWaitTime)
							{
								timeUpState_ = TimeUpState::enStop;
								app::battle::BattleManager::Get().SetTimeUpAnimFinished(true);
							}
						}
					}
				}
			}


			// 毎フレーム安全にUIを取得し、表示状態を上書き
			if (menu)
			{
				auto* readyIcon = menu->GetUI<UIIcon>(Hash32("Ready"));
				auto* goIcon = menu->GetUI<UIIcon>(Hash32("Go"));
				auto* gameOverIcon = menu->GetUI<UIIcon>(Hash32("GameOver"));
				auto* gameClearIcon = menu->GetUI<UIIcon>(Hash32("GameClear"));
				auto* timeUpIcon = menu->GetUI<UIIcon>(Hash32("TimeUp"));

				// isDraw フラグを使って物理的に描画をON/OFF
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
			// 何度も呼ばれないように、現在の状態がGameOverでない時だけ実行
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

				// 最初のアニメーション（PopUp）をここでアタッチして再生開始
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