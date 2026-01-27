/**
 * BattleCharacterファイル
 */
#include "stdafx.h"
#include "BattleCharacter.h"
#include "ActorStatus.h"


BattleCharacter::BattleCharacter()
{
	characterController_ = std::make_unique<CharacterController>();
	stateMachine_ = std::make_unique<BattleCharacterStateMachine>();
	status_ = new app::actor::BattleCharacterStatus();
	ghostBody_ = std::make_unique<app::collision::GhostBody>();
}


BattleCharacter::~BattleCharacter()
{
}


bool BattleCharacter::Start()
{
	stateMachine_->Initialize();
	stateMachine_->Setup(this);
	status_->Setup();

	characterController_->Init(status_->GetRadius(), status_->GetHeight(), transform.position);
	characterController_->SetGravity(status_->GetGravity());

	return true;
}


void BattleCharacter::Update()
{
	const float deltaTime = g_gameTime->GetFrameDeltaTime();

	stateMachine_->Update();

	auto nextPosition = characterController_->Execute(stateMachine_->transform.position, deltaTime);

	transform.position = nextPosition;
	transform.rotation = stateMachine_->transform.rotation;

	stateMachine_->transform.position = nextPosition;

	ghostBody_->SetPosition(transform.position);

	SuperClass::Update();
}


void BattleCharacter::Render(RenderContext& rc)
{
	SuperClass::Render(rc);
}


void BattleCharacter::Initialize(const CharacterInitializeParameter& param)
{
	const uint32_t animationCount = static_cast<uint32_t>(param.animationDataList.size());
	animationClips_.Create(animationCount);
	for (uint32_t i = 0; i < animationCount; ++i) {
		animationClips_[i].Load(param.animationDataList[i].filename);
		animationClips_[i].SetLoopFlag(param.animationDataList[i].loop);
	}

	modelRender_ = std::make_unique<ModelRender>();
	modelRender_->Init(param.modelName, animationClips_.data(), animationClips_.size());

	transform.position = Vector3::Zero;
	transform.scale = Vector3::One;
	transform.rotation = Quaternion::Identity;

	ghostBody_->CreateCapsule(this, ID(), status_->GetRadius(), status_->GetHeight(), app::collision::ghost::CollisionAttribute::Player, app::collision::ghost::CollisionAttributeMask::All);
}