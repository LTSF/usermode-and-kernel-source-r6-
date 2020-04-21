namespace offsets
{
	namespace managers 
	{
		uint64_t profile_manager = 0;
		uint64_t game_manager = 0;
		uint64_t round_manager = 0;
		uint64_t marker_manager = 0;
		uint64_t chams_manager = 0;
		uint64_t network_manager = 0;
		uint64_t fov_manager = 0;
		uint64_t camera_manager = 0;
	}

	namespace entity
	{
		uint64_t entity_list = 0xC8;
		uint16_t entity_count = 0xD0;
		uint64_t entity_info = 0xC8;
		uint64_t entity_operator = 0x1d0;
		uint64_t entity_team = 0x1a2;
		uint64_t entity_name = 0x1b0;
		DWORD entity_health = 0x168;
	}

	namespace local
	{
		uint64_t local_container = 0x78;
		uint64_t local_player = 0x28;
	}

	namespace camera
	{
		uint64_t local_camera = 0x78;
		uint64_t engine_camera = 0x130;
		uint64_t camera_manager = 0x420;

		uint64_t camera_right = 0x7c0;
		uint64_t camera_up = 0x7d0;
		uint64_t camera_forward = 0x7e0;
		uint64_t camera_translation = 0x7f0;
		uint64_t camera_fovx = 0x800;
		uint64_t camera_fovy = 0x814;
		uint64_t view_angles = 0x1200;
	}

	namespace skeleton
	{
		uint64_t entity_skeleton = 0x20;
		uint64_t skeleton_head = 0x6A0;
		uint64_t skeleton_pos = 0x700;
	}
}

namespace settings
{
	namespace aimbot
	{
		bool aimbot = false;
		bool fovcircle = false;
		bool team = false;
		bool sticktotarget = false;

		int aimspeed = 1;
		float aimfov = 25;
		int TargetMode = 0;
		uintptr_t EntityBone[] = { /*head*/ offsets::skeleton::skeleton_head};
		int hitbox = 0;
	}

	namespace visuals
	{
		bool box = false;
		bool name = false;
		bool health = false;
		bool chams = false;
		bool crosshair = false;
		bool outline = false;
		bool rainbowchams = true;
		bool team = false;
		bool head = false;
		bool cav = false;
		bool visuals = false;
		bool healthbased = false;
		bool snaplines = false;
		int boxMode = 0;
		int healthMode = 0;
		int nameMode = 0;

		float chamsdist = 5.f;
	}

	namespace colors
	{
		float chamscol[3] = { 1.0f , 0.1f , 0.5f };
		float espcol[3] = { 1.0f , 1.0f , 1.0f };
		float espcolteam[3] = { 1.0f , 1.0f , 1.0f };

		float crosscol[3] = { 1.0f , 1.0f , 1.0f };
	}

	namespace misc
	{
		bool misc = false;
		//bool norecoil = false;
		float fov = 90.f;
		float viewmodelfov = 1.f;
		//bool spread = false;
		bool noclip = false;
		bool speed = false;
		bool noflash = false;
		bool overlayfps = false;
		bool expfarm = false;
		bool freezelobby = false;
		float recoilval = 1.0f;
		float spreadval = 1.0f;
		int speedval = 100;
		bool unlocks = false;
	}

	namespace menu
	{
		bool menu_key = false;
		int font_size = 14;
		int box_thick = 1;
		float crosshair_size = 1.f;
		static int settsMode = 0;
	}

	namespace hotkeys
	{
		int noclip;
		int aimkey;
	}

