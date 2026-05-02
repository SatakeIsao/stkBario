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

            /** バトルキャラクター */
            app::actor::BattleCharacter* battleCharacter_ = nullptr;
            /** テスト用静的ギミックのリスト */
			std::vector<app::actor::StaticGimmick*> testGimmickList_;
            /** 土管ギミックのリスト */
            std::vector<app::actor::PipeGimmick*> pipeGimmickList_;
            /** コインギミックのリスト */
            std::vector<app::actor::CoinGimmick*> coinGimmickList_;
            /** イベントキャラクター（スライム）のリスト */
            std::vector<app::actor::EventCharacter*> eventCharacterList_;
            
            /** キャラクターの入力・移動を制御するステアリング */
            std::unique_ptr<app::actor::CharacterSteering> characterSteering_ = nullptr;
            /** カメラの追従・制御を行うステアリング */
			std::unique_ptr<app::camera::CameraSteering> cameraSteering_ = nullptr;
            /** ゲームカメラのコントローラー参照 */
			app::camera::RefCameraController gameCameraController_ = nullptr;

            /** エフェクト管理オブジェクト */
            EffectManagerObject* effectManagerObject_ = nullptr;
            /** ポーズ管理オブジェクト */
            app::core::PauseManagerObject* pauseManagerObject_ = nullptr;

            /** HPバーUI */
            app::ui::HPBarObject* hpBarObject_ = nullptr;
            /** コイン枚数UI */
            app::ui::CoinUIObject* coinUIObject_ = nullptr;
            /** タイマーUI */
            app::ui::TimerUIObject* timerUIObject_ = nullptr;
            /** バトル演出シーケンス */
            app::ui::BattleSequence* battleSequenceObject_ = nullptr;

            /** スカイキューブのオブジェクト */
            nsK2EngineLow::SkyCube* skyCube_ = nullptr;
            /** 通知リスト */
			std::vector<std::unique_ptr<INotify>> notifyList_;
            /** レイアウト */
            std::unique_ptr<app::ui::Layout> layout_;

            /** バトル開始カウントダウンタイマー */
            float countDownTimer_ = 3.0f;
            /** バトルの残り時間（秒）; */
            float remainTime_ = 120.0f;
            /** ゴールエフェクト発生位置の基準Y座標 */ 
            float baseGoalY_ = 0.0f;
            /** ゴールエフェクトの発生クールタイムタイマー */
            float goalEffectTimer_ = 0.0f;

            /** 取得済みコインの総数 */
            int totalCoin_ = 0;

            /** パンチヒットエフェクトを再生済みか */
            bool hasPlayedPunchEffect_ = false;
            /** プレイヤーが死亡したか */
            bool isPlayerDead_ = false;
            /** ポーズ中か */
            bool isPause_ = false;
            /** ゲームオーバーアニメーションが完了したか */
            bool isGameOverAnimFinished_ = false;
            /** ゲームクリアアニメーションが完了したか */
            bool isGameClearAnimFinished_ = false;
            /** ゲームオーバーUIの再生を開始済みか */
            bool hasStartedGameOverUI_ = false;
            /** タイムアップ処理を実行済みか */
            bool isTimeUp_ = false;
            /** 残り時間が点滅閾値を下回っているか */
            bool isBlinking_ = false;
            /** タイマーのセパレーター（区切り演出）表示中か */
            bool isSeparator = false;
            /** プレイヤーの入力を受け付けているか（バトルシーケンス終了後にtrue） */
            bool isInputEnabled_ = false;

            /** ゴールオブジェクトのワールド座標 */
            Vector3 goalPosition_ = Vector3::Zero;
            /** ゴールオブジェクトの回転 */
            Quaternion goalRotation_ = Quaternion::Identity;
            /** ゴールオブジェクトが配置されているか */
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
           // void LoadParameter();




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