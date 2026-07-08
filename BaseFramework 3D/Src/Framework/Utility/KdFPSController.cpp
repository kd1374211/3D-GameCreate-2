#include "KdFPSController.h"

// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
// FPSの制御コントローラー
// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
void KdFPSController::Init()
{
	m_fpsMonitorBeginTime = timeGetTime();
}

void KdFPSController::UpdateStartTime()
{
	m_frameStartTime = timeGetTime();
}

void KdFPSController::Update()
{
	Control();

	// デルタ時間の計算
	DWORD _currentTime = timeGetTime();
	m_DeltaTime = (_currentTime - m_frameStartTime) / static_cast<float>(kSecond);

	Monitoring();
}

// FPS制御
void KdFPSController::Control()
{
	// 処理終了時間Get
	DWORD frameProcessEndTime = timeGetTime();

	// 1フレームで経過すべき時間
	DWORD timePerFrame = kSecond / m_maxFps;

	if (frameProcessEndTime - m_frameStartTime < timePerFrame)
	{
		// 1秒間にMaxFPS回数以上処理が回らないように待機する
		Sleep(timePerFrame - (frameProcessEndTime - m_frameStartTime));
	}
}

// 現在のFPS計測
void KdFPSController::Monitoring()
{
	// FPS計測のタイミング　0.5秒おき
	constexpr float kFpsRefreshFrame = 500;		

	m_fpsCnt++;

	// 0.5秒おきに FPS計測
	if (m_frameStartTime - m_fpsMonitorBeginTime >= kFpsRefreshFrame)
	{
		// 現在のFPS算出
		m_nowfps = (m_fpsCnt * kSecond) / (m_frameStartTime - m_fpsMonitorBeginTime);

		m_fpsMonitorBeginTime = m_frameStartTime;

		m_fpsCnt = 0;
	}
}
