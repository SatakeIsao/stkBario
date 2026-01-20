/**
 * SoundManager.h
 * サウンド管理
 * 必要なサウンドファイルを読み込んだり再生したりなど管理する
 */
#pragma once
#include "sound/Types.h"

namespace app
{

    /** サウンド用のハンドル名 */
    using SoundHandle = uint32_t;
    /** ハンドル無効値 */
    static constexpr SoundHandle INVALID_SOUND_HANDLE = 0xffffffff;


    /**
     * サウンド管理クラス
     */
    class SoundManager
    {   
    private:
        /** BMG用のサウンドソースインスタンスを保持 */
        SoundSource* m_bgm = nullptr;
        /** SE用のサウンドソースインスタンスを保持 */
        std::map<SoundHandle, SoundSource*> m_seList;
        /**
         * マップで参照するようにハンドル数を保持
         * NOTE: 各サウンドソースを参照するための数値はユニークな数値になる
         */
        SoundHandle m_soundHandleCount = 0;


    private:
        SoundManager();
        ~SoundManager();

    
    public:
        /**
         * 更新処理
         * NOTE: 不要になったインスタンスの削除など
         */
        void Update();


    public:
        /** BGM再生 */
        void PlayBGM(const int kind);
        /** BGM停止 */
        void StopBGM();
    
        /** SE再生 */
        SoundHandle PlaySE(const int kind, const bool isLood = false, const bool is3D =false);
        /** SE停止 */
        void StopSE(const SoundHandle handle);


        SoundSource* FindSE(const SoundHandle handle)
        {
            auto it = m_seList.find(handle);
            if (it != m_seList.end()) {
                return it->second;
            }
            K2_ASSERT(false, "削除済みか追加されていないSEにアクセスしようとしています。\n");
            return nullptr;
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
            if (m_instance == nullptr)
            {
                m_instance = new SoundManager();
            }
        }


        /**
         * インスタンスを取得
         */
        static SoundManager& Get()
        {
            return *m_instance;
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
        static SoundManager* m_instance;
    };

}