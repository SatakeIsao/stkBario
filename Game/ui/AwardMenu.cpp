#include "stdafx.h"
#include "AwardMenu.h"
#include "core/ParameterManager.h"
#include "sound/SoundManager.h"
#include "ui/Layout.h"
#include "ui/UIAnimationFactory.h"
#include "ui/UIAnimation.h"
#include "AwardManager.h"

namespace
{
	static app::ui::UIAnimationSequence* seq = nullptr;
}

namespace app
{
	namespace ui
	{
		AwardMenu::AwardMenu()
		{
			/** ゲームオーバーレイアウト */
			{
				layout_ = std::make_unique<app::ui::Layout>();
				layout_->Initialize<app::ui::MenuBase>("Assets/ui/layout/AwardMenu.json");
			}
			// バイナリ経由で読み込み済みのためここでは何もしない
		}

		AwardMenu::~AwardMenu()
		{}

		void AwardMenu::Update()
		{
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::AwardMenuParameter>();

			const int MAX_ROWS = p->maxRows * 8; // maxRows=1 は「1セット=8行」
			const int DISPLAY_ROW_COUNT = static_cast<int>(p->displayRowCount) / 15;
			const float MAX_SCROLL_VAL = p->maxScrollVal / (100.0f / 6.0f);

			/** カーソルの移動（左右） */
			if (g_pad[0]->IsTrigger(enButtonRight))
			{
				if (currentCol_ == 0 && currentRow_ != (MAX_ROWS - 1)) {
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
					currentCol_ = 1;
				}
			}
			if (g_pad[0]->IsTrigger(enButtonLeft))
			{
				if (currentCol_ == 1) {
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
					currentCol_ = 0;
				}
			}

			/** カーソルの移動（上下）とスクロール管理 */
			if (g_pad[0]->IsTrigger(enButtonDown))
			{
				if (currentRow_ < (MAX_ROWS - 1)) {
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
					currentRow_++;
					if (currentRow_ >= topDisplayRow_ + DISPLAY_ROW_COUNT) {
						topDisplayRow_++;
					}
					if (currentRow_ == (MAX_ROWS - 1) && currentCol_ == 1) {
						currentCol_ = 0;
					}
				}
			}
			if (g_pad[0]->IsTrigger(enButtonUp))
			{
				if (currentRow_ > 0) {
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
					currentRow_--;
					if (currentRow_ < topDisplayRow_) {
						topDisplayRow_--;
					}
				}
			}

			/** パネルの色とテキストを更新 */
			UpdatePanel();
			UpdateTexts();

			/** UI座標の動的更新 */

			// 右のスクロールバーの更新
			{
				auto barCursol = layout_->GetMenu()->GetUI<UIIcon>(Hash32("RightBarCursor"));
				if (barCursol) {
					float topY = p->barCursolPositionYA;
					float bottomY = p->barCursolPositionYD;
					float t = (float)topDisplayRow_ / MAX_SCROLL_VAL;
					barCursol->transform.localPosition.y = topY + (bottomY - topY) * t;
				}
			}

			// 指カーソルの更新
			{
				auto fingerCursor = layout_->GetMenu()->GetUI<UIIcon>(Hash32("fingerCursor"));
				if (fingerCursor) {
					float targetX = (currentCol_ == 0) ? p->panelCursorPosX_Left : p->panelCursorPosX_Right;
					int screenRow = currentRow_ - topDisplayRow_;
					float targetY = (screenRow == 0) ? p->panelCursorPosY_Top : p->panelCursorPosY_Bottom;
					fingerCursor->transform.localPosition.x = targetX;
					fingerCursor->transform.localPosition.y = targetY;
				}
			}

			PlaySelectedAnimation();
			layout_->Update();
		}

		void AwardMenu::Render(RenderContext& rc)
		{
			if (layout_) {
				layout_->Render(rc);
			}
		}

		void AwardMenu::OnOpen()
		{
			auto fingerCursor = layout_->GetMenu()->GetUI<UIIcon>(Hash32("fingerCursor"));
			if (fingerCursor)
			{
				fingerCursor->isDraw = true;
				app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(fingerCursor, Hash32("FadeIn"));
				auto* anim = fingerCursor->FindAnimation(Hash32("FadeIn"));
				if (anim) { anim->Play(); }
			}
		}

		void AwardMenu::OnClose()
		{
			auto fingerCursor = layout_->GetMenu()->GetUI<UIIcon>(Hash32("fingerCursor"));
			if (fingerCursor)
			{
				auto* anim = fingerCursor->FindAnimation(Hash32("FadeIn"));
				if (anim) { anim->Stop(); }
				fingerCursor->isDraw = false;
			}
		}

