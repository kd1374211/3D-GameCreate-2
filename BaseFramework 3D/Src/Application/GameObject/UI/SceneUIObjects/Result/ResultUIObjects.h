#pragma once

class ResultUIObject :public KdGameObject
{
public:

	ResultUIObject() { Init(); }
	~ResultUIObject()override {}

	void DrawSprite()override;

private:

	void Init()override;

};