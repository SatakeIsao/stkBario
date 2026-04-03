/**
 * Actorファイル
 */
#include "stdafx.h"
#include "ActorStateMachine.h"
#include "Actor.h"
#include "ActorStatus.h"
#include "BattleCharacter.h"
#include "EventCharacter.h"
#include "battle/BattleManager.h"
#include "sound/SoundManager.h"


namespace
{
	Vector3 ComputeCameraDirection(const Vector3& inputDirection)
	{
		// カメラの前方向と右方向のベクトルを取得
		Vector3 forward = g_camera3D->GetForward();
		Vector3 right = g_camera3D->GetRight();

		// y方向には移動しない
		forward.y = 0.0f;
		right.y = 0.0f;

		// 左スティックの入力量を加算
		right *= inputDirection.x;
		forward *= inputDirection.z;

		Vector3 direction = right + forward;

		direction.Normalize();

		return direction;
	}


	Quaternion ComputeRotation(const Vector3& inputDirection)
	{
		// スティックの方向
		Vector3 direction = ComputeCameraDirection(inputDirection);
		// スティック入力を使ってY軸回転の情報を見る
		Quaternion q;
		q.SetRotationYFromDirectionXZ(direction);

		return q;
	}
}


namespace app
{
	namespace actor
	{
		void IStateMachine::UpdateStateCore()
		{
			//次のステートにしてね。という予約があれば
			if (nextStateId_ != INVALID_STATE_ID && currentStateId_ != nextStateId_)
			{
				// ステート変更
				// 現在のステートを抜ける
				if (currentState_) {
					currentState_->Exit();
				}
				// 次のステートに入る
				currentState_ = std::unique_ptr<ICharacterState>(CreateState(nextStateId_));
				if (currentState_)
				{
					currentState_->Enter();
				}
				currentStateId_ = nextStateId_;
				//次のステートを初期化
				nextStateId_ = INVALID_STATE_ID;
			}
			K2_ASSERT(currentState_ != nullptr, "状態の生成がされません\n");
			currentState_->Update();
		}


		bool IStateMachine::CanChangeState() const
		{
			return currentState_->CanChangeState();
		}




		/************************************/


		CharacterStateMachine::CharacterStateMachine()
		{
			//初期ステート
			//SetCurrentState(RunCharacterState::ID());
			RequestChangeState(IdleCharacterState::ID());
		}

		CharacterStateMachine::~CharacterStateMachine()
		{
		}


		void CharacterStateMachine::Update()
		{
			//ステート更新
			UpdateStateCore();
		}


		void CharacterStateMachine::Move(const float deltaTime, const float moveSpeed)
		{
			// カメラの向きを考慮するか
			Vector3 moveVector = moveDirection_;
			if (isUseCameraDirection_)
			{
				// TODO: 将来的にCharacterControllerを使って衝突判定をする
				moveVector = ComputeCameraDirection(moveDirection_);
			}

			const Vector3 moveSpeedVector = moveVector * moveSpeed;
			if (inputPower_)
			{
				moveSpeedVector_ = moveSpeedVector;
			}
			moveSpeedVector_ *= GetStatus()->GetFriction(); // 摩擦係数的な
			transform.position += moveSpeedVector_ * deltaTime;
		}


		void CharacterStateMachine::Jump(const float jumoPower)
		{
			character_->GetCharacterController()->Jump(jumoPower);
		}


		Character* CharacterStateMachine::GetCharacter()
		{
			return character_;
		}


		app::actor::CharacterStatus* CharacterStateMachine::GetStatus()
		{
			return character_->GetStatus();
		}


		CharacterController* CharacterStateMachine::GetCharacterController()
		{
			return character_->GetCharacterController();
		}


		ModelRender* CharacterStateMachine::GetModelRender()
		{
			return character_->GetModelRender();
		}




		/************************************/


		BattleCharacterStateMachine::BattleCharacterStateMachine()
		{
		}


		BattleCharacterStateMachine::~BattleCharacterStateMachine()
		{
		}


		void BattleCharacterStateMachine::Initialize()
		{
			SuperClass::Initialize();

			isUseCameraDirection_ = true;
		}


		void BattleCharacterStateMachine::Update()
		{
			UpdateState();

			SuperClass::Update();
		}


		uint32_t BattleCharacterStateMachine::GetCharacterID() const
		{
			return BattleCharacter::ID();
		}


		void BattleCharacterStateMachine::OnEnterKnockBack()
		{
			GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::KnockBack));

