/**
 * ParameterLoader.cpp
 * バイナリパラメーターを読み込んで ParameterManager に登録する
 *
 * 【設計】
 *   バイナリは BinaryLayout_Xxx（フィールドのみ）で読み込み、
 *   MasterXxxParameter（IParameter継承）に1フィールドずつコピーして登録する。
 *   これにより IParameter の基底サイズとバイナリレイアウトのズレを回避する。
 */
#include "stdafx.h"
#include "core/ParameterLoader.h"

namespace app
{
    namespace core
    {
        void ParameterLoader::LoadAll()
        {
            auto& mgr = ParameterManager::Get();

            // MasterBattleParameter
            {
                auto bin = BinaryParameterLoader::LoadAll<BinaryLayout_MasterBattleParameter>(
                    "Assets/master/battle/MasterBattleParameter.bin");
                if (bin.empty()) { OutputDebugStringA("[WARN] ParameterLoader: .bin file not found or empty\n"); }
                for (const auto& b : bin)
                {
                    MasterBattleParameter* p = new MasterBattleParameter();
                    p->battleTime = b.battleTime;
                    mgr.RegisterBinary(*p);
                }
            }

            // MasterBattleCameraParameter
            {
                auto bin = BinaryParameterLoader::LoadAll<BinaryLayout_MasterBattleCameraParameter>(
                    "Assets/master/battle/MasterBattleCameraParameter.bin");
                if (bin.empty()) { OutputDebugStringA("[WARN] ParameterLoader: .bin file not found or empty\n"); }
                for (const auto& b : bin)
                {
                    MasterBattleCameraParameter* p = new MasterBattleCameraParameter();
                    p->distance = b.distance;
                    p->height = b.height;
                    p->fov = b.fov;
                    p->nearClip = b.nearClip;
                    p->farClip = b.farClip;
                    p->rotationX = b.rotationX;
                    p->rotationY = b.rotationY;
                    mgr.RegisterBinary(*p);
                }
            }

            // MasterBattleCharacterParameter
            {
                auto bin = BinaryParameterLoader::LoadAll<BinaryLayout_MasterBattleCharacterParameter>(
                    "Assets/master/battle/MasterBattleCharacterParameter.bin");
                if (bin.empty()) { OutputDebugStringA("[WARN] ParameterLoader: .bin file not found or empty\n"); }
                for (const auto& b : bin)
                {
                    MasterBattleCharacterParameter* p = new MasterBattleCharacterParameter();
                    p->moveSpeed = b.moveSpeed;
                    p->jumpMoveSpeed = b.jumpMoveSpeed;
                    p->jumpPower = b.jumpPower;
                    p->radius = b.radius;
                    p->height = b.height;
                    p->ghostbodyPosYOffset = b.ghostbodyPosYOffset;
                    p->collisionRadiusOffset = b.collisionRadiusOffset;
                    p->collisionHeightOffset = b.collisionHeightOffset;
                    mgr.RegisterBinary(*p);
                }
            }

            // MasterEventCharacterParameter
            {
                auto bin = BinaryParameterLoader::LoadAll<BinaryLayout_MasterEventCharacterParameter>(
                    "Assets/master/battle/MasterEventCharacterParameter.bin");
                if (bin.empty()) { OutputDebugStringA("[WARN] ParameterLoader: .bin file not found or empty\n"); }
                for (const auto& b : bin)
                {
                    MasterEventCharacterParameter* p = new MasterEventCharacterParameter();
                    p->moveSpeed = b.moveSpeed;
                    p->jumpMoveSpeed = b.jumpMoveSpeed;
                    p->jumpPower = b.jumpPower;
                    p->radius = b.radius;
                    p->height = b.height;
                    p->ghostbodyPosYOffset = b.ghostbodyPosYOffset;
                    p->fallDeathThresholdPosY = b.fallDeathThresholdPosY;
                    mgr.RegisterBinary(*p);
                }
            }

            // MasterStageParameter
            {
                auto bin = BinaryParameterLoader::LoadAll<BinaryLayout_MasterStageParameter>(
                    "Assets/master/battle/MasterStageParameter.bin");
                if (bin.empty()) { OutputDebugStringA("[WARN] ParameterLoader: .bin file not found or empty\n"); }
                for (const auto& b : bin)
                {
                    MasterStageParameter* p = new MasterStageParameter();
                    p->gravity = b.gravity;
                    p->fallLimitY = b.fallLimitY;
                    p->friction = b.friction;
                    p->warpStartScale = b.warpStartScale;
                    p->warpEndScale = b.warpEndScale;
                    p->warpTime = b.warpTime;
                    p->coinShrinkSpeed = b.coinShrinkSpeed;
                    p->coinRotationSpeed = b.coinRotationSpeed;
                    p->coinJumpInitVelocityY = b.coinJumpInitVelocityY;
                    p->coinCollisionRadius = b.coinCollisionRadius;
                    p->coinCollisionHeight = b.coinCollisionHeight;
                    p->coinYOffset = b.coinYOffset;
                    p->coinEffectYOffset = b.coinEffectYOffset;
                    p->spawnYOffset = b.spawnYOffset;
                    p->goalEffectRiseSpeed = b.goalEffectRiseSpeed;
                    p->goalEffectRiseHeight = b.goalEffectRiseHeight;
                    p->goalEffectCoolTime = b.goalEffectCoolTime;
                    p->goalEffectScaleX = b.goalEffectScaleX;
                    p->goalEffectScaleY = b.goalEffectScaleY;
                    p->goalEffectScaleZ = b.goalEffectScaleZ;
                    p->goalTriggerDistance = b.goalTriggerDistance;
                    p->blinkTimeThreshold = b.blinkTimeThreshold;
                    p->separatorTime100Start = b.separatorTime100Start;
                    p->separatorTime100End = b.separatorTime100End;
                    p->separatorTime50Start = b.separatorTime50Start;
                    p->separatorTime50End = b.separatorTime50End;
                    mgr.RegisterBinary(*p);
                }
            }

            // MasterEventCharacterAIParameter
            {
                auto bin = BinaryParameterLoader::LoadAll<BinaryLayout_MasterEventCharacterAIParameter>(
                    "Assets/master/battle/MasterEventCharacterAIParameter.bin");
                if (bin.empty()) { OutputDebugStringA("[WARN] ParameterLoader: .bin file not found or empty\n"); }
                for (const auto& b : bin)
                {
                    MasterEventCharacterAIParameter* p = new MasterEventCharacterAIParameter();
                    p->knockbackJumpPower = b.knockbackJumpPower;
                    p->chaseDetectionRange = b.chaseDetectionRange;
                    p->chaseFieldOfViewDeg = b.chaseFieldOfViewDeg;
                    p->attackRange = b.attackRange;
                    p->chaseRange = b.chaseRange;
                    p->chaseAITimerInitial = b.chaseAITimerInitial;
                    p->patrolLeftTurnTime = b.patrolLeftTurnTime;
                    p->patrolRightTurnTime = b.patrolRightTurnTime;
                    p->waitTime = b.waitTime;
                    mgr.RegisterBinary(*p);
                }
            }

            // MasterSceneParameter
            {
                auto bin = BinaryParameterLoader::LoadAll<BinaryLayout_MasterSceneParameter>(
                    "Assets/master/scene/MasterSceneParameter.bin");
                if (bin.empty()) { OutputDebugStringA("[WARN] ParameterLoader: .bin file not found or empty\n"); }
                for (const auto& b : bin)
                {
                    MasterSceneParameter* p = new MasterSceneParameter();
                    p->sceneTransitionWaitTime = b.sceneTransitionWaitTime;
                    p->sceneRequestWaitTime = b.sceneRequestWaitTime;
                    p->bButtonHoldThreshold = b.bButtonHoldThreshold;
                    p->titleMenuIndexStart = b.titleMenuIndexStart;
                    p->titleMenuIndexManual = b.titleMenuIndexManual;
                    p->titleMenuIndexAward = b.titleMenuIndexAward;
                    p->titleMenuIndexExit = b.titleMenuIndexExit;
                    mgr.RegisterBinary(*p);
                }
            }

            // BattleSequenceParameter
            {
                auto bin = BinaryParameterLoader::LoadAll<BinaryLayout_BattleSequenceParameter>(
                    "Assets/master/BattleSequenceParameter.bin");
                if (bin.empty()) { OutputDebugStringA("[WARN] ParameterLoader: .bin file not found or empty\n"); }
                for (const auto& b : bin)
                {
                    BattleSequenceParameter* p = new BattleSequenceParameter();
                    p->delayWaitTime = b.delayWaitTime;
                    p->readyDisplayTime = b.readyDisplayTime;
                    p->goDisplayTime = b.goDisplayTime;
                    p->goFadeStartTime = b.goFadeStartTime;
                    p->gameoverFirstDown = b.gameoverFirstDown;
                    p->gameoverFirstUp = b.gameoverFirstUp;
                    p->gameoverSecondDown = b.gameoverSecondDown;
                    p->gameoverSecondUp = b.gameoverSecondUp;
                    p->gameoverThirdDown = b.gameoverThirdDown;
                    p->gameclearPopUpTime = b.gameclearPopUpTime;
                    p->gameclearShrinkTime = b.gameclearShrinkTime;
                    p->gameclearWaitTime = b.gameclearWaitTime;
                    p->timeupSlideInTime = b.timeupSlideInTime;
                    p->timeupSlideBackTime = b.timeupSlideBackTime;
                    p->timeupWaitTime = b.timeupWaitTime;
                    p->stageMaxSlimes = b.stageMaxSlimes;
                    p->stageMaxCoins = b.stageMaxCoins;
                    mgr.RegisterBinary(*p);
                }
            }

            // TitleMenuParameter
            {
                auto bin = BinaryParameterLoader::LoadAll<BinaryLayout_TitleMenuParameter>(
                    "Assets/master/TitleMenuParameter.bin");
                if (bin.empty()) { OutputDebugStringA("[WARN] ParameterLoader: .bin file not found or empty\n"); }
                for (const auto& b : bin)
                {
                    TitleMenuParameter* p = new TitleMenuParameter();
                    p->cursolPositionXA = b.cursolPositionXA;
                    p->cursolPositionXB = b.cursolPositionXB;
                    p->cursolPositionXC = b.cursolPositionXC;
                    p->cursolPositionXD = b.cursolPositionXD;
                    p->cursolPositionYA = b.cursolPositionYA;
                    p->cursolPositionYB = b.cursolPositionYB;
                    p->cursolPositionYC = b.cursolPositionYC;
                    p->cursolPositionYD = b.cursolPositionYD;
                    p->selectionScaleX = b.selectionScaleX;
                    p->selectionScaleY = b.selectionScaleY;
                    p->selectionScaleZ = b.selectionScaleZ;
                    p->defaultScaleX = b.defaultScaleX;
                    p->defaultScaleY = b.defaultScaleY;
                    p->defaultScaleZ = b.defaultScaleZ;
                    p->selectionColorX = b.selectionColorX;
                    p->selectionColorY = b.selectionColorY;
                    p->selectionColorZ = b.selectionColorZ;
                    p->defaultColorX = b.defaultColorX;
                    p->defaultColorY = b.defaultColorY;
                    p->defaultColorZ = b.defaultColorZ;
                    p->maxCursolIndex = b.maxCursolIndex;
                    mgr.RegisterBinary(*p);
                }
            }

            // MasterSoundOptionMenuParameter (SoundOptionMenuParameter.bin)
            {
                auto bin = BinaryParameterLoader::LoadAll<BinaryLayout_MasterSoundOptionMenuParameter>(
                    "Assets/master/SoundOptionMenuParameter.bin");
                if (bin.empty()) { OutputDebugStringA("[WARN] ParameterLoader: .bin file not found or empty\n"); }
                for (const auto& b : bin)
                {
                    MasterSoundOptionMenuParameter* p = new MasterSoundOptionMenuParameter();
                    p->knobXA = b.knobXA; p->knobXB = b.knobXB; p->knobXC = b.knobXC; p->knobXD = b.knobXD; p->knobXE = b.knobXE;
                    p->knobXF = b.knobXF; p->knobXG = b.knobXG; p->knobXH = b.knobXH; p->knobXI = b.knobXI; p->knobXJ = b.knobXJ; p->knobXK = b.knobXK;
                    p->gaugeBarXA = b.gaugeBarXA; p->gaugeBarXB = b.gaugeBarXB; p->gaugeBarXC = b.gaugeBarXC; p->gaugeBarXD = b.gaugeBarXD; p->gaugeBarXE = b.gaugeBarXE;
                    p->gaugeBarXF = b.gaugeBarXF; p->gaugeBarXG = b.gaugeBarXG; p->gaugeBarXH = b.gaugeBarXH; p->gaugeBarXI = b.gaugeBarXI; p->gaugeBarXJ = b.gaugeBarXJ; p->gaugeBarXK = b.gaugeBarXK;
                    p->gaugeBarYA = b.gaugeBarYA; p->gaugeBarYB = b.gaugeBarYB; p->gaugeBarYC = b.gaugeBarYC;
                    p->gaugeBarScaleXA = b.gaugeBarScaleXA; p->gaugeBarScaleXB = b.gaugeBarScaleXB; p->gaugeBarScaleXC = b.gaugeBarScaleXC;
                    p->gaugeBarScaleXD = b.gaugeBarScaleXD; p->gaugeBarScaleXE = b.gaugeBarScaleXE; p->gaugeBarScaleXF = b.gaugeBarScaleXF;
                    p->gaugeBarScaleXG = b.gaugeBarScaleXG; p->gaugeBarScaleXH = b.gaugeBarScaleXH; p->gaugeBarScaleXI = b.gaugeBarScaleXI;
                    p->gaugeBarScaleXJ = b.gaugeBarScaleXJ; p->gaugeBarScaleXK = b.gaugeBarScaleXK;
                    p->volumeStep = b.volumeStep; p->volumeMax = b.volumeMax; p->volumeMin = b.volumeMin;
                    p->volumeDefaultMaster = b.volumeDefaultMaster; p->volumeDefaultBgm = b.volumeDefaultBgm;
                    p->volumeDefaultSe = b.volumeDefaultSe; p->volumeDisplayMultiplier = b.volumeDisplayMultiplier;
                    p->digitScaleX = b.digitScaleX; p->digitScaleY = b.digitScaleY; p->digitScaleZ = b.digitScaleZ;
                    p->textScaleX = b.textScaleX; p->textScaleY = b.textScaleY; p->textScaleZ = b.textScaleZ;
                    p->defaultScaleX = b.defaultScaleX; p->defaultScaleY = b.defaultScaleY; p->defaultScaleZ = b.defaultScaleZ;
                    p->selectionColorX = b.selectionColorX; p->selectionColorY = b.selectionColorY; p->selectionColorZ = b.selectionColorZ;
                    p->defaultColorX = b.defaultColorX; p->defaultColorY = b.defaultColorY; p->defaultColorZ = b.defaultColorZ;
                    mgr.RegisterBinary(*p);
                }
            }

            // ReturnToTitleMenuParameter
            {
                auto bin = BinaryParameterLoader::LoadAll<BinaryLayout_ReturnToTitleMenuParameter>(
                    "Assets/master/ReturnToTitleMenuParameter.bin");
                if (bin.empty()) { OutputDebugStringA("[WARN] ParameterLoader: .bin file not found or empty\n"); }
                for (const auto& b : bin)
                {
                    ReturnToTitleMenuParameter* p = new ReturnToTitleMenuParameter();
                    p->cursolPositionXA = b.cursolPositionXA;
                    p->cursolPositionXB = b.cursolPositionXB;
                    p->cursolPositionYA = b.cursolPositionYA;
                    p->cursolPositionYB = b.cursolPositionYB;
                    p->selectionScaleX = b.selectionScaleX;
                    p->selectionScaleY = b.selectionScaleY;
                    p->selectionScaleZ = b.selectionScaleZ;
                    p->defaultScaleX = b.defaultScaleX;
                    p->defaultScaleY = b.defaultScaleY;
                    p->defaultScaleZ = b.defaultScaleZ;
                    p->selectionColorX = b.selectionColorX;
                    p->selectionColorY = b.selectionColorY;
                    p->selectionColorZ = b.selectionColorZ;
                    p->defaultColorX = b.defaultColorX;
                    p->defaultColorY = b.defaultColorY;
                    p->defaultColorZ = b.defaultColorZ;
                    p->maxCursolIndex = b.maxCursolIndex;
                    mgr.RegisterBinary(*p);
                }
            }

            // ResultMenuParameter
            {
                auto bin = BinaryParameterLoader::LoadAll<BinaryLayout_ResultMenuParameter>(
                    "Assets/master/ResultMenuParameter.bin");
                if (bin.empty()) { OutputDebugStringA("[WARN] ParameterLoader: .bin file not found or empty\n"); }
                for (const auto& b : bin)
                {
                    ResultMenuParameter* p = new ResultMenuParameter();
                    p->masterMaxTime = b.masterMaxTime;
                    p->nextStepDelay = b.nextStepDelay;
                    p->awardStepDelay = b.awardStepDelay;
                    p->exitBufferTime = b.exitBufferTime;
                    p->thresholdTimeRankS = b.thresholdTimeRankS;
                    p->thresholdTimeRankA = b.thresholdTimeRankA;
                    p->thresholdTimeRankB = b.thresholdTimeRankB;
                    p->thresholdTimeRankC = b.thresholdTimeRankC;
                    p->selectionScaleX = b.selectionScaleX;
                    p->selectionScaleY = b.selectionScaleY;
                    p->selectionScaleZ = b.selectionScaleZ;
                    p->defaultScaleX = b.defaultScaleX;
                    p->defaultScaleY = b.defaultScaleY;
                    p->defaultScaleZ = b.defaultScaleZ;
                    p->selectionColorX = b.selectionColorX;
                    p->selectionColorY = b.selectionColorY;
                    p->selectionColorZ = b.selectionColorZ;
                    p->defaultColorX = b.defaultColorX;
                    p->defaultColorY = b.defaultColorY;
                    p->defaultColorZ = b.defaultColorZ;
                    p->bonusTextColorX = b.bonusTextColorX;
                    p->bonusTextColorY = b.bonusTextColorY;
                    p->bonusTextColorZ = b.bonusTextColorZ;
                    p->bonusTextColorW = b.bonusTextColorW;
                    p->coinScoreWeight = b.coinScoreWeight;
                    p->timeBonusRankS = b.timeBonusRankS;
                    p->timeBonusRankA = b.timeBonusRankA;
                    p->timeBonusRankB = b.timeBonusRankB;
                    p->timeBonusRankC = b.timeBonusRankC;
                    mgr.RegisterBinary(*p);
                }
            }

            // MasterPauseMenuParameter
            {
                auto bin = BinaryParameterLoader::LoadAll<BinaryLayout_MasterPauseMenuParameter>(
                    "Assets/master/PauseMenuParameter.bin");
                if (bin.empty()) { OutputDebugStringA("[WARN] ParameterLoader: .bin file not found or empty\n"); }
                for (const auto& b : bin)
                {
                    MasterPauseMenuParameter* p = new MasterPauseMenuParameter();
                    p->cursolPositionXA = b.cursolPositionXA;
                    p->cursolPositionXB = b.cursolPositionXB;
                    p->cursolPositionYA = b.cursolPositionYA;
                    p->cursolPositionYB = b.cursolPositionYB;
                    p->selectionScaleX = b.selectionScaleX;
                    p->selectionScaleY = b.selectionScaleY;
                    p->selectionScaleZ = b.selectionScaleZ;
                    p->defaultScaleX = b.defaultScaleX;
                    p->defaultScaleY = b.defaultScaleY;
                    p->defaultScaleZ = b.defaultScaleZ;
                    p->selectionColorX = b.selectionColorX;
                    p->selectionColorY = b.selectionColorY;
                    p->selectionColorZ = b.selectionColorZ;
                    p->defaultColorX = b.defaultColorX;
                    p->defaultColorY = b.defaultColorY;
                    p->defaultColorZ = b.defaultColorZ;
                    p->maxCursolIndex = b.maxCursolIndex;
                    mgr.RegisterBinary(*p);
                }
            }

            // ManualMenuParameter
            {
                auto bin = BinaryParameterLoader::LoadAll<BinaryLayout_ManualMenuParameter>(
                    "Assets/master/ManualMenuParameter.bin");
                if (bin.empty()) { OutputDebugStringA("[WARN] ParameterLoader: .bin file not found or empty\n"); }
                for (const auto& b : bin)
                {
                    ManualMenuParameter* p = new ManualMenuParameter();
                    p->cursolPositionXA = b.cursolPositionXA;
                    p->cursolPositionXB = b.cursolPositionXB;
                    p->cursolPositionYA = b.cursolPositionYA;
                    p->cursolPositionYB = b.cursolPositionYB;
                    p->gaugePositionX = b.gaugePositionX;
                    p->gaugePositionY = b.gaugePositionY;
                    p->gaugePositionZ = b.gaugePositionZ;
                    p->gaugeInnerRadius = b.gaugeInnerRadius;
                    p->gaugeOuterRadius = b.gaugeOuterRadius;
                    p->gaugeScale = b.gaugeScale;
                    p->gaugeFillColorX = b.gaugeFillColorX;
                    p->gaugeFillColorY = b.gaugeFillColorY;
                    p->gaugeFillColorZ = b.gaugeFillColorZ;
                    p->gaugeFillColorW = b.gaugeFillColorW;
                    p->gaugeEmptyColorX = b.gaugeEmptyColorX;
                    p->gaugeEmptyColorY = b.gaugeEmptyColorY;
                    p->gaugeEmptyColorZ = b.gaugeEmptyColorZ;
                    p->gaugeEmptyColorW = b.gaugeEmptyColorW;
                    p->bIconPositionX = b.bIconPositionX;
                    p->bIconPositionY = b.bIconPositionY;
                    p->bIconPositionZ = b.bIconPositionZ;
                    p->bIconScaleX = b.bIconScaleX;
                    p->bIconScaleY = b.bIconScaleY;
                    p->bIconScaleZ = b.bIconScaleZ;
                    p->bIconGaugeSizeX = b.bIconGaugeSizeX;
                    p->bIconGaugeSizeY = b.bIconGaugeSizeY;
                    p->bIconButtonSizeX = b.bIconButtonSizeX;
                    p->bIconButtonSizeY = b.bIconButtonSizeY;
                    mgr.RegisterBinary(*p);
                }
            }

            // InGameUiParameter
            {
                auto bin = BinaryParameterLoader::LoadAll<BinaryLayout_InGameUiParameter>(
                    "Assets/master/InGameUiParameter.bin");
                if (bin.empty()) { OutputDebugStringA("[WARN] ParameterLoader: .bin file not found or empty\n"); }
                for (const auto& b : bin)
                {
                    InGameUiParameter* p = new InGameUiParameter();
                    p->bounceDuraction = b.bounceDuraction;
                    p->hurryUpTriggerTime = b.hurryUpTriggerTime;
                    p->colorHpMaxX = b.colorHpMaxX;
                    p->colorHpMaxY = b.colorHpMaxY;
                    p->colorHpMaxZ = b.colorHpMaxZ;
                    p->colorCautionX = b.colorCautionX;
                    p->colorCautionY = b.colorCautionY;
                    p->colorCautionZ = b.colorCautionZ;
                    p->colorDangerX = b.colorDangerX;
                    p->colorDangerY = b.colorDangerY;
                    p->colorDangerZ = b.colorDangerZ;
                    p->colorSafeX = b.colorSafeX;
                    p->colorSafeY = b.colorSafeY;
                    p->colorSafeZ = b.colorSafeZ;
                    p->colorHighscoreX = b.colorHighscoreX;
                    p->colorHighscoreY = b.colorHighscoreY;
                    p->colorHighscoreZ = b.colorHighscoreZ;
                    p->colorHighscoreW = b.colorHighscoreW;
                    p->colorDefaultX = b.colorDefaultX;
                    p->colorDefaultY = b.colorDefaultY;
                    p->colorDefaultZ = b.colorDefaultZ;
                    p->colorDefaultW = b.colorDefaultW;
                    p->alphaActive = b.alphaActive;
                    p->alphaInactive = b.alphaInactive;
                    p->lerpSpeed = b.lerpSpeed;
                    p->bounceUpLimit = b.bounceUpLimit;
                    p->bounceTotalDuration = b.bounceTotalDuration;
                    p->triggerPulse100 = b.triggerPulse100;
                    p->triggerPulse50 = b.triggerPulse50;
                    p->triggerPulse30 = b.triggerPulse30;
                    p->pulseUpDuration = b.pulseUpDuration;
                    p->pulseTotalDuration = b.pulseTotalDuration;
                    p->scaleDefaultX = b.scaleDefaultX;
                    p->scaleDefaultY = b.scaleDefaultY;
                    p->scaleDefaultZ = b.scaleDefaultZ;
                    p->maxHp = b.maxHp;
                    p->maxTime = b.maxTime;
                    p->thresholdLow = b.thresholdLow;
                    p->thresholdMid = b.thresholdMid;
                    p->thresholdHigh = b.thresholdHigh;
                    p->colorChangeCount = b.colorChangeCount;
                    mgr.RegisterBinary(*p);
                }
            }

            // GameOverMenuParameter
            {
                auto bin = BinaryParameterLoader::LoadAll<BinaryLayout_GameOverMenuParameter>(
                    "Assets/master/GameOverMenuParameter.bin");
                if (bin.empty()) { OutputDebugStringA("[WARN] ParameterLoader: .bin file not found or empty\n"); }
                for (const auto& b : bin)
                {
                    GameOverMenuParameter* p = new GameOverMenuParameter();
                    p->cursolPositionXA = b.cursolPositionXA;
                    p->cursolPositionXB = b.cursolPositionXB;
                    p->cursolPositionYA = b.cursolPositionYA;
                    p->cursolPositionYB = b.cursolPositionYB;
                    p->selectionScaleX = b.selectionScaleX;
                    p->selectionScaleY = b.selectionScaleY;
                    p->selectionScaleZ = b.selectionScaleZ;
                    p->defaultScaleX = b.defaultScaleX;
                    p->defaultScaleY = b.defaultScaleY;
                    p->defaultScaleZ = b.defaultScaleZ;
                    p->selectionColorX = b.selectionColorX;
                    p->selectionColorY = b.selectionColorY;
                    p->selectionColorZ = b.selectionColorZ;
                    p->defaultColorX = b.defaultColorX;
                    p->defaultColorY = b.defaultColorY;
                    p->defaultColorZ = b.defaultColorZ;
                    p->maxCursolIndex = b.maxCursolIndex;
                    mgr.RegisterBinary(*p);
                }
            }

            // AwardMenuParameter
            {
                auto bin = BinaryParameterLoader::LoadAll<BinaryLayout_AwardMenuParameter>(
                    "Assets/master/AwardMenuParameter.bin");
                if (bin.empty()) { OutputDebugStringA("[WARN] ParameterLoader: .bin file not found or empty\n"); }
                for (const auto& b : bin)
                {
                    AwardMenuParameter* p = new AwardMenuParameter();
                    p->barCursolPositionYA = b.barCursolPositionYA;
                    p->barCursolPositionYB = b.barCursolPositionYB;
                    p->barCursolPositionYC = b.barCursolPositionYC;
                    p->barCursolPositionYD = b.barCursolPositionYD;
                    p->panelCursorPosX_Left = b.panelCursorPosX_Left;
                    p->panelCursorPosX_Right = b.panelCursorPosX_Right;
                    p->panelCursorPosY_Top = b.panelCursorPosY_Top;
                    p->panelCursorPosY_Bottom = b.panelCursorPosY_Bottom;
                    p->fastTimeLimit = b.fastTimeLimit;
                    p->slowTimethreshold = b.slowTimethreshold;
                    p->displayRowCount = b.displayRowCount;
                    p->maxScrollVal = b.maxScrollVal;
                    p->selectionScaleX = b.selectionScaleX;
                    p->selectionScaleY = b.selectionScaleY;
                    p->selectionScaleZ = b.selectionScaleZ;
                    p->defaultScaleX = b.defaultScaleX;
                    p->defaultScaleY = b.defaultScaleY;
                    p->defaultScaleZ = b.defaultScaleZ;
                    p->selectionColorX = b.selectionColorX;
                    p->selectionColorY = b.selectionColorY;
                    p->selectionColorZ = b.selectionColorZ;
                    p->defaultColorX = b.defaultColorX;
                    p->defaultColorY = b.defaultColorY;
                    p->defaultColorZ = b.defaultColorZ;
                    p->lowHpThreshold = b.lowHpThreshold;
                    p->maxHpThreshold = b.maxHpThreshold;
                    p->jumpCountFrog = b.jumpCountFrog;
                    p->jumpCountRabbit = b.jumpCountRabbit;
                    p->maxRows = b.maxRows;
                    p->maxCols = b.maxCols;
                    p->titleYUp = b.titleYUp;
                    p->titleYDown = b.titleYDown;
                    p->condYUp = b.condYUp;
                    p->condYDown = b.condYDown;
                    mgr.RegisterBinary(*p);
                }
            }
        }

        void ParameterLoader::UnloadAll()
        {
            auto& mgr = ParameterManager::Get();
            mgr.UnloadParameter<MasterBattleParameter>();
            mgr.UnloadParameter<MasterBattleCameraParameter>();
            mgr.UnloadParameter<MasterBattleCharacterParameter>();
            mgr.UnloadParameter<MasterEventCharacterParameter>();
            mgr.UnloadParameter<MasterStageParameter>();
            mgr.UnloadParameter<MasterEventCharacterAIParameter>();
            mgr.UnloadParameter<MasterSceneParameter>();
            mgr.UnloadParameter<AwardMenuParameter>();
            mgr.UnloadParameter<BattleSequenceParameter>();
            mgr.UnloadParameter<GameOverMenuParameter>();
            mgr.UnloadParameter<InGameUiParameter>();
        }
    }
}