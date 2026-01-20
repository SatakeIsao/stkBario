/**
 * BattleCharacterファイル
 */
#pragma once
#include "Actor.h"
#include "ActorStateMachine.h"

namespace app
{
	namespace actor
	{
		struct BattleCharacterStatus;
	}
}


class BattleCharacter : public Character
{
	using SuperClass = Character;


private:
	std::unique_ptr<BattleCharacterStateMachine> stateMachine_ = nullptr;
	std::unique_ptr<app::actor::BattleCharacterStatus> status_ = nullptr;


public:
	BattleCharacter();
	~BattleCharacter();

	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;


public:
	void Initialize(const char* modelName);

	/**
	 * ステータス取得
	 * 取得する際はメンバ変数などで保持はしないで都度取得する形にする
	 */
	app::actor::BattleCharacterStatus* GetStatus() { return status_.get(); }


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