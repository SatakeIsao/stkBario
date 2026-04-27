#include "stdafx.h"
#include "ManualMenu.h"
#include "core/ParameterManager.h"
#include "sound/SoundManager.h"
#include "ui/Layout.h"
#include "ui/UIAnimationFactory.h"
#include "ui/UIAnimation.h"

namespace app
{
	namespace ui
	{
		ManualMenu::ManualMenu()
		{
			/** ゲームオーバーレイアウト */
			{
				layout_ = std::make_unique<app::ui::Layout>();
				layout_->Initialize<app::ui::MenuBase>("Assets/ui/layout/ManualMenu.json");
			}

			// 長押しゲージ初期化
			{
				holdGauge_.Init(nullptr, 90.0f, 90.0f);
				holdGauge_.SetPosition({ 575.0f, -400.0f, 0.0f });
				holdGauge_.SetInnerRadius(0.58f);
				holdGauge_.SetOuterRadius(0.95f);
				holdGauge_.SetScale(0.85f);
				holdGauge_.SetFillColor({ 0.88f, 0.38f, 0.13f, 1.0f }); // オレンジ
				holdGauge_.SetEmptyColor({ 0.0f, 0.0f, 0.0f, 1.0f });	// 黒
			}

			// アイコン
			{
				bIcon_.Init("Assets/ui/pause/volume/buttonB.DDS", 64.0f, 64.0f);
				bIcon_.SetPosition({ 580.0f, -405.0f, 0.0f });
				bIcon_.SetScale({ 1.0f,1.0f,1.0f });
			}
		}

		ManualMenu::~ManualMenu()
		{}

		void ManualMenu::Update()
		{
			layout_->Update();

			// Bボタン長押し処理
			if (g_pad[0]->IsPress(enButtonB))
			{
				holdTimer_ += g_gameTime->GetFrameDeltaTime();
				holdTimer_ = min(holdTimer_, holdMaxTime_);
			}
			else
			{
				holdTimer_ = 0.0f; // 離したらリセット
			}

			float progress = holdTimer_ / holdMaxTime_;
			holdGauge_.SetFillAmount(progress);
			holdGauge_.Update();

			bIcon_.Update();
		}

		void ManualMenu::Render(RenderContext& rc)
		{
			if (layout_) {
				layout_->Render(rc);
			}
			holdGauge_.Draw(rc);
			bIcon_.Draw(rc);
		}

		void ManualMenu::OnOpen()
		{}

		void ManualMenu::OnClose()
		{}

		void ManualMenu::PlaySelectedAnimation()
		{
		}

		void ManualMenu::InitializeLogic()
		{
			// サウンドバーの位置情報設定
			// アニメーションとかいれたり
		}
	}
}