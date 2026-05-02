"""
convert_parameters.py
ゲームパラメーターJSONをバイナリファイルに変換するコンバーター

【型の自動判定】
JSONの値が float（小数点あり）→ 'f'（float32）
JSONの値が int （小数点なし）→ 'i'（int32）
PARAMETER_DEFSの書式文字列は不要になったため "auto" と記載する。
"""

import json
import struct
import sys
import os


PARAMETER_DEFS = [
    (
        "MasterBattleParameter",
        ["battleTime"],
    ),
    (
        "MasterBattleCameraParameter",
        ["distance", "height", "fov", "nearClip", "farClip", "rotationX", "rotationY"],
    ),
    (
        "MasterBattleCharacterParameter",
        ["moveSpeed", "jumpMoveSpeed", "jumpPower", "radius", "height",
         "ghostbodyPosYOffset", "collisionRadiusOffset", "collisionHeightOffset"],
    ),
    (
        "MasterEventCharacterParameter",
        ["moveSpeed", "jumpMoveSpeed", "jumpPower", "radius", "height",
         "ghostbodyPosYOffset", "fallDeathThresholdPosY"],
    ),
    (
        "MasterStageParameter",
        ["gravity", "fallLimitY", "friction",
         "warpStartScale", "warpEndScale", "warpTime",
         "coinShrinkSpeed", "coinRotationSpeed", "coinJumpInitVelocityY",
         "coinCollisionRadius", "coinCollisionHeight",
         "coinYOffset", "coinEffectYOffset",
         "spawnYOffset",
         "goalEffectRiseSpeed", "goalEffectRiseHeight", "goalEffectCoolTime",
         "goalEffectScaleX", "goalEffectScaleY", "goalEffectScaleZ",
         "goalTriggerDistance",
         "blinkTimeThreshold",
         "separatorTime100Start", "separatorTime100End",
         "separatorTime50Start", "separatorTime50End"],
    ),
    (
        "MasterEventCharacterAIParameter",
        ["knockbackJumpPower", "chaseDetectionRange", "chaseFieldOfViewDeg",
         "attackRange", "chaseRange", "chaseAITimerInitial",
         "patrolLeftTurnTime", "patrolRightTurnTime", "waitTime"],
    ),
    (
        "MasterSceneParameter",
        ["sceneTransitionWaitTime", "sceneRequestWaitTime",
         "bButtonHoldThreshold",
         "titleMenuIndexStart", "titleMenuIndexManual",
         "titleMenuIndexAward", "titleMenuIndexExit"],
    ),
    (
        "GameOverMenuParameter",
        ["cursolPositionXA", "cursolPositionXB",
         "cursolPositionYA", "cursolPositionYB",
         "selectionScaleX", "selectionScaleY", "selectionScaleZ",
         "defaultScaleX", "defaultScaleY", "defaultScaleZ",
         "selectionColorX", "selectionColorY", "selectionColorZ",
         "defaultColorX", "defaultColorY", "defaultColorZ",
         "maxCursolIndex"],
    ),
    (
        "InGameUiParameter",
        ["bounceDuraction", "hurryUpTriggerTime",
         "colorHpMaxX", "colorHpMaxY", "colorHpMaxZ",
         "colorCautionX", "colorCautionY", "colorCautionZ",
         "colorDangerX", "colorDangerY", "colorDangerZ",
         "colorSafeX", "colorSafeY", "colorSafeZ",
         "colorHighscoreX", "colorHighscoreY", "colorHighscoreZ", "colorHighscoreW",
         "colorDefaultX", "colorDefaultY", "colorDefaultZ", "colorDefaultW",
         "alphaActive", "alphaInactive", "lerpSpeed",
         "bounceUpLimit", "bounceTotalDuration",
         "triggerPulse100", "triggerPulse50", "triggerPulse30",
         "pulseUpDuration", "pulseTotalDuration",
         "scaleDefaultX", "scaleDefaultY", "scaleDefaultZ",
         "maxHp", "maxTime",
         "thresholdLow", "thresholdMid", "thresholdHigh",
         "colorChangeCount"],
    ),
    (
        "ManualMenuParameter",
        ["cursolPositionXA", "cursolPositionXB",
         "cursolPositionYA", "cursolPositionYB",
         "gaugePositionX", "gaugePositionY", "gaugePositionZ",
         "gaugeInnerRadius", "gaugeOuterRadius", "gaugeScale",
         "gaugeFillColorX", "gaugeFillColorY", "gaugeFillColorZ", "gaugeFillColorW",
         "gaugeEmptyColorX", "gaugeEmptyColorY", "gaugeEmptyColorZ", "gaugeEmptyColorW",
         "bIconPositionX", "bIconPositionY", "bIconPositionZ",
         "bIconScaleX", "bIconScaleY", "bIconScaleZ",
         "bIconGaugeSizeX", "bIconGaugeSizeY",
         "bIconButtonSizeX", "bIconButtonSizeY"],
    ),
    (
        "PauseMenuParameter",
        ["cursolPositionXA", "cursolPositionXB",
         "cursolPositionYA", "cursolPositionYB",
         "selectionScaleX", "selectionScaleY", "selectionScaleZ",
         "defaultScaleX", "defaultScaleY", "defaultScaleZ",
         "selectionColorX", "selectionColorY", "selectionColorZ",
         "defaultColorX", "defaultColorY", "defaultColorZ",
         "maxCursolIndex"],
    ),
    (
        "ResultMenuParameter",
        ["masterMaxTime", "nextStepDelay", "awardStepDelay", "exitBufferTime",
         "thresholdTimeRankS", "thresholdTimeRankA", "thresholdTimeRankB", "thresholdTimeRankC",
         "selectionScaleX", "selectionScaleY", "selectionScaleZ",
         "defaultScaleX", "defaultScaleY", "defaultScaleZ",
         "selectionColorX", "selectionColorY", "selectionColorZ",
         "defaultColorX", "defaultColorY", "defaultColorZ",
         "bonusTextColorX", "bonusTextColorY", "bonusTextColorZ", "bonusTextColorW",
         "coinScoreWeight", "timeBonusRankS", "timeBonusRankA", "timeBonusRankB", "timeBonusRankC"],
    ),
    (
        "ReturnToTitleMenuParameter",
        ["cursolPositionXA", "cursolPositionXB",
         "cursolPositionYA", "cursolPositionYB",
         "selectionScaleX", "selectionScaleY", "selectionScaleZ",
         "defaultScaleX", "defaultScaleY", "defaultScaleZ",
         "selectionColorX", "selectionColorY", "selectionColorZ",
         "defaultColorX", "defaultColorY", "defaultColorZ",
         "maxCursolIndex"],
    ),
    (
        "SoundOptionMenuParameter",
        ["knobXA","knobXB","knobXC","knobXD","knobXE","knobXF","knobXG","knobXH","knobXI","knobXJ","knobXK",
         "gaugeBarXA","gaugeBarXB","gaugeBarXC","gaugeBarXD","gaugeBarXE","gaugeBarXF","gaugeBarXG","gaugeBarXH","gaugeBarXI","gaugeBarXJ","gaugeBarXK",
         "gaugeBarYA","gaugeBarYB","gaugeBarYC",
         "gaugeBarScaleXA","gaugeBarScaleXB","gaugeBarScaleXC","gaugeBarScaleXD","gaugeBarScaleXE",
         "gaugeBarScaleXF","gaugeBarScaleXG","gaugeBarScaleXH","gaugeBarScaleXI","gaugeBarScaleXJ","gaugeBarScaleXK",
         "volumeStep","volumeMax","volumeMin",
         "volumeDefaultMaster","volumeDefaultBgm","volumeDefaultSe","volumeDisplayMultiplier",
         "digitScaleX","digitScaleY","digitScaleZ",
         "textScaleX","textScaleY","textScaleZ",
         "defaultScaleX","defaultScaleY","defaultScaleZ",
         "selectionColorX","selectionColorY","selectionColorZ",
         "defaultColorX","defaultColorY","defaultColorZ"],
    ),
    (
        "TitleMenuParameter",
        ["cursolPositionXA", "cursolPositionXB", "cursolPositionXC", "cursolPositionXD",
         "cursolPositionYA", "cursolPositionYB", "cursolPositionYC", "cursolPositionYD",
         "selectionScaleX", "selectionScaleY", "selectionScaleZ",
         "defaultScaleX", "defaultScaleY", "defaultScaleZ",
         "selectionColorX", "selectionColorY", "selectionColorZ",
         "defaultColorX", "defaultColorY", "defaultColorZ",
         "maxCursolIndex"],
    ),
    (
        "AwardMenuParameter",
        ["barCursolPositionYA", "barCursolPositionYB", "barCursolPositionYC", "barCursolPositionYD",
         "panelCursorPosX_Left", "panelCursorPosX_Right", "panelCursorPosY_Top", "panelCursorPosY_Bottom",
         "fastTimeLimit", "slowTimethreshold",
         "displayRowCount", "maxScrollVal",
         "selectionScaleX", "selectionScaleY", "selectionScaleZ",
         "defaultScaleX", "defaultScaleY", "defaultScaleZ",
         "selectionColorX", "selectionColorY", "selectionColorZ",
         "defaultColorX", "defaultColorY", "defaultColorZ",
         "lowHpThreshold", "maxHpThreshold",
         "jumpCountFrog", "jumpCountRabbit",
         "maxRows", "maxCols",
         "titleYUp", "titleYDown",
         "condYUp", "condYDown"],
    ),
    (
        "BattleSequenceParameter",
        ["delayWaitTime", "readyDisplayTime", "goDisplayTime", "goFadeStartTime",
         "gameoverFirstDown", "gameoverFirstUp", "gameoverSecondDown", "gameoverSecondUp", "gameoverThirdDown",
         "gameclearPopUpTime", "gameclearShrinkTime", "gameclearWaitTime",
         "timeupSlideInTime", "timeupSlideBackTime", "timeupWaitTime",
         "stageMaxSlimes", "stageMaxCoins"],
    ),
]


