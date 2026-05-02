#pragma once

/*
* ParameterManager.h
* パラメーター管理
* こいつにキャラクターのステータスなど外部ファイルから読み込ませて保持させ、それを受け取って使う。
* パラメーターの識別をファイルパスで行っているので、パラメーター取得、解放などで事あるごとにパスを要求するが、
* LoadParameter関数以外で読み込みを行ったりはしない。
* シングルトンクラス。
*/

#include <iostream>
#include <fstream>
#include "json/json.hpp"
#include "util/CRC32.h"


/** ホットリロード有効か */
#ifdef K2_DEBUG
#define APP_ENABLE_PARAM_HOT_RELOAD
#endif


namespace app
{
	namespace core
	{

#ifdef APP_ENABLE_PARAM_HOT_RELOAD

#define appParameter(name)\
public:\
static constexpr uint32_t ID() {return Hash32(#name);}\
std::function<void(const nlohmann::json& j, name& p)> load;\
void Load(const nlohmann::json& j) override { load(j, *this); }

#else

#define appParameter(name)\
public:\
static constexpr uint32_t ID() {return Hash32(#name);}

#endif //APP_ENABLE_PARAM_HOT_RELOAD


		/**
		 * パラメーター基底構造体
		 */
		struct IParameter
		{
#ifdef APP_ENABLE_PARAM_HOT_RELOAD
			std::string m_path;								//パラメーターのファイルパス
			time_t m_lastWriteTime;							//最終更新時刻
			virtual void Load(const nlohmann::json& j) {};	// 読み込み関数
#endif // APP_PARAM_HOT_RELOAD
		};

		/**
		 * バイナリレイアウト検証マクロ
		 * Python の struct.pack 書式と C++ 構造体のフィールド配置が
		 * 一致しているかをコンパイル時に保証する。
		 *
		 * IParameter を継承した構造体は基底クラスのメタデータ（std::string,
		 * time_t, vtable など）を含むため sizeof(struct) では検証できない。
		 * そこでフィールドのみを持つミラー構造体（BinaryLayout_Xxx）を定義し、
		 * そちらのサイズを static_assert でチェックする。
		 * ミラー構造体はサイズ検証専用であり、実行時には使用しない。
		 */
#define APP_ASSERT_BINARY_SIZE(MirrorStruct, expectedBytes) \
			static_assert( \
				sizeof(MirrorStruct) == (expectedBytes), \
				#MirrorStruct " binary size mismatch" \
			)


		 /** バトル全般 */
		struct MasterBattleParameter : public IParameter
		{
			appParameter(MasterBattleParameter);

			float battleTime;			// 戦闘時間
		};
#pragma pack(push, 1)
		struct BinaryLayout_MasterBattleParameter { float battleTime; };
#pragma pack(pop)
		APP_ASSERT_BINARY_SIZE(BinaryLayout_MasterBattleParameter, 4);


		/** バトルキャラクター */
		struct MasterBattleCharacterParameter : public IParameter
		{
			appParameter(MasterBattleCharacterParameter);

			float moveSpeed;				// 移動速度
			float jumpMoveSpeed;			// ジャンプ中の移動速度
			float jumpPower;				// ジャンプ力
			float radius;					// 半径
			float height;					// 高さ
			float ghostbodyPosYOffset;		// ゴーストボディのY座標オフセット
			float collisionRadiusOffset;	// 当たり判定の半径拡張サイズ
			float collisionHeightOffset;	// 当たり判定の高さ拡張サイズ
		};
#pragma pack(push, 1)
		struct BinaryLayout_MasterBattleCharacterParameter { float moveSpeed, jumpMoveSpeed, jumpPower, radius, height, ghostbodyPosYOffset, collisionRadiusOffset, collisionHeightOffset; };
#pragma pack(pop)
		APP_ASSERT_BINARY_SIZE(BinaryLayout_MasterBattleCharacterParameter, 32);


		/** イベントキャラクター */
		struct MasterEventCharacterParameter : public IParameter
		{
			appParameter(MasterEventCharacterParameter);

			float moveSpeed;				// 移動速度
			float jumpMoveSpeed;			// ジャンプ中の移動速度
			float jumpPower;				// ジャンプ力
			float radius;					// 半径
			float height;					// 高さ
			float ghostbodyPosYOffset;		// ゴーストボディのY座標のオフセット
			float fallDeathThresholdPosY;   // 落下死と判定するY座標のしきい値
		};
#pragma pack(push, 1)
		struct BinaryLayout_MasterEventCharacterParameter { float moveSpeed, jumpMoveSpeed, jumpPower, radius, height, ghostbodyPosYOffset, fallDeathThresholdPosY; };
#pragma pack(pop)
		APP_ASSERT_BINARY_SIZE(BinaryLayout_MasterEventCharacterParameter, 28);


		/** イベントキャラクターAI */
		struct MasterEventCharacterAIParameter : public IParameter
		{
			appParameter(MasterEventCharacterAIParameter);

			float knockbackJumpPower;		// ノックバック時のジャンプ力
			float chaseDetectionRange;		// 追跡を開始する検知範囲
			float chaseFieldOfViewDeg;		// 追跡を開始する視野角
			float attackRange;				// 攻撃を開始する距離
			float chaseRange;				// 追跡を継続する距離
			float chaseAITimerInitial;		// 追跡開始時のAIタイマー初期値
			float patrolLeftTurnTime;		// パトロール：左折するまでの時間
			float patrolRightTurnTime;		// パトロール：右折するまでの時間
			float waitTime;					// 待機時間
		};
#pragma pack(push, 1)
		struct BinaryLayout_MasterEventCharacterAIParameter { float knockbackJumpPower, chaseDetectionRange, chaseFieldOfViewDeg, attackRange, chaseRange, chaseAITimerInitial, patrolLeftTurnTime, patrolRightTurnTime, waitTime; };
#pragma pack(pop)
		APP_ASSERT_BINARY_SIZE(BinaryLayout_MasterEventCharacterAIParameter, 36);


		/** ステージ全般 */
		struct MasterStageParameter : public IParameter
		{
			appParameter(MasterStageParameter);

			// ステージ
			float gravity;					// 重力
			float fallLimitY;				// 落下リミットY座標
			float friction;					// 摩擦係数
			float warpStartScale;			// ワープ開始スケール
			float warpEndScale;				// ワープ終了スケール
			float warpTime;					// ワープ時間
			// コイン
			float coinShrinkSpeed;			// コインが消える際の縮小速度
			float coinRotationSpeed;		// コインの回転速度
			float coinJumpInitVelocityY;	// コイン取得時の跳ね上がり初速
			float coinCollisionRadius;		// 当たり判定の半径サイズ
			float coinCollisionHeight;		// 当たり判定の高さサイズ
			float coinYOffset;				// コインの配置Y座標オフセット
			float coinEffectYOffset;		// コイン取得エフェクトのY座標オフセット
			// ゴール
			float spawnYOffset;				// プレイヤー・ゴールの配置Y座標オフセット
			float goalEffectRiseSpeed;		// ゴールエフェクトの毎秒上昇量
			float goalEffectRiseHeight;		// ゴールエフェクトの最大上昇量
			float goalEffectCoolTime;		// ゴールエフェクトの再生クールタイム
			float goalEffectScaleX;			// ゴールエフェクトのスケールX
			float goalEffectScaleY;			// ゴールエフェクトのスケールY
			float goalEffectScaleZ;			// ゴールエフェクトのスケールZ
			float goalTriggerDistance;		// ゴール判定に入る距離
			// タイマー演出
			float blinkTimeThreshold;		// 点滅開始・BGM切り替えを行う残り時間
			float separatorTime100Start;	// セパレーター表示区間1：開始残り時間
			float separatorTime100End;		// セパレーター表示区間1：終了残り時間
			float separatorTime50Start;		// セパレーター表示区間2：開始残り時間
			float separatorTime50End;		// セパレーター表示区間2：終了残り時間
		};
#pragma pack(push, 1)
		struct BinaryLayout_MasterStageParameter { float gravity, fallLimitY, friction, warpStartScale, warpEndScale, warpTime, coinShrinkSpeed, coinRotationSpeed, coinJumpInitVelocityY, coinCollisionRadius, coinCollisionHeight, coinYOffset, coinEffectYOffset, spawnYOffset, goalEffectRiseSpeed, goalEffectRiseHeight, goalEffectCoolTime, goalEffectScaleX, goalEffectScaleY, goalEffectScaleZ, goalTriggerDistance, blinkTimeThreshold, separatorTime100Start, separatorTime100End, separatorTime50Start, separatorTime50End; };
#pragma pack(pop)
		APP_ASSERT_BINARY_SIZE(BinaryLayout_MasterStageParameter, 104);


		/** バトルシーケンス */
		struct BattleSequenceParameter : public IParameter
		{
			appParameter(BattleSequenceParameter);

			// 待機・表示時間
			float delayWaitTime;			// 開始前の待機時間
			float readyDisplayTime;			// READY表示時間
			float goDisplayTime;			// GO表示時間
			float goFadeStartTime;			// GOフェードアウト開始時間
			// ゲームオーバー演出
			float gameoverFirstDown;		// 1回目落下時間
			float gameoverFirstUp;			// 1回目跳ね上がり時間
			float gameoverSecondDown;		// 2回目落下時間
			float gameoverSecondUp;			// 2回目跳ね上がり時間
			float gameoverThirdDown;		// 3回目落下時間
			// ゲームクリア演出
			float gameclearPopUpTime;		// ポップアップ時間
			float gameclearShrinkTime;		// 縮小時間
			float gameclearWaitTime;		// 待機時間
			// タイムアップ演出
			float timeupSlideInTime;		// スライドイン時間
			float timeupSlideBackTime;		// スライドバック時間
			float timeupWaitTime;			// 待機時間
			// ステージ設定
			int   stageMaxSlimes;			// ステージ最大スライム数
			int   stageMaxCoins;			// ステージ最大コイン数
		};
#pragma pack(push, 1)
		struct BinaryLayout_BattleSequenceParameter {
			float delayWaitTime, readyDisplayTime, goDisplayTime, goFadeStartTime;
			float gameoverFirstDown, gameoverFirstUp, gameoverSecondDown, gameoverSecondUp, gameoverThirdDown;
			float gameclearPopUpTime, gameclearShrinkTime, gameclearWaitTime;
			float timeupSlideInTime, timeupSlideBackTime, timeupWaitTime;
			int stageMaxSlimes, stageMaxCoins;
		};
#pragma pack(pop)
		APP_ASSERT_BINARY_SIZE(BinaryLayout_BattleSequenceParameter, 68);


		/** シーン共通 */
		struct MasterSceneParameter : public IParameter
		{
			appParameter(MasterSceneParameter);

			// シーン共通
			float sceneTransitionWaitTime;	// 演出後のシーン遷移待機時間
			float sceneRequestWaitTime;		// 直接シーン指定時の遷移待機時間
			// タイトルシーン
			float bButtonHoldThreshold;		// Bボタン長押し時間
			int   titleMenuIndexStart;		// スタート
			int   titleMenuIndexManual;		// あそびかた
			int   titleMenuIndexAward;		// アワード
			int   titleMenuIndexExit;		// おわり
		};
#pragma pack(push, 1)
		struct BinaryLayout_MasterSceneParameter { float sceneTransitionWaitTime, sceneRequestWaitTime, bButtonHoldThreshold; int titleMenuIndexStart, titleMenuIndexManual, titleMenuIndexAward, titleMenuIndexExit; };
#pragma pack(pop)
		APP_ASSERT_BINARY_SIZE(BinaryLayout_MasterSceneParameter, 28);


		/** バトルカメラ */
		struct MasterBattleCameraParameter : public IParameter
		{
			appParameter(MasterBattleCameraParameter);

			float distance;		// カメラ距離
			float height;		// カメラ高さ
			float fov;			// カメラFOV
			float nearClip;		// ニアクリップ
			float farClip;		// ファークリップ
			float rotationX;	// 回転X
			float rotationY;	// 回転Y
		};
#pragma pack(push, 1)
		struct BinaryLayout_MasterBattleCameraParameter { float distance, height, fov, nearClip, farClip, rotationX, rotationY; };
#pragma pack(pop)
		APP_ASSERT_BINARY_SIZE(BinaryLayout_MasterBattleCameraParameter, 28);


		/** インゲームUI */
		struct InGameUiParameter : public IParameter
		{
			appParameter(InGameUiParameter);

			// 時間・基本
			float bounceDuraction;			// バウンス時間
			float hurryUpTriggerTime;		// HurryUp発動残り時間
			// HPカラー
			float colorHpMaxX, colorHpMaxY, colorHpMaxZ;
			float colorCautionX, colorCautionY, colorCautionZ;
			float colorDangerX, colorDangerY, colorDangerZ;
			float colorSafeX, colorSafeY, colorSafeZ;
			float colorHighscoreX, colorHighscoreY, colorHighscoreZ, colorHighscoreW;
			float colorDefaultX, colorDefaultY, colorDefaultZ, colorDefaultW;
			// アルファ・補間
			float alphaActive;				// アクティブ時アルファ
			float alphaInactive;			// 非アクティブ時アルファ
			float lerpSpeed;				// 補間速度
			// バウンス
			float bounceUpLimit;			// バウンス上限
			float bounceTotalDuration;		// バウンス総時間
			// タイマートリガー
			float triggerPulse100;			// 100秒トリガー
			float triggerPulse50;			// 50秒トリガー
			float triggerPulse30;			// 30秒トリガー
			// パルス
			float pulseUpDuration;			// パルス上昇時間
			float pulseTotalDuration;		// パルス総時間
			// スケール
			float scaleDefaultX, scaleDefaultY, scaleDefaultZ;
			// int
			int   maxHp;					// 最大HP
			int   maxTime;					// 最大時間
			int   thresholdLow;				// 低HP閾値
			int   thresholdMid;				// 中HP閾値
			int   thresholdHigh;			// 高HP閾値
			int   colorChangeCount;			// カラー変更カウント
		};
#pragma pack(push, 1)
		struct BinaryLayout_InGameUiParameter {
			float bounceDuraction, hurryUpTriggerTime;
			float colorHpMaxX, colorHpMaxY, colorHpMaxZ;
			float colorCautionX, colorCautionY, colorCautionZ;
			float colorDangerX, colorDangerY, colorDangerZ;
			float colorSafeX, colorSafeY, colorSafeZ;
			float colorHighscoreX, colorHighscoreY, colorHighscoreZ, colorHighscoreW;
			float colorDefaultX, colorDefaultY, colorDefaultZ, colorDefaultW;
			float alphaActive, alphaInactive, lerpSpeed;
			float bounceUpLimit, bounceTotalDuration;
			float triggerPulse100, triggerPulse50, triggerPulse30;
			float pulseUpDuration, pulseTotalDuration;
			float scaleDefaultX, scaleDefaultY, scaleDefaultZ;
			int maxHp, maxTime;
			int thresholdLow, thresholdMid, thresholdHigh;
			int colorChangeCount;
		};
#pragma pack(pop)
		APP_ASSERT_BINARY_SIZE(BinaryLayout_InGameUiParameter, 164);


		/** マニュアルメニュー */
		struct ManualMenuParameter : public IParameter
		{
			appParameter(ManualMenuParameter);

			float cursolPositionXA;		// カーソルX座標A
			float cursolPositionXB;		// カーソルX座標B
			float cursolPositionYA;		// カーソルY座標A
			float cursolPositionYB;		// カーソルY座標B
			float gaugePositionX;		// ゲージX座標
			float gaugePositionY;		// ゲージY座標
			float gaugePositionZ;		// ゲージZ座標
			float gaugeInnerRadius;		// ゲージ内半径
			float gaugeOuterRadius;		// ゲージ外半径
			float gaugeScale;			// ゲージスケール
			float gaugeFillColorX;		// ゲージ塗りカラーX
			float gaugeFillColorY;		// ゲージ塗りカラーY
			float gaugeFillColorZ;		// ゲージ塗りカラーZ
			float gaugeFillColorW;		// ゲージ塗りカラーW
			float gaugeEmptyColorX;		// ゲージ空カラーX
			float gaugeEmptyColorY;		// ゲージ空カラーY
			float gaugeEmptyColorZ;		// ゲージ空カラーZ
			float gaugeEmptyColorW;		// ゲージ空カラーW
			float bIconPositionX;		// BアイコンX座標
			float bIconPositionY;		// BアイコンY座標
			float bIconPositionZ;		// BアイコンZ座標
			float bIconScaleX;			// BアイコンスケールX
			float bIconScaleY;			// BアイコンスケールY
			float bIconScaleZ;			// BアイコンスケールZ
			float bIconGaugeSizeX;		// BアイコンゲージサイズX
			float bIconGaugeSizeY;		// BアイコンゲージサイズY
			float bIconButtonSizeX;		// BアイコンボタンサイズX
			float bIconButtonSizeY;		// BアイコンボタンサイズY
		};
#pragma pack(push, 1)
		struct BinaryLayout_ManualMenuParameter {
			float cursolPositionXA, cursolPositionXB, cursolPositionYA, cursolPositionYB;
			float gaugePositionX, gaugePositionY, gaugePositionZ;
			float gaugeInnerRadius, gaugeOuterRadius, gaugeScale;
			float gaugeFillColorX, gaugeFillColorY, gaugeFillColorZ, gaugeFillColorW;
			float gaugeEmptyColorX, gaugeEmptyColorY, gaugeEmptyColorZ, gaugeEmptyColorW;
			float bIconPositionX, bIconPositionY, bIconPositionZ;
			float bIconScaleX, bIconScaleY, bIconScaleZ;
			float bIconGaugeSizeX, bIconGaugeSizeY;
			float bIconButtonSizeX, bIconButtonSizeY;
		};
#pragma pack(pop)
		APP_ASSERT_BINARY_SIZE(BinaryLayout_ManualMenuParameter, 112);


		/** リザルトメニュー */
		struct ResultMenuParameter : public IParameter
		{
			appParameter(ResultMenuParameter);

			float masterMaxTime;			// 制限時間
			float nextStepDelay;			// 次ステップ遅延
			float awardStepDelay;			// 称号ステップ遅延
			float exitBufferTime;			// 終了バッファ時間
			float thresholdTimeRankS;		// Sランク閾値
			float thresholdTimeRankA;		// Aランク閾値
			float thresholdTimeRankB;		// Bランク閾値
			float thresholdTimeRankC;		// Cランク閾値
			float selectionScaleX, selectionScaleY, selectionScaleZ;
			float defaultScaleX, defaultScaleY, defaultScaleZ;
			float selectionColorX, selectionColorY, selectionColorZ;
			float defaultColorX, defaultColorY, defaultColorZ;
			float bonusTextColorX, bonusTextColorY, bonusTextColorZ, bonusTextColorW;
			int   coinScoreWeight;			// コインスコア重み
			int   timeBonusRankS;			// Sランクタイムボーナス
			int   timeBonusRankA;			// Aランクタイムボーナス
			int   timeBonusRankB;			// Bランクタイムボーナス
			int   timeBonusRankC;			// Cランクタイムボーナス
		};
#pragma pack(push, 1)
		struct BinaryLayout_ResultMenuParameter {
			float masterMaxTime, nextStepDelay, awardStepDelay, exitBufferTime;
			float thresholdTimeRankS, thresholdTimeRankA, thresholdTimeRankB, thresholdTimeRankC;
			float selectionScaleX, selectionScaleY, selectionScaleZ;
			float defaultScaleX, defaultScaleY, defaultScaleZ;
			float selectionColorX, selectionColorY, selectionColorZ;
			float defaultColorX, defaultColorY, defaultColorZ;
			float bonusTextColorX, bonusTextColorY, bonusTextColorZ, bonusTextColorW;
			int coinScoreWeight;
			int timeBonusRankS, timeBonusRankA, timeBonusRankB, timeBonusRankC;
		};
#pragma pack(pop)
		APP_ASSERT_BINARY_SIZE(BinaryLayout_ResultMenuParameter, 116);


		struct MasterSoundOptionMenuParameter : public IParameter
		{
			appParameter(MasterSoundOptionMenuParameter);

			// ノブX座標
			float knobXA, knobXB, knobXC, knobXD, knobXE, knobXF, knobXG, knobXH, knobXI, knobXJ, knobXK;
			// ゲージバーX座標
			float gaugeBarXA, gaugeBarXB, gaugeBarXC, gaugeBarXD, gaugeBarXE, gaugeBarXF, gaugeBarXG, gaugeBarXH, gaugeBarXI, gaugeBarXJ, gaugeBarXK;
			// ゲージバーY座標
			float gaugeBarYA, gaugeBarYB, gaugeBarYC;
			// ゲージバースケール
			float gaugeBarScaleXA, gaugeBarScaleXB, gaugeBarScaleXC, gaugeBarScaleXD, gaugeBarScaleXE;
			float gaugeBarScaleXF, gaugeBarScaleXG, gaugeBarScaleXH, gaugeBarScaleXI, gaugeBarScaleXJ, gaugeBarScaleXK;
			// 音量
			float volumeStep, volumeMax, volumeMin;
			float volumeDefaultMaster, volumeDefaultBgm, volumeDefaultSe;
			float volumeDisplayMultiplier;
			// スケール・カラー
			float digitScaleX, digitScaleY, digitScaleZ;
			float textScaleX, textScaleY, textScaleZ;
			float defaultScaleX, defaultScaleY, defaultScaleZ;
			float selectionColorX, selectionColorY, selectionColorZ;
			float defaultColorX, defaultColorY, defaultColorZ;
		};
#pragma pack(push, 1)
		struct BinaryLayout_MasterSoundOptionMenuParameter {
			float knobXA, knobXB, knobXC, knobXD, knobXE, knobXF, knobXG, knobXH, knobXI, knobXJ, knobXK;
			float gaugeBarXA, gaugeBarXB, gaugeBarXC, gaugeBarXD, gaugeBarXE, gaugeBarXF, gaugeBarXG, gaugeBarXH, gaugeBarXI, gaugeBarXJ, gaugeBarXK;
			float gaugeBarYA, gaugeBarYB, gaugeBarYC;
			float gaugeBarScaleXA, gaugeBarScaleXB, gaugeBarScaleXC, gaugeBarScaleXD, gaugeBarScaleXE;
			float gaugeBarScaleXF, gaugeBarScaleXG, gaugeBarScaleXH, gaugeBarScaleXI, gaugeBarScaleXJ, gaugeBarScaleXK;
			float volumeStep, volumeMax, volumeMin;
			float volumeDefaultMaster, volumeDefaultBgm, volumeDefaultSe, volumeDisplayMultiplier;
			float digitScaleX, digitScaleY, digitScaleZ;
			float textScaleX, textScaleY, textScaleZ;
			float defaultScaleX, defaultScaleY, defaultScaleZ;
			float selectionColorX, selectionColorY, selectionColorZ;
			float defaultColorX, defaultColorY, defaultColorZ;
		};
#pragma pack(pop)
		APP_ASSERT_BINARY_SIZE(BinaryLayout_MasterSoundOptionMenuParameter, 232);


		/** ポーズメニュー */
		struct MasterPauseMenuParameter : public IParameter
		{
			appParameter(MasterPauseMenuParameter);

			float cursolPositionXA;
			float cursolPositionXB;
			float cursolPositionYA;
			float cursolPositionYB;
			float selectionScaleX, selectionScaleY, selectionScaleZ;
			float defaultScaleX, defaultScaleY, defaultScaleZ;
			float selectionColorX, selectionColorY, selectionColorZ;
			float defaultColorX, defaultColorY, defaultColorZ;
			int   maxCursolIndex;
		};
#pragma pack(push, 1)
		struct BinaryLayout_MasterPauseMenuParameter {
			float cursolPositionXA, cursolPositionXB, cursolPositionYA, cursolPositionYB;
			float selectionScaleX, selectionScaleY, selectionScaleZ;
			float defaultScaleX, defaultScaleY, defaultScaleZ;
			float selectionColorX, selectionColorY, selectionColorZ;
			float defaultColorX, defaultColorY, defaultColorZ;
			int maxCursolIndex;
		};
#pragma pack(pop)
		APP_ASSERT_BINARY_SIZE(BinaryLayout_MasterPauseMenuParameter, 68);


		struct ReturnToTitleMenuParameter : public IParameter
		{
			appParameter(ReturnToTitleMenuParameter);

			float cursolPositionXA;
			float cursolPositionXB;
			float cursolPositionYA;
			float cursolPositionYB;
			float selectionScaleX, selectionScaleY, selectionScaleZ;
			float defaultScaleX, defaultScaleY, defaultScaleZ;
			float selectionColorX, selectionColorY, selectionColorZ;
			float defaultColorX, defaultColorY, defaultColorZ;
			int   maxCursolIndex;
		};
#pragma pack(push, 1)
		struct BinaryLayout_ReturnToTitleMenuParameter {
			float cursolPositionXA, cursolPositionXB, cursolPositionYA, cursolPositionYB;
			float selectionScaleX, selectionScaleY, selectionScaleZ;
			float defaultScaleX, defaultScaleY, defaultScaleZ;
			float selectionColorX, selectionColorY, selectionColorZ;
			float defaultColorX, defaultColorY, defaultColorZ;
			int maxCursolIndex;
		};
#pragma pack(pop)
		APP_ASSERT_BINARY_SIZE(BinaryLayout_ReturnToTitleMenuParameter, 68);


		struct GameOverMenuParameter : public IParameter
		{
			appParameter(GameOverMenuParameter);

			float cursolPositionXA;		// カーソルX座標A
			float cursolPositionXB;		// カーソルX座標B
			float cursolPositionYA;		// カーソルY座標A
			float cursolPositionYB;		// カーソルY座標B
			float selectionScaleX;		// 選択スケールX
			float selectionScaleY;		// 選択スケールY
			float selectionScaleZ;		// 選択スケールZ
			float defaultScaleX;		// デフォルトスケールX
			float defaultScaleY;		// デフォルトスケールY
			float defaultScaleZ;		// デフォルトスケールZ
			float selectionColorX;		// 選択カラーX
			float selectionColorY;		// 選択カラーY
			float selectionColorZ;		// 選択カラーZ
			float defaultColorX;		// デフォルトカラーX
			float defaultColorY;		// デフォルトカラーY
			float defaultColorZ;		// デフォルトカラーZ
			int   maxCursolIndex;		// カーソル最大インデックス
		};
#pragma pack(push, 1)
		struct BinaryLayout_GameOverMenuParameter {
			float cursolPositionXA, cursolPositionXB;
			float cursolPositionYA, cursolPositionYB;
			float selectionScaleX, selectionScaleY, selectionScaleZ;
			float defaultScaleX, defaultScaleY, defaultScaleZ;
			float selectionColorX, selectionColorY, selectionColorZ;
			float defaultColorX, defaultColorY, defaultColorZ;
			int maxCursolIndex;
		};
#pragma pack(pop)
		APP_ASSERT_BINARY_SIZE(BinaryLayout_GameOverMenuParameter, 68);


		struct TitleMenuParameter : public IParameter
		{
			appParameter(TitleMenuParameter);

			float cursolPositionXA, cursolPositionXB, cursolPositionXC, cursolPositionXD;
			float cursolPositionYA, cursolPositionYB, cursolPositionYC, cursolPositionYD;
			float selectionScaleX, selectionScaleY, selectionScaleZ;
			float defaultScaleX, defaultScaleY, defaultScaleZ;
			float selectionColorX, selectionColorY, selectionColorZ;
			float defaultColorX, defaultColorY, defaultColorZ;
			int   maxCursolIndex;
		};
#pragma pack(push, 1)
		struct BinaryLayout_TitleMenuParameter {
			float cursolPositionXA, cursolPositionXB, cursolPositionXC, cursolPositionXD;
			float cursolPositionYA, cursolPositionYB, cursolPositionYC, cursolPositionYD;
			float selectionScaleX, selectionScaleY, selectionScaleZ;
			float defaultScaleX, defaultScaleY, defaultScaleZ;
			float selectionColorX, selectionColorY, selectionColorZ;
			float defaultColorX, defaultColorY, defaultColorZ;
			int maxCursolIndex;
		};
#pragma pack(pop)
		APP_ASSERT_BINARY_SIZE(BinaryLayout_TitleMenuParameter, 84);


		struct AwardMenuParameter : public IParameter
		{
			appParameter(AwardMenuParameter);

			// バーカーソル
			float barCursolPositionYA;		// バーカーソルY座標A
			float barCursolPositionYB;		// バーカーソルY座標B
			float barCursolPositionYC;		// バーカーソルY座標C
			float barCursolPositionYD;		// バーカーソルY座標D
			// 指カーソル
			float panelCursorPosX_Left;		// パネルカーソルX（左）
			float panelCursorPosX_Right;	// パネルカーソルX（右）
			float panelCursorPosY_Top;		// パネルカーソルY（上）
			float panelCursorPosY_Bottom;	// パネルカーソルY（下）
			// 称号条件
			float fastTimeLimit;			// スピードスター判定時間
			float slowTimethreshold;		// のんびりやさん判定時間
			// 称号メニュー
			float displayRowCount;			// 表示行数
			float maxScrollVal;				// 最大スクロール値
			float selectionScaleX;			// 選択スケールX
			float selectionScaleY;			// 選択スケールY
			float selectionScaleZ;			// 選択スケールZ
			float defaultScaleX;			// デフォルトスケールX
			float defaultScaleY;			// デフォルトスケールY
			float defaultScaleZ;			// デフォルトスケールZ
			float selectionColorX;			// 選択カラーX
			float selectionColorY;			// 選択カラーY
			float selectionColorZ;			// 選択カラーZ
			float defaultColorX;			// デフォルトカラーX
			float defaultColorY;			// デフォルトカラーY
			float defaultColorZ;			// デフォルトカラーZ
			// int
			int   lowHpThreshold;			// いのちだいじに判定HP
			int   maxHpThreshold;			// げんきいっぱい判定HP
			int   jumpCountFrog;			// ぴょんぴょんがえるジャンプ数
			int   jumpCountRabbit;			// はねるうさぎジャンプ数
			int   maxRows;					// 最大行数
			int   maxCols;					// 最大列数
			int   titleYUp;					// タイトルY上
			int   titleYDown;				// タイトルY下
			int   condYUp;					// 条件Y上
			int   condYDown;				// 条件Y下
		};
#pragma pack(push, 1)
		struct BinaryLayout_AwardMenuParameter {
			float barCursolPositionYA, barCursolPositionYB, barCursolPositionYC, barCursolPositionYD;
			float panelCursorPosX_Left, panelCursorPosX_Right, panelCursorPosY_Top, panelCursorPosY_Bottom;
			float fastTimeLimit, slowTimethreshold;
			float displayRowCount, maxScrollVal;
			float selectionScaleX, selectionScaleY, selectionScaleZ;
			float defaultScaleX, defaultScaleY, defaultScaleZ;
			float selectionColorX, selectionColorY, selectionColorZ;
			float defaultColorX, defaultColorY, defaultColorZ;
			int lowHpThreshold, maxHpThreshold;
			int jumpCountFrog, jumpCountRabbit;
			int maxRows, maxCols;
			int titleYUp, titleYDown;
			int condYUp, condYDown;
		};
#pragma pack(pop)
		APP_ASSERT_BINARY_SIZE(BinaryLayout_AwardMenuParameter, 136);


#undef appParameter

		/**
		 * パラメーター管理クラス
		 */
		class ParameterManager
		{
		private:
			using ParameterVector = std::vector<IParameter*>;
			using ParameterMap = std::map<uint32_t, ParameterVector>;

		private:
			/** パラメータとIDのリスト */
			ParameterMap m_parameterMap;

		private:
			ParameterManager();
			~ParameterManager();

		public:

			/**
			 * パラメーターファイルを読み込む
			 * 関数ポインタで読み込み処理を受け取る
			 * @typename T パラメーターの種類
			 * @param path ファイルパス
			 * @param func 読み込み処理
			 */
			template<typename T>
			void LoadParameter(const char* path, const std::function<void(const nlohmann::json& json, T& p)>& func)
			{
				//ファイルを開く
				std::ifstream file(path);
				if (!file.is_open())
				{
					return;
				}

				//jsonファイルとして読み込む？
				nlohmann::json jsonRoot;
				file >> jsonRoot;

				//読み込んだパラメーターを一時的に持つ受け皿
				ParameterVector parameters;

				for (const auto& j : jsonRoot)
				{
					T* parameter = new T();
#ifdef APP_ENABLE_PARAM_HOT_RELOAD
					parameter->m_path = std::string(path);
					parameter->m_lastWriteTime = GetFileLastWriteTime(path);
					parameter->load = func;
#endif // APP_ENABLE_PARAM_HOT_RELOAD

					//パラメータ読み込み処理
					func(j, *parameter);
					parameters.push_back(static_cast<IParameter*>(parameter));
				}

				//パラメーターを登録
				m_parameterMap.emplace(T::ID(), parameters);
			}

			/// <summary>
			/// パラメーター解放
			/// </summary>
			/// <param name="path">解放するパラメーターのファイルパス</param>
			template <typename T>
			void UnloadParameter()
			{
				auto it = m_parameterMap.find(T::ID());
				if (it != m_parameterMap.end())
				{
					auto& parameters = it->second;
					for (auto* p : parameters)
					{
						delete p;
					}
					m_parameterMap.erase(it);
				}
			}

			/// <summary>
			/// バイナリから読み込んだ構造体を直接登録する
			/// BinaryParameterLoader と組み合わせて使う
			/// </summary>
			template <typename T>
			void RegisterBinary(const T& record)
			{
				T* parameter = new T(record);
				m_parameterMap[T::ID()].push_back(static_cast<IParameter*>(parameter));
			}

			/// <summary>
			/// パラメーターの取得
			/// </summary>
			/// <typeparam name="T">取得するパラメーターの構造体</typeparam>
			/// <param name="path">取得するパラメーターのファイルパス</param>
			/// <param name="index">一つのファイルに複数のパラメーターを入れた場合は何番目かこれで指定する</param>
			/// <returns></returns>
			template <typename T>
			const T* GetParameter(const int index = 0) const
			{
				const auto parameters = GetParameters<T>();
				if (parameters.size() == 0)
				{
					return nullptr;
				}
				if (parameters.size() <= index)
				{
					return nullptr;
				}

				return parameters[index];
			}

			/// <summary>
			/// 複数のパラメーターを取得する
			/// </summary>
			/// <typeparam name="T">取得するパラメーターの構造体</typeparam>
			/// <param name="path">取得するパラメーターのファイルパス</param>
			/// <returns></returns>
			template <typename T>
			const std::vector<T*> GetParameters() const
			{
				std::vector<T*> parameters;

				auto it = m_parameterMap.find(T::ID());
				if (it != m_parameterMap.end())
				{
					for (auto* parameter : it->second)
					{
						parameters.push_back(static_cast<T*>(parameter));
					}
				}
				return parameters;
			}

			/// <summary>
			/// パラメーターをラムダ式で回す
			/// </summary>
			/// <typeparam name="T"></typeparam>
			/// <param name="path"></param>
			/// <param name="func"></param>
			template<typename T>
			void ForEach(std::function<void(const T&)> func) const
			{
				const std::vector<T*> parameters = GetParameters<T>();
				for (const T* parameter : parameters)
				{
					func(*parameter);
				}
			}

		public:
#ifdef APP_ENABLE_PARAM_HOT_RELOAD
			void Update()
			{
				for (auto paramPair : m_parameterMap)
				{
					for (auto param : paramPair.second)
					{
						if (CheckFileModified(param))
						{
							std::ifstream file(param->m_path);
							if (!file.is_open())
							{
								return;
							}

							nlohmann::json jsonRoot;
							file >> jsonRoot;

							ParameterVector parameters;

							for (const auto& j : jsonRoot)
							{
								param->m_lastWriteTime = GetFileLastWriteTime(param->m_path.c_str());
								param->Load(j);
							}
						}
					}
				}
			}

			//ファイル更新日時取得
			static time_t GetFileLastWriteTime(const char* path)
			{
				struct stat result;
				//stat関数でファイル情報を取得(0なら成功)
				if (stat(path, &result) == 0)
				{
					return result.st_mtime;
				}

				return 0;
			}

			//ファイル更新チェック
			static bool CheckFileModified(const IParameter* param)
			{
				//ファイルの更新日時から変更があったか確認
				if (GetFileLastWriteTime(param->m_path.c_str()) > param->m_lastWriteTime)
				{
					return true;
				}
				return false;
			}
#endif // APP_ENABLE_PARAM_HOT_RELOAD


			/*
			* シングルトン用コード
			*/
		private:
			static ParameterManager* m_instance;
		public:

			/// <summary>
			/// インスタンス生成
			/// </summary>
			static void Initialize()
			{
				if (m_instance == nullptr)
				{
					m_instance = new ParameterManager();
				}
			}

			/// <summary>
			/// インスタンスを取得
			/// </summary>
			/// <returns></returns>
			static ParameterManager& Get()
			{
				return *m_instance;
			}

			static void Finalize()
			{
				if (m_instance != nullptr)
				{
					delete m_instance;
					m_instance = nullptr;
				}
			}
		};


	}
}