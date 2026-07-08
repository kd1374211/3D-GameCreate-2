#include "KdFont.h"

// 日本語判定
static bool isSJIS(char a)
{
	return ((BYTE)a >= 0x81 && (BYTE)a <= 0x9f || (BYTE)a >= 0xe0 && (BYTE)a <= 0xfc);
}

// フォント作成
static HFONT MakeFont(const std::string& fontName, int h, int angle)
{
	HFONT hFont;
	hFont = CreateFont(h,		//フォント高さ
		0,						//文字幅
		angle,					//テキストの角度
		0,						//ベースラインとｘ軸との角度
		FW_REGULAR,				//フォントの重さ（太さ）
		FALSE,					//イタリック体
		FALSE,					//アンダーライン
		FALSE,					//打ち消し線
		SHIFTJIS_CHARSET,		//文字セット
		OUT_DEFAULT_PRECIS,		//出力精度
		CLIP_DEFAULT_PRECIS,	//クリッピング精度
		PROOF_QUALITY,			//出力品質
		FIXED_PITCH | FF_MODERN,//ピッチとファミリー
		fontName.c_str());		//書体名

	return hFont;
}

void KdFontSprite::CreateFontTexture(HDC hdc, const std::string& text, int antiAliasing, std::array<std::shared_ptr<KdFontData>, 65536>* pFontDataArray, bool bAdd)
{
	if (bAdd == false)		Release();
	if (text.size() == 0)	return;

	const char* pT = text.c_str();

	m_String += text;

	if (m_TexList.size() < text.size())
	{
		m_TexList.reserve(text.size());
	}

	while(1)
	{
		uint16_t code = 0;

		// 文字の最後
		if(pT[0] == '\0')break;

		// 日本語判定
		bool b2byte = false;
		if(isSJIS(pT[0]))
		{
			b2byte = true;
			code = (BYTE)pT[0] << 8 | (BYTE)pT[1];
		}
		else
		{
			code = pT[0];
		}

		bool bCreate = true;

		// 既に作成済みのデータかどうかを検索
		if (pFontDataArray)
		{
			if ((*pFontDataArray)[code])
			{
				bCreate = false;
				m_TexList.push_back((*pFontDataArray)[code]);
			}
		}

		// 作成
		if (bCreate)
		{
			// データ追加
			auto data = std::make_shared<KdFontData>();
			m_TexList.push_back(data);

			data->Code = code;

			int grad = 0; // 階調の最大値
			int gradFlag = GGO_BITMAP;
			switch(antiAliasing)
			{
				case 0:
					gradFlag = GGO_BITMAP;
					break;
				case 1:
					gradFlag = GGO_GRAY2_BITMAP;
					break;
				case 2:
					gradFlag = GGO_GRAY4_BITMAP;
					break;
				case 3:
					gradFlag = GGO_GRAY8_BITMAP;
					break;
			}

			switch(gradFlag)
			{
				case GGO_GRAY2_BITMAP:
					grad =  4;
					break;
				case GGO_GRAY4_BITMAP:
					grad = 16;
					break;
				case GGO_GRAY8_BITMAP:
					grad = 64;
					break;
			}

			//-------------------------------
			// フォントビットマップ取得
			//-------------------------------
			TEXTMETRIC TM;
			GetTextMetrics(hdc , &TM );
			GLYPHMETRICS GM;
			CONST MAT2 Mat = {{0,1},{0,0},{0,0},{0,1}};
			DWORD size = GetGlyphOutline(hdc, code, gradFlag, &GM, 0, NULL, &Mat);	// アンチエイリアスの時に、spaceとか0が返るのはなぜ…
			std::unique_ptr<BYTE[]> ptr(new BYTE[size]);
			GetGlyphOutline(hdc, code, gradFlag, &GM, size, ptr.get(), &Mat);


			int addX = 0;
			if(GM.gmptGlyphOrigin.x < 0)
			{
				addX = -GM.gmptGlyphOrigin.x;
				GM.gmptGlyphOrigin.x = 0;
			}

			// 文字サイズ
			int texWidth				= GM.gmCellIncX;
			int texHeight				= TM.tmHeight;
			int fontWidth_Alignment4	= (GM.gmBlackBoxX + 3) / 4 * 4;

			// 文字のバイト数
			if(b2byte)data->Bytes		= 2;
			else data->Bytes			= 1;

			// 総幅加算
			m_TotalWidth += texWidth;

			static const int PIXEL_BYTES = 4;
			// バッファ作成
			std::unique_ptr<UINT> buf(new UINT[texWidth*texHeight]);
			ZeroMemory(buf.get(), texWidth*texHeight*4);

			// フォント画像コピー
			if (size > 0)
			{
				int sx, sy, dx, dy;
				DWORD alpha;
				int bBroken = 0;
				int bytePos;

				// アンチエイリアス無し
				if (gradFlag == GGO_BITMAP)
				{
					int iUseBYTEparLine = (1 + (GM.gmBlackBoxX / 32)) * 4; // 元絵の1行のバイト数
					int total = GM.gmBlackBoxY*iUseBYTEparLine;
					// 半分になってる場合
					if ((unsigned)(total / 2) == size)
					{
						bBroken = 1;
					}
					else if ((unsigned)(total) != size)
					{
						bBroken = 2;
					}

					// テクスチャに書き込み
					for (int y = 0; y < (int)GM.gmBlackBoxY; y++)
					{
						for (int x = 0; x < (int)GM.gmBlackBoxX; x++)
						{
							sx = x + addX;
							sy = y;
							if (sx < (int)GM.gmBlackBoxX && sy < (int)GM.gmBlackBoxY)
							{
								alpha = 0;
								// なぜか高さが半分になってるやつ
								if (bBroken == 1)
								{
									sy /= 2;
								}

								int num = sx / 8;		// 何バイト目か
								BYTE bit = sx % 8;		// 何ビット目か
								BYTE mask = ((BYTE)1) << (7 - bit);
								bytePos = num + sy*iUseBYTEparLine;
								if (bytePos < (int)size)
								{
									BYTE Cur = ptr[bytePos];
									Cur &= mask;
									alpha = (Cur >> (7 - bit)) * 255;
								}

								dx = (x + GM.gmptGlyphOrigin.x);
								dy = (y + (TM.tmAscent - GM.gmptGlyphOrigin.y));
								if (dx >= 0 && dx < texWidth && dy >= 0 && dy < texHeight)
								{
									buf.get()[dx + dy*texWidth] = (alpha << 24) | 0x00ffffff;
								}
							}
						}
					}
				}
				// アンチエイリアス有り
				else
				{
					// テクスチャに書き込み
					for (int y = 0; y < (int)GM.gmBlackBoxY; y++)
					{
						for (int x = 0; x < (int)GM.gmBlackBoxX; x++)
						{
							sx = x + addX;
							sy = y;
							if (sx < (int)GM.gmBlackBoxX && sy < (int)GM.gmBlackBoxY)
							{
								alpha = 0;

								bytePos = sy*fontWidth_Alignment4 + sx;
								if (bytePos < (int)size)
								{
									alpha = ptr[bytePos] * 255 / grad;
								}

								dx = (x + GM.gmptGlyphOrigin.x);
								dy = (y + (TM.tmAscent - GM.gmptGlyphOrigin.y));
								if (dx >= 0 && dx < texWidth && dy >= 0 && dy < texHeight)
								{
									buf.get()[dx + dy*texWidth] = (alpha << 24) | 0x00ffffff;
								}
							}
						}
					}
				}
			}

			// テクスチャ作成
			data->FontTex = std::make_shared<KdTexture>();
			data->FontTex->Create(texWidth, texHeight, DXGI_FORMAT_B8G8R8A8_UNORM);
			KdDirect3D::Instance().WorkDevContext()->UpdateSubresource(data->FontTex->WorkResource(), 0, nullptr, buf.get(), data->FontTex->GetInfo().Width * 4, 0);

			// 登録
			if (pFontDataArray)
			{
				(*pFontDataArray)[code] = data;
			}

		}

		// 進める
		if(b2byte)
		{
			pT += 2;
		}
		else
		{
			pT += 1;
		}

	}
}

