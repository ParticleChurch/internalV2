#pragma once

enum EFontDrawType : int
{
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,
	FONT_DRAW_TYPE_COUNT = 2
};

class ISurface
{
public:
	void DrawSetColor(Color colDraw)
	{
		CallVFunc<void>(this, 14, colDraw);
	}

	void DrawSetColor(int r, int g, int b, int a)
	{
		CallVFunc<void>(this, 15, r, g, b, a);
	}

	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		CallVFunc<void>(this, 16, x0, y0, x1, y1);
	}

	void DrawFilledRectFade(int x0, int y0, int x1, int y1, uint32_t uAlpha0, uint32_t uAlpha1, bool bHorizontal)
	{
		CallVFunc<void>(this, 123, x0, y0, x1, y1, uAlpha0, uAlpha1, bHorizontal);
	}

	void DrawOutlinedRect(int x0, int y0, int x1, int y1)
	{
		CallVFunc<void>(this, 18, x0, y0, x1, y1);
	}

	void DrawLine(int x0, int y0, int x1, int y1)
	{
		CallVFunc<void>(this, 19, x0, y0, x1, y1);
	}

	void DrawPolyLine(int* x, int* y, int nPoints)
	{
		CallVFunc<void>(this, 20, x, y, nPoints);
	}

	void DrawSetTextFont(unsigned long hFont)
	{
		CallVFunc<void>(this, 23, hFont);
	}

	void DrawSetTextColor(Color color)
	{
		CallVFunc<void>(this, 24, color);
	}

	void DrawSetTextColor(int r, int g, int b, int a)
	{
		CallVFunc<void>(this, 25, r, g, b, a);
	}

	void DrawSetTextPos(int x, int y)
	{
		CallVFunc<void>(this, 26, x, y);
	}

	void DrawPrintText(const wchar_t* wszText, int nTextLength, EFontDrawType drawType = FONT_DRAW_DEFAULT)
	{
		CallVFunc<void>(this, 28, wszText, nTextLength, drawType);
	}

	void DrawSetTextureRGBA(int nIndex, const unsigned char* rgba, int iWide, int iTall)
	{
		CallVFunc<void>(this, 37, nIndex, rgba, iWide, iTall);
	}

	void DrawSetTexture(int nIndex)
	{
		CallVFunc<void>(this, 38, nIndex);
	}

	int CreateNewTextureID(bool bProcedural = false)
	{
		return CallVFunc<int>(this, 43, bProcedural);
	}

	void UnLockCursor()
	{
		return CallVFunc<void>(this, 66);
	}

	void LockCursor()
	{
		return CallVFunc<void>(this, 67);
	}

	unsigned long FontCreate()
	{
		return CallVFunc<unsigned long>(this, 71);
	}

	bool SetFontGlyphSet(unsigned long hFont, const char* szWindowsFontName, int iTall, int iWeight, int iBlur, int nScanLines, int iFlags, int nRangeMin = 0, int nRangeMax = 0)
	{
		return CallVFunc<bool>(this, 72, hFont, szWindowsFontName, iTall, iWeight, iBlur, nScanLines, iFlags, nRangeMin, nRangeMax);
	}

	void GetTextSize(unsigned long hFont, const wchar_t* wszText, int& iWide, int& iTall)
	{
		CallVFunc<void>(this, 79, hFont, wszText, std::ref(iWide), std::ref(iTall));
	}

	void PlaySoundSurface(const char* szFileName)
	{
		CallVFunc<void>(this, 82, szFileName);
	}

	void DrawOutlinedCircle(int x, int y, int iRadius, int nSegments)
	{
		CallVFunc<void>(this, 103, x, y, iRadius, nSegments);
	}

	void DrawTexturedPolygon(int n, Vertex_t* pVertice, bool bClipVertices = true)
	{
		CallVFunc<void>(this, 106, n, pVertice, bClipVertices);
	}
};