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
			window->DrawList->AddRectFilled(ImVec2(total_bb.Min.x + 0, total_bb.Min.y + 2), ImVec2(total_bb.Min.x + 30, total_bb.Min.y + 18), ImGui::GetColorU32(cfg->menu.AccentColor), 10, 15);

			// knob
			window->DrawList->AddRectFilled(ImVec2(total_bb.Min.x + 15, total_bb.Min.y + 4), ImVec2(total_bb.Min.x + 28, total_bb.Min.y + 16), ImGui::GetColorU32(cfg->menu.SelectTextColor), 15, 15);
		}
		else
		{
			// backgroound
			window->DrawList->AddRectFilled(ImVec2(total_bb.Min.x + 0, total_bb.Min.y + 2), ImVec2(total_bb.Min.x + 30, total_bb.Min.y + 18), ImColor(43, 43, 43, 255), 10, 15);

			// knob
			window->DrawList->AddRectFilled(ImVec2(total_bb.Min.x + 2, total_bb.Min.y + 4), ImVec2(total_bb.Min.x + 15, total_bb.Min.y + 16), ImGui::GetColorU32(cfg->menu.SelectTextColor), 15, 15);
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

	static const char* PatchFormatStringFloatToInt(const char* fmt)
	{
		if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0) // Fast legacy path for "%.0f" which is expected to be the most common case.
			return "%d";
		const char* fmt_start = ImParseFormatFindStart(fmt);    // Find % (if any, and ignore %%)
		const char* fmt_end = ImParseFormatFindEnd(fmt_start);  // Find end of format specifier, which itself is an exercise of confidence/recklessness (because snprintf is dependent on libc or user).
		if (fmt_end > fmt_start && fmt_end[-1] == 'f')
		{
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
			if (fmt_start == fmt && fmt_end[0] == 0)
				return "%d";
			ImGuiContext& g = *GImGui;
			ImFormatString(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end); // Honor leading and trailing decorations, but lose alignment/precision.
			return g.TempBuffer;
#else
			IM_ASSERT(0 && "DragInt(): Invalid format string!"); // Old versions used a default parameter of "%.0f", please replace with e.g. "%d"
#endif
		}
		return fmt;
	}

	bool SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
	{
		using namespace ImGui;
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = CalcItemWidth();

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
		ItemSize(total_bb, style.FramePadding.y);
		if (!ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemAddFlags_Focusable : 0))
			return false;

		// Default format string when passing NULL
		if (format == NULL)
			format = DataTypeGetInfo(data_type)->PrintFmt;
		else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
			format = PatchFormatStringFloatToInt(format);

		// Tabbing or CTRL-clicking on Slider turns it into an input box
		const bool hovered = ItemHoverable(frame_bb, id);
		bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
		if (!temp_input_is_active)
		{
			const bool focus_requested = temp_input_allowed && (window->DC.LastItemStatusFlags & ImGuiItemStatusFlags_Focused) != 0;
			const bool clicked = (hovered && g.IO.MouseClicked[0]);
			if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
			{
				SetActiveID(id, window);
				SetFocusID(id, window);
				FocusWindow(window);
				g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
				if (temp_input_allowed && (focus_requested || (clicked && g.IO.KeyCtrl) || g.NavInputId == id))
					temp_input_is_active = true;
			}
		}

		if (temp_input_is_active)
		{
			// Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
			const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
			return TempInputScalar(frame_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
		}

		// Draw frame
		const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
		RenderNavHighlight(frame_bb, id);

		// draw whiteline all the way accross
		window->DrawList->AddLine(ImVec2(frame_bb.Min.x, (frame_bb.Max.y + frame_bb.Min.y) / 2), ImVec2(frame_bb.Max.x, (frame_bb.Max.y + frame_bb.Min.y) / 2), ImGui::GetColorU32(cfg->menu.SelectTextColor));
		
		//RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

		// Slider behavior
		ImRect grab_bb;
		const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
		if (value_changed)
			MarkItemEdited(id);

		// draw line to grab area
		window->DrawList->AddLine(ImVec2(frame_bb.Min.x, (frame_bb.Max.y + frame_bb.Min.y) / 2), ImVec2(grab_bb.Max.x, (frame_bb.Max.y + frame_bb.Min.y) / 2), ImGui::GetColorU32(cfg->menu.AccentColor));

		// Render grab
		ImVec2 center = (grab_bb.Max + grab_bb.Min) / 2;
		float height = fabsf(grab_bb.Min.y - grab_bb.Max.y) - 5;
		if (grab_bb.Max.x > grab_bb.Min.x)
			window->DrawList->AddRectFilled(ImVec2(center.x - height / 2, center.y - (height/2)), ImVec2(center.x + height / 2, center.y + (height / 2)), ImGui::GetColorU32(cfg->menu.SelectTextColor), /*style.GrabRounding*/15, 15);

		// Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
		char value_buf[64];
		const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
		if (g.LogEnabled)
			LogSetNextTextDecoration("{", "}");
		RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y - 6), ImVec2(frame_bb.Max.x, frame_bb.Max.y - 6), value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

		if (label_size.x > 0.0f)
			RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
		return value_changed;
	}

	bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0)
	{
		return C::SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
	}

	bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0)
	{
		return C::SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
	}
}

