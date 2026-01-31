/**
 * EventCharacterファイル
 */
#pragma once
#include "Actor.h"


namespace app
{
	namespace actor
	{
		class EventCharacter : public Character
		{
			using SuperClass = Character;


		public:
			EventCharacter();
			~EventCharacter();

			bool Start() override;
			void Update() override;
			void Render(RenderContext& rc) override;


		public:
			void Initialize(CharacterInitializeParameter& param) override final;
		};
	}
}