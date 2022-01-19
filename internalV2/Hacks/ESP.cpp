#include "../Include.hpp"

ESP* esp = new ESP();

void ESP::CalculateDormant(int userid, bool dormant)
{
	// it's NEW bitch
	if (Dormant.find(userid) == Dormant.end())
	{
		// add it
		if (dormant)
			Dormant.insert(std::pair(userid, 0));
		else
			Dormant.insert(std::pair(userid, 255.f));
	}
	
	// otherwise update as appropriate
	if (dormant)
		Dormant[userid] = 0;
	else
		Dormant[userid] = 255;
	return;
}

Vector VectorTransform(Vector vecTransform, matrix3x4_t matrix)
{
	return Vector(vecTransform.DotProduct(matrix[0]) + matrix[0][3],
		vecTransform.DotProduct(matrix[1]) + matrix[1][3],
		vecTransform.DotProduct(matrix[2]) + matrix[2][3]);
}

bool ESP::GetBounds(Entity* ent)
{
	const ICollideable* pCollideable = ent->GetCollideable();

	if (pCollideable == nullptr)
		return false;

	// get mins/maxs
	const Vector vecMin = pCollideable->OBBMins();
	const Vector vecMax = pCollideable->OBBMaxs();

	/*
	 * build AABB points
	 *
	 * points navigation:
	 * [N] [back/front][left/right][bottom/top]
	 *	0 - blb
	 *	1 - brb
	 *	2 - frb
	 *	3 - flb
	 *	4 - frt
	 *	5 - brt
	 *	6 - blt
	 *	7 - flt
	 */
	static Vector arrPoints[8] =
	{
		Vector(vecMin.x, vecMin.y, vecMin.z),
		Vector(vecMin.x, vecMax.y, vecMin.z),
		Vector(vecMax.x, vecMax.y, vecMin.z),
		Vector(vecMax.x, vecMin.y, vecMin.z),
		Vector(vecMax.x, vecMax.y, vecMax.z),
		Vector(vecMin.x, vecMax.y, vecMax.z),
		Vector(vecMin.x, vecMin.y, vecMax.z),
		Vector(vecMax.x, vecMin.y, vecMax.z)
	};

	// get transformation matrix
	matrix3x4_t* matTransformed = ent->GetCoordinateFrame();

	float flLeft = FLT_MAX;
	float flTop = FLT_MAX;
	float flRight = std::numeric_limits<float>::lowest();
	float flBottom = std::numeric_limits<float>::lowest();

	// get screen points position
	static Vector arrScreen[] = { Vector(), Vector(), Vector(), Vector(), Vector(), Vector(), Vector(), Vector() };
	for (int i = 0; i < 8; i++)
	{
		Vector world = VectorTransform(arrPoints[i], *matTransformed);
		if (!WorldToScreen(world, arrScreen[i]))
			return false;

		/*
		 * transformed points navigation:
		 * [N] [back/front][left/right][bottom/top]
		 *	0 - frb
		 *	1 - flb
		 *	2 - blb
		 *	3 - brb
		 *	4 - frt
		 *	5 - blt
		 *	6 - brt
		 *	7 - flt
		 */
		flLeft = min(flLeft, arrScreen[i].x);
		flTop = min(flTop, arrScreen[i].y);
		flRight = max(flRight, arrScreen[i].x);
		flBottom = max(flBottom, arrScreen[i].y);
	}

	TL = Vector(flLeft, flTop, 0);
	BR = Vector(flRight, flBottom, 0);
	return true;
}

void ESP::DrawName(char name[128], int userid)
{
	if (!cfg->esp.Name) return;

	static DWORD FONT = I::surface->FontCreate();
	static bool Once = true;
	if (Once)
	{
		Once = false;
		I::surface->SetFontGlyphSet(FONT, "Verdana", 12, 1, 0, 0, FONTFLAG_DROPSHADOW);// FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE);
	}


	std::string TEXT = name;
	if (lagcomp->PlayerList.find(userid) != lagcomp->PlayerList.end())
		TEXT += " " + std::to_string((float)lagcomp->PlayerList[userid].goodShots / ((float)lagcomp->PlayerList[userid].goodShots + lagcomp->PlayerList[userid].badShots) * 100.f);
	static std::wstring wide_string;
	wide_string = std::wstring(TEXT.begin(), TEXT.end());

	//vgui_spew_fonts TYPE THAT IN CONSOLE TO GET ALL FONTS
	I::surface->DrawSetTextFont(FONT);
	Color clr = cfg->esp.NameColor;
	clr.color[3] = Dormant[userid];
	I::surface->DrawSetTextColor(clr);
	I::surface->DrawSetTextPos(TL.x, TL.y - 14);
	I::surface->DrawPrintText(wide_string.c_str(), wcslen(wide_string.c_str()));
}

