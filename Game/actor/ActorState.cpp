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

	jumpAmount_ = characterStatus->GetJumpPower();

	characterStateMachine->GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::Jump));
}


void JumpCharacterState::Update()
{
	auto* characterStateMachine = owner_->As<CharacterStateMachine>();
	auto* characterStatus = characterStateMachine->GetStatus();
	characterStateMachine->Move(g_gameTime->GetFrameDeltaTime(), characterStatus->GetJumpMoveSpeed());
	
	Vector3 moveSpeedVector = characterStateMachine->GetMoveSpeedVector();
	moveSpeedVector.y += jumpAmount_;
	const float gravity = characterStatus->GetGravity();
	jumpAmount_ -= gravity;

	Quaternion rotation;
	rotation.SetRotationYFromDirectionXZ(characterStateMachine->GetMoveDirection());
	characterStateMachine->transform.rotation.SetRotationYFromDirectionXZ(characterStateMachine->GetMoveDirection());

	characterStateMachine->SetMoveSpeedVector(moveSpeedVector);
}


void JumpCharacterState::Exit()
{
}