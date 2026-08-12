#pragma once

class TitleUIObject :public KdGameObject
{
public:

	TitleUIObject() { Init(); }
	~TitleUIObject()override {}

	void DrawSprite()override;

private:

	void Init()override;

	//仮画像
	std::shared_ptr<KdTexture> m_titleTex = nullptr;

};