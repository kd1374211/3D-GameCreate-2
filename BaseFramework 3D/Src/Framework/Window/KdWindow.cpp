#include "Framework/KdFramework.h"

#include "KdWindow.h"

// ImGui
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool KdWindow::Create(int clientWidth, int clientHeight, std::string_view titleName, std::string_view windowClassName, bool isBorderlessWindow)
{
	// インスタンスハンドル取得
	HINSTANCE hInst = GetInstanceHandle();

	//===================================================================
	// メインウィンドウ作成
	//===================================================================

	std::string wndClsName = windowClassName.data();

	//ウィンドウクラスの定義
	WNDCLASSEX wc;											// ウィンドウクラスの定義用
	wc.cbSize = sizeof(WNDCLASSEX);							// 構造体のサイズ
	wc.style = 0;											// スタイル
	wc.lpfnWndProc = &KdWindow::callWindowProc;			// ウインドウ関数
	wc.cbClsExtra = 0;										// エキストラクラス情報 
	wc.cbWndExtra = 0;										// エキストラウィンドウ情報
	wc.hInstance = hInst;									// インスタンスハンドル
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);			// ラージアイコン
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);		// スモールアイコン 
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);			// マウスカーソル
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// 背景色 
	wc.lpszMenuName = nullptr;								// メインメニュー名
	wc.lpszClassName = wndClsName.c_str();					// ウィンドウクラス名

	//ウィンドウクラスの登録
	if (!RegisterClassEx(&wc)) {
		return false;
	}

	//ウィンドウの作成
	DWORD _dwStyle		= WS_OVERLAPPEDWINDOW - WS_THICKFRAME;
	int _clientWidth	= clientWidth;
	int _clientHeight	= clientHeight;
	// ボーダレスウィンドウに設定しなおす
	if (isBorderlessWindow)
	{
		_dwStyle		= WS_POPUP;
		_clientWidth	= GetSystemMetrics(SM_CXSCREEN);
		_clientHeight	= GetSystemMetrics(SM_CYSCREEN);
	}
	m_hWnd = CreateWindow(
		wndClsName.c_str(),	// ウィンドウクラス名
		titleName.data(),	// ウィンドウのタイトル
		_dwStyle,			// ウィンドウタイプを標準タイプに	
		0,					// ウィンドウの位置（Ｘ座標）
		0,					// ウィンドウの位置（Ｙ座標）						
		_clientWidth,		// ウィンドウの幅
		_clientHeight,		// ウィンドウの高さ			
		nullptr,			// 親ウィンドウのハンドル
		nullptr,			// メニューのハンドル
		hInst,				// インスタンスハンドル 
		this);				// 追加情報

	if (m_hWnd == nullptr) {
		return false;
	}

	// クライアントのサイズを設定
	SetClientSize(_clientWidth, _clientHeight);

	//===================================================================
	//ウィンドウの表示
	//===================================================================
	ShowWindow(m_hWnd, SW_SHOW);
	//ウィンドウの更新
	UpdateWindow(m_hWnd);

	// timeGetTime関数の精度を1msに設定
	timeBeginPeriod(1);

	return true;
}

void KdWindow::Release()
{
	if (m_hWnd) {
		DestroyWindow(m_hWnd);
		m_hWnd = nullptr;
	}
}

