/**
 * Actorファイル
 */
#include "stdafx.h"
#include "Gimmick.h"
#include "gimmick/WarpSystem.h"
#include "core/ParameterManager.h"


namespace app
{
	namespace actor
	{
		IGimmick::IGimmick()
		{}


		IGimmick::~IGimmick()
		{}

		void IGimmick::Update()
		{
			modelRender_->SetTRS(transform.position, transform.rotation, transform.scale);
			modelRender_->Update();
		}


		void IGimmick::Render(RenderContext& rc)
		{
			modelRender_->Draw(rc);
		}




		/************************************/


		StaticGimmick::StaticGimmick()
		{}


		StaticGimmick::~StaticGimmick()
		{}


		bool StaticGimmick::Start()
		{
			return true;
		}


		void StaticGimmick::Update()
		{
			IGimmick::Update();
		}


		void StaticGimmick::Render(RenderContext& rc)
		{
			IGimmick::Render(rc);
		}


		void StaticGimmick::Initialize(const char* path)
		{
			// モデル読み込み
			modelRender_ = std::make_unique<ModelRender>();
			modelRender_->Init(path);
			modelRender_->SetTRS(transform.position, transform.rotation, transform.scale);
			modelRender_->Update();
			// 物理オブジェクト作成
			physicalBody_.reset(new app::collision::PhysicalBody());

			app::collision::Bounds bounds;
			bounds.Compute(modelRender_->GetModel());
			physicalBody_->CreateBox(bounds.maxPoint - bounds.minPoint, transform.position, app::collision::CollisionAttribute::Ground);
		}




		/************************************/


		PipeGimmick::PipeGimmick()
		{}


		PipeGimmick::~PipeGimmick()
		{
			if (endpointId_ >= 0) {
				app::gimmick::WarpSystem::Get().UnregisterPipe(endpointId_);
			}
		}


		bool PipeGimmick::Start()
		{
			IGimmick::Start();

			// システムへの登録
			if (endpointId_ >= 0) {
				app::gimmick::WarpSystem::Get().RegisterPipe(endpointId_, this);
				// TODO: 簡易実装としてここでリンクも登録してしまう（本来はLevelLoaderなどがやるべき）
				if (targetEndpointId_ >= 0) {
					app::gimmick::WarpSystem::Get().AddLink(endpointId_, targetEndpointId_);
				}
			}

			return true;
		}


		void PipeGimmick::Update()
		{
			IGimmick::Update();
		}


		void PipeGimmick::Render(RenderContext& rc)
		{
			IGimmick::Render(rc);
		}


		void PipeGimmick::Initialize(const char* path, int32_t myId, int32_t targetId, const Vector3& forward)
		{
			// ID設定
			endpointId_ = myId;
			targetEndpointId_ = targetId;

			// 方向設定
			forward_ = forward;

			// モデル読み込み
			modelRender_ = std::make_unique<ModelRender>();
			modelRender_->Init(path, nullptr, 0, enModelUpAxisZ, true, false);
			modelRender_->SetTRS(transform.position, transform.rotation, transform.scale);
			modelRender_->Update();

			// バウンディングボックス計算
			boudingVolume_.Compute(modelRender_->GetModel());
			// サイズ計算
			Vector3 size = boudingVolume_.maxPoint - boudingVolume_.minPoint;
			Vector3 halfSize = size / 2.0f;

			// 物理オブジェクト作成
			physicalBody_.reset(new app::collision::PhysicalBody());
			physicalBody_->CreateBox(size, transform.position, app::collision::CollisionAttribute::Ground);

			// ゴーストボディ作成
			ghostBody_.reset(new app::collision::GhostBody());
			ghostBody_->CreateBox(this, ID(), halfSize, app::collision::ghost::CollisionAttribute::Pipe, app::collision::ghost::CollisionAttributeMask::Pipe);
			ghostBody_->SetPosition(transform.position);
		}


