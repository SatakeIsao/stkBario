#pragma once
#include "Layout.h"
#include "AwardManager.h"

namespace app
{
	namespace ui
	{
		/** アニメーションの順番を管理するステート */
		enum class ResultState {
			Init,
			Time,
			Coin,
			Slime,
			Score,
			Award,
			Finished
		};
		class ResultMenu : public IGameObject
		{
		private:
			std::unique_ptr <app::ui::Layout> layout_;
			float gameOverTimer_ = 0.0f;
			int cursolIndex_ = 0;
			std::unique_ptr<app::ui::UIAnimationSequence> seq_;
			std::unique_ptr<app::ui::UIAnimationSequence> seqAward_;
			std::unique_ptr<app::ui::UIAnimationSequence> seqtimeMinutes_;
			std::unique_ptr<app::ui::UIAnimationSequence> seqtimeBackGroundSeconds_;
			std::unique_ptr<app::ui::UIAnimationSequence> seqtimeSeconds_;
			std::unique_ptr<app::ui::UIAnimationSequence> seqCoinNumbers_;
			std::unique_ptr<app::ui::UIAnimationSequence> seqSlimeNumbers_;
			std::unique_ptr<app::ui::UIAnimationSequence> seqScoreNumbers_;
			std::unique_ptr<app::ui::UIAnimationSequence> seqTextNext_;

			ResultState state_ = ResultState::Init;
			float sequenceTimer_ = 0.0f;
			bool isTest = false;

			app::ui::UIIcon* randomAwardIcon_ = nullptr;
			app::ui::UIIcon* GetRandomAwardUI(AwardType awardType);
		public:
			ResultMenu();
			virtual ~ResultMenu();
			void Update() override;
			void Render(RenderContext& rc);

			void OnOpen();
			void OnClose();
			void PlaySelectedAnimation();
			void ShowAwardUI(AwardType awardType);

			int GerCurrentIndex() const
			{
				return cursolIndex_;
			}

			bool IsReadyToExit() const
			{
				return state_ == ResultState::Finished && sequenceTimer_ >= 0.5f;
			}
		public:
			virtual void InitializeLogic();
		};
	}
}

