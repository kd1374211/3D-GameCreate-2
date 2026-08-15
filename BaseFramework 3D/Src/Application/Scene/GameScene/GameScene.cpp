#include "GameScene.h"
#include"../SceneManager.h"
#include "../../main.h"

#include "../../StageManager/StageManager.h"
#include "../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include "../../GameObject/Chara/Player/Player.h"
#include "../../GameObject/Chara/CharaManager.h"
#include "../../GameObject/UI/SceneUIObjects/Game/GameUIObjects.h"

void GameScene::Event()
{
	//ピンが全て倒れたらリザルト移行
	if (STAGEMGR.IsAllPinsFallen())
	{
		//リザルトをセット
		STAGEMGR.SetGameResult(m_stageTimer);

		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Result
		);
	}

	//時間経過
	m_stageTimer += Application::Instance().GetDeltaTime();
	//UIタイマーに適応
	if (!m_wpUI.expired())
	{
		m_wpUI.lock()->SetTimer((int)m_stageTimer);
	}

	//ピン残数確認(DEBUG)
	KdDebugGUI::Instance().AddLog("Total Pin :%d\n", STAGEMGR.GetTotalPinCount());
	KdDebugGUI::Instance().AddLog("Remaining Pin :%d\n", STAGEMGR.GetRemainingPinCount());

	//ステージ情報取得チェック
	KdDebugGUI::Instance().AddLog("2 Star Time : %.2f\n", STAGEMGR.GetStageInfo(SCENEMGR.GetStageNo())->m_2StarTime);
	KdDebugGUI::Instance().AddLog("3 Star Time : %.2f\n", STAGEMGR.GetStageInfo(SCENEMGR.GetStageNo())->m_3StarTime);
}

void GameScene::Init()
{
	//ステージデータ読み込み（ステージ番号取得）
	if (STAGEMGR.LoadStage(SCENEMGR.GetStageNo()))
	{
		//ステージ生成
		STAGEMGR.BuildStage();
	}

	//カメラとプレイヤー生成
	std::shared_ptr<TPSCamera> camera = std::make_shared<TPSCamera>();
	camera->Init();
	std::shared_ptr<Player> player = std::make_shared<Player>(Math::Vector3(0, 1.0f, 0), 0.1f);
	
	//取得用にセット
	m_wpCamera = camera;
	CHARAMGR.SetPlayer(player);

	//リンク
	camera->SetTarget(player);
	player->SetCamera(camera);
	
	//追加
	AddObject(camera);
	AddObject(player);

	//UI
	std::shared_ptr<GameUIObjects> UIObj = std::make_shared<GameUIObjects>();
	m_wpUI = UIObj;
	AddObject(UIObj);
}
