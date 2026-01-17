/**
 * ActorStateMachineファイル
 */
#pragma once
#include <util/util.h>
#include "ActorState.h"


class IStateMachine : public Noncopyable
{
	/** 例外としてpublic */
public:
	Transform transform;

	/** 触らせない */
private:
	std::map<uint32_t, std::unique_ptr<ICharacterState>> stateList_;
	/** ステート関連 */
	uint32_t currentStateId_ = INVALID_STATE_ID;
	uint32_t nextStateId_ = INVALID_STATE_ID;


protected:
	/** 移動関連 */
	Vector3 moveDirection_ = Vector3::Front;

	// @todo for あとでCharacterSたてMachine側にもっていこうかなぁ
	bool isActionA_ = false;
	bool isActionB_ = false;



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

	void SetCurrentState(const uint32_t stateId)
	{
		currentStateId_ = stateId;
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
//#if defined(K2_DEBUG)
//		if (IsContainsState(T::ID())) {
//			K2_ASSERT(false, "同じ状態が既に存在します");
//			return;
//		}
//#endif
		stateList_.emplace(T::ID(), std::make_unique<T>(this));
	}

private:
	ICharacterState* FindState(const uint32_t stateId)
	{
		auto it = stateList_.find(stateId);
		if (it != stateList_.end())
		{
			return it->second.get();
		}
		K2_ASSERT(false, "指定された状態がありません");
		return nullptr;
	}
	bool IsContainsState(const uint32_t stateId) const
	{
		auto it = stateList_.find(stateId);
		return it != stateList_.end();
	}





	/** 移動関連 */
public:
	void SetMoveDirection(const Vector3& direction) { moveDirection_ = direction; }
	const Vector3& GetMoveDirection() const { return moveDirection_; }

	/** 入力周り */
public:
	void SetActionA(const bool isAction) { isActionA_ = isAction; }
	bool IsActionA() const { return isActionA_; }
	void SetActionB(const bool isAction) { isActionB_ = isAction; }
	bool IsActionB() const { return isActionB_; }
};




class CharacterStateMachine : public IStateMachine
{
public:
	CharacterStateMachine();
	virtual ~CharacterStateMachine();

	virtual void Initialize() override {}
	virtual void Update() override;

	void Move(const float deltaTime, const float moveSpeed);
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

private:
	void UpdateState();
};