void Menu::DoStyle()
{
	ImGui::StyleColorsDark();

	// color
	AccentColor = ImVec4(cfg->menu.AccentColor.r() / 255.f, cfg->menu.AccentColor.g() / 255.f, cfg->menu.AccentColor.b() / 255.f, cfg->menu.AccentColor.a() / 255.f);
	MainTextColor = ImVec4(cfg->menu.MainTextColor.r() / 255.f, cfg->menu.MainTextColor.g() / 255.f, cfg->menu.MainTextColor.b() / 255.f, cfg->menu.MainTextColor.a() / 255.f);
	SelectTextColor = ImVec4(cfg->menu.SelectTextColor.r() / 255.f, cfg->menu.SelectTextColor.g() / 255.f, cfg->menu.SelectTextColor.b() / 255.f, cfg->menu.SelectTextColor.a() / 255.f);
	BackgroundColor = ImVec4(cfg->menu.BackgroundColor.r() / 255.f, cfg->menu.BackgroundColor.g() / 255.f, cfg->menu.BackgroundColor.b() / 255.f, cfg->menu.BackgroundColor.a() / 255.f);
	WindowColor = ImVec4(cfg->menu.WindowColor.r() / 255.f, cfg->menu.WindowColor.g() / 255.f, cfg->menu.WindowColor.b() / 255.f, cfg->menu.WindowColor.a() / 255.f);

	// inverted color
	ImVec4 InvertAccentColor = ImVec4(1, 1, 1, 1) - AccentColor; InvertAccentColor.w = AccentColor.w;
	ImVec4 InvertMainTextColor = ImVec4(1, 1, 1, 1) - MainTextColor; InvertMainTextColor.w = MainTextColor.w;
	ImVec4 InvertSelectTextColor = ImVec4(1, 1, 1, 1) - SelectTextColor; InvertSelectTextColor.w = SelectTextColor.w;
	ImVec4 InvertBackgroundColor = ImVec4(1, 1, 1, 1) - BackgroundColor; InvertBackgroundColor.w = BackgroundColor.w;

	auto FadeColor = [](ImVec4 clr, float amount)
	{
		clr.x *= amount;
		clr.y *= amount;
		clr.z *= amount;
		return clr;
	};

	// faded colors
	ImVec4 AccentColor_Faded = FadeColor(AccentColor, 0.9f);
	ImVec4 MainTextColor_Faded = FadeColor(MainTextColor, 0.9f);
	ImVec4 SelectTextColor_Faded = FadeColor(SelectTextColor, 0.9f);
	ImVec4 BackgroundColor_Faded = FadeColor(BackgroundColor, 0.9f);

	// light colors
	ImVec4 AccentColor_Light = FadeColor(AccentColor, 0.8f);
	ImVec4 MainTextColor_Light = FadeColor(MainTextColor, 0.8f);
	ImVec4 SelectTextColor_Light = FadeColor(SelectTextColor, 0.8f);
	ImVec4 BackgroundColor_Light = FadeColor(BackgroundColor, 0.8f);

	// Get imgui style
	ImGuiStyle* style = &ImGui::GetStyle();

	// Main window background 
	style->Colors[ImGuiCol_WindowBg] = WindowColor;

	// Child window backgrounds
	style->Colors[ImGuiCol_ChildBg] = BackgroundColor_Light; // Background of child windows
	style->Colors[ImGuiCol_PopupBg] = BackgroundColor_Light; // Background of popups, menus, tooltips windows

	// Text
	style->Colors[ImGuiCol_Text] = MainTextColor;
	style->Colors[ImGuiCol_TextSelectedBg] = InvertMainTextColor;
	style->Colors[ImGuiCol_TextDisabled] = MainTextColor_Faded;

	// selectable crap // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
	style->Colors[ImGuiCol_Header] = AccentColor;
	style->Colors[ImGuiCol_HeaderHovered] = AccentColor_Faded;
	style->Colors[ImGuiCol_HeaderActive] = AccentColor_Light;

	// button crap
	style->Colors[ImGuiCol_Button] = AccentColor;
	style->Colors[ImGuiCol_ButtonHovered] = AccentColor_Faded;
	style->Colors[ImGuiCol_ButtonActive] = AccentColor_Light;

	// checkmark crap
	style->Colors[ImGuiCol_CheckMark] = InvertAccentColor;

	// slider stuff
	style->Colors[ImGuiCol_SliderGrab] = SelectTextColor;
	style->Colors[ImGuiCol_SliderGrabActive] = SelectTextColor_Light;

	// Background of checkbox, radio button, plot, slider, text input
	style->Colors[ImGuiCol_FrameBg] = AccentColor;
	style->Colors[ImGuiCol_FrameBgHovered] = AccentColor_Faded;
	style->Colors[ImGuiCol_FrameBgActive] = AccentColor;

	// IDK
	style->Colors[ImGuiCol_TitleBg] = AccentColor;
	style->Colors[ImGuiCol_TitleBgActive] = AccentColor;

	// seperator stuff
	style->Colors[ImGuiCol_Separator] = MainTextColor;
	style->Colors[ImGuiCol_SeparatorHovered] = MainTextColor_Light;
	style->Colors[ImGuiCol_SeparatorActive] = SelectTextColor_Light;

	// resize stuff
	style->Colors[ImGuiCol_ResizeGrip] = AccentColor;
	style->Colors[ImGuiCol_ResizeGripHovered] = MainTextColor_Light;
	style->Colors[ImGuiCol_ResizeGripActive] = AccentColor_Light;

	// SQUARE
	style->WindowRounding = 0.f;
	style->ChildRounding = 0.f;
	style->WindowBorderSize = 0.f;
}

