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
}

void ConfettiSpawner::Update()
{
	// gdt取得
	float gameDt = SCENEMGR.GetDeltaGameTime();

	
	// 確率で召喚
	if (KdRandom::GetInt(1, ConfettiSpawnerConsts::ConfettiSpawnChance) == 1)
	{
		SpawnConfetti();
	}

	// 各Update召喚
	for (auto& obj : m_confettiObjects)
	{
		obj->Update();
	}

	// DEBUG
	KdDebugGUI::Instance().AddLog("ConfettiCnt : %d\n", m_confettiObjects.size());
}

void ConfettiSpawner::PreDraw()
{
	// 各PreDraw召喚
	for (auto& obj : m_confettiObjects)
	{
		obj->PreDraw();
	}
}

void ConfettiSpawner::DrawSprite()
{
	// レンダー召喚
	m_rtPack.ClearTexture(Math::Color(0.0f, 0.0f, 0.0f, 0.0f));
	m_rtChanger.ChangeRenderTarget(m_rtPack);

	// CULLNONEに
	KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);

	// 各DrawSprite召喚
	for (auto& obj : m_confettiObjects)
	{
		obj->DrawSprite();
	}

	// 戻す
	m_rtChanger.UndoRenderTarget();
	KdShaderManager::Instance().UndoRasterizerState();

	// レンダーをここで書く
	std::shared_ptr<KdTexture> m_rtTex = m_rtPack.m_RTTexture;
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_rtTex, 0, 0);
}

void ConfettiSpawner::Init()
{
	// レンダー召喚
	m_rtPack.CreateRenderTarget(1280, 720, false);
}

void ConfettiSpawner::SpawnConfetti()
{
	// 用意
	std::shared_ptr<Confetti> confetti = std::make_shared<Confetti>();

	ConfettiParam param = {};

	// ランダムにサイズ、初期位置(X)、落下速度、回転速度、色をセット
	param.m_scale = KdRandom::GetFloat(ConfettiSpawnerConsts::ScaleMin, ConfettiSpawnerConsts::ScaleMax);
	param.m_startPos.x = KdRandom::GetFloat(ConfettiSpawnerConsts::PosXMin, ConfettiSpawnerConsts::PosXMax);
	param.m_startPos.y = ConfettiSpawnerConsts::StartPosY;
	param.m_fallSpeed = KdRandom::GetFloat(ConfettiSpawnerConsts::MoveSpeedMin, ConfettiSpawnerConsts::MoveSpeedMax);
	param.m_fallDir = KdRandom::GetFloat(ConfettiSpawnerConsts::MoveDirMin, ConfettiSpawnerConsts::MoveDirMax);
	param.m_rotatSpeed = KdRandom::GetFloat(ConfettiSpawnerConsts::RotateSpeedMin, ConfettiSpawnerConsts::RotateSpeedMax);

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
