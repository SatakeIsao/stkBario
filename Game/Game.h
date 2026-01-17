#pragma once


#include "LevelRender.h"

class Game : public IGameObject
{
public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);



private:
};

