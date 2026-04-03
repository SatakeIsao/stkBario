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
#include "gimmick/WarpSystem.h"
#include "camera/CameraManager.h"
#include "camera/CameraController.h"
#include "core/ParameterManager.h"
#include "collision/GhostBodyManager.h"
#include "collision/CollisionHitManager.h"
#include "ui/HPBar.h"
#include "ui/BattleSequence.h"
#include "ui/AwardManager.h"
#include "effect/EffectManager.h"
#include "core/PauseManager.h"
#include "core/PauseManagerObject.h"
#include "sound/SoundManager.h"


namespace
{
	constexpr const char* MASTER_BATTLE_PARAM_PATH = "Assets/master/battle/MasterBattleParameter.json";
	constexpr const char* MASTER_STAGE_PARAM_PATH = "Assets/master/battle/MasterStageParameter.json";
	constexpr const char* MASTER_BATTLE_CAMERA_PARAM_PATH = "Assets/master/battle/MasterBattleCameraParameter.json";
	constexpr const char* MASTER_BATTLE_CHARACTER_PARAM_PATH = "Assets/master/battle/MasterBattleCharacterParameter.json";
	constexpr const char* MASTER_EVENT_CHARACTER_PARAM_PATH = "Assets/master/battle/MasterEventCharacterParameter.json";

	static const int MAX_HP = 8;

	// Player用
	static app::actor::CharacterInitializeParameter sPlayerInitializeParameter = app::actor::CharacterInitializeParameter([](app::actor::CharacterInitializeParameter* parameter)
		{
			parameter->modelName = "Assets/ModelData/player/player.tkm";
			parameter->animationDataList.Create(static_cast<uint8_t>(app::actor::PlayerAnimationKind::Max));

			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::Idle)].filename = "Assets/animData/player/playerIdle.tka";
			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::Idle)].loop = true;

			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::Run)].filename = "Assets/animData/player/playerRun.tka";
			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::Run)].loop = true;

			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::JumpAscend)].filename = "Assets/animData/player/PlayerJump_Start.tka";
			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::JumpAscend)].loop = false;

			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::JumpFalling)].filename = "Assets/animData/player/PlayerJump_Loop.tka";
			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::JumpFalling)].loop = false;

			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::JumpLand)].filename = "Assets/animData/player/PlayerJump_End.tka";
			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::JumpLand)].loop = false;

			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::Punch)].filename = "Assets/animData/player/playerPunch.tka";
			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::Punch)].loop = false;

			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::KnockBack)].filename = "Assets/animData/player/playerKnockBack.tka";
			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::KnockBack)].loop = false;

			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::Dead)].filename = "Assets/animData/player/playerDead.tka";
			parameter->animationDataList[static_cast<uint8_t>(app::actor::PlayerAnimationKind::Dead)].loop = false;
		});
	// Enemy用
	static app::actor::CharacterInitializeParameter sEnemyInitializeParameter = app::actor::CharacterInitializeParameter([](app::actor::CharacterInitializeParameter* parameter)
		{
			parameter->modelName = "Assets/ModelData/enemy/slime/slime.tkm";
			parameter->animationDataList.Create(static_cast<uint8_t>(app::actor::SlimeAnimationKind::Max));

			parameter->animationDataList[static_cast<uint8_t>(app::actor::SlimeAnimationKind::Idle)].filename = "Assets/animData/enemy/slime/slime_Idle.tka";
			parameter->animationDataList[static_cast<uint8_t>(app::actor::SlimeAnimationKind::Idle)].loop = true;

			parameter->animationDataList[static_cast<uint8_t>(app::actor::SlimeAnimationKind::Run)].filename = "Assets/animData/enemy/slime/slime_Run.tka";
			parameter->animationDataList[static_cast<uint8_t>(app::actor::SlimeAnimationKind::Run)].loop = true;

			parameter->animationDataList[static_cast<uint8_t>(app::actor::SlimeAnimationKind::Attack)].filename = "Assets/animData/enemy/slime/slime_Attack.tka";
			parameter->animationDataList[static_cast<uint8_t>(app::actor::SlimeAnimationKind::Attack)].loop = false;

			parameter->animationDataList[static_cast<uint8_t>(app::actor::SlimeAnimationKind::Dead)].filename = "Assets/animData/enemy/slime/slime_Dead.tka";
			parameter->animationDataList[static_cast<uint8_t>(app::actor::SlimeAnimationKind::Dead)].loop = false;

			parameter->animationDataList[static_cast<uint8_t>(app::actor::SlimeAnimationKind::knockBack)].filename = "Assets/animData/enemy/slime/slime_KnockBack.tka";
			parameter->animationDataList[static_cast<uint8_t>(app::actor::SlimeAnimationKind::knockBack)].loop = false;

		});

}