void Menu::RenderMenuOptions()
{
	static float propersize = 100.f;

	int width = max((ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) / 2, 1);

	ImGui::BeginChild("##MenuArea", ImVec2(0, propersize), true);

	float pos1 = ImGui::GetCursorPosY();

	ImGui::Text("Menu Colors");
	ImGui::Separator();

	int total_w = ImGui::GetContentRegionAvail().x - 20;

	ImGui::Text("Accent Color");
	ImGui::SameLine(total_w);
	ImGui::SetNextItemWidth(total_w);
	C::ColorPicker("AccentColor", &cfg->menu.AccentColor);

	ImGui::Text("Main Text Color");
	ImGui::SameLine(total_w);
	ImGui::SetNextItemWidth(total_w);
	C::ColorPicker("MainTextColor", &cfg->menu.MainTextColor);

	ImGui::Text("Selectable Text Color");
	ImGui::SameLine(total_w);
	ImGui::SetNextItemWidth(total_w);
	C::ColorPicker("SelectableTextColor", &cfg->menu.SelectTextColor);

	ImGui::Text("Background Color");
	ImGui::SameLine(total_w);
	ImGui::SetNextItemWidth(total_w);
	C::ColorPicker("BackgroundColor", &cfg->menu.BackgroundColor);

	ImGui::Text("Window Color");
	ImGui::SameLine(total_w);
	ImGui::SetNextItemWidth(total_w);
	C::ColorPicker("WindowColor", &cfg->menu.WindowColor);


	float pos2 = ImGui::GetCursorPosY();
	propersize = pos2 - pos1 + 20.f;

	ImGui::EndChild(); // end child
}

