#pragma once
class Menu
{
private: // actual rendering
	void MenuBar();
	void RenderMenuOptions();

	void RenderMovement();
	void RenderBacktrack();
	void RenderESP();
	void RenderVisuals();
	void RenderAimbot();
	void RenderAA();
	void RenderSpectators();
	void RenderFakelag();
	void RenderChams();
	void RenderWorldVisuals();
public:
	bool ConsoleWindow = true;
	void Render();
};

extern Menu* menu;