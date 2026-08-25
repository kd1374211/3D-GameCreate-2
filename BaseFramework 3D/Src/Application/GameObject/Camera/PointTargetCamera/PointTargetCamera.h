#pragma once
#include "../CameraBase.h"

class PointTargetCamera :public CameraBase
{
public:

	PointTargetCamera() {}
	~PointTargetCamera()			override {}

	void Init(Math::Vector3 targetPos);
	void PostUpdate()		override;

	//視点移動
	void MoveCamera(Math::Vector3 move);

	//ターゲット関連
	void SetTarget(Math::Vector3 target);
	void SetViewPoint(Math::Vector3 point);

	// ゲッター
	Math::Vector3 GetCurrentViewPoint()const { return m_viewPoint; }

private:

	//見る先・見る元
	Math::Vector3 m_targetPoint = Math::Vector3::Zero;
	Math::Vector3 m_viewPoint = Math::Vector3::Zero;

	//見下ろし用
	Math::Matrix m_mLocalRot;
};