# -----------------------------------------------------------------------
# JSONの値からバイナリ書式文字を自動判定
# float（小数点あり） → 'f'
# int （小数点なし）  → 'i'
# -----------------------------------------------------------------------
def infer_type(value) -> str:
    if isinstance(value, float):
        return 'f'
    if isinstance(value, int):
        return 'i'
    try:
        if '.' in str(value):
            return 'f'
        int(value)
        return 'i'
    except ValueError:
        return 'f'


def build_fmt(fields, sample_record) -> str:
    fmt = '<'
    for field in fields:
        fmt += infer_type(sample_record.get(field, 0.0))
    return fmt


# -----------------------------------------------------------------------
# 1レコードをバイト列に変換
# -----------------------------------------------------------------------
def pack_record(auto_fmt, fields, data, index):
    values = []
    type_chars = [c for c in auto_fmt if c in 'fid']
    for i, field in enumerate(fields):
        if field not in data:
            print(f"[エラー] index={index} フィールド '{field}' が見つかりません")
            sys.exit(1)
        t = type_chars[i] if i < len(type_chars) else 'f'
        values.append(int(data[field]) if t == 'i' else float(data[field]))
    return struct.pack(auto_fmt, *values)


# -----------------------------------------------------------------------
# JSON → バイナリ変換
# -----------------------------------------------------------------------
def convert(json_path, bin_path, fields):
    try:
        with open(json_path, "r", encoding="utf-8") as f:
            records = json.load(f)
    except FileNotFoundError:
        print(f"[エラー] {json_path} が見つかりません")
        sys.exit(1)
    except json.JSONDecodeError as e:
        print(f"[エラー] JSONの解析に失敗しました: {e}")
        sys.exit(1)

    auto_fmt = build_fmt(fields, records[0])
    record_size = struct.calcsize(auto_fmt)
    count = len(records)

    with open(bin_path, "wb") as f:
        f.write(struct.pack("<i", count))
        for i, record in enumerate(records):
            f.write(pack_record(auto_fmt, fields, record, i))

    total = 4 + count * record_size
    print(f"[完了] {os.path.basename(bin_path):48s} {count}件  {total:3d} bytes")


