/**
 * Actorファイル
 */
#pragma once
#include "collision/PhysicalBody.h"
#include "actor/Types.h"


namespace app
{
	namespace actor
	{
		class IGimmick : public IGameObject
		{
			/** 例外としてpublic */
		public:
			app::math::Transform transform;


		protected:
			std::unique_ptr<ModelRender> modelRender_ = nullptr;
			std::unique_ptr<app::collision::PhysicalBody> physicalBody_ = nullptr;


		public:
			IGimmick();
			virtual ~IGimmick();

			bool Start() override { return true; }
			void Update() override;
			void Render(RenderContext& rc) override;

			virtual void Initialize(const char* path) = 0;

			ModelRender* GetModelRender() { return modelRender_.get(); }
			app::collision::PhysicalBody* GetPhysicalBody() { return physicalBody_.get(); }
		};




		/******************************************/


		class StaticGimmick : public IGimmick
		{
			appActor(StaticGimmick);


		public:
			StaticGimmick();
			virtual ~StaticGimmick();

			bool Start() override;
			void Update() override;
			void Render(RenderContext& rc) override;

			virtual void Initialize(const char* path) override;
		};




		/******************************************/


		/** 土管の接続処理 */
		struct PipeGimmickConnection : Noncopyable
		{

		};


		class PipeGimmick : public IGimmick
		{
			appActor(PipeGimmick);


		private:
			std::unique_ptr<app::collision::GhostBody> ghostBody_ = nullptr;
			uint32_t endpointId_ = -1;
			uint32_t targetEndpointId_ = -1;
			app::collision::Bounds boudingVolume_;
			/** 吸い込まれる方向 */
			Vector3 forward_;

		public:
			PipeGimmick();
			virtual ~PipeGimmick();
			bool Start() override;
			void Update() override;
			void Render(RenderContext& rc) override;

			virtual void Initialize(const char* path) override {}
			virtual void Initialize(const char* path, int32_t myId, int32_t targetId, const Vector3& forward);


		public:
			uint32_t GetEndpointId() const { return endpointId_; }

			/** Endpoint情報取得 */
			Vector3 GetMouthPosition() const;
			const Quaternion& GetRotation() const;
			/** 土管の入り口方向（吸い込まれる方向） */
			Vector3 GetForward() const;
		};
	}
}