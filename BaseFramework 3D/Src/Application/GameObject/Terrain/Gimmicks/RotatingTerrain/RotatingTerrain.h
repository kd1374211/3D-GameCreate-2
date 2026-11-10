#pragma once
#include "../../TerrainBase.h"
#include "../../../../Const/VariantParamData.h"

class RotatingTerrain :public TerrainBase
{
public:

	RotatingTerrain(Math::Vector3 pos, Math::Vector3 rot, Math::Vector3 sca, RotatingParams param);
	~RotatingTerrain()override {}

	void Update()override;
	void PostUpdate()override;
	void DrawUnLit()override;

private:

	// クラス内定数
	struct RotatingTerrainConsts
	{
		static constexpr float ProgSpeedMulti_Standard = 1.0f;
		static constexpr float ProgSpeedMulti_Reverse = -1.0f;
	};

	// Reverse状態か
	bool IsReverse()const { return m_progSpeedMulti < 0.0f; }
	// Prog反転
	void ProgReverse() { m_progSpeedMulti = IsReverse() ? RotatingTerrainConsts::ProgSpeedMulti_Standard : RotatingTerrainConsts::ProgSpeedMulti_Reverse; }

	// 回転モード
	bool m_isBounceMode = false;
	float m_bounceProg = 0.0f;

	// プログレス進行速度
	float m_progSpeed = 0.0f;
	float m_progSpeedMulti = RotatingTerrainConsts::ProgSpeedMulti_Standard;

	// 回転軸座標
	Math::Vector3 m_rotatePivotDotPos = Math::Vector3::Zero;

	// バウンド角度
	float m_bounceDeg = 0.1f;

	// エディター用回転軸ドット
	std::shared_ptr<KdModelData> m_rotatePivotDotModel = nullptr;

};
