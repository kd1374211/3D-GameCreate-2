#pragma once
#include "../CameraBase.h"

class TerrainBase;

//ステージを真上から見下ろすカメラ
class LookingDownCamera :public CameraBase
{
public:
	LookingDownCamera() {}
	~LookingDownCamera()		override {}

	void Init()				override;
	void Update()			override;
	void PostUpdate()		override;

	// メイン地形を基準に見下ろし基準位置を算出
	void CalcViewPointYFromMainTerrain();

	// XZ座標を設定
	void SetViewPointXZ(float x, float z) { m_viewPos.x = x, m_viewPos.z = z; }

private:

	// クラス内定数
	struct LookingDownCameraConsts
	{
		// カーソル速度関連
		static constexpr float CursorSpeedLimit = 1000.0f;

		// カメラ移動関連
		static constexpr float CameraMoveSpeed = 0.01f;	// カメラ移動速度
		static constexpr float CameraMovePosLimit = 25.0f;	// カメラ移動範囲MAX

		// カメラ位置関連
		static constexpr float LookingDownPosY = 10.0f;		// どれだけ上から見るか

		// 隠す範囲
		static constexpr float HidingAreaHalf = 2.0f;		// XZ半径
		static constexpr float HidingAreaY = 100.0f;		// Yの下方向
	};

	// カーソル蓄積をリセット
	void ResetStoredCursorMove() { m_storedTotalCursorMove = Math::Vector2::Zero; }

	// 邪魔なオブジェクトを隠す
	void CheckHidingObject();
	// 隠したオブジェクトを戻す
	void ResetHiddenObjects();

	// 蓄積カーソル移動距離
	Math::Vector2 m_storedTotalCursorMove;

	// Y座標はどこか
	float m_lookingDownPosY = 0.0f;

	// 現在の座標
	Math::Vector3 m_viewPos = Math::Vector3::Zero;

	// 隠したオブジェクトのリスト
	std::list<uint64_t> m_hiddenObjectList;
};