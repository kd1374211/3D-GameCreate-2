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
	void PostUpdate()override;
	void DrawEffect()override;
	void DrawSprite()override;

private:

	struct ConfettiSpawnerConsts
	{
		// アクティブ時間
		static constexpr float LifeTime = 7.0f;

		// 1F毎の出現確確率
		static constexpr int ConfettiSpawnChance = 10;

		// 各ランダム要素の最大・最小
		static constexpr float ScaleMin = 0.8f;
		static constexpr float ScaleMax = 2.5f;

		static constexpr float PosXMin = -600.0f;
		static constexpr float PosXMax = 600.0f;

		static constexpr float MoveSpeedMin = 70.0f;
		static constexpr float MoveSpeedMax = 150.0f;

		static constexpr float MoveDirMin = -10.0f;
		static constexpr float MoveDirMax = 10.0f;

		static constexpr float RotateSpeedMin = 5.0f;
		static constexpr float RotateSpeedMax = 30.0f;

		static constexpr float ColorTotalValueMin = 1.0f;
	};

	void Init();

	// 召喚
	void SpawnConfetti();

	// リスト
	std::list<std::shared_ptr<Confetti>> m_confettiObjects;

	// 残り召喚時間
	float m_lifeTime = 0.0f;

	// 表示用レンダー
	KdRenderTargetPack m_rtPack;
	KdRenderTargetChanger m_rtChanger;
};