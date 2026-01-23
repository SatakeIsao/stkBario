/**
 * Actorファイル
 */
#pragma once
#include "collision/PhysicalBody.h"


class IGimmick : public IGameObject
{
	/** 例外としてpublic */
public:
	Transform transform;


protected:
	std::unique_ptr<ModelRender> modelRender_ = nullptr;
	std::unique_ptr<app::collision::PhysicalBody> physicalBody_ = nullptr;

public:
	IGimmick();
	virtual ~IGimmick();

	bool Start() override { return true; }
	void Update() override;
	void Render(RenderContext& rc) override;

	virtual void Initialize(const char* path) = 0;

	ModelRender* GetModelRender() { return modelRender_.get(); }
	app::collision::PhysicalBody* GetPhysicalBody() { return physicalBody_.get(); }
};




/******************************************/


class StaticGimmick : public IGimmick
{
public:
	StaticGimmick();
	virtual ~StaticGimmick();

	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;

	virtual void Initialize(const char* path) override;
};