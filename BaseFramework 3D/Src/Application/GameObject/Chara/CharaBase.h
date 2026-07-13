#pragma once

class CharaBase :public KdGameObject
{
public:

	CharaBase() {}
	virtual ~CharaBase()override {}

	void DrawLit()override;
	void GenerateDepthMapFromLight()override;

	//セッター
	void SetPos(const Math::Vector3& a_pos) { m_pos = a_pos; }

protected:

	//モデル
	std::shared_ptr<KdModelWork>m_model = nullptr;

	//座標
	Math::Vector3 m_pos = Math::Vector3::Zero;
	Math::Vector3 m_nextPos = Math::Vector3::Zero;

	//移動関連
	float m_moveSpeed = 0.0f;

	//重力
	static inline const float GRAVITY = 0.02f;
	static inline const float GRAVITYMAX = -1.0f;
};
