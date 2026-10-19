#pragma once

enum class EffectType
{
	HitImpact,
	LandingAndWallHit,
	Confetti,
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

class ConfettiSpawner;
class EffectBase;

class EffectManager
{
public:

	// 初期化
	void Init();

	// 消滅確認
	void PreUpdate();

	// エフェクト召喚
	void SpawnEffect(const Math::Vector3& pos, const Math::Vector3& rot, float scaleMulti, EffectType type);

	// エフェクト全削除
	void ClearActiveEffect();

	// エフェクトデータ取得
	const EffectData& GetEffectData(EffectType type)const { return m_effectData[(size_t)type]; }

	// 仮置きテクスチャ取得
	const std::shared_ptr<KdTexture> GetSquareTexture()const { return m_sqTex; }

private:

	EffectManager() {}
	~EffectManager() {}

	// データ読み込み
	void LoadData();

	// 召喚
	void SpawnConfettiSpawner();

	// リストの削除チェック
	void CleanActiveList();

	// データ
	EffectData m_effectData[(size_t)EffectType::Number];

	// エフェクトリスト
	std::list<std::weak_ptr<EffectBase>> m_wpEffectList;

	// コンフェッティ召喚用
	std::weak_ptr<ConfettiSpawner> m_cfSpawner;

	// 仮置きテクスチャ
	std::shared_ptr<KdTexture> m_sqTex = nullptr;

public:

	static EffectManager& Instance()
	{
		static EffectManager instance;
		return instance;
	}

};

#define EFFECTMGR EffectManager::Instance()