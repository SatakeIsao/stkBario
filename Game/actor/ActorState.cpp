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

namespace
{
	// 移動・ノックバック関連
	static const Vector3 KNOCKBACK_SQUASH_SCALE = { 1.0f,0.1f,1.0f };
	constexpr float KNOCKBACK_INITIAL_SPEED = 500.0f;
	constexpr float KNOCKBACK_DECELERATION_TIME = 1.0f;
	constexpr float KNOCKBACK_END_LIMIT_TIME = 2.0f;
	constexpr float LANDING_CHECK_DELAY = 0.1f;

	// 攻撃（パンチ）関連
	static const Vector3 ATTACK_EFFECT_SCALE = { 3.0f, 3.0f, 3.0f };
	constexpr float ATTACK_GHOST_RADIUS = 20.0f;
	constexpr float ATTACK_EFFECT_OFFSET_FORWARD = 30.0f;
	constexpr float ATTACK_EFFECT_OFFSET_Y = 30.0f;
	constexpr float PUNCH_GHOST_RADIUS = 45.0f;
	constexpr float PUNCH_EFFECT_OFFSET_FORWARD = 30.0f;
	constexpr float FORWARD_VECTOR_LENGTH_THRESHOLD = 0.1f;

	// タイム・期間関連
	constexpr float ATTACK_STATE_CHANGE_THRESHOLD = 3.0f;
	constexpr float DEAD_STATE_DEFAULT_TRANSITION_TIME = 2.0f;
	constexpr float JUMP_ANIMATION_SPEED = 2.5f;

	// その他オフセット
	constexpr float WARP_IN_Y_OFFSET = -1.0f;
}

namespace app
{
	namespace actor
	{
		IdleCharacterState::IdleCharacterState(IStateMachine* owner)
			: ICharacterState(owner)
		{}


		IdleCharacterState::~IdleCharacterState()
		{}


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
		{}


		RunCharacterState::~RunCharacterState()
		{}


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
			//if (runBody_ != nullptr)
			//{
			//	delete runBody_;
			//	runBody_ = nullptr;
			//}
		}




		/*************************************/


		AttackCharacterState::AttackCharacterState(IStateMachine* owner)
			:ICharacterState(owner)
		{}


		AttackCharacterState::~AttackCharacterState()
		{}