	static const char* keyNames[] =
	{
		"",
		"Left Mouse",
		"Right Mouse",
		"Cancel",
		"Middle Mouse",
		"Mouse 5",
		"Mouse 4",
		"",
		"Backspace",
		"Tab",
		"",
		"",
		"Clear",
		"Enter",
		"",
		"",
		"Shift",
		"Control",
		"Alt",
		"Pause",
		"Caps",
		"",
		"",
		"",
		"",
		"",
		"",
		"Escape",
		"",
		"",
		"",
		"",
		"Space",
		"Page Up",
		"Page Down",
		"End",
		"Home",
		"Left",
		"Up",
		"Right",
		"Down",
		"",
		"",
		"",
		"Print",
		"Insert",
		"Delete",
		"",
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"A",
		"B",
		"C",
		"D",
		"E",
		"F",
		"G",
		"H",
		"I",
		"J",
		"K",
		"L",
		"M",
		"N",
		"O",
		"P",
		"Q",
		"R",
		"S",
		"T",
		"U",
		"V",
		"W",
		"X",
		"Y",
		"Z",
		"",
		"",
		"",
		"",
		"",
		"Numpad 0",
		"Numpad 1",
		"Numpad 2",
		"Numpad 3",
		"Numpad 4",
		"Numpad 5",
		"Numpad 6",
		"Numpad 7",
		"Numpad 8",
		"Numpad 9",
		"Multiply",
		"Add",
		"",
		"Subtract",
		"Decimal",
		"Divide",
		"F1",
		"F2",
		"F3",
		"F4",
		"F5",
		"F6",
		"F7",
		"F8",
		"F9",
		"F10",
		"F11",
		"F12",
	};

	BOOL WritePrivateProfileInt(LPCSTR lpAppName, LPCSTR lpKeyName, int nInteger, LPCSTR lpFileName) {
		char lpString[1024];
		sprintf(lpString, "%d", nInteger);
		return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
	}

