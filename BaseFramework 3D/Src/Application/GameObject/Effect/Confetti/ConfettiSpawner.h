#pragma once

class Confetti;

// コンフェッティを召喚するだけのクラス
class ConfettiSpawner :public KdGameObject
{
public:

	ConfettiSpawner() { Init(); }
	~ConfettiSpawner()override {}

	void PreUpdate()override;
	void Update()override;
	void PreDraw()override;
	void DrawSprite()override;

private:

	struct ConfettiSpawnerConsts
	{
		// 1F毎の出現確確率
		static constexpr int ConfettiSpawnChance = 8;

		// 各ランダム要素の最大・最小
		static constexpr float ScaleMin = 0.5f;
		static constexpr float ScaleMax = 0.9f;

		static constexpr float PosXMin = -600.0f;
		static constexpr float PosXMax = 600.0f;

		static constexpr float MoveSpeedMin = 70.0f;
		static constexpr float MoveSpeedMax = 150.0f;

		static constexpr float MoveDirMin = -10.0f;
		static constexpr float MoveDirMax = 10.0f;

		static constexpr float RotateSpeedMin = 10.0f;
		static constexpr float RotateSpeedMax = 70.0f;

		static constexpr float ColorTotalValueMin = 1.0f;

		// 召喚位置
		static constexpr float StartPosY = 380.0f;
	};

	void Init();

	// 召喚
	void SpawnConfetti();

	// リスト
	std::list<std::shared_ptr<Confetti>> m_confettiObjects;

	// 表示用レンダー
	KdRenderTargetPack m_rtPack;
	KdRenderTargetChanger m_rtChanger;
};