/**
 * ActorStatusファイル
 * ステータスをまとめたクラス群
 */
#pragma once


namespace app
{
	namespace actor
	{
		/**
		 * キャラクターステータス基底構造体
		 */
		struct IActorStatus
		{
		public:
			virtual ~IActorStatus() {}

			/** パラメーターファイル読み込み */
			virtual void LoadParameter(const char* path) = 0;
			/** パラメーター数値設定等を行う関数 */
			virtual void Setup() = 0;
		};




		/** キャラクター用 */
		struct CharacterStatus : public IActorStatus
		{
		protected:
			float moveSpeed_ = 1.0f;
			float jumpMoveSpeed_ = 1.0f;
			float jumpPower_ = 1.0f;
			float friction_ = 1.0f;


		public:
			virtual ~CharacterStatus() {}

			virtual void LoadParameter(const char* path) override {}
			virtual void Setup() override {}


		public:
			inline float GetMoveSpeed() const { return moveSpeed_; }
			inline float GetJumpMoveSpeed() const { return jumpMoveSpeed_; }
			inline float GetJumpPower() const { return jumpPower_; }

			inline float GetFriction() const { return friction_; }
			inline void SetFriction(const float friction) { friction_ = friction; }
		};




		/** バトルキャラクター用 */
		struct BattleCharacterStatus : public CharacterStatus
		{
		protected:


		public:
			virtual ~BattleCharacterStatus() {}
			virtual void LoadParameter(const char* path) override;
			virtual void Setup() override;
		};
	}
}
