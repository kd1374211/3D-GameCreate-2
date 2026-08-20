#include "FadeManager.h"
#include "../main.h"

void FadeManager::Init()
{
	m_fadeState = FadeState::None;
	m_alpha = 0.0f;
	m_isFade = false;
}

void FadeManager::Update()
{
	//リセット
	m_fadeJustEnded = false;

	float dt = Application::Instance().GetDeltaTime();

	if (m_fadeState == FadeState::FadeOut)
	{
		m_alpha += FadeManagerConsts::AlphaChange * dt;

		if (m_alpha > 1.0f)
		{
			m_alpha = 1.0f;
			m_isFade = false;
			m_fadeState = FadeState::None;
			m_fadeJustEnded = true;
		}
	}
	else if (m_fadeState == FadeState::FadeIn)
	{
		m_alpha -= FadeManagerConsts::AlphaChange * dt;

		if (m_alpha < 0.0f)
		{
			m_alpha = 0.0f;
			m_isFade = false;
			m_fadeState = FadeState::None;
			m_fadeJustEnded = true;
		}
	}
}

void FadeManager::DrawFade()
{
	Math::Color color = Math::Color(0, 0, 0, m_alpha);
	KdShaderManager::Instance().m_spriteShader.DrawBox(0, 0, 1280, 720, &color, true);
}

void FadeManager::StartFadeIn()
{
	//フェードが終わっていないならリターン
	if (m_isFade)return;

	m_fadeState = FadeState::FadeIn;
	m_isFade = true;
}

void FadeManager::StartFadeOut()
{
	//フェードが終わっていないならリターン
	if (m_isFade)return;

	m_fadeState = FadeState::FadeOut;
	m_isFade = true;
}
