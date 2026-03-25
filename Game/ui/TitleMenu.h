#pragma once
#include "Layout.h"

namespace app
{
	namespace ui
	{
		enum class MenuState
		{
			enTitle,
			enMenu,
			enStart,
			enSousa,
			enAward,
			enExit
		};


		class TitleMenu : public IGameObject
		{
		private:
			std::unique_ptr <app::ui::Layout> layout_;
			int cursolIndex_ = 0;
			MenuState currentState_ = MenuState::enTitle;

			bool isOpenJustNow_ = false;
		public:
			TitleMenu();
			virtual ~TitleMenu();
			void Update() override;
			void Render(RenderContext& rc);

			void OnOpen();
			void OnClose();
			void PlaySelectedAnimation();

			int GerCurrentIndex() const
			{
				return cursolIndex_;
			}

			bool IsReadyToSelect() const
			{
				return currentState_ == MenuState::enMenu && !isOpenJustNow_;
			}
		public:
			virtual void InitializeLogic();
		};
	}
}

