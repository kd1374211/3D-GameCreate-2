#pragma once
#include "../TerrainBase.h"

class IslandDecoration :public TerrainBase
{
public:

	IslandDecoration(const std::string& modelPath, const Math::Vector3& pos, const Math::Vector3& rot, const Math::Vector3& scale);
	~IslandDecoration()override {}

	void DrawLit()override;
	void GenerateDepthMapFromLight()override;

	// 活性フラグ切り替え
	void SetIsActive(bool flg);

	// 管理番号付与と取得
	void SetIndex(int index) { m_index = index; }
	int GetIndex()const { return m_index; }

private:

	// 活性フラグ
	bool m_isActive = true;

	// 管理番号
	int m_index = 0;

};