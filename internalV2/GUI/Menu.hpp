#pragma once
class Menu
{
private:
	std::map<std::string, bool> files;
	void UpdateFiles();
private: // actual rendering
	void MenuBar();
	void RenderMenuOptions();
	void RenderConfig();

	void RenderTriggerbot();
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
	void RenderGlow();
public:
	bool ConsoleWindow = true;
	void Render();
};

extern Menu* menu;