#include "k2EngineLowPreCompile.h"
#include "Shadow.h"


void nsK2EngineLow::Shadow::Init()
{
	InitRenderTarget();
	InitLightCamera();
	SpriteInitData spriteInitData;
	spriteInitData.m_textures[0] = &m_shadowMap.GetRenderTargetTexture();
	spriteInitData.m_fxFilePath = "Assets/Shader/sprite.fx";
	spriteInitData.m_width = 256;
	spriteInitData.m_height = 256;
	m_sprite.Init(spriteInitData);
}

void nsK2EngineLow::Shadow::Render(RenderContext& rc, std::vector<IRenderer*>& renderObjects)
{
	Matrix viewMatrix;
	Matrix projectionMatrix;

	// ライト方向を真上寄りにする（Y成分を大きく）
	// → ブロック側面への投影角度が深くなりセルフシャドウが減る
	const Vector3 lightDir = Vector3(0.5f, -2.0f, -0.5f);
	const float lightDistance = 3000.0f;
	const float ORTHO_SIZE = 2000.0f;
	const float SHADOW_MAP_SIZE = 4096.0f;

	// テクセル1つ分のワールド空間サイズ
	const float texelSize = ORTHO_SIZE / SHADOW_MAP_SIZE;

	// ターゲットのXZのみ追従、Yは固定
	Vector3 cameraTarget = g_camera3D->GetTarget();
	Vector3 lightTarget = Vector3(cameraTarget.x, 0.0f, cameraTarget.z);

	// ライトターゲットをテクセルサイズ単位にスナップ
	lightTarget.x = floorf(lightTarget.x / texelSize + 0.5f) * texelSize;
	lightTarget.z = floorf(lightTarget.z / texelSize + 0.5f) * texelSize;

	// lightDir を正規化して逆方向にカメラを置く
	Vector3 lightDirNorm = lightDir;
	lightDirNorm.Normalize();
	Vector3 lightCameraPos = lightTarget + Vector3(-lightDirNorm.x, -lightDirNorm.y, -lightDirNorm.z) * lightDistance;

	viewMatrix.MakeLookAt(
		lightCameraPos,
		lightTarget,
		Vector3::AxisY
	);

	// Far を固定値にして深度バッファの精度を確保
	// g_camera3D->GetFar()（10000）だと near:far = 1:100 になり精度が低下する
	const float SHADOW_NEAR = 100.0f;
	const float SHADOW_FAR = 6000.0f;	// lightDistance(3000) * 2 で十分カバーできる

	projectionMatrix.MakeOrthoProjectionMatrix(
		ORTHO_SIZE,
		ORTHO_SIZE,
		SHADOW_NEAR,
		SHADOW_FAR
	);


	////ビュー行列の算出
	//viewMatrix.MakeLookAt(
	//	m_lightCamera.GetPosition(), 
	//	m_lightCamera.GetTarget(), 
	//	m_lightCamera.GetUp());

	////平行投影行列の計算
	//projectionMatrix.MakeOrthoProjectionMatrix(
	//	m_lightCamera.GetWidth(),
	//	m_lightCamera.GetHeight(),
	//	m_lightCamera.GetNear(),
	//	m_lightCamera.GetFar());

	//ビュープロジェクションの行列
	m_viewProjectionMatrix = viewMatrix * projectionMatrix;


	/*m_lightCamera.SetTarget(g_camera3D->GetTarget());
	m_lightCamera.SetPosition(g_camera3D->GetTarget() + Vector3{ 0.0f, 600.0f, 300.0f });
	m_lightCamera.Update();*/

	rc.WaitUntilToPossibleSetRenderTarget(m_shadowMap);
	rc.SetRenderTargetAndViewport(m_shadowMap);
	rc.ClearRenderTargetView(m_shadowMap);
	for (auto& renderer : renderObjects)
	{
		renderer->OnRenderShadowMap(rc, m_viewProjectionMatrix);//m_lightCamera.GetViewProjectionMatrix());
	}
	rc.WaitUntilFinishDrawingToRenderTarget(m_shadowMap);




}

void nsK2EngineLow::Shadow::InitRenderTarget()
{
	float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_shadowMap.Create(
		4096,
		4096,
		1,
		1,
		//DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_D32_FLOAT,
		clearColor
	);
}

void nsK2EngineLow::Shadow::InitLightCamera()
{
	//m_lightCamera.SetUpdateProjMatrixFunc(Camera::enUpdateProjMatrixFunc_Ortho);
	//m_lightCamera.SetPosition(0, 600, 300);
	//m_lightCamera.SetTarget(0, 0, 0);
	//m_lightCamera.SetUp(1, 0, 0);
	////m_lightCamera.SetViewAngle(Math::DegToRad(20.0f));
	//m_lightCamera.Update();
}

void nsK2EngineLow::Shadow::SpriteShadowDraw(RenderContext& rc)
{
	//sprite.Update({ FRAME_BUFFER_W / -2.0f, FRAME_BUFFER_H / 2.0f,  0.0f }, g_quatIdentity, g_vec3One, { 0.0f, 1.0f });
	//sprite.Draw(rc);
}