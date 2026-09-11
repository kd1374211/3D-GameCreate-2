#pragma once
#include "../CameraBase.h"

class TPSCamera : public CameraBase
{
public:
	TPSCamera()							{}
	~TPSCamera()			override	{}

	void Init()				override;
	void PostUpdate()		override;

	// 回転セッター
	void SetIsCamLocked(bool flg) { m_isCamLocked = flg; }

private:

	bool m_isCamLocked = false;
	Math::Matrix m_mLocalRot = Math::Matrix::Identity;
};