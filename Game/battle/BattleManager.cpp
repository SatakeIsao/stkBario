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
#include "actor/Types.h"
#include "actor/Gimmick.h"

#include "core/ParameterManager.h"


namespace
{
	constexpr const char* MASTER_BATTLE_PARAM_PATH = "Assets/master/battle/MasterBattleParameter.json";
	constexpr const char* MASTER_STAGE_PARAM_PATH = "Assets/master/battle/MasterStageParameter.json";
	constexpr const char* MASTER_BATTLE_CHARACTER_PARAM_PATH = "Assets/master/battle/MasterBattleCharacterParameter.json";

	// Player用
	static CharacterInitializeParameter sPlayerInitializeParameter = CharacterInitializeParameter([](CharacterInitializeParameter* parameter)
		{
			parameter->modelName = "Assets/ModelData/player/player.tkm";
			parameter->animationDataList.Create(static_cast<uint8_t>(app::actor::PlayerAnimationKind::Max));

			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::Idle)].filename = "Assets/animData/player/playerIdle.tka";
			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::Idle)].loop = true;

			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::Run)].filename = "Assets/animData/player/playerRun.tka";
			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::Run)].loop = true;

			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::Jump)].filename = "Assets/animData/player/playerJump.tka";
			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::Jump)].loop = false;

			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::Punch)].filename = "Assets/animData/player/playerPunch.tka";
			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::Punch)].loop = false;
		});
	// Enemy用
	static CharacterInitializeParameter sEnemyInitializeParameter = CharacterInitializeParameter([](CharacterInitializeParameter* parameter)
		{
			parameter->modelName = "Assets/ModelData/enemy/slime/slime.tkm";
		});

}


namespace app
{
	namespace battle
	{
		BattleManager* BattleManager::instance_ = nullptr; //初期化


		BattleManager::BattleManager()
		{
		}


		BattleManager::~BattleManager()
		{
			DeleteGO(battleCharacter_);
			DeleteGO(eventCharacter_);

			// パラメーター解放
			app::core::ParameterManager::Get().UnloadParameter<app::core::MasterBattleParameter>();
			app::core::ParameterManager::Get().UnloadParameter<app::core::MasterStageParameter>();
			app::core::ParameterManager::Get().UnloadParameter<app::core::MasterBattleCharacterParameter>();
		}


		void BattleManager::Start()
		{
			LoadParameter();

			{
				characterSteering_ = std::make_unique<CharacterSteering>();
				// マリオにしてみた
				{
					battleCharacter_ = NewGO<BattleCharacter>(static_cast<uint8_t>(ObjectPriority::Default), "mario");
					battleCharacter_->Initialize(sPlayerInitializeParameter);
					{
						battleCharacter_->AddState<IdleCharacterState>();
						battleCharacter_->AddState<RunCharacterState>();
						battleCharacter_->AddState<JumpCharacterState>();
						battleCharacter_->AddState<PunchCharacterState>();
					}
					// TODO: ステージによって変えたいので、ステージクラスが作られたら委嘱する
					{
						auto parameter = app::core::ParameterManager::Get().GetParameter<app::core::MasterStageParameter>();
						// 摩擦設定
						battleCharacter_->GetStatus()->SetFriction(parameter->friction);
						// 重力設定
						battleCharacter_->GetStatus()->SetGravity(parameter->gravity);
					}
				}
				characterSteering_->Initialize(battleCharacter_, 0);

				// 敵キャラクター
				eventCharacter_ = NewGO<EventCharacter>(static_cast<uint8_t>(ObjectPriority::Default), "nokonoko");
				eventCharacter_->Initialize(sEnemyInitializeParameter);

				// ギミック設置（テスト用）
				{
					const int gimmickNum = 100;
					const int gimmickRowNum = 10;
					const int gimmickColNum = 10;
					testGimmickList_.resize(gimmickNum);

					for (int i = 0; i < testGimmickList_.size(); ++i) {
						testGimmickList_[i] = NewGO<StaticGimmick>(static_cast<uint8_t>(ObjectPriority::Default), "testGimmick");
						// 配置
						int row = i / gimmickColNum;
						int col = i % gimmickColNum;
						float x = (static_cast<float>(col) - (gimmickColNum / 2.0f)) * 50.0f;
						float z = (static_cast<float>(row) - (gimmickRowNum / 2.0f)) * 50.0f;
						testGimmickList_[i]->transform.position = Vector3(x, -50.0f, z);
						testGimmickList_[i]->transform.scale = Vector3(1.0f, 1.0f, 1.0f);
						testGimmickList_[i]->Initialize("Assets/ModelData/stage/GroundGreenBlock.tkm");
					}
				}
			}
		}


		void BattleManager::Update()
		{
			characterSteering_->Update();
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
					p.jumpMoveSpeed = json["jumpMoveSpeed"].get<float>();
					p.jumpPower = json["jumpPower"].get<float>();
					p.radius = json["radius"].get<float>();
					p.height = json["height"].get<float>();
				});
		}
	}
}