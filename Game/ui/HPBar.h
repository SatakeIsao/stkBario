/**
 * HPBar.h
 * HPバーのUI
 */
#pragma once
#include "Layout.h"

namespace app
{
	namespace ui
	{
		class HPBarObject : public IGameObject
		{
		private:
			std::unique_ptr<app::ui::Layout> layout_;

			int currentHP_ = 8;


		public:
			HPBarObject();
			~HPBarObject();
		public:
			void Update();
			void Render(RenderContext& rc);

			int GetCurrentHP() const
			{
				return currentHP_;
			}
			void SetCurrentHP(const int currentHP)
			{
				currentHP_ = currentHP;
			}
		};




		/********************************************/


		class CoinUIObject : public IGameObject
		{
		public:
			enum class BounceState
			{
				enStop,
				enUp,
				enDown
			};

		private:
			std::unique_ptr <app::ui::Layout> layout_;
			BounceState bounceState_ = BounceState::enStop;

			float bounceTime_ = 0.0f;
			int currentCoin_ = 0;
			bool hasCoinDeadEffect_ = false;
			bool isBouncing_ = false;

		public:
			CoinUIObject();
			~CoinUIObject();
		public:
			void Update() override;
			void Render(RenderContext& rc) override;


			int GetCurrentCoin() const
			{
				return currentCoin_;
			}
			void SetCoinNumber(const int coin)
			{
				currentCoin_ = coin;
			}
			bool HasCoinDeadEffect() const { return hasCoinDeadEffect_; }
			void SetCoinDeadEffect(bool hasCoinDeadEffect) {
				hasCoinDeadEffect_ = hasCoinDeadEffect;
			}
			void GetPlayAnimation();
			
		};




		/********************************************/


		class TimerUIObject : public IGameObject
		{
		public:
			enum class BounceState
			{
				enStop,
				enUp,
				enDown
			};

		private:
			std::unique_ptr <app::ui::Layout> layout_;
			BounceState bounceState_ = BounceState::enStop;
			//std::unique_ptr<app::ui::UIAnimationSequence> seq_;

			float bounceTime_ = 0.0f;
			float timer_ = 0.0f;
			int currentCoin_ = 0;
			bool hasCoinDeadEffect_ = false;
			bool isBouncing_ = false;
			bool isCounting_ = true;
			bool isBlinking_ = false;

			bool hasPulsed100_ = false;
			bool hasPulsed50_ = false;
			bool hasPulsed30_ = false;

		public:
			TimerUIObject();
			~TimerUIObject();
		public:
			void Update() override;
			void Render(RenderContext& rc) override;


			// タイマー操作用の関数
			float GetTimer() const { return timer_; }
			void SetTimer(float time) { timer_ = time; }

			void StartTimer() { isCounting_ = true; }
			void StopTimer() { isCounting_ = false; }
			bool IsTimeUp() const { return timer_ <= 0.0f; }

		};
	}
}