# -----------------------------------------------------------------------
# 変換結果の検証（読み返して内容を表示）
# -----------------------------------------------------------------------
def verify(bin_path, auto_fmt, fields):
    record_size = struct.calcsize(auto_fmt)
    with open(bin_path, "rb") as f:
        count = struct.unpack("<i", f.read(4))[0]
        print(f"  レコード数: {count}")
        for i in range(count):
            raw = f.read(record_size)
            values = struct.unpack(auto_fmt, raw)
            pairs = ", ".join(f"{k}={v}" for k, v in zip(fields, values))
            print(f"  [{i}] {pairs}")


# -----------------------------------------------------------------------
# エントリーポイント
# -----------------------------------------------------------------------
def main():
    json_dir = "."
    bin_dir  = "bin"
    os.makedirs(bin_dir, exist_ok=True)

    print("=" * 65)
    print(" JSON → バイナリ 変換開始")
    print("=" * 65)

    for name, fields in PARAMETER_DEFS:
        json_path = os.path.join(json_dir, f"{name}.json")
        bin_path  = os.path.join(bin_dir,  f"{name}.bin")
        convert(json_path, bin_path, fields)

    print()
    print("=" * 65)
    print(" 検証（読み返し確認）")
    print("=" * 65)

    for name, fields in PARAMETER_DEFS:
        json_path = os.path.join(json_dir, f"{name}.json")
        bin_path  = os.path.join(bin_dir,  f"{name}.bin")
        print(f"\n● {name}")
        try:
            with open(json_path, "r", encoding="utf-8") as jf:
                sample = json.load(jf)
            auto_fmt = build_fmt(fields, sample[0])
            verify(bin_path, auto_fmt, fields)
        except Exception as e:
            print(f"  検証スキップ: {e}")

    print("\n変換完了")


if __name__ == "__main__":
    main()