namespace app
{
	namespace battle
	{
		BattleManager* BattleManager::instance_ = nullptr; //初期化


		BattleManager::BattleManager()
		{
			app::gimmick::WarpSystem::Initialize();
			app::collision::CollisionHitManager::Initialize();
			app::collision::GhostBodyManager::Get().RegisterCallback([](app::collision::GhostBody* a, app::collision::GhostBody* b)
				{
					// 衝突ペア登録
					app::collision::CollisionHitManager::Get().RegisterHitPair(a, b);
				});

			layout_ = std::make_unique<app::ui::Layout>();
			layout_ ->Initialize<app::ui::MenuBase>("Assets/ui/layout/BattleSequenceMenuLayout.json");
		}


		BattleManager::~BattleManager()
		{
			DeleteGO(skyCube_);
			DeleteGO(battleCharacter_);
			//DeleteGO(eventCharacter_);
			DeleteGO(hpBarObject_);
			DeleteGO(coinUIObject_);
			DeleteGO(timerUIObject_);
			DeleteGO(effectManagerObject_);
			DeleteGO(pauseManagerObject_);
			//DeleteGO(gameOverManagerObject_);
			DeleteGO(battleSequenceObject_);

			for (auto& test : testGimmickList_)
			{
				DeleteGO(test);
			}
			for (auto& pipe : pipeGimmickList_)
			{
				DeleteGO(pipe);
			}
			for (auto& coin : coinGimmickList_)
			{
				DeleteGO(coin);
			}
			for (auto& slime : eventCharacterList_)
			{
				DeleteGO(slime);
			}

			// パラメーター解放
			app::core::ParameterManager::Get().UnloadParameter<app::core::MasterBattleParameter>();
			app::core::ParameterManager::Get().UnloadParameter<app::core::MasterStageParameter>();
			app::core::ParameterManager::Get().UnloadParameter<app::core::MasterBattleCharacterParameter>();
			app::collision::GhostBodyManager::Get().ClearCallback();
			app::collision::CollisionHitManager::Finalize();
			app::gimmick::WarpSystem::Finalize();
		}


