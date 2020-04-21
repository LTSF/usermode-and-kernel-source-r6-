using namespace std;
using namespace offsets;
using namespace settings;
using namespace math;

uintptr_t m_BastFovTarget;
uintptr_t resultant_entity;
vector<uint64_t> RainbowEntityList;

uint64_t GetLocalPlayer()
{
	uint64_t entity_container = read<uint64_t>(managers::profile_manager + local::local_container);

	if (!entity_container)
		return NULL;

	entity_container = read<uint64_t>(entity_container);

	if (!entity_container)
		return NULL;

	return read<uint64_t>(entity_container + local::local_player);
}

unsigned short IsTeam(uint64_t player)
{
	if (!player)
		return 0xFF;

	uint64_t replication = read<uint64_t>(player + entity::entity_info);

	if (!replication)
		return 0xFF;

	unsigned long online_team_id = read<unsigned long>(replication + entity::entity_team);
	return LOWORD(online_team_id);
}

int RoundState()
{
	if (!managers::round_manager)
		return false;

	return read<int>(managers::round_manager + 0x2E8);
}

float GetPlayerHealth(uintptr_t entity)
{
	uintptr_t EntityInfo = read<uintptr_t>(entity + 0x28);
	uintptr_t MainComponent = read<uintptr_t>(EntityInfo + 0xD8);
	uintptr_t ChildComponent = read<uintptr_t>(MainComponent + 0x8);

	float m_health = read<DWORD>(ChildComponent + entity::entity_health);

	return m_health;
}

uint64_t ptrGMComponentContainer = read<uint64_t>(managers::game_manager + 0x250);
uint64_t ptrOutlineComp = read<uint64_t>(ptrGMComponentContainer + 0xBB8);

 
struct SVector4
{
	float x;
	float y;
	float z;
	float a;
};




std::string GetPlayerName(uintptr_t entity) {
	uintptr_t PlayerInfo = read<uintptr_t>(entity + entity::entity_info);
	uintptr_t PlayerName = read<uintptr_t>(PlayerInfo + entity::entity_name);

	if (!PlayerName)
		return "";

	return read_string<std::string>(PlayerName);
}

float RainbowCycleColor[3] = { 255,0,0 };
void rainbow() {
	if (RainbowCycleColor[0] == 255 && RainbowCycleColor[1] < 255 && RainbowCycleColor[2] == 0)
		RainbowCycleColor[1]++;

	else if (RainbowCycleColor[0] <= 255 && RainbowCycleColor[0] > 0 && RainbowCycleColor[1] == 255 && RainbowCycleColor[2] == 0)
		RainbowCycleColor[0]--;

	else if (RainbowCycleColor[0] == 0 && RainbowCycleColor[1] == 255 && RainbowCycleColor[2] >= 0 && RainbowCycleColor[2] < 255)
		RainbowCycleColor[2]++;

	else if (RainbowCycleColor[0] == 0 && RainbowCycleColor[1] <= 255 && RainbowCycleColor[1] > 0 && RainbowCycleColor[2] == 255)
		RainbowCycleColor[1]--;

	else if (RainbowCycleColor[0] >= 0 && RainbowCycleColor[0] < 255 && RainbowCycleColor[1] == 0 && RainbowCycleColor[2] == 255)
		RainbowCycleColor[0]++;

	else if (RainbowCycleColor[0] == 255 && RainbowCycleColor[1] == 0 && RainbowCycleColor[2] > 0 && RainbowCycleColor[2] <= 255)
		RainbowCycleColor[2]--;
}
bool chams()
{
	uint64_t chams = read<uint64_t>(managers::chams_manager + 0xB8);
	//write<D3DXVECTOR3>(chams + 0xD0, D3DXVECTOR3(visuals::chamsr, visuals::chamsg, visuals::chamsb));
	if (visuals::rainbowchams)
		write<D3DXVECTOR3>(chams + 0xD0, D3DXVECTOR3(RainbowCycleColor[0] * 255.f, RainbowCycleColor[1] * 255.f, RainbowCycleColor[2] * 255.f));
	else
		write<D3DXVECTOR3>(chams + 0xD0, D3DXVECTOR3(colors::chamscol[0] * 255.f, colors::chamscol[1] * 255.f, colors::chamscol[2] * 255.f));
	//write<float>(chams + 0x110, g);
	write<float>(chams + 0x11c, 4.f); // Opacity
	write<float>(chams + 0x110, visuals::chamsdist); // Opacity


	return true;
}

