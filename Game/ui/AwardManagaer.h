#pragma once

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
    enTimeStopper         // ときをとめる
};

namespace app
{
	namespace ui
	{
		class AwardManagaer
		{
        private:
            // 称号ごとの取得状況を管理するマップ (Get/Set用)
            std::map<AwardType, bool> unlockedAwards;

            // ゲーム中にカウント・保持しておくべきデータ
            int jumpCount = 0;
            int defeatedSlimes = 0;
            int collectedCoins = 0;

        public:
            // --- フラグの Get / Set ---
            void SetAward(AwardType type) {
                unlockedAwards[type] = true;
            }

            bool GetAward(AwardType type) {
                return unlockedAwards[type]; // trueなら取得済み
            }


            // プレイヤーがジャンプした時にPlayerクラスから呼ぶ
            void AddJumpCount() {
                jumpCount++;
                // 規定回数に達したらその場でフラグを立てる
                if (jumpCount == 5)  SetAward(AwardType::enJumpingFrog);
                if (jumpCount == 10) SetAward(AwardType::enBouncingRabbit);
            }

            // オプション画面で音量をいじった時にUIManagerから呼ぶ
            void OnSoundAdjusted() {
                SetAward(AwardType::enSoundPlay);
            }

            // --- リザルト移行時にBattleManagerから呼んで最終判定する関数 ---
            void CheckResultAwards(int playerHp, float clearTime, int maxSlimes, int maxCoins) {
                // HP判定
                if (playerHp == 1) {
                    SetAward(AwardType::enLifeIsPrecious);
                }

                // コイン判定
                if (collectedCoins == maxCoins) {
                    SetAward(AwardType::enCoinMaster);
                }
                else if (collectedCoins == 0) {
                    SetAward(AwardType::enForgetful);
                }

                // タイム判定
                if (clearTime <= 30.0f) {
                    SetAward(AwardType::enSpeedStar);
                }
                else if (clearTime >= 90.0f && clearTime <= 120.0f) {
                    SetAward(AwardType::enRelaxedPerson);
                }
            }

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
                    instance_ = new AwardManagaer();
                }
            }


            /**
             * インスタンスを取得
             */
            static AwardManagaer& Get()
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
            static AwardManagaer* instance_;
		};
	}
}

