#include "GameUIObjects.h"

#include "../../../../main.h"

void GameUIObjects::Update()
{
	//デルタタイム
	float dt = Application::Instance().GetDeltaTime();

	//流れるテキストアップデート
	if (m_movingTexts.m_isActive)
	{
		//未来を見る
		float currentActiveTime = m_movingTexts.m_activeTime;
		
		//移動距離用意
		float moveDist = 0.0f;

		if (currentActiveTime < GameUIConsts::SlowMoveStartSec)
		{
			float fastMoveTime = std::clamp(dt, 0.0f, GameUIConsts::SlowMoveStartSec - currentActiveTime);
			float slowMoveTime = dt - fastMoveTime;

			//高速
			moveDist += GameUIConsts::MoveSpeedFast * fastMoveTime;
			//低速
			moveDist += GameUIConsts::MoveSpeedSlow * slowMoveTime;
		}
		else if (currentActiveTime < GameUIConsts::SlowMoveEndSec)
		{
			float slowMoveTime = std::clamp(dt, 0.0f, GameUIConsts::SlowMoveEndSec - currentActiveTime);
			float fastMoveTime = dt - slowMoveTime;

			//高速
			moveDist += GameUIConsts::MoveSpeedFast * fastMoveTime;
			//低速
			moveDist += GameUIConsts::MoveSpeedSlow * slowMoveTime;
		}
		else
		{
			moveDist += GameUIConsts::MoveSpeedFast * dt;
		}

		//座標更新
		m_movingTexts.m_posX += moveDist;

		//時間更新
		m_movingTexts.m_activeTime += dt;

		//終了
		if (m_movingTexts.m_activeTime >= GameUIConsts::MovingTextActiveSec)m_movingTexts.m_isActive = false;
	}
}

void GameUIObjects::DrawSprite()
{
	//流れテキスト
	if (m_movingTexts.m_isActive)
	{
		Math::Vector2 drawPos = Math::Vector2(m_movingTexts.m_posX, GameUIConsts::MovingTextPosY);
		KdShaderManager::Instance().m_spriteShader.DrawFont(2, GameUIConsts::MovingTextSize, drawPos, &m_movingTexts.m_color, m_movingTexts.m_text.c_str());
	}

	//タイマー表示開始前はスキップ
	if (m_isTimerDraw) 
	{
		//仮に99は超えないようにする
		int drawTime = std::clamp(m_time, 0, 99);

		//位分解
		int digitTen = drawTime / 10;
		int digitOne = drawTime % 10;

		//10の位
		Math::Rectangle rec = Math::Rectangle(0, (long)(12 * digitTen), 12, 12);
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_timerTex, -42, 318, 84, 84, &rec);

		//1の位
		rec = Math::Rectangle(0, (long)(12 * digitOne), 12, 12);
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_timerTex, 42, 318, 84, 84, &rec);
	}
}

void GameUIObjects::SpawnMovingText(std::string text, Math::Color color)
{
	//既にあっても上書き
	//値設定・有効化
	m_movingTexts.m_posX = GameUIConsts::MovingTextStartX;
	m_movingTexts.m_text = text;
	m_movingTexts.m_color = color;
	m_movingTexts.m_activeTime = 0.0f;
	m_movingTexts.m_isActive = true;
}

void GameUIObjects::Init()
{
	m_timerTex = std::make_shared<KdTexture>();
	m_timerTex->Load("Asset/Textures/UI/SceneUI/Game/Number.png");

	m_movingTexts.m_isActive = false;
}
