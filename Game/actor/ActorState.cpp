/**
 * Actorファイル
 */
#include "stdafx.h"
#include "ActorState.h"
#include "ActorStateMachine.h"


IdleCharacterState::IdleCharacterState(IStateMachine* owner)
	: ICharacterState(owner)
{
}


IdleCharacterState::~IdleCharacterState()
{
}

void IdleCharacterState::Enter()
{

}


void IdleCharacterState::Update()
{

}


void IdleCharacterState::Exit()
{

}




/*************************************/


RunCharacterState::RunCharacterState(IStateMachine* owner)
	: ICharacterState(owner)
{

}


RunCharacterState::~RunCharacterState()
{

}


void RunCharacterState::Enter()
{

}


void RunCharacterState::Update()
{
	auto* characterStateMachine = owner_->As<CharacterStateMachine>();
	characterStateMachine->Move(g_gameTime->GetFrameDeltaTime(), 1.0f);
}


void RunCharacterState::Exit()
{
	
}




JumpCharacterState::JumpCharacterState(IStateMachine* owner)
	: ICharacterState(owner)
{
}


JumpCharacterState::~JumpCharacterState()
{
}


void JumpCharacterState::Enter()
{
	jumpAmount_ = 10.0f;
}


void JumpCharacterState::Update()
{
	auto* characterStateMachine = owner_->As<CharacterStateMachine>();
	characterStateMachine->Move(g_gameTime->GetFrameDeltaTime(), 0.3f);

	owner_->transform.position.y += jumpAmount_;
	constexpr float gravity = 0.98f;
	jumpAmount_ -= gravity;
}


void JumpCharacterState::Exit()
{
}