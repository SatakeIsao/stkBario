/**
 * BattleCharacterファイル
 */
#pragma once
#include "Actor.h"
#include "ActorStateMachine.h"


class BattleCharacter : public Character
{
	using SuperClass = Character;


private:
	std::unique_ptr<BattleCharacterStateMachine> stateMachine_ = nullptr;


public:
	BattleCharacter();
	~BattleCharacter();

	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;


public:
	void Initialize(const char* modelName);


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