/**
 * Types.h
 * サウンド用の定数など必要な情報を定義するファイル
 */
#pragma once
#include <string>

namespace app
{

	/** サウンドの種類 */
	enum class SoundKind : uint32_t
	{
		SE = 0,
		Button = SE,
		Coin,
		Warp,
		Pop,
		SlimeknockBack,
		PlayerPunch,
		Jump,
		BIconAmim,
		SlimeAnim,
		SEMax,
		//BGM
		BGM = SEMax,
		Game = BGM,
		HurryUp,
		Title,
		BGMMax,
		//Voice,
		Voice = BGMMax,
		Startup00 = Voice,
		Startup01,
		Ready,
		Go,
		GameClear,
		GameOver,
		TimeUp,
		VoiceMax,
		Max = VoiceMax,
		None = Max,
	};


	/** サウンドの情報の構造体 */
	struct SoundInformation
	{
		std::string assetPath;
		//
		SoundInformation(const std::string& path) : assetPath(path) {}
	};


	/** 情報を保持 */
	static SoundInformation soundInformation[static_cast<uint32_t>(SoundKind::Max)] =
	{
		// SE
		SoundInformation("Assets/sound/se/button.wav"),
		SoundInformation("Assets/sound/se/coin.wav"),
		SoundInformation("Assets/sound/se/warp.wav"),
		SoundInformation("Assets/sound/se/pop.wav"),
		SoundInformation("Assets/sound/se/slimeknockBack.wav"),
		SoundInformation("Assets/sound/se/playerPunch.wav"),
		SoundInformation("Assets/sound/se/jump.wav"),
		SoundInformation("Assets/sound/se/bIconAnim.wav"),
		SoundInformation("Assets/sound/se/slimeAnim.wav"),
		// BGM
		SoundInformation("Assets/sound/bgm/inGame_1.0xSpeed.wav"),
		SoundInformation("Assets/sound/bgm/inGame_1.5xSpeed.wav"),
		SoundInformation("Assets/sound/bgm/title.wav"),
		// Voice
		SoundInformation("Assets/sound/voice/StartupVoice_00.wav"),
		SoundInformation("Assets/sound/voice/StartupVoice_01.wav"),
		SoundInformation("Assets/sound/voice/ReadyVoice.wav"),
		SoundInformation("Assets/sound/voice/GoVoice.wav"),
		SoundInformation("Assets/sound/voice/GameClearVoice.wav"),
		SoundInformation("Assets/sound/voice/GameOverVoice.wav"),
		SoundInformation("Assets/sound/voice/TimeUpVoice.wav")
	};


}