		Vector3 PipeGimmick::GetMouthPosition() const
		{
			return Vector3(transform.position.x, boudingVolume_.maxPoint.y, transform.position.z);
		}


		const Quaternion& PipeGimmick::GetRotation() const
		{
			return transform.rotation;
		}


		Vector3 PipeGimmick::GetForward() const
		{
			return forward_;
		}



		/******************************************/


		CoinGimmick::CoinGimmick()
		{}


		CoinGimmick::~CoinGimmick()
		{}


		bool CoinGimmick::Start()
		{
			IGimmick::Start();

			// システムへの登録
			if (endpointId_ >= 0) {
				//app::gimmick::WarpSystem::Get().RegisterPipe(endpointId_, this);
				// TODO: 簡易実装としてここでリンクも登録してしまう（本来はLevelLoaderなどがやるべき）
				if (targetEndpointId_ >= 0) {
					//app::gimmick::WarpSystem::Get().AddLink(endpointId_, targetEndpointId_);
				}
			}

			return true;
		}


		void CoinGimmick::Update()
		{
			if (isPause_) { return; }
			auto* parameter = app::core::ParameterManager::Get().GetParameter<app::core::MasterStageParameter>();

			const float deltaTime = g_gameTime->GetFrameDeltaTime();

			transform.position += velocity_ * deltaTime;

			if (isDead_)
			{
				// 縮小処理
				actionScale_ -= parameter->coinShrinkSpeed * deltaTime;
				if (actionScale_ <= 0.0f) {
					actionScale_ = 0.0f;
					Dead();
				}
				transform.scale = Vector3(actionScale_, actionScale_, actionScale_);
			}

			// 高速で回転
			Quaternion rot;
			rot.SetRotationDegY(deltaTime * parameter->coinRotationSpeed);
			transform.rotation *= rot;

			// ゴーストボディの座標を毎フレーム同期
			if (ghostBody_)
			{
				ghostBody_->SetPosition(transform.position);
			}

			IGimmick::Update();
		}


		void CoinGimmick::Render(RenderContext& rc)
		{
			IGimmick::Render(rc);
		}


		void CoinGimmick::Initialize(const char* path, int32_t myId, const Vector3& forward)
		{
			auto* parameter = app::core::ParameterManager::Get().GetParameter<app::core::MasterStageParameter>();
			// ID設定
			endpointId_ = myId;

			// 方向設定
			forward_ = forward;

			// モデル読み込み
			modelRender_ = std::make_unique<ModelRender>();
			modelRender_->Init(path, nullptr, 0, enModelUpAxisZ, true, false);
			modelRender_->SetTRS(transform.position, transform.rotation, transform.scale);
			modelRender_->Update();

			// バウンディングボックス計算
			boudingVolume_.Compute(modelRender_->GetModel());
			// サイズ計算
			Vector3 size = boudingVolume_.maxPoint - boudingVolume_.minPoint;
			Vector3 halfSize = size / 2.0f;

			// ゴーストボディ作成
			ghostBody_.reset(new app::collision::GhostBody());
			ghostBody_->CreateCapsule(this, ID(), parameter->coinCollisionRadius, parameter->coinCollisionHeight, app::collision::ghost::CollisionAttribute::Coin, app::collision::ghost::CollisionAttributeMask::Coin);
			ghostBody_->SetPosition(transform.position);
		}

		void CoinGimmick::DeadAction()
		{
			auto* parameter = app::core::ParameterManager::Get().GetParameter<app::core::MasterStageParameter>();
			// 上に跳ねあがるための初速を設定
			velocity_.y += parameter->coinJumpInitVelocityY;
			actionScale_ = 1.0f;
			isDead_ = true;
		}


		const Vector3& CoinGimmick::GetPosition() const
		{
			return Vector3(transform.position.x, transform.position.y, transform.position.z);
		}


		const Quaternion& CoinGimmick::GetRotation() const
		{
			return transform.rotation;
		}
	}
}