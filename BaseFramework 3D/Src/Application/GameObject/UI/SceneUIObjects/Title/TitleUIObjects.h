#pragma once

class TitleUIObject :public KdGameObject
{
public:

	TitleUIObject() { Init(); }
	~TitleUIObject()override {}

	void Update()override;
	void DrawSprite()override;

private:

	void Init()override;

	//タイトル画像
	std::shared_ptr<KdTexture> m_titleLogoTex = nullptr;

	//スタート文字
	std::shared_ptr<KdTexture> m_startTex = nullptr;

	//透明度
	float m_startAlpha = 1.0f;
	float m_startAlphaChangeMulti = -1.0f;
};