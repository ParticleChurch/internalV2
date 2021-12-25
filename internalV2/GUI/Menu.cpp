#include "../Include.hpp"
#include "../ImGui/imgui_internal.h"

Menu* menu = new Menu();

namespace ImGui
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};
	bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter, static_cast<void*>(&values), values.size());
	}
}

// C for Custom
namespace C
{
	void ColorPicker(const char* name, Color* COLOR)
	{
		ImVec4 color = ImVec4(COLOR->color[0] / 255.f, COLOR->color[1] / 255.f, COLOR->color[2] / 255.f, COLOR->color[3] / 255.f);

		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_FrameRounding, 24.f);

		bool open_popup = ImGui::ColorButton(name, color);

		ImGui::PopStyleVar();

		if (open_popup)
		{
			ImGui::OpenPopup(((std::string)name + "picker").c_str());
		}

		if (ImGui::BeginPopup(((std::string)name + "picker").c_str()))
		{
			ImGui::SetNextItemWidth(100);
			float color_[4] = { COLOR->color[0] / 255.f, COLOR->color[1] / 255.f, COLOR->color[2] / 255.f, COLOR->color[3] / 255.f };
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
			ImGui::ColorPicker4(("##" + (std::string)name).c_str(), color_, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
			ImGui::PopStyleColor();
			*COLOR = Color(color_[0] * 255, color_[1] * 255, color_[2] * 255, color_[3] * 255);
			color = ImVec4(color_[0], color_[1], color_[2], color_[3]);

			ImGui::EndPopup();
		}
	}

	void ColorPicker(std::string name, Color* COLOR)
	{
		ImVec4 color = ImVec4(COLOR->color[0] / 255.f, COLOR->color[1] / 255.f, COLOR->color[2] / 255.f, COLOR->color[3] / 255.f);

		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_FrameRounding, 24.f);

		bool open_popup = ImGui::ColorButton(name.c_str(), color);

		ImGui::PopStyleVar();

		if (open_popup)
		{
			ImGui::OpenPopup((name + "picker").c_str());
		}

		if (ImGui::BeginPopup((name + "picker").c_str()))
		{
			ImGui::SetNextItemWidth(100);
			float color_[4] = { COLOR->color[0] / 255.f, COLOR->color[1] / 255.f, COLOR->color[2] / 255.f, COLOR->color[3] / 255.f };
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
			ImGui::ColorPicker4(("##" + name).c_str(), color_, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
			ImGui::PopStyleColor();
			*COLOR = Color(color_[0] * 255, color_[1] * 255, color_[2] * 255, color_[3] * 255);
			color = ImVec4(color_[0], color_[1], color_[2], color_[3]);

			ImGui::EndPopup();
		}
	}

	bool Checkbox(const char* label, bool* v)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const float square_sz = ImGui::GetFrameHeight();
		const ImVec2 pos = window->DC.CursorPos;
		const ImRect total_bb(pos, pos + ImVec2(square_sz + (style.ItemInnerSpacing.x + label_size.x + 0), 0 + label_size.y + style.FramePadding.y * 2));
		ImGui::ItemSize(total_bb, style.FramePadding.y);
		ImGui::ItemAdd(total_bb, id);

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);

		if (hovered || held)
			ImGui::SetMouseCursor(0);

		if (pressed)
			*v = !(*v);

		// THE COLORS
		//GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg)
		//ImU32 check_col = GetColorU32(ImGuiCol_CheckMark);
		if (*v)
		{
			// backgroound
			window->DrawList->AddRectFilled(ImVec2(total_bb.Min.x + 0, total_bb.Min.y + 2), ImVec2(total_bb.Min.x + 30, total_bb.Min.y + 18), ImGui::GetColorU32(ImVec4(0, 1, 0, 1)), 10, 15);

			// knob
			window->DrawList->AddRectFilled(ImVec2(total_bb.Min.x + 15, total_bb.Min.y + 4), ImVec2(total_bb.Min.x + 28, total_bb.Min.y + 16), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), 15, 15);
		}
		else
		{
			// backgroound
			window->DrawList->AddRectFilled(ImVec2(total_bb.Min.x + 0, total_bb.Min.y + 2), ImVec2(total_bb.Min.x + 30, total_bb.Min.y + 18), ImColor(43, 43, 43, 255), 10, 15);

			// knob
			window->DrawList->AddRectFilled(ImVec2(total_bb.Min.x + 2, total_bb.Min.y + 4), ImVec2(total_bb.Min.x + 15, total_bb.Min.y + 16), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), 15, 15);
		}

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, 255 / 255.f));
		ImGui::RenderText(ImVec2(total_bb.Min.x + style.ItemInnerSpacing.x + 30, total_bb.Min.y + style.FramePadding.y + 0), label);
		ImGui::PopStyleColor();

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);

		return pressed;
	}

	void HotKey(Keybind& keybind)
	{
		C::Checkbox(("###checkbox" + keybind.name).c_str(), &keybind.boolean);

		// is checkbox right clicked?
		bool open_popup = ImGui::IsItemClicked(ImGuiMouseButton_::ImGuiMouseButton_Right);
		if (open_popup)
		{
			ImGui::OpenPopup((keybind.name + "Keypicker").c_str());
		}

		/*ImGui::SameLine();

		std::string spec_str = keybind.mode == 0 ? "Hold: " : "Toggle: ";
		spec_str += keybind.key ? "[" + (std::string)I::inputsystem->VirtualKeyToString(keybind.key) + "]" : "[ key ]";
		ImGui::Text(spec_str.c_str());*/

		if (ImGui::BeginPopup((keybind.name + "Keypicker").c_str()))
		{
			ImGui::SetNextItemWidth(100);

			// deal with the keybind crap
			keybind.key ? ImGui::Text("[ %s ]", I::inputsystem->VirtualKeyToString(keybind.key)) : ImGui::TextUnformatted("[ key ]");
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Press any key to change keybind");
				ImGuiIO& io = ImGui::GetIO();
				for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
					if (ImGui::IsKeyPressed(i) && i != VK_INSERT)
						keybind.key = i != VK_ESCAPE ? i : 0;

				for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
					if (ImGui::IsMouseDown(i) && i + (i > 1 ? 2 : 1) != VK_INSERT)
						keybind.key = i + (i > 1 ? 2 : 1);
			}

			static const char* choices[3] = { "Hold to enable", "Toggle", "Hold to disable" };
			ImGui::Combo(("###HoldToggle" + keybind.name).c_str(), &keybind.mode, choices, 3);

			ImGui::EndPopup();
		}
	}
}

