#include "stdafx.h"
#include "GameCamera.h"

GameCamera::GameCamera()
{
}

GameCamera::~GameCamera()
{
}

bool GameCamera::Start()
{
	//注視点から視点までのベクトルを設定
	m_toCameraPos.Set(0.0f, 0.0f, 10.0f);
	//m_toCameraPos *= 0.5f;
	
	//カメラのニアクリップとファークリップを設定
	g_camera3D->SetNear(1.0f);
	g_camera3D->SetFar(80000.0f);

	//ばねカメラの初期化
	m_springCamera.Init(
		*g_camera3D,	//ばねカメラの処理を行うカメラを指定する
		1000.0f,		//カメラの移動速度の最大値
		true,			//カメラと地形とのあたり判定を取るかどうかのフラグ。trueだとあたり判定を行う
		5.0f			//カメラに設定される球体コリジョンの半径。第3引数がtrueの時に有効になる
	);
	return true;
}

void GameCamera::Update()
{
	// プレイヤーの位置と回転を取得
	//Vector3 target = m_player->GetPostion(); // プレイヤーの現在位置
	//Quaternion playerRotation = m_player->GetRotation(); //プレイヤーの回転

	//if (m_player->GetIsPathMoving())
	//{
	//	// **パス移動中はカメラの位置をプレイヤーの相対位置に固定するが、追従する**
	//	if (!m_isCameraFixed)
	//	{
	//		// **カメラの相対位置を記録**
	//		m_fixedCameraOffset = g_camera3D->GetPosition() - target;
	//		m_isCameraFixed = true;
	//	}

	//	// **プレイヤーの位置が動いたらカメラもその分動かす**
	//	Vector3 cameraPosition = target + m_fixedCameraOffset;
	//	Vector3 cameraTarget = target + Vector3(0.0f, 70.0f, 0.0f); // 注視点はプレイヤーの少し上

	//	m_springCamera.SetPosition(cameraPosition);
	//	m_springCamera.SetTarget(cameraTarget);
	//}
	//else
	//{
	//	// **通常時はプレイヤーの背後を追従**
	//	m_isCameraFixed = false; // 固定解除

	//	Vector3 offset(m_offsetPos); // 通常時のカメラの位置
	//	playerRotation.Apply(offset); // プレイヤーの回転を適用

	//	Vector3 cameraPosition = target + offset;
	//	Vector3 cameraTarget = target;
	//	cameraTarget.y += 70.0f;

	//	m_springCamera.SetPosition(cameraPosition);
	//	m_springCamera.SetTarget(cameraTarget);
	//}

	m_springCamera.Update();


	//// 右スティックの入力でカメラを左右に回転させる
	//m_stickPowerX = g_pad[0]->GetRStickXF(); // X軸のスティック入力
	//// プレイヤーの周りをY軸回転でカメラを移動
	//Quaternion yRotation;
	//yRotation.SetRotationDeg(Vector3::AxisY, 2.4f * m_stickPowerX); // Y軸周りに回転
	//yRotation.Apply(m_toCameraPos); // カメラのオフセットに回転を適用

	//プレイヤーの向きをカメラが向いている方向にする
	//カメラの正面ベクトルを取得する
	//Vector3 forward = g_camera3D->GetForward();
	//プレイヤーが斜めを向かないようにyをゼロにする
	//forward.y = 0.0f;
	//正規化
	//forward.Normalize();

	////用意した情報でプレイヤーのローテーションを用意する
	//Quaternion playerRotation;
	////正面ベクトル(0,0,1)からカメラの正面ベクトルに向かう回転を作る
	//playerRotation.SetRotation(Vector3::AxisZ, forward);
	////プレイヤーのローテーションに適用する
	//m_player->SetRotation(playerRotation);
}