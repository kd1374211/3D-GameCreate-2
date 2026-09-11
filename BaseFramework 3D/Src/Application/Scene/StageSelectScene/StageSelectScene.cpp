#include "StageSelectScene.h"
#include "../SceneManager.h"
#include "../../GameObject/UI/SceneUIObjects/StageSelect/StageSelectUIObjects.h"

void StageSelectScene::Init()
{
	//UI全般
	std::shared_ptr<StageSelectUIObject> UIObj = std::make_shared<StageSelectUIObject>();
	m_wpUI = UIObj;
	AddObject(UIObj);

	//フェードイン
	FADEMGR.StartFadeIn(&m_isFadeInEnd);
}

void StageSelectScene::Event()
{
	//フェードインが終わってかつフェードアウトも終わったら
	if (m_isSceneChangeReady)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Game
		);

		return;
	}

	//長押し対策
	static bool isSpacePressed = true;

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (!isSpacePressed)
		{
			//フェードインが終わっていたら
			if (m_isFadeInEnd)
			{
				//ステージ準備
				if (!m_wpUI.expired())
				{
					SCENEMGR.SetStageNo(m_wpUI.lock()->GetSelectedStageNo());
				}

				//フェードアウト
				FADEMGR.StartFadeOut(&m_isSceneChangeReady);
			}
		}

		isSpacePressed = true;
	}
	else isSpacePressed = false;
}
