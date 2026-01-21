/**
 * Actorファイル
 */
#pragma once


namespace app
{
	namespace actor
	{
		struct CharacterStatus;
	}
}


/**
 * キャラクターの初期化で渡す情報
 * NOTE モデル表示やアニメーションなど
 */
struct CharacterInitializeParameter
{
	struct AnimationData
	{
		const char* filename;
		bool loop;
	};

	const char* modelName = nullptr;
	app::memory::Array<AnimationData> animationDataList;
	//
	CharacterInitializeParameter(const std::function<void(CharacterInitializeParameter* parameter)>& func)
	{
		func(this);
	}
};


class Character : public IGameObject
{
	/** 例外としてpublic */
public:
	Transform transform;


protected:
	std::unique_ptr<ModelRender> modelRender_ = nullptr;
	std::unique_ptr<CharacterController> characterController_ = nullptr;
	app::memory::Array<AnimationClip> animationClips_;

	app::actor::CharacterStatus* status_ = nullptr;


public:
	Character();
	virtual ~Character();

	bool Start() override { return true; }
	void Update() override;
	void Render(RenderContext& rc) override;

	virtual void Initialize(const CharacterInitializeParameter& param) = 0;

	app::actor::CharacterStatus* GetStatus() { return status_; }
	ModelRender* GetModelRender() { return modelRender_.get(); }
};