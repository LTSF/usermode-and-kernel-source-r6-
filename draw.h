#include <xmmintrin.h>
using namespace settings;

void entity_draw()
{
	for (uint64_t entity : RainbowEntityList)
	{
		D3DXVECTOR3 EntityHead = math::skeleton::GetEntityHead(entity), EntityPosition = math::skeleton::GetEntityPos(entity), LocalPosition = math::skeleton::GetEntityPos(GetLocalPlayer());
		D3DXVECTOR2 screen_head, screen_nike, screen_normalhead;
		float health = GetPlayerHealth(entity);
		std::string PlayerName = GetPlayerName(entity); if (PlayerName == "") PlayerName = XorString("BOT");
		float distance = D3DXVec3Length(&(EntityPosition - LocalPosition));
		RGBA ESPColor = { colors::espcol[0] * 255, colors::espcol[1] * 255, colors::espcol[2] * 255, 255 };
		RGBA ESPColort = { colors::espcolteam[0] * 255, colors::espcolteam[1] * 255, colors::espcolteam[2] * 255, 255 };

		if (settings::aimbot::fovcircle) {
			DrawCircle(Width / 2, Height / 2, settings::aimbot::aimfov * 8, &Col.red, 360, false);

		}

		if (math::WorldToScreen(EntityPosition, &screen_nike) && math::WorldToScreen(D3DXVECTOR3(EntityHead.x, EntityHead.y, EntityHead.z + 0.3), &screen_head), math::WorldToScreen(EntityHead, &screen_normalhead))
		{
			char cName[300];
			int BoxHeight = (screen_nike.y - screen_head.y);
			int BoxWidth = BoxHeight / 1.8;


			int G = (255 * health / 120);
			int R = 0 - G;
			RGBA healthcol = { R, G, 0, 255 };

			if (!visuals::team)
				if (IsTeam(entity) == IsTeam(GetLocalPlayer())) continue;

			if (IsTeam(entity) == IsTeam(GetLocalPlayer())) ESPColor = ESPColort;

			if (visuals::head)
				DrawCircle(screen_normalhead.x, screen_normalhead.y, BoxHeight / 20, &Col.white_, 15);
			if (visuals::box)
			{
				if (visuals::boxMode == 0 || visuals::boxMode == 1)
				{
					if (!visuals::healthbased)
						DrawNormalBox(screen_nike.x - (BoxWidth / 2), screen_head.y, BoxWidth, BoxHeight, menu::box_thick, &ESPColor);
					else if (visuals::healthbased)
					{
						if (health <= 120 || health >= 80)
							DrawNormalBox(screen_nike.x - (BoxWidth / 2), screen_head.y, BoxWidth, BoxHeight, menu::box_thick, &Col.green);
						if (health <= 79)
							DrawNormalBox(screen_nike.x - (BoxWidth / 2), screen_head.y, BoxWidth, BoxHeight, menu::box_thick, &healthcol);
					}
				}
				else if (visuals::boxMode == 2 || visuals::boxMode == 3)
				{
					if (visuals::outline)
					{
						DrawCornerBox(screen_nike.x - BoxWidth / 2 + 1, screen_head.y, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
						DrawCornerBox(screen_nike.x - BoxWidth / 2 - 1, screen_head.y, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
						DrawCornerBox(screen_nike.x - BoxWidth / 2, screen_head.y + 1, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
						DrawCornerBox(screen_nike.x - BoxWidth / 2, screen_head.y - 1, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
					}
					if (!visuals::healthbased)
						DrawCornerBox(screen_normalhead.x, screen_normalhead.y, BoxHeight / 12, BoxHeight / 12, 1, &Col.white_);
					else if (visuals::healthbased)
					{
						if (health <= 120 || health >= 80)
							DrawCornerBox(screen_nike.x - (BoxWidth / 2), screen_head.y, BoxWidth, BoxHeight, menu::box_thick, &Col.green);
						if (health <= 79)
							DrawCornerBox(screen_nike.x - (BoxWidth / 2), screen_head.y, BoxWidth, BoxHeight, menu::box_thick, &healthcol);
					}
				}

				if (visuals::boxMode == 1 || visuals::boxMode == 3)
					DrawFilledRect(screen_nike.x - (BoxWidth / 2), screen_head.y, BoxWidth, BoxHeight, &Col.glassblack);
			}

			if (visuals::health)
			{
				if (visuals::healthMode == 0)
				{
					DrawFilledRect(screen_nike.x - 4 - (BoxWidth / 2), screen_nike.y - (BoxHeight) * 120 / 120, 2, (BoxHeight)*health / 120, &Col.black_);

					if (health <= 120 || health >= 80)
						DrawFilledRect(screen_nike.x - 4 - (BoxWidth / 2), screen_nike.y - (BoxHeight)*health / 120, 2, (BoxHeight)*health / 120, &Col.green_);
					if (health <= 79)
						DrawFilledRect(screen_nike.x - 4 - (BoxWidth / 2), screen_nike.y - (BoxHeight)*health / 120, 2, (BoxHeight)*health / 120, &healthcol);
				}
				else if (visuals::healthMode == 1)
				{
					DrawFilledRect(screen_nike.x + 6 + (BoxWidth / 2), screen_nike.y - (BoxHeight) * 120 / 120, 2, (BoxHeight)*health / 120, &Col.black_);

					if (health <= 120 || health >= 80)
						DrawFilledRect(screen_nike.x + 6 + (BoxWidth / 2), screen_nike.y - (BoxHeight)*health / 120, 2, (BoxHeight)*health / 120, &Col.green_);
					if (health <= 79)
						DrawFilledRect(screen_nike.x + 6 + (BoxWidth / 2), screen_nike.y - (BoxHeight)*health / 120, 2, (BoxHeight)*health / 120, &healthcol);
				}
			}

			if (visuals::snaplines)
				DrawLine(Width / 2, Height, screen_nike.x, screen_nike.y, &ESPColor, 1);


			if (visuals::name)
			{
				if (visuals::nameMode == 0 || visuals::nameMode == 1)
				{
					sprintf_s(cName, XorString("%s"), PlayerName.c_str());
				}
				else if (visuals::nameMode == 2 || visuals::nameMode == 3)
				{
					sprintf_s(cName, XorString("[%dm] %s"), (int)distance, PlayerName.c_str());
				}

				if (visuals::nameMode == 0 || visuals::nameMode == 2)
					DrawString(menu::font_size, screen_head.x, screen_head.y - 5, &Col.white_, true, true, cName);
				else if (visuals::nameMode == 1 || visuals::nameMode == 3)
					DrawString(menu::font_size, screen_nike.x, screen_nike.y + 15, &Col.white_, true, true, cName);
			}
		}


		
	

		rainbow();

			
				speedhack();
			
				NoRecoil_and_NoSpread();
			freeze_lobby(misc::freezelobby);
			if (misc::noflash)
				noFlash();
			if (misc::noclip)
				NoClip();
			if (visuals::chams)
				chams();
			update_fov(settings::misc::fov);
			update_viewmodel_fov(settings::misc::viewmodelfov);
		
		if (RoundState() == 3)
		{
		 Caveira(settings::visuals::cav, entity);
		}
	}
}

void draw_things()
{
	entity_draw();
	shortcurts();
	crosshair();
}