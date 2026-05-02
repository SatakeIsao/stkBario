/**
 * BinaryParameterLoader.h
 * バイナリパラメーターファイルの読み込みユーティリティ
 *
 * 【バイナリレイアウト】
 *   先頭 4バイト : レコード数 (int, リトルエンディアン)
 *   以降         : 各レコードが sizeof(T) バイトずつ連続
 *
 * 【使い方】
 *   // 単体取得
 *   auto* param = BinaryParameterLoader::LoadSingle<MasterBattleParameter>("data/bin/MasterBattleParameter.bin");
 *
 *   // 複数取得
 *   auto params = BinaryParameterLoader::LoadAll<MasterStageParameter>("data/bin/MasterStageParameter.bin");
 *
 *   // ParameterManager に登録して使う場合
 *   BinaryParameterLoader::Register<MasterBattleParameter>(
 *       ParameterManager::Get(), "data/bin/MasterBattleParameter.bin");
 */
#pragma once
#include <cstdio>
#include <vector>
#include <cassert>
#include "core/ParameterManager.h"


namespace app
{
    namespace core
    {
        class BinaryParameterLoader
        {
        public:
            /**
             * バイナリファイルから全レコードを読み込む
             * @return 読み込んだ構造体の配列（失敗時は空）
             */
            template <typename T>
            static std::vector<T> LoadAll(const char* path)
            {
                FILE* fp = fopen(path, "rb");
                if (fp == nullptr)
                {
                    K2_ASSERT(false, "バイナリファイルを開けませんでした: %s\n", path);
                    return {};
                }

                // 先頭4バイト：レコード数
                int count = 0;
                fread(&count, sizeof(int), 1, fp);
                if (count <= 0)
                {
                    fclose(fp);
                    return {};
                }

                // レコードを一括読み込み
                std::vector<T> records(count);
                fread(records.data(), sizeof(T), count, fp);
                fclose(fp);

                return records;
            }

            /**
             * バイナリファイルから先頭1レコードだけ読み込む（単一パラメーター用）
             * @return 読み込んだ構造体（失敗時は nullptr）
             */
            template <typename T>
            static T* LoadSingle(const char* path)
            {
                auto records = LoadAll<T>(path);
                if (records.empty())
                {
                    return nullptr;
                }
                T* result = new T(records[0]);
                return result;
            }

            /**
             * バイナリを読み込んで ParameterManager に登録する
             * （既存の LoadParameter の代替として使う）
             */
            template <typename T>
            static void Register(ParameterManager& manager, const char* path)
            {
                auto records = LoadAll<T>(path);
                K2_ASSERT(!records.empty(), "バイナリパラメーター読み込み失敗: %s\n", path);

                for (const auto& record : records)
                {
                    manager.RegisterBinary<T>(record);
                }
            }
        };
    }
}
