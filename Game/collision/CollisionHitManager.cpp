#include "stdafx.h"
#include "CollisionHitManager.h"


namespace app
{
	namespace collision
	{
		CollisionHitManager* CollisionHitManager::instance_ = nullptr;


		CollisionHitManager::CollisionHitManager()
		{

		}


		CollisionHitManager::~CollisionHitManager()
		{

		}


		void CollisionHitManager::Update()
		{

			hitPairList_.clear();
		}


		void CollisionHitManager::RegisterHitPair(app::collision::GhostBody* a, app::collision::GhostBody* b)
		{
			// ƒqƒbƒgƒyƒA“o˜^
			hitPairList_.push_back(std::move(Pair(a, b)));
		}
	}
}