/**
 * ActorStateMachineファイル
 */
#pragma once
#include <util/util.h>
#include "ActorState.h"


namespace app
{
	namespace actor
	{
		class CharacterStatus;
		class Character;


		class IStateMachine : public Noncopyable
		{
			/** 例外としてpublic */
		public:
			app::math::Transform transform;

			/** 触らせない */
		private:
			std::map<uint32_t, std::function<ICharacterState* ()>> stateFuncList_;
			std::unique_ptr<ICharacterState> currentState_ = nullptr;
			/** ステート関連 */
			uint32_t currentStateId_ = INVALID_STATE_ID;
			uint32_t nextStateId_ = INVALID_STATE_ID;


		public:
			IStateMachine() {}
			virtual ~IStateMachine() {}

			virtual void Initialize() = 0;
			virtual void Update() = 0;

			uint32_t GetCurrentStateID() const { return currentStateId_; }


		public:
			template <typename T>
			bool Is() const
			{
				auto* ptr = dynamic_cast<T*>(this);
				return ptr != nullptr;
			}
			template <typename T>
			T* As()
			{
				return dynamic_cast<T*>(this);
			}



			/** ステート関連 */
		protected:
			void UpdateStateCore();

			bool CanChangeState() const;

			void SetCurrentState(const uint32_t stateId)
			{
				currentStateId_ = stateId;
			}

			bool IsEqualCurrentState(const uint32_t stateId) const
			{
				return currentStateId_ == stateId;
			}

			void RequestChangeState(const uint32_t stateId)
			{
				if (nextStateId_ == stateId) return;
				if (stateId == INVALID_STATE_ID) return;
				nextStateId_ = stateId;
			}


		public:
			template <typename T>
			void AddState()
			{
				stateFuncList_.emplace(T::ID(), [&]() { return new T(this); });
			}

		private:
			ICharacterState* CreateState(const uint32_t stateId)
			{
				auto it = stateFuncList_.find(stateId);
				if (it != stateFuncList_.end())
				{
					return it->second();
				}
				K2_ASSERT(false, "指定された状態がありません");
				return nullptr;
			}
		};




		class CharacterStateMachine : public IStateMachine
		{
		protected:
			/** ステートマシンを持っているきゃらくたーの情報 */
			Character* character_ = nullptr;

			/** 移動関連 */
			Vector3 moveDirection_ = Vector3::Front;
			Vector3 moveSpeedVector_ = Vector3::Zero;
			float inputPower_ = 1.0f;

			Vector3 warpStartPosition_ = Vector3::Zero;
			Vector3 warpEndPosition_ = Vector3::Zero;
			bool isRequestWarp_ = false;

			/** 移動にカメラの向きを考慮するか */
			bool isUseCameraDirection_ = true;

			/** ボタンを押したか */
			bool isActionA_ = false;
			bool isActionB_ = false;
			bool isActionDown_ = false;


		public:
			CharacterStateMachine();
			virtual ~CharacterStateMachine();

			virtual void Initialize() override {}
			virtual void Update() override;

			/** 攻撃ステートに入った時の固有処理 */
			virtual void OnEnterAttack() {}
			/** 攻撃ステートから抜ける時の固有処理 */
			virtual void OnExitAttack() {}
			/** ノックバックステートに入った時の固有処理 */
			virtual void OnEnterKnockBack() {}
			/** ノックバックステートから抜ける時の固有処理 */
			virtual void OnExitKnockBack() {}
			/** 死んだステートに入った時の固有処理 */
			virtual void OnEnterDead() {}
			/** 死んだステートから抜ける時の固有処理 */
			virtual void OnExitDead() {}

			void Move(const float deltaTime, const float moveSpeed);
			void Jump(const float jumoPower);

			void Setup(Character* character)
			{
				character_ = character;
			}

			virtual uint32_t GetCharacterID() const { return 0; }

