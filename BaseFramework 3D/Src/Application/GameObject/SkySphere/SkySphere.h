#pragma once

class SkySphere :public KdGameObject
{
public:

	SkySphere(std::string modelPath);
	~SkySphere()override {}

	void DrawUnLit()override;

private:

	//モデル
	std::shared_ptr<KdModelWork> m_model = nullptr;
};