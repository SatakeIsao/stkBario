#pragma once
#include "Layout.h"

namespace app
{
	namespace ui
	{
		class ManualMenu : public IGameObject
		{
		private:
			std::unique_ptr <app::ui::Layout> layout_;
			int cursolIndex_ = 0;
			//ManualMenu currentState_ = ManualMenu::enTitle;

			bool isOpenJustNow_ = false;
		public:
			ManualMenu();
			virtual ~ManualMenu();
			void Update() override;
			void Render(RenderContext& rc);

			void OnOpen();
			void OnClose();
			void PlaySelectedAnimation();

			int GerCurrentIndex() const
			{
				return cursolIndex_;
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

