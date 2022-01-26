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

// menu frame work
namespace mfw
{
	int Tab = 1;

	void StartMain()
	{
		ImGui::Begin("MAIN", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

	}
	void EndMain()
	{
		/*ImGui::PopFont();*/
		ImGui::End();//hrrrr
	}
	void StartSection(std::string name, float& pos1, float& propersize)
	{
		ImGui::BeginChild(("##" + name).c_str(), ImVec2(0, propersize), true);

		pos1 = ImGui::GetCursorPosY();

		ImGui::PushStyleColor(ImGuiCol_Text, cfg->menu.AccentClr.operator ImVec4());
		ImGui::Text(name.c_str());
		ImGui::PopStyleColor();
		ImGui::Separator();
	}
	void EndSection(float& propersize, float& pos1)
	{
		float pos2 = ImGui::GetCursorPosY();
		propersize = pos2 - pos1 + 20.f;

		ImGui::EndChild(); // end child
	}
	void RefreshColors()
	{
		ImGui::StyleColorsLight();

		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_WindowBg] = cfg->menu.WindowBackgroundClr.operator ImVec4();
		style.Colors[ImGuiCol_ChildBg] = cfg->menu.ChildBackgroundClr.operator ImVec4();
		style.Colors[ImGuiCol_FrameBg] = cfg->menu.WindowBackgroundClr.operator ImVec4();
		style.Colors[ImGuiCol_FrameBgHovered] = cfg->menu.WindowBackgroundClr.operator ImVec4();
		style.Colors[ImGuiCol_FrameBgActive] = cfg->menu.ChildBackgroundClr.operator ImVec4();
		style.Colors[ImGuiCol_PopupBg] = cfg->menu.ChildBackgroundClr.operator ImVec4();
		style.Colors[ImGuiCol_ResizeGrip] = cfg->menu.ResizeClr.operator ImVec4();
		style.Colors[ImGuiCol_ResizeGripHovered] = cfg->menu.ResizeClr.operator ImVec4();
		style.Colors[ImGuiCol_ResizeGripActive] = cfg->menu.ResizeClr.operator ImVec4();
		style.Colors[ImGuiCol_Text] = cfg->menu.TextClr.operator ImVec4();

		style.Colors[ImGuiCol_Separator] = cfg->menu.AccentClr.operator ImVec4();
		style.Colors[ImGuiCol_SeparatorHovered] = cfg->menu.AccentClr.operator ImVec4();
		style.Colors[ImGuiCol_SeparatorActive] = cfg->menu.AccentClr.operator ImVec4();
	}

	// custom imgui stuff
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
			window->DrawList->AddRectFilled(ImVec2(total_bb.Min.x + 0, total_bb.Min.y + 2), ImVec2(total_bb.Min.x + 30, total_bb.Min.y + 18), ImGui::GetColorU32(cfg->menu.AccentClr), 10, 15);

