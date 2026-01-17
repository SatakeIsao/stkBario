#include "stdafx.h"
#include "Game.h"


#include "actor/BattleCharacter.h"
#include "actor/EventCharacter.h"
#include "actor/ActorState.h"

#include "actor/CharacterSteering.h"


namespace
{
	// @todo for test
	static BattleCharacter* battleCharacter = nullptr;
	static EventCharacter* eventCharacter = nullptr;

	static CharacterSteering* characterSteering = nullptr;
}


Game::Game()
{
}

Game::~Game()
{
}

bool Game::Start()
{
	characterSteering = new CharacterSteering();

	// ƒ}ƒŠƒI‚É‚µ‚Ä‚Ý‚½
	{
		battleCharacter = new BattleCharacter();
		battleCharacter->Initialize("Assets/ModelData/player/player.tkm");
		{
			battleCharacter->AddState<IdleCharacterState>();
			battleCharacter->AddState<RunCharacterState>();
			battleCharacter->AddState<JumpCharacterState>();
		}
	}

	characterSteering->Initialize(battleCharacter, 0);


	eventCharacter = new EventCharacter();
	eventCharacter->Initialize("Assets/ModelData/enemy/slime/slime.tkm");


	return true;
}

void Game::Update()
{
	battleCharacter->Update();

	eventCharacter->Update();

	characterSteering->Update();
}

void Game::Render(RenderContext& rc)
{
	battleCharacter->Render(rc);

	eventCharacter->Render(rc);
}


