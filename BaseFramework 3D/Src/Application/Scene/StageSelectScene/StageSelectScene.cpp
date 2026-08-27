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
	FADEMGR.StartFadeIn();
}

void StageSelectScene::Event()
{
	//フェードイン終了待機
	if (FADEMGR.IsFadeInEnd())
	{
		m_isFadeInEnd = true;
	}

	//フェードインが終わってかつフェードアウトも終わったら
	if (m_isFadeInEnd && FADEMGR.IsFadeOutEnd())
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
				FADEMGR.StartFadeOut();
			}
		}

		isSpacePressed = true;
	}
	else isSpacePressed = false;

	//仮置きタイトル戻り
	static bool isShiftPressed = true;
	if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
	{
		if (!isShiftPressed)
		{
			SceneManager::Instance().SetNextScene
			(
				SceneManager::SceneType::Title
			);
		}
		isShiftPressed = true;
	}
	else isShiftPressed = false;
}
