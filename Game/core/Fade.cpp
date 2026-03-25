#include "stdafx.h"
#include "Fade.h"


namespace
{
	constexpr float ICON_ROTATE_TIME = 2.0f;
	constexpr float SLIME_ANIM_TIME = 1.0f;
	constexpr float B_ANIM_TIME = 1.0f;
}


Fade* Fade::m_instance = nullptr;


Fade::Fade()
{
	m_fadeRender.Init("Assets/ui/fade.dds", 1920.0f, 1080.0f);
	m_fadeRender.SetMulColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));

	m_iconRender.Init("Assets/ui/LoadingIcon.dds", 128.0f, 128.0f);
	m_iconRender.SetPosition(Vector3(700.0f, -400.0f, 0.0f));

	// スライム画像の初期化（画面中央に配置）
	m_slimeRender.Init("Assets/ui/loading/slime_Fade.dds", 1920.0f, 1080.0f);
	m_slimeRender.SetPosition(Vector3(0.0f, 0.0f, 0.0f));

	// Bアイコン画像の初期化（画面中央に配置）
	m_IconBFadeRender.Init("Assets/ui/loading/B_Fade.dds", 1920.0f, 1080.0f);
	m_IconBFadeRender.SetPosition(Vector3(0.0f, 0.0f, 0.0f));

	// タイマーの初期化（ループさせない）
	m_slimeComputeRate.Initialize(SLIME_ANIM_TIME, false);

	m_BFadeComputeRate.Initialize(B_ANIM_TIME, false);

	m_iconConputeRate.Initialize(ICON_ROTATE_TIME, true);
}


Fade::~Fade()
{
}


void Fade::Update()
{
	if (!isEnable) return;

	if (m_state == FadeState::SlimeAnim)
	{
		// ① スライムのアニメーション処理
		const float rate = m_slimeComputeRate.Update();

		// 縮小処理 (1.0倍 から 0.0倍 へ)
		const float scale = Math::Lerp(rate, 10.0f, 1.0f);
		m_slimeRender.SetScale(Vector3(scale, scale, 1.0f));

		// スライムの黒化処理 (元の色 から 黒 へ)
		const float colorVal = Math::Lerp(rate, 1.0f, 0.0f);
		m_slimeRender.SetMulColor(Vector4(colorVal, colorVal, colorVal, 1.0f));

		// ★追加：背景を徐々に暗くする処理（透明 0.0 → 真っ黒 1.0 へ）
		const float bgAlpha = Math::Lerp(rate, 0.0f, 0.3f);
		m_fadeRender.SetMulColor(Vector4(0.0f, 0.0f, 0.0f, bgAlpha));

		// それぞれ更新
		m_fadeRender.Update();
		m_slimeRender.Update();

		// rateが1.0（100%）以上になったら次の状態へ遷移
		if (rate >= 1.0f)
		{
			m_state = FadeState::LoadingAnim;
			// 念のため完全に真っ黒に固定しておく
			m_fadeRender.SetMulColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		}

	}
	else if (m_state == FadeState::SlimeScaleUpAnim)
	{
		// ① Bアイコンのアニメーション処理
		const float rate = m_slimeComputeRate.Update();

		// 縮小処理 (1.0倍 から 0.0倍 へ)
		const float scale = Math::Lerp(rate, 1.0f, 50.0f);
		m_slimeRender.SetScale(Vector3(scale, scale, 1.0f));

		// Bアイコンの黒化処理 (元の色 から 黒 へ)
		const float colorVal = Math::Lerp(rate, 0.0f, 1.0f);
		const float alphaVal = Math::Lerp(rate, 1.0f, 0.0f);

		m_slimeRender.SetMulColor(Vector4(colorVal, colorVal, colorVal, alphaVal));

		// ★追加：背景を徐々に暗くする処理（透明 0.0 → 真っ黒 1.0 へ）
		const float bgAlpha = Math::Lerp(rate, 0.3f, 0.0f);
		m_fadeRender.SetMulColor(Vector4(0.0f, 0.0f, 0.0f, bgAlpha));

		// それぞれ更新
		m_fadeRender.Update();
		m_slimeRender.Update();

		// rateが1.0（100%）以上になったら次の状態へ遷移
		if (rate >= 1.0f)
		{
			isEnable = false;
		}
	}
	else if (m_state == FadeState::BIconAnim)
	{
		// ① Bアイコンのアニメーション処理
		const float rate = m_BFadeComputeRate.Update();

		// 縮小処理 (1.0倍 から 0.0倍 へ)
		const float scale = Math::Lerp(rate, 10.0f, 1.0f);
		m_IconBFadeRender.SetScale(Vector3(scale, scale, 1.0f));

		// Bアイコンの黒化処理 (元の色 から 黒 へ)
		const float colorVal = Math::Lerp(rate, 1.0f, 0.0f);
		m_IconBFadeRender.SetMulColor(Vector4(colorVal, colorVal, colorVal, 1.0f));

		// 背景を徐々に暗くする処理（透明 0.0 → 真っ黒 1.0 へ）
		const float bgAlpha = Math::Lerp(rate, 0.0f, 0.3f);
		m_fadeRender.SetMulColor(Vector4(0.0f, 0.0f, 0.0f, bgAlpha));

		// それぞれ更新
		m_fadeRender.Update();
		m_IconBFadeRender.Update();

		// rateが1.0（100%）以上になったら次の状態へ遷移
		if (rate >= 1.0f)
		{
			m_state = FadeState::LoadingAnim;
			// 念のため完全に真っ黒に固定しておく
			m_fadeRender.SetMulColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		}
	}
	else if (m_state == FadeState::BIconScaleUpAnim)
	{
		// ① Bアイコンのアニメーション処理
		const float rate = m_BFadeComputeRate.Update();

		// 縮小処理 (1.0倍 から 0.0倍 へ)
		const float scale = Math::Lerp(rate, 1.0f, 50.0f);
		m_IconBFadeRender.SetScale(Vector3(scale, scale, 1.0f));

		// Bアイコンの黒化処理 (元の色 から 黒 へ)
		const float colorVal = Math::Lerp(rate, 0.0f, 1.0f);
		const float alphaVal = Math::Lerp(rate, 1.0f, 0.5f);

		m_IconBFadeRender.SetMulColor(Vector4(colorVal, colorVal, colorVal, alphaVal));

		// ★追加：背景を徐々に暗くする処理（透明 0.0 → 真っ黒 1.0 へ）
		const float bgAlpha = Math::Lerp(rate, 0.3f, 0.0f);
		m_fadeRender.SetMulColor(Vector4(0.0f, 0.0f, 0.0f, bgAlpha));

		// それぞれ更新
		m_fadeRender.Update();
		m_IconBFadeRender.Update();

		// rateが1.0（100%）以上になったら次の状態へ遷移
		if (rate >= 1.0f)
		{
			isEnable = false;
		}
	}
	else if (m_state == FadeState::LoadingAnim)
	{
		// 既存の背景更新（真っ黒のまま）
		m_fadeRender.Update();

		// ② 既存のBロゴ処理
		const float rate = m_iconConputeRate.Update();
		const float rotate = Math::Lerp(rate, 0.0f, Math::PI2);
		Quaternion q;
		q.SetRotationZ(rotate);
		m_iconRender.SetRotation(q);

		m_iconRender.Update();
	}
}