void Menu::RenderMovement()
{
	static float propersize = 100.f;

	ImGui::BeginChild("##MovementArea", ImVec2(0, propersize), true);

	float pos1 = ImGui::GetCursorPosY();

	ImGui::Text("Movement");
	ImGui::Separator();

	ImGui::Columns(2, "##MovementCols"); // 4-ways, with border

	ImGui::Text("Bunnyhop");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::Checkbox("##Bunnyhop", &cfg->movement.Bunnyhop);
	ImGui::NextColumn();

	if (cfg->movement.Bunnyhop) {
		ImGui::Text("Bunnyhop Chance");
		ImGui::NextColumn();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		C::SliderInt("##BunnyhopChance", &cfg->movement.Chance, 0, 100);
		ImGui::NextColumn();
		// if the max hops is slid down... compensate min hops
		if (cfg->movement.MaxHops < cfg->movement.MinHops) {
			cfg->movement.MinHops = std::clamp(cfg->movement.MaxHops - 1, 0, 15);
		}

		// if the min hops is slid up.. compensate max hops
		if (cfg->movement.MinHops > cfg->movement.MaxHops) {
			cfg->movement.MaxHops = std::clamp(cfg->movement.MinHops + 1, 0, 15);
		}

		ImGui::Text("Bunnyhop Min Hops");
		ImGui::NextColumn();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		C::SliderInt("###BunnyhopMinHops", &cfg->movement.MinHops, 0, 15);
		ImGui::NextColumn();

		ImGui::Text("Bunnyhop Max Hops");
		ImGui::NextColumn();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		C::SliderInt("###BunnyhopMaxHops", &cfg->movement.MaxHops, 0, 15);
		ImGui::NextColumn();
	}

	ImGui::Text("Slow Walk");
	ImGui::NextColumn();
	C::HotKey(cfg->Keybinds["Slowwalk"]);
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	ImGui::NextColumn();

	ImGui::Text("Slow Walk Speed");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::SliderFloat("##slowwalkspeed", &cfg->movement.SlowWalkSpeed, 0, 100);
	ImGui::NextColumn();

	ImGui::Text("Autostrafe");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::Checkbox("##Autostrafe", &cfg->movement.Autostrafe);
	ImGui::NextColumn();
	if (cfg->movement.Autostrafe) {
		ImGui::Text("Angle Sacrifice");
		ImGui::NextColumn();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		C::SliderFloat("##AutostrafeAngSacrifice", &cfg->movement.AngSacrifice, 0, 10);
		ImGui::NextColumn();

		ImGui::Text("Strafing angle");
		ImGui::NextColumn();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		C::SliderFloat("##AutostrafeStrafeAng", &cfg->movement.StrafeAng, 0, 30);
		ImGui::NextColumn();
	}

	ImGui::Text("Fake Duck");
	ImGui::NextColumn();
	C::HotKey(cfg->Keybinds["Fake Duck"]);
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	ImGui::NextColumn();

	ImGui::EndColumns(); // end columns

	float pos2 = ImGui::GetCursorPosY();
	propersize = pos2 - pos1 + 20.f;

	ImGui::EndChild(); // end child
}

