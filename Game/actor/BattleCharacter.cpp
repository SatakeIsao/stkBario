/**
 * BattleCharacterファイル
 */
#include "stdafx.h"
#include "BattleCharacter.h"
#include "ActorStatus.h"


BattleCharacter::BattleCharacter()
{
	stateMachine_ = std::make_unique<BattleCharacterStateMachine>();
	status_ = std::make_unique<app::actor::BattleCharacterStatus>();
}


BattleCharacter::~BattleCharacter()
{
}


bool BattleCharacter::Start()
{
	stateMachine_->Initialize();
	status_->Setup();
	return true;
}


void BattleCharacter::Update()
{
	stateMachine_->Update();

	transform = stateMachine_->transform;

	SuperClass::Update();
}


void BattleCharacter::Render(RenderContext& rc)
{
	SuperClass::Render(rc);
}


void BattleCharacter::Initialize(const char* modelName)
{
	modelRender_ = std::make_unique<ModelRender>();
	modelRender_->Init(modelName);
	// TODO: アニメーションを後で設定しよう
	// ルイージだしてー


	transform.position = Vector3::Zero;
	transform.scale = Vector3::One;
	transform.rotation = Quaternion::Identity;
}