
#include "main.h"
#include "Auth/tool.h"
#include "Auth/sha256.h"



tool* tools;





UINT16 getVolumeHash()
{
	DWORD serialNum = 0;

	// Determine if this volume uses an NTFS file system.      
	GetVolumeInformation("c:\\", NULL, 0, &serialNum, NULL, NULL, NULL, 0);
	UINT16 hash = (UINT16)((serialNum + (serialNum >> 16)) & 0xFFFF);

	return hash;
}




HRESULT DirectXInit(HWND hWnd)
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(3);

	ZeroMemory(&p_Params, sizeof(p_Params));
	p_Params.Windowed = TRUE;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_Params.hDeviceWindow = hWnd;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_Params.BackBufferWidth = Width;
	p_Params.BackBufferHeight = Height;
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
	{
		p_Object->Release();
		exit(4);
	}

	Pato_CHECKVERSION();
	Pato::CreateContext();
	PatoIO& io = Pato::GetIO();

	ImFontConfig font_config;
	font_config.OversampleH = 1;
	font_config.OversampleV = 1;
	font_config.PixelSnapH = true;

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF,
		0x0400, 0x044F,
		0,
	};

	//pFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\comic.ttf", 16.f);
	//TabFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\cherryfont.ttf", 15.f);
	io.Fonts->AddFontFromFileTTF(XorString("C:\\Windows\\Fonts\\verdana.ttf"), 14.f);
	//tabfont = io.Fonts->AddFontFromFileTTF((XorString("C:\\Windows\\Fonts\\cherryfont.ttf")), 32);
	//Pato::GetIO().Fonts->AddFontFromMemoryCompressedTTF(skeet_compressed_data, skeet_compressed_size, 14.f, &font_config, ranges);

	Pato_ImplWin32_Init(hWnd);
	Pato_ImplDX9_Init(p_Device);

	Pato::StyleColorsClassic();
	PatoStyle* style = &Pato::GetStyle();
	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 0.0f;
	style->FramePadding = ImVec2(2, 2);
	style->FrameRounding = 0.0f;
	style->ItemSpacing = ImVec2(8, 8);// ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 0.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 0.0f;
	style->TabRounding = 0.f;
	style->ChildRounding = 0.f;

	style->Colors[PatoCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[PatoCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[PatoCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
	style->Colors[PatoCol_ChildWindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
	style->Colors[PatoCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[PatoCol_Border] = ImColor(70, 0, 110, 255);
	style->Colors[PatoCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[PatoCol_FrameBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
	style->Colors[PatoCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[PatoCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[PatoCol_TitleBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
	style->Colors[PatoCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[PatoCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[PatoCol_MenuBarBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
	style->Colors[PatoCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[PatoCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[PatoCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[PatoCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[PatoCol_CheckMark] = ImColor(70, 0, 110, 255);
	style->Colors[PatoCol_SliderGrab] = ImColor(70, 0, 110, 255);
	style->Colors[PatoCol_SliderGrabActive] = ImColor(80, 0, 130, 255);
	style->Colors[PatoCol_Button] = ImColor(70, 0, 110, 255);
	style->Colors[PatoCol_ButtonHovered] = ImColor(80, 0, 130, 255);
	style->Colors[PatoCol_ButtonActive] = ImColor(80, 0, 130, 255);
	style->Colors[PatoCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[PatoCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[PatoCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[PatoCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[PatoCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[PatoCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[PatoCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[PatoCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[PatoCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[PatoCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[PatoCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[PatoCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[PatoCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[PatoCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[PatoCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

	style->WindowTitleAlign.x = 0.50f;
	style->FrameRounding = 2.0f;

	p_Object->Release();
	return S_OK;
}

void create_console()
{
	if (!AllocConsole())
	{
		char buffer[1024] = { 0 };
		sprintf_s(buffer, XorString("Failed to AllocConsole( ), GetLastError( ) = %d"), GetLastError());
		MessageBox(HWND_DESKTOP, LPCSTR(buffer), XorString("Error"), MB_OK);

		return;
	}

	auto lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	auto hConHandle = _open_osfhandle(PtrToUlong(lStdHandle), _O_TEXT);
	auto fp = _fdopen(hConHandle, XorString("w"));

	freopen_s(&fp, XorString("CONOUT$"), XorString("w"), stdout);

	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);
}

void SetupWindow()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SetWindowToTarget, 0, 0, 0);

	WNDCLASSEX wClass =
	{
		sizeof(WNDCLASSEX),
		0,
		WinProc,
		0,
		0,
		nullptr,
		LoadIcon(nullptr, IDI_APPLICATION),
		LoadCursor(nullptr, IDC_ARROW),
		nullptr,
		nullptr,
		opatudobem.c_str(),
		LoadIcon(nullptr, IDI_APPLICATION)
	};

	if (!RegisterClassEx(&wClass))
		exit(1);

	GameWnd = FindWindow(XorString("R6Game"), 0);

	if (GameWnd)
	{
		GetClientRect(GameWnd, &GameRect);
		POINT xy;
		ClientToScreen(GameWnd, &xy);
		GameRect.left = xy.x;
		GameRect.top = xy.y;

		Width = GameRect.right;
		Height = GameRect.bottom;
	}

	MyWnd = CreateWindowEx(NULL, M_Name, M_Name, WS_POPUP | WS_VISIBLE, GameRect.left, GameRect.top, Width, Height, NULL, NULL, 0, NULL);
	DwmExtendFrameIntoClientArea(MyWnd, &Margin);
	SetWindowLong(MyWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	//SetWindowDisplayAffinity(MyWnd, WDA_MONITOR);
	ShowWindow(MyWnd, SW_SHOW);
	UpdateWindow(MyWnd);
}

void background()
{
	Pato::SetNextWindowPos(ImVec2(0, 0), PatoCond_Once);
	Pato::SetNextWindowSize(Pato::GetIO().DisplaySize, PatoCond_Once);

	Pato::PushStyleColor(PatoCol_WindowBg, ImVec4(0.09f, 0.09f, 0.09f, 0.40f / 1.f * 2.f));
	static const auto flags = PatoWindowFlags_NoTitleBar | PatoWindowFlags_NoResize | PatoWindowFlags_NoScrollbar | PatoWindowFlags_NoScrollWithMouse | PatoWindowFlags_NoBringToFrontOnFocus | PatoWindowFlags_NoMove;
	Pato::Begin(XorString("##background"), nullptr, flags);
	Pato::End();
	Pato::PopStyleColor();
}

void render() {

	Pato_ImplDX9_NewFrame();
	Pato_ImplWin32_NewFrame();
	Pato::NewFrame();

	draw_things();

	if (settings::menu::menu_key) {
		background();
		draw_abigsquare();
		Pato::GetIO().MouseDrawCursor = 1;
	}
	else
	{
		Pato::GetIO().MouseDrawCursor = 0;
	}


	char lol[64];
	char ent[64];
	char ents[64];

	if (settings::misc::overlayfps)
	{
		char fpsinfo[64];
		sprintf(fpsinfo, XorString("        Unfaircheats.cc       "), Pato::GetIO().Framerate);
		RGBA red = { 255,0,0,200 };
		DrawFilledBox(0, 0, 80, 15, 0, &red);
		DrawStrokeText(0, 0, &Col.white, fpsinfo);
	}


	//sprintf(lol, xorstr_("Dukkzin [Press Insert]"), Pato::GetIO().Framerate);
	//DrawStrokeText(50, 60, &Col.skyblue, lol);

	Pato::EndFrame();
	p_Device->SetRenderState(D3DRS_ZENABLE, false);
	p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (p_Device->BeginScene() >= 0)
	{
		Pato::Render();
		Pato_ImplDX9_RenderDrawData(Pato::GetDrawData());
		p_Device->EndScene();
	}
	HRESULT result = p_Device->Present(NULL, NULL, NULL, NULL);

	if (result == D3DERR_DEVICELOST && p_Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		Pato_ImplDX9_InvalidateDeviceObjects();
		p_Device->Reset(&p_Params);
		Pato_ImplDX9_CreateDeviceObjects();
	}
}


std::string random_string()
{
	std::string str(XorString("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"));

	std::random_device rd;
	std::mt19937 generator(rd());

	std::shuffle(str.begin(), str.end(), generator);

	return str.substr(0, 27);    // assumes 32 < number of characters in str         
}

std::string RandomString(int len)
{
	srand(time(0));
	std::string str = (XorString("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"));
	std::string newstr;
	int pos;
	while (newstr.size() != len) {
		pos = ((rand() % (str.size() - 1)));
		newstr += str.substr(pos, 1);
	}
	return newstr;
}

void w(std::string line) 
{
	std::cout << line << endl;
}
bool a = false;




void Auth() 
{


	settings::Load_Settings(XorString("C:\\Rainbow-Default.ini"));

	while (TRUE)
		{
			if (!initialize())
				{

				if (!a)
					std::cout << XorString("[+] Please launch Rainbow") << std::endl;
					a = true;
					std::this_thread::sleep_for(std::chrono::seconds(4));
					continue;
				}
				else
				{

					std::cout << XorString("[+] Game Found!") << std::endl;
					system("cls");
					SetupWindow();
					w("Loaded Overlay");
					DirectXInit(MyWnd);
					w("Starting Rendender");
					MainLoop();
				}
			}
}






int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	
	
	create_console();
	HWND ConsWind = GetConsoleWindow();
	comcalma = random_string();
	opatudobem = RandomString(23);
	SetConsoleTitle(comcalma.c_str());
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	Auth();


}



WPARAM MainLoop()
{
	static RECT old_rc;
	ZeroMemory(&Message, sizeof(MSG));

	while (Message.message != WM_QUIT)
	{
		if (PeekMessage(&Message, MyWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		HWND hwnd_active = GetForegroundWindow();

		if (hwnd_active == GameWnd) {
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(MyWnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(GameWnd, &rc);
		ClientToScreen(GameWnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;
		PID = GetProcessID(XorString("RainbowSix.exe"));

		if (!PID)
			exit(0);
		PatoIO& io = Pato::GetIO();
		io.ImeWindowHandle = GameWnd;
		io.DeltaTime = 1.0f / 60.0f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (GetAsyncKeyState(VK_LBUTTON)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;

		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{

			old_rc = rc;

			Width = rc.right;
			Height = rc.bottom;

			p_Params.BackBufferWidth = Width;
			p_Params.BackBufferHeight = Height;
			SetWindowPos(MyWnd, (HWND)0, xy.x, xy.y, Width, Height, SWP_NOREDRAW);
			p_Device->Reset(&p_Params);
		}
		render();

	}
	Pato_ImplDX9_Shutdown();
	Pato_ImplWin32_Shutdown();
	Pato::DestroyContext();

	CleanuoD3D();
	DestroyWindow(MyWnd);

	return Message.wParam;
}
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (Pato_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message)
	{
	case WM_DESTROY:
		CleanuoD3D();
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (p_Device != NULL && wParam != SIZE_MINIMIZED)
		{
			Pato_ImplDX9_InvalidateDeviceObjects();
			p_Params.BackBufferWidth = LOWORD(lParam);
			p_Params.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = p_Device->Reset(&p_Params);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			Pato_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}
void CleanuoD3D()
{
	if (p_Device != NULL)
	{
		p_Device->EndScene();
		p_Device->Release();
	}
	if (p_Object != NULL)
	{
		p_Object->Release();
	}
}
int isTopwin()
{
	HWND hWnd = GetForegroundWindow();
	if (hWnd == GameWnd)
		return TopWindowGame;
	if (hWnd == MyWnd)
		return TopWindowMvoe;

	return 0;
}


void SetWindowToTarget()
{
	while (true)
	{
		GameWnd = FindWindow(XorString("R6Game"), 0);

		if (!GameWnd)
			continue;
		else
		{
			ZeroMemory(&GameRect, sizeof(GameRect));
			GetWindowRect(GameWnd, &GameRect);
			Width = GameRect.right - GameRect.left;
			Height = GameRect.bottom - GameRect.top;
			DWORD dwStyle = GetWindowLong(GameWnd, GWL_STYLE);
			if (dwStyle & WS_BORDER)
			{
				GameRect.top += 32;
				Height -= 39;
			}
			ScreenCenterX = Width / 2;
			ScreenCenterY = Height / 2;
			MoveWindow(MyWnd, GameRect.left, GameRect.top, Width, Height, true);
		}
	}
}
