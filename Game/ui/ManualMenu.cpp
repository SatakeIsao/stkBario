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
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::ManualMenuParameter>();

			/** レイアウト */
			{
				layout_ = std::make_unique<app::ui::Layout>();
				layout_->Initialize<app::ui::MenuBase>("Assets/ui/layout/ManualMenu.json");
			}

			// 長押しゲージ初期化
			{
				holdGauge_.Init(nullptr, p->bIconGaugeSizeX, p->bIconGaugeSizeY);
				holdGauge_.SetPosition(Vector3(p->gaugePositionX, p->gaugePositionY, p->gaugePositionZ));
				holdGauge_.SetInnerRadius(p->gaugeInnerRadius);
				holdGauge_.SetOuterRadius(p->gaugeOuterRadius);
				holdGauge_.SetScale(p->gaugeScale);
				holdGauge_.SetFillColor(Vector4(p->gaugeFillColorX, p->gaugeFillColorY, p->gaugeFillColorZ, p->gaugeFillColorW));
				holdGauge_.SetEmptyColor(Vector4(p->gaugeEmptyColorX, p->gaugeEmptyColorY, p->gaugeEmptyColorZ, p->gaugeEmptyColorW));
			}

			// アイコン
			{
				bIcon_.Init("Assets/ui/pause/volume/buttonB.DDS", p->bIconButtonSizeX, p->bIconButtonSizeY);
				bIcon_.SetPosition(Vector3(p->bIconPositionX, p->bIconPositionY, p->bIconPositionZ));
				bIcon_.SetScale(Vector3(p->bIconScaleX, p->bIconScaleY, p->bIconScaleZ));
			}
		}

		ManualMenu::~ManualMenu()
		{}

		void ManualMenu::Update()
		{
			layout_->Update();

			// Bボタン長押し処理
			auto* p = app::core::ParameterManager::Get().GetParameter<app::core::ManualMenuParameter>();
			const float HOLD_MAX_TIME = app::core::ParameterManager::Get().GetParameter<app::core::MasterSceneParameter>()->bButtonHoldThreshold;

			if (g_pad[0]->IsPress(enButtonB))
			{
				holdTimer_ += g_gameTime->GetFrameDeltaTime();
				holdTimer_ = min(holdTimer_, HOLD_MAX_TIME);
			}
			else
			{
				holdTimer_ = 0.0f; // 離したらリセット
			}

			float progress = holdTimer_ / HOLD_MAX_TIME;
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
		{}

		void ManualMenu::InitializeLogic()
		{
			// サウンドバーの位置情報設定
			// アニメーションとかいれたり
		}
	}
}