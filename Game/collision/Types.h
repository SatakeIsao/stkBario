/**
 * Types.h
 * コリジョン関係の定義群
 */
#pragma once
#include "physics/CollisionAttr.h"
#include <cstdint>


namespace app
{
	namespace collision
	{
		enum class CollisionAttribute : uint32_t
		{
			Ground		= 1 << 0,
			Character	= 1 << 1,
			Enemy		= 1 << 2,
		};
	}
}