void KdFontManager::Init(HWND hWnd)
{
	m_LoadedFontMap.clear();

	m_hWnd	= hWnd;
	m_hDC	= GetDC(m_hWnd);
}

void KdFontManager::Release()
{
	for(auto& font : m_FontTbl)
	{
		DeleteObject(font.hFont);
		font.hFont = nullptr;
		font.CreatedFontDataTbl.fill(nullptr);
	}

	if(m_hDC)
	{
		ReleaseDC(m_hWnd, m_hDC);
	}

}

void KdFontManager::AddFont(int fontNo, const std::string& fontName, int h)
{
	HFONT hFont = MakeFont(fontName.c_str(), h, 0);
	m_FontTbl[fontNo].hFont = hFont;
	m_FontTbl[fontNo].CreatedFontDataTbl.fill(nullptr);
}

std::shared_ptr<KdFontSprite> KdFontManager::CreateFontTexture(int fontNo, const std::string& text, int antiAliasingFlag)
{
	std::shared_ptr<KdFontSprite> add = std::make_shared<KdFontSprite>();

	// フォント選択
	HFONT hFontOld = (HFONT)SelectObject(m_hDC, (HGDIOBJ)m_FontTbl[fontNo].hFont);
	// フォントテクスチャ作成
	add->CreateFontTexture(m_hDC, text, antiAliasingFlag, &m_FontTbl[fontNo].CreatedFontDataTbl, false);
	// フォント選択を基に戻す
	SelectObject(m_hDC, hFontOld);

	return add;
}

void KdFontManager::AddFontResource(const std::string& ttfFileName)
{
	// すでに存在
	auto itFound = m_LoadedFontMap.find(ttfFileName);
	if(itFound != m_LoadedFontMap.end())
	{
		return;
	}

	AddFontResourceEx(ttfFileName.c_str(), FR_PRIVATE, NULL);

	m_LoadedFontMap[ttfFileName] = 1;
}

void KdFontManager::RemoveAllFontResource()
{
	for(auto& node : m_LoadedFontMap)
	{
		RemoveFontResourceEx(node.first.c_str(), FR_PRIVATE, NULL);
	}
}