/**
 * Actorファイル
 */
#include "stdafx.h"
#include "ActorState.h"
#include "ActorStateMachine.h"
#include "ActorStatus.h"
#include "actor/Types.h"

#include "core/ParameterManager.h"


namespace
{

}


IdleCharacterState::IdleCharacterState(IStateMachine* owner)
	: ICharacterState(owner)
{
}


IdleCharacterState::~IdleCharacterState()
{
}


void IdleCharacterState::Enter()
{
	auto* characterStateMachine = owner_->As<CharacterStateMachine>();
	characterStateMachine->GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::Idle));
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
	auto* characterStateMachine = owner_->As<CharacterStateMachine>();
	characterStateMachine->GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::Run));
}


void RunCharacterState::Update()
{
	auto* characterStateMachine = owner_->As<CharacterStateMachine>();
	auto* characterStatus = characterStateMachine->GetStatus();
	characterStateMachine->Move(g_gameTime->GetFrameDeltaTime(), characterStatus->GetMoveSpeed());

	characterStateMachine->transform.rotation.SetRotationYFromDirectionXZ(characterStateMachine->GetMoveDirection());
}


void RunCharacterState::Exit()
{
}




/*************************************/


JumpCharacterState::JumpCharacterState(IStateMachine* owner)
	: ICharacterState(owner)
{
}


JumpCharacterState::~JumpCharacterState()
{
}


void JumpCharacterState::Enter()
{
	auto* characterStateMachine = owner_->As<CharacterStateMachine>();
	auto* characterStatus = characterStateMachine->GetStatus();

	characterStateMachine->Jump(characterStatus->GetJumpPower());

	characterStateMachine->GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::Jump));
}


void JumpCharacterState::Update()
{
	auto* characterStateMachine = owner_->As<CharacterStateMachine>();
	auto* characterStatus = characterStateMachine->GetStatus();
	characterStateMachine->Move(g_gameTime->GetFrameDeltaTime(), characterStatus->GetJumpMoveSpeed());
	
	Quaternion rotation;
	rotation.SetRotationYFromDirectionXZ(characterStateMachine->GetMoveDirection());
	characterStateMachine->transform.rotation.SetRotationYFromDirectionXZ(characterStateMachine->GetMoveDirection());
}


void JumpCharacterState::Exit()
{
}




/*************************************/


PunchCharacterState::PunchCharacterState(IStateMachine* owner)
	: ICharacterState(owner)
{
}


PunchCharacterState::~PunchCharacterState()
{
}


void PunchCharacterState::Enter()
{
	auto* characterStateMachine = owner_->As<CharacterStateMachine>();
	characterStateMachine->GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::Punch));
}


void PunchCharacterState::Update()
{
}


void PunchCharacterState::Exit()
{
}


bool PunchCharacterState::CanChangeState() const
{
	auto* characterStateMachine = owner_->As<CharacterStateMachine>();
	auto* modelRender = characterStateMachine->GetModelRender();
	return !modelRender->IsPlayingAnimation();
}