void NoClip()
{
	uint64_t noclipchain1 = read<uint64_t>(managers::network_manager + 0xF8);
	uint64_t noclipchain2 = read<uint64_t>(noclipchain1 + 0x8);
	if (GetAsyncKeyState(hotkeys::noclip))
	{
		write<D3DXVECTOR3>((noclipchain2 + 0x544), D3DXVECTOR3(0.000, 0.000, 0.000));
	}
}

bool noFlash() {

	uint8_t Activate = 0;
	uintptr_t lpEventManager = read<uintptr_t>(GetLocalPlayer() + 0x30);
	uintptr_t lpFxArray = read<uintptr_t>(lpEventManager + 0x30);
	const UINT uStunIndex = 5;
	uintptr_t lpFxStun = read<uintptr_t>(lpFxArray + (uStunIndex * sizeof(PVOID)));
	write<BYTE>(lpFxStun + 0x40, 0);

	return true;
}

bool speedhack()
{
	uint64_t localplayer = read<uint64_t>(GetLocalPlayer() + 0x30);
	uint64_t beforespeed = read<uint64_t>(localplayer + 0x30);
	uint64_t speedcontainer = read<uint64_t>(beforespeed + 0x38);
	write<int>(speedcontainer + 0x58, misc::speedval);

	return true;
}
void update_fov(int FOV) {
	float setFOV = (FOV / 57.2956455309);

	uint64_t temp;
	temp = offsets::managers::fov_manager;

	if (!temp)
		return;

	temp = read<uint64_t>(temp + 0x28);

	if (!temp)
		return;

	temp = read<uint64_t>(temp + 0x0);

	if (!temp)
		return;

	write<float>(temp + 0x38, setFOV);
}

void freeze_lobby(bool toggle)
{
	uint64_t freezegame = read<uint64_t>(baseAddress + 0x538AA10);
	write<int>(freezegame + 0x5C, toggle);
}
void update_viewmodel_fov(int FOV) {
	uint64_t temp;
	temp = offsets::managers::fov_manager;

	if (!temp)
		return;

	temp = read<uint64_t>(temp + 0x28);

	if (!temp)
		return;

	temp = read<uint64_t>(temp + 0x0);

	if (!temp)
		return;

	write<float>(temp + 0x3C, FOV);
}
void NoRecoil_and_NoSpread()
{
	DWORD_PTR NoRecoilChain = read<DWORD_PTR>(GetLocalPlayer() + 0x90);
	DWORD_PTR NoRecoilChain2 = read<DWORD_PTR>(NoRecoilChain + 0xC8);
	DWORD_PTR NoRecoilChain3 = read<DWORD_PTR>(NoRecoilChain2 + 0x278);

	write<bool>(NoRecoilChain3 + 0x168, false); //toggle
	write<float>(NoRecoilChain3 + 0x14C, misc::recoilval); //vertical
	write<float>(NoRecoilChain3 + 0x15C, misc::recoilval); //horizontal

	
		write<float>(NoRecoilChain3 + 0x58, misc::spreadval); //spread
}

bool Caveira(bool enable, uintptr_t entity)
{
	if (!enable)
		return false;
	uintptr_t GameManager = managers::game_manager;

	if (!GameManager)
		return false;

	uintptr_t EntityList = read<uintptr_t>(GameManager + offsets::entity::entity_list);

	if (!EntityList)
		return false;

	int EntityCount = read<DWORD>(GameManager + offsets::entity::entity_count) & 0x3fffffff;

	if (!EntityCount)
		return false;

	for (int i = 0; i < EntityCount; i++)
	{
		uintptr_t EntityObject = read<uintptr_t>(EntityList + i * 0x8);

		if (!EntityObject)
			return false;

		uintptr_t PtrChain1 = read<uintptr_t>(EntityObject + 0x28); if (!PtrChain1) continue;
		uintptr_t PtrChain2 = read<uintptr_t>(PtrChain1 + 0xD8); if (!PtrChain2) continue;

		for (auto Component = 0x80; Component < 0xf0; Component += sizeof(uintptr_t))
		{
			uintptr_t PtrChain3 = read<uintptr_t>(PtrChain2 + Component);

			if (!PtrChain3)
				continue;

			if (read<uintptr_t>(PtrChain3) != (baseAddress + 0x3a83e58))
				continue;

			write<BYTE>(PtrChain3 + 0x552, true);
			write<BYTE>(PtrChain3 + 0x554, true);
		}
	}
}



D3DXVECTOR2 screen_head, screen_nike, screen_normalhead;
RGBA ESPColor = { colors::espcol[0] * 255, colors::espcol[1] * 255, colors::espcol[2] * 255, 255 };