void Fade::Render(RenderContext& rc)
{
	if (!isEnable) {
		return;
	}
	m_fadeRender.Draw(rc);
	if (m_state == FadeState::SlimeAnim
		|| m_state == FadeState::SlimeScaleUpAnim) {
		m_slimeRender.Draw(rc);
	}
	else if (m_state == FadeState::BIconAnim 
		|| m_state == FadeState::BIconScaleUpAnim) {
		// BIconAnimIn の時も描画するように || で条件を追加
		m_IconBFadeRender.Draw(rc);
	}
	else if (m_state == FadeState::LoadingAnim) {
		m_iconRender.Draw(rc);
	}
}

void Fade::Enable(FadeState initialState)
{
	isEnable = true;
	m_state = initialState; // 引数で渡された状態をセット

	// 状態に応じたタイマーを初期化する
	if (m_state == FadeState::SlimeAnim) {
		m_slimeComputeRate.Initialize(SLIME_ANIM_TIME, false);
	}
	else if (m_state == FadeState::BIconAnim) {
		m_BFadeComputeRate.Initialize(B_ANIM_TIME, false);
	}

	m_fadeRender.SetMulColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
}

void Fade::StartFadeIn()
{
	// 状態をフェードインに切り替え、タイマーをリセットしてスタート
	m_state = FadeState::BIconScaleUpAnim;
	m_BFadeComputeRate.Initialize(B_ANIM_TIME, false);
	//m_BFadeComputeRate.Start();
}

void Fade::StartSlimeFadeIn()
{
	// 状態をフェードインに切り替え、タイマーをリセットしてスタート
	m_state = FadeState::SlimeScaleUpAnim;
	m_slimeComputeRate.Initialize(SLIME_ANIM_TIME, false);
}




/***************************/


FadeObject::FadeObject()
{
	Fade::Create();
}


FadeObject::~FadeObject()
{
	Fade::Delete();
}


bool FadeObject::Start()
{	
	return true;
}


void FadeObject::Update()
{
	Fade::Get().Update();
}


void FadeObject::Render(RenderContext& rc)
{
	Fade::Get().Render(rc);
}