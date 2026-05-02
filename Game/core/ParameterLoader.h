/**
 * ParameterLoader.h
 */
#pragma once
#include "core/ParameterManager.h"
#include "core/BinaryParameterLoader.h"

namespace app
{
    namespace core
    {
        class ParameterLoader
        {
        public:
            static void LoadAll();
            static void UnloadAll();
        };
    }
}