bool DoLevelUp()
{
	if (!settings::misc::expfarm)
		return false;

	while (TRUE)
	{
		uint64_t GetProfile = read<uint64_t>(managers::profile_manager + local::local_container);

		if (!GetProfile)
			return false;

		uint64_t LocalPlayer = GetLocalPlayer();

		uint64_t player_info = read<uint64_t>(LocalPlayer + entity::entity_info);

		if (!player_info)
			return false;

		uint64_t team_container = read<uint64_t>(player_info + 0x88);

		if (!team_container)
			return false;

		int8_t get_team = read<int8_t>(team_container + 0x30);

		if (RoundState() == 3)
		{
			uint32_t GetLevel = read<uint32_t>(GetProfile + 0x5c0);


			write<int32_t>(player_info + 0x154, 10000);
			write<int8_t>(team_container + 0x30, get_team == 3 ? 4 : 3);

		}
		else
		{
			keybd_event(VK_ESCAPE, 1, NULL, NULL);
			keybd_event(VK_ESCAPE, 1, KEYEVENTF_KEYUP, NULL);

			keybd_event(VK_RETURN, 156, NULL, NULL);
			keybd_event(VK_RETURN, 156, KEYEVENTF_KEYUP, NULL);
		}
		Sleep(10);

		if (GetAsyncKeyState(VK_F6))
			break;
	}
}

D3DXVECTOR3 GetBonePos(uint64_t player, int id)
{
	if (!player)
		return D3DXVECTOR3();

	uint64_t pSkeleton = read<uint64_t>(player + 0x20);

	if (!pSkeleton)
		return D3DXVECTOR3();

	return read<D3DXVECTOR3>(pSkeleton + settings::aimbot::EntityBone[id]);

}

bool GetAimKey()
{
	return (GetAsyncKeyState(settings::hotkeys::aimkey) & 0x8000);
}

bool aimboting = false;

uintptr_t GetNearestEnemy()
{
	float bestFov = settings::aimbot::aimfov * 8;
	float bestDistance = 9999999.0f;
	int bestHp = 120;

	UINT16 entity_count = read<UINT16>(managers::game_manager + entity::entity_count);
	uint64_t entity_list = read<uint64_t>(managers::game_manager + entity::entity_list);

	for (int i = 0; i < entity_count; i++)
	{

		uintptr_t entity = read<uintptr_t>(entity_list + i * 0x8);

		float health = GetPlayerHealth(entity);

		D3DXVECTOR3 EntityHead = math::skeleton::GetEntityHead(entity), EntityPosition = math::skeleton::GetEntityPos(entity), LocalPosition = math::skeleton::GetEntityPos(GetLocalPlayer());

		if (!entity)
			continue;

		if (entity == GetLocalPlayer())
			continue;

		if (!settings::aimbot::team)
			if (IsTeam(entity) == IsTeam(GetLocalPlayer())) continue;

		if (!health > 0)
			continue;

		float fov = math::aimbot::ScreenToEnemy(EntityHead);
		float DistPly = D3DXVec3Length(&(math::GetViewTranslation() - EntityHead));

		if (fov < bestFov)
		{
			bestFov = fov;
			resultant_entity = entity;
		}
	}
	return 0;
}

/*void watermark() 
{

misc::overlayfps;

} */

void ValidTarget()
{
	if (resultant_entity)
	{
		float health = GetPlayerHealth(resultant_entity);

		if (health <= 0)
		{
			resultant_entity = NULL;
			return;
		}
		float DistPly = D3DXVec3Length(&(math::GetViewTranslation() - math::skeleton::GetEntityHead(resultant_entity)));
		float fov = math::aimbot::ScreenToEnemy(math::skeleton::GetEntityHead(resultant_entity));
		if (fov > settings::aimbot::aimfov * 8)
			resultant_entity = NULL;
	}
	else
	{
		resultant_entity = NULL;
	}
}

