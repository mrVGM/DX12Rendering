#include "BaseFramework.h"

#include "DataLib.h"

#include <filesystem>
#include <iostream>

#include "AppSettings.h"

#include "App.h"

#include "Frontend.h"

int main(int args, const char** argv)
{
	std::string executableName = argv[0];
	int index = executableName.find_last_of('\\');

	if (index < 0) {
		std::cerr << "Can't find data directory!" << std::endl;
		return 1;
	}
	std::string executableDirectory = executableName.substr(0, index);
	std::filesystem::path executableDirPath = std::filesystem::path(executableDirectory);

#if DEBUG
	std::filesystem::path dataPath = "..\\..\\..\\..\\Data\\";
#else
	std::filesystem::path dataPath = executableDirPath.append("..\\data\\");
#endif

	bool valid = data::Init(dataPath.string().c_str());
	if (valid)
	{
		std::cout << "Data Directory Set!" << std::endl;
	}
	else
	{
		std::cout << "Invalid Path to Data Directory!" << std::endl;
	}

	new settings::AppSettings();
	app::BootApp();

	frontend::Boot();

	std::cin.get();

	app::ShutdownApp();

	return 0;
}
