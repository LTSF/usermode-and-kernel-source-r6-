#include <windows.h>
#include <mmsystem.h>
using namespace settings;

static const char* boxStyle[] =
{
	"2D",
	"2D Filled",
	"2D Corner",
	"2D Corner Filled",
	"3D Box"
};

static const char* healthStyle[] =
{
	"Left",
	"Right"
};

static const char* nameStyle[] =
{
	"Top",
	"Bottom",
	"Dist/Top",
	"Dist/Bottom"
};

static const char* targetmode[] =
{
	"Fov",
	"Distance",
	"Health"
};

static const char* Hitbox[] =
{
	"Head",
	"Chest",
	"Stomach"
};

static const char* settsName[] =
{
	"Default",
	"Config 1",
	"Config 2"
};


void styler()
{
	
	// cherry colors, 3 intensities
#define HI(v)   ImVec4(0.502f, 0.075f, 0.256f, v)
#define MED(v)  ImVec4(0.455f, 0.198f, 0.301f, v)
#define LOW(v)  ImVec4(0.232f, 0.201f, 0.271f, v)
// backgrounds (@todo: complete with BG_MED, BG_LOW)
#define BG(v)   ImVec4(0.200f, 0.220f, 0.270f, v)
// text
#define TEXT(v) ImVec4(0.860f, 0.930f, 0.890f, v)

	auto& style = Pato::GetStyle();
	style.Colors[PatoCol_Text] = TEXT(0.78f);
	style.Colors[PatoCol_TextDisabled] = TEXT(0.28f);
	style.Colors[PatoCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
	style.Colors[PatoCol_ChildWindowBg] = BG(0.58f);
	style.Colors[PatoCol_PopupBg] = BG(0.9f);
	style.Colors[PatoCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
	style.Colors[PatoCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[PatoCol_FrameBg] = BG(1.00f);
	style.Colors[PatoCol_FrameBgHovered] = MED(0.78f);
	style.Colors[PatoCol_FrameBgActive] = MED(1.00f);
	style.Colors[PatoCol_TitleBg] = LOW(1.00f);
	style.Colors[PatoCol_TitleBgActive] = HI(1.00f);
	style.Colors[PatoCol_TitleBgCollapsed] = BG(0.75f);
	style.Colors[PatoCol_MenuBarBg] = BG(0.47f);
	style.Colors[PatoCol_ScrollbarBg] = BG(1.00f);
	style.Colors[PatoCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
	style.Colors[PatoCol_ScrollbarGrabHovered] = MED(0.78f);
	style.Colors[PatoCol_ScrollbarGrabActive] = MED(1.00f);
	style.Colors[PatoCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
	style.Colors[PatoCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
	style.Colors[PatoCol_SliderGrabActive] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
	style.Colors[PatoCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
	style.Colors[PatoCol_ButtonHovered] = MED(0.86f);
	style.Colors[PatoCol_ButtonActive] = MED(1.00f);
	style.Colors[PatoCol_Header] = MED(0.76f);
	style.Colors[PatoCol_HeaderHovered] = MED(0.86f);
	style.Colors[PatoCol_HeaderActive] = HI(1.00f);
	style.Colors[PatoCol_Column] = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
	style.Colors[PatoCol_ColumnHovered] = MED(0.78f);
	style.Colors[PatoCol_ColumnActive] = MED(1.00f);
	style.Colors[PatoCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
	style.Colors[PatoCol_ResizeGripHovered] = MED(0.78f);
	style.Colors[PatoCol_ResizeGripActive] = MED(1.00f);
	style.Colors[PatoCol_PlotLines] = TEXT(0.63f);
	style.Colors[PatoCol_PlotLinesHovered] = MED(1.00f);
	style.Colors[PatoCol_PlotHistogram] = TEXT(0.63f);
	style.Colors[PatoCol_PlotHistogramHovered] = MED(1.00f);
	style.Colors[PatoCol_TextSelectedBg] = MED(0.43f);
	// [...]
	style.Colors[PatoCol_ModalWindowDarkening] = BG(0.73f);

	style.WindowPadding = ImVec2(6, 4);
	style.WindowRounding = 0.0f;
	style.FramePadding = ImVec2(5, 2);
	style.FrameRounding = 3.0f;
	style.ItemSpacing = ImVec2(7, 1);
	style.ItemInnerSpacing = ImVec2(1, 1);
	style.TouchExtraPadding = ImVec2(0, 0);
	style.IndentSpacing = 6.0f;
	style.ScrollbarSize = 12.0f;
	style.ScrollbarRounding = 16.0f;
	style.GrabMinSize = 20.0f;
	style.GrabRounding = 2.0f;

	style.WindowTitleAlign.x = 0.50f;

	style.Colors[PatoCol_Border] = ImVec4(0.539f, 0.479f, 0.255f, 0.162f);
	style.FrameBorderSize = 0.0f;
	style.WindowBorderSize = 1.0f;

	
}

void color()
{
}

#define XorStr XorString


bool testbool = false;
void MainThread()
{

	if (Pato::TreeNode("Aim")) {
		Pato::Checkbox(XorString("Aim Enabled"), &settings::aimbot::aimbot);
		Pato::Checkbox(XorString("Silent Aim"), &misc::overlayfps);
		Pato::Checkbox(XorString("Non-Sticky"), &settings::aimbot::sticktotarget);
		Pato::Checkbox(XorString("Draw FOV"), &settings::aimbot::fovcircle);
		Pato::Checkbox(XorString("Team"), &settings::aimbot::team);
		Pato::Checkbox(XorString("Drones"), &testbool);
		Pato::Checkbox(XorString("Shields"), &testbool);
		Pato::SliderFloat(XorString("Aim FOV"), &settings::aimbot::aimfov, 0, 180);
		Pato::SliderInt(XorString("Aim Smooth"), &settings::aimbot::aimspeed, 1, 150);
		Pato::Combo(XorString("Box Type"), &visuals::boxMode, boxStyle, IM_ARRAYSIZE(boxStyle));
		Pato::Combo(XorString("Name Style"), &visuals::nameMode, nameStyle, IM_ARRAYSIZE(nameStyle));
		Pato::Combo(XorString("Health Style"), &visuals::healthMode, healthStyle, IM_ARRAYSIZE(healthStyle));
		Pato::Combo(XorString("Aim Targets"), &settings::aimbot::TargetMode, targetmode, IM_ARRAYSIZE(targetmode));
		Pato::Combo(XorString("Aim Hitbox"), &settings::aimbot::hitbox, Hitbox, IM_ARRAYSIZE(Hitbox));
		Pato::Combo(XorString("Aim Key"), &settings::hotkeys::aimkey, keyNames, IM_ARRAYSIZE(keyNames));
	}


	Pato::Separator();

	if (Pato::TreeNode("Visuals")) {
		Pato::Checkbox(XorString("Box ESP"), &visuals::box);
		Pato::Checkbox(XorString("Name"), &visuals::name);
		Pato::Checkbox(XorString("Health Bar"), &visuals::health);
		Pato::Checkbox(XorString("Team"), &visuals::team);
		Pato::Checkbox(XorString("Head Circle"), &visuals::head);
		Pato::Checkbox(XorString("Health Based"), &visuals::healthbased);
		Pato::Checkbox(XorString("Glow"), &visuals::chams);
		Pato::Checkbox(XorString("Cav ESP/Ops"), &visuals::cav);
		Pato::Checkbox(XorString("Crosshair"), &visuals::crosshair);
		Pato::Checkbox(XorString("Snaplines"), &visuals::snaplines);
		if (visuals::snaplines)
			DrawLine(Width / 2, Height, screen_nike.x, screen_nike.y, &ESPColor, 1);
	}

	Pato::Separator();

	if (Pato::TreeNode("Misc")) {
		Pato::SliderFloat(XorString("Recoil Value"), &misc::recoilval, 0.f, 1.000f);
		Pato::SliderFloat(XorString("Spread Value"), &misc::spreadval, 0.f, 1.000f);
		Pato::Checkbox(XorString("No-Flash"), &misc::noflash);
		Pato::Checkbox(XorString("EXP Farmer"), &misc::expfarm);
		Pato::Checkbox(XorString("FPS/WaterMark"), &misc::overlayfps);
		if (Pato::Checkbox("Freeze Lobby", &misc::freezelobby))
			freeze_lobby(0);
		Pato::SliderInt(XorString("Speed Value"), &misc::speedval, 50, 200);
		Pato::SliderFloat(XorString("Player FOV"), &misc::fov, 0, 180);
		Pato::SliderFloat(XorString("Weapon FOV"), &misc::viewmodelfov, 0, 3);
		Pato::Checkbox(XorString("All Unlocks"), &misc::unlocks);
		if (misc::unlocks)
			unlocks(settings::misc::unlocks);
	}

	Pato::Separator();

	if (Pato::TreeNode("Settings")) {
		Pato::RadioButton(XorString("Default"), &menu::settsMode, 0);
		Pato::RadioButton(XorString("Slot 1"), &menu::settsMode, 1);
		Pato::RadioButton(XorString("Slot 2"), &menu::settsMode, 2);
		if (Pato::Button((XorString("Save Settings")), ImVec2(100, 30)))
		{
			if (menu::settsMode == 0)
				settings::Save_Settings(XorString("C:\\Rainbow-Default.ini"));
			if (menu::settsMode == 1)
				settings::Save_Settings(XorString("C:\\Rainbow-Setts1.ini"));
			if (menu::settsMode == 2)
				settings::Save_Settings(XorString("C:\\Rainbow-Setts2.ini"));
		}
		if (Pato::Button((XorString("Load Settings")), ImVec2(100, 30)))
		{
			if (menu::settsMode == 0)
				settings::Load_Settings(XorString("C:\\Rainbow-Default.ini"));
			if (menu::settsMode == 1)
				settings::Load_Settings(XorString("C:\\Rainbow-Setts1.ini"));
			if (menu::settsMode == 2)
				settings::Load_Settings(XorString("C:\\Rainbow-Setts2.ini"));
		}
		if (Pato::Button((XorString("Panic (closes)")), ImVec2(100, 30)))
		{
			system("taskkill /f /im RainbowSix.exe");
			exit(0);
		}
	}


	Pato::Separator();

   if (Pato::TreeNode("Colors")) {
		Pato::ColorEdit3("Chams/Glow Color", (float*)&colors::chamscol);
		Pato::SliderFloat("Glow Distance", &visuals::chamsdist, 0, 35);
		Pato::ColorEdit3("Crosshair Color", (float*)&colors::crosscol);
		Pato::ColorEdit3("Enemy Color", (float*)&colors::espcol);
		Pato::ColorEdit3("Team Color", (float*)&colors::espcolteam);
	}
}

bool draw_abigsquare()
{
	PatoStyle& style = Pato::GetStyle();
	color();
	
	styler();
	Pato::Begin(XorString("VirtusAPI"), NULL, ImVec2(700, 450), 1.f, PatoWindowFlags_NoCollapse | PatoWindowFlags_NoTitleBar);
	{
		char* text = XorStr("Unknown");
		text = XorStr(" R6S ");
		Pato::Text(text);
		MainThread();
		

		Pato::End();
		return true;
	}

}




void shortcurts()
{
	//switch (realaimkey)
	//{
	//case 0:
	//	Globals::Aimbot::HotKey = VK_XBUTTON1;
	//	break;
	//case 1:
	//	Globals::Aimbot::HotKey = VK_XBUTTON2;
	//	break;
	//case 2:
	//	Globals::Aimbot::HotKey = VK_RBUTTON;
	//	break;
	//case 3:
	//	Globals::Aimbot::HotKey = VK_LBUTTON;
	//	break;
	//case 4:
	//	Globals::Aimbot::HotKey = VK_MENU;
	//	break;
	//}

	if (GetAsyncKeyState(VK_INSERT))
	{
		if (menu::menu_key == false)
		{
			menu::menu_key = true;
		}
		else if (menu::menu_key == true)
		{
			menu::menu_key = false;
		}
		Sleep(200);
	}

	//if (GetAsyncKeyState(VK_F6))
	//{
	//	settings::misc::expfarm = false;
	//	Sleep(5);
	//}
}
RGBA c0l = { 255, 255, 255, 255 };
int crossx, faken_rot;
int TimePD2;

void __stdcall rotateCross() {
	crossx++;
	if ((int)faken_rot > 89) { faken_rot = (float)0; }
	faken_rot++;
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

float BOG_TO_GRD(float BOG) {
	return (180 / M_PI) * BOG;
}

float GRD_TO_BOG(float GRD) {
	return (M_PI / 180) * GRD;
}

void rotatingSwastika()
{
	POINT Screen; int oodofdfo, jbjfdbdsf;
	oodofdfo = Width;
	jbjfdbdsf = Height;
	Screen.x = oodofdfo; Screen.y = jbjfdbdsf;
	//Middle POINT
	POINT Middle; Middle.x = (int)(Screen.x / 2); Middle.y = (int)(Screen.y / 2);
	int a = (int)(Screen.y / 2 / 30); ShowWindow(GetConsoleWindow(), SW_HIDE);
	float gamma = atan(a / a);

	int Drehungswinkel = faken_rot;

	int i = 0;
	while (i < 4)
	{
		std::vector <int> p;
		p.push_back(a * sin(GRD_TO_BOG(Drehungswinkel + (i * 90))));									//p[0]		p0_A.x
		p.push_back(a * cos(GRD_TO_BOG(Drehungswinkel + (i * 90))));									//p[1]		p0_A.y
		p.push_back((a / cos(gamma)) * sin(GRD_TO_BOG(Drehungswinkel + (i * 90) + BOG_TO_GRD(gamma))));	//p[2]		p0_B.x
		p.push_back((a / cos(gamma)) * cos(GRD_TO_BOG(Drehungswinkel + (i * 90) + BOG_TO_GRD(gamma))));	//p[3]		p0_B.y

		DrawLine(Middle.x, Middle.y, Middle.x + p[0], Middle.y - p[1], &Col.blue, 3.5);
		DrawLine(Middle.x + p[0], Middle.y - p[1], Middle.x + p[2], Middle.y - p[3], &Col.blue, 3.5);

		i++;
	}
}

void crosshair()
{
	RGBA CrossColor = { colors::crosscol[0] * 255, colors::crosscol[1] * 255, colors::crosscol[2] * 255, 255 };
	if (visuals::crosshair) {
		rotatingSwastika();
		rotateCross();
	}
}