/**
 * BattleManager.h
 * バトル管理
 */
#pragma once
#include "camera/CameraCommon.h"
#include "camera/CameraSteering.h"
#include "ui/Layout.h"
#include "effect/EffectManager.h"

namespace nsK2EngineLow
{
    class SkyCube;
}
namespace app
{
    namespace actor
    {
        class BattleCharacter;
        class EventCharacter;
        class CharacterSteering;
        class StaticGimmick;
        class PipeGimmick;
        class CoinGimmick;
    }
    namespace collision
    {
        class GhostBody;
    }
    namespace core
    {
        class PauseManagerObject;
       // class GameOverManagerObject;
    }
    namespace effect
    {
        class EffectManagerObject;
    }
    namespace ui
    {
        class HPBarObject;
        class CoinUIObject;
        class TimerUIObject;
        class BattleSequence;
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
            LevelRender levelRender_;

            app::actor::BattleCharacter* battleCharacter_ = nullptr;
            //app::actor::EventCharacter* eventCharacter_ = nullptr;
            app::actor::CoinGimmick* coinGimmick_ = nullptr;
			std::vector<app::actor::StaticGimmick*> testGimmickList_;
            std::vector<app::actor::PipeGimmick*> pipeGimmickList_;
            std::vector<app::actor::CoinGimmick*> coinGimmickList_;
            std::vector<app::actor::EventCharacter*> eventCharacterList_;
            
            std::unique_ptr<app::actor::CharacterSteering> characterSteering_ = nullptr;
			std::unique_ptr<app::camera::CameraSteering> cameraSteering_ = nullptr;
			app::camera::RefCameraController gameCameraController_ = nullptr;

            EffectManagerObject* effectManagerObject_ = nullptr;
            app::core::PauseManagerObject* pauseManagerObject_ = nullptr;

            app::ui::HPBarObject* hpBarObject_ = nullptr;
            app::ui::CoinUIObject* coinUIObject_ = nullptr;
            app::ui::TimerUIObject* timerUIObject_ = nullptr;
            app::ui::BattleSequence* battleSequenceObject_ = nullptr;

            nsK2EngineLow::SkyCube* skyCube_ = nullptr;									//スカイキューブのオブジェクト
            /** 通知リスト */
			std::vector<std::unique_ptr<INotify>> notifyList_;

            /** あとで書き換える */
            //Test currentDown = Test::Compleate;
            float countDownTimer_ = 3.0f;
            // 残り時間;
            float remainTime_ = 120.0f;
            // ゴールの初期の高さを記憶する用
            float baseGoalY_ = 0.0f;
            // クールタイムを計るタイマー
            float goalEffectTimer_ = 0.0f;

            // コインの総数
            int totalCoin_ = 0;
            std::unique_ptr<app::ui::Layout> layout_;

            bool hasPlayedPunchEffect_ = false;
            /** 死亡したか */
            bool isPlayerDead_ = false;
            bool isPause_ = false;
            bool isGameOverAnimFinished_ = false;
            bool isGameClearAnimFinished_ = false;
            bool hasStartedGameOverUI_ = false;
            bool isTimeUp_ = false;
            bool isBlinking_ = false;
            bool isSeparator = false;
            bool isInputEnabled_ = false;

            Vector3 goalPosition_ = Vector3::Zero;
            Quaternion goalRotation_ = Quaternion::Identity;
            bool hasGoal_ = false;

        private:
            BattleManager();
            ~BattleManager();


        public:
            /** 初期化 */
            void Start();
            /** 更新処理 */
            void Update();

            int GetTotalCoin() const
            {
                return totalCoin_;
            }

            int GetRemainTime() const
            {
                return remainTime_;
            }


            void AddNotify(INotify* notify)
            {
                notifyList_.push_back(std::move(std::unique_ptr<INotify>(notify)));
			}

            bool IsPlayerDead()
            {
                return isPlayerDead_;
            }

            bool IsGameOverAnimFinished() const
            {
                return isGameOverAnimFinished_;
            }
            void SetGameOverAnimFinished(bool isGameOverAnimFinished)
            {
                isGameOverAnimFinished_ = isGameOverAnimFinished;
            }

            bool IsGameClearAnimFinished() const
            {
                return isGameClearAnimFinished_;
            }
            void SetGameClearAnimFinished(bool isGameClearAnimFinished)
            {
                isGameClearAnimFinished_ = isGameClearAnimFinished;
            }

            /** タイムアップ */
            bool IsTimeUpAnimFinished() const
            {
                return isTimeUp_;
            }
            void SetTimeUpAnimFinished(bool isTimeUp)
            {
                isTimeUp_ = isTimeUp;
            }

            bool IsBlinking() const
            {
                return isBlinking_;
            }

            bool IsSeparator() const
            {
                return isSeparator;
            }

            /** 入力受付可能か（バトルシーケンス終了後のみtrue） */
            bool IsInputEnabled() const 
            {
                return isInputEnabled_; 
            }

            void SetPause(bool isPause);

            /** プレイヤーの座標を取得する（スライムの索敵用） */
            Vector3 GetPlayerPosition() const;

            /** プレイヤーのHPを取得する */
            int GetPlayerHP() const;

            /** エフェクトマネージャーを取得する（各キャラが自分でエフェクトを出すため） */
            EffectManagerObject* GetEffectManager() const {
                return effectManagerObject_;
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


        //class IPauseMenu : Noncopyable
        //{
        //public:
        //    IPauseMenu() {}
        //    virtual ~IPauseMenu() {}
        //
        //    virtual bool Start() = 0;
        //    virtual void Update() = 0;
        //    virtual void Render(RenderContext& rc) = 0;
        //    virtual void CanChange(int& request) = 0;
        //};
        //
        //
        //
        ///** ポーズメニュー表示 */
        //class BattlePauseMenu : IPauseMenu
        //{
        //public:
        //    enum EnPauseMenuType
        //    {
        //        enPauseMenuType_ReGame,
        //        enPauseMenuType_Volume,
        //
        //    };
        //};
    }
}