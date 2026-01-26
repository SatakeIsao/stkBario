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
	jumpPhase_ = JumpPhase::Ascend;


	auto* characterStateMachine = owner_->As<CharacterStateMachine>();
	auto* characterStatus = characterStateMachine->GetStatus();

	characterStateMachine->Jump(characterStatus->GetJumpPower());

	characterStateMachine->GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::JumpAscend));

	characterStateMachine->GetModelRender()->SetAnimationSpeed(2.5f);
}


void JumpCharacterState::Update()
{
	auto* characterStateMachine = owner_->As<CharacterStateMachine>();
	
	switch (jumpPhase_)
	{
		case JumpPhase::Ascend:
		{
			// 上昇が終わったら落下フェーズへ
			if (characterStateMachine->GetCharacterController()->GetVerticalVelocity() < 0.0f) {
				characterStateMachine->GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::JumpDescend));
				jumpPhase_ = JumpPhase::Descend;
			}
			break;
		}
		case JumpPhase::Descend:
		{
			// 地面に着地したら着地フェーズへ
			if (characterStateMachine->GetCharacterController()->IsOnGround()) {
				characterStateMachine->GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::JumpLand));
				jumpPhase_ = JumpPhase::Land;
			}
			break;
		}
		case JumpPhase::Land:
		{
			break;
		}
	}

	auto* characterStatus = characterStateMachine->GetStatus();
	characterStateMachine->Move(g_gameTime->GetFrameDeltaTime(), characterStatus->GetJumpMoveSpeed());
	
	Quaternion rotation;
	rotation.SetRotationYFromDirectionXZ(characterStateMachine->GetMoveDirection());
	characterStateMachine->transform.rotation.SetRotationYFromDirectionXZ(characterStateMachine->GetMoveDirection());
}


void JumpCharacterState::Exit()
{
	auto* characterStateMachine = owner_->As<CharacterStateMachine>();
	characterStateMachine->GetModelRender()->SetAnimationSpeed(1.0f);
}


bool JumpCharacterState::CanChangeState() const
{
	if (jumpPhase_ != JumpPhase::Land) {
		return false;
	}
	auto* characterStateMachine = owner_->As<CharacterStateMachine>();
	if (!characterStateMachine->GetCharacterController()->IsOnGround()) {
		return false;
	}
	if(characterStateMachine->GetModelRender()->IsPlayingAnimation()) {
		return false;
	}
	return true;
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