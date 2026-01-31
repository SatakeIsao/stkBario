/**
 * BattleCharacterファイル
 */
#pragma once
#include "Actor.h"
#include "ActorStateMachine.h"
#include "actor/Types.h"


namespace app
{
	namespace actor
	{
		class BattleCharacter : public Character
		{
			appActor(BattleCharacter);


		private:
			using SuperClass = Character;


		private:
			std::unique_ptr<BattleCharacterStateMachine> stateMachine_ = nullptr;
			std::unique_ptr<app::collision::GhostBody> ghostBody_ = nullptr;


		public:
			BattleCharacter();
			~BattleCharacter();

			bool Start() override;
			void Update() override;
			void Render(RenderContext& rc) override;


		public:
			void Initialize(CharacterInitializeParameter& param) override final;


		public:
			template <typename TState>
			void AddState()
			{
				stateMachine_->AddState<TState>();
			}


			BattleCharacterStateMachine* GetStateMachine()
			{
				return stateMachine_.get();
			}
		};
	}
}