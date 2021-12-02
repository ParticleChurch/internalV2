#pragma once

namespace L
{
	extern std::string FilePath;
	extern std::ofstream File;

	extern void Init();
	extern void Log(const char* txt, const char* end = "\n", bool flush = true);
	extern void Free();

	extern void Debug(const char* txt, const char* end = "\n", bool flush = true);
	extern void Info(const char* txt, const char* end = "\n", bool flush = true);
}