#include "ConfettiSpawner.h"
#include "Confetti.h"
#include "../../../Scene/SceneManager.h"

void ConfettiSpawner::PreUpdate()
{
	// Updateの前の更新処理
	// オブジェクトリストの整理 ・・・ 無効なオブジェクトを削除
	auto it = m_confettiObjects.begin();

	while (it != m_confettiObjects.end())
	{
		if ((*it)->IsExpired())	// IsExpired() ・・・ 無効ならtrue
		{
			// 無効なオブジェクトをリストから削除
			it = m_confettiObjects.erase(it);
		}
		else
		{
			++it;	// 次の要素へイテレータを進める
		}
	}

	// ライフタイムが無いかつオブジェクトが無くなれば消去
	if (m_confettiObjects.empty() && m_lifeTime <= 0.0f)
	{
		SetExpire();
	}
}

void ConfettiSpawner::Update()
{
	// gdt取得
	float gameDt = SCENEMGR.GetDeltaGameTime();

	// ライフタイム減少
	m_lifeTime -= gameDt;
	if (m_lifeTime <= 0.0f)m_lifeTime = 0.0f;

	// ライフタイムが残っているなら確率で召喚
	if (m_lifeTime > 0.0f)
	{
		if (KdRandom::GetInt(1, ConfettiSpawnerConsts::ConfettiSpawnChance) == 1)
		{
			SpawnConfetti();
		}
	}

	// 各Update召喚
	for (auto& obj : m_confettiObjects)
	{
		obj->Update();
	}

	// DEBUG
	KdDebugGUI::Instance().AddLog("ConfettiCnt : %d\n", m_confettiObjects.size());
}

void ConfettiSpawner::PostUpdate()
{
	// 各PostUpdate召喚
	for (auto& obj : m_confettiObjects)
	{
		obj->PostUpdate();
	}
}

void ConfettiSpawner::DrawEffect()
{
	// レンダー召喚
	m_rtPack.ClearTexture(Math::Color(0.0f, 0.0f, 0.0f, 0.0f));
	m_rtChanger.ChangeRenderTarget(m_rtPack);

	// CULLNONEに
	KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);

	// 各DrawEffect召喚
	for (auto& obj : m_confettiObjects)
	{
		obj->DrawEffect();
	}

	// 戻す
	m_rtChanger.UndoRenderTarget();
	KdShaderManager::Instance().UndoRasterizerState();
}

void ConfettiSpawner::DrawSprite()
{
	// レンダーをここで書く
	std::shared_ptr<KdTexture> m_rtTex = m_rtPack.m_RTTexture;
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_rtTex, 0, 0);
}

void ConfettiSpawner::Init()
{
	// レンダー召喚
	m_rtPack.CreateRenderTarget(1280, 720, true);

	// アクティブ時間設定
	m_lifeTime = ConfettiSpawnerConsts::LifeTime;
}

void ConfettiSpawner::SpawnConfetti()
{
	// 用意
	std::shared_ptr<Confetti> confetti = std::make_shared<Confetti>();

	ConfettiParam param = {};

	// ランダムにサイズ、初期位置(X)、落下速度、回転速度、色をセット
	param.m_scale = KdRandom::GetFloat(ConfettiSpawnerConsts::ScaleMin, ConfettiSpawnerConsts::ScaleMax);
	param.m_startPosX = KdRandom::GetFloat(ConfettiSpawnerConsts::PosXMin, ConfettiSpawnerConsts::PosXMax);
	param.m_fallSpeed = KdRandom::GetFloat(ConfettiSpawnerConsts::MoveSpeedMin, ConfettiSpawnerConsts::MoveSpeedMax);
	param.m_fallDir = KdRandom::GetFloat(ConfettiSpawnerConsts::MoveDirMin, ConfettiSpawnerConsts::MoveDirMax);
	param.m_rotatSpeed = KdRandom::GetFloat(ConfettiSpawnerConsts::RotateSpeedMin, ConfettiSpawnerConsts::RotateSpeedMax);
	param.m_rotatDir.x = KdRandom::GetFloat(-1.0f, 1.0f);
	param.m_rotatDir.y = KdRandom::GetFloat(-1.0f, 1.0f);
	param.m_rotatDir.z = KdRandom::GetFloat(-1.0f, 1.0f);

	// 色は合計値が少なくとも１を超えるように
	float R = KdRandom::GetFloat(0.0f, 1.0f);
	float G = KdRandom::GetFloat(0.0f, 1.0f);
	float B = KdRandom::GetFloat(0.0f, 1.0f);
	float colValueTotal = R + G + B;

	// 足りなかったら
	if (colValueTotal <= ConfettiSpawnerConsts::ColorTotalValueMin)
	{
		// 足りない量を取得
		float needVal = ConfettiSpawnerConsts::ColorTotalValueMin - colValueTotal;

		// RGBのどれかに分配
		switch (KdRandom::GetInt(1, 3))
		{
		case 1:
			R += needVal;
			break;
		case 2:
			G += needVal;
			break;
		case 3:
			B += needVal;
			break;
		}
	}

	// 色に格納
	param.m_color = Math::Color(R, G, B, 1.0f);

	// パラメータ設定
	confetti->SetConfettiParam(param);

	// リストに追加
	m_confettiObjects.push_back(confetti);
}
