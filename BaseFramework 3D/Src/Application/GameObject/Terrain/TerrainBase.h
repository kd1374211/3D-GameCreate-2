#pragma once

class TerrainBase :public KdGameObject
{
public:

	TerrainBase() {}
	virtual ~TerrainBase()override {}

	void DrawLit()override;

	//セッター
	void SetPos(const Math::Vector3& a_pos) { m_pos = a_pos; }

protected:

	std::shared_ptr<KdModelWork> m_model = nullptr;
	Math::Vector3 m_pos = Math::Vector3::Zero;

};