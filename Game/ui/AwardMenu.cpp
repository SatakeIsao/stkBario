#include "stdafx.h"
#include "AwardMenu.h"
#include "core/ParameterManager.h"
#include "sound/SoundManager.h"
#include "ui/Layout.h"
#include "ui/UIAnimationFactory.h"
#include "ui/UIAnimation.h"
#include "AwardManager.h"


namespace app
{
	namespace ui
	{
		AwardMenu::AwardMenu()
		{
			//AwardManager::Get().Initialize();

			/** ゲームオーバーレイアウト */
			{
				layout_ = std::make_unique<app::ui::Layout>();
				layout_->Initialize<app::ui::MenuBase>("Assets/ui/layout/AwardMenu.json");
			}
			app::core::ParameterManager::Get().LoadParameter<app::core::AwardMenuParameter>("Assets/master/AwardMenuParameter.json", [](const nlohmann::json& j, app::core::AwardMenuParameter& p)
				{
					//TODO; X座標もやりたいなぁ
					//p.cursolPositionX[0] = j["cursolPositionXA"];
					//p.cursolPositionX[1] = j["cursolPositionXB"];
			
					/** 右バーのカーソル */
					p.barCursolPositionY[0] = j["barCursolPositionYA"];
					p.barCursolPositionY[1] = j["barCursolPositionYB"];
					p.barCursolPositionY[2] = j["barCursolPositionYC"];
					p.barCursolPositionY[3] = j["barCursolPositionYD"];

					/** 指カーソル用の座標 */ 
					p.panelCursorPositionX[0] = j["panelCursorPosX_Left"];
					p.panelCursorPositionX[1] = j["panelCursorPosX_Right"];
					p.panelCursorPositionY[0] = j["panelCursorPosY_Top"];
					p.panelCursorPositionY[1] = j["panelCursorPosY_Bottom"];
				});
		}

		AwardMenu::~AwardMenu()
		{}

		void AwardMenu::Update()
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

			/** カーソルの移動（左右） */
			if (g_pad[0]->IsTrigger(enButtonRight))
			{
				// 最終行(7行目)は右側(1)が存在しないので右移動を禁止する
				if (currentCol_ == 0 && currentRow_ != 7) {
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
				if (currentRow_ < 7) { // 最終行(7行目)未満なら下へ
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
					currentRow_++;

					// 下にはみ出そうとしたら、表示範囲を1行下にずらす
					if (currentRow_ >= topDisplayRow_ + 2) {
						topDisplayRow_++;
					}

					// 右列にいて、最終行(左のみ)に降りた場合、強制的に左列へ移動
					if (currentRow_ == 7 && currentCol_ == 1) {
						currentCol_ = 0;
					}
				}
			}
			if (g_pad[0]->IsTrigger(enButtonUp))
			{
				if (currentRow_ > 0) { // 0行目より大きいなら上へ
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Button));
					currentRow_--;

					// 上にはみ出そうとしたら、表示範囲を1行上にずらす
					if (currentRow_ < topDisplayRow_) {
						topDisplayRow_--;
					}
				}
			}

			/** パネルの色とテキストを更新 */
			UpdatePanel();
			UpdateTexts();

			/** UI座標の動的更新 */
			auto* parameter = app::core::ParameterManager::Get().GetParameter<app::core::AwardMenuParameter>();

			// 右のスクロールバーの更新
			{
				auto barCursol = layout_->GetMenu()->GetUI<UIIcon>(Hash32("RightBarCursor"));
				if (barCursol) {
					// parameterの[0]を一番上、[3]を一番下として補間計算する
					float topY = parameter->barCursolPositionY[0];    // 180.0
					float bottomY = parameter->barCursolPositionY[3]; // -260.0
					float maxScroll = 6.0f; // 全8行 - 画面表示2行 = 6段階

					float t = (float)topDisplayRow_ / maxScroll;
					barCursol->transform.localPosition.y = topY + (bottomY - topY) * t;
				}
			}

			// 指カーソルの更新
			{
				auto fingerCursor = layout_->GetMenu()->GetUI<UIIcon>(Hash32("fingerCursor"));
				if (fingerCursor) {
					float targetX = (currentCol_ == 0) ? parameter->panelCursorPositionX[0] : parameter->panelCursorPositionX[1];

					// 画面上で「上段(0)」か「下段(1)」かを計算
					int screenRow = currentRow_ - topDisplayRow_;
					float targetY = (screenRow == 0) ? parameter->panelCursorPositionY[0] : parameter->panelCursorPositionY[1];

					fingerCursor->transform.localPosition.x = targetX;
					fingerCursor->transform.localPosition.y = targetY;
				}
			}

