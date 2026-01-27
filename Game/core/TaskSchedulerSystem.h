/**
 * TaskSchedulerSystem.h
 * タスクスケジューラーシステム
 */
#pragma once


namespace app
{
	namespace core
	{
        class TaskSchedulerSystem : public Noncopyable
        {
            using Callback = std::function<void()>;


        private:
            struct Event
            {
                float triggerTime;      // 実行される目標時刻
                Callback callback;
                float interval;         // ループ用の間隔保持
                bool delayNextFrame;    // 実行条件を満たした後、さらに1フレーム待つか
            };


        private:
            float currentTime_ = 0.0f;
            std::vector<Event> events_;
            bool needsSort_ = false;
            // ダブルバッファ構造で「次のフレーム」を確実に保証する
            std::vector<Event> currentFrameEvents_;         // 今のフレームで実行するもの
            std::vector<Event> pendingNextFrameEvents_;     // 次のフレームに回すもの


        public:
            TaskSchedulerSystem() {}
			~TaskSchedulerSystem() {}
            

            /**
             * タイマーをスケジュールする
             * @param delay 実行までの秒数
             * @param callback 実行される処理
             * @param loop ループするか
             */
            void AddTimer(float delay, Callback callback, bool delayNextFrame = false)
            {
                float targetTime = currentTime_ + delay;
                events_.push_back({ targetTime, std::move(callback), delay, delayNextFrame });
                needsSort_ = true;
            }

            /**
             * 毎フレームの更新
             * @param deltaTime フレーム経過時間
             */
            void Update(float deltaTime)
            {
                currentTime_ += deltaTime;

                if (needsSort_) {
                    SortEvents();
                    needsSort_ = false;
                }

                // 実行時刻が来たイベントを処理
                for (auto it = events_.begin(); it != events_.end(); ) {
                    if (currentTime_ >= it->triggerTime) {
                        if (it->delayNextFrame) {
                            // 「次のフレームで実行」リストへ移送する
                            pendingNextFrameEvents_.push_back(std::move(*it));
                        } else {
                            // コールバック実行
                            if (it->callback) {
                                it->callback();
                            }
                        }
                        it = events_.erase(it);
                    } else {
                        // ソート済みなので、現在の時刻より先の話であればこれ以上回さない
                        break;
                    }
                }

                // 「次フレーム実行」タスクの処理
                // 前回のupdateで「実行待ち」になったものをここで動かす
                std::vector<Event> eventsToExecute;
                eventsToExecute.swap(currentFrameEvents_); // 前回分を吸い出す

                for (const auto& task : eventsToExecute) {
                    if (task.callback) task.callback();
                }

                // 今回のupdateで発生した「次フレーム待ち」を次回の実行用へ移動
                currentFrameEvents_.swap(pendingNextFrameEvents_);
            }

        private:
            void SortEvents()
            {
				// std::vectorのsortを使ってイベントをトリガー時間でソート
                std::sort(events_.begin(), events_.end(), [](const Event& a, const Event& b)
                    {
                        return a.triggerTime < b.triggerTime;
                    });
            }
        };
	}
}