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
		}

		ManualMenu::~ManualMenu()
		{}

		void ManualMenu::Update()
		{
			layout_->Update();
		}

		void ManualMenu::Render(RenderContext& rc)
		{
			if (layout_) {
				layout_->Render(rc);
			}
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