#pragma once
#include <map>
#include <vector>
#include <random>

// 称号の種類をEnumで定義しておく
enum class AwardType {
    enComplete,           // コンプリート
    enCoinMaster,         // コインマスター
    enForgetful,          // わすれんぼう
    enSlimeKiller,        // スライムキラー
    enGentleWorld,        // やさしいせかい
    enLifeIsPrecious,     // いのちだいじに
    enJumpingFrog,        // ぴょんぴょんがえる
    enBouncingRabbit,     // はねるうさぎ
    enRelaxedPerson,      // のんびりやさん
    enSpeedStar,          // スピードスター
    enSoundPlay,          // おとあそび
    enChallengerHeart,    // チャレンジのこころ
    enTimeStopper,        // ときをとめる
    enBeardedMan,         // ひげ男にあこがれて
    enLifeMax             // げんきいっぱい
};

namespace app
{
	namespace ui
	{
		class AwardManager
		{
        private:
            // 称号ごとの取得状況を管理するマップ (Get/Set用)
            std::map<AwardType, bool> unlockedAwards_;

            // ゲーム中にカウント・保持しておくべきデータ
            int jumpCount_ = 0;
            int defeatedSlimes_ = 0;
            int collectedCoins_ = 0;

            bool usedDokan_ = false;
            bool usedPauseMenu_ = false;

        public:
            AwardManager();
            ~AwardManager();

            // --- フラグの Get / Set ---
            void SetAward(AwardType type) {
                unlockedAwards_[type] = true;
            }

            bool GetAward(AwardType type) {
                return unlockedAwards_[type]; // trueなら取得済み
            }


            // プレイヤーがジャンプした時にPlayerクラスから呼ぶ
            void AddJumpCount();

            // オプション画面で音量をいじった時にUIManagerから呼ぶ
            void OnSoundAdjusted() {
                SetAward(AwardType::enSoundPlay);
            }

            void OnDokan() {
                SetAward(AwardType::enBeardedMan);
            }

            void OnChallengerHeart(){
                SetAward(AwardType::enChallengerHeart);
            }

            void OnTimeStopper() {
                SetAward(AwardType::enTimeStopper);
            }

            void AddDeadedSlimeCount() {
                defeatedSlimes_++;
            }

            // --- リザルト移行時にBattleManagerから呼んで最終判定する関数 ---
            void CheckResultAwards(int playerHp, float clearTime, int maxSlimes, int maxCoins, int collectedCoins);
            

            void ResetPlayData() {
                defeatedSlimes_ = 0;
            }

            // 解放済みの称号の中からランダムに1つ選んで返す関数
            bool GetRandomUnlockedAward(AwardType& outAward) const;

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
                    instance_ = new AwardManager();
                }
            }


            /**
             * インスタンスを取得
             */
            static AwardManager& Get()
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
            static AwardManager* instance_;
		};
	}
}

