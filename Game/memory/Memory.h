/**
 * Memory.h
 * メモリ周りの処理を書くファイル
 */
#pragma once
#include <algorithm>


namespace app
{
	namespace memory
	{
        /** 固定配列を扱いやすくしたクラス */
        template <typename T>
        class Array
        {
        public:
            /** C++標準コンテナに合わせた型定義 */
            using value_type = T;
            using size_type = size_t;
            using difference_type = ptrdiff_t;
            using pointer = T*;
            using const_pointer = const T*;
            using reference = T&;
            using const_reference = const T&;

            /** 連続メモリなので、ポインタそのものがイテレータとして機能する */
            using iterator = T*;
            using const_iterator = const T*;

        private:
            /** 生ポインタ */
            T* m_data = nullptr;
            size_t m_size = 0;

        public:
            /** コンストラクタ */
            Array() = default;
            /** デストラクタ(生ポインタなので明示的にdelete[]が必要) */
            ~Array() {
                if (m_data) {
                    delete[] m_data;
                }
            }

            /**
             * 生成関数
             * NOTE: ここで初めてメモリ確保
             */
            void Create(size_t size) {
				K2_ASSERT(size > 0, "サイズは0より大きい値を指定してください");
                // 既に確保されていたら解放する（安全性のため）
                if (m_data) {
                    delete[] m_data;
                    m_size = 0;
                }
                if (size > 0) {
                    m_data = new T[size]; // 連続領域確保
                    m_size = size;
                }
            }

            /** コピー禁止 */
            Array(const Array&) = delete;
            Array& operator=(const Array&) = delete;

            /**
             * ムーブ (所有権移動) は許可
             */
            Array(Array&& other) noexcept
                : m_data(other.m_data), m_size(other.m_size)
            {
                // 生ポインタ管理なので、移動元のポインタをnullptrにする処理が必須
                other.m_data = nullptr;
                other.m_size = 0;
            }

            /** 代入 */
            Array& operator=(Array&& other) noexcept
            {
                if (this != &other) {
                    // 自分のリソースを解放
                    if (m_data) {
                        delete[] m_data;
                    }
                    // 所有権を奪う
                    m_data = other.m_data;
                    m_size = other.m_size;

                    // 移動元を空にする
                    other.m_data = nullptr;
                    other.m_size = 0;
                }
                return *this;
            }




            /**
             * アクセス関数関連
             */

            /**
             * 生ポインタ取得
             * NOTE: std::vectorのdata()関数のようなもの
             */
            T* data() noexcept { return m_data; }
            const T* data() const noexcept { return m_data; }

            /** 配列アクセス */
            T& operator[](size_t index) { return m_data[index]; }
            const T& operator[](size_t index) const { return m_data[index]; }

            /** サイズ取得 */
            size_t size() const noexcept { return m_size; }
            bool empty() const noexcept { return m_size == 0; }

            /** イテレータ (ポインタを返す) */
            iterator begin() noexcept { return m_data; }
            iterator end() noexcept { return m_data + m_size; }

            const_iterator begin() const noexcept { return m_data; }
            const_iterator end() const noexcept { return m_data + m_size; }

            const_iterator cbegin() const noexcept { return m_data; }
            const_iterator cend() const noexcept { return m_data + m_size; }
        };
	}
}