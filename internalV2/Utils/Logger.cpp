#include "../Include.hpp"

namespace L
{
	std::string FilePath = "a4g4.log";
	std::ofstream File;
}

void L::Init()
{
	File = std::ofstream(FilePath, std::ios::trunc);
	if (!File.is_open())
	{
		Log("!!!!!!!!ATTENTION!!!!!!!!");
		Log("FAILED TO OPEN LOG FILE !!!!!!!!!");
	}
	auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	File << std::ctime(&time) << "\n";
	File.flush();
}

void L::Log(const char* txt, const char* end, bool flush)
{
	File << txt << end;
	if (flush)
		File.flush();
}

void L::Free()
{
	File << "Logger Finished" << "\n";

	// close the file
	File.close();

	// remove the file, as we exited succesfully
	//remove(FilePath.c_str());
}


void L::Info(const char* txt, const char* end, bool flush)
{
	File << txt << end;
	if (flush)
		File.flush();
}

void L::Debug(const char* txt, const char* end, bool flush)
{
	File << txt << end;
	if (flush)
		File.flush();
}