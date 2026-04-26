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
		class ManualMenu;
		class AwardMenu;
	}
}

enum class TitleSceneState {
	TitleMenu,  // タイトルメニュー操作中
	ManualMenu,  // マニュアルメニュー操作中
	AwardMenu	//称号メニュー操作中
};

/** タイトルシーン */
class TitleScene : public IScene
{
	appScene(TitleScene);


private:
	/** 遷移をリクエストする先のシーンID */
	uint32_t m_requestSceneId = INVALID_SCENE_ID;

	SpriteRender backGroundRender_;
	app::ui::TitleMenu* titleMenu_ = nullptr;
	app::ui::ManualMenu* manualMenu_ = nullptr;
	app::ui::AwardMenu* awardMenu_ = nullptr;

	TitleSceneState state_ = TitleSceneState::TitleMenu;

	float bButtonHoldTime_ = 0.0f;

public:
	TitleScene();
	virtual ~TitleScene();

	virtual bool Start() override;
	virtual void Update() override;
	virtual void Render(RenderContext& rc) override;

	virtual bool RequestScene(uint32_t& id, float& waitTime)  override;
};