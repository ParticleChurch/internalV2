#pragma once
class Menu
{
private:	// style
	void DoStyle();
	ImVec4 AccentColor;
	ImVec4 MainTextColor;
	ImVec4 SelectTextColor;
	ImVec4 BackgroundColor;
	ImVec4 WindowColor;
private: // actual rendering
	void RenderMenuOptions();

	void RenderMovement();
	void RenderBacktrack();
	void RenderESP();
	void RenderVisuals();
	void RenderAimbot();
	void RenderAA();
	void RenderSpectators();
	void RenderFakelag();
public:
	bool ConsoleWindow = true;
	void Render();
};

extern Menu* menu;