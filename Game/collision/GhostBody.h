/**
 * GhostBody.h
 * ゴースト用の衝突体本体
 */
#pragma once
#include "GhostPrimitive.h"


class btCollisionObject;
class btCollisionShape;


namespace app
{
	namespace collision
	{
		class GhostBodyManager;


		/**
		 * ゴースト用衝突体本体
		 */
		class GhostBody : public Noncopyable
		{
			friend class GhostBodyManager;
		
		
		private:
			IGameObject* m_owner = nullptr;
			std::unique_ptr<IGhostShape> m_shape;

			/** Broadphase用ハンドル */
			void* m_broadphaseHandle = nullptr;
			bool m_isDirty = true;
			bool m_isActive = true;

			/** 属性 */
			uint32_t m_attribute = ghost::CollisionAttribute::None;
			uint32_t m_mask = ghost::CollisionAttributeMask::All;

			/** 座標 */
			Vector3 m_position = Vector3::Zero;
			Quaternion m_rotation = Quaternion::Identity;

			/** 詳細判定用BulletObject */
			std::unique_ptr<btGhostObject> m_bulletObject;
			std::unique_ptr<btCollisionShape> m_bulletShape;


		public:
			GhostBody();
			~GhostBody();

			/** 初期化 */
			void CreateSphere(IGameObject* owner, const float radius, const uint32_t attr, const uint32_t mask);
			void CreateBox(IGameObject* owner, const Vector3& half, const uint32_t attr, const uint32_t mask);
			void CreateCapsule(IGameObject* owner, const float r, const float h, const uint32_t attr, const uint32_t mask);

			
			void SetPosition(const Vector3& pos);
			void SetRotation(const Quaternion& rot);


			const Vector3& GetPosition() const { return m_position; }
			const Quaternion& GetRotation() const { return m_rotation; }
			IGameObject* GetOwner() const { return m_owner; }
			uint32_t GetAttribute() const { return m_attribute; }
			uint32_t GetMask() const { return m_mask; }
			bool IsActive() const { return m_isActive; }
			void SetActive(bool active) { m_isActive = active; }

			GhostShapeType GetShapeType() const { return m_shape ? m_shape->GetType() : GhostShapeType::Sphere; }
			float GetBoundingRadius() const { return m_shape ? m_shape->GetBoundingRadius() : 0.0f; }

			
			/** Broadphase連携用 */
			void SetBroadphaseHandle(void* handle) { m_broadphaseHandle = handle; }
			void* GetBroadphaseHandle() const { return m_broadphaseHandle; }
			void ComputeAabb(btVector3& min, btVector3& max) const;

			bool IsDirty() const { return m_isDirty; }
			void ClearDirty() { m_isDirty = false; }

			/** 詳細判定用 */
			btCollisionObject* GetBulletObject() { return m_bulletObject.get(); }

		private:
			void RebuildBulletObject();
			btTransform GetBtTransform() const;
		};
	}
}