			Character* GetCharacter();
			app::actor::CharacterStatus* GetStatus();
			CharacterController* GetCharacterController();
			ModelRender* GetModelRender();



			/** 移動関連 */
		public:
			void SetMoveDirection(const Vector3& direction) { moveDirection_ = direction; }
			const Vector3& GetMoveDirection() const { return moveDirection_; }
			const Vector3& GetMoveSpeedVector() const { return moveSpeedVector_; }
			void ClearMomveSpeedVector() { moveSpeedVector_ = Vector3::Zero; }

			void SetInputPower(const float power) { inputPower_ = power; }
			float GetInputPower() const { return inputPower_; }


			void SetWarpPosition(const Vector3& start, const Vector3& end)
			{
				warpStartPosition_ = start;
				warpEndPosition_ = end;
				isRequestWarp_ = true;
			}
			const Vector3& GetWarpStartPosition() const { return warpStartPosition_; }
			const Vector3& GetWarpEndPosition() const { return warpEndPosition_; }
			const bool IsRequestWarp() const { return isRequestWarp_; }
			void ClearRequestWarp() { isRequestWarp_ = false; }




			/** 入力周り */
		public:
			void SetActionA(const bool isAction) { isActionA_ = isAction; }
			bool IsActionA() const { return isActionA_; }
			void SetActionB(const bool isAction) { isActionB_ = isAction; }
			bool IsActionB() const { return isActionB_; }
			void SetActionDown(const bool isAction) { isActionDown_ = isAction; }
			bool IsActionDown() const { return isActionDown_; }
		};




		class BattleCharacterStateMachine : public CharacterStateMachine
		{
		private:
			using SuperClass = CharacterStateMachine;
			/** AI用のタイマー */
			float aiTimer_ = 0.0f;
			/** 最初の待機時間 */
			const float WAIT_TIME = 1.0f;
			/** 死んだか */
			bool isDead_ = false;
			/** ノックバックしたか */
			bool isKnockBack_ = false;
			/** パンチしたか */
			bool isPunched_ = false;
		public:
			BattleCharacterStateMachine();
			virtual ~BattleCharacterStateMachine();

			virtual void Initialize() override final;
			virtual void Update() override final;

			virtual uint32_t GetCharacterID() const override;

			virtual void OnEnterKnockBack() override;
			virtual void OnExitKnockBack() override;

			virtual void OnEnterDead() override;
			virtual void OnExitDead() override;

		private:
			void UpdateState();

		public:
			/** 死んだことを教える */
			void OnDead()
			{
				isDead_ = true;
			}
			/** ノックバックしたことを教える */
			void OnKnockBack()
			{
				if (IsEqualCurrentState(KnockBackCharacterState::ID())) {
					return;
				}
				isKnockBack_ = true;
			}

			//TODO: Isに変更
			/** ノックバックしたことを取得 */
			bool GetKnockBack()
			{
				return isKnockBack_;
			}
			/** パンチしたことを取得 */
			bool IsPunched()
			{
				return isPunched_;
			}
		};




		class EventCharacterStateMachine : public CharacterStateMachine
		{
		private:
			using SuperClass = CharacterStateMachine;
			app::collision::GhostBody* attackBody_ = nullptr;
			std::unique_ptr<app::core::TaskSchedulerSystem> attackScheduler_;

			Vector3 targetPosition_ = Vector3::Zero;
			Vector3 chaseDirection_ = Vector3::Zero;
			Vector3 knockBackDirection_ = Vector3::Zero;
			/** AI用のタイマー */
			float aiTimer_ = 0.0f;
			/** 踏まれたか */
			bool isSquashed_ = false;
			/** 視野角に入ったか */
			bool isViewAngle_ = false;
			/** 追いかけているか */
			bool isChasing_ = false;
			/** パンチされたか */
			bool isKnockBack_ = false;

			bool isAttackGhostCreated_ = false;

			bool isDead_ = false;

