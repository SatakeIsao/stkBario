/**
 * CharacterSteeringファイル
 */
#include "stdafx.h"
#include "CharacterSteering.h"
#include "Actor.h"
#include "BattleCharacter.h"
#include "EventCharacter.h"


namespace
{
	template <typename T>
	void SetMoveDirection(T* target, const Vector3& direction)
	{
		// 未対応
	}
}


void CharacterSteering::Initialize(Character* target, const uint8_t index)
{
	target_ = target;
	padIndex_ = index;
}


void CharacterSteering::Update()
{
	auto inputVector = Vector3(GetPad()->GetLStickXF(), 0.0f, GetPad()->GetLStickYF());
	inputVector.Normalize();

	const bool isPressA = GetPad()->IsPress(enButtonA);

	// BattleCharacter
	{
		BattleCharacter* battleCharacter = dynamic_cast<BattleCharacter*>(target_);
		if (battleCharacter != nullptr) {
			auto* targetCharacterStateMachine = battleCharacter->GetStateMachine();
			targetCharacterStateMachine->SetMoveDirection(inputVector);
			targetCharacterStateMachine->SetActionA(isPressA);
		}
	}
}