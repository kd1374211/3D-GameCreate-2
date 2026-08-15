#include "GameUIObjects.h"

void GameUIObjects::Update()
{}

void GameUIObjects::DrawSprite()
{
	//仮に99は超えないようにする
	int drawTime = std::clamp(m_time, 0, 99);

	//位分解
	int digitTen = drawTime / 10;
	int digitOne = drawTime % 10;

	//10の位
	Math::Rectangle rec = Math::Rectangle(0, (long)(12 * digitTen), 12, 12);
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_timerTex, -42, 318, 84, 84, &rec);

	//1の位
	rec = Math::Rectangle(0, (long)(12 * digitOne), 12, 12);
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_timerTex, 42, 318, 84, 84, &rec);
}

void GameUIObjects::Init()
{
	m_timerTex = std::make_shared<KdTexture>();
	m_timerTex->Load("Asset/Textures/UI/SceneUI/Game/Number.png");
}
