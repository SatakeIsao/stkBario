/**
 * EventCharacterファイル
 */
#include "stdafx.h"
#include "EventCharacter.h"


EventCharacter::EventCharacter()
{

}


EventCharacter::~EventCharacter()
{

}


bool EventCharacter::Start()
{
	return true;
}


void EventCharacter::Update()
{
	SuperClass::Update();
}


void EventCharacter::Render(RenderContext& rc)
{
	SuperClass::Render(rc);
}


void EventCharacter::Initialize(const char* modelName)
{
	modelRender_ = std::make_unique<ModelRender>();
	modelRender_->Init(modelName);
	// TODO: アニメーションを後で設定しよう
	// ルイージだしてー


	transform.position = Vector3::Zero;
	transform.scale = Vector3::One;
	transform.rotation = Quaternion::Identity;
}