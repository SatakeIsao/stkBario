#include "stdafx.h"
#include "CollisionHitManager.h"
#include "actor/BattleCharacter.h"
#include "actor/Gimmick.h"
#include "battle/BattleManager.h"
#include "gimmick/WarpSystem.h"


namespace
{
	template <typename T>
	bool IsHitObject(const app::collision::CollisionHitManager::Pair& pair)
	{
		if (pair.a->GetOwnerId() == T::ID()) {
			return true;
		}
		if (pair.b->GetOwnerId() == T::ID()) {
			return true;
		}
		return false;
	}

	template <typename T>
	T* GetHitObject(app::collision::CollisionHitManager::Pair& pair)
	{
		if (pair.a->GetOwnerId() == T::ID()) {
			return static_cast<T*>(pair.a->GetOwner());
		}
		if (pair.b->GetOwnerId() == T::ID()) {
			return static_cast<T*>(pair.b->GetOwner());
		}
		return nullptr;
	}
}


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
			// GhostBodyのヒット処理で得たペアをもとに処理
			{
				app::memory::StackAllocatorMarker marker;
				{
					app::memory::StackVector<Pair*>  pipeHitPairList(marker);

					//std::vector<Pair*> pipeHitPairList;
					for (auto& hitPair : hitPairList_) {
						// 土管のペアか
						if (ContainsPipeGimmickPair(hitPair)) {
							pipeHitPairList.push_back(&hitPair);
						}
					}

					for (auto* pair : pipeHitPairList) {
						UpdatePipeGimmickPair(*pair);
					}
				}
			}
			hitPairList_.clear();
		}


		void CollisionHitManager::RegisterHitPair(app::collision::GhostBody* a, app::collision::GhostBody* b)
		{
			// ヒットペア登録
			hitPairList_.push_back(std::move(Pair(a, b)));
		}


		bool CollisionHitManager::ContainsPipeGimmickPair(const Pair& hitPair)
		{
			if (!IsHitObject<app::actor::PipeGimmick>(hitPair)) {
				return false;
			}
			if (!IsHitObject<app::actor::BattleCharacter>(hitPair)) {
				return false;
			}
			return true;
		}


		void CollisionHitManager::UpdatePipeGimmickPair(Pair& hitPair)
		{
			auto* pipeGimmick = GetHitObject<app::actor::PipeGimmick>(hitPair);
			auto* battleCharacter = GetHitObject<app::actor::BattleCharacter>(hitPair);

			app::gimmick::EndpointId targetEndpointId;
			if (!app::gimmick::WarpSystem::Get().TryResolve(pipeGimmick->GetEndpointId(), targetEndpointId)) {
				return;
			}

			if (battleCharacter->GetStateMachine()->IsActionDown()) {
				const Vector3 targetPosition = app::gimmick::WarpSystem::Get().FindPipe(targetEndpointId)->GetMouthPosition();
				battleCharacter->GetStateMachine()->SetWarpPosition(targetPosition);
			}

			//if (app::battle::BattleManager::IsAvailable()) {
			//	app::battle::BattleManager::DamageNotify* notify = new app::battle::BattleManager::DamageNotify();
			//	notify->a = hitPair.a;
			//	notify->b = hitPair.b;
			//	app::battle::BattleManager::Get().AddNotify(notify);
			//}
		}
	}
}