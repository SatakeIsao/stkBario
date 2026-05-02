#include "stdafx.h"
#include "BinaryParameterTest.h"
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <cassert>
#include <sstream>

namespace BinaryParameterTest
{

    // ============================================================
    // OutputDebugString ラッパー（printf の代わり）
    // ============================================================
    static void DebugPrint(const char* fmt, ...)
    {
        char buf[512];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        OutputDebugStringA(buf);
    }


    // ============================================================
    // バイナリレイアウト構造体（フィールドのみ・IParameter非継承）
    // ============================================================
#pragma pack(push, 1)

    struct Layout_Battle
    {
        float battleTime;
    };
    static_assert(sizeof(Layout_Battle) == 4, "Layout_Battle size mismatch");

    struct Layout_Camera
    {
        float distance, height, fov, nearClip, farClip, rotationX, rotationY;
    };
    static_assert(sizeof(Layout_Camera) == 28, "Layout_Camera size mismatch");

    struct Layout_BattleChara
    {
        float moveSpeed, jumpMoveSpeed, jumpPower, radius, height;
        float ghostbodyPosYOffset, collisionRadiusOffset, collisionHeightOffset;
    };
    static_assert(sizeof(Layout_BattleChara) == 32, "Layout_BattleChara size mismatch");

    struct Layout_EventChara
    {
        float moveSpeed, jumpMoveSpeed, jumpPower, radius, height;
        float ghostbodyPosYOffset, fallDeathThresholdPosY;
    };
    static_assert(sizeof(Layout_EventChara) == 28, "Layout_EventChara size mismatch");

    struct Layout_Stage
    {
        float gravity, fallLimitY, friction;
        float warpStartScale, warpEndScale, warpTime;
        float coinShrinkSpeed, coinRotationSpeed, coinJumpInitVelocityY;
        float coinCollisionRadius, coinCollisionHeight;
    };
    static_assert(sizeof(Layout_Stage) == 44, "Layout_Stage size mismatch");

    struct Layout_EventCharaAI
    {
        float knockbackJumpPower, chaseDetectionRange, chaseFieldOfViewDeg;
        float attackRange, chaseRange, chaseAITimerInitial;
        float patrolLeftTurnTime, patrolRightTurnTime, waitTime;
    };
    static_assert(sizeof(Layout_EventCharaAI) == 36, "Layout_EventCharaAI size mismatch");

#pragma pack(pop)


    // ============================================================
    // 汎用バイナリ読み込み
    // ============================================================
    template <typename T>
    std::vector<T> Load(const char* path)
    {
        FILE* fp = fopen(path, "rb");
        if (!fp)
        {
            DebugPrint("[BinTest][ERROR] 開けません: %s\n", path);
            return {};
        }
        int count = 0;
        fread(&count, sizeof(int), 1, fp);
        std::vector<T> records(count);
        fread(records.data(), sizeof(T), count, fp);
        fclose(fp);
        return records;
    }


    // ============================================================
    // テストヘルパー
    // ============================================================
    static bool s_anyFailed = false;

    static void CheckFloat(const char* label, float actual, float expected)
    {
        float diff = actual - expected;
        bool ok = (diff > -1e-3f) && (diff < 1e-3f);
        DebugPrint("  %-38s : %9.4f  %s\n", label, actual, ok ? "OK" : "NG");
        if (!ok) { s_anyFailed = true; }
    }


    // ============================================================
    // 各パラメーターテスト
    // ============================================================
    static bool TestBattle(const std::string& dir)
    {
        auto v = Load<Layout_Battle>((dir + "\\MasterBattleParameter.bin").c_str());
        if (v.empty()) return false;
        DebugPrint("\n[MasterBattleParameter]\n");
        s_anyFailed = false;
        CheckFloat("battleTime", v[0].battleTime, 300.0f);
        return !s_anyFailed;
    }

    static bool TestCamera(const std::string& dir)
    {
        auto v = Load<Layout_Camera>((dir + "\\MasterBattleCameraParameter.bin").c_str());
        if (v.empty()) return false;
        DebugPrint("\n[MasterBattleCameraParameter]\n");
        s_anyFailed = false;
        CheckFloat("distance", v[0].distance, -200.0f);
        CheckFloat("height", v[0].height, 100.0f);
        CheckFloat("fov", v[0].fov, 60.0f);
        CheckFloat("nearClip", v[0].nearClip, 0.1f);
        CheckFloat("farClip", v[0].farClip, 10000.0f);
        CheckFloat("rotationX", v[0].rotationX, -0.05f);
        CheckFloat("rotationY", v[0].rotationY, 0.05f);
        return !s_anyFailed;
    }

