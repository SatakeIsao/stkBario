/**
 * TitleScene.h
 * タイトルシーン
 */
#pragma once
#include "IScene.h"
namespace app
{
	namespace ui
	{
		class TitleMenu;
	}
}

/** タイトルシーン */
class TitleScene : public IScene
{
	appScene(TitleScene);


private:
	/** 遷移をリクエストする先のシーンID */
	uint32_t m_requestSceneId = INVALID_SCENE_ID;

	SpriteRender backGroundRender_;
	app::ui::TitleMenu* titleMenu_ = nullptr;


public:
	TitleScene();
	virtual ~TitleScene();

	virtual bool Start() override;
	virtual void Update() override;
	virtual void Render(RenderContext& rc) override;

	virtual bool RequestScene(uint32_t& id, float& waitTime)  override;
};