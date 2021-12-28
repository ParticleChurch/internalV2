#pragma once
class Menu
{
private:
	void RenderESP();
	void RenderAimbot();
	void RenderSpectators();
public:
	bool ConsoleWindow = true;
	void Render();
};

extern Menu* menu;