bool AimbotLoop()
{
	if (!settings::aimbot::aimbot)
		return false;
	//if (settings::aimbot::sticktotarget)
	//{
	//	if (GetAsyncKeyState(settings::hotkeys::aimkey) & 0x8000)
	//	{
	//		aimboting = true;
	//	}
	//	else
	//	{
	//		aimboting = false;
	//	}
	//}
	//else
	//{
	//	aimboting = false;
	//}

	ValidTarget();
	//if (!GetWeapon(GetLocalPlayer()))
	//	return;

	if (GetAsyncKeyState(0x52))
		return false;

	if (GetAsyncKeyState(0x56))
		return false;


	if (!resultant_entity)
	{
		GetNearestEnemy();
	}
	else
	{
		float health = GetPlayerHealth(resultant_entity);
		if (!resultant_entity)
			return false;
		if (health <= 0)
			return false;

		if (GetAsyncKeyState(settings::hotkeys::aimkey) & 0x8000)
		{
			D3DXVECTOR4 angle = math::aimbot::GetAngle(math::skeleton::GetEntityHead(GetLocalPlayer()), GetBonePos(resultant_entity, settings::aimbot::hitbox), math::aimbot::get_viewangle(GetLocalPlayer(), 0xc0));
			D3DXVECTOR2 twod;
			math::WorldToScreen(math::skeleton::GetEntityHead(resultant_entity), &twod);
			math::aimbot::set_viewangle(GetLocalPlayer(), 0xc0, angle);
		}
	
		//else if (Globals::Aimbot::aimonhand && GetAsyncKeyState(Globals::Aimbot::aimonhandkey))
		//{
		//	Vector4 angle = math::aimbot::(GetPlayerHead(GetLocalPlayer()), GetBonePos(resultant_entity, Globals::BONE_HAND), math::aimbot::get_viewangle(GetLocalPlayer(), 0xc0));
		//	set_viewangle(GetLocalPlayer(), 0xc0, angle);
		//}

	}
	return true;
}

DWORD WINAPI entity_thread(LPVOID)
{
	while (TRUE)
	{
		vector<uint64_t> EntityList;

		UINT16 entity_count = read<UINT16>(managers::game_manager + entity::entity_count);
		uint64_t entity_list = read<uint64_t>(managers::game_manager + entity::entity_list);
		
		for (int i = 0; i < entity_count; i++)
		{
			uint64_t entity = read<uint64_t>(entity_list + i * 0x8);

			if (!entity || entity == GetLocalPlayer())
				continue;

			float health = GetPlayerHealth(entity);

			if (!health > 0)
				continue;

			//printf("entity_count -> %d\n", entity_count);
			//printf("entity -> %d\n", entity);
			//printf("health -> %f\n", health);

			EntityList.emplace_back(entity);
		}

		RainbowEntityList.clear();
		RainbowEntityList = EntityList;

		Sleep(150);
	}

	return true;
}

DWORD WINAPI aim_thread(LPVOID)
{
	while (TRUE)
	{
		AimbotLoop();
		Sleep(1);
	}

	return true;
}

DWORD WINAPI xp_thread(LPVOID)
{
	while (TRUE)
	{
		DoLevelUp();
		Sleep(500);
	}

	return true;
}


