#pragma once
#include "SpringCamera.h"

namespace nsPlayer { class Player; }
class Player;
class GameCamera : public IGameObject
{
public:
	GameCamera();
	~GameCamera();
	bool Start();
	void Update();

	nsPlayer::Player*		m_player = nullptr;						//プレイヤー
	Vector3					m_toCameraPos;							//注視点から視点に向かうベクトル
	Vector3					m_offsetPos = { 0.0f, 70.0f, -130.0f };
	Vector3					m_fixedCameraPos;	//パス移動中のカメラの固定位置
	Vector3					m_fixedCameraTarget;	//パス移動中のカメラの固定注視点
	Vector3 m_fixedCameraOffset;   // パス移動時のカメラの相対位置
	Quaternion				XRot;
	Quaternion				YRot;
	SpringCamera			m_springCamera;							//ばねカメラ
	float					m_stickPowerX = 0.0f;
	float					m_stickPowerY = 0.0f;
	bool					m_isCameraFixed = false;	//パス移動中にカメラを固定するフラグ
};

