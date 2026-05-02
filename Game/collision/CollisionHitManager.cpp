#include "stdafx.h"
#include "CollisionHitManager.h"
#include "actor/ActorState.h"
#include "actor/BattleCharacter.h"
#include "actor/EventCharacter.h"
#include "actor/Gimmick.h"
#include "battle/BattleManager.h"
#include "gimmick/WarpSystem.h"
#include "util/ParallelFor.h"

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
			/** DEBUG: 衝突描画設定など（現在はコメントアウト） */
			//PhysicsWorld::Get().EnableDrawDebugWireFrame();
		}

		CollisionHitManager::~CollisionHitManager()
		{

		}

		void CollisionHitManager::Update()
		{
			// 1フレーム内でのヒットペアの数を出力
			char countBuf[256];
			sprintf_s(countBuf, "--- Hit Pair Count: %zu ---\n", hitPairList_.size());
			OutputDebugStringA(countBuf);

			// GhostBodyのヒット情報を各ペアごとに処理
			{
				app::memory::StackAllocatorMarker marker;
				app::memory::StackVector<Pair*>  pipeHitPairList(marker);
				app::memory::StackVector<Pair*>  eventCharacterPairList(marker);
				app::memory::StackVector<Pair*>  coinHitPairList(marker);

				for (auto& hitPair : hitPairList_) {
					char idBuf[256];
					sprintf_s(idBuf, "Collision! A_ID: %u, B_ID: %u\n", hitPair.a->GetOwnerId(), hitPair.b->GetOwnerId());
					OutputDebugStringA(idBuf);

					// 土管のペア
					if (ContainsPipeGimmickPair(hitPair)) {
						pipeHitPairList.push_back(&hitPair);
					}
					// イベントキャラクターのペア
					if (ContainsEventCharacterPair(hitPair)) {
						eventCharacterPairList.push_back(&hitPair);
					}
					// コインのペア
					if (ContainsCoinGimmickPair(hitPair)) {
						coinHitPairList.push_back(&hitPair);
					}
				}

				for (auto* pair : pipeHitPairList) {
					UpdatePipeGimmickPair(*pair);
				}
				for (auto* pair : eventCharacterPairList) {
					UpdateEventCharacterPair(*pair);
				}
				for (auto* pair : coinHitPairList) {
					UpdateCoinGimmickPair(*pair);
				}
			}
			hitPairList_.clear();
		}

		void CollisionHitManager::RegisterHitPair(app::collision::GhostBody* a, app::collision::GhostBody* b)
		{
			// ヒットペアの登録
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

			// 下入力されている場合、ワープ処理
			if (battleCharacter->GetStateMachine()->IsActionDown()) {
				const Vector3 startPosition = pipeGimmick->GetMouthPosition();
				const Vector3 endPosition = app::gimmick::WarpSystem::Get().FindPipe(targetEndpointId)->GetMouthPosition();
				battleCharacter->GetStateMachine()->SetWarpPosition(startPosition, endPosition);
			}
		}

		bool CollisionHitManager::ContainsEventCharacterPair(const Pair& hitPair)
		{
			if (!IsHitObject<app::actor::EventCharacter>(hitPair)) {
				return false;
			}
			if (!IsHitObject<app::actor::BattleCharacter>(hitPair)) {
				return false;
			}
			return true;
		}

		void CollisionHitManager::UpdateEventCharacterPair(Pair& hitPair)
		{
			auto* battleCharacter = GetHitObject<app::actor::BattleCharacter>(hitPair);
			auto* eventCharacter = GetHitObject <app::actor::EventCharacter>(hitPair);

			Vector3 playerPos = battleCharacter->transform.position;
			Vector3 slimePos = eventCharacter->transform.position;

			// パンチ判定等のチェック
			app::collision::GhostBody* colliedPlayerBody = nullptr;
			if (hitPair.a->GetOwnerId() == app::actor::BattleCharacter::ID())
			{
				colliedPlayerBody = hitPair.a;
			}
			else if (hitPair.b->GetOwnerId() == app::actor::BattleCharacter::ID())
			{
				colliedPlayerBody = hitPair.b;
			}

			// パンチなどの特殊部位（本体以外）の衝突判定
			if (colliedPlayerBody != nullptr
				&& colliedPlayerBody != battleCharacter->GetGhostBody())
			{
				if (battleCharacter->GetStateMachine()->GetCurrentStateID() == app::actor::PunchCharacterState::ID())
				{
					// プレイヤーからスライムへのベクトルを計算
					Vector3 knockBackDirection = slimePos - playerPos;
					knockBackDirection.y = 0.0f;
					knockBackDirection.Normalize();
					// スライムをノックバックさせる
					eventCharacter->GetStateMachine()->OnKnockBack(knockBackDirection);
				}
			}
			/** プレイヤー本体のゴーストボディと衝突した場合 */
			else
			{
				/** スライムからプレイヤーに向けたベクトル */
				Vector3 toPlayer = playerPos - slimePos;
				toPlayer.Normalize();
				float dot = toPlayer.y;

				// プレイヤーが上にいるかどうか
				bool isAbove = (dot > 0.1f);

				if (isAbove)
				{
					// 踏みつけ：スライム死亡
					eventCharacter->GetStateMachine()->OnSquashed();
				}
				else
				{
					// 接触：プレイヤーがノックバック
					battleCharacter->GetStateMachine()->OnKnockBack();
				}
			}
		}

		bool CollisionHitManager::ContainsCoinGimmickPair(const Pair& hitPair)
		{
			if (!IsHitObject<app::actor::CoinGimmick>(hitPair)) {
				return false;
			}
			if (!IsHitObject<app::actor::BattleCharacter>(hitPair)) {
				return false;
			}
			return true;
		}

		void CollisionHitManager::UpdateCoinGimmickPair(Pair& hitPair)
		{
			auto* battleCharacter = GetHitObject<app::actor::BattleCharacter>(hitPair);
			auto* coinCharacter = GetHitObject<app::actor::CoinGimmick>(hitPair);

			if (battleCharacter == nullptr || coinCharacter == nullptr) { return; }

			// コインはプレイヤーの本体・パンチ問わず触れたら取得
			if (!coinCharacter->IsDead())
			{
				coinCharacter->DeadAction();
			}
		}
	}
}