bool KdWindow::ProcessMessage()
{
	m_mouseWheelVal = 0;

	// メッセージ取得
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// 終了メッセージがきた
		if (msg.message == WM_QUIT) {
			return false;
		}

		//メッセージ処理
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

// ウィンドウ関数(Static関数)
LRESULT CALLBACK KdWindow::callWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// ウィンドウプロパティから、GameWindowクラスのインスタンスを取得
	KdWindow* pThis = (KdWindow*)GetProp(hWnd, "GameWindowInstance");

	// nullptrの場合は、デフォルト処理を実行
	if (pThis == nullptr) {
		switch (message) {
			case WM_CREATE:
			{
				// CreateWindow()で渡したパラメータを取得
				CREATESTRUCT * createStruct = (CREATESTRUCT*)lParam;
				KdWindow* window = (KdWindow*)createStruct->lpCreateParams;

				// ウィンドウプロパティにこのクラスのインスタンスアドレスを埋め込んでおく
				// 次回から、pThis->WindowProcの方へ処理が流れていく
				SetProp(hWnd, "GameWindowInstance", window);

			}
			return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}

	// インスタンス側のWindow関数を実行する
	return pThis->WindowProc(hWnd, message, wParam, lParam);
}

// ウィンドウ関数
LRESULT KdWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// ImGuiにイベント通知
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) {
		return true;
	}

	//===================================
	//メッセージによって処理を選択
	//===================================
	switch (message) {
	// ホイールスクロール時
	case WM_MOUSEWHEEL:
		{
			m_mouseWheelVal = (short)HIWORD(wParam);
		}
		break;
	// Xボタンが押された
	case WM_CLOSE:
		// 破棄
		Release();
		break;
	// ウィンドウ破棄直前
	case WM_DESTROY:
		RemoveProp(hWnd, "GameWindowInstance");
		PostQuitMessage(0);
		break;
	default:
		// メッセージのデフォルト処理
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ウィンドウのクライアントのサイズを指定サイズにする。
void KdWindow::SetClientSize(int w, int h)
{
	RECT rcWnd, rcCli;

	GetWindowRect(m_hWnd, &rcWnd); // ウィンドウのRECT取得
	GetClientRect(m_hWnd, &rcCli); // クライアント領域のRECT取得

	// ウィンドウの余白を考えて、クライアントのサイズを指定サイズにする。
	MoveWindow(m_hWnd,
		rcWnd.left,// X座標
		rcWnd.top,// Y座標
		w + (rcWnd.right - rcWnd.left) - (rcCli.right - rcCli.left),
		h + (rcWnd.bottom - rcWnd.top) - (rcCli.bottom - rcCli.top),
		TRUE);
}

bool KdWindow::OpenFileDialog(std::string& filepath, std::string_view title, const char* filters)
{
	// 現在のカレントディレクトリ保存
	auto current = std::filesystem::current_path();
	// ファイル名のみ
	auto filename = std::filesystem::path(filepath).filename();

	// 結果用
	static char fname[1000];
	strcpy_s(fname, sizeof(fname), filename.string().c_str());

	// デフォルトフォルダ
	std::string dir;
	if (filepath.size() == 0)
	{
		dir = current.string() + "\\";
	}
	else {
		auto path = std::filesystem::absolute(filepath);
		dir = path.parent_path().string() + "\\";
	}

	OPENFILENAMEA o;
	ZeroMemory(&o, sizeof(o));

	o.lStructSize = sizeof(o);									// 構造体サイズ
	o.hwndOwner = nullptr;										// 親ウィンドウのハンドル
	o.lpstrInitialDir = dir.c_str();							// 初期フォルダー
	o.lpstrFile = fname;										// 取得したファイル名を保存するバッファ
	o.nMaxFile = sizeof(fname);									// 取得したファイル名を保存するバッファサイズ
	o.lpstrFilter = filters;									// (例) "TXTファイル(*.TXT)\0*.TXT\0全てのファイル(*.*)\0*.*\0";
	o.lpstrDefExt = "";
	o.lpstrTitle = title.data();
	o.nFilterIndex = 1;
	if (GetOpenFileNameA(&o))
	{
		// カレントディレクトリを元に戻す
		std::filesystem::current_path(current);
		// 相対パスへ変換
		filepath = std::filesystem::relative(fname).string();
		return true;
	}
	std::filesystem::current_path(current);	// カレントディレクトリを元に戻す
	return false;
}

bool KdWindow::SaveFileDialog(std::string& filepath, std::string_view title, const char* filters, std::string_view defExt)
{
	// 現在のカレントディレクトリ保存
	auto current = std::filesystem::current_path();
	// ファイル名のみ
	auto filename = std::filesystem::path(filepath).filename();

	// 結果用
	static char fname[1000];
	strcpy_s(fname, sizeof(fname), filename.string().c_str());

	// デフォルトフォルダ
	std::string dir;
	if (filepath.size() == 0)
	{
		dir = current.string() + "\\";
	}
	else {
		auto path = std::filesystem::absolute(filepath);
		dir = path.parent_path().string() + "\\";
	}

	OPENFILENAMEA o;
	ZeroMemory(&o, sizeof(o));

	o.lStructSize = sizeof(o);									// 構造体サイズ
	o.hwndOwner = nullptr;										// 親ウィンドウのハンドル
	o.lpstrInitialDir = dir.c_str();						// 初期フォルダー
	o.lpstrFile = fname;										// 取得したファイル名を保存するバッファ
	o.nMaxFile = sizeof(fname);									// 取得したファイル名を保存するバッファサイズ
	o.lpstrFilter = filters;									// (例) "TXTファイル(*.TXT)\0*.TXT\0全てのファイル(*.*)\0*.*\0";
	o.lpstrDefExt = defExt.data();
	o.lpstrTitle = title.data();
	o.nFilterIndex = 1;
	o.Flags = OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;
	if (GetSaveFileNameA(&o))
	{
		// カレントディレクトリを元に戻す
		std::filesystem::current_path(current);
		// 相対パスへ変換
		filepath = std::filesystem::relative(fname).string();
		return true;
	}
	std::filesystem::current_path(current);	// カレントディレクトリを元に戻す
	return false;
}