void Menu::RenderESP()
{
	static float propersize = 100.f;

	ImGui::BeginChild("##ESPArea", ImVec2(0, propersize), true);

	float pos1 = ImGui::GetCursorPosY();

	ImGui::Text("ESP");
	ImGui::Separator();

	ImGui::Columns(2, "##ESPCols"); // 4-ways, with border

	ImGui::Text("Enable");
	ImGui::NextColumn();
	C::Checkbox("##ESPEnable", &cfg->esp.Enable);
	ImGui::NextColumn();

	ImGui::Text("Names");
	ImGui::NextColumn();
	C::Checkbox("##ESPNames", &cfg->esp.Name);
	if (cfg->esp.Name)
	{
		ImGui::SameLine();
		C::ColorPicker("Name Color", &cfg->esp.NameColor);
	}
	ImGui::NextColumn();

	ImGui::Text("Skeleton");
	ImGui::NextColumn();
	C::Checkbox("##ESPSkeleton", &cfg->esp.Skeleton);
	if (cfg->esp.Skeleton)
	{
		ImGui::SameLine();
		C::ColorPicker("Skeleton Color", &cfg->esp.SkeletonColor);
	}
	ImGui::NextColumn();

	ImGui::Text("Health");
	ImGui::NextColumn();
	C::Checkbox("##ESPHealth", &cfg->esp.Health);
	ImGui::NextColumn();

	ImGui::Text("Weapon");
	ImGui::NextColumn();
	C::Checkbox("##ESPWeapon", &cfg->esp.Weapon);
	if (cfg->esp.Weapon)
	{
		ImGui::SameLine();
		C::ColorPicker("Weapon Color", &cfg->esp.WeaponColor);
	}
	ImGui::NextColumn();

	ImGui::EndColumns(); // end columns

	float pos2 = ImGui::GetCursorPosY();
	propersize = pos2 - pos1 + 20.f;

	ImGui::EndChild(); // end child
}

std::vector<std::string> GetSpectators()
{
	// If no localplayer or not in game, dont bother...
	if (!I::engine->IsInGame()) return std::vector<std::string>();
	if (!G::Localplayer) return std::vector<std::string>();

	Entity* Player = G::Localplayer;
	if (!G::Localplayer->IsAlive())
	{
		// if not observing crap, return nothing
		Entity* ObserverTarget = Player->GetObserverTarget();
		if (!ObserverTarget)
		{
			return std::vector<std::string>();
		}

		// set to person we are observing
		Player = ObserverTarget;
	}

	// Get people :D
	int spectators = 0;
	std::vector<std::string> SpecList = {};

	for (int i = 1; i <= I::globalvars->nMaxClients; i++) {
		Entity* ent = I::entitylist->GetClientEntity(i);

		if (!ent)
			continue;

		PlayerInfo_t info;

		if (!I::engine->GetPlayerInfo(i, &info))
			continue;

		Entity* SpecatedPlayer = ent->GetObserverTarget();

		if (!SpecatedPlayer)
			continue;

		// if the spectated player isn't the localplayer, don't bother
		if (SpecatedPlayer != G::Localplayer)
			continue;

		if (ent->IsAlive() || ent->IsDormant())
			continue;

		PlayerInfo_t spectated_player_info;

		if (!I::engine->GetPlayerInfo(SpecatedPlayer->GetIndex(), &spectated_player_info))
			continue;

		SpecList.push_back(info.szName);
		spectators++;
	}

	return SpecList;
}

