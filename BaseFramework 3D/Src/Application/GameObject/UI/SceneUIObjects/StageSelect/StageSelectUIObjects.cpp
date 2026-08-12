#include "StageSelectUIObjects.h"
#include "../../../../StageManager/StageManager.h"
#include "../../../../Scene/SceneManager.h"

void StageSelectUIObject::Update()
{
	static bool isLeftKey = true;
	static bool isRightKey = true;
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		if (!isLeftKey)
		{
			m_selectStageNo--;
			if (m_selectStageNo < MINSTAGENO)
			{
				m_selectStageNo = m_maxStageNo;
			}

			isLeftKey = true;
		}
	}
	else isLeftKey = false;

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		if (!isRightKey)
		{
			m_selectStageNo++;
			if (m_selectStageNo > m_maxStageNo)
			{
				m_selectStageNo = MINSTAGENO;
			}

			isRightKey = true;
		}
	}
	else isRightKey = false;

	KdDebugGUI::Instance().AddLog("SelectStage : %d\n", m_selectStageNo);
}

void StageSelectUIObject::DrawSprite()
{
	//仮画像
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_stageSelectTex, 0, 0, 1280, 720);
}

void StageSelectUIObject::Init()
{
	//画像ロード
	m_stageSelectTex = std::make_shared<KdTexture>();
	m_stageSelectTex->Load("Asset/Textures/UI/SceneUI/StageSelect/kStageSelect.png");

	//現在選択・最大選択ステージ番号を取得
	m_selectStageNo = SCENEMGR.GetStageNo();
	m_maxStageNo = STAGEMGR.GetMaxStageNo();
}
