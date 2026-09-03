#pragma once

class CameraBase;
class BowlingBall;

class Player :public KdGameObject
{
public:

	Player() {}
	~Player()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawLit()override;
	void GenerateDepthMapFromLight()override;

	// カメラ設定
	void SetCamera(const std::shared_ptr<CameraBase>& camera) { m_wpCamera = camera; }
	// ボール設定
	void SetBowlingBall(const std::shared_ptr<BowlingBall>& ball) { m_wpBowlingBall = ball; }

	//移動可能フラグ切り替え
	void SetIsMovable(bool flg) { m_isMovable = flg; }

	//操作可能フラグ切り替え
	void SetIsInputEnabled(bool flg) { m_isInputEnabled = flg; }

	// リスポーン
	void Respawn(const Math::Vector3& pos, const Math::Quaternion rot);

private:

	// このクラスの定数
	struct PlayerConsts
	{
		// 方向
		static constexpr float TurnSpeed = 10.0f;		// 旋回速度

		// 強さ
		static constexpr float PowerChangeSpeed = 0.4f;	// 投げる強さを調整する速度
		static constexpr float MinPower = 0.1f;			// 最低強さ
		static constexpr float MaxPower = 1.0f;			// 最大強さ
		static constexpr float StartPower = 0.7f;			// 基礎強さ
	};

	// 状態のリセット
	void Reset();

	// Quaternion から facingAngle（度数法: Deg）を求める
	// 基準: Z+ 方向 (0, 0, 1) = 0度
	float GetFacingAngleFromQuaternion(const Math::Quaternion& rot)
	{
		// 1. Z+ 方向の単位ベクトルをクォータニオンで回転させる
		Math::Vector3 forward = Math::Vector3::Transform(Math::Vector3::UnitZ, rot);

		// 2. atan2(x, z) を使って Z+ 基準の Y軸回転角（ラジアン）を算出
		// Z+ 方向 (0, 0, 1) のとき atan2(0, 1) = 0 rad (0度)
		// X+ 方向 (1, 0, 0) のとき atan2(1, 0) = +PI/2 rad (+90度)
		// X- 方向 (-1, 0, 0) のとき atan2(-1, 0) = -PI/2 rad (-90度)
		float angleRad = std::atan2(forward.x, forward.z);

		// 3. ラジアン から 度数法（Deg）に変換
		constexpr float RadToDeg = 180.0f / M_PI;
		float facingAngleDeg = angleRad * RadToDeg;

		// 4.もしマイナスだった場合360を足してプラスにする
		if (facingAngleDeg < 0.0f)
		{
			facingAngleDeg += 360.0f;
		}

		return facingAngleDeg;
	}

	// facingAngle (0〜360度, deg) から 単位方向ベクトル (Math::Vector3) を計算
	Math::Vector3 GetForwardVectorFromFacingAngle(float facingAngleDeg)
	{
		// 1. 度数法（Deg）からラジアン（Rad）に変換
		constexpr float DegToRad = M_PI / 180.0f;
		float angleRad = facingAngleDeg * DegToRad;

		// 2. sin / cos で XZ 平面上の単位ベクトルを算出
		// Z+ 方向 (0 deg)  -> sin(0) = 0, cos(0) = 1  => (0, 0, 1)
		// X+ 方向 (90 deg) -> sin(90)= 1, cos(90) = 0  => (1, 0, 0)
		Math::Vector3 dir;
		dir.x = std::sin(angleRad);
		dir.y = 0.0f;
		dir.z = std::cos(angleRad);

		return dir; // 長さ 1 の単位ベクトル
	}

	//カメラ用
	std::weak_ptr<CameraBase> m_wpCamera;

	// ボウリングボールのweak
	std::weak_ptr<BowlingBall> m_wpBowlingBall;

	//向いている方向
	float m_facingAngle = 0.0f;

	//モデル
	std::shared_ptr<KdModelWork> m_model;

	//位置
	Math::Vector3 m_pos;

	//移動可能フラグ
	bool m_isMovable = false;

	//操作可能フラグ
	bool m_isInputEnabled = false;

	// 投げるパワー
	float m_throwPower = PlayerConsts::StartPower;
};
