#pragma once

namespace L
{
	// globally enable/disable logging
	constexpr bool LOGGING_ENABLED = true;

	// console and or file output?
	constexpr bool OUTPUT_TO_CONSOLE = false;
	constexpr bool OUTPUT_TO_FILE = true;
	constexpr const char* OUTPUT_FILE_NAME = "a4g4"; // will be placed as a sibling to csgo.exe
	constexpr const char* OUTPUT_FILE_EXTENSION = "log"; // OUTPUT_FILE_NAME + "." + OUTPUT_FILE_EXTENSION

	// how much data should be kept in the output file? (it is a cyclic log)
	// btw, this is an approximation, the actual amount of data will be slightly more
	// since it rounds to the nearest log entry
	// this is measured in kilobytes
	constexpr size_t OUTPUT_FILE_SIZE_KB = 512;

	enum class OutputVerbosity
	{
		Debug = 0,
		Info,
		Crit,
	};

	// everything above this importance will be outputted
	constexpr OutputVerbosity OUTPUT_VERBOSITY = OutputVerbosity::Debug;
	
	// main functions
	void Log(const OutputVerbosity& verbosity, const char* txt, const char* end = "\n", const bool& flush = true, const bool& decorate = true);
	void Debug(const char* txt, const char* end = "\n", const bool& flush = true);
	void Info(const char* txt, const char* end = "\n", const bool& flush = true);
	void Crit(const char* txt, const char* end = "\n", const bool& flush = true);
	void SameLine(const char* txt, const char* end = "\n", const bool& flush = true);

	// initalization and deconstruction
	void Init();
	void Free();
}