void Menu::RenderBacktrack()
{
	static float propersize = 500.f;

	ImGui::BeginChild("##BacktrackArea", ImVec2(0, propersize), true);

	float pos1 = ImGui::GetCursorPosY();

	ImGui::Text("Backtrack");
	ImGui::Separator();

	ImGui::Columns(2, "##BacktrackCols"); 

	ImGui::Text("Backtrack");
	ImGui::NextColumn();
	C::Checkbox("###Backtrack", &cfg->backtrack.Enable);
	ImGui::NextColumn();

	if (cfg->backtrack.Enable) {
		ImGui::Text("Backtrack Time");
		ImGui::NextColumn();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		C::SliderFloat("##BacktrackTIme", &cfg->backtrack.BacktrackTime, 10, 200);
		ImGui::NextColumn();
	}

	ImGui::EndColumns(); // end columns

	float pos2 = ImGui::GetCursorPosY();
	propersize = pos2 - pos1 + 20.f;

	ImGui::EndChild(); // end child
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

void Menu::RenderVisuals()
{
	static float propersize = 100.f;

	ImGui::BeginChild("##VISArea", ImVec2(0, propersize), true);

	float pos1 = ImGui::GetCursorPosY();

	ImGui::Text("Visuals");
	ImGui::Separator();

	ImGui::Columns(2, "##VISCols"); 

	ImGui::Text("Thirdperson");
	ImGui::NextColumn();
	C::HotKey(cfg->Keybinds["Thirdperson"]);
	ImGui::SameLine();
	C::SliderFloat("##VISThirdpersonDistance", &cfg->vis.ThirdPersonDistance, 50, 300);
	ImGui::NextColumn();

	ImGui::Text("No Scope");
	ImGui::NextColumn();
	C::Checkbox("##NoScope", &cfg->vis.NoScope);
	ImGui::NextColumn();


	ImGui::Text("SpectatorList");
	ImGui::NextColumn();
	C::Checkbox("##SpecListEnable", &cfg->vis.SpectatorList);
	if (cfg->vis.SpectatorList)
	{
		ImGui::SameLine();
		C::SliderFloat("##SpecListOpaq", &cfg->vis.SpecOpacity, 0.f, 1.f);
	}

	ImGui::EndColumns(); // end columns

	float pos2 = ImGui::GetCursorPosY();
	propersize = pos2 - pos1 + 20.f;

	ImGui::EndChild(); // end child
}

void Menu::RenderAimbot()
{
	static float propersize = 500.f;

	ImGui::BeginChild("##AimbotArea", ImVec2(0, propersize), true);

	float pos1 = ImGui::GetCursorPosY();

	ImGui::Text("Aimbot");
	ImGui::Separator();

	ImGui::Columns(2, "##AimCols"); // 4-ways, with border

	ImGui::Text("Enable");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::HotKey(cfg->Keybinds["Aimbot"]);
	ImGui::NextColumn();

	ImGui::Text("Hitchance");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::SliderFloat("##AimHitchance", &cfg->aimbot.Hitchance, 0, 100);
	ImGui::NextColumn();

	ImGui::Text("MinDamage");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::SliderInt("##AimMinDamage", &cfg->aimbot.MinDamage, 1, 120);
	ImGui::NextColumn();

	static std::vector<std::string> priority_hitboxes =
	{
		"NONE",
		"HEAD",
		"NECK",
		"PELVIS",
		"STOMACH",
		"LOWER_CHEST",
		"CHEST",
		"UPPER_CHEST",
		"THIGH",
		"CALF",
		"FOOT",
		"HAND",
		"UPPER_ARM",
		"FOREARM"
	};

	ImGui::Text("Pirority Hitbox");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	ImGui::Combo("##AimPriority", &cfg->aimbot.Priority, priority_hitboxes);
	ImGui::NextColumn();
	
	ImGui::Text("Hitboxes");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	static std::string preview = "";
	static std::vector<std::string> hitboxes =
	{
		"HEAD",
		"NECK",
		"PELVIS",
		"STOMACH",
		"THORAX",
		"CHEST",
		"UPPER_CHEST",
		"THIGH",
		"CALF",
		"FOOT",
		"HAND",
		"UPPER_ARM",
		"FOREARM"
	};
	if (ImGui::BeginCombo("##AimHitboxes", preview.c_str())) {
		preview = "";
		for (size_t i = 0; i < hitboxes.size(); i++) {
			ImGui::Selectable(hitboxes[i].c_str(), &cfg->aimbot.Hitboxes[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			if (cfg->aimbot.Hitboxes[i])
				preview += hitboxes[i] + ",";
		}
		ImGui::EndCombo();
	}
	ImGui::NextColumn();

	ImGui::Text("Head Point Scale");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::SliderFloat("##AimHeadPointScale", &cfg->aimbot.HeadPointScale, 0, 100);
	ImGui::NextColumn();

	ImGui::Text("Body Point Scale");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::SliderFloat("##AimBodyPointScale", &cfg->aimbot.BodyPointScale, 0, 100);
	ImGui::NextColumn();

	ImGui::Separator();

	ImGui::Text("Enable Double Tap");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::Checkbox("##Aim_DT", &cfg->aimbot.EnableDt);
	ImGui::NextColumn();

	ImGui::Text("Enable Hideshots");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::Checkbox("##AIM_HS", &cfg->aimbot.EnableHs);
	ImGui::NextColumn();

	ImGui::EndColumns(); // end columns

	float pos2 = ImGui::GetCursorPosY();
	propersize = pos2 - pos1 + 20.f;

	ImGui::EndChild(); // end child
}

void Menu::RenderAA()
{
	static float propersize = 100.f;

	ImGui::BeginChild("##AntiaimArea", ImVec2(0, propersize), true);

	float pos1 = ImGui::GetCursorPosY();

	ImGui::Text("Antiaim");
	ImGui::Separator();

	ImGui::Columns(2, "##AntiaimCols");

	ImGui::Text("Enable");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::Checkbox("##AntiaimEnable", &cfg->aa.Enable);
	ImGui::NextColumn();

	ImGui::Text("Invert");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::HotKey(cfg->Keybinds["Invert AA"]);
	ImGui::NextColumn();

	static std::vector<std::string> yaw_bases =
	{
		"180",
		"AtTarget",
		"Freestanding"
	};

	ImGui::Text("Yaw Base");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	ImGui::Combo("##AntiaimYawbase", &cfg->aa.YawBase, yaw_bases);
	ImGui::NextColumn();

	ImGui::Text("Left Yaw Base Offset");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::SliderFloat("##AntiaimLeftYawbaseOffset", &cfg->aa.LeftBaseOffset, 0, 179);
	ImGui::NextColumn();

	ImGui::Text("Right Yaw Base Offset");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::SliderFloat("##AntiaimRightYawbaseOffset", &cfg->aa.RightBaseOffset, 0, 179);
	ImGui::NextColumn();

	static std::vector<std::string> AATypes =
	{
		"Statitc",
		"Sway",
		"Opposite",
		"Jitter",
		"Random",
		"Spin"
	};

	ImGui::Text("AA Type");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	ImGui::Combo("##AntiaimAAType", &cfg->aa.AAType, AATypes);
	ImGui::NextColumn();

	ImGui::Text("Left Limit");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::SliderFloat("##AntiaimLeftLimit", &cfg->aa.LeftLimit, 0, 58);
	ImGui::NextColumn();

	ImGui::Text("Right Limit");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::SliderFloat("##AntiaimRightLimit", &cfg->aa.RightLimit, 0, 58);
	ImGui::NextColumn();

	// sway
	if (cfg->aa.AAType == 1)
	{
		ImGui::Text("Sway Step");
		ImGui::NextColumn();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		C::SliderFloat("##AntiaimSwayStep", &cfg->aa.SwayStep, 0, 25);
		ImGui::NextColumn();
	}

	// jitter
	if (cfg->aa.AAType == 3)
	{
		ImGui::Text("Jitter Time");
		ImGui::NextColumn();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		C::SliderFloat("##AntiaimJitterTime", &cfg->aa.JitterTime, 0, 200);
		ImGui::NextColumn();
	}

	// jitter
	if (cfg->aa.AAType == 4)
	{
		ImGui::Text("Random Time");
		ImGui::NextColumn();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		C::SliderFloat("##AntiaimRandomTime", &cfg->aa.RandomUpdateTime, 0, 200);
		ImGui::NextColumn();
	}

	// spin
	if (cfg->aa.AAType == 5)
	{
		ImGui::Text("Spin Step");
		ImGui::NextColumn();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		C::SliderFloat("##AntiaimSpinStep", &cfg->aa.SpinStep, -180, 180);
		ImGui::NextColumn();
	}

	ImGui::Text("Anti Brute");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::Checkbox("##AntiaimAntiBrute", &cfg->aa.AntiBrute);
	ImGui::NextColumn();

	ImGui::Separator();

	ImGui::Text("Legit AA");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::Checkbox("##ANTIAIM_LEGITAA", &cfg->aa.LegitAA);
	ImGui::NextColumn();

	ImGui::Separator();

	ImGui::Text("Lean that thang");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::SliderFloat("##AntiAimLeanAngle", &cfg->aa.LeanAngle, 0, 180);
	ImGui::NextColumn();

	ImGui::Text("Real Offset");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::SliderFloat("##AntiaimLegitAmount", &cfg->aa.LegitAmount, -58, 58);
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

void Menu::RenderSpectators()
{
	if (!cfg->vis.SpectatorList) return;

	static std::vector<std::string> specs;
	static float oldTime = I::globalvars->flCurrentTime;
	if (fabsf(I::globalvars->flCurrentTime - oldTime) > 1)
	{
		oldTime = I::globalvars->flCurrentTime;
		specs.clear();
		specs.resize(0);
		for (auto a : GetSpectators())
			specs.push_back(a);
	}

	ImGui::SetNextWindowBgAlpha(cfg->vis.SpecOpacity);
	ImGui::Begin("Spectators", 0, ImGuiWindowFlags_NoTitleBar);
	for (auto a : specs)
		ImGui::Text(a.c_str());
	ImGui::End();
}

void Menu::RenderFakelag()
{
	static float propersize = 100.f;

	ImGui::BeginChild("##FakelagArea", ImVec2(0, propersize), true);

	float pos1 = ImGui::GetCursorPosY();

	ImGui::Text("Fakelag");
	ImGui::Separator();

	ImGui::Columns(2, "##FakelagCols"); // 4-ways, with border

	ImGui::Text("Regular Lag (ticks)");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::SliderInt("##FakelagRegularLagTime", &cfg->fakelag.LagTicks, 1, 14);
	ImGui::NextColumn();

	ImGui::Text("Random Lag (ticks)");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::SliderInt("##FakelagRandomLagTime", &cfg->fakelag.RandomTicks, 0, 13);
	ImGui::NextColumn();

	ImGui::Text("Lag On Peek");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	C::Checkbox("##FakelagLagOnPeak", &cfg->fakelag.LagOnPeak);
	ImGui::NextColumn();

	ImGui::EndColumns(); // end columns

	float pos2 = ImGui::GetCursorPosY();
	propersize = pos2 - pos1 + 20.f;

	ImGui::EndChild(); // end child
}

void Menu::Render()
{
	cfg->HandleKeybinds();

	RenderSpectators();

	DoStyle();

	if (G::MenuOpen)
	{
		ImGui::Begin("A$G$ INDUSTRIES");

		ImGui::Columns(2, "##MainMenuCols"); 

		// column 1
		RenderBacktrack();
		RenderAimbot();
		RenderAA();
		RenderFakelag();
		ImGui::NextColumn();

		// column 2
		RenderESP();
		RenderVisuals();
		RenderMovement();
		ImGui::NextColumn();

		ImGui::EndColumns(); // end columns

		ImGui::Separator();

		ImGui::Columns(2, "##MainMenuCols2");

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

		ImGui::NextColumn();

		RenderMenuOptions();
		ImGui::NextColumn();

		ImGui::EndColumns();

		ImGui::End();
	}
}
