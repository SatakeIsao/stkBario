/**
 * BattleSequence.h
 * バトルシーケンス
 */

#pragma once
#include "Layout.h"
#include "UIParts.h"

namespace app
{
	namespace ui
	{
		class BattleSequence : public IGameObject
		{
			enum class SequenceName {
				Wait,
				Ready,
				GO,
				GameOver,
				GameClear,
				TimeUp,
				Finished
			};
			enum class BounceState
			{
				enStop,
				enFirstDown,
				enFirstUp,
				enSecondDown,
				enSecondUp,
				enThirdDown
			};
			enum class GameClearState
			{
				enStop,
				enPopUp,
				enShrinkBack,
				enWait,
				enExit
			};
			enum class TimeUpState
			{
				enStop,
				enSlideIn,
				enSlideBack,
				enWait
			};

		private:
			std::unique_ptr<app::ui::Layout> layout_;
			SequenceName currentDown_ = SequenceName::Wait;
			BounceState bounceState_ = BounceState::enStop;
			GameClearState gameClearState_ = GameClearState::enStop;
			TimeUpState timeUpState_ = TimeUpState::enStop;
			
			std::unique_ptr<app::ui::UIAnimationSequence> seq_;

			float delayTimer_ = 0.5f;
			float maxCountDownTimer_ = 2.0f;
			float goTimer_ = 1.5f;
			float gameOverTimer_ = 1.5f;
			float gameClearTimer_ = 1.5f;
			float timeUpTimer_ = 2.0f;

			bool hasPlayedGoFadeOut_ = false;

			bool isGameOverAnimFinished_ = false;

			UIIcon* readyIcon_ = nullptr;
			UIIcon* goIcon_ = nullptr;
			UIIcon* gameOverIcon_ = nullptr;
			UIIcon* gameClearIcon_ = nullptr;

		public:
			BattleSequence();
			~BattleSequence();
		public:
			void Update() override;
			void Render(RenderContext& rc);

		public:
			void GetPlayAnimation();
			void StartGameOver();
			void StartTimeUp();
			void StartGameClear();

			bool IsPlaying() const {
				return currentDown_ != SequenceName::Wait && currentDown_ != SequenceName::Finished;
			}
		};
	}
}

