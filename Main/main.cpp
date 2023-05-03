#include "BaseFramework.h"

#include "Window.h"
#include "DataLib.h"
#include "ColladaScene.h"
#include "Rendering.h"

#include <filesystem>
#include <iostream>

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

	rendering::InitBaseObjects();

	std::cin.get();

	BaseFrameworkShutdown();

	return 0;
}
