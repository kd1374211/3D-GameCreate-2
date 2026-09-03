#include "TitleScene.h"
#include "../SceneManager.h"
#include "../../GameObject/UI/SceneUIObjects/Title/TitleUIObjects.h"
#include "../../StageManager/StageManager.h"
#include "../../GameObject/Camera/StageViewCamera/StageViewCamera.h"
#include "../../GameObject/Camera/CameraManager.h"

void TitleScene::Event()
{
	//フェードアウト終了チェック
	if (FADEMGR.IsFadeOutEnd())
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
			//フェードアウト
			FADEMGR.StartFadeOut();
		}
		isSpacePressed = true;
	}
	else isSpacePressed = false;
}

void TitleScene::Init()
{
	//ランダムに背景ステージ読み込み
	//読み込むステージをランダムで決定
	int backStageNo = (int)(rand() / RAND_MAX * (STAGEMGR.GetMaxStageNo() - STAGEMGR.GetMinStageNo()) + STAGEMGR.GetMinStageNo());
	STAGEMGR.LoadStage(backStageNo);
	//背景モードで生成
	STAGEMGR.BuildStage(1, StageBuildMode::Background);

	//ステージ確認カメラ
	std::shared_ptr<StageViewCamera> camera = std::make_shared<StageViewCamera>();
	camera->Init();
	camera->SetViewDistance(20.0f);

	//マネージャー
	CAMERAMGR.SetGameCamera(camera);
	CAMERAMGR.SetDefaultCamera(CameraType::Game);

	//さっき生成した地形をターゲットに
	if (!STAGEMGR.GetTerrain().expired())
	{
		camera->SetTarget(STAGEMGR.GetTerrain().lock());
	}

	//追加
	AddObject(camera);

	//UI全般
	std::shared_ptr<TitleUIObject> UIObj = std::make_shared<TitleUIObject>();
	AddObject(UIObj);
}
