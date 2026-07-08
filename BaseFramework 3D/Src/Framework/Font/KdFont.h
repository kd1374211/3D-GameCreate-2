#pragma once

//=====================================================================
// 文字のデータ
//
// 1文字のデータ
//=====================================================================
struct KdFontData
{
	std::shared_ptr<KdTexture>	FontTex	= nullptr;	// 文字テクスチャ
	char						Bytes		= 1;	// 1:1バイト文字 2:2バイト文字
	uint16_t					Code		= 0;	// 文字コード

	// テクスチャの使用容量取得
	UINT DEBUG_GetBytes()
	{
		if (FontTex)
		{
			return FontTex->GetInfo().Width * FontTex->GetInfo().Height * 4;
		}
		return 0;
	}
};

//=====================================================================
// フォントスプライトクラス
//
// 文字列をテクスチャとして生成することができる
//=====================================================================
class KdFontSprite{
	struct TexData;

public:

	KdFontSprite() {}
	~KdFontSprite() { Release(); }

	// 文字リスト取得
	std::vector<std::shared_ptr<KdFontData>>&	GetTexList()	{ return m_TexList;		}
	// 文字列の総幅を取得
	int											GetTotalWidth()	{ return m_TotalWidth;	}
	// 文字列取得
	std::string									GetString()		{ return m_String;		}

	//------------------------------------------------------------
	// 解放
	//------------------------------------------------------------
	void Release()
	{
		m_TexList.clear();

		m_TotalWidth	= 0;
		m_String		= "";
	}

	//------------------------------------------------------------
	// 文字列からフォントテクスチャリストを作成
	// hdc			… fontが設定されているDCを指定。このfontが使用される。
	// antiAliasing	… アンチエイリアシングフラグ 0:なし 1:2x 2:4x 3:8x
	// pFontDataMap	… 作成したフォントデータの記憶/取得を行うMap。nullptrだと使用しない(必ずテクスチャ作成となる)
	// bAdd			… データを追加する？
	//------------------------------------------------------------
	void CreateFontTexture(HDC hdc, const std::string& text, int antiAliasing = 0, std::array<std::shared_ptr<KdFontData>, 65536>* pFontDataArray = nullptr, bool bAdd = false);

private:

	std::vector<std::shared_ptr<KdFontData>>	m_TexList{};		// テクスチャリスト
	std::string									m_String{};			// 文字列
	int											m_TotalWidth = 0;	// 文字列の総幅
};

//=====================================================================
// フォント管理マネージャクラス
//
// フォントの読み込みや、KdFontSpriteの生成を行う
// フォントの作成を毎回行うとかなり処理が遅くなるので、
// 一度作成したテクスチャを使い回せるようにしています
//=====================================================================
class KdFontManager
{
public:

	//------------------------------------------------------------
	// 初期設定
	//[in] hWnd		… ウィンドウのハンドル
	//------------------------------------------------------------
	void Init(HWND hWnd);

	//------------------------------------------------------------
	// 解放
	//------------------------------------------------------------
	void Release();

	//------------------------------------------------------------
	// 指定名のフォントを、指定番号へ登録する
	//[in] fontNo		… 登録するIndex番号
	//[in] fontName		… 追加するフォント名
	//[in] h			… フォントの大きさ(高さ)
	//------------------------------------------------------------
	void AddFont(int fontNo, const std::string& fontName, int h);

	//------------------------------------------------------------
	// 指定番号のフォントを使用し、文字列をフォントテクスチャとして生成する
	//[in] fontNo			… 使用する登録フォントのIndex番号
	//[in] text				… 文字列
	//[in] antiAliasingFlag	… アンチエイリアシングを行うか？ 0:なし 1:2x 2:4x 3:8x
	//return 生成されたテクスチャ情報
	//------------------------------------------------------------
	std::shared_ptr<KdFontSprite> CreateFontTexture(int fontNo, const std::string& text, int antiAliasingFlag);

	//------------------------------------------------------------
	// ttfファイルを読み込み、リソースとして登録する
	//------------------------------------------------------------
	void AddFontResource(const std::string& ttfFileName);

	//------------------------------------------------------------
	// 登録したttfフォントを全て解除する
	//------------------------------------------------------------
	void RemoveAllFontResource();

	// 指定番号のフォントが使用している総バイト数取得
	UINT DEBUG_GetFontBytes(int fontNo)
	{
		UINT totalByes = 0;
		auto& node = m_FontTbl[fontNo].CreatedFontDataTbl;
		for (auto& n : node)
		{
			totalByes += n->DEBUG_GetBytes();
		}
		return totalByes;
	}
	HFONT								m_hFont;
private:

	// フォント登録データ
	struct FontData
	{
		HFONT hFont;														// フォントハンドル
		std::array<std::shared_ptr<KdFontData>, 65536>	CreatedFontDataTbl;	// 作成済みフォントデータ配列
	};
	std::array<FontData, 10>			m_FontTbl;							// 登録されたフォントの配列

	HWND								m_hWnd = 0;							// ウィンドウハンドル
	HDC									m_hDC = 0;							// デバイスコンテキスト
	
	std::map<std::string, int>			m_LoadedFontMap;					// 追加したttfファイルのリスト

	//=====================================================
	// シングルトンパターン
	//=====================================================
private:
	KdFontManager()		{}
	~KdFontManager()	{ Release(); }

public:
	static KdFontManager& Instance()
	{
		static KdFontManager Instance;
		return Instance;
	}
};
