/**
 * Actorファイル
 */
#include "stdafx.h"
#include "Gimmick.h"
#include "collision/BoundingVolume.h"


namespace app
{
	namespace actor
	{
		IGimmick::IGimmick()
		{
		}


		IGimmick::~IGimmick()
		{
		}

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
		{
		}


		StaticGimmick::~StaticGimmick()
		{
		}


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
			//physicalBody_->CreateFromModel(modelRender_->GetModel(), Matrix::Identity, app::collision::CollisionAttribute::Ground);

			app::collision::Bounds bounds;
			bounds.Compute(modelRender_->GetModel());
			physicalBody_->CreateBox(bounds.maxPoint - bounds.minPoint, transform.position, app::collision::CollisionAttribute::Ground);
		}




		/************************************/


		PipeGimmick::PipeGimmick()
		{
		}


		PipeGimmick::~PipeGimmick()
		{
		}


		bool PipeGimmick::Start()
		{
			IGimmick::Start();
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


		void PipeGimmick::Initialize(const char* path)
		{
			// モデル読み込み
			modelRender_ = std::make_unique<ModelRender>();
			modelRender_->Init(path);
			modelRender_->SetTRS(transform.position, transform.rotation, transform.scale);
			modelRender_->Update();

			// バウンディングボックス計算
			app::collision::Bounds bounds;
			bounds.Compute(modelRender_->GetModel());
			// サイズ計算
			Vector3 size = bounds.maxPoint - bounds.minPoint;
			Vector3 halfSize = size / 2.0f;

			// 物理オブジェクト作成
			physicalBody_.reset(new app::collision::PhysicalBody());
			physicalBody_->CreateBox(size, transform.position, app::collision::CollisionAttribute::Ground);

			// ゴーストボディ作成
			ghostBody_.reset(new app::collision::GhostBody());
			ghostBody_->CreateBox(this, ID(), halfSize, app::collision::ghost::CollisionAttribute::Pipe, app::collision::ghost::CollisionAttributeMask::Pipe);
			ghostBody_->SetPosition(transform.position);
		}
	}
}