void ESP::DrawSkeleton(Entity* Ent, int userid)
{
	if (!cfg->esp.Skeleton) return;

	Color clr = cfg->esp.SkeletonColor;
	clr.color[3] = Dormant[userid];

	I::surface->DrawSetColor(clr);

	//https://www.unknowncheats.me/wiki/Counter_Strike_Global_Offensive:Bone_ESP 
	//p help, prob should implement properly but will do later
	studiohdr_t* StudioModel = I::modelinfo->GetStudioModel(Ent->GetModel());
	if (!StudioModel)
		return;

	Vector velocity = Ent->GetVecVelocity();

	static matrix3x4_t BoneToWorldOut[128];
	if (Ent->SetupBones(BoneToWorldOut, 128, 256, 0))
	{
		for (int i = 0; i < StudioModel->nBones; i++)
		{
			mstudiobone_t* Bone = StudioModel->GetBone(i);
			if (!Bone || !(Bone->iFlags & 256) || Bone->iParent == -1)
				continue;

			Vector Screen1;
			Vector loc1 = Vector(BoneToWorldOut[i][0][3], BoneToWorldOut[i][1][3], BoneToWorldOut[i][2][3]);
			if (!WorldToScreen(loc1, Screen1))
				continue;

			Vector Screen2;
			Vector loc2 = Vector(BoneToWorldOut[Bone->iParent][0][3], BoneToWorldOut[Bone->iParent][1][3], BoneToWorldOut[Bone->iParent][2][3]);
			if (!WorldToScreen(loc2, Screen2))
				continue;

			I::surface->DrawLine(Screen1.x, Screen1.y, Screen2.x, Screen2.y);
		}
	}
}

void ESP::DrawExtrapolatedSkeletons()
{
	for (auto& player : lagcomp->PlayerList)
	{
		if (!player.second.pEntity) continue;
		if (player.second.records.empty()) continue;
		if (!player.second.records.front().extrapolated) continue;

		I::surface->DrawSetColor(Color(255,0,0,255));

		studiohdr_t* StudioModel = I::modelinfo->GetStudioModel(player.second.pEntity->GetModel());
		if (!StudioModel)
			continue;
		
		LagComp::Record& record = player.second.records.front();

		for (int i = 0; i < StudioModel->nBones; i++)
		{
			mstudiobone_t* Bone = StudioModel->GetBone(i);
			if (!Bone || !(Bone->iFlags & 256) || Bone->iParent == -1)
				continue;

			Vector Screen1;
			Vector loc1 = Vector(record.Matrix[i][0][3], record.Matrix[i][1][3], record.Matrix[i][2][3]);
			if (!WorldToScreen(loc1, Screen1))
				continue;

			Vector Screen2;
			Vector loc2 = Vector(record.Matrix[Bone->iParent][0][3], record.Matrix[Bone->iParent][1][3], record.Matrix[Bone->iParent][2][3]);
			if (!WorldToScreen(loc2, Screen2))
				continue;

			I::surface->DrawLine(Screen1.x, Screen1.y, Screen2.x, Screen2.y);
		}
	}

	
}

