#pragma once


class SceneManagerObject;
class FadeObject;


class Game : public IGameObject
{
private:
	SceneManagerObject* m_sceneManager = nullptr;
	FadeObject* m_fadeObject = nullptr;


public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
};

