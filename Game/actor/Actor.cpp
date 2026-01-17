/**
 * Actorファイル
 */
#include "stdafx.h"
#include "Actor.h"


void Character::Update()
{
	modelRender_->SetTRS(transform.position, transform.rotation, transform.scale);
	modelRender_->Update();
}


void Character::Render(RenderContext& rc)
{
	modelRender_->Draw(rc);
}