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

};