		void BattleManager::Start()
		{
			// パラメーター読み込み
			LoadParameter();
			//エフェクトマネージャーオブジェクト
			{
				effectManagerObject_ = NewGO<EffectManagerObject>(static_cast<uint8_t>(ObjectPriority::Default));
			}

			// スカイキューブ
			{
				skyCube_ = NewGO<nsK2EngineLow::SkyCube>(0, "skycube");
				//明るさを設定
				skyCube_->SetLuminance(1.0f);
				skyCube_->SetScale(400.0f);
				skyCube_->SetPosition({ 1000.0f,0.0f,2500.0f });
				//スカイキューブの種類を設定
				skyCube_->SetType((nsK2EngineLow::EnSkyCubeType)enSkyCubeType_Day);
			}
			// ステージ配置
			{
				//レベルの初期化
				levelRender_.Init("Assets/stage/stage2.tkl",
					[&](LevelObjectData_Render& objData) {
					/** プレイヤー */
					if (objData.EqualObjectName(L"Player") == true) {
						battleCharacter_ = NewGO<app::actor::BattleCharacter>(static_cast<uint8_t>(ObjectPriority::Default), "Player");

						Vector3 playerPosition = objData.position;
						playerPosition.y -= 100.0f;
						battleCharacter_->transform.localPosition = playerPosition;
						battleCharacter_->transform.UpdateTransform();
						battleCharacter_->Initialize(sPlayerInitializeParameter);

						battleCharacter_->GetStateMachine()->transform.position = playerPosition;
						battleCharacter_->GetStateMachine()->transform.rotation = objData.rotation;
						// 瞬間移動したことをCharacterControllerに伝える
						battleCharacter_->GetCharacterController()->RequestTeleport();

						{
							battleCharacter_->AddState<app::actor::IdleCharacterState>();
							battleCharacter_->AddState<app::actor::RunCharacterState>();
							battleCharacter_->AddState<app::actor::JumpCharacterState>();
							battleCharacter_->AddState<app::actor::FallingCharacterState>();
							battleCharacter_->AddState<app::actor::PunchCharacterState>();
							battleCharacter_->AddState<app::actor::WarpInCharacterState>();
							battleCharacter_->AddState<app::actor::WarpOutCharacterState>();
							battleCharacter_->AddState<app::actor::KnockBackCharacterState>();
							battleCharacter_->AddState<app::actor::DeadCharacterState>();
						}
						// TODO: ステージによって変えたいので、ステージクラスが作られたら委嘱する
						{
							auto parameter = app::core::ParameterManager::Get().GetParameter<app::core::MasterStageParameter>();
							// 摩擦設定
							battleCharacter_->GetStatus()->SetFriction(parameter->friction);
							// 重力設定
							battleCharacter_->GetStatus()->SetGravity(parameter->gravity);
							// ワープ設定
							battleCharacter_->GetStatus()->SetWarpData(parameter->warpStartScale, parameter->warpEndScale, parameter->warpTime);
						}
						{
							characterSteering_ = std::make_unique<app::actor::CharacterSteering>();
							characterSteering_->Initialize(battleCharacter_, 0);
						}
					}

					/** 緑ブロック */
					if (objData.ForwardMatchName(L"GroundGreenBlock") == true) {
						app::actor::StaticGimmick* stage = NewGO<app::actor::StaticGimmick>(static_cast<uint8_t>(ObjectPriority::Default), "GroundGreenBlock");
						stage->transform.position = (objData.position);
						stage->transform.rotation = (objData.rotation);
						stage->transform.scale = (objData.scale);
						stage->Initialize("Assets/ModelData/stage/GroundGreenBlock.tkm");
						testGimmickList_.push_back(stage);
						
					}
					/** 地面ボックス */
					if (objData.ForwardMatchName(L"GroundBox") == true) {
						app::actor::StaticGimmick* stage = NewGO<app::actor::StaticGimmick>(static_cast<uint8_t>(ObjectPriority::Default), "GroundBox");
						stage->transform.position = (objData.position);
						stage->transform.rotation = (objData.rotation);
						stage->transform.scale = (objData.scale);
						stage->Initialize("Assets/ModelData/stage/GroundBlock.tkm");
						testGimmickList_.push_back(stage);
					}
					/** コイン */
					if (objData.ForwardMatchName(L"CoinGimmick") == true) {
						app::actor::CoinGimmick* coinGimmick = NewGO<app::actor::CoinGimmick>(static_cast<uint8_t>(ObjectPriority::Default), "coinGimmick");
						coinGimmick->transform.localPosition = (objData.position);
						coinGimmick->transform.localPosition.y -= 20.0f;
						coinGimmick->transform.UpdateTransform();
						coinGimmick->Initialize("Assets/ModelData/item/coin/coin.tkm", 0, Vector3::Down);
						coinGimmickList_.push_back(coinGimmick);
					}
					/** スライム */
					if (objData.ForwardMatchName(L"Slime") == true) {
						app::actor::EventCharacter* slime= NewGO<app::actor::EventCharacter>(static_cast<uint8_t>(ObjectPriority::Default), "Slime");
						
						slime->Initialize(sEnemyInitializeParameter);
						slime->transform.localPosition = (objData.position);
						slime->transform.localRotation = (objData.rotation);
						slime->transform.UpdateTransform();

						slime->GetStateMachine()->transform.position = objData.position;
						slime->GetStateMachine()->transform.rotation = objData.rotation;
						// 瞬間移動したことをCharacterControllerに伝える
						slime->GetCharacterController()->RequestTeleport();

						eventCharacterList_.push_back(slime);

						// 敵キャラクター
						{
							slime->AddState <app::actor::IdleCharacterState>();
							slime->AddState<app::actor::RunCharacterState>();
							slime->AddState<app::actor::AttackCharacterState>();
							slime->AddState<app::actor::PunchCharacterState>();
							slime->AddState<app::actor::DeadCharacterState>();
							slime->AddState <app::actor::KnockBackCharacterState>();
						}
						/** 敵に重力付与のテスト */
						{
							auto stageParam = app::core::ParameterManager::Get().GetParameter<app::core::MasterStageParameter>();
							slime->GetStatus()->SetFriction(stageParam->friction);
							slime->GetStatus()->SetGravity(stageParam->gravity);
						}
					}
					/** ゴール */
					if (objData.ForwardMatchName(L"Goal") == true) {
						// エフェクトはここでは再生せず、座標と回転だけを保存する
						goalPosition_ = objData.position;
						goalPosition_.y -= 100.0f;
						goalRotation_ = objData.rotation;
						baseGoalY_ = goalPosition_.y;
						hasGoal_ = true;
					}
					//if (objData.EqualObjectName(L"PipeGimmick001") == true) {
					//	app::actor::PipeGimmick* pipeGimmick = NewGO<app::actor::PipeGimmick>(static_cast<uint8_t>(ObjectPriority::Default), "pipeGimmick");
					//	pipeGimmick->transform.localPosition = objData.position;
					//	pipeGimmick->transform.UpdateTransform();
					//	pipeGimmick->Initialize("Assets/ModelData/clayPipe/ClayPipe.tkm", 0, 1, Vector3::Down);
					//	pipeGimmickList_.push_back(pipeGimmick);
					//}
					//if (objData.EqualObjectName(L"PipeGimmick002") == true) {
					//	app::actor::PipeGimmick* pipeGimmick = NewGO<app::actor::PipeGimmick>(static_cast<uint8_t>(ObjectPriority::Default), "pipeGimmick");
					//	pipeGimmick->transform.localPosition = objData.position;
					//	pipeGimmick->transform.UpdateTransform();
					//	pipeGimmick->Initialize("Assets/ModelData/clayPipe/ClayPipe.tkm", 0, 1, Vector3::Down);
					//	pipeGimmickList_.push_back(pipeGimmick);
					//}

					/** 当たり判定を変更できるようになったら有効 */
					//if (objData.ForwardMatchName(L"SingleBridge") == true) {
					//	app::actor::StaticGimmick* stage = NewGO<app::actor::StaticGimmick>(static_cast<uint8_t>(ObjectPriority::Default), "SingleBridge");
					//	stage->transform.position = (objData.position);
					//	stage->transform.rotation = (objData.rotation);
					//	stage->transform.scale = (objData.scale);
					//	stage->Initialize("Assets/ModelData/stage/SingleBridge.tkm");
					//	testGimmickList_.push_back(stage);
					//}
					//if (objData.ForwardMatchName(L"GreenGroundSlope") == true) {
					//	app::actor::StaticGimmick* stage = NewGO<app::actor::StaticGimmick>(static_cast<uint8_t>(ObjectPriority::Default), "GreenGroundSlope");
					//	stage->transform.position = (objData.position);
					//	stage->transform.rotation = (objData.rotation);
					//	stage->transform.scale = (objData.scale);
					//	stage->Initialize("Assets/ModelData/stage/GroundSlope.tkm");
					//	testGimmickList_.push_back(stage);
					//}
					return false;
				});
			}
			// 土管
			{
				app::actor::PipeGimmick* pipeGimmick = NewGO<app::actor::PipeGimmick>(static_cast<uint8_t>(ObjectPriority::Default),"pipeGimmick");
				pipeGimmick->transform.localPosition = Vector3(-1100.0f, 20.0f, 1000.0f);
				pipeGimmick->transform.UpdateTransform();
				pipeGimmick->Initialize("Assets/ModelData/clayPipe/ClayPipe.tkm", 0, 1, Vector3::Down);
				pipeGimmickList_.push_back(pipeGimmick);
			}
			{
				app::actor::PipeGimmick* pipeGimmick = NewGO<app::actor::PipeGimmick>(static_cast<uint8_t>(ObjectPriority::Default), "pipeGimmick");
				pipeGimmick->transform.localPosition = Vector3(-100.0f, 20.0f, 0.0f);
				pipeGimmick->transform.UpdateTransform();
				pipeGimmick->Initialize("Assets/ModelData/clayPipe/ClayPipe.tkm", 1, 0, Vector3::Down);
				pipeGimmickList_.push_back(pipeGimmick);
			}
			// カメラ初期化
			{
				auto parameter = app::core::ParameterManager::Get().GetParameter<app::core::MasterBattleCameraParameter>();
				cameraSteering_ = std::make_unique<app::camera::CameraSteering>();

				app::camera::CameraSteering::Config initConfig;
				initConfig.distance = parameter->distance;
				initConfig.height = parameter->height;
				initConfig.rotationSpeedX = parameter->rotationX;
				initConfig.rotationSpeedY = parameter->rotationY;
				app::camera::CameraData initData;
				initData.fov = Math::DegToRad(parameter->fov);
				initData.farClip = parameter->farClip;
				cameraSteering_->SetConfig(initConfig);
				cameraSteering_->SetTargetCharacter(battleCharacter_);

				auto gameCamera = std::make_shared<app::camera::GameCamera>();
				gameCamera->SetState(initData);
				gameCameraController_ = gameCamera;
				app::camera::CameraManager::Get().Register(app::camera::GameCamera::ID(), gameCameraController_);
				app::camera::CameraManager::Get().SwitchCamera(gameCameraController_);
			}
			//HPバー
			{
				// HPバー生成
				hpBarObject_ = NewGO<app::ui::HPBarObject>(static_cast<uint8_t>(ObjectPriority::Default));
			}
			//コインUI
			{
				coinUIObject_ = NewGO <app::ui::CoinUIObject>(static_cast<uint32_t>(ObjectPriority::Default));
			}
			//タイマーUI
			{
				timerUIObject_ = NewGO <app::ui::TimerUIObject>(static_cast<uint32_t>(ObjectPriority::Default));
			}
			
			//ポーズマネージャーオブジェクト
			{
				pauseManagerObject_ = NewGO<app::core::PauseManagerObject>(static_cast<uint8_t>(ObjectPriority::Pause));
			}
			//バトルシーケンスマネージャーオブジェクト
			{
				battleSequenceObject_ = NewGO<app::ui::BattleSequence>(static_cast<uint8_t>(ObjectPriority::Default));
			}
			//BGM再生
			{
				app::SoundManager::Get().PlayBGM(static_cast<int>(app::SoundKind::Game));
			}
			{
				if (app::ui::AwardManager::IsAvailable()) {
					app::ui::AwardManager::Get().ResetPlayData();
				}
			}
		}


