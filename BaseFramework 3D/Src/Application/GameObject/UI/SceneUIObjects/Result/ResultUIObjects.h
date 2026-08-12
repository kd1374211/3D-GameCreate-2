#pragma once

class ResultUIObject :public KdGameObject
{
public:

	ResultUIObject() { Init(); }
	~ResultUIObject()override {}

	void DrawSprite()override;

private:

	void Init()override;

	//仮画像
	std::shared_ptr<KdTexture> m_resultTex = nullptr;

};