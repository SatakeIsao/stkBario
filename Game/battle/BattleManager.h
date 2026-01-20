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
            BattleCharacter* battleCharacter = nullptr;
            EventCharacter* eventCharacter = nullptr;
            CharacterSteering* characterSteering = nullptr;


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
                if (m_instance == nullptr)
                {
                    m_instance = new BattleManager();
                }
            }


            /**
             * インスタンスを取得
             */
            static BattleManager& Get()
            {
                return *m_instance;
            }


            /**
			 * インスタンスが有効か
             */
            static bool IsAvailable()
            {
                return m_instance != nullptr;
			}


            /**
             * インスタンスを破棄
             */
            static void Finalize()
            {
                if (m_instance != nullptr)
                {
                    delete m_instance;
                    m_instance = nullptr;
                }
            }

        private:
            /** シングルトンインスタンス */
            static BattleManager* m_instance;
        };
    }
}