		void BattleManager::Update()
		{
			/** 現在のメニューポーズ状態 */
			bool currentPause = app::core::PauseManager::Get().IsPause();
			/** シーケンス中か */
			bool isSequence = false;
			if (battleSequenceObject_) {
				isSequence = battleSequenceObject_->IsPlaying();
			}

			// キャラクターたちに適用するポーズ状態（手動ポーズ中、またはシーケンス中ならポーズさせる）
			bool targetPauseState = currentPause || isSequence;

			if (isPause_ != targetPauseState)
			{
				SetPause(targetPauseState);
			}

			// シーケンス中は手動ポーズ（メニュー表示）を禁止する
			app::core::PauseManager::Get().SetCanPause(!isSequence);
			
			if(currentPause)
			{
				return;
			}

			// ポーズ中やシーケンス中は進めないなどの処理
			if (currentPause || isSequence) return;

			if (hasGoal_)
			{
				// 1. エフェクトの発生位置を毎フレーム上に移動させる（例：1秒間に 50.0f 上昇）
				goalPosition_.y += 50.0f * g_gameTime->GetFrameDeltaTime();

				// ある程度の高さまで行ったら元の高さに戻す（例：初期位置から +150.0f まで）
				if (goalPosition_.y > baseGoalY_ + 100.0f)
				{
					goalPosition_.y = baseGoalY_;
				}

				// 2. クールタイムのタイマーを進める
				goalEffectTimer_ -= g_gameTime->GetFrameDeltaTime();

				// 3. タイマーが 0 以下になったらエフェクトを再生する
				if (goalEffectTimer_ <= 0.0f)
				{
					if (effectManagerObject_) {
						effectManagerObject_->PlayEffect(
							enEffectKind_CircleGoal,
							goalPosition_,
							goalRotation_,
							Vector3(3.0f, 3.0f, 3.0f)
						);
					}
					// クールタイムをリセット（例：0.5秒ごとに発生させる）
					goalEffectTimer_ = 1.3f;
				}

				if (battleCharacter_)
				{
					// プレイヤーの座標
					Vector3 playerPos = battleCharacter_->transform.position;

					// 判定用のゴール座標（エフェクトは上に登っていくため、基準の高さ baseGoalY_ を使う）
					Vector3 targetGoalPos = goalPosition_;
					targetGoalPos.y = baseGoalY_;

					// 距離を計算（XZ平面だけの距離でもOKですが、今回は直線距離）
					Vector3 diff = playerPos - targetGoalPos;
					float distance = diff.Length();

					// 距離が一定以内（例：100.0f）ならクリア！
					if (distance < 50.0f)
					{
						if (battleSequenceObject_) {
							battleSequenceObject_->StartGameClear();
						}
					}
				}
			}

			// ① BattleManagerでカウントダウン
			if (remainTime_ > 0.0f) {
				remainTime_ -= g_gameTime->GetFrameDeltaTime();
				// 指定した時間帯の時だけ isSeparator を true
				if ((remainTime_ <= 100.0f && remainTime_ > 97.0f) ||
					(remainTime_ <= 50.0f && remainTime_ > 47.0f))
				{
					isSeparator = true;
				}
				else
				{
					isSeparator = false;
				}

				/** 30秒以下で点滅フラグON */
				if (remainTime_ <= 30.0f && remainTime_ > 0.0f) {
					// まだ点滅フラグがOFF（＝30秒以下になった最初の1フレーム）の時だけBGMを変更する
					if (!isBlinking_) {
						app::SoundManager::Get().StopBGM();
						app::SoundManager::Get().PlayBGM(static_cast<int>(app::SoundKind::HurryUp));

						// ここでフラグをONにするので、次のフレームからはこの中に入らない！
						isBlinking_ = true;
					}
				}
				else {
					isBlinking_ = false; // 31秒以上、または0秒の時はOFF
				}

				if (remainTime_ <= 0.0f) {
					remainTime_ = 0.0f;
					isBlinking_ = false;

					// タイムアップ時の処理（シーケンスへの通知など）
					if (battleSequenceObject_) {
						battleSequenceObject_->StartTimeUp();
					}
				}
			}

			// ② UIには毎フレーム「表示してほしい時間」を渡すだけ
			if (timerUIObject_) {
				timerUIObject_->SetTimer(remainTime_);
			}

			if (!isSequence)
			{
				characterSteering_->Update();

				// 衝突判定更新
				if (app::collision::GhostBodyManager::IsAvailable()) {
					app::collision::GhostBodyManager::Get().Update();
				}
				// 衝突ヒット管理更新
				app::collision::CollisionHitManager::Get().Update();

				// コイン
				{
					// 配置されているすべてのコインに対して判定を行う
					for (auto& coin : coinGimmickList_)
					{
						// コインが CollisionHitManager によって「死んだ」状態にされており、
						// かつ、まだUIの加算エフェクトなどを出していない場合
						if (coin->IsDead()
							&& !coin->HasPlayedEffect())
						{
							coin->SetPlayedEffect(true);
							// ※HasCoinDeadEffectはコインIDごとに管理するか、
							// あるいはコイン側に「演出済みフラグ(hasPlayedEffect_等)」を持たせると安全です。

							// エフェクト再生
							effectManagerObject_->PlayEffect(
								enEffectKind_PlayerKnockBack,
								coin->transform.position + Vector3(0.0f, 10.0f, 0.0f),
								Quaternion::Identity,
								Vector3::One
							);

							// コインを加算してUIに反映
							totalCoin_++;
							coinUIObject_->SetCoinNumber(totalCoin_);
							//coinUIObject_->SetCoinDeadEffect(true); // 複数コイン対応のため工夫が必要かもしれません
							coinUIObject_->GetPlayAnimation();

							app::SoundManager::Get().PlaySE(static_cast<int>(app::SoundKind::Coin));
						}
					}

					///** デバッグテスト */
					//if (g_pad[0]->IsTrigger(enButtonLB1))
					//{
					//	totalCoin_--;
					//	if (totalCoin_ <= 0) totalCoin_ = 0;
					//}
					//if (g_pad[0]->IsTrigger(enButtonRB1))
					//{
					//	totalCoin_++;
					//	coinUIObject_->GetPlayAnimation();
					//	if (totalCoin_ >= 20) totalCoin_ = 20;
					//}
					///*************************/
					//coinUIObject_->SetCoinNumber(totalCoin_);
				}


				//HPバー
				{
					const int AMOUNT_HP = 1;
					bool isKnockBack = battleCharacter_->GetStateMachine()->GetKnockBack();

					if (isKnockBack
						&& battleCharacter_->GetCurrentHP() > 0)
					{
						battleCharacter_->TakeDamage(AMOUNT_HP);
						//HPバーの現在HPの設定
						hpBarObject_->SetCurrentHP(battleCharacter_->GetCurrentHP());
						//エフェクト
						effectManagerObject_->PlayEffect(
							enEffectKind_PlayerKnockBack,
							battleCharacter_->transform.position,
							Quaternion::Identity,
							Vector3::One
						);
					}

					/************************************************************************/

					/** 死亡処理 */
					if (battleCharacter_->GetCurrentHP() <= 0)
					{
						if (!isPlayerDead_)
						{
							battleCharacter_->GetStateMachine()->OnDead();
							isPlayerDead_ = true;
						}
						
						if (isPlayerDead_
							&& !hasStartedGameOverUI_)
						{
							auto* modelRender = battleCharacter_->GetStateMachine()->GetModelRender();

							if (!modelRender->IsPlayingAnimation())
							{
								battleSequenceObject_->StartGameOver();
								hasStartedGameOverUI_ = true;
							}
						}
					}
				}
				// タイマーが0になった時の処理
				if (timerUIObject_->IsTimeUp())
				{
					if (!isTimeUp_)
					{
						// プレイヤーを死亡状態（または専用のタイムアップ待機状態）にする
						battleCharacter_->GetStateMachine()->OnDead();

						// バトルシーケンスにタイムアップを通知する
						if (battleSequenceObject_) {
							battleSequenceObject_->StartTimeUp();
						}
					}
				}

				// 衝突後の処理
				{
					for (auto& notify : notifyList_) {

					}
					notifyList_.clear();
				}
			}

			auto gameCamera = gameCameraController_->As<app::camera::GameCamera>();
			auto cameraData = gameCamera->GetCameraData();
			cameraSteering_->Update(cameraData, g_gameTime->GetFrameDeltaTime());
			gameCamera->SetState(cameraData);

			layout_->Update();
		}


