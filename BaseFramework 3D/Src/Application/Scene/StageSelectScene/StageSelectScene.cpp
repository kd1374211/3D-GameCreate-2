#include "StageSelectScene.h"
#include "../SceneManager.h"
#include "../../GameObject/UI/SceneUIObjects/StageSelect/StageSelectUIObjects.h"
#include "../../Cursor/CursorManager.h"
#include "../../Const/DeviceAndKey.h"
#include "../../Sound/SoundManager.h"

void StageSelectScene::Init()
{
	//UI全般
	std::shared_ptr<StageSelectUIObject> UIObj = std::make_shared<StageSelectUIObject>();
	m_wpUI = UIObj;
	AddObject(UIObj);

	//フェードイン
	FADEMGR.StartFadeIn(&m_isFadeInEnd);

	// カーソル表示・固定解除
	CURSOR.SetIsShowCursor(true);
	CURSOR.UnlockCursor();

	// BGM再生
	SOUNDMGR.Play(SoundType::BGM_Select);
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

		// BGM停止
		SOUNDMGR.Stop(SoundType::BGM_Select);

		return;
	}

	// 押されたことを確認
	if (KdInputManager::Instance().IsPress(GetKeyRegistName(VK_LBUTTON)))
	{
		//フェードインが終わっているかつステージ選択が終わっていないなら
		if (m_isFadeInEnd && !m_isStageSelected)
		{
			auto spUI = m_wpUI.lock();

			// いずれかのステージが選択状態なことを確認
			if (spUI && spUI->GetIsSelect())
			{
				// 効果音
				SOUNDMGR.Play(SoundType::SE_Decide);

				//ステージ準備
				if (!m_wpUI.expired())
				{
					SCENEMGR.SetStageNo(m_wpUI.lock()->GetSelectedStageNo());
				}

				//フェードアウト
				FADEMGR.StartFadeOut(&m_isSceneChangeReady);

				// ステージ選択完了
				m_isStageSelected = true;
			}
		}
	}

	// DEBUG
	if (GetAsyncKeyState('T') & 0x8000)
	{
		//フェードインが終わっているかつステージ選択が終わっていないなら
		if (m_isFadeInEnd && !m_isStageSelected)
		{
			// タイトルに
			SceneManager::Instance().SetNextScene
			(
				SceneManager::SceneType::Title
			);

			// BGM停止
			SOUNDMGR.Stop(SoundType::BGM_Select);

			return;
		}
	}
}
