#include "stdafx.h"
#include "GhostBody.h"
#include "GhostBodyManager.h"


namespace app
{
	namespace collision
	{
		GhostBody::GhostBody()
		{
		}


		GhostBody::~GhostBody()
		{
			if (GhostBodyManager::IsAvailable()) {
				GhostBodyManager::Get().RemoveBody(this);
			}
		}


		void GhostBody::CreateSphere(IGameObject* owner, const float radius, const uint32_t attr, const uint32_t mask)
		{
			m_owner = owner;
			m_shape = std::make_unique<GhostSphere>(radius);
			m_attribute = attr; m_mask = mask;
			RebuildBulletObject();
			GhostBodyManager::Get().AddBody(this);
		}


		void GhostBody::CreateBox(IGameObject* owner, const Vector3& half, const uint32_t attr, const uint32_t mask)
		{
			m_owner = owner;
			m_shape = std::make_unique<GhostBox>(half);
			m_attribute = attr; m_mask = mask;
			RebuildBulletObject();
			GhostBodyManager::Get().AddBody(this);
		}


		void GhostBody::CreateCapsule(IGameObject* owner, const float r, const float h, const uint32_t attr, const uint32_t mask)
		{
			m_owner = owner;
			m_shape = std::make_unique<GhostCapsule>(r, h);
			m_attribute = attr; m_mask = mask;
			RebuildBulletObject();
			GhostBodyManager::Get().AddBody(this);
		}


		void GhostBody::SetPosition(const Vector3& pos)
		{
			if (!m_position.IsEqual(pos)) {
				m_position = pos;
				m_isDirty = true;
				// BulletObjectも同期
				if (m_bulletObject) {
					m_bulletObject->setWorldTransform(GetBtTransform());
				}
			}
		}


		void GhostBody::SetRotation(const Quaternion& rot)
		{
			if (!m_rotation.IsEqual(rot)) {
				m_rotation = rot;
				m_isDirty = true;
				if (m_bulletObject) {
					m_bulletObject->setWorldTransform(GetBtTransform());
				}
			}
		}


		void GhostBody::ComputeAabb(btVector3& min, btVector3& max) const
		{
			if (m_shape) {
				m_shape->GetAabb(GetBtTransform(), min, max);
			}
		}


		btTransform GhostBody::GetBtTransform() const
		{
			btTransform t;
			t.setIdentity();
			t.setOrigin(btVector3(m_position.x, m_position.y, m_position.z));
			t.setRotation(btQuaternion(m_rotation.x, m_rotation.y, m_rotation.z, m_rotation.w));
			return t;
		}


		void GhostBody::RebuildBulletObject()
		{
			m_bulletShape.reset(m_shape->CreateBulletShape());
			m_bulletObject = std::make_unique<btGhostObject>();
			m_bulletObject->setCollisionShape(m_bulletShape.get());
			m_bulletObject->setWorldTransform(GetBtTransform());
			// Bullet側でCollisionのフラグが必要ならここで設定
			// m_bulletObject->setCollisionFlags(m_bulletObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		}
	}
}