		void AwardMenu::PlaySelectedAnimation()
		{}

		void AwardMenu::UpdatePanel()
		{
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::AwardMenuParameter>();

			const Vector3 SELECTION_SCALE(p->selectionScaleX, p->selectionScaleY, p->selectionScaleZ);
			const Vector3 DEFAULT_SCALE(p->defaultScaleX, p->defaultScaleY, p->defaultScaleZ);
			const Vector3 SELECTION_COLOR(p->selectionColorX, p->selectionColorY, p->selectionColorZ);
			const Vector3 DEFAULT_COLOR(p->defaultColorX, p->defaultColorY, p->defaultColorZ);

			auto panelLeftUp = layout_->GetMenu()->GetUI<UIIcon>(Hash32("panel_leftUp"));
			auto panelRightUp = layout_->GetMenu()->GetUI<UIIcon>(Hash32("panel_rightUp"));
			auto panelLeftDown = layout_->GetMenu()->GetUI<UIIcon>(Hash32("panel_leftDown"));
			auto panelRightDown = layout_->GetMenu()->GetUI<UIIcon>(Hash32("panel_rightDown"));

			UIIcon* panels[4] = { panelLeftUp, panelRightUp, panelLeftDown, panelRightDown };

			int screenRow = currentRow_ - topDisplayRow_;
			int activePanelIndex = (screenRow * 2) + currentCol_;

			for (int i = 0; i < 4; ++i)
			{
				auto panelBg = panels[i];
				if (panelBg)
				{
					if (i == activePanelIndex)
					{
						panelBg->color.Set(SELECTION_COLOR);
						panelBg->transform.localScale.Set(SELECTION_SCALE);
					}
					else
					{
						panelBg->color.Set(DEFAULT_COLOR);
						panelBg->transform.localScale.Set(DEFAULT_SCALE);
					}
				}
			}
		}

