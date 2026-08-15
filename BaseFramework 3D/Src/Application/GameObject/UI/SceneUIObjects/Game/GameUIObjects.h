#pragma once

class GameUIObjects :public KdGameObject
{
public:

	GameUIObjects() { Init(); }
	~GameUIObjects()override {}

	void Update()override;
	void DrawSprite()override;

	void SetTimer(int time) { m_time = time; }

private:

	void Init()override;

	//タイマー画像
	std::shared_ptr<KdTexture> m_timerTex = nullptr;

	//時間
	int m_time;

};