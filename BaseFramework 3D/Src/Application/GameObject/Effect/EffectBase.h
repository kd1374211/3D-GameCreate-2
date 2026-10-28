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
	void SetPos(const Math::Vector3& pos) { m_pos = pos; }
	void SetRot(const Math::Vector3& rot) { m_rot = rot; }
	void SetScaleMulti(float scale) { m_scaleMulti = scale; }

protected:

	// 現在のデータにUVを更新
	void SetUVRect();

	// アニメーションカウント
	float m_animCnt = 0.0f;

	// エフェクト画像データ
	EffectData m_data;

	// 位置
	Math::Vector3 m_pos = Math::Vector3::Zero;

	// 回転
	Math::Vector3 m_rot = Math::Vector3(0.0f, 1.0f, 0.0f);

	// 拡縮
	float m_scaleMulti = 1.0f;

	virtual void Init()override;

	// カメラを向くように設定
	const Math::Vector3& SetRotationYToCamera();
	const Math::Vector3& SetAllRotationToCamera();

};