/**
 * Actorファイル
 */
#include "stdafx.h"
#include "Actor.h"
#include "ActorState.h"
#include "ActorStateMachine.h"
#include "ActorStatus.h"
#include "actor/Types.h"
#include "core/ParameterManager.h"


namespace app
{
	namespace actor
	{
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

			characterStateMachine->transform.rotation.SetRotationYFromDirectionXZ(characterStateMachine->GetMoveSpeedVector());
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
			characterStateMachine->transform.rotation.SetRotationYFromDirectionXZ(characterStateMachine->GetMoveSpeedVector());
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
			if (characterStateMachine->GetModelRender()->IsPlayingAnimation()) {
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
			attackScheduler_ = std::make_unique<app::core::TaskSchedulerSystem>();
			attackScheduler_->AddTimer(0.1f, [&]()
				{
					auto* characterStateMachine = owner_->As<CharacterStateMachine>();
					characterStateMachine->GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::Punch));
					attackBody_ = new app::collision::GhostBody();
					attackBody_->CreateSphere(characterStateMachine->GetCharacter(), characterStateMachine->GetCharacterID(), 10.0f, app::collision::ghost::CollisionAttribute::Player, app::collision::ghost::CollisionAttributeMask::All);
					// @todo for test
					const float radius = characterStateMachine->GetStatus()->GetRadius();
					attackBody_->SetPosition(characterStateMachine->transform.position + characterStateMachine->GetMoveDirection() * (radius + radius) + Vector3(0.0f, radius, 0.0f));
				}, false);
			attackScheduler_->AddTimer(0.1f, [&]()
				{
					delete attackBody_;
					attackBody_ = nullptr;
				}, true);
		}


		void PunchCharacterState::Update()
		{
			attackScheduler_->Update(g_gameTime->GetFrameDeltaTime());
		}


		void PunchCharacterState::Exit()
		{
			attackScheduler_.reset(nullptr);
		}


		bool PunchCharacterState::CanChangeState() const
		{
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			auto* modelRender = characterStateMachine->GetModelRender();
			return !modelRender->IsPlayingAnimation();
		}




		/*************************************/


		WarpInCharacterState::WarpInCharacterState(IStateMachine* owner)
			: ICharacterState(owner)
		{
		}


		WarpInCharacterState::~WarpInCharacterState()
		{
		}


		void WarpInCharacterState::Enter()
		{
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			characterStateMachine->GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::JumpDescend));
			characterStateMachine->SetInputPower(0.0f);
			characterStateMachine->ClearMomveSpeedVector();
			auto* characterStatus = characterStateMachine->GetStatus();
			scaleCurve_.Initialize(characterStatus->GetWarpStartScale(), characterStatus->GetWarpEndScale(), characterStatus->GetWarpTimeSeconds(), app::util::EasingType::Linear);
			scaleCurve_.Play();
		}


		void WarpInCharacterState::Update()
		{
			scaleCurve_.Update(g_gameTime->GetFrameDeltaTime());

			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			characterStateMachine->GetCharacterController()->RequestTeleport();
			characterStateMachine->transform.scale = Vector3(scaleCurve_.GetCurrentValue());
			characterStateMachine->transform.position.y -= 1.0f; // NOTE: 下に埋め込みたいので
		}


		void WarpInCharacterState::Exit()
		{
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			characterStateMachine->GetCharacterController()->RequestTeleport();
			characterStateMachine->transform.position = characterStateMachine->GetWarpPosition();
		}


		bool WarpInCharacterState::CanChangeState() const
		{
			if (scaleCurve_.IsPlaying()) {
				return false;
			}
			return true;
		}




		/*************************************/


		WarpOutCharacterState::WarpOutCharacterState(IStateMachine* owner)
			: ICharacterState(owner)
		{
		}


		WarpOutCharacterState::~WarpOutCharacterState()
		{
		}


		void WarpOutCharacterState::Enter()
		{
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			characterStateMachine->GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::Idle));
			auto* characterStatus = characterStateMachine->GetStatus();
			scaleCurve_.Initialize(characterStatus->GetWarpEndScale(), characterStatus->GetWarpStartScale(), characterStatus->GetWarpTimeSeconds(), app::util::EasingType::Linear);
			scaleCurve_.Play();
		}


		void WarpOutCharacterState::Update()
		{
			scaleCurve_.Update(g_gameTime->GetFrameDeltaTime());

			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			characterStateMachine->GetCharacterController()->RequestTeleport();

			characterStateMachine->transform.scale = Vector3(scaleCurve_.GetCurrentValue());
		}


		void WarpOutCharacterState::Exit()
		{
		}


		bool WarpOutCharacterState::CanChangeState() const
		{
			if (scaleCurve_.IsPlaying()) {
				return false;
			}
			return true;
		}
	}
}