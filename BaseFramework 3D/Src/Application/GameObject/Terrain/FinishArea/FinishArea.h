#pragma once
#include "../TerrainBase.h"
#include "../../../Component/PhysicsComponent/PhysicsComponent.h"

class FinishArea :public TerrainBase
{
public:

	FinishArea(Math::Vector3 pos, Math::Quaternion rot, Math::Vector3 size);
	~FinishArea()override {}

	void Init()override;
	void Update()override;

	void DrawLit()override {}
	void DrawUnLit()override;

private:

	float CalcTrailStart();

	struct FinishAreaConsts
	{
		static constexpr float TrailMoveSpeed = 1.0f;
		static constexpr int TrailCPCount = 4;
		static constexpr float TexLoopSize = 1.0f;
		static constexpr float GoalLineTexSize = 32.0f;
	};

	//物理コンポーネント
	std::shared_ptr<PhysicsComponent> m_cPhysics;

	//サイズ
	Math::Vector3 m_scale = Math::Vector3::One;

	// トレイルポリゴン
	std::shared_ptr<KdSquarePolygon> m_polygon;

	// ラインの長さ
	float m_lineLength = 0.0f;
	float m_moveProgress = 0.0f;

	// トレイルチェックポイント
	Math::Vector3 m_trailCheckPoint[FinishAreaConsts::TrailCPCount];
	float m_trailFaceProgress[FinishAreaConsts::TrailCPCount];
	Math::Matrix m_trailFaceMat[FinishAreaConsts::TrailCPCount];

};