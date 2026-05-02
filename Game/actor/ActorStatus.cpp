/**
 * Actorファイル
 */
#include "stdafx.h"
#include "ActorStatus.h"
#include "core/ParameterManager.h"


namespace app
{
	namespace actor
	{


		void BattleCharacterStatus::LoadParameter(const char* path)
		{
			// 無し
		}


		void BattleCharacterStatus::Setup()
		{
			auto parameter = app::core::ParameterManager::Get().GetParameter<app::core::MasterBattleCharacterParameter>();

			moveSpeed_ = parameter->moveSpeed;
			jumpMoveSpeed_ = parameter->jumpMoveSpeed;
			jumpPower_ = parameter->jumpPower;
			radius_ = parameter->radius;
			height_ = parameter->height;
			ghostbodyPosYOffset_ = parameter->ghostbodyPosYOffset;
			fallDeathThresholdPosY_ = parameter->ghostbodyPosYOffset;
		}




		/**********************************/
		
		
		void EventCharacterStatus::LoadParameter(const char* path)
		{
			// 無し
		}

		void EventCharacterStatus::Setup()
		{
			auto parameter = app::core::ParameterManager::Get().GetParameter<app::core::MasterEventCharacterParameter>();

			moveSpeed_ = parameter->moveSpeed;
			jumpMoveSpeed_ = parameter->jumpMoveSpeed;
			jumpPower_ = parameter->jumpPower;
			radius_ = parameter->radius;
			height_ = parameter->height;
			ghostbodyPosYOffset_ = parameter->ghostbodyPosYOffset;
			fallDeathThresholdPosY_ = parameter->ghostbodyPosYOffset;
		}
	}
}