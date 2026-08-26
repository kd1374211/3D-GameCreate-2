#pragma once
#include "../../../Component/PhysicsComponent/PhysicsComponent.h"

//カメラ情報取得用
class CameraBase;

class Player :public KdGameObject
{
public:

	Player(const Math::Vector3& a_startPos, float a_radius);
	~Player()override {}

	void Update()override;
	void PostUpdate()override;

	void DrawLit()override;
	void GenerateDepthMapFromLight()override;

	//カメラ設定
	void SetCamera(const std::shared_ptr<CameraBase>& camera) { m_wpCamera = camera; }
	
	//壁との衝突
	void OnHitWall(const JPH::Vec3& wallNormal);

	//移動可能フラグ切り替え
	void SetIsMovable(bool flg) { m_isMovable = flg; }

	//操作可能フラグ切り替え
	void SetIsInputEnabled(bool flg) { m_isInputEnabled = flg; }

	// 落下死チェック
	bool GetIsFall()const { return m_isFall; }

	// クリア
	void Finish() { m_isFinish = true; }
	bool GetIsFinish()const { return m_isFinish; }

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

	//壁いろいろ
	JPH::Vec3 m_lastWallNormal = JPH::Vec3::sZero();
	float m_wallContactTimer = 0.0f; // int から float に変更（単位：秒）
	float m_wallHitCoolTime = 0.0f; // 壁ヒット直後の安全タイマー
	bool  m_needPushOut = false;// ★追加：押し出しフラグ
	
	//モデル
	std::shared_ptr<KdModelWork> m_model;

	//位置
	Math::Vector3 m_pos;

	//移動可能フラグ
	bool m_isMovable = false;

	//操作可能フラグ
	bool m_isInputEnabled = false;

	// 落下死フラグ
	bool m_isFall = false;

	// クリアフラグ
	bool m_isFinish = false;
};
