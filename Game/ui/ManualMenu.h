#pragma once
#include "Layout.h"

namespace app
{
	namespace ui
	{
		class ManualMenu : public IGameObject
		{
		private:
			/** 長押しゲージ */
			CircularGaugeRender holdGauge_;
			/** Bアイコン */
			SpriteRender bIcon_;

			std::unique_ptr <app::ui::Layout> layout_;
			/** 長押し経過時間 */
			float holdTimer_ = 0.0f;
			/** 長押し完了までの時間（秒） */
			float holdMaxTime_ = 0.95f;

			int cursolIndex_ = 0;
			bool isOpenJustNow_ = false;
		public:
			ManualMenu();
			virtual ~ManualMenu();
			void Update() override;
			void Render(RenderContext& rc);

			void OnOpen();
			void OnClose();
			void PlaySelectedAnimation();
			
			// 外部から長押し進捗を渡す（0.0〜1.0）
			void SetHoldProgress(float progress)
			{
				holdGauge_.SetFillAmount(progress);
			}

			int GerCurrentIndex() const
			{
				return cursolIndex_;
			}

		public:
			virtual void InitializeLogic();
		};
	}
}

