/**
 * BattleManager.cpp
 * バトル管理
 */
#include "stdafx.h"
#include "BattleManager.h"

#include "actor/BattleCharacter.h"
#include "actor/EventCharacter.h"
#include "actor/ActorState.h"
#include "actor/CharacterSteering.h"
#include "actor/ActorStatus.h"

#include "core/ParameterManager.h"


namespace
{
	constexpr const char* MASTER_BATTLE_PARAM_PATH = "Assets/master/battle/MasterBattleParameter.json";
	constexpr const char* MASTER_STAGE_PARAM_PATH = "Assets/master/battle/MasterStageParameter.json";
	constexpr const char* MASTER_BATTLE_CHARACTER_PARAM_PATH = "Assets/master/battle/MasterBattleCharacterParameter.json";
}


namespace app
{
	namespace battle
	{
		BattleManager* BattleManager::m_instance = nullptr; //初期化


		BattleManager::BattleManager()
		{
		}


		BattleManager::~BattleManager()
		{
			// パラメーター解放
			app::core::ParameterManager::Get().UnloadParameter<app::core::MasterBattleParameter>();
			app::core::ParameterManager::Get().UnloadParameter<app::core::MasterStageParameter>();
			app::core::ParameterManager::Get().UnloadParameter<app::core::MasterBattleCharacterParameter>();
		}


		void BattleManager::Start()
		{
			LoadParameter();

			{
				characterSteering = new CharacterSteering();
				// マリオにしてみた
				{
					battleCharacter = NewGO<BattleCharacter>(static_cast<uint8_t>(ObjectPriority::Default), "mario");
					battleCharacter->Initialize("Assets/ModelData/player/player.tkm");
					{
						battleCharacter->AddState<IdleCharacterState>();
						battleCharacter->AddState<RunCharacterState>();
						battleCharacter->AddState<JumpCharacterState>();
					}
					// TODO: 摩擦設定
					// ステージによって変えたいので、ステージクラスが作られたら委嘱する
					{
						auto parameter = app::core::ParameterManager::Get().GetParameter<app::core::MasterStageParameter>();
						battleCharacter->GetStatus()->SetFriction(parameter->friction);
					}
				}
				characterSteering->Initialize(battleCharacter, 0);

				// 敵キャラクター
				eventCharacter = NewGO<EventCharacter>(static_cast<uint8_t>(ObjectPriority::Default), "nokonoko");
				eventCharacter->Initialize("Assets/ModelData/enemy/slime/slime.tkm");
			}
		}


		void BattleManager::Update()
		{
			characterSteering->Update();
		}


		void BattleManager::LoadParameter()
		{
			// バトル共通パラメーター読み込み
			app::core::ParameterManager::Get().LoadParameter<app::core::MasterBattleParameter>(MASTER_BATTLE_PARAM_PATH, [](const nlohmann::json& json, app::core::MasterBattleParameter& p)
				{
					p.battleTime = json["battleTime"].get<float>();
				});
			// ステージ共通パラメーター読み込み
			app::core::ParameterManager::Get().LoadParameter<app::core::MasterStageParameter>(MASTER_STAGE_PARAM_PATH, [](const nlohmann::json& json, app::core::MasterStageParameter& p)
				{
					p.gravity = json["gravity"].get<float>();
					p.fallLimitY = json["fallLimitY"].get<float>();
					p.friction = json["friction"].get<float>();
				});
			// バトルキャラクターパラメーター読み込み
			app::core::ParameterManager::Get().LoadParameter<app::core::MasterBattleCharacterParameter>(MASTER_BATTLE_CHARACTER_PARAM_PATH, [](const nlohmann::json& json, app::core::MasterBattleCharacterParameter& p)
				{
					p.moveSpeed = json["moveSpeed"].get<float>();
					p.jumpPower = json["jumpPower"].get<float>();
				});
		}
	}
}