		void AwardMenu::UpdateTexts()
		{
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::AwardMenuParameter>();

			const float TITLE_Y_UP = static_cast<float>(p->titleYUp) * 28.0f;
			const float TITLE_Y_DOWN = static_cast<float>(p->titleYDown) * -14.0f;
			const float COND_Y_UP = static_cast<float>(p->condYUp) * 4.0f;
			const float COND_Y_DOWN = static_cast<float>(p->condYDown) * -24.0f;
			const int   MAX_ROWS = p->maxRows * 8;

			auto textCond_complete = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_complete"));
			auto textCond_soundPlay = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_soundPlay"));
			auto textCond_speedStar = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_speedStar"));
			auto textCond_jumpingFrog = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_jumpingFrog"));
			auto textCond_coinMaster = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_coinMaster"));
			auto textCond_challengeHeart = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_challengeHeart"));
			auto textCond_laidBack = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_laidBack"));
			auto textCond_jumpingRabbit = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_jumpingRabbit"));
			auto textCond_forgetful = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_forgetful"));
			auto textCond_slimeKiller = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_slimeKiller"));
			auto textCond_life = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_life"));
			auto textCond_dokan = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_dokan"));
			auto textCond_stopTime = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_stopTime"));
			auto textCond_gentleWorld = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_gentleWorld"));
			auto textCond_lifeMax = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_lifeMax"));

			auto textTitle_complete = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardComplete_leftUp"));
			auto textTitle_soundPlay = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardSoundPlay_leftUp"));
			auto textTitle_speedStar = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardSpeedStar_leftUp"));
			auto textTitle_jumpingFrog = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardJumpingFrog_leftUp"));
			auto textTitle_forgetful = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_award_forgetful"));
			auto textTitle_slimeKiller = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_award_slimeKiller"));
			auto textTitle_life = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_award_life"));
			auto textTitle_beardedMan = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_award_beardedMan"));
			auto textTitle_coinMaster = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardCoinMaster_rightUp"));
			auto textTitle_challengeHeart = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardChallengeHeart_rightUp"));
			auto textTitle_laidBack = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardLaidBack_rightUp"));
			auto textTitle_jumpingRabbit = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardJumpingRabbit_rightUp"));
			auto textTitle_stopTime = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_award_stopTime"));
			auto textTitle_gentleWorld = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_award_gentleWorld"));
			auto textTitle_lifeMax = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_award_fullOfEnergy"));

			auto secretLU = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardSecret_leftUp"));
			auto secretRU = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardSecret_rightUp"));
			auto secretLD = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardSecret_leftDown"));
			auto secretRD = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardSecret_rightDown"));

			struct AwardData { UIIcon* condUi; UIIcon* titleUi; AwardType type; };

			AwardData allTexts[8][2] = {
				{ { textCond_complete,      textTitle_complete,      AwardType::enComplete },
				  { textCond_coinMaster,    textTitle_coinMaster,    AwardType::enCoinMaster } },
				{ { textCond_forgetful,     textTitle_forgetful,     AwardType::enForgetful },
				  { textCond_stopTime,      textTitle_stopTime,      AwardType::enTimeStopper } },
				{ { textCond_soundPlay,     textTitle_soundPlay,     AwardType::enSoundPlay },
				  { textCond_challengeHeart,textTitle_challengeHeart,AwardType::enChallengerHeart } },
				{ { textCond_slimeKiller,   textTitle_slimeKiller,   AwardType::enSlimeKiller },
				  { textCond_gentleWorld,   textTitle_gentleWorld,   AwardType::enGentleWorld } },
				{ { textCond_speedStar,     textTitle_speedStar,     AwardType::enSpeedStar },
				  { textCond_laidBack,      textTitle_laidBack,      AwardType::enRelaxedPerson } },
				{ { textCond_life,          textTitle_life,          AwardType::enLifeIsPrecious },
				  { textCond_lifeMax,       textTitle_lifeMax,       AwardType::enLifeMax } },
				{ { textCond_jumpingFrog,   textTitle_jumpingFrog,   AwardType::enJumpingFrog },
				  { textCond_jumpingRabbit, textTitle_jumpingRabbit, AwardType::enBouncingRabbit } },
				{ { textCond_dokan,         textTitle_beardedMan,    AwardType::enBeardedMan },
				  { nullptr, nullptr, AwardType::enComplete } }
			};

			for (int r = 0; r < MAX_ROWS; ++r) {
				for (int c = 0; c < 2; ++c) {
					if (allTexts[r][c].condUi)  allTexts[r][c].condUi->isDraw = false;
					if (allTexts[r][c].titleUi) allTexts[r][c].titleUi->isDraw = false;
				}
			}
			if (secretLU) secretLU->isDraw = false;
			if (secretRU) secretRU->isDraw = false;
			if (secretLD) secretLD->isDraw = false;
			if (secretRD) secretRD->isDraw = false;

			int screenTopRow = topDisplayRow_;
			int screenBottomRow = topDisplayRow_ + 1;

			for (int c = 0; c < 2; ++c) {
				auto& data = allTexts[screenTopRow][c];
				if (data.condUi) {
					data.condUi->isDraw = true;
					data.condUi->transform.localPosition.y = COND_Y_UP;
					data.condUi->transform.localScale = Vector3::One;
					bool isUnlocked = app::ui::AwardManager::Get().GetAward(data.type);
					if (isUnlocked && data.titleUi) {
						data.titleUi->isDraw = true;
						data.titleUi->transform.localPosition.y = TITLE_Y_UP;
						data.titleUi->transform.localScale = Vector3::One;
					}
					else {
						if (c == 0 && secretLU) { secretLU->isDraw = true; secretLU->transform.localScale = Vector3::One; }
						if (c == 1 && secretRU) { secretRU->isDraw = true; secretRU->transform.localScale = Vector3::One; }
					}
				}
			}

			for (int c = 0; c < 2; ++c) {
				auto& data = allTexts[screenBottomRow][c];
				if (data.condUi) {
					data.condUi->isDraw = true;
					data.condUi->transform.localPosition.y = COND_Y_DOWN;
					data.condUi->transform.localScale = Vector3::One;
					bool isUnlocked = app::ui::AwardManager::Get().GetAward(data.type);
					if (isUnlocked && data.titleUi) {
						data.titleUi->isDraw = true;
						data.titleUi->transform.localPosition.y = TITLE_Y_DOWN;
						data.titleUi->transform.localScale = Vector3::One;
					}
					else {
						if (c == 0 && secretLD) { secretLD->isDraw = true; secretLD->transform.localScale = Vector3::One; }
						if (c == 1 && secretRD) { secretRD->isDraw = true; secretRD->transform.localScale = Vector3::One; }
					}
				}
			}
		}

		void AwardMenu::InitializeLogic()
		{
			auto fingerCursor = layout_->GetMenu()->GetUI<UIIcon>(Hash32("fingerCursor"));
			if (fingerCursor)
			{
				app::ui::UIAnimationFactory::Attach<app::ui::UIColorAnimation>(fingerCursor, Hash32("FadeIn"));
				seq = new app::ui::UIAnimationSequence();
				seq->Add(Hash32("FadeIn"));
				seq->Play(fingerCursor);
			}
		}
	}
}