#pragma once
#include "Layout.h"

namespace app
{
	namespace ui
	{
		class GameOverMenu : public IGameObject
		{
		private:
			std::unique_ptr <app::ui::Layout> layout_;
			int cursolIndex_ = 0;


		public:
			GameOverMenu();
			virtual ~GameOverMenu();
			void Update() override;
			void Render(RenderContext& rc);

			void OnOpen();
			void OnClose();
			void PlaySelectedAnimation();

			int GerCurrentIndex() const
			{
				return cursolIndex_;
			}
		public:
			virtual void InitializeLogic();
		};
	}
}

