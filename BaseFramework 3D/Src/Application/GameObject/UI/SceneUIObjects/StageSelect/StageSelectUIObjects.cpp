#include "StageSelectUIObjects.h"
#include "../../../../StageManager/StageManager.h"
#include "../../../../Scene/SceneManager.h"

void StageSelectUIObject::Update()
{
	static bool isUpKey = true;
	static bool isDownKey = true;
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		if (!isUpKey)
		{
			m_selectStageNo--;
			if (m_selectStageNo < m_minStageNo)
			{
				m_selectStageNo = m_maxStageNo;
			}

			isUpKey = true;
		}
	}
	else isUpKey = false;

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		if (!isDownKey)
		{
			m_selectStageNo++;
			if (m_selectStageNo > m_maxStageNo)
			{
				m_selectStageNo = m_minStageNo;
			}

			isDownKey = true;
		}
	}
	else isDownKey = false;

	KdDebugGUI::Instance().AddLog("SelectStage : %d\n", m_selectStageNo);
}

void StageSelectUIObject::DrawSprite()
{
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &kBlackColor, true);

	//仮画像
	//KdShaderManager::Instance().m_spriteShader.DrawTex(m_stageSelectTex, 0, 0, 1280, 720);

	//TEST
	KdShaderManager::Instance().m_spriteShader.DrawFont(1, 24.0f, Math::Vector2::Zero, &kWhiteColor, "TEST");
	KdShaderManager::Instance().m_spriteShader.DrawFont(1, 48.0f, Math::Vector2(0, -100.0f), &kWhiteColor, "テスト");
}

void StageSelectUIObject::Init()
{
	//画像ロード
	m_stageSelectTex = std::make_shared<KdTexture>();
	m_stageSelectTex->Load("Asset/Textures/UI/SceneUI/StageSelect/kStageSelect.png");

	//現在選択・最大・最小選択ステージ番号を取得
	m_selectStageNo = SCENEMGR.GetStageNo();
	m_maxStageNo = STAGEMGR.GetMaxStageNo();
	m_minStageNo = STAGEMGR.GetMinStageNo();
}
