#pragma once
class Menu
{
private:
	bool ConsoleWindow = true;

	void RenderESP();
	void RenderAimbot();

public:
	void Render();
};

extern Menu* menu;