bool initialize()
{
	m_driver_control = kernel_control_function();

	if (!m_driver_control)
	{
		std::cout << XorString("[-] can't connect with driver ") << '\n';
		return false;
	}

	PID = GetProcessID(XorString("RainbowSix.exe"));

	if (!PID)
		return false;

	baseAddress = call_driver_control(
		m_driver_control, ID_GET_PROCESS_BASE, PID);

	if (!baseAddress)
		return false;

	printf(XorString("Pid Size: %d\n"), PID);
	printf(XorString("Base SizeL 0x%p\n"), baseAddress);

	//ShowWindow(GetConsoleWindow(), SW_HIDE);

	managers::game_manager = read<uint64_t>(baseAddress + 0x53c81f8);//process::pattern_scan(XorString("48 8B 05 ? ? ? ? 8B 8E"), 3, 7);

	if (!managers::game_manager)
	{
		std::cout << XorString("game_manager needs to be updated") << std::endl;
		return false;
	}
	std::cout << XorString("[+] game_manager found at 0x") << std::hex << managers::game_manager << '\n';



	managers::profile_manager = read<uint64_t>(baseAddress + 0x53b5328);//process::pattern_scan(XorString("48 8b 05 ? ? ? ? 33 d2 4c 8b 40 78"), 3, 7);

	if (!managers::profile_manager)
	{
		std::cout << XorString("profile_manager needs to be updated") << std::endl;
		return false;
	}
	std::cout << XorString("[+] profile_manager found at 0x") << std::hex << managers::profile_manager << '\n';



	managers::round_manager = read<uint64_t>(baseAddress + 0x53c81f8);//process::pattern_scan(XorString("48 8b 05 ? ? ? ? 8b 90 e8 02"), 3, 7);

	//if (!managers::round_manager)
	//{
	//	std::cout << XorString("round_manager needs to be updated") << std::endl;
	//	return false;
	//}
	std::cout << XorString("[+] round_manager found at 0x") << std::hex << managers::round_manager << '\n';



	managers::marker_manager = read<uint64_t>(baseAddress + 0x3aca308);//process::pattern_scan(XorString("4c 8d 0d ? ? ? ? 48 ? ? ? 48 8d 8b ? ? ? ? 4c ? ? 48 8d ? ? ? ? ? e8"), 3, 7);

	//if (!managers::marker_manager)
	//{
	//	std::cout << XorString("marker_manager needs to be updated") << std::endl;
	//	return false;
	//}
	std::cout << XorString("[+] marker_manager found at 0x") << std::hex << managers::marker_manager << '\n';


	managers::network_manager = read<uint64_t>(baseAddress + 0x53b7a88);//process::pattern_scan(XorString("48 8b 05 ? ? ? ? 48 85 c0 0f 84 ? ? ? ? 48 8b 88 ? ? ? ? 48 85 c9 0f 84 ? ? ? ? 4c 8b 8d"), 3, 7);

	if (!managers::network_manager)
	{
		std::cout << XorString("network_manager needs to be updated") << std::endl;
		return false;
	}
	std::cout << XorString("[+] network_manager found at 0x") << std::hex << managers::network_manager << '\n';


	managers::chams_manager = read<uint64_t>(baseAddress + 0x60ab0c0);//process::pattern_scan(XorString("48 8b 0d ? ? ? ? 48 8b d7 e8 ? ? ? ? 48 85 c0 74 ? 4c"), 3, 7);

	if (!managers::chams_manager)
	{
		std::cout << XorString("chams_manager needs to be updated") << std::endl;
		return false;
	}
	std::cout << XorString("[+] chams_manager found at 0x") << std::hex << managers::chams_manager << '\n';

	managers::fov_manager = read<uint64_t>(baseAddress + 0x53c8200);//process::pattern_scan(XorString("48 8b 05 ? ? ? ? f3 44 0f 10 91"), 3, 7);
	std::cout << XorString("[+] fov_manager found at 0x") << std::hex << managers::fov_manager << '\n';


	auto GameRenderer = read<uint64_t>(managers::profile_manager + camera::local_camera);

	if (!GameRenderer)
		return false;

	auto pEngineLink = read<uint64_t>(GameRenderer + 0x0);

	auto pEngine = read<uint64_t>(pEngineLink + camera::engine_camera);

	if (!pEngine)
		return false;

	managers::camera_manager = read<uint64_t>(pEngine + camera::camera_manager);

	HANDLE top = CreateThread(NULL, 0, entity_thread, NULL, NULL, NULL);
	CloseHandle(top);

	HANDLE top2 = CreateThread(NULL, 0, aim_thread, NULL, NULL, NULL);
	CloseHandle(top2);

	//HANDLE top3 = CreateThread(NULL, 0, xp_thread, NULL, NULL, NULL);
	//CloseHandle(top3);


	return true;
}



uint8_t shellcode[]
{
	0x53, 0x48, 0x83, 0xec, 0x20, 0x48, 0xb8, 0xfe, 0xca, 0xef, 0xbe, 0xfe,
	0xca, 0xad, 0xde, 0xff, 0xd0, 0x48, 0x8d, 0x54, 0x24, 0x28, 0x48, 0x89,
	0xc3, 0x48, 0x8b, 0x42, 0x30, 0x48, 0x89, 0xc1, 0x48, 0xb8, 0xfe, 0xca,
	0xef, 0xbe, 0xfe, 0xca, 0xad, 0xde, 0x48, 0x29, 0xc1, 0x48, 0x81, 0xf9,
	0x41, 0x52, 0xe0, 0x00, 0x75, 0x1a, 0x48, 0x8b, 0x42, 0x30, 0x48, 0x8d,
	0x48, 0x68, 0x48, 0x89, 0x4a, 0x30, 0x48, 0x89, 0xf8, 0xc6, 0x40, 0x52,
	0x00, 0x48, 0x89, 0xf8, 0xc6, 0x40, 0x51, 0x00, 0x48, 0x89, 0xd8, 0x48,
	0x83, 0xc4, 0x20, 0x5b, 0xc3
};

bool unlocks(bool enable)
{
	static const auto image_allocation = AllocateMemory(sizeof(shellcode), PAGE_EXECUTE_READWRITE);

	if (!image_allocation)
		return false;

	static const auto function_address = baseAddress + 0x5adf630;

	static auto done_once = false;

	if (!done_once)
	{
		*reinterpret_cast<uint64_t*>(&shellcode[0x7]) = read<uint64_t>(function_address);
		*reinterpret_cast<uint64_t*>(&shellcode[0x22]) = baseAddress;

		for (auto i = 0; i < sizeof(shellcode); i++)
			write(image_allocation + i, shellcode[i]);

		write(function_address, image_allocation);
		done_once = true;
	}

	write(image_allocation + 0x4f, enable ? 0 : 1);

	return enable;
}