#pragma once
#include "../CameraBase.h"

class TPSCamera : public CameraBase
{
public:
	TPSCamera()							{}
	~TPSCamera()			override	{}

	void Init()				override;
	void PostUpdate()		override;

	// ピボット更新
	void SetPivotToCurrentTarget();

	// 回転セッター
	void SetIsCamLocked(bool flg) { m_isCamLocked = flg; }

private:

	// クラス内定数
	struct TPSCameraConsts
	{
		// 追従速度
		static constexpr float LerpSpeed = 12.0f;
		static constexpr float MaxDistance = 20.0f;
		static constexpr float MinDistance = 0.001f;
	};

	bool m_isCamLocked = false;
	Math::Matrix m_mLocalRot = Math::Matrix::Identity;
	Math::Vector3 m_currentPivotPos = Math::Vector3::Zero;
};