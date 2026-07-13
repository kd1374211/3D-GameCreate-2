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
	void PostUpdate()override;

	void SetCamera(const std::shared_ptr<CameraBase>& a_spCamera) { m_wpCamera = a_spCamera; }

private:

	void Init()override;

	//カメラ用
	std::weak_ptr<CameraBase> m_wpCamera;

	//当たり判定
	static inline const float HITRADIUS = 0.5f;

	//移動速度
	static inline const float MOVESPEED = 0.02f;

	//移動方向
	Math::Vector3 m_moveDir = Math::Vector3::Zero;

	//移動力
	Math::Vector3 m_moveVelocity = Math::Vector3::Zero;

};