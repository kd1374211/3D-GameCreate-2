#include "Button.h"
#include "../../../Cursor/CursorManager.h"
#include "../../../main.h"
#include "../../../Const/DeviceAndKey.h"
#include "../../../Const/WindowConsts.h"

void Button::Update()
{
	// ボタンが有効でないならリターン
	if (!m_isEnable)return;

	// dt取得
	float dt = Application::Instance().GetDeltaTime();

	// カーソルが自分の判定内にあるか確認
	if(fabs(CursorManager::Instance().GetFixedCursorPosVec2().x - m_drawPos.x) < HITSIZE.x &&
	   fabs(CursorManager::Instance().GetFixedCursorPosVec2().y - m_drawPos.y) < HITSIZE.y)
	{
		m_isCursor = true;
	}
	else
	{
		m_isCursor = false;
	}

	// カーソルが乗っているなら拡大
	if (m_isCursor)
	{
		m_sizeMulti = std::min(m_sizeMulti + ButtonConsts::SizeChangeSpeed * dt, ButtonConsts::MaxSizeMulti);
	}
	// 載っていないなら縮小
	else
	{
		m_sizeMulti = std::max(m_sizeMulti - ButtonConsts::SizeChangeSpeed * dt, ButtonConsts::MinSizeMulti);
	}

	// ボタンが押されたか確認
	if (m_isCursor && KdInputManager::Instance().IsPress(GetKeyRegistName(VK_LBUTTON)))
	{
		// ボタンが押されたことを通知
		*m_onClickFlg = true;
	}
}

void Button::DrawSprite()
{
	// 現在のレンダーターゲットを保存
	ID3D11RenderTargetView* spCurrentRT = nullptr;
	ID3D11DepthStencilView* spCurrentDS = nullptr;
	KdDirect3D::Instance().WorkDevContext()->OMGetRenderTargets(1, &spCurrentRT, &spCurrentDS);

	// レンダー作成
	std::shared_ptr<KdTexture> tmpTex = std::make_shared<KdTexture>();
	Math::Vector2 renderBase = WindowSizeConsts::WindowSize;
	tmpTex->CreateRenderTarget(renderBase.x, renderBase.y);

	//透明塗り
	KdDirect3D::Instance().WorkDevContext()->ClearRenderTargetView(tmpTex->WorkRTView(), Math::Color(0, 0, 0, 0));

	//ターゲット設定
	KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, tmpTex->WorkRTViewAddress(), tmpTex->WorkDSView());

	// ボタン描画
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_buttonTex, 0, 0);

	// テキスト表示
	KdShaderManager::Instance().m_spriteShader.DrawFont(FontTypeConst::Other_ButtonText, Math::Vector2::Zero, &kWhiteColor, m_buttonText.c_str(), TextAlign::Center);

	//ターゲット戻す
	KdDirect3D::Instance().WorkDevContext()->OMSetRenderTargets(1, &spCurrentRT, spCurrentDS);

	//tmpTexをサイズ変えて描画
	Math::Vector2 drawSize = renderBase * m_drawScale * m_sizeMulti;
	KdShaderManager::Instance().m_spriteShader.DrawTex(tmpTex, m_drawPos.x, m_drawPos.y, drawSize.x, drawSize.y);

	// 仮保存したレンダーターゲットを解放
	if (spCurrentRT)spCurrentRT->Release();
	if (spCurrentDS)spCurrentDS->Release();
}

void Button::Init()
{
	// ボタンのテクスチャをロード
	m_buttonTex = std::make_shared<KdTexture>();
	m_buttonTex->Load("Asset/Textures/UI/Buttons/KariButton.png");
}
