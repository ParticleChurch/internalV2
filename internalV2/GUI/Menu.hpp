#pragma once
class Menu
{
private:
	void RenderESP();
public:
	bool ConsoleWindow = true;
	void Render();
};

extern Menu* menu;