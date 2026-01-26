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
			JumpAscend,		// 上昇
			JumpDescend,	// 落下
			JumpLand,		// 着地
			Punch,
			Max
		};
	}
}