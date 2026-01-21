/**
 * BattleManager.h
 * バトル管理
 */
#pragma once


class BattleCharacter;
class EventCharacter;
class CharacterSteering;


namespace app
{
    namespace battle
    {
        /**
         * バトル管理クラス
         */
        class BattleManager
        {
        private:
            // @todo for test
            BattleCharacter* battleCharacter_ = nullptr;
            EventCharacter* eventCharacter_ = nullptr;
            std::unique_ptr<CharacterSteering> characterSteering_ = nullptr;


        private:
            BattleManager();
            ~BattleManager();


        public:
            /** 初期化 */
            void Start();
            /** 更新処理 */
            void Update();


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