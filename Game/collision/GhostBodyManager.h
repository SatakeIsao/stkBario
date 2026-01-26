/**
 * GhostBodyManager.h
 * ゴーストボディ管理関連
 */
#pragma once
#include "GhostBody.h"
#include "BroadphaseInterface.h"


namespace app
{
	namespace collision
	{
		/**
		 * ゴーストボディ管理クラス
		 */
		class GhostBodyManager
		{
		private:
			/** Broadphaseアルゴリズム（抽象化） */
			std::unique_ptr<IBroadphase> broadphase_;

			/** 管理リスト（Dirtyチェック用） */
			std::vector<GhostBody*> bodyList_;


		private:
			GhostBodyManager();
			~GhostBodyManager();

		public:
			void Update();

			void AddBody(GhostBody* body);
			void RemoveBody(GhostBody* body);

		private:
			/** 衝突ペアの処理 */
			void ProcessCollisionPair(GhostBody* a, GhostBody* b);

			/** Bulletを使った詳細判定 */
			bool CheckCollisionBullet(GhostBody* a, GhostBody* b);



			/**
			 * シングルトン関連
			 */
		private:
			static GhostBodyManager* m_instance;


		public:
			static void Initialize() { if (!m_instance) m_instance = new GhostBodyManager(); }
			static void Finalize() { if (m_instance) { delete m_instance; m_instance = nullptr; } }
			static GhostBodyManager& Get() { return *m_instance; }
			static bool IsAvailable() { return m_instance != nullptr; }
		};
	}
}