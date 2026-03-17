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
	}
}

