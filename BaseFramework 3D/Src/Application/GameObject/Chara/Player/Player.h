#pragma once
#include "../CharaBase.h"
#include "../../../Component/PhysicsComponent/PhysicsComponent.h"

//カメラ情報取得用
class CameraBase;

class Player :public CharaBase
{
public:

	Player(const Math::Vector3& a_startPos, float a_radius);
	~Player()override {}

	void Update()override;
	void PostUpdate()override;

	//カメラ設定
	void SetCamera(const std::shared_ptr<CameraBase>& camera) { m_wpCamera = camera; }
	
private:

	void Init()override;

	//カメラ用
	std::weak_ptr<CameraBase> m_wpCamera;

	//当たり判定
	static inline const float HITRADIUS = 0.5f;

	//移動速度
	static inline const float MAXMOVESPEED = 2.5f;
	static inline const float MOVESPEED = 0.5f;
	static inline const float MOVESPEEDCHANGE = 0.025f;
	float m_moveSpeed = 0.0f;

	//物理コンポーネント
	std::shared_ptr<PhysicsComponent> m_cPhysics;

	//進行方向
	float m_facingAngle = 0.0f;

	//半径
	float m_radius = 0.0f;

	//転がり
	float m_rollAngle = 0.0f;

	//速度
	float m_currentSpeedXZ = 0.0f;

};
