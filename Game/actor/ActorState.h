/**
 * Actorファイル
 */
#pragma once


/**
 * 当初はenumで管理しようと思ったが、
 * ハッシュ値に変更
 * 理由：単純に文字列で検索をかけるより、
 * 数値という名のIDで検索かけた方が早いから。
 */
#define appState(name)\
public:\
	static constexpr uint32_t ID() { return Hash32(#name); }



class IStateMachine;


class ICharacterState : public Noncopyable
{
protected:
	IStateMachine* owner_ = nullptr;


public:
	ICharacterState(IStateMachine* owner) : owner_(owner) {}
	virtual ~ICharacterState() {}

	virtual void Enter() = 0;
	virtual void Update() = 0;
	virtual void Exit() = 0;

	virtual bool CanChangeState() const { return false; }
};




class IdleCharacterState : public ICharacterState 
{
	appState(IdleCharacterState);


public:
	IdleCharacterState(IStateMachine* owner);
	~IdleCharacterState();

	void Enter() override;
	void Update() override;
	void Exit() override;
};




class RunCharacterState : public ICharacterState
{
	appState(RunCharacterState);


public:
	RunCharacterState(IStateMachine* owner);
	~RunCharacterState();

	void Enter() override;
	void Update() override;
	void Exit() override;
};




class JumpCharacterState : public ICharacterState
{
	appState(JumpCharacterState);


private:
	enum class JumpPhase
	{
		Ascend,		// 上昇
		Descend,	// 落下
		Land		// 着地
	};


private:
	JumpPhase jumpPhase_ = JumpPhase::Ascend;


public:
	JumpCharacterState(IStateMachine* owner);
	~JumpCharacterState();
	void Enter() override;
	void Update() override;
	void Exit() override;

	virtual bool CanChangeState() const;
};




class PunchCharacterState : public ICharacterState
{
	appState(PunchCharacterState);


public:
	PunchCharacterState(IStateMachine* owner);
	~PunchCharacterState();
	void Enter() override;
	void Update() override;
	void Exit() override;

	virtual bool CanChangeState() const;
};