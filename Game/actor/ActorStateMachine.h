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
			std::map<uint32_t, std::function<ICharacterState*()>> stateFuncList_;
			std::unique_ptr<ICharacterState> currentState_ = nullptr;
			/** ステート関連 */
			uint32_t currentStateId_ = INVALID_STATE_ID;
			uint32_t nextStateId_ = INVALID_STATE_ID;


		public:
			IStateMachine() {}
			virtual ~IStateMachine() {}

			virtual void Initialize() = 0;
			virtual void Update() = 0;


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

			Vector3 warpPosition_ = Vector3::Zero;
			bool isRequestWarp_ = false;

			/** ボタンを押したか */
			bool isActionA_ = false;
			bool isActionB_ = false;
			bool isActionDown_ = false;


		public:
			CharacterStateMachine();
			virtual ~CharacterStateMachine();

			virtual void Initialize() override {}
			virtual void Update() override;

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
			void ClearMomveSpeedVector() { moveSpeedVector_ = Vector3::Zero; }

			void SetInputPower(const float power) { inputPower_ = power; }
			float GetInputPower() const { return inputPower_; }


			void SetWarpPosition(const Vector3& position)
			{
				warpPosition_ = position;
				isRequestWarp_ = true;
			}
			const Vector3& GetWarpPosition() const { return warpPosition_; }
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


		public:
			BattleCharacterStateMachine();
			virtual ~BattleCharacterStateMachine();

			virtual void Initialize() override final;
			virtual void Update() override final;

			virtual uint32_t GetCharacterID() const override;


		private:
			void UpdateState();
		};
	}
}