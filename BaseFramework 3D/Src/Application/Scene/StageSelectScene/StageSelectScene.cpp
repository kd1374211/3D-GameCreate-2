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
	static bool isLClickPressed = true;

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		if (!isLClickPressed)
		{
			//フェードインが終わっていたら
			if (m_isFadeInEnd)
			{
				auto spUI = m_wpUI.lock();

				// いずれかのステージが選択状態なことを確認
				if (spUI && spUI->GetIsSelect())
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
		}

		isLClickPressed = true;
	}
	else isLClickPressed = false;
}
