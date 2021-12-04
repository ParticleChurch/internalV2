#include "Logger.hpp"

#include <string>
#include <fstream>
#include <iostream>

#include <cstdint>
#include <chrono>

#include <filesystem>

#include <Windows.h>

L::OutputVerbosity previousVerbosity = L::OutputVerbosity::Debug;

std::string logActiveFilePath;
std::string logHistoryFilePath;
std::ofstream outputFile;

template <size_t max>
inline size_t findNewline(const char* const& string)
{
	for (size_t i = 0; i < max; i++)
		if (string[i] == '\n') return i;
	return max;
}

void L::Log(const OutputVerbosity& verbosity, const char* txt, const char* end, const bool& flush, const bool& decorate)
{
	if constexpr (!LOGGING_ENABLED) return;
	
	previousVerbosity = verbosity; // so SameLine() can copy it
	if (verbosity < OUTPUT_VERBOSITY) return;

	static std::string decoratorString = "";
	if (decorate)
	{
		decoratorString.clear();

		const uint64_t unix = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
		decoratorString += std::to_string(unix);
		decoratorString += " - [";

		switch (verbosity)
		{
		case OutputVerbosity::Debug:
			decoratorString += "DEBUG";
			break;
		case OutputVerbosity::Info:
			decoratorString += "INFO";
			break;
		case OutputVerbosity::Crit:
			decoratorString += "CRITICAL";
			break;
		default:
			decoratorString += "UNKNOWN";
			break;
		}

		decoratorString += "]: ";
	}

	if constexpr (OUTPUT_TO_CONSOLE)
	{
		if (decorate) std::cout << decoratorString;

		std::cout << txt << end;

		if (flush) std::cout << std::flush;
	}

	if constexpr (OUTPUT_TO_FILE)
	{
		if (!outputFile.is_open()) goto AFTER_FILE_OUTPUT;

		if (decorate) // this is likely a new log entry
		{
			// so it's a good time to check if the file is full
			if (outputFile.tellp() >= 1024 * OUTPUT_FILE_SIZE_KB)
			{
				// close and copy to history
				outputFile.close();
				std::filesystem::copy_file(logActiveFilePath, logHistoryFilePath, std::filesystem::copy_options::overwrite_existing);

				// re-open and truncate
				outputFile.open(logActiveFilePath, std::ios::out | std::ios::trunc);
				if (!outputFile.is_open())
				{
					Crit("Failed to open logging file: ", "");
					SameLine(logActiveFilePath.c_str());
					goto AFTER_FILE_OUTPUT;
				}
			}

			outputFile.write(decoratorString.c_str(), decoratorString.length());
		}

		outputFile.write(txt, strlen(txt));
		outputFile.write(end, strlen(end));

		if (flush) outputFile.flush();
	
	AFTER_FILE_OUTPUT:
		{ /* have to put something afte a label*/ }
	}
}

// forwards info on to L::Log
void L::Debug(const char* txt, const char* end, const bool& flush)
{
	return L::Log(OutputVerbosity::Debug, txt, end, flush);
}
void L::Info(const char* txt, const char* end, const bool& flush)
{
	return L::Log(OutputVerbosity::Info, txt, end, flush);
}
void L::Crit(const char* txt, const char* end, const bool& flush)
{
	return L::Log(OutputVerbosity::Crit, txt, end, flush);
}
void L::SameLine(const char* txt, const char* end, const bool& flush)
{
	return L::Log(previousVerbosity, txt, end, flush, false);
}

// initalization
void L::Init()
{
	// usually in release mode
	if constexpr (LOGGING_ENABLED)
	{
		// allocate console
		if constexpr (OUTPUT_TO_CONSOLE)
		{
			AllocConsole();

			FILE* fpstdin = stdin, * fpstdout = stdout, * fpstderr = stderr;
			freopen_s(&fpstdin, "CONIN$", "r", stdin);
			freopen_s(&fpstdout, "CONOUT$", "w", stdout);
			freopen_s(&fpstderr, "CONOUT$", "w", stderr);
		}

		// open output file
		if constexpr (OUTPUT_TO_FILE)
		{
			// create path to sibling of csgo.exe
			char csgo_exe[MAX_PATH];
			GetModuleFileName(NULL, csgo_exe, MAX_PATH);
			std::string csgo_directory(csgo_exe, strlen(csgo_exe) - strlen("csgo.exe"));

			logHistoryFilePath = csgo_directory + OUTPUT_FILE_NAME + "_history." + OUTPUT_FILE_EXTENSION;
			logActiveFilePath = csgo_directory + OUTPUT_FILE_NAME + "_active." + OUTPUT_FILE_EXTENSION;

			// truncate history file
			outputFile.open(logHistoryFilePath, std::ios::out | std::ios::trunc);
			if (outputFile.is_open()) outputFile.close();

			// open and truncate the active file
			outputFile.open(logActiveFilePath, std::ios::out | std::ios::trunc);
			if (!outputFile.is_open())
			{
				Crit("Failed to open logging file: ", "");
				SameLine(logActiveFilePath.c_str());
			}
		}

		Info("Logger is constructed! This should be the first message.");
	}
}

// deconstruction
void L::Free()
{
	if constexpr (LOGGING_ENABLED)
	{
		Info("Logger is deconstructing. This should be the last message.");

		// free and destroy console
		if constexpr (OUTPUT_TO_CONSOLE)
		{
			HWND con = GetConsoleWindow();
			FreeConsole();
			if (con) SendMessage(con, WM_CLOSE, NULL, NULL);
		}

		// close output file
		if constexpr (OUTPUT_TO_FILE)
		{
			if (outputFile.is_open()) outputFile.close();
		}
	}
}
