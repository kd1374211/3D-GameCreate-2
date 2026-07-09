#pragma once
#include "../CharaBase.h"

//カメラ情報取得用
class CameraBase;

class Player :public CharaBase
{
public:

	Player() { Init(); }
	~Player() {}

	void Update()override;

	void SetCamera(const std::shared_ptr<CameraBase>& a_spCamera) { m_wpCamera = a_spCamera; }

private:

	void Init()override;

	std::weak_ptr<CameraBase> m_wpCamera;

};