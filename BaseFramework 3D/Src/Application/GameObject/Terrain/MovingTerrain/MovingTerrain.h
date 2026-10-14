#pragma once
#include "../TerrainBase.h"
#include "../../../StageManager/VariantParamData.h"

class MovingTerrain :public TerrainBase
{
public:

	MovingTerrain(Math::Vector3 pos, Math::Vector3 rot, Math::Vector3 sca, MovingParams param);
	~MovingTerrain()override {}

	void Update()override;
	void PostUpdate()override;
	void DrawUnLit()override;

private:

	struct MovingTerrainConsts
	{

	};
	
	// 次点までの距離を計算
	float CalcDistToNextPoint();
	// 次点までの方向と距離を合わせて計算
	Math::Vector3 CalcVecToNextPoint();
	// 現在点と次の点の位置を更新
	void UpdatePoints();

	// パラメータ
	MovingParams m_param;

	// 待機時間
	bool m_isWait = true;
	float m_waitTime = 0.0f;

	// 停止点の合計数
	int m_totalStopPoint = 0;
	// 次の目標番号
	int m_currentPointIndex = 0;
	// チェックポイントをまとめる
	std::vector<Math::Vector3> m_combinedWayPoints;

	// プログレッション
	float m_progress = 0.0f;
	// 現在の点と次の点の距離
	float m_distToNextPoint = 0.0f;

	// デバッグ用ウェイポイントドット
	std::shared_ptr<KdModelData> m_wayPointDotModel = nullptr;
};
