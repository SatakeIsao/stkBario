/**
 * Actorファイル
 */
#pragma once


class Character : public IGameObject
{
	/** 例外としてpublic */
public:
	Transform transform;


protected:
	std::unique_ptr<ModelRender> modelRender_ = nullptr;
	

public:
	Character() {}
	virtual ~Character() {}

	bool Start() override { return true; }
	void Update() override;
	void Render(RenderContext& rc) override;
};