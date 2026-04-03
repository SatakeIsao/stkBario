#include "stdafx.h"
#include "AwardManager.h"

namespace app {
	namespace ui {
		AwardManager* AwardManager::instance_ = nullptr; //初期化

		AwardManager::AwardManager()
		{
		}

		AwardManager::~AwardManager()
		{
		}
		void AwardManager::CheckResultAwards(int playerHp, float clearTime, int maxSlimes, int maxCoins, int collectedCoins)
		{
            // --- HP関連 ---
            if (playerHp == 1) SetAward(AwardType::enLifeIsPrecious);
            if (playerHp >= 7) SetAward(AwardType::enLifeMax); // 例：最大HPなら

            // --- コイン関連 ---
            if (collectedCoins >= maxCoins) SetAward(AwardType::enCoinMaster);
            else if (collectedCoins == 0) SetAward(AwardType::enForgetful);

            // --- タイム関連 ---
            if (clearTime <= 30.0f) SetAward(AwardType::enSpeedStar);
            else if (clearTime >= 100.0f) SetAward(AwardType::enRelaxedPerson);

            // --- 敵討伐関連 ---
            if (defeatedSlimes_ >= maxSlimes) SetAward(AwardType::enSlimeKiller);
            else if (defeatedSlimes_ == 0) SetAward(AwardType::enGentleWorld);

            // --- アクション関連 ---
            if (jumpCount_ >= 5) SetAward(AwardType::enJumpingFrog);
            if (jumpCount_ >= 10) SetAward(AwardType::enBouncingRabbit);
            if (usedDokan_) SetAward(AwardType::enBeardedMan);
            if (usedPauseMenu_) SetAward(AwardType::enTimeStopper);

            // --- コンプリート判定 ---
            // enComplete以外のすべてがtrueかチェックする
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

            // 1. 解放されている称号をリストアップする
            for (int i = 0; i <= static_cast<int>(AwardType::enLifeMax); ++i)
            {
                AwardType type = static_cast<AwardType>(i);
                auto it = unlockedAwards_.find(type);
                if (it != unlockedAwards_.end() && it->second == true)
                {
                    unlockedList.push_back(type);
                }
            }

            // 2. もし1つも称号を持っていなければ false を返して終了！
            if (unlockedList.empty())
            {
                return false;
            }

            // 3. リストの中からランダムに1つ選ぶ
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dist(0, unlockedList.size() - 1);

            outAward = unlockedList[dist(gen)];
            return true;
        }
	}
}
