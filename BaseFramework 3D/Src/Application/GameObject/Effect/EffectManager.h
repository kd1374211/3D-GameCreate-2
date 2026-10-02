#pragma once

enum class EffectType
{
	HitImpact,
	Number
};

// データ
struct EffectData
{
	std::shared_ptr<KdSquarePolygon> m_polygon = nullptr;		// 画像（スクエアポリゴン）
	float m_scale = 1.0f;							// サイズ
	float m_animSpeed = 1.0f;						// アニメーション速度（毎秒）
	float m_animMax = 1.0f;							// アニメーション最大
	bool m_isLoop = false;							// ループするか
};

class EffectManager
{
public:

	// 初期化
	void Init();

	// エフェクト召喚
	void SpawnEffect(Math::Vector3 pos, EffectType type);

	// エフェクトデータ取得
	const EffectData& GetEffectData(EffectType type)const { return m_effectData[(size_t)type]; }

private:

	EffectManager() {}
	~EffectManager() {}

	// データ読み込み
	void LoadData();

	// データ
	EffectData m_effectData[(size_t)EffectType::Number];

public:

	static EffectManager& Instance()
	{
		static EffectManager instance;
		return instance;
	}

};

#define EFFECTMGR EffectManager::Instance()