			// knob
			window->DrawList->AddRectFilled(ImVec2(total_bb.Min.x + 15, total_bb.Min.y + 4), ImVec2(total_bb.Min.x + 28, total_bb.Min.y + 16), ImGui::GetColorU32(cfg->menu.TextClr), 15, 15);
		}
		else
		{
			// backgroound
			window->DrawList->AddRectFilled(ImVec2(total_bb.Min.x + 0, total_bb.Min.y + 2), ImVec2(total_bb.Min.x + 30, total_bb.Min.y + 18), ImGui::GetColorU32(cfg->menu.WindowBackgroundClr), 10, 15);

			// knob
			window->DrawList->AddRectFilled(ImVec2(total_bb.Min.x + 2, total_bb.Min.y + 4), ImVec2(total_bb.Min.x + 15, total_bb.Min.y + 16), ImGui::GetColorU32(cfg->menu.TextClr), 15, 15);
		}

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, 255 / 255.f));
		ImGui::RenderText(ImVec2(total_bb.Min.x + style.ItemInnerSpacing.x + 30, total_bb.Min.y + style.FramePadding.y + 0), label);
		ImGui::PopStyleColor();

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);

		return pressed;
	}
	void HotKey(Keybind& keybind)
	{
		mfw::Checkbox(("###checkbox" + keybind.name).c_str(), &keybind.boolean);

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
		window->DrawList->AddLine(ImVec2(frame_bb.Min.x, (frame_bb.Max.y + frame_bb.Min.y) / 2), ImVec2(frame_bb.Max.x, (frame_bb.Max.y + frame_bb.Min.y) / 2), ImGui::GetColorU32(cfg->menu.TextClr));

		//RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

		// Slider behavior
		ImRect grab_bb;
		const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
		if (value_changed)
			MarkItemEdited(id);

		// draw line to grab area
		window->DrawList->AddLine(ImVec2(frame_bb.Min.x, (frame_bb.Max.y + frame_bb.Min.y) / 2), ImVec2(grab_bb.Max.x, (frame_bb.Max.y + frame_bb.Min.y) / 2), ImGui::GetColorU32(cfg->menu.AccentClr));

		// Render grab
		ImVec2 center = (grab_bb.Max + grab_bb.Min) / 2;
		float height = fabsf(grab_bb.Min.y - grab_bb.Max.y) - 5;
		if (grab_bb.Max.x > grab_bb.Min.x)
			window->DrawList->AddRectFilled(ImVec2(center.x - height / 2, center.y - (height / 2)), ImVec2(center.x + height / 2, center.y + (height / 2)), ImGui::GetColorU32(cfg->menu.TextClr), /*style.GrabRounding*/15, 15);

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
		return mfw::SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
	}
	bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0)
	{
		return mfw::SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
	}
	bool ButtonEx(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
			pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
		ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		if (g.CurrentItemFlags & ImGuiItemFlags_ButtonRepeat)
			flags |= ImGuiButtonFlags_Repeat;
		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

		// Render
		const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		ImGui::RenderNavHighlight(bb, id);
		ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

		if (g.LogEnabled)
			ImGui::LogSetNextTextDecoration("[", "]");
		ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

		// Automatically close popups
		//if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
		//    CloseCurrentPopup();

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
		return pressed;
	}
	bool Button(const char* label, const ImVec2& size_arg = ImVec2(0, 0))
	{
		ImGui::PushStyleColor(ImGuiCol_Button, cfg->menu.AccentClr.operator ImVec4());
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, cfg->menu.AccentClr.operator ImVec4());
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, cfg->menu.AccentClr.operator ImVec4());
		bool ret = ButtonEx(label, size_arg, ImGuiButtonFlags_None);
		ImGui::PopStyleColor(3);
		return ret;
	}
}


void Menu::MenuBar()
{
	ImGui::PushStyleColor(ImGuiCol_Button, cfg->menu.ChildBackgroundClr.operator ImVec4());
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, cfg->menu.WindowBackgroundClr.operator ImVec4());
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, cfg->menu.ChildBackgroundClr.operator ImVec4());
	static const char* tabs[5] = { "Legit", "Rage", "Visuals", "Other", "Config" };
	int button_width = max((ImGui::GetContentRegionAvail().x - ((IM_ARRAYSIZE(tabs) - 1) * ImGui::GetStyle().ItemSpacing.x)) / IM_ARRAYSIZE(tabs), 1);
	for (int i = 1; i <= IM_ARRAYSIZE(tabs); i++)
	{
		if (ImGui::Button(tabs[i - 1], ImVec2(button_width, 0)))
			mfw::Tab = i;

		if (i < IM_ARRAYSIZE(tabs))
			ImGui::SameLine();
	}
	ImGui::PopStyleColor(3);
}

