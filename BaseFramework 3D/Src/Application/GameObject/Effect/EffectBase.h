#pragma once
#include "EffectManager.h"

class EffectBase :public KdGameObject
{
public:

	EffectBase() { Init(); }
	~EffectBase() {}

	virtual void Update()override;
	virtual void PreDraw()override;
	virtual void DrawEffect()override;

	// セッター
	void SetPos(Math::Vector3 pos) { m_pos = pos; }

protected:

	// アニメーションカウント
	float m_animCnt = 0.0f;

	// エフェクト画像データ
	EffectData m_data;

	// 位置
	Math::Vector3 m_pos = Math::Vector3::Zero;

	virtual void Init()override;

	Math::Matrix SetRotationToCamera();

	void UpdateRotate();

};