			// プレイヤーがノックバックした時だけ再生
			if (app::battle::BattleManager::IsAvailable() && app::battle::BattleManager::Get().GetEffectManager())
			{
				app::battle::BattleManager::Get().GetEffectManager()->PlayEffect(
					enEffectKind_PlayerKnockBack,
					transform.position,
					Quaternion::Identity,
					Vector3::One
				);
				//後で変更
				app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::SlimeknockBack));
			}
		}


		void BattleCharacterStateMachine::OnExitKnockBack()
		{
		}


		void BattleCharacterStateMachine::OnEnterDead()
		{
			GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::PlayerAnimationKind::Dead));
		}


		void BattleCharacterStateMachine::OnExitDead()
		{
		}


		void BattleCharacterStateMachine::UpdateState()
		{
			//死亡
			{
				if (isDead_) {
					RequestChangeState(DeadCharacterState::ID());
					return;
				}
			}
			//ノックバック
			{
				if (isKnockBack_) {
					/** デバッグテスト */
					RequestChangeState(KnockBackCharacterState::ID());
					isKnockBack_ = false;
					return;
				}

				if (IsEqualCurrentState(KnockBackCharacterState::ID()))
				{
					if (CanChangeState())
					{
						RequestChangeState(IdleCharacterState::ID());
					}
					return;
				}
			}
			// ワープ
			{
				if (IsRequestWarp()) {
					RequestChangeState(WarpInCharacterState::ID());
					ClearRequestWarp();
					return;
				}
				if (IsEqualCurrentState(WarpInCharacterState::ID())) {
					if (CanChangeState()) {
						RequestChangeState(WarpOutCharacterState::ID());
						return;
					}
					else {
						return;
					}
				}
				if (IsEqualCurrentState(WarpOutCharacterState::ID()))
				{
					if (!CanChangeState()) {
						return;
					}
				}
			}
			// ジャンプ
			{
				if (IsActionA()) {
					RequestChangeState(JumpCharacterState::ID());
					isActionA_ = false;
					return;
				}
				// パンチ中は他の状態に遷移しない
				if (IsEqualCurrentState(JumpCharacterState::ID())) {
					if (!CanChangeState()) {
						return;
					}
				}
			}
			// 攻撃
			{
				if (IsActionB()) {
					RequestChangeState(PunchCharacterState::ID());
					isPunched_ = true;
					isActionB_ = false;

					return;
				}
				else {
					isPunched_ = false;
				}
				// パンチ中は他の状態に遷移しない
				if (IsEqualCurrentState(PunchCharacterState::ID()))
				{
					if (!CanChangeState()) {
						return;
					}
				}
			}
			// 落下
			{
				if (!GetCharacterController()->IsOnGround()) {
					if (!IsEqualCurrentState(FallingCharacterState::ID())) {
						RequestChangeState(FallingCharacterState::ID());
					}
					return;
				}
			}

			const Vector3 direction = moveSpeedVector_;
			if (direction.LengthSq() >= MOVE_MIN_FLOAT || inputPower_ >= MOVE_MIN_FLOAT)
			{
				RequestChangeState(RunCharacterState::ID());
				return;
			}

			RequestChangeState(IdleCharacterState::ID());
		}




		/************************************/


		EventCharacterStateMachine::EventCharacterStateMachine()
		{
		}


		EventCharacterStateMachine::~EventCharacterStateMachine()
		{
		}


		void EventCharacterStateMachine::Initialize()
		{
			SuperClass::Initialize();
			SetCurrentState(INVALID_STATE_ID);
			RequestChangeState(IdleCharacterState::ID());

			isUseCameraDirection_ = false;
		}


		void EventCharacterStateMachine::Update() 
		{
			UpdateState();
			SuperClass::Update();
		}


		void EventCharacterStateMachine::OnEnterDead()
		{
			GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::SlimeAnimationKind::Dead));
			// ぺっちゃんこ
			transform.scale = Vector3(1.0f, 0.1f, 1.0f);
		}


		void EventCharacterStateMachine::OnExitDead()
		{
			// 元に戻す
			transform.scale = Vector3::One;
		}


		void EventCharacterStateMachine::OnEnterKnockBack()
		{
			GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::SlimeAnimationKind::knockBack));
			Jump(80.0f);

			// ノックバックした瞬間にエフェクトを出す
			app::battle::BattleManager::Get().GetEffectManager()->PlayEffect(
				enEffectKind_SlimeKnockBack,
				transform.position,
				Quaternion::Identity,
				Vector3::One
			);
		}


		void EventCharacterStateMachine::OnExitKnockBack()
		{
		}


		uint32_t EventCharacterStateMachine::GetCharacterID() const
		{
			return EventCharacter::ID();
		}


		void EventCharacterStateMachine::OnEnterAttack()
		{
			GetModelRender()->PlayAnimation(static_cast<uint8_t>(app::actor::SlimeAnimationKind::Attack));
		}


		void EventCharacterStateMachine::OnExitAttack()
		{
		}


		void EventCharacterStateMachine::UpdateState()
		{
			if (isDead_)
			{
				RequestChangeState(DeadCharacterState::ID());
				return; // 落下死の場合はずっとDeadのままにするため、ここで処理を終了
			}

			/** スライム自身がプレイヤーを探す */
			Vector3 playerPosition = app::battle::BattleManager::Get().GetPlayerPosition();
			Vector3 slimePosition = transform.position;
			Vector3 diffXZ(playerPosition.x - slimePosition.x, 0.0f, playerPosition.z - slimePosition.z);
			float diff = diffXZ.Length();

			if (diff < 200.0f) {
				Vector3 DirectionToPlayer = diffXZ;
				DirectionToPlayer.Normalize();
				Vector3 slimeForward = Vector3(0.0f, 0.0f, 1.0f);
				transform.localRotation.Apply(slimeForward);
				Vector3 forwardXZ(slimeForward.x, 0.0f, slimeForward.z);
				forwardXZ.Normalize();

				float dot = forwardXZ.Dot(DirectionToPlayer);
				float threshold = std::cos(60.0f * (Math::PI / 180.0f));

				if (dot > threshold) {
					// 視野角内に入ったので追従フラグを立てる
					OnChase(DirectionToPlayer, playerPosition);
				}
			}

			/** TODO: スライムの頭上にPlayerの当たり判定が衝突したらぺっちゃんこ */
			if (IsSquashed())
			{
				RequestChangeState(DeadCharacterState::ID());
				//ぺちゃんこSE流したい

				// Deadステート側で2秒経過して「遷移可能」になったらIdleに戻す
				if (IsEqualCurrentState(DeadCharacterState::ID())
					&& CanChangeState())
				{
					isSquashed_ = false;
					RequestChangeState(IdleCharacterState::ID());
				}
				return;
			}

			/** TODO: パンチ食らったらknockBack */
			if (isKnockBack_)
			{
				// まだノックバック状態になっていない場合（初回の1回だけ通る）
				if (!IsEqualCurrentState(KnockBackCharacterState::ID()))
				{
					// ノックバックする方向を設定
					SetMoveDirection(knockBackDirection_);
					RequestChangeState(KnockBackCharacterState::ID());

					// SEの再生もここに入れることで1回だけ鳴る！
					app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::SlimeknockBack));
				}
				else // すでにノックバック状態になっている場合
				{
					// ノックバックが終わり、別の状態へ遷移可能になったら待機状態に戻す
					if (CanChangeState())
					{
						aiTimer_ = 0.0f;
						isKnockBack_ = false;
						RequestChangeState(IdleCharacterState::ID());
					}
				}
				return;
			}
			
			if (IsEqualCurrentState(AttackCharacterState::ID()))
			{
				if (CanChangeState()) {
					aiTimer_ = 0.0f;
					RequestChangeState(RunCharacterState::ID());
				}
				return;
			}

			/** TODO: 敵の視野角にPlayerが入ったら追従して */
			if (isChasing_)
			{
				isChasing_ = false;

				if (IsEqualCurrentState(IdleCharacterState::ID())
					&&aiTimer_> 5.0f)
				{

				}
				else {
					Vector3 toPlayer = targetPosition_ - transform.position;
					toPlayer.y = 0.0f;
					float distance = toPlayer.Length();

					if (distance <= 40.0f) {
						SetMoveDirection(chaseDirection_);
						RequestChangeState(AttackCharacterState::ID());
					}
					else if (distance <= 200.0f) {
						SetMoveDirection(chaseDirection_);
						RequestChangeState(RunCharacterState::ID());
						aiTimer_ = 5.0f;
					}
					/** DEBUG: いらないかも */
					else
					{
						RequestChangeState(RunCharacterState::ID());
						aiTimer_ = 0.0f;
					}
					return;
				}

				//TODO: もとの下の流れに戻したい
				
				
				
				//aiTimer_ = 5.0f;
				//isChasing_ = false;
				//return;
			}

			app::actor::BattleCharacter* player = nullptr;

			/** デバッグテスト: 待機⇒左に走る⇒右に走る⇒待機のモーション */
			if (IsEqualCurrentState(IdleCharacterState::ID()))
			{
				aiTimer_ += g_gameTime->GetFrameDeltaTime();
				if (aiTimer_ > WAIT_TIME)
				{
					RequestChangeState(RunCharacterState::ID());
					aiTimer_ = 0.0f;
				}
				return;
			}

			if (IsEqualCurrentState(RunCharacterState::ID())) 
			{
				aiTimer_ += g_gameTime->GetFrameDeltaTime();

				if (aiTimer_ <= 2.0f)
				{
					SetMoveDirection(Vector3::Left);
				}
				else if (aiTimer_ <= 4.0f)
				{
					SetMoveDirection(Vector3::Right);
				}
				else {
					SetMoveDirection(Vector3::Zero);
					RequestChangeState(IdleCharacterState::ID());
					aiTimer_ = 0.0f;
				}
				return;
			}

			RequestChangeState(IdleCharacterState::ID());
		}
	}
}