void Menu::RenderMenuOptions()
{
	mfw::RefreshColors();

	static float propersize = 100.f;
	static float pos1 = 0.f;
	mfw::StartSection("Menu Customization", pos1, propersize);

	int total_w = ImGui::GetContentRegionAvail().x - 20;

	ImGui::Text("Main Window Background Color");
	ImGui::SameLine(total_w);
	ImGui::SetNextItemWidth(total_w);
	mfw::ColorPicker("Main Window Background Color", &cfg->menu.WindowBackgroundClr);

	ImGui::Text("Child Window Background Color");
	ImGui::SameLine(total_w);
	ImGui::SetNextItemWidth(total_w);
	mfw::ColorPicker("Child Window Background Color", &cfg->menu.ChildBackgroundClr);

	ImGui::Text("Text Color");
	ImGui::SameLine(total_w);
	ImGui::SetNextItemWidth(total_w);
	mfw::ColorPicker("Text Color", &cfg->menu.TextClr);

	ImGui::Text("Accent Color");
	ImGui::SameLine(total_w);
	ImGui::SetNextItemWidth(total_w);
	mfw::ColorPicker("Accent Color", &cfg->menu.AccentClr);

	mfw::Checkbox("Console Window", &ConsoleWindow);

	bool open_popup = mfw::Button("Eject");
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

	if (mfw::Button("Connect to Big Steppa HVH"))
		I::engine->ClientCmdUnrestricted("connect 74.91.124.24:27015");

	mfw::EndSection(propersize, pos1);
}

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}
void Menu::UpdateFiles()
{
	menu->files.clear();

	auto path2 = std::filesystem::current_path();
	std::wstring ws_path(path2);
	std::string str_path(ws_path.begin(), ws_path.end());

	std::string path = ".";
	std::string search_path = str_path + "/*.cfg"; //*.ext in order to get only the EXT-files (for ex)
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile((search_path).c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				menu->files.insert(std::pair(fd.cFileName, false));
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
}

void Menu::RenderConfig()
{
	static bool init = false;
	if (!init)
	{
		UpdateFiles();
		init = true;
	}

	static float propersize = 100.f;

	ImGui::BeginChild("##ConfigArea", ImVec2(0, propersize), true);

	float pos1 = ImGui::GetCursorPosY();

	ImGui::Text("Config");
	ImGui::Separator();

	static char config_name[256];

	ImGui::Columns(2, "##ConfigCols", true); // 4-ways, with border

	for (auto& a : menu->files)
	{
		// if the selectable variable has changed
		if (ImGui::Selectable(a.first.c_str(), &a.second))
		{
			// if it is now positive
			if (a.second)
			{
				// copy over the name, and leave last character for '\0'
				for (int i = 0; i < min(255, a.first.length()); i++)
				{
					config_name[i] = a.first[i];
				}
				config_name[min(255, a.first.length())] = '\0';

				// set all other selectables to false
				for (auto& b : menu->files)
				{
					// if they aren't the one we just selected, set it to false
					if (b.first.compare(a.first) != 0)
					{
						b.second = false;
					}
				}
			}
		}
		ImGui::NextColumn();
	}

	ImGui::EndColumns(); // end columns

	ImGui::InputText("##configname", config_name, 256);

	if (mfw::Button("Load"))
	{
		cfg->OpenConfig(config_name);
	}
	ImGui::SameLine();
	if (mfw::Button("Save"))
		ImGui::OpenPopup("Save popup");
	if (ImGui::BeginPopup("Save popup"))
	{
		ImGui::Text(("Are you sure you want to save: " + (std::string)(config_name)).c_str());
		if (mfw::Button("Save##Save2"))
		{
			cfg->SaveConfig(config_name);
			UpdateFiles();
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("NOTE:\nThis will permentently overwrite the config.");

		ImGui::EndPopup();
	}
	ImGui::SameLine();
	if (mfw::Button("Delete"))
		ImGui::OpenPopup("Delete popup");
	if (ImGui::BeginPopup("Delete popup"))
	{
		ImGui::Text(("Are you sure you want to delete: " + (std::string)(config_name)).c_str());
		if (mfw::Button("Delete##Delete2"))
		{
			cfg->DeleteConfig(config_name);
			UpdateFiles();
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("NOTE:\nThis will permentently delete the config.");

		ImGui::EndPopup();
	}
	ImGui::SameLine();
	if (mfw::Button("Refresh"))
	{
		UpdateFiles();
	}

	bool open_popup = mfw::Button("Eject");
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
		if (mfw::Button("Eject##Eject2"))
			G::KillDLL = true;
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("NOTE:\nEjecting can potentially crash your game.\nEjecting can is not complete so if you want to inject another cheat, restart the game.");


		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		ImGui::EndPopup();
	}

	float pos2 = ImGui::GetCursorPosY();
	propersize = pos2 - pos1 + 20.f;

	ImGui::EndChild(); // end child
}

void Menu::RenderTriggerbot()
{
	static float propersize = 100.f;
	static float pos1 = 0.f;
	mfw::StartSection("Triggerbot", pos1, propersize);

	ImGui::Columns(2, "##TriggerbotCols", false);

	ImGui::Text("Triggerbot");
	ImGui::NextColumn();
	mfw::HotKey(cfg->Keybinds["Triggerbot"]);
	ImGui::NextColumn();

	ImGui::Text("Minimum Damage");
	ImGui::NextColumn();
	mfw::SliderFloat("##TriggerMinDamage", &cfg->triggerbot.MinDamage, 0, 120.f);
	ImGui::NextColumn();

	ImGui::Text("Delay Time");
	ImGui::NextColumn();
	mfw::SliderFloat("##TriggerDelaytime", &cfg->triggerbot.DelayTime, 0, 200.f);
	ImGui::NextColumn();

	ImGui::Text("Hitchance");
	ImGui::NextColumn();
	mfw::SliderFloat("##TriggerHitchance", &cfg->triggerbot.Hitchance, 0, 100.f);
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
	if (ImGui::BeginCombo("##TriggerHitboxes", preview.c_str())) {
		preview = "";
		for (size_t i = 0; i < hitboxes.size(); i++) {
			ImGui::Selectable(hitboxes[i].c_str(), &cfg->triggerbot.Hitboxes[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
			if (cfg->triggerbot.Hitboxes[i])
				preview += hitboxes[i] + ",";
		}
		ImGui::EndCombo();
	}
	ImGui::NextColumn();

	ImGui::EndColumns(); // end columns
	mfw::EndSection(propersize, pos1);

}

void Menu::RenderMovement()
{
	static float propersize = 100.f;

	ImGui::BeginChild("##MovementArea", ImVec2(0, propersize), true);

	float pos1 = ImGui::GetCursorPosY();

	ImGui::Text("Movement");
	ImGui::Separator();

	ImGui::Columns(2, "##MovementCols", false);

	ImGui::Text("Bunnyhop");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::Checkbox("##Bunnyhop", &cfg->movement.Bunnyhop);
	ImGui::NextColumn();

	if (cfg->movement.Bunnyhop) {
		ImGui::Text("Bunnyhop Chance");
		ImGui::NextColumn();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		mfw::SliderInt("##BunnyhopChance", &cfg->movement.Chance, 0, 100);
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
		mfw::SliderInt("###BunnyhopMinHops", &cfg->movement.MinHops, 0, 15);
		ImGui::NextColumn();

		ImGui::Text("Bunnyhop Max Hops");
		ImGui::NextColumn();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		mfw::SliderInt("###BunnyhopMaxHops", &cfg->movement.MaxHops, 0, 15);
		ImGui::NextColumn();
	}

	ImGui::Text("Slow Walk");
	ImGui::NextColumn();
	mfw::HotKey(cfg->Keybinds["Slowwalk"]);
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
	mfw::SliderFloat("##slowwalkspeed", &cfg->movement.SlowWalkSpeed, 0, 100);
	ImGui::NextColumn();

	ImGui::Text("Autostrafe");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::Checkbox("##Autostrafe", &cfg->movement.Autostrafe);
	ImGui::NextColumn();
	if (cfg->movement.Autostrafe) {
		ImGui::Text("Angle Sacrifice");
		ImGui::NextColumn();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		mfw::SliderFloat("##AutostrafeAngSacrifice", &cfg->movement.AngSacrifice, 0, 10);
		ImGui::NextColumn();

		ImGui::Text("Strafing angle");
		ImGui::NextColumn();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		mfw::SliderFloat("##AutostrafeStrafeAng", &cfg->movement.StrafeAng, 0, 30);
		ImGui::NextColumn();
	}

	ImGui::Text("Fake Duck");
	ImGui::NextColumn();
	mfw::HotKey(cfg->Keybinds["Fake Duck"]);
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

	ImGui::Columns(2, "##BacktrackCols", false); 

	ImGui::Text("Backtrack");
	ImGui::NextColumn();
	mfw::Checkbox("###Backtrack", &cfg->backtrack.Enable);
	ImGui::NextColumn();

	if (cfg->backtrack.Enable) {
		ImGui::Text("Backtrack Time");
		ImGui::NextColumn();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		mfw::SliderFloat("##BacktrackTIme", &cfg->backtrack.BacktrackTime, 10, 200);
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

	ImGui::Columns(2, "##ESPCols", false);

	ImGui::Text("Enable");
	ImGui::NextColumn();
	mfw::Checkbox("##ESPEnable", &cfg->esp.Enable);
	ImGui::NextColumn();

	ImGui::Text("Names");
	ImGui::NextColumn();
	mfw::Checkbox("##ESPNames", &cfg->esp.Name);
	if (cfg->esp.Name)
	{
		ImGui::SameLine();
		mfw::ColorPicker("Name Color", &cfg->esp.NameColor);
	}
	ImGui::NextColumn();

	ImGui::Text("Skeleton");
	ImGui::NextColumn();
	mfw::Checkbox("##ESPSkeleton", &cfg->esp.Skeleton);
	if (cfg->esp.Skeleton)
	{
		ImGui::SameLine();
		mfw::ColorPicker("Skeleton Color", &cfg->esp.SkeletonColor);
	}
	ImGui::NextColumn();

	ImGui::Text("Health");
	ImGui::NextColumn();
	mfw::Checkbox("##ESPHealth", &cfg->esp.Health);
	ImGui::NextColumn();

	ImGui::Text("Weapon");
	ImGui::NextColumn();
	mfw::Checkbox("##ESPWeapon", &cfg->esp.Weapon);
	if (cfg->esp.Weapon)
	{
		ImGui::SameLine();
		mfw::ColorPicker("Weapon Color", &cfg->esp.WeaponColor);
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

	ImGui::Columns(2, "##VISCols", false);

	ImGui::Text("Thirdperson");
	ImGui::NextColumn();
	mfw::HotKey(cfg->Keybinds["Thirdperson"]);
	ImGui::SameLine();
	mfw::SliderFloat("##VISThirdpersonDistance", &cfg->vis.ThirdPersonDistance, 50, 300);
	ImGui::NextColumn();

	ImGui::Text("No Scope");
	ImGui::NextColumn();
	mfw::Checkbox("##NoScope", &cfg->vis.NoScope);
	ImGui::NextColumn();


	ImGui::Text("SpectatorList");
	ImGui::NextColumn();
	mfw::Checkbox("##SpecListEnable", &cfg->vis.SpectatorList);
	if (cfg->vis.SpectatorList)
	{
		ImGui::SameLine();
		mfw::SliderFloat("##SpecListOpaq", &cfg->vis.SpecOpacity, 0.f, 1.f);
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

	ImGui::Columns(2, "##AimCols", false);

	ImGui::Text("Enable");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::HotKey(cfg->Keybinds["Aimbot"]);
	ImGui::NextColumn();

	ImGui::Text("Hitchance");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::SliderFloat("##AimHitchance", &cfg->aimbot.Hitchance, 0, 100);
	ImGui::NextColumn();

	ImGui::Text("MinDamage");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::SliderInt("##AimMinDamage", &cfg->aimbot.MinDamage, 1, 120);
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
	mfw::SliderFloat("##AimHeadPointScale", &cfg->aimbot.HeadPointScale, 0, 100);
	ImGui::NextColumn();

	ImGui::Text("Body Point Scale");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::SliderFloat("##AimBodyPointScale", &cfg->aimbot.BodyPointScale, 0, 100);
	ImGui::NextColumn();

	ImGui::Separator();

	ImGui::Text("Enable Double Tap");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::Checkbox("##Aim_DT", &cfg->aimbot.EnableDt);
	ImGui::NextColumn();

	ImGui::Text("Enable Hideshots");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::Checkbox("##AIM_HS", &cfg->aimbot.EnableHs);
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

	ImGui::Columns(2, "##AntiaimCols", false);

	ImGui::Text("Enable");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::Checkbox("##AntiaimEnable", &cfg->aa.Enable);
	ImGui::NextColumn();

	ImGui::Text("Invert");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::HotKey(cfg->Keybinds["Invert AA"]);
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
	mfw::SliderFloat("##AntiaimLeftYawbaseOffset", &cfg->aa.LeftBaseOffset, 0, 179);
	ImGui::NextColumn();

	ImGui::Text("Right Yaw Base Offset");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::SliderFloat("##AntiaimRightYawbaseOffset", &cfg->aa.RightBaseOffset, 0, 179);
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
	mfw::SliderFloat("##AntiaimLeftLimit", &cfg->aa.LeftLimit, 0, 58);
	ImGui::NextColumn();

	ImGui::Text("Right Limit");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::SliderFloat("##AntiaimRightLimit", &cfg->aa.RightLimit, 0, 58);
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
		mfw::SliderFloat("##AntiaimSwayStep", &cfg->aa.SwayStep, 0, 25);
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
		mfw::SliderFloat("##AntiaimJitterTime", &cfg->aa.JitterTime, 0, 200);
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
		mfw::SliderFloat("##AntiaimRandomTime", &cfg->aa.RandomUpdateTime, 0, 200);
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
		mfw::SliderFloat("##AntiaimSpinStep", &cfg->aa.SpinStep, -180, 180);
		ImGui::NextColumn();
	}

	ImGui::Text("Anti Brute");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::Checkbox("##AntiaimAntiBrute", &cfg->aa.AntiBrute);
	ImGui::NextColumn();

	ImGui::Separator();

	ImGui::Text("Legit AA");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::Checkbox("##ANTIAIM_LEGITAA", &cfg->aa.LegitAA);
	ImGui::NextColumn();

	ImGui::Separator();

	ImGui::Text("Lean that thang");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::SliderFloat("##AntiAimLeanAngle", &cfg->aa.LeanAngle, 0, 180);
	ImGui::NextColumn();

	ImGui::Text("Real Offset");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::SliderFloat("##AntiaimLegitAmount", &cfg->aa.LegitAmount, -58, 58);
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

	ImGui::Columns(2, "##FakelagCols", false);

	ImGui::Text("Regular Lag (ticks)");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::SliderInt("##FakelagRegularLagTime", &cfg->fakelag.LagTicks, 1, 14);
	ImGui::NextColumn();

	ImGui::Text("Random Lag (ticks)");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::SliderInt("##FakelagRandomLagTime", &cfg->fakelag.RandomTicks, 0, 13);
	ImGui::NextColumn();

	ImGui::Text("Lag On Peek");
	ImGui::NextColumn();
	{
		int total_w = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(total_w);
	}
	mfw::Checkbox("##FakelagLagOnPeak", &cfg->fakelag.LagOnPeak);
	ImGui::NextColumn();

	ImGui::EndColumns(); // end columns

	float pos2 = ImGui::GetCursorPosY();
	propersize = pos2 - pos1 + 20.f;

	ImGui::EndChild(); // end child
}

void Menu::RenderChams()
{
	static float propersize = 100.f;

	ImGui::BeginChild("##ChamArea", ImVec2(0, propersize), true);

	float pos1 = ImGui::GetCursorPosY();

	ImGui::Text("Chams");
	ImGui::Separator();

	ImGui::Columns(2, "##ChamCols", false); // 4-ways, with border

	static std::vector<std::string> ChamTypes = { "Flat", "Normal", "Glow", "Animated", "Glass", "Crystal", "chrome" };

	ImGui::Text("Visible Player");
	ImGui::NextColumn();
	mfw::Checkbox("##ChamVisPlayer", &cfg->chams.Vis);
	if (cfg->chams.Vis)
	{
		ImGui::SameLine();
		mfw::ColorPicker("Visible Cham Color", &cfg->chams.VisColor);
		ImGui::SameLine();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		ImGui::Combo("##ChamVisTypes", &cfg->chams.VisibleType, ChamTypes);
	}
	ImGui::NextColumn();

	ImGui::Text("Hidden Player");
	ImGui::NextColumn();
	mfw::Checkbox("##ChamHidPlayer", &cfg->chams.Hidden);
	if (cfg->chams.Hidden)
	{
		ImGui::SameLine();
		mfw::ColorPicker("Hidden Cham Color", &cfg->chams.HiddenColor);
		ImGui::SameLine();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		ImGui::Combo("##ChamHidTypes", &cfg->chams.HiddenType, ChamTypes);
	}
	ImGui::NextColumn();

	ImGui::Text("Local Weapon");
	ImGui::NextColumn();
	mfw::Checkbox("##ChamsWeapon", &cfg->chams.Weapon);
	if (cfg->chams.Weapon)
	{
		ImGui::SameLine();
		mfw::ColorPicker("Local Weapon Color", &cfg->chams.WeaponColor);
		ImGui::SameLine();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		ImGui::Combo("##ChamsWeaponTypes", &cfg->chams.WeaponType, ChamTypes);
	}
	ImGui::NextColumn();

	ImGui::Text("Local Arms");
	ImGui::NextColumn();
	mfw::Checkbox("##ChamsArms", &cfg->chams.Arms);
	if (cfg->chams.Arms)
	{
		ImGui::SameLine();
		mfw::ColorPicker("Local Arm Color", &cfg->chams.ArmColor);
		ImGui::SameLine();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		ImGui::Combo("##ChamsArmTypes", &cfg->chams.ArmType, ChamTypes);
	}
	ImGui::NextColumn();

	ImGui::Text("Local Sleeves");
	ImGui::NextColumn();
	mfw::Checkbox("##ChamsSleeves", &cfg->chams.Sleeves);
	if (cfg->chams.Sleeves)
	{
		ImGui::SameLine();
		mfw::ColorPicker("Local Sleeve Color", &cfg->chams.SleevesColor);
		ImGui::SameLine();
		{
			int total_w = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(total_w);
		}
		ImGui::Combo("##ChamsSleeveTypes", &cfg->chams.SleeveType, ChamTypes);
	}
	ImGui::NextColumn();

	ImGui::EndColumns(); // end columns

	float pos2 = ImGui::GetCursorPosY();
	propersize = pos2 - pos1 + 20.f;

	ImGui::EndChild(); // end child
}

void Menu::RenderWorldVisuals()
{
	static float propersize = 100.f;
	static float pos1 = 0.f;
	mfw::StartSection("World Modulation", pos1, propersize);

	int total_w = ImGui::GetContentRegionAvail().x - 20;

	ImGui::Columns(2, "##WorldModCols", false);

	ImGui::Text("World Brightness");
	ImGui::NextColumn();
	mfw::SliderFloat("##World Brightness", &cfg->world_vis.brightness, 0.f, 1.f);
	ImGui::NextColumn();

	ImGui::EndColumns(); // end columns

	mfw::EndSection(propersize, pos1);
}

void Menu::RenderGlow()
{
	static float propersize = 100.f;
	static float pos1 = 0.f;
	mfw::StartSection("Glow", pos1, propersize);

	ImGui::Columns(2, "##GlowCols", false);

	static std::vector<std::string> styles =
	{
		"Default",
		"Rim3D",
		"Edge",
		"Pulsing Edge"	
	};

	ImGui::Text("Enemy Glow");
	ImGui::NextColumn();
	mfw::Checkbox("##EnemyGlow", &cfg->glow.EnemyGlow);
	if (cfg->glow.EnemyGlow)
	{
		ImGui::SameLine();
		ImGui::Combo("##EnemyStyle", &cfg->glow.EnemyStyle, styles);
		ImGui::SameLine();
		mfw::ColorPicker("##EnemyGlowColor", &cfg->glow.EnemyColor);
	}
	ImGui::NextColumn();

	ImGui::Text("Friend Glow");
	ImGui::NextColumn();
	mfw::Checkbox("##FriendGlow", &cfg->glow.FriendGlow);
	if (cfg->glow.EnemyGlow)
	{
		ImGui::SameLine();
		ImGui::Combo("##FriendStyle", &cfg->glow.FriendStyle, styles);
		ImGui::SameLine();
		mfw::ColorPicker("##FriendGlowColor", &cfg->glow.FriendColor);
	}
	ImGui::NextColumn();

	ImGui::Text("Local Glow");
	ImGui::NextColumn();
	mfw::Checkbox("##LocalGlow", &cfg->glow.LocalGlow);
	if (cfg->glow.EnemyGlow)
	{
		ImGui::SameLine();
		ImGui::Combo("##LocalStyle", &cfg->glow.LocalStyle, styles);
		ImGui::SameLine();
		mfw::ColorPicker("##LocalGlowColor", &cfg->glow.LocalColor);
	}
	ImGui::NextColumn();


	ImGui::EndColumns(); // end columns

	mfw::EndSection(propersize, pos1);
}

void Menu::Render()
{
	cfg->HandleKeybinds();

	RenderSpectators();

	if (G::MenuOpen)
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(500, 420), ImVec2(1920, 1080));
		//imgui_draw.cpp line 2122 to change default font ("AddFontDefault" func)
		static bool once = false;
		if (!once)
		{
			mfw::RefreshColors();
			once = true;
		}

		// should only do this when editing the menu

		mfw::StartMain();

		MenuBar();

		ImGui::Separator();

		int total_w = ImGui::GetWindowSize().x;
		int total_w2 = ImGui::GetContentRegionAvail().x;
		ImGui::Columns(2, "##MAINCols", false); // 3 columns

		// brugh
		ImGui::SetColumnWidth(0, total_w / 2);
		ImGui::SetColumnWidth(1, total_w / 2);

		// COLUMN 1
		if (mfw::Tab == 1)
		{
			RenderBacktrack();
			RenderTriggerbot();
		}	
		if (mfw::Tab == 2)
			RenderAimbot();
		if (mfw::Tab == 3)
		{
			RenderChams();
			RenderESP();
			RenderGlow();
		}	
		if (mfw::Tab == 4)
			RenderMovement();
		if (mfw::Tab == 5)
			RenderMenuOptions();


		ImGui::NextColumn();

		// COLUMN 2
		if (mfw::Tab == 2)
		{
			RenderAA();
			RenderFakelag();
		}
		if (mfw::Tab == 3)
		{
			RenderVisuals();
			RenderWorldVisuals();
		}
		if (mfw::Tab == 5)
		{
			ImGui::Text("Config section coming soon");
			RenderConfig();
		}
			

		ImGui::EndColumns(); // end columns

		mfw::EndMain();

	}
}
