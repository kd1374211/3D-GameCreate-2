#include "SceneManager.h"

#include "BaseScene/BaseScene.h"
#include "TitleScene/TitleScene.h"
#include "StageSelectScene/StageSelectScene.h"
#include "GameScene/GameScene.h"
#include "ResultScene/ResultScene.h"

//追加7/17
//使うインクルード達
#include "../main.h"
#include "../Physics/PhysicsManager.h"
#include "../StageManager/StageManager.h"

void SceneManager::PreUpdate()
{
	//デバッグ
	KdDebugGUI::Instance().ClearLog();

	// シーン切替
	if (m_currentSceneType != m_nextSceneType)
	{
		ChangeScene(m_nextSceneType);
	}

	m_currentScene->PreUpdate();
}

void SceneManager::Update()
{
	m_currentScene->Update();
}

void SceneManager::PostUpdate()
{
	// エディットモード中は物理シミュレーションの更新ステップをスキップ
	if (!STAGEMGR.IsEditMode())
	{
		//追加7/17（仮）
		PHYSICSMGR.Update(Application::Instance().GetDeltaTime());
	}

	m_currentScene->PostUpdate();

	//追加8/10
	KdDebugGUI::Instance().AddLog("Object Count: %d\n", m_currentScene->GetObjList().size());
}

void SceneManager::PreDraw()
{
	m_currentScene->PreDraw();
}

void SceneManager::Draw()
{
	m_currentScene->Draw();

	STAGEMGR.DrawSelectedObjectOutline();
}

void SceneManager::DrawSprite()
{
	m_currentScene->DrawSprite();
}

void SceneManager::DrawDebug()
{
	m_currentScene->DrawDebug();
}

const std::list<std::shared_ptr<KdGameObject>>& SceneManager::GetObjList()
{
	return m_currentScene->GetObjList();
}

void SceneManager::AddObject(const std::shared_ptr<KdGameObject>& _obj)
{
	m_currentScene->AddObject(_obj);
}

std::weak_ptr<CameraBase> SceneManager::GetCamera()
{
	return m_currentScene->GetCamera();
}

void SceneManager::Release()
{
	//シーン破壊
	m_currentScene = nullptr;
}

void SceneManager::ChangeScene(SceneType _sceneType)
{
	// 次のシーンを作成し、現在のシーンにする
	switch (_sceneType)
	{
	case SceneType::Title:
		m_currentScene = std::make_shared<TitleScene>();
		break;
	case SceneType::StageSelect:
		m_currentScene = std::make_shared<StageSelectScene>();
		break;
	case SceneType::Game:
		m_currentScene = std::make_shared<GameScene>();
		break;
	case SceneType::Result:
		m_currentScene = std::make_shared<ResultScene>();
		break;
	}

	// シーンの初期化
	m_currentScene->Init();

	// 現在のシーン情報を更新
	m_currentSceneType = _sceneType;
}
