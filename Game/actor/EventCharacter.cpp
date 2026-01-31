/**
 * EventCharacterファイル
 */
#include "stdafx.h"
#include "EventCharacter.h"


namespace app
{
	namespace actor
	{
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


		void EventCharacter::Initialize(CharacterInitializeParameter& param)
		{
			param.Load();

			modelRender_ = std::make_unique<ModelRender>();
			modelRender_->Init(param.modelName);


			transform.position = Vector3::Zero;
			transform.scale = Vector3::One;
			transform.rotation = Quaternion::Identity;
		}
	}
}