void ESP::DrawHealth(int health, int userid)
{
	if (!cfg->esp.Health) return;

	// if at edges --> bad
	static int width = -1;
	static int height = -1;
	I::surface->GetScreenSize(width, height);
	if (BR.x <= 1 || BR.x >= width - 1)
		return;

	int Height = BR.y - TL.y;

	int HeightBar = (int)(Height * (health / 100.0f));
	int WidthBar = (int)(2 + Height / 100.0f);

	Color red = Color(255, 0, 0);
	red.color[3] = Dormant[userid];

	Color green = Color(0, 255, 0);
	green.color[3] = Dormant[userid];

	I::surface->DrawSetColor(red);		//red
	I::surface->DrawFilledRect(BR.x + 1, BR.y - Height, BR.x + WidthBar + 1, BR.y);

	I::surface->DrawSetColor(green);		//green
	I::surface->DrawFilledRect(BR.x + 1, BR.y - HeightBar, BR.x + WidthBar + 1, BR.y);

	static DWORD FONT = I::surface->FontCreate();
	static bool Once = true;
	if (Once)
	{
		Once = false;
		I::surface->SetFontGlyphSet(FONT, "Small Fonts", 10, 400, 0, 0, FONTFLAG_DROPSHADOW);// FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE);
	}

	std::string TEXT = std::to_string(health);
	static std::wstring wide_string;
	wide_string = std::wstring(TEXT.begin(), TEXT.end());

	//vgui_spew_fonts TYPE THAT IN CONSOLE TO GET ALL FONTS
	I::surface->DrawSetTextFont(FONT);
	I::surface->DrawSetTextColor(Color(255 * (100 - health) / 100.f, 255 * (health) / 100.f, 0, Dormant[userid]));
	I::surface->DrawSetTextPos(BR.x + WidthBar + 1, BR.y - HeightBar);
	I::surface->DrawPrintText(wide_string.c_str(), wcslen(wide_string.c_str()));
}

void ESP::DrawWeapon(Entity* entity, int userid)
{
	if (!cfg->esp.Weapon) return;

	static DWORD FONT = I::surface->FontCreate();
	static bool Once = true;
	if (Once)
	{
		Once = false;
		I::surface->SetFontGlyphSet(FONT, "Verdana", 12, 1, 0, 0, FONTFLAG_DROPSHADOW);// FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE);
	}

	Entity* weap = entity->GetActiveWeapon();

	if (!weap) return;

	CCSWeaponData* data = weap->GetWeaponData();
	if (!data) return;

	std::string TEXT = data->szWeaponName;
	if (TEXT.length() > 13)
		TEXT = TEXT.substr(13);

	static std::wstring wide_string;
	wide_string = std::wstring(TEXT.begin(), TEXT.end());

	//vgui_spew_fonts TYPE THAT IN CONSOLE TO GET ALL FONTS
	I::surface->DrawSetTextFont(FONT);
	Color clr = cfg->esp.WeaponColor;
	clr.color[3] = Dormant[userid];
	I::surface->DrawSetTextColor(clr);
	I::surface->DrawSetTextPos(TL.x, BR.y);
	I::surface->DrawPrintText(wide_string.c_str(), wcslen(wide_string.c_str()));
}

void ESP::DrawProjectiles(Entity* entity)
{

	// velocity check
	if (entity->GetVecVelocity().VecLength() < 4) return;

	static DWORD FONT = I::surface->FontCreate();
	static bool Once = true;
	if (Once)
	{
		Once = false;
		I::surface->SetFontGlyphSet(FONT, "Verdana", 12, 1, 0, 0, FONTFLAG_DROPSHADOW);// FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE);
	}

	auto client_class = entity->GetClientClass();

	if (!client_class)
		return;

	auto model = entity->GetModel();

	if (!model)
		return;

	auto studio_model = I::modelinfo->GetStudioModel(model);

	if (!studio_model)
		return;

	auto name = (std::string)studio_model->szName;

	if (name.find("dropped") != std::string::npos ||
		client_class->nClassID == EClassIndex::CBaseCSGrenadeProjectile || client_class->nClassID == EClassIndex::CDecoyProjectile || client_class->nClassID == EClassIndex::CMolotovProjectile)
	{

		auto grenade_origin = entity->GetVecOrigin();
		auto grenade_position = Vector(0, 0, 0);

		if (!WorldToScreen(grenade_origin, grenade_position))
			return;

		std::string grenade_name;

		if (name.find("flashbang") != std::string::npos)
		{
			grenade_name = "FLASHBANG";
		}
		else if (name.find("smokegrenade") != std::string::npos)
		{
			grenade_name = "SMOKE";
		}
		else if (name.find("incendiarygrenade") != std::string::npos)
		{
			grenade_name = "INCENDIARY";
		}
		else if (name.find("molotov") != std::string::npos)
		{
			grenade_name = "MOLOTOV";
		}
		else if (name.find("fraggrenade") != std::string::npos)
		{
			grenade_name = "HE GRENADE";
		}
		else if (name.find("decoy") != std::string::npos)
		{
			grenade_name = "DECOY";
		}
		else
			return;

		static std::wstring wide_string;
		wide_string = std::wstring(grenade_name.begin(), grenade_name.end());

		//vgui_spew_fonts TYPE THAT IN CONSOLE TO GET ALL FONTS
		I::surface->DrawSetTextFont(FONT);
		Color clr = Color(255, 0, 0, 255);
		I::surface->DrawSetTextColor(clr);
		I::surface->DrawSetTextPos(grenade_position.x, grenade_position.y);
		I::surface->DrawPrintText(wide_string.c_str(), wcslen(wide_string.c_str()));

	}
}

