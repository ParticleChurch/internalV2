#pragma once
#include "../../ImGui/imgui.h"

class Color
{
public:

	unsigned char color[4];

	Color()
	{
		color[0] = 255;
		color[1] = 255;
		color[2] = 255;
		color[3] = 255;
	}

	Color(float r, float g, float b, float a = 255.00f)
	{
		color[0] = (unsigned char)r;
		color[1] = (unsigned char)g;
		color[2] = (unsigned char)b;
		color[3] = (unsigned char)a;
	}

	inline float r() {
		return (float)color[0];
	}

	inline float g() {
		return (float)color[1];
	}

	inline float b() {
		return (float)color[2];
	}

	inline float a() {
		return (float)color[3];
	}

	std::string Stringify(bool alpha = false)
	{
		if (alpha)
		{
			//(rrr, ggg, bbb, aaa)
			// 1 + 3 + 2 + 3 + 2 + 3 + 2 + 3 + 1
			// 20 + 1 for terminator
			char buf[21];
			snprintf(buf, 21, "(%u, %u, %u, %u)", this->color[0], this->color[1], this->color[2], this->color[3]);
			buf[20] = 0;
			return std::string(buf);
		}
		else
		{
			//(rrr, ggg, bbb)
			// 1 + 3 + 2 + 3 + 2 + 3 + 1
			// 15 + 1 for terminator
			char buf[16];
			snprintf(buf, 16, "(%u, %u, %u)", this->color[0], this->color[1], this->color[2]);
			buf[15] = 0;
			return std::string(buf);
		}
	}

	operator ImVec4()
	{
		return ImVec4(r() / 255.f, g() / 255.f, b() / 255.f, a() / 255.f);
	}

	//operators
	bool operator != (const Color& b) {
		if (b.color[0] != (float)this->color[0])
			return true;
		if (b.color[1] != (float)this->color[1])
			return true;
		if (b.color[2] != (float)this->color[2])
			return true;
		if (b.color[3] != (float)this->color[3])
			return true;
		return false;
	}

	bool operator == (const Color& b) {
		if (b.color[0] != (float)this->color[0])
			return false;
		if (b.color[1] != (float)this->color[1])
			return false;
		if (b.color[2] != (float)this->color[2])
			return false;
		if (b.color[3] != (float)this->color[3])
			return false;
		return true;
	}

	bool Parse(std::string v, bool processAlpha = false)
	{
		// (rrr, ggg, bbb, aaa)
		size_t p = 0, a = 0, b = 0, c = 0;

		p = v.find("(");
		if (p == std::string::npos) return false;
		v = v.substr(p + 1);

		p = v.find(")");
		if (p == std::string::npos) return false;
		v = v.substr(0, p);

		// rrr, ggg, bbb[, aaa]
		a = v.find(",", 0);     if (a == std::string::npos) return false;
		b = v.find(",", a + 1); if (b == std::string::npos) return false;
		c = v.find(",", b + 1); if (processAlpha && c == std::string::npos) return false;

		std::string red = v.substr(0, a);
		std::string green = v.substr(a + 1, b - a - 1);
		std::string blue = v.substr(b + 1, c - b - 1);
		std::string alpha = "255";
		if (processAlpha)
		{
			alpha = v.substr(c);
		}

		try
		{
			int x, y, z, a = 255;
			x = std::stoi(red);
			y = std::stoi(green);
			z = std::stoi(blue);
			if (processAlpha) a = std::stoi(alpha);
			this->color[0] = x;
			this->color[1] = y;
			this->color[2] = z;
			this->color[3] = a;
		}
		catch (...)
		{
			return false;
		}
		return true;
	}
};