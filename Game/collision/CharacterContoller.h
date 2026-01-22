/**
 * CharacterContoller.h
 * 物理的なオブジェクトとの衝突解決を行う処理
 */
#pragma once
#include "physics/CapsuleCollider.h"
#include "physics/RigidBody.h"


namespace app
{
	namespace collision
	{
		class CharacterController : public Noncopyable
		{
		private:
			/** キャラクター用なのでカプセルとしておく */
			CCapsuleCollider collider_;
			/** 物理空間の処理に必要 */
			RigidBody rigidBody_;

			/** 座標 */
			Vector3 position_;
			/** 前回の座標 */
			Vector3 prevPosition_;

			/** 垂直方向の速度 */
			float verticalVelocity_ = 0.0f;
			/** 重力加速度 */
			float gravity_ = 0.0f;
			/** 半径 */
			float radius_ = 0.0f;
			/** 高さ */
			float height_ = 0.0f;

			/** 初期化済みフラグ */
			bool isInited_ = false;
			/** ジャンプ中フラグ */
			bool isJump_ = false;
			/** 地面に接地しているフラグ */
			bool isOnGround_ = true;
			/** テレポートリクエストフラグ */
			bool isRequestTeleport_ = false;


		public:
			CharacterController();
			~CharacterController();

			void Init(float radius, float height, const Vector3& position);

			/*!
			* @brief	実行。
			* @details  テレポートリクエストがある場合は衝突判定を行わずに移動します。
			* @param[in] targetPosition	移動先の座標。
			* @param[in] deltaTime		経過時間。
			*/
			const Vector3& Execute(const Vector3& targetPosition, float deltaTime);

			/*!
			* @brief	テレポートをリクエストする。
			* @details  これを呼んだ次の Execute() で、衝突判定を行わずに targetPosition へ移動します。
			*/
			void RequestTeleport()
			{
				isRequestTeleport_ = true;
			}

			const Vector3& GetPosition() const { return position_; }
			const Vector3& GetPrevPosition() const { return prevPosition_; }
			void SetPosition(const Vector3& position) { position_ = position; }
			bool IsJump() const { return isJump_; }
			bool IsOnGround() const { return isOnGround_; }
			void Jump(float jumpPower);

			CCapsuleCollider* GetCollider() { return &collider_; }
			RigidBody* GetRigidBody() { return &rigidBody_; }
			void RemoveRigidBoby();
		};
	}
}