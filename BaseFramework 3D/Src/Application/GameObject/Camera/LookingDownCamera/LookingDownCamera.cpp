#include "LookingDownCamera.h"
#include "../../../Physics/PhysicsManager.h"
#include "../../../StageManager/StageManager.h"
#include "../../Terrain/TerrainBase.h"
#include "../../../Const/DeviceAndKey.h"
#include "../../../Cursor/CursorManager.h"
#include "../../../Scene/SceneManager.h"

void LookingDownCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();
}

void LookingDownCamera::Update()
{
	// 隠したオブジェクトを元に戻す
	ResetHiddenObjects();

	// アクティブじゃないならスキップ
	if (!m_isDefault)return;

	// ゲームDt取得
	float gameDt = SCENEMGR.GetDeltaGameTime();

	// 左クリックを押しながら移動でカメラ移動
	if (KdInputManager::Instance().IsPress(GetKeyRegistName(VK_LBUTTON)) || KdInputManager::Instance().IsHold(GetKeyRegistName(VK_LBUTTON)))
	{
		// マウス固定でないならリターン
		if (!CURSOR.GetIsMouseLocked())return;

		// マウス位置を取得
		POINT _nowPos = CURSOR.GetFixedCursorPos();

		// 移動距離を確認
		POINT _mouseMove{};
		_mouseMove.x = _nowPos.x - m_FixMousePos.x;
		_mouseMove.y = _nowPos.y - m_FixMousePos.y;

		// 移動状況を加算
		m_storedTotalCursorMove += Math::Vector2(_mouseMove.x, _mouseMove.y);
		// 最大、最小チェック
		m_storedTotalCursorMove.x = std::clamp(m_storedTotalCursorMove.x, -LookingDownCameraConsts::CursorSpeedLimit, LookingDownCameraConsts::CursorSpeedLimit);
		m_storedTotalCursorMove.y = std::clamp(m_storedTotalCursorMove.y, -LookingDownCameraConsts::CursorSpeedLimit, LookingDownCameraConsts::CursorSpeedLimit);

		// 移動距離に補正を掛けてカメラ位置を移動
		m_viewPos.x += m_storedTotalCursorMove.x * LookingDownCameraConsts::CameraMoveSpeed * gameDt;
		m_viewPos.z += m_storedTotalCursorMove.y * LookingDownCameraConsts::CameraMoveSpeed * gameDt;

		// 最大値、最小値に合わせる
		m_viewPos.x = std::clamp(m_viewPos.x, -LookingDownCameraConsts::CameraMovePosLimit, LookingDownCameraConsts::CameraMovePosLimit);
		m_viewPos.z = std::clamp(m_viewPos.z, -LookingDownCameraConsts::CameraMovePosLimit, LookingDownCameraConsts::CameraMovePosLimit);
	}
	// 左クリックを離したら蓄積リセット
	else if (KdInputManager::Instance().IsRelease(GetKeyRegistName(VK_LBUTTON)))
	{
		ResetStoredCursorMove();
	}
}

void LookingDownCamera::PostUpdate()
{
	// アクティブじゃないならスキップ
	if (!m_isDefault)return;

	// Y座標の設定
	m_viewPos.y = m_lookingDownPosY;

	// マトリックス設定
	Math::Matrix trans = Math::Matrix::CreateTranslation(m_viewPos);
	Math::Matrix rotat = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(90.0f));
	m_mWorld = rotat * trans;

	// 邪魔なオブジェクトを隠す
	CheckHidingObject();
}

void LookingDownCamera::CalcViewPointYFromMainTerrain()
{
	// メイン地形からBodyID取得
	if (const auto& spMainTerrain = STAGEMGR.GetTerrain().lock())
	{
		const JPH::BodyID& BodyID = spMainTerrain->GetBodyID();

		// BodyIDから最高点を取得
		const auto& bodyInterface = PHYSICSMGR.GetBodyInterface();
		JPH::AABox worldBounds = bodyInterface.GetTransformedShape(BodyID).GetWorldSpaceBounds();
		float maxY = worldBounds.mMax.GetY();

		// 最高点から一定値高い位置に設定
		m_lookingDownPosY = maxY + LookingDownCameraConsts::LookingDownPosY;
	}
}

void LookingDownCamera::CheckHidingObject()
{
	// 地形フィルタ
	GroundObjectFilter grounds;

	// 1. 判定したい AABB 箱を定義(カメラの位置から半径X2Y2の範囲かつ下方向は長めに)
	JPH::Vec3 JPHViewPos = JPH::Vec3(m_viewPos.x, m_viewPos.y, m_viewPos.z);
	JPH::AABox box
	(JPHViewPos - JPH::Vec3(LookingDownCameraConsts::HidingAreaHalf, LookingDownCameraConsts::HidingAreaY, LookingDownCameraConsts::HidingAreaHalf),
	JPHViewPos + JPH::Vec3(LookingDownCameraConsts::HidingAreaHalf, 0.0f, LookingDownCameraConsts::HidingAreaHalf));

	JPH::AllHitCollisionCollector<JPH::CollideShapeBodyCollector> collector;
	PHYSICSMGR.GetSystem().GetBroadPhaseQuery().CollideAABox(box, collector, {}, grounds);

	for (const JPH::BodyID& hitBodyID : collector.mHits)
	{
		//  リストに入っているものは除外する
		if (m_hitObjList->ShouldCollide(hitBodyID)) continue; // ★ここで個別除外！

		// 対象ボディに対する処理

		// hitBodyIDから元のオブジェクトを取得
		uint64_t userData = PHYSICSMGR.GetBodyInterface().GetUserData(hitBodyID);

		if (userData != 0)
		{
			// KdGameObjectにreinCast
			auto* obj = reinterpret_cast<KdGameObject*>(userData);

			// TerrainBaseにDynamicCast
			if (auto* terrain = dynamic_cast<TerrainBase*>(obj))
			{
				// IsDrawをfalseに
				terrain->SetIsDraw(false);

				// 隠したオブジェクトリストに追加
				m_hiddenObjectList.push_back(userData);
			}
		}
	}
}

void LookingDownCamera::ResetHiddenObjects()
{
	// リストに中身がある間
	while (!m_hiddenObjectList.empty())
	{
		// 一応有効確認
		if (*m_hiddenObjectList.begin() != 0)
		{
			// 一番前を見てKdGameObjectにreinCast
			auto* obj = reinterpret_cast<KdGameObject*>(*m_hiddenObjectList.begin());

			// TerrainBaseにDynamicCast
			if (auto* terrain = dynamic_cast<TerrainBase*>(obj))
			{
				// IsDrawをtrueに
				terrain->SetIsDraw(true);
			}
		}

		// 一番前を削除
		m_hiddenObjectList.pop_front();
	}
}
