#pragma once

class SkySphere :public KdGameObject
{
public:

	SkySphere(std::string modelPath);
	~SkySphere()override {}

	void Update()override;
	void DrawUnLit()override;

private:

	struct SkySphereConsts
	{
		// 回転速度
		static constexpr float RotationSpeed = 36.0f;
	};

	// 回転
	float m_angleDeg = 0.0f;

	//モデル
	std::shared_ptr<KdModelData> m_model = nullptr;
};