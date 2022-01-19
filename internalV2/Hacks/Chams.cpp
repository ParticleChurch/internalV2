#include "../Include.hpp"

Chams* chams = new Chams();

void Chams::OverideMat(bool ignorez, int material, float transparent, Color rgba, void* thisptr, void* ctx, void* state, const ModelRenderInfo& info, matrix3x4_t* customBoneToWorld)
{
	static Material* current;
	current = GetCurMaterial(material);

	if (material == 2 || material == 4 || material == 5 || material == 6)
		current->FindVar("$envmaptint")->SetVectorValue(rgba.r() / 255.0f,
			rgba.g() / 255.0f,
			rgba.b() / 255.0f);
	else
		current->ColorModulate(
			rgba.r() / 255.0f,
			rgba.g() / 255.0f,
			rgba.b() / 255.0f);

	current->AlphaModulate(transparent);
	current->SetMaterialVarFlag(MaterialVarFlag::IGNOREZ, ignorez);

	I::modelrender->ForcedMaterialOverride(current);
}

void Chams::Init()
{
	static bool DoOnce = true;
	if (!DoOnce)
		return;
	DoOnce = false;

	flat = I::materialsystem->CreateMaterial("flat", KeyValues::FromString("UnlitGeneric", nullptr));
	normal = I::materialsystem->CreateMaterial("normal", KeyValues::FromString("VertexLitGeneric", nullptr));
	glow = I::materialsystem->CreateMaterial("glow", KeyValues::FromString("VertexLitGeneric", "$additive 1 $envmap models/effects/cube_white $envmapfresnel 1 $alpha .7"));
	{
		const auto kv = KeyValues::FromString("VertexLitGeneric", "$envmap editor/cube_vertigo $envmapcontrast 1 $basetexture dev/zone_warning proxies { texturescroll { texturescrollvar $basetexturetransform texturescrollrate 0.6 texturescrollangle 90 } }");
		kv->SetString("$envmaptint", "[.7 .7 .7]");
		animated = I::materialsystem->CreateMaterial("animated", kv);
	}
	{
		const auto kv = KeyValues::FromString("VertexLitGeneric", "$baseTexture detail/dt_metal1 $additive 1 $envmap editor/cube_vertigo");
		kv->SetString("$color", "[.05 .05 .05]");
		glass = I::materialsystem->CreateMaterial("glass", kv);
	}
	{
		const auto kv = KeyValues::FromString("VertexLitGeneric", "$baseTexture black $bumpmap effects/flat_normal $translucent 1 $envmap models/effects/crystal_cube_vertigo_hdr $envmapfresnel 0 $phong 1 $phongexponent 16 $phongboost 2");
		kv->SetString("$phongtint", "[.2 .35 .6]");
		crystal = I::materialsystem->CreateMaterial("crystal", kv);
	}
	chrome = I::materialsystem->CreateMaterial("chrome", KeyValues::FromString("VertexLitGeneric", "$envmap env_cubemap"));
}

void Chams::Run(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo& info, matrix3x4_t* customBoneToWorld)
{
	bool is_arm = strstr(info.model->szName + 17, "arms") != nullptr;
	bool is_player = strstr(info.model->szName, "models/player") != nullptr;
	bool is_sleeve = strstr(info.model->szName + 17, "sleeve") != nullptr;
	bool is_weapon = strstr(info.model->szName, "models/weapons") != nullptr;

	static Color color_blocked = Color(200, 100, 100);
	static Color color_visible(100, 200, 100);
	Entity* ent = I::entitylist->GetClientEntity(info.entityIndex);
	Entity* local = I::entitylist->GetClientEntity(I::engine->GetLocalPlayer());
	static PlayerInfo_t bInfo;
	bool ValidInfo = I::engine->GetPlayerInfo(info.entityIndex, &bInfo);

	if (ent && local && ent->GetHealth() > 0 && ent->IsPlayer() && ValidInfo)
	{
		bool isEnemy = ent->GetTeam() != local->GetTeam();
		if (info.entityIndex == I::engine->GetLocalPlayer())
		{
			// no need to try and render localplayer differently
			//RotateBoneMatrix(Vec(0, (animfix->fake.y - animfix->real.y), 0), G::LocalPlayer->GetVecOrigin(), customBoneToWorld, animfix->FakeMatrix);

			/*
			Vec origin = G::LocalPlayer->GetVecOrigin();
			std::memcpy(&bad_animfix->VisualFake, &bad_animfix->FakeMatrix, 256 * sizeof(Matrix3x4));
			for (int i = 0; i < 256; i++)
			{
				bad_animfix->VisualFake[i].c[0][3] = bad_animfix->FakeMatrix[i].c[0][3] + origin.x;
				bad_animfix->VisualFake[i].c[1][3] = bad_animfix->FakeMatrix[i].c[1][3] + origin.y;
				bad_animfix->VisualFake[i].c[2][3] = bad_animfix->FakeMatrix[i].c[2][3] + origin.z;
			}
			OverideMat(
				false,	//viz thru wall?
				1,		// material
				1.f,	//transparent?
				Color(255.f, 255.f, 255.f),
				thisptr, ctx, state, info, bad_animfix->VisualFake);
			H::oDrawModelExecute(thisptr, ctx, state, info, bad_animfix->VisualFake);
			I::modelrender->ForcedMaterialOverride(nullptr);*/
		}
		else if (isEnemy)
		{
			if (cfg->chams.Hidden)
			{
				OverideMat(
					true,	//viz thru wall?
					cfg->chams.HiddenType,	//wireframe?
					cfg->chams.HiddenColor.a() / 255.f,	//transparent?
					cfg->chams.HiddenColor,
					thisptr, ctx, state, info, customBoneToWorld);
				H::oDrawModelExecute(thisptr, ctx, state, info, customBoneToWorld);
				I::modelrender->ForcedMaterialOverride(nullptr);
			}

			if (cfg->chams.Vis)
			{
				OverideMat(
					false,
					cfg->chams.VisibleType,
					cfg->chams.VisColor.a() / 255.f,	//transparent?
					cfg->chams.VisColor,
					thisptr, ctx, state, info, customBoneToWorld);
			}

		}
		else
		{
			// no need to change teamate chams lol
		}
	}
	else if (is_sleeve)
	{

		if (cfg->chams.Sleeves)
		{
			OverideMat(
				false,
				cfg->chams.SleeveType,
				cfg->chams.SleevesColor.a() / 255.f,	//transparent?
				cfg->chams.SleevesColor,
				thisptr, ctx, state, info, customBoneToWorld);
		}

	}
	else if (is_arm)
	{

		if (cfg->chams.Arms)
		{
			OverideMat(
				false,
				cfg->chams.ArmType,
				cfg->chams.ArmColor.a() / 255.f,	//transparent?
				cfg->chams.ArmColor,
				thisptr, ctx, state, info, customBoneToWorld);
		}

	}
	else if (is_weapon)
	{
		if (cfg->chams.Weapon && false)
		{
			//ValidInfo = true;
			OverideMat(
				false,
				cfg->chams.WeaponType,
				cfg->chams.WeaponColor.a() / 255.f,	//transparent?
				cfg->chams.WeaponColor,
				thisptr, ctx, state, info, customBoneToWorld);
		}
	}

	H::oDrawModelExecute(thisptr, ctx, state, info, customBoneToWorld);
	if (ValidInfo)
	{
		I::modelrender->ForcedMaterialOverride(nullptr);
	}
}
