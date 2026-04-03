#pragma once
#include "Layout.h"

namespace app
{
	namespace ui
	{
		class AwardMenu : public IGameObject
		{
		private:
			std::unique_ptr <app::ui::Layout> layout_;
			/** 右バー用 */
			//int barCursorIndex_ = 0;
			///** 4つのパネル用 */
			//int panelCursorIndex_ = 0;
			//ManualMenu currentState_ = ManualMenu::enTitle;

			int currentRow_ = 0;    // カーソルがいる行 (0〜7：全8行)
			int currentCol_ = 0;    // カーソルがいる列 (0:左, 1:右)
			int topDisplayRow_ = 0; // 画面上段に表示されている行番号 (0〜6)

			bool isOpenJustNow_ = false;
		public:
			AwardMenu();
			virtual ~AwardMenu();
			void Update() override;
			void Render(RenderContext& rc);

			void OnOpen();
			void OnClose();
			void PlaySelectedAnimation();
			/** パネル更新 */
			void UpdatePanel();
			void UpdateTexts();

			int GerCurrentIndex() const
			{
				return currentRow_;
			}

			//bool IsReadyToSelect() const
			//{
			//	return currentState_ == ManualMenu::enMenu && !isOpenJustNow_;
			//}
		public:
			virtual void InitializeLogic();
		};
	}
}

