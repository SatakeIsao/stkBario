/**
 * 定義関連
 */
#pragma once
#include <cstdint>


namespace app
{
	namespace actor
	{
		enum class PlayerAnimationKind : uint8_t
		{
			Idle,
			Run,
			Jump,
			Max
		};
	}
}