    static bool TestBattleChara(const std::string& dir)
    {
        auto v = Load<Layout_BattleChara>((dir + "\\MasterBattleCharacterParameter.bin").c_str());
        if (v.empty()) return false;
        DebugPrint("\n[MasterBattleCharacterParameter]\n");
        s_anyFailed = false;
        CheckFloat("moveSpeed", v[0].moveSpeed, 350.0f);
        CheckFloat("jumpMoveSpeed", v[0].jumpMoveSpeed, 300.0f);
        CheckFloat("jumpPower", v[0].jumpPower, 370.0f);
        CheckFloat("radius", v[0].radius, 15.0f);
        CheckFloat("height", v[0].height, 30.0f);
        CheckFloat("ghostbodyPosYOffset", v[0].ghostbodyPosYOffset, 0.0f);
        CheckFloat("collisionRadiusOffset", v[0].collisionRadiusOffset, 0.0f);
        CheckFloat("collisionHeightOffset", v[0].collisionHeightOffset, 0.0f);
        return !s_anyFailed;
    }

    static bool TestEventChara(const std::string& dir)
    {
        auto v = Load<Layout_EventChara>((dir + "\\MasterEventCharacterParameter.bin").c_str());
        if (v.empty()) return false;
        DebugPrint("\n[MasterEventCharacterParameter]\n");
        s_anyFailed = false;
        CheckFloat("moveSpeed", v[0].moveSpeed, 100.0f);
        CheckFloat("jumpMoveSpeed", v[0].jumpMoveSpeed, 20.0f);
        CheckFloat("jumpPower", v[0].jumpPower, 30.0f);
        CheckFloat("radius", v[0].radius, 15.0f);
        CheckFloat("height", v[0].height, 1.0f);
        CheckFloat("ghostbodyPosYOffset", v[0].ghostbodyPosYOffset, 0.0f);
        CheckFloat("fallDeathThresholdPosY", v[0].fallDeathThresholdPosY, 0.0f);
        return !s_anyFailed;
    }

    static bool TestStage(const std::string& dir)
    {
        auto v = Load<Layout_Stage>((dir + "\\MasterStageParameter.bin").c_str());
        if (v.empty()) return false;
        DebugPrint("\n[MasterStageParameter]\n");
        s_anyFailed = false;
        CheckFloat("gravity", v[0].gravity, -650.0f);
        CheckFloat("fallLimitY", v[0].fallLimitY, -10.0f);
        CheckFloat("friction", v[0].friction, 0.7f);
        CheckFloat("warpStartScale", v[0].warpStartScale, 1.0f);
        CheckFloat("warpEndScale", v[0].warpEndScale, 0.3f);
        CheckFloat("warpTime", v[0].warpTime, 1.0f);
        CheckFloat("coinShrinkSpeed", v[0].coinShrinkSpeed, 1.0f);
        CheckFloat("coinRotationSpeed", v[0].coinRotationSpeed, 1.0f);
        CheckFloat("coinJumpInitVelocityY", v[0].coinJumpInitVelocityY, 0.0f);
        CheckFloat("coinCollisionRadius", v[0].coinCollisionRadius, 0.0f);
        CheckFloat("coinCollisionHeight", v[0].coinCollisionHeight, 0.0f);
        return !s_anyFailed;
    }

    static bool TestEventCharaAI(const std::string& dir)
    {
        auto v = Load<Layout_EventCharaAI>((dir + "\\MasterEventCharacterAIParameter.bin").c_str());
        if (v.empty()) return false;
        DebugPrint("\n[MasterEventCharacterAIParameter]\n");
        s_anyFailed = false;
        CheckFloat("knockbackJumpPower", v[0].knockbackJumpPower, 80.0f);
        CheckFloat("chaseDetectionRange", v[0].chaseDetectionRange, 200.0f);
        CheckFloat("chaseFieldOfViewDeg", v[0].chaseFieldOfViewDeg, 60.0f);
        CheckFloat("attackRange", v[0].attackRange, 40.0f);
        CheckFloat("chaseRange", v[0].chaseRange, 200.0f);
        CheckFloat("chaseAITimerInitial", v[0].chaseAITimerInitial, 5.0f);
        CheckFloat("patrolLeftTurnTime", v[0].patrolLeftTurnTime, 2.0f);
        CheckFloat("patrolRightTurnTime", v[0].patrolRightTurnTime, 4.0f);
        CheckFloat("waitTime", v[0].waitTime, 1.0f);
        return !s_anyFailed;
    }


    // ============================================================
    // Run
    // ============================================================
    void Run(const char* binDir)
    {
        std::string dir(binDir);

        DebugPrint("\n================================================\n");
        DebugPrint("  [BinTest] Binary Parameter Load Test \n");
        DebugPrint("  dir: %s\n", binDir);
        DebugPrint("================================================\n");

        int passed = 0, failed = 0;

        auto run = [&](bool(*fn)(const std::string&))
            {
                bool ok = fn(dir);
                DebugPrint("  >>> %s\n", ok ? "PASS" : "FAIL");
                ok ? passed++ : failed++;
            };

        run(TestBattle);
        run(TestCamera);
        run(TestBattleChara);
        run(TestEventChara);
        run(TestStage);
        run(TestEventCharaAI);

        DebugPrint("\n================================================\n");
        DebugPrint("  [BinTest] Result: %d PASS / %d FAIL\n", passed, failed);
        DebugPrint("================================================\n\n");
    }

} // namespace BinaryParameterTest