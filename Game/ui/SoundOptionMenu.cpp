#include "stdafx.h"
#include "SoundOptionMenu.h"
#include "battle/BattleManager.h"
#include "sound/SoundManager.h"
#include "core/ParameterManager.h"
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
		SoundOptionMenu::SoundOptionMenu()
			: volumeCursolIndex_(static_cast<int>(app::SoundManager::SoundVolumeType::Master))
		{
			// バイナリ経由で読み込み済みのためここでは何もしない
		}


		SoundOptionMenu::~SoundOptionMenu()
		{
			app::core::ParameterManager::Get().UnloadParameter<app::core::MasterSoundOptionMenuParameter>();
		}


		void SoundOptionMenu::Update()
		{
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::MasterSoundOptionMenuParameter>();
			auto* canvas = GetCanvas();
			if (canvas)
			{
				//閉じる
				{
					auto* closeAnim = canvas->FindAnimation(Hash32("ScaleDown_SoundMenu"));
					if (closeAnim && !closeAnim->IsPlay())
					{
						canvas->RemoveAnimation(Hash32("ScaleDown_SoundMenu"));
						closeAnim = nullptr;
						isPause_ = false;

						//ForEachUI([](app::ui::UIBase* ui)
						//	{
						//		ui->RemoveAnimation(Hash32("FadeOutPauseMenu"));
						//	});
					}
				}
				//開く
				{
					auto* openAnim = canvas->FindAnimation(Hash32("ScaleUp_SoundMenu"));
					if (openAnim && !openAnim->IsPlay())
					{
						canvas->RemoveAnimation(Hash32("ScaleUp_SoundMenu"));

						//ForEachUI([](app::ui::UIBase* ui) {
						//	ui->RemoveAnimation(Hash32("FadeInPauseMenu"));
						//	});
					}
				}
			}

			if (isPause_)
			{
				if (g_pad[0]->IsTrigger(enButtonUp))
				{
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));

					volumeCursolIndex_--;

					if (volumeCursolIndex_ < static_cast<int>(app::SoundManager::SoundVolumeType::Master))
					{
						volumeCursolIndex_ = static_cast<int>(app::SoundManager::SoundVolumeType::Master);
					}
				}
				else if (g_pad[0]->IsTrigger(enButtonDown))
				{
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));

					volumeCursolIndex_++;

					if (volumeCursolIndex_ >= static_cast<int>(app::SoundManager::SoundVolumeType::Max))
					{
						volumeCursolIndex_ = static_cast<int>(app::SoundManager::SoundVolumeType::SE);
					}
				}


				/** 左右キーで音量調整 */
				bool isVolumeChanged = false;

				auto currentVolumeType = static_cast<app::SoundManager::SoundVolumeType>(volumeCursolIndex_);
				float targetVolume = app::SoundManager::Get().GetVolume(currentVolumeType);

				if (g_pad[0]->IsTrigger(enButtonY))
				{
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
					targetVolume = 0.5f;
					isVolumeChanged = true;
				}
				if (g_pad[0]->IsTrigger(enButtonRight))
				{
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
					targetVolume += p->volumeStep;
					if (targetVolume > p->volumeMax)
					{
						targetVolume = p->volumeMax;
					}
					isVolumeChanged = true;
				}
				else if (g_pad[0]->IsTrigger(enButtonLeft))
				{
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
					targetVolume -= p->volumeStep;
					if (targetVolume < p->volumeMin)
					{
						targetVolume = p->volumeMin;
					}
					isVolumeChanged = true;
				}

				if (isVolumeChanged)
				{
					app::SoundManager::Get().SetVolume(currentVolumeType, targetVolume);

					if (app::ui::AwardManager::IsAvailable()) {
						app::ui::AwardManager::Get().OnSoundAdjusted();
					}
				}

				// 動的に数値をUIに設定
				auto* p = app::core::ParameterManager::Get().GetParameter<app::core::MasterSoundOptionMenuParameter>();

				// MASTER/BGM/SEの場所
				{
					const float y = (volumeCursolIndex_ == 0) ? p->gaugeBarYA : (volumeCursolIndex_ == 1) ? p->gaugeBarYB : p->gaugeBarYC;
					auto cursol = GetUI<UIIcon>(Hash32("Cursol"));
					cursol->transform.localPosition.y = y;
				}
				// --- MASTERのUI更新
				{
					const float volumeMaster = app::SoundManager::Get().GetVolume(app::SoundManager::SoundVolumeType::Master);
					// ゲージの大きさ
					{
						auto gaugeMASTER = GetUI<UIIcon>(Hash32("VolumeBar_MASTER"));
						gaugeMASTER->transform.localScale.x = volumeMaster;
					}
					// MASTERノブのX座標
					{
						const float minX = p->gaugeBarXA;
						const float maxX = p->gaugeBarXK;

						auto knobMASTER = GetUI<UIIcon>(Hash32("Knob_MASTER"));
						const float x = minX + ((minX - maxX) * volumeMaster);
						knobMASTER->transform.localPosition.x = x;
					}
					// MASTERノブ背景のX座標
					{
						const float minX = p->gaugeBarXA;
						const float maxX = p->gaugeBarXK;

						auto knobBackGroundMASTER = GetUI<UIIcon>(Hash32("KnobBackground_MASTER"));
						const float x = minX + ((minX - maxX) * volumeMaster);
						knobBackGroundMASTER->transform.localPosition.x = x;
					}
					//数値表示
					{
						auto digitMASTER = GetUI<UIDigit>(Hash32("VolumeDigit_MASTER"));
						if (digitMASTER) {
							digitMASTER->SetNumber(static_cast<int>(std::round(volumeMaster * p->volumeDisplayMultiplier)));
						}
					}
				}

				// --- BGMのUI更新
				{
					const float volumeBGM = app::SoundManager::Get().GetVolume(app::SoundManager::SoundVolumeType::BGM);
					// ゲージの大きさ
					{
						auto gaugeBGM = GetUI<UIIcon>(Hash32("VolumeBar_BGM"));
						gaugeBGM->transform.localScale.x = volumeBGM;
					}
					// BGMノブのX座標
					{
						const float minX = p->gaugeBarXA;
						const float maxX = p->gaugeBarXK;

						auto knobBGM = GetUI<UIIcon>(Hash32("Knob_BGM"));
						const float x = minX + ((minX - maxX) * volumeBGM);
						knobBGM->transform.localPosition.x = x;
					}
					// BGMノブ背景のX座標
					{
						const float minX = p->gaugeBarXA;
						const float maxX = p->gaugeBarXK;

						auto knobBackGroundBGM = GetUI<UIIcon>(Hash32("KnobBackground_BGM"));
						const float x = minX + ((minX - maxX) * volumeBGM);
						knobBackGroundBGM->transform.localPosition.x = x;
					}
					//数値表示
					{
						auto digitBGM = GetUI<UIDigit>(Hash32("VolumeDigit_BGM"));
						if (digitBGM) {
							digitBGM->SetNumber(static_cast<int>(std::round(volumeBGM * p->volumeDisplayMultiplier)));
						}
					}
				}

				// --- SEのUI更新
				{
					const float volumeSE = app::SoundManager::Get().GetVolume(app::SoundManager::SoundVolumeType::SE);
					// ゲージの大きさ
					{
						auto gaugeScaleSE = GetUI<UIIcon>(Hash32("VolumeBar_SE"));
						gaugeScaleSE->transform.localScale.x = volumeSE;
					}
					// SEノブのX座標
					{
						const float minX = p->gaugeBarXA;
						const float maxX = p->gaugeBarXK;

						// 四角の場所 ＝ minX + ((maxX - minX) * ボリュームパーセント)
						// ※ (maxX - minX) = 長さ

						auto knobSE = GetUI<UIIcon>(Hash32("Knob_SE"));
						const float x = minX + ((minX - maxX) * volumeSE);
						knobSE->transform.localPosition.x = x;
					}
					// SEノブ背景のX座標
					{
						const float minX = p->gaugeBarXA;
						const float maxX = p->gaugeBarXK;

						auto knobBackGroundSE = GetUI<UIIcon>(Hash32("KnobBackground_SE"));
						const float x = minX + ((minX - maxX) * volumeSE);
						knobBackGroundSE->transform.localPosition.x = x;
					}
					//数値の表示
					{
						auto digitSE = GetUI<UIDigit>(Hash32("VolumeDigit_SE"));
						if (digitSE) {
							digitSE->SetNumber(static_cast<int>(std::round(volumeSE * p->volumeDisplayMultiplier)));
						}
					}
				}
				PlaySelectedAnimation();
			}

			seq->Update(g_gameTime->GetFrameDeltaTime());

			MenuBase::Update();
		}


		void SoundOptionMenu::OnOpen()
		{
			isPause_ = true;

			//キャンバス
			{
				auto* canvas = GetCanvas();
				if (canvas)
				{
					canvas->RemoveAnimation(Hash32("ScaleUp_SoundMenu"));
					canvas->RemoveAnimation(Hash32("ScaleDown_SoundMenu"));
					//アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(canvas, Hash32("ScaleUp_SoundMenu"));
					auto* openAnim = canvas->FindAnimation(Hash32("ScaleUp_SoundMenu"));
					if (openAnim) openAnim->Play();
				}
			}

			//各UIパーツの一斉フェードイン
			{
				ForEachUI([](app::ui::UIBase* ui)
					{
						ui->RemoveAnimation(Hash32("FadeInPauseMenu"));
						ui->RemoveAnimation(Hash32("FadeOutPauseMenu"));
						app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(ui, Hash32("FadeInPauseMenu"));
						auto* openAnim = ui->FindAnimation(Hash32("FadeInPauseMenu"));
						//if (openAnim) openAnim->Play();
					});
			}
		}


		void SoundOptionMenu::OnClose()
		{
			// キャンバス
			{
				auto* canvas = GetCanvas();
				if (canvas)
				{
					canvas->RemoveAnimation(Hash32("ScaleUp_SoundMenu"));
					canvas->RemoveAnimation(Hash32("ScaleDown_SoundMenu"));
					//アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(canvas, Hash32("ScaleDown_SoundMenu"));
					auto* closeAnim = canvas->FindAnimation(Hash32("ScaleDown_SoundMenu"));
					if (closeAnim) closeAnim->Play();
				}
			}

			//各UIパーツの一斉フェードアウト
			{
				ForEachUI([](app::ui::UIBase* ui) {
					ui->RemoveAnimation(Hash32("FadeInPauseMenu"));
					ui->RemoveAnimation(Hash32("FadeOutPauseMenu"));
					app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(ui, Hash32("FadeOutPauseMenu"));
					auto* closeAnim = ui->FindAnimation(Hash32("FadeOutPauseMenu"));
					//if (closeAnim) closeAnim->Play();
					});
			}

		}


		void SoundOptionMenu::PlaySelectedAnimation()
		{
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::MasterSoundOptionMenuParameter>();
			const Vector3 p_selection_color(p->selectionColorX, p->selectionColorY, p->selectionColorZ);
			const Vector3 p_default_color(p->defaultColorX, p->defaultColorY, p->defaultColorZ);
			const Vector3 p_text_scale(p->textScaleX, p->textScaleY, p->textScaleZ);
			const Vector3 p_digit_scale(p->digitScaleX, p->digitScaleY, p->digitScaleZ);
			const Vector3 p_default_scale(p->defaultScaleX, p->defaultScaleY, p->defaultScaleZ);

			/** TODO: Updateで、処理が走っているので、無駄な処理を改善したい */
			auto* textMASTER = GetUI<app::ui::UIIcon>(Hash32("Text_MASTER"));
			auto* textBGM = GetUI<app::ui::UIIcon>(Hash32("Text_BGM"));
			auto* textSE = GetUI<app::ui::UIIcon>(Hash32("Text_SE"));

			auto* digitMASTER = GetUI<app::ui::UIDigit>(Hash32("VolumeDigit_MASTER"));
			auto* digitBGM = GetUI<app::ui::UIDigit>(Hash32("VolumeDigit_BGM"));
			auto* digitSE = GetUI<app::ui::UIDigit>(Hash32("VolumeDigit_SE"));

			/** 各項目選択中に拡大アニメーションを再生 */
			if (volumeCursolIndex_ == static_cast<int>(app::SoundManager::SoundVolumeType::Master)
				&& textMASTER
				&& digitMASTER)
			{
				/** リセット: 黄色から白 */
				textBGM->color.Set(p_default_color);
				textSE->color.Set(p_default_color);
				digitBGM->color.Set(p_default_color);
				digitSE->color.Set(p_default_color);
				/** リセット: 等倍に戻す */
				textBGM->transform.localScale = p_default_scale;
				textSE->transform.localScale = p_default_scale;
				digitBGM->transform.localScale = p_default_scale;
				digitSE->transform.localScale = p_default_scale;

				/** 黄色 */
				textMASTER->color.Set(p_selection_color);
				digitMASTER->color.Set(p_selection_color);
				/** スケール拡大 */
				textMASTER->transform.localScale = p_text_scale;
				digitMASTER->transform.localScale = p_digit_scale;
			}
			else if (volumeCursolIndex_ == static_cast<int>(app::SoundManager::SoundVolumeType::BGM)
				&& textBGM
				&& digitBGM)
			{
				/** リセット: 黄色から白 */
				textMASTER->color.Set(p_default_color);
				textSE->color.Set(p_default_color);
				digitMASTER->color.Set(p_default_color);
				digitSE->color.Set(p_default_color);
				/** リセット: 等倍に戻す */
				textMASTER->transform.localScale = p_default_scale;
				textSE->transform.localScale = p_default_scale;
				digitMASTER->transform.localScale = p_default_scale;
				digitSE->transform.localScale = p_default_scale;

				/** 黄色 */
				textBGM->color.Set(p_selection_color);
				digitBGM->color.Set(p_selection_color);
				/** スケール拡大 */
				textBGM->transform.localScale = p_text_scale;
				digitBGM->transform.localScale = p_digit_scale;
			}
			else if (volumeCursolIndex_ == static_cast<int>(app::SoundManager::SoundVolumeType::SE)
				&& textSE
				&& digitSE)
			{
				/** リセット: 黄色から白 */
				textMASTER->color.Set(p_default_color);
				textBGM->color.Set(p_default_color);
				digitMASTER->color.Set(p_default_color);
				digitBGM->color.Set(p_default_color);
				/** リセット: 等倍に戻す */
				textMASTER->transform.localScale = p_default_scale;
				textBGM->transform.localScale = p_default_scale;
				digitMASTER->transform.localScale = p_default_scale;
				digitBGM->transform.localScale = p_default_scale;


				/** 黄色 */
				textSE->color.Set(p_selection_color);
				digitSE->color.Set(p_selection_color);
				/** スケール拡大 */
				textSE->transform.localScale = p_text_scale;
				digitSE->transform.localScale = p_digit_scale;
			}
		}


		void SoundOptionMenu::InitializeLogic()
		{
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::MasterSoundOptionMenuParameter>();
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

			/** カーソルUI */
			{
				auto* cursol = GetUI<app::ui::UIIcon>(Hash32("Cursol"));
				if (cursol)
				{
					// アニメーションをアタッチ
					app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(cursol, Hash32("FadeIn"));
					app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(cursol, Hash32("FadeOut"));
					app::ui::UIAnimationFactory::Attach<app::ui::UIScaleAnimation>(cursol, Hash32("ScaleUp"));


					seq = new app::ui::UIAnimationSequence();
					seq->Add(Hash32("FadeIn"));
					//.Add(Hash32("ScaleUp"))
					//.Add(Hash32("FadeOut"));


				// アニメーションを再生
					seq->Play(cursol);
				}
			}

			// バー
			{
				// MASTER
				{
					auto* gauge = GetUI<app::ui::UIIcon>(Hash32("VolumeBar_MASTER"));
					gauge->SetPivot(Vector2(0.0f, 0.5f));
				}
				// BGM
				{
					auto* gauge = GetUI<app::ui::UIIcon>(Hash32("VolumeBar_BGM"));
					gauge->SetPivot(Vector2(0.0f, 0.5f));
				}
				// SE
				{
					auto* gauge = GetUI<app::ui::UIIcon>(Hash32("VolumeBar_SE"));
					gauge->SetPivot(Vector2(0.0f, 0.5f));
				}
			}
			//初期位置
			{
				app::SoundManager::Get().SetVolume(app::SoundManager::SoundVolumeType::Master, p->volumeDefaultMaster);
				app::SoundManager::Get().SetVolume(app::SoundManager::SoundVolumeType::BGM, p->volumeDefaultBgm);
				app::SoundManager::Get().SetVolume(app::SoundManager::SoundVolumeType::SE, p->volumeDefaultSe);
			}
		}
	}
}