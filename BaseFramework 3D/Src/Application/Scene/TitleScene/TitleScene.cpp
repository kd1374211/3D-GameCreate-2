#include "TitleScene.h"
#include "../SceneManager.h"
#include "../../GameObject/UI/SceneUIObjects/Title/TitleUIObjects.h"
#include "../../StageManager/StageManager.h"
#include "../../GameObject/Camera/StageViewCamera/StageViewCamera.h"
#include "../../GameObject/Camera/CameraManager.h"
#include "../../Const/DeviceAndKey.h"
#include "../../Cursor/CursorManager.h"
#include "../../Sound/SoundManager.h"

void TitleScene::Event()
{
	//フェードアウト終了チェック(FADEMGRからオンにされる)
	if (m_isSceneChangeReady)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::StageSelect
		);

		// BGM停止
		SOUNDMGR.Stop(SoundType::BGM_Title);

		return;
	}

	// UI経由でスタートボタンが押されたかを確認
	if (auto UIObj = m_wpUI.lock())
	{
		if (UIObj->IsStartButtonPressed())
		{
			//フェードアウト
			FADEMGR.StartFadeOut(&m_isSceneChangeReady);
		}
	}
}

void TitleScene::Init()
{
	//ランダムに背景ステージ読み込み
	//読み込むステージをランダムで決定
	int backStageNo = (int)(rand() / RAND_MAX * (STAGEMGR.GetMaxStageNo() - STAGEMGR.GetMinStageNo()) + STAGEMGR.GetMinStageNo());
	STAGEMGR.LoadStage(backStageNo);
	//背景モードで生成
	STAGEMGR.BuildStage(KdRandom::GetInt(BowlingSystemConsts::StartFrame, BowlingSystemConsts::LastFrame), StageBuildMode::Background);

	//ステージ確認カメラ
	std::shared_ptr<StageViewCamera> camera = std::make_shared<StageViewCamera>();
	camera->Init();
	camera->SetViewDistance(25.0f);

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
	m_wpUI = UIObj;

	// カーソル表示・固定解除
	CURSOR.SetIsShowCursor(true);
	CURSOR.UnlockCursor();
	
	// BGM再生
	SOUNDMGR.Play(SoundType::BGM_Title);
}