void Menu::Render()
{
	cfg->HandleKeybinds();

	static float oldTime = I::globalvars->flCurrentTime;
	if (fabsf(I::globalvars->flCurrentTime - oldTime) > 1)
	{
		oldTime = I::globalvars->flCurrentTime;
		H::console.clear();
		H::console.resize(0);
		for (auto a : GetSpectators())
			H::console.push_back(a);
	}

	if (G::MenuOpen)
	{
		ImGui::Begin("A$G$ INDUSTRIES");

		ImGui::Text("Bunnyhop");
		ImGui::SameLine();
		C::Checkbox("##Bunnyhop", &cfg->movement.Bunnyhop);

		if (cfg->movement.Bunnyhop) {
			ImGui::Text("Bunnyhop Chance");
			ImGui::SameLine();
			ImGui::SliderInt("###BunnyhopChance", &cfg->movement.Chance, 0, 100);

			// if the max hops is slid down... compensate min hops
			if (cfg->movement.MaxHops < cfg->movement.MinHops) {
				cfg->movement.MinHops = std::clamp(cfg->movement.MaxHops - 1, 0, 15);
			}

			// if the min hops is slid up.. compensate max hops
			if (cfg->movement.MinHops > cfg->movement.MaxHops) {
				cfg->movement.MaxHops = std::clamp(cfg->movement.MinHops + 1, 0, 15);
			}

			ImGui::Text("Bunnyhop Min Hops");
			ImGui::SameLine();
			ImGui::SliderInt("###BunnyhopMinHops", &cfg->movement.MinHops, 0, 15);

			ImGui::Text("Bunnyhop Max Hops");
			ImGui::SameLine();
			ImGui::SliderInt("###BunnyhopMaxHops", &cfg->movement.MaxHops, 0, 15);
		}

		ImGui::Separator();

		ImGui::Text("Autostrafe");
		ImGui::SameLine();
		C::Checkbox("##Autostrafe", &cfg->movement.Autostrafe);
		if (cfg->movement.Autostrafe) {
			ImGui::Text("Angle Sacrifice");
			ImGui::SameLine();
			ImGui::SliderFloat("##AutostrafeAngSacrifice", &cfg->movement.AngSacrifice, 0, 10);

			ImGui::Text("Strafing angle");
			ImGui::SameLine();
			ImGui::SliderFloat("##AutostrafeStrafeAng", &cfg->movement.StrafeAng, 0, 30);
		}

		ImGui::Separator();

		ImGui::Text("Backtrack");
		ImGui::SameLine();
		C::Checkbox("###Backtrack", &cfg->offense.Backtrack);

		if (cfg->offense.Backtrack) {
			ImGui::Text("Backtrack Time");
			ImGui::SameLine();
			ImGui::SliderFloat("##BacktrackTIme", &cfg->offense.BacktrackTime, 10, 200);
		}

		ImGui::Separator();

		RenderESP();

		ImGui::Separator();

		ImGui::Text("Antiaim");
		ImGui::SameLine();
		ImGui::Checkbox("##AntiaimEnable", &cfg->aa.Enable);

		ImGui::Text("Thirdperson");
		ImGui::SameLine();
		C::HotKey(cfg->Keybinds["Thirdperson"]);
		ImGui::SameLine();
		ImGui::SliderFloat("##VISThirdpersonDistance", &cfg->vis.ThirdPersonDistance, 50, 300);

		ImGui::Separator();

		ImGui::Text("Aimbot");
		ImGui::NextColumn();
		C::HotKey(cfg->Keybinds["Aimbot"]);
		ImGui::NextColumn();

		ImGui::Separator();

		C::Checkbox("Console Window", &ConsoleWindow);

		bool open_popup = ImGui::Button("Eject");
		if (open_popup)
		{
			ImGui::OpenPopup("Eject popup");
		}

		if (ImGui::BeginPopup("Eject popup"))
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0, 0, 1.00f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 0.1f, 0.1f, 1.00f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.f, 0.2f, 0.2f, 1.00f));

			ImGui::Text("Are you sure you want to Eject?");
			if (ImGui::Button("Eject##Eject2"))
				G::KillDLL = true;
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("NOTE:\nEjecting can potentially crash your game.\nEjecting can is not complete so if you want to inject another cheat, restart the game.");


			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();

			ImGui::EndPopup();
		}

		if (ImGui::Button("Connect to Big Steppa HVH"))
			I::engine->ClientCmdUnrestricted("connect 74.91.124.24:27015");

		ImGui::End();
	}
}