			/** ライフ（パンチ3回で死亡） */
			int hp_ = 3;
			/** 死亡スケール縮小が完了したか（外部からの削除タイミング検知用） */
			bool isDeadScaleFinished_ = false;
			/** 踏まれてHP0になって死んだか（ぺっちゃんこ縮小の判定用） */
			bool isSquashedDead_ = false;
			/** パンチでHP0になって死んだか（吹き飛びながら縮小の判定用） */
			bool isKnockBackDead_ = false;

		public:
			EventCharacterStateMachine();
			virtual ~EventCharacterStateMachine();

			virtual void Initialize() override final;
			virtual void Update() override final;

			virtual uint32_t GetCharacterID() const override;

			virtual void OnEnterAttack() override;
			virtual void OnExitAttack() override;
			virtual void OnEnterDead() override;
			virtual void OnExitDead() override;
			virtual void OnEnterKnockBack() override;
			virtual void OnExitKnockBack() override;

		private:
			void UpdateState();

		public:
			void OnDead() { isDead_ = true; }
			bool IsDead() const { return isDead_; }

			/** 外から踏まれたことを教える */
			void OnSquashed()
			{
				// 既に死亡済み・Squash中は無視
				if (isDead_) { return; }
				if (IsEqualCurrentState(SquashCharacterState::ID())) { return; }

				hp_--;
				if (hp_ <= 0)
				{
					hp_ = 0;
					isDead_ = true;
					isSquashedDead_ = true;	// 踏まれてHP0 → ぺっちゃんこ縮小フラグ
				}
				isSquashed_ = true;
				aiTimer_ = 0.0f;
			}
			bool IsSquashed() const { return isSquashed_; }
			bool IsSquashedDead() const { return isSquashedDead_; }

			/** 視野角に入ったことを教える */
			void OnViewAngle(const Vector3& targetPos) {
				isViewAngle_ = true;
				targetPosition_ = targetPos;
			}

			bool IsViewAngle() const { return isViewAngle_; }

			/** 追跡開始を教える */
			void OnChase(const Vector3& direction, const Vector3& targetPos) {
				isChasing_ = true;
				chaseDirection_ = direction;
				targetPosition_ = targetPos;
			}

			/**
			 * パンチを食らった時に呼ぶ。
			 * HP を 1 減算し、0以下になれば死亡状態へ遷移する。
			 * HP が残っている場合はノックバック状態へ遷移する。
			 */
			void OnKnockBack(const Vector3& direction)
			{
				// 既に死亡済み・ノックバック中は無視
				if (isDead_) { return; }
				if (IsEqualCurrentState(KnockBackCharacterState::ID())) { return; }

				hp_--;
				if (hp_ <= 0)
				{
					// HP 切れ → 死亡
					hp_ = 0;
					isDead_ = true;
					isKnockBackDead_ = true;	// パンチで死亡 → 吹き飛びながら縮小
				}
				// HP残あり・HP0問わずノックバックは発生する
				isKnockBack_ = true;
				knockBackDirection_ = direction;
			}
			bool IsKnockBack()
			{
				return isKnockBack_;
			}

			/** 現在のHPを取得する */
			int GetHP() const { return hp_; }

			/** 死亡スケール縮小が完了したか（EventCharacter が削除タイミングを検知するために使う） */
			bool IsDeadScaleFinished() const { return isDeadScaleFinished_; }
			void SetDeadScaleFinished(bool value) { isDeadScaleFinished_ = value; }

			bool IsKnockBackDead() const { return isKnockBackDead_; }

			//ゴーストが生成されたことを通知
			void NontifyAttackGhostCreated()
			{
				isAttackGhostCreated_ = true;
			}


			bool CheckAndConsumeAttackGhostCreated()
			{
				if (isAttackGhostCreated_) {
					isAttackGhostCreated_ = false;
					return true;
				}
				return false;
			}
		};
	}
}