void ESP::RunPaintTraverse()
{
	//DrawExtrapolatedSkeletons();

	if (!cfg->esp.Enable)
		return;

	for (int i = 0; i < I::engine->GetMaxClients(); i++)
	{
		if (i == G::LocalplayerIndex)
			continue;

		Entity* Ent = I::entitylist->GetClientEntity(i);
		if (!Ent) continue;

		if (Ent->GetTeam() == G::LocalplayerTeam) continue;

		PlayerInfo_t PlayerInfo;
		if (!I::engine->GetPlayerInfo(i, &PlayerInfo))
			continue;

		float health = Ent->GetHealth();
		if (!(health > 0))
			continue;

		bool dormant = false;

		CalculateDormant(PlayerInfo.nUserID, Ent->IsDormant());

		// do bounds for needed ones
		if (!GetBounds(Ent)) continue;
		DrawName(PlayerInfo.szName, PlayerInfo.nUserID);
		DrawSkeleton(Ent, PlayerInfo.nUserID);
		DrawHealth(health, PlayerInfo.nUserID);
		DrawWeapon(Ent, PlayerInfo.nUserID);

	}

	for (int i = 1; i <= I::entitylist->GetHighestEntityIndex(); i++)
	{
		Entity* e = I::entitylist->GetClientEntity(i);

		if (!e)
			continue;

		if (e->IsPlayer())
			continue;

		if (e->IsDormant())
			continue;

		auto client_class = e->GetClientClass();

		if (!client_class)
			continue;

		switch (client_class->nClassID)
		{
		case EClassIndex::CEnvTonemapController:
			/*world_modulation(e);*/
			break;
		case EClassIndex::CInferno:
			/*molotov_timer(e);*/
			break;
		case EClassIndex::CSmokeGrenadeProjectile:
			/*smoke_timer(e);*/
			break;
		case EClassIndex::CPlantedC4:
			/*bomb_timer(e);*/
			break;
		case EClassIndex::CC4:
			/*
			if (g_cfg.player.type[ENEMY].flags[FLAGS_C4] || g_cfg.player.type[TEAM].flags[FLAGS_C4] || g_cfg.player.type[LOCAL].flags[FLAGS_C4] || g_cfg.esp.bomb_timer)
			{
				auto owner = (player_t*)m_entitylist()->GetClientEntityFromHandle(e->m_hOwnerEntity());

				if ((g_cfg.player.type[ENEMY].flags[FLAGS_C4] || g_cfg.player.type[TEAM].flags[FLAGS_C4] || g_cfg.player.type[LOCAL].flags[FLAGS_C4]) && owner->valid(false, false))
					g_ctx.globals.bomb_carrier = owner->EntIndex();
				else if (g_cfg.esp.bomb_timer && !owner->is_player())
				{
					auto screen = ZERO;

					if (math::world_to_screen(e->GetAbsOrigin(), screen))
						render::get().text(fonts[ESP], screen.x, screen.y, Color(215, 20, 20), HFONT_CENTERED_X | HFONT_CENTERED_Y, "BOMB");
				}
			}
			*/

			break;
		default:
			DrawProjectiles(e);

			break;
		}
	}
}

void ESP::RunEndScene()
{
}

