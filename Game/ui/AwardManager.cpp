#include "stdafx.h"
#include "AwardManager.h"


namespace
{
    /** いのちだいじに */ 
    static constexpr int   LOW_HP_THRESHOLD = 1;
    /** げんきいっぱい */
    static constexpr int   MAX_HP_THRESHOLD = 7;
    /** スピードスター */
    static constexpr float FAST_TIME_LIMIT = 30.0f;
    /** のんびりやさん */
    static constexpr float SLOW_TIME_THRESHOLD = 100.0f;
    /** ぴょんぴょんがえる */
    static constexpr int   JUMP_COUNT_FROG = 5;
    /** はねるうさぎ */
    static constexpr int   JUMP_COUNT_RABBIT = 10;
}


namespace app 
{
	namespace ui 
    {
		AwardManager* AwardManager::instance_ = nullptr; //初期化

		AwardManager::AwardManager()
		{
		}

		AwardManager::~AwardManager()
		{
		}

        void AwardManager::AddJumpCount()
        {
            jumpCount_++;
            // 規定回数に達したらその場でフラグを立てる
            if (jumpCount_ == JUMP_COUNT_FROG)  SetAward(AwardType::enJumpingFrog);
            if (jumpCount_ == JUMP_COUNT_RABBIT) SetAward(AwardType::enBouncingRabbit);
        }

		void AwardManager::CheckResultAwards(int playerHp, float clearTime, int maxSlimes, int maxCoins, int collectedCoins)
		{
            /** HP関連 */
            if (playerHp == LOW_HP_THRESHOLD) SetAward(AwardType::enLifeIsPrecious);
            if (playerHp >= MAX_HP_THRESHOLD) SetAward(AwardType::enLifeMax); // 例：最大HPなら

            /** コイン関連 */
            if (collectedCoins >= maxCoins) SetAward(AwardType::enCoinMaster);
            else if (collectedCoins == 0) SetAward(AwardType::enForgetful);

            /** タイム関連 */
            if (clearTime <= FAST_TIME_LIMIT) SetAward(AwardType::enSpeedStar);
            else if (clearTime >= SLOW_TIME_THRESHOLD) SetAward(AwardType::enRelaxedPerson);

            /** 敵討伐関連 */
            if (defeatedSlimes_ >= maxSlimes) SetAward(AwardType::enSlimeKiller);
            else if (defeatedSlimes_ == 0) SetAward(AwardType::enGentleWorld);

            /** アクション関連 */
            if (jumpCount_ >= JUMP_COUNT_FROG) SetAward(AwardType::enJumpingFrog);
            if (jumpCount_ >= JUMP_COUNT_RABBIT) SetAward(AwardType::enBouncingRabbit);

            if (usedDokan_) SetAward(AwardType::enBeardedMan);
            if (usedPauseMenu_) SetAward(AwardType::enTimeStopper);

            /** コンプリート判定 */
            bool isAllUnlocked = true;
            for (int i = 1; i <= static_cast<int>(AwardType::enLifeMax); ++i) {
                if (!unlockedAwards_[static_cast<AwardType>(i)]) {
                    isAllUnlocked = false;
                    break;
                }
            }
            if (isAllUnlocked) {
                SetAward(AwardType::enComplete);
            }
		}

        bool AwardManager::GetRandomUnlockedAward(AwardType& outAward) const
        {
            std::vector<AwardType> unlockedList;

            /** 解放されている称号をリストアップ */
            for (int i = 0; i <= static_cast<int>(AwardType::enLifeMax); ++i)
            {
                AwardType type = static_cast<AwardType>(i);
                auto it = unlockedAwards_.find(type);
                if (it != unlockedAwards_.end() && it->second == true)
                {
                    unlockedList.push_back(type);
                }
            }

            /** もし1つも称号を持っていなければ false を返して終了 */
            if (unlockedList.empty())
            {
                return false;
            }

            /** リストの中からランダムに1つ選択 */
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dist(0, unlockedList.size() - 1);

            outAward = unlockedList[dist(gen)];
            return true;
        }
	}
}
