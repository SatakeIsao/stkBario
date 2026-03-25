/**
 * Fafde.h
 * 読み込みなどで表示するためFade処理
 */
#pragma once

 // フェードの状態を定義
enum class FadeState {
	SlimeAnim,   // スライムが縮小・暗転するフェーズ
	SlimeScaleUpAnim,
	BIconAnim,
	BIconScaleUpAnim,
	LoadingAnim  // 既存のBロゴが回転するフェーズ
};

/**
 * Fade処理本体
 */
class Fade
{
private:
	SpriteRender m_fadeRender;
	SpriteRender m_iconRender;
	SpriteRender m_slimeRender;
	SpriteRender m_IconBFadeRender;

	bool isEnable = false;
	FadeState m_state = FadeState::SlimeAnim;

	app::ComputeRate m_iconConputeRate;
	app::ComputeRate m_slimeComputeRate;
	app::ComputeRate m_BFadeComputeRate;


private:
	Fade();
	~Fade();


public:
	void Update();
	void Render(RenderContext& rc);

	
public:
	void Enable(FadeState initialState);
	void Disable() { isEnable = false; }

	void StartFadeIn();
	void StartSlimeFadeIn();
	bool IsFadedOut() const
	{
		return m_state == FadeState::LoadingAnim;
	}



	/**
	 * シングルトン用
	 */
private:
	static Fade* m_instance;


public:
	static void Create()
	{
		if (m_instance == nullptr)
		{
			m_instance = new Fade();
		}
	}
	static Fade& Get()
	{
		return *m_instance;
	}
	static void Delete()
	{
		if (m_instance != nullptr)
		{
			delete m_instance;
			m_instance = nullptr;
		}
	}
};




/***************************/


class FadeObject : public IGameObject
{
public:
	FadeObject();
	virtual ~FadeObject();


	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;
};