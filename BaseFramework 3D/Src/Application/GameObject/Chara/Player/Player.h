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

	//カメラ情報を取得
	void SetCamera(const std::shared_ptr<CameraBase>& a_spCamera) { m_wpCamera = a_spCamera; }

private:

	void Init()override;

	//カメラ用
	std::weak_ptr<CameraBase> m_wpCamera;

	//当たり判定
	static inline const float HITRADIUS = 0.5f;
	
	//物理コンポーネント
	std::shared_ptr<PhysicsComponent> m_cPhysics;
};