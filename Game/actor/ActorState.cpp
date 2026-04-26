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
#include "battle/BattleManager.h"
#include "ui/AwardManager.h"
#include "sound/SoundManager.h"


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


			//  runBody_ = new app::collision::GhostBody();
			//  runBody_->CreateSphere(characterStateMachine->GetCharacter(), characterStateMachine->GetCharacterID(),10.0f, app::collision::ghost::CollisionAttribute::Player, app::collision::ghost::CollisionAttributeMask::All);
			//  // @todo for test
			//  const float radius = characterStateMachine->GetStatus()->GetRadius();
			//  // キャラクターの現在の回転から前方向ベクトルを計算する
			//  Vector3 forward = Vector3(0.0f, 0.0f, 1.0f); // または Vector3::Front
			//  characterStateMachine->transform.rotation.Apply(forward);
			//  forward.Normalize();
			//  
			//  // キャラクターの正面にゴーストを配置
			//  runBody_->SetPosition(characterStateMachine->transform.position + forward * (radius) + Vector3(0.0f, radius, 0.0f));
			//  //runBody_->SetPosition(characterStateMachine->transform.position + characterStateMachine->GetMoveDirection() * (radius + radius) + Vector3(0.0f, radius, 0.0f));
			//  runBody_->SetRotation(characterStateMachine->transform.rotation);
		}


		void RunCharacterState::Update()
		{
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			auto* characterStatus = characterStateMachine->GetStatus();
			characterStateMachine->Move(g_gameTime->GetFrameDeltaTime(), characterStatus->GetMoveSpeed());

			characterStateMachine->transform.rotation.SetRotationYFromDirectionXZ(characterStateMachine->GetMoveSpeedVector());

			//  //ゴーストの位置をスライムの現在位置に合わせて追従させる
			//  if (runBody_)
			//  {
			//  	const float radius = characterStateMachine->GetStatus()->GetRadius();
			//  	// GetMoveDirection() ではなく、キャラクターの実際の向きを使う
			//  	Vector3 forward = Vector3(0.0f, 0.0f, 1.0f);
			//  	characterStateMachine->transform.rotation.Apply(forward);
			//  	forward.Normalize();
			//  
			//  	// キャラクターの正面に追従させる
			//  	runBody_->SetPosition(characterStateMachine->transform.position + forward * (radius) + Vector3(0.0f, radius, 0.0f));
			//  	runBody_->SetRotation(characterStateMachine->transform.rotation);
			//  }
		}


		void RunCharacterState::Exit()
		{
			//if (runBody_ != nullptr)
			//{
			//	delete runBody_;
			//	runBody_ = nullptr;
			//}
		}




		/*************************************/


		AttackCharacterState::AttackCharacterState(IStateMachine* owner)
			:ICharacterState(owner)
		{
		}


		AttackCharacterState::~AttackCharacterState()
		{
		}


		void AttackCharacterState::Enter()
		{
			stateTimer_ = 0.0f;

			// 攻撃用のゴーストオブジェクトを作成
			attackScheduler_ = std::make_unique<app::core::TaskSchedulerSystem>();
			attackScheduler_->AddTimer(0.1f, [&]()
				{
					auto* characterStateMachine = owner_->As<CharacterStateMachine>();
					attackBody_ = new app::collision::GhostBody();
					attackBody_->CreateSphere(characterStateMachine->GetCharacter(), characterStateMachine->GetCharacterID(), 20.0f, app::collision::ghost::CollisionAttribute::Enemy, app::collision::ghost::CollisionAttributeMask::All);
					isAttackBody_ = true;

					// スライムが攻撃した瞬間に自分でエフェクトを出す
					if (app::battle::BattleManager::Get().GetPlayerHP() > 0) {
						app::battle::BattleManager::Get().GetEffectManager()->PlayEffect(
							enEffectKind_SlimeAttack,
							characterStateMachine->transform.position + (characterStateMachine->GetMoveDirection() * 30.0f) + Vector3(0.0f, 30.0f, 0.0f),
							Quaternion::Identity,
							Vector3(3.0f, 3.0f, 3.0f)
						);

						app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::PlayerPunch));
					}

					if (auto* eventMachine = owner_->As<app::actor::EventCharacterStateMachine>())
					{
						eventMachine->NontifyAttackGhostCreated();
					}

					// @todo for test
					const float radius = characterStateMachine->GetStatus()->GetRadius();
					
					Vector3 forward = characterStateMachine->GetMoveDirection();
					
					if (forward.LengthSq() < 0.01f) {
						forward = Vector3::Front;
					}
					attackBody_->SetPosition(characterStateMachine->transform.position + forward * (radius + radius) + Vector3(0.0f, radius, 0.0f));
			}, false);

			// DEBUG; 削除はEnterではしない
			//ゴースト削除タイマー
			attackScheduler_->AddTimer(0.1f, [&]()
				{
					if (attackBody_ != nullptr) {
						delete attackBody_;
						attackBody_ = nullptr;
						isAttackBody_ = false;
					}
				}, true);
		}


		void AttackCharacterState::Update()
		{
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			auto* characterStatus = characterStateMachine->GetStatus();

			//攻撃中も移動を続けるための処理
			//移動処理とY回転の更新
			characterStateMachine->Move(g_gameTime->GetFrameDeltaTime(), characterStatus->GetMoveSpeed());
			characterStateMachine->transform.rotation.SetRotationYFromDirectionXZ(characterStateMachine->GetMoveSpeedVector());

			//  //ゴーストの位置をスライムの現在位置に合わせて追従させる
			if (attackBody_)
			{
				const float radius = characterStateMachine->GetStatus()->GetRadius();
				Vector3 forward = characterStateMachine->GetMoveDirection();
				if (forward.LengthSq() <= 0.01f)
				{
					forward = Vector3::Front;
				}
				attackBody_->SetPosition(characterStateMachine->transform.position + forward * (radius + radius) + Vector3(0.0f, radius, 0.0f));
			}

			stateTimer_ += g_gameTime->GetFrameDeltaTime();
			if (attackScheduler_)
			{
				attackScheduler_->Update(g_gameTime->GetFrameDeltaTime());
			}
		}


		void AttackCharacterState::Exit()
		{
			attackScheduler_.reset(nullptr);

			if (attackBody_ != nullptr) 
			{
				delete attackBody_;
				attackBody_ = nullptr;
				isAttackBody_ = false;
			}
		}


		bool AttackCharacterState::CanChangeState() const
		{
			/** TODO; ある程度の距離外になったら　　アニメーション再生は廃止したいな
			     あくまで攻撃ステートは攻撃用のゴーストオブジェクトを付与してるだけ
				 ゴーストの付与の切り替えかな
			 */


			return stateTimer_ >3.0f;
			
			//auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			//auto* modelRender = characterStateMachine->GetModelRender();
			//return !modelRender->IsPlayingAnimation();
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

			if (app::ui::AwardManager::IsAvailable()) {
				app::ui::AwardManager::Get().AddJumpCount();
			}

			app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Jump));
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
					characterStateMachine->GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::JumpFalling));
					jumpPhase_ = JumpPhase::Falling;
				}
				break;
			}
			case JumpPhase::Falling:
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


		FallingCharacterState::FallingCharacterState(IStateMachine* owner)
			: ICharacterState(owner)
		{
		}


		FallingCharacterState::~FallingCharacterState()
		{
		}


		void FallingCharacterState::Enter()
		{
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			characterStateMachine->GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::JumpFalling));
		}


		void FallingCharacterState::Update()
		{
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			auto* characterStatus = characterStateMachine->GetStatus();
			characterStateMachine->Move(g_gameTime->GetFrameDeltaTime(), characterStatus->GetJumpMoveSpeed());
			characterStateMachine->transform.rotation.SetRotationYFromDirectionXZ(characterStateMachine->GetMoveSpeedVector());
		}


		void FallingCharacterState::Exit()
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
			attackScheduler_ = std::make_unique<app::core::TaskSchedulerSystem>();
			attackScheduler_->AddTimer(0.1f, [&]()
				{
					auto* characterStateMachine = owner_->As<CharacterStateMachine>();
					characterStateMachine->GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::Punch));
					attackBody_ = new app::collision::GhostBody();
					attackBody_->CreateSphere(characterStateMachine->GetCharacter(), characterStateMachine->GetCharacterID(), 30.0f, app::collision::ghost::CollisionAttribute::Player, app::collision::ghost::CollisionAttributeMask::All);
					// @todo for test
					const float radius = characterStateMachine->GetStatus()->GetRadius();
					attackBody_->SetPosition(characterStateMachine->transform.position + characterStateMachine->GetMoveDirection() * (radius + radius) + Vector3(0.0f, radius, 0.0f));

					// パンチが発生した瞬間にエフェクトを再生
					if (app::battle::BattleManager::IsAvailable() && app::battle::BattleManager::Get().GetEffectManager())
					{
						app::battle::BattleManager::Get().GetEffectManager()->PlayEffect(
							enEffectKind_SlimeAttack, // ※元のコードの指定のままにしています。プレイヤー用エフェクトがあれば変更してください。
							characterStateMachine->transform.position + (characterStateMachine->GetMoveDirection() * 30.0f),
							Quaternion::Identity,
							Vector3::One
						);

						app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::PlayerPunch));
					}
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
			if (attackBody_ != nullptr) 
			{
				delete attackBody_;
				attackBody_ = nullptr;
			}
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
			characterStateMachine->GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::JumpFalling));
			characterStateMachine->SetInputPower(0.0f);
			characterStateMachine->ClearMomveSpeedVector();
			auto* characterStatus = characterStateMachine->GetStatus();
			scaleCurve_.Initialize(characterStatus->GetWarpStartScale(), characterStatus->GetWarpEndScale(), characterStatus->GetWarpTimeSeconds(), app::util::EasingType::Linear);
			scaleCurve_.Play();
			translateCurve_.Initialize(characterStateMachine->transform.position, characterStateMachine->GetWarpStartPosition(), characterStatus->GetWarpTimeSeconds() * 0.3f, app::util::EasingType::Linear);
			translateCurve_.Play();

			if (app::ui::AwardManager::IsAvailable()) {
				app::ui::AwardManager::Get().OnDokan();
			}

			app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Warp));
		} 


		void WarpInCharacterState::Update()
		{
			const float deltaTime = g_gameTime->GetFrameDeltaTime();
			scaleCurve_.Update(deltaTime);
			translateCurve_.Update(deltaTime);

			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			characterStateMachine->GetCharacterController()->RequestTeleport();
			characterStateMachine->transform.scale = Vector3(scaleCurve_.GetCurrentValue());
			Vector3 newPosition = translateCurve_.GetCurrentValue();
			characterStateMachine->transform.position.x = newPosition.x;
			characterStateMachine->transform.position.y -= 1.0f; // NOTE: 下に埋め込みたいので
			characterStateMachine->transform.position.z = newPosition.z;
		}


		void WarpInCharacterState::Exit()
		{
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			characterStateMachine->GetCharacterController()->RequestTeleport();
			characterStateMachine->transform.position = characterStateMachine->GetWarpEndPosition();
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


			app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Warp));
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




		/*************************************/


		DeadCharacterState::DeadCharacterState(IStateMachine* owner)
			:ICharacterState(owner)
		{
		}


		DeadCharacterState::~DeadCharacterState()
		{
		}


		void DeadCharacterState::Enter()
		{
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			//キャラクター固有の死亡処理を実行
			characterStateMachine->OnEnterDead();

			timer_ = 0.0f;
		}


		void DeadCharacterState::Update()
		{
			timer_ += g_gameTime->GetFrameDeltaTime();
		}


		void DeadCharacterState::Exit()
		{ 
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			// DEBUG_TEST: キャラクター固有の志望解除のを実行
			characterStateMachine->OnExitDead();
		}


		bool DeadCharacterState::CanChangeState() const
		{
			return timer_ > 2.0f;
		}

		


		/*************************************/


		KnockBackCharacterState::KnockBackCharacterState(IStateMachine* owner)
			: ICharacterState(owner)
		{
		}


		KnockBackCharacterState::~KnockBackCharacterState()
		{
		}


		void KnockBackCharacterState::Enter()
		{
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			characterStateMachine->OnEnterKnockBack();

			/** SE流す */
		}
		

		void KnockBackCharacterState::Update()
		{
			timer_ += g_gameTime->GetFrameDeltaTime();
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();

			bool isLanded = false;

			if (timer_ > 0.1f)
			{
				isLanded = characterStateMachine->GetCharacterController()->IsOnGround();
			}

			if (!isLanded)
			{
				//時間経過で徐々に減衰させる
				float deceleration = 1.0f - timer_;
				if (deceleration < 0.0f) {
					deceleration = 0.0f;
				}
				//スピード調整
				float currentSpeed = 500.0f * deceleration;

				characterStateMachine->Move(g_gameTime->GetFrameDeltaTime(), currentSpeed);
			}
		}


		void KnockBackCharacterState::Exit()
		{
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			characterStateMachine->OnExitKnockBack();
		}


		bool KnockBackCharacterState::CanChangeState() const
		{
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();

			bool isAnimFinished = !characterStateMachine->GetModelRender()->IsPlayingAnimation();
			bool isLanded = false;

			if (timer_ > 0.1f)
			{
				isLanded = characterStateMachine->GetCharacterController()->IsOnGround();
			}

			return ((isAnimFinished && isLanded) || timer_ > 2.0f);
		}
	}
}