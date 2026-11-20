#pragma once
#include "../CameraBase.h"

//ターゲットの周りを回転するカメラ
class StageViewCamera :public CameraBase
{
public:
	StageViewCamera() {}
	~StageViewCamera()		override {}

	void Init()				override;
	void PostUpdate()		override;

	void SetViewDistance(float distance) { m_viewDistance = distance; }

private:

	// クラス内定数
	struct StageViewCameraConsts
	{
		static constexpr float RotateSpeed = 18.0f;
	};

	Math::Matrix m_mLocalRot = Math::Matrix::Identity;

	//どれだけ離れて見るか
	float m_viewDistance = 0.0f;

	//回転角度（Y）
	float m_angleDeg = 0.0f;
};