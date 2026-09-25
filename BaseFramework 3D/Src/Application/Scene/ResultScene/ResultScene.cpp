#include "ResultScene.h"
#include "../SceneManager.h"
#include "../../GameObject/UI/SceneUIObjects/Result/ResultUIObjects.h"
#include "../../StageManager/StageManager.h"
#include "../../Const/BowlingSystemConst.h"

#include "../../GameObject/Camera/StageViewCamera/StageViewCamera.h"
#include "../../FadeManager/FadeManager.h"
#include "../../GameObject/Camera/CameraManager.h"

void ResultScene::Init()
{
	//背景ステージ読み込み
	STAGEMGR.LoadStage(SCENEMGR.GetStageNo());
	//背景モードで生成
	STAGEMGR.BuildStage(1, StageBuildMode::Background);

	//ステージ確認カメラ
	std::shared_ptr<StageViewCamera> camera = std::make_shared<StageViewCamera>();
	camera->Init();
	camera->SetViewDistance(20.0f);

	//さっき生成した地形をターゲットに
	if (!STAGEMGR.GetTerrain().expired())
	{
		camera->SetTarget(STAGEMGR.GetTerrain().lock());
	}

	//追加
	AddObject(camera);

	//マネージャーに追加
	CAMERAMGR.SetGameCamera(camera);
	CAMERAMGR.SetDefaultCamera(CameraType::Game);

	//UI全般
	std::shared_ptr<ResultUIObject> UIObj = std::make_shared<ResultUIObject>();
	AddObject(UIObj);

	//ゲーム速度戻す
	SCENEMGR.SetGameSpeed(1.0f);

	//フェードイン
	FADEMGR.StartFadeIn(&m_isFadeInEnd);

	// SCENEMGRからリザルトを取得
	ScoreDatas::GameResult result = SCENEMGR.GetGameResult();

	// UIに送信
}

void ResultScene::Event()
{
	if (m_isSceneChangeReady)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::StageSelect
		);

		return;
	}

	static bool isSpacePressed = true;
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (!isSpacePressed)
		{
			if (m_isFadeInEnd)
			{
				FADEMGR.StartFadeOut(&m_isSceneChangeReady);
			}
		}

		isSpacePressed = true;
	}
	else isSpacePressed = false;

	// DEBUG
	KdDebugGUI::Instance().AddLog("Total Score : %d\n", STAGEMGR.GetScore());
}