		void AttackCharacterState::Enter()
		{
			stateTimer_ = 0.0f;

			// 攻撃用のゴーストオブジェクトを作成
			attackScheduler_ = std::make_unique<app::core::TaskSchedulerSystem>();
			attackScheduler_->AddTimer(0.1f, [&]()
				{
					auto* characterStateMachine = owner_->As<CharacterStateMachine>();
					attackBody_ = new app::collision::GhostBody();
					attackBody_->CreateSphere(characterStateMachine->GetCharacter(), characterStateMachine->GetCharacterID(), ATTACK_GHOST_RADIUS, app::collision::ghost::CollisionAttribute::Enemy, app::collision::ghost::CollisionAttributeMask::All);
					isAttackBody_ = true;

					// スライムが攻撃した瞬間に自分でエフェクトを出す
					if (app::battle::BattleManager::Get().GetPlayerHP() > 0) {
						app::battle::BattleManager::Get().GetEffectManager()->PlayEffect(
							enEffectKind_SlimeAttack,
							characterStateMachine->transform.position + (characterStateMachine->GetMoveDirection() * ATTACK_EFFECT_OFFSET_FORWARD) + Vector3(0.0f, ATTACK_EFFECT_OFFSET_Y, 0.0f),
							Quaternion::Identity,
							ATTACK_EFFECT_SCALE
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

					if (forward.LengthSq() < FORWARD_VECTOR_LENGTH_THRESHOLD) {
						forward = Vector3::Front;
					}
					attackBody_->SetPosition(characterStateMachine->transform.position + forward * (radius + radius) + Vector3(0.0f, radius, 0.0f));
				}, false);

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
			return stateTimer_ > ATTACK_STATE_CHANGE_THRESHOLD;
		}


		/*************************************/


		JumpCharacterState::JumpCharacterState(IStateMachine* owner)
			: ICharacterState(owner)
		{}


		JumpCharacterState::~JumpCharacterState()
		{}


		void JumpCharacterState::Enter()
		{
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			auto* characterStatus = characterStateMachine->GetStatus();

			characterStateMachine->Jump(characterStatus->GetJumpPower());

			characterStateMachine->GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::JumpAscend));

			characterStateMachine->GetModelRender()->SetAnimationSpeed(JUMP_ANIMATION_SPEED);

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
		{}


		FallingCharacterState::~FallingCharacterState()
		{}


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
		{}




		/*************************************/


		PunchCharacterState::PunchCharacterState(IStateMachine* owner)
			: ICharacterState(owner)
		{}


		PunchCharacterState::~PunchCharacterState()
		{}


		void PunchCharacterState::Enter()
		{
			attackScheduler_ = std::make_unique<app::core::TaskSchedulerSystem>();
			attackScheduler_->AddTimer(0.1f, [&]()
				{
					auto* characterStateMachine = owner_->As<CharacterStateMachine>();
					characterStateMachine->GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::Punch));
					attackBody_ = new app::collision::GhostBody();
					attackBody_->CreateSphere(characterStateMachine->GetCharacter(), characterStateMachine->GetCharacterID(), PUNCH_GHOST_RADIUS, app::collision::ghost::CollisionAttribute::Player, app::collision::ghost::CollisionAttributeMask::All);
					// @todo for test
					const float radius = characterStateMachine->GetStatus()->GetRadius();
					attackBody_->SetPosition(characterStateMachine->transform.position + characterStateMachine->GetMoveDirection() * (radius + radius) + Vector3(0.0f, radius, 0.0f));

					// パンチが発生した瞬間にエフェクトを再生
					if (app::battle::BattleManager::IsAvailable() && app::battle::BattleManager::Get().GetEffectManager())
					{
						app::battle::BattleManager::Get().GetEffectManager()->PlayEffect(
							enEffectKind_SlimeAttack,
							characterStateMachine->transform.position + (characterStateMachine->GetMoveDirection() * PUNCH_EFFECT_OFFSET_FORWARD),
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
		{}


		WarpInCharacterState::~WarpInCharacterState()
		{}


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
			characterStateMachine->transform.position.y += WARP_IN_Y_OFFSET; // NOTE: 下に埋め込みたいので
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
		{}


		WarpOutCharacterState::~WarpOutCharacterState()
		{}


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
		{}


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
		{}


		DeadCharacterState::~DeadCharacterState()
		{}


		void DeadCharacterState::Enter()
		{
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			characterStateMachine->OnEnterDead();

			timer_ = 0.0f;
			shrinkTimer_ = 0.0f;
			knockBackTimer_ = 0.0f;

			// Squashから遷移した場合は (1,0.1,1)、通常死亡は (1,1,1) が入る
			startScale_ = characterStateMachine->transform.scale;

			// ノックバック中に死亡したか（パンチでHP0限定）
			isKnockBackDead_ = false;
			if (auto* eventMachine = owner_->As<EventCharacterStateMachine>())
			{
				isKnockBackDead_ = eventMachine->IsKnockBackDead();
			}

			// Jump(80.0f) による上昇が始まる前に着地判定が走らないよう
			// knockBackTimer_ を少し進めておく（0.1f 未満は着地チェックしない）
			if (isKnockBackDead_)
			{
				knockBackTimer_ = 0.0f; // 着地チェックは 0.1f 以降から行う
			}
		}


		void DeadCharacterState::Update()
		{
			timer_ += g_gameTime->GetFrameDeltaTime();

			// スケール縮小はスライム（EventCharacterStateMachine）のみ
			auto* eventMachine = owner_->As<EventCharacterStateMachine>();
			if (eventMachine == nullptr) { return; }

			auto* characterStateMachine = owner_->As<CharacterStateMachine>();

			// ノックバック死の場合：KnockBackStateと同じ移動ロジックで吹っ飛びを継続
			if (isKnockBackDead_)
			{
				knockBackTimer_ += g_gameTime->GetFrameDeltaTime();

				float deceleration = 1.0f - knockBackTimer_;
				if (deceleration < 0.0f) { deceleration = 0.0f; }

				float currentSpeed = KNOCKBACK_INITIAL_SPEED * deceleration;
				characterStateMachine->Move(g_gameTime->GetFrameDeltaTime(), currentSpeed);

				// 着地 or 一定時間で移動終了
				bool isLanded = false;
				if (knockBackTimer_ > LANDING_CHECK_DELAY)
				{
					isLanded = characterStateMachine->GetCharacterController()->IsOnGround();
				}
				if (isLanded || knockBackTimer_ > KNOCKBACK_END_LIMIT_TIME)
				{
					isKnockBackDead_ = false;
				}
			}

			shrinkTimer_ += g_gameTime->GetFrameDeltaTime();
			const float t = min(shrinkTimer_ / SHRINK_DURATION, 1.0f);
			if (t < 1.0f)
			{
				if (eventMachine->IsSquashedDead())
				{
					// 踏まれてHP0 → ぺっちゃんこ(1,0.1,1)のまま XZ を縮小
					// Y は 0.1 固定で徐々に消える
					const float xz = startScale_.x * (1.0f - t);
					const float y = startScale_.y;				// Y は変えない
					characterStateMachine->transform.scale = Vector3(xz, y, xz);
				}
				else
				{
					// パンチ死など通常死亡 → XYZ 均等縮小
					characterStateMachine->transform.scale = Vector3(
						startScale_.x * (1.0f - t),
						startScale_.y * (1.0f - t),
						startScale_.z * (1.0f - t)
					);
				}
			}
			else
			{
				characterStateMachine->transform.scale = Vector3::Zero;
				eventMachine->SetDeadScaleFinished(true);
			}
		}


		void DeadCharacterState::Exit()
		{
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			characterStateMachine->OnExitDead();
		}


		bool DeadCharacterState::CanChangeState() const
		{
			// スライム: 縮小完了で遷移可能
			if (owner_->As<EventCharacterStateMachine>() != nullptr)
			{
				return shrinkTimer_ >= SHRINK_DURATION;
			}
			return timer_ > DEAD_STATE_DEFAULT_TRANSITION_TIME;
		}




		/*************************************/


		SquashCharacterState::SquashCharacterState(IStateMachine* owner)
			: ICharacterState(owner)
		{}


		SquashCharacterState::~SquashCharacterState()
		{}


		void SquashCharacterState::Enter()
		{
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();

			// ぺっちゃんこスケールに即座にセット
			characterStateMachine->transform.scale = KNOCKBACK_SQUASH_SCALE;

			// アニメーション再生
			characterStateMachine->GetModelRender()->PlayAnimation(
				static_cast<uint8_t>(app::actor::SlimeAnimationKind::Dead));

			// エフェクト
			if (app::battle::BattleManager::IsAvailable() && app::battle::BattleManager::Get().GetEffectManager())
			{
				app::battle::BattleManager::Get().GetEffectManager()->PlayEffect(
					enEffectKind_SlimeKnockBack,
					characterStateMachine->transform.position,
					Quaternion::Identity,
					Vector3::One
				);
			}

			// SE
			app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::SlimeSquash));

			timer_ = 0.0f;
		}


		void SquashCharacterState::Update()
		{
			timer_ += g_gameTime->GetFrameDeltaTime();
		}


		void SquashCharacterState::Exit()
		{
			auto* characterStateMachine = owner_->As<CharacterStateMachine>();
			auto* eventMachine = owner_->As<EventCharacterStateMachine>();

			// Dead に遷移する場合はスケールをそのまま維持
			if (eventMachine != nullptr && eventMachine->IsDead())
			{
				// スケールはそのまま
				return;
			}

			// 復活する場合のみスケールを元に戻してSEを鳴らす
			characterStateMachine->transform.scale = Vector3::One;
			app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::SlimeRevival));
		}


		bool SquashCharacterState::CanChangeState() const
		{
			return timer_ >= SQUASH_DURATION;
		}




		/*************************************/


		KnockBackCharacterState::KnockBackCharacterState(IStateMachine* owner)
			: ICharacterState(owner)
		{}


		KnockBackCharacterState::~KnockBackCharacterState()
		{}


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

			if (timer_ > LANDING_CHECK_DELAY)
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
				float currentSpeed = KNOCKBACK_INITIAL_SPEED * deceleration;

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

			if (timer_ > LANDING_CHECK_DELAY)
			{
				isLanded = characterStateMachine->GetCharacterController()->IsOnGround();
			}

			return ((isAnimFinished && isLanded) || timer_ > KNOCKBACK_END_LIMIT_TIME);
		}
	}
}