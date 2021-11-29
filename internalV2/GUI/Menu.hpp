#pragma once
class Menu
{
private:
	bool ConsoleWindow = true;

	void RenderESP();

public:
	void Render();
};

extern Menu* menu;