			//PlaySelectedAnimation();
			layout_->Update();
		}

		void AwardMenu::Render(RenderContext& rc)
		{
			if (layout_) {
				layout_->Render(rc);
			}
		}

		void AwardMenu::OnOpen()
		{}

		void AwardMenu::OnClose()
		{}

		void AwardMenu::PlaySelectedAnimation()
		{
			///** TODO: Updateで、処理が走っているので、無駄な処理を改善したい */
			//auto textRetry = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_retry"));
			//auto textTitle = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_ReturnToTitle"));
			//
			///** 各項目選択中に拡大アニメーションを再生 */
			//if (cursolIndex_ == 0
			//	&& textRetry)
			//{
			//	/** リセット: 黄色から白 */
			//	textTitle->color.Set(Vector3(1.0f, 1.0f, 1.0f));
			//	/** リセット: 等倍に戻す */
			//	textTitle->transform.localScale = Vector3(1.0f, 1.0f, 0.0f);
			//
			//	/** 黄色 */
			//	textRetry->color.Set(Vector3(1.0f, 1.0f, 0.0f)); 
			//	/** スケール拡大 */
			//	textRetry->transform.localScale = Vector3(1.3f, 1.3f, 0.0f);
			//}
			//else if (cursolIndex_ == 1
			//	&& textTitle)
			//{
			//	/** リセット: 黄色から白 */
			//	textRetry->color.Set(Vector3(1.0f, 1.0f, 1.0f));
			//	/** リセット: 等倍に戻す */
			//	textRetry->transform.localScale = Vector3(1.0f, 1.0f, 0.0f);
			//
			//	/** 黄色 */
			//	textTitle->color.Set(Vector3(1.0f, 1.0f, 0.0f));
			//	/** スケール拡大 */
			//	textTitle->transform.localScale = Vector3(1.3f, 1.3f, 0.0f);
			//}
		}

		void AwardMenu::UpdatePanel()
		{
			// 4つのパネル背景UIの名前（ハッシュ）を配列に定義
			auto panelLeftUp = layout_->GetMenu()->GetUI<UIIcon>(Hash32("panel_leftUp"));
			auto panelRightUp = layout_->GetMenu()->GetUI<UIIcon>(Hash32("panel_rightUp"));
			auto panelLeftDown = layout_->GetMenu()->GetUI<UIIcon>(Hash32("panel_leftDown"));
			auto panelRightDown = layout_->GetMenu()->GetUI<UIIcon>(Hash32("panel_rightDown"));

			// ループで処理できるように、取得したポインタを配列にまとめる
			UIIcon* panels[4] = {
				panelLeftUp,  
				panelRightUp, 
				panelLeftDown,
				panelRightDown
			};

			// 色を定義 (Vector3(R, G, B))
			static const Vector3 selectionColor(1.0f, 1.0f, 0.0f); // 黄色 (選択中)
			static const Vector3 defaultColor = Vector3::One;      // 白 (非選択・元の色が白なら)

			static const Vector3 selectionScale(1.1f, 1.1f, 0.0f);
			static const Vector3 defaultScale = Vector3::One;

			// 画面上の何番目のパネルを選択しているか (0:左上, 1:右上, 2:左下, 3:右下) を算出
			int screenRow = currentRow_ - topDisplayRow_;
			int activePanelIndex = (screenRow * 2) + currentCol_;

			for (int i = 0; i < 4; ++i)
			{
				auto panelBg = panels[i];
				if (panelBg)
				{
					if (i == activePanelIndex)
					{
						panelBg->color.Set(selectionColor);
						panelBg->transform.localScale.Set(selectionScale);
					}
					else
					{
						panelBg->color.Set(defaultColor);
						panelBg->transform.localScale.Set(defaultScale);
					}
				}
			}
		}

		void AwardMenu::UpdateTexts()
		{
			// すべてのテキストUIを取得
			// 左上
			auto textCond_complete = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_complete"));
			auto textCond_soundPlay = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_soundPlay"));
			auto textCond_speedStar = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_speedStar"));
			auto textCond_jumpingFrog = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_jumpingFrog"));
			// 右上
			auto textCond_coinMaster = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_coinMaster"));
			auto textCond_challengeHeart = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_challengeHeart"));
			auto textCond_laidBack = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_laidBack"));
			auto textCond_jumpingRabbit = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_jumpingRabbit"));
			// 左下
			auto textCond_forgetful = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_forgetful"));
			auto textCond_slimeKiller = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_slimeKiller"));
			auto textCond_life = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_life"));
			auto textCond_dokan = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_dokan"));
			// 右下
			auto textCond_stopTime = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_stopTime"));
			auto textCond_gentleWorld = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_gentleWorld"));
			auto textCond_lifeMax = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textCondition_lifeMax"));


			// 【称号名テキスト（新規追加分）】
			auto textTitle_complete = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardComplete_leftUp"));
			auto textTitle_soundPlay = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardSoundPlay_leftUp"));
			auto textTitle_speedStar = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardSpeedStar_leftUp"));
			auto textTitle_jumpingFrog = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardJumpingFrog_leftUp"));
			auto textTitle_forgetful = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textAward_forgetful"));
			auto textTitle_slimeKiller = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textAward_slimeKiller"));
			auto textTitle_life = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textAward_life"));
			auto textTitle_beardedMan = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textAward_beardedMan"));

			auto textTitle_coinMaster = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardCoinMaster_rightUp"));
			auto textTitle_challengeHeart = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardChallengeHeart_rightUp"));
			auto textTitle_laidBack = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardLaidBack_rightUp"));
			auto textTitle_jumpingRabbit = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardJumpingRabbit_rightUp"));
			auto textTitle_stopTime = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textAward_stopTime"));
			auto textTitle_gentleWorld = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textAward_gentleWorld"));
			auto textTitle_lifeMax = layout_->GetMenu()->GetUI<UIIcon>(Hash32("textAward_fullOfEnergy"));

			// 【シークレット(???)テキスト】
			auto secretLU = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardSecret_leftUp"));
			auto secretRU = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardSecret_rightUp"));
			auto secretLD = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardSecret_leftDown"));
			auto secretRD = layout_->GetMenu()->GetUI<UIIcon>(Hash32("text_awardSecret_rightDown"));

			// --- 2. 構造体にまとめる（条件・タイトル・AwardTypeの紐付け） ---
			struct AwardData {
				UIIcon* condUi;
				UIIcon* titleUi;
				AwardType type;
			};

			AwardData allTexts[8][2] = {
				// 0行目
				{ { textCond_complete, textTitle_complete, AwardType::enComplete },
				  { textCond_coinMaster, textTitle_coinMaster, AwardType::enCoinMaster } },
				  // 1行目
				  { { textCond_forgetful, textTitle_forgetful, AwardType::enForgetful },
					{ textCond_stopTime, textTitle_stopTime, AwardType::enTimeStopper } },
					// 2行目
					{ { textCond_soundPlay, textTitle_soundPlay, AwardType::enSoundPlay },
					  { textCond_challengeHeart, textTitle_challengeHeart, AwardType::enChallengerHeart } },
					  // 3行目
					  { { textCond_slimeKiller, textTitle_slimeKiller, AwardType::enSlimeKiller },
						{ textCond_gentleWorld, textTitle_gentleWorld, AwardType::enGentleWorld } },
						// 4行目
						{ { textCond_speedStar, textTitle_speedStar, AwardType::enSpeedStar },
						  { textCond_laidBack, textTitle_laidBack, AwardType::enRelaxedPerson } },
						  // 5行目
						  { { textCond_life, textTitle_life, AwardType::enLifeIsPrecious },
							{ textCond_lifeMax, textTitle_lifeMax, AwardType::enLifeMax } },
							// 6行目
							{ { textCond_jumpingFrog, textTitle_jumpingFrog, AwardType::enJumpingFrog },
							  { textCond_jumpingRabbit, textTitle_jumpingRabbit, AwardType::enBouncingRabbit } },
							  // 7行目
							  { { textCond_dokan, textTitle_beardedMan, AwardType::enBeardedMan },
								{ nullptr, nullptr, AwardType::enComplete } } // 右下は空き
			};

			// --- 3. 一旦すべての isDraw を false にする ---
			for (int r = 0; r < 8; ++r) {
				for (int c = 0; c < 2; ++c) {
					if (allTexts[r][c].condUi)  allTexts[r][c].condUi->isDraw = false;
					if (allTexts[r][c].titleUi) allTexts[r][c].titleUi->isDraw = false;
				}
			}
			if (secretLU) secretLU->isDraw = false;
			if (secretRU) secretRU->isDraw = false;
			if (secretLD) secretLD->isDraw = false;
			if (secretRD) secretRD->isDraw = false;

			// --- 4. 画面に表示されている2行分(上段・下段)だけ表示＆Y座標設定 ---
			int screenTopRow = topDisplayRow_;        // 上段の行番号
			int screenBottomRow = topDisplayRow_ + 1; // 下段の行番号

			// Y座標の基準値
			const float TITLE_Y_UP = 140.0f;
			const float TITLE_Y_DOWN = -140.0f; // 内パネルのY座標に合わせました
			const float COND_Y_UP = 40.0f;
			const float COND_Y_DOWN = -240.0f;

			// 【上段の配置】
			for (int c = 0; c < 2; ++c) {
				auto& data = allTexts[screenTopRow][c];
				if (data.condUi) {
					data.condUi->isDraw = true;
					data.condUi->transform.localPosition.y = COND_Y_UP;
					data.condUi->transform.localScale = Vector3::One; // JSONでScale0になっていた場合の対策

					// マネージャーから取得状況を確認
					bool isUnlocked = app::ui::AwardManager::Get().GetAward(data.type);
					if (isUnlocked && data.titleUi) {
						// 取得済み：称号名を表示
						data.titleUi->isDraw = true;
						data.titleUi->transform.localPosition.y = TITLE_Y_UP;
						data.titleUi->transform.localScale = Vector3::One;
					}
					else {
						// 未取得：「???」を表示
						if (c == 0 && secretLU) { secretLU->isDraw = true; secretLU->transform.localScale = Vector3::One; }
						if (c == 1 && secretRU) { secretRU->isDraw = true; secretRU->transform.localScale = Vector3::One; }
					}
				}
			}

			// 【下段の配置】
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
			//// 全テキストを「8行 × 2列」の配列に整理する
			//UIIcon* allTexts[8][2] = {
			//	{ textLU_0, textRU_0 }, // 0行目 (元のページ1の上段)
			//	{ textLD_0, textRD_0 }, // 1行目 (元のページ1の下段)
			//	{ textLU_1, textRU_1 }, // 2行目 (元のページ2の上段)
			//	{ textLD_1, textRD_1 }, // 3行目 (元のページ2の下段)
			//	{ textLU_2, textRU_2 }, // 4行目 (元のページ3の上段)
			//	{ textLD_2, textRD_2 }, // 5行目 (元のページ3の下段)
			//	{ textLU_3, textRU_3 }, // 6行目 (元のページ4の上段)
			//	{ textLD_3, nullptr  }  // 7行目 (元のページ4の下段 ※右下は無いのでnullptr)
			//};

			//// 一旦すべての isDraw を false にする
			//for (int r = 0; r < 8; ++r) {
			//	for (int c = 0; c < 2; ++c) {
			//		if (allTexts[r][c]) {
			//			allTexts[r][c]->isDraw = false;
			//		}
			//	}
			//}

			//// 画面に表示されている2行分(上段・下段)だけ true にし、Y座標を合わせる
			//int screenTopRow = topDisplayRow_;        // 上段の行番号
			//int screenBottomRow = topDisplayRow_ + 1; // 下段の行番号

			//// JSONの値に基づく、上段・下段スロットのテキスト基準Y座標
			//const float Y_POS_UP = 40.0f;
			//const float Y_POS_DOWN = -240.0f;

			//// 上段のテキストを配置
			//for (int c = 0; c < 2; ++c) {
			//	if (allTexts[screenTopRow][c]) {
			//		allTexts[screenTopRow][c]->isDraw = true;
			//		allTexts[screenTopRow][c]->transform.localPosition.y = Y_POS_UP;
			//	}
			//}

			//// 下段のテキストを配置
			//for (int c = 0; c < 2; ++c) {
			//	if (allTexts[screenBottomRow][c]) {
			//		allTexts[screenBottomRow][c]->isDraw = true;
			//		allTexts[screenBottomRow][c]->transform.localPosition.y = Y_POS_DOWN;
			//	}
			//}
		}

		void AwardMenu::InitializeLogic()
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