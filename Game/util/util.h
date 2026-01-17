/**
 * Utilファイル
 */
#pragma once


/** 必要な要素だけ */
class Transform
{
public:
	Vector3 position = Vector3::Zero;
	Vector3 scale = Vector3::One;
	Quaternion rotation = Quaternion::Identity;
};
