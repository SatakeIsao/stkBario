/**
 * Actorファイル
 */
#include "stdafx.h"
#include "ActorStateMachine.h"
#include "Actor.h"
#include "ActorStatus.h"


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



void IStateMachine::UpdateStateCore()
{
	ICharacterState* currentState = stateList_[currentStateId_].get();
	if(nextStateId_ != INVALID_STATE_ID)
	{
		// ステート変更
		// 現在のステートを抜ける
		currentState->Exit();
		// 次のステートに入る
		currentState = FindState(nextStateId_);
		if(currentState)
		{
			currentState->Enter();
		}
		currentStateId_ = nextStateId_;
		//次のステートを初期化
		nextStateId_ = INVALID_STATE_ID;
	}
	currentState->Update();
}


bool IStateMachine::CanChangeState() const
{
	const ICharacterState* currentState = stateList_.find(currentStateId_)->second.get();
	return currentState->CanChangeState();
}




/************************************/


CharacterStateMachine::CharacterStateMachine()
{
	//初期ステート
	SetCurrentState(RunCharacterState::ID());
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
	// TODO: 将来的にCharacterControllerを使って衝突判定をする
	const Vector3 moveVector = ComputeCameraDirection(moveDirection_) * moveSpeed;
	if(inputPower_)
	{
		moveSpeedVector_ = moveVector;
	}
	moveSpeedVector_ *= GetStatus()->GetFriction(); // 摩擦係数的な
	transform.position += moveSpeedVector_ * deltaTime;
}


void CharacterStateMachine::Jump(const float jumoPower)
{
	character_->GetCharacterController()->Jump(jumoPower);
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
}


void BattleCharacterStateMachine::Update()
{
	UpdateState();

	SuperClass::Update();
}


void BattleCharacterStateMachine::UpdateState()
{
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
			isActionB_ = false;
			return;
		}
		// パンチ中は他の状態に遷移しない
		if (IsEqualCurrentState(PunchCharacterState::ID()))
		{
			if (!CanChangeState()) {
				return;
			}
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