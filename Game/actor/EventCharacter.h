/**
 * EventCharacterファイル
 */
#pragma once
#include "Actor.h"


class EventCharacter : public Character
{
	using SuperClass = Character;


public:
	EventCharacter();
	~EventCharacter();

	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;


public:
	void Initialize(const CharacterInitializeParameter& param) override final;
};