	BOOL WritePrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, float nInteger, LPCSTR lpFileName) {
		char lpString[1024];
		sprintf(lpString, "%f", nInteger);
		return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
	}

	float GetPrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, FLOAT flDefault, LPCSTR lpFileName)
	{
		char szData[32];

		GetPrivateProfileStringA(lpAppName, lpKeyName, std::to_string(flDefault).c_str(), szData, 32, lpFileName);

		return (float)atof(szData);
	}

	void Save_Settings(LPCSTR path)
	{
		WritePrivateProfileInt("Visuals", "visuals", visuals::visuals, path);
		WritePrivateProfileInt("Visuals", "m_playeresp", visuals::box, path);
		WritePrivateProfileInt("Visuals", "m_boxstyle", visuals::boxMode, path);
		WritePrivateProfileInt("Visuals", "m_chams", visuals::chams, path);
		WritePrivateProfileFloat("Visuals", "m_chamsdist", visuals::chamsdist, path);
		WritePrivateProfileInt("Visuals", "m_crosshair", visuals::crosshair, path);
		WritePrivateProfileFloat("Visuals", "m_crossize", menu::crosshair_size, path);
		WritePrivateProfileInt("Visuals", "m_playerhealth", visuals::health, path);
		WritePrivateProfileInt("Visuals", "m_healthstyle", visuals::healthMode, path);
		WritePrivateProfileInt("Visuals", "m_playername", visuals::name, path);
		WritePrivateProfileInt("Visuals", "m_namestyle", visuals::nameMode, path);
		WritePrivateProfileInt("Visuals", "m_cav", visuals::cav, path);
		WritePrivateProfileInt("Visuals", "m_playerhead", visuals::head, path);
		WritePrivateProfileInt("Visuals", "m_outline", visuals::outline, path);
		WritePrivateProfileInt("Visuals", "m_team", visuals::team, path);
		WritePrivateProfileInt("Visuals", "m_fontsize", menu::font_size, path);
		WritePrivateProfileInt("Visuals", "m_healthbased", visuals::healthbased, path);
		WritePrivateProfileInt("Visuals", "m_boxthick", menu::box_thick, path);

		WritePrivateProfileInt("Aimbot", "m_aimbot", aimbot::aimbot, path);
		WritePrivateProfileInt("Aimbot", "m_fovcircle", aimbot::fovcircle, path);
		WritePrivateProfileInt("Aimbot", "m_team", aimbot::team, path);
		WritePrivateProfileInt("Aimbot", "m_aimkey", hotkeys::aimkey, path);
		WritePrivateProfileInt("Aimbot", "m_aimstyle", aimbot::TargetMode, path);
		WritePrivateProfileInt("Aimbot", "m_hitbox", aimbot::hitbox, path);
		WritePrivateProfileInt("Aimbot", "m_smooth", aimbot::aimspeed, path);
		WritePrivateProfileFloat("Aimbot", "m_fov", aimbot::aimfov, path);
		WritePrivateProfileInt("Aimbot", "m_sticky", aimbot::sticktotarget, path);

		WritePrivateProfileInt("Misc", "misc", misc::misc, path);
		//WritePrivateProfileInt("Misc", "m_recoil", misc::norecoil, path);
		WritePrivateProfileFloat("Misc", "m_recoilval", misc::recoilval, path);
		//WritePrivateProfileInt("Misc", "m_spread", misc::spread, path);
		WritePrivateProfileFloat("Misc", "m_spreadval", misc::spreadval, path);
		WritePrivateProfileFloat("Misc", "m_fov", misc::fov, path);
		WritePrivateProfileFloat("Misc", "m_viewmodel", misc::viewmodelfov, path);

		WritePrivateProfileInt("Misc", "m_speed", misc::speed, path);
		WritePrivateProfileInt("Misc", "m_speedval", misc::speedval, path);
		//WritePrivateProfileInt("Misc", "m_noclip", misc::noclip, path);
		WritePrivateProfileInt("Misc", "m_noclipkey", hotkeys::noclip, path);
		WritePrivateProfileInt("Misc", "m_noflash", misc::noflash, path);
		WritePrivateProfileInt("Misc", "m_overlayfps", misc::overlayfps, path);

		WritePrivateProfileFloat("Colors", "m_chamsr", colors::chamscol[0], path);
		WritePrivateProfileFloat("Colors", "m_chamsg", colors::chamscol[1], path);
		WritePrivateProfileFloat("Colors", "m_chamsb", colors::chamscol[2], path);
		WritePrivateProfileInt("Colors", "m_chamsrain", visuals::rainbowchams, path);

		WritePrivateProfileFloat("Colors", "m_espr", colors::espcol[0], path);
		WritePrivateProfileFloat("Colors", "m_espg", colors::espcol[1], path);
		WritePrivateProfileFloat("Colors", "m_espb", colors::espcol[2], path);
		WritePrivateProfileFloat("Colors", "m_crossr", colors::crosscol[0], path);
		WritePrivateProfileFloat("Colors", "m_crossg", colors::crosscol[1], path);
		WritePrivateProfileFloat("Colors", "m_crossb", colors::crosscol[2], path);
	}

	void Load_Settings(LPCSTR path)
	{
		visuals::box = GetPrivateProfileIntA("Visuals", "m_playeresp", visuals::box, path);
		visuals::boxMode = GetPrivateProfileIntA("Visuals", "m_boxstyle", visuals::boxMode, path);
		visuals::chams = GetPrivateProfileIntA("Visuals", "m_chams", visuals::chams, path);
		visuals::chamsdist = GetPrivateProfileFloat("Visuals", "m_chamsdist", visuals::chamsdist, path);
		visuals::crosshair = GetPrivateProfileIntA("Visuals", "m_crosshair", visuals::crosshair, path);
		menu::crosshair_size = GetPrivateProfileFloat("Visuals", "m_crossize", menu::crosshair_size, path);
		visuals::health = GetPrivateProfileIntA("Visuals", "m_playerhealth", visuals::health, path);
		visuals::healthMode = GetPrivateProfileIntA("Visuals", "m_healthstyle", visuals::healthMode, path);
		visuals::name = GetPrivateProfileIntA("Visuals", "m_playername", visuals::name, path);
		visuals::nameMode = GetPrivateProfileIntA("Visuals", "m_namestyle", visuals::nameMode, path);
		visuals::cav = GetPrivateProfileIntA("Visuals", "m_cav", visuals::cav, path);
		visuals::head = GetPrivateProfileIntA("Visuals", "m_playerhead", visuals::head, path);
		visuals::outline = GetPrivateProfileIntA("Visuals", "m_outline", visuals::outline, path);
		visuals::team = GetPrivateProfileIntA("Visuals", "m_team", visuals::team, path);
		menu::font_size = GetPrivateProfileIntA("Visuals", "m_fontsize", menu::font_size, path);
		visuals::healthbased = GetPrivateProfileIntA("Visuals", "m_healthbased", visuals::healthbased, path);
		menu::box_thick = GetPrivateProfileIntA("Visuals", "m_boxthick", menu::box_thick, path);
		visuals::visuals = GetPrivateProfileFloat("Visuals", "visuals", visuals::visuals, path);
		aimbot::aimbot = GetPrivateProfileIntA("Aimbot", "m_aimbot", aimbot::aimbot, path);
		aimbot::fovcircle = GetPrivateProfileIntA("Aimbot", "m_fovcircle", aimbot::fovcircle, path);
		aimbot::team = GetPrivateProfileIntA("Aimbot", "m_team", aimbot::team, path);
		hotkeys::aimkey = GetPrivateProfileIntA("Aimbot", "m_aimkey", hotkeys::aimkey, path);
		aimbot::TargetMode = GetPrivateProfileIntA("Aimbot", "m_aimstyle", aimbot::TargetMode, path);
		aimbot::hitbox = GetPrivateProfileIntA("Aimbot", "m_hitbox", aimbot::hitbox, path);
		aimbot::aimspeed = GetPrivateProfileIntA("Aimbot", "m_smooth", aimbot::aimspeed, path);
		aimbot::aimfov = GetPrivateProfileFloat("Aimbot", "m_fov", aimbot::aimfov, path);
		aimbot::sticktotarget = GetPrivateProfileIntA("Aimbot", "m_sticky", aimbot::sticktotarget, path);

		//misc::norecoil = GetPrivateProfileIntA("Misc", "m_recoil", misc::norecoil, path);
		misc::recoilval = GetPrivateProfileFloat("Misc", "m_recoilval", misc::recoilval, path);
		misc::fov = GetPrivateProfileFloat("Misc", "m_fov", misc::fov, path);
		misc::viewmodelfov = GetPrivateProfileFloat("Misc", "m_viewmodel", misc::viewmodelfov, path);
		misc::misc = GetPrivateProfileFloat("Misc", "misc", misc::misc, path);
		//misc::spread = GetPrivateProfileIntA("Misc", "m_spread", misc::spread, path);
		misc::spreadval = GetPrivateProfileFloat("Misc", "m_spreadval", misc::spreadval, path);
		misc::speed = GetPrivateProfileIntA("Misc", "m_speed", misc::speed, path);
		misc::speedval = GetPrivateProfileIntA("Misc", "m_speedval", misc::speedval, path);
		misc::noclip = GetPrivateProfileIntA("Misc", "m_noclip", misc::noclip, path);
		//hotkeys::noclip = GetPrivateProfileIntA("Misc", "m_noclipkey", hotkeys::noclip, path);
		misc::noflash = GetPrivateProfileIntA("Misc", "m_noflash", misc::noflash, path);
		misc::overlayfps = GetPrivateProfileIntA("Misc", "m_overlayfps", misc::overlayfps, path);
		


		colors::chamscol[0] = GetPrivateProfileFloat("Colors", "m_chamsr", colors::chamscol[0], path);
		colors::chamscol[1] = GetPrivateProfileFloat("Colors", "m_chamsg", colors::chamscol[1], path);
		colors::chamscol[2] = GetPrivateProfileFloat("Colors", "m_chamsb", colors::chamscol[2], path);
		visuals::rainbowchams = GetPrivateProfileInt("Colors", "m_chamsrain", visuals::rainbowchams, path);
		colors::espcol[0] = GetPrivateProfileFloat("Colors", "m_espr", colors::espcol[0], path);
		colors::espcol[1] = GetPrivateProfileFloat("Colors", "m_espg", colors::espcol[1], path);
		colors::espcol[2] = GetPrivateProfileFloat("Colors", "m_espb", colors::espcol[2], path);
		colors::crosscol[0] = GetPrivateProfileFloat("Colors", "m_crossr", colors::crosscol[0], path);
		colors::crosscol[1] = GetPrivateProfileFloat("Colors", "m_crossg", colors::crosscol[1], path);
		colors::crosscol[2] = GetPrivateProfileFloat("Colors", "m_crossb", colors::crosscol[2], path);
	};
	
}