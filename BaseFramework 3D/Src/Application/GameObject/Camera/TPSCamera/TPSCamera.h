#pragma once
#include "../CameraBase.h"

class TPSCamera : public CameraBase
{
public:
	TPSCamera()							{}
	~TPSCamera()			override	{}

	void Init()				override;
	void PostUpdate()		override;

private:

	Math::Matrix m_mLocalRot = Math::Matrix::Identity;
};