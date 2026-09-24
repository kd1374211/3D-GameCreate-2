#pragma once
#include "../../../Component/PhysicsComponent/PhysicsComponent.h"

// 投球終了の理由
enum class RollEndReason
{
	None,
	Stop,
	Fall,
	Finish
};

class TPSCamera;
class ShotCursor;
class ShotPowerBar;

class BowlingBall :public KdGameObject
{
public:

	BowlingBall();
	~BowlingBall()override {}

	void Init(float a_radius);
	void Update()override;
	void PostUpdate()override;
	void DrawLit()override;
	void GenerateDepthMapFromLight()override;

	// 投げ
	void Throw(const Math::Vector3& startPos, const Math::Vector3& direction, float power);

	// リセット
	void Reset();

	// 停止・原因確認用
	bool IsRolling() const { return m_isRolling; }
	RollEndReason GetRollEndReason()const { return m_reason; }

	// 位置・回転セット
	void Respawn(const Math::Vector3& pos, const Math::Vector3& rot);

	// ゴールに触れた
	void HitFinishArea();

	//操作可能フラグ切り替え
	void SetIsInputEnabled(bool flg) { m_isInputEnabled = flg; }

	// カメラ設定
	void SetCamera(const std::shared_ptr<TPSCamera>& camera) { m_wpCamera = camera; }

private:

	// このクラス内の定数
	struct BowlingBallConsts
	{
		// 停止確認関連
		static constexpr float StopCheckBorder = 0.01f;
		static constexpr float RollEndTime = 1.5f;
		
		// 発射速度
		static constexpr float ThrowPowerMulti = 9.0f;

		// 重さ
		static constexpr float BallMass = 7.0f;

		// 強さ
		static constexpr float ThrowSpeedDiv = 1000.0f;
		static constexpr float ThrowSpeedMax = 1.0f;
		static constexpr float ThrowSpeedMin = 0.1f;
		static constexpr float AverageCalcTime = 0.1f;

		// 引っ張り発射
		static constexpr float ShootMinSpeed = 0.1f;
	};

	// 活性化・非活性化
	void ActivateBody();
	void DeactivateBody();

	// 停止確認
	void CheckIsStop();

	//モデル
	std::shared_ptr<KdModelData> m_model;

	//位置
	Math::Vector3 m_pos;
	//回転
	Math::Quaternion m_rot;

	// 連続停止時間
	float m_stopTimer = 0.0f;

	// 転がり中か
	bool m_isRolling = false;
	// 転がり終了した原因
	RollEndReason m_reason = RollEndReason::None;

	// 転がり開始か
	bool m_canRoll = false;

	// 物理コンポーネント
	std::shared_ptr<PhysicsComponent> m_cPhysics;

	// 物理が有効か
	bool m_isActive = false;

	// 引っ張りハンティング用
	bool m_isShootStart = false;

	// 速度平均管理用
	struct SpeedStoreData
	{
		float m_gameDt = 0.0f;
		float m_moveDist = 0.0f;
	};
	std::vector<SpeedStoreData> m_speedStoreData = {};
	float m_totalStoredTime = 0.0f;
	
	//↓playerクラスから移行した

	// 強さと方向の矢印
	std::shared_ptr<KdModelData> m_arrowModel = nullptr;
	Math::Matrix m_arrowMat;

	//操作可能フラグ
	bool m_isInputEnabled = false;

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

	// 平均計算
	float CalcAvgMoveSpeed();

	//カメラ用
	std::weak_ptr<TPSCamera> m_wpCamera;

	// カーソル用
	std::weak_ptr<ShotCursor> m_wpCursor;

	// バー
	std::weak_ptr<ShotPowerBar> m_wpPowerBar;
};