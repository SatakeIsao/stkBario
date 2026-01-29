/**
 * BattleManager.h
 * バトル管理
 */
#pragma once
#include "camera/CameraCommon.h"
#include "camera/CameraSteering.h"


namespace app
{
    namespace actor
    {
        class BattleCharacter;
        class EventCharacter;
        class CharacterSteering;
        class StaticGimmick;
    }
    namespace collision
    {
        class GhostBody;
    }
}


namespace app
{
    namespace battle
    {
        /**
         * バトル管理クラス
         */
        class BattleManager
        {
        public:
            /**
             * 通知処理
			 * NOTE: 単体テストをしやすいように分離している
			 *       通信などで非同期に処理する場合にも対応しやすい
             */
            struct INotify : Noncopyable
            {
				app::collision::GhostBody* a = nullptr;
				app::collision::GhostBody* b = nullptr;
                //
                virtual uint32_t ID() const = 0;
            };


            struct DamageNotify : public INotify
            {
				virtual uint32_t ID() const override { return 1; }
            };




        private:
            // @todo for test
            app::actor::BattleCharacter* battleCharacter_ = nullptr;
            app::actor::EventCharacter* eventCharacter_ = nullptr;
			std::vector<app::actor::StaticGimmick*> testGimmickList_;
            
            std::unique_ptr<app::actor::CharacterSteering> characterSteering_ = nullptr;
			std::unique_ptr<app::camera::CameraSteering> cameraSteering_ = nullptr;
			app::camera::RefCameraController gameCameraController_ = nullptr;
            /** 通知リスト */
			std::vector<std::unique_ptr<INotify>> notifyList_;


        private:
            BattleManager();
            ~BattleManager();


        public:
            /** 初期化 */
            void Start();
            /** 更新処理 */
            void Update();

            void AddNotify(INotify* notify)
            {
                notifyList_.push_back(std::move(std::unique_ptr<INotify>(notify)));
			}

        private:
            void LoadParameter();




            /**
             * シングルトン用
             */
        public:
            /**
             * インスタンスを作る
             */
            static void Initialize()
            {
                if (instance_ == nullptr)
                {
                    instance_ = new BattleManager();
                }
            }


            /**
             * インスタンスを取得
             */
            static BattleManager& Get()
            {
                return *instance_;
            }


            /**
			 * インスタンスが有効か
             */
            static bool IsAvailable()
            {
                return instance_ != nullptr;
			}


            /**
             * インスタンスを破棄
             */
            static void Finalize()
            {
                if (instance_ != nullptr)
                {
                    delete instance_;
                    instance_ = nullptr;
                }
            }

        private:
            /** シングルトンインスタンス */
            static BattleManager* instance_;
        };
    }
}