		void BattleManager::SetPause(bool isPause)
		{
			isPause_ = isPause;
			if (battleCharacter_) battleCharacter_->SetPouse(isPause_);
			
			for(auto* slime : eventCharacterList_)
			{
				if (slime) slime->SetPause(isPause_);
			}
			for (auto* coin : coinGimmickList_)
			{
				if (coin) coin->SetPause(isPause_);
			}
		}

		Vector3 BattleManager::GetPlayerPosition() const
		{
			if (battleCharacter_) return battleCharacter_->transform.position;
			return Vector3::Zero;
		}

		int BattleManager::GetPlayerHP() const
		{
			if (battleCharacter_) return battleCharacter_->GetCurrentHP();
                return 0;
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
					p.warpStartScale = json["warpStartScale"].get<float>();
					p.warpEndScale = json["warpEndScale"].get<float>();
					p.warpTime = json["warpTime"].get<float>();
				});
			// バトルカメラパラメーター読み込み
			app::core::ParameterManager::Get().LoadParameter<app::core::MasterBattleCameraParameter>(MASTER_BATTLE_CAMERA_PARAM_PATH, [](const nlohmann::json& json, app::core::MasterBattleCameraParameter& p)
				{
					p.distance = json["distance"].get<float>();
					p.height = json["height"].get<float>();
					p.fov = json["fov"].get<float>();
					p.nearClip = json["nearClip"].get<float>();
					p.farClip = json["farClip"].get<float>();
					p.rotationX = json["rotationX"].get<float>();
					p.rotationY = json["rotationY"].get<float>();
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
			// イベントキャラクターパラメーター読み込み
			app::core::ParameterManager::Get().LoadParameter<app::core::MasterEventCharacterParameter>(MASTER_EVENT_CHARACTER_PARAM_PATH, [](const nlohmann::json& json, app::core::MasterEventCharacterParameter& p)
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